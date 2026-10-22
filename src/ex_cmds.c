/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_cmds.c: some functions for command line commands
 */

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#અન્તર્ભાવ <float.h>

સ્થિર પૂર્ણાંક linelen(પૂર્ણાંક *has_tab);
સ્થિર શૂન્ય do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, પૂર્ણાંક do_in, પૂર્ણાંક do_out);
સ્થિર પૂર્ણાંક not_writing(શૂન્ય);
સ્થિર પૂર્ણાંક check_readonly(પૂર્ણાંક *forceit, buf_T *buf);
સ્થિર શૂન્ય delbuf_સંદેશ(char_u *name);

/*
 * ":ascii" and "ga".
 */
    શૂન્ય
do_ascii(exarg_T *eap UNUSED)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		cval;
    અક્ષર	buf1[20];
    અક્ષર	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    પૂર્ણાંક		cc[MAX_MCO];
    પૂર્ણાંક		ci = 0;
    પૂર્ણાંક		len;

    જો (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    નહીંતર
	c = gchar_cursor();
    જો (c == NUL)
    {
	સંદેશ("NUL");
	પાછા;
    }

    IObuff[0] = NUL;
    જો (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	જો (c == NL)	    // NUL is stored as NL
	    c = NUL;
	જો (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	નહીંતર
	    cval = c;
	જો (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, કદ(buf1), "  <%s>", (અક્ષર *)buf3);
	}
	નહીંતર
	    buf1[0] = NUL;
	જો (c >= 0x80)
	    vim_snprintf(buf2, કદ(buf2), "  <M-%s>",
						 (અક્ષર *)transchar(c & 0x7f));
	નહીંતર
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	જો (dig != ખાલી)
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	નહીંતર
#endif
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	જો (enc_utf8)
	    c = cc[ci++];
	નહીંતર
	    c = 0;
    }

    // Repeat for combining characters.
    જ્યારે (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (પૂર્ણાંક)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	જો (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	જો (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	જો (dig != ખાલી)
	    vim_snprintf((અક્ષર *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	નહીંતર
#endif
	    vim_snprintf((અક્ષર *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	જો (ci == MAX_MCO)
	    તોડો;
	જો (enc_utf8)
	    c = cc[ci++];
	નહીંતર
	    c = 0;
    }

    સંદેશ((અક્ષર *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    શૂન્ય
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    પૂર્ણાંક		len;
    પૂર્ણાંક		indent = 0;
    પૂર્ણાંક		new_indent;
    પૂર્ણાંક		has_tab;
    પૂર્ણાંક		width;

#ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl)
    {
	// switch left and right aligning
	જો (eap->cmdidx == CMD_જમણું)
	    eap->cmdidx = CMD_ડાબું;
	નહીંતર જો (eap->cmdidx == CMD_ડાબું)
	    eap->cmdidx = CMD_જમણું;
    }
#endif

    width = atoi((અક્ષર *)eap->arg);
    save_curpos = curwin->w_cursor;
    જો (eap->cmdidx == CMD_ડાબું)    // width is used for new indent
    {
	જો (width >= 0)
	    indent = width;
    }
    નહીંતર
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	જો (width <= 0)
	    width = curbuf->b_p_tw;
	જો (width == 0 && curbuf->b_p_wm > 0)
	    width = curwin->w_width - curbuf->b_p_wm;
	જો (width <= 0)
	    width = 80;
    }

    જો (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	પાછા;

    માટે (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	જો (eap->cmdidx == CMD_ડાબું)		// left align
	    new_indent = indent;
	નહીંતર
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_જમણું ? &has_tab
						   : ખાલી) - get_indent();

	    જો (len <= 0)			// skip blank lines
		ચાલુ;

	    જો (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    નહીંતર
	    {
		new_indent = width - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		જો (has_tab)
		    જ્યારે (new_indent > 0)
		    {
			(શૂન્ય)set_indent(new_indent, 0);
			જો (linelen(ખાલી) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    કરો
				(શૂન્ય)set_indent(++new_indent, 0);
			    જ્યારે (linelen(ખાલી) <= width);
			    --new_indent;
			    તોડો;
			}
			--new_indent;
		    }
	    }
	}
	જો (new_indent < 0)
	    new_indent = 0;
	(શૂન્ય)set_indent(new_indent, 0);		// set indent
    }
    changed_લીટીઓ(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    સ્થિર પૂર્ણાંક
linelen(પૂર્ણાંક *has_tab)
{
    char_u  *લીટી;
    char_u  *first;
    char_u  *last;
    પૂર્ણાંક	    save;
    પૂર્ણાંક	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    લીટી = ml_get_curline();
    જો (*લીટી == NUL)
	પાછા 0;

    // find the first non-blank character
    first = skipwhite(લીટી);

    // find the character after the last non-blank character
    માટે (last = first + STRLEN(first);
				last > first && VIM_ISWHITE(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize_str(લીટી);	// get line length on screen
    જો (has_tab != ખાલી)		// check for embedded TAB
	*has_tab = (vim_strchr(first, TAB) != ખાલી);
    *last = save;

    પાછા len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
સ્થિર char_u	*sortbuf1;
સ્થિર char_u	*sortbuf2;

સ્થિર પૂર્ણાંક	sort_lc;	// sort using locale
સ્થિર પૂર્ણાંક	sort_ic;	// ignore case
સ્થિર પૂર્ણાંક	sort_nr;	// sort on number
સ્થિર પૂર્ણાંક	sort_rx;	// sort on regex instead of skipping it
સ્થિર પૂર્ણાંક	sort_flt;	// sort on floating number

સ્થિર પૂર્ણાંક	sort_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
પ્રકારવ્યાખ્યા સંરચના
{
    linenr_T	lnum;			// line number
    સંઘ {
	સંરચના
	{
	    varnumber_T	start_col_nr;	// starting column number
	    varnumber_T	end_col_nr;	// ending column number
	} લીટી;
	સંરચના
	{
	    varnumber_T	કિંમત;		// value if sorting by integer
	    પૂર્ણાંક is_સંખ્યા;		// TRUE when line contains a number
	} અંક;
	float_T કિંમત_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    સ્થિર પૂર્ણાંક
string_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    જો (sort_lc)
	પાછા strcoll((અક્ષર *)s1, (અક્ષર *)s2);
    પાછા sort_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    સ્થિર પૂર્ણાંક
sort_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    પૂર્ણાંક		result = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    જો (sort_abort)
	પાછા 0;
    fast_breakcheck();
    જો (got_int)
	sort_abort = TRUE;

    જો (sort_nr)
    {
	જો (l1.st_u.અંક.is_સંખ્યા != l2.st_u.અંક.is_સંખ્યા)
	    result = l1.st_u.અંક.is_સંખ્યા > l2.st_u.અંક.is_સંખ્યા ? 1 : -1;
	નહીંતર
	    result = l1.st_u.અંક.કિંમત == l2.st_u.અંક.કિંમત ? 0
			     : l1.st_u.અંક.કિંમત > l2.st_u.અંક.કિંમત ? 1 : -1;
    }
    નહીંતર જો (sort_flt)
	result = l1.st_u.કિંમત_flt == l2.st_u.કિંમત_flt ? 0
			     : l1.st_u.કિંમત_flt > l2.st_u.કિંમત_flt ? 1 : -1;
    નહીંતર
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.લીટી.start_col_nr,
		     l1.st_u.લીટી.end_col_nr - l1.st_u.લીટી.start_col_nr + 1);
	sortbuf1[l1.st_u.લીટી.end_col_nr - l1.st_u.લીટી.start_col_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.લીટી.start_col_nr,
		     l2.st_u.લીટી.end_col_nr - l2.st_u.લીટી.start_col_nr + 1);
	sortbuf2[l2.st_u.લીટી.end_col_nr - l2.st_u.લીટી.start_col_nr] = 0;

	result = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    જો (result == 0)
	પાછા (પૂર્ણાંક)(l1.lnum - l2.lnum);
    પાછા result;
}

/*
 * ":sort".
 */
    શૂન્ય
ex_sort(exarg_T *eap)
{
    regmatch_T	regmatch;
    પૂર્ણાંક		len;
    linenr_T	lnum;
    long	maxlen = 0;
    sorti_T	*nrs;
    size_t	ગણતરી = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    પૂર્ણાંક		unique = FALSE;
    long	deleted;
    colnr_T	start_col;
    colnr_T	end_col;
    પૂર્ણાંક		sort_what = 0;
    પૂર્ણાંક		format_found = 0;
    પૂર્ણાંક		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    જો (ગણતરી <= 1)
	પાછા;

    જો (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	પાછા;
    sortbuf1 = ખાલી;
    sortbuf2 = ખાલી;
    regmatch.regprog = ખાલી;
    nrs = ALLOC_MULT(sorti_T, ગણતરી);
    જો (nrs == ખાલી)
	જાઓ sortend;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    માટે (p = eap->arg; *p != NUL; ++p)
    {
	જો (VIM_ISWHITE(*p))
	    ;
	નહીંતર જો (*p == 'i')
	    sort_ic = TRUE;
	નહીંતર જો (*p == 'l')
	    sort_lc = TRUE;
	નહીંતર જો (*p == 'r')
	    sort_rx = TRUE;
	નહીંતર જો (*p == 'n')
	{
	    sort_nr = 1;
	    ++format_found;
	}
	નહીંતર જો (*p == 'f')
	{
	    sort_flt = 1;
	    ++format_found;
	}
	નહીંતર જો (*p == 'b')
	{
	    sort_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_found;
	}
	નહીંતર જો (*p == 'o')
	{
	    sort_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_found;
	}
	નહીંતર જો (*p == 'x')
	{
	    sort_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_found;
	}
	નહીંતર જો (*p == 'u')
	    unique = TRUE;
	નહીંતર જો (*p == '"')	// comment start
	    તોડો;
	નહીંતર જો (eap->nextcmd == ખાલી && check_nextcmd(p) != ખાલી)
	{
	    eap->nextcmd = check_nextcmd(p);
	    તોડો;
	}
	નહીંતર જો (!ASCII_ISALPHA(*p) && regmatch.regprog == ખાલી)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    જો (s == ખાલી)
		જાઓ sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    જો (s == p + 1)
	    {
		જો (last_શોધ_pat() == ખાલી)
		{
		    emsg(_(e_no_previous_regular_expression));
		    જાઓ sortend;
		}
		regmatch.regprog = vim_regcomp(last_શોધ_pat(), RE_MAGIC);
	    }
	    નહીંતર
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    જો (regmatch.regprog == ખાલી)
		જાઓ sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	નહીંતર
	{
	    semsg(_(e_invalid_argument_str), p);
	    જાઓ sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    જો (format_found > 1)
    {
	emsg(_(e_invalid_argument));
	જાઓ sortend;
    }

    // From here on "sort_nr" is used as a flag for any integer number
    // sorting.
    sort_nr += sort_what;

    /*
     * Make an array with all line numbers.  This avoids having to copy all
     * the lines into allocated memory.
     * When sorting on strings "start_col_nr" is the offset in the line, for
     * numbers sorting it's the number to sort on.  This means the pattern
     * matching and number conversion only has to be done once per line.
     * Also get the longest line length for allocating "sortbuf".
     */
    માટે (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	જો (maxlen < len)
	    maxlen = len;

	start_col = 0;
	end_col = len;
	જો (regmatch.regprog != ખાલી && vim_regexec(&regmatch, s, 0))
	{
	    જો (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    નહીંતર
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	નહીંતર
	    જો (regmatch.regprog != ખાલી)
		end_col = 0;

	જો (sort_nr || sort_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + end_col;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + start_col;
	    જો (sort_nr)
	    {
		જો (sort_what & STR2NR_HEX)
		    s = skiptohex(p);
		નહીંતર જો (sort_what & STR2NR_BIN)
		    s = skiptobin(p);
		નહીંતર
		    s = skiptodigit(p);
		જો (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		જો (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.અંક.is_સંખ્યા = FALSE;
		    nrs[lnum - eap->line1].st_u.અંક.કિંમત = 0;
		}
		નહીંતર
		{
		    nrs[lnum - eap->line1].st_u.અંક.is_સંખ્યા = TRUE;
		    vim_str2nr(s, ખાલી, ખાલી, sort_what,
			&nrs[lnum - eap->line1].st_u.અંક.કિંમત,
			ખાલી, 0, FALSE, ખાલી);
		}
	    }
	    નહીંતર
	    {
		s = skipwhite(p);
		જો (*s == '+')
		    s = skipwhite(s + 1);

		જો (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.કિંમત_flt = -DBL_MAX;
		નહીંતર
		    nrs[lnum - eap->line1].st_u.કિંમત_flt =
						      strtod((અક્ષર *)s, ખાલી);
	    }
	    *s2 = c;
	}
	નહીંતર
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.લીટી.start_col_nr = start_col;
	    nrs[lnum - eap->line1].st_u.લીટી.end_col_nr = end_col;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	જો (regmatch.regprog != ખાલી)
	    fast_breakcheck();
	જો (got_int)
	    જાઓ sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    જો (sortbuf1 == ખાલી)
	જાઓ sortend;
    sortbuf2 = alloc(maxlen + 1);
    જો (sortbuf2 == ખાલી)
	જાઓ sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((શૂન્ય *)nrs, ગણતરી, કદ(sorti_T), sort_compare);

    જો (sort_abort)
	જાઓ sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    માટે (i = 0; i < ગણતરી; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? ગણતરી - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	જો (get_lnum + ((linenr_T)ગણતરી - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	જો (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    જો (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		તોડો;
	}
	fast_breakcheck();
	જો (got_int)
	    જાઓ sortend;
    }

    // delete the original lines if appending worked
    જો (i == ગણતરી)
	માટે (i = 0; i < ગણતરી; ++i)
	    ml_delete(eap->line1);
    નહીંતર
	ગણતરી = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (long)(ગણતરી - (lnum - eap->line2));
    જો (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    નહીંતર જો (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    જો (change_occurred || deleted != 0)
	changed_લીટીઓ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    જો (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    શૂન્ય
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    પૂર્ણાંક		len;
    linenr_T	lnum;
    long	maxlen = 0;
    linenr_T	ગણતરી = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    પૂર્ણાંક		keep_only_unique = FALSE;
    પૂર્ણાંક		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    long	deleted = 0;
    colnr_T	start_col;
    colnr_T	end_col;
    પૂર્ણાંક		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    જો (ગણતરી <= 1)
	પાછા;

    જો (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	પાછા;
    sortbuf1 = ખાલી;
    regmatch.regprog = ખાલી;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    માટે (p = eap->arg; *p != NUL; ++p)
    {
	જો (VIM_ISWHITE(*p))
	    ;
	નહીંતર જો (*p == 'i')
	    sort_ic = TRUE;
	નહીંતર જો (*p == 'l')
	    sort_lc = TRUE;
	નહીંતર જો (*p == 'r')
	    sort_rx = TRUE;
	નહીંતર જો (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    જો (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	નહીંતર જો (*p == '"')	// comment start
	    તોડો;
	નહીંતર જો (eap->nextcmd == ખાલી && check_nextcmd(p) != ખાલી)
	{
	    eap->nextcmd = check_nextcmd(p);
	    તોડો;
	}
	નહીંતર જો (!ASCII_ISALPHA(*p) && regmatch.regprog == ખાલી)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    જો (s == ખાલી)
		જાઓ uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    જો (s == p + 1)
	    {
		જો (last_શોધ_pat() == ખાલી)
		{
		    emsg(_(e_no_previous_regular_expression));
		    જાઓ uniqend;
		}
		regmatch.regprog = vim_regcomp(last_શોધ_pat(), RE_MAGIC);
	    }
	    નહીંતર
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    જો (regmatch.regprog == ખાલી)
		જાઓ uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	નહીંતર
	{
	    semsg(_(e_invalid_argument_str), p);
	    જાઓ uniqend;
	}
    }

    // Find the length of the longest line.
    માટે (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	જો (maxlen < len)
	    maxlen = len;

	જો (got_int)
	    જાઓ uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    જો (sortbuf1 == ખાલી)
	જાઓ uniqend;

    // Delete lines according to options.
    પૂર્ણાંક match_continue = FALSE;
    પૂર્ણાંક next_is_unmatch = FALSE;
    પૂર્ણાંક is_match;
    linenr_T done_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    માટે (linenr_T i = 0; i < ગણતરી; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	start_col = 0;
	end_col = len;
	જો (regmatch.regprog != ખાલી && vim_regexec(&regmatch, s, 0))
	{
	    જો (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    નહીંતર
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	નહીંતર
	    જો (regmatch.regprog != ખાલી)
		end_col = 0;
	જો (end_col > 0)
	{
	    save_c = s[end_col];
	    s[end_col] = NUL;
	}

	is_match = i > 0 ? !string_compare(&s[start_col], sortbuf1) : FALSE;
	delete_lnum = 0;
	જો (next_is_unmatch)
	{
	    is_match = FALSE;
	    next_is_unmatch = FALSE;
	}

	જો (!keep_only_unique && !keep_only_not_unique)
	{
	    જો (is_match)
		delete_lnum = get_lnum;
	    નહીંતર
		STRCPY(sortbuf1, &s[start_col]);
	}
	નહીંતર જો (keep_only_not_unique)
	{
	    જો (is_match)
	    {
		done_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    નહીંતર
	    {
		જો (i > 0 && !match_continue && get_lnum - 1 > done_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    next_is_unmatch = TRUE;
		}
		નહીંતર જો (i >= ગણતરી - 1)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}
	નહીંતર // keep_only_unique
	{
	    જો (is_match)
	    {
		જો (!match_continue)
		    delete_lnum = get_lnum - 1;
		નહીંતર
		    delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    નહીંતર
	    {
		જો (i == 0 && match_continue)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}

	જો (end_col > 0)
	    s[end_col] = save_c;

	જો (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    ગણતરી--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	જો (got_int)
	    જાઓ uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
    msgmore(-deleted);

    જો (change_occurred)
	changed_લીટીઓ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    જો (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	અંક_લીટીઓ;  // Num lines moved
    linenr_T	last_લીટી;  // Last line in file after adding new text
#ifdef FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#endif

    જો (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_of_લીટીઓ_into_itself));
	પાછા FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    જો (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	જો (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	નહીંતર
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	પાછા OK;
    }

    અંક_લીટીઓ = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    જો (u_save(dest, dest + 1) == FAIL)
	પાછા FAIL;
    માટે (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	જો (str != ખાલી)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    જો (dest < line1)
		extra++;
	}
    }

    /*
     * Now we must be careful adjusting our marks so that we don't overlap our
     * mark_adjust() calls.
     *
     * We adjust the marks within the old text so that they refer to the
     * last lines of the file (temporarily), because we know no other marks
     * will be set there since these line numbers did not exist until we added
     * our new lines.
     *
     * Then we adjust the marks on lines between the old and new text positions
     * (either forwards or backwards).
     *
     * And Finally we adjust the marks we put at the end of the file back to
     * their final destination at the new text position -- webb
     */
    last_લીટી = curbuf->b_ml.ml_લીટી_ગણતરી;
    mark_adjust_nofold(line1, line2, last_લીટી - line2, 0L);
    જો (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -અંક_લીટીઓ, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    જો (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#endif
	જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest - અંક_લીટીઓ + 1;
	    curbuf->b_op_end.lnum = dest;
	}
    }
    નહીંતર
    {
	mark_adjust_nofold(dest + 1, line1 - 1, અંક_લીટીઓ, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    જો (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#endif
	જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest + 1;
	    curbuf->b_op_end.lnum = dest + અંક_લીટીઓ;
	}
    }
    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust_nofold(last_લીટી - અંક_લીટીઓ + 1, last_લીટી,
					     -(last_લીટી - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    જો (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	પાછા FAIL;

    માટે (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_MESSAGE);

    જો (!global_busy && અંક_લીટીઓ > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", અંક_લીટીઓ),
			(long)અંક_લીટીઓ);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    જો (dest >= line1)
	curwin->w_cursor.lnum = dest;
    નહીંતર
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    જો (line1 < dest)
    {
	dest += અંક_લીટીઓ + 1;
	last_લીટી = curbuf->b_ml.ml_લીટી_ગણતરી;
	જો (dest > last_લીટી + 1)
	    dest = last_લીટી + 1;
	changed_લીટીઓ(line1, 0, dest, 0L);
    }
    નહીંતર
	changed_લીટીઓ(dest + 1, 0, line1 + અંક_લીટીઓ, 0L);

    પાછા OK;
}

/*
 * ":copy"
 */
    શૂન્ય
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	ગણતરી;
    char_u	*p;

    ગણતરી = line2 - line1 + 1;
    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = n + 1;
	curbuf->b_op_end.lnum = n + ગણતરી;
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    }

    /*
     * there are three situations:
     * 1. destination is above line1
     * 2. destination is between line1 and line2
     * 3. destination is below line2
     *
     * n = destination (when starting)
     * curwin->w_cursor.lnum = destination (while copying)
     * line1 = start of source (while copying)
     * line2 = end of source (while copying)
     */
    જો (u_save(n, n + 1) == FAIL)
	પાછા;

    curwin->w_cursor.lnum = n;
    જ્યારે (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	જો (p != ખાલી)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	જો (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	જો (curwin->w_cursor.lnum < line1)
	    ++line1;
	જો (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_લીટીઓ_mark(n, ગણતરી);
    જો (VIsual_active)
	check_pos(curbuf, &VIsual);

    msgmore((long)ગણતરી);
}

સ્થિર char_u	*prevcmd = ખાલી;	// the previous command

#જો defined(EXITFREE)
    શૂન્ય
free_prev_shellcmd(શૂન્ય)
{
    vim_free(prevcmd);
}
#endif

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    સ્થિર પૂર્ણાંક
prevcmd_is_set(શૂન્ય)
{
    જો (prevcmd == ખાલી)
    {
	emsg(_(e_no_previous_command));
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    શૂન્ય
do_bang(
    પૂર્ણાંક		addr_ગણતરી,
    exarg_T	*eap,
    પૂર્ણાંક		forceit,
    પૂર્ણાંક		do_in,
    પૂર્ણાંક		do_out)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = ખાલી;		// the new command
    પૂર્ણાંક			free_newcmd = FALSE;    // need to free() newcmd
    પૂર્ણાંક			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    પૂર્ણાંક			len;
    પૂર્ણાંક			scroll_save = સંદેશ_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    જો (check_restricted() || check_secure())
	પાછા;

    જો (addr_ગણતરી == 0)		// :!
    {
	સંદેશ_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	સંદેશ_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    કરો
    {
	len = (પૂર્ણાંક)STRLEN(trailarg) + 1;
	જો (newcmd != ખાલી)
	    len += (પૂર્ણાંક)STRLEN(newcmd);
	જો (ins_prevcmd)
	{
	    જો (!prevcmd_is_set())
	    {
		vim_free(newcmd);
		પાછા;
	    }
	    len += (પૂર્ણાંક)STRLEN(prevcmd);
	}
	જો ((t = alloc(len)) == ખાલી)
	{
	    vim_free(newcmd);
	    પાછા;
	}
	*t = NUL;
	જો (newcmd != ખાલી)
	    STRCAT(t, newcmd);
	જો (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = ખાલી;
	જ્યારે (*p)
	{
	    જો (*p == '!')
	    {
		જો (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		નહીંતર
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    તોડો;
		}
	    }
	    ++p;
	}
    } જ્યારે (trailarg != ખાલી);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    જો (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    નહીંતર
	free_newcmd = TRUE;

    જો (bangredo)	    // put cmd in redo buffer for ! command
    {
	જો (!prevcmd_is_set())
	    જાઓ theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	જો (cmd != ખાલી)
	{
	    AppendToRedobuffLit(cmd, -1);
	    vim_free(cmd);
	}
	નહીંતર
	    AppendToRedobuffLit(prevcmd, -1);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    જો (*p_shq != NUL)
    {
	જો (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	જો (newcmd == ખાલી)
	    પાછા;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    જો (addr_ગણતરી == 0)		// :!
    {
	// echo the command
	સંદેશ_start();
	સંદેશ_putchar(':');
	સંદેશ_putchar('!');
	સંદેશ_outtrans(newcmd);
	સંદેશ_clr_eos();
	windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);

	do_shell(newcmd, 0);
    }
    નહીંતર				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
	apply_autocmds(EVENT_SHELLFILTERPOST, ખાલી, ખાલી, FALSE, curbuf);
    }

theend:
    જો (free_newcmd)
	vim_free(newcmd);
}

/*
 * do_filter: filter lines through a command given by the user
 *
 * We mostly use temp files and the call_shell() routine here. This would
 * normally be done using pipes on a UNIX machine, but this is more portable
 * to non-unix machines. The call_shell() routine needs to be able
 * to deal with redirection somehow, and should handle things like looking
 * at the PATH env. variable, and adding reasonable extensions to the
 * command name given by the user. All reasonable versions of call_shell()
 * do this.
 * Alternatively, if on Unix and redirecting input or output, but not both,
 * and the 'shelltemp' option isn't set, use pipes.
 * We use input redirection if do_in is TRUE.
 * We use output redirection if do_out is TRUE.
 */
    સ્થિર શૂન્ય
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    પૂર્ણાંક		do_in,
    પૂર્ણાંક		do_out)
{
    char_u	*itmp = ખાલી;
    char_u	*otmp = ખાલી;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    પૂર્ણાંક		shell_flags = 0;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    પૂર્ણાંક		save_cmod_flags = cmdmod.cmod_flags;
#ifdef FEAT_FILTERPIPE
    પૂર્ણાંક		stmp = p_stmp;
#endif

    જો (*cmd == NUL)	    // no filter command
	પાછા;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_લીટી_abv_curs();
    invalidate_botline();

    /*
     * When using temp files:
     * 1. * Form temp file names
     * 2. * Write the lines to a temp file
     * 3.   Run the filter command on the temp file
     * 4. * Read the output of the command into the buffer
     * 5. * Delete the original lines to be filtered
     * 6. * Remove the temp files
     *
     * When writing the input with a pipe or when catching the output with a
     * pipe only need to do 3.
     */

    જો (do_out)
	shell_flags |= SHELL_DOOUT;

#ifdef FEAT_FILTERPIPE
# ifdef VIMDLL
    જો (!gui.in_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    જો (!do_in && do_out && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    નહીંતર જો (do_in && !do_out && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
    }
    નહીંતર જો (do_in && do_out && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    નહીંતર
#endif
	જો ((do_in && (itmp = vim_tempname('i', FALSE)) == ખાલી)
		|| (do_out && (otmp = vim_tempname('o', FALSE)) == ખાલી))
	{
	    emsg(_(e_cant_get_temp_file_name));
	    જાઓ filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    જો (itmp != ખાલી && buf_write(curbuf, itmp, ખાલી, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	સંદેશ_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#જો defined(FEAT_EVAL)
	જો (!aborting())
#endif
	    // will call wait_return()
	    (શૂન્ય)semsg(_(e_cant_create_file_str), itmp);
	જાઓ filterend;
    }
    જો (curbuf != old_curbuf)
	જાઓ filterend;

    જો (!do_out)
	સંદેશ_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    જો (cmd_buf == ખાલી)
	જાઓ filterend;

    windgoto((પૂર્ણાંક)Rows - 1, cmdline_col_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    જો (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	redraw_later_clear();

    જો (do_out)
    {
	જો (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    જાઓ error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_લીટી_ગણતરી;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    જો (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
    {
	redraw_later_clear();
	wait_return(FALSE);
    }
    vim_free(cmd_buf);

    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    // When interrupting the shell command, it may still have produced some
    // useful output.  Reset got_int here, so that readfile() won't cancel
    // reading.
    ui_breakcheck();
    got_int = FALSE;

    જો (do_out)
    {
	જો (otmp != ખાલી)
	{
	    જો (readfile(otmp, ખાલી, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#જો defined(FEAT_EVAL)
		જો (!aborting())
#endif
		{
		    સંદેશ_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		જાઓ error;
	    }
	    જો (curbuf != old_curbuf)
		જાઓ filterend;
	}

	read_linecount = curbuf->b_ml.ml_લીટી_ગણતરી - read_linecount;

	જો (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_start.lnum = line2 + 1;
	    curbuf->b_op_end.lnum = curwin->w_cursor.lnum;
	    appended_લીટીઓ_mark(line2, read_linecount);
	}

	જો (do_in)
	{
	    જો ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == ખાલી)
	    {
		જો (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		નહીંતર જો (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		નહીંતર
		{
		    // move marks from old lines to new lines, delete marks
		    // that are in deleted lines
		    mark_adjust(line1, line1 + read_linecount - 1,
								linecount, 0L);
		    mark_adjust(line1 + read_linecount, line2, MAXLNUM, 0L);
		}
	    }

	    /*
	     * Put cursor on first filtered line for ":range!cmd".
	     * Adjust '[ and '] (set by buf_write()).
	     */
	    curwin->w_cursor.lnum = line1;
	    del_લીટીઓ(linecount, TRUE);
	    જો (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_start.lnum = curbuf->b_op_end.lnum =
					MIN(line1, curbuf->b_ml.ml_લીટી_ગણતરી);
		curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	    }
	    નહીંતર
	    {
		curbuf->b_op_start.lnum -= linecount;	// adjust '[
		curbuf->b_op_end.lnum -= linecount;	// adjust ']
	    }
	    write_lnum_adjust(-linecount);		// adjust last line
							// for next write
#ifdef FEAT_FOLDING
	    foldUpdate(curwin, curbuf->b_op_start.lnum, curbuf->b_op_end.lnum);
#endif
	}
	નહીંતર
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	જો (linecount > p_report)
	{
	    જો (do_in)
	    {
		vim_snprintf(સંદેશ_buf, કદ(સંદેશ_buf),
				    _("%ld lines filtered"), (long)linecount);
		જો (સંદેશ(સંદેશ_buf) && !સંદેશ_scroll)
		    // save message to display it after redraw
		    set_keep_સંદેશ((char_u *)સંદેશ_buf, 0);
	    }
	    નહીંતર
		msgmore((long)linecount);
	}
    }
    નહીંતર
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    જો (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_buffer));
    }
    નહીંતર જો (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

    જો (itmp != ખાલી)
	mch_remove(itmp);
    જો (otmp != ખાલી)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    શૂન્ય
do_shell(
    char_u	*cmd,
    પૂર્ણાંક		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    પૂર્ણાંક		save_nwr;
#endif
#ifdef MSWIN
    પૂર્ણાંક		winstart = FALSE;
#endif
    પૂર્ણાંક		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    જો (check_restricted() || check_secure())
    {
	સંદેશ_end();
	પાછા;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    જો (cmd != ખાલી)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# જો defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    જો (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ખાલી)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    સંદેશ_putchar('\r');			// put cursor at start of line
    જો (!autocmd_busy)
    {
	જો (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    જો (!winstart)
#endif
	સંદેશ_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    જો (p_warn && !autocmd_busy && સંદેશ_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    જો (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		જો (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		સંદેશ_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		જો (!keep_termcap)
		    stoptermcap();
#endif
		તોડો;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    જો (!swapping_screen())
	windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
    cursor_on();
    (શૂન્ય)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    જો (!swapping_screen())
    {
	સંદેશ_row = Rows - 1;
	સંદેશ_col = 0;
    }

    જો (autocmd_busy)
    {
	જો (સંદેશ_silent == 0)
	    redraw_later_clear();
    }
    નહીંતર
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	જો (!gui.in_use)
# endif
	{
	    જો (cmd == ખાલી
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		જો (સંદેશ_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    નહીંતર
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		જો (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		પૂર્ણાંક redraw = સંદેશ_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		જો (gui.in_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(term_console ? -1 : redraw); // see below
# નહીંતર
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	જો (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	જો (skip_redraw)		// ':' hit in wait_return()
	{
	    જો (સંદેશ_silent == 0)
		redraw_later_clear();
	}
	નહીંતર જો (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    જો (got_int && સંદેશ_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    નહીંતર
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, ખાલી, ખાલી, FALSE, curbuf);
}

#જો !defined(UNIX)
    સ્થિર char_u *
find_pipe(char_u *cmd)
{
    char_u  *p;
    પૂર્ણાંક	    inquote = FALSE;

    માટે (p = cmd; *p != NUL; ++p)
    {
	જો (!inquote && *p == '|')
	    પાછા p;
	જો (*p == '"')
	    inquote = !inquote;
	નહીંતર જો (rem_backslash(p))
	    ++p;
    }
    પાછા ખાલી;
}
#endif

/*
 * Create a shell command from a command string, input redirection file and
 * output redirection file.
 * Returns an allocated string with the shell command, or NULL for failure.
 */
    char_u *
make_filter_cmd(
    char_u	*cmd,		// command
    char_u	*itmp,		// NULL or name of input file
    char_u	*otmp)		// NULL or name of output file
{
    char_u	*buf;
    long_u	len;
    પૂર્ણાંક		is_powershell = FALSE;
#ifdef UNIX
    પૂર્ણાંક		is_fish_shell;
#endif

    char_u *shell_name = get_isolated_shell_name();
    જો (shell_name == ખાલી)
	પાછા ખાલી;

#જો defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    જો (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    નહીંતર
#endif
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	જો (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	નહીંતર
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    જો (itmp != ખાલી)
    {
	જો (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	નહીંતર
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    જો (otmp != ખાલી)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    જો (buf == ખાલી)
	પાછા ખાલી;

    જો (is_powershell)
    {
	જો (itmp != ખાલી)
	    vim_snprintf((અક્ષર *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	નહીંતર
	    vim_snprintf((અક્ષર *)buf, len, "& { %s }", cmd);
    }
    નહીંતર
    {
#જો defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	જો (itmp != ખાલી || otmp != ખાલી)
	{
	    જો (is_fish_shell)
		vim_snprintf((અક્ષર *)buf, len, "begin; %s; end", (અક્ષર *)cmd);
	    નહીંતર
		vim_snprintf((અક્ષર *)buf, len, "(%s)", (અક્ષર *)cmd);
	}
	નહીંતર
	    STRCPY(buf, cmd);
	જો (itmp != ખાલી)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#નહીંતર
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	જો (*p_sxe != NUL && *p_sxq == '(')
	{
	    જો (itmp != ખાલી || otmp != ખાલી)
		vim_snprintf((અક્ષર *)buf, len, "(%s)", (અક્ષર *)cmd);
	    નહીંતર
		STRCPY(buf, cmd);
	    જો (itmp != ખાલી)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	નહીંતર
	{
	    STRCPY(buf, cmd);
	    જો (itmp != ખાલી)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		જો (*p_shq == NUL)
		{
		    p = find_pipe(buf);
		    જો (p != ખાલી)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		જો (*p_shq == NUL)
		{
		    p = find_pipe(cmd);
		    જો (p != ખાલી)
		    {
			// insert a space before the '|' for DOS
			STRCAT(buf, " ");
			STRCAT(buf, p);
		    }
		}
	    }
	}
#endif
    }
    જો (otmp != ખાલી)
	append_redir(buf, (પૂર્ણાંક)len, p_srr, otmp);

    પાછા buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    શૂન્ય
append_redir(
    char_u	*buf,
    પૂર્ણાંક		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*end;

    end = buf + STRLEN(buf);
    // find "%s"
    માટે (p = opt; (p = vim_strchr(p, '%')) != ખાલી; ++p)
    {
	જો (p[1] == 's') // found %s
	    તોડો;
	જો (p[1] == '%') // skip %%
	    ++p;
    }
    જો (p != ખાલી)
    {
#ifdef MSWIN
	*end++ = ' '; // not really needed? Not with sh, ksh or bash
#endif
	vim_snprintf((અક્ષર *)end, (size_t)(buflen - (end - buf)),
						  (અક્ષર *)opt, (અક્ષર *)fname);
    }
    નહીંતર
	vim_snprintf((અક્ષર *)end, (size_t)(buflen - (end - buf)), " %s %s",
		(અક્ષર *)opt, (અક્ષર *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    શૂન્ય
do_fixdel(exarg_T *eap UNUSED)
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != ખાલી
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    શૂન્ય
print_લીટી_no_prefix(
    linenr_T	lnum,
    પૂર્ણાંક		use_સંખ્યા,
    પૂર્ણાંક		list)
{
    અક્ષર	numbuf[30];

    જો (curwin->w_p_nu || use_સંખ્યા)
    {
	vim_snprintf(numbuf, કદ(numbuf),
				   "%*ld ", સંખ્યા_width(curwin), (long)lnum);
	સંદેશ_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    સંદેશ_prt_લીટી(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    શૂન્ય
print_લીટી(linenr_T lnum, પૂર્ણાંક use_સંખ્યા, પૂર્ણાંક list)
{
    પૂર્ણાંક		save_silent = silent_mode;

    // apply :filter /pat/
    જો (message_filtered(ml_get(lnum)))
	પાછા;

    સંદેશ_start();
    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()
    print_લીટી_no_prefix(lnum, use_સંખ્યા, list);
    જો (save_silent)
    {
	સંદેશ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = save_silent;
    }
    info_message = FALSE;
}

    પૂર્ણાંક
rename_buffer(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, ખાલી, ખાલી, FALSE, curbuf);
    // buffer changed, don't change name now
    જો (buf != curbuf)
	પાછા FAIL;
#ifdef FEAT_EVAL
    જો (aborting())	    // autocmds may abort script processing
	પાછા FAIL;
#endif
    /*
     * The name of the current buffer will be changed.
     * A new (unlisted) buffer entry needs to be made to hold the old file
     * name, which will become the alternate file name.
     * But don't set the alternate file name if the buffer didn't have a
     * name.
     */
    fname = curbuf->b_ffname;
    sfname = curbuf->b_sfname;
    xfname = curbuf->b_fname;
    curbuf->b_ffname = ખાલી;
    curbuf->b_sfname = ખાલી;
    જો (setfname(curbuf, new_fname, ખાલી, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	પાછા FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    જો (xfname != ખાલી && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	જો (buf != ખાલી && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, ખાલી, ખાલી, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    પાછા OK;
}

/*
 * ":file[!] [fname]".
 */
    શૂન્ય
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    જો (eap->addr_ગણતરી > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_ગણતરી > 1))
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }

    જો (*eap->arg != NUL || eap->addr_ગણતરી == 1)
    {
	જો (rename_buffer(eap->arg) == FAIL)
	    પાછા;
	redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    જો (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    શૂન્ય
ex_update(exarg_T *eap)
{
    જો (curbufIsChanged())
	(શૂન્ય)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    શૂન્ય
ex_write(exarg_T *eap)
{
    જો (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
    }

    જો (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    નહીંતર
	(શૂન્ય)do_write(eap);
}

#ifdef UNIX
    સ્થિર પૂર્ણાંક
check_writable(char_u *fname)
{
    જો (mch_nodetype(fname) == ગાંઠ_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	પાછા FAIL;
    }
    પાછા OK;
}
#endif

/*
 * Write the current buffer to file "eap->arg".
 * If "eap->append" is TRUE, append to the file.
 *
 * If "*eap->arg == NUL" write to current file.
 *
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
do_write(exarg_T *eap)
{
    પૂર્ણાંક		other;
    char_u	*fname = ખાલી;		// init to shut up gcc
    char_u	*ffname;
    પૂર્ણાંક		retval = FAIL;
    char_u	*free_fname = ખાલી;
#ifdef FEAT_BROWSE
    char_u	*browse_file = ખાલી;
#endif
    buf_T	*alt_buf = ખાલી;
    પૂર્ણાંક		name_was_missing;

    જો (not_writing())		// check 'write' option
	પાછા FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    જો ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    ખાલી, ખાલી, ખાલી, curbuf);
	જો (browse_file == ખાલી)
	    જાઓ theend;
	ffname = browse_file;
    }
#endif
    જો (*ffname == NUL)
    {
	જો (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_argument_required));
	    જાઓ theend;
	}
	other = FALSE;
    }
    નહીંતર
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	જો (free_fname != ખાલી)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    જો (other)
    {
	જો (vim_strchr(p_cpo, CPO_ALTWRITE) != ખાલી
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	નહીંતર
	    alt_buf = buflist_findname(ffname);
	જો (alt_buf != ખાલી && alt_buf->b_ml.ml_mfp != ખાલી)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    જાઓ theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    જો (!other && (bt_dontwrite_સંદેશ(curbuf) || check_fname() == FAIL
#ifdef UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#endif
		|| check_readonly(&eap->forceit, curbuf)))
	જાઓ theend;

    જો (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	જો (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_લીટી_ગણતરી)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    જો (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		જો (vim_dialog_yesno(VIM_QUESTION, ખાલી,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    જાઓ theend;
		eap->forceit = TRUE;
	    }
	    નહીંતર
#endif
	    {
		emsg(_(e_use_bang_to_write_partial_buffer));
		જાઓ theend;
	    }
	}
    }

    જો (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	જો (eap->cmdidx == CMD_saveas && alt_buf != ખાલી)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, ખાલી, ખાલી, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, ખાલી, ખાલી, FALSE, alt_buf);
#ifdef FEAT_EVAL
	    જો (curbuf != was_curbuf || aborting())
#નહીંતર
	    જો (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		જાઓ theend;
	    }
	    // Exchange the file names for the current and the alternate
	    // buffer.  This makes it look like we are now editing the buffer
	    // under the new name.  Must be done before buf_write(), because
	    // if there is no file name and 'cpo' contains 'F', it will set
	    // the file name.
	    fname = alt_buf->b_fname;
	    alt_buf->b_fname = curbuf->b_fname;
	    curbuf->b_fname = fname;
	    fname = alt_buf->b_ffname;
	    alt_buf->b_ffname = curbuf->b_ffname;
	    curbuf->b_ffname = fname;
	    fname = alt_buf->b_sfname;
	    alt_buf->b_sfname = curbuf->b_sfname;
	    curbuf->b_sfname = fname;
	    buf_name_changed(curbuf);

	    apply_autocmds(EVENT_BUFFILEPOST, ખાલી, ખાલી, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, ખાલી, ખાલી, FALSE, alt_buf);
	    જો (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, ખાલી, ખાલી, FALSE, alt_buf);
	    }
#ifdef FEAT_EVAL
	    જો (curbuf != was_curbuf || aborting())
#નહીંતર
	    જો (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		જાઓ theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    જો (*curbuf->b_p_ft == NUL)
	    {
		જો (au_has_group((char_u *)"filetypedetect"))
		    (શૂન્ય)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, ખાલી);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == ખાલી;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	જો (eap->cmdidx == CMD_saveas)
	{
	    જો (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	જો (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    પાછા retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    પૂર્ણાંક
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    પૂર્ણાંક		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    જો (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == ખાલી)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	જો (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    જો (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		પાછા FAIL;
	    }
#endif
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    જો (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_સંદેશ_SIZE];

		dialog_સંદેશ(buff, _("Overwrite existing file \"%s\"?"), fname);
		જો (vim_dialog_yesno(VIM_QUESTION, ખાલી, buff, 2) != VIM_YES)
		    પાછા FAIL;
		eap->forceit = TRUE;
	    }
	    નહીંતર
#endif
	    {
		emsg(_(e_file_exists));
		પાછા FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	જો (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    પૂર્ણાંક		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    જો (*p_dir == NUL)
	    {
		dir = alloc(5);
		જો (dir == ખાલી)
		    પાછા FAIL;
		STRCPY(dir, ".");
	    }
	    નહીંતર
	    {
		dir = alloc(MAXPATHL);
		જો (dir == ખાલી)
		    પાછા FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    જો (r)
	    {
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		જો (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_સંદેશ_SIZE];

		    dialog_સંદેશ(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    જો (vim_dialog_yesno(VIM_QUESTION, ખાલી, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			પાછા FAIL;
		    }
		    eap->forceit = TRUE;
		}
		નહીંતર
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    પાછા FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    પાછા OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    શૂન્ય
ex_wnext(exarg_T *eap)
{
    પૂર્ણાંક		i;

    જો (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (પૂર્ણાંક)eap->line2;
    નહીંતર
	i = curwin->w_arg_idx - (પૂર્ણાંક)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
    જો (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    શૂન્ય
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    પૂર્ણાંક		error = 0;
    પૂર્ણાંક		save_forceit = eap->forceit;
    પૂર્ણાંક		save_exiting = exiting;

    જો (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	જો (before_quit_all(eap) == FAIL)
	    પાછા;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ifdef FEAT_TERMINAL
	જો (exiting && !eap->forceit && term_job_running(buf->b_term))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);
	    જો (term_try_stop_job(buf) == FAIL)
	    {
		no_write_message_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    નહીંતર જો (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	નહીંતર
#endif
	જો (bufIsChanged(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    જો (not_writing())
	    {
		++error;
		તોડો;
	    }
#ifdef FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    જો (buf->b_ffname == ખાલી && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#endif
	    જો (buf->b_ffname == ખાલી)
	    {
		semsg(_(e_no_file_name_for_buffer_nr), (long)buf->b_fnum);
		++error;
	    }
	    નહીંતર જો (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    નહીંતર
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		જો (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		જો (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    જો (exiting)
    {
	જો (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    સ્થિર પૂર્ણાંક
not_writing(શૂન્ય)
{
    જો (p_write)
	પાછા FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    પાછા TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    સ્થિર પૂર્ણાંક
check_readonly(પૂર્ણાંક *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    જો (!*forceit && (buf->b_p_ro
		|| (mch_stat((અક્ષર *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	જો ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != ખાલી)
	{
	    char_u	buff[DIALOG_સંદેશ_SIZE];

	    જો (buf->b_p_ro)
		dialog_સંદેશ(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    નહીંતર
		dialog_સંદેશ(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    જો (vim_dialog_yesno(VIM_QUESTION, ખાલી, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		પાછા FALSE;
	    }
	    નહીંતર
		પાછા TRUE;
	}
	નહીંતર
#endif
	જો (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_set_add_bang_to_override));
	નહીંતર
	    semsg(_(e_str_is_read_only_add_bang_to_override), buf->b_fname);
	પાછા TRUE;
    }

    પાછા FALSE;
}

/*
 * Try to abandon the current file and edit a new or existing file.
 * "fnum" is the number of the file, if zero use "ffname_arg"/"sfname_arg".
 * "lnum" is the line number for the cursor in the new file (if non-zero).
 *
 * Return:
 * GETFILE_ERROR for "normal" error,
 * GETFILE_NOT_WRITTEN for "not written" error,
 * GETFILE_SAME_FILE for success
 * GETFILE_OPEN_OTHER for successfully opening another file.
 */
    પૂર્ણાંક
getfile(
    પૂર્ણાંક		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    પૂર્ણાંક		setpm,
    linenr_T	lnum,
    પૂર્ણાંક		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    પૂર્ણાંક		other;
    પૂર્ણાંક		retval;
    char_u	*free_me = ખાલી;

    જો (!check_can_set_curbuf_forceit(forceit))
	પાછા GETFILE_ERROR;

    જો (text_locked())
	પાછા GETFILE_ERROR;
    જો (curbuf_locked())
	પાછા GETFILE_ERROR;

    જો (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    નહીંતર
	other = (fnum != curbuf->b_fnum);

    જો (other)
	++no_wait_return;	    // don't wait for autowrite message
    જો (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	જો (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	જો (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    જાઓ theend;
	}
    }
    જો (other)
	--no_wait_return;
    જો (setpm)
	setpcmark();
    જો (!other)
    {
	જો (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    નહીંતર જો (do_ecmd(fnum, ffname, sfname, ખાલી, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    નહીંતર
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    પાછા retval;
}

/*
 * start editing a new file
 *
 *     fnum: file number; if zero use ffname/sfname
 *   ffname: the file name
 *		- full path if sfname used,
 *		- any file name if sfname is NULL
 *		- empty string to re-edit with the same file name (but may be
 *		    in a different directory)
 *		- NULL to start an empty buffer
 *   sfname: the short file name (or NULL)
 *	eap: contains the command to be executed after loading the file and
 *	     forced 'ff' and 'fenc'
 *  newlnum: if > 0: put cursor on this line number (if possible)
 *	     if ECMD_LASTL: use last position in loaded file
 *	     if ECMD_LAST: use last position in all files
 *	     if ECMD_ONE: use first line
 *    flags:
 *	   ECMD_HIDE: if TRUE don't free the current buffer
 *     ECMD_SET_HELP: set b_help flag of (new) buffer before opening file
 *	 ECMD_OLDBUF: use existing buffer if it exists
 *	ECMD_FORCEIT: ! used for Ex command
 *	 ECMD_ADDBUF: don't edit, just add to buffer list
 *	 ECMD_ALTBUF: like ECMD_ADDBUF and also set the alternate file
 *   ECMD_NOWINENTER: Do not trigger BufWinEnter
 *   oldwin: Should be "curwin" when editing a new buffer in the current
 *	     window, NULL when splitting the window first.  When not NULL info
 *	     of the previous buffer for "oldwin" is stored.
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
do_ecmd(
    પૂર્ણાંક		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    પૂર્ણાંક		flags,
    win_T	*oldwin)
{
    પૂર્ણાંક		other_file;		// TRUE if editing another file
    પૂર્ણાંક		oldbuf;			// TRUE if using existing buffer
    પૂર્ણાંક		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = ખાલી;
#જો defined(FEAT_EVAL)
    પૂર્ણાંક		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = ખાલી;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = ખાલી;
#endif
    પૂર્ણાંક		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    પૂર્ણાંક		newcol = -1;
    પૂર્ણાંક		solcol = -1;
    pos_T	*pos;
    char_u	*command = ખાલી;
#ifdef FEAT_SPELL
    પૂર્ણાંક		did_get_winopts = FALSE;
#endif
    પૂર્ણાંક		readfile_flags = 0;
    પૂર્ણાંક		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    જો (ERROR_IF_TERM_POPUP_WINDOW)
	પાછા FAIL;
#endif

    જો (eap != ખાલી)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    જો (fnum != 0)
    {
	જો (fnum == curbuf->b_fnum)	// file is already being edited
	    પાછા OK;			// nothing to do
	other_file = TRUE;
    }
    નહીંતર
    {
#ifdef FEAT_BROWSE
	જો ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    જો (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		જો (ffname == ખાલી || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    નહીંતર
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    ખાલી, ખાલી, ખાલી, curbuf);
		જો (browse_file == ખાલી)
		    જાઓ theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	જો (sfname == ખાલી)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	જો (sfname != ખાલી)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	જો ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == ખાલી || *ffname == NUL))
	    જાઓ theend;

	જો (ffname == ખાલી)
	    other_file = TRUE;
					    // there is no file name
	નહીંતર જો (*ffname == NUL && curbuf->b_ffname == ખાલી)
	    other_file = FALSE;
	નહીંતર
	{
	    જો (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    જો (free_fname != ખાલી)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    જો (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == ખાલી ? 0 : CCGD_EXCMD)))
    {
	જો (fnum == 0 && other_file && ffname != ખાલી)
	    (શૂન્ય)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	જાઓ theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    જો (oldwin != ખાલી && !win_valid(oldwin))
	oldwin = ખાલી;

#જો defined(FEAT_EVAL)
    જો ((command != ખાલી || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	જો (command != ખાલી)
	    valsize = (પૂર્ણાંક)STRLEN(command) + 3;
	નહીંતર
	    valsize = 30;
	val.string = alloc(valsize);
	જો (val.string != ખાલી)
	{
	    જો (command != ખાલી)
		val.length = vim_snprintf_safelen((અક્ષર *)val.string,
		    valsize, ":%s\r", command);
	    નહીંતર
		val.length = vim_snprintf_safelen((અક્ષર *)val.string,
		    valsize, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, val.string, (પૂર્ણાંક)val.length);
	    did_set_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    જો (other_file)
    {
	પૂર્ણાંક prev_alt_fnum = curwin->w_alt_fnum;

	જો (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    જો ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    જો (oldwin != ખાલી)
		buflist_altfpos(oldwin);
	}

	જો (fnum)
	    buf = buflist_findnr(fnum);
	નહીંતર
	{
	    જો (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		જો (command != ખાલી)
		{
		    tlnum = atol((અક્ષર *)command);
		    જો (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		જો (newbuf != ખાલી)
		{
		    જો (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    જો (tlnum > 0)
			newbuf->b_last_cursor.lnum = tlnum;
		}
		જાઓ theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    જો (oldwin != ખાલી)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	જો (buf == ખાલી)
	    જાઓ theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	જો (buf->b_locked_split)
	{
	    emsg(_(e_cannot_switch_to_a_closing_buffer));
	    જાઓ theend;
	}
	જો (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	જો (buf->b_ml.ml_mfp == ખાલી)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	નહીંતર					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (શૂન્ય)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    જો (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		જાઓ theend;
#ifdef FEAT_EVAL
	    જો (aborting())	    // autocmds may abort script processing
		જાઓ theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	જો ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->col;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	જો (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    પૂર્ણાંક		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    પ્રતિજ્ઞા(cmdwin_buf == ખાલી);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = ખાલી;

	    /*
	     * Be careful: The autocommands may delete any buffer and change
	     * the current buffer.
	     * - If the buffer we are going to edit is deleted, give up.
	     * - If the current buffer is deleted, prefer to load the new
	     *   buffer when loading a buffer is required.  This avoids
	     *   loading another buffer which then must be closed again.
	     * - If we ended up in the new buffer already, need to skip a few
	     *	 things, set auto_buf.
	     */
	    જો (buf->b_fname != ખાલી)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, ખાલી, ખાલી, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    જો (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_સંદેશ(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		જાઓ theend;
	    }
#ifdef FEAT_EVAL
	    જો (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		જાઓ theend;
	    }
#endif
	    જો (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    નહીંતર
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		જો (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_buffer(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != ખાલી);

		// Autocommands may have closed the window.
		જો (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		જો (aborting() && curwin->w_buffer != ખાલી)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    જાઓ theend;
		}
#endif
		// Be careful again, like above.
		જો (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    જો (curwin->w_buffer == ખાલી)
			buf = lastbuf;
		    નહીંતર
		    {
			delbuf_સંદેશ(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			જાઓ theend;
		    }
		}
		જો (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		નહીંતર
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    જો (curwin->w_buffer == ખાલી
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    જો (curwin->w_buffer != ખાલી)
			--curwin->w_buffer->b_nwindows;

		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    જો (!oldbuf && eap != ખાલી)
		    {
			set_file_options(TRUE, eap);
			set_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#ifdef FEAT_SPELL
		did_get_winopts = TRUE;
#endif
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.col = 0;
    }
    નહીંતર // !other_file
    {
	જો ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    જાઓ theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    જો ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    નહીંતર
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	જો (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    જો (buf != curbuf)
	જાઓ theend;
#ifdef FEAT_EVAL
    જો (aborting())	    // autocmds may abort script processing
	જાઓ theend;
#endif

    // Since we are starting to edit a file, consider the filetype to be
    // unset.  Helps for when an autocommand changes files and expects syntax
    // highlighting to work in the other file.
    curbuf->b_did_filetype = false;

/*
 * other_file	oldbuf
 *  FALSE	FALSE	    re-edit same file, buffer is re-used
 *  FALSE	TRUE	    re-edit same file, nothing changes
 *  TRUE	FALSE	    start editing new file, new buffer
 *  TRUE	TRUE	    start editing in existing buffer (nothing to do)
 */
    જો (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	જો (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	જો (buf->b_fname != ખાલી)
	    new_name = vim_strsave(buf->b_fname);
	નહીંતર
	    new_name = ખાલી;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	જો (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_લીટી_ગણતરી <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    જો (u_savecommon(0, curbuf->b_ml.ml_લીટી_ગણતરી + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		જાઓ theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	નહીંતર
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	જો (!bufref_valid(&bufref))
	{
	    delbuf_સંદેશ(new_name);	// frees new_name
	    જાઓ theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	જો (buf != curbuf)
	    જાઓ theend;
#ifdef FEAT_EVAL
	જો (aborting())	    // autocmds may abort script processing
	    જાઓ theend;
#endif
	buf_clear_file(curbuf);
	curbuf->b_op_start.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_end.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    // If the file name was changed, reset the not-edit flag so that ":write"
    // works.
    જો (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    જો (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#ifdef FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		જો (win->w_buffer == curbuf)
		    foldUpdateAll(win);
	}
#endif

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	જો (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    જો (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    જો (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#જો defined(FEAT_EVAL)
	    જો (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#નહીંતર
	    (શૂન્ય)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    જો (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	નહીંતર
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, ખાલી, ખાલી, FALSE,
							      curbuf, &retval);
	    જો ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, ખાલી, ખાલી, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	જો (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    જો (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (પૂર્ણાંક)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	જો (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_લીટી_abv_curs();

	maketitle();
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	જો (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    જો (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    જો (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(શૂન્ય)parse_spelllang(curwin);
#endif

    જો (command == ખાલી)
    {
	જો (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	નહીંતર જો (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    જો (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = true;
	    }
	    નહીંતર
		beginline(BL_SOL | BL_FIX);
	}
	નહીંતર			// no line number, go to last line in Ex mode
	{
	    જો (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    જો (oldbuf && !auto_buf)
    {
	પૂર્ણાંક	સંદેશ_scroll_save = સંદેશ_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	જો (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    સંદેશ_scroll = FALSE;
	જો (!સંદેશ_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	સંદેશ_start();
	સંદેશ_scroll = સંદેશ_scroll_save;
	સંદેશ_scrolled_ign = TRUE;

	જો (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	સંદેશ_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    જો (command != ખાલી)
	do_cmdline(command, ખાલી, ખાલી, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    જો (curbuf->b_kmap_state & KEYMAP_INIT)
	(શૂન્ય)keymap_init();
#endif

    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    જો (!skip_redraw)
    {
	n = *so_ptr;
	જો (topline == 0 && command == ખાલી)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    જો (p_im && (State & MODE_ઉમેરો) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    જો (p_acd && curbuf->b_ffname != ખાલી)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	જો (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#જો defined(FEAT_NETBEANS_INTG)
    જો (curbuf->b_ffname != ખાલી)
    {
# ifdef FEAT_NETBEANS_INTG
	જો ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    જો (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#જો defined(FEAT_EVAL)
    જો (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, ખાલી, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    પાછા retval;
}

    સ્થિર શૂન્ય
delbuf_સંદેશ(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_buffer_str),
					   name == ખાલી ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = ખાલી;
    au_new_curbuf.br_buf_free_ગણતરી = 0;
}

સ્થિર પૂર્ણાંક append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    સ્થિર char_u *
get_append_લીટી(exarg_T *eap, પૂર્ણાંક indent)
{
    char_u	*theline;
    char_u	*p;

    જો (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    નહીંતર જો (eap->ea_getline == ખાલી)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	જો (eap->nextcmd == ખાલી)
	    પાછા ખાલી;
	p = vim_strchr(eap->nextcmd, NL);
	જો (p == ખાલી)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	જો (*p != NUL)
	    ++p;
	નહીંતર
	    p = ખાલી;
	eap->nextcmd = p;
    }
    નહીંતર
    {
	પૂર્ણાંક save_State = State;

	// Set State to avoid the cursor shape to be set to MODE_INSERT
	// state when getline() returns.
	State = MODE_CMDLINE;
	theline = eap->ea_getline(
#ifdef FEAT_EVAL
		eap->cstack->cs_looplevel > 0 ? -1 :
#endif
		NUL, eap->cookie, indent, TRUE);
	State = save_State;
    }
    પાછા theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    શૂન્ય
ex_append(exarg_T *eap)
{
    char_u	*theline;
    પૂર્ણાંક		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    પૂર્ણાંક		indent = 0;
    char_u	*p;
    પૂર્ણાંક		vcol;
    પૂર્ણાંક		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    જો (not_in_vim9(eap, eap->skip) == FAIL)
	પાછા;
#endif
    જો (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	જ્યારે ((theline = get_append_લીટી(eap, 0)) != ખાલી)
	{
	    પૂર્ણાંક end = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    જો (end)
		તોડો;
	}
	પાછા;
    }

    // the ! flag toggles autoindent
    જો (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    જો (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    જો (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    જો (empty && lnum == 1)
	lnum = 0;

    State = MODE_ઉમેરો;		    // behave like in Insert mode
    જો (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    માટે (;;)
    {
	સંદેશ_scroll = TRUE;
	need_wait_return = FALSE;
	જો (curbuf->b_p_ai)
	{
	    જો (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    નહીંતર જો (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_લીટી(eap, indent);
	લીટીઓ_ડાબું = Rows - 1;
	જો (theline == ખાલી)
	    તોડો;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	જો (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	માટે (p = theline; indent > vcol; ++p)
	{
	    જો (*p == ' ')
		++vcol;
	    નહીંતર જો (*p == TAB)
		vcol += 8 - vcol % 8;
	    નહીંતર
		તોડો;
	}
	જો ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    તોડો;
	}

	// don't use autoindent if nothing was typed.
	જો (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	જો (empty)
	    // there are no marks below the inserted lines
	    appended_લીટીઓ(lnum, 1L);
	નહીંતર
	    appended_લીટીઓ_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	જો (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    જો (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_લીટી_ગણતરી) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_લીટી_ગણતરી;
	જો (eap->cmdidx != CMD_append)
	    --curbuf->b_op_start.lnum;
	curbuf->b_op_end.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_start.lnum;
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    }
    curwin->w_cursor.lnum = lnum;
    check_cursor_lnum();
    beginline(BL_SOL | BL_FIX);

    need_wait_return = FALSE;	// don't use wait_return() now
    ex_no_reprint = TRUE;
}

/*
 * ":change"
 */
    શૂન્ય
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ifdef FEAT_EVAL
    જો (not_in_vim9(eap, eap->skip) == FAIL)
	પાછા;
#endif
    જો (eap->skip)
    {
	ex_append(eap);
	પાછા;
    }
    જો (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	પાછા;

    // the ! flag toggles autoindent
    જો (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    માટે (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	જો (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    તોડો;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_લીટીઓ_mark(eap->line1, (long)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    શૂન્ય
ex_z(exarg_T *eap)
{
    char_u	*x;
    long	bigness;
    char_u	*kind;
    પૂર્ણાંક		minus = 0;
    linenr_T	start, end, curs, i;
    પૂર્ણાંક		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    જો (eap->forceit)
	bigness = Rows - 1;
    નહીંતર જો (!ONE_WINDOW)
	bigness = curwin->w_height - 3;
    નહીંતર
	bigness = curwin->w_p_scr * 2;
    જો (bigness < 1)
	bigness = 1;

    x = eap->arg;
    kind = x;
    જો (*kind == '-' || *kind == '+' || *kind == '='
					      || *kind == '^' || *kind == '.')
	++x;
    જ્યારે (*x == '-' || *x == '+')
	++x;

    જો (*x != 0)
    {
	જો (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_argument_to_z));
	    પાછા;
	}
	નહીંતર
	{
	    bigness = atol((અક્ષર *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    જો (bigness > 2 * curbuf->b_ml.ml_લીટી_ગણતરી || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_લીટી_ગણતરી;

	    p_window = bigness;
	    જો (*kind == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    જો (*kind == '-' || *kind == '+')
	માટે (x = kind + 1; *x == *kind; ++x)
	    ;

    પસંદગી (*kind)
    {
	સ્થિતિ '-':
	    start = lnum - bigness * (linenr_T)(x - kind) + 1;
	    end = start + bigness - 1;
	    curs = end;
	    તોડો;

	સ્થિતિ '=':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    તોડો;

	સ્થિતિ '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    તોડો;

	સ્થિતિ '.':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = end;
	    તોડો;

	મૂળભૂત:  // '+'
	    start = lnum;
	    જો (*kind == '+')
		start += bigness * (linenr_T)(x - kind - 1) + 1;
	    નહીંતર જો (eap->addr_ગણતરી == 0)
		++start;
	    end = start + bigness - 1;
	    curs = end;
	    તોડો;
    }

    જો (start < 1)
	start = 1;

    જો (end > curbuf->b_ml.ml_લીટી_ગણતરી)
	end = curbuf->b_ml.ml_લીટી_ગણતરી;

    જો (curs > curbuf->b_ml.ml_લીટી_ગણતરી)
	curs = curbuf->b_ml.ml_લીટી_ગણતરી;
    નહીંતર જો (curs < 1)
	curs = 1;

    માટે (i = start; i <= end; i++)
    {
	જો (minus && i == lnum)
	{
	    સંદેશ_putchar('\n');

	    માટે (j = 1; j < Columns; j++)
		સંદેશ_putchar('-');
	}

	print_લીટી(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	જો (minus && i == lnum)
	{
	    સંદેશ_putchar('\n');

	    માટે (j = 1; j < Columns; j++)
		સંદેશ_putchar('-');
	}
    }

    જો (curwin->w_cursor.lnum != curs)
    {
	curwin->w_cursor.lnum = curs;
	curwin->w_cursor.col = 0;
    }
    ex_no_reprint = TRUE;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    પૂર્ણાંક
check_restricted(શૂન્ય)
{
    જો (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_in_rvim));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    પૂર્ણાંક
check_secure(શૂન્ય)
{
    જો (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_શોધ));
	પાછા TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    જો (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	પાછા TRUE;
    }
#endif
    પાછા FALSE;
}

સ્થિર char_u	*old_sub = ખાલી;	// previous substitute pattern
સ્થિર પૂર્ણાંક	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક	do_all;		// do multiple substitutions per line
    પૂર્ણાંક	do_ask;		// ask for confirmation
    પૂર્ણાંક	do_ગણતરી;	// count only
    પૂર્ણાંક	do_error;	// if false, ignore errors
    પૂર્ણાંક	do_print;	// print last line with subs.
    પૂર્ણાંક	do_list;	// list last line with subs.
    પૂર્ણાંક	do_સંખ્યા;	// list last line with line nr
    પૂર્ણાંક	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *start, પૂર્ણાંક delimiter)
{
    char_u *p = start;

    જ્યારે (p[0])
    {
	જો (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    તોડો;
	}
	જો (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    પાછા p;
}

    સ્થિર પૂર્ણાંક
check_regexp_delim(પૂર્ણાંક c)
{
    જો (SAFE_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    શૂન્ય
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    long	i = 0;
    regmmatch_T regmatch;
    સ્થિર subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ifdef FEAT_EVAL
    subflags_T	subflags_save;
#endif
    પૂર્ણાંક		save_do_all;		// remember user specified 'g' flag
    પૂર્ણાંક		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = ખાલી;		// init for GCC
    string_T	pat = {ખાલી, 0};
    પૂર્ણાંક		delimiter;
    size_t	sublen;
    પૂર્ણાંક		got_quit = FALSE;
    પૂર્ણાંક		got_match = FALSE;
    પૂર્ણાંક		which_pat;
    char_u	*cmd;
    char_u	*p;
    પૂર્ણાંક		save_State;
    linenr_T	first_લીટી = 0;		// first changed line
    linenr_T	last_લીટી = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_લીટી_ગણતરી = curbuf->b_ml.ml_લીટી_ગણતરી;
    linenr_T	line2;
    long	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    પૂર્ણાંક		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    પૂર્ણાંક		start_nsubs;
    પૂર્ણાંક		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ifdef FEAT_EVAL
    પૂર્ણાંક		save_ma = 0;
    પૂર્ણાંક		save_sandbox = 0;
#endif
#ifdef FEAT_PROP_POPUP
    textprop_T	*text_props = ખાલી;
#endif

    cmd = eap->arg;
    જો (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    start_nsubs = sub_nsubs;

    જો (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	// use last used regexp
    નહીંતર
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    જો (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == ખાલી)
    {
	// don't accept alphanumeric for separator
	જો (check_regexp_delim(*cmd) == FAIL)
	    પાછા;
#ifdef FEAT_EVAL
	જો (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    પાછા;
#endif

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	જો (*cmd == '\\')
	{
	    જો (in_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_in_vim9_script));
		પાછા;
	    }
	    ++cmd;
	    જો (vim_strchr((char_u *)"/?&", *cmd) == ખાલી)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		પાછા;
	    }
	    જો (*cmd != '&')
		which_pat = RE_શોધ;	    // use last '/' pattern
	    STR_LITERAL_SET(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	નહીંતર		// find the end of the regexp
	{
	    which_pat = RE_LAST;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, ખાલી, ખાલી);
	    pat.length = (size_t)(cmd - pat.string);
	    જો (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	જો (sub == ખાલી)
	    // out of memory
	    પાછા;

	જો (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    જો (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != ખાલી)
	    {
		જો (old_sub == ખાલી)	// there is no previous command
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    vim_free(sub);
		    પાછા;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		જો (sub == ખાલી)
		    // out of memory
		    પાછા;
	    }
	    નહીંતર જો (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		જો (old_sub == ખાલી)
		{
		    // out of memory
		    vim_free(sub);
		    પાછા;
		}
	    }
	}
    }
    નહીંતર જો (!eap->skip)	// use previous pattern and substitution
    {
	જો (old_sub == ખાલી)	// there is no previous command
	{
	    emsg(_(e_no_previous_substitute_regular_expression));
	    પાછા;
	}
	pat.string = ખાલી;		// search_regcomp() will use previous pattern
	pat.length = 0;
	sub = vim_strsave(old_sub);
	જો (sub == ખાલી)
	    પાછા;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    જો (pat.string != ખાલી && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_લીટીઓ_ગણતરી;

	જો (eap->skip)
	{
	    vim_free(sub);
	    પાછા;
	}
	curwin->w_cursor.lnum = eap->line1;
	જો (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	નહીંતર જો (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	નહીંતર જો (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_લીટીઓ_ગણતરી = eap->line2 - eap->line1 + 1;
	જો (eap->line2 < curbuf->b_ml.ml_લીટી_ગણતરી)
	    ++joined_લીટીઓ_ગણતરી;
	જો (joined_લીટીઓ_ગણતરી > 1)
	{
	    (શૂન્ય)do_join(joined_લીટીઓ_ગણતરી, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_લીટીઓ_ગણતરી - 1;
	    sub_nlines = 1;
	    (શૂન્ય)do_sub_સંદેશ(FALSE);
	    ex_may_print(eap);
	}

	જો (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.length, magic_isset());
	// put pattern in history
	add_to_history(HIST_શોધ, pat.string, pat.length, TRUE, NUL);
	vim_free(sub);

	પાછા;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    જો (*cmd == '&')
	++cmd;
    નહીંતર
    {
#ifdef FEAT_EVAL
	જો (in_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	નહીંતર
#endif
	જો (!p_ed)
	{
	    જો (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    નહીંતર
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_ગણતરી = FALSE;
	subflags.do_સંખ્યા = FALSE;
	subflags.do_ic = 0;
    }
    જ્યારે (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	જો (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	નહીંતર જો (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	નહીંતર જો (*cmd == 'n')
	    subflags.do_ગણતરી = TRUE;
	નહીંતર જો (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	નહીંતર જો (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_LAST;
	નહીંતર જો (*cmd == 'p')
	    subflags.do_print = TRUE;
	નહીંતર જો (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_સંખ્યા = TRUE;
	}
	નહીંતર જો (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	નહીંતર જો (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	નહીંતર જો (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	નહીંતર
	    તોડો;
	++cmd;
    }
    જો (subflags.do_ગણતરી)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    જો (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	જો (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_positive_ગણતરી_required));
	    vim_free(sub);
	    પાછા;
	}
	નહીંતર જો (i >= INT_MAX)
	{
	    અક્ષર	buf[20];
	    vim_snprintf(buf, કદ(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    પાછા;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	જો (eap->line2 > curbuf->b_ml.ml_લીટી_ગણતરી)
	    eap->line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    જો (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	set_nextcmd(eap, cmd);
	જો (eap->nextcmd == ખાલી)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    પાછા;
	}
    }

    જો (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	પાછા;
    }

    જો (!subflags.do_ગણતરી && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	પાછા;
    }

    જો (શોધ_regcomp(pat.string, pat.length, ખાલી, RE_SUBST, which_pat, શોધ_HIS, &regmatch) == FAIL)
    {
	જો (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	પાછા;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    જો (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    નહીંતર જો (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = ખાલી;
    sub_firstline.length = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    જો (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	જો (p == ખાલી)
	    પાછા;
	sub = p;
    }
    નહીંતર
    {
	p = regtilde(sub, magic_isset());

	જો (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    માટે (lnum = eap->line1; lnum <= line2 && !(got_quit
#જો defined(FEAT_EVAL)
		|| aborting()
#endif
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ખાલી);
	જો (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_start = {ખાલી, 0};
	    size_t	new_start_size = 0;
	    char_u	*new_end;
	    પૂર્ણાંક		did_sub = FALSE;
	    પૂર્ણાંક		lastone;
	    size_t	copy_len;
	    size_t	needed_size;
	    long	nmatch_tl = 0;	// nr of lines matched below lnum
	    પૂર્ણાંક		do_again;	// do it again after joining lines
	    પૂર્ણાંક		skip_match = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_split = false;	// "\r" split the line
#ifdef FEAT_PROP_POPUP
	    પૂર્ણાંક		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	total_added =  0;
	    પૂર્ણાંક		text_prop_ગણતરી = 0;
#endif

	    /*
	     * The new text is build up step by step, to avoid too much
	     * copying.  There are these pieces:
	     * sub_firstline	The old text, unmodified.
	     * copycol		Column in the old text where we started
	     *			looking for a match; from here old text still
	     *			needs to be copied to the new text.
	     * matchcol		Column number of the old text where to look
	     *			for the next match.  It's just after the
	     *			previous match or one further.
	     * prev_matchcol	Column just after the previous match (if any).
	     *			Mostly equal to matchcol, except for the first
	     *			match and after skipping an empty match.
	     * regmatch.*pos	Where the pattern matched in the old text.
	     * new_start	The new text, all that has been produced so
	     *			far.
	     * new_end		The new text, where to append new text.
	     *
	     * lnum		The line number where we found the start of
	     *			the match.  Can be below the line we searched
	     *			when there is a \n before a \zs in the
	     *			pattern.
	     * sub_firstlnum	The line number in the buffer where to look
	     *			for a match.  Can be different from "lnum"
	     *			when the pattern or substitute string contains
	     *			line breaks.
	     *
	     * Special situations:
	     * - When the substitute string contains a line break, the part up
	     *   to the line break is inserted in the text, but the copy of
	     *   the original line is kept.  "sub_firstlnum" is adjusted for
	     *   the inserted lines.
	     * - When the matched pattern contains a line break, the old line
	     *   is taken from the line at the end of the pattern.  The lines
	     *   in the match are deleted later, "sub_firstlnum" is adjusted
	     *   accordingly.
	     *
	     * The new text is built up in new_start[].  It has some extra
	     * room to avoid using alloc()/free() too often.  new_start_size is
	     * the length of the allocated memory at new_start.
	     *
	     * Make a copy of the old line, so it won't be taken away when
	     * updating the screen or handling a multi-line match.  The "old_"
	     * pointers point into this copy.
	     */
	    sub_firstlnum = lnum;
	    copycol = 0;
	    matchcol = 0;

	    // At first match, remember current cursor position.
	    જો (!got_match)
	    {
		setpcmark();
		got_match = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    માટે (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		જો (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
		    તોડો;

		જો (sub_firstline.string == ખાલી)
		{
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    જો (sub_firstline.string == ખાલી)
		    {
			vim_free(new_start.string);
			જાઓ outofmem;
		    }
		}

		// Save the line number of the last change for the final
		// cursor position (just like Vi).
		curwin->w_cursor.lnum = lnum;
		do_again = FALSE;

		/*
		 * 1. Match empty string does not count, except for first
		 * match.  This reproduces the strange vi behaviour.
		 * This also catches endless loops.
		 */
		જો (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    જો (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_match = TRUE;
		    નહીંતર
		    {
			 // search for a match at next column
			જો (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			નહીંતર
			    ++matchcol;
		    }
		    જાઓ skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		જો (subflags.do_ગણતરી)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    જો (nmatch > 1)
		    {
			matchcol = (colnr_T)sub_firstline.length;
			nmatch = 1;
			skip_match = TRUE;
		    }
		    sub_nsubs++;
		    did_sub = TRUE;
#ifdef FEAT_EVAL
		    // Skip the substitution, unless an expression is used,
		    // then it is evaluated in the sandbox.
		    જો (!(sub[0] == '\\' && sub[1] == '='))
#endif
			જાઓ skip;
		}

		જો (subflags.do_ask)
		{
		    પૂર્ણાંક typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = MODE_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.col = regmatch.startpos[0].col;
		    જો (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    જો (vim_strchr(p_cpo, CPO_UNDO) != ખાલી)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    જ્યારે (subflags.do_ask)
		    {
			જો (exmode_active)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_લીટી_no_prefix(lnum,
					 subflags.do_સંખ્યા, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, ખાલી, ખાલી, 0);
			    curwin->w_cursor.col = regmatch.endpos[0].col - 1;
			    જો (curwin->w_cursor.col < 0)
				curwin->w_cursor.col = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   ખાલી, ખાલી, &ec, 0);
			    curwin->w_cursor.col = regmatch.startpos[0].col;
			    જો (subflags.do_સંખ્યા || curwin->w_p_nu)
			    {
				પૂર્ણાંક numw = સંખ્યા_width(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    સંદેશ_start();
			    માટે (i = 0; i < (long)sc; ++i)
				સંદેશ_putchar(' ');
			    માટે ( ; i <= (long)ec; ++i)
				સંદેશ_putchar('^');

			    resp = getexmodeline('?', ખાલી, 0, TRUE);
			    જો (resp != ખાલી)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				જો (ex_normal_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			નહીંતર
			{
			    string_T orig_લીટી = {ખાલી, 0};
			    પૂર્ણાંક    len_change = 0;
			    પૂર્ણાંક	   save_p_lz = p_lz;
#ifdef FEAT_FOLDING
			    પૂર્ણાંક save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#endif
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    જો (new_start.string != ખાલી)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_લીટી.length = ml_get_len(lnum);
				orig_લીટી.string = vim_strnsave(ml_get(lnum), orig_લીટી.length);
				જો (orig_લીટી.string != ખાલી)
				{
				    string_T new_લીટી;

				    new_લીટી.string = concat_str(new_start.string,
						     sub_firstline.string + copycol);
				    જો (new_લીટી.string == ખાલી)
					VIM_CLEAR_STRING(orig_લીટી);
				    નહીંતર
				    {
					new_લીટી.length =
					    new_start.length + (sub_firstline.length - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (પૂર્ણાંક)new_લીટી.length - (પૂર્ણાંક)orig_લીટી.length;
					curwin->w_cursor.col += len_change;
					ml_replace(lnum, new_લીટી.string, FALSE);
				    }
				}
			    }

			    શોધ_match_લીટીઓ = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    શોધ_match_endcol = regmatch.endpos[0].col
								 + len_change;
			    જો (શોધ_match_લીટીઓ == 0
						   && શોધ_match_endcol == 0)
				// highlight at least one character for /^/
				શોધ_match_endcol = 1;
			    highlight_match = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_match = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ifdef FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#endif
			    જો (સંદેશ_row == Rows - 1)
				સંદેશ_didout = FALSE;	// avoid a scroll-up
			    સંદેશ_starthere();
			    i = સંદેશ_scroll;
			    સંદેશ_scroll = 0;		// truncate msg when
							// needed
			    સંદેશ_no_more = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    સંદેશ_no_more = FALSE;
			    સંદેશ_scroll = i;
			    showruler(TRUE);
			    windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
			    RedrawingDisabled = save_RedrawingDisabled;

#ifdef USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#endif
			    ++no_mapping;	// don't map this key
			    ++allow_ચાવીઓ;	// allow special keys
			    typed = plain_vgetc();
			    --allow_ચાવીઓ;
			    --no_mapping;

			    // clear the question
			    સંદેશ_didout = FALSE;	// don't scroll up
			    સંદેશ_col = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    જો (orig_લીટી.string != ખાલી)
				ml_replace(lnum, orig_લીટી.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			જો (typed == 'q' || typed == ESC || typed == Ctrl_C
#ifdef UNIX
				|| typed == intr_char
#endif
				)
			{
			    got_quit = TRUE;
			    તોડો;
			}
			જો (typed == 'n')
			    તોડો;
			જો (typed == 'y')
			    તોડો;
			જો (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    તોડો;
			}
			જો (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    તોડો;
			}
			જો (typed == Ctrl_E)
			    scrollup_clamp();
			નહીંતર જો (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    જો (vim_strchr(p_cpo, CPO_UNDO) != ખાલી)
			--no_u_sync;

		    જો (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			જો (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.length;
			    skip_match = TRUE;
			}
			જાઓ skip;
		    }
		    જો (got_quit)
			જાઓ skip;
		}

		// Move the cursor to the start of the match, so that we can
		// use "\=col(".").
		curwin->w_cursor.col = regmatch.startpos[0].col;

		/*
		 * 3. substitute the string.
		 */
#ifdef FEAT_EVAL
		save_ma = curbuf->b_p_ma;
		save_sandbox = sandbox;
		જો (subflags.do_ગણતરી)
		{
		    // prevent accidentally changing the buffer by a function
		    curbuf->b_p_ma = FALSE;
		    sandbox++;
		}
		// Save flags for recursion.  They can change for e.g.
		// :s/^/\=execute("s#^##gn")
		subflags_save = subflags;

		// Disallow changing text or switching window in an expression.
		++textlock;
#endif
		// Get length of substitution part, including the NUL.
		// When it fails sublen is zero.
		sublen = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, sub_firstline.string, 0,
		    REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));
#ifdef FEAT_EVAL
		--textlock;

		// If getting the substitute string caused an error, don't do
		// the replacement.
		// Don't keep flags set by a recursive call.
		subflags = subflags_save;
		જો (sublen == 0 || aborting() || subflags.do_ગણતરી)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    જાઓ skip;
		}
#endif

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		જો (nmatch > curbuf->b_ml.ml_લીટી_ગણતરી - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_લીટી_ગણતરી - sub_firstlnum + 1;
		    skip_match = TRUE;
		    // safety check
		    જો (nmatch < 0)
			જાઓ skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		જો (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.length = sub_firstline.length;
#ifdef FEAT_PROP_POPUP
		    જો (curbuf->b_has_textprop)
		    {
			પૂર્ણાંક bytes_added = (પૂર્ણાંક)sublen - 1 - (regmatch.endpos[0].col
						   - regmatch.startpos[0].col);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			જો (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       bytes_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			total_added += (colnr_T)bytes_added;
		    }
#endif
		}
		નહીંતર
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    જો (curbuf->b_has_textprop)
		    {
			char_u	*prop_start;

			// Props in the first line may be shortened or deleted
			જો (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			total_added -=
			    (colnr_T)(sub_firstline.length - regmatch.startpos[0].col);

			// Props in the last line may be moved or deleted
			જો (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].col, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			text_prop_ગણતરી = get_text_props(curbuf, lastlnum,
							   &prop_start, FALSE);
			જો (text_prop_ગણતરી > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(text_props);
			    text_props = ALLOC_MULT(textprop_T,
							      text_prop_ગણતરી);
			    જો (text_props != ખાલી)
			    {
				mch_memmove(text_props, prop_start,
					 text_prop_ગણતરી * કદ(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				પૂર્ણાંક wi = 0;
				માટે (પૂર્ણાંક pi = 0; pi < text_prop_ગણતરી; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    જો (text_props[pi].tp_id < 0
					    || (text_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					ચાલુ;
				    text_props[wi] = text_props[pi];
				    text_props[wi].tp_col +=
					regmatch.startpos[0].col + (colnr_T)sublen - 1;
				    text_props[wi].u.tp_text = ખાલી;
				    ++wi;
				}
				text_prop_ગણતરી = wi;
			    }
			}
		    }
#endif
		    tmp.string = ml_get(lastlnum);
		    tmp.length = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    જો (curbuf->b_has_textprop)
			total_added +=
			    (colnr_T)(tmp.length - regmatch.endpos[0].col);
#endif
		}

		copy_len = (size_t)(regmatch.startpos[0].col - copycol);
		needed_size = copy_len
		    + (tmp.length - (size_t)regmatch.endpos[0].col) + sublen + 1;

		જો (new_start.string == ખાલી)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_size = needed_size + 50;
		    જો ((new_start.string = alloc_clear(new_start_size)) == ખાલી)
			જાઓ outofmem;
		    *new_start.string = NUL;
		    new_start.length = 0;
		}
		નહીંતર
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_size += new_start.length;
		    જો (needed_size > new_start_size)
		    {
			new_start_size = needed_size + 50;
			જો ((p1 = alloc_clear(new_start_size)) == ખાલી)
			{
			    vim_free(new_start.string);
			    જાઓ outofmem;
			}
			mch_memmove(p1, new_start.string, new_start.length + 1);
			vim_free(new_start.string);
			new_start.string = p1;
		    }
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_start.string + new_start.length,
		    sub_firstline.string + copycol, copy_len);
		new_start.length += copy_len;

		// new text added here
		new_end = new_start.string + new_start.length;

		જો (new_start_size - copy_len < sublen)
		    sublen = new_start_size - copy_len - 1;

#ifdef FEAT_EVAL
		++textlock;
#endif
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_end, (પૂર્ણાંક)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		જો (n > 0)
		    new_start.length += n - 1;	    // remove 1 for the NUL

#ifdef FEAT_EVAL
		--textlock;
#endif
		sub_nsubs++;
		did_sub = TRUE;

		// Move the cursor to the start of the line, to avoid that it
		// is beyond the end of the line after the substitution.
		curwin->w_cursor.col = 0;

		// For a multi-line match, make a copy of the last matched
		// line and continue in that one.
		જો (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    જો (sub_firstline.string == ખાલી)
		    {
			vim_free(new_start.string);
			જાઓ outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    જો (sub_firstlnum <= line2)
			do_again = TRUE;
		    નહીંતર
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].col;

		જો (skip_match)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    જો (sub_firstline.string == ખાલી)
		    {
			vim_free(new_start.string);
			જાઓ outofmem;
		    }
		    sub_firstline.length = 0;
		    copycol = 0;
		}

		/*
		 * Now the trick is to replace CTRL-M chars with a real line
		 * break.  This would make it impossible to insert a CTRL-M in
		 * the text.  The line break can be avoided by preceding the
		 * CTRL-M with a backslash.  To be able to insert a backslash,
		 * they must be doubled in the string and are halved here.
		 * That is Vi compatible.
		 */
		માટે (p1 = new_end; *p1; ++p1)
		{
		    જો (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_start.length - (p1 - new_start.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_start.length;
#ifdef FEAT_PROP_POPUP
			જો (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    જો (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_start.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#endif
		    }
		    નહીંતર જો (*p1 == CAR)
		    {
			જો (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_start.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_start.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    જો (subflags.do_ask)
				appended_લીટીઓ(lnum - 1, 1L);
			    નહીંતર
			    {
				જો (first_લીટી == 0)
				    first_લીટી = lnum;
				last_લીટી = lnum + 1;
			    }
#ifdef FEAT_PROP_POPUP
			    adjust_props_for_split(lnum + 1, lnum,
							       plen, 1, FALSE);
#endif
			    // all line numbers increase
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    did_split = true;
			    // move the cursor to the new line, like Vi
			    ++curwin->w_cursor.lnum;
			    // copy the rest
			    n = new_start.length - (size_t)plen;
			    mch_memmove(new_start.string, p1 + 1, n + 1);
			    new_start.length = n;
			    p1 = new_start.string - 1;
			}
		    }
		    નહીંતર જો (has_mbyte)
			p1 += (*mb_ptr2len)(p1) - 1;
		}

		/*
		 * 4. If do_all is set, find next match.
		 * Prevent endless loop with patterns that match empty
		 * strings, e.g. :s/$/pat/g or :s/[a-z]* /(&)/g.
		 * But ":s/\n/#/" is OK.
		 */
skip:
		// We already know that we did the last subst when we are at
		// the end of the line, except that a pattern like
		// "bar\|\nfoo" may match at the NUL.  "lnum" can be below
		// "line2" when there is a \zs in the pattern after a line
		// break.
		lastone = (skip_match
			|| got_int
			|| got_quit
			|| lnum > line2
			|| !(subflags.do_all || do_again)
			|| (sub_firstline.string[matchcol] == NUL && nmatch <= 1
					 && !re_multiline(regmatch.regprog)));
		nmatch = -1;

		/*
		 * Replace the line in the buffer when needed.  This is
		 * skipped when there are more matches.
		 * The check for nmatch_tl is needed for when multi-line
		 * matching must replace the lines before trying to do another
		 * match, otherwise "\@<=" won't work.
		 * When the match starts below where we start searching also
		 * need to replace the line first (using \zs after \n).
		 * When asking, undo is synced at every match, so a line split
		 * by "\r" must be replaced in the same undo block.
		 */
		જો (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_split)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, ખાલી)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    જો (new_start.string != ખાલી)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * "matchcol" has to be adjusted, we use the end of
			 * the line as reference, because the substitute may
			 * have changed the number of characters.  Same for
			 * "prev_matchcol".
			 */
			STRCPY(new_start.string + new_start.length,
			    sub_firstline.string + copycol);
			new_start.length += sub_firstline.length - (size_t)copycol;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);

			જો (u_savesub(lnum) != OK)
			    તોડો;
			ml_replace(lnum, new_start.string, TRUE);
#ifdef FEAT_PROP_POPUP
			જો (text_props != ખાલી)
			    add_text_props(lnum, text_props, text_prop_ગણતરી);
#endif
			જો (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    જો (u_savedel(lnum, nmatch_tl) != OK)
				તોડો;
			    માટે (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    જો (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    નહીંતર
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    જો (subflags.do_ask)
				deleted_લીટીઓ(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			જો (subflags.do_ask)
			    changed_bytes(lnum, 0);
			નહીંતર
			{
			    જો (first_લીટી == 0)
				first_લીટી = lnum;
			    last_લીટી = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_start.string;
			sub_firstline.length = new_start.length;
			new_start.string = ખાલી;
			new_start.length = 0;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);
			copycol = 0;
			did_split = false;
		    }
		    જો (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, ખાલી);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    જો (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			જો (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			તોડો;
		    }
		}

		લીટી_breakcheck();
	    }

	    જો (did_sub)
		++sub_nlines;
	    vim_free(new_start.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	લીટી_breakcheck();
    }

    જો (first_લીટી != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_લીટી_ગણતરી - old_લીટી_ગણતરી;
	changed_લીટીઓ(first_લીટી, 0, last_લીટી - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ifdef FEAT_PROP_POPUP
    vim_free(text_props);
#endif

    // ":s/pat//n" doesn't move the cursor
    જો (subflags.do_ગણતરી)
	curwin->w_cursor = old_cursor;

    જો (sub_nsubs > start_nsubs)
    {
	જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_start.lnum = eap->line1;
	    curbuf->b_op_end.lnum = line2;
	    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	}

	જો (!global_busy)
	{
	    // when interactive leave cursor on the match
	    જો (!subflags.do_ask)
	    {
		જો (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		નહીંતર
		    beginline(BL_WHITE | BL_FIX);
	    }
	    જો (!do_sub_સંદેશ(subflags.do_ગણતરી) && subflags.do_ask)
		સંદેશ("");
	}
	નહીંતર
	    global_need_beginline = TRUE;
	જો (subflags.do_print)
	    print_લીટી(curwin->w_cursor.lnum,
					 subflags.do_સંખ્યા, subflags.do_list);
    }
    નહીંતર જો (!global_busy)
    {
	જો (got_int)		// interrupted
	    emsg(_(e_interrupted));
	નહીંતર જો (got_match)	// did find something but nothing substituted
	    સંદેશ("");
	નહીંતર જો (subflags.do_error)	// nothing found
	    semsg(_(e_pattern_not_found_str), get_શોધ_pat());
    }

#ifdef FEAT_FOLDING
    જો (subflags.do_ask && hasAnyFolding(curwin))
	// Cursor position may require updating
	changed_window_setting();
#endif

    vim_regfree(regmatch.regprog);
    vim_free(sub);

    // Restore the flag values, they can be used for ":&&".
    subflags.do_all = save_do_all;
    subflags.do_ask = save_do_ask;
}

/*
 * Give message for number of substitutions.
 * Can also be used after a ":global" command.
 * Return TRUE if a message was given.
 */
    પૂર્ણાંક
do_sub_સંદેશ(
    પૂર્ણાંક	    ગણતરી_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    જો (((sub_nsubs > p_report && (ચાવી_Typed || sub_nlines > 1 || p_report < 1))
		|| ગણતરી_only)
	    && messaging())
    {
	અક્ષર	*સંદેશ_single;
	અક્ષર	*સંદેશ_plural;

	જો (got_int)
	    STRCPY(સંદેશ_buf, _("(Interrupted) "));
	નહીંતર
	    *સંદેશ_buf = NUL;

	સંદેશ_single = ગણતરી_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	સંદેશ_plural = ગણતરી_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_add(સંદેશ_buf, કદ(સંદેશ_buf),
				 NGETTEXT(સંદેશ_single, સંદેશ_plural, sub_nlines),
				 sub_nsubs, (long)sub_nlines);

	જો (સંદેશ(સંદેશ_buf))
	    // save message to display it after redraw
	    set_keep_સંદેશ((char_u *)સંદેશ_buf, 0);
	પાછા TRUE;
    }
    જો (got_int)
    {
	emsg(_(e_interrupted));
	પાછા TRUE;
    }
    પાછા FALSE;
}

    સ્થિર શૂન્ય
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    જો (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", ખાલી, ખાલી, DOCMD_NOWAIT);
    નહીંતર
	do_cmdline(cmd, ખાલી, ખાલી, DOCMD_NOWAIT);
}

/*
 * Execute a global command of the form:
 *
 * g/pattern/X : execute X on all lines where pattern matches
 * v/pattern/X : execute X on all lines where pattern does not match
 *
 * where 'X' is an EX command
 *
 * The command character (as well as the trailing slash) is optional, and
 * is assumed to be 'p' if missing.
 *
 * This is implemented in two passes: first we scan the file for the pattern and
 * set a mark for each line that (not) matches. Secondly we execute the command
 * for each line that has a mark. This is required because after deleting
 * lines we do not know where to search for the next match.
 */
    શૂન્ય
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    પૂર્ણાંક		ndone = 0;
    પૂર્ણાંક		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*used_pat;
    regmmatch_T	regmatch;
    પૂર્ણાંક		match;
    પૂર્ણાંક		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    જો (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_લીટી_ગણતરી))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	પાછા;
    }

    જો (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    નહીંતર
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    જો (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	પાછા;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    જો (*cmd == '\\')
    {
	++cmd;
	જો (vim_strchr((char_u *)"/?&", *cmd) == ખાલી)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    પાછા;
	}
	જો (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	નહીંતર
	    which_pat = RE_શોધ;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    નહીંતર જો (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	પાછા;
    }
    નહીંતર જો (check_regexp_delim(*cmd) == FAIL)
    {
	પાછા;
    }
    નહીંતર
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, ખાલી, ખાલી);
	જો (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    જો (શોધ_regcomp(pat, patlen, &used_pat, RE_BOTH, which_pat, શોધ_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	પાછા;
    }

    જો (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ખાલી);
	જો ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    નહીંતર
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	માટે (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ખાલી);
	    જો (regmatch.regprog == ખાલી)
		તોડો;  // re-compiling regprog failed
	    જો ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    લીટી_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	જો (got_int)
	    સંદેશ(_(e_interrupted));
	નહીંતર જો (ndone == 0)
	{
	    જો (type == 'v')
	    {
		જો (in_vim9script())
		    semsg(_(e_pattern_found_in_every_લીટી_str), used_pat);
		નહીંતર
		    smsg(_("Pattern found in every line: %s"), used_pat);
	    }
	    નહીંતર
	    {
		જો (in_vim9script())
		    semsg(_(e_pattern_not_found_str), used_pat);
		નહીંતર
		    smsg(_("Pattern not found: %s"), used_pat);
	    }
	}
	નહીંતર
	{
#ifdef FEAT_CLIPBOARD_PROVIDER
	    inc_clip_provider();
#endif
#ifdef FEAT_CLIPBOARD
	    start_global_changes();
#endif
	    global_exe(cmd);
#ifdef FEAT_CLIPBOARD
	    end_global_changes();
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	    dec_clip_provider();
#endif
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Execute "cmd" on lines marked with ml_setmarked().
 */
    શૂન્ય
global_exe(char_u *cmd)
{
    linenr_T old_lcount;	// b_ml.ml_line_count before the command
    buf_T    *old_buf = curbuf;	// remember what buffer we started in
    linenr_T lnum;		// line number according to old situation

    /*
     * Set current position only once for a global command.
     * If global_busy is set, setpcmark() will not do anything.
     * If there is an error, global_busy will be incremented.
     */
    setpcmark();

    // When the command writes a message, don't overwrite the command.
    સંદેશ_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_લીટી_ગણતરી;
    જ્યારે (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    જો (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    નહીંતર
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    changed_લીટી_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    જો (સંદેશ_col == 0 && સંદેશ_scrolled == 0)
	સંદેશ_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    જો (!do_sub_સંદેશ(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_લીટી_ગણતરી - old_lcount);
}

#ifdef FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(શૂન્ય)
{
    પાછા old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    શૂન્ય
set_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#endif // FEAT_VIMINFO

#જો defined(EXITFREE)
    શૂન્ય
free_old_sub(શૂન્ય)
{
    vim_free(old_sub);
}
#endif

#જો defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    પૂર્ણાંક
prepare_tagpreview(
    પૂર્ણાંક		undo_sync,	    // sync undo when leaving the window
    પૂર્ણાંક		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    જો (curwin->w_p_pvw)
	પાછા FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    જો (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_find_preview_window();
	જો (wp != ખાલી)
	    popup_set_wantpos_cursor(wp, wp->w_minwidth, ખાલી);
    }
    નહીંતર જો (use_popup != USEPOPUP_NONE)
    {
	wp = popup_find_info_window();
	જો (wp != ખાલી)
	{
	    જો (use_popup == USEPOPUP_NORMAL)
		popup_show(wp);
	    નહીંતર
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    નહીંતર
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    જો (wp->w_p_pvw)
		તોડો;
    }
    જો (wp != ખાલી)
    {
	win_enter(wp, undo_sync);
	પાછા FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    જો ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	પાછા popup_create_preview_window(use_popup != USEPOPUP_NONE);
# endif
    જો (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	પાછા FALSE;
    curwin->w_p_pvw = TRUE;
    curwin->w_p_wfh = TRUE;
    RESET_BINDING(curwin);	    // don't take over 'scrollbind'
				    // and 'cursorbind'
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;	    // no 'diff'
# endif
# ifdef FEAT_FOLDING
    curwin->w_p_fdc = 0;	    // no 'foldcolumn'
# endif
    પાછા TRUE;
}

#endif

/*
 * Make the user happy.
 */
    શૂન્ય
ex_smile(exarg_T *eap UNUSED)
{
    સ્થિર અક્ષર *code[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    અક્ષર *p;
    પૂર્ણાંક n;
    પૂર્ણાંક i;

    સંદેશ_start();
    સંદેશ_putchar('\n');
    માટે (i = 0; i < 2; ++i)
	માટે (p = code[i]; *p != NUL; ++p)
	    જો (*p == 'x')
		સંદેશ_putchar('\n');
	    નહીંતર
		માટે (n = *p++; n > 0; --n)
		    જો (*p == 'o' || *p == '$')
			સંદેશ_putchar_attr(*p, HL_ATTR(HLF_L));
		    નહીંતર
			સંદેશ_putchar(*p);
    સંદેશ_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    શૂન્ય
ex_drop(exarg_T *eap)
{
    પૂર્ણાંક		split = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    જો (ERROR_IF_POPUP_WINDOW || ERROR_IF_TERM_POPUP_WINDOW)
	પાછા;

    /*
     * Check if the first argument is already being edited in a window.  If
     * so, jump to that window.
     * We would actually need to check all arguments, but that's complicated
     * and mostly only one file is dropped.
     * This also ignores wildcards, since it is very unlikely the user is
     * editing a file name with a wildcard character.
     */
    set_arglist(eap->arg);

    /*
     * Expanding wildcards may result in an empty argument list.  E.g. when
     * editing "foo.pyc" and ".pyc" is in 'wildignore'.  Assume that we
     * already did an error message for this.
     */
    જો (ARGCOUNT == 0)
	પાછા;

    જો (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	પાછા;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	જો (wp->w_buffer == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    જો (!bufIsChanged(curbuf))
	    {
		પૂર્ણાંક save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
		ex_rewind(eap);
	    પાછા;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    જો (!buf_hide(curbuf))
    {
	++emsg_off;
	split = check_changed(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    જો (split)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    નહીંતર
	eap->cmdidx = CMD_first;
    ex_rewind(eap);
}

/*
 * Skip over the pattern argument of ":vimgrep /pat/[g][j]".
 * Put the start of the pattern in "*s", unless "s" is NULL.
 * If "flags" is not NULL put the flags in it: VGR_GLOBAL, VGR_NOJUMP.
 * If "s" is not NULL terminate the pattern with a NUL.
 * Return a pointer to the char just past the pattern plus flags.
 */
    char_u *
skip_vimgrep_pat(char_u *p, char_u **s, પૂર્ણાંક *flags)
{
    પાછા skip_vimgrep_pat_ext(p, s, flags, ખાલી, ખાલી);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, પૂર્ણાંક *flags, char_u **nulp, પૂર્ણાંક *cp)
{
    પૂર્ણાંક		c;

    જો (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	જો (s != ખાલી)
	    *s = p;
	p = skiptowhite(p);
	જો (s != ખાલી && *p != NUL)
	{
	    જો (nulp != ખાલી)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    નહીંતર
    {
	// ":vimgrep /pattern/[g][j] fname"
	જો (s != ખાલી)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	જો (*p != c)
	    પાછા ખાલી;

	// Truncate the pattern.
	જો (s != ખાલી)
	{
	    જો (nulp != ખાલી)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	જ્યારે (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    જો (flags != ખાલી)
	    {
		જો (*p == 'g')
		    *flags |= VGR_GLOBAL;
		નહીંતર જો (*p == 'j')
		    *flags |= VGR_NOJUMP;
		નહીંતર
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    પાછા p;
}

#જો defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    શૂન્ય
ex_oldfiles(exarg_T *eap UNUSED)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    પૂર્ણાંક		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    પૂર્ણાંક		matches = -1;

    જો (l == ખાલી)
    {
	સંદેશ(_("No old files"));
	પાછા;
    }

    સંદેશ_start();
    સંદેશ_scroll = TRUE;
    માટે (li = l->lv_first; li != ખાલી && !got_int; li = li->li_next)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	જો (!message_filtered(fname))
	{
	    જો (matches < 0)
		matches = nr;
	    નહીંતર
		matches = 0;
	    સંદેશ_outnum((long)nr);
	    સંદેશ_puts(": ");
	    સંદેશ_outtrans(fname);
	    સંદેશ_clr_eos();
	    સંદેશ_putchar('\n');
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    જો (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_more = FALSE;
	// we only need to prompt if there is more than 1 match
	જો (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	નહીંતર
	    nr = prompt_for_સંખ્યા(FALSE);
	સંદેશ_starthere();
	જો (nr > 0)
	{
	    char_u *p = list_find_str(get_vim_var_list(VV_OLDFILES),
		    (long)nr);

	    જો (p != ખાલી)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, ખાલી);
		vim_free(p);
	    }
	}
    }
# endif
}
#endif
