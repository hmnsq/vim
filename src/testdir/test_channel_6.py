#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

ਤਃ test_channel ਆਯਾਤ ਮੁੱਖ, ThreadedTCPServer
ਆਯਾਤ socket

ਵਰਗ ThreadedTCP6Server(ThreadedTCPServer):
    ਪਤਾ_family = socket.AF_INET6

ਜੇ __name__ == "__main__":
    ਮੁੱਖ("::", 0, ThreadedTCP6Server)
