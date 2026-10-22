#!/usr/bin/python
#
# Server that will communicate over stdin/stderr
#
# This requires Python 2.6 or later.

from __future__ import print_function
import os
import sys
import time

ако __name__ == "__main__":

    ако len(sys.argv) > 1:
        ако sys.argv[1].startswith("err"):
            print(sys.argv[1], file=sys.stderr)
            sys.stderr.flush()
        elif sys.argv[1].startswith("incomplete"):
            print(sys.argv[1], end='')
            sys.stdout.flush()
            sys.exit(0)
        elif sys.argv[1].startswith("busy"):
            time.sleep(100)
            sys.exit(0)
        иначе:
            print(sys.argv[1])
            sys.stdout.flush()
            ако sys.argv[1].startswith("quit"):
                sys.exit(0)

    ако os.getenv('CI'):
        try:
            import thread_util
            thread_util.set_high_priority()
        except Exception:
            pass

    докато True:
        typed = sys.stdin.readline()
        ако typed == "":  # EOF -- stop
            break
        ако typed.startswith("quit"):
            print("Goodbye!")
            sys.stdout.flush()
            break
        ако typed.startswith("echo "):
            print(typed[5:-1])
            sys.stdout.flush()
        ако typed.startswith("echosplit "):
            за part in typed[10:-1].split('|'):
                sys.stdout.write(part)
                sys.stdout.flush()
                time.sleep(0.05)
        ако typed.startswith("double "):
            print(typed[7:-1] + "\nAND " + typed[7:-1])
            sys.stdout.flush()
        ако typed.startswith("split "):
            print(typed[6:-1], end='')
            sys.stdout.flush()
            time.sleep(0.05)
            print(typed[6:-1], end='')
            sys.stdout.flush()
            time.sleep(0.05)
            print(typed[6:-1])
            sys.stdout.flush()
        ако typed.startswith("echoerr "):
            print(typed[8:-1], file=sys.stderr)
            sys.stderr.flush()
        ако typed.startswith("doubleerr "):
            print(typed[10:-1] + "\nAND " + typed[10:-1], file=sys.stderr)
            sys.stderr.flush()
        ако typed.startswith("XXX"):
            print(typed, end='')
            sys.stderr.flush()
            break

