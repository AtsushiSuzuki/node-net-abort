#include <node.h>
#include <node_internals.h>
#include <tcp_wrap.h>
#include <v8.h>

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <unistd.h>
	#include <errno.h>
#endif


using namespace node;
using namespace v8;


/**
 * Emit an RST packet and close socket.
 * @this TCP
 */
Handle<Value> Abort(const Arguments& args) {
	HandleScope scope;
	
	// TODO: type check
	
	UNWRAP(TCPWrap)
	
#ifdef WIN32
	SOCKET socket = wrap->UVHandle()->socket;
	linger val = { 1, 0 };
	if (setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) != 0) {
		ThrowException(WinapiErrnoException(WSAGetLastError(), "setsockopt"));
		return scope.Close(Undefined());
	}
	if (closesocket(socket) != 0) {
		ThrowException(WinapiErrnoException(WSAGetLastError(), "closesocket"));
		return scope.Close(Undefined());
	}
#else
	int fd = wrap->UVHandle()->io_watcher.fd;
	linger val = { 1, 0 };
	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) != 0) {
		ThrowException(ErrnoException(errno, "setsockopt"));
		return scope.Close(Undefined());
	}
	if (close(fd) != 0) {
		ThrowException(ErrnoException(errno, "close"));
		return scope.Close(Undefined());
	}
#endif
	
	return scope.Close(Undefined());
}

/**
 * Initialize this module
 * @param {Handle<Object>} exports - Module object
 */
void init(Handle<Object> exports) {
	exports->Set(String::NewSymbol("abort"),
		FunctionTemplate::New(Abort)->GetFunction());
}

// HACK: no TCPWrap::UVHandle in node.lib
uv_tcp_t* TCPWrap::UVHandle() {
  return &handle_;
}

/**
 * Module declaration
 */
NODE_MODULE(net_abort, init);
