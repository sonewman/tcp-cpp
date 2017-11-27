#pragma once

// std
#include <functional>

// deps
#include "../deps/libuv/include/uv.h"

// local
#include "../include/log.h"
#include "./write-request.h"
#include "./slice.h"

using OnDataFn = std::function<void (Slice*)>;

class Client {
public:
  const char* address;
  const int port;
  struct sockaddr_in dest;

  struct ClientState {
    uv_tcp_t socket{};
    uv_connect_t connect;
    uv_loop_t* loop = uv_default_loop(); // maybe use a different loop?   
  };

  ClientState state{};

  Client(const char* address, const int port);

  class Connection {
  public:
    Connection(ClientState* s);

    ClientState* clientState;

    OnDataFn onDataFn;

    int Accept();

    int ReadStart(OnDataFn onDataFn);

    int Write(WriteRequest* wr);

    int Write(const char* d, OnEnd onEnd);

    int End(OnEnd onEnd);
  };

  using OnConnectFn = std::function<void (Connection*)>;

  OnConnectFn onConnectFn;

  int Connect(OnConnectFn);
};