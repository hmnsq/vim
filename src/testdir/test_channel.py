#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

ତଃ __future__ ଆମଦାନୀ print_function
ଆମଦାନୀ json
ଆମଦାନୀ socket
ଆମଦାନୀ sys
ଆମଦାନୀ time
ଆମଦାନୀ threading

ଚେଷ୍ଟା:
    # Python 3
    ଆମଦାନୀ socketserver
ବ୍ୟତିକ୍ରମ ImportError:
    # Python 2
    ଆମଦାନୀ SocketServer ଇଵ socketserver

ଶ୍ରେଣୀ TestingRequestHandler(socketserver.BaseRequestHandler):
    def handle(self):
        print("=== socket opened ===")
        ଯେତେବେଳେ ସତ୍ୟ:
            ଚେଷ୍ଟା:
                received = self.request.recv(4096).decode('utf-8')
            ବ୍ୟତିକ୍ରମ socket.error:
                print("=== socket error ===")
                ଭାଙ୍ଗ
            ବ୍ୟତିକ୍ରମ IOError:
                print("=== socket closed ===")
                ଭାଙ୍ଗ
            ଯଦି received == '':
                print("=== socket closed ===")
                ଭାଙ୍ଗ
            print("received: {0}".format(received))

            # We may receive two messages at once. Take the part up to the
            # newline, which should be after the matching "]".
            todo = received
            ଯେତେବେଳେ todo != '':
                splitidx = todo.find('\n')
                ଯଦି splitidx < 0:
                     used = todo
                     todo = ''
                ଅନ୍ୟଥା:
                     used = todo[:splitidx]
                     todo = todo[splitidx + 1:]
                ଯଦି used != received:
                    print("using: {0}".format(used))

                ଚେଷ୍ଟା:
                    decoded = json.loads(used)
                ବ୍ୟତିକ୍ରମ ମୂଲ୍ୟ_Error:
                    print("json decoding failed")
                    decoded = [-1, '']

                # Send a response if the sequence number is positive.
                ଯଦି decoded[0] >= 0:
                    ଯଦି decoded[1] == 'hello!':
                        # simply send back a string
                        response = "got it"
                    ନଚେତଯଦି decoded[1] == 'malformed1':
                        cmd = '["ex",":"]wrong!["ex","smi"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Need to wait for Vim to give up, otherwise it
                        # sometimes fails on OS X.
                        time.ସୁଷୁପ୍ତି(0.2)
                    ନଚେତଯଦି decoded[1] == 'malformed2':
                        cmd = '"unterminated string'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Need to wait for Vim to give up, otherwise the double
                        # quote in the "ok" response terminates the string.
                        time.ସୁଷୁପ୍ତି(0.2)
                    ନଚେତଯଦି decoded[1] == 'malformed3':
                        cmd = '["ex","missing ]"'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Need to wait for Vim to give up, otherwise the ]
                        # in the "ok" response terminates the list.
                        time.ସୁଷୁପ୍ତି(0.2)
                    ନଚେତଯଦି decoded[1] == 'split':
                        cmd = '["ex","let '
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        time.ସୁଷୁପ୍ତି(0.01)
                        cmd = 'g:split = 123"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1].startswith("echo "):
                        # send back the argument
                        response = decoded[1][5:]
                    ନଚେତଯଦି decoded[1] == 'make change':
                        # Send two ex commands at the same time, before
                        # replying to the request.
                        cmd = '["ex","call append(\\"$\\",\\"added1\\")"]'
                        cmd += '["ex","call append(\\"$\\",\\"added2\\")"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'echoerr':
                        cmd = '["ex","echoerr \\\"this is an error\\\""]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                        # Wait a bit, so that the "ex" command is handled
                        # before the "ch_evalexpr() returns.  Otherwise we are
                        # outside the try/catch when the "ex" command is
                        # handled.
                        time.ସୁଷୁପ୍ତି(0.02)
                    ନଚେତଯଦି decoded[1] == 'bad command':
                        cmd = '["ex","foo bar"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'do normal':
                        # Send a normal command.
                        cmd = '["normal","G$s more\u001b"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-works':
                        # Send an eval request.  We ignore the response.
                        cmd = '["expr","\\"foo\\" . 123", -1]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-special':
                        # Send an eval request.  We ignore the response.
                        cmd = '["expr","\\"foo\x7f\x10\x01bar\\"", -2]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-getline':
                        # Send an eval request.  We ignore the response.
                        cmd = '["expr","getline(3)", -3]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-fails':
                        # Send an eval request that will fail.
                        cmd = '["expr","xxx", -4]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-error':
                        # Send an eval request that works but the result can't
                        # be encoded.
                        cmd = '["expr","function(\\"tr\\")", -5]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-bad':
                        # Send an eval request missing the third argument.
                        cmd = '["expr","xxx"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'an expr':
                        # Send an expr request.
                        cmd = '["expr","setline(\\"$\\", [\\"one\\",\\"two\\",\\"three\\"])"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'call-func':
                        cmd = '["call","MyFunction",[1,2,3], 0]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'redraw':
                        cmd = '["redraw",""]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'redraw!':
                        cmd = '["redraw","force"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'empty-request':
                        cmd = '[]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'eval-result':
                        # Send back the last received eval result.
                        response = last_eval
                    ନଚେତଯଦି decoded[1] == 'call me':
                        cmd = '[0,"we called you"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "ok"
                    ନଚେତଯଦି decoded[1] == 'call me again':
                        cmd = '[0,"we did call you"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = ""
                    ନଚେତଯଦି decoded[1] == 'send zero':
                        cmd = '[0,"zero index"]'
                        print("sending: {0}".format(cmd))
                        self.request.sendall(cmd.encode('utf-8'))
                        response = "sent zero"
                    ନଚେତଯଦି decoded[1] == 'close me':
                        print("closing")
                        self.request.close()
                        response = ""
                    ନଚେତଯଦି decoded[1] == 'wait a bit':
                        time.ସୁଷୁପ୍ତି(0.2)
                        response = "waited"
                    ନଚେତଯଦି decoded[1] == '!quit!':
                        # we're done
                        self.server.shutdown()
                        ଫେରନ୍ତୁ
                    ନଚେତଯଦି decoded[1] == '!crash!':
                        # Crash!
                        42 / 0
                    ଅନ୍ୟଥା:
                        response = "what?"

                    ଯଦି response == "":
                        print("no response")
                    ଅନ୍ୟଥା:
                        encoded = json.dumps([decoded[0], response])
                        print("sending: {0}".format(encoded))
                        self.request.sendall(encoded.encode('utf-8'))

                # Negative numbers are used for "eval" responses.
                ନଚେତଯଦି decoded[0] < 0:
                    last_eval = decoded

ଶ୍ରେଣୀ ThreadedTCPRequestHandler(TestingRequestHandler):
    def setup(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

ଶ୍ରେଣୀ ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

def writePortInFile(port):
    # Write the port number in Xportnr, so that the test knows it.
    f = open("Xportnr", "w")
    f.ଲିଖ("{0}".format(port))
    f.close()

def ମୁଖ୍ୟ(host, port, server_class=ThreadedTCPServer):
    # Wait half a second before opening the port to test waittime in ch_open().
    # We do want to get the port number, get that first.  We cannot open the
    # socket, guess a port is free.
    ଯଦି len(sys.argv) >= 2 ଏବଂ sys.argv[1] == 'delay':
        port = 13684
        writePortInFile(port)

        print("Wait for it...")
        time.ସୁଷୁପ୍ତି(0.5)

    addrs = socket.getaddrinfo(host, port, 0, 0, socket.IPPROTO_TCP)
    # Each addr is a (family, type, proto, canonname, sockaddr) tuple
    sockaddr = addrs[0][4]
    server_class.address_family = addrs[0][0]

    server = server_class(sockaddr[0:2], ThreadedTCPRequestHandler)
    ip, port = server.server_address[0:2]

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.ସୂତା(target=server.serve_forever)
    server_thread.start()

    writePortInFile(port)

    print("Listening on port {0}".format(port))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    ଚେଷ୍ଟା:
        ଯେତେବେଳେ server_thread.is_alive():
            server_thread.join(1)
    ବ୍ୟତିକ୍ରମ (KeyboardInterrupt, SystemExit):
        server.shutdown()

ଯଦି __name__ == "__main__":
    ମୁଖ୍ୟ("127.0.0.1", 0)
