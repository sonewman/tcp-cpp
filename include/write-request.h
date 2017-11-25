#pragma once
#include <cstring>
#include <functional>

using OnEnd = std::function<void ()>;

struct WriteRequest {
  const char* buf;
  std::size_t size;
  OnEnd onEnd;

  WriteRequest(const char* b, OnEnd o):
    buf(b), size(std::strlen(b)), onEnd(o) {}
};