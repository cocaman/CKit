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

#include <cmath>
#include <algorithm>
#include <numeric>

/***/
#define PARSER_CONST_PI  3.141592653589793238462643

/** \brief The eulerian number. */
#define PARSER_CONST_E   2.718281828459045235360287

/** \brief Namespace for mathematical applications. */
namespace MathUtils
{

// Trigonometric function
Parser::value_type Parser::Sin(Parser::value_type & v)   { return Parser::value_type(sin(v.getDoubleValue()));  }
Parser::value_type Parser::Cos(Parser::value_type & v)   { return Parser::value_type(cos(v.getDoubleValue()));  }
Parser::value_type Parser::Tan(Parser::value_type & v)   { return Parser::value_type(tan(v.getDoubleValue()));  }
Parser::value_type Parser::ASin(Parser::value_type & v)  { return Parser::value_type(asin(v.getDoubleValue())); }
Parser::value_type Parser::ACos(Parser::value_type & v)  { return Parser::value_type(acos(v.getDoubleValue())); }
Parser::value_type Parser::ATan(Parser::value_type & v)  { return Parser::value_type(atan(v.getDoubleValue())); }
Parser::value_type Parser::Sinh(Parser::value_type & v)  { return Parser::value_type(sinh(v.getDoubleValue())); }
Parser::value_type Parser::Cosh(Parser::value_type & v)  { return Parser::value_type(cosh(v.getDoubleValue())); }
Parser::value_type Parser::Tanh(Parser::value_type & v)  { return Parser::value_type(tanh(v.getDoubleValue())); }
Parser::value_type Parser::ASinh(Parser::value_type & v)
{
	double	x = v.getDoubleValue();
	return Parser::value_type(log(x + sqrt(x * x + 1)));
}
Parser::value_type Parser::ACosh(Parser::value_type & v)
{
	double	x = v.getDoubleValue();
	return Parser::value_type(log(x + sqrt(x * x - 1.0)));
}
Parser::value_type Parser::ATanh(Parser::value_type & v)
{
	double	x = v.getDoubleValue();
	return (Parser::value_type((double)0.5 * log((1.0 + x) / (1 - x))));
}
// Logarithm functions
Parser::value_type Parser::Log2(Parser::value_type & v)
{
	// Logarithm base 2
	return Parser::value_type(log(v.getDoubleValue())/log((double)2.0));
}
Parser::value_type Parser::Log10(Parser::value_type & v)
{
	// Logarithm base 10
	return Parser::value_type(log10(v.getDoubleValue()));
}
Parser::value_type Parser::Ln(Parser::value_type & v)    { return Parser::value_type(log(v.getDoubleValue()));   } // Logarithm base e (natural logarithm)
//  misc
Parser::value_type Parser::Exp(Parser::value_type & v)   { return Parser::value_type(exp(v.getDoubleValue()));   }
Parser::value_type Parser::Abs(Parser::value_type & v)   { return Parser::value_type(fabs(v.getDoubleValue()));  }
Parser::value_type Parser::Sqrt(Parser::value_type & v)  { return Parser::value_type(sqrt(v.getDoubleValue()));  }
Parser::value_type Parser::Rint(Parser::value_type & v)  { return Parser::value_type(floor(v.getDoubleValue() + (double)0.5)); }
Parser::value_type Parser::Sign(Parser::value_type & v)
{
	double	x = v.getDoubleValue();
	return Parser::value_type(((x<0) ? (double)-1.0 : (x>0) ? (double)1.0 : (double)0.0));
}
// string functions
Parser::value_type Parser::UpperStr(value_type & arg)
{
	CKString	ans = *arg.getStringValue();
	ans.toUpper();
	return Parser::value_type(&ans);
}
Parser::value_type Parser::LowerStr(value_type & arg)
{
	CKString	ans = *arg.getStringValue();
	ans.toLower();
	return Parser::value_type(&ans);
}
Parser::value_type Parser::TrimStr(value_type & arg)
{
	CKString	ans = *arg.getStringValue();
	ans.trim();
	return Parser::value_type(&ans);
}
Parser::value_type Parser::RightStr(value_type & arg, value_type & num)
{
	Parser::value_type	retval;
	const CKString	*str = arg.getStringValue();
	if (str != NULL) {
		CKString	ans = str->right(num.getIntValue());
		retval = ans;
	}
	return retval;
}
Parser::value_type Parser::LeftStr(value_type & arg, value_type & num)
{
	Parser::value_type	retval;
	const CKString	*str = arg.getStringValue();
	if (str != NULL) {
		CKString	ans = str->left(num.getIntValue());
		retval = ans;
	}
	return retval;
}
Parser::value_type Parser::MidStr(value_type & arg, value_type & start, value_type & end)
{
	Parser::value_type	retval;
	const CKString	*str = arg.getStringValue();
	if (str != NULL) {
		CKString	ans = str->mid(start.getIntValue(), end.getIntValue());
		retval = ans;
	}
	return retval;
}
Parser::value_type Parser::LenStr(value_type & arg)
{
	const CKString	*str = arg.getStringValue();
	return (str != NULL ? Parser::value_type((double)str->length()) : Parser::value_type());
}
// Conditional (if then else)
Parser::value_type Parser::Ite(Parser::value_type & v1, Parser::value_type & v2, Parser::value_type & v3)
{
	return (v1.getDoubleValue() == 1.0) ? v2 : v3;
}

// Unary operator Callbacks: Postfix operators
Parser::value_type Parser::Milli(Parser::value_type & v) { return Parser::value_type(v.getDoubleValue()/(double)1.0e3); }
Parser::value_type Parser::Nano(Parser::value_type & v)  { return Parser::value_type(v.getDoubleValue()/(double)1.0e6); }
Parser::value_type Parser::Micro(Parser::value_type & v) { return Parser::value_type(v.getDoubleValue()/(double)1.0e9); }
// Unary operator Callbacks: Infix operators
Parser::value_type Parser::UnaryMinus(Parser::value_type & v) { return Parser::value_type((double)-1.0 * v.getDoubleValue()); }
Parser::value_type Parser::Not(Parser::value_type & v) { return Parser::value_type((double)(v.getDoubleValue() == 0)); }

// Functions with variable number of arguments
// sum
Parser::value_type Parser::Sum(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function sum.");

	double	sum = 0.0;
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		sum += i->getDoubleValue();
	}
	return Parser::value_type(sum);
}

// mean value
Parser::value_type Parser::Avg(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function avg.");

	double	sum = 0.0;
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		sum += i->getDoubleValue();
	}
	return Parser::value_type((double)sum/a_vArg.size());
}

// minimum
Parser::value_type Parser::Min(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function min.");

	double	min = NAN;
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		if (isnan(min) || (min > i->getDoubleValue())) {
			min = i->getDoubleValue();
		}
	}
	return Parser::value_type(min);
}

// maximum
Parser::value_type Parser::Max(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function max.");

	double	max = NAN;
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		if (isnan(max) || (max < i->getDoubleValue())) {
			max = i->getDoubleValue();
		}
	}
	return Parser::value_type(max);
}


//---------------------------------------------------------------------------
/** \brief Constructor.

  Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
*/
Parser::Parser() :
	ParserBase(),
	m_fEpsilon((Parser::value_type)1e-7)
{
	InitFun();
	InitConst();
	InitOprt();
}

//---------------------------------------------------------------------------
/** \brief Initialize the default functions. */
void Parser::InitFun()
{
	// trigonometric functions
	AddFun("sin", Sin);
	AddFun("cos", Cos);
	AddFun("tan", Tan);
	// arcus functions
	AddFun("asin", ASin);
	AddFun("acos", ACos);
	AddFun("atan", ATan);
	// hyperbolic functions
	AddFun("sinh", Sinh);
	AddFun("cosh", Cosh);
	AddFun("tanh", Tanh);
	// arcus hyperbolic functions
	AddFun("asinh", ASinh);
	AddFun("acosh", ACosh);
	AddFun("atanh", ATanh);
	// Logarithm functions
	AddFun("log2", Log2);
	AddFun("log10", Log10);
	AddFun("log", Log10);
	AddFun("ln", Ln);
	// misc
	AddFun("exp", Exp);
	AddFun("sqrt", Sqrt);
	AddFun("sign", Sign);
	AddFun("rint", Rint);
	AddFun("abs", Abs);
	AddFun("if", Ite);
	// string functions
	AddFun("upper$", UpperStr);
	AddFun("lower$", LowerStr);
	AddFun("trim$", TrimStr);
	AddFun("right$", RightStr);
	AddFun("left$", LeftStr);
	AddFun("mid$", MidStr);
	AddFun("length", LenStr);

	// Functions with variable number of arguments
	AddFun("sum", Sum);
	AddFun("avg", Avg);
	AddFun("min", Min);
	AddFun("max", Max);
}

//---------------------------------------------------------------------------
/** \brief Initialize operators. */
void Parser::InitConst()
{
	AddConst("_pi", (Parser::value_type)PARSER_CONST_PI);
	AddConst("_e", (Parser::value_type)PARSER_CONST_E);
}

//---------------------------------------------------------------------------
/** \brief Initialize operators. */
void Parser::InitOprt()
{
	AddPostfixOp("m", Milli);
	AddPostfixOp("n", Nano);
	AddPostfixOp("mu", Micro);

	AddPrefixOp("-", UnaryMinus);
	AddPrefixOp("!", Not);
}

//---------------------------------------------------------------------------
/** \brief Return characters valid for identifier names. */
const Parser::char_type* Parser::ValidNameChars() const
{
	return "$0123456789_.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

//---------------------------------------------------------------------------
/** \brief Return characters valid for operator identifiers. */
const Parser::char_type* Parser::ValidOprtChars() const
{
	return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-*^/?<>=#!$%&|~'_";
}

//---------------------------------------------------------------------------
/** \brief Return characters valid for prefix operators. */
const Parser::char_type* Parser::ValidPrefixOprtChars() const
{
	return "/+-*^?<>=#!$%&|~'_";
}

//---------------------------------------------------------------------------
/** \brief Numerically differentiate with regard to a variable. */
Parser::value_type Parser::Diff(Parser::value_type *a_Var, Parser::value_type a_fPos) const
{
	assert(m_fEpsilon.getDoubleValue());
	Parser::value_type	fEpsilon( (a_fPos==0) ? (double)1e-10 : m_fEpsilon.getDoubleValue() * a_fPos.getDoubleValue() );
	Parser::value_type	fRes(0);
	Parser::value_type	fBuf(*a_Var);
	Parser::value_type	f[4] = {0,0,0,0};

	*a_Var = a_fPos.getDoubleValue() + 2.0*fEpsilon.getDoubleValue();
	f[0] = Calc();
	*a_Var = a_fPos.getDoubleValue() + 1.0*fEpsilon.getDoubleValue();
	f[1] = Calc();
	*a_Var = a_fPos.getDoubleValue() - 1.0*fEpsilon.getDoubleValue();
	f[2] = Calc();
	*a_Var = a_fPos.getDoubleValue() - 2.0*fEpsilon.getDoubleValue();
	f[3] = Calc();
	*a_Var = fBuf; // restore variable

	fRes = (-1.0*f[0].getDoubleValue() + 8.0*f[1].getDoubleValue() - 8.0*f[2].getDoubleValue() + f[3].getDoubleValue()) / (12.0*fEpsilon.getDoubleValue());
	return fRes;
}


} // namespace MathUtils
