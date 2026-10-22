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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

	পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
	অক্ষর বাফার[BUFSIZ];
	অক্ষর *p;

	যখন (fgets(বাফার, BUFSIZ, stdin) != ফাঁকা)
	{
		জন্য (p = বাফার; *p != 0; p++)
		{
			যদি (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			অন্যথা যদি (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				যখন (p[1] != '\n')
					++p;
			}
			অন্যথা
			{
				যদি (*(unsigned অক্ষর *)p >= 0xA1)
				{
					putchar(*p++);
					যদি (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}
