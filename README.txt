Team Members:  Micah Schafer

> make 
     Builds program

> make clean 
     Removes generated files

> make test TEST_DIR=<path-to-test-dir>
     Runs program on all files in directory <path-to-test-dir>

> make lint 
     




###########################################################################
# Non-lab-related changes
###########################################################################

- Adjusted scanner to output correct locations by capturing location at
correct time.

- Reformatted exception hierarchy to be more consistent.

- Rolled line/column numbers into a SourceLocation struct that's easily passable
     between layers to be able to print location in errors for any layer.

###########################################################################
# Implementation Issues and Solutions
###########################################################################

- Visitor Duplication:
     I realized that any individual visitor would need to duplicate transition logic for every node as-is.
     To address this, I created a virtual parent Visitor class. Any visitor (Print, SymbolTable, etc.) will
     implement the Visitor class, and override any node operators as necessary, but default to its parent's 
     standard transition logic for any other nodes.


- Const AstNode Operators:
     I had been passing AstNodes in the visitor operator overrides as const values. This was causing issues
     when attempting to attach the declaration symbols to use nodes in the symbolTable visitor.
     The first solution I attempted was to use mutable keywords. These, I discovered, can be used in the 
     struct member of a const object to allow changing of that value.
     After much refactoring, I could not resolve the issue, since I kept having to cascade the const property
     while passing through to other functions. This eventually reached a point at which I wasn't able to modify
     the node due to type incompatibility.
     I was very close to succeeding, though my VarExprNode SymbolTable operator override became:

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
     }

     However, I was unable to make the same format work with CallExprNode, because though I could have a VarSymbol
     that could be declared a mutable, but express a const node, I could not combine mutable and const with a concrete
     type such as FunDeclNode*
     
     Eventually, I decided to attempt to solve this by mirroring the VarSymbol use with FunSymbol:
     using FunDeclSymbol = const FunDeclNode*;
     This allowed combining the mutable and const keywords




###########################################################################
# Linting Report
###########################################################################

     I starting using clang-tidy's linting to clean up my code.
     The following are issues it brought up and the solutions to resolve 
     the error/warning

     -Enums switched to uint8_t when int is larger than necessary 
          > Ast.hh, Token.hh

     -Greedy usage of template constructor taking forward reference (T&&)
          Matches non-const objects
          Use concepts i.e. "requires (!std::derived_from<std::decay_t<T>, NodeVariant>)"
          > Ast.hh

     -Using replace_back over push_back when adding new default object
          > SymbolTableVisitor.cc

     - Add [[nodiscard]] flag to various functions to ensure that
          the return result does not get discarded when used.
          > InvalidSyntaxException.hh, Visitor.hh

     - Replaced C-style structures with modern approaches


###########################################################################
