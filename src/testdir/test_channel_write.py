#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

ਤਃ __future__ ਆਯਾਤ print_function
ਆਯਾਤ sys
ਆਯਾਤ ਸਮਾਂ

ਜੇ __name__ == "__main__":

    ਪੂਰਾ = 0
    ਜਦੋਂ ਪੂਰਾ < 10:
        ਪੂਰਾ = ਪੂਰਾ + 1
        ਛਾਪੋ(ਪੂਰਾ)
        sys.stdout.flush()
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(0.05)  # sleep 50 msec
