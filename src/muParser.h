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
#ifndef MU_PARSER_H
#define MU_PARSER_H

#include "muParserBase.h"
#include <vector>


namespace MathUtils
{

/** \brief Mathematical expressions parser.

  Standard implementation of the mathematical expressions parser.
  Can be used as a reference implementation for subclassing the parser.

  Version 1.09<br>
  <small>
  (C) 2004 Ingo Berg<br>
  ingo_berg(at)gmx.de
  </small>
*/
class Parser : public ParserBase
{
	public:
		Parser();

		// Trigonometric functions
		static value_type  Sin(value_type & arg);
		static value_type  Cos(value_type & arg);
		static value_type  Tan(value_type & arg);
		// arcus functions
		static value_type  ASin(value_type & arg);
		static value_type  ACos(value_type & arg);
		static value_type  ATan(value_type & arg);
		// hyperbolic functions
		static value_type  Sinh(value_type & arg);
		static value_type  Cosh(value_type & arg);
		static value_type  Tanh(value_type & arg);
		// arcus hyperbolic functions
		static value_type  ASinh(value_type & arg);
		static value_type  ACosh(value_type & arg);
		static value_type  ATanh(value_type & arg);
		// Logarithm functions
		static value_type  Log2(value_type & arg);
		static value_type  Log10(value_type & arg);
		static value_type  Ln(value_type & arg);
		// misc
		static value_type  Exp(value_type & arg);
		static value_type  Abs(value_type & arg);
		static value_type  Sqrt(value_type & arg);
		static value_type  Rint(value_type & arg);
		static value_type  Sign(value_type & arg);
		// string functions
		static value_type  UpperStr(value_type & arg);
		static value_type  LowerStr(value_type & arg);
		static value_type  TrimStr(value_type & arg);
		static value_type  RightStr(value_type & arg, value_type & num);
		static value_type  LeftStr(value_type & arg, value_type & num);
		static value_type  MidStr(value_type & arg, value_type & start, value_type & end);
		static value_type  LenStr(value_type & arg);
		// branching
		static value_type  Ite(value_type & cond, value_type & tval, value_type & fval);
		
		// Postfix operator
		static value_type  Milli(value_type & arg);
		static value_type  Nano(value_type & arg);
		static value_type  Micro(value_type & arg);
		static value_type  UnaryMinus(value_type & arg);
		static value_type  Not(value_type & arg);
		
		// Functions with variable number of arguments
		static value_type Sum(const std::vector<value_type> &a_vArg);
		static value_type Avg(const std::vector<value_type> &a_vArg);
		static value_type Min(const std::vector<value_type> &a_vArg);
		static value_type Max(const std::vector<value_type> &a_vArg);

		value_type m_fEpsilon; ///< Epsilon used for numerical differentiation.

		virtual const char_type* ValidNameChars() const;
		virtual const char_type* ValidOprtChars() const;
		virtual const char_type* ValidPrefixOprtChars() const;
		
		virtual void InitFun();
		virtual void InitConst();
		virtual void InitOprt();
		
		value_type Diff(value_type *a_Var, value_type a_fPos) const;
};

} // namespace MathUtils

#endif
