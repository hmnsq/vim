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

#ifndef _MSC_VER
# include <unistd.h>
#endif
#ಅನ್ತರ್ಭಾವ <malloc.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>

#ifdef _WIN32
# define sysconf(x) -1
#endif

ಶೂನ್ಯ usage(ಶೂನ್ಯ)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its ಒಳಹರಿವು, and writes to each of the specified files,\n\
as well as to the standard output.\n\
\n\
This version supplied with Vim 4.2 to make ':make' possible.\n\
For a more complete and stable version, consider getting\n\
[a port of] the GNU shellutils package.\n\
");
}

/*
 * fread only returns when count is read or at EOF.
 * We could use fgets, but I want to be able to handle binary blubber.
 */

ಪೂರ್ಣಾಙ್ಕ
myfread(ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ elsize /*ignored*/, ಪೂರ್ಣಾಙ್ಕ max, FILE *fp)
{
	ಪೂರ್ಣಾಙ್ಕ	c;
	ಪೂರ್ಣಾಙ್ಕ	n = 0;

	ಯಾವತ್ ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		ಯದಿ (c == '\n' || c == '\r')
			ಭಙ್ಗ;
	}
	ಪ್ರತ್ಯಾಗಮ n;
}


ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
	ಪೂರ್ಣಾಙ್ಕ	append = 0;
	size_t	numfiles;
	ಪೂರ್ಣಾಙ್ಕ	maxfiles;
	FILE	**filepointers;
	ಪೂರ್ಣಾಙ್ಕ	i;
	ವರ್ಣ	buf[BUFSIZ];
	ಪೂರ್ಣಾಙ್ಕ	n;
	ಪೂರ್ಣಾಙ್ಕ	optind = 1;

	ಪ್ರತಿ (i = 1; i < argc; i++)
	{
		ಯದಿ (argv[i][0] != '-')
			ಭಙ್ಗ;
		ಯದಿ (!strcmp(argv[i], "-a"))
			append++;
		ಅನ್ಯಥಾ
			usage();
		optind++;
	}

	numfiles = argc - optind;

	ಯದಿ (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	ಯದಿ (maxfiles < 0)
		maxfiles = 10;
	ಯದಿ (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, ಪರಿಮಾಣ(FILE *));
	ಯದಿ (filepointers == ರಿಕ್ತ)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	ಪ್ರತಿ (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		ಯದಿ (filepointers[i] == ರಿಕ್ತ)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			exit(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(stdin),  O_ದ್ವಿಮಾನ);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_ದ್ವಿಮಾನ);
#endif

	ಯಾವತ್ ((n = myfread(buf, ಪರಿಮಾಣ(ವರ್ಣ), ಪರಿಮಾಣ(buf), stdin)) > 0)
	{
		fwrite(buf, ಪರಿಮಾಣ(ವರ್ಣ), n, stdout);
		fflush(stdout);
		ಪ್ರತಿ (i = 0; i < numfiles; i++)
		{
			ಯದಿ (filepointers[i] &&
			     fwrite(buf, ಪರಿಮಾಣ(ವರ್ಣ), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = ರಿಕ್ತ;
			}
		}
	}
	ಪ್ರತಿ (i = 0; i < numfiles; i++)
	{
		ಯದಿ (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
