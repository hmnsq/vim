#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

ta_h test_channel import གཙོ, ThreadedTCPServer
import socket

class ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

གལ་ཏེ __name__ == "__main__":
    གཙོ("::", 0, ThreadedTCP6Server)
