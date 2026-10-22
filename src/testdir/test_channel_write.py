#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

من __future__ استورد print_function
استورد sys
استورد time

إذا __name__ == "__main__":

    done = 0
    طالما done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.نوم(0.05)  # sleep 50 msec
