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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#அந்தர்பாவ <float.h>

நிலையான முழு linelen(முழு *has_tab);
நிலையான வெறுமை do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, முழு do_in, முழு do_out);
நிலையான முழு not_writing(வெறுமை);
நிலையான முழு check_readonly(முழு *forceit, buf_T *buf);
நிலையான வெறுமை delbuf_செய்தி(char_u *name);

/*
 * ":ascii" and "ga".
 */
    வெறுமை
do_ascii(exarg_T *eap UNUSED)
{
    முழு		c;
    முழு		cval;
    எழுத்து	buf1[20];
    எழுத்து	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    முழு		cc[MAX_MCO];
    முழு		ci = 0;
    முழு		len;

    எனில் (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    இல்லையெனில்
	c = gchar_cursor();
    எனில் (c == NUL)
    {
	செய்தி("NUL");
	திரும்பு;
    }

    IObuff[0] = NUL;
    எனில் (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	எனில் (c == NL)	    // NUL is stored as NL
	    c = NUL;
	எனில் (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	இல்லையெனில்
	    cval = c;
	எனில் (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, அளவு(buf1), "  <%s>", (எழுத்து *)buf3);
	}
	இல்லையெனில்
	    buf1[0] = NUL;
	எனில் (c >= 0x80)
	    vim_snprintf(buf2, அளவு(buf2), "  <M-%s>",
						 (எழுத்து *)transchar(c & 0x7f));
	இல்லையெனில்
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	எனில் (dig != வெற்று)
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	இல்லையெனில்
#endif
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	எனில் (enc_utf8)
	    c = cc[ci++];
	இல்லையெனில்
	    c = 0;
    }

    // Repeat for combining characters.
    போது (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (முழு)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	எனில் (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	எனில் (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	எனில் (dig != வெற்று)
	    vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	இல்லையெனில்
#endif
	    vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	எனில் (ci == MAX_MCO)
	    நிறுத்து;
	எனில் (enc_utf8)
	    c = cc[ci++];
	இல்லையெனில்
	    c = 0;
    }

    செய்தி((எழுத்து *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    வெறுமை
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    முழு		len;
    முழு		indent = 0;
    முழு		new_indent;
    முழு		has_tab;
    முழு		width;

#ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl)
    {
	// switch left and right aligning
	எனில் (eap->cmdidx == CMD_வலது)
	    eap->cmdidx = CMD_இடது;
	இல்லையெனில் எனில் (eap->cmdidx == CMD_இடது)
	    eap->cmdidx = CMD_வலது;
    }
#endif

    width = atoi((எழுத்து *)eap->arg);
    save_curpos = curwin->w_cursor;
    எனில் (eap->cmdidx == CMD_இடது)    // width is used for new indent
    {
	எனில் (width >= 0)
	    indent = width;
    }
    இல்லையெனில்
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	எனில் (width <= 0)
	    width = curbuf->b_p_tw;
	எனில் (width == 0 && curbuf->b_p_wm > 0)
	    width = curwin->w_width - curbuf->b_p_wm;
	எனில் (width <= 0)
	    width = 80;
    }

    எனில் (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	திரும்பு;

    க்கு (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	எனில் (eap->cmdidx == CMD_இடது)		// left align
	    new_indent = indent;
	இல்லையெனில்
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_வலது ? &has_tab
						   : வெற்று) - get_indent();

	    எனில் (len <= 0)			// skip blank lines
		தொடர்;

	    எனில் (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    இல்லையெனில்
	    {
		new_indent = width - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		எனில் (has_tab)
		    போது (new_indent > 0)
		    {
			(வெறுமை)set_indent(new_indent, 0);
			எனில் (linelen(வெற்று) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    செய்
				(வெறுமை)set_indent(++new_indent, 0);
			    போது (linelen(வெற்று) <= width);
			    --new_indent;
			    நிறுத்து;
			}
			--new_indent;
		    }
	    }
	}
	எனில் (new_indent < 0)
	    new_indent = 0;
	(வெறுமை)set_indent(new_indent, 0);		// set indent
    }
    changed_வரிகள்(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    நிலையான முழு
linelen(முழு *has_tab)
{
    char_u  *வரி;
    char_u  *first;
    char_u  *last;
    முழு	    save;
    முழு	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    வரி = ml_get_curline();
    எனில் (*வரி == NUL)
	திரும்பு 0;

    // find the first non-blank character
    first = skipwhite(வரி);

    // find the character after the last non-blank character
    க்கு (last = first + STRLEN(first);
				last > first && VIM_ISWHITE(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize_str(வரி);	// get line length on screen
    எனில் (has_tab != வெற்று)		// check for embedded TAB
	*has_tab = (vim_strchr(first, TAB) != வெற்று);
    *last = save;

    திரும்பு len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
நிலையான char_u	*sortbuf1;
நிலையான char_u	*sortbuf2;

நிலையான முழு	sort_lc;	// sort using locale
நிலையான முழு	sort_ic;	// ignore case
நிலையான முழு	sort_nr;	// sort on number
நிலையான முழு	sort_rx;	// sort on regex instead of skipping it
நிலையான முழு	sort_flt;	// sort on floating number

நிலையான முழு	sort_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
வகைவரை கட்டமைப்பு
{
    linenr_T	lnum;			// line number
    ஒன்றிப்பு {
	கட்டமைப்பு
	{
	    varnumber_T	start_col_nr;	// starting column number
	    varnumber_T	end_col_nr;	// ending column number
	} வரி;
	கட்டமைப்பு
	{
	    varnumber_T	மதிப்பு;		// value if sorting by integer
	    முழு is_எண்;		// TRUE when line contains a number
	} இலக்கம்;
	float_T மதிப்பு_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    நிலையான முழு
string_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    எனில் (sort_lc)
	திரும்பு strcoll((எழுத்து *)s1, (எழுத்து *)s2);
    திரும்பு sort_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    நிலையான முழு
sort_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    முழு		result = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    எனில் (sort_abort)
	திரும்பு 0;
    fast_breakcheck();
    எனில் (got_int)
	sort_abort = TRUE;

    எனில் (sort_nr)
    {
	எனில் (l1.st_u.இலக்கம்.is_எண் != l2.st_u.இலக்கம்.is_எண்)
	    result = l1.st_u.இலக்கம்.is_எண் > l2.st_u.இலக்கம்.is_எண் ? 1 : -1;
	இல்லையெனில்
	    result = l1.st_u.இலக்கம்.மதிப்பு == l2.st_u.இலக்கம்.மதிப்பு ? 0
			     : l1.st_u.இலக்கம்.மதிப்பு > l2.st_u.இலக்கம்.மதிப்பு ? 1 : -1;
    }
    இல்லையெனில் எனில் (sort_flt)
	result = l1.st_u.மதிப்பு_flt == l2.st_u.மதிப்பு_flt ? 0
			     : l1.st_u.மதிப்பு_flt > l2.st_u.மதிப்பு_flt ? 1 : -1;
    இல்லையெனில்
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.வரி.start_col_nr,
		     l1.st_u.வரி.end_col_nr - l1.st_u.வரி.start_col_nr + 1);
	sortbuf1[l1.st_u.வரி.end_col_nr - l1.st_u.வரி.start_col_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.வரி.start_col_nr,
		     l2.st_u.வரி.end_col_nr - l2.st_u.வரி.start_col_nr + 1);
	sortbuf2[l2.st_u.வரி.end_col_nr - l2.st_u.வரி.start_col_nr] = 0;

	result = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    எனில் (result == 0)
	திரும்பு (முழு)(l1.lnum - l2.lnum);
    திரும்பு result;
}

/*
 * ":sort".
 */
    வெறுமை
ex_sort(exarg_T *eap)
{
    regmatch_T	regmatch;
    முழு		len;
    linenr_T	lnum;
    long	maxlen = 0;
    sorti_T	*nrs;
    size_t	மொத்தம் = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    முழு		unique = FALSE;
    long	deleted;
    colnr_T	start_col;
    colnr_T	end_col;
    முழு		sort_what = 0;
    முழு		format_found = 0;
    முழு		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    எனில் (மொத்தம் <= 1)
	திரும்பு;

    எனில் (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	திரும்பு;
    sortbuf1 = வெற்று;
    sortbuf2 = வெற்று;
    regmatch.regprog = வெற்று;
    nrs = ALLOC_MULT(sorti_T, மொத்தம்);
    எனில் (nrs == வெற்று)
	செல் sortend;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    க்கு (p = eap->arg; *p != NUL; ++p)
    {
	எனில் (VIM_ISWHITE(*p))
	    ;
	இல்லையெனில் எனில் (*p == 'i')
	    sort_ic = TRUE;
	இல்லையெனில் எனில் (*p == 'l')
	    sort_lc = TRUE;
	இல்லையெனில் எனில் (*p == 'r')
	    sort_rx = TRUE;
	இல்லையெனில் எனில் (*p == 'n')
	{
	    sort_nr = 1;
	    ++format_found;
	}
	இல்லையெனில் எனில் (*p == 'f')
	{
	    sort_flt = 1;
	    ++format_found;
	}
	இல்லையெனில் எனில் (*p == 'b')
	{
	    sort_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_found;
	}
	இல்லையெனில் எனில் (*p == 'o')
	{
	    sort_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_found;
	}
	இல்லையெனில் எனில் (*p == 'x')
	{
	    sort_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_found;
	}
	இல்லையெனில் எனில் (*p == 'u')
	    unique = TRUE;
	இல்லையெனில் எனில் (*p == '"')	// comment start
	    நிறுத்து;
	இல்லையெனில் எனில் (eap->nextcmd == வெற்று && check_nextcmd(p) != வெற்று)
	{
	    eap->nextcmd = check_nextcmd(p);
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (!ASCII_ISALPHA(*p) && regmatch.regprog == வெற்று)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    எனில் (s == வெற்று)
		செல் sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    எனில் (s == p + 1)
	    {
		எனில் (last_தேடல்_pat() == வெற்று)
		{
		    emsg(_(e_no_previous_regular_expression));
		    செல் sortend;
		}
		regmatch.regprog = vim_regcomp(last_தேடல்_pat(), RE_MAGIC);
	    }
	    இல்லையெனில்
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    எனில் (regmatch.regprog == வெற்று)
		செல் sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	இல்லையெனில்
	{
	    semsg(_(e_invalid_argument_str), p);
	    செல் sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    எனில் (format_found > 1)
    {
	emsg(_(e_invalid_argument));
	செல் sortend;
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
    க்கு (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	எனில் (maxlen < len)
	    maxlen = len;

	start_col = 0;
	end_col = len;
	எனில் (regmatch.regprog != வெற்று && vim_regexec(&regmatch, s, 0))
	{
	    எனில் (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    இல்லையெனில்
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	இல்லையெனில்
	    எனில் (regmatch.regprog != வெற்று)
		end_col = 0;

	எனில் (sort_nr || sort_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + end_col;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + start_col;
	    எனில் (sort_nr)
	    {
		எனில் (sort_what & STR2NR_HEX)
		    s = skiptohex(p);
		இல்லையெனில் எனில் (sort_what & STR2NR_BIN)
		    s = skiptobin(p);
		இல்லையெனில்
		    s = skiptodigit(p);
		எனில் (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		எனில் (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.இலக்கம்.is_எண் = FALSE;
		    nrs[lnum - eap->line1].st_u.இலக்கம்.மதிப்பு = 0;
		}
		இல்லையெனில்
		{
		    nrs[lnum - eap->line1].st_u.இலக்கம்.is_எண் = TRUE;
		    vim_str2nr(s, வெற்று, வெற்று, sort_what,
			&nrs[lnum - eap->line1].st_u.இலக்கம்.மதிப்பு,
			வெற்று, 0, FALSE, வெற்று);
		}
	    }
	    இல்லையெனில்
	    {
		s = skipwhite(p);
		எனில் (*s == '+')
		    s = skipwhite(s + 1);

		எனில் (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.மதிப்பு_flt = -DBL_MAX;
		இல்லையெனில்
		    nrs[lnum - eap->line1].st_u.மதிப்பு_flt =
						      strtod((எழுத்து *)s, வெற்று);
	    }
	    *s2 = c;
	}
	இல்லையெனில்
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.வரி.start_col_nr = start_col;
	    nrs[lnum - eap->line1].st_u.வரி.end_col_nr = end_col;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	எனில் (regmatch.regprog != வெற்று)
	    fast_breakcheck();
	எனில் (got_int)
	    செல் sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    எனில் (sortbuf1 == வெற்று)
	செல் sortend;
    sortbuf2 = alloc(maxlen + 1);
    எனில் (sortbuf2 == வெற்று)
	செல் sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((வெறுமை *)nrs, மொத்தம், அளவு(sorti_T), sort_compare);

    எனில் (sort_abort)
	செல் sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? மொத்தம் - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	எனில் (get_lnum + ((linenr_T)மொத்தம் - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	எனில் (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    எனில் (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		நிறுத்து;
	}
	fast_breakcheck();
	எனில் (got_int)
	    செல் sortend;
    }

    // delete the original lines if appending worked
    எனில் (i == மொத்தம்)
	க்கு (i = 0; i < மொத்தம்; ++i)
	    ml_delete(eap->line1);
    இல்லையெனில்
	மொத்தம் = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (long)(மொத்தம் - (lnum - eap->line2));
    எனில் (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    இல்லையெனில் எனில் (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    எனில் (change_occurred || deleted != 0)
	changed_வரிகள்(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    எனில் (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    வெறுமை
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    முழு		len;
    linenr_T	lnum;
    long	maxlen = 0;
    linenr_T	மொத்தம் = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    முழு		keep_only_unique = FALSE;
    முழு		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    long	deleted = 0;
    colnr_T	start_col;
    colnr_T	end_col;
    முழு		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    எனில் (மொத்தம் <= 1)
	திரும்பு;

    எனில் (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	திரும்பு;
    sortbuf1 = வெற்று;
    regmatch.regprog = வெற்று;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    க்கு (p = eap->arg; *p != NUL; ++p)
    {
	எனில் (VIM_ISWHITE(*p))
	    ;
	இல்லையெனில் எனில் (*p == 'i')
	    sort_ic = TRUE;
	இல்லையெனில் எனில் (*p == 'l')
	    sort_lc = TRUE;
	இல்லையெனில் எனில் (*p == 'r')
	    sort_rx = TRUE;
	இல்லையெனில் எனில் (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    எனில் (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	இல்லையெனில் எனில் (*p == '"')	// comment start
	    நிறுத்து;
	இல்லையெனில் எனில் (eap->nextcmd == வெற்று && check_nextcmd(p) != வெற்று)
	{
	    eap->nextcmd = check_nextcmd(p);
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (!ASCII_ISALPHA(*p) && regmatch.regprog == வெற்று)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    எனில் (s == வெற்று)
		செல் uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    எனில் (s == p + 1)
	    {
		எனில் (last_தேடல்_pat() == வெற்று)
		{
		    emsg(_(e_no_previous_regular_expression));
		    செல் uniqend;
		}
		regmatch.regprog = vim_regcomp(last_தேடல்_pat(), RE_MAGIC);
	    }
	    இல்லையெனில்
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    எனில் (regmatch.regprog == வெற்று)
		செல் uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	இல்லையெனில்
	{
	    semsg(_(e_invalid_argument_str), p);
	    செல் uniqend;
	}
    }

    // Find the length of the longest line.
    க்கு (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	எனில் (maxlen < len)
	    maxlen = len;

	எனில் (got_int)
	    செல் uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    எனில் (sortbuf1 == வெற்று)
	செல் uniqend;

    // Delete lines according to options.
    முழு match_continue = FALSE;
    முழு next_is_unmatch = FALSE;
    முழு is_match;
    linenr_T done_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    க்கு (linenr_T i = 0; i < மொத்தம்; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	start_col = 0;
	end_col = len;
	எனில் (regmatch.regprog != வெற்று && vim_regexec(&regmatch, s, 0))
	{
	    எனில் (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    இல்லையெனில்
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	இல்லையெனில்
	    எனில் (regmatch.regprog != வெற்று)
		end_col = 0;
	எனில் (end_col > 0)
	{
	    save_c = s[end_col];
	    s[end_col] = NUL;
	}

	is_match = i > 0 ? !string_compare(&s[start_col], sortbuf1) : FALSE;
	delete_lnum = 0;
	எனில் (next_is_unmatch)
	{
	    is_match = FALSE;
	    next_is_unmatch = FALSE;
	}

	எனில் (!keep_only_unique && !keep_only_not_unique)
	{
	    எனில் (is_match)
		delete_lnum = get_lnum;
	    இல்லையெனில்
		STRCPY(sortbuf1, &s[start_col]);
	}
	இல்லையெனில் எனில் (keep_only_not_unique)
	{
	    எனில் (is_match)
	    {
		done_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    இல்லையெனில்
	    {
		எனில் (i > 0 && !match_continue && get_lnum - 1 > done_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    next_is_unmatch = TRUE;
		}
		இல்லையெனில் எனில் (i >= மொத்தம் - 1)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}
	இல்லையெனில் // keep_only_unique
	{
	    எனில் (is_match)
	    {
		எனில் (!match_continue)
		    delete_lnum = get_lnum - 1;
		இல்லையெனில்
		    delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    இல்லையெனில்
	    {
		எனில் (i == 0 && match_continue)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}

	எனில் (end_col > 0)
	    s[end_col] = save_c;

	எனில் (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    மொத்தம்--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	எனில் (got_int)
	    செல் uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
    msgmore(-deleted);

    எனில் (change_occurred)
	changed_வரிகள்(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    எனில் (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	இலக்கம்_வரிகள்;  // Num lines moved
    linenr_T	last_வரி;  // Last line in file after adding new text
#ifdef FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#endif

    எனில் (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_of_வரிகள்_into_itself));
	திரும்பு FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    எனில் (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	எனில் (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	இல்லையெனில்
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	திரும்பு OK;
    }

    இலக்கம்_வரிகள் = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    எனில் (u_save(dest, dest + 1) == FAIL)
	திரும்பு FAIL;
    க்கு (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	எனில் (str != வெற்று)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    எனில் (dest < line1)
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
    last_வரி = curbuf->b_ml.ml_வரி_மொத்தம்;
    mark_adjust_nofold(line1, line2, last_வரி - line2, 0L);
    எனில் (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -இலக்கம்_வரிகள், 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    எனில் (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#endif
	எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest - இலக்கம்_வரிகள் + 1;
	    curbuf->b_op_end.lnum = dest;
	}
    }
    இல்லையெனில்
    {
	mark_adjust_nofold(dest + 1, line1 - 1, இலக்கம்_வரிகள், 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    எனில் (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#endif
	எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest + 1;
	    curbuf->b_op_end.lnum = dest + இலக்கம்_வரிகள்;
	}
    }
    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust_nofold(last_வரி - இலக்கம்_வரிகள் + 1, last_வரி,
					     -(last_வரி - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    எனில் (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	திரும்பு FAIL;

    க்கு (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_MESSAGE);

    எனில் (!global_busy && இலக்கம்_வரிகள் > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", இலக்கம்_வரிகள்),
			(long)இலக்கம்_வரிகள்);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    எனில் (dest >= line1)
	curwin->w_cursor.lnum = dest;
    இல்லையெனில்
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    எனில் (line1 < dest)
    {
	dest += இலக்கம்_வரிகள் + 1;
	last_வரி = curbuf->b_ml.ml_வரி_மொத்தம்;
	எனில் (dest > last_வரி + 1)
	    dest = last_வரி + 1;
	changed_வரிகள்(line1, 0, dest, 0L);
    }
    இல்லையெனில்
	changed_வரிகள்(dest + 1, 0, line1 + இலக்கம்_வரிகள், 0L);

    திரும்பு OK;
}

/*
 * ":copy"
 */
    வெறுமை
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	மொத்தம்;
    char_u	*p;

    மொத்தம் = line2 - line1 + 1;
    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = n + 1;
	curbuf->b_op_end.lnum = n + மொத்தம்;
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
    எனில் (u_save(n, n + 1) == FAIL)
	திரும்பு;

    curwin->w_cursor.lnum = n;
    போது (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	எனில் (p != வெற்று)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	எனில் (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	எனில் (curwin->w_cursor.lnum < line1)
	    ++line1;
	எனில் (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_வரிகள்_mark(n, மொத்தம்);
    எனில் (VIsual_active)
	check_pos(curbuf, &VIsual);

    msgmore((long)மொத்தம்);
}

நிலையான char_u	*prevcmd = வெற்று;	// the previous command

#எனில் defined(EXITFREE)
    வெறுமை
free_prev_shellcmd(வெறுமை)
{
    vim_free(prevcmd);
}
#endif

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    நிலையான முழு
prevcmd_is_set(வெறுமை)
{
    எனில் (prevcmd == வெற்று)
    {
	emsg(_(e_no_previous_command));
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    வெறுமை
do_bang(
    முழு		addr_மொத்தம்,
    exarg_T	*eap,
    முழு		forceit,
    முழு		do_in,
    முழு		do_out)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = வெற்று;		// the new command
    முழு			free_newcmd = FALSE;    // need to free() newcmd
    முழு			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    முழு			len;
    முழு			scroll_save = செய்தி_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (addr_மொத்தம் == 0)		// :!
    {
	செய்தி_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	செய்தி_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    செய்
    {
	len = (முழு)STRLEN(trailarg) + 1;
	எனில் (newcmd != வெற்று)
	    len += (முழு)STRLEN(newcmd);
	எனில் (ins_prevcmd)
	{
	    எனில் (!prevcmd_is_set())
	    {
		vim_free(newcmd);
		திரும்பு;
	    }
	    len += (முழு)STRLEN(prevcmd);
	}
	எனில் ((t = alloc(len)) == வெற்று)
	{
	    vim_free(newcmd);
	    திரும்பு;
	}
	*t = NUL;
	எனில் (newcmd != வெற்று)
	    STRCAT(t, newcmd);
	எனில் (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = வெற்று;
	போது (*p)
	{
	    எனில் (*p == '!')
	    {
		எனில் (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		இல்லையெனில்
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    நிறுத்து;
		}
	    }
	    ++p;
	}
    } போது (trailarg != வெற்று);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    எனில் (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    இல்லையெனில்
	free_newcmd = TRUE;

    எனில் (bangredo)	    // put cmd in redo buffer for ! command
    {
	எனில் (!prevcmd_is_set())
	    செல் theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	எனில் (cmd != வெற்று)
	{
	    AppendToRedobuffLit(cmd, -1);
	    vim_free(cmd);
	}
	இல்லையெனில்
	    AppendToRedobuffLit(prevcmd, -1);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    எனில் (*p_shq != NUL)
    {
	எனில் (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	எனில் (newcmd == வெற்று)
	    திரும்பு;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    எனில் (addr_மொத்தம் == 0)		// :!
    {
	// echo the command
	செய்தி_start();
	செய்தி_putchar(':');
	செய்தி_putchar('!');
	செய்தி_outtrans(newcmd);
	செய்தி_clr_eos();
	windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);

	do_shell(newcmd, 0);
    }
    இல்லையெனில்				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
	apply_autocmds(EVENT_SHELLFILTERPOST, வெற்று, வெற்று, FALSE, curbuf);
    }

theend:
    எனில் (free_newcmd)
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
    நிலையான வெறுமை
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    முழு		do_in,
    முழு		do_out)
{
    char_u	*itmp = வெற்று;
    char_u	*otmp = வெற்று;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    முழு		shell_flags = 0;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    முழு		save_cmod_flags = cmdmod.cmod_flags;
#ifdef FEAT_FILTERPIPE
    முழு		stmp = p_stmp;
#endif

    எனில் (*cmd == NUL)	    // no filter command
	திரும்பு;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_வரி_abv_curs();
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

    எனில் (do_out)
	shell_flags |= SHELL_DOOUT;

#ifdef FEAT_FILTERPIPE
# ifdef VIMDLL
    எனில் (!gui.in_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    எனில் (!do_in && do_out && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    இல்லையெனில் எனில் (do_in && !do_out && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
    }
    இல்லையெனில் எனில் (do_in && do_out && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    இல்லையெனில்
#endif
	எனில் ((do_in && (itmp = vim_tempname('i', FALSE)) == வெற்று)
		|| (do_out && (otmp = vim_tempname('o', FALSE)) == வெற்று))
	{
	    emsg(_(e_cant_get_temp_file_name));
	    செல் filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    எனில் (itmp != வெற்று && buf_write(curbuf, itmp, வெற்று, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	செய்தி_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#எனில் defined(FEAT_EVAL)
	எனில் (!aborting())
#endif
	    // will call wait_return()
	    (வெறுமை)semsg(_(e_cant_create_file_str), itmp);
	செல் filterend;
    }
    எனில் (curbuf != old_curbuf)
	செல் filterend;

    எனில் (!do_out)
	செய்தி_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    எனில் (cmd_buf == வெற்று)
	செல் filterend;

    windgoto((முழு)Rows - 1, cmdline_col_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    எனில் (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	redraw_later_clear();

    எனில் (do_out)
    {
	எனில் (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    செல் error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_வரி_மொத்தம்;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    எனில் (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    எனில் (do_out)
    {
	எனில் (otmp != வெற்று)
	{
	    எனில் (readfile(otmp, வெற்று, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#எனில் defined(FEAT_EVAL)
		எனில் (!aborting())
#endif
		{
		    செய்தி_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		செல் error;
	    }
	    எனில் (curbuf != old_curbuf)
		செல் filterend;
	}

	read_linecount = curbuf->b_ml.ml_வரி_மொத்தம் - read_linecount;

	எனில் (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_start.lnum = line2 + 1;
	    curbuf->b_op_end.lnum = curwin->w_cursor.lnum;
	    appended_வரிகள்_mark(line2, read_linecount);
	}

	எனில் (do_in)
	{
	    எனில் ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == வெற்று)
	    {
		எனில் (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		இல்லையெனில் எனில் (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		இல்லையெனில்
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
	    del_வரிகள்(linecount, TRUE);
	    எனில் (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_start.lnum = curbuf->b_op_end.lnum =
					MIN(line1, curbuf->b_ml.ml_வரி_மொத்தம்);
		curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	    }
	    இல்லையெனில்
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
	இல்லையெனில்
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	எனில் (linecount > p_report)
	{
	    எனில் (do_in)
	    {
		vim_snprintf(செய்தி_buf, அளவு(செய்தி_buf),
				    _("%ld lines filtered"), (long)linecount);
		எனில் (செய்தி(செய்தி_buf) && !செய்தி_scroll)
		    // save message to display it after redraw
		    set_keep_செய்தி((char_u *)செய்தி_buf, 0);
	    }
	    இல்லையெனில்
		msgmore((long)linecount);
	}
    }
    இல்லையெனில்
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    எனில் (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_buffer));
    }
    இல்லையெனில் எனில் (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

    எனில் (itmp != வெற்று)
	mch_remove(itmp);
    எனில் (otmp != வெற்று)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    வெறுமை
do_shell(
    char_u	*cmd,
    முழு		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    முழு		save_nwr;
#endif
#ifdef MSWIN
    முழு		winstart = FALSE;
#endif
    முழு		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    எனில் (check_restricted() || check_secure())
    {
	செய்தி_end();
	திரும்பு;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    எனில் (cmd != வெற்று)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# எனில் defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    எனில் (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != வெற்று)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    செய்தி_putchar('\r');			// put cursor at start of line
    எனில் (!autocmd_busy)
    {
	எனில் (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    எனில் (!winstart)
#endif
	செய்தி_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    எனில் (p_warn && !autocmd_busy && செய்தி_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    எனில் (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		எனில் (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		செய்தி_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		எனில் (!keep_termcap)
		    stoptermcap();
#endif
		நிறுத்து;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    எனில் (!swapping_screen())
	windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
    cursor_on();
    (வெறுமை)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    எனில் (!swapping_screen())
    {
	செய்தி_row = Rows - 1;
	செய்தி_col = 0;
    }

    எனில் (autocmd_busy)
    {
	எனில் (செய்தி_silent == 0)
	    redraw_later_clear();
    }
    இல்லையெனில்
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	எனில் (!gui.in_use)
# endif
	{
	    எனில் (cmd == வெற்று
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		எனில் (செய்தி_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    இல்லையெனில்
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		எனில் (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		முழு redraw = செய்தி_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		எனில் (gui.in_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(term_console ? -1 : redraw); // see below
# இல்லையெனில்
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	எனில் (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	எனில் (skip_redraw)		// ':' hit in wait_return()
	{
	    எனில் (செய்தி_silent == 0)
		redraw_later_clear();
	}
	இல்லையெனில் எனில் (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    எனில் (got_int && செய்தி_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    இல்லையெனில்
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, வெற்று, வெற்று, FALSE, curbuf);
}

#எனில் !defined(UNIX)
    நிலையான char_u *
find_pipe(char_u *cmd)
{
    char_u  *p;
    முழு	    inquote = FALSE;

    க்கு (p = cmd; *p != NUL; ++p)
    {
	எனில் (!inquote && *p == '|')
	    திரும்பு p;
	எனில் (*p == '"')
	    inquote = !inquote;
	இல்லையெனில் எனில் (rem_backslash(p))
	    ++p;
    }
    திரும்பு வெற்று;
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
    முழு		is_powershell = FALSE;
#ifdef UNIX
    முழு		is_fish_shell;
#endif

    char_u *shell_name = get_isolated_shell_name();
    எனில் (shell_name == வெற்று)
	திரும்பு வெற்று;

#எனில் defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    எனில் (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    இல்லையெனில்
#endif
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	எனில் (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	இல்லையெனில்
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    எனில் (itmp != வெற்று)
    {
	எனில் (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	இல்லையெனில்
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    எனில் (otmp != வெற்று)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;

    எனில் (is_powershell)
    {
	எனில் (itmp != வெற்று)
	    vim_snprintf((எழுத்து *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	இல்லையெனில்
	    vim_snprintf((எழுத்து *)buf, len, "& { %s }", cmd);
    }
    இல்லையெனில்
    {
#எனில் defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	எனில் (itmp != வெற்று || otmp != வெற்று)
	{
	    எனில் (is_fish_shell)
		vim_snprintf((எழுத்து *)buf, len, "begin; %s; end", (எழுத்து *)cmd);
	    இல்லையெனில்
		vim_snprintf((எழுத்து *)buf, len, "(%s)", (எழுத்து *)cmd);
	}
	இல்லையெனில்
	    STRCPY(buf, cmd);
	எனில் (itmp != வெற்று)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#இல்லையெனில்
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	எனில் (*p_sxe != NUL && *p_sxq == '(')
	{
	    எனில் (itmp != வெற்று || otmp != வெற்று)
		vim_snprintf((எழுத்து *)buf, len, "(%s)", (எழுத்து *)cmd);
	    இல்லையெனில்
		STRCPY(buf, cmd);
	    எனில் (itmp != வெற்று)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	இல்லையெனில்
	{
	    STRCPY(buf, cmd);
	    எனில் (itmp != வெற்று)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		எனில் (*p_shq == NUL)
		{
		    p = find_pipe(buf);
		    எனில் (p != வெற்று)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		எனில் (*p_shq == NUL)
		{
		    p = find_pipe(cmd);
		    எனில் (p != வெற்று)
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
    எனில் (otmp != வெற்று)
	append_redir(buf, (முழு)len, p_srr, otmp);

    திரும்பு buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    வெறுமை
append_redir(
    char_u	*buf,
    முழு		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*end;

    end = buf + STRLEN(buf);
    // find "%s"
    க்கு (p = opt; (p = vim_strchr(p, '%')) != வெற்று; ++p)
    {
	எனில் (p[1] == 's') // found %s
	    நிறுத்து;
	எனில் (p[1] == '%') // skip %%
	    ++p;
    }
    எனில் (p != வெற்று)
    {
#ifdef MSWIN
	*end++ = ' '; // not really needed? Not with sh, ksh or bash
#endif
	vim_snprintf((எழுத்து *)end, (size_t)(buflen - (end - buf)),
						  (எழுத்து *)opt, (எழுத்து *)fname);
    }
    இல்லையெனில்
	vim_snprintf((எழுத்து *)end, (size_t)(buflen - (end - buf)), " %s %s",
		(எழுத்து *)opt, (எழுத்து *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    வெறுமை
do_fixdel(exarg_T *eap UNUSED)
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != வெற்று
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    வெறுமை
print_வரி_no_prefix(
    linenr_T	lnum,
    முழு		use_எண்,
    முழு		list)
{
    எழுத்து	numbuf[30];

    எனில் (curwin->w_p_nu || use_எண்)
    {
	vim_snprintf(numbuf, அளவு(numbuf),
				   "%*ld ", எண்_width(curwin), (long)lnum);
	செய்தி_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    செய்தி_prt_வரி(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    வெறுமை
print_வரி(linenr_T lnum, முழு use_எண், முழு list)
{
    முழு		save_silent = silent_mode;

    // apply :filter /pat/
    எனில் (message_filtered(ml_get(lnum)))
	திரும்பு;

    செய்தி_start();
    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()
    print_வரி_no_prefix(lnum, use_எண், list);
    எனில் (save_silent)
    {
	செய்தி_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = save_silent;
    }
    info_message = FALSE;
}

    முழு
rename_buffer(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, வெற்று, வெற்று, FALSE, curbuf);
    // buffer changed, don't change name now
    எனில் (buf != curbuf)
	திரும்பு FAIL;
#ifdef FEAT_EVAL
    எனில் (aborting())	    // autocmds may abort script processing
	திரும்பு FAIL;
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
    curbuf->b_ffname = வெற்று;
    curbuf->b_sfname = வெற்று;
    எனில் (setfname(curbuf, new_fname, வெற்று, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	திரும்பு FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    எனில் (xfname != வெற்று && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	எனில் (buf != வெற்று && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, வெற்று, வெற்று, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    திரும்பு OK;
}

/*
 * ":file[!] [fname]".
 */
    வெறுமை
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    எனில் (eap->addr_மொத்தம் > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_மொத்தம் > 1))
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }

    எனில் (*eap->arg != NUL || eap->addr_மொத்தம் == 1)
    {
	எனில் (rename_buffer(eap->arg) == FAIL)
	    திரும்பு;
	redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    எனில் (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    வெறுமை
ex_update(exarg_T *eap)
{
    எனில் (curbufIsChanged())
	(வெறுமை)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    வெறுமை
ex_write(exarg_T *eap)
{
    எனில் (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
    }

    எனில் (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    இல்லையெனில்
	(வெறுமை)do_write(eap);
}

#ifdef UNIX
    நிலையான முழு
check_writable(char_u *fname)
{
    எனில் (mch_nodetype(fname) == முனை_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	திரும்பு FAIL;
    }
    திரும்பு OK;
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
    முழு
do_write(exarg_T *eap)
{
    முழு		other;
    char_u	*fname = வெற்று;		// init to shut up gcc
    char_u	*ffname;
    முழு		retval = FAIL;
    char_u	*free_fname = வெற்று;
#ifdef FEAT_BROWSE
    char_u	*browse_file = வெற்று;
#endif
    buf_T	*alt_buf = வெற்று;
    முழு		name_was_missing;

    எனில் (not_writing())		// check 'write' option
	திரும்பு FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    எனில் ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    வெற்று, வெற்று, வெற்று, curbuf);
	எனில் (browse_file == வெற்று)
	    செல் theend;
	ffname = browse_file;
    }
#endif
    எனில் (*ffname == NUL)
    {
	எனில் (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_argument_required));
	    செல் theend;
	}
	other = FALSE;
    }
    இல்லையெனில்
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	எனில் (free_fname != வெற்று)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    எனில் (other)
    {
	எனில் (vim_strchr(p_cpo, CPO_ALTWRITE) != வெற்று
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	இல்லையெனில்
	    alt_buf = buflist_findname(ffname);
	எனில் (alt_buf != வெற்று && alt_buf->b_ml.ml_mfp != வெற்று)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    செல் theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    எனில் (!other && (bt_dontwrite_செய்தி(curbuf) || check_fname() == FAIL
#ifdef UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#endif
		|| check_readonly(&eap->forceit, curbuf)))
	செல் theend;

    எனில் (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	எனில் (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_வரி_மொத்தம்)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    எனில் (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		எனில் (vim_dialog_yesno(VIM_QUESTION, வெற்று,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    செல் theend;
		eap->forceit = TRUE;
	    }
	    இல்லையெனில்
#endif
	    {
		emsg(_(e_use_bang_to_write_partial_buffer));
		செல் theend;
	    }
	}
    }

    எனில் (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	எனில் (eap->cmdidx == CMD_saveas && alt_buf != வெற்று)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, வெற்று, வெற்று, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, வெற்று, வெற்று, FALSE, alt_buf);
#ifdef FEAT_EVAL
	    எனில் (curbuf != was_curbuf || aborting())
#இல்லையெனில்
	    எனில் (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		செல் theend;
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

	    apply_autocmds(EVENT_BUFFILEPOST, வெற்று, வெற்று, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, வெற்று, வெற்று, FALSE, alt_buf);
	    எனில் (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, வெற்று, வெற்று, FALSE, alt_buf);
	    }
#ifdef FEAT_EVAL
	    எனில் (curbuf != was_curbuf || aborting())
#இல்லையெனில்
	    எனில் (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		செல் theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    எனில் (*curbuf->b_p_ft == NUL)
	    {
		எனில் (au_has_group((char_u *)"filetypedetect"))
		    (வெறுமை)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, வெற்று);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == வெற்று;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	எனில் (eap->cmdidx == CMD_saveas)
	{
	    எனில் (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	எனில் (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    திரும்பு retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    முழு
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    முழு		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    எனில் (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == வெற்று)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	எனில் (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    எனில் (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		திரும்பு FAIL;
	    }
#endif
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    எனில் (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_செய்தி_SIZE];

		dialog_செய்தி(buff, _("Overwrite existing file \"%s\"?"), fname);
		எனில் (vim_dialog_yesno(VIM_QUESTION, வெற்று, buff, 2) != VIM_YES)
		    திரும்பு FAIL;
		eap->forceit = TRUE;
	    }
	    இல்லையெனில்
#endif
	    {
		emsg(_(e_file_exists));
		திரும்பு FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	எனில் (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    முழு		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    எனில் (*p_dir == NUL)
	    {
		dir = alloc(5);
		எனில் (dir == வெற்று)
		    திரும்பு FAIL;
		STRCPY(dir, ".");
	    }
	    இல்லையெனில்
	    {
		dir = alloc(MAXPATHL);
		எனில் (dir == வெற்று)
		    திரும்பு FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    எனில் (r)
	    {
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		எனில் (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_செய்தி_SIZE];

		    dialog_செய்தி(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    எனில் (vim_dialog_yesno(VIM_QUESTION, வெற்று, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			திரும்பு FAIL;
		    }
		    eap->forceit = TRUE;
		}
		இல்லையெனில்
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    திரும்பு FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    திரும்பு OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    வெறுமை
ex_wnext(exarg_T *eap)
{
    முழு		i;

    எனில் (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (முழு)eap->line2;
    இல்லையெனில்
	i = curwin->w_arg_idx - (முழு)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
    எனில் (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    வெறுமை
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    முழு		error = 0;
    முழு		save_forceit = eap->forceit;
    முழு		save_exiting = exiting;

    எனில் (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	எனில் (before_quit_all(eap) == FAIL)
	    திரும்பு;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ifdef FEAT_TERMINAL
	எனில் (exiting && !eap->forceit && term_job_running(buf->b_term))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);
	    எனில் (term_try_stop_job(buf) == FAIL)
	    {
		no_write_message_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    இல்லையெனில் எனில் (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	இல்லையெனில்
#endif
	எனில் (bufIsChanged(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    எனில் (not_writing())
	    {
		++error;
		நிறுத்து;
	    }
#ifdef FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    எனில் (buf->b_ffname == வெற்று && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#endif
	    எனில் (buf->b_ffname == வெற்று)
	    {
		semsg(_(e_no_file_name_for_buffer_nr), (long)buf->b_fnum);
		++error;
	    }
	    இல்லையெனில் எனில் (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    இல்லையெனில்
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		எனில் (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		எனில் (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    எனில் (exiting)
    {
	எனில் (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    நிலையான முழு
not_writing(வெறுமை)
{
    எனில் (p_write)
	திரும்பு FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    திரும்பு TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    நிலையான முழு
check_readonly(முழு *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    எனில் (!*forceit && (buf->b_p_ro
		|| (mch_stat((எழுத்து *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	எனில் ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != வெற்று)
	{
	    char_u	buff[DIALOG_செய்தி_SIZE];

	    எனில் (buf->b_p_ro)
		dialog_செய்தி(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    இல்லையெனில்
		dialog_செய்தி(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    எனில் (vim_dialog_yesno(VIM_QUESTION, வெற்று, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		திரும்பு FALSE;
	    }
	    இல்லையெனில்
		திரும்பு TRUE;
	}
	இல்லையெனில்
#endif
	எனில் (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_set_add_bang_to_override));
	இல்லையெனில்
	    semsg(_(e_str_is_read_only_add_bang_to_override), buf->b_fname);
	திரும்பு TRUE;
    }

    திரும்பு FALSE;
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
    முழு
getfile(
    முழு		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    முழு		setpm,
    linenr_T	lnum,
    முழு		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    முழு		other;
    முழு		retval;
    char_u	*free_me = வெற்று;

    எனில் (!check_can_set_curbuf_forceit(forceit))
	திரும்பு GETFILE_ERROR;

    எனில் (text_locked())
	திரும்பு GETFILE_ERROR;
    எனில் (curbuf_locked())
	திரும்பு GETFILE_ERROR;

    எனில் (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    இல்லையெனில்
	other = (fnum != curbuf->b_fnum);

    எனில் (other)
	++no_wait_return;	    // don't wait for autowrite message
    எனில் (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	எனில் (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	எனில் (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    செல் theend;
	}
    }
    எனில் (other)
	--no_wait_return;
    எனில் (setpm)
	setpcmark();
    எனில் (!other)
    {
	எனில் (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    இல்லையெனில் எனில் (do_ecmd(fnum, ffname, sfname, வெற்று, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    இல்லையெனில்
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    திரும்பு retval;
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
    முழு
do_ecmd(
    முழு		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    முழு		flags,
    win_T	*oldwin)
{
    முழு		other_file;		// TRUE if editing another file
    முழு		oldbuf;			// TRUE if using existing buffer
    முழு		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = வெற்று;
#எனில் defined(FEAT_EVAL)
    முழு		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = வெற்று;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = வெற்று;
#endif
    முழு		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    முழு		newcol = -1;
    முழு		solcol = -1;
    pos_T	*pos;
    char_u	*command = வெற்று;
#ifdef FEAT_SPELL
    முழு		did_get_winopts = FALSE;
#endif
    முழு		readfile_flags = 0;
    முழு		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    எனில் (ERROR_IF_TERM_POPUP_WINDOW)
	திரும்பு FAIL;
#endif

    எனில் (eap != வெற்று)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    எனில் (fnum != 0)
    {
	எனில் (fnum == curbuf->b_fnum)	// file is already being edited
	    திரும்பு OK;			// nothing to do
	other_file = TRUE;
    }
    இல்லையெனில்
    {
#ifdef FEAT_BROWSE
	எனில் ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    எனில் (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		எனில் (ffname == வெற்று || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    இல்லையெனில்
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    வெற்று, வெற்று, வெற்று, curbuf);
		எனில் (browse_file == வெற்று)
		    செல் theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	எனில் (sfname == வெற்று)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	எனில் (sfname != வெற்று)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	எனில் ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == வெற்று || *ffname == NUL))
	    செல் theend;

	எனில் (ffname == வெற்று)
	    other_file = TRUE;
					    // there is no file name
	இல்லையெனில் எனில் (*ffname == NUL && curbuf->b_ffname == வெற்று)
	    other_file = FALSE;
	இல்லையெனில்
	{
	    எனில் (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    எனில் (free_fname != வெற்று)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    எனில் (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == வெற்று ? 0 : CCGD_EXCMD)))
    {
	எனில் (fnum == 0 && other_file && ffname != வெற்று)
	    (வெறுமை)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	செல் theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    எனில் (oldwin != வெற்று && !win_valid(oldwin))
	oldwin = வெற்று;

#எனில் defined(FEAT_EVAL)
    எனில் ((command != வெற்று || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	எனில் (command != வெற்று)
	    valsize = (முழு)STRLEN(command) + 3;
	இல்லையெனில்
	    valsize = 30;
	val.string = alloc(valsize);
	எனில் (val.string != வெற்று)
	{
	    எனில் (command != வெற்று)
		val.length = vim_snprintf_safelen((எழுத்து *)val.string,
		    valsize, ":%s\r", command);
	    இல்லையெனில்
		val.length = vim_snprintf_safelen((எழுத்து *)val.string,
		    valsize, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, val.string, (முழு)val.length);
	    did_set_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    எனில் (other_file)
    {
	முழு prev_alt_fnum = curwin->w_alt_fnum;

	எனில் (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    எனில் ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    எனில் (oldwin != வெற்று)
		buflist_altfpos(oldwin);
	}

	எனில் (fnum)
	    buf = buflist_findnr(fnum);
	இல்லையெனில்
	{
	    எனில் (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		எனில் (command != வெற்று)
		{
		    tlnum = atol((எழுத்து *)command);
		    எனில் (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		எனில் (newbuf != வெற்று)
		{
		    எனில் (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    எனில் (tlnum > 0)
			newbuf->b_last_cursor.lnum = tlnum;
		}
		செல் theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    எனில் (oldwin != வெற்று)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	எனில் (buf == வெற்று)
	    செல் theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	எனில் (buf->b_locked_split)
	{
	    emsg(_(e_cannot_switch_to_a_closing_buffer));
	    செல் theend;
	}
	எனில் (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	எனில் (buf->b_ml.ml_mfp == வெற்று)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	இல்லையெனில்					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (வெறுமை)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    எனில் (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		செல் theend;
#ifdef FEAT_EVAL
	    எனில் (aborting())	    // autocmds may abort script processing
		செல் theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	எனில் ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
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
	எனில் (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    முழு		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    ப்ரதிஜ்ஞா(cmdwin_buf == வெற்று);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = வெற்று;

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
	    எனில் (buf->b_fname != வெற்று)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, வெற்று, வெற்று, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    எனில் (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_செய்தி(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		செல் theend;
	    }
#ifdef FEAT_EVAL
	    எனில் (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		செல் theend;
	    }
#endif
	    எனில் (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    இல்லையெனில்
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		எனில் (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_buffer(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != வெற்று);

		// Autocommands may have closed the window.
		எனில் (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		எனில் (aborting() && curwin->w_buffer != வெற்று)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    செல் theend;
		}
#endif
		// Be careful again, like above.
		எனில் (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    எனில் (curwin->w_buffer == வெற்று)
			buf = lastbuf;
		    இல்லையெனில்
		    {
			delbuf_செய்தி(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			செல் theend;
		    }
		}
		எனில் (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		இல்லையெனில்
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    எனில் (curwin->w_buffer == வெற்று
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    எனில் (curwin->w_buffer != வெற்று)
			--curwin->w_buffer->b_nwindows;

		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    எனில் (!oldbuf && eap != வெற்று)
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
    இல்லையெனில் // !other_file
    {
	எனில் ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    செல் theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    எனில் ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    இல்லையெனில்
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	எனில் (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    எனில் (buf != curbuf)
	செல் theend;
#ifdef FEAT_EVAL
    எனில் (aborting())	    // autocmds may abort script processing
	செல் theend;
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
    எனில் (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	எனில் (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	எனில் (buf->b_fname != வெற்று)
	    new_name = vim_strsave(buf->b_fname);
	இல்லையெனில்
	    new_name = வெற்று;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	எனில் (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_வரி_மொத்தம் <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    எனில் (u_savecommon(0, curbuf->b_ml.ml_வரி_மொத்தம் + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		செல் theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	இல்லையெனில்
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	எனில் (!bufref_valid(&bufref))
	{
	    delbuf_செய்தி(new_name);	// frees new_name
	    செல் theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	எனில் (buf != curbuf)
	    செல் theend;
#ifdef FEAT_EVAL
	எனில் (aborting())	    // autocmds may abort script processing
	    செல் theend;
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
    எனில் (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    எனில் (!auto_buf)
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
		எனில் (win->w_buffer == curbuf)
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
	எனில் (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    எனில் (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    எனில் (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#எனில் defined(FEAT_EVAL)
	    எனில் (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#இல்லையெனில்
	    (வெறுமை)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    எனில் (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	இல்லையெனில்
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, வெற்று, வெற்று, FALSE,
							      curbuf, &retval);
	    எனில் ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, வெற்று, வெற்று, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	எனில் (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    எனில் (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (முழு)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	எனில் (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_வரி_abv_curs();

	maketitle();
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	எனில் (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    எனில் (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    எனில் (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(வெறுமை)parse_spelllang(curwin);
#endif

    எனில் (command == வெற்று)
    {
	எனில் (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	இல்லையெனில் எனில் (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    எனில் (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = true;
	    }
	    இல்லையெனில்
		beginline(BL_SOL | BL_FIX);
	}
	இல்லையெனில்			// no line number, go to last line in Ex mode
	{
	    எனில் (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    எனில் (oldbuf && !auto_buf)
    {
	முழு	செய்தி_scroll_save = செய்தி_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	எனில் (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    செய்தி_scroll = FALSE;
	எனில் (!செய்தி_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	செய்தி_start();
	செய்தி_scroll = செய்தி_scroll_save;
	செய்தி_scrolled_ign = TRUE;

	எனில் (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	செய்தி_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    எனில் (command != வெற்று)
	do_cmdline(command, வெற்று, வெற்று, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    எனில் (curbuf->b_kmap_state & KEYMAP_INIT)
	(வெறுமை)keymap_init();
#endif

    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    எனில் (!skip_redraw)
    {
	n = *so_ptr;
	எனில் (topline == 0 && command == வெற்று)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    எனில் (p_im && (State & MODE_செருகு) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    எனில் (p_acd && curbuf->b_ffname != வெற்று)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	எனில் (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#எனில் defined(FEAT_NETBEANS_INTG)
    எனில் (curbuf->b_ffname != வெற்று)
    {
# ifdef FEAT_NETBEANS_INTG
	எனில் ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    எனில் (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#எனில் defined(FEAT_EVAL)
    எனில் (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, வெற்று, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    திரும்பு retval;
}

    நிலையான வெறுமை
delbuf_செய்தி(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_buffer_str),
					   name == வெற்று ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = வெற்று;
    au_new_curbuf.br_buf_free_மொத்தம் = 0;
}

நிலையான முழு append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    நிலையான char_u *
get_append_வரி(exarg_T *eap, முழு indent)
{
    char_u	*theline;
    char_u	*p;

    எனில் (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    இல்லையெனில் எனில் (eap->ea_getline == வெற்று)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	எனில் (eap->nextcmd == வெற்று)
	    திரும்பு வெற்று;
	p = vim_strchr(eap->nextcmd, NL);
	எனில் (p == வெற்று)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	எனில் (*p != NUL)
	    ++p;
	இல்லையெனில்
	    p = வெற்று;
	eap->nextcmd = p;
    }
    இல்லையெனில்
    {
	முழு save_State = State;

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
    திரும்பு theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    வெறுமை
ex_append(exarg_T *eap)
{
    char_u	*theline;
    முழு		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    முழு		indent = 0;
    char_u	*p;
    முழு		vcol;
    முழு		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    எனில் (not_in_vim9(eap, eap->skip) == FAIL)
	திரும்பு;
#endif
    எனில் (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	போது ((theline = get_append_வரி(eap, 0)) != வெற்று)
	{
	    முழு end = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    எனில் (end)
		நிறுத்து;
	}
	திரும்பு;
    }

    // the ! flag toggles autoindent
    எனில் (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    எனில் (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    எனில் (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    எனில் (empty && lnum == 1)
	lnum = 0;

    State = MODE_செருகு;		    // behave like in Insert mode
    எனில் (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    க்கு (;;)
    {
	செய்தி_scroll = TRUE;
	need_wait_return = FALSE;
	எனில் (curbuf->b_p_ai)
	{
	    எனில் (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    இல்லையெனில் எனில் (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_வரி(eap, indent);
	வரிகள்_இடது = Rows - 1;
	எனில் (theline == வெற்று)
	    நிறுத்து;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	எனில் (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	க்கு (p = theline; indent > vcol; ++p)
	{
	    எனில் (*p == ' ')
		++vcol;
	    இல்லையெனில் எனில் (*p == TAB)
		vcol += 8 - vcol % 8;
	    இல்லையெனில்
		நிறுத்து;
	}
	எனில் ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    நிறுத்து;
	}

	// don't use autoindent if nothing was typed.
	எனில் (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	எனில் (empty)
	    // there are no marks below the inserted lines
	    appended_வரிகள்(lnum, 1L);
	இல்லையெனில்
	    appended_வரிகள்_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	எனில் (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    எனில் (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_வரி_மொத்தம்) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_வரி_மொத்தம்;
	எனில் (eap->cmdidx != CMD_append)
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
    வெறுமை
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ifdef FEAT_EVAL
    எனில் (not_in_vim9(eap, eap->skip) == FAIL)
	திரும்பு;
#endif
    எனில் (eap->skip)
    {
	ex_append(eap);
	திரும்பு;
    }
    எனில் (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	திரும்பு;

    // the ! flag toggles autoindent
    எனில் (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    க்கு (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    நிறுத்து;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_வரிகள்_mark(eap->line1, (long)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    வெறுமை
ex_z(exarg_T *eap)
{
    char_u	*x;
    long	bigness;
    char_u	*kind;
    முழு		minus = 0;
    linenr_T	start, end, curs, i;
    முழு		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    எனில் (eap->forceit)
	bigness = Rows - 1;
    இல்லையெனில் எனில் (!ONE_WINDOW)
	bigness = curwin->w_height - 3;
    இல்லையெனில்
	bigness = curwin->w_p_scr * 2;
    எனில் (bigness < 1)
	bigness = 1;

    x = eap->arg;
    kind = x;
    எனில் (*kind == '-' || *kind == '+' || *kind == '='
					      || *kind == '^' || *kind == '.')
	++x;
    போது (*x == '-' || *x == '+')
	++x;

    எனில் (*x != 0)
    {
	எனில் (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_argument_to_z));
	    திரும்பு;
	}
	இல்லையெனில்
	{
	    bigness = atol((எழுத்து *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    எனில் (bigness > 2 * curbuf->b_ml.ml_வரி_மொத்தம் || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_வரி_மொத்தம்;

	    p_window = bigness;
	    எனில் (*kind == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    எனில் (*kind == '-' || *kind == '+')
	க்கு (x = kind + 1; *x == *kind; ++x)
	    ;

    தேர்வு (*kind)
    {
	நிலை '-':
	    start = lnum - bigness * (linenr_T)(x - kind) + 1;
	    end = start + bigness - 1;
	    curs = end;
	    நிறுத்து;

	நிலை '=':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    நிறுத்து;

	நிலை '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    நிறுத்து;

	நிலை '.':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = end;
	    நிறுத்து;

	இயல்பு:  // '+'
	    start = lnum;
	    எனில் (*kind == '+')
		start += bigness * (linenr_T)(x - kind - 1) + 1;
	    இல்லையெனில் எனில் (eap->addr_மொத்தம் == 0)
		++start;
	    end = start + bigness - 1;
	    curs = end;
	    நிறுத்து;
    }

    எனில் (start < 1)
	start = 1;

    எனில் (end > curbuf->b_ml.ml_வரி_மொத்தம்)
	end = curbuf->b_ml.ml_வரி_மொத்தம்;

    எனில் (curs > curbuf->b_ml.ml_வரி_மொத்தம்)
	curs = curbuf->b_ml.ml_வரி_மொத்தம்;
    இல்லையெனில் எனில் (curs < 1)
	curs = 1;

    க்கு (i = start; i <= end; i++)
    {
	எனில் (minus && i == lnum)
	{
	    செய்தி_putchar('\n');

	    க்கு (j = 1; j < Columns; j++)
		செய்தி_putchar('-');
	}

	print_வரி(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	எனில் (minus && i == lnum)
	{
	    செய்தி_putchar('\n');

	    க்கு (j = 1; j < Columns; j++)
		செய்தி_putchar('-');
	}
    }

    எனில் (curwin->w_cursor.lnum != curs)
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
    முழு
check_restricted(வெறுமை)
{
    எனில் (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_in_rvim));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    முழு
check_secure(வெறுமை)
{
    எனில் (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_தேடல்));
	திரும்பு TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    எனில் (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	திரும்பு TRUE;
    }
#endif
    திரும்பு FALSE;
}

நிலையான char_u	*old_sub = வெற்று;	// previous substitute pattern
நிலையான முழு	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
வகைவரை கட்டமைப்பு {
    முழு	do_all;		// do multiple substitutions per line
    முழு	do_ask;		// ask for confirmation
    முழு	do_மொத்தம்;	// count only
    முழு	do_error;	// if false, ignore errors
    முழு	do_print;	// print last line with subs.
    முழு	do_list;	// list last line with subs.
    முழு	do_எண்;	// list last line with line nr
    முழு	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *start, முழு delimiter)
{
    char_u *p = start;

    போது (p[0])
    {
	எனில் (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    நிறுத்து;
	}
	எனில் (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    திரும்பு p;
}

    நிலையான முழு
check_regexp_delim(முழு c)
{
    எனில் (SAFE_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    வெறுமை
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    long	i = 0;
    regmmatch_T regmatch;
    நிலையான subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ifdef FEAT_EVAL
    subflags_T	subflags_save;
#endif
    முழு		save_do_all;		// remember user specified 'g' flag
    முழு		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = வெற்று;		// init for GCC
    string_T	pat = {வெற்று, 0};
    முழு		delimiter;
    size_t	sublen;
    முழு		got_quit = FALSE;
    முழு		got_match = FALSE;
    முழு		which_pat;
    char_u	*cmd;
    char_u	*p;
    முழு		save_State;
    linenr_T	first_வரி = 0;		// first changed line
    linenr_T	last_வரி = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;
    linenr_T	line2;
    long	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    முழு		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    முழு		start_nsubs;
    முழு		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ifdef FEAT_EVAL
    முழு		save_ma = 0;
    முழு		save_sandbox = 0;
#endif
#ifdef FEAT_PROP_POPUP
    textprop_T	*text_props = வெற்று;
#endif

    cmd = eap->arg;
    எனில் (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    start_nsubs = sub_nsubs;

    எனில் (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	// use last used regexp
    இல்லையெனில்
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    எனில் (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == வெற்று)
    {
	// don't accept alphanumeric for separator
	எனில் (check_regexp_delim(*cmd) == FAIL)
	    திரும்பு;
#ifdef FEAT_EVAL
	எனில் (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    திரும்பு;
#endif

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	எனில் (*cmd == '\\')
	{
	    எனில் (in_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_in_vim9_script));
		திரும்பு;
	    }
	    ++cmd;
	    எனில் (vim_strchr((char_u *)"/?&", *cmd) == வெற்று)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		திரும்பு;
	    }
	    எனில் (*cmd != '&')
		which_pat = RE_தேடல்;	    // use last '/' pattern
	    STR_LITERAL_SET(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	இல்லையெனில்		// find the end of the regexp
	{
	    which_pat = RE_LAST;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, வெற்று, வெற்று);
	    pat.length = (size_t)(cmd - pat.string);
	    எனில் (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	எனில் (sub == வெற்று)
	    // out of memory
	    திரும்பு;

	எனில் (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    எனில் (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != வெற்று)
	    {
		எனில் (old_sub == வெற்று)	// there is no previous command
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    vim_free(sub);
		    திரும்பு;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		எனில் (sub == வெற்று)
		    // out of memory
		    திரும்பு;
	    }
	    இல்லையெனில் எனில் (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		எனில் (old_sub == வெற்று)
		{
		    // out of memory
		    vim_free(sub);
		    திரும்பு;
		}
	    }
	}
    }
    இல்லையெனில் எனில் (!eap->skip)	// use previous pattern and substitution
    {
	எனில் (old_sub == வெற்று)	// there is no previous command
	{
	    emsg(_(e_no_previous_substitute_regular_expression));
	    திரும்பு;
	}
	pat.string = வெற்று;		// search_regcomp() will use previous pattern
	pat.length = 0;
	sub = vim_strsave(old_sub);
	எனில் (sub == வெற்று)
	    திரும்பு;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    எனில் (pat.string != வெற்று && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_வரிகள்_மொத்தம்;

	எனில் (eap->skip)
	{
	    vim_free(sub);
	    திரும்பு;
	}
	curwin->w_cursor.lnum = eap->line1;
	எனில் (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	இல்லையெனில் எனில் (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	இல்லையெனில் எனில் (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_வரிகள்_மொத்தம் = eap->line2 - eap->line1 + 1;
	எனில் (eap->line2 < curbuf->b_ml.ml_வரி_மொத்தம்)
	    ++joined_வரிகள்_மொத்தம்;
	எனில் (joined_வரிகள்_மொத்தம் > 1)
	{
	    (வெறுமை)do_join(joined_வரிகள்_மொத்தம், FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_வரிகள்_மொத்தம் - 1;
	    sub_nlines = 1;
	    (வெறுமை)do_sub_செய்தி(FALSE);
	    ex_may_print(eap);
	}

	எனில் (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.length, magic_isset());
	// put pattern in history
	add_to_history(HIST_தேடல், pat.string, pat.length, TRUE, NUL);
	vim_free(sub);

	திரும்பு;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    எனில் (*cmd == '&')
	++cmd;
    இல்லையெனில்
    {
#ifdef FEAT_EVAL
	எனில் (in_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	இல்லையெனில்
#endif
	எனில் (!p_ed)
	{
	    எனில் (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    இல்லையெனில்
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_மொத்தம் = FALSE;
	subflags.do_எண் = FALSE;
	subflags.do_ic = 0;
    }
    போது (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	எனில் (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	இல்லையெனில் எனில் (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	இல்லையெனில் எனில் (*cmd == 'n')
	    subflags.do_மொத்தம் = TRUE;
	இல்லையெனில் எனில் (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	இல்லையெனில் எனில் (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_LAST;
	இல்லையெனில் எனில் (*cmd == 'p')
	    subflags.do_print = TRUE;
	இல்லையெனில் எனில் (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_எண் = TRUE;
	}
	இல்லையெனில் எனில் (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	இல்லையெனில் எனில் (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	இல்லையெனில் எனில் (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	இல்லையெனில்
	    நிறுத்து;
	++cmd;
    }
    எனில் (subflags.do_மொத்தம்)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    எனில் (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	எனில் (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_positive_மொத்தம்_required));
	    vim_free(sub);
	    திரும்பு;
	}
	இல்லையெனில் எனில் (i >= INT_MAX)
	{
	    எழுத்து	buf[20];
	    vim_snprintf(buf, அளவு(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    திரும்பு;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	எனில் (eap->line2 > curbuf->b_ml.ml_வரி_மொத்தம்)
	    eap->line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    எனில் (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	set_nextcmd(eap, cmd);
	எனில் (eap->nextcmd == வெற்று)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    திரும்பு;
	}
    }

    எனில் (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	திரும்பு;
    }

    எனில் (!subflags.do_மொத்தம் && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	திரும்பு;
    }

    எனில் (தேடல்_regcomp(pat.string, pat.length, வெற்று, RE_SUBST, which_pat, தேடல்_HIS, &regmatch) == FAIL)
    {
	எனில் (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	திரும்பு;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    எனில் (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    இல்லையெனில் எனில் (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = வெற்று;
    sub_firstline.length = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    எனில் (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	எனில் (p == வெற்று)
	    திரும்பு;
	sub = p;
    }
    இல்லையெனில்
    {
	p = regtilde(sub, magic_isset());

	எனில் (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    க்கு (lnum = eap->line1; lnum <= line2 && !(got_quit
#எனில் defined(FEAT_EVAL)
		|| aborting()
#endif
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, வெற்று);
	எனில் (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_start = {வெற்று, 0};
	    size_t	new_start_size = 0;
	    char_u	*new_end;
	    முழு		did_sub = FALSE;
	    முழு		lastone;
	    size_t	copy_len;
	    size_t	needed_size;
	    long	nmatch_tl = 0;	// nr of lines matched below lnum
	    முழு		do_again;	// do it again after joining lines
	    முழு		skip_match = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_split = false;	// "\r" split the line
#ifdef FEAT_PROP_POPUP
	    முழு		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	total_added =  0;
	    முழு		text_prop_மொத்தம் = 0;
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
	    எனில் (!got_match)
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
	    க்கு (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		எனில் (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
		    நிறுத்து;

		எனில் (sub_firstline.string == வெற்று)
		{
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    எனில் (sub_firstline.string == வெற்று)
		    {
			vim_free(new_start.string);
			செல் outofmem;
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
		எனில் (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    எனில் (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_match = TRUE;
		    இல்லையெனில்
		    {
			 // search for a match at next column
			எனில் (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			இல்லையெனில்
			    ++matchcol;
		    }
		    செல் skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		எனில் (subflags.do_மொத்தம்)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    எனில் (nmatch > 1)
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
		    எனில் (!(sub[0] == '\\' && sub[1] == '='))
#endif
			செல் skip;
		}

		எனில் (subflags.do_ask)
		{
		    முழு typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = MODE_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.col = regmatch.startpos[0].col;
		    எனில் (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    எனில் (vim_strchr(p_cpo, CPO_UNDO) != வெற்று)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    போது (subflags.do_ask)
		    {
			எனில் (exmode_active)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_வரி_no_prefix(lnum,
					 subflags.do_எண், subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, வெற்று, வெற்று, 0);
			    curwin->w_cursor.col = regmatch.endpos[0].col - 1;
			    எனில் (curwin->w_cursor.col < 0)
				curwin->w_cursor.col = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   வெற்று, வெற்று, &ec, 0);
			    curwin->w_cursor.col = regmatch.startpos[0].col;
			    எனில் (subflags.do_எண் || curwin->w_p_nu)
			    {
				முழு numw = எண்_width(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    செய்தி_start();
			    க்கு (i = 0; i < (long)sc; ++i)
				செய்தி_putchar(' ');
			    க்கு ( ; i <= (long)ec; ++i)
				செய்தி_putchar('^');

			    resp = getexmodeline('?', வெற்று, 0, TRUE);
			    எனில் (resp != வெற்று)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				எனில் (ex_normal_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			இல்லையெனில்
			{
			    string_T orig_வரி = {வெற்று, 0};
			    முழு    len_change = 0;
			    முழு	   save_p_lz = p_lz;
#ifdef FEAT_FOLDING
			    முழு save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#endif
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    முழு save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    எனில் (new_start.string != வெற்று)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_வரி.length = ml_get_len(lnum);
				orig_வரி.string = vim_strnsave(ml_get(lnum), orig_வரி.length);
				எனில் (orig_வரி.string != வெற்று)
				{
				    string_T new_வரி;

				    new_வரி.string = concat_str(new_start.string,
						     sub_firstline.string + copycol);
				    எனில் (new_வரி.string == வெற்று)
					VIM_CLEAR_STRING(orig_வரி);
				    இல்லையெனில்
				    {
					new_வரி.length =
					    new_start.length + (sub_firstline.length - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (முழு)new_வரி.length - (முழு)orig_வரி.length;
					curwin->w_cursor.col += len_change;
					ml_replace(lnum, new_வரி.string, FALSE);
				    }
				}
			    }

			    தேடல்_match_வரிகள் = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    தேடல்_match_endcol = regmatch.endpos[0].col
								 + len_change;
			    எனில் (தேடல்_match_வரிகள் == 0
						   && தேடல்_match_endcol == 0)
				// highlight at least one character for /^/
				தேடல்_match_endcol = 1;
			    highlight_match = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_match = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ifdef FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#endif
			    எனில் (செய்தி_row == Rows - 1)
				செய்தி_didout = FALSE;	// avoid a scroll-up
			    செய்தி_starthere();
			    i = செய்தி_scroll;
			    செய்தி_scroll = 0;		// truncate msg when
							// needed
			    செய்தி_no_more = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    செய்தி_no_more = FALSE;
			    செய்தி_scroll = i;
			    showruler(TRUE);
			    windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
			    RedrawingDisabled = save_RedrawingDisabled;

#ifdef USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#endif
			    ++no_mapping;	// don't map this key
			    ++allow_சாவிகள்;	// allow special keys
			    typed = plain_vgetc();
			    --allow_சாவிகள்;
			    --no_mapping;

			    // clear the question
			    செய்தி_didout = FALSE;	// don't scroll up
			    செய்தி_col = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    எனில் (orig_வரி.string != வெற்று)
				ml_replace(lnum, orig_வரி.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			எனில் (typed == 'q' || typed == ESC || typed == Ctrl_C
#ifdef UNIX
				|| typed == intr_char
#endif
				)
			{
			    got_quit = TRUE;
			    நிறுத்து;
			}
			எனில் (typed == 'n')
			    நிறுத்து;
			எனில் (typed == 'y')
			    நிறுத்து;
			எனில் (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    நிறுத்து;
			}
			எனில் (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    நிறுத்து;
			}
			எனில் (typed == Ctrl_E)
			    scrollup_clamp();
			இல்லையெனில் எனில் (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    எனில் (vim_strchr(p_cpo, CPO_UNDO) != வெற்று)
			--no_u_sync;

		    எனில் (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			எனில் (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.length;
			    skip_match = TRUE;
			}
			செல் skip;
		    }
		    எனில் (got_quit)
			செல் skip;
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
		எனில் (subflags.do_மொத்தம்)
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
		எனில் (sublen == 0 || aborting() || subflags.do_மொத்தம்)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    செல் skip;
		}
#endif

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		எனில் (nmatch > curbuf->b_ml.ml_வரி_மொத்தம் - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_வரி_மொத்தம் - sub_firstlnum + 1;
		    skip_match = TRUE;
		    // safety check
		    எனில் (nmatch < 0)
			செல் skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		எனில் (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.length = sub_firstline.length;
#ifdef FEAT_PROP_POPUP
		    எனில் (curbuf->b_has_textprop)
		    {
			முழு bytes_added = (முழு)sublen - 1 - (regmatch.endpos[0].col
						   - regmatch.startpos[0].col);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			எனில் (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       bytes_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			total_added += (colnr_T)bytes_added;
		    }
#endif
		}
		இல்லையெனில்
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    எனில் (curbuf->b_has_textprop)
		    {
			char_u	*prop_start;

			// Props in the first line may be shortened or deleted
			எனில் (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			total_added -=
			    (colnr_T)(sub_firstline.length - regmatch.startpos[0].col);

			// Props in the last line may be moved or deleted
			எனில் (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].col, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			text_prop_மொத்தம் = get_text_props(curbuf, lastlnum,
							   &prop_start, FALSE);
			எனில் (text_prop_மொத்தம் > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(text_props);
			    text_props = ALLOC_MULT(textprop_T,
							      text_prop_மொத்தம்);
			    எனில் (text_props != வெற்று)
			    {
				mch_memmove(text_props, prop_start,
					 text_prop_மொத்தம் * அளவு(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				முழு wi = 0;
				க்கு (முழு pi = 0; pi < text_prop_மொத்தம்; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    எனில் (text_props[pi].tp_id < 0
					    || (text_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					தொடர்;
				    text_props[wi] = text_props[pi];
				    text_props[wi].tp_col +=
					regmatch.startpos[0].col + (colnr_T)sublen - 1;
				    text_props[wi].u.tp_text = வெற்று;
				    ++wi;
				}
				text_prop_மொத்தம் = wi;
			    }
			}
		    }
#endif
		    tmp.string = ml_get(lastlnum);
		    tmp.length = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    எனில் (curbuf->b_has_textprop)
			total_added +=
			    (colnr_T)(tmp.length - regmatch.endpos[0].col);
#endif
		}

		copy_len = (size_t)(regmatch.startpos[0].col - copycol);
		needed_size = copy_len
		    + (tmp.length - (size_t)regmatch.endpos[0].col) + sublen + 1;

		எனில் (new_start.string == வெற்று)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_size = needed_size + 50;
		    எனில் ((new_start.string = alloc_clear(new_start_size)) == வெற்று)
			செல் outofmem;
		    *new_start.string = NUL;
		    new_start.length = 0;
		}
		இல்லையெனில்
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_size += new_start.length;
		    எனில் (needed_size > new_start_size)
		    {
			new_start_size = needed_size + 50;
			எனில் ((p1 = alloc_clear(new_start_size)) == வெற்று)
			{
			    vim_free(new_start.string);
			    செல் outofmem;
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

		எனில் (new_start_size - copy_len < sublen)
		    sublen = new_start_size - copy_len - 1;

#ifdef FEAT_EVAL
		++textlock;
#endif
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_end, (முழு)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		எனில் (n > 0)
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
		எனில் (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    எனில் (sub_firstline.string == வெற்று)
		    {
			vim_free(new_start.string);
			செல் outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    எனில் (sub_firstlnum <= line2)
			do_again = TRUE;
		    இல்லையெனில்
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].col;

		எனில் (skip_match)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    எனில் (sub_firstline.string == வெற்று)
		    {
			vim_free(new_start.string);
			செல் outofmem;
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
		க்கு (p1 = new_end; *p1; ++p1)
		{
		    எனில் (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_start.length - (p1 - new_start.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_start.length;
#ifdef FEAT_PROP_POPUP
			எனில் (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    எனில் (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_start.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#endif
		    }
		    இல்லையெனில் எனில் (*p1 == CAR)
		    {
			எனில் (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_start.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_start.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    எனில் (subflags.do_ask)
				appended_வரிகள்(lnum - 1, 1L);
			    இல்லையெனில்
			    {
				எனில் (first_வரி == 0)
				    first_வரி = lnum;
				last_வரி = lnum + 1;
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
		    இல்லையெனில் எனில் (has_mbyte)
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
		எனில் (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_split)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, வெற்று)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    எனில் (new_start.string != வெற்று)
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

			எனில் (u_savesub(lnum) != OK)
			    நிறுத்து;
			ml_replace(lnum, new_start.string, TRUE);
#ifdef FEAT_PROP_POPUP
			எனில் (text_props != வெற்று)
			    add_text_props(lnum, text_props, text_prop_மொத்தம்);
#endif
			எனில் (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    எனில் (u_savedel(lnum, nmatch_tl) != OK)
				நிறுத்து;
			    க்கு (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    எனில் (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    இல்லையெனில்
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    எனில் (subflags.do_ask)
				deleted_வரிகள்(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			எனில் (subflags.do_ask)
			    changed_bytes(lnum, 0);
			இல்லையெனில்
			{
			    எனில் (first_வரி == 0)
				first_வரி = lnum;
			    last_வரி = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_start.string;
			sub_firstline.length = new_start.length;
			new_start.string = வெற்று;
			new_start.length = 0;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);
			copycol = 0;
			did_split = false;
		    }
		    எனில் (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, வெற்று);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    எனில் (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			எனில் (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			நிறுத்து;
		    }
		}

		வரி_breakcheck();
	    }

	    எனில் (did_sub)
		++sub_nlines;
	    vim_free(new_start.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	வரி_breakcheck();
    }

    எனில் (first_வரி != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_வரி_மொத்தம் - old_வரி_மொத்தம்;
	changed_வரிகள்(first_வரி, 0, last_வரி - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ifdef FEAT_PROP_POPUP
    vim_free(text_props);
#endif

    // ":s/pat//n" doesn't move the cursor
    எனில் (subflags.do_மொத்தம்)
	curwin->w_cursor = old_cursor;

    எனில் (sub_nsubs > start_nsubs)
    {
	எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_start.lnum = eap->line1;
	    curbuf->b_op_end.lnum = line2;
	    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	}

	எனில் (!global_busy)
	{
	    // when interactive leave cursor on the match
	    எனில் (!subflags.do_ask)
	    {
		எனில் (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		இல்லையெனில்
		    beginline(BL_WHITE | BL_FIX);
	    }
	    எனில் (!do_sub_செய்தி(subflags.do_மொத்தம்) && subflags.do_ask)
		செய்தி("");
	}
	இல்லையெனில்
	    global_need_beginline = TRUE;
	எனில் (subflags.do_print)
	    print_வரி(curwin->w_cursor.lnum,
					 subflags.do_எண், subflags.do_list);
    }
    இல்லையெனில் எனில் (!global_busy)
    {
	எனில் (got_int)		// interrupted
	    emsg(_(e_interrupted));
	இல்லையெனில் எனில் (got_match)	// did find something but nothing substituted
	    செய்தி("");
	இல்லையெனில் எனில் (subflags.do_error)	// nothing found
	    semsg(_(e_pattern_not_found_str), get_தேடல்_pat());
    }

#ifdef FEAT_FOLDING
    எனில் (subflags.do_ask && hasAnyFolding(curwin))
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
    முழு
do_sub_செய்தி(
    முழு	    மொத்தம்_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    எனில் (((sub_nsubs > p_report && (சாவி_Typed || sub_nlines > 1 || p_report < 1))
		|| மொத்தம்_only)
	    && messaging())
    {
	எழுத்து	*செய்தி_single;
	எழுத்து	*செய்தி_plural;

	எனில் (got_int)
	    STRCPY(செய்தி_buf, _("(Interrupted) "));
	இல்லையெனில்
	    *செய்தி_buf = NUL;

	செய்தி_single = மொத்தம்_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	செய்தி_plural = மொத்தம்_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_add(செய்தி_buf, அளவு(செய்தி_buf),
				 NGETTEXT(செய்தி_single, செய்தி_plural, sub_nlines),
				 sub_nsubs, (long)sub_nlines);

	எனில் (செய்தி(செய்தி_buf))
	    // save message to display it after redraw
	    set_keep_செய்தி((char_u *)செய்தி_buf, 0);
	திரும்பு TRUE;
    }
    எனில் (got_int)
    {
	emsg(_(e_interrupted));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

    நிலையான வெறுமை
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    எனில் (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", வெற்று, வெற்று, DOCMD_NOWAIT);
    இல்லையெனில்
	do_cmdline(cmd, வெற்று, வெற்று, DOCMD_NOWAIT);
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
    வெறுமை
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    முழு		ndone = 0;
    முழு		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*used_pat;
    regmmatch_T	regmatch;
    முழு		match;
    முழு		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    எனில் (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_வரி_மொத்தம்))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	திரும்பு;
    }

    எனில் (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    இல்லையெனில்
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    எனில் (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	திரும்பு;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    எனில் (*cmd == '\\')
    {
	++cmd;
	எனில் (vim_strchr((char_u *)"/?&", *cmd) == வெற்று)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    திரும்பு;
	}
	எனில் (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	இல்லையெனில்
	    which_pat = RE_தேடல்;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    இல்லையெனில் எனில் (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	திரும்பு;
    }
    இல்லையெனில் எனில் (check_regexp_delim(*cmd) == FAIL)
    {
	திரும்பு;
    }
    இல்லையெனில்
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, வெற்று, வெற்று);
	எனில் (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    எனில் (தேடல்_regcomp(pat, patlen, &used_pat, RE_BOTH, which_pat, தேடல்_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	திரும்பு;
    }

    எனில் (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, வெற்று);
	எனில் ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    இல்லையெனில்
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	க்கு (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, வெற்று);
	    எனில் (regmatch.regprog == வெற்று)
		நிறுத்து;  // re-compiling regprog failed
	    எனில் ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    வரி_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	எனில் (got_int)
	    செய்தி(_(e_interrupted));
	இல்லையெனில் எனில் (ndone == 0)
	{
	    எனில் (type == 'v')
	    {
		எனில் (in_vim9script())
		    semsg(_(e_pattern_found_in_every_வரி_str), used_pat);
		இல்லையெனில்
		    smsg(_("Pattern found in every line: %s"), used_pat);
	    }
	    இல்லையெனில்
	    {
		எனில் (in_vim9script())
		    semsg(_(e_pattern_not_found_str), used_pat);
		இல்லையெனில்
		    smsg(_("Pattern not found: %s"), used_pat);
	    }
	}
	இல்லையெனில்
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
    வெறுமை
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
    செய்தி_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_வரி_மொத்தம்;
    போது (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    எனில் (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    இல்லையெனில்
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    changed_வரி_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    எனில் (செய்தி_col == 0 && செய்தி_scrolled == 0)
	செய்தி_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    எனில் (!do_sub_செய்தி(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_வரி_மொத்தம் - old_lcount);
}

#ifdef FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(வெறுமை)
{
    திரும்பு old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    வெறுமை
set_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#endif // FEAT_VIMINFO

#எனில் defined(EXITFREE)
    வெறுமை
free_old_sub(வெறுமை)
{
    vim_free(old_sub);
}
#endif

#எனில் defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    முழு
prepare_tagpreview(
    முழு		undo_sync,	    // sync undo when leaving the window
    முழு		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    எனில் (curwin->w_p_pvw)
	திரும்பு FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    எனில் (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_find_preview_window();
	எனில் (wp != வெற்று)
	    popup_set_wantpos_cursor(wp, wp->w_minwidth, வெற்று);
    }
    இல்லையெனில் எனில் (use_popup != USEPOPUP_NONE)
    {
	wp = popup_find_info_window();
	எனில் (wp != வெற்று)
	{
	    எனில் (use_popup == USEPOPUP_NORMAL)
		popup_show(wp);
	    இல்லையெனில்
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    இல்லையெனில்
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    எனில் (wp->w_p_pvw)
		நிறுத்து;
    }
    எனில் (wp != வெற்று)
    {
	win_enter(wp, undo_sync);
	திரும்பு FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    எனில் ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	திரும்பு popup_create_preview_window(use_popup != USEPOPUP_NONE);
# endif
    எனில் (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	திரும்பு FALSE;
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
    திரும்பு TRUE;
}

#endif

/*
 * Make the user happy.
 */
    வெறுமை
ex_smile(exarg_T *eap UNUSED)
{
    நிலையான எழுத்து *code[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    எழுத்து *p;
    முழு n;
    முழு i;

    செய்தி_start();
    செய்தி_putchar('\n');
    க்கு (i = 0; i < 2; ++i)
	க்கு (p = code[i]; *p != NUL; ++p)
	    எனில் (*p == 'x')
		செய்தி_putchar('\n');
	    இல்லையெனில்
		க்கு (n = *p++; n > 0; --n)
		    எனில் (*p == 'o' || *p == '$')
			செய்தி_putchar_attr(*p, HL_ATTR(HLF_L));
		    இல்லையெனில்
			செய்தி_putchar(*p);
    செய்தி_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    வெறுமை
ex_drop(exarg_T *eap)
{
    முழு		split = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    எனில் (ERROR_IF_POPUP_WINDOW || ERROR_IF_TERM_POPUP_WINDOW)
	திரும்பு;

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
    எனில் (ARGCOUNT == 0)
	திரும்பு;

    எனில் (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	திரும்பு;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	எனில் (wp->w_buffer == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    எனில் (!bufIsChanged(curbuf))
	    {
		முழு save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
		ex_rewind(eap);
	    திரும்பு;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    எனில் (!buf_hide(curbuf))
    {
	++emsg_off;
	split = check_changed(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    எனில் (split)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    இல்லையெனில்
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
skip_vimgrep_pat(char_u *p, char_u **s, முழு *flags)
{
    திரும்பு skip_vimgrep_pat_ext(p, s, flags, வெற்று, வெற்று);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, முழு *flags, char_u **nulp, முழு *cp)
{
    முழு		c;

    எனில் (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	எனில் (s != வெற்று)
	    *s = p;
	p = skiptowhite(p);
	எனில் (s != வெற்று && *p != NUL)
	{
	    எனில் (nulp != வெற்று)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    இல்லையெனில்
    {
	// ":vimgrep /pattern/[g][j] fname"
	எனில் (s != வெற்று)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	எனில் (*p != c)
	    திரும்பு வெற்று;

	// Truncate the pattern.
	எனில் (s != வெற்று)
	{
	    எனில் (nulp != வெற்று)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	போது (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    எனில் (flags != வெற்று)
	    {
		எனில் (*p == 'g')
		    *flags |= VGR_GLOBAL;
		இல்லையெனில் எனில் (*p == 'j')
		    *flags |= VGR_NOJUMP;
		இல்லையெனில்
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    திரும்பு p;
}

#எனில் defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    வெறுமை
ex_oldfiles(exarg_T *eap UNUSED)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    முழு		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    முழு		matches = -1;

    எனில் (l == வெற்று)
    {
	செய்தி(_("No old files"));
	திரும்பு;
    }

    செய்தி_start();
    செய்தி_scroll = TRUE;
    க்கு (li = l->lv_first; li != வெற்று && !got_int; li = li->li_next)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	எனில் (!message_filtered(fname))
	{
	    எனில் (matches < 0)
		matches = nr;
	    இல்லையெனில்
		matches = 0;
	    செய்தி_outnum((long)nr);
	    செய்தி_puts(": ");
	    செய்தி_outtrans(fname);
	    செய்தி_clr_eos();
	    செய்தி_putchar('\n');
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_more = FALSE;
	// we only need to prompt if there is more than 1 match
	எனில் (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	இல்லையெனில்
	    nr = prompt_for_எண்(FALSE);
	செய்தி_starthere();
	எனில் (nr > 0)
	{
	    char_u *p = list_find_str(get_vim_var_list(VV_OLDFILES),
		    (long)nr);

	    எனில் (p != வெற்று)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, வெற்று);
		vim_free(p);
	    }
	}
    }
# endif
}
#endif
