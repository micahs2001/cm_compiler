/******************************************************************************
 File Name:    SymbolTableVisitor.cc
 Description:  Symbol Table Visitor class implementation for CMinus language
 Author:       Micah Schafer
 Date:         November 9, 2026

 *****************************************************************************/

#include <format>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "SymbolTableVisitor.hh"
#include "exceptions/Exceptions.hh"

/**************************************************************************/

SymbolTableVisitor::SymbolTableVisitor ()
    : scopeStack_ (std::vector<std::unordered_map<std::string, Symbol>> (1))
{
  // Ensure input and output are inserted at global level
  insertSymbol ("input", &inputDecl_);
  insertSymbol ("output", &outputDecl_);
}

/*********************************************************************
 * Declaration Nodes
 *
 * Declaration nodes will throw exception if they are redeclaring
 * a variable, parameter, or function
 *********************************************************************/

void
SymbolTableVisitor::operator() (const VarDeclNode& node)
{
  if (not insertSymbol (node.identifier, &node))
  {
    throw SemanticErrorException (
      std::format ("Redeclaration of variable \'{}\'", node.identifier),
      currentLocation ());
  }
}

void
SymbolTableVisitor::operator() (const FunDeclNode& node)
{
  if (not insertSymbol (node.identifier, &node))
  {
    throw SemanticErrorException (
      std::format ("Redeclaration of function \'{}\'", node.identifier),
      currentLocation ());
  }

  enterScope ();
  Visitor::operator() (node);
  exitScope ();
}

void
SymbolTableVisitor::operator() (const ParamDeclNode& node)
{
  if (not insertSymbol (node.identifier, &node))
  {
    throw SemanticErrorException (
      std::format ("Redeclaration of parameter \'{}\'", node.identifier),
      currentLocation ());
  }
}

/*********************************************************************
 * Scope-Changing Nodes
 *
 * Scope depth will increase on Function Declaration (Already implemented in
 * previous section), If statements, and While statements
 *
 *********************************************************************/

void
SymbolTableVisitor::operator() (const IfStmtNode& node)
{
  Visitor::visit (node.condition);
  enterScope ();
  Visitor::visit (node.thenStmt);
  exitScope ();
  enterScope ();
  Visitor::visit (node.elseStmt);
  exitScope ();
}

void
SymbolTableVisitor::operator() (const WhileStmtNode& node)
{
  visit (node.condition);
  enterScope ();
  visit (node.body);
  exitScope ();
}

/*********************************************************************
 * Use Nodes
 *
 * Symbols are looked-up on Variable Expression nodes and Call Expression nodes.
 * Throw exception on unfound variable
 *
 *********************************************************************/

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
  else
  {
    throw SemanticErrorException (
      std::format ("Variable \'{}\' is not declared in this scope",
                   node.identifier),
      currentLocation ());
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
  else
  {
    throw SemanticErrorException (
      std::format ("Function \'{}\' is not declared in this scope",
                   node.identifier),
      currentLocation ());
  }

  Visitor::operator() (node);
}

/*********************************************************************/
//

// Increase scope depth, add hash_map to back of scope stack
void
SymbolTableVisitor::enterScope ()
{
  scopeStack_.emplace_back ();
}

// Decrease scope depth, pop hash_map from back of scope stack
void
SymbolTableVisitor::exitScope ()
{
  scopeStack_.pop_back ();
}

// Insert <identifier, symbol> to lowest depth hash_map in scope stack
// Return false if symbol already exists in scope
bool
SymbolTableVisitor::insertSymbol (const std::string& name, Symbol sym)
{
  return scopeStack_.back ().insert ({ name, sym }).second;
}

// Lookup symbol by identifier
// Search each scope sequentially beginning with lowerst active scope
// Break early when found
// Return symbol if found, or None value if not
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
FunDeclNode inputDecl_ { .returnType = TypeSpecifier::Void };
FunDeclNode outputDecl_ { .returnType = TypeSpecifier::Void };
;