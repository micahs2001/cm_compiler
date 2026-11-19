/*
    Operators being applied to non-integer types.

    Assigning to array or function names.

    Calling non-functions.

    Types of arguments and parameters not being compatible.

    Too few or too many function arguments.

    Returning a value from a void function.

    Not having at least one return expression for a non-void function.

    Declaring variables or parameters as void.

    Not declaring main last.
*/

/******************************************************************************
 File Name:    SemanticAnalysis.hh
 Description:  Semantic Analysis Visitor class definition for CMinus language
 Author:       Micah Schafer
 Date:         November 16, 2026

 *****************************************************************************/

/*****************************************************************************/
// Includes

#include <format>
#include <ostream>
#include <print>

#include "SemanticAnalysisVisitor.hh"
#include "ast/Ast.hh"
#include "exceptions/CompilerException.hh"
#include "exceptions/Exceptions.hh"
#include "exceptions/InvalidSyntaxException.hh"
#include "exceptions/SemanticErrorException.hh"

/*****************************************************************************/
// SemanticAnalysisVisitor Class Definition

SemanticAnalysisVisitor::SemanticAnalysisVisitor (std::ostream& out)
    : out_ (out)
{
}

/*****************************************************************************/

const std::vector<std::exception_ptr>&
SemanticAnalysisVisitor::getErrors ()
{
  return errors_;
}

// Operator Overloads
// Defines visit(node) functionality for AST Nodes

// void
// SemanticAnalysisVisitor::operator() (const ProgramNode& node)
// {
// }

// void
// SemanticAnalysisVisitor::operator() (const VarDeclNode& node)
// {
// }

// void
// SemanticAnalysisVisitor::operator() (const FunDeclNode& node) override;

// void
// SemanticAnalysisVisitor::operator() (const ParamDeclNode& node) override;

// void
// SemanticAnalysisVisitor::operator() (const CompoundStmtNode& node)
// override;

// void
// SemanticAnalysisVisitor::operator() (const ExprStmtNode& node) override;

// void
// SemanticAnalysisVisitor::operator() (const IfStmtNode& node) override;

// void
// SemanticAnalysisVisitor::operator() (const WhileStmtNode& node) override;

// void
// operator() (const ReturnStmtNode& node) override;

/******************************************************************/

// void
// operator() (const AssignExprNode& node)
// {
// }

// void
// operator() (const BinaryExprNode& node)
// {
// }

//
void
SemanticAnalysisVisitor::operator() (const VarExprNode& node)
{
  if (node.index)
  {
    if (std::holds_alternative<const VarDeclNode*> (node.declNode) and
        !std::get<const VarDeclNode*> (node.declNode)->arraySize)
    {
      errors_.emplace_back (std::make_exception_ptr (SemanticErrorException (
        std::format ("Variable \'{}\' cannot be indexed", node.identifier),
        currentLocation ())));
    }

    if (std::holds_alternative<const ParamDeclNode*> (node.declNode) and
        !std::get<const ParamDeclNode*> (node.declNode)->isArray)
    {
      errors_.emplace_back (std::make_exception_ptr (SemanticErrorException (
        std::format ("Variable \'{}\' cannot be indexed", node.identifier),
        currentLocation ())));
    }
  }
}

// void
// SemanticAnalysisVisitor::operator() (const CallExprNode& node)
// {
// }

// void
// SemanticAnalysisVisitor::operator() (const NumExprNode& node)
// {
// }