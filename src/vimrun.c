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

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#അന്തര്ഭാവ <windows.h>

    പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
    സ്ഥിരം wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    പൂർണ്ണസംഖ്യ		    retval;
    പൂർണ്ണസംഖ്യ		    inquote = 0;
    പൂർണ്ണസംഖ്യ		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (സ്ഥിരം wchar_t *)Get_Command_വരി_W();

    /*
     * Skip the executable name, which might be in "".
     */
    ആകുമ്പോൾ (*p)
    {
	എങ്കിൽ (*p == L'"')
	    inquote = !inquote;
	അല്ലെങ്കിൽ എങ്കിൽ (!inquote && *p == L' ')
	{
	    ++p;
	    നിർത്തുക;
	}
	++p;
    }
    ആകുമ്പോൾ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    എങ്കിൽ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ആകുമ്പോൾ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ശൂന്യ);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ശൂന്യ);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ശൂന്യ;
    cmdlen = wcslen(p);
    എങ്കിൽ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * വലുപ്പം(wchar_t));
	എങ്കിൽ (cmd == ശൂന്യ)
	{
	    perror("vimrun malloc(): ");
	    തിരികെ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    എങ്കിൽ (cmd)
	സ്വതന്ത്രം(cmd);

    എങ്കിൽ (retval == -1)
	perror("vimrun system(): ");
    അല്ലെങ്കിൽ എങ്കിൽ (retval != 0)
	printf("shell returned %d\n", retval);

    എങ്കിൽ (!silent)
    {
	puts("Hit any key to close this window...");

	ആകുമ്പോൾ (_kbhit())
	    (ശൂന്യം)_getch();
	(ശൂന്യം)_getch();
    }

    തിരികെ retval;
}
