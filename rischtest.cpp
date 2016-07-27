#include "risch.h"
#include <string>

int main(){
	
	std::string sa = "x^2 + 7*x + -6";
	std::string sb = "x^2 + -5*x + -6";

	std::unique_ptr<funk> curr1;
	curr1 = string_to_funk(sa);

	std::unique_ptr<funk> curr2;
	curr2 = string_to_funk(sb);
	
	curr1.print();
	std::cout << std::endl;
	curr2.print();
	std::cout << std::endl;	

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
