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

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#ضمن <windows.h>

    صحيح
main(فراغ)
{
    ثابت wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    صحيح		    retval;
    صحيح		    inquote = 0;
    صحيح		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ثابت wchar_t *)GetCommandLineW();

    /*
     * Skip the executable name, which might be in "".
     */
    ܟܡܐ (*p)
    {
	ܐܢ (*p == L'"')
	    inquote = !inquote;
	ܐܚܪܢܐ ܐܢ (!inquote && *p == L' ')
	{
	    ++p;
	    اكسر;
	}
	++p;
    }
    ܟܡܐ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ܐܢ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ܟܡܐ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, عدم);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, عدم);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = عدم;
    cmdlen = wcslen(p);
    ܐܢ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * حجم(wchar_t));
	ܐܢ (cmd == عدم)
	{
	    perror("vimrun malloc(): ");
	    ܦܢܐ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ܐܢ (cmd)
	حرر(cmd);

    ܐܢ (retval == -1)
	perror("vimrun system(): ");
    ܐܚܪܢܐ ܐܢ (retval != 0)
	printf("shell returned %d\n", retval);

    ܐܢ (!silent)
    {
	puts("Hit any key to close this window...");

	ܟܡܐ (_kbhit())
	    (فراغ)_getch();
	(فراغ)_getch();
    }

    ܦܢܐ retval;
}
