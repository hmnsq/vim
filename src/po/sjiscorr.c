// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
	எழுத்து buffer[BUFSIZ];
	எழுத்து *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_இருமம்_2);
	_setmode(_fileno(stdout), O_இருமம்_2);
#endif

	போது (fgets(buffer, BUFSIZ, stdin) != வெற்று)
	{
		க்கு (p = buffer; *p != 0; p++)
		{
			எனில் (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			இல்லையெனில் எனில் (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			இல்லையெனில் எனில் (*(unsigned எழுத்து *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			இல்லையெனில்
			{
				எனில் (*p & 0x80)
				{
					putchar(*p++);
					எனில் (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
