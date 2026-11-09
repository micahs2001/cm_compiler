#include "InvalidTokenException.hh"

InvalidTokenException::InvalidTokenException (const std::string& message)
    : CompilerException (message)
{
}

InvalidTokenException::InvalidTokenException (const std::string& message,
                                             SourceLocation location)
    : CompilerException (message, location)
{
}