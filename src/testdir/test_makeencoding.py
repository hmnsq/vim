#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

سے __future__ درآمد print_function, unicode_literals
درآمد locale
درآمد io
درآمد sys

تعریف سیٹ_output_encoding(enc=کچھ_نہیں):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    اگر enc ہے کچھ_نہیں:
        enc = locale.getpreferredencoding()

    تعریف get_متن_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', جھوٹ)

        اگر sys.نسخہ_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), منوال='w', newline='', **kw)
            اكتب = writer.اكتب    # save the original write() function
            enc = locale.getpreferredencoding()
            تعریف convwrite(s):
                اگر isinstance(s, بائٹیں):
                    اكتب(s.decode(enc))    # convert to unistr
                ورنہ:
                    اكتب(s)
                کوشش:
                    writer.flush()  # needed on Windows
                استثنا IOError:
                    گزرو
            writer.اكتب = convwrite
        ورنہ:
            writer = io.open(fo.fileno(), منوال='w', **kw)
        واپس writer

    sys.stdout = get_متن_writer(sys.stdout, encoding=enc)
    sys.stderr = get_متن_writer(sys.stderr, encoding=enc)


تعریف مین():
    enc = 'utf-8'
    اگر لمبائی(sys.argv) > 1:
        enc = sys.argv[1]
    سیٹ_output_encoding(enc)

    پیغام_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    چھاپو('Xfoobar.c(10) : %s (%s)' % (پیغام_tbl[enc], enc))


اگر __name__ == "__main__":
    مین()
