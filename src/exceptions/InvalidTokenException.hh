#pragma once

#include <string>

#include "CompilerException.hh"

class InvalidTokenException : public CompilerException
{
public:
  explicit InvalidTokenException (const std::string& message);
  InvalidTokenException (const std::string& message, SourceLocation location);
};
