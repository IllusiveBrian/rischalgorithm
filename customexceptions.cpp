#include <stdexcept>
#include <string>

class math_too_hard : public std::domain_error{
 public:
  math_too_hard(const char* error) : std::domain_error(error){}
  virtual const char* what() const throw()
  {
    return (std::string("Operation is not supported: ")+=std::domain_error::what()).c_str();
  }
};

  class math_not_possible : public std::domain_error{
 public:
    math_not_possible(const char* error) : std::domain_error(error){}
  virtual const char* what() const throw()
  {
    return (std::string("Operation is not mathematically possible: ")+=std::domain_error::what()).c_str();
  }
};
