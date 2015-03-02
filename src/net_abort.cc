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
void Abort(const FunctionCallbackInfo<Value>& args) {
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	// TODO: type check
	TCPWrap* wrap = Unwrap<TCPWrap>(args.Holder());

#ifdef WIN32
	SOCKET socket = wrap->UVHandle()->socket;
	linger val = { 1, 0 };
	if (setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) != 0) {
		isolate->ThrowException(WinapiErrnoException(WSAGetLastError(), "setsockopt"));
		return;
	}
	if (closesocket(socket) != 0) {
		isolate->ThrowException(WinapiErrnoException(WSAGetLastError(), "closesocket"));
		return;
	}
#else
	int fd = wrap->UVHandle()->io_watcher.fd;
	linger val = { 1, 0 };
	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) != 0) {
		isolate->ThrowException(ErrnoException(errno, "setsockopt"));
	}
	if (close(fd) != 0) {
		isolate->ThrowException(ErrnoException(errno, "close"));
	}
#endif
}

/**
 * Initialize this module
 * @param {Handle<Object>} exports - Module object
 */
void init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "abort", Abort);
}

// HACK: no TCPWrap::UVHandle in node.lib
uv_tcp_t* TCPWrap::UVHandle() {
	return &handle_;
}

/**
 * Module declaration
 */
NODE_MODULE(net_abort, init);
