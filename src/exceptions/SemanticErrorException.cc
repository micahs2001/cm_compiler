#include <format>

#include "CompilerException.hh"
#include "SemanticErrorException.hh"

SemanticErrorException::SemanticErrorException (const std::string& message)
    : CompilerException (message)
{
}

SemanticErrorException::SemanticErrorException (const std::string& message,
                                                SourceLocation location)
    : CompilerException (
        std::format ("Semantic Error [Line {}, Col {}]\n    {}",
                     location.line,
                     location.column,
                     message))
{
}