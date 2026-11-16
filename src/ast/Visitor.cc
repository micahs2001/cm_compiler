/******************************************************************************
 File Name:    Visitor.cc
 Description:  Virtual Visitor class implementation for CMinus language
 Author:       Micah Schafer
 Date:         November 9, 2026

 *****************************************************************************/

/*****************************************************************************/
// Includes
#include "Visitor.hh"

/*****************************************************************************/
// Parser Class Implementation

Visitor::Visitor () = default;

void
Visitor::visit (const AstNodePtr& node)
{
  if (!node)
    return;

  const SourceLocation previousLocation = currentLocation_;

  // Save location of caller
  // Destructing restore struct restores previous position
  currentLocation_ = node->location;
  struct LocationRestore
  {
    SourceLocation& current;
    SourceLocation previous;
    ~LocationRestore () { current = previous; }
  } restore { .current = currentLocation_, .previous = previousLocation };

  std::visit (*this, node->data);
}

/*****************************************************************************/
// Visitor Overload Implementations

void
Visitor::operator() (const ProgramNode& node)
{
  for (const auto& decl : node.declarations)
    visit (decl);
}

void
Visitor::operator() (const VarDeclNode& node)
{
}

void
Visitor::operator() (const FunDeclNode& node)
{
  for (const auto& param : node.params)
    visit (param);
  visit (node.body);
}

void
Visitor::operator() (const ParamDeclNode& node)
{
}

void
Visitor::operator() (const CompoundStmtNode& node)
{
  for (const auto& decl : node.localDecls)
    visit (decl);
  for (const auto& stmt : node.stmtList)
    visit (stmt);
}

void
Visitor::operator() (const ExprStmtNode& node)
{
  visit (node.expr);
}

void
Visitor::operator() (const IfStmtNode& node)
{
  visit (node.condition);
  visit (node.thenStmt);
  visit (node.elseStmt);
}

void
Visitor::operator() (const WhileStmtNode& node)
{
  visit (node.condition);
  visit (node.body);
}

void
Visitor::operator() (const ReturnStmtNode& node)
{
  visit (node.expr);
}

void
Visitor::operator() (const AssignExprNode& node)
{
  visit (node.var);
  visit (node.expression);
}

void
Visitor::operator() (const BinaryExprNode& node)
{
  visit (node.left);
  visit (node.right);
}

void
Visitor::operator() (const VarExprNode& node)
{
  visit (node.index);
}

void
Visitor::operator() (const CallExprNode& node)
{
  for (const auto& arg : node.args)
    visit (arg);
}

void
Visitor::operator() (const NumExprNode& node)
{
}