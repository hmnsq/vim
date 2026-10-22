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

#include <stdio.h>
#include <string.h>

	số_nguyên
main(số_nguyên argc, char **argv)
{
	char buffer[BUFSIZ];
	char *p;

	trong_khi (fgets(buffer, BUFSIZ, stdin) != NULL)
	{
		cho (p = buffer; *p != 0; p++)
		{
			nếu (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			không_thì nếu (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				trong_khi (p[1] != '\n')
					++p;
			}
			không_thì
			{
				nếu (*(unsigned char *)p >= 0xA1)
				{
					putchar(*p++);
					nếu (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}
