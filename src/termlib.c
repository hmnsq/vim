/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#ضمن "vim.h"
#ضمن "termlib.pro"

#אם !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

ساكن صحيح  getent(حرف *, حرف *, FILE *, صحيح);
ساكن صحيح  nextent(حرف *, FILE *, صحيح);
ساكن صحيح  _match(حرف *, حرف *);
ساكن حرف *_addfmt(حرف *, حرف *, صحيح);
ساكن حرف *_find(حرف *, حرف *);

/*
 * Global variables for termlib
 */

حرف	*tent;		      // Pointer to terminal entry, set by tgetent
حرف	PC = 0;		      // Pad character, default NULL
حرف	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# אחרת
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  אחרת
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    صحيح
tgetent(
    حرف    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    حرف    *term)		// Name of terminal
{
    حرف    tcbuf[32];		// Temp buffer to handle
    حرف    *tcptr = tcbuf;	// extended entries
    حرف    *tcap = TERMCAPFILE; // Default termcap file
    حرف    *tmp;
    FILE    *termcap;
    صحيح	    retval = 0;
    صحيح	    len;

    אם ((tmp = (حرف *)mch_getenv((char_u *)"TERMCAP")) != عدم)
    {
	אם (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#אם defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    אם (tmp)
		*tmp = ':';
#endif
	}
	אחרת				// TERMCAP = termcap entry itself
	{
	    صحيح tlen = strlen(term);

	    כלעוד (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		حرف *nexttmp;

		כלעוד (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		אם (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    החזר 1;
		}
		אחרת
		    tmp = nexttmp;
	    }
	}
    }
    אם (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	החזר -1;
    }

    len = 0;
    כלעוד (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	אם ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	אחרת
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    اكسر;
	}
    }
    fclose(termcap);
    החזר retval;
}

    ساكن صحيح
getent(حرف *tbuf, حرف *term, FILE *termcap, صحيح buflen)
{
    حرف    *tptr;
    صحيح	    tlen = strlen(term);

    כלעוד (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	כלעוד (*tptr && *tptr != ':')		// : terminates name field
	{
	    حرف    *nexttptr;

	    כלעוד (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    אם (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		החזר 1;
	    }
	    אחרת				// Look for next name
		tptr = nexttptr;
	}
    }
    החזר 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ساكن صحيح
nextent(حرف *tbuf, FILE *termcap, صحيح buflen)
{
    حرف *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    כלעוד (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (صحيح)(tbuf+buflen-lbuf), termcap)) // another line
    {
	صحيح llen = strlen(lbuf);

	אם (*lbuf == '#')			// eat comments
	    استمر;
	אם (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	אם (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	אחרת
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    החזר 1;
	}
    }

    החזר 0;					// ran into end of file
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

    صحيح
tgetflag(حرف *id)
{
    حرف    buf[256], *ptr = buf;

    החזר tgetstr(id, &ptr) ? 1 : 0;
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

    صحيح
tgetnum(حرف *id)
{
    حرف *ptr, buf[256];
    ptr = buf;

    אם (tgetstr(id, &ptr))
	החזר atoi(buf);
    אחרת
	החזר 0;
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

    حرف *
tgetstr(حرف *id, حرف **buf)
{
    صحيح		len = strlen(id);
    حرف	*tmp=tent;
    حرف	*hold;
    صحيح		i;

    עשה
    {
	tmp = _find(tmp, ":");			// For each field
	כלעוד (*tmp == ':')			// skip empty fields
	    tmp++;
	אם (!*tmp)
	    اكسر;

	אם (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    אם (*tmp == '@')			// :xx@: entry for tc
		החזר 0;			// deleted entry
	    hold= *buf;
	    כלעוד (*++tmp && *tmp != ':')	// not at end of field
		{
		بدل(*tmp)
		{
		حالة '\\':			// Expand escapes here
		    بدل(*++tmp)
		    {
		    حالة 0:			// ignore backslashes
			tmp--;			// at end of entry
			اكسر;			// shouldn't happen
		    حالة 'e':
		    حالة 'E':			// ESC
			*(*buf)++ = ESC;
			اكسر;
		    حالة 'n':			// \n
			*(*buf)++ = '\n';
			اكسر;
		    حالة 'r':			// \r
			*(*buf)++ = '\r';
			اكسر;
		    حالة 't':			// \t
			*(*buf)++ = '\t';
			اكسر;
		    حالة 'b':			// \b
			*(*buf)++ = '\b';
			اكسر;
		    حالة 'f':			// \f
			*(*buf)++ = '\f';
			اكسر;
		    حالة '0':			// \nnn
		    حالة '1':
		    حالة '2':
		    حالة '3':
		    حالة '4':
		    حالة '5':
		    حالة '6':
		    حالة '7':
		    حالة '8':
		    حالة '9':
			**buf = 0;
			    // get up to three digits
			עבור (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			اكسر;
		    افتراضي:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    اكسر;
		حالة '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    اكسر;
		افتراضي:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    החזר hold;
	}
    } כלעוד (*tmp);

    החזר 0;
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

    حرف *
tgoto(
    حرف    *cm,				// cm string, from termcap
    صحيح	    col,				// column, x position
    صحيح	    line)				// line, y position
{
    حرف    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ساكن حرف buffer[32];

    אם (!cm)
	החזר "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    כלעוד (*ptr)
    {
	אם ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	אחרת
	{				// % escape
	    بدل(c = *ptr++)
	    {
	    حالة 'd':				// decimal
		bufp = _addfmt(bufp, "%d", line);
		line = col;
		اكسر;
	    حالة '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", line);
		line = col;
		اكسر;
	    حالة '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", line);
		line = col;
		اكسر;
	    حالة '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		אם (col>gx) col += gy;
		אם (line>gx) line += gy;
		اكسر;
	    حالة '+':				// %+c: add c
		line += *ptr++;
	    حالة '.':				// print x/y
		אם (line == '\t' ||		// these are
		   line == '\n' ||		// chars that
		   line == '\004' ||		// UNIX hates
		   line == '\0')
		{
		    line++;			// so go to next pos
		    אם (reverse == (line == col))
			addup=1;		// and mark UP
		    אחרת
			addbak=1;		// or BC
		}
		*bufp++=line;
		line = col;
		اكسر;
	    حالة 'r':				// r: reverse
		gx = line;
		line = col;
		col = gx;
		reverse = 1;
		اكسر;
	    حالة 'i':			// increment (1-origin screen)
		col++;
		line++;
		اكسر;
	    حالة '%':				// %%=% literally
		*bufp++='%';
		اكسر;
	    حالة 'n':				// magic DM2500 code
		line ^= 0140;
		col ^= 0140;
		اكسر;
	    حالة 'B':				// bcd encoding
		line = line/10<<4+line%10;
		col = col/10<<4+col%10;
		اكسر;
	    حالة 'D':				// magic Delta Data code
		line = line-2*(line&15);
		col = col-2*(col&15);
		اكسر;
	    افتراضي:				// Unknown escape
		החזר "OOPS";
	    }
	}
    }

    אם (addup)					// add upline
	אם (UP)
	{
	    ptr=UP;
	    כלעוד (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    אם (*ptr == '*')
		ptr++;
	    כלעוד (*ptr)
		*bufp++ = *ptr++;
	}

    אם (addbak)					// add backspace
	אם (BC)
	{
	    ptr=BC;
	    כלעוד (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    אם (*ptr == '*')
		ptr++;
	    כלעוד (*ptr)
		*bufp++ = *ptr++;
	}
	אחרת
	    *bufp++='\b';

    *bufp = 0;

    החזר(buffer);
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

    صحيح
tputs(
    حرف *cp,				// string to print
    صحيح affcnt,				// Number of lines affected
    فراغ (*outc)(unsigned صحيح))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ثابت حرف *);

    אם (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	כלעוד (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	אם (*cp == '.')
	    כלעוד (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	אם (*cp!='*')			// multiply by affected lines
	{
	    אם (affcnt>1) affcnt = 1;
	}
	אחרת
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	אם (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	כלעוד (*cp)			// output string
	    (*outc)(*cp++);
	אם (ospeed)
	    כלעוד (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    אחרת
	כלעוד (*cp)
	    (*outc)(*cp++);
    החזר 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ساكن صحيح
_match(حرف *s1, حرف *s2)
{
    صحيح i = 0;

    כלעוד (s1[i] && s1[i] == s2[i])
	i++;

    החזר i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ساكن حرف *
_find(حرف *s, حرف *set)
{
    עבור (; *s; s++)
    {
	حرف	*ptr = set;

	כלעוד (*ptr && *s != *ptr)
	    ptr++;

	אם (*ptr)
	    החזר s;
    }

    החזר s;
}

/*
 * add val to buf according to format fmt
 */
    ساكن حرف *
_addfmt(حرف *buf, حرف *fmt, صحيح val)
{
    sprintf(buf, fmt, val);
    כלעוד (*buf)
	buf++;
    החזר buf;
}
