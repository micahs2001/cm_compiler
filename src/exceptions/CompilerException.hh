/******************************************************************************
 File Name:    CompilerException.cc
 Description:  General Compiler Exception class definition
 for CMinus language

 Author:       Micah Schafer
 Date:         November 9, 2026

 *****************************************************************************/

#pragma once

#include <stdexcept>
#include <string>

class CompilerException : public std::runtime_error
{
public:
  explicit CompilerException (const std::string& msg);
};