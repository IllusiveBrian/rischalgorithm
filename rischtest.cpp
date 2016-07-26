#include "risch.h"

//using namespace std;

int main(){
	
	string sa = "x^2 + 7*x + -6";
	string sb = "x^2 + -5*x + -6";

	funk * curr1;
	curr1 = string_to_funk(sa);

	funk * curr2;
	curr2 = string_to_funk(sb);
	
	//simplify into one function?
	curr1 -> simplify();
	curr1 -> degOrg();

	curr2 -> simplify();
	curr2 -> degOrg();

	funk socks;
	socks = GCD( *curr1, *curr2);
	
	socks.print();
	cout << endl;
	
	return 0;
}
