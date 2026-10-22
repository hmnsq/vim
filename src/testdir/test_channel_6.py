#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

तः test_channel आनयन main, ThreadedTCPServer
आनयन socket

वर्ग ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

जइ __name__ == "__main__":
    main("::", 0, ThreadedTCP6Server)
