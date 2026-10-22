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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

	పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
	అక్షరం బఫరు[BUFSIZ];
	అక్షరం *p;

	అయితే (fgets(బఫరు, BUFSIZ, stdin) != ఖాళీ)
	{
		కొరకు (p = బఫరు; *p != 0; p++)
		{
			ఒకవేళ (అక్షరశ్రేణి_పోలిక(p, "charset=utf-8", 13) == 0
				|| అక్షరశ్రేణి_పోలిక(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				అయితే (p[1] != '\n')
					++p;
			}
			లేకపోతే
			{
				ఒకవేళ (*(గుర్తులేని అక్షరం *)p >= 0xA1)
				{
					అక్షరం_రాయి(*p++);
					ఒకవేళ (*p == '\\')
						అక్షరం_రాయి(*p);
				}
				అక్షరం_రాయి(*p);
			}
		}
	}
}
