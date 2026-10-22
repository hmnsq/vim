/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "termlib.pro"

#எனில் !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

நிலையான முழு  getent(எழுத்து *, எழுத்து *, FILE *, முழு);
நிலையான முழு  nextent(எழுத்து *, FILE *, முழு);
நிலையான முழு  _match(எழுத்து *, எழுத்து *);
நிலையான எழுத்து *_addfmt(எழுத்து *, எழுத்து *, முழு);
நிலையான எழுத்து *_find(எழுத்து *, எழுத்து *);

/*
 * Global variables for termlib
 */

எழுத்து	*tent;		      // Pointer to terminal entry, set by tgetent
எழுத்து	PC = 0;		      // Pad character, default NULL
எழுத்து	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# இல்லையெனில்
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  இல்லையெனில்
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    முழு
tgetent(
    எழுத்து    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    எழுத்து    *term)		// Name of terminal
{
    எழுத்து    tcbuf[32];		// Temp buffer to handle
    எழுத்து    *tcptr = tcbuf;	// extended entries
    எழுத்து    *tcap = TERMCAPFILE; // Default termcap file
    எழுத்து    *tmp;
    FILE    *termcap;
    முழு	    retval = 0;
    முழு	    len;

    எனில் ((tmp = (எழுத்து *)mch_getenv((char_u *)"TERMCAP")) != வெற்று)
    {
	எனில் (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#எனில் defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    எனில் (tmp)
		*tmp = ':';
#endif
	}
	இல்லையெனில்				// TERMCAP = termcap entry itself
	{
	    முழு tlen = strlen(term);

	    போது (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		எழுத்து *nexttmp;

		போது (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		எனில் (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    திரும்பு 1;
		}
		இல்லையெனில்
		    tmp = nexttmp;
	    }
	}
    }
    எனில் (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	திரும்பு -1;
    }

    len = 0;
    போது (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	எனில் ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	இல்லையெனில்
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    நிறுத்து;
	}
    }
    fclose(termcap);
    திரும்பு retval;
}

    நிலையான முழு
getent(எழுத்து *tbuf, எழுத்து *term, FILE *termcap, முழு buflen)
{
    எழுத்து    *tptr;
    முழு	    tlen = strlen(term);

    போது (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	போது (*tptr && *tptr != ':')		// : terminates name field
	{
	    எழுத்து    *nexttptr;

	    போது (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    எனில் (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		திரும்பு 1;
	    }
	    இல்லையெனில்				// Look for next name
		tptr = nexttptr;
	}
    }
    திரும்பு 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    நிலையான முழு
nextent(எழுத்து *tbuf, FILE *termcap, முழு buflen)
{
    எழுத்து *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    போது (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (முழு)(tbuf+buflen-lbuf), termcap)) // another line
    {
	முழு llen = strlen(lbuf);

	எனில் (*lbuf == '#')			// eat comments
	    தொடர்;
	எனில் (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	எனில் (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	இல்லையெனில்
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    திரும்பு 1;
	}
    }

    திரும்பு 0;					// ran into end of file
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

    முழு
tgetflag(எழுத்து *id)
{
    எழுத்து    buf[256], *ptr = buf;

    திரும்பு tgetstr(id, &ptr) ? 1 : 0;
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

    முழு
tgetnum(எழுத்து *id)
{
    எழுத்து *ptr, buf[256];
    ptr = buf;

    எனில் (tgetstr(id, &ptr))
	திரும்பு atoi(buf);
    இல்லையெனில்
	திரும்பு 0;
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

    எழுத்து *
tgetstr(எழுத்து *id, எழுத்து **buf)
{
    முழு		len = strlen(id);
    எழுத்து	*tmp=tent;
    எழுத்து	*hold;
    முழு		i;

    செய்
    {
	tmp = _find(tmp, ":");			// For each field
	போது (*tmp == ':')			// skip empty fields
	    tmp++;
	எனில் (!*tmp)
	    நிறுத்து;

	எனில் (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    எனில் (*tmp == '@')			// :xx@: entry for tc
		திரும்பு 0;			// deleted entry
	    hold= *buf;
	    போது (*++tmp && *tmp != ':')	// not at end of field
		{
		தேர்வு(*tmp)
		{
		நிலை '\\':			// Expand escapes here
		    தேர்வு(*++tmp)
		    {
		    நிலை 0:			// ignore backslashes
			tmp--;			// at end of entry
			நிறுத்து;			// shouldn't happen
		    நிலை 'e':
		    நிலை 'E':			// ESC
			*(*buf)++ = ESC;
			நிறுத்து;
		    நிலை 'n':			// \n
			*(*buf)++ = '\n';
			நிறுத்து;
		    நிலை 'r':			// \r
			*(*buf)++ = '\r';
			நிறுத்து;
		    நிலை 't':			// \t
			*(*buf)++ = '\t';
			நிறுத்து;
		    நிலை 'b':			// \b
			*(*buf)++ = '\b';
			நிறுத்து;
		    நிலை 'f':			// \f
			*(*buf)++ = '\f';
			நிறுத்து;
		    நிலை '0':			// \nnn
		    நிலை '1':
		    நிலை '2':
		    நிலை '3':
		    நிலை '4':
		    நிலை '5':
		    நிலை '6':
		    நிலை '7':
		    நிலை '8':
		    நிலை '9':
			**buf = 0;
			    // get up to three digits
			க்கு (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			நிறுத்து;
		    இயல்பு:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    நிறுத்து;
		நிலை '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    நிறுத்து;
		இயல்பு:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    திரும்பு hold;
	}
    } போது (*tmp);

    திரும்பு 0;
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

    எழுத்து *
tgoto(
    எழுத்து    *cm,				// cm string, from termcap
    முழு	    col,				// column, x position
    முழு	    வரி)				// line, y position
{
    எழுத்து    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    நிலையான எழுத்து buffer[32];

    எனில் (!cm)
	திரும்பு "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    போது (*ptr)
    {
	எனில் ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	இல்லையெனில்
	{				// % escape
	    தேர்வு(c = *ptr++)
	    {
	    நிலை 'd':				// decimal
		bufp = _addfmt(bufp, "%d", வரி);
		வரி = col;
		நிறுத்து;
	    நிலை '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", வரி);
		வரி = col;
		நிறுத்து;
	    நிலை '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", வரி);
		வரி = col;
		நிறுத்து;
	    நிலை '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		எனில் (col>gx) col += gy;
		எனில் (வரி>gx) வரி += gy;
		நிறுத்து;
	    நிலை '+':				// %+c: add c
		வரி += *ptr++;
	    நிலை '.':				// print x/y
		எனில் (வரி == '\t' ||		// these are
		   வரி == '\n' ||		// chars that
		   வரி == '\004' ||		// UNIX hates
		   வரி == '\0')
		{
		    வரி++;			// so go to next pos
		    எனில் (reverse == (வரி == col))
			addup=1;		// and mark UP
		    இல்லையெனில்
			addbak=1;		// or BC
		}
		*bufp++=வரி;
		வரி = col;
		நிறுத்து;
	    நிலை 'r':				// r: reverse
		gx = வரி;
		வரி = col;
		col = gx;
		reverse = 1;
		நிறுத்து;
	    நிலை 'i':			// increment (1-origin screen)
		col++;
		வரி++;
		நிறுத்து;
	    நிலை '%':				// %%=% literally
		*bufp++='%';
		நிறுத்து;
	    நிலை 'n':				// magic DM2500 code
		வரி ^= 0140;
		col ^= 0140;
		நிறுத்து;
	    நிலை 'B':				// bcd encoding
		வரி = வரி/10<<4+வரி%10;
		col = col/10<<4+col%10;
		நிறுத்து;
	    நிலை 'D':				// magic Delta Data code
		வரி = வரி-2*(வரி&15);
		col = col-2*(col&15);
		நிறுத்து;
	    இயல்பு:				// Unknown escape
		திரும்பு "OOPS";
	    }
	}
    }

    எனில் (addup)					// add upline
	எனில் (UP)
	{
	    ptr=UP;
	    போது (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    எனில் (*ptr == '*')
		ptr++;
	    போது (*ptr)
		*bufp++ = *ptr++;
	}

    எனில் (addbak)					// add backspace
	எனில் (BC)
	{
	    ptr=BC;
	    போது (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    எனில் (*ptr == '*')
		ptr++;
	    போது (*ptr)
		*bufp++ = *ptr++;
	}
	இல்லையெனில்
	    *bufp++='\b';

    *bufp = 0;

    திரும்பு(buffer);
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

    முழு
tputs(
    எழுத்து *cp,				// string to print
    முழு affcnt,				// Number of lines affected
    வெறுமை (*outc)(unsigned முழு))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(மாறிலி எழுத்து *);

    எனில் (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	போது (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	எனில் (*cp == '.')
	    போது (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	எனில் (*cp!='*')			// multiply by affected lines
	{
	    எனில் (affcnt>1) affcnt = 1;
	}
	இல்லையெனில்
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	எனில் (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	போது (*cp)			// output string
	    (*outc)(*cp++);
	எனில் (ospeed)
	    போது (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    இல்லையெனில்
	போது (*cp)
	    (*outc)(*cp++);
    திரும்பு 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    நிலையான முழு
_match(எழுத்து *s1, எழுத்து *s2)
{
    முழு i = 0;

    போது (s1[i] && s1[i] == s2[i])
	i++;

    திரும்பு i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    நிலையான எழுத்து *
_find(எழுத்து *s, எழுத்து *set)
{
    க்கு (; *s; s++)
    {
	எழுத்து	*ptr = set;

	போது (*ptr && *s != *ptr)
	    ptr++;

	எனில் (*ptr)
	    திரும்பு s;
    }

    திரும்பு s;
}

/*
 * add val to buf according to format fmt
 */
    நிலையான எழுத்து *
_addfmt(எழுத்து *buf, எழுத்து *fmt, முழு val)
{
    sprintf(buf, fmt, val);
    போது (*buf)
	buf++;
    திரும்பு buf;
}
