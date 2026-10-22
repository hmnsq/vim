#!/usr/bin/python
#
# Server that will communicate over stdin/stderr
#
# This requires Python 2.6 or later.

from __future__ import print_function
import os
import sys
import time

якщо __name__ == "__main__":

    якщо len(sys.argv) > 1:
        якщо sys.argv[1].startswith("err"):
            print(sys.argv[1], file=sys.stderr)
            sys.stderr.flush()
        elif sys.argv[1].startswith("incomplete"):
            print(sys.argv[1], end='')
            sys.stdout.flush()
            sys.exit(0)
        elif sys.argv[1].startswith("busy"):
            time.sleep(100)
            sys.exit(0)
        інакше:
            print(sys.argv[1])
            sys.stdout.flush()
            якщо sys.argv[1].startswith("quit"):
                sys.exit(0)

    якщо os.getenv('CI'):
        try:
            import thread_util
            thread_util.set_high_priority()
        except Exception:
            pass

    поки True:
        typed = sys.stdin.readline()
        якщо typed == "":  # EOF -- stop
            break
        якщо typed.startswith("quit"):
            print("Goodbye!")
            sys.stdout.flush()
            break
        якщо typed.startswith("echo "):
            print(typed[5:-1])
            sys.stdout.flush()
        якщо typed.startswith("echosplit "):
            для part in typed[10:-1].split('|'):
                sys.stdout.write(part)
                sys.stdout.flush()
                time.sleep(0.05)
        якщо typed.startswith("double "):
            print(typed[7:-1] + "\nAND " + typed[7:-1])
            sys.stdout.flush()
        якщо typed.startswith("split "):
            print(typed[6:-1], end='')
            sys.stdout.flush()
            time.sleep(0.05)
            print(typed[6:-1], end='')
            sys.stdout.flush()
            time.sleep(0.05)
            print(typed[6:-1])
            sys.stdout.flush()
        якщо typed.startswith("echoerr "):
            print(typed[8:-1], file=sys.stderr)
            sys.stderr.flush()
        якщо typed.startswith("doubleerr "):
            print(typed[10:-1] + "\nAND " + typed[10:-1], file=sys.stderr)
            sys.stderr.flush()
        якщо typed.startswith("XXX"):
            print(typed, end='')
            sys.stderr.flush()
            break

