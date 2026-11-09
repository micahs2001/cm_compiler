/******************************************************************************
 File Name:    Parser.cc
 Description:  Parser class implementation for CMinus language
 Author:       Micah Schafer
 Date:         October 28, 2026

 Detailed product rules in Parser.hh
 *****************************************************************************/

/*****************************************************************************/
// Includes
#include <algorithm>
#include <print>
#include <vector>

#include "ast/Ast.hh"
#include "exceptions/Exceptions.hh"
#include "parsing/Parser.hh"
#include "tokenizing/Token.hh"

/*****************************************************************************/
// Parser Class Implementation

Parser::Parser (Scanner scanner)
    : scanner_ (std::move (scanner))
{
}

AstNodePtr
Parser::parse ()
{
  advance ();
  AstNodePtr prog = parseProgram ();

  return prog;
}

void
Parser::printTokens (std::ostream& out)
{
  out << ("Token           Lexeme           Location\n");
  out << ("=====           ======           ========\n");
  for (auto token : toks_)
  {
    out << std::format ("{:<16}{:<17}({},{})\n",
                        tokenToString (token.type),
                        token.lexeme,
                        token.location.line,
                        token.location.column);
  }
}

Token
Parser::advance ()
{
  Token save = current_;
  toks_.push_back (save);
  current_ = scanner_.scanNext ();
  if (check (TokenType::Error))
    throw InvalidTokenException (current_.location);
  return save;
}

Token
Parser::match (TokenType expected)
{
  if (!check (expected))
  {
    throw InvalidSyntaxException (
      current_.location, { expected }, current_.lexeme);
    return {};
  }
  return advance ();
}

bool
Parser::check (TokenType type) const
{
  return (current_.type == type);
}

bool
Parser::findToken (std::initializer_list<TokenType> types) const
{
  return std::ranges::find (types, current_.type) != types.end ();
}

template<std::invocable Fun>
void
Parser::repeatUntil (Fun rule, std::initializer_list<TokenType> terminators)
{
  while (!findToken (terminators) && current_.type != TokenType::EndOfFile)
    rule ();
}

template<std::invocable Fun>
void
Parser::repeatWhile (Fun rule, std::initializer_list<TokenType> first)
{
  while (findToken (first))
    rule ();
}

AstNodePtr
Parser::parseProgram ()
{
  const SourceLocation start = current_.location;
  std::vector<AstNodePtr> decls;
  decls.push_back (parseDecl ());
  repeatUntil ([this, &decls] { decls.push_back (parseDecl ()); },
               { TokenType::EndOfFile });
  return makeAstNode<ProgramNode> (start, std::move (decls));
}

AstNodePtr
Parser::parseDecl ()
{
  TypeSpecifier ty = parseTypeSpec ();
  Token identifier = match (TokenType::Id);
  std::string idName = identifier.lexeme;

  switch (current_.type)
  {
    case TokenType::Semi:
    {
      match (TokenType::Semi);
      return makeAstNode<VarDeclNode> (
        identifier.location, ty, std::move (idName));
    }
    case TokenType::LBrack:
    {
      match (TokenType::LBrack);
      int arraySize = std::stoi (match (TokenType::Num).lexeme);
      match (TokenType::RBrack);
      match (TokenType::Semi);
      return makeAstNode<VarDeclNode> (
        identifier.location, ty, std::move (idName), arraySize);
    }
    case TokenType::LParen:
    {
      match (TokenType::LParen);
      std::vector<AstNodePtr> params = parseFullParams ();
      match (TokenType::RParen);
      AstNodePtr cmpstmt = parseCompoundStmt ();
      return makeAstNode<FunDeclNode> (identifier.location,
                                       ty,
                                       std::move (idName),
                                       std::move (params),
                                       std::move (cmpstmt));
    }
    default:
    {
      throw InvalidSyntaxException (
        current_.location,
        { TokenType::Semi, TokenType::LBrack, TokenType::LParen },
        current_.lexeme);
    }
  }
}

AstNodePtr
Parser::parseVarDeclaration ()
{
  TypeSpecifier ty = parseTypeSpec ();
  Token identifier = match (TokenType::Id);
  std::string idName = identifier.lexeme;

  switch (current_.type)
  {
    case TokenType::Semi:
    {
      match (TokenType::Semi);
      return makeAstNode<VarDeclNode> (
        identifier.location, ty, std::move (idName));
    }
    case TokenType::LBrack:
    {
      match (TokenType::LBrack);
      int arraySize = std::stoi (match (TokenType::Num).lexeme);
      match (TokenType::RBrack);
      match (TokenType::Semi);
      return makeAstNode<VarDeclNode> (
        identifier.location, ty, std::move (idName), arraySize);
    }
    default:
    {
      throw InvalidSyntaxException (current_.location,
                                    { TokenType::Semi, TokenType::LBrack },
                                    current_.lexeme);
    }
  }
}

TypeSpecifier
Parser::parseTypeSpec ()
{
  switch (current_.type)
  {
    case TokenType::Int:
      match (TokenType::Int);
      return TypeSpecifier::Int;
    case TokenType::Void:
      match (TokenType::Void);
      return TypeSpecifier::Void;
    default:
      throw InvalidSyntaxException (current_.location,
                                    { TokenType::Int, TokenType::Void },
                                    current_.lexeme);
  }
}

std::vector<AstNodePtr>
Parser::parseFullParams ()
{
  std::vector<AstNodePtr> params;
  bool isArray { false };
  if (check (TokenType::Void))
  {
    match (TokenType::Void);
    // If "void" is at the end of params
    if (check (TokenType::RParen))
      return params;
    Token identifier = match (TokenType::Id);
    std::string idName = identifier.lexeme;
    if (check (TokenType::LBrack))
    {
      match (TokenType::LBrack);
      match (TokenType::RBrack);
      isArray = true;
    }
    params.push_back (makeAstNode<ParamDeclNode> (
      identifier.location, TypeSpecifier::Void, std::move (idName), isArray));
  }
  else
    params.push_back (parseParam ());

  repeatWhile (
    [this, &params]
    {
      match (TokenType::Comma);
      params.push_back (parseParam ());
    },
    { TokenType::Comma });
  return params;
}

AstNodePtr
Parser::parseParam ()
{
  bool isArray { false };
  TypeSpecifier ty = parseTypeSpec ();
  Token identifier = match (TokenType::Id);
  std::string idName = identifier.lexeme;
  if (check (TokenType::LBrack))
  {
    match (TokenType::LBrack);
    match (TokenType::RBrack);
    isArray = true;
  }
  return makeAstNode<ParamDeclNode> (
    identifier.location, ty, std::move (idName), isArray);
}

AstNodePtr
Parser::parseCompoundStmt ()
{
  const SourceLocation start = current_.location;
  match (TokenType::LBrace);
  std::vector<AstNodePtr> localDecls;
  repeatWhile ([this, &localDecls]
               { localDecls.push_back (parseVarDeclaration ()); },
               { TokenType::Int, TokenType::Void });
  std::vector<AstNodePtr> stmtList;
  repeatUntil ([this, &stmtList] { stmtList.push_back (parseStatement ()); },
               { TokenType::RBrace });
  match (TokenType::RBrace);
  return makeAstNode<CompoundStmtNode> (
    start, std::move (localDecls), std::move (stmtList));
}

AstNodePtr
Parser::parseStatement ()
{
  switch (current_.type)
  {
    case TokenType::Semi:
    case TokenType::LParen:
    case TokenType::Id:
    case TokenType::Num:
      return parseExpressionStmt ();
    case TokenType::LBrace:
      return parseCompoundStmt ();
    case TokenType::If:
      return parseSelectionStmt ();
    case TokenType::While:
      return parseIterationStmt ();
    case TokenType::Return:
      return parseReturnStmt ();
    default:
      throw InvalidSyntaxException (current_.location,
                                    { TokenType::Semi,
                                      TokenType::LParen,
                                      TokenType::Id,
                                      TokenType::Num,
                                      TokenType::LBrace,
                                      TokenType::If,
                                      TokenType::While,
                                      TokenType::Return },
                                    current_.lexeme);
  }
}

AstNodePtr
Parser::parseExpressionStmt ()
{
  const SourceLocation start = current_.location;
  AstNodePtr exp;
  switch (current_.type)
  {
    case TokenType::LParen:
    case TokenType::Id:
    case TokenType::Num:
      exp = parseExpression ();
      break;
    case TokenType::Semi:
      break;
    default:
      throw InvalidSyntaxException (
        current_.location,
        { TokenType::Semi, TokenType::LParen, TokenType::Id, TokenType::Num },
        current_.lexeme);
  }
  match (TokenType::Semi);
  return makeAstNode<ExprStmtNode> (start, std::move (exp));
}

AstNodePtr
Parser::parseSelectionStmt ()
{
  const SourceLocation start = current_.location;
  match (TokenType::If);
  match (TokenType::LParen);
  AstNodePtr condition = parseExpression ();
  match (TokenType::RParen);
  AstNodePtr thenStmt = parseStatement ();
  AstNodePtr elseStmt;
  if (check (TokenType::Else))
  {
    match (TokenType::Else);
    elseStmt = parseStatement ();
  }
  return makeAstNode<IfStmtNode> (
    start, std::move (condition), std::move (thenStmt), std::move (elseStmt));
}

AstNodePtr
Parser::parseIterationStmt ()
{
  const SourceLocation start = current_.location;
  match (TokenType::While);
  match (TokenType::LParen);
  AstNodePtr condition = parseExpression ();
  match (TokenType::RParen);
  AstNodePtr body = parseStatement ();
  return makeAstNode<WhileStmtNode> (
    start, std::move (condition), std::move (body));
}

AstNodePtr
Parser::parseReturnStmt ()
{
  const SourceLocation start = current_.location;
  AstNodePtr expr;
  match (TokenType::Return);
  if (!check (TokenType::Semi))
    expr = parseExpression ();
  match (TokenType::Semi);
  return makeAstNode<ReturnStmtNode> (start, std::move (expr));
}

AstNodePtr
Parser::parseExpression ()
{
  AstNodePtr leftTree;
  // If assignExpr or first factor is callExpr or varExpr
  if (check (TokenType::Id))
  {
    Token identifier = match (TokenType::Id);
    std::string idName = identifier.lexeme;

    // If <var>
    // Parse and return Assignment exp or set factor = <var>
    if (not check (TokenType::LParen))
    {
      AstNodePtr varIndex;
      if (check (TokenType::LBrack))
      {
        match (TokenType::LBrack);
        varIndex = parseExpression ();
        match (TokenType::RBrack);
      }
      // Roll <var> into factor
      leftTree = makeAstNode<VarExprNode> (
        identifier.location, std::move (idName), std::move (varIndex));
      // If assign expr
      if (check (TokenType::Assign))
      {
        match (TokenType::Assign);
        AstNodePtr assignExpr = parseExpression ();
        const SourceLocation assignmentLocation = leftTree->location;
        return makeAstNode<AssignExprNode> (
          assignmentLocation, std::move (leftTree), std::move (assignExpr));
      }
    }
    else
    // Roll <call> into factor
    {
      match (TokenType::LParen);
      std::vector<AstNodePtr> args = parseArgs ();
      match (TokenType::RParen);
      leftTree = makeAstNode<CallExprNode> (
        identifier.location, std::move (idName), std::move (args));
    }
  }
  else
  {
    leftTree = parseFactor ();
  }

  // leftTree is first factor of expression
  repeatWhile (
    [this, &leftTree]
    {
      // Build tree for first term
      Token operatorToken = match (current_.type);
      AstNodePtr rightTree = parseFactor ();
      leftTree = makeAstNode<BinaryExprNode> (operatorToken.location,
                                              operatorToken.type,
                                              std::move (leftTree),
                                              std::move (rightTree));
    },
    { TokenType::Multiply, TokenType::Divide });

  repeatWhile (
    [this, &leftTree]
    {
      // Build tree for first additive expression
      Token operatorToken = match (current_.type);
      AstNodePtr rightTree = parseTerm ();
      leftTree = makeAstNode<BinaryExprNode> (operatorToken.location,
                                              operatorToken.type,
                                              std::move (leftTree),
                                              std::move (rightTree));
    },
    { TokenType::Plus, TokenType::Minus });

  // Attach right side additive expression if relop is seen
  if (isRelop ())
  {
    Token operatorToken = match (current_.type);
    AstNodePtr rightTree = parseAdditiveExpr ();
    leftTree = makeAstNode<BinaryExprNode> (operatorToken.location,
                                            operatorToken.type,
                                            std::move (leftTree),
                                            std::move (rightTree));
  }
  return leftTree;
}

bool
Parser::isRelop ()
{
  return (check (TokenType::LessEqual) || check (TokenType::Less) ||
          check (TokenType::Greater) || check (TokenType::GreaterEqual) ||
          check (TokenType::Equal) || check (TokenType::NotEqual));
}

AstNodePtr
Parser::parseAdditiveExpr ()
{
  AstNodePtr leftTree = parseTerm ();
  repeatWhile (
    [this, &leftTree]
    {
      Token operatorToken = match (current_.type);
      AstNodePtr rightTree = parseTerm ();
      leftTree = makeAstNode<BinaryExprNode> (operatorToken.location,
                                              operatorToken.type,
                                              std::move (leftTree),
                                              std::move (rightTree));
    },
    { TokenType::Plus, TokenType::Minus });
  return leftTree;
}

AstNodePtr
Parser::parseTerm ()
{
  AstNodePtr leftTree = parseFactor ();
  repeatWhile (
    [this, &leftTree]
    {
      Token operatorToken = match (current_.type);
      AstNodePtr rightTree = parseFactor ();
      leftTree = makeAstNode<BinaryExprNode> (operatorToken.location,
                                              operatorToken.type,
                                              std::move (leftTree),
                                              std::move (rightTree));
    },
    { TokenType::Multiply, TokenType::Divide });
  return leftTree;
}

AstNodePtr
Parser::parseFactor ()
{
  switch (current_.type)
  {
    case TokenType::LParen:
    {
      match (TokenType::LParen);
      AstNodePtr expr = parseExpression ();
      match (TokenType::RParen);
      return expr;
    }
    case TokenType::Num:
    {
      Token number = match (TokenType::Num);
      int value = std::stoi (number.lexeme);
      return makeAstNode<NumExprNode> (number.location, value);
    }
    case TokenType::Id:
    {
      Token identifier = match (TokenType::Id);
      std::string idName = identifier.lexeme;
      switch (current_.type)
      {
        case TokenType::LBrack:
        {
          // Var Factor with index
          match (TokenType::LBrack);
          AstNodePtr varIndex;
          varIndex = parseExpression ();
          match (TokenType::RBrack);
          return makeAstNode<VarExprNode> (
            identifier.location, std::move (idName), std::move (varIndex));
        }
        // Call Factor
        case TokenType::LParen:
        {
          match (TokenType::LParen);
          std::vector<AstNodePtr> args = parseArgs ();
          match (TokenType::RParen);
          return makeAstNode<CallExprNode> (
            identifier.location, std::move (idName), std::move (args));
        }
        default:
          // Var Factor without index
          return makeAstNode<VarExprNode> (identifier.location,
                                           std::move (idName));
      }
    }
    break;
    default:
      throw InvalidSyntaxException (current_.location,
                                    {
                                      TokenType::LParen,
                                      TokenType::Id,
                                      TokenType::Num,
                                    },
                                    current_.lexeme);
  }
}

std::vector<AstNodePtr>
Parser::parseArgs ()
{
  std::vector<AstNodePtr> args;
  if (check (TokenType::RParen))
    return args;
  args.push_back (parseExpression ());
  repeatWhile (
    [this, &args]
    {
      match (TokenType::Comma);
      args.push_back (parseExpression ());
    },
    { TokenType::Comma });
  return args;
}