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
#include "muParserTest.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>

#define PARSER_CONST_PI  3.141592653589793238462643
#define PARSER_CONST_E   2.718281828459045235360287

using namespace std;


namespace MathUtils
{

namespace Test
{
	//---------------------------------------------------------------------------
	ParserTester::ParserTester() :
		m_vTestFun(),
		m_stream(&std::cout)
	{
		AddTest(&ParserTester::TestNames);
		AddTest(&ParserTester::TestSyntax);
		AddTest(&ParserTester::TestPostFix);
		AddTest(&ParserTester::TestInfixOprt);
		AddTest(&ParserTester::TestVarConst);
		AddTest(&ParserTester::TestVolatile);
		AddTest(&ParserTester::TestMultiArg);
		AddTest(&ParserTester::TestFormula);
		AddTest(&ParserTester::TestInterface);
	}

	//---------------------------------------------------------------------------
	bool ParserTester::TestInterface()
	{
		bool result = true;
		*m_stream << "testing member functions...";

		// Test RemoveVar
		mu::Parser::value_type afVal[3] = {1,2,3};
		Parser p;
		try {
			p.AddVar("a", &afVal[0]);
			p.AddVar("b", &afVal[1]);
			p.AddVar("c", &afVal[2]);
			p.SetFormula("a+b+c");
			p.Calc();
		} catch(...) {
			// this is not supposed to happen
			result = false;
		}

		try {
			p.RemoveVar("c");
			p.Calc();

			// not supposed to reach this, variable "c" deleted...
			result = false;
		} catch(...) {
			// failure is expected...
		}

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}

	//---------------------------------------------------------------------------
	bool ParserTester::TestNames()
	{
		bool result = true;
		bool bStat = false;
		*m_stream << "testing name restriction enforcement...";

		Parser p;

#define PARSER_THROWCHECK(DOMAIN, FAIL, EXPR, ARG) \
			bStat = FAIL;                   \
			try                             \
			{                               \
			p.Add##DOMAIN(EXPR, ARG);     \
			}                               \
			catch(...)                      \
			{                               \
			bStat ^= true;                \
			}                               \
			result = result & bStat;

		// constant names
		PARSER_THROWCHECK(Const, false, "0a", 1)
		PARSER_THROWCHECK(Const, false, "9a", 1)
		PARSER_THROWCHECK(Const, false, "+a", 1)
		PARSER_THROWCHECK(Const, false, "-a", 1)
		PARSER_THROWCHECK(Const, false, "a-", 1)
		PARSER_THROWCHECK(Const, false, "a*", 1)
		PARSER_THROWCHECK(Const, false, "a?", 1)
		PARSER_THROWCHECK(Const, false, "a?", 1)
		PARSER_THROWCHECK(Const, false, "a?", 1)
		PARSER_THROWCHECK(Const, true, "a", 1)
		PARSER_THROWCHECK(Const, true, "a_min", 1)
		PARSER_THROWCHECK(Const, true, "a_min0", 1)
		PARSER_THROWCHECK(Const, true, "a_min9", 1)

		// variable names
		mu::Parser::value_type a;
		p.ClearConst();
		PARSER_THROWCHECK(Var, false, "9a", &a)
		PARSER_THROWCHECK(Var, false, "0a", &a)
		PARSER_THROWCHECK(Var, false, "+a", &a)
		PARSER_THROWCHECK(Var, false, "-a", &a)
		PARSER_THROWCHECK(Var, false, "?a", &a)
		PARSER_THROWCHECK(Var, false, "!a", &a)
		PARSER_THROWCHECK(Var, false, "a+", &a)
		PARSER_THROWCHECK(Var, false, "a-", &a)
		PARSER_THROWCHECK(Var, false, "a*", &a)
		PARSER_THROWCHECK(Var, false, "a?", &a)
		PARSER_THROWCHECK(Var, true, "a", &a)
		PARSER_THROWCHECK(Var, true, "a_min", &a)
		PARSER_THROWCHECK(Var, true, "a_min0", &a)
		PARSER_THROWCHECK(Var, true, "a_min9", &a)
		PARSER_THROWCHECK(Var, false, "a_min9", 0)

		// operator names
		// fail
		PARSER_THROWCHECK(PostfixOp, false, "(k", Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, false, "9+", Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, false, "+", 0)
		// pass
		PARSER_THROWCHECK(PostfixOp, true, "-a",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "?a",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "_",   Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "#",   Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "&&",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "||",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "&",   Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "|",   Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "++",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "--",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "?>",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "?<",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "**",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "xor", Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "and", Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "or",  Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "not", Parser::Sin)
		PARSER_THROWCHECK(PostfixOp, true, "!",   Parser::Sin)
#undef PARSER_THROWCHECK

		if (result) *m_stream << "passed" << endl;
			else *m_stream << "failed" << endl;

		return result;
	}

	//---------------------------------------------------------------------------
	bool ParserTester::TestSyntax()
	{
		bool result = true;
		*m_stream << "testing syntax engine...";

		result = result & EqnTest("(1+ 2*a)", 3, true); // Spaces within formula
		result = result & EqnTest("(1 + 2*a)", 3, true); // Spaces within formula
		result = result & EqnTest("(2+", 0, false);   // missing closing bracket
		result = result & EqnTest("2++4", 0, false);  // unexpected operator
		result = result & EqnTest("2+-4", 0, false);  // unexpected operator
		result = result & EqnTest("(2+)", 0, false);  // unexpected closing bracket
		result = result & EqnTest("--2", 0, false);   // double sign
		result = result & EqnTest("ksdfj", 0, false); // unknown token
		result = result & EqnTest("()", 0, false);    // empty bracket
		result = result & EqnTest("sin(cos)", 0, false); // unexpected function
		result = result & EqnTest("5t6", 0, false);   // unknown token
		result = result & EqnTest("5 t 6", 0, false); // unknown token
		result = result & EqnTest("8*", 0, false);    // unexpected end of formula
		result = result & EqnTest(",3", 0, false);    // unexpected comma
		result = result & EqnTest("3,5", 0, false);   // unexpected comma
		result = result & EqnTest("sin(8,8)", 0, false); // too many function args
		result = result & EqnTest("(7,8)", 0, false); // too many function args
		result = result & EqnTest("sin)", 0, false);  // unexpected closing bracket
		result = result & EqnTest("a)", 0, false);    // unexpected closing bracket
		result = result & EqnTest("pi)", 0, false);   // unexpected closing bracket
		result = result & EqnTest("sin(())", 0, false);   // unexpected closing bracket
		result = result & EqnTest("sin()", 0, false);   // unexpected closing bracket

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}

	//---------------------------------------------------------------------------
	bool ParserTester::TestVarConst()
	{
		bool result = true;
		*m_stream << "testing variables / constants name recognition...";

		// distinguish constants with same basename
		result = result & EqnTest("const", 1, true);
		result = result & EqnTest("const1", 2, true);
		result = result & EqnTest("const2", 3, true);
		result = result & EqnTest("2*const", 2, true);
		result = result & EqnTest("2*const1", 4, true);
		result = result & EqnTest("2*const2", 6, true);
		result = result & EqnTest("2*const+1", 3, true);
		result = result & EqnTest("2*const1+1", 5, true);
		result = result & EqnTest("2*const2+1", 7, true);
		result = result & EqnTest("const", 0, false);
		result = result & EqnTest("const1", 0, false);
		result = result & EqnTest("const2", 0, false);

		// distinguish variables with same basename
		result = result & EqnTest("a", 1, true);
		result = result & EqnTest("aa", 2, true);
		result = result & EqnTest("2*a", 2, true);
		result = result & EqnTest("2*aa", 4, true);
		result = result & EqnTest("2*a-1", 1, true);
		result = result & EqnTest("2*aa-1", 3, true);

		// Finally test querying of used variables
		try {
			MathUtils::Parser p;
			mu::Parser::value_type vVarVal[] = { 1, 2, 3, 4, 5};
			p.AddVar("a", &vVarVal[0]);
			p.AddVar("b", &vVarVal[1]);
			p.AddVar("c", &vVarVal[2]);
			p.AddVar("d", &vVarVal[3]);
			p.AddVar("e", &vVarVal[4]);

			// Test lookup of defined variables
			// 4 used variables
			p.SetFormula("a+b+c+d");
			MathUtils::Parser::varmap_type UsedVar = p.GetUsedVar();
			int iCount = (int)UsedVar.size();
			if (iCount!=4) throw false;

			MathUtils::Parser::varmap_type::const_iterator item = UsedVar.begin();
			for (int idx=0; item!=UsedVar.end(); ++item) {
				if (&vVarVal[idx++]!=item->second)
					throw false;
			}

			// Test lookup of undefined variables
			p.SetFormula("undef1+undef2+undef3");
			UsedVar = p.GetUsedVar();
			iCount = (int)UsedVar.size();
			if (iCount!=3) throw false;

			for (item=UsedVar.begin(); item!=UsedVar.end(); ++item) {
				if (item->second!=0)
					throw false; // all pointers to undefined variables must be null
			}

			// 1 used variables
			p.SetFormula("a+b");
			UsedVar = p.GetUsedVar();
			iCount = (int)UsedVar.size();
			if (iCount!=2) throw false;
			item = UsedVar.begin();
			for (int idx=0; item!=UsedVar.end(); ++item)
				if (&vVarVal[idx++]!=item->second) throw false;
		} catch(...) {
			result = false;
		}

	  if (result)
      *m_stream << "passed" << endl;
	  else
      *m_stream << "failed" << endl;

		return result;
	}

	//---------------------------------------------------------------------------
	bool ParserTester::TestMultiArg()
	{
		bool result = true;
		*m_stream << "testing multiarg functions...";

		// picking the right argument
		result = result & EqnTest("f1of1(1)", 1, true);
		result = result & EqnTest("f1of2(1, 2)", 1, true);
		result = result & EqnTest("f2of2(1, 2)", 2, true);
		result = result & EqnTest("f1of3(1, 2, 3)", 1, true);
		result = result & EqnTest("f2of3(1, 2, 3)", 2, true);
		result = result & EqnTest("f3of3(1, 2, 3)", 3, true);
		result = result & EqnTest("f1of4(1, 2, 3, 4)", 1, true);
		result = result & EqnTest("f2of4(1, 2, 3, 4)", 2, true);
		result = result & EqnTest("f3of4(1, 2, 3, 4)", 3, true);
		result = result & EqnTest("f4of4(1, 2, 3, 4)", 4, true);
		result = result & EqnTest("f1of5(1, 2, 3, 4, 5)", 1, true);
		result = result & EqnTest("f2of5(1, 2, 3, 4, 5)", 2, true);
		result = result & EqnTest("f3of5(1, 2, 3, 4, 5)", 3, true);
		result = result & EqnTest("f4of5(1, 2, 3, 4, 5)", 4, true);
		result = result & EqnTest("f5of5(1, 2, 3, 4, 5)", 5, true);
		// Too few arguments / Too many arguments
		result = result & EqnTest("f1of1(1,2)", 0, false);
		result = result & EqnTest("f1of1()", 0, false);
		result = result & EqnTest("f1of2(1, 2, 3)", 0, false);
		result = result & EqnTest("f1of2(1)", 0, false);
		result = result & EqnTest("f1of3(1, 2, 3, 4)", 0, false);
		result = result & EqnTest("f1of3(1)", 0, false);
		result = result & EqnTest("f1of4(1, 2, 3, 4, 5)", 0, false);
		result = result & EqnTest("f1of4(1)", 0, false);
		result = result & EqnTest("(1,2,3)", 0, false);
		result = result & EqnTest("1,2,3", 0, false);
		result = result & EqnTest("(1*a,2,3)", 0, false);
		result = result & EqnTest("1,2*a,3", 0, false);

		// correct calculation of arguments
		result = result & EqnTest("min(a, 1)",  1, true);
		result = result & EqnTest("min(3*2, 1)",  1, true);
		result = result & EqnTest("min(3*2, 1)",  6, false);
		// correct calculation of arguments
		result = result & EqnTest("min(3*a+1, 1)",  1, true);
		result = result & EqnTest("max(3*a+1, 1)",  4, true);
		result = result & EqnTest("max(3*a+1, 1)*2",  8, true);
		result = result & EqnTest("2*max(3*a+1, 1)+2",  10, true);

		// functions with Variable argument count
		result = result & EqnTest("sum(1,2,3)",  6, true);
		result = result & EqnTest("2*sum(1,2,3)",  12, true);
		result = result & EqnTest("2*sum(1,2,3)+2",  14, true);
		result = result & EqnTest("2*sum(-1,2,3)+2",  10, true);
		result = result & EqnTest("2*sum(-1,2,-(-a))+2",  6, true);
		result = result & EqnTest("2*sum(-1,10,-a)+2",  18, true);
		result = result & EqnTest("2*sum(1,2,3)*2",  24, true);
		result = result & EqnTest("sum(1,-max(1,2),3)*2",  4, true);
		result = result & EqnTest("sum(1*3, 4, a+2)",  10, true);
		result = result & EqnTest("sum(1*3, 2*sum(1,2,2), a+2)",  16, true);
		result = result & EqnTest("sum(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2)", 24, true);

		// some failures
		result = result & EqnTest("sum()",  0, false);
		result = result & EqnTest("sum(,)",  0, false);
		result = result & EqnTest("sum(1,2,)",  0, false);
		result = result & EqnTest("sum(,1,2)",  0, false);

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}


	//---------------------------------------------------------------------------
	bool ParserTester::TestInfixOprt()
	{
		bool result = true;
		*m_stream << "testing infix operators...";

		result = result & EqnTest("-1",    -1, true);
		result = result & EqnTest("-(-1)",  1, true);
		result = result & EqnTest("-(-1)*2",  2, true);
		result = result & EqnTest("-(-2)*sqrt(4)",  4, true);
		result = result & EqnTest("-a",  -1, true);
		result = result & EqnTest("-(a)",  -1, true);
		result = result & EqnTest("-(-a)",  1, true);
		result = result & EqnTest("-(-a)*2",  2, true);
		result = result & EqnTest("-(8)", -8, true);
		result = result & EqnTest("-8", -8, true);
		result = result & EqnTest("-(2+1)", -3, true);
		result = result & EqnTest("-(f1of1(1+2*3)+1*2)", -9, true);
		result = result & EqnTest("-(-f1of1(1+2*3)+1*2)", 5, true);
		result = result & EqnTest("-sin(8)", -0.989358, true);
		result = result & EqnTest("3-(-a)", 4, true);
		result = result & EqnTest("3--a", 4, true);

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}


	//---------------------------------------------------------------------------
	bool ParserTester::TestPostFix()
	{
		bool result = true;
		*m_stream << "testing postfix operators...";

		// application
		result = result & EqnTest("3m+5", 5.003, true);
		result = result & EqnTest("1000m", 1, true);
		result = result & EqnTest("1000 m", 1, true);
		result = result & EqnTest("(a)m", 1e-3, true);
		result = result & EqnTest("-(a)m", -1e-3, true);
		result = result & EqnTest("-2m", -2e-3, true);
		result = result & EqnTest("f1of1(1000)m", 1, true);
		result = result & EqnTest("-f1of1(1000)m", -1, true);
		result = result & EqnTest("-f1of1(-1000)m", 1, true);
		result = result & EqnTest("f4of4(0,0,0,1000)m", 1, true);
		result = result & EqnTest("2+(a*1000)m", 3, true);
		// some incorrect results
		result = result & EqnTest("1000m", 0.1, false);
		result = result & EqnTest("(a)m", 2, false);
		// failure due to syntax checking
		result = result & EqnTest("a m", 0, false);
		result = result & EqnTest("4 + m", 0, false);
		result = result & EqnTest("m4", 0, false);
		result = result & EqnTest("sin(m)", 0, false);
		result = result & EqnTest("m m", 0, false);
		result = result & EqnTest("m(8)", 0, false);
		result = result & EqnTest("4,m", 0, false);
		result = result & EqnTest("-m", 0, false);
		result = result & EqnTest("2(-m)", 0, false);
		result = result & EqnTest("2(m)", 0, false);

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}

	//---------------------------------------------------------------------------
	/** \brief Test volatile (nonoptimizeable functions). */
	bool ParserTester::TestVolatile()
	{
		bool result = true;
		*m_stream << "testing volatile/nonvolatile functions...";

		// First test with volatile flag turned on
		try {
			MathUtils::Parser p;
			p.AddFun("rnd", Rnd, false);

			// 1st test, compare results from sucessive calculations
			p.SetFormula("3+rnd(8)");
			if (p.Calc()==p.Calc()) result = false;

			// 2nd test, force bytecode creation, compare two results both
			// calculated from bytecode
			p.SetFormula("3+rnd(8)");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()==p.Calc()) result = false;

			p.SetFormula("3*rnd(8)+3");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()==p.Calc()) result = false;

			p.SetFormula("10+3*sin(rnd(8))-1");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()==p.Calc()) result = false;

			p.SetFormula("3+rnd(rnd(8))*2");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()==p.Calc()) result = false;
		} catch(ParserException &e) {
			*m_stream << e.GetMsg();
			result = false;
		}

		// Second test with volatile flag turned off
		try {
			MathUtils::Parser p;
			p.AddFun("rnd", Rnd);

			// compare string parsing with bytecode
			p.SetFormula("3+rnd(8)");
			if (p.Calc()!=p.Calc()) result = false;

			p.SetFormula("3+rnd(8)");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()!=p.Calc()) result = false;

			p.SetFormula("3*rnd(8)+3");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()!=p.Calc()) result = false;

			p.SetFormula("10+3*sin(rnd(8))-1");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()!=p.Calc()) result = false;

			p.SetFormula("3+rnd(rnd(8))*2");
			p.Calc(); //<- Force bytecode creation
			if (p.Calc()!=p.Calc()) result = false;
		} catch(ParserException &e) {
			*m_stream << e.GetMsg();
			result = false;
		}

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}

	//---------------------------------------------------------------------------
	bool ParserTester::TestFormula()
	{
		bool result = true;
		*m_stream << "testing sample formulas...";

		// operator precedencs
		result = result & EqnTest("1+2-3*4/5^6", 2.99923, true);
		result = result & EqnTest("1^2/3*4-5+6", 2.3333, true);
		result = result & EqnTest("1+2*3", 7, true);
		result = result & EqnTest("1+2*3", 7, true);
		result = result & EqnTest("(1+2)*3", 9, true);
		result = result & EqnTest("(1+2)*(-3)", -9, true);
		result = result & EqnTest("2/4", 0.5, true);

		result = result & EqnTest("exp(ln(7))", 7, true);
		result = result & EqnTest("e^ln(7)", 7, true);
		result = result & EqnTest("e^(ln(7))", 7, true);
		result = result & EqnTest("(e^(ln(7)))", 7, true);
		result = result & EqnTest("1-(e^(ln(7)))", -6, true);
		result = result & EqnTest("2*(e^(ln(7)))", 14, true);
		result = result & EqnTest("10^log(5)", 5, true);
		result = result & EqnTest("10^log10(5)", 5, true);
		result = result & EqnTest("2^log2(4)", 4, true);
		result = result & EqnTest("-(sin(0)+1)", -1, true);
		result = result & EqnTest("-(2^1.1)", -2.14354692, true);

		result = result & EqnTest("(cos(2.41)/b)", -0.372056, true);

		// long formula (Reference result: Matlab)
		result = result & EqnTest(
			"(((-9))-e/(((((((pi-(((-7)+(-3)/4/e))))/(((-5))-2)-((pi+(-0))*(sqrt((e+e))*(-8))*(((-pi)+(-pi)-(-9)*(6*5))"
			"/(-e)-e))/2)/((((sqrt(2/(-e)+6)-(4-2))+((5/(-2))/(1*(-pi)+3))/8)*pi*((pi/((-2)/(-6)*1*(-1))*(-6)+(-e)))))/"
			"((e+(-2)+(-e)*((((-3)*9+(-e)))+(-9)))))))-((((e-7+(((5/pi-(3/1+pi)))))/e)/(-5))/(sqrt((((((1+(-7))))+((((-"
			"e)*(-e)))-8))*(-5)/((-e)))*(-6)-((((((-2)-(-9)-(-e)-1)/3))))/(sqrt((8+(e-((-6))+(9*(-9))))*(((3+2-8))*(7+6"
			"+(-5))+((0/(-e)*(-pi))+7)))+(((((-e)/e/e)+((-6)*5)*e+(3+(-5)/pi))))+pi))/sqrt((((9))+((((pi))-8+2))+pi))/e"
			"*4)*((-5)/(((-pi))*(sqrt(e)))))-(((((((-e)*(e)-pi))/4+(pi)*(-9)))))))+(-pi)", -12.23016549, true);

		// long formula (Reference result: Matlab)
		result = result & EqnTest("1+2-3*4/5^6*(2*(1-5+(3*7^9)*(4+6*7-3)))+12", -7995810.09926, true);

		if (result)
			*m_stream << "passed" << endl;
		else
			*m_stream << "failed" << endl;

		return result;
	}


	//---------------------------------------------------------------------------
	void ParserTester::AddTest(testfun_type a_pFun)
	{
		m_vTestFun.push_back(a_pFun);
	}


	//---------------------------------------------------------------------------
	/** \brief Set the stream that takes the output of the test session. */
	void ParserTester::SetStream(std::ostream *a_stream)
	{
		assert(a_stream);

		m_stream = a_stream;
	}

	//---------------------------------------------------------------------------
	void ParserTester::Run()
	{
		bool bPass = true;
		try {
			for (int i=0; i<(int)m_vTestFun.size(); ++i)
				bPass &= (this->*m_vTestFun[i])();
		} catch(mu::ParserException &e) {
			*m_stream << e.GetMsg() << endl;
			*m_stream << e.GetToken() << endl;
			Abort();
		} catch(std::exception &e) {
			*m_stream << e.what() << endl;
			Abort();
		} catch(...) {
			*m_stream << "Internal error";
			Abort();
		}

		if (bPass)
			*m_stream << "Test passed" << endl;
		else
			*m_stream << "Test failed" << endl;
	}


	//---------------------------------------------------------------------------
	bool ParserTester::EqnTest(std::string a_str, double a_fRes, bool a_fPass)
	{
		try {
			Parser p;
			p.AddConst("pi", mu::Parser::value_type(PARSER_CONST_PI));
			p.AddConst("e", mu::Parser::value_type(PARSER_CONST_E));

			  // variables
			mu::Parser::value_type vVarVal[] = { 1, 2, 3, -2};
			p.AddVar("a", &vVarVal[0]);
			p.AddVar("aa", &vVarVal[1]);
			p.AddVar("b", &vVarVal[1]);
			p.AddVar("c", &vVarVal[2]);
			p.AddVar("d", &vVarVal[3]);
			// constants
			p.AddConst("const", mu::Parser::value_type(1));
			p.AddConst("const1", mu::Parser::value_type(2));
			p.AddConst("const2", mu::Parser::value_type(3));

			// Add a volatile function
			p.AddFun("rnd", Rnd, false);  // a function that is not optimizeable

			// functions
			// one parameter
			p.AddFun("f1of1", f1of1);
			// two parameter
			p.AddFun("f1of2", f1of2);
			p.AddFun("f2of2", f2of2);
			// three parameter
			p.AddFun("f1of3", f1of3);
			p.AddFun("f2of3", f2of3);
			p.AddFun("f3of3", f3of3);
			// four parameter
			p.AddFun("f1of4", f1of4);
			p.AddFun("f2of4", f2of4);
			p.AddFun("f3of4", f3of4);
			p.AddFun("f4of4", f4of4);
			// four parameter
			p.AddFun("f1of5", f1of5);
			p.AddFun("f2of5", f2of5);
			p.AddFun("f3of5", f3of5);
			p.AddFun("f4of5", f4of5);
			p.AddFun("f5of5", f5of5);
			// sample functions
			p.AddFun("min", Min);
			p.AddFun("max", Max);
			p.AddFun("sum", Sum);
			// postfix operator
			p.AddPostfixOp("m", Milli);
			p.SetFormula(a_str);

			mu::Parser::value_type fVal[4] = {1,2,3,4}; // initially should be different
			fVal[0] = p.Calc(); // result from string parsing
			fVal[1] = p.Calc(); // result from bytecode

			if (fVal[0]!=fVal[1])
				throw ParserException("Bytecode corrupt.");

			try {
				// Test copy constructor
				std::vector<MathUtils::Parser> vParser;
				vParser.push_back(p);
				MathUtils::Parser p2 = vParser[0];
				fVal[2] = p2.Calc();

				// Test assignement operator
				// additionally  disable Optimizer this time
				MathUtils::Parser p3;
				p3 = p2;
				p3.EnableOptimizer(false);
				fVal[3] = p3.Calc();
			} catch(exception &e) {
				*m_stream << e.what() << "\n";
			}

			// limited floating point accuracy requires the following test
			bool bCloseEnough(true);
			for (int i=0; i<4; ++i) {
				bCloseEnough &= (fabs(a_fRes-fVal[i].getDoubleValue()) < fabs(fVal[i].getDoubleValue()*0.0001));
			}

			return (bCloseEnough && a_fPass) || (!bCloseEnough && !a_fPass);
		} catch(ParserException &e) {
			if (a_fPass)
				*m_stream << "\n" << e.GetFormula() << ":" << e.GetMsg();
		} catch(...) {
		}

		return (a_fPass==false) ? true : false;
	}

	//---------------------------------------------------------------------------
	/** \brief Internal error in test class Test is going to be aborted. */
	void ParserTester::Abort() const
	{
		*m_stream << "Test failed (internal error in test class)" << endl;
		while (!getchar());
		exit(-1);
	}

} // namespace test

} // namespace MathUtils
