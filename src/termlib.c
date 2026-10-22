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

#ຖ້າ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

static ຈຳນວນເຕັມ  getent(var_na *, var_na *, FILE *, ຈຳນວນເຕັມ);
static ຈຳນວນເຕັມ  nextent(var_na *, FILE *, ຈຳນວນເຕັມ);
static ຈຳນວນເຕັມ  _match(var_na *, var_na *);
static var_na *_addfmt(var_na *, var_na *, ຈຳນວນເຕັມ);
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
# ບໍ່ດັ່ງນັ້ນ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ບໍ່ດັ່ງນັ້ນ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    ຈຳນວນເຕັມ
tgetent(
    var_na    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    var_na    *term)		// Name of terminal
{
    var_na    tcbuf[32];		// Temp buffer to handle
    var_na    *tcptr = tcbuf;	// extended entries
    var_na    *tcap = TERMCAPFILE; // Default termcap file
    var_na    *tmp;
    FILE    *termcap;
    ຈຳນວນເຕັມ	    retval = 0;
    ຈຳນວນເຕັມ	    len;

    ຖ້າ ((tmp = (var_na *)mch_getenv((char_u *)"TERMCAP")) != NULL)
    {
	ຖ້າ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ຖ້າ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ຖ້າ (tmp)
		*tmp = ':';
#endif
	}
	ບໍ່ດັ່ງນັ້ນ				// TERMCAP = termcap entry itself
	{
	    ຈຳນວນເຕັມ tlen = strlen(term);

	    ໃນຂະນະທີ່ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		var_na *nexttmp;

		ໃນຂະນະທີ່ (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		ຖ້າ (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ສົ່ງຄືນ 1;
		}
		ບໍ່ດັ່ງນັ້ນ
		    tmp = nexttmp;
	    }
	}
    }
    ຖ້າ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ສົ່ງຄືນ -1;
    }

    len = 0;
    ໃນຂະນະທີ່ (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ຖ້າ ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    bha_nga;
	}
    }
    fclose(termcap);
    ສົ່ງຄືນ retval;
}

    static ຈຳນວນເຕັມ
getent(var_na *tbuf, var_na *term, FILE *termcap, ຈຳນວນເຕັມ buflen)
{
    var_na    *tptr;
    ຈຳນວນເຕັມ	    tlen = strlen(term);

    ໃນຂະນະທີ່ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ໃນຂະນະທີ່ (*tptr && *tptr != ':')		// : terminates name field
	{
	    var_na    *nexttptr;

	    ໃນຂະນະທີ່ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    ຖ້າ (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ສົ່ງຄືນ 1;
	    }
	    ບໍ່ດັ່ງນັ້ນ				// Look for next name
		tptr = nexttptr;
	}
    }
    ສົ່ງຄືນ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    static ຈຳນວນເຕັມ
nextent(var_na *tbuf, FILE *termcap, ຈຳນວນເຕັມ buflen)
{
    var_na *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ໃນຂະນະທີ່ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ຈຳນວນເຕັມ)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ຈຳນວນເຕັມ llen = strlen(lbuf);

	ຖ້າ (*lbuf == '#')			// eat comments
	    continue;
	ຖ້າ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ຖ້າ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ບໍ່ດັ່ງນັ້ນ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ສົ່ງຄືນ 1;
	}
    }

    ສົ່ງຄືນ 0;					// ran into end of file
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

    ຈຳນວນເຕັມ
tgetflag(var_na *id)
{
    var_na    buf[256], *ptr = buf;

    ສົ່ງຄືນ tgetstr(id, &ptr) ? 1 : 0;
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

    ຈຳນວນເຕັມ
tgetnum(var_na *id)
{
    var_na *ptr, buf[256];
    ptr = buf;

    ຖ້າ (tgetstr(id, &ptr))
	ສົ່ງຄືນ atoi(buf);
    ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ 0;
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
    ຈຳນວນເຕັມ		len = strlen(id);
    var_na	*tmp=tent;
    var_na	*hold;
    ຈຳນວນເຕັມ		i;

    do
    {
	tmp = _find(tmp, ":");			// For each field
	ໃນຂະນະທີ່ (*tmp == ':')			// skip empty fields
	    tmp++;
	ຖ້າ (!*tmp)
	    bha_nga;

	ຖ້າ (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ຖ້າ (*tmp == '@')			// :xx@: entry for tc
		ສົ່ງຄືນ 0;			// deleted entry
	    hold= *buf;
	    ໃນຂະນະທີ່ (*++tmp && *tmp != ':')	// not at end of field
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
			ສຳລັບ (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
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
	    ສົ່ງຄືນ hold;
	}
    } ໃນຂະນະທີ່ (*tmp);

    ສົ່ງຄືນ 0;
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
    ຈຳນວນເຕັມ	    col,				// column, x position
    ຈຳນວນເຕັມ	    line)				// line, y position
{
    var_na    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    static var_na buffer[32];

    ຖ້າ (!cm)
	ສົ່ງຄືນ "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    ໃນຂະນະທີ່ (*ptr)
    {
	ຖ້າ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ບໍ່ດັ່ງນັ້ນ
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
		ຖ້າ (col>gx) col += gy;
		ຖ້າ (line>gx) line += gy;
		bha_nga;
	    pak_sa '+':				// %+c: add c
		line += *ptr++;
	    pak_sa '.':				// print x/y
		ຖ້າ (line == '\t' ||		// these are
		   line == '\n' ||		// chars that
		   line == '\004' ||		// UNIX hates
		   line == '\0')
		{
		    line++;			// so go to next pos
		    ຖ້າ (reverse == (line == col))
			addup=1;		// and mark UP
		    ບໍ່ດັ່ງນັ້ນ
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
		ສົ່ງຄືນ "OOPS";
	    }
	}
    }

    ຖ້າ (addup)					// add upline
	ຖ້າ (UP)
	{
	    ptr=UP;
	    ໃນຂະນະທີ່ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ຖ້າ (*ptr == '*')
		ptr++;
	    ໃນຂະນະທີ່ (*ptr)
		*bufp++ = *ptr++;
	}

    ຖ້າ (addbak)					// add backspace
	ຖ້າ (BC)
	{
	    ptr=BC;
	    ໃນຂະນະທີ່ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ຖ້າ (*ptr == '*')
		ptr++;
	    ໃນຂະນະທີ່ (*ptr)
		*bufp++ = *ptr++;
	}
	ບໍ່ດັ່ງນັ້ນ
	    *bufp++='\b';

    *bufp = 0;

    ສົ່ງຄືນ(buffer);
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

    ຈຳນວນເຕັມ
tputs(
    var_na *cp,				// string to print
    ຈຳນວນເຕັມ affcnt,				// Number of lines affected
    void (*outc)(unsigned ຈຳນວນເຕັມ))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(const var_na *);

    ຖ້າ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ໃນຂະນະທີ່ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	ຖ້າ (*cp == '.')
	    ໃນຂະນະທີ່ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	ຖ້າ (*cp!='*')			// multiply by affected lines
	{
	    ຖ້າ (affcnt>1) affcnt = 1;
	}
	ບໍ່ດັ່ງນັ້ນ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ຖ້າ (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	ໃນຂະນະທີ່ (*cp)			// output string
	    (*outc)(*cp++);
	ຖ້າ (ospeed)
	    ໃນຂະນະທີ່ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    ບໍ່ດັ່ງນັ້ນ
	ໃນຂະນະທີ່ (*cp)
	    (*outc)(*cp++);
    ສົ່ງຄືນ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    static ຈຳນວນເຕັມ
_match(var_na *s1, var_na *s2)
{
    ຈຳນວນເຕັມ i = 0;

    ໃນຂະນະທີ່ (s1[i] && s1[i] == s2[i])
	i++;

    ສົ່ງຄືນ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    static var_na *
_find(var_na *s, var_na *set)
{
    ສຳລັບ (; *s; s++)
    {
	var_na	*ptr = set;

	ໃນຂະນະທີ່ (*ptr && *s != *ptr)
	    ptr++;

	ຖ້າ (*ptr)
	    ສົ່ງຄືນ s;
    }

    ສົ່ງຄືນ s;
}

/*
 * add val to buf according to format fmt
 */
    static var_na *
_addfmt(var_na *buf, var_na *fmt, ຈຳນວນເຕັມ val)
{
    sprintf(buf, fmt, val);
    ໃນຂະນະທີ່ (*buf)
	buf++;
    ສົ່ງຄືນ buf;
}
