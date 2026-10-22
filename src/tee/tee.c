/*  vim:set ts=4 sw=4:
 *
 *	Copyright (c) 1996, Paul Slootman
 *
 *	Author: Paul Slootman
 *			(paul@wurtel.hobby.nl, paul@murphy.nl, paulS@toecompst.nl)
 *	Modifications for MSVC: Yasuhiro Matsumoto
 *
 *	This source code is released into the public domain. It is provided on an
 *	as-is basis and no responsibility is accepted for its failure to perform
 *	as expected. It is worth at least as much as you paid for it!
 *
 * tee.c - pipe fitting
 *
 * tee reads stdin, and writes what it reads to each of the specified
 * files. The primary reason of existence for this version is a quick
 * and dirty implementation to distribute with Vim, to make one of the
 * most useful features of Vim possible on OS/2: quickfix.
 *
 * Of course, not using tee but instead redirecting make's output directly
 * into a temp file and then processing that is possible, but if we have a
 * system capable of correctly piping (unlike DOS, for example), why not
 * use it as well as possible? This tee should also work on other systems,
 * but it's not been tested there, only on OS/2.
 *
 * tee is also available in the GNU shellutils package, which is available
 * precompiled for OS/2. That one probably works better.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _MSC_VER
# include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <malloc.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
# define sysconf(x) -1
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ usage(ಶೂನ್ಯ)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend ಗಮ್ಯ files instead ದ truncating\n\
\nTee reads its ಒಳಹರಿವು, and writes ಗಮ್ಯ each ದ the specified files,\n\
as well as ಗಮ್ಯ the standard output.\n\
\n\
This ಆವೃತ್ತಿ_2 supplied with Vim 4.2 ಗಮ್ಯ make ':make' possible.\n\
For a ಇನ್ನಷ್ಟು complete and stable ಆವೃತ್ತಿ_2, consider getting\n\
[a ಪೋರ್ಟ್ ದ] the GNU shellutils package.\n\
");
}

/*
 * fread only returns when count is read or at EOF.
 * We could use fgets, but I want to be able to handle binary blubber.
 */

ಪೂರ್ಣಾಂಕ
myfread(ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ elsize /*ignored*/, ಪೂರ್ಣಾಂಕ max, FILE *fp)
{
	ಪೂರ್ಣಾಂಕ	c;
	ಪೂರ್ಣಾಂಕ	n = 0;

	ಆಗ ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		ಆದರೆ (c == '\n' || c == '\r')
			ನಿಲ್ಲಿಸು;
	}
	ಹಿಂತಿರುಗಿ n;
}


ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
	ಪೂರ್ಣಾಂಕ	append = 0;
	size_t	numfiles;
	ಪೂರ್ಣಾಂಕ	maxfiles;
	FILE	**filepointers;
	ಪೂರ್ಣಾಂಕ	i;
	ಅಕ್ಷರ	buf[BUFSIZ];
	ಪೂರ್ಣಾಂಕ	n;
	ಪೂರ್ಣಾಂಕ	optind = 1;

	ಗಾಗಿ (i = 1; i < argc; i++)
	{
		ಆದರೆ (argv[i][0] != '-')
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (!strcmp(argv[i], "-a"))
			append++;
		ಇಲ್ಲದಿದ್ದರೆ
			usage();
		optind++;
	}

	numfiles = argc - optind;

	ಆದರೆ (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		ನಿರ್ಗಮಿಸು(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	ಆದರೆ (maxfiles < 0)
		maxfiles = 10;
	ಆದರೆ (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		ನಿರ್ಗಮಿಸು(1);
	}
	filepointers = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(numfiles, ಗಾತ್ರ(FILE *));
	ಆದರೆ (filepointers == ಖಾಲಿ)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (ದೀರ್ಘ)numfiles);
		ನಿರ್ಗಮಿಸು(1);
	}
	ಗಾಗಿ (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		ಆದರೆ (filepointers[i] == ಖಾಲಿ)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			ನಿರ್ಗಮಿಸು(1);
		}
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
	setmode(fileno(stdin),  O_ದ್ವಿಮಾನ);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_ದ್ವಿಮಾನ);
#ಆದರೆ_ಅಂತ್ಯ

	ಆಗ ((n = myfread(buf, ಗಾತ್ರ(ಅಕ್ಷರ), ಗಾತ್ರ(buf), stdin)) > 0)
	{
		fwrite(buf, ಗಾತ್ರ(ಅಕ್ಷರ), n, stdout);
		fflush(stdout);
		ಗಾಗಿ (i = 0; i < numfiles; i++)
		{
			ಆದರೆ (filepointers[i] &&
			     fwrite(buf, ಗಾತ್ರ(ಅಕ್ಷರ), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = ಖಾಲಿ;
			}
		}
	}
	ಗಾಗಿ (i = 0; i < numfiles; i++)
	{
		ಆದರೆ (filepointers[i])
			fclose(filepointers[i]);
	}

	ನಿರ್ಗಮಿಸು(0);
}
