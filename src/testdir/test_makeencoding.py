#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

ਤਃ __future__ ਆਯਾਤ print_function, unicode_literals
ਆਯਾਤ locale
ਆਯਾਤ io
ਆਯਾਤ sys

ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_output_encoding(enc=ਕੁਝ_ਨਹੀਂ):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    ਜੇ enc ਹੈ ਕੁਝ_ਨਹੀਂ:
        enc = locale.getpreferredencoding()

    ਪਰਿਭਾਸ਼ਾ get_ਪਾਠ_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', ਝੂਠ)

        ਜੇ sys.ਸੰਸਕਰਣ_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), ਬਹੁਲਕ='w', newline='', **kw)
            ਲਿਖ = writer.ਲਿਖ    # save the original write() function
            enc = locale.getpreferredencoding()
            ਪਰਿਭਾਸ਼ਾ convwrite(s):
                ਜੇ isinstance(s, ਬਾਈਟਾਂ):
                    ਲਿਖ(s.decode(enc))    # convert to unistr
                ਨਹੀਂਤਾਂ:
                    ਲਿਖ(s)
                ਕੋਸ਼ਿਸ਼:
                    writer.flush()  # needed on Windows
                ਅਪਵਾਦ IOError:
                    ਲੰਘੋ
            writer.ਲਿਖ = convwrite
        ਨਹੀਂਤਾਂ:
            writer = io.open(fo.fileno(), ਬਹੁਲਕ='w', **kw)
        ਵਾਪਸ writer

    sys.stdout = get_ਪਾਠ_writer(sys.stdout, encoding=enc)
    sys.stderr = get_ਪਾਠ_writer(sys.stderr, encoding=enc)


ਪਰਿਭਾਸ਼ਾ ਮੁੱਖ():
    enc = 'utf-8'
    ਜੇ ਲੰਬਾਈ(sys.argv) > 1:
        enc = sys.argv[1]
    ਸੈੱਟ_output_encoding(enc)

    ਸੁਨੇਹਾ_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    ਛਾਪੋ('Xfoobar.c(10) : %s (%s)' % (ਸੁਨੇਹਾ_tbl[enc], enc))


ਜੇ __name__ == "__main__":
    ਮੁੱਖ()
