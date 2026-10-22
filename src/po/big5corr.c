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

#شامل <stdio.h>
#شامل <string.h>

	صحیح
اصلی(صحیح argc, نویسه **argv)
{
	نویسه buffer[BUFSIZ];
	نویسه *p;

	وقتی (fgets(buffer, BUFSIZ, stdin) != پوچ)
	{
		برای (p = buffer; *p != 0; p++)
		{
			اگر (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			وگرنه اگر (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				وقتی (p[1] != '\n')
					++p;
			}
			وگرنه
			{
				اگر (*(unsigned نویسه *)p >= 0xA1)
				{
					putchar(*p++);
					اگر (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}
