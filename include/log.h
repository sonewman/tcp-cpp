#pragma once
#include <iostream>
#include "../deps/libuv/include/uv.h"

static void log_uv_error(int r) {
  if (r != 0)
    std::cerr << uv_err_name(r) << std::endl;
 }

static void log(const char* s) {
  std::cout << s << std::endl;
}