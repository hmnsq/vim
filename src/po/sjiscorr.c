// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#నిర్వచితమైతే _WIN32
#	include <fcntl.h>
#	include <io.h>
#అయితే_ముగింపు

	పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
	అక్షరం బఫరు[BUFSIZ];
	అక్షరం *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#నిర్వచితమైతే _WIN32
	_setmode(_fileno(stdin), O_ద్విమాన_2);
	_setmode(_fileno(stdout), O_ద్విమాన_2);
#అయితే_ముగింపు

	అయితే (fgets(బఫరు, BUFSIZ, stdin) != ఖాళీ)
	{
		కొరకు (p = బఫరు; *p != 0; p++)
		{
			ఒకవేళ (అక్షరశ్రేణి_పోలిక(p, "charset=utf-8", 13) == 0
				|| అక్షరశ్రేణి_పోలిక(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			లేకపోతే ఒకవేళ (*(గుర్తులేని అక్షరం *)p == 0x81 && p[1] == '_')
			{
				అక్షరం_రాయి('\\');
				++p;
			}
			లేకపోతే
			{
				ఒకవేళ (*p & 0x80)
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

// vim:set ts=4 sts=4 sw=4 noet:
