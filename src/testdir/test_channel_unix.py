#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

তঃ __future__ আমদানি print_function
তঃ test_channel আমদানি ThreadedTCPServer, TestingRequestHandler, \
    writePortInFile
আমদানি socket
আমদানি threading
আমদানি os

চেষ্টা:
    FileNotFoundError
ব্যতিক্রম NameError:
    # Python 2
    FileNotFoundError = (IOError, OSError)

যদি নয় hasattr(socket, "AF_UNIX"):
    তোলো NotImplementedError("Unix sockets are not supported on this platform")

শ্রেণী ThreadedUnixServer(ThreadedTCPServer):
    address_family = socket.AF_UNIX

শ্রেণী ThreadedUnixRequestHandler(TestingRequestHandler):
    pass

def প্রধান(path):
    server = ThreadedUnixServer(path, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.সুতা(target=server.serve_forever)
    server_thread.start()

    # Signal the test harness we're ready, the port value has no meaning.
    writePortInFile(1234)

    print("Listening on {0}".format(server.server_address))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    চেষ্টা:
        যখন server_thread.is_alive():
            server_thread.join(1)
    ব্যতিক্রম (KeyboardInterrupt, SystemExit):
        server.shutdown()

যদি __name__ == "__main__":
    চেষ্টা:
        os.remove("Xtestsocket")
    ব্যতিক্রম FileNotFoundError:
        pass
    প্রধান("Xtestsocket")
