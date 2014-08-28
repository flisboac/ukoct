
//#pragma GCC diagnostic ignored "-Wreorder"

#include <sstream>
#include <iostream>
#include "plas.hpp"


int main(void) {
#ifdef __GNUC__
    std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
#endif
	std::string source(
			"p oct 2 5\n"
			"e  1  2  2\n"
			"e -1 -2  1\n"
			"e  1 -2 -3\n"
			"e  0  2 -2\n"
			"e  0 -1  1\n"
			"n  1 x0   \n"
			"n  2 x1   \n");
	std::stringstream ss(source);
	std::cout << "Creating problems objects..." << std::endl;
	plas::Problem<double> problem;
	plas::Problem<double> transformedProblem;
	problem.matrix()->defaultVal(std::numeric_limits<double>::infinity());
	transformedProblem.matrix()->defaultVal(std::numeric_limits<double>::infinity());
	std::cout << "Opening test problem..." << std::endl;
	plas::open(ss, problem, &std::clog);
	std::cout << problem;
	std::cout << *problem.matrix();
	std::cout << std::endl << "* Transforming..." << std::endl << std::endl;
	plas::toOctDiffProblem(problem, transformedProblem);
	std::cout << transformedProblem;
	std::cout << *transformedProblem.matrix();
	return 0;
}
