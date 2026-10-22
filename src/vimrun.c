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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#شامل <windows.h>

    صحیح
اصلی(تهی)
{
    ثابت wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    صحیح		    retval;
    صحیح		    inquote = 0;
    صحیح		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ثابت wchar_t *)Get_Command_سطر_W();

    /*
     * Skip the executable name, which might be in "".
     */
    وقتی (*p)
    {
	اگر (*p == L'"')
	    inquote = !inquote;
	وگرنه اگر (!inquote && *p == L' ')
	{
	    ++p;
	    بشکن;
	}
	++p;
    }
    وقتی (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    اگر (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	وقتی (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, پوچ);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, پوچ);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = پوچ;
    cmdlen = wcslen(p);
    اگر (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * اندازه(wchar_t));
	اگر (cmd == پوچ)
	{
	    perror("vimrun malloc(): ");
	    بازگشت -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    اگر (cmd)
	آزاد(cmd);

    اگر (retval == -1)
	perror("vimrun system(): ");
    وگرنه اگر (retval != 0)
	printf("shell returned %d\n", retval);

    اگر (!silent)
    {
	puts("Hit any key to close this window...");

	وقتی (_kbhit())
	    (تهی)_getch();
	(تهی)_getch();
    }

    بازگشت retval;
}
