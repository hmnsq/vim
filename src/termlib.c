/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#شامل "vim.h"
#شامل "termlib.pro"

#اگر !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

ایستا صحیح  getent(نویسه *, نویسه *, FILE *, صحیح);
ایستا صحیح  nextent(نویسه *, FILE *, صحیح);
ایستا صحیح  _match(نویسه *, نویسه *);
ایستا نویسه *_addfmt(نویسه *, نویسه *, صحیح);
ایستا نویسه *_find(نویسه *, نویسه *);

/*
 * Global variables for termlib
 */

نویسه	*tent;		      // Pointer to terminal entry, set by tgetent
نویسه	PC = 0;		      // Pad character, default NULL
نویسه	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# وگرنه
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  وگرنه
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    صحیح
tgetent(
    نویسه    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    نویسه    *term)		// Name of terminal
{
    نویسه    tcbuf[32];		// Temp buffer to handle
    نویسه    *tcptr = tcbuf;	// extended entries
    نویسه    *tcap = TERMCAPFILE; // Default termcap file
    نویسه    *tmp;
    FILE    *termcap;
    صحیح	    retval = 0;
    صحیح	    len;

    اگر ((tmp = (نویسه *)mch_getenv((char_u *)"TERMCAP")) != پوچ)
    {
	اگر (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#اگر defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    اگر (tmp)
		*tmp = ':';
#endif
	}
	وگرنه				// TERMCAP = termcap entry itself
	{
	    صحیح tlen = strlen(term);

	    وقتی (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		نویسه *nexttmp;

		وقتی (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		اگر (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    بازگشت 1;
		}
		وگرنه
		    tmp = nexttmp;
	    }
	}
    }
    اگر (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	بازگشت -1;
    }

    len = 0;
    وقتی (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	اگر ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	وگرنه
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    بشکن;
	}
    }
    fclose(termcap);
    بازگشت retval;
}

    ایستا صحیح
getent(نویسه *tbuf, نویسه *term, FILE *termcap, صحیح buflen)
{
    نویسه    *tptr;
    صحیح	    tlen = strlen(term);

    وقتی (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	وقتی (*tptr && *tptr != ':')		// : terminates name field
	{
	    نویسه    *nexttptr;

	    وقتی (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    اگر (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		بازگشت 1;
	    }
	    وگرنه				// Look for next name
		tptr = nexttptr;
	}
    }
    بازگشت 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ایستا صحیح
nextent(نویسه *tbuf, FILE *termcap, صحیح buflen)
{
    نویسه *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    وقتی (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (صحیح)(tbuf+buflen-lbuf), termcap)) // another line
    {
	صحیح llen = strlen(lbuf);

	اگر (*lbuf == '#')			// eat comments
	    ادامه;
	اگر (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	اگر (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	وگرنه
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    بازگشت 1;
	}
    }

    بازگشت 0;					// ran into end of file
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

    صحیح
tgetflag(نویسه *id)
{
    نویسه    buf[256], *ptr = buf;

    بازگشت tgetstr(id, &ptr) ? 1 : 0;
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

    صحیح
tgetnum(نویسه *id)
{
    نویسه *ptr, buf[256];
    ptr = buf;

    اگر (tgetstr(id, &ptr))
	بازگشت atoi(buf);
    وگرنه
	بازگشت 0;
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

    نویسه *
tgetstr(نویسه *id, نویسه **buf)
{
    صحیح		len = strlen(id);
    نویسه	*tmp=tent;
    نویسه	*hold;
    صحیح		i;

    انجامبده
    {
	tmp = _find(tmp, ":");			// For each field
	وقتی (*tmp == ':')			// skip empty fields
	    tmp++;
	اگر (!*tmp)
	    بشکن;

	اگر (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    اگر (*tmp == '@')			// :xx@: entry for tc
		بازگشت 0;			// deleted entry
	    hold= *buf;
	    وقتی (*++tmp && *tmp != ':')	// not at end of field
		{
		انتخاب(*tmp)
		{
		حالت '\\':			// Expand escapes here
		    انتخاب(*++tmp)
		    {
		    حالت 0:			// ignore backslashes
			tmp--;			// at end of entry
			بشکن;			// shouldn't happen
		    حالت 'e':
		    حالت 'E':			// ESC
			*(*buf)++ = ESC;
			بشکن;
		    حالت 'n':			// \n
			*(*buf)++ = '\n';
			بشکن;
		    حالت 'r':			// \r
			*(*buf)++ = '\r';
			بشکن;
		    حالت 't':			// \t
			*(*buf)++ = '\t';
			بشکن;
		    حالت 'b':			// \b
			*(*buf)++ = '\b';
			بشکن;
		    حالت 'f':			// \f
			*(*buf)++ = '\f';
			بشکن;
		    حالت '0':			// \nnn
		    حالت '1':
		    حالت '2':
		    حالت '3':
		    حالت '4':
		    حالت '5':
		    حالت '6':
		    حالت '7':
		    حالت '8':
		    حالت '9':
			**buf = 0;
			    // get up to three digits
			برای (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			بشکن;
		    پیشفرض:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    بشکن;
		حالت '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    بشکن;
		پیشفرض:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    بازگشت hold;
	}
    } وقتی (*tmp);

    بازگشت 0;
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

    نویسه *
tgoto(
    نویسه    *cm,				// cm string, from termcap
    صحیح	    col,				// column, x position
    صحیح	    سطر)				// line, y position
{
    نویسه    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ایستا نویسه buffer[32];

    اگر (!cm)
	بازگشت "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    وقتی (*ptr)
    {
	اگر ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	وگرنه
	{				// % escape
	    انتخاب(c = *ptr++)
	    {
	    حالت 'd':				// decimal
		bufp = _addfmt(bufp, "%d", سطر);
		سطر = col;
		بشکن;
	    حالت '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", سطر);
		سطر = col;
		بشکن;
	    حالت '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", سطر);
		سطر = col;
		بشکن;
	    حالت '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		اگر (col>gx) col += gy;
		اگر (سطر>gx) سطر += gy;
		بشکن;
	    حالت '+':				// %+c: add c
		سطر += *ptr++;
	    حالت '.':				// print x/y
		اگر (سطر == '\t' ||		// these are
		   سطر == '\n' ||		// chars that
		   سطر == '\004' ||		// UNIX hates
		   سطر == '\0')
		{
		    سطر++;			// so go to next pos
		    اگر (reverse == (سطر == col))
			addup=1;		// and mark UP
		    وگرنه
			addbak=1;		// or BC
		}
		*bufp++=سطر;
		سطر = col;
		بشکن;
	    حالت 'r':				// r: reverse
		gx = سطر;
		سطر = col;
		col = gx;
		reverse = 1;
		بشکن;
	    حالت 'i':			// increment (1-origin screen)
		col++;
		سطر++;
		بشکن;
	    حالت '%':				// %%=% literally
		*bufp++='%';
		بشکن;
	    حالت 'n':				// magic DM2500 code
		سطر ^= 0140;
		col ^= 0140;
		بشکن;
	    حالت 'B':				// bcd encoding
		سطر = سطر/10<<4+سطر%10;
		col = col/10<<4+col%10;
		بشکن;
	    حالت 'D':				// magic Delta Data code
		سطر = سطر-2*(سطر&15);
		col = col-2*(col&15);
		بشکن;
	    پیشفرض:				// Unknown escape
		بازگشت "OOPS";
	    }
	}
    }

    اگر (addup)					// add upline
	اگر (UP)
	{
	    ptr=UP;
	    وقتی (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    اگر (*ptr == '*')
		ptr++;
	    وقتی (*ptr)
		*bufp++ = *ptr++;
	}

    اگر (addbak)					// add backspace
	اگر (BC)
	{
	    ptr=BC;
	    وقتی (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    اگر (*ptr == '*')
		ptr++;
	    وقتی (*ptr)
		*bufp++ = *ptr++;
	}
	وگرنه
	    *bufp++='\b';

    *bufp = 0;

    بازگشت(buffer);
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

    صحیح
tputs(
    نویسه *cp,				// string to print
    صحیح affcnt,				// Number of lines affected
    تهی (*outc)(unsigned صحیح))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ثابت نویسه *);

    اگر (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	وقتی (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	اگر (*cp == '.')
	    وقتی (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	اگر (*cp!='*')			// multiply by affected lines
	{
	    اگر (affcnt>1) affcnt = 1;
	}
	وگرنه
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	اگر (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	وقتی (*cp)			// output string
	    (*outc)(*cp++);
	اگر (ospeed)
	    وقتی (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    وگرنه
	وقتی (*cp)
	    (*outc)(*cp++);
    بازگشت 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ایستا صحیح
_match(نویسه *s1, نویسه *s2)
{
    صحیح i = 0;

    وقتی (s1[i] && s1[i] == s2[i])
	i++;

    بازگشت i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ایستا نویسه *
_find(نویسه *s, نویسه *set)
{
    برای (; *s; s++)
    {
	نویسه	*ptr = set;

	وقتی (*ptr && *s != *ptr)
	    ptr++;

	اگر (*ptr)
	    بازگشت s;
    }

    بازگشت s;
}

/*
 * add val to buf according to format fmt
 */
    ایستا نویسه *
_addfmt(نویسه *buf, نویسه *fmt, صحیح val)
{
    sprintf(buf, fmt, val);
    وقتی (*buf)
	buf++;
    بازگشت buf;
}
