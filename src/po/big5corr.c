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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

	முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
	எழுத்து buffer[BUFSIZ];
	எழுத்து *p;

	போது (fgets(buffer, BUFSIZ, stdin) != வெற்று)
	{
		க்கு (p = buffer; *p != 0; p++)
		{
			எனில் (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			இல்லையெனில் எனில் (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				போது (p[1] != '\n')
					++p;
			}
			இல்லையெனில்
			{
				எனில் (*(unsigned எழுத்து *)p >= 0xA1)
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
