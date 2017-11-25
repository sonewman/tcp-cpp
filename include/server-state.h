#pragma once
#include "../deps/libuv/include/uv.h"

struct ServerState {
  uv_tcp_t server;
  uv_loop_t* loop = uv_default_loop();
  struct sockaddr_in addr;
};