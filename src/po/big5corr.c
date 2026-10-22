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

	정수
main(정수 argc, char **argv)
{
	char buffer[BUFSIZ];
	char *p;

	동안 (fgets(buffer, BUFSIZ, stdin) != NULL)
	{
		위해 (p = buffer; *p != 0; p++)
		{
			만약 (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			아니면 만약 (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				동안 (p[1] != '\n')
					++p;
			}
			아니면
			{
				만약 (*(unsigned char *)p >= 0xA1)
				{
					putchar(*p++);
					만약 (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}
