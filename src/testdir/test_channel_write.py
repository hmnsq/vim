#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

தஃ __future__ இறக்குமதி print_function
இறக்குமதி sys
இறக்குமதி time

எனில் __name__ == "__main__":

    done = 0
    போது done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.ஸுஷுப்தி(0.05)  # sleep 50 msec
