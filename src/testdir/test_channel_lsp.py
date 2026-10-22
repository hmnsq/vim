#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim to test LSP functionality.
#
# This requires Python 2.6 or later.

తః __future__ దిగుమతి print_function
దిగుమతి json
దిగుమతి socket
దిగుమతి sys
దిగుమతి సమయం
దిగుమతి threading

ప్రయత్నించు:
    # Python 3
    దిగుమతి socketserver
మినహాయింపు ImportError:
    # Python 2
    దిగుమతి SocketServer ఇవ socketserver

తరగతి ThreadedTCPRequestHandler(socketserver.పునాది_Request_Handler):

    నిర్వచనం అమరిక(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    నిర్వచనం debuglog(self, సందేశం):
        ఒకవేళ self.debug:
            తోడుగా open("Xlspserver.log", "a") ఇవ myfile:
                myfile.లిఖ(సందేశం)

    నిర్వచనం send_lsp_req(self, msgid, పద్ధతి, params):
        v = {'jsonrpc': '2.0', 'id': msgid, 'method': పద్ధతి}
        ఒకవేళ పొడవు(params) != 0:
            v['params'] = params
        s = json.dumps(v)
        req = "Content-Length: " + str(పొడవు(s)) + "\r\n"
        req += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        req += "\r\n"
        req += s
        ఒకవేళ self.debug:
            self.debuglog("SEND: ({0} bytes) '{1}'\n".format(పొడవు(req), req))
        self.request.sendall(req.encode('utf-8'))

    నిర్వచనం send_lsp_resp(self, msgid, resp_dict):
        v = {'jsonrpc': '2.0', 'result': resp_dict}
        ఒకవేళ msgid != -1:
            v['id'] = msgid
        s = json.dumps(v)
        resp = "Content-Length: " + str(పొడవు(s)) + "\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        ఒకవేళ self.debug:
            self.debuglog("SEND: ({0} bytes) '{1}'\n".format(పొడవు(resp), resp))
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_wrong_payload(self):
        v = 'wrong-payload'
        s = json.dumps(v)
        resp = "Content-Length: " + str(పొడవు(s)) + "\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_ఖాళీలు_శీర్షిక(self, msgid, resp_dict):
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_ఖాళీలు_payload(self):
        resp = "Content-Length: 0\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_extra_hdr_fields(self, msgid, resp_dict):
        # test for sending extra fields in the http header
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Host: abc.vim.org\r\n"
        resp += "User-Agent: Python\r\n"
        resp += "Accept-Language: en-US,en\r\n"
        resp += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "Content-Length: " + str(పొడవు(s)) + "\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_delayed_payload(self, msgid, resp_dict):
        # test for sending the hdr first and then after some delay, send the
        # payload
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: " + str(పొడవు(s)) + "\r\n"
        resp += "\r\n"
        self.request.sendall(resp.encode('utf-8'))
        సమయం.సుషుప్తి(0.05)
        resp = s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_hdr_without_len(self, msgid, resp_dict):
        # test for sending the http header without length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_hdr_with_wrong_len(self, msgid, resp_dict):
        # test for sending the http header with wrong length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: 1000\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం send_hdr_with_negative_len(self, msgid, resp_dict):
        # test for sending the http header with negative length
        v = {'jsonrpc': '2.0', 'id': msgid, 'result': resp_dict}
        s = json.dumps(v)
        resp = "Content-Length: -1\r\n"
        resp += "\r\n"
        resp += s
        self.request.sendall(resp.encode('utf-8'))

    నిర్వచనం do_ping(self, payload):
        సమయం.సుషుప్తి(0.2)
        self.send_lsp_resp(payload['id'], 'alive')

    నిర్వచనం do_echo(self, payload):
        self.send_lsp_resp(-1, payload)

    నిర్వచనం do_simple_rpc(self, payload):
        # test for a simple RPC request
        self.send_lsp_resp(payload['id'], 'simple-rpc')

    నిర్వచనం do_rpc_with_notif(self, payload):
        # test for sending a notification before replying to a request message
        self.send_lsp_resp(-1, 'rpc-with-notif-notif')
        # sleep for some time to make sure the notification is delivered
        సమయం.సుషుప్తి(0.2)
        self.send_lsp_resp(payload['id'], 'rpc-with-notif-resp')

    నిర్వచనం do_wrong_payload(self, payload):
        # test for sending a non dict payload
        self.send_wrong_payload()
        సమయం.సుషుప్తి(0.2)
        self.send_lsp_resp(-1, 'wrong-payload')

    నిర్వచనం do_large_payload(self, payload):
        # test for sending a large (> 64K) payload
        self.send_lsp_resp(payload['id'], payload)

    నిర్వచనం do_rpc_resp_incorrect_id(self, payload):
        self.send_lsp_resp(-1, 'rpc-resp-incorrect-id-1')
        self.send_lsp_resp(-1, 'rpc-resp-incorrect-id-2')
        self.send_lsp_resp(1, 'rpc-resp-incorrect-id-3')
        సమయం.సుషుప్తి(0.2)
        self.send_lsp_resp(payload['id'], 'rpc-resp-incorrect-id-4')

    నిర్వచనం do_simple_notif(self, payload):
        # notification message test
        self.send_lsp_resp(-1, 'simple-notif')

    నిర్వచనం do_multi_notif(self, payload):
        # send multiple notifications
        self.send_lsp_resp(-1, 'multi-notif1')
        self.send_lsp_resp(-1, 'multi-notif2')

    నిర్వచనం do_సందేశం_with_id(self, payload):
        self.send_lsp_resp(payload['id'], 'msg-with-id')

    నిర్వచనం do_సందేశం_specific_cb(self, payload):
        self.send_lsp_resp(payload['id'], 'msg-specific-cb')

    నిర్వచనం do_server_req(self, payload):
        self.send_lsp_resp(201, {'method': 'checkhealth', 'params': {'a': 20}})

    నిర్వచనం do_extra_hdr_fields(self, payload):
        self.send_extra_hdr_fields(payload['id'], 'extra-hdr-fields')

    నిర్వచనం do_delayed_payload(self, payload):
        self.send_delayed_payload(payload['id'], 'delayed-payload')

    నిర్వచనం do_hdr_without_len(self, payload):
        self.send_hdr_without_len(payload['id'], 'hdr-without-len')

    నిర్వచనం do_hdr_with_wrong_len(self, payload):
        self.send_hdr_with_wrong_len(payload['id'], 'hdr-with-wrong-len')

    నిర్వచనం do_hdr_with_negative_len(self, payload):
        self.send_hdr_with_negative_len(payload['id'], 'hdr-with-negative-len')

    నిర్వచనం do_ఖాళీలు_శీర్షిక(self, payload):
        self.send_ఖాళీలు_శీర్షిక(payload['id'], 'empty-header')

    నిర్వచనం do_ఖాళీలు_payload(self, payload):
        self.send_ఖాళీలు_payload()

    నిర్వచనం do_server_req_లోపల_మధ్య(self, payload):
        # Send a notification message to the client in the middle of processing
        # a request message from the client
        self.send_lsp_req(-1, 'server-req-in-middle', {'text': 'server-notif'})
        # Send a request message to the client in the middle of processing a
        # request message from the client.
        self.send_lsp_req(payload['id'], 'server-req-in-middle', {'text': 'server-req'})

    నిర్వచనం do_server_req_లోపల_మధ్య_resp(self, payload):
        # After receiving a response from the client send the response to the
        # client request.
        self.send_lsp_resp(payload['id'], {'text': 'server-resp'})

    నిర్వచనం process_సందేశం_2(self, సందేశం):
        ప్రయత్నించు:
            decoded = json.loads(సందేశం)
            ఒకవేళ 'method' లో decoded:
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
                        'msg-with-id': self.do_సందేశం_with_id,
                        'msg-specific-cb': self.do_సందేశం_specific_cb,
                        'server-req': self.do_server_req,
                        'extra-hdr-fields': self.do_extra_hdr_fields,
                        'delayed-payload': self.do_delayed_payload,
                        'hdr-without-len': self.do_hdr_without_len,
                        'hdr-with-wrong-len': self.do_hdr_with_wrong_len,
                        'hdr-with-negative-len': self.do_hdr_with_negative_len,
                        'empty-header': self.do_ఖాళీలు_శీర్షిక,
                        'empty-payload': self.do_ఖాళీలు_payload,
                        'server-req-in-middle': self.do_server_req_లోపల_మధ్య,
                        'server-req-in-middle-resp': self.do_server_req_లోపల_మధ్య_resp,
                        }
                ఒకవేళ decoded['method'] లో test_map:
                    test_map[decoded['method']](decoded)
                లేకపోతే:
                    self.debuglog("Error: Unsupported method - " + decoded['method'] + "\n")
            లేకపోతే:
                self.debuglog("Error: 'method' field is not found\n")

        మినహాయింపు విలువ_Error:
            self.debuglog("Error: json decoding failed\n")

    నిర్వచనం process_msgs(self, msgbuf):
        అయితే నిజం:
            sidx = msgbuf.కనుగొను('Content-Length: ')
            ఒకవేళ sidx == -1:
                # partial message received
                తిరిగి msgbuf
            sidx += 16
            eidx = msgbuf.కనుగొను('\r\n')
            ఒకవేళ eidx == -1:
                # partial message received
                తిరిగి msgbuf
            msglen = పూర్ణాంకం(msgbuf[sidx:eidx])

            hdrend = msgbuf.కనుగొను('\r\n\r\n')
            ఒకవేళ hdrend == -1:
                # partial message received
                తిరిగి msgbuf

            ఒకవేళ msglen > పొడవు(msgbuf[hdrend + 4:]):
                ఒకవేళ self.debug:
                    self.debuglog("Partial message ({0} bytes)\n".format(పొడవు(msgbuf)))
                # partial message received
                తిరిగి msgbuf

            ఒకవేళ self.debug:
                self.debuglog("Complete message ({0} bytes) received\n".format(msglen))

            # Remove the header
            msgbuf = msgbuf[hdrend + 4:]
            payload = msgbuf[:msglen]

            self.process_సందేశం_2(payload)

            # Remove the processed message
            msgbuf = msgbuf[msglen:]

    నిర్వచనం handle(self):
        self.debug = అబద్ధం
        self.debuglog("=== socket opened ===\n")
        msgbuf = ''
        అయితే నిజం:
            ప్రయత్నించు:
                received = self.request.recv(4096).decode('utf-8')
            మినహాయింపు socket.error:
                self.debuglog("=== socket error ===\n")
                ఆపు
            మినహాయింపు IOError:
                self.debuglog("=== socket closed ===\n")
                ఆపు
            ఒకవేళ received == '':
                self.debuglog("=== socket closed ===\n")
                ఆపు

            # Write the received lines into the file for debugging
            ఒకవేళ self.debug:
                self.debuglog("RECV: ({0} bytes) '{1}'\n".format(పొడవు(received), received))

            # Can receive more than one line in a response or a partial line.
            # Accumulate all the received characters and process one line at
            # a time.
            msgbuf += received
            msgbuf = self.process_msgs(msgbuf)

తరగతి ThreadedTCPServer(socketserver.Threading_Mix_లోపల, socketserver.TCPServer):
    దాటు

నిర్వచనం write_పోర్టు_లోపల_File(పోర్టు):
    # Write the port number in Xportnr, so that the test knows it.
    f = open("Xportnr", "w")
    f.లిఖ("{0}".format(పోర్టు))
    f.close()

నిర్వచనం ప్రధాన(host, పోర్టు, server_class=ThreadedTCPServer):
    # Wait half a second before opening the port to test waittime in ch_open().
    # We do want to get the port number, get that first.  We cannot open the
    # socket, guess a port is free.
    ఒకవేళ పొడవు(sys.argv) >= 2 మరియు sys.argv[1] == 'delay':
        పోర్టు = 13684
        write_పోర్టు_లోపల_File(పోర్టు)
        సమయం.సుషుప్తి(0.5)

    addrs = socket.getaddrinfo(host, పోర్టు, 0, 0, socket.IPPROTO_TCP)
    # Each addr is a (family, type, proto, canonname, sockaddr) tuple
    sockaddr = addrs[0][4]
    server_class.చిరునామా_family = addrs[0][0]

    server = server_class(sockaddr[0:2], ThreadedTCPRequestHandler)
    ip, పోర్టు = server.server_చిరునామా[0:2]

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.తంతువు(లక్ష్యం=server.serve_forever)
    server_thread.ప్రారంభం()

    write_పోర్టు_లోపల_File(పోర్టు)

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    ప్రయత్నించు:
        అయితే server_thread.is_alive():
            server_thread.join(1)
    మినహాయింపు (Keyboard_అంతరాయం, SystemExit):
        server.shutdown()

ఒకవేళ __name__ == "__main__":
    ప్రధాన("localhost", 0)
