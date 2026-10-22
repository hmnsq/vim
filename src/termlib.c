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

#सचे !defined(AMIGA) && !defined(VMS)
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
# अञ्ञथा
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  अञ्ञथा
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

    सचे ((tmp = (वर्ण *)mch_getenv((char_u *)"TERMCAP")) != रिक्त)
    {
	सचे (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#सचे defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    सचे (tmp)
		*tmp = ':';
#endif
	}
	अञ्ञथा				// TERMCAP = termcap entry itself
	{
	    पूर्णाङ्क tlen = strlen(term);

	    याव (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		वर्ण *nexttmp;

		याव (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		सचे (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    पटिनिवत्त 1;
		}
		अञ्ञथा
		    tmp = nexttmp;
	    }
	}
    }
    सचे (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	पटिनिवत्त -1;
    }

    len = 0;
    याव (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	सचे ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	अञ्ञथा
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    भङ्ग;
	}
    }
    fclose(termcap);
    पटिनिवत्त retval;
}

    स्थिर पूर्णाङ्क
getent(वर्ण *tbuf, वर्ण *term, FILE *termcap, पूर्णाङ्क buflen)
{
    वर्ण    *tptr;
    पूर्णाङ्क	    tlen = strlen(term);

    याव (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	याव (*tptr && *tptr != ':')		// : terminates name field
	{
	    वर्ण    *nexttptr;

	    याव (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    सचे (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		पटिनिवत्त 1;
	    }
	    अञ्ञथा				// Look for next name
		tptr = nexttptr;
	}
    }
    पटिनिवत्त 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    स्थिर पूर्णाङ्क
nextent(वर्ण *tbuf, FILE *termcap, पूर्णाङ्क buflen)
{
    वर्ण *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    याव (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (पूर्णाङ्क)(tbuf+buflen-lbuf), termcap)) // another line
    {
	पूर्णाङ्क llen = strlen(lbuf);

	सचे (*lbuf == '#')			// eat comments
	    अनुवर्त;
	सचे (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	सचे (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	अञ्ञथा
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    पटिनिवत्त 1;
	}
    }

    पटिनिवत्त 0;					// ran into end of file
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

    पटिनिवत्त tgetstr(id, &ptr) ? 1 : 0;
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

    सचे (tgetstr(id, &ptr))
	पटिनिवत्त atoi(buf);
    अञ्ञथा
	पटिनिवत्त 0;
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

    करोहि
    {
	tmp = _find(tmp, ":");			// For each field
	याव (*tmp == ':')			// skip empty fields
	    tmp++;
	सचे (!*tmp)
	    भङ्ग;

	सचे (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    सचे (*tmp == '@')			// :xx@: entry for tc
		पटिनिवत्त 0;			// deleted entry
	    hold= *buf;
	    याव (*++tmp && *tmp != ':')	// not at end of field
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
			अत्थाय (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
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
	    पटिनिवत्त hold;
	}
    } याव (*tmp);

    पटिनिवत्त 0;
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
    पूर्णाङ्क	    पङ्क्तिः)				// line, y position
{
    वर्ण    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    स्थिर वर्ण buffer[32];

    सचे (!cm)
	पटिनिवत्त "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    याव (*ptr)
    {
	सचे ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	अञ्ञथा
	{				// % escape
	    विकल्प(c = *ptr++)
	    {
	    पक्ष 'd':				// decimal
		bufp = _addfmt(bufp, "%d", पङ्क्तिः);
		पङ्क्तिः = col;
		भङ्ग;
	    पक्ष '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", पङ्क्तिः);
		पङ्क्तिः = col;
		भङ्ग;
	    पक्ष '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", पङ्क्तिः);
		पङ्क्तिः = col;
		भङ्ग;
	    पक्ष '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		सचे (col>gx) col += gy;
		सचे (पङ्क्तिः>gx) पङ्क्तिः += gy;
		भङ्ग;
	    पक्ष '+':				// %+c: add c
		पङ्क्तिः += *ptr++;
	    पक्ष '.':				// print x/y
		सचे (पङ्क्तिः == '\t' ||		// these are
		   पङ्क्तिः == '\n' ||		// chars that
		   पङ्क्तिः == '\004' ||		// UNIX hates
		   पङ्क्तिः == '\0')
		{
		    पङ्क्तिः++;			// so go to next pos
		    सचे (reverse == (पङ्क्तिः == col))
			addup=1;		// and mark UP
		    अञ्ञथा
			addbak=1;		// or BC
		}
		*bufp++=पङ्क्तिः;
		पङ्क्तिः = col;
		भङ्ग;
	    पक्ष 'r':				// r: reverse
		gx = पङ्क्तिः;
		पङ्क्तिः = col;
		col = gx;
		reverse = 1;
		भङ्ग;
	    पक्ष 'i':			// increment (1-origin screen)
		col++;
		पङ्क्तिः++;
		भङ्ग;
	    पक्ष '%':				// %%=% literally
		*bufp++='%';
		भङ्ग;
	    पक्ष 'n':				// magic DM2500 code
		पङ्क्तिः ^= 0140;
		col ^= 0140;
		भङ्ग;
	    पक्ष 'B':				// bcd encoding
		पङ्क्तिः = पङ्क्तिः/10<<4+पङ्क्तिः%10;
		col = col/10<<4+col%10;
		भङ्ग;
	    पक्ष 'D':				// magic Delta Data code
		पङ्क्तिः = पङ्क्तिः-2*(पङ्क्तिः&15);
		col = col-2*(col&15);
		भङ्ग;
	    स्वभाव:				// Unknown escape
		पटिनिवत्त "OOPS";
	    }
	}
    }

    सचे (addup)					// add upline
	सचे (UP)
	{
	    ptr=UP;
	    याव (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    सचे (*ptr == '*')
		ptr++;
	    याव (*ptr)
		*bufp++ = *ptr++;
	}

    सचे (addbak)					// add backspace
	सचे (BC)
	{
	    ptr=BC;
	    याव (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    सचे (*ptr == '*')
		ptr++;
	    याव (*ptr)
		*bufp++ = *ptr++;
	}
	अञ्ञथा
	    *bufp++='\b';

    *bufp = 0;

    पटिनिवत्त(buffer);
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

    सचे (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	याव (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	सचे (*cp == '.')
	    याव (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	सचे (*cp!='*')			// multiply by affected lines
	{
	    सचे (affcnt>1) affcnt = 1;
	}
	अञ्ञथा
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	सचे (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	याव (*cp)			// output string
	    (*outc)(*cp++);
	सचे (ospeed)
	    याव (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    अञ्ञथा
	याव (*cp)
	    (*outc)(*cp++);
    पटिनिवत्त 0;
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

    याव (s1[i] && s1[i] == s2[i])
	i++;

    पटिनिवत्त i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    स्थिर वर्ण *
_find(वर्ण *s, वर्ण *set)
{
    अत्थाय (; *s; s++)
    {
	वर्ण	*ptr = set;

	याव (*ptr && *s != *ptr)
	    ptr++;

	सचे (*ptr)
	    पटिनिवत्त s;
    }

    पटिनिवत्त s;
}

/*
 * add val to buf according to format fmt
 */
    स्थिर वर्ण *
_addfmt(वर्ण *buf, वर्ण *fmt, पूर्णाङ्क val)
{
    sprintf(buf, fmt, val);
    याव (*buf)
	buf++;
    पटिनिवत्त buf;
}
