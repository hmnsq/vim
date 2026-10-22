#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

سے test_channel درآمد مین, ThreadedTCPServer
درآمد socket

جماعت ThreadedTCP6Server(ThreadedTCPServer):
    پتہ_family = socket.AF_INET6

اگر __name__ == "__main__":
    مین("::", 0, ThreadedTCP6Server)
