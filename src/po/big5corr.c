/*
 * Simplistic program to correct Big5 inside strings.  When a trail byte is a
 * backslash it needs to be doubled.
 * Public domain.
 */

/*
 * 06.11.23, added by Restorer:
 * For more details, see:
 * https://github.com/vim/vim/pull/3261
 * https://github.com/vim/vim/pull/3476
 * https://github.com/vim/vim/pull/12153
 * (read all comments)
 *
 * I checked the workability on the list of backslash characters
 * specified in zh_TW.UTF-8.po. It works.
 * But it is better to have someone native speaker check it.
 *
 */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

	പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
	അക്ഷരം buffer[BUFSIZ];
	അക്ഷരം *p;

	ആകുമ്പോൾ (fgets(buffer, BUFSIZ, stdin) != ശൂന്യ)
	{
		വേണ്ടി (p = buffer; *p != 0; p++)
		{
			എങ്കിൽ (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				ആകുമ്പോൾ (p[1] != '\n')
					++p;
			}
			അല്ലെങ്കിൽ
			{
				എങ്കിൽ (*(unsigned അക്ഷരം *)p >= 0xA1)
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
