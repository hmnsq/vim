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

#నిర్వచితం_కాకపోతే _MSC_VER
# include <unistd.h>
#అయితే_ముగింపు
#అన్తర్భావ <malloc.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <fcntl.h>

#నిర్వచితమైతే _WIN32
# define sysconf(x) -1
#అయితే_ముగింపు

శూన్య usage(శూన్య)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend గమ్యం files instead యొక్క truncating\n\
\nTee reads its ఆదానం, and writes గమ్యం each యొక్క the specified files,\n\
as well as గమ్యం the standard output.\n\
\n\
This సంస్కరణ_2 supplied with Vim 4.2 గమ్యం make ':make' possible.\n\
For a మరిన్ని complete and stable సంస్కరణ_2, consider getting\n\
[a పోర్టు యొక్క] the GNU shellutils package.\n\
");
}

/*
 * fread only returns when count is read or at EOF.
 * We could use fgets, but I want to be able to handle binary blubber.
 */

పూర్ణాంకం
myfread(అక్షరం *buf, పూర్ణాంకం elsize /*ignored*/, పూర్ణాంకం max, FILE *fp)
{
	పూర్ణాంకం	c;
	పూర్ణాంకం	n = 0;

	అయితే ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		ఒకవేళ (c == '\n' || c == '\r')
			ఆపు;
	}
	తిరిగి n;
}


పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
	పూర్ణాంకం	append = 0;
	size_t	numfiles;
	పూర్ణాంకం	maxfiles;
	FILE	**filepointers;
	పూర్ణాంకం	i;
	అక్షరం	buf[BUFSIZ];
	పూర్ణాంకం	n;
	పూర్ణాంకం	optind = 1;

	కొరకు (i = 1; i < argc; i++)
	{
		ఒకవేళ (argv[i][0] != '-')
			ఆపు;
		ఒకవేళ (!strcmp(argv[i], "-a"))
			append++;
		లేకపోతే
			usage();
		optind++;
	}

	numfiles = argc - optind;

	ఒకవేళ (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		నిష్క్రమించు(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	ఒకవేళ (maxfiles < 0)
		maxfiles = 10;
	ఒకవేళ (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		నిష్క్రమించు(1);
	}
	filepointers = సున్నా_జ్ఞాపకం_ఇవ్వు(numfiles, పరిమాణం(FILE *));
	ఒకవేళ (filepointers == ఖాళీ)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (దీర్ఘ)numfiles);
		నిష్క్రమించు(1);
	}
	కొరకు (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		ఒకవేళ (filepointers[i] == ఖాళీ)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			నిష్క్రమించు(1);
		}
	}
#నిర్వచితమైతే _WIN32
	setmode(fileno(stdin),  O_ద్విమాన);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_ద్విమాన);
#అయితే_ముగింపు

	అయితే ((n = myfread(buf, పరిమాణం(అక్షరం), పరిమాణం(buf), stdin)) > 0)
	{
		fwrite(buf, పరిమాణం(అక్షరం), n, stdout);
		fflush(stdout);
		కొరకు (i = 0; i < numfiles; i++)
		{
			ఒకవేళ (filepointers[i] &&
			     fwrite(buf, పరిమాణం(అక్షరం), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = ఖాళీ;
			}
		}
	}
	కొరకు (i = 0; i < numfiles; i++)
	{
		ఒకవేళ (filepointers[i])
			fclose(filepointers[i]);
	}

	నిష్క్రమించు(0);
}
