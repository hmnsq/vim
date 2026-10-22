#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

தஃ __future__ இறக்குமதி print_function, unicode_literals
இறக்குமதி locale
இறக்குமதி io
இறக்குமதி sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    எனில் enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', பொய்)

        எனில் sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            லிக = writer.லிக    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                எனில் isinstance(s, bytes):
                    லிக(s.decode(enc))    # convert to unistr
                இல்லையெனில்:
                    லிக(s)
                முயல்:
                    writer.flush()  # needed on Windows
                விதிவிலக்கு IOError:
                    pass
            writer.லிக = convwrite
        இல்லையெனில்:
            writer = io.open(fo.fileno(), mode='w', **kw)
        திரும்பு writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def முதன்மை():
    enc = 'utf-8'
    எனில் len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


எனில் __name__ == "__main__":
    முதன்மை()
