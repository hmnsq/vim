// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
	অক্ষর বাফার[BUFSIZ];
	অক্ষর *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_দ্বিমিক_2);
	_setmode(_fileno(stdout), O_দ্বিমিক_2);
#endif

	যখন (fgets(বাফার, BUFSIZ, stdin) != ফাঁকা)
	{
		জন্য (p = বাফার; *p != 0; p++)
		{
			যদি (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			অন্যথা যদি (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			অন্যথা যদি (*(unsigned অক্ষর *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			অন্যথা
			{
				যদি (*p & 0x80)
				{
					putchar(*p++);
					যদি (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
