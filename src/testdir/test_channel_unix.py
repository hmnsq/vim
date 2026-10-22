#!/usr/bin/env python
#
# Server that will accept connections from a Vim channel.
# Used by test_channel.vim.
#
# This requires Python 2.6 or later.

તઃ __future__ આયાત print_function
તઃ test_channel આયાત ThreadedTCPServer, TestingRequestHandler, \
    writePortInFile
આયાત socket
આયાત threading
આયાત os

પ્રયાસ:
    FileNotFoundError
અપવાદ NameError:
    # Python 2
    FileNotFoundError = (IOError, OSError)

જો નહીં hasattr(socket, "AF_UNIX"):
    ઉઠાવો NotImplementedError("Unix sockets are not supported on this platform")

વર્ગ ThreadedUnixServer(ThreadedTCPServer):
    address_family = socket.AF_UNIX

વર્ગ ThreadedUnixRequestHandler(TestingRequestHandler):
    pass

def મુખ્ય(path):
    server = ThreadedUnixServer(path, ThreadedUnixRequestHandler)

    # Start a thread with the server.  That thread will then start a new thread
    # for each connection.
    server_thread = threading.તંતુ(target=server.serve_forever)
    server_thread.start()

    # Signal the test harness we're ready, the port value has no meaning.
    writePortInFile(1234)

    print("Listening on {0}".format(server.server_address))

    # Main thread terminates, but the server continues running
    # until server.shutdown() is called.
    પ્રયાસ:
        જ્યારે server_thread.is_alive():
            server_thread.join(1)
    અપવાદ (KeyboardInterrupt, SystemExit):
        server.shutdown()

જો __name__ == "__main__":
    પ્રયાસ:
        os.remove("Xtestsocket")
    અપવાદ FileNotFoundError:
        pass
    મુખ્ય("Xtestsocket")
