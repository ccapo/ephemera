#pragma once

#include "common.h"

// Data struct
struct data_t {
  std::string name;
  int count;

  // Constructors
  data_t(): name(""), count(-1) {}
  data_t(const std::string &n, const int &c): name(n), count(c) {}

  // Test if data_t is empty
  bool empty() {
    return name == "" && count == -1;
  }
};

// Custom output stream for data struct
std::ostream& operator << (std::ostream &o, const data_t &d) {
  o << "{" << std::endl;
  o << "  name: '" << d.name << "'" << std::endl;
  o << "  count: " << d.count << std::endl;
  o << "}";
  return o;
}
