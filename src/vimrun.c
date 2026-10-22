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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <conio.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32_LEAN_AND_ਔਸਤ
# define WIN32_LEAN_AND_ਔਸਤ
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>

    ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
    ਅਚਲ wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    ਪੂਰਨ		    retval;
    ਪੂਰਨ		    inquote = 0;
    ਪੂਰਨ		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ਅਚਲ wchar_t *)Get_Command_ਸਤਰ_W();

    /*
     * Skip the executable name, which might be in "".
     */
    ਜਦੋਂ (*p)
    {
	ਜੇ (*p == L'"')
	    inquote = !inquote;
	ਨਹੀਂਤਾਂ ਜੇ (!inquote && *p == L' ')
	{
	    ++p;
	    ਰੋਕੋ;
	}
	++p;
    }
    ਜਦੋਂ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ਜੇ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ਜਦੋਂ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ਨਿਰਾ);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ਨਿਰਾ);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ਨਿਰਾ;
    cmdlen = wcslen(p);
    ਜੇ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = ਸਮ੍ਰਿਤੀ_ਦਿਓ(cmdlen * ਆਕਾਰ(wchar_t));
	ਜੇ (cmd == ਨਿਰਾ)
	{
	    perror("vimrun malloc(): ");
	    ਵਾਪਸ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ਜੇ (cmd)
	ਮੁਕਤ(cmd);

    ਜੇ (retval == -1)
	perror("vimrun system(): ");
    ਨਹੀਂਤਾਂ ਜੇ (retval != 0)
	ਲਿਖੋ("shell returned %d\n", retval);

    ਜੇ (!silent)
    {
	puts("Hit any key to close this window...");

	ਜਦੋਂ (_kbhit())
	    (ਖਾਲੀ)_getch();
	(ਖਾਲੀ)_getch();
    }

    ਵਾਪਸ retval;
}
