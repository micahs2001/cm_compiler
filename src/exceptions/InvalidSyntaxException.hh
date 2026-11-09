#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "CompilerException.hh"

class InvalidSyntaxException : public CompilerException
{
public:
  InvalidSyntaxException (SourceLocation location,
                          std::initializer_list<TokenType> expected,
                          std::string got);

  [[nodiscard]]
  int
  line () const noexcept
  {
    return line_;
  }

  [[nodiscard]]
  int
  column () const noexcept
  {
    return column_;
  }

  [[nodiscard]]
  const std::vector<TokenType>&
  expected () const noexcept
  {
    return expected_;
  }

  [[nodiscard]]
  const std::string&
  got () const noexcept
  {
    return got_;
  }

private:
  static std::string
  format_message (std::initializer_list<TokenType> expected,
                  const std::string& got);

  int line_;
  int column_;
  std::vector<TokenType> expected_;
  std::string got_;
};
