#include <format>
#include <ranges>
#include <string>
#include <utility>

#include "InvalidSyntaxException.hh"

InvalidSyntaxException::InvalidSyntaxException (
  SourceLocation location,
  std::initializer_list<TokenType> expected,
  std::string got)
    : CompilerException (format_message (expected, got), location),
      line_ (static_cast<int> (location.line)),
      column_ (static_cast<int> (location.column)),
      expected_ (expected),
      got_ (std::move (got))
{
}

std::string
InvalidSyntaxException::format_message (
  std::initializer_list<TokenType> expected,
  const std::string& got)
{
  std::string expStr =
    std::format ("{{ \'{:s}\' }}",
                 expected | std::views::transform (tokenToString) |
                   std::views::join_with (std::string_view ("\', \'")));

  return std::format (
    "Compilation Error\n  Parse Error\n  Expected {} but got \'{}\'",
    expStr,
    got);
}