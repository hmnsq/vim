// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
	ଅକ୍ଷର buffer[BUFSIZ];
	ଅକ୍ଷର *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_ଦ୍ୱିମିକ_2);
	_setmode(_fileno(stdout), O_ଦ୍ୱିମିକ_2);
#endif

	ଯେତେବେଳେ (fgets(buffer, BUFSIZ, stdin) != ଖାଲି)
	{
		ପାଇଁ (p = buffer; *p != 0; p++)
		{
			ଯଦି (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			ଅନ୍ୟଥା ଯଦି (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			ଅନ୍ୟଥା ଯଦି (*(unsigned ଅକ୍ଷର *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			ଅନ୍ୟଥା
			{
				ଯଦି (*p & 0x80)
				{
					putchar(*p++);
					ଯଦି (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
