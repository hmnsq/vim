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

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

    整数
主要(void)
{
    const wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    整数		    retval;
    整数		    inquote = 0;
    整数		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (const wchar_t *)GetCommandLineW();

    /*
     * Skip the executable name, which might be in "".
     */
    当 (*p)
    {
	如果 (*p == L'"')
	    inquote = !inquote;
	否则 如果 (!inquote && *p == L' ')
	{
	    ++p;
	    break;
	}
	++p;
    }
    当 (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    如果 (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	当 (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, NULL);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, NULL);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = NULL;
    cmdlen = wcslen(p);
    如果 (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * sizeof(wchar_t));
	如果 (cmd == NULL)
	{
	    perror("vimrun malloc(): ");
	    返回 -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    如果 (cmd)
	free(cmd);

    如果 (retval == -1)
	perror("vimrun system(): ");
    否则 如果 (retval != 0)
	printf("shell returned %d\n", retval);

    如果 (!silent)
    {
	puts("Hit any key to close this window...");

	当 (_kbhit())
	    (void)_getch();
	(void)_getch();
    }

    返回 retval;
}
