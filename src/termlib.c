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

#ပေႃး !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

static puur_naa_nka  getent(var_na *, var_na *, FILE *, puur_naa_nka);
static puur_naa_nka  nextent(var_na *, FILE *, puur_naa_nka);
static puur_naa_nka  _match(var_na *, var_na *);
static var_na *_addfmt(var_na *, var_na *, puur_naa_nka);
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
# ပေႃးဢမ်ႇ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ပေႃးဢမ်ႇ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    puur_naa_nka
tgetent(
    var_na    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    var_na    *term)		// Name of terminal
{
    var_na    tcbuf[32];		// Temp buffer to handle
    var_na    *tcptr = tcbuf;	// extended entries
    var_na    *tcap = TERMCAPFILE; // Default termcap file
    var_na    *tmp;
    FILE    *termcap;
    puur_naa_nka	    retval = 0;
    puur_naa_nka	    len;

    ပေႃး ((tmp = (var_na *)mch_getenv((char_u *)"TERMCAP")) != NULL)
    {
	ပေႃး (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ပေႃး defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ပေႃး (tmp)
		*tmp = ':';
#endif
	}
	ပေႃးဢမ်ႇ				// TERMCAP = termcap entry itself
	{
	    puur_naa_nka tlen = strlen(term);

	    ၶၢဝ်း (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		var_na *nexttmp;

		ၶၢဝ်း (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		ပေႃး (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ပွၵ်ႈ 1;
		}
		ပေႃးဢမ်ႇ
		    tmp = nexttmp;
	    }
	}
    }
    ပေႃး (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ပွၵ်ႈ -1;
    }

    len = 0;
    ၶၢဝ်း (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ပေႃး ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	ပေႃးဢမ်ႇ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    bha_nga;
	}
    }
    fclose(termcap);
    ပွၵ်ႈ retval;
}

    static puur_naa_nka
getent(var_na *tbuf, var_na *term, FILE *termcap, puur_naa_nka buflen)
{
    var_na    *tptr;
    puur_naa_nka	    tlen = strlen(term);

    ၶၢဝ်း (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ၶၢဝ်း (*tptr && *tptr != ':')		// : terminates name field
	{
	    var_na    *nexttptr;

	    ၶၢဝ်း (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    ပေႃး (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ပွၵ်ႈ 1;
	    }
	    ပေႃးဢမ်ႇ				// Look for next name
		tptr = nexttptr;
	}
    }
    ပွၵ်ႈ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    static puur_naa_nka
nextent(var_na *tbuf, FILE *termcap, puur_naa_nka buflen)
{
    var_na *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ၶၢဝ်း (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (puur_naa_nka)(tbuf+buflen-lbuf), termcap)) // another line
    {
	puur_naa_nka llen = strlen(lbuf);

	ပေႃး (*lbuf == '#')			// eat comments
	    continue;
	ပေႃး (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ပေႃး (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ပေႃးဢမ်ႇ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ပွၵ်ႈ 1;
	}
    }

    ပွၵ်ႈ 0;					// ran into end of file
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

    puur_naa_nka
tgetflag(var_na *id)
{
    var_na    buf[256], *ptr = buf;

    ပွၵ်ႈ tgetstr(id, &ptr) ? 1 : 0;
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

    puur_naa_nka
tgetnum(var_na *id)
{
    var_na *ptr, buf[256];
    ptr = buf;

    ပေႃး (tgetstr(id, &ptr))
	ပွၵ်ႈ atoi(buf);
    ပေႃးဢမ်ႇ
	ပွၵ်ႈ 0;
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
    puur_naa_nka		len = strlen(id);
    var_na	*tmp=tent;
    var_na	*hold;
    puur_naa_nka		i;

    do
    {
	tmp = _find(tmp, ":");			// For each field
	ၶၢဝ်း (*tmp == ':')			// skip empty fields
	    tmp++;
	ပေႃး (!*tmp)
	    bha_nga;

	ပေႃး (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ပေႃး (*tmp == '@')			// :xx@: entry for tc
		ပွၵ်ႈ 0;			// deleted entry
	    hold= *buf;
	    ၶၢဝ်း (*++tmp && *tmp != ':')	// not at end of field
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
			for (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
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
	    ပွၵ်ႈ hold;
	}
    } ၶၢဝ်း (*tmp);

    ပွၵ်ႈ 0;
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
    puur_naa_nka	    col,				// column, x position
    puur_naa_nka	    line)				// line, y position
{
    var_na    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    static var_na buffer[32];

    ပေႃး (!cm)
	ပွၵ်ႈ "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    ၶၢဝ်း (*ptr)
    {
	ပေႃး ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ပေႃးဢမ်ႇ
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
		ပေႃး (col>gx) col += gy;
		ပေႃး (line>gx) line += gy;
		bha_nga;
	    pak_sa '+':				// %+c: add c
		line += *ptr++;
	    pak_sa '.':				// print x/y
		ပေႃး (line == '\t' ||		// these are
		   line == '\n' ||		// chars that
		   line == '\004' ||		// UNIX hates
		   line == '\0')
		{
		    line++;			// so go to next pos
		    ပေႃး (reverse == (line == col))
			addup=1;		// and mark UP
		    ပေႃးဢမ်ႇ
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
		ပွၵ်ႈ "OOPS";
	    }
	}
    }

    ပေႃး (addup)					// add upline
	ပေႃး (UP)
	{
	    ptr=UP;
	    ၶၢဝ်း (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ပေႃး (*ptr == '*')
		ptr++;
	    ၶၢဝ်း (*ptr)
		*bufp++ = *ptr++;
	}

    ပေႃး (addbak)					// add backspace
	ပေႃး (BC)
	{
	    ptr=BC;
	    ၶၢဝ်း (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ပေႃး (*ptr == '*')
		ptr++;
	    ၶၢဝ်း (*ptr)
		*bufp++ = *ptr++;
	}
	ပေႃးဢမ်ႇ
	    *bufp++='\b';

    *bufp = 0;

    ပွၵ်ႈ(buffer);
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

    puur_naa_nka
tputs(
    var_na *cp,				// string to print
    puur_naa_nka affcnt,				// Number of lines affected
    void (*outc)(unsigned puur_naa_nka))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(const var_na *);

    ပေႃး (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ၶၢဝ်း (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	ပေႃး (*cp == '.')
	    ၶၢဝ်း (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	ပေႃး (*cp!='*')			// multiply by affected lines
	{
	    ပေႃး (affcnt>1) affcnt = 1;
	}
	ပေႃးဢမ်ႇ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ပေႃး (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	ၶၢဝ်း (*cp)			// output string
	    (*outc)(*cp++);
	ပေႃး (ospeed)
	    ၶၢဝ်း (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    ပေႃးဢမ်ႇ
	ၶၢဝ်း (*cp)
	    (*outc)(*cp++);
    ပွၵ်ႈ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    static puur_naa_nka
_match(var_na *s1, var_na *s2)
{
    puur_naa_nka i = 0;

    ၶၢဝ်း (s1[i] && s1[i] == s2[i])
	i++;

    ပွၵ်ႈ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    static var_na *
_find(var_na *s, var_na *set)
{
    for (; *s; s++)
    {
	var_na	*ptr = set;

	ၶၢဝ်း (*ptr && *s != *ptr)
	    ptr++;

	ပေႃး (*ptr)
	    ပွၵ်ႈ s;
    }

    ပွၵ်ႈ s;
}

/*
 * add val to buf according to format fmt
 */
    static var_na *
_addfmt(var_na *buf, var_na *fmt, puur_naa_nka val)
{
    sprintf(buf, fmt, val);
    ၶၢဝ်း (*buf)
	buf++;
    ပွၵ်ႈ buf;
}
