#include "risch.h"
#include <string>
#include <cstring>

funk testSimplify(const std::string& test, bool print = true)
{
  if (print) std::cout << "Testing " << test << std::endl;
  funk theTest(test);
  if (print) std::cout << "Funk before simplify: " << theTest << std::endl;
  theTest.simplify();
  if (print) std::cout << "Funk after simplify: " << theTest  << std::endl;
  return theTest;
}

void test(const std::string& test, const std::string& compare = "", bool printProcess = true)
{
  funk theTest = testSimplify(test, printProcess);
  //Put more tests here
  if (compare != ""){
    funk compareTest(compare);
    if (compareTest != theTest)
      std::cout << "Result funk " << theTest << " does not match expected funk " << compareTest << std::endl;
  }
}

int main(int argc, char** argv){
  if (argc == 1 || !std::strcmp(argv[1], "-dst")){
    test("0");
    test("1");
    test("52");
    test("0000000001");
    test("0*x");
    test("392*x");
    test("29*x^2");
    test("029*x^5");
    test("8*x^0");
    test("0*x^0");
    test("x + 0");
    test("0 + x");
    test("0*x + x^2");
    test("x + x");
    test("3*x + 6*x");
    test("-9*x + 20*x", "11x");
    test("2 + 2", "4");
    test("3+-1", "2");
    test("592 * 2193", "1298256");
    test("6 / 2", "3");
    test("(6 * x)/(3 * x)", "2");
    test("(x / 2) * (2 / x)", "1");
    test("log(1)", "0");
    test("log(x)", "log(x)");
    test("e^0", "1");
    test("e^1", "e^1");
    test("2 * e^(3 * x + x)", "2 * e^(4x)");
    test("log(e^(2 + x))", "2 + x");
    test("log(e^4 * x + -3 * (2 * x))", "log((e^4 * x) + -6x)");
    test("sin(30)", "sin(30)");
    test("sin(0)", "0");
    test("cos(0)", "1");
    test("cos(5 + x)", "cos(5 + x)");
  }
  test("x^2 + 7*x + -6");
  test("x^2 + -5*x + 0");

	/*	std::unique_ptr<funk> curr1;
	curr1 = string_to_funk(sa);
	
	std::unique_ptr<funk> curr2;
	curr2 = string_to_funk(sb);
	
	curr1 -> print();
	std::cout << std::endl;
	curr2 -> print();
	std::cout << std::endl;	

	std::cout << "Organgizing...";
	
	std::cout << std::endl;
	curr1 -> simplify();
	curr1 -> print();
	std::cout << std::endl;
	//	curr2 -> simplify();
	std::cout << "finished simplify";
	curr2 -> print();
	std::cout << (*curr1 == 0) << std::endl;
	
	std::cout << "Unit Tests" << std::endl;
	std::string zero = "0x";
	std::cout << zero;

	std::cout << (funk(zero) == 0);
	//simplify into one function?
	//curr1 -> simplify();
	//curr1 -> degOrg();

	//curr2 -> simplify();
	//curr2 -> degOrg();

	//funk socks;
	//socks = GCD( *curr1, *curr2);
	
	//socks.print();
	std::	cout << std::endl;*/
	
  return 0;
}
