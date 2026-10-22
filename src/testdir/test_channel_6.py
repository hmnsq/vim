#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

from test_channel import основной, ThreadedTCPServer
import socket

class ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

если __name__ == "__main__":
    основной("::", 0, ThreadedTCP6Server)
