/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "termlib.pro"

#എങ്കിൽ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ  getent(അക്ഷരം *, അക്ഷരം *, FILE *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ  nextent(അക്ഷരം *, FILE *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ  _match(അക്ഷരം *, അക്ഷരം *);
സ്ഥിതം അക്ഷരം *_addfmt(അക്ഷരം *, അക്ഷരം *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം അക്ഷരം *_find(അക്ഷരം *, അക്ഷരം *);

/*
 * Global variables for termlib
 */

അക്ഷരം	*tent;		      // Pointer to terminal entry, set by tgetent
അക്ഷരം	PC = 0;		      // Pad character, default NULL
അക്ഷരം	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
short	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

/*
 * Module: tgetent
 *
 * Purpose: Get termcap entry for <term> into buffer at <tbuf>.
 *
 * Calling conventions: char tbuf[TBUFSZ+], term=canonical name for terminal.
 *
 * Returned values: 1 = success, -1 = can't open file,
 *	    0 = can't find terminal.
 *
 * Notes:
 * - Should probably supply static buffer.
 * - Uses environment variables "TERM" and "TERMCAP". If TERM = term (that is,
 *   if the argument matches the environment) then it looks at TERMCAP.
 * - If TERMCAP begins with a slash, then it assumes this is the file to
 *   search rather than /etc/termcap.
 * - If TERMCAP does not begin with a slash, and it matches TERM, then this is
 *   used as the entry.
 * - This could be simplified considerably for non-UNIX systems.
 */

#ifndef TERMCAPFILE
# ifdef AMIGA
#  define TERMCAPFILE "s:termcap"
# അല്ലെങ്കിൽ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  അല്ലെങ്കിൽ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    പൂർണ്ണസംഖ്യ
tgetent(
    അക്ഷരം    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    അക്ഷരം    *term)		// Name of terminal
{
    അക്ഷരം    tcbuf[32];		// Temp buffer to handle
    അക്ഷരം    *tcptr = tcbuf;	// extended entries
    അക്ഷരം    *tcap = TERMCAPFILE; // Default termcap file
    അക്ഷരം    *tmp;
    FILE    *termcap;
    പൂർണ്ണസംഖ്യ	    retval = 0;
    പൂർണ്ണസംഖ്യ	    len;

    എങ്കിൽ ((tmp = (അക്ഷരം *)mch_getenv((char_u *)"TERMCAP")) != ശൂന്യ)
    {
	എങ്കിൽ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#എങ്കിൽ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    എങ്കിൽ (tmp)
		*tmp = ':';
#endif
	}
	അല്ലെങ്കിൽ				// TERMCAP = termcap entry itself
	{
	    പൂർണ്ണസംഖ്യ tlen = strlen(term);

	    ആകുമ്പോൾ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		അക്ഷരം *nexttmp;

		ആകുമ്പോൾ (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		എങ്കിൽ (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    തിരികെ 1;
		}
		അല്ലെങ്കിൽ
		    tmp = nexttmp;
	    }
	}
    }
    എങ്കിൽ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	തിരികെ -1;
    }

    len = 0;
    ആകുമ്പോൾ (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	എങ്കിൽ ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	അല്ലെങ്കിൽ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    നിർത്തുക;
	}
    }
    fclose(termcap);
    തിരികെ retval;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
getent(അക്ഷരം *tbuf, അക്ഷരം *term, FILE *termcap, പൂർണ്ണസംഖ്യ buflen)
{
    അക്ഷരം    *tptr;
    പൂർണ്ണസംഖ്യ	    tlen = strlen(term);

    ആകുമ്പോൾ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ആകുമ്പോൾ (*tptr && *tptr != ':')		// : terminates name field
	{
	    അക്ഷരം    *nexttptr;

	    ആകുമ്പോൾ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    എങ്കിൽ (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		തിരികെ 1;
	    }
	    അല്ലെങ്കിൽ				// Look for next name
		tptr = nexttptr;
	}
    }
    തിരികെ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nextent(അക്ഷരം *tbuf, FILE *termcap, പൂർണ്ണസംഖ്യ buflen)
{
    അക്ഷരം *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ആകുമ്പോൾ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (പൂർണ്ണസംഖ്യ)(tbuf+buflen-lbuf), termcap)) // another line
    {
	പൂർണ്ണസംഖ്യ llen = strlen(lbuf);

	എങ്കിൽ (*lbuf == '#')			// eat comments
	    തുടരുക;
	എങ്കിൽ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	എങ്കിൽ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	അല്ലെങ്കിൽ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    തിരികെ 1;
	}
    }

    തിരികെ 0;					// ran into end of file
}

/*
 * Module: tgetflag
 *
 * Purpose: returns flag true or false as to the existence of a given entry.
 * used with 'bs', 'am', etc...
 *
 * Calling conventions: id is the 2 character capability id.
 *
 * Returned values: 1 for success, 0 for failure.
 */

    പൂർണ്ണസംഖ്യ
tgetflag(അക്ഷരം *id)
{
    അക്ഷരം    buf[256], *ptr = buf;

    തിരികെ tgetstr(id, &ptr) ? 1 : 0;
}

/*
 * Module: tgetnum
 *
 * Purpose: get numeric value such as 'li' or 'co' from termcap.
 *
 * Calling conventions: id = 2 character id.
 *
 * Returned values: -1 for failure, else numerical value.
 */

    പൂർണ്ണസംഖ്യ
tgetnum(അക്ഷരം *id)
{
    അക്ഷരം *ptr, buf[256];
    ptr = buf;

    എങ്കിൽ (tgetstr(id, &ptr))
	തിരികെ atoi(buf);
    അല്ലെങ്കിൽ
	തിരികെ 0;
}

/*
 * Module: tgetstr
 *
 * Purpose: get terminal capability string from database.
 *
 * Calling conventions: id is the two character capability id.
 *	    (*buf) points into a hold buffer for the
 *	    id. the capability is copied into the buffer
 *	    and (*buf) is advanced to point to the next
 *	    free byte in the buffer.
 *
 * Returned values: 0 = no such entry, otherwise returns original
 *	    (*buf) (now a pointer to the string).
 *
 * Notes
 *	It also decodes certain escape sequences in the buffer.
 *  they should be obvious from the code:
 *	\E = escape.
 *	\n, \r, \t, \f, \b match the 'c' escapes.
 *	^x matches control-x (^@...^_).
 *	\nnn matches nnn octal.
 *	\x, where x is anything else, matches x. I differ
 *  from the standard library here, in that I allow ^: to match
 *  :.
 *
 */

    അക്ഷരം *
tgetstr(അക്ഷരം *id, അക്ഷരം **buf)
{
    പൂർണ്ണസംഖ്യ		len = strlen(id);
    അക്ഷരം	*tmp=tent;
    അക്ഷരം	*hold;
    പൂർണ്ണസംഖ്യ		i;

    ചെയ്യുക
    {
	tmp = _find(tmp, ":");			// For each field
	ആകുമ്പോൾ (*tmp == ':')			// skip empty fields
	    tmp++;
	എങ്കിൽ (!*tmp)
	    നിർത്തുക;

	എങ്കിൽ (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    എങ്കിൽ (*tmp == '@')			// :xx@: entry for tc
		തിരികെ 0;			// deleted entry
	    hold= *buf;
	    ആകുമ്പോൾ (*++tmp && *tmp != ':')	// not at end of field
		{
		തിരഞ്ഞെടുപ്പ്(*tmp)
		{
		അവസ്ഥ '\\':			// Expand escapes here
		    തിരഞ്ഞെടുപ്പ്(*++tmp)
		    {
		    അവസ്ഥ 0:			// ignore backslashes
			tmp--;			// at end of entry
			നിർത്തുക;			// shouldn't happen
		    അവസ്ഥ 'e':
		    അവസ്ഥ 'E':			// ESC
			*(*buf)++ = ESC;
			നിർത്തുക;
		    അവസ്ഥ 'n':			// \n
			*(*buf)++ = '\n';
			നിർത്തുക;
		    അവസ്ഥ 'r':			// \r
			*(*buf)++ = '\r';
			നിർത്തുക;
		    അവസ്ഥ 't':			// \t
			*(*buf)++ = '\t';
			നിർത്തുക;
		    അവസ്ഥ 'b':			// \b
			*(*buf)++ = '\b';
			നിർത്തുക;
		    അവസ്ഥ 'f':			// \f
			*(*buf)++ = '\f';
			നിർത്തുക;
		    അവസ്ഥ '0':			// \nnn
		    അവസ്ഥ '1':
		    അവസ്ഥ '2':
		    അവസ്ഥ '3':
		    അവസ്ഥ '4':
		    അവസ്ഥ '5':
		    അവസ്ഥ '6':
		    അവസ്ഥ '7':
		    അവസ്ഥ '8':
		    അവസ്ഥ '9':
			**buf = 0;
			    // get up to three digits
			വേണ്ടി (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    നിർത്തുക;
		അവസ്ഥ '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    തിരികെ hold;
	}
    } ആകുമ്പോൾ (*tmp);

    തിരികെ 0;
}

/*
 * Module: tgoto
 *
 * Purpose: decode cm cursor motion string.
 *
 * Calling conventions: cm is cursor motion string.  line, col, are the
 * desired destination.
 *
 * Returned values: a string pointing to the decoded string, or "OOPS" if it
 * cannot be decoded.
 *
 * Notes
 *	The accepted escapes are:
 *	%d	 as in printf, 0 origin.
 *	%2, %3   like %02d, %03d in printf.
 *	%.	 like %c
 *	%+x	 adds <x> to value, then %.
 *	%>xy     if value>x, adds y. No output.
 *	%i	 increments line& col, no output.
 *	%r	 reverses order of line&col. No output.
 *	%%	 prints as a single %.
 *	%n	 exclusive or row & col with 0140.
 *	%B	 BCD, no output.
 *	%D	 reverse coding (x-2*(x%16)), no output.
 */

    അക്ഷരം *
tgoto(
    അക്ഷരം    *cm,				// cm string, from termcap
    പൂർണ്ണസംഖ്യ	    col,				// column, x position
    പൂർണ്ണസംഖ്യ	    വരി)				// line, y position
{
    അക്ഷരം    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    സ്ഥിതം അക്ഷരം buffer[32];

    എങ്കിൽ (!cm)
	തിരികെ "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    ആകുമ്പോൾ (*ptr)
    {
	എങ്കിൽ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	അല്ലെങ്കിൽ
	{				// % escape
	    തിരഞ്ഞെടുപ്പ്(c = *ptr++)
	    {
	    അവസ്ഥ 'd':				// decimal
		bufp = _addfmt(bufp, "%d", വരി);
		വരി = col;
		നിർത്തുക;
	    അവസ്ഥ '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", വരി);
		വരി = col;
		നിർത്തുക;
	    അവസ്ഥ '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", വരി);
		വരി = col;
		നിർത്തുക;
	    അവസ്ഥ '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		എങ്കിൽ (col>gx) col += gy;
		എങ്കിൽ (വരി>gx) വരി += gy;
		നിർത്തുക;
	    അവസ്ഥ '+':				// %+c: add c
		വരി += *ptr++;
	    അവസ്ഥ '.':				// print x/y
		എങ്കിൽ (വരി == '\t' ||		// these are
		   വരി == '\n' ||		// chars that
		   വരി == '\004' ||		// UNIX hates
		   വരി == '\0')
		{
		    വരി++;			// so go to next pos
		    എങ്കിൽ (reverse == (വരി == col))
			addup=1;		// and mark UP
		    അല്ലെങ്കിൽ
			addbak=1;		// or BC
		}
		*bufp++=വരി;
		വരി = col;
		നിർത്തുക;
	    അവസ്ഥ 'r':				// r: reverse
		gx = വരി;
		വരി = col;
		col = gx;
		reverse = 1;
		നിർത്തുക;
	    അവസ്ഥ 'i':			// increment (1-origin screen)
		col++;
		വരി++;
		നിർത്തുക;
	    അവസ്ഥ '%':				// %%=% literally
		*bufp++='%';
		നിർത്തുക;
	    അവസ്ഥ 'n':				// magic DM2500 code
		വരി ^= 0140;
		col ^= 0140;
		നിർത്തുക;
	    അവസ്ഥ 'B':				// bcd encoding
		വരി = വരി/10<<4+വരി%10;
		col = col/10<<4+col%10;
		നിർത്തുക;
	    അവസ്ഥ 'D':				// magic Delta Data code
		വരി = വരി-2*(വരി&15);
		col = col-2*(col&15);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:				// Unknown escape
		തിരികെ "OOPS";
	    }
	}
    }

    എങ്കിൽ (addup)					// add upline
	എങ്കിൽ (UP)
	{
	    ptr=UP;
	    ആകുമ്പോൾ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    എങ്കിൽ (*ptr == '*')
		ptr++;
	    ആകുമ്പോൾ (*ptr)
		*bufp++ = *ptr++;
	}

    എങ്കിൽ (addbak)					// add backspace
	എങ്കിൽ (BC)
	{
	    ptr=BC;
	    ആകുമ്പോൾ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    എങ്കിൽ (*ptr == '*')
		ptr++;
	    ആകുമ്പോൾ (*ptr)
		*bufp++ = *ptr++;
	}
	അല്ലെങ്കിൽ
	    *bufp++='\b';

    *bufp = 0;

    തിരികെ(buffer);
}

/*
 * Module: tputs
 *
 * Purpose: decode padding information
 *
 * Calling conventions: cp = string to be padded, affcnt = # of items affected
 *	(lines, characters, whatever), outc = routine to output 1 character.
 *
 * Returned values: none
 *
 * Notes
 *	cp has padding information ahead of it, in the form
 *  nnnTEXT or nnn*TEXT. nnn is the number of milliseconds to delay,
 *  and may be a decimal (nnn.mmm). If the asterisk is given, then
 *  the delay is multiplied by afcnt. The delay is produced by outputting
 *  a number of nulls (or other padding char) after printing the
 *  TEXT.
 *
 */

long _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    പൂർണ്ണസംഖ്യ
tputs(
    അക്ഷരം *cp,				// string to print
    പൂർണ്ണസംഖ്യ affcnt,				// Number of lines affected
    ശൂന്യം (*outc)(unsigned പൂർണ്ണസംഖ്യ))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(സ്ഥിരം അക്ഷരം *);

    എങ്കിൽ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ആകുമ്പോൾ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	എങ്കിൽ (*cp == '.')
	    ആകുമ്പോൾ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	എങ്കിൽ (*cp!='*')			// multiply by affected lines
	{
	    എങ്കിൽ (affcnt>1) affcnt = 1;
	}
	അല്ലെങ്കിൽ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	എങ്കിൽ (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	ആകുമ്പോൾ (*cp)			// output string
	    (*outc)(*cp++);
	എങ്കിൽ (ospeed)
	    ആകുമ്പോൾ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    അല്ലെങ്കിൽ
	ആകുമ്പോൾ (*cp)
	    (*outc)(*cp++);
    തിരികെ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
_match(അക്ഷരം *s1, അക്ഷരം *s2)
{
    പൂർണ്ണസംഖ്യ i = 0;

    ആകുമ്പോൾ (s1[i] && s1[i] == s2[i])
	i++;

    തിരികെ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    സ്ഥിതം അക്ഷരം *
_find(അക്ഷരം *s, അക്ഷരം *set)
{
    വേണ്ടി (; *s; s++)
    {
	അക്ഷരം	*ptr = set;

	ആകുമ്പോൾ (*ptr && *s != *ptr)
	    ptr++;

	എങ്കിൽ (*ptr)
	    തിരികെ s;
    }

    തിരികെ s;
}

/*
 * add val to buf according to format fmt
 */
    സ്ഥിതം അക്ഷരം *
_addfmt(അക്ഷരം *buf, അക്ഷരം *fmt, പൂർണ്ണസംഖ്യ val)
{
    sprintf(buf, fmt, val);
    ആകുമ്പോൾ (*buf)
	buf++;
    തിരികെ buf;
}
