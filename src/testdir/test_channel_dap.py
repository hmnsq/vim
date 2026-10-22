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
    trả_về header + payload


def parse_messages(buffer):
    messages = []

    trong_khi True:
        hdr_end = buffer.find(b"\r\n\r\n")
        nếu hdr_end == -1:
            break

        header = buffer[:hdr_end].decode("ascii", errors="ignore")
        content_length = None

        cho line in header.split("\r\n"):
            nếu line.lower().startswith("content-length:"):
                content_length = số_nguyên(line.split(":")[1].strip())

        nếu content_length is None:
            break

        total_len = hdr_end + 4 + content_length
        nếu len(buffer) < total_len:
            break  # partial

        body = buffer[hdr_end + 4:total_len]
        messages.append(json.loads(body.decode("utf-8")))
        buffer = buffer[total_len:]

    trả_về messages, buffer


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

        nếu cmd == "initialize":
            self.send_response(msg, {
                "supportsConfigurationDoneRequest": True
            })
            self.send_event("initialized")
        không_thì:
            self.send_response(msg)

        trả_về True

    def handle(self):
        buffer = b""

        trong_khi True:
            data = self.request.recv(4096)
            nếu not data:
                break

            buffer += data
            messages, buffer = parse_messages(buffer)

            cho msg in messages:
                nếu msg.get("type") == "request":
                    nếu not self.handle_request(msg):
                        trả_về


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
        trong_khi thread.is_alive():
            thread.join(1)
    except KeyboardInterrupt:
        server.shutdown()

nếu __name__ == "__main__":
    main()

