#include "muParserTest.h"

/** \brief This macro will enable mathematical constants like M_PI. */
#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>

#include "muParser.h"

//typedef mu::Parser::value_type value_type;

// Operator callback functions
mu::Parser::value_type Mega(mu::Parser::value_type & a_fVal) { return mu::Parser::value_type(a_fVal.getDoubleValue() * 1.0e6); }
mu::Parser::value_type Milli(mu::Parser::value_type & a_fVal) { return mu::Parser::value_type(a_fVal.getDoubleValue() / 1.0e3); }
mu::Parser::value_type Rnd(mu::Parser::value_type & v) { return mu::Parser::value_type(v.getDoubleValue()*std::rand()/(RAND_MAX+1.0)); }
mu::Parser::value_type Add(mu::Parser::value_type & v1, mu::Parser::value_type & v2) { return v1+v2; };

//---------------------------------------------------------------------------
void SelfTest()
{
	std::cout << "---------------------------------------\n";
	std::cout << "\n";
	std::cout << "  Math Parser sample application\n";
	std::cout << "\n";
	std::cout << "---------------------------------------\n";
	
	MathUtils::Test::ParserTester pt;
	pt.Run();
	
	std::cout << "---------------------------------------\n";
	std::cout << "Functions:\n";
	std::cout << "  min(x,y) return minimum of x and y\n";
	std::cout << "  max(x,y) return maximum of x and y\n";
	std::cout << "  avg(...) return mean value of all input values\n";
	std::cout << "  sum(...) return sum of input values\n";
	std::cout << "Postfix operators:\n";
	std::cout << "  \"m\"  milli - divide by 1e3\n";
	std::cout << "  \"M\"  Mega - multiply with by 1e6\n";
	std::cout << "Prefix Operators:\n";
	std::cout << "  \"!\"  Not - logical not\n";
	std::cout << "Constants:\n";
	std::cout << "  \"_e\"   2.718281828459045235360287\n";
	std::cout << "  \"_pi\"  3.141592653589793238462643\n";
	std::cout << "---------------------------------------\n";
	std::cout << "Please enter a formula:\n";
}

//---------------------------------------------------------------------------
void DumpUsedVariables(const MathUtils::Parser &parser)
{
	// Query the used variables (must be done after calc)
	MathUtils::Parser::varmap_type UsedVar = parser.GetUsedVar();
	if (UsedVar.size()) {
		std::cout << "\nUsed variables:\n";
		std::cout << "---------------\n";
		std::cout << "Number: " << (int)UsedVar.size() << "\n";
		MathUtils::Parser::varmap_type::const_iterator item = UsedVar.begin();
		for (; item!=UsedVar.end(); ++item) {
			std::cout << "Name: " << item->first << "   Address: [0x" << item->second << "]\n";
		} // for all used variables
	} // if used variables exist
}

//---------------------------------------------------------------------------
void Calc()
{
	char line[100];
	mu::Parser parser;
	
	// Add some variables
	mu::Parser::value_type  vVarVal[] = { 1, 2 }; // Values of the parser variables
	parser.AddVar("a", &vVarVal[0]);  // Assign Variable names and bind them to the C++ variables
	parser.AddVar("b", &vVarVal[1]);
	// Add user defined unary operators
	parser.AddPostfixOp("M", Mega);
	parser.AddPostfixOp("m", Milli);
	// Add a volatile function
	parser.AddFun("rnd", Rnd, false); // Add an unoptimizeable function
	parser.AddFun("add", Add);        // Add an unoptimizeable function
	parser.EnableOptimizer(true);     // set optimizer state (defaults to true)

	while(true) {
		try {
			while ( fgets(line, 99, stdin) ) {
				if (!strncmp("quit", line, 4))
					exit(0);

				line[strlen(line)-1] = 0; // overwrite nl
				parser.SetFormula(line);
				DumpUsedVariables(parser);
/*
				vVarVal[0]=1;
				cout << parser.Calc() << "\n";
				
				vVarVal[0]=0;
*/
				std::cout << parser.Calc() << "\n";

				// next line demonstrates numerical differentiation with respect to a at position 0:
				//      cout << "d(formula)/d(a)|a=0 = " << parser.Diff(&vVarVal[0], 1) << "\n";
			}
		} catch(MathUtils::ParserException &e) {
#define LONG_ERR
#if !defined(LONG_ERR)
			std::cout << e.GetMsg() << "\n";
#else
			std::cout << "\nError:\n";
			std::cout << "------\n";
			std::cout << "Message:  " << e.GetMsg() << "\n";
			std::cout << "Formula:  " << e.GetFormula() << "\n";
			std::cout << "Token:    " << e.GetToken() << "\n";
			std::cout << "Position: " << e.GetPos() << "\n";
			std::cout << "Errc:     " << e.GetCode() << "\n";
#endif
		}
	} // while running
}

//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	using namespace mu;
	
	SelfTest();
	
	try {
		Calc();
	} catch(Parser::exception_type &e) {
		// Only erros raised during the initialization will end up here
		// formula related errors are treated in Calc()
		std::cout << "Message:  " << e.GetMsg() << std::endl;
	}
	
	return 0;
}
