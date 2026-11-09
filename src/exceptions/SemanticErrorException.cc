#include "SemanticErrorException.hh"

SemanticErrorException::SemanticErrorException (const std::string& message)
    : CompilerException (message)
{
}

SemanticErrorException::SemanticErrorException (const std::string& message,
                                               SourceLocation location)
    : CompilerException (message, location)
{
}