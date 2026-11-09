#pragma once

#include "exceptions/CompilerException.hh"
#include "exceptions/InvalidSyntaxException.hh"
#include "exceptions/InvalidTokenException.hh"
#include "exceptions/SemanticErrorException.hh"

// Expression and Operator Errors:
//   Operaant type mismatch for unary or binary
//   Invalid l-lavue in assignment target or increment/decremenet ops
//   Member access (./->) on non-record or undefined fields