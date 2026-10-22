#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

ਤਃ __future__ ਆਯਾਤ print_function
ਆਯਾਤ json
ਆਯਾਤ socket
ਆਯਾਤ sys
ਆਯਾਤ ਸਮਾਂ
ਆਯਾਤ threading

ਕੋਸ਼ਿਸ਼:
    # Python 3
    ਆਯਾਤ socketserver
ਅਪਵਾਦ ImportError:
    # Python 2
    ਆਯਾਤ SocketServer ਇਵ socketserver

ਵਰਗ TestingRequestHandler(socketserver.ਅਧਾਰ_Request_Handler):
    ਪਰਿਭਾਸ਼ਾ handle(self):
        ਛਾਪੋ("=== socket opened ===")
        ਜਦੋਂ ਸੱਚ:
            ਕੋਸ਼ਿਸ਼:
                received = self.request.recv(4096).decode('utf-8')
            ਅਪਵਾਦ socket.error:
                ਛਾਪੋ("=== socket error ===")
                ਰੋਕੋ
            ਅਪਵਾਦ IOError:
                ਛਾਪੋ("=== socket closed ===")
                ਰੋਕੋ
            ਜੇ received == '':
                ਛਾਪੋ("=== socket closed ===")
                ਰੋਕੋ
            ਛਾਪੋ("received: {0}".format(received))

            # We may receive two messages at once. Take the part up to the
            # newline, which should be after the matching "]".
            todo = received
            ਜਦੋਂ todo != '':
                splitidx = todo.ਲੱਭੋ('\n')
                ਜੇ splitidx < 0:
                     ਵਰਤਿਆ = todo
                     todo = ''
                ਨਹੀਂਤਾਂ:
                     ਵਰਤਿਆ = todo[:splitidx]
                     todo = todo[splitidx + 1:]
                ਜੇ ਵਰਤਿਆ != received:
                    ਛਾਪੋ("using: {0}".format(ਵਰਤਿਆ))

                ਕੋਸ਼ਿਸ਼:
                    decoded = json.loads(ਵਰਤਿਆ)
                ਅਪਵਾਦ ਮੁੱਲ_Error:
                    ਛਾਪੋ("json decoding failed")
                    decoded = [-1, '']

                # Send a response if the sequence number is positive.
                ਜੇ decoded[0] >= 0:
                    ਜੇ decoded[1] == 'hello!':
                        # simply send back a string
                        response = "got it"
                    ਜਾਂਜੇ decoded[1] == 'malformed1':
                        cmd = '["ex",":"]wrong!["ex","smi"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Need to wait for Vim to give up, otherwise it
                        # sometimes fails on OS X.
                        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
                    ਜਾਂਜੇ decoded[1] == 'malformed2':
                        cmd = '"unterminated string'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Need to wait for Vim to give up, otherwise the double
                        # quote in the "ok" response terminates the string.
                        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
                    ਜਾਂਜੇ decoded[1] == 'malformed3':
                        cmd = '["ex","missing ]"'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Need to wait for Vim to give up, otherwise the ]
                        # in the "ok" response terminates the list.
                        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
                    ਜਾਂਜੇ decoded[1] == 'split':
                        cmd = '["ex","let '
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.01)
                        cmd = 'g:split = 123"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1].startswith("echo "):
                        # send back the argument
                        response = decoded[1][5:]
                    ਜਾਂਜੇ decoded[1] == 'make change':
                        # Send two ex commands at the same time, before
                        # replying to the request.
                        cmd = '["ex","call append(\\"$\\",\\"added1\\")"]'
                        cmd += '["ex","call append(\\"$\\",\\"added2\\")"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'echoerr':
                        cmd = '["ex","echoerr \\\"this is an error\\\""]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Wait a bit, so that the "ex" command is handled
                        # before the "ch_evalexpr() returns.  Otherwise we are
                        # outside the try/catch when the "ex" command is
                        # handled.
                        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.02)
                    ਜਾਂਜੇ decoded[1] == 'bad command':
                        cmd = '["ex","foo bar"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'do normal':
                        # Send a normal command.
                        cmd = '["normal","G$s more\u001b"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-works':
                        # Send an eval request.  We ignore the response.
                        cmd = '["expr","\\"foo\\" . 123", -1]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-special':
                        # Send an eval request.  We ignore the response.
                        cmd = '["expr","\\"foo\x7f\x10\x01bar\\"", -2]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-getline':
                        # Send an eval request.  We ignore the response.
                        cmd = '["expr","getline(3)", -3]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-fails':
                        # Send an eval request that will fail.
                        cmd = '["expr","xxx", -4]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-error':
                        # Send an eval request that works but the result can't
                        # be encoded.
                        cmd = '["expr","function(\\"tr\\")", -5]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-bad':
                        # Send an eval request missing the third argument.
                        cmd = '["expr","xxx"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'an expr':
                        # Send an expr request.
                        cmd = '["expr","setline(\\"$\\", [\\"one\\",\\"two\\",\\"three\\"])"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'call-func':
                        cmd = '["call","MyFunction",[1,2,3], 0]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'redraw':
                        cmd = '["redraw",""]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'redraw!':
                        cmd = '["redraw","force"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'empty-request':
                        cmd = '[]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'eval-result':
                        # Send back the last received eval result.
                        response = ਆਖਰੀ_eval
                    ਜਾਂਜੇ decoded[1] == 'call me':
                        cmd = '[0,"we called you"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ਜਾਂਜੇ decoded[1] == 'call me again':
                        cmd = '[0,"we did call you"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = ""
                    ਜਾਂਜੇ decoded[1] == 'send zero':
                        cmd = '[0,"zero index"]'
                        ਛਾਪੋ("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "sent zero"
                    ਜਾਂਜੇ decoded[1] == 'close me':
                        ਛਾਪੋ("closing")
                        self.request.close()
                        response = ""
                    ਜਾਂਜੇ decoded[1] == 'wait a bit':
                        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
                        response = "waited"
                    ਜਾਂਜੇ decoded[1] == '!quit!':
                        # we're done
                        self.server.shutdown()
                        ਵਾਪਸ
                    ਜਾਂਜੇ decoded[1] == '!crash!':
                        # Crash!
                        42 / 0
                    ਨਹੀਂਤਾਂ:
                        response = "what?"

                    ਜੇ response == "":
                        ਛਾਪੋ("no response")
                    ਨਹੀਂਤਾਂ:
                        encoded = json.dumps([decoded[0], response])
                        ਛਾਪੋ("sending: {0}".format(encoded))
                        self.request.sendall(encoded.encode('utf-8'))

                # Negative numbers are used for "eval" responses.
                ਜਾਂਜੇ decoded[0] < 0:
                    ਆਖਰੀ_eval = decoded

ਵਰਗ ThreadedTCPRequestHandler(TestingRequestHandler):
    ਪਰਿਭਾਸ਼ਾ ਤਿਆਰੀ(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

ਵਰਗ ThreadedTCPServer(socketserver.Threading_Mix_ਵਿੱਚ, socketserver.TCPServer):
    ਲੰਘੋ

ਪਰਿਭਾਸ਼ਾ write_ਪੋਰਟ_ਵਿੱਚ_File(ਪੋਰਟ):
    # Write the port number in Xportnr, so that the test knows it.
    f = open("Xportnr", "w")
    f.ਲਿਖ("{0}".format(ਪੋਰਟ))
    f.close()

ਪਰਿਭਾਸ਼ਾ ਮੁੱਖ(host, ਪੋਰਟ, server_class=ThreadedTCPServer):
    # Wait half a second before opening the port to test waittime in ch_open().
    # We do want to get the port number, get that first.  We cannot open the
    # socket, guess a port is free.
    ਜੇ ਲੰਬਾਈ(sys.argv) >= 2 ਅਤੇ sys.argv[1] == 'delay':
        ਪੋਰਟ = 13684
        write_ਪੋਰਟ_ਵਿੱਚ_File(ਪੋਰਟ)

        ਛਾਪੋ("Wait for it...")
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.5)

    addrs = socket.getaddrinfo(host, ਪੋਰਟ, 0, 0, socket.IPPROTO_TCP)
    # Each addr is a (family, type, proto, canonname, sockaddr) tuple
    sockaddr = addrs[0][4]
    server_class.ਪਤਾ_family = addrs[0][0]

    server = server_class(sockaddr[0:2], ThreadedTCPRequestHandler)
    ip, ਪੋਰਟ = server.server_ਪਤਾ[0:2]

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.ਧਾਗਾ(ਨਿਸ਼ਾਨਾ=server.serve_forever)
    server_thread.ਸ਼ੁਰੂ()

    write_ਪੋਰਟ_ਵਿੱਚ_File(ਪੋਰਟ)

    ਛਾਪੋ("Listening on port {0}".format(ਪੋਰਟ))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    ਕੋਸ਼ਿਸ਼:
        ਜਦੋਂ server_thread.is_alive():
            server_thread.join(1)
    ਅਪਵਾਦ (Keyboard_ਰੁਕਾਵਟ, SystemExit):
        server.shutdown()

ਜੇ __name__ == "__main__":
    ਮੁੱਖ("127.0.0.1", 0)
