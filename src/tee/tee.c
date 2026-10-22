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
\nTee reads its आदान, and writes to each of the specified files,\n\
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

पूर्णांक
myfread(अक्षर *buf, पूर्णांक elsize /*ignored*/, पूर्णांक max, खाता *fp)
{
	पूर्णांक	c;
	पूर्णांक	n = 0;

	पर्यंत ((n < max) && ((c = अक्षर_लो()) != खातापूर्ण))
	{
		*(buf++) = c;
		n++;
		जर (c == '\n' || c == '\r')
			थांबा;
	}
	परत n;
}


पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
	पूर्णांक	append = 0;
	माप_प्रकार	numfiles;
	पूर्णांक	maxfiles;
	खाता	**filepointers;
	पूर्णांक	i;
	अक्षर	buf[बफ_मान];
	पूर्णांक	n;
	पूर्णांक	optind = 1;

	साठी (i = 1; i < argc; i++)
	{
		जर (argv[i][0] != '-')
			थांबा;
		जर (!म_भेद(argv[i], "-a"))
			append++;
		अन्यथा
			usage();
		optind++;
	}

	numfiles = argc - optind;

	जर (numfiles == 0)
	{
		ख_लिखो(मानक_त्रुटि, "doesn't make much sense using tee without any file name arguments...\n");
		usage();
		निकास(2);
	}

	maxfiles = sysconf(_SC_OPEN_MAX);	/* or fill in 10 or so */
	जर (maxfiles < 0)
		maxfiles = 10;
	जर (numfiles + 3 > maxfiles)	/* +3 accounts for stdin, out, err */
	{
		ख_लिखो(मानक_त्रुटि, "Sorry, there is a limit of max %d files.\n", maxfiles - 3);
		निकास(1);
	}
	filepointers = सुस्मृति(numfiles, आकार(खाता *));
	जर (filepointers == रिक्त)
	{
		ख_लिखो(मानक_त्रुटि, "Error allocating memory for %ld files\n",
															   (दीर्घ)numfiles);
		निकास(1);
	}
	साठी (i = 0; i < numfiles; i++)
	{
		filepointers[i] = ख_खोलो(argv[i+optind], append ? "ab" : "wb");
		जर (filepointers[i] == रिक्त)
		{
			ख_लिखो(मानक_त्रुटि, "Can't open \"%s\"\n", argv[i+optind]);
			निकास(1);
		}
	}
#ifdef _WIN32
	setmode(fileno(मानक_निवेश),  O_द्विमान);
	ख_साफ(मानक_निकास);	/* needed for _fsetmode(stdout) */
	setmode(fileno(मानक_निकास),  O_द्विमान);
#endif

	पर्यंत ((n = myfread(buf, आकार(अक्षर), आकार(buf), मानक_निवेश)) > 0)
	{
		ख_डालो(buf, आकार(अक्षर), n, मानक_निकास);
		ख_साफ(मानक_निकास);
		साठी (i = 0; i < numfiles; i++)
		{
			जर (filepointers[i] &&
			     ख_डालो(buf, आकार(अक्षर), n, filepointers[i]) != n)
			{
				ख_लिखो(मानक_त्रुटि, "Error writing to file \"%s\"\n", argv[i+optind]);
				ख_बंद(filepointers[i]);
				filepointers[i] = रिक्त;
			}
		}
	}
	साठी (i = 0; i < numfiles; i++)
	{
		जर (filepointers[i])
			ख_बंद(filepointers[i]);
	}

	निकास(0);
}
