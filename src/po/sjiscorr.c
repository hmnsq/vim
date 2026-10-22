// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
	वर्ण buffer[BUFSIZ];
	वर्ण *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_द्विआधारी_2);
	_setmode(_fileno(stdout), O_द्विआधारी_2);
#endif

	यावत् (fgets(buffer, BUFSIZ, stdin) != रिक्त)
	{
		प्रति (p = buffer; *p != 0; p++)
		{
			यदि (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			अन्यथा यदि (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			अन्यथा यदि (*(unsigned वर्ण *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			अन्यथा
			{
				यदि (*p & 0x80)
				{
					putchar(*p++);
					यदि (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
