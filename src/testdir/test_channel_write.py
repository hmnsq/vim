#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

من __future__ استورد print_function
استورد sys
استورد ویلا

إذا __name__ == "__main__":

    پورا = 0
    طالما پورا < 10:
        پورا = پورا + 1
        چھاپو(پورا)
        sys.stdout.flush()
        ویلا.نوم(0.05)  # sleep 50 msec
