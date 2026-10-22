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

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "version.h"

#অন্তর্ভাৱ <float.h>

স্থির পূর্ণাঙ্ক linelen(পূর্ণাঙ্ক *has_tab);
স্থির শূন্য do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, পূর্ণাঙ্ক do_in, পূর্ণাঙ্ক do_out);
স্থির পূর্ণাঙ্ক not_writing(শূন্য);
স্থির পূর্ণাঙ্ক check_readonly(পূর্ণাঙ্ক *forceit, buf_T *buf);
স্থির শূন্য delbuf_বাৰ্তা(char_u *name);

/*
 * ":ascii" and "ga".
 */
    শূন্য
do_ascii(exarg_T *eap UNUSED)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		cval;
    ৱর্ণ	buf1[20];
    ৱর্ণ	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    পূর্ণাঙ্ক		cc[MAX_MCO];
    পূর্ণাঙ্ক		ci = 0;
    পূর্ণাঙ্ক		len;

    যদি (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    অন্যথা
	c = gchar_cursor();
    যদি (c == NUL)
    {
	বাৰ্তা("NUL");
	ঘূৰাই;
    }

    IObuff[0] = NUL;
    যদি (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	যদি (c == NL)	    // NUL is stored as NL
	    c = NUL;
	যদি (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	অন্যথা
	    cval = c;
	যদি (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, পরিমাণ(buf1), "  <%s>", (ৱর্ণ *)buf3);
	}
	অন্যথা
	    buf1[0] = NUL;
	যদি (c >= 0x80)
	    vim_snprintf(buf2, পরিমাণ(buf2), "  <M-%s>",
						 (ৱর্ণ *)transchar(c & 0x7f));
	অন্যথা
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	যদি (dig != রিক্ত)
	    vim_snprintf((ৱর্ণ *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	অন্যথা
#endif
	    vim_snprintf((ৱর্ণ *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	যদি (enc_utf8)
	    c = cc[ci++];
	অন্যথা
	    c = 0;
    }

    // Repeat for combining characters.
    যেতিয়ালৈ (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (পূর্ণাঙ্ক)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	যদি (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	যদি (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	যদি (dig != রিক্ত)
	    vim_snprintf((ৱর্ণ *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	অন্যথা
#endif
	    vim_snprintf((ৱর্ণ *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	যদি (ci == MAX_MCO)
	    ভঙ্গ;
	যদি (enc_utf8)
	    c = cc[ci++];
	অন্যথা
	    c = 0;
    }

    বাৰ্তা((ৱর্ণ *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    শূন্য
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		indent = 0;
    পূর্ণাঙ্ক		new_indent;
    পূর্ণাঙ্ক		has_tab;
    পূর্ণাঙ্ক		width;

#ifdef FEAT_RIGHTLEFT
    যদি (curwin->w_p_rl)
    {
	// switch left and right aligning
	যদি (eap->cmdidx == CMD_ডান)
	    eap->cmdidx = CMD_বাম;
	অন্যথা যদি (eap->cmdidx == CMD_বাম)
	    eap->cmdidx = CMD_ডান;
    }
#endif

    width = atoi((ৱর্ণ *)eap->arg);
    save_curpos = curwin->w_cursor;
    যদি (eap->cmdidx == CMD_বাম)    // width is used for new indent
    {
	যদি (width >= 0)
	    indent = width;
    }
    অন্যথা
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	যদি (width <= 0)
	    width = curbuf->b_p_tw;
	যদি (width == 0 && curbuf->b_p_wm > 0)
	    width = curwin->w_width - curbuf->b_p_wm;
	যদি (width <= 0)
	    width = 80;
    }

    যদি (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ঘূৰাই;

    বাবে (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	যদি (eap->cmdidx == CMD_বাম)		// left align
	    new_indent = indent;
	অন্যথা
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_ডান ? &has_tab
						   : রিক্ত) - get_indent();

	    যদি (len <= 0)			// skip blank lines
		অনুৱর্ত;

	    যদি (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    অন্যথা
	    {
		new_indent = width - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		যদি (has_tab)
		    যেতিয়ালৈ (new_indent > 0)
		    {
			(শূন্য)set_indent(new_indent, 0);
			যদি (linelen(রিক্ত) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    কৰা
				(শূন্য)set_indent(++new_indent, 0);
			    যেতিয়ালৈ (linelen(রিক্ত) <= width);
			    --new_indent;
			    ভঙ্গ;
			}
			--new_indent;
		    }
	    }
	}
	যদি (new_indent < 0)
	    new_indent = 0;
	(শূন্য)set_indent(new_indent, 0);		// set indent
    }
    changed_সাৰিগুলি(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    স্থির পূর্ণাঙ্ক
linelen(পূর্ণাঙ্ক *has_tab)
{
    char_u  *সাৰি;
    char_u  *first;
    char_u  *last;
    পূর্ণাঙ্ক	    save;
    পূর্ণাঙ্ক	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    সাৰি = ml_get_curline();
    যদি (*সাৰি == NUL)
	ঘূৰাই 0;

    // find the first non-blank character
    first = skipwhite(সাৰি);

    // find the character after the last non-blank character
    বাবে (last = first + STRLEN(first);
				last > first && VIM_ISWHITE(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize_str(সাৰি);	// get line length on screen
    যদি (has_tab != রিক্ত)		// check for embedded TAB
	*has_tab = (vim_strchr(first, TAB) != রিক্ত);
    *last = save;

    ঘূৰাই len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
স্থির char_u	*sortbuf1;
স্থির char_u	*sortbuf2;

স্থির পূর্ণাঙ্ক	sort_lc;	// sort using locale
স্থির পূর্ণাঙ্ক	sort_ic;	// ignore case
স্থির পূর্ণাঙ্ক	sort_nr;	// sort on number
স্থির পূর্ণাঙ্ক	sort_rx;	// sort on regex instead of skipping it
স্থির পূর্ণাঙ্ক	sort_flt;	// sort on floating number

স্থির পূর্ণাঙ্ক	sort_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
প্রকারনির্দেশ সংরচনা
{
    linenr_T	lnum;			// line number
    সঙ্ঘ {
	সংরচনা
	{
	    varnumber_T	start_col_nr;	// starting column number
	    varnumber_T	end_col_nr;	// ending column number
	} সাৰি;
	সংরচনা
	{
	    varnumber_T	মান;		// value if sorting by integer
	    পূর্ণাঙ্ক is_সংখ্যা;		// TRUE when line contains a number
	} অঙ্ক;
	float_T মান_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    স্থির পূর্ণাঙ্ক
string_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    যদি (sort_lc)
	ঘূৰাই strcoll((ৱর্ণ *)s1, (ৱর্ণ *)s2);
    ঘূৰাই sort_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    স্থির পূর্ণাঙ্ক
sort_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    পূর্ণাঙ্ক		result = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    যদি (sort_abort)
	ঘূৰাই 0;
    fast_breakcheck();
    যদি (got_int)
	sort_abort = TRUE;

    যদি (sort_nr)
    {
	যদি (l1.st_u.অঙ্ক.is_সংখ্যা != l2.st_u.অঙ্ক.is_সংখ্যা)
	    result = l1.st_u.অঙ্ক.is_সংখ্যা > l2.st_u.অঙ্ক.is_সংখ্যা ? 1 : -1;
	অন্যথা
	    result = l1.st_u.অঙ্ক.মান == l2.st_u.অঙ্ক.মান ? 0
			     : l1.st_u.অঙ্ক.মান > l2.st_u.অঙ্ক.মান ? 1 : -1;
    }
    অন্যথা যদি (sort_flt)
	result = l1.st_u.মান_flt == l2.st_u.মান_flt ? 0
			     : l1.st_u.মান_flt > l2.st_u.মান_flt ? 1 : -1;
    অন্যথা
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.সাৰি.start_col_nr,
		     l1.st_u.সাৰি.end_col_nr - l1.st_u.সাৰি.start_col_nr + 1);
	sortbuf1[l1.st_u.সাৰি.end_col_nr - l1.st_u.সাৰি.start_col_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.সাৰি.start_col_nr,
		     l2.st_u.সাৰি.end_col_nr - l2.st_u.সাৰি.start_col_nr + 1);
	sortbuf2[l2.st_u.সাৰি.end_col_nr - l2.st_u.সাৰি.start_col_nr] = 0;

	result = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    যদি (result == 0)
	ঘূৰাই (পূর্ণাঙ্ক)(l1.lnum - l2.lnum);
    ঘূৰাই result;
}

/*
 * ":sort".
 */
    শূন্য
ex_sort(exarg_T *eap)
{
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		len;
    linenr_T	lnum;
    long	maxlen = 0;
    sorti_T	*nrs;
    size_t	গুনতি = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    পূর্ণাঙ্ক		unique = FALSE;
    long	deleted;
    colnr_T	start_col;
    colnr_T	end_col;
    পূর্ণাঙ্ক		sort_what = 0;
    পূর্ণাঙ্ক		format_found = 0;
    পূর্ণাঙ্ক		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    যদি (গুনতি <= 1)
	ঘূৰাই;

    যদি (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ঘূৰাই;
    sortbuf1 = রিক্ত;
    sortbuf2 = রিক্ত;
    regmatch.regprog = রিক্ত;
    nrs = ALLOC_MULT(sorti_T, গুনতি);
    যদি (nrs == রিক্ত)
	গচ্ছ sortend;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    বাবে (p = eap->arg; *p != NUL; ++p)
    {
	যদি (VIM_ISWHITE(*p))
	    ;
	অন্যথা যদি (*p == 'i')
	    sort_ic = TRUE;
	অন্যথা যদি (*p == 'l')
	    sort_lc = TRUE;
	অন্যথা যদি (*p == 'r')
	    sort_rx = TRUE;
	অন্যথা যদি (*p == 'n')
	{
	    sort_nr = 1;
	    ++format_found;
	}
	অন্যথা যদি (*p == 'f')
	{
	    sort_flt = 1;
	    ++format_found;
	}
	অন্যথা যদি (*p == 'b')
	{
	    sort_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_found;
	}
	অন্যথা যদি (*p == 'o')
	{
	    sort_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_found;
	}
	অন্যথা যদি (*p == 'x')
	{
	    sort_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_found;
	}
	অন্যথা যদি (*p == 'u')
	    unique = TRUE;
	অন্যথা যদি (*p == '"')	// comment start
	    ভঙ্গ;
	অন্যথা যদি (eap->nextcmd == রিক্ত && check_nextcmd(p) != রিক্ত)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ভঙ্গ;
	}
	অন্যথা যদি (!ASCII_ISALPHA(*p) && regmatch.regprog == রিক্ত)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    যদি (s == রিক্ত)
		গচ্ছ sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    যদি (s == p + 1)
	    {
		যদি (last_অনুসন্ধান_pat() == রিক্ত)
		{
		    emsg(_(e_no_previous_regular_expression));
		    গচ্ছ sortend;
		}
		regmatch.regprog = vim_regcomp(last_অনুসন্ধান_pat(), RE_MAGIC);
	    }
	    অন্যথা
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    যদি (regmatch.regprog == রিক্ত)
		গচ্ছ sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	অন্যথা
	{
	    semsg(_(e_invalid_argument_str), p);
	    গচ্ছ sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    যদি (format_found > 1)
    {
	emsg(_(e_invalid_argument));
	গচ্ছ sortend;
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
    বাবে (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	যদি (maxlen < len)
	    maxlen = len;

	start_col = 0;
	end_col = len;
	যদি (regmatch.regprog != রিক্ত && vim_regexec(&regmatch, s, 0))
	{
	    যদি (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    অন্যথা
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	অন্যথা
	    যদি (regmatch.regprog != রিক্ত)
		end_col = 0;

	যদি (sort_nr || sort_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + end_col;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + start_col;
	    যদি (sort_nr)
	    {
		যদি (sort_what & STR2NR_HEX)
		    s = skiptohex(p);
		অন্যথা যদি (sort_what & STR2NR_BIN)
		    s = skiptobin(p);
		অন্যথা
		    s = skiptodigit(p);
		যদি (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		যদি (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.অঙ্ক.is_সংখ্যা = FALSE;
		    nrs[lnum - eap->line1].st_u.অঙ্ক.মান = 0;
		}
		অন্যথা
		{
		    nrs[lnum - eap->line1].st_u.অঙ্ক.is_সংখ্যা = TRUE;
		    vim_str2nr(s, রিক্ত, রিক্ত, sort_what,
			&nrs[lnum - eap->line1].st_u.অঙ্ক.মান,
			রিক্ত, 0, FALSE, রিক্ত);
		}
	    }
	    অন্যথা
	    {
		s = skipwhite(p);
		যদি (*s == '+')
		    s = skipwhite(s + 1);

		যদি (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.মান_flt = -DBL_MAX;
		অন্যথা
		    nrs[lnum - eap->line1].st_u.মান_flt =
						      strtod((ৱর্ণ *)s, রিক্ত);
	    }
	    *s2 = c;
	}
	অন্যথা
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.সাৰি.start_col_nr = start_col;
	    nrs[lnum - eap->line1].st_u.সাৰি.end_col_nr = end_col;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	যদি (regmatch.regprog != রিক্ত)
	    fast_breakcheck();
	যদি (got_int)
	    গচ্ছ sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    যদি (sortbuf1 == রিক্ত)
	গচ্ছ sortend;
    sortbuf2 = alloc(maxlen + 1);
    যদি (sortbuf2 == রিক্ত)
	গচ্ছ sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((শূন্য *)nrs, গুনতি, পরিমাণ(sorti_T), sort_compare);

    যদি (sort_abort)
	গচ্ছ sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    বাবে (i = 0; i < গুনতি; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? গুনতি - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	যদি (get_lnum + ((linenr_T)গুনতি - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	যদি (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    যদি (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		ভঙ্গ;
	}
	fast_breakcheck();
	যদি (got_int)
	    গচ্ছ sortend;
    }

    // delete the original lines if appending worked
    যদি (i == গুনতি)
	বাবে (i = 0; i < গুনতি; ++i)
	    ml_delete(eap->line1);
    অন্যথা
	গুনতি = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (long)(গুনতি - (lnum - eap->line2));
    যদি (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    অন্যথা যদি (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    যদি (change_occurred || deleted != 0)
	changed_সাৰিগুলি(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    যদি (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    শূন্য
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		len;
    linenr_T	lnum;
    long	maxlen = 0;
    linenr_T	গুনতি = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    পূর্ণাঙ্ক		keep_only_unique = FALSE;
    পূর্ণাঙ্ক		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    long	deleted = 0;
    colnr_T	start_col;
    colnr_T	end_col;
    পূর্ণাঙ্ক		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    যদি (গুনতি <= 1)
	ঘূৰাই;

    যদি (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ঘূৰাই;
    sortbuf1 = রিক্ত;
    regmatch.regprog = রিক্ত;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    বাবে (p = eap->arg; *p != NUL; ++p)
    {
	যদি (VIM_ISWHITE(*p))
	    ;
	অন্যথা যদি (*p == 'i')
	    sort_ic = TRUE;
	অন্যথা যদি (*p == 'l')
	    sort_lc = TRUE;
	অন্যথা যদি (*p == 'r')
	    sort_rx = TRUE;
	অন্যথা যদি (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    যদি (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	অন্যথা যদি (*p == '"')	// comment start
	    ভঙ্গ;
	অন্যথা যদি (eap->nextcmd == রিক্ত && check_nextcmd(p) != রিক্ত)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ভঙ্গ;
	}
	অন্যথা যদি (!ASCII_ISALPHA(*p) && regmatch.regprog == রিক্ত)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    যদি (s == রিক্ত)
		গচ্ছ uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    যদি (s == p + 1)
	    {
		যদি (last_অনুসন্ধান_pat() == রিক্ত)
		{
		    emsg(_(e_no_previous_regular_expression));
		    গচ্ছ uniqend;
		}
		regmatch.regprog = vim_regcomp(last_অনুসন্ধান_pat(), RE_MAGIC);
	    }
	    অন্যথা
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    যদি (regmatch.regprog == রিক্ত)
		গচ্ছ uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	অন্যথা
	{
	    semsg(_(e_invalid_argument_str), p);
	    গচ্ছ uniqend;
	}
    }

    // Find the length of the longest line.
    বাবে (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	যদি (maxlen < len)
	    maxlen = len;

	যদি (got_int)
	    গচ্ছ uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    যদি (sortbuf1 == রিক্ত)
	গচ্ছ uniqend;

    // Delete lines according to options.
    পূর্ণাঙ্ক match_continue = FALSE;
    পূর্ণাঙ্ক next_is_unmatch = FALSE;
    পূর্ণাঙ্ক is_match;
    linenr_T done_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    বাবে (linenr_T i = 0; i < গুনতি; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	start_col = 0;
	end_col = len;
	যদি (regmatch.regprog != রিক্ত && vim_regexec(&regmatch, s, 0))
	{
	    যদি (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    অন্যথা
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	অন্যথা
	    যদি (regmatch.regprog != রিক্ত)
		end_col = 0;
	যদি (end_col > 0)
	{
	    save_c = s[end_col];
	    s[end_col] = NUL;
	}

	is_match = i > 0 ? !string_compare(&s[start_col], sortbuf1) : FALSE;
	delete_lnum = 0;
	যদি (next_is_unmatch)
	{
	    is_match = FALSE;
	    next_is_unmatch = FALSE;
	}

	যদি (!keep_only_unique && !keep_only_not_unique)
	{
	    যদি (is_match)
		delete_lnum = get_lnum;
	    অন্যথা
		STRCPY(sortbuf1, &s[start_col]);
	}
	অন্যথা যদি (keep_only_not_unique)
	{
	    যদি (is_match)
	    {
		done_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    অন্যথা
	    {
		যদি (i > 0 && !match_continue && get_lnum - 1 > done_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    next_is_unmatch = TRUE;
		}
		অন্যথা যদি (i >= গুনতি - 1)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}
	অন্যথা // keep_only_unique
	{
	    যদি (is_match)
	    {
		যদি (!match_continue)
		    delete_lnum = get_lnum - 1;
		অন্যথা
		    delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    অন্যথা
	    {
		যদি (i == 0 && match_continue)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}

	যদি (end_col > 0)
	    s[end_col] = save_c;

	যদি (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    গুনতি--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	যদি (got_int)
	    গচ্ছ uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
    msgmore(-deleted);

    যদি (change_occurred)
	changed_সাৰিগুলি(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    যদি (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণাঙ্ক
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	অঙ্ক_সাৰিগুলি;  // Num lines moved
    linenr_T	last_সাৰি;  // Last line in file after adding new text
#ifdef FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#endif

    যদি (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_of_সাৰিগুলি_into_itself));
	ঘূৰাই FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    যদি (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	যদি (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	অন্যথা
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	ঘূৰাই OK;
    }

    অঙ্ক_সাৰিগুলি = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    যদি (u_save(dest, dest + 1) == FAIL)
	ঘূৰাই FAIL;
    বাবে (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	যদি (str != রিক্ত)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    যদি (dest < line1)
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
    last_সাৰি = curbuf->b_ml.ml_সাৰি_গুনতি;
    mark_adjust_nofold(line1, line2, last_সাৰি - line2, 0L);
    যদি (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -অঙ্ক_সাৰিগুলি, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    যদি (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#endif
	যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest - অঙ্ক_সাৰিগুলি + 1;
	    curbuf->b_op_end.lnum = dest;
	}
    }
    অন্যথা
    {
	mark_adjust_nofold(dest + 1, line1 - 1, অঙ্ক_সাৰিগুলি, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    যদি (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#endif
	যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest + 1;
	    curbuf->b_op_end.lnum = dest + অঙ্ক_সাৰিগুলি;
	}
    }
    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust_nofold(last_সাৰি - অঙ্ক_সাৰিগুলি + 1, last_সাৰি,
					     -(last_সাৰি - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    যদি (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	ঘূৰাই FAIL;

    বাবে (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_MESSAGE);

    যদি (!global_busy && অঙ্ক_সাৰিগুলি > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", অঙ্ক_সাৰিগুলি),
			(long)অঙ্ক_সাৰিগুলি);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    যদি (dest >= line1)
	curwin->w_cursor.lnum = dest;
    অন্যথা
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    যদি (line1 < dest)
    {
	dest += অঙ্ক_সাৰিগুলি + 1;
	last_সাৰি = curbuf->b_ml.ml_সাৰি_গুনতি;
	যদি (dest > last_সাৰি + 1)
	    dest = last_সাৰি + 1;
	changed_সাৰিগুলি(line1, 0, dest, 0L);
    }
    অন্যথা
	changed_সাৰিগুলি(dest + 1, 0, line1 + অঙ্ক_সাৰিগুলি, 0L);

    ঘূৰাই OK;
}

/*
 * ":copy"
 */
    শূন্য
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	গুনতি;
    char_u	*p;

    গুনতি = line2 - line1 + 1;
    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = n + 1;
	curbuf->b_op_end.lnum = n + গুনতি;
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
    যদি (u_save(n, n + 1) == FAIL)
	ঘূৰাই;

    curwin->w_cursor.lnum = n;
    যেতিয়ালৈ (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	যদি (p != রিক্ত)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	যদি (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	যদি (curwin->w_cursor.lnum < line1)
	    ++line1;
	যদি (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_সাৰিগুলি_mark(n, গুনতি);
    যদি (VIsual_active)
	check_pos(curbuf, &VIsual);

    msgmore((long)গুনতি);
}

স্থির char_u	*prevcmd = রিক্ত;	// the previous command

#যদি defined(EXITFREE)
    শূন্য
free_prev_shellcmd(শূন্য)
{
    vim_free(prevcmd);
}
#endif

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    স্থির পূর্ণাঙ্ক
prevcmd_is_set(শূন্য)
{
    যদি (prevcmd == রিক্ত)
    {
	emsg(_(e_no_previous_command));
	ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    শূন্য
do_bang(
    পূর্ণাঙ্ক		addr_গুনতি,
    exarg_T	*eap,
    পূর্ণাঙ্ক		forceit,
    পূর্ণাঙ্ক		do_in,
    পূর্ণাঙ্ক		do_out)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = রিক্ত;		// the new command
    পূর্ণাঙ্ক			free_newcmd = FALSE;    // need to free() newcmd
    পূর্ণাঙ্ক			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    পূর্ণাঙ্ক			len;
    পূর্ণাঙ্ক			scroll_save = বাৰ্তা_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    যদি (check_restricted() || check_secure())
	ঘূৰাই;

    যদি (addr_গুনতি == 0)		// :!
    {
	বাৰ্তা_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	বাৰ্তা_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    কৰা
    {
	len = (পূর্ণাঙ্ক)STRLEN(trailarg) + 1;
	যদি (newcmd != রিক্ত)
	    len += (পূর্ণাঙ্ক)STRLEN(newcmd);
	যদি (ins_prevcmd)
	{
	    যদি (!prevcmd_is_set())
	    {
		vim_free(newcmd);
		ঘূৰাই;
	    }
	    len += (পূর্ণাঙ্ক)STRLEN(prevcmd);
	}
	যদি ((t = alloc(len)) == রিক্ত)
	{
	    vim_free(newcmd);
	    ঘূৰাই;
	}
	*t = NUL;
	যদি (newcmd != রিক্ত)
	    STRCAT(t, newcmd);
	যদি (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = রিক্ত;
	যেতিয়ালৈ (*p)
	{
	    যদি (*p == '!')
	    {
		যদি (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		অন্যথা
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    ভঙ্গ;
		}
	    }
	    ++p;
	}
    } যেতিয়ালৈ (trailarg != রিক্ত);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    যদি (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    অন্যথা
	free_newcmd = TRUE;

    যদি (bangredo)	    // put cmd in redo buffer for ! command
    {
	যদি (!prevcmd_is_set())
	    গচ্ছ theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	যদি (cmd != রিক্ত)
	{
	    AppendToRedobuffLit(cmd, -1);
	    vim_free(cmd);
	}
	অন্যথা
	    AppendToRedobuffLit(prevcmd, -1);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    যদি (*p_shq != NUL)
    {
	যদি (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	যদি (newcmd == রিক্ত)
	    ঘূৰাই;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    যদি (addr_গুনতি == 0)		// :!
    {
	// echo the command
	বাৰ্তা_start();
	বাৰ্তা_putchar(':');
	বাৰ্তা_putchar('!');
	বাৰ্তা_outtrans(newcmd);
	বাৰ্তা_clr_eos();
	windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);

	do_shell(newcmd, 0);
    }
    অন্যথা				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
	apply_autocmds(EVENT_SHELLFILTERPOST, রিক্ত, রিক্ত, FALSE, curbuf);
    }

theend:
    যদি (free_newcmd)
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
    স্থির শূন্য
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    পূর্ণাঙ্ক		do_in,
    পূর্ণাঙ্ক		do_out)
{
    char_u	*itmp = রিক্ত;
    char_u	*otmp = রিক্ত;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    পূর্ণাঙ্ক		shell_flags = 0;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    পূর্ণাঙ্ক		save_cmod_flags = cmdmod.cmod_flags;
#ifdef FEAT_FILTERPIPE
    পূর্ণাঙ্ক		stmp = p_stmp;
#endif

    যদি (*cmd == NUL)	    // no filter command
	ঘূৰাই;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_সাৰি_abv_curs();
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

    যদি (do_out)
	shell_flags |= SHELL_DOOUT;

#ifdef FEAT_FILTERPIPE
# ifdef VIMDLL
    যদি (!gui.in_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    যদি (!do_in && do_out && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    অন্যথা যদি (do_in && !do_out && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
    }
    অন্যথা যদি (do_in && do_out && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    অন্যথা
#endif
	যদি ((do_in && (itmp = vim_tempname('i', FALSE)) == রিক্ত)
		|| (do_out && (otmp = vim_tempname('o', FALSE)) == রিক্ত))
	{
	    emsg(_(e_cant_get_temp_file_name));
	    গচ্ছ filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    যদি (itmp != রিক্ত && buf_write(curbuf, itmp, রিক্ত, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	বাৰ্তা_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#যদি defined(FEAT_EVAL)
	যদি (!aborting())
#endif
	    // will call wait_return()
	    (শূন্য)semsg(_(e_cant_create_file_str), itmp);
	গচ্ছ filterend;
    }
    যদি (curbuf != old_curbuf)
	গচ্ছ filterend;

    যদি (!do_out)
	বাৰ্তা_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    যদি (cmd_buf == রিক্ত)
	গচ্ছ filterend;

    windgoto((পূর্ণাঙ্ক)Rows - 1, cmdline_col_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    যদি (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	redraw_later_clear();

    যদি (do_out)
    {
	যদি (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    গচ্ছ error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_সাৰি_গুনতি;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    যদি (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    যদি (do_out)
    {
	যদি (otmp != রিক্ত)
	{
	    যদি (readfile(otmp, রিক্ত, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#যদি defined(FEAT_EVAL)
		যদি (!aborting())
#endif
		{
		    বাৰ্তা_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		গচ্ছ error;
	    }
	    যদি (curbuf != old_curbuf)
		গচ্ছ filterend;
	}

	read_linecount = curbuf->b_ml.ml_সাৰি_গুনতি - read_linecount;

	যদি (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_start.lnum = line2 + 1;
	    curbuf->b_op_end.lnum = curwin->w_cursor.lnum;
	    appended_সাৰিগুলি_mark(line2, read_linecount);
	}

	যদি (do_in)
	{
	    যদি ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == রিক্ত)
	    {
		যদি (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		অন্যথা যদি (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		অন্যথা
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
	    del_সাৰিগুলি(linecount, TRUE);
	    যদি (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_start.lnum = curbuf->b_op_end.lnum =
					MIN(line1, curbuf->b_ml.ml_সাৰি_গুনতি);
		curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	    }
	    অন্যথা
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
	অন্যথা
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	যদি (linecount > p_report)
	{
	    যদি (do_in)
	    {
		vim_snprintf(বাৰ্তা_buf, পরিমাণ(বাৰ্তা_buf),
				    _("%ld lines filtered"), (long)linecount);
		যদি (বাৰ্তা(বাৰ্তা_buf) && !বাৰ্তা_scroll)
		    // save message to display it after redraw
		    set_keep_বাৰ্তা((char_u *)বাৰ্তা_buf, 0);
	    }
	    অন্যথা
		msgmore((long)linecount);
	}
    }
    অন্যথা
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    যদি (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_buffer));
    }
    অন্যথা যদি (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

    যদি (itmp != রিক্ত)
	mch_remove(itmp);
    যদি (otmp != রিক্ত)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    শূন্য
do_shell(
    char_u	*cmd,
    পূর্ণাঙ্ক		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    পূর্ণাঙ্ক		save_nwr;
#endif
#ifdef MSWIN
    পূর্ণাঙ্ক		winstart = FALSE;
#endif
    পূর্ণাঙ্ক		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    যদি (check_restricted() || check_secure())
    {
	বাৰ্তা_end();
	ঘূৰাই;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    যদি (cmd != রিক্ত)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# যদি defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    যদি (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != রিক্ত)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    বাৰ্তা_putchar('\r');			// put cursor at start of line
    যদি (!autocmd_busy)
    {
	যদি (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    যদি (!winstart)
#endif
	বাৰ্তা_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    যদি (p_warn && !autocmd_busy && বাৰ্তা_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    যদি (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		যদি (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		বাৰ্তা_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		যদি (!keep_termcap)
		    stoptermcap();
#endif
		ভঙ্গ;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    যদি (!swapping_screen())
	windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
    cursor_on();
    (শূন্য)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    যদি (!swapping_screen())
    {
	বাৰ্তা_row = Rows - 1;
	বাৰ্তা_col = 0;
    }

    যদি (autocmd_busy)
    {
	যদি (বাৰ্তা_silent == 0)
	    redraw_later_clear();
    }
    অন্যথা
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	যদি (!gui.in_use)
# endif
	{
	    যদি (cmd == রিক্ত
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		যদি (বাৰ্তা_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    অন্যথা
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		যদি (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		পূর্ণাঙ্ক redraw = বাৰ্তা_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		যদি (gui.in_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(term_console ? -1 : redraw); // see below
# অন্যথা
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	যদি (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	যদি (skip_redraw)		// ':' hit in wait_return()
	{
	    যদি (বাৰ্তা_silent == 0)
		redraw_later_clear();
	}
	অন্যথা যদি (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    যদি (got_int && বাৰ্তা_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    অন্যথা
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, রিক্ত, রিক্ত, FALSE, curbuf);
}

#যদি !defined(UNIX)
    স্থির char_u *
find_pipe(char_u *cmd)
{
    char_u  *p;
    পূর্ণাঙ্ক	    inquote = FALSE;

    বাবে (p = cmd; *p != NUL; ++p)
    {
	যদি (!inquote && *p == '|')
	    ঘূৰাই p;
	যদি (*p == '"')
	    inquote = !inquote;
	অন্যথা যদি (rem_backslash(p))
	    ++p;
    }
    ঘূৰাই রিক্ত;
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
    পূর্ণাঙ্ক		is_powershell = FALSE;
#ifdef UNIX
    পূর্ণাঙ্ক		is_fish_shell;
#endif

    char_u *shell_name = get_isolated_shell_name();
    যদি (shell_name == রিক্ত)
	ঘূৰাই রিক্ত;

#যদি defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    যদি (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    অন্যথা
#endif
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	যদি (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	অন্যথা
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    যদি (itmp != রিক্ত)
    {
	যদি (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	অন্যথা
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    যদি (otmp != রিক্ত)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    যদি (buf == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (is_powershell)
    {
	যদি (itmp != রিক্ত)
	    vim_snprintf((ৱর্ণ *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	অন্যথা
	    vim_snprintf((ৱর্ণ *)buf, len, "& { %s }", cmd);
    }
    অন্যথা
    {
#যদি defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	যদি (itmp != রিক্ত || otmp != রিক্ত)
	{
	    যদি (is_fish_shell)
		vim_snprintf((ৱর্ণ *)buf, len, "begin; %s; end", (ৱর্ণ *)cmd);
	    অন্যথা
		vim_snprintf((ৱর্ণ *)buf, len, "(%s)", (ৱর্ণ *)cmd);
	}
	অন্যথা
	    STRCPY(buf, cmd);
	যদি (itmp != রিক্ত)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#অন্যথা
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	যদি (*p_sxe != NUL && *p_sxq == '(')
	{
	    যদি (itmp != রিক্ত || otmp != রিক্ত)
		vim_snprintf((ৱর্ণ *)buf, len, "(%s)", (ৱর্ণ *)cmd);
	    অন্যথা
		STRCPY(buf, cmd);
	    যদি (itmp != রিক্ত)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	অন্যথা
	{
	    STRCPY(buf, cmd);
	    যদি (itmp != রিক্ত)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		যদি (*p_shq == NUL)
		{
		    p = find_pipe(buf);
		    যদি (p != রিক্ত)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		যদি (*p_shq == NUL)
		{
		    p = find_pipe(cmd);
		    যদি (p != রিক্ত)
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
    যদি (otmp != রিক্ত)
	append_redir(buf, (পূর্ণাঙ্ক)len, p_srr, otmp);

    ঘূৰাই buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    শূন্য
append_redir(
    char_u	*buf,
    পূর্ণাঙ্ক		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*end;

    end = buf + STRLEN(buf);
    // find "%s"
    বাবে (p = opt; (p = vim_strchr(p, '%')) != রিক্ত; ++p)
    {
	যদি (p[1] == 's') // found %s
	    ভঙ্গ;
	যদি (p[1] == '%') // skip %%
	    ++p;
    }
    যদি (p != রিক্ত)
    {
#ifdef MSWIN
	*end++ = ' '; // not really needed? Not with sh, ksh or bash
#endif
	vim_snprintf((ৱর্ণ *)end, (size_t)(buflen - (end - buf)),
						  (ৱর্ণ *)opt, (ৱর্ণ *)fname);
    }
    অন্যথা
	vim_snprintf((ৱর্ণ *)end, (size_t)(buflen - (end - buf)), " %s %s",
		(ৱর্ণ *)opt, (ৱর্ণ *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    শূন্য
do_fixdel(exarg_T *eap UNUSED)
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != রিক্ত
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    শূন্য
print_সাৰি_no_prefix(
    linenr_T	lnum,
    পূর্ণাঙ্ক		use_সংখ্যা,
    পূর্ণাঙ্ক		list)
{
    ৱর্ণ	numbuf[30];

    যদি (curwin->w_p_nu || use_সংখ্যা)
    {
	vim_snprintf(numbuf, পরিমাণ(numbuf),
				   "%*ld ", সংখ্যা_width(curwin), (long)lnum);
	বাৰ্তা_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    বাৰ্তা_prt_সাৰি(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    শূন্য
print_সাৰি(linenr_T lnum, পূর্ণাঙ্ক use_সংখ্যা, পূর্ণাঙ্ক list)
{
    পূর্ণাঙ্ক		save_silent = silent_mode;

    // apply :filter /pat/
    যদি (message_filtered(ml_get(lnum)))
	ঘূৰাই;

    বাৰ্তা_start();
    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()
    print_সাৰি_no_prefix(lnum, use_সংখ্যা, list);
    যদি (save_silent)
    {
	বাৰ্তা_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = save_silent;
    }
    info_message = FALSE;
}

    পূর্ণাঙ্ক
rename_buffer(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, রিক্ত, রিক্ত, FALSE, curbuf);
    // buffer changed, don't change name now
    যদি (buf != curbuf)
	ঘূৰাই FAIL;
#ifdef FEAT_EVAL
    যদি (aborting())	    // autocmds may abort script processing
	ঘূৰাই FAIL;
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
    curbuf->b_ffname = রিক্ত;
    curbuf->b_sfname = রিক্ত;
    যদি (setfname(curbuf, new_fname, রিক্ত, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	ঘূৰাই FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    যদি (xfname != রিক্ত && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	যদি (buf != রিক্ত && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, রিক্ত, রিক্ত, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    ঘূৰাই OK;
}

/*
 * ":file[!] [fname]".
 */
    শূন্য
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    যদি (eap->addr_গুনতি > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_গুনতি > 1))
    {
	emsg(_(e_invalid_argument));
	ঘূৰাই;
    }

    যদি (*eap->arg != NUL || eap->addr_গুনতি == 1)
    {
	যদি (rename_buffer(eap->arg) == FAIL)
	    ঘূৰাই;
	redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    যদি (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    শূন্য
ex_update(exarg_T *eap)
{
    যদি (curbufIsChanged())
	(শূন্য)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    শূন্য
ex_write(exarg_T *eap)
{
    যদি (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_সাৰি_গুনতি;
    }

    যদি (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    অন্যথা
	(শূন্য)do_write(eap);
}

#ifdef UNIX
    স্থির পূর্ণাঙ্ক
check_writable(char_u *fname)
{
    যদি (mch_nodetype(fname) == গ্ৰন্থি_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
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
    পূর্ণাঙ্ক
do_write(exarg_T *eap)
{
    পূর্ণাঙ্ক		other;
    char_u	*fname = রিক্ত;		// init to shut up gcc
    char_u	*ffname;
    পূর্ণাঙ্ক		retval = FAIL;
    char_u	*free_fname = রিক্ত;
#ifdef FEAT_BROWSE
    char_u	*browse_file = রিক্ত;
#endif
    buf_T	*alt_buf = রিক্ত;
    পূর্ণাঙ্ক		name_was_missing;

    যদি (not_writing())		// check 'write' option
	ঘূৰাই FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    যদি ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    রিক্ত, রিক্ত, রিক্ত, curbuf);
	যদি (browse_file == রিক্ত)
	    গচ্ছ theend;
	ffname = browse_file;
    }
#endif
    যদি (*ffname == NUL)
    {
	যদি (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_argument_required));
	    গচ্ছ theend;
	}
	other = FALSE;
    }
    অন্যথা
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	যদি (free_fname != রিক্ত)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    যদি (other)
    {
	যদি (vim_strchr(p_cpo, CPO_ALTWRITE) != রিক্ত
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	অন্যথা
	    alt_buf = buflist_findname(ffname);
	যদি (alt_buf != রিক্ত && alt_buf->b_ml.ml_mfp != রিক্ত)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    গচ্ছ theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    যদি (!other && (bt_dontwrite_বাৰ্তা(curbuf) || check_fname() == FAIL
#ifdef UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#endif
		|| check_readonly(&eap->forceit, curbuf)))
	গচ্ছ theend;

    যদি (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	যদি (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_সাৰি_গুনতি)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    যদি (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		যদি (vim_dialog_yesno(VIM_QUESTION, রিক্ত,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    গচ্ছ theend;
		eap->forceit = TRUE;
	    }
	    অন্যথা
#endif
	    {
		emsg(_(e_use_bang_to_write_partial_buffer));
		গচ্ছ theend;
	    }
	}
    }

    যদি (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	যদি (eap->cmdidx == CMD_saveas && alt_buf != রিক্ত)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, রিক্ত, রিক্ত, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, রিক্ত, রিক্ত, FALSE, alt_buf);
#ifdef FEAT_EVAL
	    যদি (curbuf != was_curbuf || aborting())
#অন্যথা
	    যদি (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		গচ্ছ theend;
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

	    apply_autocmds(EVENT_BUFFILEPOST, রিক্ত, রিক্ত, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, রিক্ত, রিক্ত, FALSE, alt_buf);
	    যদি (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, রিক্ত, রিক্ত, FALSE, alt_buf);
	    }
#ifdef FEAT_EVAL
	    যদি (curbuf != was_curbuf || aborting())
#অন্যথা
	    যদি (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		গচ্ছ theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    যদি (*curbuf->b_p_ft == NUL)
	    {
		যদি (au_has_group((char_u *)"filetypedetect"))
		    (শূন্য)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, রিক্ত);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == রিক্ত;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	যদি (eap->cmdidx == CMD_saveas)
	{
	    যদি (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	যদি (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    ঘূৰাই retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    পূর্ণাঙ্ক
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    পূর্ণাঙ্ক		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    যদি (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == রিক্ত)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	যদি (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    যদি (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		ঘূৰাই FAIL;
	    }
#endif
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    যদি (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_বাৰ্তা_SIZE];

		dialog_বাৰ্তা(buff, _("Overwrite existing file \"%s\"?"), fname);
		যদি (vim_dialog_yesno(VIM_QUESTION, রিক্ত, buff, 2) != VIM_YES)
		    ঘূৰাই FAIL;
		eap->forceit = TRUE;
	    }
	    অন্যথা
#endif
	    {
		emsg(_(e_file_exists));
		ঘূৰাই FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	যদি (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    পূর্ণাঙ্ক		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    যদি (*p_dir == NUL)
	    {
		dir = alloc(5);
		যদি (dir == রিক্ত)
		    ঘূৰাই FAIL;
		STRCPY(dir, ".");
	    }
	    অন্যথা
	    {
		dir = alloc(MAXPATHL);
		যদি (dir == রিক্ত)
		    ঘূৰাই FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    যদি (r)
	    {
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		যদি (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_বাৰ্তা_SIZE];

		    dialog_বাৰ্তা(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    যদি (vim_dialog_yesno(VIM_QUESTION, রিক্ত, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			ঘূৰাই FAIL;
		    }
		    eap->forceit = TRUE;
		}
		অন্যথা
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    ঘূৰাই FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    ঘূৰাই OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    শূন্য
ex_wnext(exarg_T *eap)
{
    পূর্ণাঙ্ক		i;

    যদি (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (পূর্ণাঙ্ক)eap->line2;
    অন্যথা
	i = curwin->w_arg_idx - (পূর্ণাঙ্ক)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_সাৰি_গুনতি;
    যদি (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    শূন্য
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    পূর্ণাঙ্ক		error = 0;
    পূর্ণাঙ্ক		save_forceit = eap->forceit;
    পূর্ণাঙ্ক		save_exiting = exiting;

    যদি (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	যদি (before_quit_all(eap) == FAIL)
	    ঘূৰাই;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ifdef FEAT_TERMINAL
	যদি (exiting && !eap->forceit && term_job_running(buf->b_term))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);
	    যদি (term_try_stop_job(buf) == FAIL)
	    {
		no_write_message_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    অন্যথা যদি (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	অন্যথা
#endif
	যদি (bufIsChanged(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    যদি (not_writing())
	    {
		++error;
		ভঙ্গ;
	    }
#ifdef FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    যদি (buf->b_ffname == রিক্ত && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#endif
	    যদি (buf->b_ffname == রিক্ত)
	    {
		semsg(_(e_no_file_name_for_buffer_nr), (long)buf->b_fnum);
		++error;
	    }
	    অন্যথা যদি (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    অন্যথা
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		যদি (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		যদি (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    যদি (exiting)
    {
	যদি (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    স্থির পূর্ণাঙ্ক
not_writing(শূন্য)
{
    যদি (p_write)
	ঘূৰাই FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    ঘূৰাই TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    স্থির পূর্ণাঙ্ক
check_readonly(পূর্ণাঙ্ক *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    যদি (!*forceit && (buf->b_p_ro
		|| (mch_stat((ৱর্ণ *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	যদি ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != রিক্ত)
	{
	    char_u	buff[DIALOG_বাৰ্তা_SIZE];

	    যদি (buf->b_p_ro)
		dialog_বাৰ্তা(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    অন্যথা
		dialog_বাৰ্তা(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    যদি (vim_dialog_yesno(VIM_QUESTION, রিক্ত, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		ঘূৰাই FALSE;
	    }
	    অন্যথা
		ঘূৰাই TRUE;
	}
	অন্যথা
#endif
	যদি (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_set_add_bang_to_override));
	অন্যথা
	    semsg(_(e_str_is_read_only_add_bang_to_override), buf->b_fname);
	ঘূৰাই TRUE;
    }

    ঘূৰাই FALSE;
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
    পূর্ণাঙ্ক
getfile(
    পূর্ণাঙ্ক		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    পূর্ণাঙ্ক		setpm,
    linenr_T	lnum,
    পূর্ণাঙ্ক		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    পূর্ণাঙ্ক		other;
    পূর্ণাঙ্ক		retval;
    char_u	*free_me = রিক্ত;

    যদি (!check_can_set_curbuf_forceit(forceit))
	ঘূৰাই GETFILE_ERROR;

    যদি (text_locked())
	ঘূৰাই GETFILE_ERROR;
    যদি (curbuf_locked())
	ঘূৰাই GETFILE_ERROR;

    যদি (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    অন্যথা
	other = (fnum != curbuf->b_fnum);

    যদি (other)
	++no_wait_return;	    // don't wait for autowrite message
    যদি (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	যদি (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	যদি (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    গচ্ছ theend;
	}
    }
    যদি (other)
	--no_wait_return;
    যদি (setpm)
	setpcmark();
    যদি (!other)
    {
	যদি (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    অন্যথা যদি (do_ecmd(fnum, ffname, sfname, রিক্ত, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    অন্যথা
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    ঘূৰাই retval;
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
    পূর্ণাঙ্ক
do_ecmd(
    পূর্ণাঙ্ক		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    পূর্ণাঙ্ক		flags,
    win_T	*oldwin)
{
    পূর্ণাঙ্ক		other_file;		// TRUE if editing another file
    পূর্ণাঙ্ক		oldbuf;			// TRUE if using existing buffer
    পূর্ণাঙ্ক		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = রিক্ত;
#যদি defined(FEAT_EVAL)
    পূর্ণাঙ্ক		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = রিক্ত;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = রিক্ত;
#endif
    পূর্ণাঙ্ক		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    পূর্ণাঙ্ক		newcol = -1;
    পূর্ণাঙ্ক		solcol = -1;
    pos_T	*pos;
    char_u	*command = রিক্ত;
#ifdef FEAT_SPELL
    পূর্ণাঙ্ক		did_get_winopts = FALSE;
#endif
    পূর্ণাঙ্ক		readfile_flags = 0;
    পূর্ণাঙ্ক		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    যদি (ERROR_IF_TERM_POPUP_WINDOW)
	ঘূৰাই FAIL;
#endif

    যদি (eap != রিক্ত)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    যদি (fnum != 0)
    {
	যদি (fnum == curbuf->b_fnum)	// file is already being edited
	    ঘূৰাই OK;			// nothing to do
	other_file = TRUE;
    }
    অন্যথা
    {
#ifdef FEAT_BROWSE
	যদি ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    যদি (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		যদি (ffname == রিক্ত || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    অন্যথা
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    রিক্ত, রিক্ত, রিক্ত, curbuf);
		যদি (browse_file == রিক্ত)
		    গচ্ছ theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	যদি (sfname == রিক্ত)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	যদি (sfname != রিক্ত)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	যদি ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == রিক্ত || *ffname == NUL))
	    গচ্ছ theend;

	যদি (ffname == রিক্ত)
	    other_file = TRUE;
					    // there is no file name
	অন্যথা যদি (*ffname == NUL && curbuf->b_ffname == রিক্ত)
	    other_file = FALSE;
	অন্যথা
	{
	    যদি (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    যদি (free_fname != রিক্ত)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    যদি (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == রিক্ত ? 0 : CCGD_EXCMD)))
    {
	যদি (fnum == 0 && other_file && ffname != রিক্ত)
	    (শূন্য)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	গচ্ছ theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    যদি (oldwin != রিক্ত && !win_valid(oldwin))
	oldwin = রিক্ত;

#যদি defined(FEAT_EVAL)
    যদি ((command != রিক্ত || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	যদি (command != রিক্ত)
	    valsize = (পূর্ণাঙ্ক)STRLEN(command) + 3;
	অন্যথা
	    valsize = 30;
	val.string = alloc(valsize);
	যদি (val.string != রিক্ত)
	{
	    যদি (command != রিক্ত)
		val.length = vim_snprintf_safelen((ৱর্ণ *)val.string,
		    valsize, ":%s\r", command);
	    অন্যথা
		val.length = vim_snprintf_safelen((ৱর্ণ *)val.string,
		    valsize, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, val.string, (পূর্ণাঙ্ক)val.length);
	    did_set_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    যদি (other_file)
    {
	পূর্ণাঙ্ক prev_alt_fnum = curwin->w_alt_fnum;

	যদি (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    যদি ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    যদি (oldwin != রিক্ত)
		buflist_altfpos(oldwin);
	}

	যদি (fnum)
	    buf = buflist_findnr(fnum);
	অন্যথা
	{
	    যদি (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		যদি (command != রিক্ত)
		{
		    tlnum = atol((ৱর্ণ *)command);
		    যদি (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		যদি (newbuf != রিক্ত)
		{
		    যদি (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    যদি (tlnum > 0)
			newbuf->b_last_cursor.lnum = tlnum;
		}
		গচ্ছ theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    যদি (oldwin != রিক্ত)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	যদি (buf == রিক্ত)
	    গচ্ছ theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	যদি (buf->b_locked_split)
	{
	    emsg(_(e_cannot_switch_to_a_closing_buffer));
	    গচ্ছ theend;
	}
	যদি (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	যদি (buf->b_ml.ml_mfp == রিক্ত)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	অন্যথা					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (শূন্য)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    যদি (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		গচ্ছ theend;
#ifdef FEAT_EVAL
	    যদি (aborting())	    // autocmds may abort script processing
		গচ্ছ theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	যদি ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
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
	যদি (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    পূর্ণাঙ্ক		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    প্রতিজ্ঞা(cmdwin_buf == রিক্ত);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = রিক্ত;

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
	    যদি (buf->b_fname != রিক্ত)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, রিক্ত, রিক্ত, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    যদি (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_বাৰ্তা(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		গচ্ছ theend;
	    }
#ifdef FEAT_EVAL
	    যদি (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		গচ্ছ theend;
	    }
#endif
	    যদি (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    অন্যথা
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		যদি (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_buffer(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != রিক্ত);

		// Autocommands may have closed the window.
		যদি (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		যদি (aborting() && curwin->w_buffer != রিক্ত)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    গচ্ছ theend;
		}
#endif
		// Be careful again, like above.
		যদি (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    যদি (curwin->w_buffer == রিক্ত)
			buf = lastbuf;
		    অন্যথা
		    {
			delbuf_বাৰ্তা(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			গচ্ছ theend;
		    }
		}
		যদি (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		অন্যথা
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    যদি (curwin->w_buffer == রিক্ত
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    যদি (curwin->w_buffer != রিক্ত)
			--curwin->w_buffer->b_nwindows;

		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    যদি (!oldbuf && eap != রিক্ত)
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
    অন্যথা // !other_file
    {
	যদি ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    গচ্ছ theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    যদি ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    অন্যথা
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	যদি (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    যদি (buf != curbuf)
	গচ্ছ theend;
#ifdef FEAT_EVAL
    যদি (aborting())	    // autocmds may abort script processing
	গচ্ছ theend;
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
    যদি (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	যদি (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	যদি (buf->b_fname != রিক্ত)
	    new_name = vim_strsave(buf->b_fname);
	অন্যথা
	    new_name = রিক্ত;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	যদি (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_সাৰি_গুনতি <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    যদি (u_savecommon(0, curbuf->b_ml.ml_সাৰি_গুনতি + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		গচ্ছ theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	অন্যথা
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	যদি (!bufref_valid(&bufref))
	{
	    delbuf_বাৰ্তা(new_name);	// frees new_name
	    গচ্ছ theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	যদি (buf != curbuf)
	    গচ্ছ theend;
#ifdef FEAT_EVAL
	যদি (aborting())	    // autocmds may abort script processing
	    গচ্ছ theend;
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
    যদি (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    যদি (!auto_buf)
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
		যদি (win->w_buffer == curbuf)
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
	যদি (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    যদি (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    যদি (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#যদি defined(FEAT_EVAL)
	    যদি (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#অন্যথা
	    (শূন্য)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    যদি (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	অন্যথা
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, রিক্ত, রিক্ত, FALSE,
							      curbuf, &retval);
	    যদি ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, রিক্ত, রিক্ত, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	যদি (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    যদি (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (পূর্ণাঙ্ক)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	যদি (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_সাৰি_abv_curs();

	maketitle();
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	যদি (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    যদি (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    যদি (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(শূন্য)parse_spelllang(curwin);
#endif

    যদি (command == রিক্ত)
    {
	যদি (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	অন্যথা যদি (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    যদি (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = true;
	    }
	    অন্যথা
		beginline(BL_SOL | BL_FIX);
	}
	অন্যথা			// no line number, go to last line in Ex mode
	{
	    যদি (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_সাৰি_গুনতি;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    যদি (oldbuf && !auto_buf)
    {
	পূর্ণাঙ্ক	বাৰ্তা_scroll_save = বাৰ্তা_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	যদি (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    বাৰ্তা_scroll = FALSE;
	যদি (!বাৰ্তা_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	বাৰ্তা_start();
	বাৰ্তা_scroll = বাৰ্তা_scroll_save;
	বাৰ্তা_scrolled_ign = TRUE;

	যদি (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	বাৰ্তা_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    যদি (command != রিক্ত)
	do_cmdline(command, রিক্ত, রিক্ত, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    যদি (curbuf->b_kmap_state & KEYMAP_INIT)
	(শূন্য)keymap_init();
#endif

    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    যদি (!skip_redraw)
    {
	n = *so_ptr;
	যদি (topline == 0 && command == রিক্ত)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    যদি (p_im && (State & MODE_ঢোকাও) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    যদি (p_acd && curbuf->b_ffname != রিক্ত)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	যদি (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#যদি defined(FEAT_NETBEANS_INTG)
    যদি (curbuf->b_ffname != রিক্ত)
    {
# ifdef FEAT_NETBEANS_INTG
	যদি ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    যদি (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#যদি defined(FEAT_EVAL)
    যদি (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, রিক্ত, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    ঘূৰাই retval;
}

    স্থির শূন্য
delbuf_বাৰ্তা(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_buffer_str),
					   name == রিক্ত ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = রিক্ত;
    au_new_curbuf.br_buf_free_গুনতি = 0;
}

স্থির পূর্ণাঙ্ক append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    স্থির char_u *
get_append_সাৰি(exarg_T *eap, পূর্ণাঙ্ক indent)
{
    char_u	*theline;
    char_u	*p;

    যদি (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    অন্যথা যদি (eap->ea_getline == রিক্ত)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	যদি (eap->nextcmd == রিক্ত)
	    ঘূৰাই রিক্ত;
	p = vim_strchr(eap->nextcmd, NL);
	যদি (p == রিক্ত)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	যদি (*p != NUL)
	    ++p;
	অন্যথা
	    p = রিক্ত;
	eap->nextcmd = p;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক save_State = State;

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
    ঘূৰাই theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    শূন্য
ex_append(exarg_T *eap)
{
    char_u	*theline;
    পূর্ণাঙ্ক		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    পূর্ণাঙ্ক		indent = 0;
    char_u	*p;
    পূর্ণাঙ্ক		vcol;
    পূর্ণাঙ্ক		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    যদি (not_in_vim9(eap, eap->skip) == FAIL)
	ঘূৰাই;
#endif
    যদি (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	যেতিয়ালৈ ((theline = get_append_সাৰি(eap, 0)) != রিক্ত)
	{
	    পূর্ণাঙ্ক end = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    যদি (end)
		ভঙ্গ;
	}
	ঘূৰাই;
    }

    // the ! flag toggles autoindent
    যদি (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    যদি (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    যদি (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    যদি (empty && lnum == 1)
	lnum = 0;

    State = MODE_ঢোকাও;		    // behave like in Insert mode
    যদি (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    বাবে (;;)
    {
	বাৰ্তা_scroll = TRUE;
	need_wait_return = FALSE;
	যদি (curbuf->b_p_ai)
	{
	    যদি (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    অন্যথা যদি (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_সাৰি(eap, indent);
	সাৰিগুলি_বাম = Rows - 1;
	যদি (theline == রিক্ত)
	    ভঙ্গ;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	যদি (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	বাবে (p = theline; indent > vcol; ++p)
	{
	    যদি (*p == ' ')
		++vcol;
	    অন্যথা যদি (*p == TAB)
		vcol += 8 - vcol % 8;
	    অন্যথা
		ভঙ্গ;
	}
	যদি ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    ভঙ্গ;
	}

	// don't use autoindent if nothing was typed.
	যদি (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	যদি (empty)
	    // there are no marks below the inserted lines
	    appended_সাৰিগুলি(lnum, 1L);
	অন্যথা
	    appended_সাৰিগুলি_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	যদি (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    যদি (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_সাৰি_গুনতি) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_সাৰি_গুনতি;
	যদি (eap->cmdidx != CMD_append)
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
    শূন্য
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ifdef FEAT_EVAL
    যদি (not_in_vim9(eap, eap->skip) == FAIL)
	ঘূৰাই;
#endif
    যদি (eap->skip)
    {
	ex_append(eap);
	ঘূৰাই;
    }
    যদি (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	ঘূৰাই;

    // the ! flag toggles autoindent
    যদি (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    বাবে (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	যদি (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    ভঙ্গ;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_সাৰিগুলি_mark(eap->line1, (long)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    শূন্য
ex_z(exarg_T *eap)
{
    char_u	*x;
    long	bigness;
    char_u	*kind;
    পূর্ণাঙ্ক		minus = 0;
    linenr_T	start, end, curs, i;
    পূর্ণাঙ্ক		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    যদি (eap->forceit)
	bigness = Rows - 1;
    অন্যথা যদি (!ONE_WINDOW)
	bigness = curwin->w_height - 3;
    অন্যথা
	bigness = curwin->w_p_scr * 2;
    যদি (bigness < 1)
	bigness = 1;

    x = eap->arg;
    kind = x;
    যদি (*kind == '-' || *kind == '+' || *kind == '='
					      || *kind == '^' || *kind == '.')
	++x;
    যেতিয়ালৈ (*x == '-' || *x == '+')
	++x;

    যদি (*x != 0)
    {
	যদি (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_argument_to_z));
	    ঘূৰাই;
	}
	অন্যথা
	{
	    bigness = atol((ৱর্ণ *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    যদি (bigness > 2 * curbuf->b_ml.ml_সাৰি_গুনতি || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_সাৰি_গুনতি;

	    p_window = bigness;
	    যদি (*kind == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    যদি (*kind == '-' || *kind == '+')
	বাবে (x = kind + 1; *x == *kind; ++x)
	    ;

    ৱিকল্প (*kind)
    {
	পক্ষ '-':
	    start = lnum - bigness * (linenr_T)(x - kind) + 1;
	    end = start + bigness - 1;
	    curs = end;
	    ভঙ্গ;

	পক্ষ '=':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    ভঙ্গ;

	পক্ষ '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    ভঙ্গ;

	পক্ষ '.':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = end;
	    ভঙ্গ;

	স্ৱভাৱ:  // '+'
	    start = lnum;
	    যদি (*kind == '+')
		start += bigness * (linenr_T)(x - kind - 1) + 1;
	    অন্যথা যদি (eap->addr_গুনতি == 0)
		++start;
	    end = start + bigness - 1;
	    curs = end;
	    ভঙ্গ;
    }

    যদি (start < 1)
	start = 1;

    যদি (end > curbuf->b_ml.ml_সাৰি_গুনতি)
	end = curbuf->b_ml.ml_সাৰি_গুনতি;

    যদি (curs > curbuf->b_ml.ml_সাৰি_গুনতি)
	curs = curbuf->b_ml.ml_সাৰি_গুনতি;
    অন্যথা যদি (curs < 1)
	curs = 1;

    বাবে (i = start; i <= end; i++)
    {
	যদি (minus && i == lnum)
	{
	    বাৰ্তা_putchar('\n');

	    বাবে (j = 1; j < Columns; j++)
		বাৰ্তা_putchar('-');
	}

	print_সাৰি(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	যদি (minus && i == lnum)
	{
	    বাৰ্তা_putchar('\n');

	    বাবে (j = 1; j < Columns; j++)
		বাৰ্তা_putchar('-');
	}
    }

    যদি (curwin->w_cursor.lnum != curs)
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
    পূর্ণাঙ্ক
check_restricted(শূন্য)
{
    যদি (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_in_rvim));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    পূর্ণাঙ্ক
check_secure(শূন্য)
{
    যদি (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_অনুসন্ধান));
	ঘূৰাই TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    যদি (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ঘূৰাই TRUE;
    }
#endif
    ঘূৰাই FALSE;
}

স্থির char_u	*old_sub = রিক্ত;	// previous substitute pattern
স্থির পূর্ণাঙ্ক	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক	do_all;		// do multiple substitutions per line
    পূর্ণাঙ্ক	do_ask;		// ask for confirmation
    পূর্ণাঙ্ক	do_গুনতি;	// count only
    পূর্ণাঙ্ক	do_error;	// if false, ignore errors
    পূর্ণাঙ্ক	do_print;	// print last line with subs.
    পূর্ণাঙ্ক	do_list;	// list last line with subs.
    পূর্ণাঙ্ক	do_সংখ্যা;	// list last line with line nr
    পূর্ণাঙ্ক	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *start, পূর্ণাঙ্ক delimiter)
{
    char_u *p = start;

    যেতিয়ালৈ (p[0])
    {
	যদি (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    ভঙ্গ;
	}
	যদি (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    ঘূৰাই p;
}

    স্থির পূর্ণাঙ্ক
check_regexp_delim(পূর্ণাঙ্ক c)
{
    যদি (SAFE_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    শূন্য
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    long	i = 0;
    regmmatch_T regmatch;
    স্থির subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ifdef FEAT_EVAL
    subflags_T	subflags_save;
#endif
    পূর্ণাঙ্ক		save_do_all;		// remember user specified 'g' flag
    পূর্ণাঙ্ক		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = রিক্ত;		// init for GCC
    string_T	pat = {রিক্ত, 0};
    পূর্ণাঙ্ক		delimiter;
    size_t	sublen;
    পূর্ণাঙ্ক		got_quit = FALSE;
    পূর্ণাঙ্ক		got_match = FALSE;
    পূর্ণাঙ্ক		which_pat;
    char_u	*cmd;
    char_u	*p;
    পূর্ণাঙ্ক		save_State;
    linenr_T	first_সাৰি = 0;		// first changed line
    linenr_T	last_সাৰি = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_সাৰি_গুনতি = curbuf->b_ml.ml_সাৰি_গুনতি;
    linenr_T	line2;
    long	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    পূর্ণাঙ্ক		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    পূর্ণাঙ্ক		start_nsubs;
    পূর্ণাঙ্ক		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		save_ma = 0;
    পূর্ণাঙ্ক		save_sandbox = 0;
#endif
#ifdef FEAT_PROP_POPUP
    textprop_T	*text_props = রিক্ত;
#endif

    cmd = eap->arg;
    যদি (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    start_nsubs = sub_nsubs;

    যদি (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	// use last used regexp
    অন্যথা
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    যদি (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == রিক্ত)
    {
	// don't accept alphanumeric for separator
	যদি (check_regexp_delim(*cmd) == FAIL)
	    ঘূৰাই;
#ifdef FEAT_EVAL
	যদি (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    ঘূৰাই;
#endif

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	যদি (*cmd == '\\')
	{
	    যদি (in_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_in_vim9_script));
		ঘূৰাই;
	    }
	    ++cmd;
	    যদি (vim_strchr((char_u *)"/?&", *cmd) == রিক্ত)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		ঘূৰাই;
	    }
	    যদি (*cmd != '&')
		which_pat = RE_অনুসন্ধান;	    // use last '/' pattern
	    STR_LITERAL_SET(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	অন্যথা		// find the end of the regexp
	{
	    which_pat = RE_LAST;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, রিক্ত, রিক্ত);
	    pat.length = (size_t)(cmd - pat.string);
	    যদি (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	যদি (sub == রিক্ত)
	    // out of memory
	    ঘূৰাই;

	যদি (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    যদি (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != রিক্ত)
	    {
		যদি (old_sub == রিক্ত)	// there is no previous command
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    vim_free(sub);
		    ঘূৰাই;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		যদি (sub == রিক্ত)
		    // out of memory
		    ঘূৰাই;
	    }
	    অন্যথা যদি (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		যদি (old_sub == রিক্ত)
		{
		    // out of memory
		    vim_free(sub);
		    ঘূৰাই;
		}
	    }
	}
    }
    অন্যথা যদি (!eap->skip)	// use previous pattern and substitution
    {
	যদি (old_sub == রিক্ত)	// there is no previous command
	{
	    emsg(_(e_no_previous_substitute_regular_expression));
	    ঘূৰাই;
	}
	pat.string = রিক্ত;		// search_regcomp() will use previous pattern
	pat.length = 0;
	sub = vim_strsave(old_sub);
	যদি (sub == রিক্ত)
	    ঘূৰাই;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    যদি (pat.string != রিক্ত && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_সাৰিগুলি_গুনতি;

	যদি (eap->skip)
	{
	    vim_free(sub);
	    ঘূৰাই;
	}
	curwin->w_cursor.lnum = eap->line1;
	যদি (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	অন্যথা যদি (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	অন্যথা যদি (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_সাৰিগুলি_গুনতি = eap->line2 - eap->line1 + 1;
	যদি (eap->line2 < curbuf->b_ml.ml_সাৰি_গুনতি)
	    ++joined_সাৰিগুলি_গুনতি;
	যদি (joined_সাৰিগুলি_গুনতি > 1)
	{
	    (শূন্য)do_join(joined_সাৰিগুলি_গুনতি, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_সাৰিগুলি_গুনতি - 1;
	    sub_nlines = 1;
	    (শূন্য)do_sub_বাৰ্তা(FALSE);
	    ex_may_print(eap);
	}

	যদি (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.length, magic_isset());
	// put pattern in history
	add_to_history(HIST_অনুসন্ধান, pat.string, pat.length, TRUE, NUL);
	vim_free(sub);

	ঘূৰাই;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    যদি (*cmd == '&')
	++cmd;
    অন্যথা
    {
#ifdef FEAT_EVAL
	যদি (in_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	অন্যথা
#endif
	যদি (!p_ed)
	{
	    যদি (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    অন্যথা
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_গুনতি = FALSE;
	subflags.do_সংখ্যা = FALSE;
	subflags.do_ic = 0;
    }
    যেতিয়ালৈ (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	যদি (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	অন্যথা যদি (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	অন্যথা যদি (*cmd == 'n')
	    subflags.do_গুনতি = TRUE;
	অন্যথা যদি (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	অন্যথা যদি (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_LAST;
	অন্যথা যদি (*cmd == 'p')
	    subflags.do_print = TRUE;
	অন্যথা যদি (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_সংখ্যা = TRUE;
	}
	অন্যথা যদি (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	অন্যথা যদি (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	অন্যথা যদি (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	অন্যথা
	    ভঙ্গ;
	++cmd;
    }
    যদি (subflags.do_গুনতি)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    যদি (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	যদি (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_positive_গুনতি_required));
	    vim_free(sub);
	    ঘূৰাই;
	}
	অন্যথা যদি (i >= INT_MAX)
	{
	    ৱর্ণ	buf[20];
	    vim_snprintf(buf, পরিমাণ(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    ঘূৰাই;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	যদি (eap->line2 > curbuf->b_ml.ml_সাৰি_গুনতি)
	    eap->line2 = curbuf->b_ml.ml_সাৰি_গুনতি;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    যদি (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	set_nextcmd(eap, cmd);
	যদি (eap->nextcmd == রিক্ত)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    ঘূৰাই;
	}
    }

    যদি (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	ঘূৰাই;
    }

    যদি (!subflags.do_গুনতি && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	ঘূৰাই;
    }

    যদি (অনুসন্ধান_regcomp(pat.string, pat.length, রিক্ত, RE_SUBST, which_pat, অনুসন্ধান_HIS, &regmatch) == FAIL)
    {
	যদি (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	ঘূৰাই;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    যদি (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    অন্যথা যদি (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = রিক্ত;
    sub_firstline.length = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    যদি (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	যদি (p == রিক্ত)
	    ঘূৰাই;
	sub = p;
    }
    অন্যথা
    {
	p = regtilde(sub, magic_isset());

	যদি (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    বাবে (lnum = eap->line1; lnum <= line2 && !(got_quit
#যদি defined(FEAT_EVAL)
		|| aborting()
#endif
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, রিক্ত);
	যদি (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_start = {রিক্ত, 0};
	    size_t	new_start_size = 0;
	    char_u	*new_end;
	    পূর্ণাঙ্ক		did_sub = FALSE;
	    পূর্ণাঙ্ক		lastone;
	    size_t	copy_len;
	    size_t	needed_size;
	    long	nmatch_tl = 0;	// nr of lines matched below lnum
	    পূর্ণাঙ্ক		do_again;	// do it again after joining lines
	    পূর্ণাঙ্ক		skip_match = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_split = false;	// "\r" split the line
#ifdef FEAT_PROP_POPUP
	    পূর্ণাঙ্ক		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	total_added =  0;
	    পূর্ণাঙ্ক		text_prop_গুনতি = 0;
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
	    যদি (!got_match)
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
	    বাবে (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		যদি (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		যদি (lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
		    ভঙ্গ;

		যদি (sub_firstline.string == রিক্ত)
		{
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    যদি (sub_firstline.string == রিক্ত)
		    {
			vim_free(new_start.string);
			গচ্ছ outofmem;
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
		যদি (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    যদি (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_match = TRUE;
		    অন্যথা
		    {
			 // search for a match at next column
			যদি (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			অন্যথা
			    ++matchcol;
		    }
		    গচ্ছ skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		যদি (subflags.do_গুনতি)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    যদি (nmatch > 1)
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
		    যদি (!(sub[0] == '\\' && sub[1] == '='))
#endif
			গচ্ছ skip;
		}

		যদি (subflags.do_ask)
		{
		    পূর্ণাঙ্ক typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = MODE_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.col = regmatch.startpos[0].col;
		    যদি (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    যদি (vim_strchr(p_cpo, CPO_UNDO) != রিক্ত)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    যেতিয়ালৈ (subflags.do_ask)
		    {
			যদি (exmode_active)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_সাৰি_no_prefix(lnum,
					 subflags.do_সংখ্যা, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, রিক্ত, রিক্ত, 0);
			    curwin->w_cursor.col = regmatch.endpos[0].col - 1;
			    যদি (curwin->w_cursor.col < 0)
				curwin->w_cursor.col = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   রিক্ত, রিক্ত, &ec, 0);
			    curwin->w_cursor.col = regmatch.startpos[0].col;
			    যদি (subflags.do_সংখ্যা || curwin->w_p_nu)
			    {
				পূর্ণাঙ্ক numw = সংখ্যা_width(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    বাৰ্তা_start();
			    বাবে (i = 0; i < (long)sc; ++i)
				বাৰ্তা_putchar(' ');
			    বাবে ( ; i <= (long)ec; ++i)
				বাৰ্তা_putchar('^');

			    resp = getexmodeline('?', রিক্ত, 0, TRUE);
			    যদি (resp != রিক্ত)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				যদি (ex_normal_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			অন্যথা
			{
			    string_T orig_সাৰি = {রিক্ত, 0};
			    পূর্ণাঙ্ক    len_change = 0;
			    পূর্ণাঙ্ক	   save_p_lz = p_lz;
#ifdef FEAT_FOLDING
			    পূর্ণাঙ্ক save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#endif
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    পূর্ণাঙ্ক save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    যদি (new_start.string != রিক্ত)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_সাৰি.length = ml_get_len(lnum);
				orig_সাৰি.string = vim_strnsave(ml_get(lnum), orig_সাৰি.length);
				যদি (orig_সাৰি.string != রিক্ত)
				{
				    string_T new_সাৰি;

				    new_সাৰি.string = concat_str(new_start.string,
						     sub_firstline.string + copycol);
				    যদি (new_সাৰি.string == রিক্ত)
					VIM_CLEAR_STRING(orig_সাৰি);
				    অন্যথা
				    {
					new_সাৰি.length =
					    new_start.length + (sub_firstline.length - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (পূর্ণাঙ্ক)new_সাৰি.length - (পূর্ণাঙ্ক)orig_সাৰি.length;
					curwin->w_cursor.col += len_change;
					ml_replace(lnum, new_সাৰি.string, FALSE);
				    }
				}
			    }

			    অনুসন্ধান_match_সাৰিগুলি = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    অনুসন্ধান_match_endcol = regmatch.endpos[0].col
								 + len_change;
			    যদি (অনুসন্ধান_match_সাৰিগুলি == 0
						   && অনুসন্ধান_match_endcol == 0)
				// highlight at least one character for /^/
				অনুসন্ধান_match_endcol = 1;
			    highlight_match = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_match = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ifdef FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#endif
			    যদি (বাৰ্তা_row == Rows - 1)
				বাৰ্তা_didout = FALSE;	// avoid a scroll-up
			    বাৰ্তা_starthere();
			    i = বাৰ্তা_scroll;
			    বাৰ্তা_scroll = 0;		// truncate msg when
							// needed
			    বাৰ্তা_no_more = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    বাৰ্তা_no_more = FALSE;
			    বাৰ্তা_scroll = i;
			    showruler(TRUE);
			    windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
			    RedrawingDisabled = save_RedrawingDisabled;

#ifdef USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#endif
			    ++no_mapping;	// don't map this key
			    ++allow_চাবিগুলি;	// allow special keys
			    typed = plain_vgetc();
			    --allow_চাবিগুলি;
			    --no_mapping;

			    // clear the question
			    বাৰ্তা_didout = FALSE;	// don't scroll up
			    বাৰ্তা_col = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    যদি (orig_সাৰি.string != রিক্ত)
				ml_replace(lnum, orig_সাৰি.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			যদি (typed == 'q' || typed == ESC || typed == Ctrl_C
#ifdef UNIX
				|| typed == intr_char
#endif
				)
			{
			    got_quit = TRUE;
			    ভঙ্গ;
			}
			যদি (typed == 'n')
			    ভঙ্গ;
			যদি (typed == 'y')
			    ভঙ্গ;
			যদি (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    ভঙ্গ;
			}
			যদি (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    ভঙ্গ;
			}
			যদি (typed == Ctrl_E)
			    scrollup_clamp();
			অন্যথা যদি (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    যদি (vim_strchr(p_cpo, CPO_UNDO) != রিক্ত)
			--no_u_sync;

		    যদি (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			যদি (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.length;
			    skip_match = TRUE;
			}
			গচ্ছ skip;
		    }
		    যদি (got_quit)
			গচ্ছ skip;
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
		যদি (subflags.do_গুনতি)
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
		যদি (sublen == 0 || aborting() || subflags.do_গুনতি)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    গচ্ছ skip;
		}
#endif

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		যদি (nmatch > curbuf->b_ml.ml_সাৰি_গুনতি - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_সাৰি_গুনতি - sub_firstlnum + 1;
		    skip_match = TRUE;
		    // safety check
		    যদি (nmatch < 0)
			গচ্ছ skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		যদি (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.length = sub_firstline.length;
#ifdef FEAT_PROP_POPUP
		    যদি (curbuf->b_has_textprop)
		    {
			পূর্ণাঙ্ক bytes_added = (পূর্ণাঙ্ক)sublen - 1 - (regmatch.endpos[0].col
						   - regmatch.startpos[0].col);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			যদি (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       bytes_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			total_added += (colnr_T)bytes_added;
		    }
#endif
		}
		অন্যথা
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    যদি (curbuf->b_has_textprop)
		    {
			char_u	*prop_start;

			// Props in the first line may be shortened or deleted
			যদি (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			total_added -=
			    (colnr_T)(sub_firstline.length - regmatch.startpos[0].col);

			// Props in the last line may be moved or deleted
			যদি (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].col, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			text_prop_গুনতি = get_text_props(curbuf, lastlnum,
							   &prop_start, FALSE);
			যদি (text_prop_গুনতি > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(text_props);
			    text_props = ALLOC_MULT(textprop_T,
							      text_prop_গুনতি);
			    যদি (text_props != রিক্ত)
			    {
				mch_memmove(text_props, prop_start,
					 text_prop_গুনতি * পরিমাণ(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				পূর্ণাঙ্ক wi = 0;
				বাবে (পূর্ণাঙ্ক pi = 0; pi < text_prop_গুনতি; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    যদি (text_props[pi].tp_id < 0
					    || (text_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					অনুৱর্ত;
				    text_props[wi] = text_props[pi];
				    text_props[wi].tp_col +=
					regmatch.startpos[0].col + (colnr_T)sublen - 1;
				    text_props[wi].u.tp_text = রিক্ত;
				    ++wi;
				}
				text_prop_গুনতি = wi;
			    }
			}
		    }
#endif
		    tmp.string = ml_get(lastlnum);
		    tmp.length = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    যদি (curbuf->b_has_textprop)
			total_added +=
			    (colnr_T)(tmp.length - regmatch.endpos[0].col);
#endif
		}

		copy_len = (size_t)(regmatch.startpos[0].col - copycol);
		needed_size = copy_len
		    + (tmp.length - (size_t)regmatch.endpos[0].col) + sublen + 1;

		যদি (new_start.string == রিক্ত)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_size = needed_size + 50;
		    যদি ((new_start.string = alloc_clear(new_start_size)) == রিক্ত)
			গচ্ছ outofmem;
		    *new_start.string = NUL;
		    new_start.length = 0;
		}
		অন্যথা
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_size += new_start.length;
		    যদি (needed_size > new_start_size)
		    {
			new_start_size = needed_size + 50;
			যদি ((p1 = alloc_clear(new_start_size)) == রিক্ত)
			{
			    vim_free(new_start.string);
			    গচ্ছ outofmem;
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

		যদি (new_start_size - copy_len < sublen)
		    sublen = new_start_size - copy_len - 1;

#ifdef FEAT_EVAL
		++textlock;
#endif
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_end, (পূর্ণাঙ্ক)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		যদি (n > 0)
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
		যদি (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    যদি (sub_firstline.string == রিক্ত)
		    {
			vim_free(new_start.string);
			গচ্ছ outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    যদি (sub_firstlnum <= line2)
			do_again = TRUE;
		    অন্যথা
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].col;

		যদি (skip_match)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    যদি (sub_firstline.string == রিক্ত)
		    {
			vim_free(new_start.string);
			গচ্ছ outofmem;
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
		বাবে (p1 = new_end; *p1; ++p1)
		{
		    যদি (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_start.length - (p1 - new_start.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_start.length;
#ifdef FEAT_PROP_POPUP
			যদি (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    যদি (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_start.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#endif
		    }
		    অন্যথা যদি (*p1 == CAR)
		    {
			যদি (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_start.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_start.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    যদি (subflags.do_ask)
				appended_সাৰিগুলি(lnum - 1, 1L);
			    অন্যথা
			    {
				যদি (first_সাৰি == 0)
				    first_সাৰি = lnum;
				last_সাৰি = lnum + 1;
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
		    অন্যথা যদি (has_mbyte)
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
		যদি (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_split)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, রিক্ত)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    যদি (new_start.string != রিক্ত)
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

			যদি (u_savesub(lnum) != OK)
			    ভঙ্গ;
			ml_replace(lnum, new_start.string, TRUE);
#ifdef FEAT_PROP_POPUP
			যদি (text_props != রিক্ত)
			    add_text_props(lnum, text_props, text_prop_গুনতি);
#endif
			যদি (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    যদি (u_savedel(lnum, nmatch_tl) != OK)
				ভঙ্গ;
			    বাবে (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    যদি (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    অন্যথা
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    যদি (subflags.do_ask)
				deleted_সাৰিগুলি(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			যদি (subflags.do_ask)
			    changed_bytes(lnum, 0);
			অন্যথা
			{
			    যদি (first_সাৰি == 0)
				first_সাৰি = lnum;
			    last_সাৰি = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_start.string;
			sub_firstline.length = new_start.length;
			new_start.string = রিক্ত;
			new_start.length = 0;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);
			copycol = 0;
			did_split = false;
		    }
		    যদি (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, রিক্ত);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    যদি (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			যদি (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			ভঙ্গ;
		    }
		}

		সাৰি_breakcheck();
	    }

	    যদি (did_sub)
		++sub_nlines;
	    vim_free(new_start.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	সাৰি_breakcheck();
    }

    যদি (first_সাৰি != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_সাৰি_গুনতি - old_সাৰি_গুনতি;
	changed_সাৰিগুলি(first_সাৰি, 0, last_সাৰি - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ifdef FEAT_PROP_POPUP
    vim_free(text_props);
#endif

    // ":s/pat//n" doesn't move the cursor
    যদি (subflags.do_গুনতি)
	curwin->w_cursor = old_cursor;

    যদি (sub_nsubs > start_nsubs)
    {
	যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_start.lnum = eap->line1;
	    curbuf->b_op_end.lnum = line2;
	    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	}

	যদি (!global_busy)
	{
	    // when interactive leave cursor on the match
	    যদি (!subflags.do_ask)
	    {
		যদি (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		অন্যথা
		    beginline(BL_WHITE | BL_FIX);
	    }
	    যদি (!do_sub_বাৰ্তা(subflags.do_গুনতি) && subflags.do_ask)
		বাৰ্তা("");
	}
	অন্যথা
	    global_need_beginline = TRUE;
	যদি (subflags.do_print)
	    print_সাৰি(curwin->w_cursor.lnum,
					 subflags.do_সংখ্যা, subflags.do_list);
    }
    অন্যথা যদি (!global_busy)
    {
	যদি (got_int)		// interrupted
	    emsg(_(e_interrupted));
	অন্যথা যদি (got_match)	// did find something but nothing substituted
	    বাৰ্তা("");
	অন্যথা যদি (subflags.do_error)	// nothing found
	    semsg(_(e_pattern_not_found_str), get_অনুসন্ধান_pat());
    }

#ifdef FEAT_FOLDING
    যদি (subflags.do_ask && hasAnyFolding(curwin))
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
    পূর্ণাঙ্ক
do_sub_বাৰ্তা(
    পূর্ণাঙ্ক	    গুনতি_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    যদি (((sub_nsubs > p_report && (চাবি_Typed || sub_nlines > 1 || p_report < 1))
		|| গুনতি_only)
	    && messaging())
    {
	ৱর্ণ	*বাৰ্তা_single;
	ৱর্ণ	*বাৰ্তা_plural;

	যদি (got_int)
	    STRCPY(বাৰ্তা_buf, _("(Interrupted) "));
	অন্যথা
	    *বাৰ্তা_buf = NUL;

	বাৰ্তা_single = গুনতি_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	বাৰ্তা_plural = গুনতি_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_add(বাৰ্তা_buf, পরিমাণ(বাৰ্তা_buf),
				 NGETTEXT(বাৰ্তা_single, বাৰ্তা_plural, sub_nlines),
				 sub_nsubs, (long)sub_nlines);

	যদি (বাৰ্তা(বাৰ্তা_buf))
	    // save message to display it after redraw
	    set_keep_বাৰ্তা((char_u *)বাৰ্তা_buf, 0);
	ঘূৰাই TRUE;
    }
    যদি (got_int)
    {
	emsg(_(e_interrupted));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

    স্থির শূন্য
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    যদি (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", রিক্ত, রিক্ত, DOCMD_NOWAIT);
    অন্যথা
	do_cmdline(cmd, রিক্ত, রিক্ত, DOCMD_NOWAIT);
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
    শূন্য
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    পূর্ণাঙ্ক		ndone = 0;
    পূর্ণাঙ্ক		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*used_pat;
    regmmatch_T	regmatch;
    পূর্ণাঙ্ক		match;
    পূর্ণাঙ্ক		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    যদি (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_সাৰি_গুনতি))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	ঘূৰাই;
    }

    যদি (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    অন্যথা
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    যদি (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	ঘূৰাই;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    যদি (*cmd == '\\')
    {
	++cmd;
	যদি (vim_strchr((char_u *)"/?&", *cmd) == রিক্ত)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    ঘূৰাই;
	}
	যদি (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	অন্যথা
	    which_pat = RE_অনুসন্ধান;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    অন্যথা যদি (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	ঘূৰাই;
    }
    অন্যথা যদি (check_regexp_delim(*cmd) == FAIL)
    {
	ঘূৰাই;
    }
    অন্যথা
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, রিক্ত, রিক্ত);
	যদি (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    যদি (অনুসন্ধান_regcomp(pat, patlen, &used_pat, RE_BOTH, which_pat, অনুসন্ধান_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	ঘূৰাই;
    }

    যদি (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, রিক্ত);
	যদি ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    অন্যথা
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	বাবে (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, রিক্ত);
	    যদি (regmatch.regprog == রিক্ত)
		ভঙ্গ;  // re-compiling regprog failed
	    যদি ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    সাৰি_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	যদি (got_int)
	    বাৰ্তা(_(e_interrupted));
	অন্যথা যদি (ndone == 0)
	{
	    যদি (type == 'v')
	    {
		যদি (in_vim9script())
		    semsg(_(e_pattern_found_in_every_সাৰি_str), used_pat);
		অন্যথা
		    smsg(_("Pattern found in every line: %s"), used_pat);
	    }
	    অন্যথা
	    {
		যদি (in_vim9script())
		    semsg(_(e_pattern_not_found_str), used_pat);
		অন্যথা
		    smsg(_("Pattern not found: %s"), used_pat);
	    }
	}
	অন্যথা
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
    শূন্য
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
    বাৰ্তা_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_সাৰি_গুনতি;
    যেতিয়ালৈ (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    যদি (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    অন্যথা
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    changed_সাৰি_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    যদি (বাৰ্তা_col == 0 && বাৰ্তা_scrolled == 0)
	বাৰ্তা_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    যদি (!do_sub_বাৰ্তা(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_সাৰি_গুনতি - old_lcount);
}

#ifdef FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(শূন্য)
{
    ঘূৰাই old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    শূন্য
set_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#endif // FEAT_VIMINFO

#যদি defined(EXITFREE)
    শূন্য
free_old_sub(শূন্য)
{
    vim_free(old_sub);
}
#endif

#যদি defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    পূর্ণাঙ্ক
prepare_tagpreview(
    পূর্ণাঙ্ক		undo_sync,	    // sync undo when leaving the window
    পূর্ণাঙ্ক		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    যদি (curwin->w_p_pvw)
	ঘূৰাই FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    যদি (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_find_preview_window();
	যদি (wp != রিক্ত)
	    popup_set_wantpos_cursor(wp, wp->w_minwidth, রিক্ত);
    }
    অন্যথা যদি (use_popup != USEPOPUP_NONE)
    {
	wp = popup_find_info_window();
	যদি (wp != রিক্ত)
	{
	    যদি (use_popup == USEPOPUP_NORMAL)
		popup_show(wp);
	    অন্যথা
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    অন্যথা
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    যদি (wp->w_p_pvw)
		ভঙ্গ;
    }
    যদি (wp != রিক্ত)
    {
	win_enter(wp, undo_sync);
	ঘূৰাই FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    যদি ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	ঘূৰাই popup_create_preview_window(use_popup != USEPOPUP_NONE);
# endif
    যদি (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	ঘূৰাই FALSE;
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
    ঘূৰাই TRUE;
}

#endif

/*
 * Make the user happy.
 */
    শূন্য
ex_smile(exarg_T *eap UNUSED)
{
    স্থির ৱর্ণ *code[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    ৱর্ণ *p;
    পূর্ণাঙ্ক n;
    পূর্ণাঙ্ক i;

    বাৰ্তা_start();
    বাৰ্তা_putchar('\n');
    বাবে (i = 0; i < 2; ++i)
	বাবে (p = code[i]; *p != NUL; ++p)
	    যদি (*p == 'x')
		বাৰ্তা_putchar('\n');
	    অন্যথা
		বাবে (n = *p++; n > 0; --n)
		    যদি (*p == 'o' || *p == '$')
			বাৰ্তা_putchar_attr(*p, HL_ATTR(HLF_L));
		    অন্যথা
			বাৰ্তা_putchar(*p);
    বাৰ্তা_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    শূন্য
ex_drop(exarg_T *eap)
{
    পূর্ণাঙ্ক		split = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    যদি (ERROR_IF_POPUP_WINDOW || ERROR_IF_TERM_POPUP_WINDOW)
	ঘূৰাই;

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
    যদি (ARGCOUNT == 0)
	ঘূৰাই;

    যদি (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	ঘূৰাই;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	যদি (wp->w_buffer == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    যদি (!bufIsChanged(curbuf))
	    {
		পূর্ণাঙ্ক save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
		ex_rewind(eap);
	    ঘূৰাই;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    যদি (!buf_hide(curbuf))
    {
	++emsg_off;
	split = check_changed(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    যদি (split)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    অন্যথা
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
skip_vimgrep_pat(char_u *p, char_u **s, পূর্ণাঙ্ক *flags)
{
    ঘূৰাই skip_vimgrep_pat_ext(p, s, flags, রিক্ত, রিক্ত);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, পূর্ণাঙ্ক *flags, char_u **nulp, পূর্ণাঙ্ক *cp)
{
    পূর্ণাঙ্ক		c;

    যদি (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	যদি (s != রিক্ত)
	    *s = p;
	p = skiptowhite(p);
	যদি (s != রিক্ত && *p != NUL)
	{
	    যদি (nulp != রিক্ত)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    অন্যথা
    {
	// ":vimgrep /pattern/[g][j] fname"
	যদি (s != রিক্ত)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	যদি (*p != c)
	    ঘূৰাই রিক্ত;

	// Truncate the pattern.
	যদি (s != রিক্ত)
	{
	    যদি (nulp != রিক্ত)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	যেতিয়ালৈ (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    যদি (flags != রিক্ত)
	    {
		যদি (*p == 'g')
		    *flags |= VGR_GLOBAL;
		অন্যথা যদি (*p == 'j')
		    *flags |= VGR_NOJUMP;
		অন্যথা
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    ঘূৰাই p;
}

#যদি defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    শূন্য
ex_oldfiles(exarg_T *eap UNUSED)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    পূর্ণাঙ্ক		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    পূর্ণাঙ্ক		matches = -1;

    যদি (l == রিক্ত)
    {
	বাৰ্তা(_("No old files"));
	ঘূৰাই;
    }

    বাৰ্তা_start();
    বাৰ্তা_scroll = TRUE;
    বাবে (li = l->lv_first; li != রিক্ত && !got_int; li = li->li_next)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	যদি (!message_filtered(fname))
	{
	    যদি (matches < 0)
		matches = nr;
	    অন্যথা
		matches = 0;
	    বাৰ্তা_outnum((long)nr);
	    বাৰ্তা_puts(": ");
	    বাৰ্তা_outtrans(fname);
	    বাৰ্তা_clr_eos();
	    বাৰ্তা_putchar('\n');
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_more = FALSE;
	// we only need to prompt if there is more than 1 match
	যদি (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	অন্যথা
	    nr = prompt_for_সংখ্যা(FALSE);
	বাৰ্তা_starthere();
	যদি (nr > 0)
	{
	    char_u *p = list_find_str(get_vim_var_list(VV_OLDFILES),
		    (long)nr);

	    যদি (p != রিক্ত)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, রিক্ত);
		vim_free(p);
	    }
	}
    }
# endif
}
#endif
