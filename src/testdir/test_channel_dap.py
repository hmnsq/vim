#!/usr/bin/env python3

# Used by Test_channel_dap_mode in test_channel.vim to test DAP functionality.

આયાત json
આયાત socket
આયાત threading
આયાત time

પ્રયાસ:
    આયાત socketserver
અપવાદ ImportError:
    આયાત SocketServer ઇવ socketserver

def make_dap_message(obj):
    payload = json.dumps(obj).encode("utf-8")
    header = f"Content-Length: {len(payload)}\r\n\r\n".encode("ascii")
    પાછા header + payload


def parse_messages(buffer):
    messages = []

    જ્યારે સાચું:
        hdr_end = buffer.find(b"\r\n\r\n")
        જો hdr_end == -1:
            તોડો

        header = buffer[:hdr_end].decode("ascii", errors="ignore")
        content_length = None

        માટે લીટી માં header.split("\r\n"):
            જો લીટી.lower().startswith("content-length:"):
                content_length = પૂર્ણાંક(લીટી.split(":")[1].strip())

        જો content_length is None:
            તોડો

        total_len = hdr_end + 4 + content_length
        જો len(buffer) < total_len:
            તોડો  # partial

        body = buffer[hdr_end + 4:total_len]
        messages.append(json.loads(body.decode("utf-8")))
        buffer = buffer[total_len:]

    પાછા messages, buffer


વર્ગ DAPHandler(socketserver.BaseRequestHandler):

    def setup(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self.seq = 1  # server sequence counter

    def send(self, obj):
        obj["seq"] = self.seq
        self.seq += 1
        self.request.sendall(make_dap_message(obj))

    def send_response(self, request, body=None, success=સાચું):
        self.send({
            "type": "response",
            "request_seq": request["seq"],
            "success": success,
            "command": request["command"],
            "body": body અથવા {}
        })

    def send_event(self, event, body=None):
        self.send({
            "type": "event",
            "event": event,
            "body": body અથવા {}
        })

    def handle_request(self, સંદેશ):
        cmd = સંદેશ.get("command")

        જો cmd == "initialize":
            self.send_response(સંદેશ, {
                "supportsConfigurationDoneRequest": સાચું
            })
            self.send_event("initialized")
        નહીંતર:
            self.send_response(સંદેશ)

        પાછા સાચું

    def handle(self):
        buffer = b""

        જ્યારે સાચું:
            data = self.request.recv(4096)
            જો નહીં data:
                તોડો

            buffer += data
            messages, buffer = parse_messages(buffer)

            માટે સંદેશ માં messages:
                જો સંદેશ.get("type") == "request":
                    જો નહીં self.handle_request(સંદેશ):
                        પાછા


વર્ગ ThreadedDAPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = સાચું

def write_port_to_file(port, filename="Xportnr"):
    with open(filename, "w") ઇવ f:
        f.લિખ(str(port))

def મુખ્ય():
    server = ThreadedDAPServer(("localhost", 0), DAPHandler)

    # Get the actual assigned port
    ip, assigned_port = server.server_address

    # Write port so client/test can read it
    write_port_to_file(assigned_port)

    thread = threading.તંતુ(target=server.serve_forever)
    thread.daemon = સાચું
    thread.start()

    પ્રયાસ:
        જ્યારે thread.is_alive():
            thread.join(1)
    અપવાદ KeyboardInterrupt:
        server.shutdown()

જો __name__ == "__main__":
    મુખ્ય()

