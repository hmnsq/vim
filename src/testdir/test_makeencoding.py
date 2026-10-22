#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

તઃ __future__ આયાત print_function, unicode_literals
આયાત locale
આયાત io
આયાત sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    જો enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', ખોટું)

        જો sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            લિખ = writer.લિખ    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                જો isinstance(s, bytes):
                    લિખ(s.decode(enc))    # convert to unistr
                નહીંતર:
                    લિખ(s)
                પ્રયાસ:
                    writer.flush()  # needed on Windows
                અપવાદ IOError:
                    pass
            writer.લિખ = convwrite
        નહીંતર:
            writer = io.open(fo.fileno(), mode='w', **kw)
        પાછા writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def મુખ્ય():
    enc = 'utf-8'
    જો len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


જો __name__ == "__main__":
    મુખ્ય()
