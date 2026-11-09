/******************************************************************************
 File Name:    CMinus.cc
 Description:  Compiler driver for CMinus language
 Author:       Micah Schafer
 Date:         October 16, 2026

 *****************************************************************************/

/*****************************************************************************/
// Includes
#include "ast/PrintVisitor.hh"
#include "ast/SymbolTableVisitor.hh"
#include "exceptions/Exceptions.hh"
#include "lexing/Scanner.hh"
#include "parsing/Parser.hh"

#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <string>

/*****************************************************************************/
// Forward Declarations

unsigned
consumeArgs (std::string& filename, std::string& input, int argc, char* argv[]);

std::string
fileToStr (const std::string& filename);

std::string
readFromIn ();

std::string
tokToStr (TokenType type);

void
replaceExt (std::string& filename);

AstNodePtr
parseTokens (Scanner scanner, unsigned flagDigit);

void
attachSymbols (const AstNodePtr& node);

void
printAstFile (const AstNodePtr& node, std::string& filename);

/*****************************************************************************/
// Main

int
main (int argc, char* argv[])
{
  // Select input method based on if there was a file inputted
  std::string filename;
  std::string input;
  unsigned flagDigit = consumeArgs (filename, input, argc, argv);

  // Input is string of file or input stream from terminal
  Scanner scanner (input);

  AstNodePtr root = parseTokens (scanner, flagDigit);

  // Tree is printed to file by default right now.
  // Later will be producible with flag
  attachSymbols (root);

  printAstFile (root, filename);
}

/*****************************************************************************/
// Function Implementations

// Consumes arguments
// Valid flags set flagDigit
unsigned
consumeArgs (std::string& filename, std::string& input, int argc, char* argv[])
{
  unsigned flagDigit = 0;
  if (argc > 1)
  {
    for (int i : std::views::iota (1u, (unsigned) argc))
    {
      std::string_view arg (argv[i]);
      if (arg == "-tokens")
        // Set second digit -> X1X
        flagDigit |= 2;
      else if (arg == "-ast")
        // Set third digit -> 1XX
        flagDigit |= 4;
      else if (i + 1 == argc)
      {
        // Assume filename, set first digit -> X1X
        flagDigit |= 1;
        filename = arg;
        input = fileToStr (filename);
        std::println ("File inputted");
        return flagDigit;
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
  return flagDigit;
}

// Convert a file to a string for input
std::string
fileToStr (const std::string& filename)
{
  std::ifstream file (filename);

  return std::string ((std::istreambuf_iterator<char> (file)),
                      std::istreambuf_iterator<char> ());
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
  std::string toks[29] = { // Keywords
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
parseTokens (Scanner scanner, unsigned flagDigit)
{
  Parser parser (std::move (scanner));
  try
  {
    auto tree = parser.parse ();
    // If token digit is set, print tokens
    if (flagDigit & 2)
      parser.printTokens (std::cout);
    return tree;
  }
  catch (InvalidSyntaxException& e)
  {
    std::println ("{}", e.what ());
  }
  return nullptr;
}

void
attachSymbols (const AstNodePtr& node)
{
  try
  {
    SymbolTableVisitor visitor;
    visitor.visit (node);
  }
  catch (SemanticErrorException& e)
  {
    std::println ("{}", e.what ());
  }
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
    try
    {
      PrintVisitor prtvisit (outFile);
      prtvisit.visit (node);
    }
    catch (SemanticErrorException& e)
    {
      std::println ("{}", e.what ());
    }
  }
}
