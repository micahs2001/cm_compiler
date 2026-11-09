
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "Ast.hh"
#include "SymbolTableVisitor.hh"
#include "exceptions/Exceptions.hh"

// Temp fun for testing
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
  inputDecl_.returnType = TypeSpecifier::Void;
  outputDecl_.returnType = TypeSpecifier::Void;
  insertSymbol ("input", &inputDecl_);
  insertSymbol ("output", &outputDecl_);
}

/*********************************************************************/
// Declaration Nodes

void
SymbolTableVisitor::operator() (const VarDeclNode& node)
{
  std::println ("Encountered a Var");
  if (not insertSymbol (node.identifier, &node))
  {
    throw SemanticErrorException ("Previously Declared Variable",
                                  currentLocation ());
  }

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
// Use Nodes

void
SymbolTableVisitor::operator() (const VarExprNode& node)
{
  auto var = lookupSymbol (node.identifier);
  if (var)
  {
    if (std::holds_alternative<const VarDeclNode*> (*var))
      node.declNode = std::get<const VarDeclNode*> (*var);

    else if (std::holds_alternative<const ParamDeclNode*> (*var))
      node.declNode = std::get<const ParamDeclNode*> (*var);
  }

  Visitor::operator() (node);
}

void
SymbolTableVisitor::operator() (const CallExprNode& node)
{
  auto var = lookupSymbol (node.identifier);
  if (var)
  {
    node.declNode = std::get<const FunDeclNode*> (*var);
  }

  Visitor::operator() (node);
}

/*********************************************************************/
//

void
SymbolTableVisitor::enterScope ()
{
  scopeStack_.emplace_back ();
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

const std::optional<Symbol>
SymbolTableVisitor::lookupSymbol (const std::string& name) const
{
  // Walk scope vector in reverse
  for (const auto& sco : std::views::reverse (scopeStack_))
  {
    // Look for var name in each scope
    if (sco.find (name) not_eq sco.end ())
    {
      // Return once found
      return sco.at (name);
    }
  }
  return {};
}

// Built-in I/O function decls for input() and output().
FunDeclNode inputDecl_;
FunDeclNode outputDecl_;
;