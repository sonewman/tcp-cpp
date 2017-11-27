#include <iostream>
#include <functional>
#include <thread>

#include "./deps/libuv/include/uv.h"

#include "./include/log.h"
#include "./include/tcp.h"

// static void on_connect

int main() {
  struct sockaddr_in addr;

  const char* address = "0.0.0.0";
  const int port = 8999;

  Server server([](Server::Connection* conn) {
    conn->Accept();

    conn->ReadStart([&conn](Slice* s) {

      log("server recieved:");
      log(s->data);
      // delete Slice

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

  std::thread client_thread([&]() {
    auto client = new Client(address, port);

    client->Connect([&](Client::Connection* conn) {

      conn->ReadStart([](Slice* s) {
        log("client recieved:");
        log(s->data);

        // delete Slice
      });

      conn->Write("stuff", []() {

      });
      // delete client;
    });
  });

  server_thread.join();

  return 0;
}