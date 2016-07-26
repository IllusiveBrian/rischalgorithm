#ifndef _risch
#define _risch

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <utility>

enum class type { broken, variable, scalar, addition, multiply, divide, cosine, sine, logarithm, exponential};

class funk{
 public:

	funk();
	funk(const funk &obj);
	//funk(funk&& obj);	

	~funk();

	funk& operator=(const funk& obj);
	//funk& operator=(const funk&& obj);

	//operators needed:
	funk& operator+(const funk& obj);	
	funk& operator*(const funk& obj);
	funk& operator/(const funk& obj);	

	//need some way to incoroprate functions like sin, cos, tan.
	//need some way to find neccisary functions...


	
	void reduce();
	
	int deg();
	int coeff();
	int lcoeff();
	funk lt();
	int cont();
	void pp();	
	void print();
	///organize function tools
	void supersimplify();
	void simplify();
	void intoReady();
	void degOrg();
	void degOrg(char c);
	
	//holds protostate during parsing
	std::string pstring;

	type state;
	char var;
	int sca;
	int expo;	

	std::vector <funk*> add;
	std::vector <funk*> mul;
	funk* num;
	funk* den;

	funk* sin;
	funk* cos;
	funk* log;
	funk* exp;
};

funk* string_to_funk(std::string);

funk GCD(funk, funk);

#endif
