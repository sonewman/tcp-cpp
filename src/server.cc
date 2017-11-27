// deps
#include "../deps/libuv/include/uv.h"

// local
#include "../include/tcp.h"

Server::Server(OnConnectFn on): onConnect(on) {
  this->state.server.data = this;
}

Server::Connection::Connection(ServerState* s, uv_stream_t* str):
  serverState(s), serverStream(str) {
  this->stream.data = this;

  int r = uv_tcp_init(this->serverState->loop, (uv_tcp_t*) &this->stream);

  if (r != 0) log_uv_error(r);
}

int Server::Connection::Accept() {
  int r = uv_accept(this->serverStream, &this->stream);
  if (r != 0) log_uv_error(r);
  return r;
}

static void allocBuffer(
  uv_handle_t*,
  size_t suggested_size,
  uv_buf_t* buf
) {
  buf->base = (char*) malloc(suggested_size);
  buf->len = suggested_size;
}

static void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  auto connection = static_cast<Server::Connection*>(stream->data);

  if (nread < 0) {
    log_uv_error(nread);
    return;
  }

  connection->onDataFn(new Slice(buf->base, buf->len));
}

int Server::Connection::ReadStart(OnDataFn onDataFn) {
  this->onDataFn = onDataFn;
  return uv_read_start(&this->stream, allocBuffer, onRead);
}

int Server::Connection::Write(WriteRequest* wr) {
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  req->data = wr;

  uv_buf_t buf[] = {
    { .base = (char*) wr->buf, .len = wr->size }
  };

  auto onWrite = [](uv_write_t* req, int status) {
    auto wr = static_cast<WriteRequest*>(req->data);
    if (wr->onEnd) wr->onEnd();

    delete wr;
    free(req);

    if (status != 0) {
      fprintf(
        stderr,
        "uv_write error: %s - %s\n",
        uv_err_name(status),
        uv_strerror(status)
      );
    }
  };

  return uv_write(req, &this->stream, buf, 1, onWrite);
}

int Server::Connection::Write(const char* d, OnEnd onEnd) {
  return this->Write(new WriteRequest(d, onEnd));
}

static void onConnection(uv_stream_t* stream, int status) {
  if (status != 0) {
    log_uv_error(status);
    return;
  };

  auto server = static_cast<Server*>(stream->data);
  auto connection = new Server::Connection(&server->state, stream);

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