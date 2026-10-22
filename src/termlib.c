/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "termlib.pro"

#যদি !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

স্থির পূর্ণসংখ্যা  getent(অক্ষর *, অক্ষর *, FILE *, পূর্ণসংখ্যা);
স্থির পূর্ণসংখ্যা  nextent(অক্ষর *, FILE *, পূর্ণসংখ্যা);
স্থির পূর্ণসংখ্যা  _match(অক্ষর *, অক্ষর *);
স্থির অক্ষর *_addfmt(অক্ষর *, অক্ষর *, পূর্ণসংখ্যা);
স্থির অক্ষর *_find(অক্ষর *, অক্ষর *);

/*
 * Global variables for termlib
 */

অক্ষর	*tent;		      // Pointer to terminal entry, set by tgetent
অক্ষর	PC = 0;		      // Pad character, default NULL
অক্ষর	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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

    পূর্ণসংখ্যা
tgetent(
    অক্ষর    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    অক্ষর    *term)		// Name of terminal
{
    অক্ষর    tcbuf[32];		// Temp buffer to handle
    অক্ষর    *tcptr = tcbuf;	// extended entries
    অক্ষর    *tcap = TERMCAPFILE; // Default termcap file
    অক্ষর    *tmp;
    FILE    *termcap;
    পূর্ণসংখ্যা	    retval = 0;
    পূর্ণসংখ্যা	    len;

    যদি ((tmp = (অক্ষর *)mch_getenv((char_u *)"TERMCAP")) != ফাঁকা)
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
	    পূর্ণসংখ্যা tlen = strlen(term);

	    যখন (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		অক্ষর *nexttmp;

		যখন (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		যদি (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ফেরত 1;
		}
		অন্যথা
		    tmp = nexttmp;
	    }
	}
    }
    যদি (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ফেরত -1;
    }

    len = 0;
    যখন (getent(tbuf + len, term, termcap, TBUFSZ - len))
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
	    বিরতি;
	}
    }
    fclose(termcap);
    ফেরত retval;
}

    স্থির পূর্ণসংখ্যা
getent(অক্ষর *tbuf, অক্ষর *term, FILE *termcap, পূর্ণসংখ্যা buflen)
{
    অক্ষর    *tptr;
    পূর্ণসংখ্যা	    tlen = strlen(term);

    যখন (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	যখন (*tptr && *tptr != ':')		// : terminates name field
	{
	    অক্ষর    *nexttptr;

	    যখন (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    যদি (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ফেরত 1;
	    }
	    অন্যথা				// Look for next name
		tptr = nexttptr;
	}
    }
    ফেরত 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    স্থির পূর্ণসংখ্যা
nextent(অক্ষর *tbuf, FILE *termcap, পূর্ণসংখ্যা buflen)
{
    অক্ষর *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    যখন (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (পূর্ণসংখ্যা)(tbuf+buflen-lbuf), termcap)) // another line
    {
	পূর্ণসংখ্যা llen = strlen(lbuf);

	যদি (*lbuf == '#')			// eat comments
	    চালু;
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
	    ফেরত 1;
	}
    }

    ফেরত 0;					// ran into end of file
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

    পূর্ণসংখ্যা
tgetflag(অক্ষর *id)
{
    অক্ষর    buf[256], *ptr = buf;

    ফেরত tgetstr(id, &ptr) ? 1 : 0;
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

    পূর্ণসংখ্যা
tgetnum(অক্ষর *id)
{
    অক্ষর *ptr, buf[256];
    ptr = buf;

    যদি (tgetstr(id, &ptr))
	ফেরত atoi(buf);
    অন্যথা
	ফেরত 0;
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

    অক্ষর *
tgetstr(অক্ষর *id, অক্ষর **buf)
{
    পূর্ণসংখ্যা		len = strlen(id);
    অক্ষর	*tmp=tent;
    অক্ষর	*hold;
    পূর্ণসংখ্যা		i;

    করো
    {
	tmp = _find(tmp, ":");			// For each field
	যখন (*tmp == ':')			// skip empty fields
	    tmp++;
	যদি (!*tmp)
	    বিরতি;

	যদি (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    যদি (*tmp == '@')			// :xx@: entry for tc
		ফেরত 0;			// deleted entry
	    hold= *buf;
	    যখন (*++tmp && *tmp != ':')	// not at end of field
		{
		নির্বাচন(*tmp)
		{
		ক্ষেত্র '\\':			// Expand escapes here
		    নির্বাচন(*++tmp)
		    {
		    ক্ষেত্র 0:			// ignore backslashes
			tmp--;			// at end of entry
			বিরতি;			// shouldn't happen
		    ক্ষেত্র 'e':
		    ক্ষেত্র 'E':			// ESC
			*(*buf)++ = ESC;
			বিরতি;
		    ক্ষেত্র 'n':			// \n
			*(*buf)++ = '\n';
			বিরতি;
		    ক্ষেত্র 'r':			// \r
			*(*buf)++ = '\r';
			বিরতি;
		    ক্ষেত্র 't':			// \t
			*(*buf)++ = '\t';
			বিরতি;
		    ক্ষেত্র 'b':			// \b
			*(*buf)++ = '\b';
			বিরতি;
		    ক্ষেত্র 'f':			// \f
			*(*buf)++ = '\f';
			বিরতি;
		    ক্ষেত্র '0':			// \nnn
		    ক্ষেত্র '1':
		    ক্ষেত্র '2':
		    ক্ষেত্র '3':
		    ক্ষেত্র '4':
		    ক্ষেত্র '5':
		    ক্ষেত্র '6':
		    ক্ষেত্র '7':
		    ক্ষেত্র '8':
		    ক্ষেত্র '9':
			**buf = 0;
			    // get up to three digits
			জন্য (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			বিরতি;
		    পূর্বনির্ধারিত:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    বিরতি;
		ক্ষেত্র '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    বিরতি;
		পূর্বনির্ধারিত:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ফেরত hold;
	}
    } যখন (*tmp);

    ফেরত 0;
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

    অক্ষর *
tgoto(
    অক্ষর    *cm,				// cm string, from termcap
    পূর্ণসংখ্যা	    col,				// column, x position
    পূর্ণসংখ্যা	    সারি)				// line, y position
{
    অক্ষর    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    স্থির অক্ষর বাফার[32];

    যদি (!cm)
	ফেরত "OOPS";				// Kludge, but standard

    bufp = বাফার;
    ptr = cm;

    যখন (*ptr)
    {
	যদি ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	অন্যথা
	{				// % escape
	    নির্বাচন(c = *ptr++)
	    {
	    ক্ষেত্র 'd':				// decimal
		bufp = _addfmt(bufp, "%d", সারি);
		সারি = col;
		বিরতি;
	    ক্ষেত্র '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", সারি);
		সারি = col;
		বিরতি;
	    ক্ষেত্র '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", সারি);
		সারি = col;
		বিরতি;
	    ক্ষেত্র '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		যদি (col>gx) col += gy;
		যদি (সারি>gx) সারি += gy;
		বিরতি;
	    ক্ষেত্র '+':				// %+c: add c
		সারি += *ptr++;
	    ক্ষেত্র '.':				// print x/y
		যদি (সারি == '\t' ||		// these are
		   সারি == '\n' ||		// chars that
		   সারি == '\004' ||		// UNIX hates
		   সারি == '\0')
		{
		    সারি++;			// so go to next pos
		    যদি (reverse == (সারি == col))
			addup=1;		// and mark UP
		    অন্যথা
			addbak=1;		// or BC
		}
		*bufp++=সারি;
		সারি = col;
		বিরতি;
	    ক্ষেত্র 'r':				// r: reverse
		gx = সারি;
		সারি = col;
		col = gx;
		reverse = 1;
		বিরতি;
	    ক্ষেত্র 'i':			// increment (1-origin screen)
		col++;
		সারি++;
		বিরতি;
	    ক্ষেত্র '%':				// %%=% literally
		*bufp++='%';
		বিরতি;
	    ক্ষেত্র 'n':				// magic DM2500 code
		সারি ^= 0140;
		col ^= 0140;
		বিরতি;
	    ক্ষেত্র 'B':				// bcd encoding
		সারি = সারি/10<<4+সারি%10;
		col = col/10<<4+col%10;
		বিরতি;
	    ক্ষেত্র 'D':				// magic Delta Data code
		সারি = সারি-2*(সারি&15);
		col = col-2*(col&15);
		বিরতি;
	    পূর্বনির্ধারিত:				// Unknown escape
		ফেরত "OOPS";
	    }
	}
    }

    যদি (addup)					// add upline
	যদি (UP)
	{
	    ptr=UP;
	    যখন (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    যদি (*ptr == '*')
		ptr++;
	    যখন (*ptr)
		*bufp++ = *ptr++;
	}

    যদি (addbak)					// add backspace
	যদি (BC)
	{
	    ptr=BC;
	    যখন (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    যদি (*ptr == '*')
		ptr++;
	    যখন (*ptr)
		*bufp++ = *ptr++;
	}
	অন্যথা
	    *bufp++='\b';

    *bufp = 0;

    ফেরত(বাফার);
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

    পূর্ণসংখ্যা
tputs(
    অক্ষর *cp,				// string to print
    পূর্ণসংখ্যা affcnt,				// Number of lines affected
    শূন্য (*outc)(unsigned পূর্ণসংখ্যা))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	গণক,			// digits
	atol(ধ্রুবক অক্ষর *);

    যদি (VIM_ISDIGIT(*cp))
    {
	গণক = 0;
	frac = 1000;
	যখন (VIM_ISDIGIT(*cp))
	    গণক = গণক * 10L + (long)(*cp++ - '0');
	যদি (*cp == '.')
	    যখন (VIM_ISDIGIT(*++cp))
	    {
		গণক = গণক * 10L + (long)(*cp++ - '0');
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
	    গণক = (গণক * _bauds[ospeed] * (long)affcnt) / frac;

	যখন (*cp)			// output string
	    (*outc)(*cp++);
	যদি (ospeed)
	    যখন (গণক--)		// followed by pad characters
		(*outc)(PC);
    }
    অন্যথা
	যখন (*cp)
	    (*outc)(*cp++);
    ফেরত 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    স্থির পূর্ণসংখ্যা
_match(অক্ষর *s1, অক্ষর *s2)
{
    পূর্ণসংখ্যা i = 0;

    যখন (s1[i] && s1[i] == s2[i])
	i++;

    ফেরত i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    স্থির অক্ষর *
_find(অক্ষর *s, অক্ষর *set)
{
    জন্য (; *s; s++)
    {
	অক্ষর	*ptr = set;

	যখন (*ptr && *s != *ptr)
	    ptr++;

	যদি (*ptr)
	    ফেরত s;
    }

    ফেরত s;
}

/*
 * add val to buf according to format fmt
 */
    স্থির অক্ষর *
_addfmt(অক্ষর *buf, অক্ষর *fmt, পূর্ণসংখ্যা val)
{
    sprintf(buf, fmt, val);
    যখন (*buf)
	buf++;
    ফেরত buf;
}
