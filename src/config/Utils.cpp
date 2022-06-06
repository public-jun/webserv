#include "Utils.hpp"

Utils::Utils() {
}

Utils::Utils(const Utils &src)  {
  *this = src;
}

Utils::~Utils() {

}

Utils &Utils::operator=(const Utils &src) {
  if (this != &src) {
    (void)src;
  }
  return (*this);
}

void Utils::findEndBrace(str_vec_itr &it) {
  while (*it != "}") {
    if (*it == "{")
      findEndBrace(++it);
    it++;
  }
}
