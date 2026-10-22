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

#ضمن <stdio.h>
#ضمن <string.h>

	صحيح
اصلي(صحيح argc, حرف **argv)
{
	حرف بفر[BUFSIZ];
	حرف *p;

	طالما (fgets(بفر, BUFSIZ, stdin) != عدم)
	{
		لكل (p = بفر; *p != 0; p++)
		{
			إذا (مزي_پرتله(p, "charset=utf-8", 13) == 0
				|| مزي_پرتله(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			وإلا إذا (مزي_پرتله(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				طالما (p[1] != '\n')
					++p;
			}
			وإلا
			{
				إذا (*(بینښه حرف *)p >= 0xA1)
				{
					توری_چاپ_کړه(*p++);
					إذا (*p == '\\')
						توری_چاپ_کړه(*p);
				}
				توری_چاپ_کړه(*p);
			}
		}
	}
}
