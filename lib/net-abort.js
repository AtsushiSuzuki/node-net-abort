var net = require('net');
var net_abort = require('../build/Release/net_abort');


/**
 * Emit an RST packet and close socket.
 * @param {net.Socket} socket - an connected TCP socket to close.
 */
function abort() {
  var socket;
  if (this instanceof net.Socket) {
    socket = this;
  } else if (arguments[0] instanceof net.Socket) {
    socket = arguments[0];
  } else {
    throw new TypeError('Supplied argument is not net.Socket');
  }
  
  if (!socket._handle) {
    throw new Error('Socket is not connected or is not TCP socket');
  }
  
  net_abort.abort.apply(socket._handle);
  socket.destroy();
}

/**
 * Install socket.abort to net.Socket class
 */
function install() {
  net.Socket.prototype.abort = abort;
}

module.exports = abort;
module.exports.install = install;
