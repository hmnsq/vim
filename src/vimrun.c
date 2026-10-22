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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#ଅନ୍ତର୍ଭାଵ <windows.h>

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ଶୂନ୍ୟ)
{
    ଅଚଳ wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    retval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    inquote = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ଅଚଳ wchar_t *)Get_Command_ଧାଡ଼ି_W();

    /*
     * Skip the executable name, which might be in "".
     */
    ଯେତେବେଳେ (*p)
    {
	ଯଦି (*p == L'"')
	    inquote = !inquote;
	ଅନ୍ୟଥା ଯଦି (!inquote && *p == L' ')
	{
	    ++p;
	    ଭାଙ୍ଗ;
	}
	++p;
    }
    ଯେତେବେଳେ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ଯଦି (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ଯେତେବେଳେ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ଖାଲି);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ଖାଲି);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ଖାଲି;
    cmdlen = wcslen(p);
    ଯଦି (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * ଆକାର(wchar_t));
	ଯଦି (cmd == ଖାଲି)
	{
	    perror("vimrun malloc(): ");
	    ଫେରନ୍ତୁ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ଯଦି (cmd)
	ମୁକ୍ତ(cmd);

    ଯଦି (retval == -1)
	perror("vimrun system(): ");
    ଅନ୍ୟଥା ଯଦି (retval != 0)
	printf("shell returned %d\n", retval);

    ଯଦି (!silent)
    {
	puts("Hit any key to close this window...");

	ଯେତେବେଳେ (_kbhit())
	    (ଶୂନ୍ୟ)_getch();
	(ଶୂନ୍ୟ)_getch();
    }

    ଫେରନ୍ତୁ retval;
}
