#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

তঃ test_channel আমদানি প্রধান, ThreadedTCPServer
আমদানি socket

শ্রেণী ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

যদি __name__ == "__main__":
    প্রধান("::", 0, ThreadedTCP6Server)
