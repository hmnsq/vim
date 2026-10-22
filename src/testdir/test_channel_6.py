#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

ತಃ test_channel ಆಮದು ಮುಖ್ಯ, ThreadedTCPServer
ಆಮದು socket

ವರ್ಗ ThreadedTCP6Server(ThreadedTCPServer):
    ವಿಳಾಸ_family = socket.AF_INET6

ಆದರೆ __name__ == "__main__":
    ಮುಖ್ಯ("::", 0, ThreadedTCP6Server)
