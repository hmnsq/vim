// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ضمن <stdio.h>
#ضمن <string.h>

#که_تعریف_شوی _WIN32
#	include <fcntl.h>
#	include <io.h>
#د_که_پای

	صحيح
اصلي(صحيح argc, حرف **argv)
{
	حرف بفر[BUFSIZ];
	حرف *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#که_تعریف_شوی _WIN32
	_setmode(_fileno(stdin), O_دوګونی_2);
	_setmode(_fileno(stdout), O_دوګونی_2);
#د_که_پای

	طالما (fgets(بفر, BUFSIZ, stdin) != عدم)
	{
		لكل (p = بفر; *p != 0; p++)
		{
			إذا (مزي_پرتله(p, "charset=utf-8", 13) == 0
				|| مزي_پرتله(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			وإلا إذا (مزي_پرتله(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			وإلا إذا (*(بینښه حرف *)p == 0x81 && p[1] == '_')
			{
				توری_چاپ_کړه('\\');
				++p;
			}
			وإلا
			{
				إذا (*p & 0x80)
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

// vim:set ts=4 sts=4 sw=4 noet:
