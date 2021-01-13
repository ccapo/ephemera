// MIT License
//
// Copyright (c) 2021 Chris Capobianco
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#ifndef INCLUDE_DATA_H_
#define INCLUDE_DATA_H_

#include <string>
#include <iostream>

// Data struct
struct data_t {
  std::string name;
  int count;

  // Constructors
  data_t(): name(""), count(-1) {}
  data_t(const std::string &n, const int &c): name(n), count(c) {}
};

// Custom output stream for data struct
std::ostream& operator << (std::ostream &o, const data_t &d) {
  o << "{" << std::endl;
  o << "  name: '" << d.name << "'" << std::endl;
  o << "  count: " << d.count << std::endl;
  o << "}";
  return o;
}

#endif  // INCLUDE_DATA_H_
