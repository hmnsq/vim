#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

তঃ __future__ আমদানি print_function, unicode_literals
আমদানি locale
আমদানি io
আমদানি sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    যদি enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', মিথ্যা)

        যদি sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            লিখ = writer.লিখ    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                যদি isinstance(s, bytes):
                    লিখ(s.decode(enc))    # convert to unistr
                অন্যথা:
                    লিখ(s)
                চেষ্টা:
                    writer.flush()  # needed on Windows
                ব্যতিক্রম IOError:
                    pass
            writer.লিখ = convwrite
        অন্যথা:
            writer = io.open(fo.fileno(), mode='w', **kw)
        ফেরত writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def প্রধান():
    enc = 'utf-8'
    যদি len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


যদি __name__ == "__main__":
    প্রধান()
