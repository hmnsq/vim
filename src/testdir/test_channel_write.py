#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

من __future__ استورد print_function
استورد sys
استورد وقت

إذا __name__ == "__main__":

    پورو = 0
    طالما پورو < 10:
        پورو = پورو + 1
        ڇاپيو(پورو)
        sys.stdout.flush()
        وقت.نوم(0.05)  # sleep 50 msec
