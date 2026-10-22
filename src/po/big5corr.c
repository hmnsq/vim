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

	পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
	বর্ণ buffer[BUFSIZ];
	বর্ণ *p;

	যাবত্ (fgets(buffer, BUFSIZ, stdin) != রিক্ত)
	{
		প্রতি (p = buffer; *p != 0; p++)
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
				যাবত্ (p[1] != '\n')
					++p;
			}
			অন্যথা
			{
				যদি (*(unsigned বর্ণ *)p >= 0xA1)
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
