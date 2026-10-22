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

    puur_naa_nka
main(void)
{
    const wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    puur_naa_nka		    retval;
    puur_naa_nka		    inquote = 0;
    puur_naa_nka		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (const wchar_t *)GetCommandLineW();

    /*
     * Skip the executable name, which might be in "".
     */
    ᮞᮨᮓᮨᮜ (*p)
    {
	ᮚᮦᮔ᮪ (*p == L'"')
	    inquote = !inquote;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (!inquote && *p == L' ')
	{
	    ++p;
	    bha_nga;
	}
	++p;
    }
    ᮞᮨᮓᮨᮜ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ᮚᮦᮔ᮪ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ᮞᮨᮓᮨᮜ (*p == L' ')
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
    ᮚᮦᮔ᮪ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * parimaa_na(wchar_t));
	ᮚᮦᮔ᮪ (cmd == NULL)
	{
	    perror("vimrun malloc(): ");
	    ᮝᮜᮤ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ᮚᮦᮔ᮪ (cmd)
	free(cmd);

    ᮚᮦᮔ᮪ (retval == -1)
	perror("vimrun system(): ");
    ᮜᮤᮚ ᮚᮦᮔ᮪ (retval != 0)
	printf("shell returned %d\n", retval);

    ᮚᮦᮔ᮪ (!silent)
    {
	puts("Hit any key to close this window...");

	ᮞᮨᮓᮨᮜ (_kbhit())
	    (void)_getch();
	(void)_getch();
    }

    ᮝᮜᮤ retval;
}
