// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	puur_naa_nka
main(puur_naa_nka argc, var_na **argv)
{
	var_na buffer[BUFSIZ];
	var_na *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif

	ᤀᤠ᤺ (fgets(buffer, BUFSIZ, stdin) != NULL)
	{
		for (p = buffer; *p != 0; p++)
		{
			ᤀᤡ᤺ (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*(unsigned var_na *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			ᤀᤡ᤺ᤆᤡ
			{
				ᤀᤡ᤺ (*p & 0x80)
				{
					putchar(*p++);
					ᤀᤡ᤺ (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
