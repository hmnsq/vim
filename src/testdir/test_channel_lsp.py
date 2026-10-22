#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim to test LSP functionality.
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

ਵਰਗ ThreadedTCPRequestHandler(socketserver.ਅਧਾਰ_Request_Handler):

    ਪਰਿਭਾਸ਼ਾ ਤਿਆਰੀ(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    ਪਰਿਭਾਸ਼ਾ debuglog(self, ਸੁਨੇਹਾ):
        ਜੇ self.debug:
            ਨਾਲ open("Xlspserver.log", "a") ਇਵ myfile:
                myfile.ਲਿਖ(ਸੁਨੇਹਾ)

    ਪਰਿਭਾਸ਼ਾ send_lsp_req(self, msgid, ਵਿਧੀ, params):
        v = {'jsonrpc': '2.0', 'id': msgid, 'method': ਵਿਧੀ}
        ਜੇ ਲੰਬਾਈ(params) != 0:
            v['params'] = params
        s = json.dumps(v)
        req = "Content-Length: " + str(ਲੰਬਾਈ(s)) + "\r\n"
        req += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        req += "\r\n"
        req += s
        ਜੇ self.debug:
            self.debuglog("SEND: ({0} bytes) '{1}'\n".format(ਲੰਬਾਈ(req), req))
        self.request.sendall(req.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_lsp_resp(self, msgid, resp_dict):
        v = {'jsonrpc': '2.0', 'result': resp_dict}
        ਜੇ msgid != -1:
            v['id'] = msgid
        s = json.dumps(v)
        resp = "Content-Length: " + str(ਲੰਬਾਈ(s)) + "\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        ਜੇ self.debug:
            self.debuglog("SEND: ({0} bytes) '{1}'\n".format(ਲੰਬਾਈ(resp), resp))
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_wrong_payload(self):
        v = 'wrong-payload'
        s = json.dumps(v)
        resp = "Content-Length: " + str(ਲੰਬਾਈ(s)) + "\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_ਖਾਲੀ_ਥਾਵਾਂ_ਸਿਰਲੇਖ(self, msgid, resp_dict):
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_ਖਾਲੀ_ਥਾਵਾਂ_payload(self):
        resp = "Content-Length: 0\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_extra_hdr_fields(self, msgid, resp_dict):
        # test for sending extra fields in the http header
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Host: abc.vim.org\r\n"
        resp += "User-Agent: Python\r\n"
        resp += "Accept-Language: en-US,en\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "Content-Length: " + str(ਲੰਬਾਈ(s)) + "\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_delayed_payload(self, msgid, resp_dict):
        # test for sending the hdr first and then after some delay, send the
        # payload
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: " + str(ਲੰਬਾਈ(s)) + "\r\n"
        resp += "\r\n"
        self.request.sendall(resp.encode('utf-8'))
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.05)
        resp = s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_hdr_without_len(self, msgid, resp_dict):
        # test for sending the http header without length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_hdr_with_wrong_len(self, msgid, resp_dict):
        # test for sending the http header with wrong length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: 1000\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ send_hdr_with_negative_len(self, msgid, resp_dict):
        # test for sending the http header with negative length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: -1\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    ਪਰਿਭਾਸ਼ਾ do_ping(self, payload):
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
        self.send_lsp_resp(payload['id'], 'alive')

    ਪਰਿਭਾਸ਼ਾ do_echo(self, payload):
        self.send_lsp_resp(-1, payload)

    ਪਰਿਭਾਸ਼ਾ do_simple_rpc(self, payload):
        # test for a simple RPC request
        self.send_lsp_resp(payload['id'], 'simple-rpc')

    ਪਰਿਭਾਸ਼ਾ do_rpc_with_notif(self, payload):
        # test for sending a notification before replying to a request message
        self.send_lsp_resp(-1, 'rpc-with-notif-notif')
        # sleep for some time to make sure the notification is delivered
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
        self.send_lsp_resp(payload['id'], 'rpc-with-notif-resp')

    ਪਰਿਭਾਸ਼ਾ do_wrong_payload(self, payload):
        # test for sending a non dict payload
        self.send_wrong_payload()
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
        self.send_lsp_resp(-1, 'wrong-payload')

    ਪਰਿਭਾਸ਼ਾ do_large_payload(self, payload):
        # test for sending a large (> 64K) payload
        self.send_lsp_resp(payload['id'], payload)

    ਪਰਿਭਾਸ਼ਾ do_rpc_resp_incorrect_id(self, payload):
        self.send_lsp_resp(-1, 'rpc-resp-incorrect-id-1')
        self.send_lsp_resp(-1, 'rpc-resp-incorrect-id-2')
        self.send_lsp_resp(1, 'rpc-resp-incorrect-id-3')
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.2)
        self.send_lsp_resp(payload['id'], 'rpc-resp-incorrect-id-4')

    ਪਰਿਭਾਸ਼ਾ do_simple_notif(self, payload):
        # notification message test
        self.send_lsp_resp(-1, 'simple-notif')

    ਪਰਿਭਾਸ਼ਾ do_multi_notif(self, payload):
        # send multiple notifications
        self.send_lsp_resp(-1, 'multi-notif1')
        self.send_lsp_resp(-1, 'multi-notif2')

    ਪਰਿਭਾਸ਼ਾ do_ਸੁਨੇਹਾ_with_id(self, payload):
        self.send_lsp_resp(payload['id'], 'msg-with-id')

    ਪਰਿਭਾਸ਼ਾ do_ਸੁਨੇਹਾ_specific_cb(self, payload):
        self.send_lsp_resp(payload['id'], 'msg-specific-cb')

    ਪਰਿਭਾਸ਼ਾ do_server_req(self, payload):
        self.send_lsp_resp(201, {'method': 'checkhealth', 'params': {'a': 20}})

    ਪਰਿਭਾਸ਼ਾ do_extra_hdr_fields(self, payload):
        self.send_extra_hdr_fields(payload['id'], 'extra-hdr-fields')

    ਪਰਿਭਾਸ਼ਾ do_delayed_payload(self, payload):
        self.send_delayed_payload(payload['id'], 'delayed-payload')

    ਪਰਿਭਾਸ਼ਾ do_hdr_without_len(self, payload):
        self.send_hdr_without_len(payload['id'], 'hdr-without-len')

    ਪਰਿਭਾਸ਼ਾ do_hdr_with_wrong_len(self, payload):
        self.send_hdr_with_wrong_len(payload['id'], 'hdr-with-wrong-len')

    ਪਰਿਭਾਸ਼ਾ do_hdr_with_negative_len(self, payload):
        self.send_hdr_with_negative_len(payload['id'], 'hdr-with-negative-len')

    ਪਰਿਭਾਸ਼ਾ do_ਖਾਲੀ_ਥਾਵਾਂ_ਸਿਰਲੇਖ(self, payload):
        self.send_ਖਾਲੀ_ਥਾਵਾਂ_ਸਿਰਲੇਖ(payload['id'], 'empty-header')

    ਪਰਿਭਾਸ਼ਾ do_ਖਾਲੀ_ਥਾਵਾਂ_payload(self, payload):
        self.send_ਖਾਲੀ_ਥਾਵਾਂ_payload()

    ਪਰਿਭਾਸ਼ਾ do_server_req_ਵਿੱਚ_ਵਿਚਕਾਰ(self, payload):
        # Send a notification message to the client in the middle of processing
        # a request message from the client
        self.send_lsp_req(-1, 'server-req-in-middle', {'text': 'server-notif'})
        # Send a request message to the client in the middle of processing a
        # request message from the client.
        self.send_lsp_req(payload['id'], 'server-req-in-middle', {'text': 'server-req'})

    ਪਰਿਭਾਸ਼ਾ do_server_req_ਵਿੱਚ_ਵਿਚਕਾਰ_resp(self, payload):
        # After receiving a response from the client send the response to the
        # client request.
        self.send_lsp_resp(payload['id'], {'text': 'server-resp'})

    ਪਰਿਭਾਸ਼ਾ process_ਸੁਨੇਹਾ_2(self, ਸੁਨੇਹਾ):
        ਕੋਸ਼ਿਸ਼:
            decoded = json.loads(ਸੁਨੇਹਾ)
            ਜੇ 'method' ਵਿੱਚ decoded:
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
                        'msg-with-id': self.do_ਸੁਨੇਹਾ_with_id,
                        'msg-specific-cb': self.do_ਸੁਨੇਹਾ_specific_cb,
                        'server-req': self.do_server_req,
                        'extra-hdr-fields': self.do_extra_hdr_fields,
                        'delayed-payload': self.do_delayed_payload,
                        'hdr-without-len': self.do_hdr_without_len,
                        'hdr-with-wrong-len': self.do_hdr_with_wrong_len,
                        'hdr-with-negative-len': self.do_hdr_with_negative_len,
                        'empty-header': self.do_ਖਾਲੀ_ਥਾਵਾਂ_ਸਿਰਲੇਖ,
                        'empty-payload': self.do_ਖਾਲੀ_ਥਾਵਾਂ_payload,
                        'server-req-in-middle': self.do_server_req_ਵਿੱਚ_ਵਿਚਕਾਰ,
                        'server-req-in-middle-resp': self.do_server_req_ਵਿੱਚ_ਵਿਚਕਾਰ_resp,
                        }
                ਜੇ decoded['method'] ਵਿੱਚ test_map:
                    test_map[decoded['method']](decoded)
                ਨਹੀਂਤਾਂ:
                    self.debuglog("Error: Unsupported method - " + decoded['method'] + "\n")
            ਨਹੀਂਤਾਂ:
                self.debuglog("Error: 'method' field is not found\n")

        ਅਪਵਾਦ ਮੁੱਲ_Error:
            self.debuglog("Error: json decoding failed\n")

    ਪਰਿਭਾਸ਼ਾ process_msgs(self, msgbuf):
        ਜਦੋਂ ਸੱਚ:
            sidx = msgbuf.ਲੱਭੋ('Content-Length: ')
            ਜੇ sidx == -1:
                # partial message received
                ਵਾਪਸ msgbuf
            sidx += 16
            eidx = msgbuf.ਲੱਭੋ('\r\n')
            ਜੇ eidx == -1:
                # partial message received
                ਵਾਪਸ msgbuf
            msglen = ਪੂਰਨ(msgbuf[sidx:eidx])

            hdrend = msgbuf.ਲੱਭੋ('\r\n\r\n')
            ਜੇ hdrend == -1:
                # partial message received
                ਵਾਪਸ msgbuf

            ਜੇ msglen > ਲੰਬਾਈ(msgbuf[hdrend + 4:]):
                ਜੇ self.debug:
                    self.debuglog("Partial message ({0} bytes)\n".format(ਲੰਬਾਈ(msgbuf)))
                # partial message received
                ਵਾਪਸ msgbuf

            ਜੇ self.debug:
                self.debuglog("Complete message ({0} bytes) received\n".format(msglen))

            # Remove the header
            msgbuf = msgbuf[hdrend + 4:]
            payload = msgbuf[:msglen]

            self.process_ਸੁਨੇਹਾ_2(payload)

            # Remove the processed message
            msgbuf = msgbuf[msglen:]

    ਪਰਿਭਾਸ਼ਾ handle(self):
        self.debug = ਝੂਠ
        self.debuglog("=== socket opened ===\n")
        msgbuf = ''
        ਜਦੋਂ ਸੱਚ:
            ਕੋਸ਼ਿਸ਼:
                received = self.request.recv(4096).decode('utf-8')
            ਅਪਵਾਦ socket.error:
                self.debuglog("=== socket error ===\n")
                ਰੋਕੋ
            ਅਪਵਾਦ IOError:
                self.debuglog("=== socket closed ===\n")
                ਰੋਕੋ
            ਜੇ received == '':
                self.debuglog("=== socket closed ===\n")
                ਰੋਕੋ

            # Write the received lines into the file for debugging
            ਜੇ self.debug:
                self.debuglog("RECV: ({0} bytes) '{1}'\n".format(ਲੰਬਾਈ(received), received))

            # Can receive more than one line in a response or a partial line.
            # Accumulate all the received characters and process one line at
            # a time.
            msgbuf += received
            msgbuf = self.process_msgs(msgbuf)

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

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    ਕੋਸ਼ਿਸ਼:
        ਜਦੋਂ server_thread.is_alive():
            server_thread.join(1)
    ਅਪਵਾਦ (Keyboard_ਰੁਕਾਵਟ, SystemExit):
        server.shutdown()

ਜੇ __name__ == "__main__":
    ਮੁੱਖ("localhost", 0)
