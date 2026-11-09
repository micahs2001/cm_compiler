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
    : std::runtime_error (std::format ("Compilation Error\n  {}", msg))
{
}
