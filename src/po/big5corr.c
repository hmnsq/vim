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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

	ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
	ವರ್ಣ buffer[BUFSIZ];
	ವರ್ಣ *p;

	ಯಾವತ್ (fgets(buffer, BUFSIZ, stdin) != ರಿಕ್ತ)
	{
		ಪ್ರತಿ (p = buffer; *p != 0; p++)
		{
			ಯದಿ (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			ಅನ್ಯಥಾ ಯದಿ (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				ಯಾವತ್ (p[1] != '\n')
					++p;
			}
			ಅನ್ಯಥಾ
			{
				ಯದಿ (*(unsigned ವರ್ಣ *)p >= 0xA1)
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
