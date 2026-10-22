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

#जर !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

स्थिर पूर्णांक  getent(अक्षर *, अक्षर *, खाता *, पूर्णांक);
स्थिर पूर्णांक  nextent(अक्षर *, खाता *, पूर्णांक);
स्थिर पूर्णांक  _match(अक्षर *, अक्षर *);
स्थिर अक्षर *_addfmt(अक्षर *, अक्षर *, पूर्णांक);
स्थिर अक्षर *_find(अक्षर *, अक्षर *);

/*
 * Global variables for termlib
 */

अक्षर	*tent;		      // Pointer to terminal entry, set by tgetent
अक्षर	PC = 0;		      // Pad character, default NULL
अक्षर	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
लघु	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

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

    पूर्णांक
tgetent(
    अक्षर    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    अक्षर    *term)		// Name of terminal
{
    अक्षर    tcbuf[32];		// Temp buffer to handle
    अक्षर    *tcptr = tcbuf;	// extended entries
    अक्षर    *tcap = TERMCAPFILE; // Default termcap file
    अक्षर    *tmp;
    खाता    *termcap;
    पूर्णांक	    retval = 0;
    पूर्णांक	    len;

    जर ((tmp = (अक्षर *)mch_getenv((char_u *)"TERMCAP")) != रिक्त)
    {
	जर (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#जर defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = म_अक्षर(tcap, '/');
	    जर (tmp)
		*tmp = ':';
#endif
	}
	अन्यथा				// TERMCAP = termcap entry itself
	{
	    पूर्णांक tlen = म_माप(term);

	    पर्यंत (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		अक्षर *nexttmp;

		पर्यंत (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		जर (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    परत 1;
		}
		अन्यथा
		    tmp = nexttmp;
	    }
	}
    }
    जर (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	परत -1;
    }

    len = 0;
    पर्यंत (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	जर ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    शुरुआत(termcap);
	    len = म_माप(tbuf);
	}
	अन्यथा
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    थांबा;
	}
    }
    ख_बंद(termcap);
    परत retval;
}

    स्थिर पूर्णांक
getent(अक्षर *tbuf, अक्षर *term, खाता *termcap, पूर्णांक buflen)
{
    अक्षर    *tptr;
    पूर्णांक	    tlen = म_माप(term);

    पर्यंत (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	पर्यंत (*tptr && *tptr != ':')		// : terminates name field
	{
	    अक्षर    *nexttptr;

	    पर्यंत (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    जर (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		परत 1;
	    }
	    अन्यथा				// Look for next name
		tptr = nexttptr;
	}
    }
    परत 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    स्थिर पूर्णांक
nextent(अक्षर *tbuf, खाता *termcap, पूर्णांक buflen)
{
    अक्षर *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    पर्यंत (lbuf < tbuf+buflen &&		// There's room and
	  ख_माला_लो(lbuf, (पूर्णांक)(tbuf+buflen-lbuf), termcap)) // another line
    {
	पूर्णांक llen = म_माप(lbuf);

	जर (*lbuf == '#')			// eat comments
	    सुरूठेवा;
	जर (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	जर (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	अन्यथा
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    परत 1;
	}
    }

    परत 0;					// ran into end of file
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

    पूर्णांक
tgetflag(अक्षर *id)
{
    अक्षर    buf[256], *ptr = buf;

    परत tgetstr(id, &ptr) ? 1 : 0;
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

    पूर्णांक
tgetnum(अक्षर *id)
{
    अक्षर *ptr, buf[256];
    ptr = buf;

    जर (tgetstr(id, &ptr))
	परत म_से_प(buf);
    अन्यथा
	परत 0;
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

    अक्षर *
tgetstr(अक्षर *id, अक्षर **buf)
{
    पूर्णांक		len = म_माप(id);
    अक्षर	*tmp=tent;
    अक्षर	*hold;
    पूर्णांक		i;

    करा
    {
	tmp = _find(tmp, ":");			// For each field
	पर्यंत (*tmp == ':')			// skip empty fields
	    tmp++;
	जर (!*tmp)
	    थांबा;

	जर (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    जर (*tmp == '@')			// :xx@: entry for tc
		परत 0;			// deleted entry
	    hold= *buf;
	    पर्यंत (*++tmp && *tmp != ':')	// not at end of field
		{
		निवड(*tmp)
		{
		स्थिती '\\':			// Expand escapes here
		    निवड(*++tmp)
		    {
		    स्थिती 0:			// ignore backslashes
			tmp--;			// at end of entry
			थांबा;			// shouldn't happen
		    स्थिती 'e':
		    स्थिती 'E':			// ESC
			*(*buf)++ = ESC;
			थांबा;
		    स्थिती 'n':			// \n
			*(*buf)++ = '\n';
			थांबा;
		    स्थिती 'r':			// \r
			*(*buf)++ = '\r';
			थांबा;
		    स्थिती 't':			// \t
			*(*buf)++ = '\t';
			थांबा;
		    स्थिती 'b':			// \b
			*(*buf)++ = '\b';
			थांबा;
		    स्थिती 'f':			// \f
			*(*buf)++ = '\f';
			थांबा;
		    स्थिती '0':			// \nnn
		    स्थिती '1':
		    स्थिती '2':
		    स्थिती '3':
		    स्थिती '4':
		    स्थिती '5':
		    स्थिती '6':
		    स्थिती '7':
		    स्थिती '8':
		    स्थिती '9':
			**buf = 0;
			    // get up to three digits
			साठी (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			थांबा;
		    पूर्वनिर्धारित:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    थांबा;
		स्थिती '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    थांबा;
		पूर्वनिर्धारित:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    परत hold;
	}
    } पर्यंत (*tmp);

    परत 0;
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

    अक्षर *
tgoto(
    अक्षर    *cm,				// cm string, from termcap
    पूर्णांक	    col,				// column, x position
    पूर्णांक	    ओळ)				// line, y position
{
    अक्षर    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    स्थिर अक्षर buffer[32];

    जर (!cm)
	परत "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    पर्यंत (*ptr)
    {
	जर ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	अन्यथा
	{				// % escape
	    निवड(c = *ptr++)
	    {
	    स्थिती 'd':				// decimal
		bufp = _addfmt(bufp, "%d", ओळ);
		ओळ = col;
		थांबा;
	    स्थिती '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", ओळ);
		ओळ = col;
		थांबा;
	    स्थिती '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", ओळ);
		ओळ = col;
		थांबा;
	    स्थिती '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		जर (col>gx) col += gy;
		जर (ओळ>gx) ओळ += gy;
		थांबा;
	    स्थिती '+':				// %+c: add c
		ओळ += *ptr++;
	    स्थिती '.':				// print x/y
		जर (ओळ == '\t' ||		// these are
		   ओळ == '\n' ||		// chars that
		   ओळ == '\004' ||		// UNIX hates
		   ओळ == '\0')
		{
		    ओळ++;			// so go to next pos
		    जर (reverse == (ओळ == col))
			addup=1;		// and mark UP
		    अन्यथा
			addbak=1;		// or BC
		}
		*bufp++=ओळ;
		ओळ = col;
		थांबा;
	    स्थिती 'r':				// r: reverse
		gx = ओळ;
		ओळ = col;
		col = gx;
		reverse = 1;
		थांबा;
	    स्थिती 'i':			// increment (1-origin screen)
		col++;
		ओळ++;
		थांबा;
	    स्थिती '%':				// %%=% literally
		*bufp++='%';
		थांबा;
	    स्थिती 'n':				// magic DM2500 code
		ओळ ^= 0140;
		col ^= 0140;
		थांबा;
	    स्थिती 'B':				// bcd encoding
		ओळ = ओळ/10<<4+ओळ%10;
		col = col/10<<4+col%10;
		थांबा;
	    स्थिती 'D':				// magic Delta Data code
		ओळ = ओळ-2*(ओळ&15);
		col = col-2*(col&15);
		थांबा;
	    पूर्वनिर्धारित:				// Unknown escape
		परत "OOPS";
	    }
	}
    }

    जर (addup)					// add upline
	जर (UP)
	{
	    ptr=UP;
	    पर्यंत (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    जर (*ptr == '*')
		ptr++;
	    पर्यंत (*ptr)
		*bufp++ = *ptr++;
	}

    जर (addbak)					// add backspace
	जर (BC)
	{
	    ptr=BC;
	    पर्यंत (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    जर (*ptr == '*')
		ptr++;
	    पर्यंत (*ptr)
		*bufp++ = *ptr++;
	}
	अन्यथा
	    *bufp++='\b';

    *bufp = 0;

    परत(buffer);
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

दीर्घ _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    पूर्णांक
tputs(
    अक्षर *cp,				// string to print
    पूर्णांक affcnt,				// Number of lines affected
    शून्य (*outc)(अचिन्हित पूर्णांक))		// routine to output 1 character
{
    दीर्घ    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	म_से_द(स्थिरांक अक्षर *);

    जर (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	पर्यंत (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (दीर्घ)(*cp++ - '0');
	जर (*cp == '.')
	    पर्यंत (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (दीर्घ)(*cp++ - '0');
		frac = frac * 10;
	    }
	जर (*cp!='*')			// multiply by affected lines
	{
	    जर (affcnt>1) affcnt = 1;
	}
	अन्यथा
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	जर (ospeed)
	    counter = (counter * _bauds[ospeed] * (दीर्घ)affcnt) / frac;

	पर्यंत (*cp)			// output string
	    (*outc)(*cp++);
	जर (ospeed)
	    पर्यंत (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    अन्यथा
	पर्यंत (*cp)
	    (*outc)(*cp++);
    परत 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    स्थिर पूर्णांक
_match(अक्षर *s1, अक्षर *s2)
{
    पूर्णांक i = 0;

    पर्यंत (s1[i] && s1[i] == s2[i])
	i++;

    परत i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    स्थिर अक्षर *
_find(अक्षर *s, अक्षर *set)
{
    साठी (; *s; s++)
    {
	अक्षर	*ptr = set;

	पर्यंत (*ptr && *s != *ptr)
	    ptr++;

	जर (*ptr)
	    परत s;
    }

    परत s;
}

/*
 * add val to buf according to format fmt
 */
    स्थिर अक्षर *
_addfmt(अक्षर *buf, अक्षर *fmt, पूर्णांक val)
{
    प्र_लिखो(buf, fmt, val);
    पर्यंत (*buf)
	buf++;
    परत buf;
}
