#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

از __future__ واردکن print_function
از test_channel واردکن ThreadedTCPServer, TestingRequestHandler, \
    writePortInFile
واردکن socket
واردکن threading
واردکن os

تلاش:
    FileNotFoundError
استثنا NameError:
    # Python 2
    FileNotFoundError = (IOError, OSError)

اگر نه hasattr(socket, "AF_UNIX"):
    بالاببر NotImplementedError("Unix sockets are not supported on this platform")

رده ThreadedUnixServer(ThreadedTCPServer):
    address_family = socket.AF_UNIX

رده ThreadedUnixRequestHandler(TestingRequestHandler):
    pass

def اصلی(path):
    server = ThreadedUnixServer(path, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.نخ(target=server.serve_forever)
    server_thread.start()

    # Signal the test harness we're ready, the port value has no meaning.
    writePortInFile(1234)

    print("Listening on {0}".format(server.server_address))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    تلاش:
        وقتی server_thread.is_alive():
            server_thread.join(1)
    استثنا (KeyboardInterrupt, SystemExit):
        server.shutdown()

اگر __name__ == "__main__":
    تلاش:
        os.remove("Xtestsocket")
    استثنا FileNotFoundError:
        pass
    اصلی("Xtestsocket")
