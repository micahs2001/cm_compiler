#pragma once

#include "exceptions/InvalidSyntaxException.cc"
#include "exceptions/InvalidTokenException.cc"
#include "exceptions/UndeclaredVarException.cc"

// Expression and Operator Errors:
//   Operaant type mismatch for unary or binary
//   Invalid l-lavue in assignment target or increment/decremenet ops
//   Member access (./->) on non-record or undefined fields