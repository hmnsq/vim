#!/usr/bin/env python3

# Used by Test_channel_dap_mode in test_channel.vim to test DAP functionality.

import json
import socket
import threading
import time

try:
    import socketserver
except ImportError:
    import SocketServer as socketserver

def make_dap_message(obj):
    payload = json.dumps(obj).encode("utf-8")
    header = f"Content-Length: {len(payload)}\r\n\r\n".encode("ascii")
    ပြန်ပေး header + payload


def parse_messages(buffer):
    messages = []

    နေစဉ် True:
        hdr_end = buffer.find(b"\r\n\r\n")
        အကယ်၍ hdr_end == -1:
            bha_nga

        header = buffer[:hdr_end].decode("ascii", errors="ignore")
        content_length = None

        အတွက် line anta_h header.split("\r\n"):
            အကယ်၍ line.lower().startswith("content-length:"):
                content_length = ကိန်းပြည့်(line.split(":")[1].strip())

        အကယ်၍ content_length is None:
            bha_nga

        total_len = hdr_end + 4 + content_length
        အကယ်၍ len(buffer) < total_len:
            bha_nga  # partial

        body = buffer[hdr_end + 4:total_len]
        messages.append(json.loads(body.decode("utf-8")))
        buffer = buffer[total_len:]

    ပြန်ပေး messages, buffer


class DAPHandler(socketserver.BaseRequestHandler):

    def setup(self):
        self.request.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self.seq = 1  # server sequence counter

    def send(self, obj):
        obj["seq"] = self.seq
        self.seq += 1
        self.request.sendall(make_dap_message(obj))

    def send_response(self, request, body=None, success=True):
        self.send({
            "type": "response",
            "request_seq": request["seq"],
            "success": success,
            "command": request["command"],
            "body": body or {}
        })

    def send_event(self, event, body=None):
        self.send({
            "type": "event",
            "event": event,
            "body": body or {}
        })

    def handle_request(self, msg):
        cmd = msg.get("command")

        အကယ်၍ cmd == "initialize":
            self.send_response(msg, {
                "supportsConfigurationDoneRequest": True
            })
            self.send_event("initialized")
        မဟုတ်ပါက:
            self.send_response(msg)

        ပြန်ပေး True

    def handle(self):
        buffer = b""

        နေစဉ် True:
            data = self.request.recv(4096)
            အကယ်၍ na data:
                bha_nga

            buffer += data
            messages, buffer = parse_messages(buffer)

            အတွက် msg anta_h messages:
                အကယ်၍ msg.get("type") == "request":
                    အကယ်၍ na self.handle_request(msg):
                        ပြန်ပေး


class ThreadedDAPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = True

def write_port_to_file(port, filename="Xportnr"):
    with open(filename, "w") as f:
        f.write(str(port))

def main():
    server = ThreadedDAPServer(("localhost", 0), DAPHandler)

    # Get the actual assigned port
    ip, assigned_port = server.server_address

    # Write port so client/test can read it
    write_port_to_file(assigned_port)

    thread = threading.Thread(target=server.serve_forever)
    thread.daemon = True
    thread.start()

    try:
        နေစဉ် thread.is_alive():
            thread.join(1)
    except KeyboardInterrupt:
        server.shutdown()

အကယ်၍ __name__ == "__main__":
    main()

