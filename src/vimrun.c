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

    p = (ثابت wchar_t *)Get_Command_قۇر_W();

    /*
     * Skip the executable name, which might be in "".
     */
    طالما (*p)
    {
	إذا (*p == L'"')
	    inquote = !inquote;
	وإلا إذا (!inquote && *p == L' ')
	{
	    ++p;
	    اكسر;
	}
	++p;
    }
    طالما (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    إذا (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	طالما (*p == L' ')
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
    إذا (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * حجم(wchar_t));
	إذا (cmd == عدم)
	{
	    perror("vimrun malloc(): ");
	    أرجع -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    إذا (cmd)
	حرر(cmd);

    إذا (retval == -1)
	perror("vimrun system(): ");
    وإلا إذا (retval != 0)
	printf("shell returned %d\n", retval);

    إذا (!silent)
    {
	puts("Hit any key to close this window...");

	طالما (_kbhit())
	    (فراغ)_getch();
	(فراغ)_getch();
    }

    أرجع retval;
}
