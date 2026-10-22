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
#அந்தர்பாவ <malloc.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <fcntl.h>

#ifdef _WIN32
# define sysconf(x) -1
#endif

வெறுமை usage(வெறுமை)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its உள்ளீடு, and writes to each of the specified files,\n\
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

முழு
myfread(எழுத்து *buf, முழு elsize /*ignored*/, முழு max, FILE *fp)
{
	முழு	c;
	முழு	n = 0;

	போது ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		எனில் (c == '\n' || c == '\r')
			நிறுத்து;
	}
	திரும்பு n;
}


முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
	முழு	append = 0;
	size_t	numfiles;
	முழு	maxfiles;
	FILE	**filepointers;
	முழு	i;
	எழுத்து	buf[BUFSIZ];
	முழு	n;
	முழு	optind = 1;

	க்கு (i = 1; i < argc; i++)
	{
		எனில் (argv[i][0] != '-')
			நிறுத்து;
		எனில் (!strcmp(argv[i], "-a"))
			append++;
		இல்லையெனில்
			usage();
		optind++;
	}

	numfiles = argc - optind;

	எனில் (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	எனில் (maxfiles < 0)
		maxfiles = 10;
	எனில் (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, அளவு(FILE *));
	எனில் (filepointers == வெற்று)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	க்கு (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		எனில் (filepointers[i] == வெற்று)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			exit(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(stdin),  O_இருமம்);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_இருமம்);
#endif

	போது ((n = myfread(buf, அளவு(எழுத்து), அளவு(buf), stdin)) > 0)
	{
		fwrite(buf, அளவு(எழுத்து), n, stdout);
		fflush(stdout);
		க்கு (i = 0; i < numfiles; i++)
		{
			எனில் (filepointers[i] &&
			     fwrite(buf, அளவு(எழுத்து), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = வெற்று;
			}
		}
	}
	க்கு (i = 0; i < numfiles; i++)
	{
		எனில் (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
