#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <utility>
#include <memory>
#include "risch.h"

using namespace std;

funk::funk(void){
	state = type::broken;
	expo = 1;
	coef = 1;
}

funk::funk(int i){
	state = type::broken;
	expo = 0;
	coef = i;
}

funk::funk(const funk &obj){
	
	state = obj.state;
	coef = obj.coef;
	expo = obj.expo;
	
	nodeA = obj.nodeA ? std::unique_ptr<funk>(new funk(*obj.nodeA)) : std::unique_ptr<funk>(nullptr);
	nodeB = obj.nodeB ? std::unique_ptr<funk>(new funk(*obj.nodeB)) : std::unique_ptr<funk>(nullptr);

}

funk::funk(const std::string& str){
  this->moveTo(*(string_to_funk(str).release()));
}

funk::funk(funk&& obj){
  this->moveTo(obj);
}

void funk::moveTo(funk& obj){
  this->state = obj.state;
  this->coef = obj.coef;
  this->expo = obj.expo;
  this->nodeA = std::move(obj.nodeA);
  this->nodeB = std::move(obj.nodeB);
}

/*

void funk::reduce(){
	std::unique_ptr<funk> temp;
	switch (state)
	{
		case type::addition:
		  temp = std::move(add.at(0));
					break;	
		case type::multiply:
		  temp = std::move(mul.at(0));
					break;
		case type::divide:
		  temp = std::move(num);
					break;
		default: return;
	}
	*this = std::move(*temp);
	delete temp;

	type tempstate = temp -> state;	
	var = temp -> var;
	sca = temp -> sca;
	expo = temp -> expo;
	
	
	while (!temp -> add.empty()){
	  add.push_back(std::move(temp -> add.back()));	
		temp -> add.pop_back();
	}
	while (!temp -> mul.empty()){
	  mul.push_back(std::move(temp -> mul.back()));
		temp -> mul.pop_back();
	}
	
	if (state != type::divide){
	  num = std::move(temp -> num);
	  den = std::move(temp -> den);
	}
	sin = std::move(temp -> sin);
	cos = std::move(temp -> cos);
	log = std::move(temp -> log);
	exp = std::move(temp -> exp);
 
	switch (state)
	{
		case type::addition:
					add.erase(add.begin());	
					break;
		case type::multiply:
					mul.erase(mul.begin());
					break;
		case type::divide:
		  den = std::move(temp -> den);
		  num = std::move(temp -> num);	
	}
	
	state = tempstate;
}	

*/
funk& funk::operator=(const funk& obj){
	if (this != &obj)
	{
	  this->replaceWith(obj);
	}
	return *this;
}

funk& funk::operator=(funk&& obj){
  this->moveTo(obj);
  return *this;
}

funk& funk::operator=(const int& constant) //Assign a funk to be a constant
{
  this->expo = 0;
  this->coef = constant;
  this->state = type::base;
  this->nodeA.reset(nullptr);
  this->nodeB.reset(nullptr);
  return *this;
}

void funk::replaceWith(const funk& obj){
  this->state = obj.state;
  this->coef = obj.coef;
  this->expo = obj.expo;
  this->nodeA.reset(new funk(*obj.nodeA));
  this->nodeB.reset(new funk(*obj.nodeB));
}

void funk::replaceWith(const std::unique_ptr<funk>& obj)
{
  this->replaceWith(*obj);
}

/*

//currently, deg is broken since the degree isnt compared against multivariates
int funk::deg(){
	switch (state){
		case type::multiply:
		{
			int save = 0;
			for (int i = 0; i < mul.size(); i++){
				if (mul[i] -> state == type::variable){
					if (mul[i] -> expo > mul[save] -> expo){
						save = i;
					}
				}
			}
			return save;
			break;
		}
		case type::addition:
		{
			return add[0] ->  deg();
			break;
		}
		default:
		{
			return 0;
			break;
		}
	}
}

bool degCompare(const std::unique_ptr<funk>& a, const std::unique_ptr<funk>& b){
	
	if (a -> expo > b -> expo){
		return a -> expo > b -> expo;
	}

	switch (a -> state){
		case type::scalar:
			return a -> sca > b -> sca;	
			break;
		case type::variable:
			return a -> var > b -> var;
			break;
		case type::addition:
			{
				for (int i = 0; i < a -> add.size() && i < b -> add.size(); i++){
					if (compareA(*(a -> add[i])) == compareA(*(b -> add[i]))){
						if (degCompare(a -> add[i], b -> add[i])){
							return degCompare(a -> add[i], b -> add[i]);
						}			
					}else{
						return compareA(*(a -> add[i])) > compareA(*(b -> add[i]));
					}
					return a -> add.size() > b -> add.size(); 
				}
				break;
			}
		case type::multiply:
		{
				for (int i = 0; i < a -> mul.size() && i < b -> mul.size(); i++){
					if (compareA(*(a -> mul[i])) == compareA(*(b -> mul[i]))){
						if (degCompare(a -> mul[i], b -> mul[i])){
							return degCompare(a -> mul[i], b -> mul[i]);
						}
					}else{
						return compareM(*(a -> mul[i])) > compareM(*(b -> mul[i]));
					}
					return a -> mul.size() > b -> mul.size();
				}
				break;
			}	
		case type::divide:
			if (!(compareM(*(a -> num)) == compareM(*(b -> num)))){
				return compareM(*(a -> num)) > compareM(*(b -> num));
			}else{
				if (degCompare(a -> num, b -> num) ){
					return degCompare(a -> num, b -> num);	
				}else{  
					if (!(compareM(*(a -> den)) == compareM(*(b -> den)))){
					return compareM(*(a -> den)) > compareM(*(b -> den));
					}
					return degCompare(a -> den, b -> den);
				}
			}
			return false;
			break;


		case type::cosine:
			if (!(compareM(*(a -> cos)) == compareM(*(b -> cos)))){
				return compareM(*(a -> cos)) > compareM(*(b -> cos));
			}
			return degCompare(a -> cos, b -> cos);
			break;
		case type::sine:
			if (!(compareM(*(a -> sin)) == compareM(*(b -> sin)))){
				return compareM(*(a -> sin)) > compareM(*(b -> sin));
			}
			return degCompare(a -> sin, b -> sin);
			break;
		case type::logarithm:
			if (!(compareM(*(a -> log)) == compareM(*(b -> log)))){
				return compareM(*(a -> log)) > compareM(*(b -> log));
			}
			return degCompare(a -> log, b -> log);
			break;
		case type::exponential:
			if (!(compareM(*(a -> exp)) == compareM(*(b -> exp)))){
				return compareM(*(a -> exp)) > compareM(*(b -> exp));
			}
			return degCompare(a -> exp, b -> exp);
			break;
	}
}
*/

void funk::print(){
	if (coef != 1){
		cout << coef;
	}

	switch (state)
	{
		case type::base: 	
					if (expo != 0) cout << "x" ;
					break;
		case type::addition:			
					cout << "(";
					nodeA -> print();
					cout << "+";
					nodeB -> print();			
					cout << ")";
					break;
		case  type::multiply: 
					cout << "(";
					nodeA -> print();
					cout << "*";
					nodeB -> print();
					cout << ")";
					break;
		case  type::divide:
					cout << "(";
					nodeA -> print();
					cout << ")/(";
					nodeB -> print();
					cout << ")";
					break;
		
		case  type::cosine:
					cout << "cos(";
					nodeA -> print();
					cout << ")";
					break;
		case  type::sine:
					cout << "sin(";
					nodeA -> print();
					cout << ")";
					break;
		case  type::logarithm:
					cout << "log(";
					nodeA -> print();
					cout << ")";
					break;
		case type::exponential:
					cout << "exp(";
					nodeA -> print();
					cout << ")";
					break;
	
		default:		cout << "This should never occur also IB was here";
	}
	if (expo > 1) cout << "^" << expo ;
}

//Returns whether the function is constant, ie whether all greatest grandchildren are constants
bool funk::isConstant(){
  return isInteger() || ((nodeA ? nodeA->isConstant() : true) && (nodeB ? nodeB->isConstant(): true));
}

//Returns whether this particular funk is a constant (checks independent of type)
bool funk::isInteger()
{
  return expo == 0 || isZero();
}

bool funk::isZero()
{
  return coef == 0;
}

void funk::breakExpo(){
  if (this->expo > 1){
    unique_ptr<funk> tempMult, tempRepA(new funk(*this)), tempRepB(new funk(*this));
    tempMult->state = type::multiply;
    tempMult->expo = 1;
    tempMult->coef = 1;
    
    tempRepA->expo = 1;
    tempRepB->expo = this->expo - 1;
    tempMult->nodeA = std::move(tempRepA);
    tempMult->nodeB = std::move(tempRepB);
    this->replaceWith(*tempMult);
    this->nodeB->breakExpo();
  }
}

void funk::evaluateCoef(int updatedCoef, std::unique_ptr<funk>& nodeToCopy)
{
  this->coef = updatedCoef;
  this->expo = nodeToCopy->expo;
  this->state = nodeToCopy->state;
  unique_ptr<funk> tempA = std::move(nodeToCopy->nodeA), tempB = std::move(nodeToCopy->nodeB);//I don't know whether nodeToCopy is nodeA or nodeB, so I need to do this before invalidating it in the next two lines
  this->nodeA = std::move(tempA);
  this->nodeB = std::move(tempB);
}

//call this if BOTH children are division in multiplication funk
void funk::multiplyDivisions()
{
  unique_ptr<funk> toswitch;
  toswitch->state = type::divide;
  toswitch->coef = this->nodeA->coef * this->nodeB->coef;
  toswitch->expo = 1;

  toswitch->nodeA.reset(new funk(*(this->nodeA->nodeA) * *(this->nodeB->nodeA)));
  toswitch->nodeB.reset(new funk(*(this->nodeA->nodeB) * *(this->nodeB->nodeB)));
  this->replaceWith(toswitch);
  this->simplify();
}

//Call this if EXACTLY ONE child is division in multiplication funk
void funk::upjumpDivision()
{
#define upjumpMacro(divNode, otherNode) (temp->nodeA->replaceWith(*(otherNode) * *this),	\
  temp->nodeB->replaceWith((divNode)->nodeB),\
  temp->state = type::divide,\
  temp->coef = divNode->coef,\
  temp->expo = divNode->expo)
  std::unique_ptr<funk> temp;
  this->nodeA->state == type::divide ? upjumpMacro(this->nodeA, this->nodeB) : upjumpMacro(this->nodeB, this->nodeA);
  this->replaceWith(temp);
  this->simplify();
}

void funk::distributeNodes(){
  std::unique_ptr<funk> temp;
  temp->nodeA->replaceWith(*(this->nodeA) * *(this->nodeB->nodeA));
  temp->nodeB->replaceWith(*(this->nodeA) * *(this->nodeB->nodeB));
  temp->state = type::addition;
  temp->coef = 1;
  temp->expo = 1;
  this->replaceWith(temp);
  this->simplify();
}

void funk::setBase()
{
  this->state = type::base;
  this->nodeA.reset(nullptr);
  this->nodeB.reset(nullptr);
}

//Only call these on objects with the correct state, or bad things will happen
void funk::simplifyAddition()
{
  if(nodeA->coef == 0){
    this->replaceWith(nodeB);
  }
  else if (nodeB->coef == 0){
    this->replaceWith(nodeA);
  }
  else if (nodeA->compareWithoutCoef(*nodeB)){
    evaluateCoef(nodeA->coef + nodeB->coef, nodeB);
  }
  
}

void funk::lazyAddDivide(){
  funk div1, div2;
  div1.state = type::divide;
  div2.state = type::divide;

  div1.nodeA = std::move(this->nodeA->nodeA); 
  div1.nodeB = std::move(this->nodeB);

  div2.nodeA = std::move(this->nodeA->nodeB);
  div2.nodeB = std::move(this->nodeB);

  this->replaceWith(div1 + div2);
}

void funk::expandToDivide(std::unique_ptr<funk>& node){    
  node->replaceWith(funk(1)/funk(std::move(*node)));
}

void funk::divXdiv(){//A rated XXX function
  std::unique_ptr<funk>ret(new funk());
  ret->state = type::divide;

//    A/B / C/D => D*A / B*C

  ret->nodeA->replaceWith(*(this->nodeA->nodeA) * *(this->nodeB->nodeB));
  ret->nodeB->replaceWith(*(this->nodeA->nodeB) * *(this->nodeB->nodeA));
  this->replaceWith(ret);
  this->simplify();
}

void funk::simplifyDivide()
{
  if(nodeA->coef == 0){
    this->replaceWith(nodeA);
    return;
  }
  if (nodeB->coef == 0){
    //an error has occured
  }
  if (*nodeB == 1) {
    this->replaceWith(nodeA);
    return;
  } 
  if (nodeA->statesAndNodesEqual(*nodeB)){
    int div = iGCD(nodeA->coef, nodeB->coef);
    nodeA->coef = nodeA->coef / div;
    nodeB->coef = nodeB->coef / div;

    if (nodeA->expo < nodeB->expo){
      nodeB -> expo -= nodeA->expo;
      nodeA -> expo = 0;
    }
    else{
      nodeA -> expo -= nodeB->expo;
      nodeB -> expo = 0;
    }
  }
  if (nodeA->state == type::addition){
    lazyAddDivide();
  }
  if (nodeA -> state == type::divide || nodeB -> state == type::divide){
    if(nodeA -> state != type::divide){
      expandToDivide(nodeA); 	 
    }
    if(nodeB -> state != type::divide){
      expandToDivide(nodeB);
    }
    divXdiv();
  }
}

void funk::simplifyMulitiplication()
{
  bool nodeADiv = nodeA->state == type::divide, nodeBDiv = nodeB->state == type::divide;

  if (nodeADiv || nodeBDiv){
    if (nodeADiv && nodeBDiv){
      multiplyDivisions();
    }
    else{
      upjumpDivision();
    }
  }
  else if (nodeA->state == type::addition || nodeB->state == type::addition){
    distributeNodes();
  }
  else if (nodeA->isInteger()){
    evaluateCoef(nodeA->coef * nodeB->coef, nodeB);
  }
  else if (nodeB->isInteger()){
    evaluateCoef(nodeA->coef * nodeB->coef, nodeA); 
  }
    
}

void funk::simplifyExponent(){
  if (nodeA->isZero()){
    *this = 1;
  }
  else{
    if(expo > 1){
      nodeA->coef *= expo;
      expo = 1;
    }
    if (nodeA->state == type::logarithm && nodeA->expo == 1){
      this->replaceWith(nodeA->nodeA);
    }
  }
}

void funk::simplifyLogarithm(){
  if (nodeA->isZero()){
    throw math_too_hard("Expression log(0) is not supported");
  }
  else if (*nodeA == 1){
    *this = 0;
  }
  else if (nodeA->state == type::exponential){
    this->replaceWith(nodeA->nodeA); //Note that this is not actually defined for negative values of x, but we don't check that
  }
}

/*int compare(funk a){
	switch(a.state){
		case type::addition:
			return 8;
			break;
		case type::multiply:
			return 7;
			break;
		case type::divide:
			return 6;
			break;
		case type::cosine:
			return 5;
			break;
		case type::sine:
			return 4;
			break;
		case type::logarithm:
			return 3;
			break;
		case type::exponential:
			return 2;
			break;
		case type::base:
			return 9;
			break;
	}
	}*/

void funk::organize(){
  if ((state == type::addition || state == type::multiply) && *nodeA < *nodeB) {
    std::swap(nodeA, nodeB);
  }	
}
	
void funk::simplify(){
  if (nodeA){
    nodeA->simplify();
  }
  if (nodeB){
    nodeB->simplify();
  }
  if (isInteger())
  {
    setBase();
  }
  switch (state){
  case type::base:
    break;
  case type::addition:
    breakExpo();
    simplifyAddition();
    break;
  case type::multiply:
    breakExpo();
    simplifyMulitiplication();
    break;
  case type::divide:
    breakExpo();
    simplifyDivide();
    break;
  case type::sine:
    if (this->nodeA->isZero())
      *this = 0;
    break;
  case type::cosine:
    if (this->nodeA->isZero())
      *this = 1;
    break;
  case type::exponential:
    simplifyExponent();
    break;
  case type::logarithm:
    simplifyLogarithm();
    break;
  case type::broken:
  default:
    throw std::domain_error("Funk was not assigned a type");
  
  }
  this -> organize();
  return;
}

bool funk::compareMathEquiv(const funk& obj){
  return *this == obj || (this->coef == 0 && obj.coef == 0) || (this->coef == obj.coef && this->expo == 0 && obj.expo == 0);
}

bool funk::statesAndNodesEqual(const funk& obj){
  return this->state == obj.state && (this->state == type::base ? true : this->nodeA == obj.nodeA && this->nodeB == obj.nodeB);
}

bool funk::compareWithoutCoef(const funk& obj){
  return this->expo == obj.expo && statesAndNodesEqual(obj);
}

bool funk::operator==(const funk& obj){
    return this->coef == obj.coef && compareWithoutCoef(obj);
}

bool funk::operator==(const int& constant){
  return (this->expo == 0 && this->coef == constant) || (constant == 0 && isZero());
}

#define inequalityComparison(sign) (this->state sign obj.state) || (this->state == obj.state && this->expo sign obj.expo) || (this->expo == obj.expo && this->nodeA sign obj.nodeA) || (this->nodeA == obj.nodeA && this->nodeB sign obj.nodeB)
bool funk::operator<(const funk& obj)
{
  return inequalityComparison(<);
}

bool funk::operator>(const funk& obj)
{
  return inequalityComparison(>);
}

funk funk::operator+(const funk& obj){ //Might need to return a reference
	funk ret;				
	ret.state = type::addition;
	
	ret.nodeA.reset(new funk(*this));
	ret.nodeB.reset(new funk(obj));
	ret.simplify();
        			
	return ret;
}

funk funk::operator*(const funk& obj){
	funk ret;
	ret.state = type::multiply;
	ret.nodeA.reset(new funk(*this));
	ret.nodeA.reset(new funk(obj));

	ret.simplify();
	return ret;
}

funk funk::operator/(const funk& obj){
  // std::unique_ptr<funk> objcopy(new funk);
  // 	* objcopy = obj;
  // 	std::unique_ptr<funk> thiscopy(new funk);
  // 	* thiscopy = *this;
	funk ret;
	ret.state = type::divide;

	ret.nodeA.reset(new funk(*this));
	ret.nodeB.reset(new funk(obj));
	
	ret.simplify();
	
	return ret;
}


/*
funk fcoeff(funk curr, char c ){
	if (curr.state == type::multiply){
		int check = -1;
		for (int j = 0; j < curr.mul.size(); j++){
			if (curr.mul[j] -> state == type::variable && curr.mul[j] -> var == c){
				check = j;	
			}	
		}
		if (check != -1){
			
		}
	}
	if (curr.state != type::variable && curr.var != c){
		
	}
}

int funk::coeff(){
	int var = 0;
	if (state == type::multiply){
		if (mul.back() ->  state == type::scalar)
			var = mul.back() -> sca;
	}
	if (state == type::scalar){
		var = sca;
	}	
	if (var == 0) var = 1;
	return var;		
}

int funk::lcoeff(){
	if (state == type::addition) return add[0] -> coeff();
	return coeff();
	
}

funk funk::lt(){
	if (state == type::addition) return *(add[0]);
	else return *this;
}

bool aDevidesb( funk a, funk b){
	if (a.state == type::scalar && b.state == type::multiply) return false;
	if (a.state == type::multiply && a.mul.size() == 0) return false;
	if (a.state == type::scalar) return true;
	if (b.state == type::scalar && a.state != type::scalar) return false;
	
	for (int i = 0; i < b.mul.size(); i++){
		if (b.mul[i] -> state == type::variable){
			for (int j = 0; j < a.mul.size(); j++){
				if (a.mul[j] -> state == type::variable){
					if  (a.mul[j] -> var == b.mul[i] -> var){
						 if (a.mul[j] -> expo > b.mul[i] -> expo) return false;	
					}else{
						return false;
					}
				}
			}
		}
	}
	return true;
}
*/
int iGCD(int a, int b){
	//integer GCD
	
	int c = abs(a);
	int d = abs(b);

	int r = 0;
	while (d!= 0){
		r = c % d;
		c = d;
		d = r;	
	}
	return abs(c);
}
/*
int funk::cont(){
	if (add.size() < 1) return 0;
	else if (add.size() < 2) return lcoeff();
	else{
		int lastGCD = lcoeff();
		for (int i = 1; i < add.size(); i++){
			lastGCD = iGCD(lastGCD, add[i] -> coeff());
		}
		return lastGCD;
	}
}

void funk::pp(){
	int c = cont();
	if (cont() == 0)  return;
	else if (cont() == 1) return;
	else{
		for (int i = 0; i < add.size(); i++){
			if (add[i] -> state == type::scalar) add[i] -> sca = (add[i] -> sca) /c;
			if (add[i] -> state == type::multiply){
				for (int j = 0; j < add[i] -> mul.size(); j++){
					if (add[i] -> mul[j] -> state == type::scalar) add[i] -> mul[j] -> sca = (add[i] -> mul[j] -> sca) /c;	
				}
			}
		}
	}
}

//due to its highly requested nature, polynomial division will result in a pair 
// representation ( quo(a,b), rem(a,b))

//readers note*
//in theory this would be alot easier to do with / operator but I'll burn before
//I master using operators.

//This code was deeply inspired by the Multivariate Division with remainder 
//Algorithm found in "reduction Modulo Ideals and Multivariate Polynomial
//Interpolation" -by Vo Ngoc Thieu (<3 Hubbubble)

pair <funk, funk> pdiv( funk a, funk b){
	
	if (b.state == type::scalar && b.sca == 1){	
		std::unique_ptr<funk> zip = new funk;
		zip -> state = type::scalar;
		zip -> sca = 0;

		return make_pair(a, *zip);	
	}
	
	funk p;
	std::unique_ptr<funk> r(new funk);
	r -> state = type::scalar;
	r -> sca = 0;

	std::unique_ptr<funk> q(new funk);
	* q = * r;

	p = a;
	cout << "Print p: ";
	p.print();
	cout << endl;
	do {
		if (aDevidesb(b.lt() , p.lt())){
				
			p.lt().print();
			cout << " / ";
			b.lt().print();
			cout << " = ";
		
			std::unique_ptr<funk> qtemp(new funk);
			* qtemp = p.lt() / b.lt();

			qtemp -> print(); 
			cout << endl;
			
			* q = * q + *qtemp;

			std::unique_ptr<funk> none(new funk);
			none -> state = type::scalar;
			none -> sca = -1;

			std::unique_ptr<funk> ptemp(new funk);
			* ptemp = p.lt() / b.lt();

			* ptemp = (*none) * (*ptemp);
			* ptemp = *ptemp * b;
					
			p = p + *ptemp;

		}else{
			*r = *r + p.lt();
				
			std::unique_ptr<funk> negone(new funk);
			negone -> state = type::scalar;
			negone -> sca = -1;
			
			*negone = *negone * p.lt() ;
			
			p = p + *negone;	
			
			}
				
	} while (!(p.state == type::scalar && p.sca == 0));
	
	q -> intoReady();
	r -> intoReady();
	return make_pair( *q, *r );
}

//while it was fun to live in the dream world, it was too hard to call pdiv while
//understanding what part of the pair I actually wanted.
//plus, the notation in the book makes copy+paste ezpz

funk quo(funk a, funk b){
	return pdiv(a,b).first;
}

funk rem(funk a, funk b){
	return pdiv(a,b).second;
}

// from wikipedia (<3 Hubbubble)
funk GCD(funk a, funk b){
  std::unique_ptr<funk> c (new funk);
	funk d;

	*c = a;
	d = b;
	c -> pp();
	d.pp();
	funk r;
	int i = 0;
	while ( !(d.state == type::scalar && (d.sca == 0||d.sca == 1)) ){
		cout << i << ":r(x) "; r.print(); cout << endl;
		cout << i << ":c(x) "; c -> print(); cout << endl;
		cout << i << ":d(x) "; d.print(); cout << endl;

		c -> intoReady();
		d.intoReady(); 		
		r = rem( *c, d );
		*c = d;
		r.pp();
		d = r;
		i++;

		cout << i << ":2r(x) "; r.print(); cout << endl;
		cout << i << ":2c(x) "; c -> print(); cout << endl;
		cout << i << ":2d(x) "; d.print(); cout << endl;
	}

	std::unique_ptr<funk> g(new funk);
	g -> state = type::multiply;
	std::unique_ptr<funk> y(new funk);
	y -> state = type::scalar;
	y -> sca = iGCD( a.cont(), b.cont());
	
	g -> mul.push_back(std::move(y));
	g -> mul.push_back(std::move(c));
	g -> intoReady();
	return *g;
}

bool contains( funk curr, char c){
	bool ret = false;
	if (curr.state == type::variable){
		if (curr.var == c) return true;
	}
	switch (curr.state){
		case type::multiply: 
			{
				for(int i = 0; i< curr.mul.size(); i++){
					if(!ret) ret = contains(*curr.mul[i], c);
				}
				break;
			}
		case type::addition:
			{
				for(int i = 0; i < curr.add.size(); i++){
					if(!ret) ret = contains(*curr.add[i], c);
				}
				break;
			}
		case type::divide:
			{
				if(!ret) ret = contains(*curr.num, c);
				if(!ret) ret = contains(*curr.den, c);
				break;
			}
		case type::cosine:
				if(!ret) ret = contains(*curr.cos, c);
				break;
		case type::sine:
				if(!ret) ret = contains(*curr.sin, c);
				break;
		case type::exponential:
				if(!ret) ret = contains(*curr.exp, c);
				break;
		case type::logarithm:
				if(!ret) ret = contains(*curr.log, c);
				break;
	}
	return ret;
}



funk derive(funk curr, char c);

funk derivehandle(funk curr, char c){
	if (!contains(curr, c)){
	  std::unique_ptr<funk> zip(new funk);
		zip -> state = type::scalar;
		zip -> sca = 0;
		return *zip;
	}

	if (curr.expo > 1){
	  std::unique_ptr<funk> expand(new funk);
		expand -> state = type::multiply;

		for (int i = 0; i < curr.expo; i++){
		  std::unique_ptr<funk> currcopy(new funk);
			*currcopy = curr;
			expand -> mul.push_back(std::move(currcopy));
			expand -> mul.back() -> expo = 1;
		}
		return derive(*expand, c);
	}

	switch (curr.state){
		case type::variable:
			{
				if (curr.var == c){
				  std::unique_ptr<funk> one(new funk);
					one -> state = type::scalar;
					one -> sca = 1;
					return *one;
				}
			}
		case type::addition:
			for (int i = 0; i < curr.add.size(); i++){
				// (a+b)' = a' + b'
				*curr.add[i] = derive(*curr.add[i], c);	
			}
			return curr;
			break;
		case type::multiply:
			while (	curr.mul.size() > 1){
				// (a*b)' = a*b' + b*a'

			  std::unique_ptr<funk> aprime(new funk);
			  std::unique_ptr<funk> a(new funk);
			  std::unique_ptr<funk> bprime(new funk);
			  std::unique_ptr<funk> b(new funk);
				
				*aprime = derive(*curr.mul[0], c);
				*a = *curr.mul[0];
				*bprime = derive(*curr.mul[1], c);
				*b = *curr.mul[1];


				std::unique_ptr<funk> mula(new funk);
				mula -> state = type::multiply;
				mula -> mul.push_back(std::move(a));
				mula -> mul.push_back(std::move(bprime));

				std::unique_ptr<funk> mulb(new funk);
				mulb -> state = type::multiply;
				mulb -> mul.push_back(std::move(b));
				mulb -> mul.push_back(std::move(aprime));

				std::unique_ptr<funk> addi(new funk);
				addi -> state = type::addition;
				addi -> add.push_back(std::move(mula));
				addi -> add.push_back(std::move(mulb));
				
				curr.mul.push_back(std::move(addi));
				curr.mul.erase(curr.mul.begin() + 1);
				curr.mul.erase(curr.mul.begin());
			}
			return curr;
			break;	
		//(a(b))' = a'(b)*b'
		case type::sine:
		{
			curr.state = type::cosine;
			curr.cos = std::move(curr.sin);
			curr.sin.reset(nullptr);
	
			std::unique_ptr<funk> newcos(new funk);
			*newcos = curr;

			std::unique_ptr<funk> b(new funk);
			*b = derive(*curr.cos, c);
			
			std::unique_ptr<funk>  group(new funk);
			group -> state = type::multiply;

			group -> mul.push_back(std::move(newcos));
			group -> mul.push_back(std::move(b));

			return *group;
			break;
		}	
		case type::cosine:
		{
			curr.state = type::sine;
			curr.sin = std::move(curr.cos);
			curr.cos.reset(nullptr);

			std::unique_ptr<funk> newcos(new funk);
			*newcos = curr;

			std::unique_ptr<funk> mone(new funk);
			newcos -> state = type::scalar;
			newcos -> sca = -1;
			
			std::unique_ptr<funk> b(new funk);
			*b = derive(*curr.sin, c);
			
			std::unique_ptr<funk>  group(new funk);
			group -> state = type::multiply;

			group -> mul.push_back(std::move(mone));
			group -> mul.push_back(std::move(newcos));
			group -> mul.push_back(std::move(b));

			return *group;	
			break;
		}
		case type::logarithm:
		{	
		  std::unique_ptr<funk> incopy(new funk);
			*incopy = *curr.log;
			
			std::unique_ptr<funk> one(new funk);
			one -> state = type::scalar;
			one -> sca = 1;
			
			std::unique_ptr<funk> newlog(new funk);
			newlog -> state = type::divide;
			newlog -> num = std::move(one);
			newlog -> den = std::move(incopy);

			std::unique_ptr<funk> b(new funk);
			*b = derive(*curr.log, c);
			
			std::unique_ptr<funk>  group(new funk);
			group -> state = type::multiply;

			group -> mul.push_back(std::move(newlog));
			group -> mul.push_back(std::move(b));

			return *group;	
			break;
		}
		case type::exponential:
			return curr; //I love d/dx e^x
			break;
	}
}

funk derive(funk curr, char c){
  std::unique_ptr<funk> ret(new funk);
	*ret = derivehandle(curr, c);
	ret -> intoReady();
	return *ret;
}

funk table( funk curr, char x){
	//THE TAAAAAAAAAAABBBLLLLLLLLLLLLLLLEEEEEEEEEEEEEEEE
	//scalar protection
	if (curr.state == type::variable){
		int expo2 = curr.expo + 1;
		
		std::unique_ptr<funk> one(new funk);
		one -> state = type::scalar;
		one -> sca = 1;

		std::unique_ptr<funk> under(new funk);
		under -> state = type::scalar;
		under -> sca = expo2;

		std::unique_ptr<funk> coeff(new funk);
		coeff -> state = type::divide;
		coeff -> num = std::move(one);
		coeff -> den = std::move(under);

		std::unique_ptr<funk> curr2(new funk);
		* curr2 = curr;
		curr2 -> expo = expo2;
		
		std::unique_ptr<funk> vot(new funk);
		vot -> state = type::multiply;
		vot -> mul.push_back(std::move(curr2));
		vot -> mul.push_back(std::move(coeff));

		return *vot;
	}if (curr.state == type::divide){
		if (curr.num -> state == type::scalar && curr.den -> state == type::variable){
			if (curr.den -> expo > 1){ 
				int expo2 = curr.den -> expo - 1;

				std::unique_ptr<funk> one(new funk);
				one -> state = type::scalar;
				one -> sca = -1;

				std::unique_ptr<funk> under(new funk);
				under -> state = type::scalar;
				under -> sca = expo2;

				std::unique_ptr<funk> coeff(new funk);
				coeff -> state = type::divide;
				coeff -> num = std::move(one);
				coeff -> den = std::move(under);

				std::unique_ptr<funk> curr2(new funk);
				* curr2 = curr;
				curr2 -> den -> expo = expo2;
				
				std::unique_ptr<funk> vot(new funk);
				vot -> state = type::multiply;
				vot -> mul.push_back(std::move(curr2));
				vot -> mul.push_back(std::move(coeff));

				return *vot;
			}
			else{
			  std::unique_ptr<funk> lnin(new funk);
				*lnin = *curr.den;

				std::unique_ptr<funk> ln(new funk);
				ln -> state = type::logarithm;
				ln -> log = std::move(lnin);

				return * ln;
			}
		}
	}
	
	return curr;
}

bool hasvar(funk curr, char x){

		
	if(curr.state == type::addition){
		bool parse = false;
		for (int i = 0; i < curr.add.size(); i++){
			if (hasvar(*curr.add[i], x)){
				parse = true;
			}
		}
		return parse;
	}
	else if(curr.state == type::multiply){
		bool parse = false;
		for (int i = 0; i < curr.mul.size(); i++){
			if(hasvar(*curr.mul[i], x)){
				parse = true;
			}
		}
		return parse;
	}
	else if(curr.state ==  type::divide){
		return (hasvar(*curr.num, x) || hasvar(*curr.den, x));
	}
	else if(curr.state == type::cosine){
		return (hasvar(*curr.cos, x));	
	}
	else if(curr.state == type::sine){
		return (hasvar(*curr.sin, x));
	}
	else if (curr.state == type::exponential){
		return (hasvar(*curr.exp, x));
	}
	else if (curr.state ==  type::logarithm){
		return (hasvar(*curr.log, x));
	}
	else if(curr.state == type::variable && curr.var == x){
		return true;
	}
	else{
		return false;
	}
}

funk integrate(funk curr, char x){
	if(!hasvar(curr, x)){
	  std::unique_ptr<funk> plusx(new funk);
		plusx -> state = type::multiply;

		std::unique_ptr<funk> xp(new funk);
		xp -> state = type::variable;
		xp -> var = x; 

		std::unique_ptr<funk> curry(new funk);
		* curry = curr;

		plusx -> mul.push_back(std::move(curry));
		plusx -> mul.push_back(std::move(xp));

		return *plusx;
	}
	if(curr.state == type::addition){
	  std::unique_ptr<funk> newf(new funk);
		newf -> state = type::addition;
		for ( int i = 0; i <  curr.add.size(); i++){
		  std::unique_ptr<funk> vot(new funk);
			*vot = integrate (*curr.add[i], x);
			newf -> add.push_back(std::move(vot));
		}
		return *newf;
	}
	else if(curr.state == type::multiply && curr.mul.back() -> state == type::scalar){
		
	  std::unique_ptr<funk> newf(new funk);
		newf -> state = type::multiply;
		
		std::unique_ptr<funk> temp(new funk);
		temp = std::move(curr.mul.back());
	
		curr.mul.pop_back();
		curr.simplify();
		curr.degOrg();

		std::unique_ptr<funk> vot(new funk);
		* vot = integrate(curr, x);

		newf -> mul.push_back(std::move(vot));
		newf -> mul.push_back(std::move(temp));
		return *newf;
	}
	//elseif hermite -> rothgar -> risch
	else{
		return table(curr, x);
	}
}
funk sqff (funk a, char x){
	
	int i = 1;
	
	std::unique_ptr<funk> one(new funk);
	one -> state = type::scalar;
	one -> sca = 1;
	
	std::unique_ptr<funk> output(new funk);
	output -> state = type::multiply;
	output -> mul.push_back(std::move(one));

	funk b = derive(a, x);

	std::unique_ptr<funk> c(new funk);
	*c =  GCD(a, b);
	
	return *c;
	std::unique_ptr<funk> acopy(new funk);
	*acopy = a;

	std::unique_ptr<funk> w(new funk);
	w -> state = type::divide;
	w -> num.reset(new funk(a));
	w -> den = std::move(c);


	std::unique_ptr<funk> y(new funk);
	std::unique_ptr<funk> z(new funk);

	while (!(c -> state == type::scalar && (c -> sca == 1))){
		*y = GCD(*w, *c);
		*z = *w / *y;
		
		z -> expo = i;
		output -> mul.push_back(std::move(z)); 
		i++;
		
		*w = *y;
		std::unique_ptr<funk> ccopy(new funk);
		*ccopy = *c;
		*c = *c / *y;
	} 

	w -> expo = i;
	output -> mul.push_back(std::move(w));
	
	return *output;
}

funk rothtrag( funk a, funk b, char x){
	//PP of poly
	a.pp();
	funk R = a;
	int k = R.add.size();
	
	std::unique_ptr<funk> c(new funk);
	c -> state = type::addition;

	std::unique_ptr<funk> v(new funk);
	v -> state = type::addition;
	
	std::unique_ptr<funk> integral(new funk);
	integral -> state = type::addition;
	
	std::unique_ptr<funk> logpart(new funk);
	logpart -> state = type::logarithm;

	if (R.den -> expo > R.num -> expo){
	  std::unique_ptr<funk> integral(new funk);
		integral -> state = type::addition;
		
		for (int i = 0; i < k; i++){
			int d = R.expo; 
			if (d == 1){
				c -> simplify();
				integral -> add.push_back(std::move(logpart));
			}
			else if (d == 2){
			 	c -> simplify();
				for (int n = 1; n < 2; n++){
				  logpart -> log = std::move(c);
				  integral -> add.push_back(std::move(logpart));
				}
			}else{
				for (int n = 1; n < d; n++){
				  logpart -> log = std::move(c);
				  integral -> add.push_back(std::move(logpart));
				}
			}
		}
	}
	return (*integral);
}
funk horowitz(){
	funk* poly = new funk;
	poly -> state = type::addition;
	
	simplify();

	poly.push_back(den);
	
	funk* a = new funk;
	a -> state = type::addition;
	funk* b = new funk;
	b -> state = type::addition;
	funk* c = new funk;
	c -> state = type::addition;
	funk* d = new funk;
	d -> state = type::addition;
	
	int n;
	int m;
	
	d = num;
	b = num;
	n = b -> expo;
	m = d -> expo;

	for(i = 0; i < n-1; i++){
		a -> add.push_back(n)
	}
	for(i = 0; i < m-1; i++){
		c -> add.push_back(m);
	}
	
	std::unique_ptr<funk> r = new funk;
	r -> state = type::addition;
	r -> add.at(c);
	r -> add.at(b);
	
	double sum = 0;
	for (int i = 0; i < m + n -1; i++){
		double temp add.at(i) -> mul.at(0); 
		temp = sum;
		//solve for these euations
	}
	
	std::unique_ptr<funk> front = new funk;
	front -> state = type::divide;
	num = c;
	den = d;

	std::unique_ptr<funk> back = new funk;
	back -> state = type::divide;
	num = a;
	den = b;	

	std::unique_ptr<funk> sender = new funk;
	front -> state = type::addition;

	sender ->  add.push_back(front);

	for (int i = 0; i < add.size(); i++){
		sender -> add.push_back(integrate(add.at(i)));
	}
	
	sender -> add.push_back(integrate(back));
	
	return(sender);
}
pair <funk, funk> solve(funk a, funk b, funk c ){
	//create variable matrix
	int n = a.deg();
	int m = b.deg();

	//Matrix2d A = 
//	Martix2d C = Matrix2d::Constant((2*(n-1)), 1);
	
//	for (int i = 0; i < n; i++){
//		C(i, 0) = coeff();
//	}
	//Matrix2d B = ;
	//
	//

} 

funk hermite(funk p, funk q, char x){
	//determine the polynomial part of an integral, rational part and integral part, and send them packing
	
	s(x^2 + 3x + 2) + t(3x + 6) = 9x + 10


	|1 0 : 3 0 0| |s0|	|10|
	|3 1 : 6 3 0| |s1| 	| 9|
	|2 3 : 0 6 3|x|t0| = 	| 0|
	|0 2 : 0 0 6| |t1|	| 0|

	//partial fraction decomposition
	funk* total = new funk;
	total -> state = type::addition;

	funk* ratpart = new funk;
	ratpart -> state = type::addition;

	funk* intpart = new funk;
	intpart -> state = type::addition;
	
	for (int i = 0; i < total -> add.size(); i++){
		intpart -> add.push_back (total -> add.at(0));
		for (int j = 2; j < i; j++){
			int n = j;
			while (n > 1) do {
				pair <funk, funk> st;
				//st = solve()
				n--;
				ratpart -> add.push_back(total -> add.at(s-t));
			} 
			intpart -> add.push_back(total -> add.at (0)  );
		}

		total -> add.push_back(horowitz(ratpart));
		
		for (int i = 0; i < add.size(); i++){
			sender -> add.push_back(integrate(add.at(i)));
		}

		total -> add.push_back(integrate(intpart));
		return total;
	}
}

funk integrate(funk curr, char x){
	funk* finished = new funk;
	finished -> state = type::addition;
	if (curr.state == type::addition){
		for (int i = 0; i < curr.add.size(); i++){
			std::unique_ptr<funk> ipart = new funk;
			*ipart = integrate(*(curr.add[i]), x);	
			finished -> add.push_back(ipart);
		}
	}
	
	while (curr.lcoeff() == Tintegrate(curr, x).lcoeff()){
		// curr = hermite(curr);
		
	}
}
*/
bool isOnlyDouble(const char* str) //code by Chris on SO (thanks, <3 Hubbubble)
{
    char* endptr = 0;
    strtod(str, &endptr);

    if(*endptr != '\0' || endptr == str)
        return false;
    return true;
}

size_t ignore_parens(size_t i, string str){
	size_t pcounter = 1;
	while (pcounter != 0){
		i++;
		if (str.at(i) == '(') pcounter++;
	if (str.at(i) == ')') pcounter--;	
	}
	if (i+1 == str.size()) return i;
	else return i+1;
}

std::unique_ptr<funk> F2M(std::unique_ptr<funk>curr);

std::unique_ptr<funk> P2F(std::unique_ptr<funk>curr){
	curr -> pstring = curr -> pstring.substr(1, (curr -> pstring.size())-2);
	return F2M(std::move(curr));
}

std::unique_ptr<funk> X2C(std::unique_ptr<funk>curr){	
	if (curr -> pstring.at(0) == '('){
	  return P2F(std::move(curr));
	}
	
	if (curr -> pstring.size() > 2){
		if (curr -> pstring.compare(0,3, "cos") == 0){
		  std::unique_ptr<funk>temp(new funk);
			 
			temp -> pstring = curr -> pstring.substr(3, string::npos);
			curr -> nodeA = P2F(std::move(temp));
			curr -> state = type::cosine;
			curr -> pstring.clear();
			return curr;
		}
		if (curr -> pstring.compare(0,3, "sin") == 0){
		  std::unique_ptr<funk>temp(new funk);
			
			temp -> pstring = curr -> pstring.substr(3, string::npos);
			curr -> nodeA = P2F(std::move(temp));
			curr -> state = type::sine;
			curr -> pstring.clear();
			return curr;
		}
		if (curr -> pstring.compare(0,3, "log") == 0){
		  std::unique_ptr<funk>temp(new funk);

			temp -> pstring = curr -> pstring.substr(3, string::npos);
			curr -> nodeA = P2F(std::move(temp));
			curr -> state = type::logarithm;
			curr -> pstring.clear();
			return curr;
		}
		if (curr -> pstring.compare(0,3, "exp") == 0){
		  std::unique_ptr<funk>temp(new funk);
			 
			temp -> pstring = curr -> pstring.substr(3, string::npos);
			curr -> nodeA = P2F(std::move(temp));
			curr -> state = type::exponential;
			curr -> pstring.clear();
			return curr;
		}	
	}
	if (isOnlyDouble(curr -> pstring.c_str())){
		curr -> coef = stod(curr -> pstring);
		curr -> state = type::base;
		curr -> expo = 0;
	}else{
		curr -> state = type::base;	
	} 
	curr -> pstring.clear();
	return curr;
}

std::unique_ptr<funk> E2X(std::unique_ptr<funk> curr){
  for (size_t i = 0; i < curr -> pstring.size(); ++i){	
		if (curr -> pstring.at(i) == '(') i = ignore_parens(i, curr->pstring);	

		if (curr -> pstring.at(i) == '^'){
			curr -> expo = stoi( curr -> pstring.substr(i+1, string::npos ));
			curr -> pstring = curr -> pstring.substr (0, i);
			return X2C(std::move(curr));
		}
	}

	return X2C(std::move(curr));
}

std::unique_ptr<funk> M2E(std::unique_ptr<funk> curr){
	for (size_t i = 0; i < curr -> pstring.size(); ++i){
		if (curr -> pstring.at(i) == '(') i = ignore_parens(i, curr->pstring);	

		if (curr -> pstring.at(i) == '*'){
		  auto lh = std::unique_ptr<funk>(new funk);				
		  auto rh = std::unique_ptr<funk>(new funk);

			lh -> pstring = curr -> pstring.substr (0, i);
			rh -> pstring = curr -> pstring.substr (i+1, string::npos );	
			
			curr -> nodeA = E2X(std::move(lh));
			curr -> nodeB = M2E(std::move(rh));

			curr -> state = type::multiply;
			curr -> pstring.clear();
			
			return curr;
		}
		if (curr -> pstring.at(i) == '/'){
		  std::unique_ptr<funk> lh(new funk);				
		  std::unique_ptr<funk> rh(new funk);

			lh -> pstring = curr -> pstring.substr (0, i);
			rh -> pstring = curr -> pstring.substr (i+1, string::npos );	
			
			curr -> nodeA = E2X(std::move(lh));
			curr -> nodeB = M2E(std::move(rh));

			curr -> state = type::divide;
			curr -> pstring.clear();
			
			return curr;
		}
	}
	return E2X(std::move(curr));
}

std::unique_ptr<funk> F2M(std::unique_ptr<funk> curr){
	for (size_t i = 0; i < curr -> pstring.size(); ++i){
		if (curr -> pstring.at(i) == '(') i = ignore_parens(i, curr->pstring);	

		if (curr -> pstring.at(i) == '+'){
		  std::unique_ptr<funk> lh(new funk);				
		  std::unique_ptr<funk> rh(new funk);

			lh -> pstring = curr -> pstring.substr (0, (i));
			rh -> pstring = curr -> pstring.substr (i+1, string::npos );	
			
			curr -> nodeA = M2E(std::move(lh));
			curr -> nodeB = F2M(std::move(rh));

			curr -> state = type::addition;
			curr -> pstring.clear();
			
			return curr;
		}
	}
	return M2E(std::move(curr));
}

std::unique_ptr<funk> string_to_funk(const string& str){
  std::unique_ptr<funk> start(new funk);
  std::string in(str);
  in.erase(std::remove(in.begin(),in.end(),' '),in.end());
  start -> pstring = in;
  start = F2M(std::move(start));
  return start; 
}
