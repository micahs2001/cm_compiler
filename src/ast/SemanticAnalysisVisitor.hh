/******************************************************************************
 File Name:    SemanticAnalysis.hh
 Description:  Semantic Analysis Visitor class definition for CMinus language
 Author:       Micah Schafer
 Date:         November 16, 2026

 *****************************************************************************/

/*****************************************************************************/
// Includes
#pragma once

#include <format>
#include <ostream>

#include "Visitor.hh"
#include "exceptions/CompilerException.hh"

/*****************************************************************************/
// SemanticAnalysisVisitor Class Definition

class SemanticAnalysisVisitor : public Visitor
{
public:
  explicit SemanticAnalysisVisitor (std::ostream& out);

  /*****************************************************************************/

  const std::vector<std::exception_ptr>&
  getErrors ();

  // Operator Overloads
  // Defines visit(node) functionality for AST Nodes

  // void
  // operator() (const ProgramNode& node) override;

  // void
  // operator() (const VarDeclNode& node) override;

  // void
  // operator() (const FunDeclNode& node) override;

  // void
  // operator() (const ParamDeclNode& node) override;

  // void
  // operator() (const CompoundStmtNode& node) override;

  // void
  // operator() (const ExprStmtNode& node) override;

  // void
  // operator() (const IfStmtNode& node) override;

  // void
  // operator() (const WhileStmtNode& node) override;

  // void
  // operator() (const ReturnStmtNode& node) override;

  // void
  // operator() (const AssignExprNode& node) override;

  // void
  // operator() (const BinaryExprNode& node) override;

  void
  operator() (const VarExprNode& node) override;

  // void
  // operator() (const CallExprNode& node) override;

  // void
  // operator() (const NumExprNode& node) override;

private:
  std::vector<std::exception_ptr> errors_ {};
  std::ostream& out_;
};