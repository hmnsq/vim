/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "termlib.pro"

#যদি !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

স্থির পূর্ণাঙ্ক  getent(ৱর্ণ *, ৱর্ণ *, FILE *, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক  nextent(ৱর্ণ *, FILE *, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক  _match(ৱর্ণ *, ৱর্ণ *);
স্থির ৱর্ণ *_addfmt(ৱর্ণ *, ৱর্ণ *, পূর্ণাঙ্ক);
স্থির ৱর্ণ *_find(ৱর্ণ *, ৱর্ণ *);

/*
 * Global variables for termlib
 */

ৱর্ণ	*tent;		      // Pointer to terminal entry, set by tgetent
ৱর্ণ	PC = 0;		      // Pad character, default NULL
ৱর্ণ	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# অন্যথা
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  অন্যথা
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    পূর্ণাঙ্ক
tgetent(
    ৱর্ণ    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    ৱর্ণ    *term)		// Name of terminal
{
    ৱর্ণ    tcbuf[32];		// Temp buffer to handle
    ৱর্ণ    *tcptr = tcbuf;	// extended entries
    ৱর্ণ    *tcap = TERMCAPFILE; // Default termcap file
    ৱর্ণ    *tmp;
    FILE    *termcap;
    পূর্ণাঙ্ক	    retval = 0;
    পূর্ণাঙ্ক	    len;

    যদি ((tmp = (ৱর্ণ *)mch_getenv((char_u *)"TERMCAP")) != রিক্ত)
    {
	যদি (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#যদি defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    যদি (tmp)
		*tmp = ':';
#endif
	}
	অন্যথা				// TERMCAP = termcap entry itself
	{
	    পূর্ণাঙ্ক tlen = strlen(term);

	    যেতিয়ালৈ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		ৱর্ণ *nexttmp;

		যেতিয়ালৈ (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		যদি (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ঘূৰাই 1;
		}
		অন্যথা
		    tmp = nexttmp;
	    }
	}
    }
    যদি (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ঘূৰাই -1;
    }

    len = 0;
    যেতিয়ালৈ (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	যদি ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	অন্যথা
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    ভঙ্গ;
	}
    }
    fclose(termcap);
    ঘূৰাই retval;
}

    স্থির পূর্ণাঙ্ক
getent(ৱর্ণ *tbuf, ৱর্ণ *term, FILE *termcap, পূর্ণাঙ্ক buflen)
{
    ৱর্ণ    *tptr;
    পূর্ণাঙ্ক	    tlen = strlen(term);

    যেতিয়ালৈ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	যেতিয়ালৈ (*tptr && *tptr != ':')		// : terminates name field
	{
	    ৱর্ণ    *nexttptr;

	    যেতিয়ালৈ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    যদি (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ঘূৰাই 1;
	    }
	    অন্যথা				// Look for next name
		tptr = nexttptr;
	}
    }
    ঘূৰাই 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    স্থির পূর্ণাঙ্ক
nextent(ৱর্ণ *tbuf, FILE *termcap, পূর্ণাঙ্ক buflen)
{
    ৱর্ণ *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    যেতিয়ালৈ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (পূর্ণাঙ্ক)(tbuf+buflen-lbuf), termcap)) // another line
    {
	পূর্ণাঙ্ক llen = strlen(lbuf);

	যদি (*lbuf == '#')			// eat comments
	    অনুৱর্ত;
	যদি (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	যদি (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	অন্যথা
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ঘূৰাই 1;
	}
    }

    ঘূৰাই 0;					// ran into end of file
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

    পূর্ণাঙ্ক
tgetflag(ৱর্ণ *id)
{
    ৱর্ণ    buf[256], *ptr = buf;

    ঘূৰাই tgetstr(id, &ptr) ? 1 : 0;
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

    পূর্ণাঙ্ক
tgetnum(ৱর্ণ *id)
{
    ৱর্ণ *ptr, buf[256];
    ptr = buf;

    যদি (tgetstr(id, &ptr))
	ঘূৰাই atoi(buf);
    অন্যথা
	ঘূৰাই 0;
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

    ৱর্ণ *
tgetstr(ৱর্ণ *id, ৱর্ণ **buf)
{
    পূর্ণাঙ্ক		len = strlen(id);
    ৱর্ণ	*tmp=tent;
    ৱর্ণ	*hold;
    পূর্ণাঙ্ক		i;

    কৰা
    {
	tmp = _find(tmp, ":");			// For each field
	যেতিয়ালৈ (*tmp == ':')			// skip empty fields
	    tmp++;
	যদি (!*tmp)
	    ভঙ্গ;

	যদি (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    যদি (*tmp == '@')			// :xx@: entry for tc
		ঘূৰাই 0;			// deleted entry
	    hold= *buf;
	    যেতিয়ালৈ (*++tmp && *tmp != ':')	// not at end of field
		{
		ৱিকল্প(*tmp)
		{
		পক্ষ '\\':			// Expand escapes here
		    ৱিকল্প(*++tmp)
		    {
		    পক্ষ 0:			// ignore backslashes
			tmp--;			// at end of entry
			ভঙ্গ;			// shouldn't happen
		    পক্ষ 'e':
		    পক্ষ 'E':			// ESC
			*(*buf)++ = ESC;
			ভঙ্গ;
		    পক্ষ 'n':			// \n
			*(*buf)++ = '\n';
			ভঙ্গ;
		    পক্ষ 'r':			// \r
			*(*buf)++ = '\r';
			ভঙ্গ;
		    পক্ষ 't':			// \t
			*(*buf)++ = '\t';
			ভঙ্গ;
		    পক্ষ 'b':			// \b
			*(*buf)++ = '\b';
			ভঙ্গ;
		    পক্ষ 'f':			// \f
			*(*buf)++ = '\f';
			ভঙ্গ;
		    পক্ষ '0':			// \nnn
		    পক্ষ '1':
		    পক্ষ '2':
		    পক্ষ '3':
		    পক্ষ '4':
		    পক্ষ '5':
		    পক্ষ '6':
		    পক্ষ '7':
		    পক্ষ '8':
		    পক্ষ '9':
			**buf = 0;
			    // get up to three digits
			বাবে (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			ভঙ্গ;
		    স্ৱভাৱ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    ভঙ্গ;
		পক্ষ '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    ভঙ্গ;
		স্ৱভাৱ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ঘূৰাই hold;
	}
    } যেতিয়ালৈ (*tmp);

    ঘূৰাই 0;
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

    ৱর্ণ *
tgoto(
    ৱর্ণ    *cm,				// cm string, from termcap
    পূর্ণাঙ্ক	    col,				// column, x position
    পূর্ণাঙ্ক	    সাৰি)				// line, y position
{
    ৱর্ণ    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    স্থির ৱর্ণ buffer[32];

    যদি (!cm)
	ঘূৰাই "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    যেতিয়ালৈ (*ptr)
    {
	যদি ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	অন্যথা
	{				// % escape
	    ৱিকল্প(c = *ptr++)
	    {
	    পক্ষ 'd':				// decimal
		bufp = _addfmt(bufp, "%d", সাৰি);
		সাৰি = col;
		ভঙ্গ;
	    পক্ষ '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", সাৰি);
		সাৰি = col;
		ভঙ্গ;
	    পক্ষ '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", সাৰি);
		সাৰি = col;
		ভঙ্গ;
	    পক্ষ '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		যদি (col>gx) col += gy;
		যদি (সাৰি>gx) সাৰি += gy;
		ভঙ্গ;
	    পক্ষ '+':				// %+c: add c
		সাৰি += *ptr++;
	    পক্ষ '.':				// print x/y
		যদি (সাৰি == '\t' ||		// these are
		   সাৰি == '\n' ||		// chars that
		   সাৰি == '\004' ||		// UNIX hates
		   সাৰি == '\0')
		{
		    সাৰি++;			// so go to next pos
		    যদি (reverse == (সাৰি == col))
			addup=1;		// and mark UP
		    অন্যথা
			addbak=1;		// or BC
		}
		*bufp++=সাৰি;
		সাৰি = col;
		ভঙ্গ;
	    পক্ষ 'r':				// r: reverse
		gx = সাৰি;
		সাৰি = col;
		col = gx;
		reverse = 1;
		ভঙ্গ;
	    পক্ষ 'i':			// increment (1-origin screen)
		col++;
		সাৰি++;
		ভঙ্গ;
	    পক্ষ '%':				// %%=% literally
		*bufp++='%';
		ভঙ্গ;
	    পক্ষ 'n':				// magic DM2500 code
		সাৰি ^= 0140;
		col ^= 0140;
		ভঙ্গ;
	    পক্ষ 'B':				// bcd encoding
		সাৰি = সাৰি/10<<4+সাৰি%10;
		col = col/10<<4+col%10;
		ভঙ্গ;
	    পক্ষ 'D':				// magic Delta Data code
		সাৰি = সাৰি-2*(সাৰি&15);
		col = col-2*(col&15);
		ভঙ্গ;
	    স্ৱভাৱ:				// Unknown escape
		ঘূৰাই "OOPS";
	    }
	}
    }

    যদি (addup)					// add upline
	যদি (UP)
	{
	    ptr=UP;
	    যেতিয়ালৈ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    যদি (*ptr == '*')
		ptr++;
	    যেতিয়ালৈ (*ptr)
		*bufp++ = *ptr++;
	}

    যদি (addbak)					// add backspace
	যদি (BC)
	{
	    ptr=BC;
	    যেতিয়ালৈ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    যদি (*ptr == '*')
		ptr++;
	    যেতিয়ালৈ (*ptr)
		*bufp++ = *ptr++;
	}
	অন্যথা
	    *bufp++='\b';

    *bufp = 0;

    ঘূৰাই(buffer);
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

    পূর্ণাঙ্ক
tputs(
    ৱর্ণ *cp,				// string to print
    পূর্ণাঙ্ক affcnt,				// Number of lines affected
    শূন্য (*outc)(unsigned পূর্ণাঙ্ক))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ধ্রুৱ ৱর্ণ *);

    যদি (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	যেতিয়ালৈ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	যদি (*cp == '.')
	    যেতিয়ালৈ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	যদি (*cp!='*')			// multiply by affected lines
	{
	    যদি (affcnt>1) affcnt = 1;
	}
	অন্যথা
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	যদি (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	যেতিয়ালৈ (*cp)			// output string
	    (*outc)(*cp++);
	যদি (ospeed)
	    যেতিয়ালৈ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    অন্যথা
	যেতিয়ালৈ (*cp)
	    (*outc)(*cp++);
    ঘূৰাই 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    স্থির পূর্ণাঙ্ক
_match(ৱর্ণ *s1, ৱর্ণ *s2)
{
    পূর্ণাঙ্ক i = 0;

    যেতিয়ালৈ (s1[i] && s1[i] == s2[i])
	i++;

    ঘূৰাই i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    স্থির ৱর্ণ *
_find(ৱর্ণ *s, ৱর্ণ *set)
{
    বাবে (; *s; s++)
    {
	ৱর্ণ	*ptr = set;

	যেতিয়ালৈ (*ptr && *s != *ptr)
	    ptr++;

	যদি (*ptr)
	    ঘূৰাই s;
    }

    ঘূৰাই s;
}

/*
 * add val to buf according to format fmt
 */
    স্থির ৱর্ণ *
_addfmt(ৱর্ণ *buf, ৱর্ণ *fmt, পূর্ণাঙ্ক val)
{
    sprintf(buf, fmt, val);
    যেতিয়ালৈ (*buf)
	buf++;
    ঘূৰাই buf;
}
