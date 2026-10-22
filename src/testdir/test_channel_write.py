#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

তঃ __future__ আনযন print_function
আনযন sys
আনযন time

যদি __name__ == "__main__":

    done = 0
    যাবত্ done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.সুষুপ্তি(0.05)  # sleep 50 msec
