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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#અન્તર્ભાવ <windows.h>

    પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
    અચલ wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    પૂર્ણાંક		    retval;
    પૂર્ણાંક		    inquote = 0;
    પૂર્ણાંક		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (અચલ wchar_t *)Get_Command_લીટી_W();

    /*
     * Skip the executable name, which might be in "".
     */
    જ્યારે (*p)
    {
	જો (*p == L'"')
	    inquote = !inquote;
	નહીંતર જો (!inquote && *p == L' ')
	{
	    ++p;
	    તોડો;
	}
	++p;
    }
    જ્યારે (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    જો (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	જ્યારે (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ખાલી);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ખાલી);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ખાલી;
    cmdlen = wcslen(p);
    જો (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * કદ(wchar_t));
	જો (cmd == ખાલી)
	{
	    perror("vimrun malloc(): ");
	    પાછા -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    જો (cmd)
	મુક્ત(cmd);

    જો (retval == -1)
	perror("vimrun system(): ");
    નહીંતર જો (retval != 0)
	printf("shell returned %d\n", retval);

    જો (!silent)
    {
	puts("Hit any key to close this window...");

	જ્યારે (_kbhit())
	    (શૂન્ય)_getch();
	(શૂન્ય)_getch();
    }

    પાછા retval;
}
