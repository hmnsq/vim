// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
	અક્ષર buffer[BUFSIZ];
	અક્ષર *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_દ્વિઅંકી_2);
	_setmode(_fileno(stdout), O_દ્વિઅંકી_2);
#endif

	જ્યારે (fgets(buffer, BUFSIZ, stdin) != ખાલી)
	{
		માટે (p = buffer; *p != 0; p++)
		{
			જો (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			નહીંતર જો (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			નહીંતર જો (*(unsigned અક્ષર *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			નહીંતર
			{
				જો (*p & 0x80)
				{
					putchar(*p++);
					જો (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
