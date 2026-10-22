#!/usr/bin/python
#
# Program that writes a number to stdout repeatedly
#
# This requires Python 2.6 or later.

ತಃ __future__ ಆಮದು print_function
ಆಮದು sys
ಆಮದು ಸಮಯ

ಆದರೆ __name__ == "__main__":

    ಮುಗಿದ = 0
    ಆಗ ಮುಗಿದ < 10:
        ಮುಗಿದ = ಮುಗಿದ + 1
        ಮುದ್ರಿಸು(ಮುಗಿದ)
        sys.stdout.flush()
        ಸಮಯ.ಸುಷುಪ್ತಿ(0.05)  # sleep 50 msec
