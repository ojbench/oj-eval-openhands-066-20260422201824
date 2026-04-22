#include "src.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

int main(){
  using namespace sjtu;
  any_ptr a = make_any_ptr(1);
  any_ptr b = a;
  a.unwrap<int>() = 2;
  std::cout << b.unwrap<int>() << '\n';
  b = new std::string;
  b.unwrap<std::string>() = "Hello, world!";
  std::cout << b.unwrap<std::string>() << '\n';
  any_ptr v = make_any_ptr<std::vector<int>>(1,2,3);
  std::cout << v.unwrap<std::vector<int>>().size() << '\n';
  any_ptr m = make_any_ptr<std::map<int,int>>({{1,2},{3,4}});
  std::cout << m.unwrap<std::map<int,int>>().size() << '\n';
  try{ a.unwrap<std::string>(); }
  catch(const std::bad_cast &e){ std::cout << "bad cast" << '\n'; }
  return 0;
}
