#include <node.h>
#include <node_internals.h>
#include <tcp_wrap.h>
#include <v8.h>

#ifdef WIN32
#else
	#include <unistd.h>
#endif

using namespace node;
using namespace v8;

Handle<Value> Abort(const Arguments& args) {
	HandleScope scope;
	
	UNWRAP(TCPWrap)
	
#ifdef WIN32
	SOCKET socket = wrap->UVHandle()->socket;
	linger val = { 1, 0 };
	assert(setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) == 0);
	assert(closesocket(socket) == 0);
#else
	int fd = wrap->UVHandle()->io_watcher.fd;
	linger val = { 1, 0 };
	assert(setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) == 0);
	assert(close(fd) == 0);
#endif
	
	return Null();
}

void init(Handle<Object> exports) {
	exports->Set(String::NewSymbol("abort"),
		FunctionTemplate::New(Abort)->GetFunction());
}

uv_tcp_t* TCPWrap::UVHandle() {
  return &handle_;
}

NODE_MODULE(net_abort, init);
