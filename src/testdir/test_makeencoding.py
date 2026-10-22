#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

من __future__ استورد print_function, unicode_literals
استورد locale
استورد io
استورد sys

تعریف مجموعه_output_encoding(enc=هیچ):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    إذا enc است هیچ:
        enc = locale.getpreferredencoding()

    تعریف get_متن_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', خطأ)

        إذا sys.نسخه_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), نما='w', newline='', **kw)
            اكتب = writer.اكتب    # save the original write() function
            enc = locale.getpreferredencoding()
            تعریف convwrite(s):
                إذا isinstance(s, بایتها):
                    اكتب(s.decode(enc))    # convert to unistr
                وإلا:
                    اكتب(s)
                حاول:
                    writer.flush()  # needed on Windows
                استثناء IOError:
                    بگذر
            writer.اكتب = convwrite
        وإلا:
            writer = io.open(fo.fileno(), نما='w', **kw)
        أرجع writer

    sys.stdout = get_متن_writer(sys.stdout, encoding=enc)
    sys.stderr = get_متن_writer(sys.stderr, encoding=enc)


تعریف اصلی():
    enc = 'utf-8'
    إذا طول(sys.argv) > 1:
        enc = sys.argv[1]
    مجموعه_output_encoding(enc)

    پیام_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    چاپ('Xfoobar.c(10) : %s (%s)' % (پیام_tbl[enc], enc))


إذا __name__ == "__main__":
    اصلی()
