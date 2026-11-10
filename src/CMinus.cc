/******************************************************************************
 File Name:    CMinus.cc
 Description:  Compiler driver for CMinus language
 Author:       Micah Schafer
 Date:         October 16, 2026

 *****************************************************************************/

/*****************************************************************************/
// Includes
#include "CMFlags.hh"
#include "ast/PrintVisitor.hh"
#include "ast/SymbolTableVisitor.hh"
#include "exceptions/Exceptions.hh"
#include "lexing/Scanner.hh"
#include "parsing/Parser.hh"

#include <cstring>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <string>

/*****************************************************************************/
// Forward Declarations

// Parse input, setting flags on a flag object according to arguments
void
consumeArgs (CMFlags& flag,
             std::string& filename,
             std::string& input,
             unsigned argc,
             std::span<char*> argv);

// Takes a filename and returns a string containing the content of that file
std::string
fileToStr (const std::string& filename);

// Reads from stdin, returning content after EOF token is entered
std::string
readFromIn ();

std::string
tokToStr (TokenType type);

// Takes a filename and transforms is from a .cm name to a .ast name
void
replaceExt (std::string& filename);

// Parses tokens contained in a completed scanner obj
// Returns AST root node
AstNodePtr
parseTokens (Scanner scanner, CMFlags& flag);

// Attaches variable symbols to an AST
void
attachSymbols (const AstNodePtr& node);

// Prints an AST given a root node
// Outputs a readable AST to filename
void
printAstFile (const AstNodePtr& node, std::string& filename);

/*****************************************************************************/
// Main

int
main (int argc, char* argv[]) // NOLINT(bugprone-exception-escape)
{
  try
  {
    CMFlags flag;
    std::string filename;
    std::string input;
    // filename = Default.cm if no file input as arg
    // input = file contents, or reads from terminal if no file input as org
    consumeArgs (flag, filename, input, argc, std::span<char*> (argv, argc));

    Scanner scanner (input);

    AstNodePtr root = parseTokens (scanner, flag);

    attachSymbols (root);

    printAstFile (root, filename);

    std::println ("\n\nValid!");
    return 0;
  }
  catch (CompilerException& e)
  {
    std::println (std::cerr, "{}", e.what ());
    return 1;
  }
  catch (const std::exception& e)
  {
    std::println (std::cerr, "{}", e.what ());
    return 1;
  }
  catch (...)
  {
    std::println (std::cerr, "Unknown Error Thrown");
    return 1;
  }
}

/*****************************************************************************/
// Function Implementations

void
consumeArgs (CMFlags& flag,
             std::string& filename,
             std::string& input,
             unsigned argc,
             std::span<char*> argv)
{
  if (argc > 1)
  {
    for (unsigned i : std::views::iota (1u, argc))
    {
      std::string_view arg (argv[i]);
      if (arg == "-tokens")
        flag.printTokens = true;
      else if (arg == "-ast")
        flag.printAst = true;
      else if (i + 1 == argc)
      {
        // Assume filename
        flag.fileInputted = true;
        filename = arg;
        input = fileToStr (filename);
        std::println ("File inputted");
        return;
      }
      else
      {
        std::println ("Unrecognized flag");
        exit (1);
      }
    }
  }
  filename = "Default.cm";
  input = readFromIn ();
}

// Convert a file to a string for input
std::string
fileToStr (const std::string& filename)
{
  std::ifstream file (filename);

  return { (std::istreambuf_iterator<char> (file)),
           std::istreambuf_iterator<char> () };
}

// Read input from terminal
// Ctrl+D to end input stream
std::string
readFromIn ()
{
  std::string input;
  std::string line;
  while (std::getline (std::cin, line))
  {
    input += line + "\n";
  }
  return input;
}

// Convert enumerated token type to its string version
std::string
tokToStr (TokenType type)
{
  std::array<std::string, 29> toks = { // Keywords
                                       "If",
                                       "Else",
                                       "Int",
                                       "Void",
                                       "Return",
                                       "While",
                                       // Operators
                                       "Plus",
                                       "Minus",
                                       "Multiply",
                                       "Divide",
                                       "Less",
                                       "LessEqual",
                                       "Greater",
                                       "GreaterEqual",
                                       "Equal",
                                       "NotEqual",
                                       "Assign",
                                       // Delimiters/Punctuators
                                       "Semi",
                                       "Comma",
                                       "LParen",
                                       "RParen",
                                       // Bracks are [ and ]
                                       "LBrack",
                                       "RBrack",
                                       // Braces are { and }
                                       "LBrace",
                                       "RBrace",
                                       // Identifiers and Literals
                                       "Id",
                                       "Num",
                                       // Special
                                       "EndOfFile",
                                       "Error"
  };
  return toks[static_cast<int> (type)];
}

// Replace string of form <file>.cm to form <file>.ast
void
replaceExt (std::string& filename)
{
  size_t pos = filename.rfind (".cm");

  if (pos != std::string::npos)
  {
    filename.replace (pos, 3, ".ast");
  }
}

// Given an input scanner containing tokenization for program,
//  Return pointer to root of parsed AST tree
AstNodePtr
parseTokens (Scanner scanner, CMFlags& flag)
{
  Parser parser (std::move (scanner));

  auto tree = parser.parse ();
  // If token digit is set, print tokens
  if (flag.printTokens)
    parser.printTokens (std::cout);
  return tree;

  return nullptr;
}

void
attachSymbols (const AstNodePtr& node)
{
  SymbolTableVisitor visitor;
  visitor.visit (node);
}

// Given an input AST root node and filename,
//  Print AST to filename.ast
void
printAstFile (const AstNodePtr& node, std::string& filename)
{
  if (node != nullptr)
  {
    replaceExt (filename);
    std::ofstream outFile (filename);
    PrintVisitor prtvisit (outFile);
    prtvisit.visit (node);
  }
}
