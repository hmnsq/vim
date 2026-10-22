#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

തഃ __future__ ഇറക്കുമതി print_function
ഇറക്കുമതി sys
ഇറക്കുമതി time

എങ്കിൽ __name__ == "__main__":

    done = 0
    ആകുമ്പോൾ done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.സുഷുപ്തി(0.05)  # sleep 50 msec
