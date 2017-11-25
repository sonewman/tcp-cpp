#include "../deps/libuv/include/uv.h"
#include "../include/log.h"

static void on_connect(uv_connect_t* connect, int status) {
  if (status != 0) {
    log_uv_error(status);
    return;
  };

  // auto server = static_cast<Server*>(stream->data);
  // auto connection = new Connection(&server->state, stream);

  // server->onConnect(connection);
}

struct Client {
  uv_tcp_t socket{};
  uv_connect_t connect{};
  uv_loop_t* loop = uv_default_loop(); // maybe use a different loop?

  const char* address;
  const int port;
  struct sockaddr_in dest;

  Client(const char* address, const int port):
    address(address),
    port(port) {
      uv_ip4_addr(address, port, &this->dest);
      this->connect.data = this;

    // uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    // uv_tcp_init(loop, socket);

    // uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));

    // struct sockaddr_in dest;
    // uv_ip4_addr("127.0.0.1", 80, &dest);

    // uv_tcp_connect(connect, socket, (const struct sockaddr*)&dest, on_connect);
  }

  int Connect() { // TODO maybe return something not just number?/
    uv_tcp_init(this->loop, &this->socket);
    uv_tcp_connect(
      &this->connect,
      &this->socket,
      (const struct sockaddr*)&dest,
      on_connect
    );

    return 0;
  }


};