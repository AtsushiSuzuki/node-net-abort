node-net-abort
==============

Add force-close (sending RST) support to net.Socket.

##usage

	var net = require('net');
	require('net-abort').install();
	
	net.createServer(function (conn) {
		conn.on('data', function (data) {
			// process data
			var isValid = processData(data);
			
			// if received data is not valid
			if (!isValid) {
				// close connection by sending RST,
				// to notify peer that something is wrong.
				conn.abort();
			}
		});
	}).listen(8080);
	