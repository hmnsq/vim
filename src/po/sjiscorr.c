// Simplistic program to correct SJIS inside strings.  When a trail byte is a
// backslash it needs to be doubled.
// Public domain.

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#	include <fcntl.h>
#	include <io.h>
#ಆದರೆ_ಅಂತ್ಯ

	ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
	ಅಕ್ಷರ ಬಫರ್[BUFSIZ];
	ಅಕ್ಷರ *p;

	// Windows only: put standard input and output into binary mode so that the
	// line endings in the output match those in the input.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
	_setmode(_fileno(stdin), O_ದ್ವಿಮಾನ_2);
	_setmode(_fileno(stdout), O_ದ್ವಿಮಾನ_2);
#ಆದರೆ_ಅಂತ್ಯ

	ಆಗ (fgets(ಬಫರ್, BUFSIZ, stdin) != ಖಾಲಿ)
	{
		ಗಾಗಿ (p = ಬಫರ್; *p != 0; p++)
		{
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "charset=utf-8", 13) == 0
				|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=CP932", stdout);
				p += 12;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "# Original translations.", 24) == 0)
			{
				p += 24 - 1; // subtracting 1 by considering "p++" in the loop.
				fputs("# Generated from ja.po, DO NOT EDIT.", stdout);
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)p == 0x81 && p[1] == '_')
			{
				ಅಕ್ಷರ_ಬರೆ('\\');
				++p;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
				ಆದರೆ (*p & 0x80)
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

// vim:set ts=4 sts=4 sw=4 noet:
