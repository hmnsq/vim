#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

తః __future__ దిగుమతి print_function, unicode_literals
దిగుమతి locale
దిగుమతి io
దిగుమతి sys

నిర్వచనం సమితి_output_encoding(enc=ఏమీలేదు):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    ఒకవేళ enc అయిన ఏమీలేదు:
        enc = locale.getpreferredencoding()

    నిర్వచనం get_పాఠ్యం_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', అబద్ధం)

        ఒకవేళ sys.సంస్కరణ_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), బహుళకం='w', newline='', **kw)
            లిఖ = writer.లిఖ    # save the original write() function
            enc = locale.getpreferredencoding()
            నిర్వచనం convwrite(s):
                ఒకవేళ isinstance(s, బైట్లు):
                    లిఖ(s.decode(enc))    # convert to unistr
                లేకపోతే:
                    లిఖ(s)
                ప్రయత్నించు:
                    writer.flush()  # needed on Windows
                మినహాయింపు IOError:
                    దాటు
            writer.లిఖ = convwrite
        లేకపోతే:
            writer = io.open(fo.fileno(), బహుళకం='w', **kw)
        తిరిగి writer

    sys.stdout = get_పాఠ్యం_writer(sys.stdout, encoding=enc)
    sys.stderr = get_పాఠ్యం_writer(sys.stderr, encoding=enc)


నిర్వచనం ప్రధాన():
    enc = 'utf-8'
    ఒకవేళ పొడవు(sys.argv) > 1:
        enc = sys.argv[1]
    సమితి_output_encoding(enc)

    సందేశం_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    ముద్రించు('Xfoobar.c(10) : %s (%s)' % (సందేశం_tbl[enc], enc))


ఒకవేళ __name__ == "__main__":
    ప్రధాన()
