#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

తః __future__ దిగుమతి print_function
దిగుమతి sys
దిగుమతి సమయం

ఒకవేళ __name__ == "__main__":

    పూర్తి = 0
    అయితే పూర్తి < 10:
        పూర్తి = పూర్తి + 1
        ముద్రించు(పూర్తి)
        sys.stdout.flush()
        సమయం.సుషుప్తి(0.05)  # sleep 50 msec
