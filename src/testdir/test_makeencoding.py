#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

از __future__ واردکن print_function, unicode_literals
واردکن locale
واردکن io
واردکن sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    اگر enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', نادرست)

        اگر sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            اكتب = writer.اكتب    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                اگر isinstance(s, bytes):
                    اكتب(s.decode(enc))    # convert to unistr
                وگرنه:
                    اكتب(s)
                تلاش:
                    writer.flush()  # needed on Windows
                استثنا IOError:
                    pass
            writer.اكتب = convwrite
        وگرنه:
            writer = io.open(fo.fileno(), mode='w', **kw)
        بازگشت writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def اصلی():
    enc = 'utf-8'
    اگر len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


اگر __name__ == "__main__":
    اصلی()
