#pragma once
#include <functional>
// #include "./log.h"
#include "./write-request.h"

#include "./slice.h"
#include "./server-state.h"

using OnDataFn = std::function<void (Slice*)>;

struct Connection {
  uv_stream_t stream{};
  
  ServerState* serverState;
  uv_stream_t* serverStream;

  OnDataFn onDataFn;

  Connection(ServerState* s, uv_stream_t* str);

  int Accept();

  int ReadStart(OnDataFn onDataFn);

  int Write(WriteRequest* wr);

  int Write(const char* d, OnEnd onEnd);
};