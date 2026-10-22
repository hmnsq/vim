#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

തഃ test_channel ഇറക്കുമതി പ്രധാന, ThreadedTCPServer
ഇറക്കുമതി socket

വർഗ്ഗം ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

എങ്കിൽ __name__ == "__main__":
    പ്രധാന("::", 0, ThreadedTCP6Server)
