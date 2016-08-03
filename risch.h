#ifndef _risch
#define _risch

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <utility>

#include "customexceptions.cpp"

enum class type { broken, exponential, logarithm, cosine, sine, divide, multiply, addition, base };

class funk{
 public:

	funk();
	funk(int i);
	funk(const funk &obj);
	funk(const std::string&);
	funk(funk&& obj);	

        ~funk() = default;

	funk& operator=(const funk& obj);
	funk& operator=(funk&& obj);
	funk& operator=(const int&);

	funk operator+(const funk& obj);	
	funk operator*(const funk& obj);
	funk operator/(const funk& obj);	

	bool operator==(const funk&);
	bool operator==(const int&);
	bool operator<(const funk&);
	bool operator>(const funk&);
	bool statesAndNodesEqual(const funk& obj);
	bool compareWithoutCoef(const funk& obj);
	bool compareMathEquiv(const funk&);
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
	void organize();
	void degOrg();
	void degOrg(char c);
	bool isConstant();
	bool isInteger();
	bool isZero();
	
	//holds protostate during parsing
	std::string pstring;

	type state;
	int coef;
	int expo;	

	std::unique_ptr<funk> nodeA;
	std::unique_ptr<funk> nodeB;
	
	//divide numer A / denom B
	//sinngle nodes go to A

 private:
	void moveTo(funk&);
	void replaceWith(const funk&);
	void replaceWith(const std::unique_ptr<funk>&);
	void breakExpo();
	void evaluateCoef(int, std::unique_ptr<funk>&);
	void multiplyDivisions();
	void upjumpDivision();
	void distributeNodes();
	void lazyAddDivide();
	void expandToDivide(std::unique_ptr<funk>&);
	void divXdiv();
	void setBase();
	void simplifyAddition();
	void simplifyMulitiplication();
	void simplifyDivide();
	void simplifyExponent();
	void simplifyLogarithm();
	
};

std::unique_ptr<funk> string_to_funk(const std::string&);

funk GCD(funk, funk);
int iGCD(int, int);

#endif
