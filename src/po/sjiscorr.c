// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
	ವರ್ಣ buffer[BUFSIZ];
	ವರ್ಣ *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_ದ್ವಿಮಾನ_2);
	_setmode(_fileno(stdout), O_ದ್ವಿಮಾನ_2);
#endif

	ಯಾವತ್ (fgets(buffer, BUFSIZ, stdin) != ರಿಕ್ತ)
	{
		ಪ್ರತಿ (p = buffer; *p != 0; p++)
		{
			ಯದಿ (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			ಅನ್ಯಥಾ ಯದಿ (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			ಅನ್ಯಥಾ ಯದಿ (*(unsigned ವರ್ಣ *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			ಅನ್ಯಥಾ
			{
				ಯದಿ (*p & 0x80)
				{
					putchar(*p++);
					ಯದಿ (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
