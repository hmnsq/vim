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

#གལ་སྲིད !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

static ཧྲིལ  getent(var_na *, var_na *, FILE *, ཧྲིལ);
static ཧྲིལ  nextent(var_na *, FILE *, ཧྲིལ);
static ཧྲིལ  _match(var_na *, var_na *);
static var_na *_addfmt(var_na *, var_na *, ཧྲིལ);
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
# དེ་མིན
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  དེ་མིན
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    ཧྲིལ
tgetent(
    var_na    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    var_na    *term)		// Name of terminal
{
    var_na    tcbuf[32];		// Temp buffer to handle
    var_na    *tcptr = tcbuf;	// extended entries
    var_na    *tcap = TERMCAPFILE; // Default termcap file
    var_na    *tmp;
    FILE    *termcap;
    ཧྲིལ	    retval = 0;
    ཧྲིལ	    len;

    གལ་སྲིད ((tmp = (var_na *)mch_getenv((char_u *)"TERMCAP")) != NULL)
    {
	གལ་སྲིད (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#གལ་སྲིད defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    གལ་སྲིད (tmp)
		*tmp = ':';
#endif
	}
	དེ་མིན				// TERMCAP = termcap entry itself
	{
	    ཧྲིལ tlen = strlen(term);

	    སྐབས (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		var_na *nexttmp;

		སྐབས (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		གལ་སྲིད (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ལོག 1;
		}
		དེ་མིན
		    tmp = nexttmp;
	    }
	}
    }
    གལ་སྲིད (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ལོག -1;
    }

    len = 0;
    སྐབས (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	གལ་སྲིད ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	དེ་མིན
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    bha_nga;
	}
    }
    fclose(termcap);
    ལོག retval;
}

    static ཧྲིལ
getent(var_na *tbuf, var_na *term, FILE *termcap, ཧྲིལ buflen)
{
    var_na    *tptr;
    ཧྲིལ	    tlen = strlen(term);

    སྐབས (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	སྐབས (*tptr && *tptr != ':')		// : terminates name field
	{
	    var_na    *nexttptr;

	    སྐབས (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    གལ་སྲིད (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ལོག 1;
	    }
	    དེ་མིན				// Look for next name
		tptr = nexttptr;
	}
    }
    ལོག 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    static ཧྲིལ
nextent(var_na *tbuf, FILE *termcap, ཧྲིལ buflen)
{
    var_na *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    སྐབས (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ཧྲིལ)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ཧྲིལ llen = strlen(lbuf);

	གལ་སྲིད (*lbuf == '#')			// eat comments
	    continue;
	གལ་སྲིད (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	གལ་སྲིད (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	དེ་མིན
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ལོག 1;
	}
    }

    ལོག 0;					// ran into end of file
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

    ཧྲིལ
tgetflag(var_na *id)
{
    var_na    buf[256], *ptr = buf;

    ལོག tgetstr(id, &ptr) ? 1 : 0;
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

    ཧྲིལ
tgetnum(var_na *id)
{
    var_na *ptr, buf[256];
    ptr = buf;

    གལ་སྲིད (tgetstr(id, &ptr))
	ལོག atoi(buf);
    དེ་མིན
	ལོག 0;
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
    ཧྲིལ		len = strlen(id);
    var_na	*tmp=tent;
    var_na	*hold;
    ཧྲིལ		i;

    do
    {
	tmp = _find(tmp, ":");			// For each field
	སྐབས (*tmp == ':')			// skip empty fields
	    tmp++;
	གལ་སྲིད (!*tmp)
	    bha_nga;

	གལ་སྲིད (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    གལ་སྲིད (*tmp == '@')			// :xx@: entry for tc
		ལོག 0;			// deleted entry
	    hold= *buf;
	    སྐབས (*++tmp && *tmp != ':')	// not at end of field
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
			དོན་ལུ (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
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
	    ལོག hold;
	}
    } སྐབས (*tmp);

    ལོག 0;
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
    ཧྲིལ	    col,				// column, x position
    ཧྲིལ	    line)				// line, y position
{
    var_na    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    static var_na buffer[32];

    གལ་སྲིད (!cm)
	ལོག "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    སྐབས (*ptr)
    {
	གལ་སྲིད ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	དེ་མིན
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
		གལ་སྲིད (col>gx) col += gy;
		གལ་སྲིད (line>gx) line += gy;
		bha_nga;
	    pak_sa '+':				// %+c: add c
		line += *ptr++;
	    pak_sa '.':				// print x/y
		གལ་སྲིད (line == '\t' ||		// these are
		   line == '\n' ||		// chars that
		   line == '\004' ||		// UNIX hates
		   line == '\0')
		{
		    line++;			// so go to next pos
		    གལ་སྲིད (reverse == (line == col))
			addup=1;		// and mark UP
		    དེ་མིན
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
		ལོག "OOPS";
	    }
	}
    }

    གལ་སྲིད (addup)					// add upline
	གལ་སྲིད (UP)
	{
	    ptr=UP;
	    སྐབས (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    གལ་སྲིད (*ptr == '*')
		ptr++;
	    སྐབས (*ptr)
		*bufp++ = *ptr++;
	}

    གལ་སྲིད (addbak)					// add backspace
	གལ་སྲིད (BC)
	{
	    ptr=BC;
	    སྐབས (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    གལ་སྲིད (*ptr == '*')
		ptr++;
	    སྐབས (*ptr)
		*bufp++ = *ptr++;
	}
	དེ་མིན
	    *bufp++='\b';

    *bufp = 0;

    ལོག(buffer);
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

    ཧྲིལ
tputs(
    var_na *cp,				// string to print
    ཧྲིལ affcnt,				// Number of lines affected
    void (*outc)(unsigned ཧྲིལ))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(const var_na *);

    གལ་སྲིད (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	སྐབས (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	གལ་སྲིད (*cp == '.')
	    སྐབས (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	གལ་སྲིད (*cp!='*')			// multiply by affected lines
	{
	    གལ་སྲིད (affcnt>1) affcnt = 1;
	}
	དེ་མིན
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	གལ་སྲིད (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	སྐབས (*cp)			// output string
	    (*outc)(*cp++);
	གལ་སྲིད (ospeed)
	    སྐབས (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    དེ་མིན
	སྐབས (*cp)
	    (*outc)(*cp++);
    ལོག 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    static ཧྲིལ
_match(var_na *s1, var_na *s2)
{
    ཧྲིལ i = 0;

    སྐབས (s1[i] && s1[i] == s2[i])
	i++;

    ལོག i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    static var_na *
_find(var_na *s, var_na *set)
{
    དོན་ལུ (; *s; s++)
    {
	var_na	*ptr = set;

	སྐབས (*ptr && *s != *ptr)
	    ptr++;

	གལ་སྲིད (*ptr)
	    ལོག s;
    }

    ལོག s;
}

/*
 * add val to buf according to format fmt
 */
    static var_na *
_addfmt(var_na *buf, var_na *fmt, ཧྲིལ val)
{
    sprintf(buf, fmt, val);
    སྐབས (*buf)
	buf++;
    ལོག buf;
}
