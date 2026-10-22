/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#अन्तर्भाव "vim.h"
#अन्तर्भाव "termlib.pro"

#यदि !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

स्थिर पूर्णाङ्क  getent(वर्ण *, वर्ण *, FILE *, पूर्णाङ्क);
स्थिर पूर्णाङ्क  nextent(वर्ण *, FILE *, पूर्णाङ्क);
स्थिर पूर्णाङ्क  _match(वर्ण *, वर्ण *);
स्थिर वर्ण *_addfmt(वर्ण *, वर्ण *, पूर्णाङ्क);
स्थिर वर्ण *_find(वर्ण *, वर्ण *);

/*
 * Global variables for termlib
 */

वर्ण	*tent;		      // Pointer to terminal entry, set by tgetent
वर्ण	PC = 0;		      // Pad character, default NULL
वर्ण	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# अन्यथा
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  अन्यथा
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    पूर्णाङ्क
tgetent(
    वर्ण    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    वर्ण    *term)		// Name of terminal
{
    वर्ण    tcbuf[32];		// Temp buffer to handle
    वर्ण    *tcptr = tcbuf;	// extended entries
    वर्ण    *tcap = TERMCAPFILE; // Default termcap file
    वर्ण    *tmp;
    FILE    *termcap;
    पूर्णाङ्क	    retval = 0;
    पूर्णाङ्क	    len;

    यदि ((tmp = (वर्ण *)mch_getenv((char_u *)"TERMCAP")) != रिक्त)
    {
	यदि (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#यदि defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    यदि (tmp)
		*tmp = ':';
#endif
	}
	अन्यथा				// TERMCAP = termcap entry itself
	{
	    पूर्णाङ्क tlen = strlen(term);

	    यावत् (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		वर्ण *nexttmp;

		यावत् (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		यदि (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    प्रत्यागम 1;
		}
		अन्यथा
		    tmp = nexttmp;
	    }
	}
    }
    यदि (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	प्रत्यागम -1;
    }

    len = 0;
    यावत् (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	यदि ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	अन्यथा
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    भङ्ग;
	}
    }
    fclose(termcap);
    प्रत्यागम retval;
}

    स्थिर पूर्णाङ्क
getent(वर्ण *tbuf, वर्ण *term, FILE *termcap, पूर्णाङ्क buflen)
{
    वर्ण    *tptr;
    पूर्णाङ्क	    tlen = strlen(term);

    यावत् (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	यावत् (*tptr && *tptr != ':')		// : terminates name field
	{
	    वर्ण    *nexttptr;

	    यावत् (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    यदि (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		प्रत्यागम 1;
	    }
	    अन्यथा				// Look for next name
		tptr = nexttptr;
	}
    }
    प्रत्यागम 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    स्थिर पूर्णाङ्क
nextent(वर्ण *tbuf, FILE *termcap, पूर्णाङ्क buflen)
{
    वर्ण *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    यावत् (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (पूर्णाङ्क)(tbuf+buflen-lbuf), termcap)) // another line
    {
	पूर्णाङ्क llen = strlen(lbuf);

	यदि (*lbuf == '#')			// eat comments
	    अनुवर्त;
	यदि (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	यदि (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	अन्यथा
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    प्रत्यागम 1;
	}
    }

    प्रत्यागम 0;					// ran into end of file
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

    पूर्णाङ्क
tgetflag(वर्ण *id)
{
    वर्ण    buf[256], *ptr = buf;

    प्रत्यागम tgetstr(id, &ptr) ? 1 : 0;
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

    पूर्णाङ्क
tgetnum(वर्ण *id)
{
    वर्ण *ptr, buf[256];
    ptr = buf;

    यदि (tgetstr(id, &ptr))
	प्रत्यागम atoi(buf);
    अन्यथा
	प्रत्यागम 0;
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

    वर्ण *
tgetstr(वर्ण *id, वर्ण **buf)
{
    पूर्णाङ्क		len = strlen(id);
    वर्ण	*tmp=tent;
    वर्ण	*hold;
    पूर्णाङ्क		i;

    कुरु
    {
	tmp = _find(tmp, ":");			// For each field
	यावत् (*tmp == ':')			// skip empty fields
	    tmp++;
	यदि (!*tmp)
	    भङ्ग;

	यदि (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    यदि (*tmp == '@')			// :xx@: entry for tc
		प्रत्यागम 0;			// deleted entry
	    hold= *buf;
	    यावत् (*++tmp && *tmp != ':')	// not at end of field
		{
		विकल्प(*tmp)
		{
		पक्ष '\\':			// Expand escapes here
		    विकल्प(*++tmp)
		    {
		    पक्ष 0:			// ignore backslashes
			tmp--;			// at end of entry
			भङ्ग;			// shouldn't happen
		    पक्ष 'e':
		    पक्ष 'E':			// ESC
			*(*buf)++ = ESC;
			भङ्ग;
		    पक्ष 'n':			// \n
			*(*buf)++ = '\n';
			भङ्ग;
		    पक्ष 'r':			// \r
			*(*buf)++ = '\r';
			भङ्ग;
		    पक्ष 't':			// \t
			*(*buf)++ = '\t';
			भङ्ग;
		    पक्ष 'b':			// \b
			*(*buf)++ = '\b';
			भङ्ग;
		    पक्ष 'f':			// \f
			*(*buf)++ = '\f';
			भङ्ग;
		    पक्ष '0':			// \nnn
		    पक्ष '1':
		    पक्ष '2':
		    पक्ष '3':
		    पक्ष '4':
		    पक्ष '5':
		    पक्ष '6':
		    पक्ष '7':
		    पक्ष '8':
		    पक्ष '9':
			**buf = 0;
			    // get up to three digits
			प्रति (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			भङ्ग;
		    स्वभाव:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    भङ्ग;
		पक्ष '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    भङ्ग;
		स्वभाव:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    प्रत्यागम hold;
	}
    } यावत् (*tmp);

    प्रत्यागम 0;
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

    वर्ण *
tgoto(
    वर्ण    *cm,				// cm string, from termcap
    पूर्णाङ्क	    col,				// column, x position
    पूर्णाङ्क	    पंक्ति)				// line, y position
{
    वर्ण    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    स्थिर वर्ण buffer[32];

    यदि (!cm)
	प्रत्यागम "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    यावत् (*ptr)
    {
	यदि ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	अन्यथा
	{				// % escape
	    विकल्प(c = *ptr++)
	    {
	    पक्ष 'd':				// decimal
		bufp = _addfmt(bufp, "%d", पंक्ति);
		पंक्ति = col;
		भङ्ग;
	    पक्ष '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", पंक्ति);
		पंक्ति = col;
		भङ्ग;
	    पक्ष '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", पंक्ति);
		पंक्ति = col;
		भङ्ग;
	    पक्ष '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		यदि (col>gx) col += gy;
		यदि (पंक्ति>gx) पंक्ति += gy;
		भङ्ग;
	    पक्ष '+':				// %+c: add c
		पंक्ति += *ptr++;
	    पक्ष '.':				// print x/y
		यदि (पंक्ति == '\t' ||		// these are
		   पंक्ति == '\n' ||		// chars that
		   पंक्ति == '\004' ||		// UNIX hates
		   पंक्ति == '\0')
		{
		    पंक्ति++;			// so go to next pos
		    यदि (reverse == (पंक्ति == col))
			addup=1;		// and mark UP
		    अन्यथा
			addbak=1;		// or BC
		}
		*bufp++=पंक्ति;
		पंक्ति = col;
		भङ्ग;
	    पक्ष 'r':				// r: reverse
		gx = पंक्ति;
		पंक्ति = col;
		col = gx;
		reverse = 1;
		भङ्ग;
	    पक्ष 'i':			// increment (1-origin screen)
		col++;
		पंक्ति++;
		भङ्ग;
	    पक्ष '%':				// %%=% literally
		*bufp++='%';
		भङ्ग;
	    पक्ष 'n':				// magic DM2500 code
		पंक्ति ^= 0140;
		col ^= 0140;
		भङ्ग;
	    पक्ष 'B':				// bcd encoding
		पंक्ति = पंक्ति/10<<4+पंक्ति%10;
		col = col/10<<4+col%10;
		भङ्ग;
	    पक्ष 'D':				// magic Delta Data code
		पंक्ति = पंक्ति-2*(पंक्ति&15);
		col = col-2*(col&15);
		भङ्ग;
	    स्वभाव:				// Unknown escape
		प्रत्यागम "OOPS";
	    }
	}
    }

    यदि (addup)					// add upline
	यदि (UP)
	{
	    ptr=UP;
	    यावत् (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    यदि (*ptr == '*')
		ptr++;
	    यावत् (*ptr)
		*bufp++ = *ptr++;
	}

    यदि (addbak)					// add backspace
	यदि (BC)
	{
	    ptr=BC;
	    यावत् (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    यदि (*ptr == '*')
		ptr++;
	    यावत् (*ptr)
		*bufp++ = *ptr++;
	}
	अन्यथा
	    *bufp++='\b';

    *bufp = 0;

    प्रत्यागम(buffer);
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

    पूर्णाङ्क
tputs(
    वर्ण *cp,				// string to print
    पूर्णाङ्क affcnt,				// Number of lines affected
    शून्य (*outc)(unsigned पूर्णाङ्क))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ध्रुव वर्ण *);

    यदि (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	यावत् (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	यदि (*cp == '.')
	    यावत् (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	यदि (*cp!='*')			// multiply by affected lines
	{
	    यदि (affcnt>1) affcnt = 1;
	}
	अन्यथा
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	यदि (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	यावत् (*cp)			// output string
	    (*outc)(*cp++);
	यदि (ospeed)
	    यावत् (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    अन्यथा
	यावत् (*cp)
	    (*outc)(*cp++);
    प्रत्यागम 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    स्थिर पूर्णाङ्क
_match(वर्ण *s1, वर्ण *s2)
{
    पूर्णाङ्क i = 0;

    यावत् (s1[i] && s1[i] == s2[i])
	i++;

    प्रत्यागम i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    स्थिर वर्ण *
_find(वर्ण *s, वर्ण *set)
{
    प्रति (; *s; s++)
    {
	वर्ण	*ptr = set;

	यावत् (*ptr && *s != *ptr)
	    ptr++;

	यदि (*ptr)
	    प्रत्यागम s;
    }

    प्रत्यागम s;
}

/*
 * add val to buf according to format fmt
 */
    स्थिर वर्ण *
_addfmt(वर्ण *buf, वर्ण *fmt, पूर्णाङ्क val)
{
    sprintf(buf, fmt, val);
    यावत् (*buf)
	buf++;
    प्रत्यागम buf;
}
