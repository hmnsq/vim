// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#	include <fcntl.h>
#	include <io.h>
#ਜੇ_ਅੰਤ

	ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	ਅੱਖਰ ਬਫਰ[BUFSIZ];
	ਅੱਖਰ *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
	_setmode(_fileno(stdin), O_ਦੁਆਧਾਰੀ_2);
	_setmode(_fileno(stdout), O_ਦੁਆਧਾਰੀ_2);
#ਜੇ_ਅੰਤ

	ਜਦੋਂ (fgets(ਬਫਰ, BUFSIZ, stdin) != ਨਿਰਾ)
	{
		ਲਈ (p = ਬਫਰ; *p != 0; p++)
		{
			ਜੇ (ਲੜੀ_ਤੁਲਨਾ(p, "charset=utf-8", 13) == 0
				|| ਲੜੀ_ਤੁਲਨਾ(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			ਨਹੀਂਤਾਂ ਜੇ (*(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)p == 0x81 && p[1] == '_')
			{
				ਅੱਖਰ_ਲਿਖੋ('\\');
				++p;
			}
			ਨਹੀਂਤਾਂ
			{
				ਜੇ (*p & 0x80)
				{
					ਅੱਖਰ_ਲਿਖੋ(*p++);
					ਜੇ (*p == '\\')
						ਅੱਖਰ_ਲਿਖੋ(*p);
				}
				ਅੱਖਰ_ਲਿਖੋ(*p);
			}
		}
	}
}

// vim:set ts=4 sts=4 sw=4 noet:
