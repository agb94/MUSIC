#ifndef MUSIC_MUTANT_ENTRY_H_
#define MUSIC_MUTANT_ENTRY_H_ 

#include <string>
#include <iostream>

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"

class MutantEntry
{
public:
  clang::SourceManager &src_mgr_;

  MutantEntry(std::string token, std::string mutated_token,
              clang::SourceLocation start_loc, clang::SourceLocation end_loc,
              clang::SourceManager &src_mgr, int proteum_style_line_num,
              int proteum_style_column_num, std::string function_decl_name);

  // getters
  int getProteumStyleLineNum() const;
  int getProteumStyleColumnNum() const;
  std::string getFunctionDeclName() const;
  std::string getToken() const;
  std::string getMutatedToken() const;
  // only end location changes after mutation
  clang::SourceLocation getStartLocation() const;
  clang::SourceLocation getTokenEndLocation() const;
  clang::SourceRange getTokenRange() const;
  clang::SourceLocation getMutatedTokenEndLocation() const;
  clang::SourceRange getMutatedTokenRange() const;

  bool operator==(const MutantEntry &rhs) const;

private:
  int proteum_style_line_num_;
  int proteum_style_column_num_;
  std::string function_decl_name_;
  std::string token_;
  std::string mutated_token_;
  clang::SourceLocation start_location_;
  clang::SourceLocation end_location_before_mutation_;
  clang::SourceLocation end_location_after_mutation_;
};

#endif  // MUSIC_MUTANT_ENTRY_H_
