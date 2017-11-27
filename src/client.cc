#include "../deps/libuv/include/uv.h"

#include "../include/tcp.h"

Client::Client(const char* address, const int port):
  address(address),
  port(port) {
    uv_ip4_addr(address, port, &this->dest);
    this->state.connect.data = this;
}

Client::Connection::Connection(Client::ClientState* s):
  clientState(s) {
  this->clientState->connect.handle->data = this;
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
  auto connection = static_cast<Client::Connection*>(stream->data);

  if (nread < 0) {
    log_uv_error(nread);
    return;
  }

  connection->onDataFn(new Slice(buf->base, buf->len));
}

int Client::Connection::ReadStart(OnDataFn onDataFn) {
  this->onDataFn = onDataFn;
  return uv_read_start(this->clientState->connect.handle, allocBuffer, onRead);
}

int Client::Connection::Write(WriteRequest* wr) {
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

  return uv_write(req, this->clientState->connect.handle, buf, 1, onWrite);
}

int Client::Connection::Write(const char* d, OnEnd onEnd) {
  return this->Write(new WriteRequest(d, onEnd));
}

static void onConnect(uv_connect_t* connect, int status) {
  if (status != 0) {
    log_uv_error(status);
    return;
  };

  auto client = static_cast<Client*>(connect->data);

  auto connection = new Client::Connection(
    &client->state
  ); // could make shared ptr?

  client->onConnectFn(connection);
}


int Client::Connect(OnConnectFn onConnectFn) { // TODO maybe return something not just number?/
  uv_tcp_init(this->state.loop, &this->state.socket);

  this->onConnectFn = onConnectFn;

  uv_tcp_connect(
    &this->state.connect,
    &this->state.socket,
    (const struct sockaddr*)&dest,
    onConnect
  );

  // TODO maybe create new loop per connection?
  uv_run(this->state.loop, UV_RUN_DEFAULT);


  // log("here");
  return 0;
}