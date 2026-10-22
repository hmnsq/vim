/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#شامل "vim.h"
#شامل "termlib.pro"

#اگر !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#اگر_ختم

ساکن صحیح  getent(حرف *, حرف *, FILE *, صحیح);
ساکن صحیح  nextent(حرف *, FILE *, صحیح);
ساکن صحیح  مطابقت_3(حرف *, حرف *);
ساکن حرف *_addfmt(حرف *, حرف *, صحیح);
ساکن حرف *ڈھونڈو_3(حرف *, حرف *);

/*
 * Global variables for termlib
 */

حرف	*tent;		      // Pointer to terminal entry, set by tgetent
حرف	PC = 0;		      // Pad character, default NULL
حرف	*اوپر_2 = 0, *BC = 0;     // Pointers to UP and BC strings from database
مختصر	ospeed;		      // Baud rate (1-16, 1=300, 16=19200), as in stty

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

#اگر_غیر_تعریف_شدہ TERMCAPFILE
# ifdef AMIGA
#  define TERMCAPFILE "s:termcap"
# ورنہ
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ورنہ
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#اگر_ختم

    صحیح
tgetent(
    حرف    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    حرف    *رکن)		// Name of terminal
{
    حرف    tcbuf[32];		// Temp buffer to handle
    حرف    *tcptr = tcbuf;	// extended entries
    حرف    *tcap = TERMCAPFILE; // Default termcap file
    حرف    *tmp;
    FILE    *termcap;
    صحیح	    retval = 0;
    صحیح	    len;

    اگر ((tmp = (حرف *)mch_getenv((char_u *)"TERMCAP")) != تہی)
    {
	اگر (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#اگر defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    اگر (tmp)
		*tmp = ':';
#اگر_ختم
	}
	ورنہ				// TERMCAP = termcap entry itself
	{
	    صحیح tlen = لڑی_لمبائی(رکن);

	    جب (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		حرف *nexttmp;

		جب (*tmp == '|')
		    tmp++;
		nexttmp  = ڈھونڈو_3(tmp, ":|");	// Rhialto
		اگر (tmp+tlen == nexttmp && مطابقت_3(tmp, رکن) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    واپس 1;
		}
		ورنہ
		    tmp = nexttmp;
	    }
	}
    }
    اگر (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	واپس -1;
    }

    len = 0;
    جب (getent(tbuf + len, رکن, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	اگر ((رکن = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = لڑی_لمبائی(tbuf);
	}
	ورنہ
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    توڑو;
	}
    }
    fclose(termcap);
    واپس retval;
}

    ساکن صحیح
getent(حرف *tbuf, حرف *رکن, FILE *termcap, صحیح buflen)
{
    حرف    *tptr;
    صحیح	    tlen = لڑی_لمبائی(رکن);

    جب (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	جب (*tptr && *tptr != ':')		// : terminates name field
	{
	    حرف    *nexttptr;

	    جب (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = ڈھونڈو_3(tptr, ":|");	// Rhialto
	    اگر (tptr + tlen == nexttptr &&
		مطابقت_3(tptr, رکن) == tlen)	// FOUND!
	    {
		tent = tbuf;
		واپس 1;
	    }
	    ورنہ				// Look for next name
		tptr = nexttptr;
	}
    }
    واپس 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ساکن صحیح
nextent(حرف *tbuf, FILE *termcap, صحیح buflen)
{
    حرف *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    جب (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (صحیح)(tbuf+buflen-lbuf), termcap)) // another line
    {
	صحیح llen = لڑی_لمبائی(lbuf);

	اگر (*lbuf == '#')			// eat comments
	    جاری;
	اگر (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	اگر (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ورنہ
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    واپس 1;
	}
    }

    واپس 0;					// ran into end of file
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

    صحیح
tgetflag(حرف *id)
{
    حرف    buf[256], *ptr = buf;

    واپس tgetstr(id, &ptr) ? 1 : 0;
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

    صحیح
tgetnum(حرف *id)
{
    حرف *ptr, buf[256];
    ptr = buf;

    اگر (tgetstr(id, &ptr))
	واپس atoi(buf);
    ورنہ
	واپس 0;
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
    صحیح		len = لڑی_لمبائی(id);
    حرف	*tmp=tent;
    حرف	*hold;
    صحیح		i;

    کرو
    {
	tmp = ڈھونڈو_3(tmp, ":");			// For each field
	جب (*tmp == ':')			// skip empty fields
	    tmp++;
	اگر (!*tmp)
	    توڑو;

	اگر (مطابقت_3(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    اگر (*tmp == '@')			// :xx@: entry for tc
		واپس 0;			// deleted entry
	    hold= *buf;
	    جب (*++tmp && *tmp != ':')	// not at end of field
		{
		انتخاب(*tmp)
		{
		حالت '\\':			// Expand escapes here
		    انتخاب(*++tmp)
		    {
		    حالت 0:			// ignore backslashes
			tmp--;			// at end of entry
			توڑو;			// shouldn't happen
		    حالت 'e':
		    حالت 'E':			// ESC
			*(*buf)++ = ESC;
			توڑو;
		    حالت 'n':			// \n
			*(*buf)++ = '\n';
			توڑو;
		    حالت 'r':			// \r
			*(*buf)++ = '\r';
			توڑو;
		    حالت 't':			// \t
			*(*buf)++ = '\t';
			توڑو;
		    حالت 'b':			// \b
			*(*buf)++ = '\b';
			توڑو;
		    حالت 'f':			// \f
			*(*buf)++ = '\f';
			توڑو;
		    حالت '0':			// \nnn
		    حالت '1':
		    حالت '2':
		    حالت '3':
		    حالت '4':
		    حالت '5':
		    حالت '6':
		    حالت '7':
		    حالت '8':
		    حالت '9':
			**buf = 0;
			    // get up to three digits
			برائے (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			توڑو;
		    طےشدہ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    توڑو;
		حالت '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    توڑو;
		طےشدہ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    واپس hold;
	}
    } جب (*tmp);

    واپس 0;
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
    صحیح	    ستون,				// column, x position
    صحیح	    سطر)				// line, y position
{
    حرف    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	الٹ = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ساکن حرف بفر[32];

    اگر (!cm)
	واپس "OOPS";				// Kludge, but standard

    bufp = بفر;
    ptr = cm;

    جب (*ptr)
    {
	اگر ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ورنہ
	{				// % escape
	    انتخاب(c = *ptr++)
	    {
	    حالت 'd':				// decimal
		bufp = _addfmt(bufp, "%d", سطر);
		سطر = ستون;
		توڑو;
	    حالت '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", سطر);
		سطر = ستون;
		توڑو;
	    حالت '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", سطر);
		سطر = ستون;
		توڑو;
	    حالت '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		اگر (ستون>gx) ستون += gy;
		اگر (سطر>gx) سطر += gy;
		توڑو;
	    حالت '+':				// %+c: add c
		سطر += *ptr++;
	    حالت '.':				// print x/y
		اگر (سطر == '\t' ||		// these are
		   سطر == '\n' ||		// chars that
		   سطر == '\004' ||		// UNIX hates
		   سطر == '\0')
		{
		    سطر++;			// so go to next pos
		    اگر (الٹ == (سطر == ستون))
			addup=1;		// and mark UP
		    ورنہ
			addbak=1;		// or BC
		}
		*bufp++=سطر;
		سطر = ستون;
		توڑو;
	    حالت 'r':				// r: reverse
		gx = سطر;
		سطر = ستون;
		ستون = gx;
		الٹ = 1;
		توڑو;
	    حالت 'i':			// increment (1-origin screen)
		ستون++;
		سطر++;
		توڑو;
	    حالت '%':				// %%=% literally
		*bufp++='%';
		توڑو;
	    حالت 'n':				// magic DM2500 code
		سطر ^= 0140;
		ستون ^= 0140;
		توڑو;
	    حالت 'B':				// bcd encoding
		سطر = سطر/10<<4+سطر%10;
		ستون = ستون/10<<4+ستون%10;
		توڑو;
	    حالت 'D':				// magic Delta Data code
		سطر = سطر-2*(سطر&15);
		ستون = ستون-2*(ستون&15);
		توڑو;
	    طےشدہ:				// Unknown escape
		واپس "OOPS";
	    }
	}
    }

    اگر (addup)					// add upline
	اگر (اوپر_2)
	{
	    ptr=اوپر_2;
	    جب (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    اگر (*ptr == '*')
		ptr++;
	    جب (*ptr)
		*bufp++ = *ptr++;
	}

    اگر (addbak)					// add backspace
	اگر (BC)
	{
	    ptr=BC;
	    جب (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    اگر (*ptr == '*')
		ptr++;
	    جب (*ptr)
		*bufp++ = *ptr++;
	}
	ورنہ
	    *bufp++='\b';

    *bufp = 0;

    واپس(بفر);
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

طویل _bauds[16]={
    0,	50, 75,	110,
    134,    150,    200,    300,
    600,    1200,   1800,   2400,
    4800,   9600,   19200,  19200 };

    صحیح
tputs(
    حرف *cp,				// string to print
    صحیح affcnt,				// Number of lines affected
    خالی (*outc)(غیرعلامتی صحیح))		// routine to output 1 character
{
    طویل    frac,			// 10^(#digits after decimal point)
	شمار_2,			// digits
	atol(ثابت حرف *);

    اگر (VIM_ISDIGIT(*cp))
    {
	شمار_2 = 0;
	frac = 1000;
	جب (VIM_ISDIGIT(*cp))
	    شمار_2 = شمار_2 * 10L + (طویل)(*cp++ - '0');
	اگر (*cp == '.')
	    جب (VIM_ISDIGIT(*++cp))
	    {
		شمار_2 = شمار_2 * 10L + (طویل)(*cp++ - '0');
		frac = frac * 10;
	    }
	اگر (*cp!='*')			// multiply by affected lines
	{
	    اگر (affcnt>1) affcnt = 1;
	}
	ورنہ
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	اگر (ospeed)
	    شمار_2 = (شمار_2 * _bauds[ospeed] * (طویل)affcnt) / frac;

	جب (*cp)			// output string
	    (*outc)(*cp++);
	اگر (ospeed)
	    جب (شمار_2--)		// followed by pad characters
		(*outc)(PC);
    }
    ورنہ
	جب (*cp)
	    (*outc)(*cp++);
    واپس 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ساکن صحیح
مطابقت_3(حرف *s1, حرف *s2)
{
    صحیح i = 0;

    جب (s1[i] && s1[i] == s2[i])
	i++;

    واپس i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ساکن حرف *
ڈھونڈو_3(حرف *s, حرف *سیٹ_2)
{
    برائے (; *s; s++)
    {
	حرف	*ptr = سیٹ_2;

	جب (*ptr && *s != *ptr)
	    ptr++;

	اگر (*ptr)
	    واپس s;
    }

    واپس s;
}

/*
 * add val to buf according to format fmt
 */
    ساکن حرف *
_addfmt(حرف *buf, حرف *fmt, صحیح val)
{
    sprintf(buf, fmt, val);
    جب (*buf)
	buf++;
    واپس buf;
}
