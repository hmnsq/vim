#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

तः __future__ आयात print_function
आयात sys
आयात time

जर __name__ == "__main__":

    done = 0
    पर्यंत done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.सुषुप्ति(0.05)  # sleep 50 msec
