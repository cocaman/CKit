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
#ifndef MU_PARSER_EXCEPTION_H
#define MU_PARSER_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <sstream>


namespace MathUtils {};
namespace mu = MathUtils;

namespace MathUtils
{

/** \brief Error codes. */
enum EErrorCodes
{
  // Formula syntax errors
  ecUNEXPECTED_OPERATOR =  0, ///< 
  ecUNASSIGNABLE_TOKEN  =  1, ///< Token cant be identified.
  ecUNEXPECTED_EOF      =  2, ///< Unexpected end of formula.
  ecUNEXPECTED_COMMA    =  3,   
  ecUNEXPECTED_ARG      =  4,
  ecUNEXPECTED_VAL      =  5, ///< An unexpected value token has been found
  ecUNEXPECTED_VAR      =  6, ///< An unexpected variable token has been found
  ecUNEXPECTED_PARENS   =  7, ///< Unexpected Parenthesis, opening or closing
  ecMISSING_PARENS      =  8, ///< Missing parens 
  ecUNEXPECTED_FUN      =  9, ///< Unexpected Parenthesis, opening or closing
  ecTOO_MANY_PARAMS     = 10, ///< Too many function parameters
  ecTOO_FEW_PARAMS      = 11, ///< Too many function parameters

  // Invalid Parser input Parameters
  ecINVALID_NAME        = 12, ///< Invalid function, variable or constant name.
  ecINVALID_FUN_PTR     = 13, ///< Invalid callback function pointer 
  ecINVALID_VAR_PTR     = 14, ///< Invalid variable pointer 

  ecNAME_CONFLICT       = 15, ///< Name conflict

  // 
  ecDOMAIN_ERROR        = 16, ///< catch division by zero, sqrt(-1), log(0)
  ecDIV_BY_ZERO         = 17, ///< Division by zero

  // misc
  ecINTERNAL_ERROR      = 18, ///< Internal error of any kind.
  ecCOUNT               = 19, ///< This is no error code, It just stores just the total number of error codes
  ecUNDEFINED           = -1, ///< Undefined message, placeholder to detect unassigned error messages
};

//---------------------------------------------------------------------------
/** \brief Error class of the parser. 

  Part of the math parser package.

  \author Ingo Berg
*/
template<typename TString>
class ParserError
{
private:

    /** \brief Type of the string characters. */
    typedef typename TString::value_type char_type;

    //------------------------------------------------------------------------------
    /** \brief Replace all ocuurences of a substring with another string. */
    void ReplaceSubString( TString &strSource, 
                           const TString &strFind,
                           const TString &strReplaceWith)
    {
      TString strResult;
      typename TString::size_type iPos(0), iNext(0);

      for(;;)
      {
        iNext = strSource.find(strFind, iPos);
        strResult.append(strSource, iPos, iNext-iPos);

        if( iNext==TString::npos )
          break;

        strResult.append(strReplaceWith);
        iPos = iNext + strFind.length();
      } 

      strSource.swap(strResult);
    }

public:

    //------------------------------------------------------------------------------
    ParserError( const TString &szMsg = "Parser:  Unspecified error.",
                 const TString &szFormula = "(formula is not available)",
                 int a_iPos = -1,
                 EErrorCodes a_iErrc = ecUNDEFINED) 
    :m_strMsg(szMsg)
    ,m_strFormula(szFormula)
    ,m_strTok()
    ,m_iPos(a_iPos)
    ,m_iErrc(a_iErrc)
    {
    }

    //------------------------------------------------------------------------------
    ParserError( const TString &szMsg, 
                 const TString &szTok,
                 const TString &szFormula,
                 int a_iPos = -1,
                 EErrorCodes a_iErrc = ecUNDEFINED) 
    :m_strMsg(szMsg)
    ,m_strFormula(szFormula)
    ,m_strTok(szTok)
    ,m_iPos(a_iPos)
    ,m_iErrc(a_iErrc)
    {
      std::ostringstream stream;
      stream << m_iPos;
      ReplaceSubString(m_strMsg, "$POS$", stream.str());
      ReplaceSubString(m_strMsg, "$TOK$", m_strTok);
    }

    //------------------------------------------------------------------------------
    virtual ~ParserError() throw()
    {};
    
    //------------------------------------------------------------------------------
    void SetFormula(const TString &a_strFormula)
    {
      m_strFormula = a_strFormula;
    }

    //------------------------------------------------------------------------------
    const TString& GetFormula() const 
    {
  	  return m_strFormula;
    }

    //------------------------------------------------------------------------------
    const TString& GetMsg() const
    {
      return m_strMsg;
    }

    //------------------------------------------------------------------------------
    /** \brief Return the formula position related to the error. 
    
      If the error is not related to a distinct position this will return -1
    */
    int GetPos() const
    {
      return m_iPos;
    }

    //------------------------------------------------------------------------------
    /** \brief Return string related with this token (if available). */
    const TString& GetToken() const
    {
      return m_strTok;
    }

    //------------------------------------------------------------------------------
    /** \brief Return the error code. */
    EErrorCodes GetCode() const
    {
      return m_iErrc;
    }

private:
    TString  m_strMsg;     ///< The message string
    TString  m_strFormula; ///< Formula string
    TString  m_strTok;     ///< Token related with the error
    int m_iPos;            ///< Formula position related with the error 
    EErrorCodes m_iErrc;   ///< Error code
};		

/** \brief Type definition needed for backwards compatibility. */
typedef ParserError<std::string> ParserException;

} // namespace MathUtils

#endif

