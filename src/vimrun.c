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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#ಅನ್ತರ್ಭಾವ <windows.h>

    ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
    ಧ್ರುವ wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    ಪೂರ್ಣಾಙ್ಕ		    retval;
    ಪೂರ್ಣಾಙ್ಕ		    inquote = 0;
    ಪೂರ್ಣಾಙ್ಕ		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ಧ್ರುವ wchar_t *)Get_Command_ಸಾಲು_W();

    /*
     * Skip the executable name, which might be in "".
     */
    ಯಾವತ್ (*p)
    {
	ಯದಿ (*p == L'"')
	    inquote = !inquote;
	ಅನ್ಯಥಾ ಯದಿ (!inquote && *p == L' ')
	{
	    ++p;
	    ಭಙ್ಗ;
	}
	++p;
    }
    ಯಾವತ್ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ಯದಿ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ಯಾವತ್ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ರಿಕ್ತ);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ರಿಕ್ತ);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ರಿಕ್ತ;
    cmdlen = wcslen(p);
    ಯದಿ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * ಪರಿಮಾಣ(wchar_t));
	ಯದಿ (cmd == ರಿಕ್ತ)
	{
	    perror("vimrun malloc(): ");
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ಯದಿ (cmd)
	ಮುಕ್ತ(cmd);

    ಯದಿ (retval == -1)
	perror("vimrun system(): ");
    ಅನ್ಯಥಾ ಯದಿ (retval != 0)
	printf("shell returned %d\n", retval);

    ಯದಿ (!silent)
    {
	puts("Hit any key to close this window...");

	ಯಾವತ್ (_kbhit())
	    (ಶೂನ್ಯ)_getch();
	(ಶೂನ್ಯ)_getch();
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}
