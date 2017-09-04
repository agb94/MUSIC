#ifndef COMUT_OLSN_H_
#define COMUT_OLSN_H_

#include "mutant_operator_template.h"

class OLSN : public MutantOperatorTemplate
{
public:
	OLSN(const std::string name = "OLSN")
		: MutantOperatorTemplate(name)
	{}

	virtual bool ValidateDomain(const std::set<std::string> &domain);
	virtual bool ValidateRange(const std::set<std::string> &range);

	virtual void setDomain(std::set<std::string> &domain);
  virtual void setRange(std::set<std::string> &range);

	// Return True if the mutant operator can mutate this expression
	virtual bool CanMutate(clang::Expr *e, ComutContext *context);

	// Return True if the mutant operator can mutate this statement
	virtual bool CanMutate(clang::Stmt *s, ComutContext *context);

	virtual void Mutate(clang::Expr *e, ComutContext *context);
	virtual void Mutate(clang::Stmt *s, ComutContext *context);

private:
	bool CanMutate(BinaryOperator *bo, string mutated_token,
								 ComutContext *context);
};

#endif	// COMUT_OLSN_H_