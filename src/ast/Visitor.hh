/******************************************************************************
 File Name:    Visitor.hh
 Description:  Virtual Visitor class definition for CMinus language
 Author:       Micah Schafer
 Date:         November 9, 2026

 Defines default traversal visitor operators for all grammar symbol nodes
 detailed in Ast.hh
 *****************************************************************************/

/*****************************************************************************/
// Includes
#pragma once

#include "Ast.hh"
#include "exceptions/Exceptions.hh"

/*****************************************************************************/
// Visitor Class Definition

class Visitor
{
public:
  explicit Visitor ();
  virtual ~Visitor () = default;

  // Call with the root node to traverse the AST.
  void
  visit (const AstNodePtr& node);

  virtual void
  operator() (const ProgramNode& node);

  virtual void
  operator() (const VarDeclNode& node);

  virtual void
  operator() (const FunDeclNode& node);

  virtual void
  operator() (const ParamDeclNode& node);

  virtual void
  operator() (const CompoundStmtNode& node);

  virtual void
  operator() (const ExprStmtNode& node);

  virtual void
  operator() (const IfStmtNode& node);

  virtual void
  operator() (const WhileStmtNode& node);

  virtual void
  operator() (const ReturnStmtNode& node);

  virtual void
  operator() (const AssignExprNode& node);

  virtual void
  operator() (const BinaryExprNode& node);

  virtual void
  operator() (const VarExprNode& node);

  virtual void
  operator() (const CallExprNode& node);

  virtual void
  operator() (const NumExprNode& node);

protected:
  [[nodiscard]]
  SourceLocation
  currentLocation () const noexcept
  {
    return currentLocation_;
  }

private:
  SourceLocation currentLocation_;
};