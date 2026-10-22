/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "termlib.pro"

#જો !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

સ્થિર પૂર્ણાંક  getent(અક્ષર *, અક્ષર *, FILE *, પૂર્ણાંક);
સ્થિર પૂર્ણાંક  nextent(અક્ષર *, FILE *, પૂર્ણાંક);
સ્થિર પૂર્ણાંક  _match(અક્ષર *, અક્ષર *);
સ્થિર અક્ષર *_addfmt(અક્ષર *, અક્ષર *, પૂર્ણાંક);
સ્થિર અક્ષર *_find(અક્ષર *, અક્ષર *);

/*
 * Global variables for termlib
 */

અક્ષર	*tent;		      // Pointer to terminal entry, set by tgetent
અક્ષર	PC = 0;		      // Pad character, default NULL
અક્ષર	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# નહીંતર
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  નહીંતર
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    પૂર્ણાંક
tgetent(
    અક્ષર    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    અક્ષર    *term)		// Name of terminal
{
    અક્ષર    tcbuf[32];		// Temp buffer to handle
    અક્ષર    *tcptr = tcbuf;	// extended entries
    અક્ષર    *tcap = TERMCAPFILE; // Default termcap file
    અક્ષર    *tmp;
    FILE    *termcap;
    પૂર્ણાંક	    retval = 0;
    પૂર્ણાંક	    len;

    જો ((tmp = (અક્ષર *)mch_getenv((char_u *)"TERMCAP")) != ખાલી)
    {
	જો (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#જો defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    જો (tmp)
		*tmp = ':';
#endif
	}
	નહીંતર				// TERMCAP = termcap entry itself
	{
	    પૂર્ણાંક tlen = strlen(term);

	    જ્યારે (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		અક્ષર *nexttmp;

		જ્યારે (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		જો (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    પાછા 1;
		}
		નહીંતર
		    tmp = nexttmp;
	    }
	}
    }
    જો (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	પાછા -1;
    }

    len = 0;
    જ્યારે (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	જો ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	નહીંતર
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    તોડો;
	}
    }
    fclose(termcap);
    પાછા retval;
}

    સ્થિર પૂર્ણાંક
getent(અક્ષર *tbuf, અક્ષર *term, FILE *termcap, પૂર્ણાંક buflen)
{
    અક્ષર    *tptr;
    પૂર્ણાંક	    tlen = strlen(term);

    જ્યારે (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	જ્યારે (*tptr && *tptr != ':')		// : terminates name field
	{
	    અક્ષર    *nexttptr;

	    જ્યારે (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    જો (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		પાછા 1;
	    }
	    નહીંતર				// Look for next name
		tptr = nexttptr;
	}
    }
    પાછા 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    સ્થિર પૂર્ણાંક
nextent(અક્ષર *tbuf, FILE *termcap, પૂર્ણાંક buflen)
{
    અક્ષર *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    જ્યારે (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (પૂર્ણાંક)(tbuf+buflen-lbuf), termcap)) // another line
    {
	પૂર્ણાંક llen = strlen(lbuf);

	જો (*lbuf == '#')			// eat comments
	    ચાલુ;
	જો (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	જો (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	નહીંતર
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    પાછા 1;
	}
    }

    પાછા 0;					// ran into end of file
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

    પૂર્ણાંક
tgetflag(અક્ષર *id)
{
    અક્ષર    buf[256], *ptr = buf;

    પાછા tgetstr(id, &ptr) ? 1 : 0;
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

    પૂર્ણાંક
tgetnum(અક્ષર *id)
{
    અક્ષર *ptr, buf[256];
    ptr = buf;

    જો (tgetstr(id, &ptr))
	પાછા atoi(buf);
    નહીંતર
	પાછા 0;
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

    અક્ષર *
tgetstr(અક્ષર *id, અક્ષર **buf)
{
    પૂર્ણાંક		len = strlen(id);
    અક્ષર	*tmp=tent;
    અક્ષર	*hold;
    પૂર્ણાંક		i;

    કરો
    {
	tmp = _find(tmp, ":");			// For each field
	જ્યારે (*tmp == ':')			// skip empty fields
	    tmp++;
	જો (!*tmp)
	    તોડો;

	જો (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    જો (*tmp == '@')			// :xx@: entry for tc
		પાછા 0;			// deleted entry
	    hold= *buf;
	    જ્યારે (*++tmp && *tmp != ':')	// not at end of field
		{
		પસંદગી(*tmp)
		{
		સ્થિતિ '\\':			// Expand escapes here
		    પસંદગી(*++tmp)
		    {
		    સ્થિતિ 0:			// ignore backslashes
			tmp--;			// at end of entry
			તોડો;			// shouldn't happen
		    સ્થિતિ 'e':
		    સ્થિતિ 'E':			// ESC
			*(*buf)++ = ESC;
			તોડો;
		    સ્થિતિ 'n':			// \n
			*(*buf)++ = '\n';
			તોડો;
		    સ્થિતિ 'r':			// \r
			*(*buf)++ = '\r';
			તોડો;
		    સ્થિતિ 't':			// \t
			*(*buf)++ = '\t';
			તોડો;
		    સ્થિતિ 'b':			// \b
			*(*buf)++ = '\b';
			તોડો;
		    સ્થિતિ 'f':			// \f
			*(*buf)++ = '\f';
			તોડો;
		    સ્થિતિ '0':			// \nnn
		    સ્થિતિ '1':
		    સ્થિતિ '2':
		    સ્થિતિ '3':
		    સ્થિતિ '4':
		    સ્થિતિ '5':
		    સ્થિતિ '6':
		    સ્થિતિ '7':
		    સ્થિતિ '8':
		    સ્થિતિ '9':
			**buf = 0;
			    // get up to three digits
			માટે (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			તોડો;
		    મૂળભૂત:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    તોડો;
		સ્થિતિ '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    તોડો;
		મૂળભૂત:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    પાછા hold;
	}
    } જ્યારે (*tmp);

    પાછા 0;
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

    અક્ષર *
tgoto(
    અક્ષર    *cm,				// cm string, from termcap
    પૂર્ણાંક	    col,				// column, x position
    પૂર્ણાંક	    લીટી)				// line, y position
{
    અક્ષર    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    સ્થિર અક્ષર buffer[32];

    જો (!cm)
	પાછા "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    જ્યારે (*ptr)
    {
	જો ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	નહીંતર
	{				// % escape
	    પસંદગી(c = *ptr++)
	    {
	    સ્થિતિ 'd':				// decimal
		bufp = _addfmt(bufp, "%d", લીટી);
		લીટી = col;
		તોડો;
	    સ્થિતિ '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", લીટી);
		લીટી = col;
		તોડો;
	    સ્થિતિ '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", લીટી);
		લીટી = col;
		તોડો;
	    સ્થિતિ '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		જો (col>gx) col += gy;
		જો (લીટી>gx) લીટી += gy;
		તોડો;
	    સ્થિતિ '+':				// %+c: add c
		લીટી += *ptr++;
	    સ્થિતિ '.':				// print x/y
		જો (લીટી == '\t' ||		// these are
		   લીટી == '\n' ||		// chars that
		   લીટી == '\004' ||		// UNIX hates
		   લીટી == '\0')
		{
		    લીટી++;			// so go to next pos
		    જો (reverse == (લીટી == col))
			addup=1;		// and mark UP
		    નહીંતર
			addbak=1;		// or BC
		}
		*bufp++=લીટી;
		લીટી = col;
		તોડો;
	    સ્થિતિ 'r':				// r: reverse
		gx = લીટી;
		લીટી = col;
		col = gx;
		reverse = 1;
		તોડો;
	    સ્થિતિ 'i':			// increment (1-origin screen)
		col++;
		લીટી++;
		તોડો;
	    સ્થિતિ '%':				// %%=% literally
		*bufp++='%';
		તોડો;
	    સ્થિતિ 'n':				// magic DM2500 code
		લીટી ^= 0140;
		col ^= 0140;
		તોડો;
	    સ્થિતિ 'B':				// bcd encoding
		લીટી = લીટી/10<<4+લીટી%10;
		col = col/10<<4+col%10;
		તોડો;
	    સ્થિતિ 'D':				// magic Delta Data code
		લીટી = લીટી-2*(લીટી&15);
		col = col-2*(col&15);
		તોડો;
	    મૂળભૂત:				// Unknown escape
		પાછા "OOPS";
	    }
	}
    }

    જો (addup)					// add upline
	જો (UP)
	{
	    ptr=UP;
	    જ્યારે (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    જો (*ptr == '*')
		ptr++;
	    જ્યારે (*ptr)
		*bufp++ = *ptr++;
	}

    જો (addbak)					// add backspace
	જો (BC)
	{
	    ptr=BC;
	    જ્યારે (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    જો (*ptr == '*')
		ptr++;
	    જ્યારે (*ptr)
		*bufp++ = *ptr++;
	}
	નહીંતર
	    *bufp++='\b';

    *bufp = 0;

    પાછા(buffer);
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

    પૂર્ણાંક
tputs(
    અક્ષર *cp,				// string to print
    પૂર્ણાંક affcnt,				// Number of lines affected
    શૂન્ય (*outc)(unsigned પૂર્ણાંક))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(અચલ અક્ષર *);

    જો (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	જ્યારે (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	જો (*cp == '.')
	    જ્યારે (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	જો (*cp!='*')			// multiply by affected lines
	{
	    જો (affcnt>1) affcnt = 1;
	}
	નહીંતર
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	જો (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	જ્યારે (*cp)			// output string
	    (*outc)(*cp++);
	જો (ospeed)
	    જ્યારે (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    નહીંતર
	જ્યારે (*cp)
	    (*outc)(*cp++);
    પાછા 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    સ્થિર પૂર્ણાંક
_match(અક્ષર *s1, અક્ષર *s2)
{
    પૂર્ણાંક i = 0;

    જ્યારે (s1[i] && s1[i] == s2[i])
	i++;

    પાછા i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    સ્થિર અક્ષર *
_find(અક્ષર *s, અક્ષર *set)
{
    માટે (; *s; s++)
    {
	અક્ષર	*ptr = set;

	જ્યારે (*ptr && *s != *ptr)
	    ptr++;

	જો (*ptr)
	    પાછા s;
    }

    પાછા s;
}

/*
 * add val to buf according to format fmt
 */
    સ્થિર અક્ષર *
_addfmt(અક્ષર *buf, અક્ષર *fmt, પૂર્ણાંક val)
{
    sprintf(buf, fmt, val);
    જ્યારે (*buf)
	buf++;
    પાછા buf;
}
