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
مین(صحیح argc, حرف **argv)
{
	حرف بفر[BUFSIZ];
	حرف *p;

	جب (fgets(بفر, BUFSIZ, stdin) != تہی)
	{
		برائے (p = بفر; *p != 0; p++)
		{
			اگر (لڑی_موازنہ(p, "charset=utf-8", 13) == 0
				|| لڑی_موازنہ(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			ورنہ اگر (لڑی_موازنہ(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				جب (p[1] != '\n')
					++p;
			}
			ورنہ
			{
				اگر (*(غیرعلامتی حرف *)p >= 0xA1)
				{
					حرف_لکھو(*p++);
					اگر (*p == '\\')
						حرف_لکھو(*p);
				}
				حرف_لکھو(*p);
			}
		}
	}
}
