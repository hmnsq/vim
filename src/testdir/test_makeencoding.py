#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

ತಃ __future__ ಆನಯನ print_function, unicode_literals
ಆನಯನ locale
ಆನಯನ io
ಆನಯನ sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    ಯದಿ enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', ಅಸತ್ಯ)

        ಯದಿ sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            ಲಿಖ = writer.ಲಿಖ    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                ಯದಿ isinstance(s, bytes):
                    ಲಿಖ(s.decode(enc))    # convert to unistr
                ಅನ್ಯಥಾ:
                    ಲಿಖ(s)
                ಯತ್ನ:
                    writer.flush()  # needed on Windows
                ಅಪವಾದ IOError:
                    pass
            writer.ಲಿಖ = convwrite
        ಅನ್ಯಥಾ:
            writer = io.open(fo.fileno(), mode='w', **kw)
        ಪ್ರತ್ಯಾಗಮ writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def main():
    enc = 'utf-8'
    ಯದಿ len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


ಯದಿ __name__ == "__main__":
    main()
