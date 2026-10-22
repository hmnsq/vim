#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

ତଃ __future__ ଆମଦାନୀ print_function, unicode_literals
ଆମଦାନୀ locale
ଆମଦାନୀ io
ଆମଦାନୀ sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    ଯଦି enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', ମିଥ୍ୟା)

        ଯଦି sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            ଲିଖ = writer.ଲିଖ    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                ଯଦି isinstance(s, bytes):
                    ଲିଖ(s.decode(enc))    # convert to unistr
                ଅନ୍ୟଥା:
                    ଲିଖ(s)
                ଚେଷ୍ଟା:
                    writer.flush()  # needed on Windows
                ବ୍ୟତିକ୍ରମ IOError:
                    pass
            writer.ଲିଖ = convwrite
        ଅନ୍ୟଥା:
            writer = io.open(fo.fileno(), mode='w', **kw)
        ଫେରନ୍ତୁ writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def ମୁଖ୍ୟ():
    enc = 'utf-8'
    ଯଦି len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


ଯଦି __name__ == "__main__":
    ମୁଖ୍ୟ()
