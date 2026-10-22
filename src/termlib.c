/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#అన్తర్భావ "vim.h"
#అన్తర్భావ "termlib.pro"

#ఒకవేళ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#అయితే_ముగింపు

స్థిర పూర్ణాంకం  getent(అక్షరం *, అక్షరం *, FILE *, పూర్ణాంకం);
స్థిర పూర్ణాంకం  nextent(అక్షరం *, FILE *, పూర్ణాంకం);
స్థిర పూర్ణాంకం  సరిపోలిక_3(అక్షరం *, అక్షరం *);
స్థిర అక్షరం *_addfmt(అక్షరం *, అక్షరం *, పూర్ణాంకం);
స్థిర అక్షరం *కనుగొను_3(అక్షరం *, అక్షరం *);

/*
 * Global variables for termlib
 */

అక్షరం	*tent;		      // Pointer to terminal entry, set by tgetent
అక్షరం	PC = 0;		      // Pad character, default NULL
అక్షరం	*పైకి_2 = 0, *BC = 0;     // Pointers to UP and BC strings from database
పొట్టి	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

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

#నిర్వచితం_కాకపోతే TERMCAPFILE
# ifdef AMIGA
#  define TERMCAPFILE "s:termcap"
# లేకపోతే
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  లేకపోతే
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#అయితే_ముగింపు

    పూర్ణాంకం
tgetent(
    అక్షరం    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    అక్షరం    *పదం)		// Name of terminal
{
    అక్షరం    tcbuf[32];		// Temp buffer to handle
    అక్షరం    *tcptr = tcbuf;	// extended entries
    అక్షరం    *tcap = TERMCAPFILE; // Default termcap file
    అక్షరం    *tmp;
    FILE    *termcap;
    పూర్ణాంకం	    retval = 0;
    పూర్ణాంకం	    len;

    ఒకవేళ ((tmp = (అక్షరం *)mch_getenv((char_u *)"TERMCAP")) != ఖాళీ)
    {
	ఒకవేళ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ఒకవేళ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ఒకవేళ (tmp)
		*tmp = ':';
#అయితే_ముగింపు
	}
	లేకపోతే				// TERMCAP = termcap entry itself
	{
	    పూర్ణాంకం tlen = అక్షరశ్రేణి_పొడవు(పదం);

	    అయితే (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		అక్షరం *nexttmp;

		అయితే (*tmp == '|')
		    tmp++;
		nexttmp  = కనుగొను_3(tmp, ":|");	// Rhialto
		ఒకవేళ (tmp+tlen == nexttmp && సరిపోలిక_3(tmp, పదం) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    తిరిగి 1;
		}
		లేకపోతే
		    tmp = nexttmp;
	    }
	}
    }
    ఒకవేళ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	తిరిగి -1;
    }

    len = 0;
    అయితే (getent(tbuf + len, పదం, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ఒకవేళ ((పదం = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = అక్షరశ్రేణి_పొడవు(tbuf);
	}
	లేకపోతే
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    ఆపు;
	}
    }
    fclose(termcap);
    తిరిగి retval;
}

    స్థిర పూర్ణాంకం
getent(అక్షరం *tbuf, అక్షరం *పదం, FILE *termcap, పూర్ణాంకం buflen)
{
    అక్షరం    *tptr;
    పూర్ణాంకం	    tlen = అక్షరశ్రేణి_పొడవు(పదం);

    అయితే (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	అయితే (*tptr && *tptr != ':')		// : terminates name field
	{
	    అక్షరం    *nexttptr;

	    అయితే (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = కనుగొను_3(tptr, ":|");	// Rhialto
	    ఒకవేళ (tptr + tlen == nexttptr &&
		సరిపోలిక_3(tptr, పదం) == tlen)	// FOUND!
	    {
		tent = tbuf;
		తిరిగి 1;
	    }
	    లేకపోతే				// Look for next name
		tptr = nexttptr;
	}
    }
    తిరిగి 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    స్థిర పూర్ణాంకం
nextent(అక్షరం *tbuf, FILE *termcap, పూర్ణాంకం buflen)
{
    అక్షరం *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    అయితే (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (పూర్ణాంకం)(tbuf+buflen-lbuf), termcap)) // another line
    {
	పూర్ణాంకం llen = అక్షరశ్రేణి_పొడవు(lbuf);

	ఒకవేళ (*lbuf == '#')			// eat comments
	    కొనసాగు;
	ఒకవేళ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ఒకవేళ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	లేకపోతే
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    తిరిగి 1;
	}
    }

    తిరిగి 0;					// ran into end of file
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

    పూర్ణాంకం
tgetflag(అక్షరం *id)
{
    అక్షరం    buf[256], *ptr = buf;

    తిరిగి tgetstr(id, &ptr) ? 1 : 0;
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

    పూర్ణాంకం
tgetnum(అక్షరం *id)
{
    అక్షరం *ptr, buf[256];
    ptr = buf;

    ఒకవేళ (tgetstr(id, &ptr))
	తిరిగి atoi(buf);
    లేకపోతే
	తిరిగి 0;
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

    అక్షరం *
tgetstr(అక్షరం *id, అక్షరం **buf)
{
    పూర్ణాంకం		len = అక్షరశ్రేణి_పొడవు(id);
    అక్షరం	*tmp=tent;
    అక్షరం	*hold;
    పూర్ణాంకం		i;

    చేయి
    {
	tmp = కనుగొను_3(tmp, ":");			// For each field
	అయితే (*tmp == ':')			// skip empty fields
	    tmp++;
	ఒకవేళ (!*tmp)
	    ఆపు;

	ఒకవేళ (సరిపోలిక_3(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ఒకవేళ (*tmp == '@')			// :xx@: entry for tc
		తిరిగి 0;			// deleted entry
	    hold= *buf;
	    అయితే (*++tmp && *tmp != ':')	// not at end of field
		{
		ఎంపిక(*tmp)
		{
		స్థితి '\\':			// Expand escapes here
		    ఎంపిక(*++tmp)
		    {
		    స్థితి 0:			// ignore backslashes
			tmp--;			// at end of entry
			ఆపు;			// shouldn't happen
		    స్థితి 'e':
		    స్థితి 'E':			// ESC
			*(*buf)++ = ESC;
			ఆపు;
		    స్థితి 'n':			// \n
			*(*buf)++ = '\n';
			ఆపు;
		    స్థితి 'r':			// \r
			*(*buf)++ = '\r';
			ఆపు;
		    స్థితి 't':			// \t
			*(*buf)++ = '\t';
			ఆపు;
		    స్థితి 'b':			// \b
			*(*buf)++ = '\b';
			ఆపు;
		    స్థితి 'f':			// \f
			*(*buf)++ = '\f';
			ఆపు;
		    స్థితి '0':			// \nnn
		    స్థితి '1':
		    స్థితి '2':
		    స్థితి '3':
		    స్థితి '4':
		    స్థితి '5':
		    స్థితి '6':
		    స్థితి '7':
		    స్థితి '8':
		    స్థితి '9':
			**buf = 0;
			    // get up to three digits
			కొరకు (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			ఆపు;
		    అప్రమేయ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    ఆపు;
		స్థితి '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    ఆపు;
		అప్రమేయ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    తిరిగి hold;
	}
    } అయితే (*tmp);

    తిరిగి 0;
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

    అక్షరం *
tgoto(
    అక్షరం    *cm,				// cm string, from termcap
    పూర్ణాంకం	    నిలువు,				// column, x position
    పూర్ణాంకం	    పంక్తి)				// line, y position
{
    అక్షరం    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	తిరగేయి = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    స్థిర అక్షరం బఫరు[32];

    ఒకవేళ (!cm)
	తిరిగి "OOPS";				// Kludge, but standard

    bufp = బఫరు;
    ptr = cm;

    అయితే (*ptr)
    {
	ఒకవేళ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	లేకపోతే
	{				// % escape
	    ఎంపిక(c = *ptr++)
	    {
	    స్థితి 'd':				// decimal
		bufp = _addfmt(bufp, "%d", పంక్తి);
		పంక్తి = నిలువు;
		ఆపు;
	    స్థితి '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", పంక్తి);
		పంక్తి = నిలువు;
		ఆపు;
	    స్థితి '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", పంక్తి);
		పంక్తి = నిలువు;
		ఆపు;
	    స్థితి '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		ఒకవేళ (నిలువు>gx) నిలువు += gy;
		ఒకవేళ (పంక్తి>gx) పంక్తి += gy;
		ఆపు;
	    స్థితి '+':				// %+c: add c
		పంక్తి += *ptr++;
	    స్థితి '.':				// print x/y
		ఒకవేళ (పంక్తి == '\t' ||		// these are
		   పంక్తి == '\n' ||		// chars that
		   పంక్తి == '\004' ||		// UNIX hates
		   పంక్తి == '\0')
		{
		    పంక్తి++;			// so go to next pos
		    ఒకవేళ (తిరగేయి == (పంక్తి == నిలువు))
			addup=1;		// and mark UP
		    లేకపోతే
			addbak=1;		// or BC
		}
		*bufp++=పంక్తి;
		పంక్తి = నిలువు;
		ఆపు;
	    స్థితి 'r':				// r: reverse
		gx = పంక్తి;
		పంక్తి = నిలువు;
		నిలువు = gx;
		తిరగేయి = 1;
		ఆపు;
	    స్థితి 'i':			// increment (1-origin screen)
		నిలువు++;
		పంక్తి++;
		ఆపు;
	    స్థితి '%':				// %%=% literally
		*bufp++='%';
		ఆపు;
	    స్థితి 'n':				// magic DM2500 code
		పంక్తి ^= 0140;
		నిలువు ^= 0140;
		ఆపు;
	    స్థితి 'B':				// bcd encoding
		పంక్తి = పంక్తి/10<<4+పంక్తి%10;
		నిలువు = నిలువు/10<<4+నిలువు%10;
		ఆపు;
	    స్థితి 'D':				// magic Delta Data code
		పంక్తి = పంక్తి-2*(పంక్తి&15);
		నిలువు = నిలువు-2*(నిలువు&15);
		ఆపు;
	    అప్రమేయ:				// Unknown escape
		తిరిగి "OOPS";
	    }
	}
    }

    ఒకవేళ (addup)					// add upline
	ఒకవేళ (పైకి_2)
	{
	    ptr=పైకి_2;
	    అయితే (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ఒకవేళ (*ptr == '*')
		ptr++;
	    అయితే (*ptr)
		*bufp++ = *ptr++;
	}

    ఒకవేళ (addbak)					// add backspace
	ఒకవేళ (BC)
	{
	    ptr=BC;
	    అయితే (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ఒకవేళ (*ptr == '*')
		ptr++;
	    అయితే (*ptr)
		*bufp++ = *ptr++;
	}
	లేకపోతే
	    *bufp++='\b';

    *bufp = 0;

    తిరిగి(బఫరు);
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

దీర్ఘ _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    పూర్ణాంకం
tputs(
    అక్షరం *cp,				// string to print
    పూర్ణాంకం affcnt,				// Number of lines affected
    శూన్య (*outc)(గుర్తులేని పూర్ణాంకం))		// routine to output 1 character
{
    దీర్ఘ    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(స్థిరరాశి అక్షరం *);

    ఒకవేళ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	అయితే (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (దీర్ఘ)(*cp++ - '0');
	ఒకవేళ (*cp == '.')
	    అయితే (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (దీర్ఘ)(*cp++ - '0');
		frac = frac * 10;
	    }
	ఒకవేళ (*cp!='*')			// multiply by affected lines
	{
	    ఒకవేళ (affcnt>1) affcnt = 1;
	}
	లేకపోతే
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ఒకవేళ (ospeed)
	    counter = (counter * _bauds[ospeed] * (దీర్ఘ)affcnt) / frac;

	అయితే (*cp)			// output string
	    (*outc)(*cp++);
	ఒకవేళ (ospeed)
	    అయితే (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    లేకపోతే
	అయితే (*cp)
	    (*outc)(*cp++);
    తిరిగి 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_3(అక్షరం *s1, అక్షరం *s2)
{
    పూర్ణాంకం i = 0;

    అయితే (s1[i] && s1[i] == s2[i])
	i++;

    తిరిగి i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    స్థిర అక్షరం *
కనుగొను_3(అక్షరం *s, అక్షరం *సమితి_2)
{
    కొరకు (; *s; s++)
    {
	అక్షరం	*ptr = సమితి_2;

	అయితే (*ptr && *s != *ptr)
	    ptr++;

	ఒకవేళ (*ptr)
	    తిరిగి s;
    }

    తిరిగి s;
}

/*
 * add val to buf according to format fmt
 */
    స్థిర అక్షరం *
_addfmt(అక్షరం *buf, అక్షరం *fmt, పూర్ణాంకం val)
{
    sprintf(buf, fmt, val);
    అయితే (*buf)
	buf++;
    తిరిగి buf;
}
