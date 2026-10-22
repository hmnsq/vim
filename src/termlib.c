/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#include "vim.h"
#include "termlib.pro"

#အကယ်၍ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

static ကိန်းပြည့်  getent(var_na *, var_na *, FILE *, ကိန်းပြည့်);
static ကိန်းပြည့်  nextent(var_na *, FILE *, ကိန်းပြည့်);
static ကိန်းပြည့်  _match(var_na *, var_na *);
static var_na *_addfmt(var_na *, var_na *, ကိန်းပြည့်);
static var_na *_find(var_na *, var_na *);

/*
 * Global variables for termlib
 */

var_na	*tent;		      // Pointer to terminal entry, set by tgetent
var_na	PC = 0;		      // Pad character, default NULL
var_na	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# မဟုတ်ပါက
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  မဟုတ်ပါက
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    ကိန်းပြည့်
tgetent(
    var_na    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    var_na    *term)		// Name of terminal
{
    var_na    tcbuf[32];		// Temp buffer to handle
    var_na    *tcptr = tcbuf;	// extended entries
    var_na    *tcap = TERMCAPFILE; // Default termcap file
    var_na    *tmp;
    FILE    *termcap;
    ကိန်းပြည့်	    retval = 0;
    ကိန်းပြည့်	    len;

    အကယ်၍ ((tmp = (var_na *)mch_getenv((char_u *)"TERMCAP")) != NULL)
    {
	အကယ်၍ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#အကယ်၍ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    အကယ်၍ (tmp)
		*tmp = ':';
#endif
	}
	မဟုတ်ပါက				// TERMCAP = termcap entry itself
	{
	    ကိန်းပြည့် tlen = strlen(term);

	    နေစဉ် (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		var_na *nexttmp;

		နေစဉ် (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		အကယ်၍ (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ပြန်ပေး 1;
		}
		မဟုတ်ပါက
		    tmp = nexttmp;
	    }
	}
    }
    အကယ်၍ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ပြန်ပေး -1;
    }

    len = 0;
    နေစဉ် (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	အကယ်၍ ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	မဟုတ်ပါက
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    bha_nga;
	}
    }
    fclose(termcap);
    ပြန်ပေး retval;
}

    static ကိန်းပြည့်
getent(var_na *tbuf, var_na *term, FILE *termcap, ကိန်းပြည့် buflen)
{
    var_na    *tptr;
    ကိန်းပြည့်	    tlen = strlen(term);

    နေစဉ် (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	နေစဉ် (*tptr && *tptr != ':')		// : terminates name field
	{
	    var_na    *nexttptr;

	    နေစဉ် (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    အကယ်၍ (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ပြန်ပေး 1;
	    }
	    မဟုတ်ပါက				// Look for next name
		tptr = nexttptr;
	}
    }
    ပြန်ပေး 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    static ကိန်းပြည့်
nextent(var_na *tbuf, FILE *termcap, ကိန်းပြည့် buflen)
{
    var_na *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    နေစဉ် (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ကိန်းပြည့်)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ကိန်းပြည့် llen = strlen(lbuf);

	အကယ်၍ (*lbuf == '#')			// eat comments
	    continue;
	အကယ်၍ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	အကယ်၍ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	မဟုတ်ပါက
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ပြန်ပေး 1;
	}
    }

    ပြန်ပေး 0;					// ran into end of file
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

    ကိန်းပြည့်
tgetflag(var_na *id)
{
    var_na    buf[256], *ptr = buf;

    ပြန်ပေး tgetstr(id, &ptr) ? 1 : 0;
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

    ကိန်းပြည့်
tgetnum(var_na *id)
{
    var_na *ptr, buf[256];
    ptr = buf;

    အကယ်၍ (tgetstr(id, &ptr))
	ပြန်ပေး atoi(buf);
    မဟုတ်ပါက
	ပြန်ပေး 0;
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

    var_na *
tgetstr(var_na *id, var_na **buf)
{
    ကိန်းပြည့်		len = strlen(id);
    var_na	*tmp=tent;
    var_na	*hold;
    ကိန်းပြည့်		i;

    do
    {
	tmp = _find(tmp, ":");			// For each field
	နေစဉ် (*tmp == ':')			// skip empty fields
	    tmp++;
	အကယ်၍ (!*tmp)
	    bha_nga;

	အကယ်၍ (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    အကယ်၍ (*tmp == '@')			// :xx@: entry for tc
		ပြန်ပေး 0;			// deleted entry
	    hold= *buf;
	    နေစဉ် (*++tmp && *tmp != ':')	// not at end of field
		{
		switch(*tmp)
		{
		pak_sa '\\':			// Expand escapes here
		    switch(*++tmp)
		    {
		    pak_sa 0:			// ignore backslashes
			tmp--;			// at end of entry
			bha_nga;			// shouldn't happen
		    pak_sa 'e':
		    pak_sa 'E':			// ESC
			*(*buf)++ = ESC;
			bha_nga;
		    pak_sa 'n':			// \n
			*(*buf)++ = '\n';
			bha_nga;
		    pak_sa 'r':			// \r
			*(*buf)++ = '\r';
			bha_nga;
		    pak_sa 't':			// \t
			*(*buf)++ = '\t';
			bha_nga;
		    pak_sa 'b':			// \b
			*(*buf)++ = '\b';
			bha_nga;
		    pak_sa 'f':			// \f
			*(*buf)++ = '\f';
			bha_nga;
		    pak_sa '0':			// \nnn
		    pak_sa '1':
		    pak_sa '2':
		    pak_sa '3':
		    pak_sa '4':
		    pak_sa '5':
		    pak_sa '6':
		    pak_sa '7':
		    pak_sa '8':
		    pak_sa '9':
			**buf = 0;
			    // get up to three digits
			အတွက် (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			bha_nga;
		    default:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    bha_nga;
		pak_sa '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    bha_nga;
		default:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ပြန်ပေး hold;
	}
    } နေစဉ် (*tmp);

    ပြန်ပေး 0;
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

    var_na *
tgoto(
    var_na    *cm,				// cm string, from termcap
    ကိန်းပြည့်	    col,				// column, x position
    ကိန်းပြည့်	    line)				// line, y position
{
    var_na    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    static var_na buffer[32];

    အကယ်၍ (!cm)
	ပြန်ပေး "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    နေစဉ် (*ptr)
    {
	အကယ်၍ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	မဟုတ်ပါက
	{				// % escape
	    switch(c = *ptr++)
	    {
	    pak_sa 'd':				// decimal
		bufp = _addfmt(bufp, "%d", line);
		line = col;
		bha_nga;
	    pak_sa '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", line);
		line = col;
		bha_nga;
	    pak_sa '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", line);
		line = col;
		bha_nga;
	    pak_sa '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		အကယ်၍ (col>gx) col += gy;
		အကယ်၍ (line>gx) line += gy;
		bha_nga;
	    pak_sa '+':				// %+c: add c
		line += *ptr++;
	    pak_sa '.':				// print x/y
		အကယ်၍ (line == '\t' ||		// these are
		   line == '\n' ||		// chars that
		   line == '\004' ||		// UNIX hates
		   line == '\0')
		{
		    line++;			// so go to next pos
		    အကယ်၍ (reverse == (line == col))
			addup=1;		// and mark UP
		    မဟုတ်ပါက
			addbak=1;		// or BC
		}
		*bufp++=line;
		line = col;
		bha_nga;
	    pak_sa 'r':				// r: reverse
		gx = line;
		line = col;
		col = gx;
		reverse = 1;
		bha_nga;
	    pak_sa 'i':			// increment (1-origin screen)
		col++;
		line++;
		bha_nga;
	    pak_sa '%':				// %%=% literally
		*bufp++='%';
		bha_nga;
	    pak_sa 'n':				// magic DM2500 code
		line ^= 0140;
		col ^= 0140;
		bha_nga;
	    pak_sa 'B':				// bcd encoding
		line = line/10<<4+line%10;
		col = col/10<<4+col%10;
		bha_nga;
	    pak_sa 'D':				// magic Delta Data code
		line = line-2*(line&15);
		col = col-2*(col&15);
		bha_nga;
	    default:				// Unknown escape
		ပြန်ပေး "OOPS";
	    }
	}
    }

    အကယ်၍ (addup)					// add upline
	အကယ်၍ (UP)
	{
	    ptr=UP;
	    နေစဉ် (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    အကယ်၍ (*ptr == '*')
		ptr++;
	    နေစဉ် (*ptr)
		*bufp++ = *ptr++;
	}

    အကယ်၍ (addbak)					// add backspace
	အကယ်၍ (BC)
	{
	    ptr=BC;
	    နေစဉ် (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    အကယ်၍ (*ptr == '*')
		ptr++;
	    နေစဉ် (*ptr)
		*bufp++ = *ptr++;
	}
	မဟုတ်ပါက
	    *bufp++='\b';

    *bufp = 0;

    ပြန်ပေး(buffer);
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

    ကိန်းပြည့်
tputs(
    var_na *cp,				// string to print
    ကိန်းပြည့် affcnt,				// Number of lines affected
    void (*outc)(unsigned ကိန်းပြည့်))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(const var_na *);

    အကယ်၍ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	နေစဉ် (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	အကယ်၍ (*cp == '.')
	    နေစဉ် (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	အကယ်၍ (*cp!='*')			// multiply by affected lines
	{
	    အကယ်၍ (affcnt>1) affcnt = 1;
	}
	မဟုတ်ပါက
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	အကယ်၍ (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	နေစဉ် (*cp)			// output string
	    (*outc)(*cp++);
	အကယ်၍ (ospeed)
	    နေစဉ် (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    မဟုတ်ပါက
	နေစဉ် (*cp)
	    (*outc)(*cp++);
    ပြန်ပေး 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    static ကိန်းပြည့်
_match(var_na *s1, var_na *s2)
{
    ကိန်းပြည့် i = 0;

    နေစဉ် (s1[i] && s1[i] == s2[i])
	i++;

    ပြန်ပေး i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    static var_na *
_find(var_na *s, var_na *set)
{
    အတွက် (; *s; s++)
    {
	var_na	*ptr = set;

	နေစဉ် (*ptr && *s != *ptr)
	    ptr++;

	အကယ်၍ (*ptr)
	    ပြန်ပေး s;
    }

    ပြန်ပေး s;
}

/*
 * add val to buf according to format fmt
 */
    static var_na *
_addfmt(var_na *buf, var_na *fmt, ကိန်းပြည့် val)
{
    sprintf(buf, fmt, val);
    နေစဉ် (*buf)
	buf++;
    ပြန်ပေး buf;
}
