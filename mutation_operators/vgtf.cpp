#include "../music_utility.h"
#include "vgtf.h"

bool VGTF::ValidateDomain(const std::set<std::string> &domain)
{
	return true;
}

bool VGTF::ValidateRange(const std::set<std::string> &range)
{
	return true;
}

// Return True if the mutant operator can mutate this expression
bool VGTF::IsMutationTarget(clang::Expr *e, MusicContext *context)
{
	if (CallExpr *ce = dyn_cast<CallExpr>(e))
	{
		SourceLocation start_loc = ce->getLocStart();

    // getRParenLoc returns the location before the right parenthesis
    SourceLocation end_loc = ce->getRParenLoc();
    end_loc = end_loc.getLocWithOffset(1);

    string token{
        ConvertToString(ce->getCallee(), context->comp_inst_->getLangOpts())};
    bool is_in_domain = domain_.empty() ? true : 
                        IsStringElementOfSet(token, domain_);

    // Return True if expr is in mutation range, NOT inside enum decl
    // and is structure type.
		return (context->IsRangeInMutationRange(SourceRange(start_loc, end_loc)) &&
            !context->getStmtContext().IsInEnumDecl() && ExprIsStruct(e)) &&
            is_in_domain;
	}

	return false;
}

void VGTF::Mutate(clang::Expr *e, MusicContext *context)
{
	CallExpr *ce;
	if (!(ce = dyn_cast<CallExpr>(e)))
		return;

	SourceLocation start_loc = ce->getLocStart();

  // getRParenLoc returns the location before the right parenthesis
  SourceLocation end_loc = ce->getRParenLoc();
  end_loc = end_loc.getLocWithOffset(1);

  Rewriter rewriter;
	SourceManager &src_mgr = context->comp_inst_->getSourceManager();
	rewriter.setSourceMgr(src_mgr, context->comp_inst_->getLangOpts());

	string token{ConvertToString(e, context->comp_inst_->getLangOpts())};

	// cannot mutate variable in switch condition to a floating-type variable
  bool skip_float_vardecl = \
      context->getStmtContext().IsInSwitchStmtConditionRange(e);

  string struct_type{
  		getStructureType(e->getType().getCanonicalType())};

  for (auto vardecl: *(context->getSymbolTable()->getGlobalStructVarDeclList()))
  {
    if (!(vardecl->getLocStart() < start_loc))
      break;
    
    if (skip_float_vardecl && IsVarDeclFloating(vardecl))
        continue;

    string mutated_token{GetVarDeclName(vardecl)};

    // Mutated token is not inside user-specified range.
    if (!range_.empty() && range_.find(mutated_token) == range_.end())
      continue;

    if (token.compare(mutated_token) != 0 &&
        struct_type.compare(getStructureType(vardecl->getType())) == 0)
    {
      context->mutant_database_.AddMutantEntry(
          name_, start_loc, end_loc, token, mutated_token, 
          context->getStmtContext().getProteumStyleLineNum(), context->getStmtContext().getProteumStyleColumnNum());
    }
  }
}


