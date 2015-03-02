#include <node.h>
#include <tcp_wrap.h>
#include <v8.h>
#include <nan.h>

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
NAN_METHOD(Abort) {
	NanScope();

	// TODO: type check
	TCPWrap* wrap = (TCPWrap *)NanGetInternalFieldPointer(args.Holder(), 0);

#ifdef WIN32
	SOCKET socket = wrap->UVHandle()->socket;
	linger val = { 1, 0 };
	if (setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) != 0) {
		NanThrowError(WinapiErrnoException(WSAGetLastError(), "setsockopt"));
		NanReturnUndefined();
	}
	if (closesocket(socket) != 0) {
		NanThrowError(WinapiErrnoException(WSAGetLastError(), "closesocket"));
		NanReturnUndefined();
	}
#else
	int fd = wrap->UVHandle()->io_watcher.fd;
	linger val = { 1, 0 };
	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof(val)) != 0) {
		NanThrowError(ErrnoException(errno, "setsockopt"));
		NanReturnUndefined();
	}
	if (close(fd) != 0) {
		NanThrowError(ErrnoException(errno, "close"));
		NanReturnUndefined();
	}
#endif

	NanReturnUndefined();
}

/**
 * Initialize this module
 * @param {Handle<Object>} exports - Module object
 */
void init(Handle<Object> exports) {
	exports->Set(NanNew<String>("abort"), NanNew<FunctionTemplate>(Abort)->GetFunction());
}

// HACK: no TCPWrap::UVHandle in node.lib
uv_tcp_t* TCPWrap::UVHandle() {
	return &handle_;
}

/**
 * Module declaration
 */
NODE_MODULE(net_abort, init);
