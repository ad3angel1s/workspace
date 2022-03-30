#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fmt/format.h>

std::ostream& operator<<(std::ostream &os, std::vector<uint8_t> const& v) {
  int counter = 0;
  for (auto &i: v) {
    if (!(counter % 16) && counter != 0) {
      os << std::endl;
    }
    os << fmt::format("0x{:02x} ", i);
    counter++;
  }
  return os;
}

std::vector<uint8_t> operator+(std::vector<uint8_t> const& lhs, std::vector<uint8_t> const& rhs) {
  auto i = lhs.begin();
  auto j = rhs.begin();
  std::vector<uint8_t> v;
  size_t t_num = lhs.size() < rhs.size() ? lhs.size() : rhs.size();
  for (int k=0; k<t_num; k++) {
    v.push_back((*i++) + (*j++));
  }
  return v;
}

std::vector<uint8_t> operator^(std::vector<uint8_t> const& lhs, std::vector<uint8_t> const& rhs) {
  auto i = lhs.begin();
  auto j = rhs.begin();
  std::vector<uint8_t> v;
  size_t t_num = lhs.size() < rhs.size() ? lhs.size() : rhs.size();
  for (int k=0; k<t_num; k++) {
    v.push_back((*i++) ^ (*j++));
  }
  return v;
}

int main(void) {

  return 0;
}
