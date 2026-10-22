#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

તઃ __future__ આયાત print_function
આયાત sys
આયાત time

જો __name__ == "__main__":

    done = 0
    જ્યારે done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.સુષુપ્તિ(0.05)  # sleep 50 msec
