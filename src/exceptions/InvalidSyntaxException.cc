#include <format>
#include <ranges>
#include <string>
#include <utility>

#include "InvalidSyntaxException.hh"
#include "tokenizing/SourceLocation.hh"

InvalidSyntaxException::InvalidSyntaxException (
  SourceLocation location,
  std::initializer_list<TokenType> expected,
  std::string& got)
    : CompilerException (format_message (expected, got, location))
{
}

std::string
InvalidSyntaxException::format_message (
  std::initializer_list<TokenType> expected,
  const std::string& got,
  SourceLocation location)
{
  std::string expStr =
    std::format ("{{ \'{:s}\' }}",
                 expected | std::views::transform (tokenToString) |
                   std::views::join_with (std::string_view ("\', \'")));

  return std::format ("Parse Error [{}, {}]\n    Expected {} but got \'{}\'",
                      location.line,
                      location.column,
                      expStr,
                      got);
}