#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

ତଃ __future__ ଆମଦାନୀ print_function
ଆମଦାନୀ sys
ଆମଦାନୀ time

ଯଦି __name__ == "__main__":

    done = 0
    ଯେତେବେଳେ done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.ସୁଷୁପ୍ତି(0.05)  # sleep 50 msec
