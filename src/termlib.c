/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#ضمن "vim.h"
#ضمن "termlib.pro"

#إذا !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#پایان_اگر

ساكن صحيح  getent(حرف *, حرف *, FILE *, صحيح);
ساكن صحيح  nextent(حرف *, FILE *, صحيح);
ساكن صحيح  تطابق_3(حرف *, حرف *);
ساكن حرف *_addfmt(حرف *, حرف *, صحيح);
ساكن حرف *پیدا_کن_3(حرف *, حرف *);

/*
 * Global variables for termlib
 */

حرف	*tent;		      // Pointer to terminal entry, set by tgetent
حرف	PC = 0;		      // Pad character, default NULL
حرف	*به_بالا_2 = 0, *BC = 0;     // Pointers to UP and BC strings from database
کوتاه	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

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

#اگر_تعریف_نشده TERMCAPFILE
# ifdef AMIGA
#  define TERMCAPFILE "s:termcap"
# وإلا
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  وإلا
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#پایان_اگر

    صحيح
tgetent(
    حرف    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    حرف    *جمله)		// Name of terminal
{
    حرف    tcbuf[32];		// Temp buffer to handle
    حرف    *tcptr = tcbuf;	// extended entries
    حرف    *tcap = TERMCAPFILE; // Default termcap file
    حرف    *tmp;
    FILE    *termcap;
    صحيح	    retval = 0;
    صحيح	    len;

    إذا ((tmp = (حرف *)mch_getenv((char_u *)"TERMCAP")) != عدم)
    {
	إذا (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#إذا defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    إذا (tmp)
		*tmp = ':';
#پایان_اگر
	}
	وإلا				// TERMCAP = termcap entry itself
	{
	    صحيح tlen = طول_رشته(جمله);

	    طالما (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		حرف *nexttmp;

		طالما (*tmp == '|')
		    tmp++;
		nexttmp  = پیدا_کن_3(tmp, ":|");	// Rhialto
		إذا (tmp+tlen == nexttmp && تطابق_3(tmp, جمله) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    أرجع 1;
		}
		وإلا
		    tmp = nexttmp;
	    }
	}
    }
    إذا (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	أرجع -1;
    }

    len = 0;
    طالما (getent(tbuf + len, جمله, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	إذا ((جمله = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = طول_رشته(tbuf);
	}
	وإلا
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    اكسر;
	}
    }
    fclose(termcap);
    أرجع retval;
}

    ساكن صحيح
getent(حرف *tbuf, حرف *جمله, FILE *termcap, صحيح buflen)
{
    حرف    *tptr;
    صحيح	    tlen = طول_رشته(جمله);

    طالما (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	طالما (*tptr && *tptr != ':')		// : terminates name field
	{
	    حرف    *nexttptr;

	    طالما (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = پیدا_کن_3(tptr, ":|");	// Rhialto
	    إذا (tptr + tlen == nexttptr &&
		تطابق_3(tptr, جمله) == tlen)	// FOUND!
	    {
		tent = tbuf;
		أرجع 1;
	    }
	    وإلا				// Look for next name
		tptr = nexttptr;
	}
    }
    أرجع 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ساكن صحيح
nextent(حرف *tbuf, FILE *termcap, صحيح buflen)
{
    حرف *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    طالما (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (صحيح)(tbuf+buflen-lbuf), termcap)) // another line
    {
	صحيح llen = طول_رشته(lbuf);

	إذا (*lbuf == '#')			// eat comments
	    استمر;
	إذا (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	إذا (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	وإلا
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    أرجع 1;
	}
    }

    أرجع 0;					// ran into end of file
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

    صحيح
tgetflag(حرف *id)
{
    حرف    buf[256], *ptr = buf;

    أرجع tgetstr(id, &ptr) ? 1 : 0;
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

    صحيح
tgetnum(حرف *id)
{
    حرف *ptr, buf[256];
    ptr = buf;

    إذا (tgetstr(id, &ptr))
	أرجع atoi(buf);
    وإلا
	أرجع 0;
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

    حرف *
tgetstr(حرف *id, حرف **buf)
{
    صحيح		len = طول_رشته(id);
    حرف	*tmp=tent;
    حرف	*hold;
    صحيح		i;

    افعل
    {
	tmp = پیدا_کن_3(tmp, ":");			// For each field
	طالما (*tmp == ':')			// skip empty fields
	    tmp++;
	إذا (!*tmp)
	    اكسر;

	إذا (تطابق_3(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    إذا (*tmp == '@')			// :xx@: entry for tc
		أرجع 0;			// deleted entry
	    hold= *buf;
	    طالما (*++tmp && *tmp != ':')	// not at end of field
		{
		بدل(*tmp)
		{
		حالة '\\':			// Expand escapes here
		    بدل(*++tmp)
		    {
		    حالة 0:			// ignore backslashes
			tmp--;			// at end of entry
			اكسر;			// shouldn't happen
		    حالة 'e':
		    حالة 'E':			// ESC
			*(*buf)++ = ESC;
			اكسر;
		    حالة 'n':			// \n
			*(*buf)++ = '\n';
			اكسر;
		    حالة 'r':			// \r
			*(*buf)++ = '\r';
			اكسر;
		    حالة 't':			// \t
			*(*buf)++ = '\t';
			اكسر;
		    حالة 'b':			// \b
			*(*buf)++ = '\b';
			اكسر;
		    حالة 'f':			// \f
			*(*buf)++ = '\f';
			اكسر;
		    حالة '0':			// \nnn
		    حالة '1':
		    حالة '2':
		    حالة '3':
		    حالة '4':
		    حالة '5':
		    حالة '6':
		    حالة '7':
		    حالة '8':
		    حالة '9':
			**buf = 0;
			    // get up to three digits
			لكل (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			اكسر;
		    افتراضي:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    اكسر;
		حالة '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    اكسر;
		افتراضي:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    أرجع hold;
	}
    } طالما (*tmp);

    أرجع 0;
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

    حرف *
tgoto(
    حرف    *cm,				// cm string, from termcap
    صحيح	    ستون,				// column, x position
    صحيح	    سطر)				// line, y position
{
    حرف    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	معکوس = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ساكن حرف میانگیر[32];

    إذا (!cm)
	أرجع "OOPS";				// Kludge, but standard

    bufp = میانگیر;
    ptr = cm;

    طالما (*ptr)
    {
	إذا ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	وإلا
	{				// % escape
	    بدل(c = *ptr++)
	    {
	    حالة 'd':				// decimal
		bufp = _addfmt(bufp, "%d", سطر);
		سطر = ستون;
		اكسر;
	    حالة '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", سطر);
		سطر = ستون;
		اكسر;
	    حالة '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", سطر);
		سطر = ستون;
		اكسر;
	    حالة '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		إذا (ستون>gx) ستون += gy;
		إذا (سطر>gx) سطر += gy;
		اكسر;
	    حالة '+':				// %+c: add c
		سطر += *ptr++;
	    حالة '.':				// print x/y
		إذا (سطر == '\t' ||		// these are
		   سطر == '\n' ||		// chars that
		   سطر == '\004' ||		// UNIX hates
		   سطر == '\0')
		{
		    سطر++;			// so go to next pos
		    إذا (معکوس == (سطر == ستون))
			addup=1;		// and mark UP
		    وإلا
			addbak=1;		// or BC
		}
		*bufp++=سطر;
		سطر = ستون;
		اكسر;
	    حالة 'r':				// r: reverse
		gx = سطر;
		سطر = ستون;
		ستون = gx;
		معکوس = 1;
		اكسر;
	    حالة 'i':			// increment (1-origin screen)
		ستون++;
		سطر++;
		اكسر;
	    حالة '%':				// %%=% literally
		*bufp++='%';
		اكسر;
	    حالة 'n':				// magic DM2500 code
		سطر ^= 0140;
		ستون ^= 0140;
		اكسر;
	    حالة 'B':				// bcd encoding
		سطر = سطر/10<<4+سطر%10;
		ستون = ستون/10<<4+ستون%10;
		اكسر;
	    حالة 'D':				// magic Delta Data code
		سطر = سطر-2*(سطر&15);
		ستون = ستون-2*(ستون&15);
		اكسر;
	    افتراضي:				// Unknown escape
		أرجع "OOPS";
	    }
	}
    }

    إذا (addup)					// add upline
	إذا (به_بالا_2)
	{
	    ptr=به_بالا_2;
	    طالما (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    إذا (*ptr == '*')
		ptr++;
	    طالما (*ptr)
		*bufp++ = *ptr++;
	}

    إذا (addbak)					// add backspace
	إذا (BC)
	{
	    ptr=BC;
	    طالما (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    إذا (*ptr == '*')
		ptr++;
	    طالما (*ptr)
		*bufp++ = *ptr++;
	}
	وإلا
	    *bufp++='\b';

    *bufp = 0;

    أرجع(میانگیر);
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

دراز _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    صحيح
tputs(
    حرف *cp,				// string to print
    صحيح affcnt,				// Number of lines affected
    فراغ (*outc)(بیعلامت صحيح))		// routine to output 1 character
{
    دراز    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ثابت حرف *);

    إذا (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	طالما (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (دراز)(*cp++ - '0');
	إذا (*cp == '.')
	    طالما (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (دراز)(*cp++ - '0');
		frac = frac * 10;
	    }
	إذا (*cp!='*')			// multiply by affected lines
	{
	    إذا (affcnt>1) affcnt = 1;
	}
	وإلا
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	إذا (ospeed)
	    counter = (counter * _bauds[ospeed] * (دراز)affcnt) / frac;

	طالما (*cp)			// output string
	    (*outc)(*cp++);
	إذا (ospeed)
	    طالما (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    وإلا
	طالما (*cp)
	    (*outc)(*cp++);
    أرجع 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ساكن صحيح
تطابق_3(حرف *s1, حرف *s2)
{
    صحيح i = 0;

    طالما (s1[i] && s1[i] == s2[i])
	i++;

    أرجع i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ساكن حرف *
پیدا_کن_3(حرف *s, حرف *مجموعه_2)
{
    لكل (; *s; s++)
    {
	حرف	*ptr = مجموعه_2;

	طالما (*ptr && *s != *ptr)
	    ptr++;

	إذا (*ptr)
	    أرجع s;
    }

    أرجع s;
}

/*
 * add val to buf according to format fmt
 */
    ساكن حرف *
_addfmt(حرف *buf, حرف *fmt, صحيح val)
{
    sprintf(buf, fmt, val);
    طالما (*buf)
	buf++;
    أرجع buf;
}
