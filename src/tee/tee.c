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
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifdef _WIN32
# define sysconf(x) -1
#endif

void usage(void)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its input, and writes to each of the specified files,\n\
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

整数
myfread(char *buf, 整数 elsize /*ignored*/, 整数 max, FILE *fp)
{
	整数	c;
	整数	n = 0;

	当 ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		如果 (c == '\n' || c == '\r')
			break;
	}
	返回 n;
}


整数
主要(整数 argc, char *argv[])
{
	整数	append = 0;
	size_t	numfiles;
	整数	maxfiles;
	FILE	**filepointers;
	整数	i;
	char	buf[BUFSIZ];
	整数	n;
	整数	optind = 1;

	为 (i = 1; i < argc; i++)
	{
		如果 (argv[i][0] != '-')
			break;
		如果 (!strcmp(argv[i], "-a"))
			append++;
		否则
			usage();
		optind++;
	}

	numfiles = argc - optind;

	如果 (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	如果 (maxfiles < 0)
		maxfiles = 10;
	如果 (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, sizeof(FILE *));
	如果 (filepointers == NULL)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	为 (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		如果 (filepointers[i] == NULL)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			exit(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(stdin),  O_BINARY);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_BINARY);
#endif

	当 ((n = myfread(buf, sizeof(char), sizeof(buf), stdin)) > 0)
	{
		fwrite(buf, sizeof(char), n, stdout);
		fflush(stdout);
		为 (i = 0; i < numfiles; i++)
		{
			如果 (filepointers[i] &&
			     fwrite(buf, sizeof(char), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = NULL;
			}
		}
	}
	为 (i = 0; i < numfiles; i++)
	{
		如果 (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
