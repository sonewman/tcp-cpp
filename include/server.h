#pragma once
#include <functional>
#include "../deps/libuv/include/uv.h"
#include "./connection.h"

using OnConnectFn = std::function<void (Connection*)>;

struct Server {
  ServerState state;
  OnConnectFn onConnect;
  Server(OnConnectFn on);
  int Listen(const char* address, const int port);
};
