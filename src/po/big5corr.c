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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

	ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	ਅੱਖਰ ਬਫਰ[BUFSIZ];
	ਅੱਖਰ *p;

	ਜਦੋਂ (fgets(ਬਫਰ, BUFSIZ, stdin) != ਨਿਰਾ)
	{
		ਲਈ (p = ਬਫਰ; *p != 0; p++)
		{
			ਜੇ (ਲੜੀ_ਤੁਲਨਾ(p, "charset=utf-8", 13) == 0
				|| ਲੜੀ_ਤੁਲਨਾ(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				ਜਦੋਂ (p[1] != '\n')
					++p;
			}
			ਨਹੀਂਤਾਂ
			{
				ਜੇ (*(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)p >= 0xA1)
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
