/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "termlib.pro"

#ಯದಿ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  getent(ವರ್ಣ *, ವರ್ಣ *, FILE *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  nextent(ವರ್ಣ *, FILE *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  _match(ವರ್ಣ *, ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *_addfmt(ವರ್ಣ *, ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ವರ್ಣ *_find(ವರ್ಣ *, ವರ್ಣ *);

/*
 * Global variables for termlib
 */

ವರ್ಣ	*tent;		      // Pointer to terminal entry, set by tgetent
ವರ್ಣ	PC = 0;		      // Pad character, default NULL
ವರ್ಣ	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# ಅನ್ಯಥಾ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ಅನ್ಯಥಾ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    ಪೂರ್ಣಾಙ್ಕ
tgetent(
    ವರ್ಣ    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    ವರ್ಣ    *term)		// Name of terminal
{
    ವರ್ಣ    tcbuf[32];		// Temp buffer to handle
    ವರ್ಣ    *tcptr = tcbuf;	// extended entries
    ವರ್ಣ    *tcap = TERMCAPFILE; // Default termcap file
    ವರ್ಣ    *tmp;
    FILE    *termcap;
    ಪೂರ್ಣಾಙ್ಕ	    retval = 0;
    ಪೂರ್ಣಾಙ್ಕ	    len;

    ಯದಿ ((tmp = (ವರ್ಣ *)mch_getenv((char_u *)"TERMCAP")) != ರಿಕ್ತ)
    {
	ಯದಿ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ಯದಿ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ಯದಿ (tmp)
		*tmp = ':';
#endif
	}
	ಅನ್ಯಥಾ				// TERMCAP = termcap entry itself
	{
	    ಪೂರ್ಣಾಙ್ಕ tlen = strlen(term);

	    ಯಾವತ್ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		ವರ್ಣ *nexttmp;

		ಯಾವತ್ (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		ಯದಿ (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ಪ್ರತ್ಯಾಗಮ 1;
		}
		ಅನ್ಯಥಾ
		    tmp = nexttmp;
	    }
	}
    }
    ಯದಿ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    len = 0;
    ಯಾವತ್ (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ಯದಿ ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	ಅನ್ಯಥಾ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    ಭಙ್ಗ;
	}
    }
    fclose(termcap);
    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getent(ವರ್ಣ *tbuf, ವರ್ಣ *term, FILE *termcap, ಪೂರ್ಣಾಙ್ಕ buflen)
{
    ವರ್ಣ    *tptr;
    ಪೂರ್ಣಾಙ್ಕ	    tlen = strlen(term);

    ಯಾವತ್ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ಯಾವತ್ (*tptr && *tptr != ':')		// : terminates name field
	{
	    ವರ್ಣ    *nexttptr;

	    ಯಾವತ್ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    ಯದಿ (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	    ಅನ್ಯಥಾ				// Look for next name
		tptr = nexttptr;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nextent(ವರ್ಣ *tbuf, FILE *termcap, ಪೂರ್ಣಾಙ್ಕ buflen)
{
    ವರ್ಣ *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ಯಾವತ್ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ಪೂರ್ಣಾಙ್ಕ)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ಪೂರ್ಣಾಙ್ಕ llen = strlen(lbuf);

	ಯದಿ (*lbuf == '#')			// eat comments
	    ಅನುವರ್ತ;
	ಯದಿ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ಯದಿ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ಅನ್ಯಥಾ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ಪ್ರತ್ಯಾಗಮ 1;
	}
    }

    ಪ್ರತ್ಯಾಗಮ 0;					// ran into end of file
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

    ಪೂರ್ಣಾಙ್ಕ
tgetflag(ವರ್ಣ *id)
{
    ವರ್ಣ    buf[256], *ptr = buf;

    ಪ್ರತ್ಯಾಗಮ tgetstr(id, &ptr) ? 1 : 0;
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

    ಪೂರ್ಣಾಙ್ಕ
tgetnum(ವರ್ಣ *id)
{
    ವರ್ಣ *ptr, buf[256];
    ptr = buf;

    ಯದಿ (tgetstr(id, &ptr))
	ಪ್ರತ್ಯಾಗಮ atoi(buf);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 0;
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

    ವರ್ಣ *
tgetstr(ವರ್ಣ *id, ವರ್ಣ **buf)
{
    ಪೂರ್ಣಾಙ್ಕ		len = strlen(id);
    ವರ್ಣ	*tmp=tent;
    ವರ್ಣ	*hold;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಕುರು
    {
	tmp = _find(tmp, ":");			// For each field
	ಯಾವತ್ (*tmp == ':')			// skip empty fields
	    tmp++;
	ಯದಿ (!*tmp)
	    ಭಙ್ಗ;

	ಯದಿ (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ಯದಿ (*tmp == '@')			// :xx@: entry for tc
		ಪ್ರತ್ಯಾಗಮ 0;			// deleted entry
	    hold= *buf;
	    ಯಾವತ್ (*++tmp && *tmp != ':')	// not at end of field
		{
		ವಿಕಲ್ಪ(*tmp)
		{
		ಪಕ್ಷ '\\':			// Expand escapes here
		    ವಿಕಲ್ಪ(*++tmp)
		    {
		    ಪಕ್ಷ 0:			// ignore backslashes
			tmp--;			// at end of entry
			ಭಙ್ಗ;			// shouldn't happen
		    ಪಕ್ಷ 'e':
		    ಪಕ್ಷ 'E':			// ESC
			*(*buf)++ = ESC;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'n':			// \n
			*(*buf)++ = '\n';
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'r':			// \r
			*(*buf)++ = '\r';
			ಭಙ್ಗ;
		    ಪಕ್ಷ 't':			// \t
			*(*buf)++ = '\t';
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'b':			// \b
			*(*buf)++ = '\b';
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'f':			// \f
			*(*buf)++ = '\f';
			ಭಙ್ಗ;
		    ಪಕ್ಷ '0':			// \nnn
		    ಪಕ್ಷ '1':
		    ಪಕ್ಷ '2':
		    ಪಕ್ಷ '3':
		    ಪಕ್ಷ '4':
		    ಪಕ್ಷ '5':
		    ಪಕ್ಷ '6':
		    ಪಕ್ಷ '7':
		    ಪಕ್ಷ '8':
		    ಪಕ್ಷ '9':
			**buf = 0;
			    // get up to three digits
			ಪ್ರತಿ (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			ಭಙ್ಗ;
		    ಸ್ವಭಾವ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    ಭಙ್ಗ;
		ಪಕ್ಷ '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ಪ್ರತ್ಯಾಗಮ hold;
	}
    } ಯಾವತ್ (*tmp);

    ಪ್ರತ್ಯಾಗಮ 0;
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

    ವರ್ಣ *
tgoto(
    ವರ್ಣ    *cm,				// cm string, from termcap
    ಪೂರ್ಣಾಙ್ಕ	    col,				// column, x position
    ಪೂರ್ಣಾಙ್ಕ	    ಸಾಲು)				// line, y position
{
    ವರ್ಣ    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ಸ್ಥಿರ ವರ್ಣ buffer[32];

    ಯದಿ (!cm)
	ಪ್ರತ್ಯಾಗಮ "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    ಯಾವತ್ (*ptr)
    {
	ಯದಿ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ಅನ್ಯಥಾ
	{				// % escape
	    ವಿಕಲ್ಪ(c = *ptr++)
	    {
	    ಪಕ್ಷ 'd':				// decimal
		bufp = _addfmt(bufp, "%d", ಸಾಲು);
		ಸಾಲು = col;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", ಸಾಲು);
		ಸಾಲು = col;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", ಸಾಲು);
		ಸಾಲು = col;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		ಯದಿ (col>gx) col += gy;
		ಯದಿ (ಸಾಲು>gx) ಸಾಲು += gy;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '+':				// %+c: add c
		ಸಾಲು += *ptr++;
	    ಪಕ್ಷ '.':				// print x/y
		ಯದಿ (ಸಾಲು == '\t' ||		// these are
		   ಸಾಲು == '\n' ||		// chars that
		   ಸಾಲು == '\004' ||		// UNIX hates
		   ಸಾಲು == '\0')
		{
		    ಸಾಲು++;			// so go to next pos
		    ಯದಿ (reverse == (ಸಾಲು == col))
			addup=1;		// and mark UP
		    ಅನ್ಯಥಾ
			addbak=1;		// or BC
		}
		*bufp++=ಸಾಲು;
		ಸಾಲು = col;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'r':				// r: reverse
		gx = ಸಾಲು;
		ಸಾಲು = col;
		col = gx;
		reverse = 1;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'i':			// increment (1-origin screen)
		col++;
		ಸಾಲು++;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '%':				// %%=% literally
		*bufp++='%';
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'n':				// magic DM2500 code
		ಸಾಲು ^= 0140;
		col ^= 0140;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'B':				// bcd encoding
		ಸಾಲು = ಸಾಲು/10<<4+ಸಾಲು%10;
		col = col/10<<4+col%10;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'D':				// magic Delta Data code
		ಸಾಲು = ಸಾಲು-2*(ಸಾಲು&15);
		col = col-2*(col&15);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:				// Unknown escape
		ಪ್ರತ್ಯಾಗಮ "OOPS";
	    }
	}
    }

    ಯದಿ (addup)					// add upline
	ಯದಿ (UP)
	{
	    ptr=UP;
	    ಯಾವತ್ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ಯದಿ (*ptr == '*')
		ptr++;
	    ಯಾವತ್ (*ptr)
		*bufp++ = *ptr++;
	}

    ಯದಿ (addbak)					// add backspace
	ಯದಿ (BC)
	{
	    ptr=BC;
	    ಯಾವತ್ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ಯದಿ (*ptr == '*')
		ptr++;
	    ಯಾವತ್ (*ptr)
		*bufp++ = *ptr++;
	}
	ಅನ್ಯಥಾ
	    *bufp++='\b';

    *bufp = 0;

    ಪ್ರತ್ಯಾಗಮ(buffer);
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

    ಪೂರ್ಣಾಙ್ಕ
tputs(
    ವರ್ಣ *cp,				// string to print
    ಪೂರ್ಣಾಙ್ಕ affcnt,				// Number of lines affected
    ಶೂನ್ಯ (*outc)(unsigned ಪೂರ್ಣಾಙ್ಕ))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ಧ್ರುವ ವರ್ಣ *);

    ಯದಿ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ಯಾವತ್ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	ಯದಿ (*cp == '.')
	    ಯಾವತ್ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	ಯದಿ (*cp!='*')			// multiply by affected lines
	{
	    ಯದಿ (affcnt>1) affcnt = 1;
	}
	ಅನ್ಯಥಾ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ಯದಿ (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	ಯಾವತ್ (*cp)			// output string
	    (*outc)(*cp++);
	ಯದಿ (ospeed)
	    ಯಾವತ್ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    ಅನ್ಯಥಾ
	ಯಾವತ್ (*cp)
	    (*outc)(*cp++);
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_match(ವರ್ಣ *s1, ವರ್ಣ *s2)
{
    ಪೂರ್ಣಾಙ್ಕ i = 0;

    ಯಾವತ್ (s1[i] && s1[i] == s2[i])
	i++;

    ಪ್ರತ್ಯಾಗಮ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ಸ್ಥಿರ ವರ್ಣ *
_find(ವರ್ಣ *s, ವರ್ಣ *set)
{
    ಪ್ರತಿ (; *s; s++)
    {
	ವರ್ಣ	*ptr = set;

	ಯಾವತ್ (*ptr && *s != *ptr)
	    ptr++;

	ಯದಿ (*ptr)
	    ಪ್ರತ್ಯಾಗಮ s;
    }

    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * add val to buf according to format fmt
 */
    ಸ್ಥಿರ ವರ್ಣ *
_addfmt(ವರ್ಣ *buf, ವರ್ಣ *fmt, ಪೂರ್ಣಾಙ್ಕ val)
{
    sprintf(buf, fmt, val);
    ಯಾವತ್ (*buf)
	buf++;
    ಪ್ರತ್ಯಾಗಮ buf;
}
