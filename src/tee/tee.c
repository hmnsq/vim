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

ကိန်းပြည့်
myfread(var_na *buf, ကိန်းပြည့် elsize /*ignored*/, ကိန်းပြည့် max, FILE *fp)
{
	ကိန်းပြည့်	c;
	ကိန်းပြည့်	n = 0;

	နေစဉ် ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		အကယ်၍ (c == '\n' || c == '\r')
			bha_nga;
	}
	ပြန်ပေး n;
}


ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
	ကိန်းပြည့်	append = 0;
	size_t	numfiles;
	ကိန်းပြည့်	maxfiles;
	FILE	**filepointers;
	ကိန်းပြည့်	i;
	var_na	buf[BUFSIZ];
	ကိန်းပြည့်	n;
	ကိန်းပြည့်	optind = 1;

	အတွက် (i = 1; i < argc; i++)
	{
		အကယ်၍ (argv[i][0] != '-')
			bha_nga;
		အကယ်၍ (!strcmp(argv[i], "-a"))
			append++;
		မဟုတ်ပါက
			usage();
		optind++;
	}

	numfiles = argc - optind;

	အကယ်၍ (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	အကယ်၍ (maxfiles < 0)
		maxfiles = 10;
	အကယ်၍ (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, parimaa_na(FILE *));
	အကယ်၍ (filepointers == NULL)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	အတွက် (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		အကယ်၍ (filepointers[i] == NULL)
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

	နေစဉ် ((n = myfread(buf, parimaa_na(var_na), parimaa_na(buf), stdin)) > 0)
	{
		fwrite(buf, parimaa_na(var_na), n, stdout);
		fflush(stdout);
		အတွက် (i = 0; i < numfiles; i++)
		{
			အကယ်၍ (filepointers[i] &&
			     fwrite(buf, parimaa_na(var_na), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = NULL;
			}
		}
	}
	အတွက် (i = 0; i < numfiles; i++)
	{
		အကယ်၍ (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
