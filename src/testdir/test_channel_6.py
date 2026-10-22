#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

من test_channel استورد مکيه, ThreadedTCPServer
استورد socket

صنف ThreadedTCP6Server(ThreadedTCPServer):
    پتو_family = socket.AF_INET6

إذا __name__ == "__main__":
    مکيه("::", 0, ThreadedTCP6Server)
