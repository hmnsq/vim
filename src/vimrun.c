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

    ကိန်းပြည့်
main(void)
{
    const wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    ကိန်းပြည့်		    retval;
    ကိန်းပြည့်		    inquote = 0;
    ကိန်းပြည့်		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (const wchar_t *)GetCommandLineW();

    /*
     * Skip the executable name, which might be in "".
     */
    နေစဉ် (*p)
    {
	အကယ်၍ (*p == L'"')
	    inquote = !inquote;
	မဟုတ်ပါက အကယ်၍ (!inquote && *p == L' ')
	{
	    ++p;
	    bha_nga;
	}
	++p;
    }
    နေစဉ် (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    အကယ်၍ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	နေစဉ် (*p == L' ')
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
    အကယ်၍ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * parimaa_na(wchar_t));
	အကယ်၍ (cmd == NULL)
	{
	    perror("vimrun malloc(): ");
	    ပြန်ပေး -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    အကယ်၍ (cmd)
	free(cmd);

    အကယ်၍ (retval == -1)
	perror("vimrun system(): ");
    မဟုတ်ပါက အကယ်၍ (retval != 0)
	printf("shell returned %d\n", retval);

    အကယ်၍ (!silent)
    {
	puts("Hit any key to close this window...");

	နေစဉ် (_kbhit())
	    (void)_getch();
	(void)_getch();
    }

    ပြန်ပေး retval;
}
