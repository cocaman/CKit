/*
  Copyright (C) 2004 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSERDEF_H
#define MU_PARSERDEF_H


/** \brief Define the base datatype for values. 

  This datatype must be a built in value type. You can not use custom classes.
  It has been tested with float, double and long double types, int should 
  work as well.
*/
//#define MU_PARSER_BASETYPE double

//------------------------------------------------------------------------------
//
// do not change anything beyond this point...
//
// !!! This section is devoted to macros that are used for debugging
// !!! or for features that are not fully implemented yet.
//
//#define MU_PARSER_DUMP_STACK
//#define MU_PARSER_DUMP_CMDCODE
//#define MU_PARSER_WIDESTRING   // <- for testing, does not work
#if !defined(MU_PARSER_WIDESTRING)
  #define MU_PARSER_STRINGTYPE std::string
#else
  #define MU_PARSER_STRINGTYPE std::wstring
#endif

#endif

