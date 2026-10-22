#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

من test_channel استورد اصلی, ThreadedTCPServer
استورد socket

صنف ThreadedTCP6Server(ThreadedTCPServer):
    آدرس_family = socket.AF_INET6

إذا __name__ == "__main__":
    اصلی("::", 0, ThreadedTCP6Server)
