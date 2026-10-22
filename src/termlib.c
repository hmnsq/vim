/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "termlib.pro"

#ਜੇ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ  getent(ਅੱਖਰ *, ਅੱਖਰ *, FILE *, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ  nextent(ਅੱਖਰ *, FILE *, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ  ਮੇਲ_3(ਅੱਖਰ *, ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *_addfmt(ਅੱਖਰ *, ਅੱਖਰ *, ਪੂਰਨ);
ਸਥਿਰ ਅੱਖਰ *ਲੱਭੋ_3(ਅੱਖਰ *, ਅੱਖਰ *);

/*
 * Global variables for termlib
 */

ਅੱਖਰ	*tent;		      // Pointer to terminal entry, set by tgetent
ਅੱਖਰ	PC = 0;		      // Pad character, default NULL
ਅੱਖਰ	*ਉੱਪਰ_2 = 0, *BC = 0;     // Pointers to UP and BC strings from database
ਛੋਟਾ	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TERMCAPFILE
# ifdef AMIGA
#  define TERMCAPFILE "s:termcap"
# ਨਹੀਂਤਾਂ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ਨਹੀਂਤਾਂ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
tgetent(
    ਅੱਖਰ    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    ਅੱਖਰ    *ਪਦ)		// Name of terminal
{
    ਅੱਖਰ    tcbuf[32];		// Temp buffer to handle
    ਅੱਖਰ    *tcptr = tcbuf;	// extended entries
    ਅੱਖਰ    *tcap = TERMCAPFILE; // Default termcap file
    ਅੱਖਰ    *tmp;
    FILE    *termcap;
    ਪੂਰਨ	    retval = 0;
    ਪੂਰਨ	    len;

    ਜੇ ((tmp = (ਅੱਖਰ *)mch_getenv((char_u *)"TERMCAP")) != ਨਿਰਾ)
    {
	ਜੇ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ਜੇ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ਜੇ (tmp)
		*tmp = ':';
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ				// TERMCAP = termcap entry itself
	{
	    ਪੂਰਨ tlen = ਲੜੀ_ਲੰਬਾਈ(ਪਦ);

	    ਜਦੋਂ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		ਅੱਖਰ *nexttmp;

		ਜਦੋਂ (*tmp == '|')
		    tmp++;
		nexttmp  = ਲੱਭੋ_3(tmp, ":|");	// Rhialto
		ਜੇ (tmp+tlen == nexttmp && ਮੇਲ_3(tmp, ਪਦ) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ਵਾਪਸ 1;
		}
		ਨਹੀਂਤਾਂ
		    tmp = nexttmp;
	    }
	}
    }
    ਜੇ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ਵਾਪਸ -1;
    }

    len = 0;
    ਜਦੋਂ (getent(tbuf + len, ਪਦ, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ਜੇ ((ਪਦ = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = ਲੜੀ_ਲੰਬਾਈ(tbuf);
	}
	ਨਹੀਂਤਾਂ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    ਰੋਕੋ;
	}
    }
    fclose(termcap);
    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਪੂਰਨ
getent(ਅੱਖਰ *tbuf, ਅੱਖਰ *ਪਦ, FILE *termcap, ਪੂਰਨ buflen)
{
    ਅੱਖਰ    *tptr;
    ਪੂਰਨ	    tlen = ਲੜੀ_ਲੰਬਾਈ(ਪਦ);

    ਜਦੋਂ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ਜਦੋਂ (*tptr && *tptr != ':')		// : terminates name field
	{
	    ਅੱਖਰ    *nexttptr;

	    ਜਦੋਂ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = ਲੱਭੋ_3(tptr, ":|");	// Rhialto
	    ਜੇ (tptr + tlen == nexttptr &&
		ਮੇਲ_3(tptr, ਪਦ) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ਵਾਪਸ 1;
	    }
	    ਨਹੀਂਤਾਂ				// Look for next name
		tptr = nexttptr;
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ਸਥਿਰ ਪੂਰਨ
nextent(ਅੱਖਰ *tbuf, FILE *termcap, ਪੂਰਨ buflen)
{
    ਅੱਖਰ *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ਜਦੋਂ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ਪੂਰਨ)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ਪੂਰਨ llen = ਲੜੀ_ਲੰਬਾਈ(lbuf);

	ਜੇ (*lbuf == '#')			// eat comments
	    ਜਾਰੀ;
	ਜੇ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ਜੇ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ਨਹੀਂਤਾਂ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ਵਾਪਸ 1;
	}
    }

    ਵਾਪਸ 0;					// ran into end of file
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

    ਪੂਰਨ
tgetflag(ਅੱਖਰ *id)
{
    ਅੱਖਰ    buf[256], *ptr = buf;

    ਵਾਪਸ tgetstr(id, &ptr) ? 1 : 0;
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

    ਪੂਰਨ
tgetnum(ਅੱਖਰ *id)
{
    ਅੱਖਰ *ptr, buf[256];
    ptr = buf;

    ਜੇ (tgetstr(id, &ptr))
	ਵਾਪਸ atoi(buf);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ 0;
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

    ਅੱਖਰ *
tgetstr(ਅੱਖਰ *id, ਅੱਖਰ **buf)
{
    ਪੂਰਨ		len = ਲੜੀ_ਲੰਬਾਈ(id);
    ਅੱਖਰ	*tmp=tent;
    ਅੱਖਰ	*hold;
    ਪੂਰਨ		i;

    ਕਰੋ
    {
	tmp = ਲੱਭੋ_3(tmp, ":");			// For each field
	ਜਦੋਂ (*tmp == ':')			// skip empty fields
	    tmp++;
	ਜੇ (!*tmp)
	    ਰੋਕੋ;

	ਜੇ (ਮੇਲ_3(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ਜੇ (*tmp == '@')			// :xx@: entry for tc
		ਵਾਪਸ 0;			// deleted entry
	    hold= *buf;
	    ਜਦੋਂ (*++tmp && *tmp != ':')	// not at end of field
		{
		ਚੋਣ(*tmp)
		{
		ਹਾਲਤ '\\':			// Expand escapes here
		    ਚੋਣ(*++tmp)
		    {
		    ਹਾਲਤ 0:			// ignore backslashes
			tmp--;			// at end of entry
			ਰੋਕੋ;			// shouldn't happen
		    ਹਾਲਤ 'e':
		    ਹਾਲਤ 'E':			// ESC
			*(*buf)++ = ESC;
			ਰੋਕੋ;
		    ਹਾਲਤ 'n':			// \n
			*(*buf)++ = '\n';
			ਰੋਕੋ;
		    ਹਾਲਤ 'r':			// \r
			*(*buf)++ = '\r';
			ਰੋਕੋ;
		    ਹਾਲਤ 't':			// \t
			*(*buf)++ = '\t';
			ਰੋਕੋ;
		    ਹਾਲਤ 'b':			// \b
			*(*buf)++ = '\b';
			ਰੋਕੋ;
		    ਹਾਲਤ 'f':			// \f
			*(*buf)++ = '\f';
			ਰੋਕੋ;
		    ਹਾਲਤ '0':			// \nnn
		    ਹਾਲਤ '1':
		    ਹਾਲਤ '2':
		    ਹਾਲਤ '3':
		    ਹਾਲਤ '4':
		    ਹਾਲਤ '5':
		    ਹਾਲਤ '6':
		    ਹਾਲਤ '7':
		    ਹਾਲਤ '8':
		    ਹਾਲਤ '9':
			**buf = 0;
			    // get up to three digits
			ਲਈ (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			ਰੋਕੋ;
		    ਮੂਲ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    ਰੋਕੋ;
		ਹਾਲਤ '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    ਰੋਕੋ;
		ਮੂਲ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ਵਾਪਸ hold;
	}
    } ਜਦੋਂ (*tmp);

    ਵਾਪਸ 0;
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

    ਅੱਖਰ *
tgoto(
    ਅੱਖਰ    *cm,				// cm string, from termcap
    ਪੂਰਨ	    ਖਾਨਾ,				// column, x position
    ਪੂਰਨ	    ਸਤਰ)				// line, y position
{
    ਅੱਖਰ    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	ਉਲਟ = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ਸਥਿਰ ਅੱਖਰ ਬਫਰ[32];

    ਜੇ (!cm)
	ਵਾਪਸ "OOPS";				// Kludge, but standard

    bufp = ਬਫਰ;
    ptr = cm;

    ਜਦੋਂ (*ptr)
    {
	ਜੇ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ਨਹੀਂਤਾਂ
	{				// % escape
	    ਚੋਣ(c = *ptr++)
	    {
	    ਹਾਲਤ 'd':				// decimal
		bufp = _addfmt(bufp, "%d", ਸਤਰ);
		ਸਤਰ = ਖਾਨਾ;
		ਰੋਕੋ;
	    ਹਾਲਤ '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", ਸਤਰ);
		ਸਤਰ = ਖਾਨਾ;
		ਰੋਕੋ;
	    ਹਾਲਤ '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", ਸਤਰ);
		ਸਤਰ = ਖਾਨਾ;
		ਰੋਕੋ;
	    ਹਾਲਤ '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		ਜੇ (ਖਾਨਾ>gx) ਖਾਨਾ += gy;
		ਜੇ (ਸਤਰ>gx) ਸਤਰ += gy;
		ਰੋਕੋ;
	    ਹਾਲਤ '+':				// %+c: add c
		ਸਤਰ += *ptr++;
	    ਹਾਲਤ '.':				// print x/y
		ਜੇ (ਸਤਰ == '\t' ||		// these are
		   ਸਤਰ == '\n' ||		// chars that
		   ਸਤਰ == '\004' ||		// UNIX hates
		   ਸਤਰ == '\0')
		{
		    ਸਤਰ++;			// so go to next pos
		    ਜੇ (ਉਲਟ == (ਸਤਰ == ਖਾਨਾ))
			addup=1;		// and mark UP
		    ਨਹੀਂਤਾਂ
			addbak=1;		// or BC
		}
		*bufp++=ਸਤਰ;
		ਸਤਰ = ਖਾਨਾ;
		ਰੋਕੋ;
	    ਹਾਲਤ 'r':				// r: reverse
		gx = ਸਤਰ;
		ਸਤਰ = ਖਾਨਾ;
		ਖਾਨਾ = gx;
		ਉਲਟ = 1;
		ਰੋਕੋ;
	    ਹਾਲਤ 'i':			// increment (1-origin screen)
		ਖਾਨਾ++;
		ਸਤਰ++;
		ਰੋਕੋ;
	    ਹਾਲਤ '%':				// %%=% literally
		*bufp++='%';
		ਰੋਕੋ;
	    ਹਾਲਤ 'n':				// magic DM2500 code
		ਸਤਰ ^= 0140;
		ਖਾਨਾ ^= 0140;
		ਰੋਕੋ;
	    ਹਾਲਤ 'B':				// bcd encoding
		ਸਤਰ = ਸਤਰ/10<<4+ਸਤਰ%10;
		ਖਾਨਾ = ਖਾਨਾ/10<<4+ਖਾਨਾ%10;
		ਰੋਕੋ;
	    ਹਾਲਤ 'D':				// magic Delta Data code
		ਸਤਰ = ਸਤਰ-2*(ਸਤਰ&15);
		ਖਾਨਾ = ਖਾਨਾ-2*(ਖਾਨਾ&15);
		ਰੋਕੋ;
	    ਮੂਲ:				// Unknown escape
		ਵਾਪਸ "OOPS";
	    }
	}
    }

    ਜੇ (addup)					// add upline
	ਜੇ (ਉੱਪਰ_2)
	{
	    ptr=ਉੱਪਰ_2;
	    ਜਦੋਂ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ਜੇ (*ptr == '*')
		ptr++;
	    ਜਦੋਂ (*ptr)
		*bufp++ = *ptr++;
	}

    ਜੇ (addbak)					// add backspace
	ਜੇ (BC)
	{
	    ptr=BC;
	    ਜਦੋਂ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ਜੇ (*ptr == '*')
		ptr++;
	    ਜਦੋਂ (*ptr)
		*bufp++ = *ptr++;
	}
	ਨਹੀਂਤਾਂ
	    *bufp++='\b';

    *bufp = 0;

    ਵਾਪਸ(ਬਫਰ);
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

ਲੰਮਾ _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    ਪੂਰਨ
tputs(
    ਅੱਖਰ *cp,				// string to print
    ਪੂਰਨ affcnt,				// Number of lines affected
    ਖਾਲੀ (*outc)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ))		// routine to output 1 character
{
    ਲੰਮਾ    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ਅਚਲ ਅੱਖਰ *);

    ਜੇ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ਜਦੋਂ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (ਲੰਮਾ)(*cp++ - '0');
	ਜੇ (*cp == '.')
	    ਜਦੋਂ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (ਲੰਮਾ)(*cp++ - '0');
		frac = frac * 10;
	    }
	ਜੇ (*cp!='*')			// multiply by affected lines
	{
	    ਜੇ (affcnt>1) affcnt = 1;
	}
	ਨਹੀਂਤਾਂ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ਜੇ (ospeed)
	    counter = (counter * _bauds[ospeed] * (ਲੰਮਾ)affcnt) / frac;

	ਜਦੋਂ (*cp)			// output string
	    (*outc)(*cp++);
	ਜੇ (ospeed)
	    ਜਦੋਂ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    ਨਹੀਂਤਾਂ
	ਜਦੋਂ (*cp)
	    (*outc)(*cp++);
    ਵਾਪਸ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_3(ਅੱਖਰ *s1, ਅੱਖਰ *s2)
{
    ਪੂਰਨ i = 0;

    ਜਦੋਂ (s1[i] && s1[i] == s2[i])
	i++;

    ਵਾਪਸ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ਸਥਿਰ ਅੱਖਰ *
ਲੱਭੋ_3(ਅੱਖਰ *s, ਅੱਖਰ *ਸੈੱਟ_2)
{
    ਲਈ (; *s; s++)
    {
	ਅੱਖਰ	*ptr = ਸੈੱਟ_2;

	ਜਦੋਂ (*ptr && *s != *ptr)
	    ptr++;

	ਜੇ (*ptr)
	    ਵਾਪਸ s;
    }

    ਵਾਪਸ s;
}

/*
 * add val to buf according to format fmt
 */
    ਸਥਿਰ ਅੱਖਰ *
_addfmt(ਅੱਖਰ *buf, ਅੱਖਰ *fmt, ਪੂਰਨ val)
{
    sprintf(buf, fmt, val);
    ਜਦੋਂ (*buf)
	buf++;
    ਵਾਪਸ buf;
}
