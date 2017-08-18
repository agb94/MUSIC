#include "../comut_utility.h"
#include "oppo.h"

bool OPPO::ValidateDomain(const std::set<std::string> &domain)
{
	return domain.empty();
}

bool OPPO::ValidateRange(const std::set<std::string> &range)
{
	return range.empty();
}

// Return True if the mutant operator can mutate this expression
bool OPPO::CanMutate(clang::Expr *e, ComutContext *context)
{
	if (UnaryOperator *uo = dyn_cast<UnaryOperator>(e))
		if (uo->getOpcode() == UO_PostInc || uo->getOpcode() == UO_PreInc)
		{
			SourceLocation start_loc = uo->getLocStart();
    	SourceLocation end_loc = GetEndLocOfUnaryOpExpr(uo, context->comp_inst);

    	return Range1IsPartOfRange2(
					SourceRange(start_loc, end_loc), 
					SourceRange(*(context->userinput->getStartOfMutationRange()),
											*(context->userinput->getEndOfMutationRange())));
		}

	return false;
}

// Return True if the mutant operator can mutate this statement
bool OPPO::CanMutate(clang::Stmt *s, ComutContext *context)
{
	return false;
}

void OPPO::Mutate(clang::Expr *e, ComutContext *context)
{
	UnaryOperator *uo;
	if (!(uo = dyn_cast<UnaryOperator>(e)))
		return;

	if (uo->getOpcode() == UO_PostInc)  // x++
		GenerateMutantForPostInc(uo, context);
	else  // ++x
		GenerateMutantForPreInc(uo, context);
}

void OPPO::Mutate(clang::Stmt *s, ComutContext *context)
{}

void OPPO::GenerateMutantForPostInc(UnaryOperator *uo, ComutContext *context)
{
	SourceLocation start_loc = uo->getLocStart();
	SourceLocation end_loc = GetEndLocOfUnaryOpExpr(uo, context->comp_inst);

	SourceManager &src_mgr = context->comp_inst->getSourceManager();
	Rewriter rewriter;
	rewriter.setSourceMgr(src_mgr, context->comp_inst->getLangOpts());

	string token{rewriter.ConvertToString(uo)};

	// generate ++x
  uo->setOpcode(UO_PreInc);
  string mutated_token = rewriter.ConvertToString(uo);

  GenerateMutantFile(context, start_loc, end_loc, mutated_token);
	WriteMutantInfoToMutantDbFile(context, start_loc, end_loc, 
																token, mutated_token);

  // generate x--
  uo->setOpcode(UO_PostDec);
  mutated_token = rewriter.ConvertToString(uo);

  GenerateMutantFile(context, start_loc, end_loc, mutated_token);
	WriteMutantInfoToMutantDbFile(context, start_loc, end_loc, 
																token, mutated_token);

  // reset the code structure
  uo->setOpcode(UO_PostInc);
}

void OPPO::GenerateMutantForPreInc(UnaryOperator *uo, ComutContext *context)
{
	SourceLocation start_loc = uo->getLocStart();
	SourceLocation end_loc = GetEndLocOfUnaryOpExpr(uo, context->comp_inst);

	SourceManager &src_mgr = context->comp_inst->getSourceManager();
	Rewriter rewriter;
	rewriter.setSourceMgr(src_mgr, context->comp_inst->getLangOpts());

	string token{rewriter.ConvertToString(uo)};

	// generate x++
  uo->setOpcode(UO_PostInc);
  string mutated_token = rewriter.ConvertToString(uo);
  
  GenerateMutantFile(context, start_loc, end_loc, mutated_token);
	WriteMutantInfoToMutantDbFile(context, start_loc, end_loc, 
																token, mutated_token);

  // generate --x
  uo->setOpcode(UO_PreDec);
  mutated_token = rewriter.ConvertToString(uo);

  GenerateMutantFile(context, start_loc, end_loc, mutated_token);
	WriteMutantInfoToMutantDbFile(context, start_loc, end_loc, 
																token, mutated_token);

  // reset the code structure
  uo->setOpcode(UO_PreInc);
}