/******************************************************************************
 File Name:    PrintVisitor.hh
 Description:  Print Visitor class definition for CMinus language
 Author:       Micah Schafer
 Date:         October 28, 2026

 Defines visitor operators for all grammar symbol nodes detailed in Ast.hh
 Calling this.visit(ASTNodePtr) returns AST tree rooted at that ptr
 *****************************************************************************/

/*****************************************************************************/
// Includes
#pragma once

#include <format>
#include <ostream>

#include "Visitor.hh"

/*****************************************************************************/
// PrintVisitor Class Definition

class PrintVisitor : public Visitor
{
public:
  explicit PrintVisitor (std::ostream& out);

  /*****************************************************************************/
  // Operator Overloads
  // Defines visit(node) functionality for AST Nodes

  void
  operator() (const ProgramNode& node) override;

  void
  operator() (const VarDeclNode& node) override;

  void
  operator() (const FunDeclNode& node) override;

  void
  operator() (const ParamDeclNode& node) override;

  void
  operator() (const CompoundStmtNode& node) override;

  void
  operator() (const ExprStmtNode& node) override;

  void
  operator() (const IfStmtNode& node) override;

  void
  operator() (const WhileStmtNode& node) override;

  void
  operator() (const ReturnStmtNode& node) override;

  void
  operator() (const AssignExprNode& node) override;

  void
  operator() (const BinaryExprNode& node) override;

  void
  operator() (const VarExprNode& node) override;

  void
  operator() (const CallExprNode& node) override;

  void
  operator() (const NumExprNode& node) override;

private:
  // println wrapper help function that indents according to depth_ and prints
  // formatted string to out_ stream
  template<typename... Args>
  void
  printLine (std::format_string<Args...> fmt, Args&&... args);

  static const char*
  typeSpecToString (TypeSpecifier type);

  std::ostream& out_;
  int depth_ { 0 };
};