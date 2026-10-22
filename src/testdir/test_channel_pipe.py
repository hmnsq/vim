#!/usr/bin/python
#
# Server that will communicate over stdin/stderr
#
# This requires Python 2.6 or later.

from __future__ import print_function
import os
import sys
import time

nếu __name__ == "__main__":

    nếu len(sys.argv) > 1:
        nếu sys.argv[1].startswith("err"):
            print(sys.argv[1], file=sys.stderr)
            sys.stderr.flush()
        elif sys.argv[1].startswith("incomplete"):
            print(sys.argv[1], end='')
            sys.stdout.flush()
            sys.exit(0)
        elif sys.argv[1].startswith("busy"):
            time.sleep(100)
            sys.exit(0)
        không_thì:
            print(sys.argv[1])
            sys.stdout.flush()
            nếu sys.argv[1].startswith("quit"):
                sys.exit(0)

    nếu os.getenv('CI'):
        try:
            import thread_util
            thread_util.set_high_priority()
        except Exception:
            pass

    trong_khi True:
        typed = sys.stdin.readline()
        nếu typed == "":  # EOF -- stop
            break
        nếu typed.startswith("quit"):
            print("Goodbye!")
            sys.stdout.flush()
            break
        nếu typed.startswith("echo "):
            print(typed[5:-1])
            sys.stdout.flush()
        nếu typed.startswith("echosplit "):
            cho part in typed[10:-1].split('|'):
                sys.stdout.write(part)
                sys.stdout.flush()
                time.sleep(0.05)
        nếu typed.startswith("double "):
            print(typed[7:-1] + "\nAND " + typed[7:-1])
            sys.stdout.flush()
        nếu typed.startswith("split "):
            print(typed[6:-1], end='')
            sys.stdout.flush()
            time.sleep(0.05)
            print(typed[6:-1], end='')
            sys.stdout.flush()
            time.sleep(0.05)
            print(typed[6:-1])
            sys.stdout.flush()
        nếu typed.startswith("echoerr "):
            print(typed[8:-1], file=sys.stderr)
            sys.stderr.flush()
        nếu typed.startswith("doubleerr "):
            print(typed[10:-1] + "\nAND " + typed[10:-1], file=sys.stderr)
            sys.stderr.flush()
        nếu typed.startswith("XXX"):
            print(typed, end='')
            sys.stderr.flush()
            break

