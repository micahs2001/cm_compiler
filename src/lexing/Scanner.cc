/******************************************************************************
 File Name:    Scanner.cc
 Description:  Scanner class for CMinus language
 Author:       Micah Schafer
 Date:         September 24, 2026

 *Function descriptors in header file
 *****************************************************************************/

#include <cctype>
#include <string>

#include "lexing/Scanner.hh"

Scanner::Scanner (std::string source)
    : source_ (source)
{
}

Token
Scanner::scanNext ()
{
  skipWhitespaceAndComments ();
  if (peek () == EOF)
  {
    return Token { TokenType::EndOfFile, "", location_ };
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
      return Token { TokenType::Plus, "+", start };
    case '-':
      return Token { TokenType::Minus, "-", start };
    case '*':
      return Token { TokenType::Multiply, "*", start };
    case '/':
      return Token { TokenType::Divide, "/", start };
    case '<':
    {
      if (peek () == '=')
      {
        advance ();
        return Token { TokenType::LessEqual, "<=", start };
      }
      return Token { TokenType::Less, "<", start };
    }
    case '>':
    {
      if (peek () == '=')
      {
        advance ();
        return Token { TokenType::GreaterEqual, ">=", start };
      }
      return Token { TokenType::Greater, ">", start };
    }
    case '=':
    {
      if (peek () == '=')
      {
        advance ();
        return Token { TokenType::Equal, "==", start };
      }
      return Token { TokenType::Assign, "=", start };
    }
    case '!':
    {
      if (peek () == '=')
      {
        advance ();
        return Token { TokenType::NotEqual, "!=", start };
      }
      break;
    }
    case ';':
      return Token { TokenType::Semi, ";", start };
    case ',':
      return Token { TokenType::Comma, ",", start };
    case '(':
      return Token { TokenType::LParen, "(", start };
    case ')':
      return Token { TokenType::RParen, ")", start };
    case '[':
      return Token { TokenType::LBrack, "[", start };
    case ']':
      return Token { TokenType::RBrack, "]", start };
    case '{':
      return Token { TokenType::LBrace, "{", start };
    case '}':
      return Token { TokenType::RBrace, "}", start };
  }
  return Token {
    TokenType::Error, std::string (1, static_cast<char> (c)), start
  };
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

  std::string keywords[6] = { "if", "else", "int", "void", "return", "while" };
  for (size_t i = 0; i < 6; ++i)
  {
    if (s == keywords[i])
    {
      return Token { TokenType (i), s, start };
    }
  }
  return Token { TokenType::Id, s, start };
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
  return Token { TokenType::Num, s, start };
}

void
Scanner::putBack ()
{
  --cursor_;
  --location_.column;
}