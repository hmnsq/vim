#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

ତଃ test_channel ଆମଦାନୀ ମୁଖ୍ୟ, ThreadedTCPServer
ଆମଦାନୀ socket

ଶ୍ରେଣୀ ThreadedTCP6Server(ThreadedTCPServer):
    address_family = socket.AF_INET6

ଯଦି __name__ == "__main__":
    ମୁଖ୍ୟ("::", 0, ThreadedTCP6Server)
