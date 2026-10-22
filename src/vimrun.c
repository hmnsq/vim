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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#অন্তর্ভাব <windows.h>

    পূর্ণাঙ্ক
main(শূন্য)
{
    ধ্রুব wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    পূর্ণাঙ্ক		    retval;
    পূর্ণাঙ্ক		    inquote = 0;
    পূর্ণাঙ্ক		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ধ্রুব wchar_t *)Get_Command_সারি_W();

    /*
     * Skip the executable name, which might be in "".
     */
    যাবত্ (*p)
    {
	যদি (*p == L'"')
	    inquote = !inquote;
	অন্যথা যদি (!inquote && *p == L' ')
	{
	    ++p;
	    ভঙ্গ;
	}
	++p;
    }
    যাবত্ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    যদি (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	যাবত্ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, রিক্ত);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, রিক্ত);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = রিক্ত;
    cmdlen = wcslen(p);
    যদি (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * পরিমাণ(wchar_t));
	যদি (cmd == রিক্ত)
	{
	    perror("vimrun malloc(): ");
	    প্রত্যাগম -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    যদি (cmd)
	মুক্ত(cmd);

    যদি (retval == -1)
	perror("vimrun system(): ");
    অন্যথা যদি (retval != 0)
	printf("shell returned %d\n", retval);

    যদি (!silent)
    {
	puts("Hit any key to close this window...");

	যাবত্ (_kbhit())
	    (শূন্য)_getch();
	(শূন্য)_getch();
    }

    প্রত্যাগম retval;
}
