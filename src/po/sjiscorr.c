// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#ifdef _WIN32
#	include <fcntl.h>
#	include <io.h>
#endif

	പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
	അക്ഷരം buffer[BUFSIZ];
	അക്ഷരം *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ifdef _WIN32
	_setmode(_fileno(stdin), O_ദ്വിമാന_2);
	_setmode(_fileno(stdout), O_ദ്വിമാന_2);
#endif

	ആകുമ്പോൾ (fgets(buffer, BUFSIZ, stdin) != ശൂന്യ)
	{
		വേണ്ടി (p = buffer; *p != 0; p++)
		{
			എങ്കിൽ (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (strncmp(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			അല്ലെങ്കിൽ എങ്കിൽ (*(unsigned അക്ഷരം *)p == 0x81 && p[1] == '_')
			{
				putchar('\\');
				++p;
			}
			അല്ലെങ്കിൽ
			{
				എങ്കിൽ (*p & 0x80)
				{
					putchar(*p++);
					എങ്കിൽ (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
