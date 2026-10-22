#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

तः __future__ आनयन print_function
आनयन sys
आनयन time

यदि __name__ == "__main__":

    done = 0
    जबसम्म done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.सुषुप्ति(0.05)  # sleep 50 msec
