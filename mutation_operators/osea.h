#ifndef MUSIC_OSEA_H_
#define MUSIC_OSEA_H_

#include "expr_mutant_operator.h"

class OSEA : public ExprMutantOperator
{
public:
	OSEA(const std::string name = "OSEA")
		: ExprMutantOperator(name)
	{}

	virtual bool ValidateDomain(const std::set<std::string> &domain);
	virtual bool ValidateRange(const std::set<std::string> &range);

	virtual void setDomain(std::set<std::string> &domain);
  virtual void setRange(std::set<std::string> &range);

	// Return True if the mutant operator can mutate this expression
	virtual bool IsMutationTarget(clang::Expr *e, MusicContext *context);

	virtual void Mutate(clang::Expr *e, MusicContext *context);
};

#endif	// MUSIC_OSEA_H_