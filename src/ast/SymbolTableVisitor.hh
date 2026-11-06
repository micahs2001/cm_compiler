#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Ast.hh"
#include "Visitor.hh"

// Symbols stored in the table (Variables, Parameters, Functions).
using Symbol =
  std::variant<const VarDeclNode*, const ParamDeclNode*, const FunDeclNode*>;

class SymbolTableVisitor : public Visitor
{
public:
  explicit SymbolTableVisitor ();

  void
  operator() (const VarDeclNode& node) override;

  void
  operator() (const FunDeclNode& node) override;

  void
  operator() (const ParamDeclNode& node) override;

  void
  operator() (const IfStmtNode& node) override;

  void
  operator() (const WhileStmtNode& node) override;

  void
  operator() (const VarExprNode& node) override;

  void
  operator() (const CallExprNode& node) override;

private:
  void
  enterScope ();

  void
  exitScope ();

  bool
  insertSymbol (const std::string& name, Symbol sym);

  const std::optional<Symbol>
  lookupSymbol (const std::string& name) const;

private:
  std::vector<std::unordered_map<std::string, Symbol>> scopeStack_;

  // Built-in I/O function decls for input() and output().
  FunDeclNode inputDecl_;
  FunDeclNode outputDecl_;
};