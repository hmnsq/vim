#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

তঃ __future__ আমদানি print_function
আমদানি sys
আমদানি time

যদি __name__ == "__main__":

    done = 0
    যখন done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.সুষুপ্তি(0.05)  # sleep 50 msec
