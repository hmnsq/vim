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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _MSC_VER
# include <unistd.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <malloc.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
# define sysconf(x) -1
#ਜੇ_ਅੰਤ

ਖਾਲੀ usage(ਖਾਲੀ)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend ਮੰਜ਼ਿਲ files instead ਦਾ truncating\n\
\nTee reads its ਨਿਵੇਸ਼, and writes ਮੰਜ਼ਿਲ each ਦਾ the specified files,\n\
as well as ਮੰਜ਼ਿਲ the standard output.\n\
\n\
This ਸੰਸਕਰਣ_2 supplied with Vim 4.2 ਮੰਜ਼ਿਲ make ':make' possible.\n\
For a ਹੋਰ complete and stable ਸੰਸਕਰਣ_2, consider getting\n\
[a ਪੋਰਟ ਦਾ] the GNU shellutils package.\n\
");
}

/*
 * fread only returns when count is read or at EOF.
 * We could use fgets, but I want to be able to handle binary blubber.
 */

ਪੂਰਨ
myfread(ਅੱਖਰ *buf, ਪੂਰਨ elsize /*ignored*/, ਪੂਰਨ max, FILE *fp)
{
	ਪੂਰਨ	c;
	ਪੂਰਨ	n = 0;

	ਜਦੋਂ ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		ਜੇ (c == '\n' || c == '\r')
			ਰੋਕੋ;
	}
	ਵਾਪਸ n;
}


ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
	ਪੂਰਨ	append = 0;
	size_t	numfiles;
	ਪੂਰਨ	maxfiles;
	FILE	**filepointers;
	ਪੂਰਨ	i;
	ਅੱਖਰ	buf[BUFSIZ];
	ਪੂਰਨ	n;
	ਪੂਰਨ	optind = 1;

	ਲਈ (i = 1; i < argc; i++)
	{
		ਜੇ (argv[i][0] != '-')
			ਰੋਕੋ;
		ਜੇ (!strcmp(argv[i], "-a"))
			append++;
		ਨਹੀਂਤਾਂ
			usage();
		optind++;
	}

	numfiles = argc - optind;

	ਜੇ (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		ਬਾਹਰ_ਨਿਕਲੋ(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	ਜੇ (maxfiles < 0)
		maxfiles = 10;
	ਜੇ (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
	filepointers = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(numfiles, ਆਕਾਰ(FILE *));
	ਜੇ (filepointers == ਨਿਰਾ)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (ਲੰਮਾ)numfiles);
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
	ਲਈ (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		ਜੇ (filepointers[i] == ਨਿਰਾ)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			ਬਾਹਰ_ਨਿਕਲੋ(1);
		}
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
	setmode(fileno(stdin),  O_ਦੁਆਧਾਰੀ);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_ਦੁਆਧਾਰੀ);
#ਜੇ_ਅੰਤ

	ਜਦੋਂ ((n = myfread(buf, ਆਕਾਰ(ਅੱਖਰ), ਆਕਾਰ(buf), stdin)) > 0)
	{
		fwrite(buf, ਆਕਾਰ(ਅੱਖਰ), n, stdout);
		fflush(stdout);
		ਲਈ (i = 0; i < numfiles; i++)
		{
			ਜੇ (filepointers[i] &&
			     fwrite(buf, ਆਕਾਰ(ਅੱਖਰ), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = ਨਿਰਾ;
			}
		}
	}
	ਲਈ (i = 0; i < numfiles; i++)
	{
		ਜੇ (filepointers[i])
			fclose(filepointers[i]);
	}

	ਬਾਹਰ_ਨਿਕਲੋ(0);
}
