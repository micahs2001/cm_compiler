/******************************************************************************
 File Name:    PrintVisitor.cc
 Description:  Print Visitor class implementation for CMinus language
 Author:       Micah Schafer
 Date:         October 28, 2026


 *****************************************************************************/

/*****************************************************************************/
// Includes
#include <format>
#include <ostream>
#include <print>
#include <ranges>

#include "PrintVisitor.hh"

/*****************************************************************************/
// Print Visitor Class Implementation

PrintVisitor::PrintVisitor (std::ostream& out)
    : out_ (out)
{
}

/*****************************************************************************/
// PrintVisitor Overload Implementations

void
PrintVisitor::operator() (const ProgramNode& node)
{
  printLine ("ProgramNode:");
  ++depth_;
  for (const auto& decl : node.declarations)
    visit (decl);
  --depth_;
}

void
PrintVisitor::operator() (const VarDeclNode& node)
{
  printLine ("VarDeclNode: {} {}{}",
             typeSpecToString (node.type),
             node.identifier,
             (node.arraySize) ? std::format ("[{}]", *node.arraySize) : "");
}

void
PrintVisitor::operator() (const FunDeclNode& node)
{
  printLine (
    "FunDeclNode: {} {}", typeSpecToString (node.returnType), node.identifier);
  ++depth_;
  printLine ("Params:");
  ++depth_;
  for (const auto& param : node.params)
    visit (param);
  --depth_;
  printLine ("Body:");
  ++depth_;
  visit (node.body);
  depth_ -= 2;
}

void
PrintVisitor::operator() (const ParamDeclNode& node)
{
  printLine ("ParamDeclNode: {} {}{}",
             typeSpecToString (node.type),
             node.identifier,
             (node.isArray) ? "[]" : "");
}

void
PrintVisitor::operator() (const CompoundStmtNode& node)
{
  printLine ("CompoundStmtNode:");
  ++depth_;
  if (not node.localDecls.empty ())
  {
    printLine ("Local Decls:");
    ++depth_;
    for (const auto& decl : node.localDecls)
      visit (decl);
    --depth_;
  }
  printLine ("Statements:");
  ++depth_;
  for (const auto& stmt : node.stmtList)
    visit (stmt);
  depth_ -= 2;
}

void
PrintVisitor::operator() (const ExprStmtNode& node)
{
  printLine ("ExprStmtNode:");
  ++depth_;
  visit (node.expr);
  --depth_;
}

void
PrintVisitor::operator() (const IfStmtNode& node)
{
  printLine ("IfStmtNode:");
  ++depth_;
  printLine ("Condition:");
  ++depth_;
  visit (node.condition);
  --depth_;
  printLine ("Then:");
  ++depth_;
  visit (node.thenStmt);
  --depth_;
  if (node.elseStmt != nullptr)
  {
    printLine ("Else:");
    ++depth_;
    visit (node.elseStmt);
    --depth_;
  }
  --depth_;
}

void
PrintVisitor::operator() (const WhileStmtNode& node)
{
  printLine ("WhileStmtNode:");
  ++depth_;
  printLine ("Condition:");
  ++depth_;
  visit (node.condition);
  --depth_;
  printLine ("Body:");
  ++depth_;
  visit (node.body);
  depth_ -= 2;
}

void
PrintVisitor::operator() (const ReturnStmtNode& node)
{
  printLine ("ReturnStmtNode:");
  ++depth_;
  visit (node.expr);
  --depth_;
}

void
PrintVisitor::operator() (const AssignExprNode& node)
{
  printLine ("AssignExprNode:");
  ++depth_;
  printLine ("LHS:");
  ++depth_;
  visit (node.var);
  --depth_;
  printLine ("RHS:");
  ++depth_;
  visit (node.expression);
  depth_ -= 2;
}

void
PrintVisitor::operator() (const BinaryExprNode& node)
{
  printLine ("BinaryExprNode ({}):", tokenToString (node.op));
  ++depth_;
  visit (node.left);
  visit (node.right);
  --depth_;
}

void
PrintVisitor::operator() (const VarExprNode& node)
{
  printLine ("VarExprNode: {}", node.identifier);
  if (node.index)
  {
    ++depth_;
    printLine ("Index:");
    ++depth_;
    visit (node.index);
    depth_ -= 2;
  }
}

void
PrintVisitor::operator() (const CallExprNode& node)
{
  printLine ("CallExprNode: {}", node.identifier);
  ++depth_;
  printLine ("Args:");
  ++depth_;
  for (const auto& arg : node.args)
    visit (arg);
  depth_ -= 2;
}

void
PrintVisitor::operator() (const NumExprNode& node)
{
  printLine ("NumExprNode: {}", node.value);
}

template<typename... Args>
void
PrintVisitor::printLine (std::format_string<Args...> fmt, Args&&... args)
{
  for (int _ : std::views::iota (0u, (unsigned) depth_))
  {
    out_ << "  ";
  }
  std::println (out_, fmt, std::forward<Args> (args)...);
}

const char*
PrintVisitor::typeSpecToString (TypeSpecifier type)
{
  switch (type)
  {
    case TypeSpecifier::Int:
      return "int";
    case TypeSpecifier::Void:
      return "void";
    default:
      return "unknown-type";
  }
}