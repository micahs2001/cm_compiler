/******************************************************************************
 File Name:    Scanner.cc
 Description:  Scanner class for CMinus language
 Author:       Micah Schafer
 Date:         September 24, 2026

 *Function descriptors in header file
 *****************************************************************************/

#include <array>
#include <cctype>
#include <string>
#include <utility>

#include "lexing/Scanner.hh"

Scanner::Scanner (std::string source)
    : source_ (std::move (source))
{
}

Token
Scanner::scanNext ()
{
  skipWhitespaceAndComments ();
  if (peek () == EOF)
  {
    return Token { .type = TokenType::EndOfFile,
                   .lexeme = "",
                   .location = location_ };
  }

  const SourceLocation start = location_;
  int c = peek ();
  if (isLetter (c))
  {
    return scanIdentifier ();
  }
  if (isNumber (c))
  {
    return scanNumber ();
  }

  c = advance ();
  switch (c)
  {
    case '+':
      return Token { .type = TokenType::Plus,
                     .lexeme = "+",
                     .location = start };
    case '-':
      return Token { .type = TokenType::Minus,
                     .lexeme = "-",
                     .location = start };
    case '*':
      return Token { .type = TokenType::Multiply,
                     .lexeme = "*",
                     .location = start };
    case '/':
      return Token { .type = TokenType::Divide,
                     .lexeme = "/",
                     .location = start };
    case '<':
    {
      if (peek () == '=')
      {
        advance ();
        return Token {
          .type = TokenType::LessEqual, .lexeme = "<=", .location = start
        };
      }
      return Token { .type = TokenType::Less,
                     .lexeme = "<",
                     .location = start };
    }
    case '>':
    {
      if (peek () == '=')
      {
        advance ();
        return Token {
          .type = TokenType::GreaterEqual, .lexeme = ">=", .location = start
        };
      }
      return Token { .type = TokenType::Greater,
                     .lexeme = ">",
                     .location = start };
    }
    case '=':
    {
      if (peek () == '=')
      {
        advance ();
        return Token {
          .type = TokenType::Equal, .lexeme = "==", .location = start
        };
      }
      return Token { .type = TokenType::Assign,
                     .lexeme = "=",
                     .location = start };
    }
    case '!':
    {
      if (peek () == '=')
      {
        advance ();
        return Token {
          .type = TokenType::NotEqual, .lexeme = "!=", .location = start
        };
      }
      break;
    }
    case ';':
      return Token { .type = TokenType::Semi,
                     .lexeme = ";",
                     .location = start };
    case ',':
      return Token { .type = TokenType::Comma,
                     .lexeme = ",",
                     .location = start };
    case '(':
      return Token { .type = TokenType::LParen,
                     .lexeme = "(",
                     .location = start };
    case ')':
      return Token { .type = TokenType::RParen,
                     .lexeme = ")",
                     .location = start };
    case '[':
      return Token { .type = TokenType::LBrack,
                     .lexeme = "[",
                     .location = start };
    case ']':
      return Token { .type = TokenType::RBrack,
                     .lexeme = "]",
                     .location = start };
    case '{':
      return Token { .type = TokenType::LBrace,
                     .lexeme = "{",
                     .location = start };
    case '}':
      return Token { .type = TokenType::RBrace,
                     .lexeme = "}",
                     .location = start };
    default:
      break;
  }
  return Token { .type = TokenType::Error,
                 .lexeme = std::string (1, static_cast<char> (c)),
                 .location = start };
}

int
Scanner::peek () const
{
  if (cursor_ < source_.size ())
  {
    return source_.at (cursor_);
  }
  else
  {
    return EOF;
  }
}

int
Scanner::advance ()
{
  int c = peek ();
  ++cursor_;
  if (c == '\n')
  {
    ++location_.line;
    location_.column = 1;
  }
  else
  {
    ++location_.column;
  }
  return c;
}

int
Scanner::lookAhead ()
{
  if (cursor_ + 1 < source_.size ())
  {
    return source_.at (cursor_ + 1);
  }
  else
  {
    return EOF;
  }
}

bool
Scanner::isLetter (int c)
{
  return (c > 64 && c < 91) || (c > 96 && c < 123);
}

bool
Scanner::isNumber (int c)
{
  return (c > 47 && c < 58);
}

void
Scanner::skipWhitespaceAndComments ()
{
  while (true)
  {
    // Skip whitespace
    int c = peek ();
    if (c != EOF && std::isspace (static_cast<char> (c)))
    {
      advance ();
    }
    // Skip comments starting with /* and ending with */
    else if (c == '/' && lookAhead () == '*')
    {
      skipCommentBlock ();
    }
    else
    {
      return;
    }
  }
}

void
Scanner::skipCommentBlock ()
{
  advance ();
  advance ();
  while (peek () != EOF && !(peek () == '*' && lookAhead () == '/'))
  {
    advance ();
  }
  if (peek () != EOF)
  {
    advance ();
    advance ();
  }
}

Token
Scanner::scanIdentifier ()
{
  SourceLocation start = location_;
  std::string s;
  while (peek () != EOF && isLetter (peek ()))
  {
    s += static_cast<char> (advance ());
  }

  std::array<std::string, 6> keywords = { "if",   "else",   "int",
                                          "void", "return", "while" };
  for (size_t i = 0; i < 6; ++i)
  {
    if (s == keywords[i])
    {
      return Token { .type = TokenType (i), .lexeme = s, .location = start };
    }
  }
  return Token { .type = TokenType::Id, .lexeme = s, .location = start };
}

Token
Scanner::scanNumber ()
{
  SourceLocation start = location_;
  std::string s;
  while (peek () != EOF && isNumber (peek ()))
  {
    s += static_cast<char> (advance ());
  }
  return Token { .type = TokenType::Num, .lexeme = s, .location = start };
}

void
Scanner::putBack ()
{
  --cursor_;
  --location_.column;
}