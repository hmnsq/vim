#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

തഃ __future__ ഇറക്കുമതി print_function, unicode_literals
ഇറക്കുമതി locale
ഇറക്കുമതി io
ഇറക്കുമതി sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    എങ്കിൽ enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', അസത്യം)

        എങ്കിൽ sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            ലിഖ = writer.ലിഖ    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                എങ്കിൽ isinstance(s, bytes):
                    ലിഖ(s.decode(enc))    # convert to unistr
                അല്ലെങ്കിൽ:
                    ലിഖ(s)
                ശ്രമിക്കുക:
                    writer.flush()  # needed on Windows
                അപവാദം IOError:
                    pass
            writer.ലിഖ = convwrite
        അല്ലെങ്കിൽ:
            writer = io.open(fo.fileno(), mode='w', **kw)
        തിരികെ writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def പ്രധാന():
    enc = 'utf-8'
    എങ്കിൽ len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


എങ്കിൽ __name__ == "__main__":
    പ്രധാന()
