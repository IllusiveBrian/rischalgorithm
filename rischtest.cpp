#include "risch.h"
#include <string>

int main(){
	
	std::string sa = "x^2 + 7*x + -6";
	std::string sb = "x^2 + -5*x + 0";

	std::unique_ptr<funk> curr1;
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
	std::	cout << std::endl;
	
	return 0;
}
