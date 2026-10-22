#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

ತಃ __future__ ಆಮದು print_function, unicode_literals
ಆಮದು locale
ಆಮದು io
ಆಮದು sys

ವ್ಯಾಖ್ಯಾನ ಗಣ_output_encoding(enc=ಏನೂಇಲ್ಲ):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    ಆದರೆ enc ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        enc = locale.getpreferredencoding()

    ವ್ಯಾಖ್ಯಾನ get_ಪಠ್ಯ_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', ಸುಳ್ಳು)

        ಆದರೆ sys.ಆವೃತ್ತಿ_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), ಬಹುಲಕ='w', newline='', **kw)
            ಲಿಖ = writer.ಲಿಖ    # save the original write() function
            enc = locale.getpreferredencoding()
            ವ್ಯಾಖ್ಯಾನ convwrite(s):
                ಆದರೆ isinstance(s, ಬೈಟುಗಳು):
                    ಲಿಖ(s.decode(enc))    # convert to unistr
                ಇಲ್ಲದಿದ್ದರೆ:
                    ಲಿಖ(s)
                ಪ್ರಯತ್ನಿಸು:
                    writer.flush()  # needed on Windows
                ಅಪವಾದ IOError:
                    ದಾಟು
            writer.ಲಿಖ = convwrite
        ಇಲ್ಲದಿದ್ದರೆ:
            writer = io.open(fo.fileno(), ಬಹುಲಕ='w', **kw)
        ಹಿಂತಿರುಗಿ writer

    sys.stdout = get_ಪಠ್ಯ_writer(sys.stdout, encoding=enc)
    sys.stderr = get_ಪಠ್ಯ_writer(sys.stderr, encoding=enc)


ವ್ಯಾಖ್ಯಾನ ಮುಖ್ಯ():
    enc = 'utf-8'
    ಆದರೆ ಉದ್ದ(sys.argv) > 1:
        enc = sys.argv[1]
    ಗಣ_output_encoding(enc)

    ಸಂದೇಶ_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    ಮುದ್ರಿಸು('Xfoobar.c(10) : %s (%s)' % (ಸಂದೇಶ_tbl[enc], enc))


ಆದರೆ __name__ == "__main__":
    ಮುಖ್ಯ()
