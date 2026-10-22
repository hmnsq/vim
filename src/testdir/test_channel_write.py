#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

از __future__ واردکن print_function
واردکن sys
واردکن time

اگر __name__ == "__main__":

    done = 0
    وقتی done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.نوم(0.05)  # sleep 50 msec
