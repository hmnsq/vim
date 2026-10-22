#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

من __future__ استورد print_function, unicode_literals
استورد locale
استورد io
استورد sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    ܐܢ enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', ܕܓܠܐ)

        ܐܢ sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            اكتب = writer.اكتب    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                ܐܢ isinstance(s, bytes):
                    اكتب(s.decode(enc))    # convert to unistr
                ܐܚܪܢܐ:
                    اكتب(s)
                حاول:
                    writer.flush()  # needed on Windows
                استثناء IOError:
                    pass
            writer.اكتب = convwrite
        ܐܚܪܢܐ:
            writer = io.open(fo.fileno(), mode='w', **kw)
        ܦܢܐ writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def main():
    enc = 'utf-8'
    ܐܢ len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


ܐܢ __name__ == "__main__":
    main()
