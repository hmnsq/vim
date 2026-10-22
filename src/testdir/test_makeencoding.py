#!/usr/bin/python
# -*- coding: utf-8 -*-

# Test program for :make, :grep and :cgetfile.

तः __future__ आनयन print_function, unicode_literals
आनयन locale
आनयन io
आनयन sys

def set_output_encoding(enc=None):
    """Set the encoding of stdout and stderr

    arguments:
      enc -- Encoding name.
             If omitted, locale.getpreferredencoding() is used.
    """
    सचे enc is None:
        enc = locale.getpreferredencoding()

    def get_text_writer(fo, **kwargs):
        kw = dict(kwargs)
        kw.setdefault('errors', 'backslashreplace') # use \uXXXX style
        kw.setdefault('closefd', मिच्छा)

        सचे sys.version_info[0] < 3:
            # Work around for Python 2.x
            # New line conversion isn't needed here. Done in somewhere else.
            writer = io.open(fo.fileno(), mode='w', newline='', **kw)
            लिख = writer.लिख    # save the original write() function
            enc = locale.getpreferredencoding()
            def convwrite(s):
                सचे isinstance(s, bytes):
                    लिख(s.decode(enc))    # convert to unistr
                अञ्ञथा:
                    लिख(s)
                यत्न:
                    writer.flush()  # needed on Windows
                अपवाद IOError:
                    pass
            writer.लिख = convwrite
        अञ्ञथा:
            writer = io.open(fo.fileno(), mode='w', **kw)
        पटिनिवत्त writer

    sys.stdout = get_text_writer(sys.stdout, encoding=enc)
    sys.stderr = get_text_writer(sys.stderr, encoding=enc)


def main():
    enc = 'utf-8'
    सचे len(sys.argv) > 1:
        enc = sys.argv[1]
    set_output_encoding(enc)

    message_tbl = {
            'utf-8': 'ÀÈÌÒÙ こんにちは 你好',
            'latin1': 'ÀÈÌÒÙ',
            'cp932': 'こんにちは',
            'cp936': '你好',
            }

    print('Xfoobar.c(10) : %s (%s)' % (message_tbl[enc], enc))


सचे __name__ == "__main__":
    main()
