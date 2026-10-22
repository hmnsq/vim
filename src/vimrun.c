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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#அந்தர்பாவ <windows.h>

    முழு
முதன்மை(வெறுமை)
{
    மாறிலி wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    முழு		    retval;
    முழு		    inquote = 0;
    முழு		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (மாறிலி wchar_t *)Get_Command_வரி_W();

    /*
     * Skip the executable name, which might be in "".
     */
    போது (*p)
    {
	எனில் (*p == L'"')
	    inquote = !inquote;
	இல்லையெனில் எனில் (!inquote && *p == L' ')
	{
	    ++p;
	    நிறுத்து;
	}
	++p;
    }
    போது (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    எனில் (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	போது (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, வெற்று);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, வெற்று);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = வெற்று;
    cmdlen = wcslen(p);
    எனில் (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = malloc(cmdlen * அளவு(wchar_t));
	எனில் (cmd == வெற்று)
	{
	    perror("vimrun malloc(): ");
	    திரும்பு -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    எனில் (cmd)
	விடுவி(cmd);

    எனில் (retval == -1)
	perror("vimrun system(): ");
    இல்லையெனில் எனில் (retval != 0)
	printf("shell returned %d\n", retval);

    எனில் (!silent)
    {
	puts("Hit any key to close this window...");

	போது (_kbhit())
	    (வெறுமை)_getch();
	(வெறுமை)_getch();
    }

    திரும்பு retval;
}
