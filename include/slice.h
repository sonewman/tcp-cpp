#pragma once
#include <cstddef>

struct Slice {
  const char* data;
  const std::size_t size;

  Slice(const char* d, const std::size_t s):
    data(d), size(s) {}

  ~Slice() {
    delete this->data;
  }
};