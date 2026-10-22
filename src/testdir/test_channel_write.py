#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

ta_h __future__ import print_function
import sys
import time

အကယ်၍ __name__ == "__main__":

    done = 0
    နေစဉ် done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.su_supti(0.05)  # sleep 50 msec
