#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

தஃ test_channel இறக்குமதி முதன்மை, ThreadedTCPServer
இறக்குமதி socket

வகுப்பு ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

எனில் __name__ == "__main__":
    முதன்மை("::", 0, ThreadedTCP6Server)
