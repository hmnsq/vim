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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <conio.h>
#اگر_غیر_تعریف_شدہ WIN32_LEAN_AND_اوسط
# define WIN32_LEAN_AND_اوسط
#اگر_ختم
#شامل <windows.h>

    صحیح
مین(خالی)
{
    ثابت wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    صحیح		    retval;
    صحیح		    inquote = 0;
    صحیح		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ثابت wchar_t *)Get_Command_سطر_W();

    /*
     * Skip the executable name, which might be in "".
     */
    جب (*p)
    {
	اگر (*p == L'"')
	    inquote = !inquote;
	ورنہ اگر (!inquote && *p == L' ')
	{
	    ++p;
	    توڑو;
	}
	++p;
    }
    جب (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    اگر (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	جب (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, تہی);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, تہی);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = تہی;
    cmdlen = wcslen(p);
    اگر (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = حافظہ_دو(cmdlen * حجم(wchar_t));
	اگر (cmd == تہی)
	{
	    perror("vimrun malloc(): ");
	    واپس -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    اگر (cmd)
	آزاد(cmd);

    اگر (retval == -1)
	perror("vimrun system(): ");
    ورنہ اگر (retval != 0)
	لکھو("shell returned %d\n", retval);

    اگر (!silent)
    {
	puts("Hit any key to close this window...");

	جب (_kbhit())
	    (خالی)_getch();
	(خالی)_getch();
    }

    واپس retval;
}
