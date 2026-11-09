#pragma once

#include <string>

#include "CompilerException.hh"

class SemanticErrorException : public CompilerException
{
public:
  explicit SemanticErrorException (const std::string& message);
  SemanticErrorException (const std::string& message, SourceLocation location);
};
