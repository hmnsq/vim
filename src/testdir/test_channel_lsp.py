#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim to test LSP functionality.
#
# This requires Python 2.6 or later.

سے __future__ درآمد print_function
درآمد json
درآمد socket
درآمد sys
درآمد وقت
درآمد threading

کوشش:
    # Python 3
    درآمد socketserver
استثنا ImportError:
    # Python 2
    درآمد SocketServer بطور socketserver

جماعت ThreadedTCPRequestHandler(socketserver.بنیاد_Request_Handler):

    تعریف تیاری(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    تعریف debuglog(self, پیغام):
        اگر self.debug:
            ساتھ open("Xlspserver.log", "a") بطور myfile:
                myfile.اكتب(پیغام)

    تعریف send_lsp_req(self, msgid, طریقہ, params):
        v = {'jsonrpc': '2.0', 'id': msgid, 'method': طریقہ}
        اگر لمبائی(params) != 0:
            v['params'] = params
        s = json.dumps(v)
        req = "Content-Length: " + str(لمبائی(s)) + "\r\n"
        req += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        req += "\r\n"
        req += s
        اگر self.debug:
            self.debuglog("SEND: ({0} bytes) '{1}'\n".format(لمبائی(req), req))
        self.request.sendall(req.encode('utf-8'))

    تعریف send_lsp_resp(self, msgid, resp_dict):
        v = {'jsonrpc': '2.0', 'result': resp_dict}
        اگر msgid != -1:
            v['id'] = msgid
        s = json.dumps(v)
        resp = "Content-Length: " + str(لمبائی(s)) + "\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        اگر self.debug:
            self.debuglog("SEND: ({0} bytes) '{1}'\n".format(لمبائی(resp), resp))
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_wrong_payload(self):
        v = 'wrong-payload'
        s = json.dumps(v)
        resp = "Content-Length: " + str(لمبائی(s)) + "\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_خالی_خانے_سرخی(self, msgid, resp_dict):
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_خالی_خانے_payload(self):
        resp = "Content-Length: 0\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_extra_hdr_fields(self, msgid, resp_dict):
        # test for sending extra fields in the http header
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Host: abc.vim.org\r\n"
        resp += "User-Agent: Python\r\n"
        resp += "Accept-Language: en-US,en\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "Content-Length: " + str(لمبائی(s)) + "\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_delayed_payload(self, msgid, resp_dict):
        # test for sending the hdr first and then after some delay, send the
        # payload
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: " + str(لمبائی(s)) + "\r\n"
        resp += "\r\n"
        self.request.sendall(resp.encode('utf-8'))
        وقت.نوم(0.05)
        resp = s
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_hdr_without_len(self, msgid, resp_dict):
        # test for sending the http header without length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_hdr_with_wrong_len(self, msgid, resp_dict):
        # test for sending the http header with wrong length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: 1000\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    تعریف send_hdr_with_negative_len(self, msgid, resp_dict):
        # test for sending the http header with negative length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: -1\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    تعریف do_ping(self, payload):
        وقت.نوم(0.2)
        self.send_lsp_resp(payload['id'], 'alive')

    تعریف do_echo(self, payload):
        self.send_lsp_resp(-1, payload)

    تعریف do_simple_rpc(self, payload):
        # test for a simple RPC request
        self.send_lsp_resp(payload['id'], 'simple-rpc')

    تعریف do_rpc_with_notif(self, payload):
        # test for sending a notification before replying to a request message
        self.send_lsp_resp(-1, 'rpc-with-notif-notif')
        # sleep for some time to make sure the notification is delivered
        وقت.نوم(0.2)
        self.send_lsp_resp(payload['id'], 'rpc-with-notif-resp')

    تعریف do_wrong_payload(self, payload):
        # test for sending a non dict payload
        self.send_wrong_payload()
        وقت.نوم(0.2)
        self.send_lsp_resp(-1, 'wrong-payload')

    تعریف do_large_payload(self, payload):
        # test for sending a large (> 64K) payload
        self.send_lsp_resp(payload['id'], payload)

    تعریف do_rpc_resp_incorrect_id(self, payload):
        self.send_lsp_resp(-1, 'rpc-resp-incorrect-id-1')
        self.send_lsp_resp(-1, 'rpc-resp-incorrect-id-2')
        self.send_lsp_resp(1, 'rpc-resp-incorrect-id-3')
        وقت.نوم(0.2)
        self.send_lsp_resp(payload['id'], 'rpc-resp-incorrect-id-4')

    تعریف do_simple_notif(self, payload):
        # notification message test
        self.send_lsp_resp(-1, 'simple-notif')

    تعریف do_multi_notif(self, payload):
        # send multiple notifications
        self.send_lsp_resp(-1, 'multi-notif1')
        self.send_lsp_resp(-1, 'multi-notif2')

    تعریف do_پیغام_with_id(self, payload):
        self.send_lsp_resp(payload['id'], 'msg-with-id')

    تعریف do_پیغام_specific_cb(self, payload):
        self.send_lsp_resp(payload['id'], 'msg-specific-cb')

    تعریف do_server_req(self, payload):
        self.send_lsp_resp(201, {'method': 'checkhealth', 'params': {'a': 20}})

    تعریف do_extra_hdr_fields(self, payload):
        self.send_extra_hdr_fields(payload['id'], 'extra-hdr-fields')

    تعریف do_delayed_payload(self, payload):
        self.send_delayed_payload(payload['id'], 'delayed-payload')

    تعریف do_hdr_without_len(self, payload):
        self.send_hdr_without_len(payload['id'], 'hdr-without-len')

    تعریف do_hdr_with_wrong_len(self, payload):
        self.send_hdr_with_wrong_len(payload['id'], 'hdr-with-wrong-len')

    تعریف do_hdr_with_negative_len(self, payload):
        self.send_hdr_with_negative_len(payload['id'], 'hdr-with-negative-len')

    تعریف do_خالی_خانے_سرخی(self, payload):
        self.send_خالی_خانے_سرخی(payload['id'], 'empty-header')

    تعریف do_خالی_خانے_payload(self, payload):
        self.send_خالی_خانے_payload()

    تعریف do_server_req_اندر_درمیان(self, payload):
        # Send a notification message to the client in the middle of processing
        # a request message from the client
        self.send_lsp_req(-1, 'server-req-in-middle', {'text': 'server-notif'})
        # Send a request message to the client in the middle of processing a
        # request message from the client.
        self.send_lsp_req(payload['id'], 'server-req-in-middle', {'text': 'server-req'})

    تعریف do_server_req_اندر_درمیان_resp(self, payload):
        # After receiving a response from the client send the response to the
        # client request.
        self.send_lsp_resp(payload['id'], {'text': 'server-resp'})

    تعریف process_پیغام_2(self, پیغام):
        کوشش:
            decoded = json.loads(پیغام)
            اگر 'method' میں decoded:
                test_map = {
                        'ping': self.do_ping,
                        'echo': self.do_echo,
                        'simple-rpc': self.do_simple_rpc,
                        'rpc-with-notif': self.do_rpc_with_notif,
                        'wrong-payload': self.do_wrong_payload,
                        'large-payload': self.do_large_payload,
                        'rpc-resp-incorrect-id': self.do_rpc_resp_incorrect_id,
                        'simple-notif': self.do_simple_notif,
                        'multi-notif': self.do_multi_notif,
                        'msg-with-id': self.do_پیغام_with_id,
                        'msg-specific-cb': self.do_پیغام_specific_cb,
                        'server-req': self.do_server_req,
                        'extra-hdr-fields': self.do_extra_hdr_fields,
                        'delayed-payload': self.do_delayed_payload,
                        'hdr-without-len': self.do_hdr_without_len,
                        'hdr-with-wrong-len': self.do_hdr_with_wrong_len,
                        'hdr-with-negative-len': self.do_hdr_with_negative_len,
                        'empty-header': self.do_خالی_خانے_سرخی,
                        'empty-payload': self.do_خالی_خانے_payload,
                        'server-req-in-middle': self.do_server_req_اندر_درمیان,
                        'server-req-in-middle-resp': self.do_server_req_اندر_درمیان_resp,
                        }
                اگر decoded['method'] میں test_map:
                    test_map[decoded['method']](decoded)
                ورنہ:
                    self.debuglog("Error: Unsupported method - " + decoded['method'] + "\n")
            ورنہ:
                self.debuglog("Error: 'method' field is not found\n")

        استثنا قدر_Error:
            self.debuglog("Error: json decoding failed\n")

    تعریف process_msgs(self, msgbuf):
        جب سچ:
            sidx = msgbuf.ڈھونڈو('Content-Length: ')
            اگر sidx == -1:
                # partial message received
                واپس msgbuf
            sidx += 16
            eidx = msgbuf.ڈھونڈو('\r\n')
            اگر eidx == -1:
                # partial message received
                واپس msgbuf
            msglen = صحیح(msgbuf[sidx:eidx])

            hdrend = msgbuf.ڈھونڈو('\r\n\r\n')
            اگر hdrend == -1:
                # partial message received
                واپس msgbuf

            اگر msglen > لمبائی(msgbuf[hdrend + 4:]):
                اگر self.debug:
                    self.debuglog("Partial message ({0} bytes)\n".format(لمبائی(msgbuf)))
                # partial message received
                واپس msgbuf

            اگر self.debug:
                self.debuglog("Complete message ({0} bytes) received\n".format(msglen))

            # Remove the header
            msgbuf = msgbuf[hdrend + 4:]
            payload = msgbuf[:msglen]

            self.process_پیغام_2(payload)

            # Remove the processed message
            msgbuf = msgbuf[msglen:]

    تعریف handle(self):
        self.debug = جھوٹ
        self.debuglog("=== socket opened ===\n")
        msgbuf = ''
        جب سچ:
            کوشش:
                received = self.request.recv(4096).decode('utf-8')
            استثنا socket.error:
                self.debuglog("=== socket error ===\n")
                توڑو
            استثنا IOError:
                self.debuglog("=== socket closed ===\n")
                توڑو
            اگر received == '':
                self.debuglog("=== socket closed ===\n")
                توڑو

            # Write the received lines into the file for debugging
            اگر self.debug:
                self.debuglog("RECV: ({0} bytes) '{1}'\n".format(لمبائی(received), received))

            # Can receive more than one line in a response or a partial line.
            # Accumulate all the received characters and process one line at
            # a time.
            msgbuf += received
            msgbuf = self.process_msgs(msgbuf)

جماعت ThreadedTCPServer(socketserver.Threading_Mix_اندر, socketserver.TCPServer):
    گزرو

تعریف write_پورٹ_اندر_File(پورٹ):
    # Write the port number in Xportnr, so that the test knows it.
    f = open("Xportnr", "w")
    f.اكتب("{0}".format(پورٹ))
    f.close()

تعریف مین(host, پورٹ, server_class=ThreadedTCPServer):
    # Wait half a second before opening the port to test waittime in ch_open().
    # We do want to get the port number, get that first.  We cannot open the
    # socket, guess a port is free.
    اگر لمبائی(sys.argv) >= 2 اور sys.argv[1] == 'delay':
        پورٹ = 13684
        write_پورٹ_اندر_File(پورٹ)
        وقت.نوم(0.5)

    addrs = socket.getaddrinfo(host, پورٹ, 0, 0, socket.IPPROTO_TCP)
    # Each addr is a (family, type, proto, canonname, sockaddr) tuple
    sockaddr = addrs[0][4]
    server_class.پتہ_family = addrs[0][0]

    server = server_class(sockaddr[0:2], ThreadedTCPRequestHandler)
    ip, پورٹ = server.server_پتہ[0:2]

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.دھاگہ(ہدف=server.serve_forever)
    server_thread.آغاز()

    write_پورٹ_اندر_File(پورٹ)

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    کوشش:
        جب server_thread.is_alive():
            server_thread.join(1)
    استثنا (Keyboard_مداخلت, SystemExit):
        server.shutdown()

اگر __name__ == "__main__":
    مین("localhost", 0)
