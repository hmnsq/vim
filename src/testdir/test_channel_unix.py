#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

سے __future__ درآمد print_function
سے test_channel درآمد ThreadedTCPServer, TestingRequestHandler, \
    write_پورٹ_اندر_File
درآمد socket
درآمد threading
درآمد os

کوشش:
    File_Not_ملا_Error
استثنا NameError:
    # Python 2
    File_Not_ملا_Error = (IOError, OSError)

اگر نہیں hasattr(socket, "AF_UNIX"):
    اٹھاؤ NotImplementedError("Unix sockets are not supported on this platform")

جماعت ThreadedUnixServer(ThreadedTCPServer):
    پتہ_family = socket.AF_UNIX

جماعت ThreadedUnixRequestHandler(TestingRequestHandler):
    گزرو

تعریف مین(راستہ):
    server = ThreadedUnixServer(راستہ, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.دھاگہ(ہدف=server.serve_forever)
    server_thread.آغاز()

    # Signal the test harness we're ready, the port value has no meaning.
    write_پورٹ_اندر_File(1234)

    چھاپو("Listening on {0}".format(server.server_پتہ))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    کوشش:
        جب server_thread.is_alive():
            server_thread.join(1)
    استثنا (Keyboard_مداخلت, SystemExit):
        server.shutdown()

اگر __name__ == "__main__":
    کوشش:
        os.remove("Xtestsocket")
    استثنا File_Not_ملا_Error:
        گزرو
    مین("Xtestsocket")
