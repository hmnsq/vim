#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

من test_channel استورد مکھ, ThreadedTCPServer
استورد socket

صنف ThreadedTCP6Server(ThreadedTCPServer):
    پتہ_family = socket.AF_INET6

إذا __name__ == "__main__":
    مکھ("::", 0, ThreadedTCP6Server)
