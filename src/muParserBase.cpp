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
#include "muParser.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#ifdef GPP2
#include <limits.h>
#else
#include <limits>
#endif
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace std;


namespace MathUtils
{

#if !defined(MU_PARSER_WIDESTRING)
ParserBase::char_type* ParserBase::c_DefaultOprt[] = { "<=", ">=", "!=", "==", "<",
                                                       ">", "+", "-", "*", "/", "^",
                                                       "&&", "||", "(", ")", ",", 0 };
#else
ParserBase::char_type* ParserBase::c_DefaultOprt[] = { L"<=", L">=", L"!=", L"==", L"<",
                                                       L">", L"+", L"-", L"*", L"/", L"^",
                                                       L"&&", L"||", L"(", L")", L",", 0 };
#endif

//------------------------------------------------------------------------------
/** \brief Constructor.
  \param a_szFormula the formula to interpret.
  \throw ParserException if a_szFormula is null.
*/
ParserBase::ParserBase() :
	m_pParseFormula(&ParserBase::ParseString),
	m_iPos(0),
	m_pCmdCode(0),
	m_vByteCode(),
	m_UsedVar(),
	m_strFormula(),
	m_FunDef(),
	m_ConstDef(),
	m_VarDef(),
	m_PostOprtDef(),
	m_ErrMsg(),
	m_InfixOprtDef(),
	m_bOptimize(true),
	m_bUseByteCode(true),
	m_bSkipErrors(false),
	m_fZero((double)0.0)
{
	InitErrMsg();
}

//---------------------------------------------------------------------------
/** \brief Copy constructor.

 Implemented by calling Assign(a_Parser)
*/
ParserBase::ParserBase(const ParserBase &a_Parser) :
	m_pParseFormula(&ParserBase::ParseString),
	m_iPos(0),
	m_pCmdCode(0),
	m_vByteCode(),
	m_UsedVar(),
	m_strFormula(),
	m_FunDef(),
	m_ConstDef(),
	m_VarDef(),
	m_PostOprtDef(),
	m_ErrMsg(),
	m_InfixOprtDef(),
	m_bOptimize(true),
	m_bUseByteCode(true),
	m_bSkipErrors(false),
	m_fZero((double)0.0)
{
	Assign(a_Parser);
}

//---------------------------------------------------------------------------
/** \brief Assignement operator.

 Implemented by calling Assign(a_Parser)
*/
ParserBase& ParserBase::operator=(const ParserBase &a_Parser)
{
	Assign(a_Parser);
	return *this;
}

//---------------------------------------------------------------------------
/** \brief Check if a name contains invalid characters.

  \throw ParserException if the name contains invalid charakters.
*/
void ParserBase::CheckName(const string_type &a_strName,
                           const string_type &a_szCharSet) const
{
	if (!a_strName.length() ||
		(a_strName.find_first_not_of(a_szCharSet)!=string_type::npos) ||
		(a_strName[0]>='0' && a_strName[0]<='9')) {
		Error(ecINVALID_NAME);
	}
}

//---------------------------------------------------------------------------
/** \brief Copy state of a parser object to this.

  Clears Variables and Functions of this parser.
  Copies the states of all internal variables.
  Resets parse function to string parse mode.

  \param a_Parser the source object.
*/
void ParserBase::Assign(const ParserBase &a_Parser)
{
	if (&a_Parser==this)
		return;

	// Don't copy bytecode instead cause the parser to create new bytecode
	// by resetting the parse function.
	m_pParseFormula = &ParserBase::ParseString;
	m_vByteCode.clear();

	m_iPos = a_Parser.m_iPos;
	m_strFormula = a_Parser.m_strFormula;     // Copy the formula
	m_FunDef = a_Parser.m_FunDef;             // Copy function definitions
	m_ConstDef = a_Parser.m_ConstDef;         // Copy user define constants
	m_VarDef = a_Parser.m_VarDef;             // Copy user defined variables
	m_UsedVar = a_Parser.m_UsedVar;           // Copy used map holding the used variables
	m_PostOprtDef = a_Parser.m_PostOprtDef;   // post value unary operators
	m_InfixOprtDef = a_Parser.m_InfixOprtDef; // unary operators for infix notation
	m_bOptimize  = a_Parser.m_bOptimize;
	m_bUseByteCode = a_Parser.m_bUseByteCode;
	m_bSkipErrors = a_Parser.m_bSkipErrors;
	m_ErrMsg = a_Parser.m_ErrMsg;
}

//---------------------------------------------------------------------------
/** \brief Initialize the Error Messages. */
void ParserBase::InitErrMsg()
{
	m_ErrMsg.resize(ecCOUNT);

	m_ErrMsg[ecUNASSIGNABLE_TOKEN]  = "Undefined token \"$TOK$\" found at position $POS$.";
	m_ErrMsg[ecINTERNAL_ERROR]      = "Internal error";
	m_ErrMsg[ecINVALID_NAME]        = "Invalid function-, variable- or constant name.";
	m_ErrMsg[ecINVALID_FUN_PTR]     = "Invalid pointer to callback function.";
	m_ErrMsg[ecINVALID_VAR_PTR]     = "Invalid pointer to variable.";
	m_ErrMsg[ecUNEXPECTED_OPERATOR] = "Unexpected operator \"$TOK$\" found at position $POS$";
	m_ErrMsg[ecUNEXPECTED_EOF]      = "Unexpected end of formula at position $POS$";
	m_ErrMsg[ecUNEXPECTED_COMMA]    = "Unexpected comma at position $POS$";
	m_ErrMsg[ecUNEXPECTED_PARENS]   = "Unexpected parenthesis \"$TOK$\" at position $POS$";
	m_ErrMsg[ecUNEXPECTED_FUN]      = "Unexpected function \"$TOK$\" at position $POS$";
	m_ErrMsg[ecUNEXPECTED_VAL]      = "Unexpected value \"$TOK$\" found at position $POS$";
	m_ErrMsg[ecUNEXPECTED_VAR]      = "Unexpected variable \"$TOK$\" found at position $POS$";
	m_ErrMsg[ecUNEXPECTED_ARG]      = "Function arguments used without a function (position: $POS$)";
	m_ErrMsg[ecMISSING_PARENS]      = "Missing parenthesis";
	m_ErrMsg[ecTOO_MANY_PARAMS]     = "Too many parameters for function \"$TOK$\" at formula position $POS$";
	m_ErrMsg[ecTOO_FEW_PARAMS]      = "Too few parameters for function \"$TOK$\" at formula position $POS$";
	m_ErrMsg[ecDIV_BY_ZERO]         = "Divide by zero";
	m_ErrMsg[ecDOMAIN_ERROR]        = "Domain error";
	m_ErrMsg[ecNAME_CONFLICT]       = "Name conflict";

#if defined(_DEBUG)
	for (int i=0; i<ecCOUNT; ++i) {
		if (!m_ErrMsg[i].length())
			Error(ecINTERNAL_ERROR, -1, "Error messages incomplete");
	}
#endif
}

//---------------------------------------------------------------------------
/** \brief Destructor. (trivial) */
ParserBase::~ParserBase()
{
}

//---------------------------------------------------------------------------
/** \brief Initialize user defined functions.

  Calls the virtual functions InitFun(), InitConst() and InitOprt().
*/
void ParserBase::Init()
{
	InitFun();
	InitConst();
	InitOprt();
}

//---------------------------------------------------------------------------
/** \brief Set Parser Variables.

  Input will be provided as a map containing the names and pointers of the variables.

  \throw ParserException in case of an out of memory condition
*/
void ParserBase::SetVar(const varmap_type &a_vVar)
{
	m_VarDef = a_vVar;
	m_pParseFormula = &ParserBase::ParseString; // switch to string parse mode
}

//---------------------------------------------------------------------------
/** \brief Set user defined constants. */
void ParserBase::SetConst(const valmap_type &a_vConst)
{
	m_ConstDef = a_vConst;
	m_pParseFormula = &ParserBase::ParseString; // switch to string parse mode
}

//---------------------------------------------------------------------------
/** \brief Set the formula.
    Triggers first time calculation thus the creation of the bytecode and
    scanning of used variables.

    \param a_strFormula Formula as string_type
    \throw ParserException in case of syntax errors.
*/
void ParserBase::SetFormula(const string_type &a_strFormula)
{
	m_strFormula = a_strFormula;
	m_pParseFormula = &ParserBase::ParseString; // switch to string parse mode
}

//---------------------------------------------------------------------------
/** \brief Add a user defined unary function.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddFun(const string_type &a_strFun, fun_type1 a_pFun, bool a_bAllowOpt)
{
	if (a_pFun==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strFun, ValidNameChars());
	m_FunDef[a_strFun] = FunProt((void*)a_pFun, 1, a_bAllowOpt);
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined function binary function.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddFun(const string_type &a_strFun, fun_type2 a_pFun, bool a_bAllowOpt)
{
	if (a_pFun==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strFun, ValidNameChars());
	m_FunDef[a_strFun] = FunProt((void*)a_pFun, 2, a_bAllowOpt);
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined function binary function.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddFun(const string_type &a_strFun, fun_type3 a_pFun, bool a_bAllowOpt)
{
	if (a_pFun==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strFun, ValidNameChars());
	m_FunDef[a_strFun] = FunProt((void*)a_pFun, 3, a_bAllowOpt);
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined function binary function.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddFun(const string_type &a_strFun, fun_type4 a_pFun, bool a_bAllowOpt)
{
	if (a_pFun==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strFun, ValidNameChars());
	m_FunDef[a_strFun] = FunProt((void*)a_pFun, 4, a_bAllowOpt);
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined function binary function.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddFun(const string_type &a_strFun, fun_type5 a_pFun, bool a_bAllowOpt)
{
	if (a_pFun==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strFun, ValidNameChars());
	m_FunDef[a_strFun] = FunProt((void*)a_pFun, 5, a_bAllowOpt);
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
void ParserBase::AddFun(const string_type &a_strFun, multfun_type a_pFun, bool a_bAllowOpt)
{
	if (a_pFun==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strFun, ValidNameChars());
	m_FunDef[a_strFun] = FunProt((void*)a_pFun, -1, a_bAllowOpt);
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined variable.
    \post Will reset the Parser to string parsing mode.
    \pre [assert] a_fVar!=0
    \throw ParserException in case the name contains invalid signs.
*/
void ParserBase::AddVar(const string_type &a_strVar, value_type *a_pVar)
{
	if (a_pVar==0)
		Error(ecINVALID_VAR_PTR);

	// Test if a constant with that names already exists
	if (m_ConstDef.find(a_strVar)!=m_ConstDef.end())
		Error(ecNAME_CONFLICT);

	if (m_FunDef.find(a_strVar)!=m_FunDef.end())
		Error(ecNAME_CONFLICT);

	CheckName(a_strVar, ValidNameChars());
	m_VarDef[a_strVar] = a_pVar;
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined operator.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddPostfixOp(const string_type &a_strName, fun_type1 a_pOprt)
{
	if (a_pOprt==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strName, ValidOprtChars());
	m_PostOprtDef[a_strName] = a_pOprt;
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined operator.
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::AddPrefixOp(const string_type &a_strName, fun_type1 a_pOprt)
{
	if (a_pOprt==0)
		Error(ecINVALID_FUN_PTR);

	CheckName(a_strName, ValidOprtChars());
	m_InfixOprtDef[a_strName] = a_pOprt;
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Add a user defined constant.
    \post Will reset the Parser to string parsing mode.
    \throw ParserException in case the name contains invalid signs.
*/
void ParserBase::AddConst(const string_type &a_strName, value_type a_fVal)
{
	CheckName(a_strName, ValidNameChars());
	m_ConstDef[a_strName] = a_fVal;
	m_pParseFormula = &ParserBase::ParseString;
}

//---------------------------------------------------------------------------
/** \brief Read a Token from String.
  \param a_iPos [in/out] Index from where to start reading, will be advanced after reading the token.
  \param a_iSynFlags [in/out] Flags that control which tokens are allowed her, and which will be allowed in the next step.
  \return token that has been read
*/
ParserBase::token_type ParserBase::ReadToken(int &a_iPos, int &a_iSynFlags) const
{
	std::stack<int> FunArgs;
	const char_type* szFormula = m_strFormula.c_str();

	while (szFormula[a_iPos] == ' ') {
		++a_iPos;
	}

	token_type tok;

	// check for EOF
	if ( (szFormula[a_iPos] == '\0') || (szFormula[a_iPos] == '\n') ) {
		if ( a_iSynFlags & noEND )
			Error(ecUNEXPECTED_EOF, m_iPos);

		a_iSynFlags = 0;
		tok.Set(token_type::cmEND);
		return tok;
	}

	// Compare token with function and operator strings
	// check string for operator/function
	for (int i=0; c_DefaultOprt[i]; i++) {
		int len = (int)strlen( c_DefaultOprt[i] );

		if (!strncmp(&szFormula[a_iPos], c_DefaultOprt[i], len)) {
			switch(i) {
				case token_type::cmAND:
				case token_type::cmOR:
				case token_type::cmLT:
				case token_type::cmGT:
				case token_type::cmLE:
				case token_type::cmGE:
				case token_type::cmNEQ:
				case token_type::cmEQ:
				case token_type::cmADD:
				case token_type::cmSUB:
				case token_type::cmMUL:
				case token_type::cmDIV:
				case token_type::cmPOW:
					if (a_iSynFlags & noOPT) {
						// Maybe its an infix operator not an operator
						// Both operator types can share characters in
						// their identifiers
						if (IsInfixOpTok(a_iPos, tok)) {
							if (a_iSynFlags & noINFIXOP)
								Error(ecUNEXPECTED_OPERATOR, m_iPos, tok.GetAsString());

							a_iSynFlags = noPOSTOP | noINFIXOP | noOPT | noBC;
							return tok;
						}

						Error(ecUNEXPECTED_OPERATOR, m_iPos, c_DefaultOprt[i]);
					}

					a_iSynFlags  = noBC | noOPT | noCOMMA | noPOSTOP;
					a_iSynFlags |= ((i != token_type::cmEND) && (i != token_type::cmBC)) ? noEND : 0;
					break;
				case token_type::cmCOMMA:
					if (a_iSynFlags & noCOMMA)
						Error(ecUNEXPECTED_COMMA, m_iPos, c_DefaultOprt[i]);

					a_iSynFlags  = noBC | noOPT | noEND | noCOMMA | noPOSTOP;
					break;
				case token_type::cmBO:
					if (a_iSynFlags & noBO)
						Error(ecUNEXPECTED_PARENS, m_iPos, c_DefaultOprt[i]);

					a_iSynFlags = noBC | noOPT | noEND | noCOMMA | noPOSTOP;
					break;
				case token_type::cmBC:
					if (a_iSynFlags & noBC)
						Error(ecUNEXPECTED_PARENS, m_iPos, c_DefaultOprt[i]);

					a_iSynFlags  = noBO | noVAR | noVAL | noFUN | noINFIXOP;
					break;
				default:
					// The operator is listed in c_DefaultOprt, but not here. This is a bad thing...
					Error(ecINTERNAL_ERROR);
			} // switch operator id

			a_iPos += len;
			return tok.Set( (token_type::ECmdCode)i, c_DefaultOprt[i] );
		} // if operator string found
	} // end of for all operator strings

	// Read the next token from string
	if ( IsFunTok(a_iPos, tok) ) {
		if (a_iSynFlags & noFUN)
			Error(ecUNEXPECTED_FUN, m_iPos-(int)tok.GetAsString().length(), tok.GetAsString());

		a_iSynFlags = noCOMMA | noBC | noFUN | noVAR | noVAL | noOPT | noPOSTOP | noEND;
		return tok;
	}

	// Check String for values
	if ( IsValTok(a_iPos, tok) ) {
		if (a_iSynFlags & noVAL)
			Error(ecUNEXPECTED_VAL, m_iPos-(int)tok.GetAsString().length(), tok.GetAsString());

		a_iSynFlags = noVAL | noVAR | noFUN | noBO | noINFIXOP;
		return tok;
	}

	// Check String for variable
	if ( IsVarTok(a_iPos, tok) ) {
		if (a_iSynFlags & noVAR)
			Error(ecUNEXPECTED_VAR, m_iPos-(int)tok.GetAsString().length(), tok.GetAsString());

		a_iSynFlags = noVAL | noVAR | noFUN | noBO | noPOSTOP | noINFIXOP;
		return tok;
	}

	// Check for unary operators
	if ( IsInfixOpTok(a_iPos, tok) ) {
		if (a_iSynFlags & noINFIXOP)
			Error(ecUNEXPECTED_OPERATOR, m_iPos-(int)tok.GetAsString().length(), tok.GetAsString());

		a_iSynFlags = noPOSTOP | noINFIXOP | noOPT | noBC;
		return tok;
	}

	// Check for unary operators
	if ( IsPostOpTok(a_iPos, tok) ) {
		if (a_iSynFlags & noPOSTOP)
			Error(ecUNEXPECTED_OPERATOR, m_iPos-(int)tok.GetAsString().length(), tok.GetAsString());

		a_iSynFlags = noVAL | noVAR | noFUN | noBO | noPOSTOP;
		return tok;
	}

	// Check String for undefined variable token. Done only if a
	// flag is set indicating to ignore undefined variables.
	// This is a way to conditionally avoid an error if
	// undefined variables occur.
	// The GetUsedVar function must supress the error for
	// undefined variables in order to collect all variable
	// names including the undefined ones.
	if ( m_bSkipErrors && IsUndefVarTok(a_iPos, tok) ) {
		if (a_iSynFlags & noVAR)
			Error(ecUNEXPECTED_VAR, m_iPos-(int)tok.GetAsString().length(), tok.GetAsString());

		a_iSynFlags = noVAL | noVAR | noFUN | noBO | noPOSTOP | noINFIXOP;
		return tok;
	}

	// Check for unknown token
	//
	// !!! From this point on there is no exit without an exception possible...
	//
	string_type strTok;
	int iEnd = ExtractToken(ValidNameChars(), strTok, m_iPos);
	if (iEnd!=a_iPos)
		Error(ecUNASSIGNABLE_TOKEN, m_iPos, strTok);

	Error(ecUNASSIGNABLE_TOKEN, m_iPos, m_strFormula.substr(m_iPos));
	return token_type(); // never reached
}

//---------------------------------------------------------------------------
/** \brief Extract all chars that belong to a certain charset.

  \param a_strTok [out]  The string that consists entirely of characters listed in a_szCharSet.
  \param a_iPos [in] The token search will start at this position.
  \param a_szCharSet [in] Const char array of the characters allowed in the token.
  \return The Position of the first character not listed in a_szCharSet.
*/
int ParserBase::ExtractToken(const char_type *a_szCharSet,
                             string_type &a_strTok,
                             int a_iPos) const
{
	int iEnd = (int)m_strFormula.find_first_not_of(a_szCharSet, a_iPos);
	if (iEnd==a_iPos)
		return iEnd;

	if (iEnd==(int)string_type::npos)
		iEnd = (int)m_strFormula.length();  // Everything else could be a postfix op.

	a_strTok = string_type( m_strFormula.begin()+a_iPos, m_strFormula.begin()+iEnd);
	a_iPos = iEnd;

	return iEnd;
}

//---------------------------------------------------------------------------
/** \brief Check whether the token at a given position is a function token.
  \param a_Tok [out] If a value token is found it will be placed here.
  \param a_iPos [in/out] Position within the formula, that should be checked for a value item.
  \return true if a function token has been found.
*/
bool ParserBase::IsFunTok(int &a_iPos, token_type &a_Tok) const
{
	string_type strTok;
	int iEnd = ExtractToken(ValidNameChars(), strTok, a_iPos);
	if (iEnd==a_iPos)
		return false;

	funmap_type::const_iterator item = m_FunDef.find(strTok);
	if (item!=m_FunDef.end()) {
		// item->second = function prototype consisting of a pair made up of a pointer and argument count
		a_Tok.SetFun(item->second.pFun,        // The function pointer
					 item->second.iArgc,       // the function argument count, determines implicitely the pointer type of item->pFun
					 strTok,                   // the function string
					 item->second.bAllowOpti); // flag indication optimizeability
		a_iPos = iEnd;
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------
/** \brief Check if a string position contains a unary post value operator. */
bool ParserBase::IsPostOpTok(int &a_iPos, token_type &a_Tok) const
{
	// Tricky problem with equations like "3m+5":
	//     m is a postfix operator, + is a valid sign for postfix operators and
	//     for binary operators parser detects "m+" as operator string and
	//     finds no matching postfix operator.
	//
	// This is a special case so this routine slightly differs from the other
	// token readers.

	// Test if there could be a postfix operator
	string_type strTok;
	int iEnd = ExtractToken(ValidOprtChars(), strTok, a_iPos);
	if (iEnd==a_iPos)
		return false;

	// iteraterate over all postfix operator strings
	optmap_type::const_iterator item = m_PostOprtDef.begin();
	for (item=m_PostOprtDef.begin(); item!= m_PostOprtDef.end(); ++item) {
		if (strTok.find(item->first)==0) {
			a_Tok.SetPostOp((void*)item->second, strTok);
			a_iPos += (int)item->first.length();
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
/** \brief Check if a string position contains a unary infix operator. */
bool ParserBase::IsInfixOpTok(int &a_iPos, token_type &a_Tok) const
{
	string_type strTok;
	int iEnd = ExtractToken(ValidPrefixOprtChars(), strTok, a_iPos);
	if (iEnd==a_iPos)
		return false;

	optmap_type::const_iterator item = m_InfixOprtDef.find(strTok);
	if (item!=m_InfixOprtDef.end()) {
		a_Tok.SetInfixOp((void*)item->second, strTok);
		a_iPos = iEnd;
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/** \brief Check whether the token at a given position is a value token.

  Value tokens are either values or constants.

  \param a_Tok [out] If a value token is found it will be placed here.
  \param a_iPos [in/out] Position within the formula, that should be checked for a value item.
  \return true if a value token has been found.
*/
bool ParserBase::IsValTok(int &a_iPos, token_type &a_Tok) const
{
	string_type	strTok;
	value_type	fVal(NAN);

	// 1.) Check for string value
	const char	*formula = m_strFormula.c_str();
	if (formula[a_iPos] == '"') {
		// find the next '"' that's not escaped by a '\'
		int		size = m_strFormula.length();
		int		end = a_iPos + 1;
		while ((formula[end] != '\0') && (formula[end] != '\n') && (end < size)) {
			if (formula[end] == '\\') {
				end++;
			} else if (formula[end] == '"') {
				// at the end of the string, update things
				if (end > (a_iPos + 1)) {
					// pick out the substring between the double quotes
					CKString	val(formula, (a_iPos+1), (end-a_iPos-1));
					// move the parser past the last double quote
					a_iPos = end + 1;
					// convert the string into a variant
					fVal = val;
					// add it as a value to the parser
					a_Tok.SetVal(fVal, val.stl_str());
				} else {
					// make an empty string as that's all we'll have
					CKString	val;
					// move past the end
					a_iPos = end + 1;
					// convert it to a variant and save it
					fVal = val;
					a_Tok.SetVal(fVal, val.stl_str());
				}
				return true;
			}
			// check the next character in the string
			end++;
		}
	}

	// 2.) Check for numeric value
	char		*remainder = NULL;
	fVal = strtod(&formula[a_iPos], &remainder);
	int	cnt = ((int) (remainder - &formula[a_iPos]))/sizeof(char);
	if (cnt > 0) {
		a_iPos += cnt;

		std::ostringstream buf;
		buf << fVal;

		a_Tok.SetVal(fVal, buf.str());
		return true;
	}

	// 3.) Check for user defined constant
	// Read everything that could be a constant name
	int iEnd = ExtractToken(ValidNameChars(), strTok, a_iPos);
	if (iEnd==a_iPos)
		return false;

	valmap_type::const_iterator item = m_ConstDef.find(strTok);
	if (item!=m_ConstDef.end()) {
		a_iPos = iEnd;
		a_Tok.SetVal(item->second, strTok);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/** \brief Check wheter a token at a given position is a variable token.
    \param a_iPos [in/out] Position within the formula, that should be checked for a variable item.
    \param a_Tok [out] If a variable token has been found it will be placed here.
	\return true if a variable token has been found.
*/
bool ParserBase::IsVarTok(int &a_iPos, token_type &a_Tok) const
{
	if (!m_VarDef.size())
		return false;

	string_type strTok;
	int iEnd = ExtractToken(ValidNameChars(), strTok, a_iPos);
	if (iEnd==a_iPos)
		return false;

	varmap_type::const_iterator item =  m_VarDef.find(strTok);
	if (item!=m_VarDef.end()) {
		a_iPos = iEnd;
		a_Tok.SetVar(item->second, strTok);
		m_UsedVar[item->first] = item->second;  // Add variable to used-var-list
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/** \brief Check wheter a token at a given position is an undefined variable.
    \param a_iPos [in/out] Position within the formula, that should be checked for a variable item.
    \param a_Tok [out] If a variable token has been found it will be placed here.
	\return true if a variable token has been found.
*/
bool ParserBase::IsUndefVarTok(int &a_iPos, token_type &a_Tok) const
{
	string_type strTok;
	int iEnd = ExtractToken(ValidNameChars(), strTok, a_iPos);
	if (iEnd==a_iPos)
		return false;

	a_iPos = iEnd;
	a_Tok.SetVar((value_type*)&m_fZero, strTok);
	m_UsedVar[strTok] = 0;  // Add variable to used-var-list
	return true;
}

//---------------------------------------------------------------------------
/** \brief Get operator priority.

 \throw ParserException if a_Oprt is no operator code
*/
int ParserBase::GetOprtPri(const token_type &a_Tok) const
{
	switch (a_Tok.GetType()) {
		case token_type::cmEND:   return -2;
		case token_type::cmCOMMA: return -1;
		case token_type::cmBO :
		case token_type::cmBC :   return 0;
		case token_type::cmAND:
		case token_type::cmOR:    return 1;
		case token_type::cmLT :
		case token_type::cmGT :
		case token_type::cmLE :
		case token_type::cmGE :
		case token_type::cmNEQ:
		case token_type::cmEQ :   return 2;
		case token_type::cmADD:
		case token_type::cmSUB:   return 3;
		case token_type::cmMUL:
		case token_type::cmDIV:   return 4;
		case token_type::cmPOW:   return 5;
		default:  Error(ecINTERNAL_ERROR);
			return 999;
	}
}

//---------------------------------------------------------------------------
/** \brief Return a map containing the used variables only. */
const ParserBase::varmap_type& ParserBase::GetUsedVar() const
{
	try {
		m_bSkipErrors = true;
		ParseString(); // implicitely create or update m_UsedVar if not already done
		m_bSkipErrors = false;
	} catch(...) {
		m_bSkipErrors = false;
		throw;
	}

	// Do not change to bytecode mode, after all undefined variables may have been found
	m_pParseFormula = &ParserBase::ParseString;

	return m_UsedVar;
}

//---------------------------------------------------------------------------
/** \brief Return a map containing the used variables only. */
const ParserBase::varmap_type& ParserBase::GetVar() const
{
	return m_VarDef;
}

//---------------------------------------------------------------------------
/** \brief Return prototypes of all parser functions. */
const ParserBase::funmap_type& ParserBase::GetFunDef() const
{
	return m_FunDef;
}

//---------------------------------------------------------------------------
/** \brief Return map containing all constant definitions. */
const ParserBase::valmap_type& ParserBase::GetConst() const
{
	return m_ConstDef;
}

//---------------------------------------------------------------------------
/** \brief Retrieve the formula. */
const ParserBase::string_type& ParserBase::GetFormula() const
{
	return m_strFormula;
}

//---------------------------------------------------------------------------
/** \brief Apply an operator to two values.

  \param opt code of the operator to apply.
  \param x first value
  \param y second value
  \return x opt y (whatever opt means)
  \attention opt must be an operator code!
  \throw ParserException if opt is no operator token or either a_Val1 or a_Val2 is no value tokens.
*/
ParserBase::token_type ParserBase::ApplyOprt( const token_type &a_Val1,
											  const token_type &a_OptTok,
											  const token_type &a_Val2 ) const
{
	value_type x = a_Val1.GetVal();
	value_type y = a_Val2.GetVal();
	token_type tok;

	switch (a_OptTok.GetType()) {
		case token_type::cmAND: tok.SetVal( (x != m_fZero) && (y != m_fZero) ); break;
		case token_type::cmOR:  tok.SetVal( (x != m_fZero) || (y != m_fZero) ); break;
		case token_type::cmLT:  tok.SetVal( x < y ); break;
		case token_type::cmGT:  tok.SetVal( x > y ); break;
		case token_type::cmLE:  tok.SetVal( x <= y ); break;
		case token_type::cmGE:  tok.SetVal( x >= y ); break;
		case token_type::cmNEQ: tok.SetVal( x != y ); break;
		case token_type::cmEQ:  tok.SetVal( x == y ); break;
		case token_type::cmADD: tok.SetVal( x + y ); break;
		case token_type::cmSUB: tok.SetVal( x - y ); break;
		case token_type::cmMUL: tok.SetVal( x * y ); break;
		case token_type::cmDIV: tok.SetVal( x / y ); break;

		case token_type::cmPOW: tok.SetVal(pow(x.getDoubleValue(), y.getDoubleValue())); break;
		default:  Error(ecINTERNAL_ERROR);
	}

	if (!m_bOptimize) {
		// Optimization flag is not set
		m_vByteCode.AddOp(a_OptTok.GetType());
	} else if ( a_Val1.IsFlagSet(token_type::flVOLATILE) ||
				a_Val2.IsFlagSet(token_type::flVOLATILE) ) {
		// Optimization flag is not set, but one of the value
		// depends on a variable
		m_vByteCode.AddOp(a_OptTok.GetType());
		tok.AddFlags(token_type::flVOLATILE);
		tok.SetDep( GetOprtPri(a_OptTok) );
	} else {
		// operator call can be optimized; If optimization is possible
		// the two previous tokens must be value tokens / they will be removed
		// and replaced with the result of the pending operation.
		m_vByteCode.RemoveValEntries(2);
		m_vByteCode.AddVal(tok.GetVal());
	}

	return tok;
}

//---------------------------------------------------------------------------
void ParserBase::ApplyInfixOp( ParserStack<token_type> &stOpt,
                               ParserStack<token_type> &stVal) const
{
	if (stOpt.empty() || stOpt.top().GetType()!=token_type::cmINFIXOP)
		return;

	if (stVal.empty())
		Error(ecINTERNAL_ERROR);

	token_type optTok = stOpt.pop();
	token_type valTok = stVal.pop();
	token_type resTok;

	fun_type1 pFunc = (fun_type1)optTok.GetFuncAddr();
	assert(pFunc);

	Parser::value_type	arg = valTok.GetVal();
	resTok.SetVal(pFunc(arg));

	// optimization if the result does not depend on a variable (direct or indirect)
	if (m_bOptimize && ! valTok.IsFlagSet(token_type::flVOLATILE)) {
		m_vByteCode.RemoveValEntries(1);
		m_vByteCode.AddVal( resTok.GetVal() );
	} else {
		m_vByteCode.AddPostOp( (void*)pFunc );
		resTok.AddFlags( token_type::flVOLATILE );
	}

	stVal.push(resTok);
}

//---------------------------------------------------------------------------
void ParserBase::ApplyFunction( int iArgCount,
                                ParserStack<token_type> &stOpt,
                                ParserStack<token_type> &stVal) const
{
	if (stOpt.empty() || stOpt.top().GetType()!=token_type::cmFUNC)
		return;

	token_type funTok = stOpt.pop();
	token_type valTok;

	if ( funTok.GetArgCount()!=-1 && iArgCount>funTok.GetArgCount())
		Error(ecTOO_MANY_PARAMS, m_iPos-1, funTok.GetAsString());

	if ( iArgCount<funTok.GetArgCount() )
		Error(ecTOO_FEW_PARAMS, m_iPos-1, funTok.GetAsString());

	void *pFunc = funTok.GetFuncAddr();
	assert(pFunc);

	// Collect the function arguments from the value stack
	std::vector<token_type> stArg;
	for (int i=0; i<iArgCount; ++i)
		stArg.push_back( stVal.pop() );

	Parser::value_type	arg0;
	Parser::value_type	arg1;
	Parser::value_type	arg2;
	Parser::value_type	arg3;
	Parser::value_type	arg4;
	switch(funTok.GetArgCount()) {
		case -1:
			// Function with variable argument count
			// copy arguments into a vector<value_type>
			{
				/** \todo remove the unnecessary argument vector by changing order in stArg. */
				std::vector<value_type> vArg;
				for (int i=0; i<(int)stArg.size(); ++i)
					vArg.push_back(stArg[i].GetVal());

				valTok.SetVal( ((multfun_type)funTok.GetFuncAddr())(vArg) );
			}
			break;
		case 1:
			arg0 = stArg[0].GetVal();
			valTok.SetVal( ((fun_type1)funTok.GetFuncAddr())(arg0) );
			break;
		case 2:
			arg0 = stArg[0].GetVal();
			arg1 = stArg[1].GetVal();
			valTok.SetVal( ((fun_type2)funTok.GetFuncAddr())(arg1, arg0) );
			break;
		case 3:
			arg0 = stArg[0].GetVal();
			arg1 = stArg[1].GetVal();
			arg2 = stArg[2].GetVal();
			valTok.SetVal( ((fun_type3)funTok.GetFuncAddr())(arg2, arg1, arg0) );
			break;
		case 4:
			arg0 = stArg[0].GetVal();
			arg1 = stArg[1].GetVal();
			arg2 = stArg[2].GetVal();
			arg3 = stArg[3].GetVal();
			valTok.SetVal( ((fun_type4)funTok.GetFuncAddr())(arg3, arg2, arg1, arg0) );
			break;
		case 5:
			arg0 = stArg[0].GetVal();
			arg1 = stArg[1].GetVal();
			arg2 = stArg[2].GetVal();
			arg3 = stArg[3].GetVal();
			arg4 = stArg[4].GetVal();
			valTok.SetVal( ((fun_type5)funTok.GetFuncAddr())(arg4, arg3, arg2, arg1, arg0) );
			break;
		default: Error(ecINTERNAL_ERROR);
	}

	// Find out if the result will depend on a variable
	/** \todo remove this loop, put content in the loop that takes the argument values.

	  (Attention: SetVal will reset Flags.)
	*/
	bool bVolatile = funTok.IsFlagSet(token_type::flVOLATILE);
	for (int i=0; (bVolatile==false) && (i<iArgCount); ++i)
		bVolatile |= stArg[i].IsFlagSet(token_type::flVOLATILE);

	if (bVolatile)
		valTok.AddFlags(token_type::flVOLATILE);

	// Store the result; create Bytecode
	stVal.push(valTok);

	// Formula optimization
	if (m_bOptimize &&
		!valTok.IsFlagSet(token_type::flVOLATILE) &&
		!funTok.IsFlagSet(token_type::flVOLATILE) ) {
		m_vByteCode.RemoveValEntries(iArgCount);
		m_vByteCode.AddVal( valTok.GetVal() );
	} else {
		// operation dosnt depends on a variable or the function is flagged unoptimizeable
		// we cant optimize here...
		m_vByteCode.AddFun(pFunc, (funTok.GetArgCount()==-1) ? -iArgCount : iArgCount);
	}
}

//---------------------------------------------------------------------------
/** \brief Apply an unary operator.

  Bytecode for the operation will be created and optimized if applicable.

  \param a_FunTok Operator token
  \param a_FunTok Value token to which the operator should be applied
  \param a_iPos Index into current bytecopde position
  \param a_iStackIdx Stack position the result will take
*/
ParserBase::token_type ParserBase::ApplyUnaryOprt(const token_type &a_FunTok,
                                                  const token_type &a_ValTok) const
{
	fun_type1 pFunc = (fun_type1)a_FunTok.GetFuncAddr();
	Parser::value_type	arg = a_ValTok.GetVal();
	token_type result = pFunc(arg);

	// optimization if the result does not depend on a variable (direct or indirect)
	if (m_bOptimize && !a_ValTok.IsFlagSet(token_type::flVOLATILE)) {
		m_vByteCode.RemoveValEntries(1);
		m_vByteCode.AddVal(result.GetVal());
	} else {
		result.AddFlags( token_type::flVOLATILE );
		m_vByteCode.AddPostOp((void*)pFunc);
	}

	return result;
}

//---------------------------------------------------------------------------
/** \brief Parse the command code.

  Command code contains precalculated stack positions of the values and the
  associated operators.
  The Stack is filled beginning from index one the value at index zero is
  not used at all.

  \sa ParseString(), ParseValue()
*/
ParserBase::value_type ParserBase::ParseCmdCode() const
{
	value_type Stack[99];
	int	i = 0;
	int	iCode = 0;
	int	idx = 0;

	__start:

	idx = m_pCmdCode[i];
	iCode = m_pCmdCode[i+1];
	i += 2;
	assert(idx<99); // Formula too complex

	switch (iCode) {
		case token_type::cmAND: Stack[idx]  = (Stack[idx] != m_fZero) && (Stack[idx+1] != m_fZero); goto __start;
		case token_type::cmOR:  Stack[idx]  = (Stack[idx] != m_fZero) || (Stack[idx+1] != m_fZero); goto __start;
		case token_type::cmLE:  Stack[idx]  = Stack[idx] <= Stack[idx+1]; goto __start;
		case token_type::cmGE:  Stack[idx]  = Stack[idx] >= Stack[idx+1]; goto __start;
		case token_type::cmNEQ: Stack[idx]  = Stack[idx] != Stack[idx+1]; goto __start;
		case token_type::cmEQ:  Stack[idx]  = Stack[idx] == Stack[idx+1]; goto __start;
		case token_type::cmLT:  Stack[idx]  = Stack[idx] < Stack[idx+1];  goto __start;
		case token_type::cmGT:  Stack[idx]  = Stack[idx] > Stack[idx+1];  goto __start;
		case token_type::cmADD: Stack[idx] += Stack[1+idx]; goto __start;
		case token_type::cmSUB: Stack[idx] -= Stack[1+idx]; goto __start;
		case token_type::cmMUL: Stack[idx] *= Stack[1+idx]; goto __start;
		case token_type::cmDIV: Stack[idx] /= Stack[1+idx]; goto __start;
		case token_type::cmPOW: Stack[idx] = pow(Stack[idx].getDoubleValue(), Stack[1+idx].getDoubleValue()); goto __start;
		case token_type::cmVAR:
			Stack[idx] = *( (value_type*)(m_pCmdCode[i]) );
			i += m_vByteCode.GetPtrSize();
			goto __start;
		case token_type::cmVAL:
			Stack[idx] = *(value_type*)(&m_pCmdCode[i]);
			i += m_vByteCode.GetValSize(); // +=2
			goto __start;
		case token_type::cmFUNC:
			{
				int iArgCount = m_pCmdCode[i];

				switch(iArgCount) {  // switch according to argument count
					case 1: Stack[idx] = ((fun_type1)(m_pCmdCode[++i]))(Stack[idx]); break;
					case 2: Stack[idx] = ((fun_type2)(m_pCmdCode[++i]))(Stack[idx], Stack[idx+1]); break;
					case 3: Stack[idx] = ((fun_type3)(m_pCmdCode[++i]))(Stack[idx], Stack[idx+1], Stack[idx+2]); break;
					case 4: Stack[idx] = ((fun_type4)(m_pCmdCode[++i]))(Stack[idx], Stack[idx+1], Stack[idx+2], Stack[idx+3]); break;
					case 5: Stack[idx] = ((fun_type5)(m_pCmdCode[++i]))(Stack[idx], Stack[idx+1], Stack[idx+2], Stack[idx+3], Stack[idx+4]); break;
					default:
					if (iArgCount<=0) { // function with variable arguments store the number as a negative value
						std::vector<value_type> vArg(&Stack[idx], &Stack[idx-iArgCount]);
						Stack[idx] =((multfun_type)(m_pCmdCode[++i]))(vArg); break;
					} else {
						Error(ecINTERNAL_ERROR);
					}
				}
				++i;
			}
			goto __start;
		case token_type::cmPOSTOP:
			Stack[idx] = ((fun_type1)(m_pCmdCode[i]))(Stack[idx]);
			++i;
			goto __start;
		case token_type::cmEND:
			return Stack[1];
		default:
			Error(ecINTERNAL_ERROR);
			return 0;
	}
}

//---------------------------------------------------------------------------
/** \brief Return result for constant functions.

  Seems pointless, but for parser functions that are made up of only a value, which occur
  in real world applications, this speeds up things by removing the parser overhead almost
  completely.
*/
ParserBase::value_type ParserBase::ParseValue() const
{
	return *(value_type*)(&m_pCmdCode[2]);
}

//---------------------------------------------------------------------------
/** \brief One of the two main parse functions.

 Parse expression from input string. Perform syntax checking and create bytecode.
 After parsing the string and creating the bytecode the function pointer
 #m_pParseFormula will be changed to the second parse routine the uses bytecode instead of string parsing.

 \sa ParseCmdCode(), ParseValue()
*/
ParserBase::value_type ParserBase::ParseString() const
{
	if (!m_strFormula.length())
		Error(ecUNEXPECTED_EOF, 0);

	// Command code cannot be longe then 10 times formula length, in fact it will be much shorter
	// make sure to allocate at least 4 integer.
	m_vByteCode.clear();
	m_iPos = 0;         // global formula position index
	m_UsedVar.clear();  // Clear the map holding the variables actually used in an expression

	ParserStack<token_type> stOpt, stVal;
	ParserStack<int> stArgCount;
	token_type opta, opt;  // for storing operators
	token_type val, tval;  // for storing value
	int		iSynCtrl = noOPT | noBC | noPOSTOP;
	int		iErrc;
	int		iBrackets = 0; // bracket level counter

	while (true) {
		opt = ReadToken(m_iPos, iSynCtrl);

		switch (opt.GetType()) {
			case token_type::cmVAR:
				stVal.push(opt);
				m_vByteCode.AddVar( static_cast<value_type*>(opt.GetVar()) );
				ApplyInfixOp(stOpt, stVal);
				break;
			case token_type::cmVAL:
				stVal.push(opt);
				m_vByteCode.AddVal( opt.GetVal() );
				ApplyInfixOp(stOpt, stVal);
				break;
			case token_type::cmBC:    // For closing brackets make some syntax checks
				if ( --iBrackets < 0 ) {
					Error(ecUNEXPECTED_PARENS, m_iPos, opt.GetAsString());
				}
				// no break; Fall through behaviour!
			case token_type::cmCOMMA:
			case token_type::cmEND:
			case token_type::cmAND:
			case token_type::cmOR:
			case token_type::cmLT:
			case token_type::cmGT:
			case token_type::cmLE:
			case token_type::cmGE:
			case token_type::cmNEQ:
			case token_type::cmEQ:
			case token_type::cmADD:
			case token_type::cmSUB:
			case token_type::cmMUL:
			case token_type::cmDIV:
			case token_type::cmPOW:
				if (opt.GetType()==token_type::cmCOMMA) {
					if (stArgCount.empty())
						Error(ecUNEXPECTED_COMMA, m_iPos);
					++stArgCount.top(); // Record number of arguments
				}

				// There must be at least two operator tokens  available before we
				// can start the calculation. It's necessary to compare their
				// priorities. Get the previous operator from the operator stack in
				// order to find out if this operator can be applied (must have a
				// higher priority)
				opta = stOpt.pop(iErrc);
				if ( iErrc && opt.GetType()!=token_type::cmEND ) {
					stOpt.push(opt);
					break;
				}

				// Order does matter in the while statement because Priority will fail
				// if errc is set!
				while ( opta.GetType()!=token_type::cmBO &&
						opta.GetType()!=token_type::cmCOMMA &&
						!iErrc &&
						GetOprtPri(opta)>=GetOprtPri(opt)  ) {
					tval = stVal.pop(iErrc);
					val  = stVal.pop(iErrc);
					if (iErrc) {
						Error(ecINTERNAL_ERROR);
					}

					stVal.push( ApplyOprt(val, opta, tval) );
					opta = stOpt.pop(iErrc);
				} // while ( ... )

				// if opt is ")" and opta is "(" the bracket has been evaluated, now
				// its time to check if there is either a function or a sign pending
				// neither the opening nor the closing bracket will be pushed back to
				// the operator stack
				if ( (opta.GetType()==token_type::cmBO && opt.GetType()==token_type::cmBC) ) {
					// Check if a function is standing in front of the opening bracket,
					// if yes evaluate it afterwards check for infix operators
					assert(stArgCount.size());
					int iArgCount = stArgCount.pop();

					if (iArgCount>1 && stOpt.top().GetType()!=token_type::cmFUNC) {
						Error(ecUNEXPECTED_ARG, m_iPos);
					}

					ApplyFunction(iArgCount, stOpt, stVal);
					ApplyInfixOp(stOpt, stVal);
					break;
				} // if bracket content is evaluated

				if (!iErrc) {
					stOpt.push(opta);
				}

				// The operator can't be evaluated right now, push back to the operator stack
				// CmdEND, CmdCOMMA will not be pushed back, they have done their job now
				// (and triggerd evaluation of the formula or an expression used as function argument)
				if (opt.GetType()!=token_type::cmEND &&
					opt.GetType()!=token_type::cmCOMMA) {
					stOpt.push(opt);
				}
				break;
			case token_type::cmBO:
				++iBrackets;
				stArgCount.push(1);
			case token_type::cmFUNC:
			case token_type::cmINFIXOP:
				stOpt.push(opt);
				break;
			case token_type::cmPOSTOP: // Postfix operators will be treated like functions
				val = stVal.pop(iErrc);
				if (iErrc) {
					Error(ecINTERNAL_ERROR);
				}

				stVal.push( ApplyUnaryOprt(opt, val) );
				break;
			default:  Error(ecINTERNAL_ERROR);
		} // end of switch operator-token

		if ( opt.GetType() == token_type::cmEND ) {
			m_vByteCode.Finalize();
			break;
		}

#if defined(MU_PARSER_DUMP_STACK)
		StackDump(stVal, ops);
#endif
	} // while (true)

	// Store pointer to start of bytecode
	m_pCmdCode = m_vByteCode.GetRawData();

#if defined(MU_PARSER_DUMP_CMDCODE)
	m_vByteCode.AsciiDump();
#endif

	// check bracket counter
	if (iBrackets>0)
		Error(ecMISSING_PARENS, m_iPos);

	// get the last value (= final result) from the stack
	val = stVal.pop(iErrc);
	if (iErrc)
		Error(ecUNEXPECTED_EOF, 0);

	if (stVal.size())
		Error(ecINTERNAL_ERROR);

	// no error, so change the function pointer for the main parse routine
	value_type fVal = val.GetVal();   // Result from String parsing
	if (!m_bUseByteCode)
		return fVal; // returning here prevents switching to bytecode parsing mode

	// Constant result? If yes even bytecode parsing is pointless...
	if (m_pCmdCode[1]==token_type::cmVAL && m_pCmdCode[6]==token_type::cmEND)
		m_pParseFormula = &ParserBase::ParseValue;
	else
		m_pParseFormula = &ParserBase::ParseCmdCode;

	return fVal;
}

//---------------------------------------------------------------------------
/** \brief create an error containing the parse error position.

  This function will create an Parser Exception object containing the error text and its position.

  \throw ParserException, always throws thats the only purpose of this function.
*/
void  ParserBase::Error(EErrorCodes a_iErrc, int a_iPos, const string_type &a_strTok) const
{
	if (a_iErrc<=(int)m_ErrMsg.size())
		throw ParserException(m_ErrMsg[a_iErrc], a_strTok, m_strFormula, a_iPos, a_iErrc);

	throw ParserException("internal error: unknown error code", a_strTok, m_strFormula, a_iPos, a_iErrc);
}

//---------------------------------------------------------------------------
void  ParserBase::Error(EErrorCodes a_iErrc, const string_type &a_strTok) const
{
	Error(a_iErrc, -1, a_strTok);
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined variables.
\post Resets the parser to string parsing mode.
\throw nothrow
*/
void ParserBase::ClearVar()
{
	m_VarDef.clear();
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Remove a variable from internal storage.

Removes a variable if it exists. If the Variable does not exist nothing will be done.

\throw nothrow
*/
void ParserBase::RemoveVar(const string_type &a_strVarName)
{
	varmap_type::iterator item = m_VarDef.find(a_strVarName);
	if (item!=m_VarDef.end()) {
		m_VarDef.erase(item);
		m_pParseFormula = &ParserBase::ParseString;
	}
}

//------------------------------------------------------------------------------
/** \brief Clear the formula.

Clear the formula and existing bytecode.

\post Resets the parser to string parsing mode.
\throw nothrow
*/
void ParserBase::ClearFormula()
{
	m_vByteCode.clear();
	m_pCmdCode = 0;

	m_strFormula = "";
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Clear all functions.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearFun()
{
	m_FunDef.clear();
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined constants.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearConst()
{
	m_ConstDef.clear();
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined postfix operators.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearPostfixOp()
{
	m_PostOprtDef.clear();
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Clear the user defined Prefix operators.
    \post Resets the parser to string parser mode.
    \throw nothrow
*/
void ParserBase::ClearPrefixOp()
{
	m_InfixOprtDef.clear();
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Enable or disable the formula optimization feature.
    \post Resets the parser to string parser mode.
    \throw nothrow
*/
void ParserBase::EnableOptimizer(bool a_bIsOn)
{
	m_bOptimize = a_bIsOn;
	m_pParseFormula = &ParserBase::ParseString;
}

//------------------------------------------------------------------------------
/** \brief Enable or disable parsing from Bytecode.

  \attention There is no reason to disable bytecode. It will
             drastically decrease parsing speed.
*/
void ParserBase::EnableByteCode(bool a_bIsOn)
{
	m_bUseByteCode = a_bIsOn;

	if (!a_bIsOn)
		m_pParseFormula = &ParserBase::ParseString;
}

#if defined(MU_PARSER_DUMP_STACK) | defined(MU_PARSER_DUMP_CMDCODE)

//------------------------------------------------------------------------------
/** \brief Dump stack content.
*/
void ParserBase::StackDump( const ParserStack<token_type > &a_stVal,
							const ParserStack<token_type > &a_stOprt )
{
	ParserStack<token_type> stOprt(a_stOprt), stVal(a_stVal);
	int iErrc;

	cout << "\nValue stack:\n";
	while ( !stVal.empty() ) {
		token_type val = stVal.pop(iErrc);
		cout << " " << val.GetVal() << " ";
	}
	cout << "\nOperator stack:\n";

	while ( !stOprt.empty() ) {
		if (stOprt.top().GetType()<=13) {
			cout << ParserBase::c_DefaultOprt[stOprt.top().GetType()] << " ";
		} else {
			switch(stOprt.top().GetType()) {
				case token_type::cmVAR:     cout << "VAR "; break;
				case token_type::cmVAL:     cout << "VAL "; break;
				case token_type::cmFUNC:    cout << "FUNC "; break;
				case token_type::cmPOSTOP:  cout << "POSTOP "; break;
				case token_type::cmEND:     cout << "END "; break;
				case token_type::cmUNKNOWN: cout << "UNKNOWN "; break;
				default:  cout << stOprt.top().GetType() << " "; break;
			}
		}
		stOprt.pop();
	}

	cout <<endl;
}

#endif

} // namespace MathUtils
