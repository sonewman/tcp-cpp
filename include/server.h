#pragma once

// std
#include <functional>

// deps
#include "../deps/libuv/include/uv.h"

// local
#include "./write-request.h"
#include "./slice.h"

using OnDataFn = std::function<void (Slice*)>;

class Server {
public:
  struct ServerState {
    uv_tcp_t server;
    uv_loop_t* loop;
    struct sockaddr_in addr;

    ServerState() {
      this->loop = (uv_loop_t*) malloc(sizeof(uv_loop_t));
      uv_loop_init(this->loop);
    }
  };

  class Connection {
  public:
    uv_stream_t stream{};

    ServerState* serverState;
    uv_stream_t* serverStream;

    OnDataFn onDataFn;

    Connection(ServerState* s, uv_stream_t* str);

    int Accept();

    int ReadStart(OnDataFn onDataFn);

    int Write(WriteRequest* wr);

    int Write(const char* d, OnEnd onEnd);

    int End(OnEnd onEnd);
  };

  using OnConnectFn = std::function<void (Server::Connection*)>;

  ServerState state;
  OnConnectFn onConnect;
  Server(OnConnectFn on);
  int Listen(const char* address, const int port);
};
