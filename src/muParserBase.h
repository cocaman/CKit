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
#ifndef MU_PARSER_BASE_H
#define MU_PARSER_BASE_H

#include <cmath>
#include <string>
#include <iosfwd>
#include <stdexcept>
#include <map>

#include "muParserDef.h"
#include "muParserGenerics.h"
#include "muParserException.h"
#include "CKVariant.h"

namespace MathUtils {};
namespace mu = MathUtils;

namespace MathUtils
{

/** \brief Mathematical expressions parser.

  Version 1.10

  This is the implementation of a bytecode based mathematical expressions parser.
  The formula will be parsed from string and converted into a bytecode.
  Future calculations will be done with the bytecode instead the formula string
  resulting in a significant performance increase.
  Complementary to a set of internally implemented functions the parser is able to handle
  user defined functions and variables.

  \author (C) 2004 Ingo Berg
*/
class ParserBase
{
	private:
		friend class ParserDebug;

		/** \brief Parser function prototype.
		
		   Encapsulates a Parser function prototype.
		*/
		class FunProt
		{
			friend class ParserBase;
		
			public:
				FunProt(void *a_pFun, int a_iArgc, bool a_bAllowOpti) :
					pFun(a_pFun),
					iArgc(a_iArgc),
					bAllowOpti(a_bAllowOpti)
				{}
		
				/** \brief Default constructor. */
				FunProt() :
					pFun(0),
					iArgc(0),
					bAllowOpti(0)
				{}
		
				/** \brief Copy constructor. */
				FunProt(const FunProt &a_Fun)
				{
					pFun = a_Fun.pFun;
					iArgc = a_Fun.iArgc;
					bAllowOpti = a_Fun.bAllowOpti;
				}
		
				/** \brief Get number of Arguments.
				
				  If this value is negative this indicates a function with variable argument count.
				  Internally abs(iArgc) is the actual number of arguments given to a specific call
				  of this function during string/bytecode parsing.
				*/
				int GetArgc() const
				{
					return iArgc;
				}
		
			private:
				void *pFun;       ///< Callback pointer
				int   iArgc;      ///< Argument count
				bool  bAllowOpti; ///< Flag indication optimizeability
		};

	public:
		/** \brief Type of exceptions thrown by the parser. */
		typedef ParserException exception_type;
		
		/** \brief Base datatype of the parser. */
		// typedef MU_PARSER_BASETYPE value_type;
		typedef CKVariant value_type;
		
		/** \brief String type of the parser. */
		typedef MU_PARSER_STRINGTYPE  string_type;
		
		/** \brief Type of the string characters. */
		typedef string_type::value_type char_type;
		
		/** \brief User defined function with one parameter. */
		typedef value_type (*fun_type1)(value_type &);
		
		/** \brief User defined function with two parameters. */
		typedef value_type (*fun_type2)(value_type &, value_type &);
		
		/** \brief User defined function with three parameters. */
		typedef value_type (*fun_type3)(value_type &, value_type &, value_type &);
		
		/** \brief User defined function with four parameters. */
		typedef value_type (*fun_type4)(value_type &, value_type &, value_type &, value_type &);
		
		/** \brief User defined function with five parameters. */
		typedef value_type (*fun_type5)(value_type &, value_type &, value_type &, value_type &, value_type &);
		
		/** \brief User defined function arbitrary number of parameters */
		typedef value_type (*multfun_type)(const std::vector<value_type>&);
		
		/** \brief Storage for user variables and their pointers */
		typedef std::map<string_type, value_type*> varmap_type;
		
		/** \brief Storage for user defined constants. */
		typedef std::map<string_type, value_type> valmap_type;
		
		/** \brief Storage type for names of user defined functions and their pointers. */
		typedef std::map<string_type, FunProt > funmap_type;

	private:
		/** \brief Storage type for names of user defined functions and their pointers. */
		typedef std::map<string_type, fun_type1 > optmap_type;
		
		/** \brief Storage type for names of user defined functions and their pointers. */
		typedef std::map<string_type, fun_type1 > infix_oprt_type;
		
		/** \brief Pointer to 'value_type foo()' type of memberfunction. */
		typedef value_type (ParserBase::*ParseFunction)() const;
		
		/** \brief Token type for internal use only. */
		typedef ParserToken<value_type, MU_PARSER_STRINGTYPE> token_type;
		
		/** \brief Type of the underlying bytecode. */
		typedef ParserByteCode<value_type> bytecode_type;

		/** \brief Syntax codes.
		
		  The syntax codes control the syntax check done during the first time parsing of the
		  expression string. They are flags that indicate which tokens are allowed next if certain
		  tokens are identified.
		*/
		enum ESynCodes {
		  noBO      = 1 << 0, ///< to avoid i.e. "cos(7)("
		  noBC      = 1 << 1, ///< to avoid i.e. "sin)" or "()"
		  noVAL     = 1 << 2, ///< to avoid i.e. "tan 2" or "sin(8)3.14"
		  noVAR     = 1 << 3, ///< to avoid i.e. "sin a" or "sin(8)a"
		  noCOMMA   = 1 << 4, ///< to avoid i.e. ",," or "+," ...
		  noFUN     = 1 << 5, ///< to avoid i.e. "sqrt cos" or "(1)sin"
		  noOPT     = 1 << 6, ///< to avoid i.e. "(+)"
		  noPOSTOP  = 1 << 7, ///< to avoid i.e. "(5!!)" "sin!"
		  noINFIXOP = 1 << 8, ///< to avoid i.e. "++4" "!!4"
		  noEND     = 1 << 9  ///< to avoid unexpected end of formula
		};

		/** \brief Operator strings. */
		static char_type *c_DefaultOprt[];

	public:
		ParserBase();
		ParserBase( const ParserBase &a_Parser );
		ParserBase& operator=(const ParserBase &a_Parser);
		virtual ~ParserBase();
		
		void Init();

		/** \brief Calcule the result.
		
		  \pre a formula must be set.
		  \pre variables must have been set (if needed)
		
		  \sa #m_pParseFormula
		  \throw ParseException if no Formula is set or in case of any other error.
		
		  A note on const correctness:
		  I considere it important that Calc is a const function.
		  Due to caching operations Calc changes only the state of internal variables with one exception
		  m_UsedVar this is reset during string parsing and accessible from the outside. Instead of making
		  Calc non const GetUsedVar is non const because it explicitely calls Calc() forcing this update.
		*/
		inline value_type Calc() const
		{
			return (this->*m_pParseFormula)();
		}

		void SetFormula(const string_type &a_strFormula);
		void SetVar(const varmap_type &a_vVar);
		void SetConst(const valmap_type &a_vVar);
		void EnableOptimizer(bool a_bIsOn=true);
		void EnableByteCode(bool a_bIsOn=true);

		// Add a single function, comnstant, variable or operator
		void AddFun(const string_type &a_strFun, fun_type1, bool a_bAllowOpt = true);
		void AddFun(const string_type &a_strFun, fun_type2, bool a_bAllowOpt = true);
		void AddFun(const string_type &a_strFun, fun_type3, bool a_bAllowOpt = true);
		void AddFun(const string_type &a_strFun, fun_type4, bool a_bAllowOpt = true);
		void AddFun(const string_type &a_strFun, fun_type5, bool a_bAllowOpt = true);
		void AddFun(const string_type &a_strFun, multfun_type, bool a_bAllowOpt = true);
		void AddConst(const string_type &a_strConst, value_type a_fVal);
		void AddVar(const string_type &a_strConst, value_type *a_fVar);
		void AddPostfixOp(const string_type &a_strFun, fun_type1);
		void AddPrefixOp(const string_type &a_strName, fun_type1 a_pOprt);
		
		// Clear user defined variables, constants or functions
		void ClearVar();
		void ClearFun();
		void ClearConst();
		void ClearPrefixOp();
		void ClearPostfixOp();
		
		void RemoveVar(const string_type &a_strVarName);
		const varmap_type& GetUsedVar() const;
		const varmap_type& GetVar() const;
		const string_type& GetFormula() const;
		const funmap_type& GetFunDef() const;
		const valmap_type& GetConst() const;
		
		virtual const char_type* ValidNameChars() const = 0;
		virtual const char_type* ValidOprtChars() const = 0;
		virtual const char_type* ValidPrefixOprtChars() const = 0;

	protected:
		virtual void InitFun() {};
		virtual void InitConst() {};
		virtual void InitOprt() {};
		
		void  Error(EErrorCodes a_iErrc, const string_type &a_strTok) const;
		void  Error(EErrorCodes a_iErrc, int a_iPos=-1, const string_type &a_strTok = "") const;
		void  CheckName(const string_type &a_strName, const string_type &a_CharSet) const;

	private:
		void Assign(const ParserBase &a_Parser);
		void InitErrMsg();
		
		int  ExtractToken(const char_type *a_szCharSet, string_type &a_strTok, int a_iPos) const;
		bool IsValTok(int &a_iPos, token_type &a_Tok) const;
		bool IsVarTok(int &a_iPos, token_type &a_Tok) const;
		bool IsUndefVarTok(int &a_iPos, token_type &a_Tok) const;
		bool IsFunTok(int &a_iPos, token_type &a_Tok) const;
		bool IsPostOpTok(int &a_iPos, token_type &a_Tok) const;
		bool IsInfixOpTok(int &a_iPos, token_type &a_Tok) const;

		token_type ApplyOprt(const token_type &a_Val1,
							 const token_type &a_OptTok,
							 const token_type &a_Val2) const;
		
		void ApplyInfixOp(ParserStack<token_type> &a_stOpt,
						  ParserStack<token_type> &a_stVal) const;
		
		void ApplyFunction(int iArgCount,
						   ParserStack<token_type> &a_stOpt,
						   ParserStack<token_type> &a_stVal) const;
		
		token_type ApplyUnaryOprt(const token_type &a_FunTok,
								  const token_type &a_ValTok) const;
		
		token_type ReadToken(int &index, int &SyntaxFlags) const;
		int GetOprtPri(const token_type &a_Tok) const;
		
		value_type ParseString() const;
		value_type ParseCmdCode() const;
		value_type ParseValue() const;
		
		void  ClearFormula();

#if defined(MU_PARSER_DUMP_STACK) | defined(MU_PARSER_DUMP_CMDCODE)
		void StackDump(const ParserStack<token_type > &a_stVal,
					   const ParserStack<token_type > &a_stOprt);
#endif

		/** \brief Pointer to the parser function.
		
		  Calc() calls the function whose address is stored there.
		*/
		mutable ParseFunction m_pParseFormula;
		mutable int	m_iPos;	               ///< Global index within bytecode.
		mutable const int *m_pCmdCode;     ///< Formula converted to bytecode, points to the data of the bytecode class.
		mutable bytecode_type m_vByteCode; ///< The Bytecode class.
		mutable varmap_type m_UsedVar;     ///< Map holding the variable found in the current expression.
		
		string_type  m_strFormula;  ///< The original formula.
		funmap_type  m_FunDef;      ///< Map of function names and pointers.
		valmap_type  m_ConstDef;    ///< user constants.
		varmap_type  m_VarDef;      ///< user defind variables.
		optmap_type  m_PostOprtDef; ///< unary postfix operators.
		std::vector<string_type>  m_ErrMsg;  ///< Storage for error messages
		infix_oprt_type  m_InfixOprtDef; ///< unary operator (infix notation).
		bool m_bOptimize;     ///< Flag that indicates if the optimizer is on or off.
		bool m_bUseByteCode;  ///< Flag that indicates if bytecode parsing is on or off.

		/** \brief Flag that contronls behaviour if undefined variables have been found.
		
		  If true, the parser does not throw an exception if an undefined variable is found.
		  otherwise it does. This variable is used internally only!
		  It supresses a "undefined variable" exception in GetUsedVar().
		  Those function should return a complete list of variables including
		  those the are not defined by the time of it's call.
		*/
		mutable bool m_bSkipErrors;
		
		value_type m_fZero;  ///< Dummy value of zero, referenced by undefined variables
};

} // namespace MathUtils

#endif
