#include <format>

#include "InvalidTokenException.hh"

InvalidTokenException::InvalidTokenException (const std::string& message,
                                              SourceLocation location)
    : CompilerException (std::format ("Invalid Token [Line {}, Col {}]\n    {}",
                                      location.line,
                                      location.column,
                                      message))
{
}

InvalidTokenException::InvalidTokenException (SourceLocation location)
    : CompilerException (std::format ("Invalid Token [Line {}, Col {}]",
                                      location.line,
                                      location.column))
{
}