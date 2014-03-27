var assert = require('assert');
var net = require('net');
var async = require('async');
var abort = require('../lib/net-abort');

describe('net-abort', function () {
  describe('#install', function () {
    it('should install net.Socket#abort', function () {
      assert.strictEqual(typeof net.Socket.prototype.abort, "undefined");
      require('../lib/net-abort').install();
      assert.strictEqual(net.Socket.prototype.abort, abort);
    });
  });
});

describe('net', function () {
  before(function () {
    require('../lib/net-abort').install();
  });
  
  var client, listener, server, events;
  beforeEach(function (done) {
    events = [];
    async.auto({
      listen: function (done) {
        listener = net.createServer();
        listener.listen(0, done);
      },
      connect: ['listen', function (done) {
        client = net.connect({
          port: listener.address().port,
          host: 'localhost',
          allowHalfOpen: true
        }, done);
      }],
      accept: ['listen', function (done) {
        listener.once('connection', function (conn) {
          server = conn;
          done();
        });
      }],
      log: ['connect', 'accept', function (done) {
        [client, server].forEach(function (conn) {
          ['data', 'end', 'close', 'error'].forEach(function (event) {
            conn.on(event, function () {
              events.push({
                socket: (conn === client) ? 'client' : (conn === server) ? 'server' : null,
                event: event,
                args: Array.prototype.slice.call(arguments).map(function (item) {
                  if (item instanceof Error && item.code) {
                    return item.code;
                  } else {
                    return item;
                  }
                })
              });
            });
          });
        });
        done();
      }]
    }, done);
  });
  afterEach(function (done) {
    client.destroy();
    server.destroy();
    listener.close(done);
  });
  
  describe('#end', function () {
    it('should close gracefully', function (done) {
      client.end();
      async.parallel([
        function (done) {
          client.once('close', done);
        },
        function (done) {
          server.once('close', done);
        }
      ], function (err) {
        assert.ifError(err);
        
        assert.deepEqual(events, [
          { socket: 'server', event: 'end', args: [] },
          { socket: 'server', event: 'close', args: [false] },
          { socket: 'client', event: 'end', args: [] },
          { socket: 'client', event: 'close', args: [false] }
        ]);
        
        done();
      });
    });
  });
  
  describe('#abort', function () {
    it('should close forcedly from client', function (done) {
      client.abort();
      async.parallel([
        function (done) {
          client.once('close', function () {
            done();
          });
        },
        function (done) {
          server.once('close', function () {
            done();
          });
        }
      ], function (err) {
        assert.ifError(err);
        
        assert.deepEqual(events.filter(function (item) {
          return item.socket === 'client';
        }), [
          { socket: 'client', event: 'close', args: [false] } // TODO: should #abort emit error?
        ]);
        assert.deepEqual(events.filter(function (item) {
          return item.socket === 'server';
        }), [
          { socket: 'server', event: 'error', args: ['ECONNRESET'] },
          { socket: 'server', event: 'close', args: [true] }
        ]);
        
        done();
      });
    });
    
    it('should close forcedly from server', function (done) {
      server.abort();
      async.parallel([
        function (done) {
          client.once('close', function () {
            done();
          });
        },
        function (done) {
          server.once('close', function () {
            done();
          });
        }
      ], function (err) {
        assert.ifError(err);
        
        assert.deepEqual(events.filter(function (item) {
          return item.socket === 'client';
        }), [
          { socket: 'client', event: 'error', args: ['ECONNRESET'] },
          { socket: 'client', event: 'close', args: [true] }
        ]);
        assert.deepEqual(events.filter(function (item) {
          return item.socket === 'server';
        }), [
          { socket: 'server', event: 'close', args: [false] } // TODO: should #abort emit error?
        ]);
        
        done();
      });
    });
    
    // TODO: tests with pending write
    // TODO: tests with pending read
    // TODO: tests with half-close state
  });
});
