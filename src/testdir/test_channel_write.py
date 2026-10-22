#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

من __future__ استورد print_function
استورد sys
استورد وخت

إذا __name__ == "__main__":

    بشپړ = 0
    طالما بشپړ < 10:
        بشپړ = بشپړ + 1
        چاپ(بشپړ)
        sys.stdout.flush()
        وخت.نوم(0.05)  # sleep 50 msec
