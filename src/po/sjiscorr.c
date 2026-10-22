// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ضمن <stdio.h>
#ضمن <string.h>

#جيڪڏهن_وصف_ٿيل _WIN32
#	include <fcntl.h>
#	include <io.h>
#جيڪڏهن_پورو

	صحيح
مکيه(صحيح argc, حرف **argv)
{
	حرف بفر[BUFSIZ];
	حرف *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#جيڪڏهن_وصف_ٿيل _WIN32
	_setmode(_fileno(stdin), O_ٻٽو_2);
	_setmode(_fileno(stdout), O_ٻٽو_2);
#جيڪڏهن_پورو

	طالما (fgets(بفر, BUFSIZ, stdin) != عدم)
	{
		لكل (p = بفر; *p != 0; p++)
		{
			إذا (لڙي_ڀيٽ(p, "charset=utf-8", 13) == 0
				|| لڙي_ڀيٽ(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			وإلا إذا (لڙي_ڀيٽ(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			وإلا إذا (*(بنانشان حرف *)p == 0x81 && p[1] == '_')
			{
				اکر_لکو('\\');
				++p;
			}
			وإلا
			{
				إذا (*p & 0x80)
				{
					اکر_لکو(*p++);
					إذا (*p == '\\')
						اکر_لکو(*p);
				}
				اکر_لکو(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
