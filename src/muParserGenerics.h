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
#ifndef MU_PARSER_TOKEN_H
#define MU_PARSER_TOKEN_H

#include <cassert>
#include <string>
#include <stack>
#include <vector>

#include "muParserException.h"

/*
 * Because we're using the NAN value in some places in this object,
 * we need to make sure that it's defined for all the platforms that
 * will be using this object.
 */
#ifdef __linux__
#define __USE_ISOC99 1
#endif
#include <math.h>
/*
 * Oddly enough, Sun doesn't seem to have NAN defined, so we need to
 * do that here so that things run more smoothly. This is very interesting
 * because Sun has isnan() defined, but no obvious way to set a value.
 */
#ifdef __sun__
#ifndef NAN
#define	NAN	(__extension__ ((union { unsigned __l __attribute__((__mode__(__SI__))); \
			float __d; }) { __l: 0x7fc00000UL }).__d)
#endif
#endif
/*
 * This is most odd, but it seems that at least on Darwin (Mac OS X)
 * there's a problem with the definition of isnan(). So... to make it
 * easier on all parties, I'm simply going to repeat the definition
 * that's in Linux and Darwin here, and it should get picked up even
 * if the headers fail us.
 */
#ifdef __MACH__
#ifndef isnan
#define	isnan(x)	((sizeof(x) == sizeof(double)) ? __isnand(x) : \
					(sizeof(x) == sizeof(float)) ? __isnanf(x) : __isnan(x))
#endif
#endif

namespace MathUtils
{

//---------------------------------------------------------------------------
//
//
//   Parser Stack implementation
//
//
//---------------------------------------------------------------------------

/** \brief Parser stack implementation.

 Stack implementation based on a std::stack. The behaviour of pop() had been
 slightly changed in order to get an error code if the stack is empty.
 The stack is used within the Parser both as a value stack and as an operator stack.

 \author (C) 2004 Ingo Berg
*/
template <typename TValueType>
class ParserStack : private std::stack<TValueType, std::vector<TValueType> >
{
#ifdef PARSER_DEBUG
	friend class Parser;
#endif

	private:
		/** \brief Type of the underlying stack implementation. */
		typedef std::stack<TValueType, std::vector<TValueType> > parent_type;

	public:
		//---------------------------------------------------------------------------
		/** \brief Pop a value from the stack.

		  Unlike the standard implementation this function will return the
		  value that is going to be taken from the stack.

		  \throw ParserException in case the stack is empty.
		  \sa pop(int &a_iErrc)
		*/
		TValueType pop()
		{
			if (empty())
				throw ParserException("stack is empty.");

			TValueType el = top();
			parent_type::pop();
			return el;
		}

		//---------------------------------------------------------------------------
		/** \brief Pop a value from the stack.

		  If no value is present an error code will be set in this case
		  a default constructed element will be returned.

		  \param a_iErrc Error code; (0:operation succeeded  1:stack is empty)
		  \throw nothrow
		*/
		TValueType pop(int &a_iErrc)
		{
			a_iErrc = 0;

			if (empty()) {
				a_iErrc = 1;
				return TValueType();
			}

			TValueType el(top());
			pop();

			return el;
		}

		void push(const TValueType& a_Val) { parent_type::push(a_Val); }
		unsigned size() { return (unsigned)parent_type::size(); }
		bool empty()    { return parent_type::size()==0; }
		TValueType& top() { return parent_type::top(); }
};


//---------------------------------------------------------------------------
//
//
//   Parser Token implementation
//
//
//---------------------------------------------------------------------------

/** \brief Encapsulation of the data for a single formula token.

  Formula token implementation. Part of the Math Parser Package.

  <pre>
  Formula tokens can be either one of the following:
    - value
    - variable
    - function
    - operator
	- postfix operator
  </pre>

 \author (C) 2004 Ingo Berg
*/
template<typename TBase, typename TString>
class ParserToken
{
	public:
		/** \brief Bytecode values.

		  \attention The order of the operator entries must match the order in legalOprt!
		*/
		enum ECmdCode {
			cmLE     =  0, ///< Operator item:  less or equal
			cmGE     =  1, ///< Operator item:  greater or equal
			cmNEQ    =  2, ///< Operator item:  not equal
			cmEQ     =  3, ///< Operator item:  equals
			cmLT     =  4, ///< Operator item:  less than
			cmGT     =  5, ///< Operator item:  greater than
			cmADD    =  6, ///< Operator item:  add
			cmSUB    =  7, ///< Operator item:  subtract
			cmMUL    =  8, ///< Operator item:  multiply
			cmDIV    =  9, ///< Operator item:  division
			cmPOW    = 10, ///< Operator item:  y to the power of ...
			cmAND    = 11, ///< Operator item:  logical and
			cmOR     = 12, ///< Operator item:  logical or
			cmBO     = 13, ///< Operator item:  opening bracket
			cmBC     = 14, ///< Operator item:  closing bracket
			cmCOMMA  = 15, ///< Operator item:  comma
			cmVAR,         ///< variable item
			cmVAL,         ///< value item
			cmFUNC,        ///< function item
			cmPOSTOP,	     ///< post value unary operator
			cmINFIXOP,	   ///< post value unary operator
			cmEND,         ///< end of formula
			cmUNKNOWN,     ///< uninitialized item
		};

		/** \brief Token flags. */
		enum ETokFlags {
			flVOLATILE = 1, ///< Mark a token that depends on a variable or a function that is not conservative
		};

	private:
		ECmdCode m_iType;  ///< Type of the token
		TBase  m_fVal;     ///< Stores Token value; not applicable for all tokens
		void  *m_pTok;     ///< Stores Token pointer; not applicable for all tokens
		int  m_iArgCount;  ///< Valid only for function tokens; Number of function arguments
		int  m_iFlags;
		int  m_iDep;       ///< Token depends on a variable token and is connected with operator of this priority
		bool m_bAllowOpti; ///< True if the token is optimizeable
		TString m_strTok;  ///< Token string

	public:
		//---------------------------------------------------------------------------
		ParserToken() :
			m_iType(cmUNKNOWN),
			m_fVal(0),
			m_pTok(0),
			m_iArgCount(0),
			m_iFlags(0),
			m_iDep(999),
			m_strTok()
		{
		}

		//------------------------------------------------------------------------------
		/** \brief Create token from another one.

		  Implemented by calling Assign(...)
		*/
		ParserToken(const ParserToken &a_Tok) :
			m_iType(cmUNKNOWN),
			m_fVal(0),
			m_pTok(0),
			m_iArgCount(0),
			m_iFlags(0),
			m_iDep(999),
			m_strTok()
		{
			Assign(a_Tok);
		}

		//------------------------------------------------------------------------------
		/** \brief Make token a value token and set its value. */
		ParserToken(TBase a_fVal) :
			m_iType(cmVAL),
			m_fVal(a_fVal),
			m_pTok(0),
			m_iArgCount(0),
			m_iFlags(0),
			m_iDep(999),
			m_strTok()
		{
		}

		//------------------------------------------------------------------------------
		/** \brief Assignement operator.

		  Copy token state from another token and return this.
		  Implemented by calling Assign(...)
		*/
		ParserToken& operator=(const ParserToken &a_Tok)
		{
			Assign(a_Tok);
			return *this;
		}

		//------------------------------------------------------------------------------
		/** \brief Copy token information from argument. */
		void Assign(const ParserToken &a_Tok)
		{
			m_iType = a_Tok.m_iType;
			m_pTok = a_Tok.m_pTok;
			m_fVal = a_Tok.m_fVal;
			m_iArgCount = a_Tok.m_iArgCount;
			m_iFlags = a_Tok.m_iFlags;
			m_iDep = a_Tok.m_iDep;
			m_strTok = a_Tok.m_strTok;
		}

		//------------------------------------------------------------------------------
		void AddFlags(int a_iFlags)
		{
			m_iFlags |= a_iFlags;
		}

		//------------------------------------------------------------------------------
		/** \brief Check if a certain flag ist set. */
		bool IsFlagSet(int a_iFlags) const
		{
			return (bool)(m_iFlags & a_iFlags);
		}

		//------------------------------------------------------------------------------
		/** \brief currently unused. */
		void SetDep(int a_iOpPri)
		{
			m_iDep = a_iOpPri;
		}

		//------------------------------------------------------------------------------
		/** \brief currently unused. */
		int GetDep() const
		{
			return m_iDep;
		}

		//------------------------------------------------------------------------------
		/** \brief Return token string. */
		const TString & GetTok()
		{
			return m_strTok;
		}

		//------------------------------------------------------------------------------
		/** \brief Assign a token type.

		  Token may not be of type value, variable or function. Those have seperate set functions.

		  \pre [assert] a_iType!=cmVAR
		  \pre [assert] a_iType!=cmVAL
		  \pre [assert] a_iType!=cmFUNC
		  \post m_fVal = 0
		  \post m_pTok = 0
		*/
		ParserToken& Set(ECmdCode a_iType, const TString &a_strTok="")
		{
			// The following types cant be set this way, they have special Set functions
			assert(a_iType!=cmVAR);
			assert(a_iType!=cmVAL);
			assert(a_iType!=cmFUNC);

			m_iType = a_iType;
			m_fVal = 0;
			m_pTok = 0;
			m_iFlags = 0;
			m_strTok = a_strTok;

			return *this;
		}

		//------------------------------------------------------------------------------
		/** \brief Make this token a value token.

		  Member variables not necessary for value tokens will be invalidated.
		*/
		ParserToken& SetVal(TBase a_fVal, const TString &a_strTok="")
		{
			m_iType = cmVAL;
			m_fVal = a_fVal;
			m_pTok = 0;
			m_iFlags = 0;
			m_strTok = a_strTok;

			return *this;
		}

		//------------------------------------------------------------------------------
		/** \brief make this token a variable token.

		  Member variables not necessary for variable tokens will be invalidated.
		*/
		ParserToken& SetVar(TBase *a_pVar, const TString &a_strTok)
		{
			m_iType = cmVAR;
			m_fVal = 0;
			m_pTok = (void*)a_pVar;
			m_iFlags = 0;
			m_strTok = a_strTok;

			AddFlags(ParserToken::flVOLATILE);
			return *this;
		}

		//------------------------------------------------------------------------------
		ParserToken& SetFun( void *a_pFun, int a_iArgc,
							 const TString &a_strTok,
							 bool a_bAllowOpti = true)
		{
			m_pTok = a_pFun;
			m_iArgCount = a_iArgc;
			m_iType = cmFUNC;
			m_fVal = 0;
			m_iFlags = 0;
			m_strTok = a_strTok;

			if (a_bAllowOpti==false)
				AddFlags(flVOLATILE);

			return *this;
		}

		//------------------------------------------------------------------------------
		/** \brief Make token a unary post value operator. */
		ParserToken& SetPostOp(void *a_pPostOp, const TString &a_strTok)
		{
			m_pTok = a_pPostOp;
			m_iArgCount = 0;
			m_iType = cmPOSTOP;
			m_fVal = 0;
			m_iFlags = 0;
			m_strTok = a_strTok;

			return *this;
		}

		//------------------------------------------------------------------------------
		/** \brief Make token a unary post value operator. */
		ParserToken& SetInfixOp(void *a_pFun, const TString &a_strTok)
		{
			m_pTok = a_pFun;
			m_iArgCount = 0;
			m_iType = cmINFIXOP;
			m_fVal = 0;
			m_iFlags = 0;
			m_strTok = a_strTok;

			return *this;
		}

		//------------------------------------------------------------------------------
		/** \brief Return the token type. */
		ECmdCode GetType() const
		{
			return m_iType;
		}

		//------------------------------------------------------------------------------
		/** \brief Return the function address for function and operator tokens.
		  \throw ParserException if token is no function or operator token
		*/
		void* GetFuncAddr() const
		{
			if (!(m_iType==cmFUNC || m_iType==cmPOSTOP || m_iType==cmINFIXOP))
				throw ParserException("internal error:  GetFuncAddr() called non function/postfix operator token.");

			return m_pTok;
		}

		//------------------------------------------------------------------------------
		/** \biref Get value of the token.

		  Only applicable to variable and value tokens.
		  \throw ParserException if token is no value/variable token.
		*/
		TBase GetVal() const
		{
			switch (m_iType) {
				case cmVAL:  return m_fVal;
				case cmVAR:  return *((TBase*)m_pTok);
				default:
					throw ParserException("internal error:  GetVal() called for non value token.");
			}
		}

		//------------------------------------------------------------------------------
		/** \brief Get address of a variable token.

		  Valid only if m_iType==CmdVar.
		  \throw ParserException if token is no variable token.
		*/
		TBase* GetVar() const
		{
			if (m_iType!=cmVAR)
				throw ParserException("internal error:  Token::GetArgCount() called for non variable token.");

			return (TBase*)m_pTok;
		}

		//------------------------------------------------------------------------------
		/** \brief Return the number of function arguments.

		  Valid only if m_iType==CmdFUNC.
		*/
		int GetArgCount() const
		{
			if (m_iType!=cmFUNC)
				throw ParserException("internal error:  Token::GetArgCount() called for non function token.");

			return m_iArgCount;
		}

		//------------------------------------------------------------------------------
		/** \brief Get token as string. */
		const TString& GetAsString() const
		{
			return m_strTok;
		};
};


//---------------------------------------------------------------------------
//
//
//   Parser Bytecode implementation
//
//
//---------------------------------------------------------------------------

/** \brief Bytecode implementation of the Math Parser.

  The bytecode contains the formula converted to revers polish notation stored in a continious
  memory area. Associated with this data are operator codes, variable pointers, constant
  values and function pointers. Those are necessary in order to calculate the result.
  All those data items will be casted to the underlying datatype of the bytecode.

  \author (C) 2004 Ingo Berg
*/
template<typename TBaseData, typename TMapType = int>
class ParserByteCode
{
	private:
		/** \brief Token type for internal use only. */
		typedef ParserToken<TBaseData, MU_PARSER_STRINGTYPE> token_type;

		/** \brief Core type of the bytecode. */
		typedef std::vector<TMapType> storage_type;

		/** \brief Position in the Calculation array. */
		unsigned m_iStackPos;

		/** \brief Core type of the bytecode. */
		storage_type m_vBase;

		/** \brief Size of a value entry in the bytecode, relative to TMapType size. */
		const int mc_iSizeVal;

		/** \brief Size of a pointer, relative to size of underlying TMapType.

			\attention The size is related to the size of TMapType not bytes!
		*/
		const int mc_iSizePtr;

		/** \brief A value entry requires that much entires in the bytecode.

			Value entry consists of:
			<ul>
			  <li>One entry for Stack index</li>
			  <li>One entry for Token identifier</li>
			  <li>mc_iSizeVal entries for the value</li>
			<ul>

			\sa AddVal(TBaseData a_fVal)
		*/
		const int mc_iSizeValEntry;

		/** \brief Store an address in bytecode.
			\param a_pAddr Address to be stored.
			\throw nothrow
		*/
		void StorePtr(void *a_pAddr)
		{
			for (int i=0; i<mc_iSizePtr; ++i)
				m_vBase.push_back( (TMapType)( (TMapType*)a_pAddr + i ) );
		}

	public:
		/** \brief Bytecode default constructor.

		  \pre [assert] sizeof(TBaseData)>=sizeof(TMapType)
		  \pre [assert] sizeof(TBaseData*)>=sizeof(TMapType)
		*/
		ParserByteCode() :
			m_iStackPos(0),
			m_vBase(),
			mc_iSizeVal( sizeof(TBaseData)/sizeof(TMapType) ),
			mc_iSizePtr( sizeof(TBaseData*) / sizeof(TMapType) ),
			mc_iSizeValEntry( 2 + mc_iSizeVal)
		{
			m_vBase.reserve(1000);
			assert( sizeof(TBaseData)>=sizeof(TMapType) );
			assert( sizeof(TBaseData*)>=sizeof(TMapType) );
		}

		/** \brief Destructor (trivial).*/
		~ParserByteCode()
		{
		};

		/** \brief Copy constructor.

		  Implemented in Terms of Assign(const ParserByteCode &a_ByteCode)
		*/
		ParserByteCode(const ParserByteCode &a_ByteCode)
		{
			Assign(a_ByteCode);
		}

		/** \brief Assignement operator.

		  Implemented in Terms of Assign(const ParserByteCode &a_ByteCode)
		*/
		ParserByteCode& operator=(const ParserByteCode &a_ByteCode)
		{
			Assign(a_ByteCode);
			return *this;
		}

		/** \brief Copy state of another object to this.

		  \throw nowthrow
		*/
		void Assign(const ParserByteCode &a_ByteCode)
		{
			if (this==&a_ByteCode)
				return;

			m_iStackPos = a_ByteCode.m_iStackPos;
			m_vBase = a_ByteCode.m_vBase;
		}

		/** \brief Add a Variable pointer to bytecode.
			\param a_pVar Pointer to be added.
			\throw nothrow
		*/
		void AddVar(TBaseData *a_pVar)
		{
			m_vBase.push_back( ++m_iStackPos );
			m_vBase.push_back( token_type::cmVAR );

			StorePtr(a_pVar);
		}

		/** \brief Add a Variable pointer to bytecode.

			Value entries in byte code consist of:
			<ul>
			  <li>value array position of the value</li>
			  <li>the operator code according to ParserToken::cmVAL</li>
			  <li>the value stored in #mc_iSizeVal number of bytecode entries.</li>
			</ul>

			\param a_pVal Value to be added.
			\throw nothrow
		*/
		void AddVal(TBaseData a_fVal)
		{
			m_vBase.push_back( ++m_iStackPos );
			m_vBase.push_back( token_type::cmVAL );

			for (int i=0; i<mc_iSizeVal; ++i) {
				m_vBase.push_back( *(reinterpret_cast<TMapType*>(&a_fVal) + i) );
			}
		}

		/** \brief Add an operator identifier to bytecode.

		  Operator entries in byte code consist of:
		  <ul>
			<li>value array position of the result</li>
			<li>the operator code according to ParserToken::ECmdCode</li>
		  </ul>

		  \sa  ParserToken::ECmdCode
		*/
		void AddOp(typename token_type::ECmdCode a_Oprt)
		{
			m_vBase.push_back(--m_iStackPos);
			m_vBase.push_back(a_Oprt);
		}

		/** \brief Add postfix operator to bytecode.

		  Postfix operator entries in byte code consist of:
		  <ul>
			<li>value array position of the result</li>
			<li>the postfix operator code token_type::cmPOSTOP
			<li>its callback function pointer.</li>
		  </ul>

		  \param a_pFun Pointer to postfix operator callback function.
		  \throw nothrow
		*/
		void AddPostOp(void *a_pFun)
		{
			m_vBase.push_back(m_iStackPos);
			m_vBase.push_back(token_type::cmPOSTOP);

			StorePtr(a_pFun);
		}

		/** \brief Add function to bytecode.
			\param a_iArgc Number of arguments, negative numbers indicate multiarg functions.
			\param a_pFun Pointer to function callback.
		*/
		void AddFun(void *a_pFun, int a_iArgc)
		{
			if (a_iArgc>=0) {
				m_iStackPos = m_iStackPos - a_iArgc + 1;
			} else {
				m_iStackPos = m_iStackPos + a_iArgc + 1;
			}

			m_vBase.push_back(m_iStackPos);
			m_vBase.push_back(token_type::cmFUNC);
			m_vBase.push_back(a_iArgc);

			StorePtr(a_pFun);
		}

		/** \brief Add end marker to bytecode.

			\throw nothrow
		*/
		void Finalize()
		{
			m_vBase.push_back(token_type::cmEND);
			m_vBase.push_back(token_type::cmEND);
			m_vBase.push_back(token_type::cmEND);

			// shrink bytecode vector to fit
			storage_type(m_vBase).swap(m_vBase);
		}

		/** \brief Get Pointer to bytecode data storage. */
		const TMapType* GetRawData() const
		{
			assert(m_vBase.size());
			return &m_vBase[0];
		}

		/** \brief Delete the bytecode. */
		void clear()
		{
			m_vBase.clear();
			m_iStackPos = 0;
		}

		/** \brief Return size of a value entry.

		  That many bytecode entries are necessary to store a value.

		  \sa mc_iSizeVal
		*/
		inline unsigned GetValSize() const
		{
			return mc_iSizeVal;
		}

		/** \brief Return size of a pointer entry.

		  That many bytecode entries are necessary to store a pointer.

		  \sa mc_iSizePtr
		*/
		inline unsigned GetPtrSize() const
		{
			return mc_iSizePtr;
		}

		/** \brief Remove a value number of entries from the bytecode.

			\attention Currently I dont test if the entries are really value entries.
		*/
		void RemoveValEntries(unsigned a_iNumber)
		{
			unsigned iSize = a_iNumber * mc_iSizeValEntry;
			assert( (m_vBase.size()-iSize) >= 0);
			m_vBase.resize(m_vBase.size()-iSize);

			m_iStackPos -= (a_iNumber);
			assert(m_iStackPos>=0);
		}

		/** \brief Dump bytecode (for debugging only!). */
		void AsciiDump()
		{
			if (!m_vBase.size()) {
				std::cout << "No bytecode available\n";
				return;
			}

			std::cout << "Entries:" << (int)m_vBase.size()
				<< " (ValSize:" << mc_iSizeVal
				<<  ", PtrSize:" << mc_iSizePtr << ")\n";

			int i = 0;

			while ( m_vBase[i] != token_type::cmEND ) {
				std::cout << "IDX[" << m_vBase[i++] << "]\t";
				switch (m_vBase[i]) {
					case token_type::cmVAL:
						std::cout << "VAL "; ++i;
						std::cout << "[" << *( reinterpret_cast<double*>(&m_vBase[i]) ) << "]\n";
						i += mc_iSizeVal;
						break;
					case token_type::cmVAR:
						std::cout << "VAR "; ++i;
						std::cout << "[ADDR: 0x" << hex << m_vBase[i] << "]\n"; ++i;
						break;
					case token_type::cmFUNC:
						std::cout << "CALL\t"; ++i;
						std::cout << "[Arg:" << dec << m_vBase[i] << "]"; ++i;
						std::cout << "[ADDR: 0x" << hex << m_vBase[i] << "]\n"; ++i;
						break;
					case token_type::cmPOSTOP:
						std::cout << "POSTOP\t"; ++i;
						std::cout << "[ADDR: 0x" << m_vBase[i] << "]\n"; ++i;
						break;
					case token_type::cmINFIXOP:
						std::cout << "INFIXOP\t"; ++i;
						std::cout << "[ADDR: 0x" << m_vBase[i] << "]\n"; ++i;
						break;
					case token_type::cmADD: std::cout << "ADD\n"; ++i; break;
					case token_type::cmAND: std::cout << "AND\n"; ++i; break;
					case token_type::cmOR:  std::cout << "OR\n";  ++i; break;
					case token_type::cmSUB: std::cout << "SUB\n"; ++i; break;
					case token_type::cmMUL: std::cout << "MUL\n"; ++i; break;
					case token_type::cmDIV: std::cout << "DIV\n"; ++i; break;
					case token_type::cmPOW: std::cout << "POW\n"; ++i; break;
					default: std::cout << "(unknown code: " << m_vBase[i] << ")\n"; ++i;	break;
				}
			}

			std::cout << "END" << endl;
		}
};

} // namespace MathUtils

#endif
