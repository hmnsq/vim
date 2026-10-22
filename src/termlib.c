/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * The following software is (C) 1984 Peter da Silva, the Mad Australian, in
 * the public domain. It may be re-distributed for any purpose with the
 * inclusion of this notice.
 */

// Modified by Bram Moolenaar for use with VIM - Vi Improved.
// A few bugs removed by Olaf 'Rhialto' Seibert.

// TERMLIB: Terminal independent database.

#include "vim.h"
#include "termlib.pro"

#만약 !defined(AMIGA) && !defined(VMS)
# include <sgtty.h>
#endif

static 정수  getent(char *, char *, FILE *, 정수);
static 정수  nextent(char *, FILE *, 정수);
static 정수  _match(char *, char *);
static char *_addfmt(char *, char *, 정수);
static char *_find(char *, char *);

/*
 * Global variables for termlib
 */

char	*tent;		      // Pointer to terminal entry, set by tgetent
char	PC = 0;		      // Pad character, default NULL
char	*UP = 0, *BC = 0;     // Pointers to UP and BC strings from database
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
# 아니면
#  ifdef VMS
#   define TERMCAPFILE "VIMRUNTIME:termcap"
#  아니면
#   define TERMCAPFILE "/etc/termcap"
#  endif
# endif
#endif

    정수
tgetent(
    char    *tbuf,		// Buffer to hold termcap entry, TBUFSZ bytes max
    char    *term)		// Name of terminal
{
    char    tcbuf[32];		// Temp buffer to handle
    char    *tcptr = tcbuf;	// extended entries
    char    *tcap = TERMCAPFILE; // Default termcap file
    char    *tmp;
    FILE    *termcap;
    정수	    retval = 0;
    정수	    len;

    만약 ((tmp = (char *)mch_getenv((char_u *)"TERMCAP")) != NULL)
    {
	만약 (*tmp == '/')		// TERMCAP = name of termcap file
	{
	    tcap = tmp ;
#만약 defined(AMIGA)
	    // Convert /usr/share/lib/termcap to usr:share/lib/termcap
	    tcap++;
	    tmp = strchr(tcap, '/');
	    만약 (tmp)
		*tmp = ':';
#endif
	}
	아니면				// TERMCAP = termcap entry itself
	{
	    정수 tlen = strlen(term);

	    동안 (*tmp && *tmp != ':')		// Check if TERM matches
	    {
		char *nexttmp;

		동안 (*tmp == '|')
		    tmp++;
		nexttmp  = _find(tmp, ":|");	// Rhialto
		만약 (tmp+tlen == nexttmp && _match(tmp, term) == tlen)
		{
		    vim_strncpy(tbuf, tmp, TBUFSZ - 1);
		    tent = tbuf;
		    반환 1;
		}
		아니면
		    tmp = nexttmp;
	    }
	}
    }
    만약 (!(termcap = mch_fopen(tcap, "r")))
    {
	vim_strncpy(tbuf, tcap, TBUFSZ - 1);
	반환 -1;
    }

    len = 0;
    동안 (getent(tbuf + len, term, termcap, TBUFSZ - len))
    {
	tcptr = tcbuf;				// Rhialto
	만약 ((term = tgetstr("tc", &tcptr)))	// extended entry
	{
	    rewind(termcap);
	    len = strlen(tbuf);
	}
	아니면
	{
	    retval = 1;
	    tent = tbuf;	// reset it back to the beginning
	    break;
	}
    }
    fclose(termcap);
    반환 retval;
}

    static 정수
getent(char *tbuf, char *term, FILE *termcap, 정수 buflen)
{
    char    *tptr;
    정수	    tlen = strlen(term);

    동안 (nextent(tbuf, termcap, buflen))	// For each possible entry
    {
	tptr = tbuf;
	동안 (*tptr && *tptr != ':')		// : terminates name field
	{
	    char    *nexttptr;

	    동안 (*tptr == '|')		// | separates names
		tptr++;
	    nexttptr = _find(tptr, ":|");	// Rhialto
	    만약 (tptr + tlen == nexttptr &&
		_match(tptr, term) == tlen)	// FOUND!
	    {
		tent = tbuf;
		반환 1;
	    }
	    아니면				// Look for next name
		tptr = nexttptr;
	}
    }
    반환 0;
}

/*
 * Read 1 entry from TERMCAP file.
 */
    static 정수
nextent(char *tbuf, FILE *termcap, 정수 buflen)
{
    char *lbuf = tbuf;				// lbuf=line buffer
				// read lines straight into buffer

    동안 (lbuf < tbuf+buflen &&		// There's room and
	  fgets(lbuf, (정수)(tbuf+buflen-lbuf), termcap)) // another line
    {
	정수 llen = strlen(lbuf);

	만약 (*lbuf == '#')			// eat comments
	    continue;
	만약 (lbuf[-1] == ':' &&			// and whitespace
	    lbuf[0] == '\t' &&
	    lbuf[1] == ':')
	{
	    STRMOVE(lbuf, lbuf + 2);
	    llen -= 2;
	}
	만약 (lbuf[llen-2] == '\\')		// and continuations
	    lbuf += llen-2;
	아니면
	{
	    lbuf[llen-1]=0;			// no continuation, return
	    반환 1;
	}
    }

    반환 0;					// ran into end of file
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

    정수
tgetflag(char *id)
{
    char    buf[256], *ptr = buf;

    반환 tgetstr(id, &ptr) ? 1 : 0;
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

    정수
tgetnum(char *id)
{
    char *ptr, buf[256];
    ptr = buf;

    만약 (tgetstr(id, &ptr))
	반환 atoi(buf);
    아니면
	반환 0;
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

    char *
tgetstr(char *id, char **buf)
{
    정수		len = strlen(id);
    char	*tmp=tent;
    char	*hold;
    정수		i;

    do
    {
	tmp = _find(tmp, ":");			// For each field
	동안 (*tmp == ':')			// skip empty fields
	    tmp++;
	만약 (!*tmp)
	    break;

	만약 (_match(id, tmp) == len)
	{
	    tmp += len;				// find '=' '@' or '#'
	    만약 (*tmp == '@')			// :xx@: entry for tc
		반환 0;			// deleted entry
	    hold= *buf;
	    동안 (*++tmp && *tmp != ':')	// not at end of field
		{
		switch(*tmp)
		{
		case '\\':			// Expand escapes here
		    switch(*++tmp)
		    {
		    case 0:			// ignore backslashes
			tmp--;			// at end of entry
			break;			// shouldn't happen
		    case 'e':
		    case 'E':			// ESC
			*(*buf)++ = ESC;
			break;
		    case 'n':			// \n
			*(*buf)++ = '\n';
			break;
		    case 'r':			// \r
			*(*buf)++ = '\r';
			break;
		    case 't':			// \t
			*(*buf)++ = '\t';
			break;
		    case 'b':			// \b
			*(*buf)++ = '\b';
			break;
		    case 'f':			// \f
			*(*buf)++ = '\f';
			break;
		    case '0':			// \nnn
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			**buf = 0;
			    // get up to three digits
			위해 (i = 0; i < 3 && VIM_ISDIGIT(*tmp); ++i)
			    **buf = **buf * 8 + *tmp++ - '0';
			(*buf)++;
			tmp--;
			break;
		    default:			// \x, for all other x
			*(*buf)++= *tmp;
		    }
		    break;
		case '^':			// control characters
		    ++tmp;
		    *(*buf)++ = Ctrl_chr(*tmp);
		    break;
		default:
		    *(*buf)++ = *tmp;
		}
	    }
	    *(*buf)++ = 0;
	    반환 hold;
	}
    } 동안 (*tmp);

    반환 0;
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

    char *
tgoto(
    char    *cm,				// cm string, from termcap
    정수	    col,				// column, x position
    정수	    line)				// line, y position
{
    char    gx, gy,				// x, y
	*ptr,					// pointer in 'cm'
	reverse = 0,				// reverse flag
	*bufp,					// pointer in returned string
	addup = 0,				// add upline
	addbak = 0,				// add backup
	c;
    static char buffer[32];

    만약 (!cm)
	반환 "OOPS";				// Kludge, but standard

    bufp = buffer;
    ptr = cm;

    동안 (*ptr)
    {
	만약 ((c = *ptr++) != '%')		// normal char
	{
	    *bufp++ = c;
	}
	아니면
	{				// % escape
	    switch(c = *ptr++)
	    {
	    case 'd':				// decimal
		bufp = _addfmt(bufp, "%d", line);
		line = col;
		break;
	    case '2':				// 2 digit decimal
		bufp = _addfmt(bufp, "%02d", line);
		line = col;
		break;
	    case '3':				// 3 digit decimal
		bufp = _addfmt(bufp, "%03d", line);
		line = col;
		break;
	    case '>':				// %>xy: if >x, add y
		gx = *ptr++;
		gy = *ptr++;
		만약 (col>gx) col += gy;
		만약 (line>gx) line += gy;
		break;
	    case '+':				// %+c: add c
		line += *ptr++;
	    case '.':				// print x/y
		만약 (line == '\t' ||		// these are
		   line == '\n' ||		// chars that
		   line == '\004' ||		// UNIX hates
		   line == '\0')
		{
		    line++;			// so go to next pos
		    만약 (reverse == (line == col))
			addup=1;		// and mark UP
		    아니면
			addbak=1;		// or BC
		}
		*bufp++=line;
		line = col;
		break;
	    case 'r':				// r: reverse
		gx = line;
		line = col;
		col = gx;
		reverse = 1;
		break;
	    case 'i':			// increment (1-origin screen)
		col++;
		line++;
		break;
	    case '%':				// %%=% literally
		*bufp++='%';
		break;
	    case 'n':				// magic DM2500 code
		line ^= 0140;
		col ^= 0140;
		break;
	    case 'B':				// bcd encoding
		line = line/10<<4+line%10;
		col = col/10<<4+col%10;
		break;
	    case 'D':				// magic Delta Data code
		line = line-2*(line&15);
		col = col-2*(col&15);
		break;
	    default:				// Unknown escape
		반환 "OOPS";
	    }
	}
    }

    만약 (addup)					// add upline
	만약 (UP)
	{
	    ptr=UP;
	    동안 (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    만약 (*ptr == '*')
		ptr++;
	    동안 (*ptr)
		*bufp++ = *ptr++;
	}

    만약 (addbak)					// add backspace
	만약 (BC)
	{
	    ptr=BC;
	    동안 (VIM_ISDIGIT(*ptr) || *ptr == '.')
		ptr++;
	    만약 (*ptr == '*')
		ptr++;
	    동안 (*ptr)
		*bufp++ = *ptr++;
	}
	아니면
	    *bufp++='\b';

    *bufp = 0;

    반환(buffer);
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

    정수
tputs(
    char *cp,				// string to print
    정수 affcnt,				// Number of lines affected
    void (*outc)(unsigned 정수))		// routine to output 1 character
{
    long    frac,			// 10^(#digits after decimal point)
	counter,			// digits
	atol(const char *);

    만약 (VIM_ISDIGIT(*cp))
    {
	counter = 0;
	frac = 1000;
	동안 (VIM_ISDIGIT(*cp))
	    counter = counter * 10L + (long)(*cp++ - '0');
	만약 (*cp == '.')
	    동안 (VIM_ISDIGIT(*++cp))
	    {
		counter = counter * 10L + (long)(*cp++ - '0');
		frac = frac * 10;
	    }
	만약 (*cp!='*')			// multiply by affected lines
	{
	    만약 (affcnt>1) affcnt = 1;
	}
	아니면
	    cp++;

	// Calculate number of characters for padding counter/frac ms delay
	만약 (ospeed)
	    counter = (counter * _bauds[ospeed] * (long)affcnt) / frac;

	동안 (*cp)			// output string
	    (*outc)(*cp++);
	만약 (ospeed)
	    동안 (counter--)		// followed by pad characters
		(*outc)(PC);
    }
    아니면
	동안 (*cp)
	    (*outc)(*cp++);
    반환 0;
}

/*
 * Module: tutil.c
 *
 * Purpose: Utility routines for TERMLIB functions.
 * Returns length of text common to s1 and s2.
 */
    static 정수
_match(char *s1, char *s2)
{
    정수 i = 0;

    동안 (s1[i] && s1[i] == s2[i])
	i++;

    반환 i;
}

/*
 * finds next c in s that's a member of set, returns pointer
 */
    static char *
_find(char *s, char *set)
{
    위해 (; *s; s++)
    {
	char	*ptr = set;

	동안 (*ptr && *s != *ptr)
	    ptr++;

	만약 (*ptr)
	    반환 s;
    }

    반환 s;
}

/*
 * add val to buf according to format fmt
 */
    static char *
_addfmt(char *buf, char *fmt, 정수 val)
{
    sprintf(buf, fmt, val);
    동안 (*buf)
	buf++;
    반환 buf;
}
