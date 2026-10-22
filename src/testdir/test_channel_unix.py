#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

തഃ __future__ ഇറക്കുമതി print_function
തഃ test_channel ഇറക്കുമതി ThreadedTCPServer, TestingRequestHandler, \
    writePortInFile
ഇറക്കുമതി socket
ഇറക്കുമതി threading
ഇറക്കുമതി os

ശ്രമിക്കുക:
    FileNotFoundError
അപവാദം NameError:
    # Python 2
    FileNotFoundError = (IOError, OSError)

എങ്കിൽ അല്ല hasattr(socket, "AF_UNIX"):
    ഉയർത്തുക NotImplementedError("Unix sockets are not supported on this platform")

വർഗ്ഗം ThreadedUnixServer(ThreadedTCPServer):
    address_family = socket.AF_UNIX

വർഗ്ഗം ThreadedUnixRequestHandler(TestingRequestHandler):
    pass

def പ്രധാന(path):
    server = ThreadedUnixServer(path, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.നൂൽ(target=server.serve_forever)
    server_thread.start()

    # Signal the test harness we're ready, the port value has no meaning.
    writePortInFile(1234)

    print("Listening on {0}".format(server.server_address))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    ശ്രമിക്കുക:
        ആകുമ്പോൾ server_thread.is_alive():
            server_thread.join(1)
    അപവാദം (KeyboardInterrupt, SystemExit):
        server.shutdown()

എങ്കിൽ __name__ == "__main__":
    ശ്രമിക്കുക:
        os.remove("Xtestsocket")
    അപവാദം FileNotFoundError:
        pass
    പ്രധാന("Xtestsocket")
