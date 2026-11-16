
#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "SourceLocation.hh"

enum class TokenType : uint8_t
{
  // Keywords
  If,
  Else,
  Int,
  Void,
  Return,
  While,
  // Operators
  Plus,
  Minus,
  Multiply,
  Divide,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  Equal,
  NotEqual,
  Assign,
  // Delimiters/Punctuators
  Semi,
  Comma,
  LParen,
  RParen,
  // Bracks are [ and ]
  LBrack,
  RBrack,
  // Braces are { and }
  LBrace,
  RBrace,
  // Identifiers and Literals
  Id,
  Num,
  // Special
  EndOfFile,
  Error
};

struct Token
{
  TokenType type { TokenType::Error };
  std::string lexeme;
  SourceLocation location;
};

std::string_view
tokenToString (TokenType type);
