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

#اگر_غیر_تعریف_شدہ _MSC_VER
# include <unistd.h>
#اگر_ختم
#شامل <malloc.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <fcntl.h>

#اگر_تعریف_شدہ _WIN32
# define sysconf(x) -1
#اگر_ختم

خالی usage(خالی)
{
	fprintf(stderr,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend مقصد files instead کا truncating\n\
\nTee reads its داخلہ, and writes مقصد each کا the specified files,\n\
as well as مقصد the standard output.\n\
\n\
This نسخہ_2 supplied with Vim 4.2 مقصد make ':make' possible.\n\
For a مزید complete and stable نسخہ_2, consider getting\n\
[a پورٹ کا] the GNU shellutils package.\n\
");
}

/*
 * fread only returns when count is read or at EOF.
 * We could use fgets, but I want to be able to handle binary blubber.
 */

صحیح
myfread(حرف *buf, صحیح elsize /*ignored*/, صحیح max, FILE *fp)
{
	صحیح	c;
	صحیح	n = 0;

	جب ((n < max) && ((c = getchar()) != EOF))
	{
		*(buf++) = c;
		n++;
		اگر (c == '\n' || c == '\r')
			توڑو;
	}
	واپس n;
}


صحیح
مین(صحیح argc, حرف *argv[])
{
	صحیح	append = 0;
	size_t	numfiles;
	صحیح	maxfiles;
	FILE	**filepointers;
	صحیح	i;
	حرف	buf[BUFSIZ];
	صحیح	n;
	صحیح	optind = 1;

	برائے (i = 1; i < argc; i++)
	{
		اگر (argv[i][0] != '-')
			توڑو;
		اگر (!strcmp(argv[i], "-a"))
			append++;
		ورنہ
			usage();
		optind++;
	}

	numfiles = argc - optind;

	اگر (numfiles == 0)
	{
		fprintf(stderr, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		باہر_نکلو(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	اگر (maxfiles < 0)
		maxfiles = 10;
	اگر (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		fprintf(stderr, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		باہر_نکلو(1);
	}
	filepointers = صفر_حافظہ_دو(numfiles, حجم(FILE *));
	اگر (filepointers == تہی)
	{
		fprintf(stderr, "Error allocating memory for %ld files\n",
															   (طویل)numfiles);
		باہر_نکلو(1);
	}
	برائے (i = 0; i < numfiles; i++)
	{
		filepointers[i] = fopen(argv[i+optind], append ? "ab" : "wb");
		اگر (filepointers[i] == تہی)
		{
			fprintf(stderr, "Can't open \"%s\"\n", argv[i+optind]);
			باہر_نکلو(1);
		}
	}
#اگر_تعریف_شدہ _WIN32
	setmode(fileno(stdin),  O_ثنائی);
	fflush(stdout);	/* needed for _fsetmode(stdout) */
	setmode(fileno(stdout),  O_ثنائی);
#اگر_ختم

	جب ((n = myfread(buf, حجم(حرف), حجم(buf), stdin)) > 0)
	{
		fwrite(buf, حجم(حرف), n, stdout);
		fflush(stdout);
		برائے (i = 0; i < numfiles; i++)
		{
			اگر (filepointers[i] &&
			     fwrite(buf, حجم(حرف), n, filepointers[i]) != n)
			{
				fprintf(stderr, "Error writing to file \"%s\"\n", argv[i+optind]);
				fclose(filepointers[i]);
				filepointers[i] = تہی;
			}
		}
	}
	برائے (i = 0; i < numfiles; i++)
	{
		اگر (filepointers[i])
			fclose(filepointers[i]);
	}

	باہر_نکلو(0);
}
