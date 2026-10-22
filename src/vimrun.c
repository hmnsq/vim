/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			this file by Vince Negri
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vimrun.c - Tiny Win32 program to safely run an external command in a
 *	      DOS console.
 *	      This program is required to avoid that typing CTRL-C in the DOS
 *	      console kills Vim.  Now it only kills vimrun.
 */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#अन्तर्भाव <windows.h>

    पूर्णाङ्क
main(शून्य)
{
    ध्रुव wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    पूर्णाङ्क		    retval;
    पूर्णाङ्क		    inquote = 0;
    पूर्णाङ्क		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ध्रुव wchar_t *)Get_Command_ओळ_W();

    /*
     * Skip the executable name, which might be in "".
     */
    यावत् (*p)
    {
	यदि (*p == L'"')
	    inquote = !inquote;
	अन्यथा यदि (!inquote && *p == L' ')
	{
	    ++p;
	    भङ्ग;
	}
	++p;
    }
    यावत् (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    यदि (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	यावत् (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, रिक्त);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, रिक्त);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = रिक्त;
    cmdlen = wcslen(p);
    यदि (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * परिमाण(wchar_t));
	यदि (cmd == रिक्त)
	{
	    perror("vimrun malloc(): ");
	    प्रत्यागम -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    यदि (cmd)
	मुक्त(cmd);

    यदि (retval == -1)
	perror("vimrun system(): ");
    अन्यथा यदि (retval != 0)
	printf("shell returned %d\n", retval);

    यदि (!silent)
    {
	puts("Hit any key to close this window...");

	यावत् (_kbhit())
	    (शून्य)_getch();
	(शून्य)_getch();
    }

    प्रत्यागम retval;
}
