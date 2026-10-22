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
	ख_लिखो(मानक_त्रुटि,
"tee usage:\n\
\ttee [-a] file ... file_n\n\
\n\
\t-a\tappend to files instead of truncating\n\
\nTee reads its आगत, and writes to each of the specified files,\n\
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
myfread(वर्ण *buf, पूर्णाङ्क elsize /*ignored*/, पूर्णाङ्क max, खाता *fp)
{
	पूर्णाङ्क	c;
	पूर्णाङ्क	n = 0;

	जबसम्म ((n < max) && ((c = अक्षर_लो()) != खातापूर्ण))
	{
		*(buf++) = c;
		n++;
		यदि (c == '\n' || c == '\r')
			रोक;
	}
	फर्काऊ n;
}


पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क	append = 0;
	माप_प्रकार	numfiles;
	पूर्णाङ्क	maxfiles;
	खाता	**filepointers;
	पूर्णाङ्क	i;
	वर्ण	buf[बफ_मान];
	पूर्णाङ्क	n;
	पूर्णाङ्क	optind = 1;

	लागि (i = 1; i < argc; i++)
	{
		यदि (argv[i][0] != '-')
			रोक;
		यदि (!म_भेद(argv[i], "-a"))
			append++;
		अन्यथा
			usage();
		optind++;
	}

	numfiles = argc - optind;

	यदि (numfiles == 0)
	{
		ख_लिखो(मानक_त्रुटि, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		निकास(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	यदि (maxfiles < 0)
		maxfiles = 10;
	यदि (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		ख_लिखो(मानक_त्रुटि, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		निकास(1);
	}
	filepointers = सुस्मृति(numfiles, परिमाण(खाता *));
	यदि (filepointers == रिक्त)
	{
		ख_लिखो(मानक_त्रुटि, "Error allocating memory for %ld files\n",
															   (दीर्घ)numfiles);
		निकास(1);
	}
	लागि (i = 0; i < numfiles; i++)
	{
		filepointers[i] = ख_खोलो(argv[i+optind], append ? "ab" : "wb");
		यदि (filepointers[i] == रिक्त)
		{
			ख_लिखो(मानक_त्रुटि, "Can't open \"%s\"\n", argv[i+optind]);
			निकास(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(मानक_निवेश),  O_द्विआधारी);
	ख_साफ(मानक_निकास);	/* needed for _fsetmode(stdout) */
	setmode(fileno(मानक_निकास),  O_द्विआधारी);
#endif

	जबसम्म ((n = myfread(buf, परिमाण(वर्ण), परिमाण(buf), मानक_निवेश)) > 0)
	{
		ख_डालो(buf, परिमाण(वर्ण), n, मानक_निकास);
		ख_साफ(मानक_निकास);
		लागि (i = 0; i < numfiles; i++)
		{
			यदि (filepointers[i] &&
			     ख_डालो(buf, परिमाण(वर्ण), n, filepointers[i]) != n)
			{
				ख_लिखो(मानक_त्रुटि, "Error writing to file \"%s\"\n", argv[i+optind]);
				ख_बंद(filepointers[i]);
				filepointers[i] = रिक्त;
			}
		}
	}
	लागि (i = 0; i < numfiles; i++)
	{
		यदि (filepointers[i])
			ख_बंद(filepointers[i]);
	}

	निकास(0);
}
