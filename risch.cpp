#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <utility>
#include <memory>
#include "risch.h"

using namespace std;

funk::funk(void)
{
	state = type::broken;
	expo = 1;
	coef = 1;
}

funk::funk(int i)
{
  state = type::base;
	expo = 0;
	coef = i;
}

funk::funk(const funk &obj)
{
  state = obj.state;
  coef = obj.coef;
  expo = obj.expo;
	
  nodeA = obj.nodeA ? std::unique_ptr<funk>(new funk(*obj.nodeA)) : std::unique_ptr<funk>(nullptr);
  nodeB = obj.nodeB ? std::unique_ptr<funk>(new funk(*obj.nodeB)) : std::unique_ptr<funk>(nullptr);
}

funk::funk(const std::string& str)
{
  this->moveTo(*(string_to_funk(str).release()));
}

funk::funk(funk&& obj)
{
  this->moveTo(obj);
}

void funk::moveTo(funk& obj)
{
  this->state = obj.state;
  this->coef = obj.coef;
  this->expo = obj.expo;
  this->nodeA = std::move(obj.nodeA);
  this->nodeB = std::move(obj.nodeB);
}

funk& funk::operator=(const funk& obj)
{
	if (this != &obj)
	{
	  this->replaceWith(obj);
	}
	return *this;
}

funk& funk::operator=(funk&& obj)
{
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
  if (obj.nodeA){
    this->nodeA.reset(new funk(*obj.nodeA));
  }
  if (obj.nodeB){
    this->nodeB.reset(new funk(*obj.nodeB));
  }
}

void funk::replaceWith(const std::unique_ptr<funk>& obj)
{
  this->replaceWith(*obj);
}


std::ostream& operator<<(std::ostream& os, const funk& out){
  return os << out.build_print_string();
}

void funk::print() const{
  std::cout << build_print_string();
}

std::string funk::build_print_string() const{
  std::stringstream ret;
  if (*this == 1 || coef != 1){
    ret << coef;
  }

  switch (state)
    {
    case type::base: 	
      if (expo != 0 && !isZero())
	ret << "x" ;
      break;
    case type::addition:			
      ret << "(";
      ret << nodeA -> build_print_string();
      ret << "+";
      ret << nodeB -> build_print_string();
      ret << ")";
      break;
    case  type::multiply: 
      ret << "(";
      ret << nodeA -> build_print_string();
      ret << "*";
      ret << nodeB -> build_print_string();
      ret << ")";
      break;
    case  type::divide:
      ret << "(";
      ret << nodeA -> build_print_string();
      ret << ")/(";
      ret << nodeB -> build_print_string();
      ret << ")";
      break;
		
    case  type::cosine:
      ret << "cos(";
      ret << nodeA -> build_print_string();
      ret << ")";
      break;
    case  type::sine:
      ret << "sin(";
      ret << nodeA -> build_print_string();
      ret << ")";
      break;
    case  type::logarithm:
      ret << "log(";
      ret << nodeA -> build_print_string();
      ret << ")";
      break;
    case type::exponential:
      ret << "exp(";
      ret << nodeA -> build_print_string();
      ret << ")";
      break;
	
    default:
      ret << "This should never occur also IB was here";
    }
  if (expo > 1)
    ret << "^" << expo;
  return ret.str();
}

//Returns whether the function is constant, ie whether all greatest grandchildren are constants
bool funk::isConstant() const
{
  return isInteger() || ((nodeA ? nodeA->isConstant() : true) && (nodeB ? nodeB->isConstant(): true));
}

//Returns whether this particular funk is a constant (checks independent of type)
bool funk::isInteger() const
{
  return expo == 0 || isZero();
}

bool funk::isZero() const
{
  return coef == 0;
}

void funk::breakExpo()
{
  //std::cout << "Calling breakExpo: " << *this << std::endl;
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
  funk toswitch;
  toswitch.state = type::divide;
  toswitch.coef = this->nodeA->coef * this->nodeB->coef;
  toswitch.expo = 1;
  toswitch.nodeA.reset(new funk(*(this->nodeA->nodeA) * *(this->nodeB->nodeA)));
  toswitch.nodeB.reset(new funk(*(this->nodeA->nodeB) * *(this->nodeB->nodeB)));
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
  if(*nodeA == 0){
    this->replaceWith(nodeB);
  }
  else if (*nodeB == 0){
    this->replaceWith(nodeA);
  }
  else if (nodeA->compareWithoutCoef(*nodeB)){
    evaluateCoef(nodeA->coef + nodeB->coef, nodeB);
  }
  //std::cout << "exiting simplifyAddition" << std::endl;
  
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
  
  if (*nodeB == 0){
    throw math_not_possible("Attempt to divide by zero");
  }
  if(*nodeA == 0){
    *this = 0;
    return;
  }
  else if (*nodeB == 1) {
    this->replaceWith(nodeA);
    return;
  } 
  else{
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
    else if (nodeA->state == type::addition){
      lazyAddDivide();
    }
    else if (nodeA -> state == type::divide || nodeB -> state == type::divide){
      if(nodeA -> state != type::divide){
	expandToDivide(nodeA); 	 
      }
      if(nodeB -> state != type::divide){
	expandToDivide(nodeB);
      }
      divXdiv();
    }
    if (*nodeB == 1){
      this->replaceWith(*nodeA);
    }
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

void funk::organize(){
  if ((state == type::addition || state == type::multiply) && *nodeA < *nodeB) {
    std::swap(nodeA, nodeB);
  }	
}
	
funk& funk::simplify(){
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
  return *this;
}

bool funk::compareMathEquiv(const funk& obj) const
{
  return *this == obj || (this->coef == 0 && obj.coef == 0) || (this->coef == obj.coef && this->expo == 0 && obj.expo == 0);
}

bool funk::statesAndNodesEqual(const funk& obj) const
{
  return this->state == obj.state && (this->state == type::base ? true : this->nodeA == obj.nodeA && this->nodeB == obj.nodeB);
}

bool funk::compareWithoutCoef(const funk& obj) const
{
  return this->expo == obj.expo && statesAndNodesEqual(obj);
}

bool funk::operator==(const funk& obj) const
{
    return this->coef == obj.coef && compareWithoutCoef(obj);
}

bool funk::operator==(const int& constant) const
{
  return (this->expo == 0 && this->coef == constant) || (constant == 0 && isZero());
}

bool funk::operator!=(const funk& obj) const
{
  return !(*this == obj);
}

bool funk::operator!=(const int& constant) const
{
  return !(*this == constant);
}

#define inequalityComparison(sign) (this->state sign obj.state) || (this->state == obj.state && this->expo sign obj.expo) || (this->expo == obj.expo && this->nodeA sign obj.nodeA) || (this->nodeA == obj.nodeA && this->nodeB sign obj.nodeB)
bool funk::operator<(const funk& obj) const
{
  return inequalityComparison(<);
}

bool funk::operator>(const funk& obj) const
{
  return inequalityComparison(>);
}

funk funk::operator+(const funk& obj)
{
	funk ret;				
	ret.state = type::addition;
	
	ret.nodeA.reset(new funk(*this));
	ret.nodeB.reset(new funk(obj));
	ret.simplify();
        			
	return ret;
}

funk funk::operator*(const funk& obj)
{
  funk ret;
  ret.state = type::multiply;
  ret.nodeA.reset(new funk(*this));
  ret.nodeB.reset(new funk(obj));

  ret.simplify();
  return ret;
}

funk funk::operator/(const funk& obj)
{
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
