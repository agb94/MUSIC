#include "../music_utility.h"
#include "olsn.h"

extern set<string> shift_operators;
extern set<string> logical_operators;

bool OLSN::ValidateDomain(const std::set<std::string> &domain)
{
	for (auto it: domain)
  	if (logical_operators.find(it) == logical_operators.end())
    	// cannot find input domain inside valid domain
      return false;

  return true;
}

bool OLSN::ValidateRange(const std::set<std::string> &range)
{
	for (auto it: range)
  	if (shift_operators.find(it) == shift_operators.end())
    	// cannot find input range inside valid range
      return false;

  return true;
}

void OLSN::setDomain(std::set<std::string> &domain)
{
	if (domain.empty())
		domain_ = logical_operators;
	else
		domain_ = domain;
}

void OLSN::setRange(std::set<std::string> &range)
{
	if (range.empty())
		range_ = shift_operators;
	else
		range_ = range;
}

bool OLSN::IsMutationTarget(clang::Expr *e, MusicContext *context)
{
	if (BinaryOperator *bo = dyn_cast<BinaryOperator>(e))
	{
		string binary_operator{bo->getOpcodeStr()};
		SourceLocation start_loc = bo->getOperatorLoc();
		SourceManager &src_mgr = context->comp_inst_->getSourceManager();
		// cout << "cp olsn\n";
		SourceLocation end_loc = src_mgr.translateLineCol(
				src_mgr.getMainFileID(),
				GetLineNumber(src_mgr, start_loc),
				GetColumnNumber(src_mgr, start_loc) + binary_operator.length());
		StmtContext &stmt_context = context->getStmtContext();
		
		// Return True if expr is in mutation range, NOT inside array decl size
		// and NOT inside enum declaration.
		if (!context->IsRangeInMutationRange(SourceRange(start_loc, end_loc)) ||
				stmt_context.IsInArrayDeclSize() ||
				stmt_context.IsInEnumDecl() ||
				stmt_context.IsInTypedefRange(e) ||
				domain_.find(binary_operator) == domain_.end())
			return false;

		return true;
	}

	return false;
}



void OLSN::Mutate(clang::Expr *e, MusicContext *context)
{
	BinaryOperator *bo;
	if (!(bo = dyn_cast<BinaryOperator>(e))) return;

	string token{bo->getOpcodeStr()};
	SourceLocation start_loc = bo->getOperatorLoc();
	SourceManager &src_mgr = context->comp_inst_->getSourceManager();
	// cout << "cp olsn\n";
	SourceLocation end_loc = src_mgr.translateLineCol(
			src_mgr.getMainFileID(),
			GetLineNumber(src_mgr, start_loc),
			GetColumnNumber(src_mgr, start_loc) + token.length());

	Rewriter rewriter;
	rewriter.setSourceMgr(src_mgr, context->comp_inst_->getLangOpts());

	for (auto mutated_token: range_)
	{
		if (token.compare(mutated_token) == 0)
			continue;

		if (!IsMutationTarget(bo, mutated_token, context))
			continue;

		context->mutant_database_.AddMutantEntry(name_, start_loc, end_loc, token, mutated_token, context->getStmtContext().getProteumStyleLineNum(), context->getStmtContext().getProteumStyleColumnNum(), context->getStmtContext().getFunctionDeclName());
	}
}



bool OLSN::IsMutationTarget(BinaryOperator *bo, string mutated_token,
										 MusicContext *context)
{
	Expr *lhs = GetLeftOperandAfterMutation(
			bo->getLHS()->IgnoreImpCasts(), TranslateToOpcode(mutated_token));
	Expr *rhs = GetRightOperandAfterMutation(
			bo->getRHS()->IgnoreImpCasts(), TranslateToOpcode(mutated_token));

	// bitwise operator only takes integral operands
	return ExprIsIntegral(context->comp_inst_, lhs) &&
				 ExprIsIntegral(context->comp_inst_, rhs);
}