#include "../include/log.h"
#include "../include/connection.h"


Connection::Connection(ServerState* s, uv_stream_t* str):
  serverState(s), serverStream(str) {
  this->stream.data = this;

  int r = uv_tcp_init(this->serverState->loop, (uv_tcp_t*) &this->stream);

  if (r != 0) log_uv_error(r);
}

int Connection::Accept() {
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
  auto connection = static_cast<Connection*>(stream->data);

  if (nread < 0) {
    log_uv_error(nread);
    return;
  }

  log("on read");

  connection->onDataFn(new Slice(buf->base, buf->len));
}

int Connection::ReadStart(OnDataFn onDataFn) {
  this->onDataFn = onDataFn;
  return uv_read_start(&this->stream, allocBuffer, onRead);
}

int Connection::Write(WriteRequest* wr) {
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  req->data = wr;
  
  uv_buf_t buf[] = {
    { .base = (char*) wr->buf, .len = wr->size }
  };

  auto onWrite = [](uv_write_t* req, int status) {
    std::cout << "on write" << std::endl;

    auto wr = static_cast<WriteRequest*>(req->data);

    if (wr->onEnd) {
      wr->onEnd();
    }

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

int Connection::Write(const char* d, OnEnd onEnd) {
  return this->Write(new WriteRequest(d, onEnd));
}