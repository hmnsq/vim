#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

తః test_channel దిగుమతి ప్రధాన, ThreadedTCPServer
దిగుమతి socket

తరగతి ThreadedTCP6Server(ThreadedTCPServer):
    చిరునామా_family = socket.AF_INET6

ఒకవేళ __name__ == "__main__":
    ప్రధాన("::", 0, ThreadedTCP6Server)
