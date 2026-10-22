#!/usr/bin/env python3

# Used by Test_channel_dap_mode in test_channel.vim to test DAP functionality.

आयात json
आयात socket
आयात threading
आयात time

प्रयत्न:
    आयात socketserver
अपवाद ImportError:
    आयात SocketServer इव socketserver

def make_dap_message(obj):
    payload = json.dumps(obj).encode("utf-8")
    header = f"Content-Length: {len(payload)}\r\n\r\n".encode("ascii")
    परत header + payload


def parse_messages(buffer):
    messages = []

    पर्यंत सत्य:
        hdr_end = buffer.find(b"\r\n\r\n")
        जर hdr_end == -1:
            थांबा

        header = buffer[:hdr_end].decode("ascii", errors="ignore")
        content_length = None

        साठी ओळ मध्ये header.split("\r\n"):
            जर ओळ.lower().startswith("content-length:"):
                content_length = पूर्णांक(ओळ.split(":")[1].strip())

        जर content_length is None:
            थांबा

        total_len = hdr_end + 4 + content_length
        जर len(buffer) < total_len:
            थांबा  # partial

        body = buffer[hdr_end + 4:total_len]
        messages.append(json.loads(body.decode("utf-8")))
        buffer = buffer[total_len:]

    परत messages, buffer


वर्ग DAPHandler(socketserver.BaseRequestHandler):

    def setup(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self.seq = 1  # server sequence counter

    def send(self, obj):
        obj["seq"] = self.seq
        self.seq += 1
        self.request.sendall(make_dap_message(obj))

    def send_response(self, request, body=None, success=सत्य):
        self.send({
            "type": "response",
            "request_seq": request["seq"],
            "success": success,
            "command": request["command"],
            "body": body किंवा {}
        })

    def send_event(self, event, body=None):
        self.send({
            "type": "event",
            "event": event,
            "body": body किंवा {}
        })

    def handle_request(self, संदेश):
        cmd = संदेश.get("command")

        जर cmd == "initialize":
            self.send_response(संदेश, {
                "supportsConfigurationDoneRequest": सत्य
            })
            self.send_event("initialized")
        अन्यथा:
            self.send_response(संदेश)

        परत सत्य

    def handle(self):
        buffer = b""

        पर्यंत सत्य:
            data = self.request.recv(4096)
            जर नाही data:
                थांबा

            buffer += data
            messages, buffer = parse_messages(buffer)

            साठी संदेश मध्ये messages:
                जर संदेश.get("type") == "request":
                    जर नाही self.handle_request(संदेश):
                        परत


वर्ग ThreadedDAPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = सत्य

def write_port_to_file(port, filename="Xportnr"):
    with open(filename, "w") इव f:
        f.लिख(str(port))

def main():
    server = ThreadedDAPServer(("localhost", 0), DAPHandler)

    # Get the actual assigned port
    ip, assigned_port = server.server_address

    # Write port so client/test can read it
    write_port_to_file(assigned_port)

    thread = threading.धागा(target=server.serve_forever)
    thread.daemon = सत्य
    thread.start()

    प्रयत्न:
        पर्यंत thread.is_alive():
            thread.join(1)
    अपवाद KeyboardInterrupt:
        server.shutdown()

जर __name__ == "__main__":
    main()

