#include "../include/log.h"
#include "../include/server.h"

Server::Server(OnConnectFn on): onConnect(on) {
  this->state.server.data = this;
}

static void onConnection(uv_stream_t* stream, int status) {
  if (status != 0) {
    log_uv_error(status);
    return;
  };

  auto server = static_cast<Server*>(stream->data);
  auto connection = new Connection(&server->state, stream);

  server->onConnect(connection);
}

int Server::Listen(const char* address, const int port) {
  int r = uv_ip4_addr(address, port, &this->state.addr);
  log_uv_error(r);

  r = uv_tcp_init(this->state.loop, &this->state.server);
  log_uv_error(r);

  r = uv_tcp_bind(
    &this->state.server,
    (const struct sockaddr*) &this->state.addr,
    0
  );
  log_uv_error(r);

  r = uv_listen(
    (uv_stream_t*) &this->state.server,
    SOMAXCONN,
    onConnection
  );

  if (r) {
    log_uv_error(r);
    return r;
  }

  return uv_run(this->state.loop, UV_RUN_DEFAULT);
}