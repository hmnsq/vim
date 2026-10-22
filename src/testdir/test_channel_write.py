#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

ತಃ __future__ ಆನಯನ print_function
ಆನಯನ sys
ಆನಯನ time

ಯದಿ __name__ == "__main__":

    done = 0
    ಯಾವತ್ done < 10:
        done = done + 1
        print(done)
        sys.stdout.flush()
        time.ಸುಷುಪ್ತಿ(0.05)  # sleep 50 msec
