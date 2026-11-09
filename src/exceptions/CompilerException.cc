/******************************************************************************
 File Name:    CompilerException.cc
 Description:  General Compiler Exception class definition
 for CMinus language

 Author:       Micah Schafer
 Date:         November 9, 2026

 *****************************************************************************/

#include <format>
#include <stdexcept>
#include <string>

#include "CompilerException.hh"

CompilerException::CompilerException (const std::string& msg)
    : std::runtime_error (msg)
{
}

CompilerException::CompilerException (const std::string& msg,
                                      SourceLocation location)
    : std::runtime_error (std::format ("{} at line: {}, column {}",
                                       msg,
                                       location.line,
                                       location.column))
{
}

CompilerException::CompilerException (const std::string& msg, const Token& tok)
    : CompilerException (msg, tok.location)
{
}
