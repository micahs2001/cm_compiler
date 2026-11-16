#pragma once

#include <string>

#include "CompilerException.hh"
#include "tokenizing/SourceLocation.hh"

class InvalidTokenException : public CompilerException
{
public:
  InvalidTokenException (const std::string& message, SourceLocation location);
  InvalidTokenException (SourceLocation location);
};
