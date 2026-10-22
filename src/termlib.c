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

#ಆದರೆ !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  getent(ಅಕ್ಷರ *, ಅಕ್ಷರ *, FILE *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  nextent(ಅಕ್ಷರ *, FILE *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ಹೊಂದಿಕೆ_3(ಅಕ್ಷರ *, ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *_addfmt(ಅಕ್ಷರ *, ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಅಕ್ಷರ *ಕಂಡುಹಿಡಿ_3(ಅಕ್ಷರ *, ಅಕ್ಷರ *);

/*
 * Global variables for termlib
 */

ಅಕ್ಷರ	*tent;		      // Pointer to terminal entry, set by tgetent
ಅಕ್ಷರ	PC = 0;		      // Pad character, default NULL
ಅಕ್ಷರ	*ಮೇಲೆ_2 = 0, *BC = 0;     // Pointers to UP and BC strings from database
ಗಿಡ್ಡ	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TERMCAPFILE
# ifdef AMIGA
#  define TERMCAPFILE "s:termcap"
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ಇಲ್ಲದಿದ್ದರೆ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
tgetent(
    ಅಕ್ಷರ    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    ಅಕ್ಷರ    *ಪದ)		// Name of terminal
{
    ಅಕ್ಷರ    tcbuf[32];		// Temp buffer to handle
    ಅಕ್ಷರ    *tcptr = tcbuf;	// extended entries
    ಅಕ್ಷರ    *tcap = TERMCAPFILE; // Default termcap file
    ಅಕ್ಷರ    *tmp;
    FILE    *termcap;
    ಪೂರ್ಣಾಂಕ	    retval = 0;
    ಪೂರ್ಣಾಂಕ	    len;

    ಆದರೆ ((tmp = (ಅಕ್ಷರ *)mch_getenv((char_u *)"TERMCAP")) != ಖಾಲಿ)
    {
	ಆದರೆ (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ಆದರೆ defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ಆದರೆ (tmp)
		*tmp = ':';
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ				// TERMCAP = termcap entry itself
	{
	    ಪೂರ್ಣಾಂಕ tlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪದ);

	    ಆಗ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		ಅಕ್ಷರ *nexttmp;

		ಆಗ (*tmp == '|')
		    tmp++;
		nexttmp  = ಕಂಡುಹಿಡಿ_3(tmp, ":|");	// Rhialto
		ಆದರೆ (tmp+tlen == nexttmp && ಹೊಂದಿಕೆ_3(tmp, ಪದ) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ಹಿಂತಿರುಗಿ 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    tmp = nexttmp;
	    }
	}
    }
    ಆದರೆ (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ಹಿಂತಿರುಗಿ -1;
    }

    len = 0;
    ಆಗ (getent(tbuf + len, ಪದ, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ಆದರೆ ((ಪದ = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tbuf);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    ನಿಲ್ಲಿಸು;
	}
    }
    fclose(termcap);
    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getent(ಅಕ್ಷರ *tbuf, ಅಕ್ಷರ *ಪದ, FILE *termcap, ಪೂರ್ಣಾಂಕ buflen)
{
    ಅಕ್ಷರ    *tptr;
    ಪೂರ್ಣಾಂಕ	    tlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪದ);

    ಆಗ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ಆಗ (*tptr && *tptr != ':')		// : terminates name field
	{
	    ಅಕ್ಷರ    *nexttptr;

	    ಆಗ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = ಕಂಡುಹಿಡಿ_3(tptr, ":|");	// Rhialto
	    ಆದರೆ (tptr + tlen == nexttptr &&
		ಹೊಂದಿಕೆ_3(tptr, ಪದ) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ಹಿಂತಿರುಗಿ 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ				// Look for next name
		tptr = nexttptr;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nextent(ಅಕ್ಷರ *tbuf, FILE *termcap, ಪೂರ್ಣಾಂಕ buflen)
{
    ಅಕ್ಷರ *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ಆಗ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ಪೂರ್ಣಾಂಕ)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ಪೂರ್ಣಾಂಕ llen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(lbuf);

	ಆದರೆ (*lbuf == '#')			// eat comments
	    ಮುಂದುವರಿ;
	ಆದರೆ (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ಆದರೆ (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ಹಿಂತಿರುಗಿ 1;
	}
    }

    ಹಿಂತಿರುಗಿ 0;					// ran into end of file
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

    ಪೂರ್ಣಾಂಕ
tgetflag(ಅಕ್ಷರ *id)
{
    ಅಕ್ಷರ    buf[256], *ptr = buf;

    ಹಿಂತಿರುಗಿ tgetstr(id, &ptr) ? 1 : 0;
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

    ಪೂರ್ಣಾಂಕ
tgetnum(ಅಕ್ಷರ *id)
{
    ಅಕ್ಷರ *ptr, buf[256];
    ptr = buf;

    ಆದರೆ (tgetstr(id, &ptr))
	ಹಿಂತಿರುಗಿ atoi(buf);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 0;
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

    ಅಕ್ಷರ *
tgetstr(ಅಕ್ಷರ *id, ಅಕ್ಷರ **buf)
{
    ಪೂರ್ಣಾಂಕ		len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(id);
    ಅಕ್ಷರ	*tmp=tent;
    ಅಕ್ಷರ	*hold;
    ಪೂರ್ಣಾಂಕ		i;

    ಮಾಡು
    {
	tmp = ಕಂಡುಹಿಡಿ_3(tmp, ":");			// For each field
	ಆಗ (*tmp == ':')			// skip empty fields
	    tmp++;
	ಆದರೆ (!*tmp)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (ಹೊಂದಿಕೆ_3(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ಆದರೆ (*tmp == '@')			// :xx@: entry for tc
		ಹಿಂತಿರುಗಿ 0;			// deleted entry
	    hold= *buf;
	    ಆಗ (*++tmp && *tmp != ':')	// not at end of field
		{
		ಆಯ್ಕೆ(*tmp)
		{
		ಸ್ಥಿತಿ '\\':			// Expand escapes here
		    ಆಯ್ಕೆ(*++tmp)
		    {
		    ಸ್ಥಿತಿ 0:			// ignore backslashes
			tmp--;			// at end of entry
			ನಿಲ್ಲಿಸು;			// shouldn't happen
		    ಸ್ಥಿತಿ 'e':
		    ಸ್ಥಿತಿ 'E':			// ESC
			*(*buf)++ = ESC;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'n':			// \n
			*(*buf)++ = '\n';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'r':			// \r
			*(*buf)++ = '\r';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 't':			// \t
			*(*buf)++ = '\t';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'b':			// \b
			*(*buf)++ = '\b';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'f':			// \f
			*(*buf)++ = '\f';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '0':			// \nnn
		    ಸ್ಥಿತಿ '1':
		    ಸ್ಥಿತಿ '2':
		    ಸ್ಥಿತಿ '3':
		    ಸ್ಥಿತಿ '4':
		    ಸ್ಥಿತಿ '5':
		    ಸ್ಥಿತಿ '6':
		    ಸ್ಥಿತಿ '7':
		    ಸ್ಥಿತಿ '8':
		    ಸ್ಥಿತಿ '9':
			**buf = 0;
			    // get up to three digits
			ಗಾಗಿ (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ಹಿಂತಿರುಗಿ hold;
	}
    } ಆಗ (*tmp);

    ಹಿಂತಿರುಗಿ 0;
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

    ಅಕ್ಷರ *
tgoto(
    ಅಕ್ಷರ    *cm,				// cm string, from termcap
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು,				// column, x position
    ಪೂರ್ಣಾಂಕ	    ಸಾಲು)				// line, y position
{
    ಅಕ್ಷರ    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	ಹಿಮ್ಮುಖ = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ಸ್ಥಿರ ಅಕ್ಷರ ಬಫರ್[32];

    ಆದರೆ (!cm)
	ಹಿಂತಿರುಗಿ "OOPS";				// Kludge, but standard

    bufp = ಬಫರ್;
    ptr = cm;

    ಆಗ (*ptr)
    {
	ಆದರೆ ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{				// % escape
	    ಆಯ್ಕೆ(c = *ptr++)
	    {
	    ಸ್ಥಿತಿ 'd':				// decimal
		bufp = _addfmt(bufp, "%d", ಸಾಲು);
		ಸಾಲು = ಲಂಬಸಾಲು;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", ಸಾಲು);
		ಸಾಲು = ಲಂಬಸಾಲು;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", ಸಾಲು);
		ಸಾಲು = ಲಂಬಸಾಲು;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		ಆದರೆ (ಲಂಬಸಾಲು>gx) ಲಂಬಸಾಲು += gy;
		ಆದರೆ (ಸಾಲು>gx) ಸಾಲು += gy;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '+':				// %+c: add c
		ಸಾಲು += *ptr++;
	    ಸ್ಥಿತಿ '.':				// print x/y
		ಆದರೆ (ಸಾಲು == '\t' ||		// these are
		   ಸಾಲು == '\n' ||		// chars that
		   ಸಾಲು == '\004' ||		// UNIX hates
		   ಸಾಲು == '\0')
		{
		    ಸಾಲು++;			// so go to next pos
		    ಆದರೆ (ಹಿಮ್ಮುಖ == (ಸಾಲು == ಲಂಬಸಾಲು))
			addup=1;		// and mark UP
		    ಇಲ್ಲದಿದ್ದರೆ
			addbak=1;		// or BC
		}
		*bufp++=ಸಾಲು;
		ಸಾಲು = ಲಂಬಸಾಲು;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'r':				// r: reverse
		gx = ಸಾಲು;
		ಸಾಲು = ಲಂಬಸಾಲು;
		ಲಂಬಸಾಲು = gx;
		ಹಿಮ್ಮುಖ = 1;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'i':			// increment (1-origin screen)
		ಲಂಬಸಾಲು++;
		ಸಾಲು++;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '%':				// %%=% literally
		*bufp++='%';
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'n':				// magic DM2500 code
		ಸಾಲು ^= 0140;
		ಲಂಬಸಾಲು ^= 0140;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'B':				// bcd encoding
		ಸಾಲು = ಸಾಲು/10<<4+ಸಾಲು%10;
		ಲಂಬಸಾಲು = ಲಂಬಸಾಲು/10<<4+ಲಂಬಸಾಲು%10;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'D':				// magic Delta Data code
		ಸಾಲು = ಸಾಲು-2*(ಸಾಲು&15);
		ಲಂಬಸಾಲು = ಲಂಬಸಾಲು-2*(ಲಂಬಸಾಲು&15);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:				// Unknown escape
		ಹಿಂತಿರುಗಿ "OOPS";
	    }
	}
    }

    ಆದರೆ (addup)					// add upline
	ಆದರೆ (ಮೇಲೆ_2)
	{
	    ptr=ಮೇಲೆ_2;
	    ಆಗ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ಆದರೆ (*ptr == '*')
		ptr++;
	    ಆಗ (*ptr)
		*bufp++ = *ptr++;
	}

    ಆದರೆ (addbak)					// add backspace
	ಆದರೆ (BC)
	{
	    ptr=BC;
	    ಆಗ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ಆದರೆ (*ptr == '*')
		ptr++;
	    ಆಗ (*ptr)
		*bufp++ = *ptr++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *bufp++='\b';

    *bufp = 0;

    ಹಿಂತಿರುಗಿ(ಬಫರ್);
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

ದೀರ್ಘ _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    ಪೂರ್ಣಾಂಕ
tputs(
    ಅಕ್ಷರ *cp,				// string to print
    ಪೂರ್ಣಾಂಕ affcnt,				// Number of lines affected
    ಶೂನ್ಯ (*outc)(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ))		// routine to output 1 character
{
    ದೀರ್ಘ    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);

    ಆದರೆ (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ಆಗ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (ದೀರ್ಘ)(*cp++ - '0');
	ಆದರೆ (*cp == '.')
	    ಆಗ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (ದೀರ್ಘ)(*cp++ - '0');
		frac = frac * 10;
	    }
	ಆದರೆ (*cp!='*')			// multiply by affected lines
	{
	    ಆದರೆ (affcnt>1) affcnt = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ಆದರೆ (ospeed)
	    counter = (counter * _bauds[ospeed] * (ದೀರ್ಘ)affcnt) / frac;

	ಆಗ (*cp)			// output string
	    (*outc)(*cp++);
	ಆದರೆ (ospeed)
	    ಆಗ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (*cp)
	    (*outc)(*cp++);
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_3(ಅಕ್ಷರ *s1, ಅಕ್ಷರ *s2)
{
    ಪೂರ್ಣಾಂಕ i = 0;

    ಆಗ (s1[i] && s1[i] == s2[i])
	i++;

    ಹಿಂತಿರುಗಿ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
ಕಂಡುಹಿಡಿ_3(ಅಕ್ಷರ *s, ಅಕ್ಷರ *ಗಣ_2)
{
    ಗಾಗಿ (; *s; s++)
    {
	ಅಕ್ಷರ	*ptr = ಗಣ_2;

	ಆಗ (*ptr && *s != *ptr)
	    ptr++;

	ಆದರೆ (*ptr)
	    ಹಿಂತಿರುಗಿ s;
    }

    ಹಿಂತಿರುಗಿ s;
}

/*
 * add val to buf according to format fmt
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
_addfmt(ಅಕ್ಷರ *buf, ಅಕ್ಷರ *fmt, ಪೂರ್ಣಾಂಕ val)
{
    sprintf(buf, fmt, val);
    ಆಗ (*buf)
	buf++;
    ಹಿಂತಿರುಗಿ buf;
}
