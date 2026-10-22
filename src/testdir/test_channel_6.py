#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

from test_channel import メインページ, ThreadedTCPServer
import socket

class ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

もし __name__ == "__main__":
    メインページ("::", 0, ThreadedTCP6Server)
