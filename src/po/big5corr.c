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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
	ଅକ୍ଷର buffer[BUFSIZ];
	ଅକ୍ଷର *p;

	ଯେତେବେଳେ (fgets(buffer, BUFSIZ, stdin) != ଖାଲି)
	{
		ପାଇଁ (p = buffer; *p != 0; p++)
		{
			ଯଦି (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			ଅନ୍ୟଥା ଯଦି (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				ଯେତେବେଳେ (p[1] != '\n')
					++p;
			}
			ଅନ୍ୟଥା
			{
				ଯଦି (*(unsigned ଅକ୍ଷର *)p >= 0xA1)
				{
					putchar(*p++);
					ଯଦି (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}
