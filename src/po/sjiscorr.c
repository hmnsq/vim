// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#شامل <stdio.h>
#شامل <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	صحیح
اصلی(صحیح argc, نویسه **argv)
{
	نویسه buffer[BUFSIZ];
	نویسه *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_دودویی_2);
	_setmode(_fileno(stdout), O_دودویی_2);
#endif

	وقتی (fgets(buffer, BUFSIZ, stdin) != پوچ)
	{
		برای (p = buffer; *p != 0; p++)
		{
			اگر (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			وگرنه اگر (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			وگرنه اگر (*(unsigned نویسه *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			وگرنه
			{
				اگر (*p & 0x80)
				{
					putchar(*p++);
					اگر (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
