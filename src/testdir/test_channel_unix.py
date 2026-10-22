#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

तः __future__ आयात print_function
तः test_channel आयात ThreadedTCPServer, TestingRequestHandler, \
    writePortInFile
आयात socket
आयात threading
आयात os

प्रयत्न:
    FileNotFoundError
अपवाद NameError:
    # Python 2
    FileNotFoundError = (IOError, OSError)

जर नाही hasattr(socket, "AF_UNIX"):
    उठवा NotImplementedError("Unix sockets are not supported on this platform")

वर्ग ThreadedUnixServer(ThreadedTCPServer):
    address_family = socket.AF_UNIX

वर्ग ThreadedUnixRequestHandler(TestingRequestHandler):
    pass

def main(path):
    server = ThreadedUnixServer(path, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.धागा(target=server.serve_forever)
    server_thread.start()

    # Signal the test harness we're ready, the port value has no meaning.
    writePortInFile(1234)

    print("Listening on {0}".format(server.server_address))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    प्रयत्न:
        पर्यंत server_thread.is_alive():
            server_thread.join(1)
    अपवाद (KeyboardInterrupt, SystemExit):
        server.shutdown()

जर __name__ == "__main__":
    प्रयत्न:
        os.remove("Xtestsocket")
    अपवाद FileNotFoundError:
        pass
    main("Xtestsocket")
