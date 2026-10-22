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
#अन्तर्भाव <malloc.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <fcntl.h>

#ifdef _WIN32
# define sysconf(x) -1
#endif

शून्य usage(शून्य)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its निवेशः, and writes to each of the specified files,\n\
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

पूर्णाङ्क
myfread(वर्ण *buf, पूर्णाङ्क elsize /*ignored*/, पूर्णाङ्क max, FILE *fp)
{
	पूर्णाङ्क	c;
	पूर्णाङ्क	n = 0;

	याव ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		सचे (c == '\n' || c == '\r')
			भङ्ग;
	}
	पटिनिवत्त n;
}


पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क	append = 0;
	size_t	numfiles;
	पूर्णाङ्क	maxfiles;
	FILE	**filepointers;
	पूर्णाङ्क	i;
	वर्ण	buf[BUFSIZ];
	पूर्णाङ्क	n;
	पूर्णाङ्क	optind = 1;

	अत्थाय (i = 1; i < argc; i++)
	{
		सचे (argv[i][0] != '-')
			भङ्ग;
		सचे (!strcmp(argv[i], "-a"))
			append++;
		अञ्ञथा
			usage();
		optind++;
	}

	numfiles = argc - optind;

	सचे (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		exit(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	सचे (maxfiles < 0)
		maxfiles = 10;
	सचे (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		exit(1);
	}
	filepointers = calloc(numfiles, परिमाण(FILE *));
	सचे (filepointers == रिक्त)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (long)numfiles);
		exit(1);
	}
	अत्थाय (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		सचे (filepointers[i] == रिक्त)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			exit(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(stdin),  O_द्विमान);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_द्विमान);
#endif

	याव ((n = myfread(buf, परिमाण(वर्ण), परिमाण(buf), stdin)) > 0)
	{
		fwrite(buf, परिमाण(वर्ण), n, stdout);
		fflush(stdout);
		अत्थाय (i = 0; i < numfiles; i++)
		{
			सचे (filepointers[i] &&
			     fwrite(buf, परिमाण(वर्ण), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = रिक्त;
			}
		}
	}
	अत्थाय (i = 0; i < numfiles; i++)
	{
		सचे (filepointers[i])
			fclose(filepointers[i]);
	}

	exit(0);
}
