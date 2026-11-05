
#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <unordered_map>
#include <vector>

#include "SymbolTableVisitor.hh"

void
printScopes (
  std::vector<std::unordered_map<std::string, Symbol>> vector_of_maps)
{
  for (size_t i = 0; i < vector_of_maps.size (); ++i)
  {
    std::cout << "Map at index " << i << ":\n";
    for (const auto& [key, value] : vector_of_maps[i])
    {
      std::cout << "  " << key << " => " << "symbol" << "\n";
    }
    std::cout << "\n";
  }
}

/**************************************************************************/

SymbolTableVisitor::SymbolTableVisitor ()
    : scopeStack_ (std::vector<std::unordered_map<std::string, Symbol>> (1))
{
}

/*********************************************************************/
// Declaration Nodes

void
SymbolTableVisitor::operator() (const VarDeclNode& node)
{
  std::println ("Encountered a Var");
  insertSymbol (node.identifier, &node);
  printScopes (scopeStack_);
}

void
SymbolTableVisitor::operator() (const FunDeclNode& node)
{
  std::println ("Encountered a Fun");
  insertSymbol (node.identifier, &node);
  printScopes (scopeStack_);
  enterScope ();
  Visitor::operator() (node);
  exitScope ();
}

void
SymbolTableVisitor::operator() (const ParamDeclNode& node)
{
  std::println ("Encountered a Param");
  insertSymbol (node.identifier, &node);
  printScopes (scopeStack_);
}

/*********************************************************************/
// Scope-changing Nodes

void
SymbolTableVisitor::operator() (const IfStmtNode& node)
{
  std::println ("Encountered an if Stmt");
  Visitor::visit (node.condition);
  enterScope ();
  Visitor::visit (node.thenStmt);
  exitScope ();
  enterScope ();
  Visitor::visit (node.elseStmt);
  exitScope ();

  printScopes (scopeStack_);
}

void
SymbolTableVisitor::operator() (const WhileStmtNode& node)
{
  std::println ("Encountered a while Stmt");
  visit (node.condition);
  enterScope ();
  visit (node.body);
  exitScope ();

  printScopes (scopeStack_);
}

/*********************************************************************/
//

void
SymbolTableVisitor::enterScope ()
{
  scopeStack_.push_back (std::unordered_map<std::string, Symbol> {});
}

void
SymbolTableVisitor::exitScope ()
{
  scopeStack_.pop_back ();
}

bool
SymbolTableVisitor::insertSymbol (const std::string& name, Symbol sym)
{
  return scopeStack_.back ().insert ({ name, sym }).second;
}

std::optional<Symbol>
SymbolTableVisitor::lookupSymbol (const std::string& name) const
{
  return {};
}

// Built-in I/O function decls for input() and output().
FunDeclNode inputDecl_;
FunDeclNode outputDecl_;
;