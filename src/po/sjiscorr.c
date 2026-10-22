// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ضمن <stdio.h>
#ضمن <string.h>

#اگر_تعریف_شده _WIN32
#	include <fcntl.h>
#	include <io.h>
#پایان_اگر

	صحيح
اصلی(صحيح argc, حرف **argv)
{
	حرف میانگیر[BUFSIZ];
	حرف *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#اگر_تعریف_شده _WIN32
	_setmode(_fileno(stdin), O_دودویی_2);
	_setmode(_fileno(stdout), O_دودویی_2);
#پایان_اگر

	طالما (fgets(میانگیر, BUFSIZ, stdin) != عدم)
	{
		لكل (p = میانگیر; *p != 0; p++)
		{
			إذا (مقایسه_رشته(p, "charset=utf-8", 13) == 0
				|| مقایسه_رشته(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			وإلا إذا (مقایسه_رشته(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			وإلا إذا (*(بیعلامت حرف *)p == 0x81 && p[1] == '_')
			{
				حرف_چاپ_کن('\\');
				++p;
			}
			وإلا
			{
				إذا (*p & 0x80)
				{
					حرف_چاپ_کن(*p++);
					إذا (*p == '\\')
						حرف_چاپ_کن(*p);
				}
				حرف_چاپ_کن(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
