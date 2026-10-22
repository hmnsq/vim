#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

તઃ test_channel આયાત મુખ્ય, ThreadedTCPServer
આયાત socket

વર્ગ ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

જો __name__ == "__main__":
    મુખ્ય("::", 0, ThreadedTCP6Server)
