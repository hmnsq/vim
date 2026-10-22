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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

	ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
	ಅಕ್ಷರ ಬಫರ್[BUFSIZ];
	ಅಕ್ಷರ *p;

	ಆಗ (fgets(ಬಫರ್, BUFSIZ, stdin) != ಖಾಲಿ)
	{
		ಗಾಗಿ (p = ಬಫರ್; *p != 0; p++)
		{
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "charset=utf-8", 13) == 0
				|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				ಆಗ (p[1] != '\n')
					++p;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
				ಆದರೆ (*(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)p >= 0xA1)
				{
					ಅಕ್ಷರ_ಬರೆ(*p++);
					ಆದರೆ (*p == '\\')
						ಅಕ್ಷರ_ಬರೆ(*p);
				}
				ಅಕ್ಷರ_ಬರೆ(*p);
			}
		}
	}
}
