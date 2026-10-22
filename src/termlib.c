/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "termlib.pro"

#ଯଦି !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  getent(ଅକ୍ଷର *, ଅକ୍ଷର *, FILE *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  nextent(ଅକ୍ଷର *, FILE *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  _match(ଅକ୍ଷର *, ଅକ୍ଷର *);
ସ୍ଥିର ଅକ୍ଷର *_addfmt(ଅକ୍ଷର *, ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଅକ୍ଷର *_find(ଅକ୍ଷର *, ଅକ୍ଷର *);

/*
 * Global variables for termlib
 */

ଅକ୍ଷର	*tent;		      // Pointer to terminal entry, set by tgetent
ଅକ୍ଷର	PC = 0;		      // Pad character, default NULL
ଅକ୍ଷର	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# ଅନ୍ୟଥା
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  ଅନ୍ୟଥା
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tgetent(
    ଅକ୍ଷର    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    ଅକ୍ଷର    *term)		// Name of terminal
{
    ଅକ୍ଷର    tcbuf[32];		// Temp buffer to handle
    ଅକ୍ଷର    *tcptr = tcbuf;	// extended entries
    ଅକ୍ଷର    *tcap = TERMCAPFILE; // Default termcap file
    ଅକ୍ଷର    *tmp;
    FILE    *termcap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

    ଯଦି ((tmp = (ଅକ୍ଷର *)mch_getenv((char_u *)"TERMCAP")) != ଖାଲି)
    {
	ଯଦି (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#ଯଦି defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    ଯଦି (tmp)
		*tmp = ':';
#endif
	}
	ଅନ୍ୟଥା				// TERMCAP = termcap entry itself
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tlen = strlen(term);

	    ଯେତେବେଳେ (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		ଅକ୍ଷର *nexttmp;

		ଯେତେବେଳେ (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		ଯଦି (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    ଫେରନ୍ତୁ 1;
		}
		ଅନ୍ୟଥା
		    tmp = nexttmp;
	    }
	}
    }
    ଯଦି (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	ଫେରନ୍ତୁ -1;
    }

    len = 0;
    ଯେତେବେଳେ (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	ଯଦି ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	ଅନ୍ୟଥା
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    ଭାଙ୍ଗ;
	}
    }
    fclose(termcap);
    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getent(ଅକ୍ଷର *tbuf, ଅକ୍ଷର *term, FILE *termcap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflen)
{
    ଅକ୍ଷର    *tptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tlen = strlen(term);

    ଯେତେବେଳେ (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	ଯେତେବେଳେ (*tptr && *tptr != ':')		// : terminates name field
	{
	    ଅକ୍ଷର    *nexttptr;

	    ଯେତେବେଳେ (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    ଯଦି (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		ଫେରନ୍ତୁ 1;
	    }
	    ଅନ୍ୟଥା				// Look for next name
		tptr = nexttptr;
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nextent(ଅକ୍ଷର *tbuf, FILE *termcap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflen)
{
    ଅକ୍ଷର *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    ଯେତେବେଳେ (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tbuf+buflen-lbuf), termcap)) // another line
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା llen = strlen(lbuf);

	ଯଦି (*lbuf == '#')			// eat comments
	    ଜାରି;
	ଯଦି (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	ଯଦି (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	ଅନ୍ୟଥା
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    ଫେରନ୍ତୁ 1;
	}
    }

    ଫେରନ୍ତୁ 0;					// ran into end of file
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tgetflag(ଅକ୍ଷର *id)
{
    ଅକ୍ଷର    buf[256], *ptr = buf;

    ଫେରନ୍ତୁ tgetstr(id, &ptr) ? 1 : 0;
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tgetnum(ଅକ୍ଷର *id)
{
    ଅକ୍ଷର *ptr, buf[256];
    ptr = buf;

    ଯଦି (tgetstr(id, &ptr))
	ଫେରନ୍ତୁ atoi(buf);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 0;
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

    ଅକ୍ଷର *
tgetstr(ଅକ୍ଷର *id, ଅକ୍ଷର **buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = strlen(id);
    ଅକ୍ଷର	*tmp=tent;
    ଅକ୍ଷର	*hold;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    କର
    {
	tmp = _find(tmp, ":");			// For each field
	ଯେତେବେଳେ (*tmp == ':')			// skip empty fields
	    tmp++;
	ଯଦି (!*tmp)
	    ଭାଙ୍ଗ;

	ଯଦି (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    ଯଦି (*tmp == '@')			// :xx@: entry for tc
		ଫେରନ୍ତୁ 0;			// deleted entry
	    hold= *buf;
	    ଯେତେବେଳେ (*++tmp && *tmp != ':')	// not at end of field
		{
		ବାଛ(*tmp)
		{
		ସ୍ଥିତି '\\':			// Expand escapes here
		    ବାଛ(*++tmp)
		    {
		    ସ୍ଥିତି 0:			// ignore backslashes
			tmp--;			// at end of entry
			ଭାଙ୍ଗ;			// shouldn't happen
		    ସ୍ଥିତି 'e':
		    ସ୍ଥିତି 'E':			// ESC
			*(*buf)++ = ESC;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'n':			// \n
			*(*buf)++ = '\n';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'r':			// \r
			*(*buf)++ = '\r';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 't':			// \t
			*(*buf)++ = '\t';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'b':			// \b
			*(*buf)++ = '\b';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'f':			// \f
			*(*buf)++ = '\f';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି '0':			// \nnn
		    ସ୍ଥିତି '1':
		    ସ୍ଥିତି '2':
		    ସ୍ଥିତି '3':
		    ସ୍ଥିତି '4':
		    ସ୍ଥିତି '5':
		    ସ୍ଥିତି '6':
		    ସ୍ଥିତି '7':
		    ସ୍ଥିତି '8':
		    ସ୍ଥିତି '9':
			**buf = 0;
			    // get up to three digits
			ପାଇଁ (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    ଫେରନ୍ତୁ hold;
	}
    } ଯେତେବେଳେ (*tmp);

    ଫେରନ୍ତୁ 0;
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

    ଅକ୍ଷର *
tgoto(
    ଅକ୍ଷର    *cm,				// cm string, from termcap
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col,				// column, x position
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଧାଡ଼ି)				// line, y position
{
    ଅକ୍ଷର    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    ସ୍ଥିର ଅକ୍ଷର buffer[32];

    ଯଦି (!cm)
	ଫେରନ୍ତୁ "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    ଯେତେବେଳେ (*ptr)
    {
	ଯଦି ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	ଅନ୍ୟଥା
	{				// % escape
	    ବାଛ(c = *ptr++)
	    {
	    ସ୍ଥିତି 'd':				// decimal
		bufp = _addfmt(bufp, "%d", ଧାଡ଼ି);
		ଧାଡ଼ି = col;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", ଧାଡ଼ି);
		ଧାଡ଼ି = col;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", ଧାଡ଼ି);
		ଧାଡ଼ି = col;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		ଯଦି (col>gx) col += gy;
		ଯଦି (ଧାଡ଼ି>gx) ଧାଡ଼ି += gy;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '+':				// %+c: add c
		ଧାଡ଼ି += *ptr++;
	    ସ୍ଥିତି '.':				// print x/y
		ଯଦି (ଧାଡ଼ି == '\t' ||		// these are
		   ଧାଡ଼ି == '\n' ||		// chars that
		   ଧାଡ଼ି == '\004' ||		// UNIX hates
		   ଧାଡ଼ି == '\0')
		{
		    ଧାଡ଼ି++;			// so go to next pos
		    ଯଦି (reverse == (ଧାଡ଼ି == col))
			addup=1;		// and mark UP
		    ଅନ୍ୟଥା
			addbak=1;		// or BC
		}
		*bufp++=ଧାଡ଼ି;
		ଧାଡ଼ି = col;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'r':				// r: reverse
		gx = ଧାଡ଼ି;
		ଧାଡ଼ି = col;
		col = gx;
		reverse = 1;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'i':			// increment (1-origin screen)
		col++;
		ଧାଡ଼ି++;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '%':				// %%=% literally
		*bufp++='%';
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'n':				// magic DM2500 code
		ଧାଡ଼ି ^= 0140;
		col ^= 0140;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'B':				// bcd encoding
		ଧାଡ଼ି = ଧାଡ଼ି/10<<4+ଧାଡ଼ି%10;
		col = col/10<<4+col%10;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'D':				// magic Delta Data code
		ଧାଡ଼ି = ଧାଡ଼ି-2*(ଧାଡ଼ି&15);
		col = col-2*(col&15);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:				// Unknown escape
		ଫେରନ୍ତୁ "OOPS";
	    }
	}
    }

    ଯଦି (addup)					// add upline
	ଯଦି (UP)
	{
	    ptr=UP;
	    ଯେତେବେଳେ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ଯଦି (*ptr == '*')
		ptr++;
	    ଯେତେବେଳେ (*ptr)
		*bufp++ = *ptr++;
	}

    ଯଦି (addbak)					// add backspace
	ଯଦି (BC)
	{
	    ptr=BC;
	    ଯେତେବେଳେ (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    ଯଦି (*ptr == '*')
		ptr++;
	    ଯେତେବେଳେ (*ptr)
		*bufp++ = *ptr++;
	}
	ଅନ୍ୟଥା
	    *bufp++='\b';

    *bufp = 0;

    ଫେରନ୍ତୁ(buffer);
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tputs(
    ଅକ୍ଷର *cp,				// string to print
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା affcnt,				// Number of lines affected
    ଶୂନ୍ୟ (*outc)(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(ଅଚଳ ଅକ୍ଷର *);

    ଯଦି (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	ଯେତେବେଳେ (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	ଯଦି (*cp == '.')
	    ଯେତେବେଳେ (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	ଯଦି (*cp!='*')			// multiply by affected lines
	{
	    ଯଦି (affcnt>1) affcnt = 1;
	}
	ଅନ୍ୟଥା
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	ଯଦି (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	ଯେତେବେଳେ (*cp)			// output string
	    (*outc)(*cp++);
	ଯଦି (ospeed)
	    ଯେତେବେଳେ (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    ଅନ୍ୟଥା
	ଯେତେବେଳେ (*cp)
	    (*outc)(*cp++);
    ଫେରନ୍ତୁ 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_match(ଅକ୍ଷର *s1, ଅକ୍ଷର *s2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

    ଯେତେବେଳେ (s1[i] && s1[i] == s2[i])
	i++;

    ଫେରନ୍ତୁ i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    ସ୍ଥିର ଅକ୍ଷର *
_find(ଅକ୍ଷର *s, ଅକ୍ଷର *set)
{
    ପାଇଁ (; *s; s++)
    {
	ଅକ୍ଷର	*ptr = set;

	ଯେତେବେଳେ (*ptr && *s != *ptr)
	    ptr++;

	ଯଦି (*ptr)
	    ଫେରନ୍ତୁ s;
    }

    ଫେରନ୍ତୁ s;
}

/*
 * add val to buf according to format fmt
 */
    ସ୍ଥିର ଅକ୍ଷର *
_addfmt(ଅକ୍ଷର *buf, ଅକ୍ଷର *fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
    sprintf(buf, fmt, val);
    ଯେତେବେଳେ (*buf)
	buf++;
    ଫେରନ୍ତୁ buf;
}
