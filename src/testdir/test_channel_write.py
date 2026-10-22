#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

سے __future__ درآمد print_function
درآمد sys
درآمد وقت

اگر __name__ == "__main__":

    مکمل = 0
    جب مکمل < 10:
        مکمل = مکمل + 1
        چھاپو(مکمل)
        sys.stdout.flush()
        وقت.نوم(0.05)  # sleep 50 msec
