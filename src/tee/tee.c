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
#അന്തര്ഭാവ <malloc.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <fcntl.h>

#ifdef _WIN32
# define sysconf(x) -1
#endif

ശൂന്യം usage(ശൂന്യം)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its നിവേശം, and writes to each of the specified files,\n\
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

പൂർണ്ണസംഖ്യ
myfread(അക്ഷരം *buf, പൂർണ്ണസംഖ്യ elsize /*ignored*/, പൂർണ്ണസംഖ്യ max, FILE *fp)
{
	പൂർണ്ണസംഖ്യ	c;
	പൂർണ്ണസംഖ്യ	n = 0;

	ആകുമ്പോൾ ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		എങ്കിൽ (c == '\n' || c == '\r')
			നിർത്തുക;
	}
	തിരികെ n;
}


പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
	പൂർണ്ണസംഖ്യ	append = 0;
	size_t	numfiles;
	പൂർണ്ണസംഖ്യ	maxfiles;
	FILE	**filepointers;
	പൂർണ്ണസംഖ്യ	i;
	അക്ഷരം	buf[BUFSIZ];
	പൂർണ്ണസംഖ്യ	n;
	പൂർണ്ണസംഖ്യ	optind = 1;

	വേണ്ടി (i = 1; i < argc; i++)
	{
		എങ്കിൽ (argv[i][0] != '-')
			നിർത്തുക;
		എങ്കിൽ (!strcmp(argv[i], "-a"))
			append++;
		അല്ലെങ്കിൽ
			usage();
		optind++;
	}

	numfiles = argc - optind;

	എങ്കിൽ (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	എങ്കിൽ (maxfiles < 0)
		maxfiles = 10;
	എങ്കിൽ (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, വലുപ്പം(FILE *));
	എങ്കിൽ (filepointers == ശൂന്യ)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	വേണ്ടി (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		എങ്കിൽ (filepointers[i] == ശൂന്യ)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			exit(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(stdin),  O_ദ്വിമാന);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_ദ്വിമാന);
#endif

	ആകുമ്പോൾ ((n = myfread(buf, വലുപ്പം(അക്ഷരം), വലുപ്പം(buf), stdin)) > 0)
	{
		fwrite(buf, വലുപ്പം(അക്ഷരം), n, stdout);
		fflush(stdout);
		വേണ്ടി (i = 0; i < numfiles; i++)
		{
			എങ്കിൽ (filepointers[i] &&
			     fwrite(buf, വലുപ്പം(അക്ഷരം), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = ശൂന്യ;
			}
		}
	}
	വേണ്ടി (i = 0; i < numfiles; i++)
	{
		എങ്കിൽ (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
