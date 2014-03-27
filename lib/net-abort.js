var net = require('net');
var net_abort = require('../build/Release/net_abort');

function abort() {
  var socket, args;
  if (this instanceof net.Socket) {
    socket = this;
    args = Array.prototype.slice.call(arguments, 0);
  } else if (arguments[0] instanceof net.Socket) {
    socket = arguments[0];
    args = Array.prototype.slice.call(arguments, 1);
  } else {
    throw new TypeError('Supplied argument is not net.Socket');
  }
  
  if (!socket._handle) {
    throw new Error('Socket is not connected or not TCP socket');
  }
  
  net_abort.abort.apply(socket._handle, args);
  socket.destroy();
}

function install() {
  net.Socket.prototype.abort = abort;
}

module.exports = abort;
module.exports.install = install;
