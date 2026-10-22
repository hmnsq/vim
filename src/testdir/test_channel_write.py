#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

من __future__ استورد print_function
استورد sys
استورد زمان

إذا __name__ == "__main__":

    انجامشده = 0
    طالما انجامشده < 10:
        انجامشده = انجامشده + 1
        چاپ(انجامشده)
        sys.stdout.flush()
        زمان.نوم(0.05)  # sleep 50 msec
