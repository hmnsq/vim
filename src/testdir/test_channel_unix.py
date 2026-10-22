#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

من __future__ استورد print_function
من test_channel استورد ThreadedTCPServer, TestingRequestHandler, \
    write_پورټ_دننه_File
استورد socket
استورد threading
استورد os

حاول:
    File_Not_وموندل_Error
استثناء NameError:
    # Python 2
    File_Not_وموندل_Error = (IOError, OSError)

إذا ليس hasattr(socket, "AF_UNIX"):
    ارفع NotImplementedError("Unix sockets are not supported on this platform")

صنف ThreadedUnixServer(ThreadedTCPServer):
    پته_family = socket.AF_UNIX

صنف ThreadedUnixRequestHandler(TestingRequestHandler):
    تیرشه

تعریف اصلي(لار):
    server = ThreadedUnixServer(لار, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.خيط(موخه=server.serve_forever)
    server_thread.پیل()

    # Signal the test harness we're ready, the port value has no meaning.
    write_پورټ_دننه_File(1234)

    چاپ("Listening on {0}".format(server.server_پته))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    حاول:
        طالما server_thread.is_alive():
            server_thread.join(1)
    استثناء (Keyboard_مداخله, SystemExit):
        server.shutdown()

إذا __name__ == "__main__":
    حاول:
        os.remove("Xtestsocket")
    استثناء File_Not_وموندل_Error:
        تیرشه
    اصلي("Xtestsocket")
