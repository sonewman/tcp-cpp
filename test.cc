#include <iostream>
#include <functional>
#include <thread>

#include "./deps/libuv/include/uv.h"

#include "./include/log.h"
#include "./include/server.h"

// static void on_connect

int main() {
  struct sockaddr_in addr;

  const char* address = "0.0.0.0";
  const int port = 8999;

  Server server([](Connection* conn) {
    conn->Accept();

    conn->ReadStart([&conn](Slice* data) {
      conn->Write("World", []() {
        // log("second write");
      });
    });

    conn->Write("Hey", []() {
      // log("first write");
    });
  });

  std::thread server_thread([&]() {
    server.Listen(address, port);
  });

  // std::thread client_thread([&]() {
  //   uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  //   uv_tcp_init(loop, socket);

  //   uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));

  //   struct sockaddr_in dest;
  //   uv_ip4_addr("127.0.0.1", 80, &dest);

  //   uv_tcp_connect(connect, socket, (const struct sockaddr*)&dest, on_connect);
  // });

  server_thread.join();

  return 0;
}