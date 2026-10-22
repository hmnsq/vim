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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <conio.h>
#నిర్వచితం_కాకపోతే WIN32_LEAN_AND_సగటు
# define WIN32_LEAN_AND_సగటు
#అయితే_ముగింపు
#అన్తర్భావ <windows.h>

    పూర్ణాంకం
ప్రధాన(శూన్య)
{
    స్థిరరాశి wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    పూర్ణాంకం		    retval;
    పూర్ణాంకం		    inquote = 0;
    పూర్ణాంకం		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (స్థిరరాశి wchar_t *)Get_Command_పంక్తి_W();

    /*
     * Skip the executable name, which might be in "".
     */
    అయితే (*p)
    {
	ఒకవేళ (*p == L'"')
	    inquote = !inquote;
	లేకపోతే ఒకవేళ (!inquote && *p == L' ')
	{
	    ++p;
	    ఆపు;
	}
	++p;
    }
    అయితే (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ఒకవేళ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	అయితే (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ఖాళీ);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ఖాళీ);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ఖాళీ;
    cmdlen = wcslen(p);
    ఒకవేళ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = జ్ఞాపకం_ఇవ్వు(cmdlen * పరిమాణం(wchar_t));
	ఒకవేళ (cmd == ఖాళీ)
	{
	    perror("vimrun malloc(): ");
	    తిరిగి -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ఒకవేళ (cmd)
	విడుదల(cmd);

    ఒకవేళ (retval == -1)
	perror("vimrun system(): ");
    లేకపోతే ఒకవేళ (retval != 0)
	రాయి("shell returned %d\n", retval);

    ఒకవేళ (!silent)
    {
	puts("Hit any key to close this window...");

	అయితే (_kbhit())
	    (శూన్య)_getch();
	(శూన్య)_getch();
    }

    తిరిగి retval;
}
