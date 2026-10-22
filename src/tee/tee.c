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
#অন্তর্ভাব <malloc.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <fcntl.h>

#ifdef _WIN32
# define sysconf(x) -1
#endif

শূন্য usage(শূন্য)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its প্রবেশ, and writes to each of the specified files,\n\
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

পূর্ণসংখ্যা
myfread(অক্ষর *buf, পূর্ণসংখ্যা elsize /*ignored*/, পূর্ণসংখ্যা max, FILE *fp)
{
	পূর্ণসংখ্যা	c;
	পূর্ণসংখ্যা	n = 0;

	যখন ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		যদি (c == '\n' || c == '\r')
			বিরতি;
	}
	ফেরত n;
}


পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
	পূর্ণসংখ্যা	append = 0;
	size_t	numfiles;
	পূর্ণসংখ্যা	maxfiles;
	FILE	**filepointers;
	পূর্ণসংখ্যা	i;
	অক্ষর	buf[BUFSIZ];
	পূর্ণসংখ্যা	n;
	পূর্ণসংখ্যা	optind = 1;

	জন্য (i = 1; i < argc; i++)
	{
		যদি (argv[i][0] != '-')
			বিরতি;
		যদি (!strcmp(argv[i], "-a"))
			append++;
		অন্যথা
			usage();
		optind++;
	}

	numfiles = argc - optind;

	যদি (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	যদি (maxfiles < 0)
		maxfiles = 10;
	যদি (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, আকার(FILE *));
	যদি (filepointers == ফাঁকা)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	জন্য (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		যদি (filepointers[i] == ফাঁকা)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			exit(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(stdin),  O_দ্বিমিক);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_দ্বিমিক);
#endif

	যখন ((n = myfread(buf, আকার(অক্ষর), আকার(buf), stdin)) > 0)
	{
		fwrite(buf, আকার(অক্ষর), n, stdout);
		fflush(stdout);
		জন্য (i = 0; i < numfiles; i++)
		{
			যদি (filepointers[i] &&
			     fwrite(buf, আকার(অক্ষর), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = ফাঁকা;
			}
		}
	}
	জন্য (i = 0; i < numfiles; i++)
	{
		যদি (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
