#ifndef _risch
#define _risch

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <utility>

enum class type { broken, base, addition, multiply, divide, cosine, sine, logarithm, exponential};

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
	int coef;
	int expo;	

	std::unique_ptr<funk> nodeA;
	std::unique_ptr<funk> nodeB;
	
	//divide numer A / denom B
	//sinngle nodes go to A
	
};

std::unique_ptr<funk> string_to_funk(std::string);

funk GCD(funk, funk);

#endif
