#include "../music_utility.h"
#include "oasa.h"

extern set<string> arith_assignment_operators;
extern set<string> shift_assignment_operators;

bool OASA::ValidateDomain(const std::set<std::string> &domain)
{
	for (auto it: domain)
  	if (arith_assignment_operators.find(it) == arith_assignment_operators.end())
    	// cannot find input domain inside valid domain
      return false;

  return true;
}

bool OASA::ValidateRange(const std::set<std::string> &range)
{
	for (auto it: range)
  	if (shift_assignment_operators.find(it) == shift_assignment_operators.end())
    	// cannot find input range inside valid range
      return false;

  return true;
}

void OASA::setDomain(std::set<std::string> &domain)
{
	if (domain.empty())
		domain_ = arith_assignment_operators;
	else
		domain_ = domain;
}

void OASA::setRange(std::set<std::string> &range)
{
	if (range.empty())
		range_ = shift_assignment_operators;
	else
		range_ = range;
}

bool OASA::IsMutationTarget(clang::Expr *e, MusicContext *context)
{
	if (BinaryOperator *bo = dyn_cast<BinaryOperator>(e))
	{
		string binary_operator{bo->getOpcodeStr()};
		SourceLocation start_loc = bo->getOperatorLoc();
		SourceManager &src_mgr = context->comp_inst_->getSourceManager();

		// cout << "cp oasa\n";
		SourceLocation end_loc = src_mgr.translateLineCol(
				src_mgr.getMainFileID(),
				GetLineNumber(src_mgr, start_loc),
				GetColumnNumber(src_mgr, start_loc) + binary_operator.length());
		StmtContext &stmt_context = context->getStmtContext();

		// Return False if expr is NOT in mutation range, inside array decl size
		// and inside enum declaration.
		if (!context->IsRangeInMutationRange(SourceRange(start_loc, end_loc)) ||
				stmt_context.IsInArrayDeclSize() ||
				stmt_context.IsInEnumDecl() ||
				domain_.find(binary_operator) == domain_.end())
			return false;

		// shift operator can only take integral operands
		Expr *lhs = bo->getLHS()->IgnoreImpCasts();
		Expr *rhs = bo->getRHS()->IgnoreImpCasts();

		if (ExprIsIntegral(context->comp_inst_, lhs) &&
				ExprIsIntegral(context->comp_inst_, rhs))
			return true;
	}

	return false;
}



void OASA::Mutate(clang::Expr *e, MusicContext *context)
{
	BinaryOperator *bo;
	if (!(bo = dyn_cast<BinaryOperator>(e)))
		return;

	string token{bo->getOpcodeStr()};
	SourceLocation start_loc = bo->getOperatorLoc();
	SourceManager &src_mgr = context->comp_inst_->getSourceManager();

	// cout << "cp oasa 2\n";
	SourceLocation end_loc = src_mgr.translateLineCol(
			src_mgr.getMainFileID(),
			GetLineNumber(src_mgr, start_loc),
			GetColumnNumber(src_mgr, start_loc) + token.length());

	for (auto mutated_token: range_)
		if (token.compare(mutated_token) != 0)
		{
			context->mutant_database_.AddMutantEntry(name_, start_loc, end_loc, token, mutated_token, context->getStmtContext().getProteumStyleLineNum(), context->getStmtContext().getProteumStyleColumnNum(), context->getStmtContext().getFunctionDeclName());
		}
}

