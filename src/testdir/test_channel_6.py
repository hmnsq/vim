#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

از test_channel واردکن اصلی, ThreadedTCPServer
واردکن socket

رده ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

اگر __name__ == "__main__":
    اصلی("::", 0, ThreadedTCP6Server)
