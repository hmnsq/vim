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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#അന്തര്ഭാവ <float.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ linelen(പൂർണ്ണസംഖ്യ *has_tab);
സ്ഥിതം ശൂന്യം do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, പൂർണ്ണസംഖ്യ do_in, പൂർണ്ണസംഖ്യ do_out);
സ്ഥിതം പൂർണ്ണസംഖ്യ not_writing(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ check_readonly(പൂർണ്ണസംഖ്യ *forceit, buf_T *buf);
സ്ഥിതം ശൂന്യം delbuf_സന്ദേശം(char_u *name);

/*
 * ":ascii" and "ga".
 */
    ശൂന്യം
do_ascii(exarg_T *eap UNUSED)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		cval;
    അക്ഷരം	buf1[20];
    അക്ഷരം	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    പൂർണ്ണസംഖ്യ		cc[MAX_MCO];
    പൂർണ്ണസംഖ്യ		ci = 0;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    അല്ലെങ്കിൽ
	c = gchar_cursor();
    എങ്കിൽ (c == NUL)
    {
	സന്ദേശം("NUL");
	തിരികെ;
    }

    IObuff[0] = NUL;
    എങ്കിൽ (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	എങ്കിൽ (c == NL)	    // NUL is stored as NL
	    c = NUL;
	എങ്കിൽ (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	അല്ലെങ്കിൽ
	    cval = c;
	എങ്കിൽ (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, വലുപ്പം(buf1), "  <%s>", (അക്ഷരം *)buf3);
	}
	അല്ലെങ്കിൽ
	    buf1[0] = NUL;
	എങ്കിൽ (c >= 0x80)
	    vim_snprintf(buf2, വലുപ്പം(buf2), "  <M-%s>",
						 (അക്ഷരം *)transchar(c & 0x7f));
	അല്ലെങ്കിൽ
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	എങ്കിൽ (dig != ശൂന്യ)
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	അല്ലെങ്കിൽ
#endif
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	എങ്കിൽ (enc_utf8)
	    c = cc[ci++];
	അല്ലെങ്കിൽ
	    c = 0;
    }

    // Repeat for combining characters.
    ആകുമ്പോൾ (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	എങ്കിൽ (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	എങ്കിൽ (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	എങ്കിൽ (dig != ശൂന്യ)
	    vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	അല്ലെങ്കിൽ
#endif
	    vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	എങ്കിൽ (ci == MAX_MCO)
	    നിർത്തുക;
	എങ്കിൽ (enc_utf8)
	    c = cc[ci++];
	അല്ലെങ്കിൽ
	    c = 0;
    }

    സന്ദേശം((അക്ഷരം *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    ശൂന്യം
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		indent = 0;
    പൂർണ്ണസംഖ്യ		new_indent;
    പൂർണ്ണസംഖ്യ		has_tab;
    പൂർണ്ണസംഖ്യ		width;

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl)
    {
	// switch left and right aligning
	എങ്കിൽ (eap->cmdidx == CMD_വലത്)
	    eap->cmdidx = CMD_ഇടത്;
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_ഇടത്)
	    eap->cmdidx = CMD_വലത്;
    }
#endif

    width = atoi((അക്ഷരം *)eap->arg);
    save_curpos = curwin->w_cursor;
    എങ്കിൽ (eap->cmdidx == CMD_ഇടത്)    // width is used for new indent
    {
	എങ്കിൽ (width >= 0)
	    indent = width;
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	എങ്കിൽ (width <= 0)
	    width = curbuf->b_p_tw;
	എങ്കിൽ (width == 0 && curbuf->b_p_wm > 0)
	    width = curwin->w_width - curbuf->b_p_wm;
	എങ്കിൽ (width <= 0)
	    width = 80;
    }

    എങ്കിൽ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	തിരികെ;

    വേണ്ടി (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	എങ്കിൽ (eap->cmdidx == CMD_ഇടത്)		// left align
	    new_indent = indent;
	അല്ലെങ്കിൽ
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_വലത് ? &has_tab
						   : ശൂന്യ) - get_indent();

	    എങ്കിൽ (len <= 0)			// skip blank lines
		തുടരുക;

	    എങ്കിൽ (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    അല്ലെങ്കിൽ
	    {
		new_indent = width - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		എങ്കിൽ (has_tab)
		    ആകുമ്പോൾ (new_indent > 0)
		    {
			(ശൂന്യം)set_indent(new_indent, 0);
			എങ്കിൽ (linelen(ശൂന്യ) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    ചെയ്യുക
				(ശൂന്യം)set_indent(++new_indent, 0);
			    ആകുമ്പോൾ (linelen(ശൂന്യ) <= width);
			    --new_indent;
			    നിർത്തുക;
			}
			--new_indent;
		    }
	    }
	}
	എങ്കിൽ (new_indent < 0)
	    new_indent = 0;
	(ശൂന്യം)set_indent(new_indent, 0);		// set indent
    }
    changed_വരികൾ(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
linelen(പൂർണ്ണസംഖ്യ *has_tab)
{
    char_u  *വരി;
    char_u  *first;
    char_u  *last;
    പൂർണ്ണസംഖ്യ	    save;
    പൂർണ്ണസംഖ്യ	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    വരി = ml_get_curline();
    എങ്കിൽ (*വരി == NUL)
	തിരികെ 0;

    // find the first non-blank character
    first = skipwhite(വരി);

    // find the character after the last non-blank character
    വേണ്ടി (last = first + STRLEN(first);
				last > first && VIM_ISWHITE(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize_str(വരി);	// get line length on screen
    എങ്കിൽ (has_tab != ശൂന്യ)		// check for embedded TAB
	*has_tab = (vim_strchr(first, TAB) != ശൂന്യ);
    *last = save;

    തിരികെ len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
സ്ഥിതം char_u	*sortbuf1;
സ്ഥിതം char_u	*sortbuf2;

സ്ഥിതം പൂർണ്ണസംഖ്യ	sort_lc;	// sort using locale
സ്ഥിതം പൂർണ്ണസംഖ്യ	sort_ic;	// ignore case
സ്ഥിതം പൂർണ്ണസംഖ്യ	sort_nr;	// sort on number
സ്ഥിതം പൂർണ്ണസംഖ്യ	sort_rx;	// sort on regex instead of skipping it
സ്ഥിതം പൂർണ്ണസംഖ്യ	sort_flt;	// sort on floating number

സ്ഥിതം പൂർണ്ണസംഖ്യ	sort_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
തരനിർവചനം ഘടന
{
    linenr_T	lnum;			// line number
    സംഘം {
	ഘടന
	{
	    varnumber_T	start_col_nr;	// starting column number
	    varnumber_T	end_col_nr;	// ending column number
	} വരി;
	ഘടന
	{
	    varnumber_T	മൂല്യം;		// value if sorting by integer
	    പൂർണ്ണസംഖ്യ is_സംഖ്യ;		// TRUE when line contains a number
	} അക്കം;
	float_T മൂല്യം_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
string_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    എങ്കിൽ (sort_lc)
	തിരികെ strcoll((അക്ഷരം *)s1, (അക്ഷരം *)s2);
    തിരികെ sort_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
sort_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    പൂർണ്ണസംഖ്യ		result = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    എങ്കിൽ (sort_abort)
	തിരികെ 0;
    fast_breakcheck();
    എങ്കിൽ (got_int)
	sort_abort = TRUE;

    എങ്കിൽ (sort_nr)
    {
	എങ്കിൽ (l1.st_u.അക്കം.is_സംഖ്യ != l2.st_u.അക്കം.is_സംഖ്യ)
	    result = l1.st_u.അക്കം.is_സംഖ്യ > l2.st_u.അക്കം.is_സംഖ്യ ? 1 : -1;
	അല്ലെങ്കിൽ
	    result = l1.st_u.അക്കം.മൂല്യം == l2.st_u.അക്കം.മൂല്യം ? 0
			     : l1.st_u.അക്കം.മൂല്യം > l2.st_u.അക്കം.മൂല്യം ? 1 : -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (sort_flt)
	result = l1.st_u.മൂല്യം_flt == l2.st_u.മൂല്യം_flt ? 0
			     : l1.st_u.മൂല്യം_flt > l2.st_u.മൂല്യം_flt ? 1 : -1;
    അല്ലെങ്കിൽ
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.വരി.start_col_nr,
		     l1.st_u.വരി.end_col_nr - l1.st_u.വരി.start_col_nr + 1);
	sortbuf1[l1.st_u.വരി.end_col_nr - l1.st_u.വരി.start_col_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.വരി.start_col_nr,
		     l2.st_u.വരി.end_col_nr - l2.st_u.വരി.start_col_nr + 1);
	sortbuf2[l2.st_u.വരി.end_col_nr - l2.st_u.വരി.start_col_nr] = 0;

	result = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    എങ്കിൽ (result == 0)
	തിരികെ (പൂർണ്ണസംഖ്യ)(l1.lnum - l2.lnum);
    തിരികെ result;
}

/*
 * ":sort".
 */
    ശൂന്യം
ex_sort(exarg_T *eap)
{
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		len;
    linenr_T	lnum;
    long	maxlen = 0;
    sorti_T	*nrs;
    size_t	എണ്ണം = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    പൂർണ്ണസംഖ്യ		unique = FALSE;
    long	deleted;
    colnr_T	start_col;
    colnr_T	end_col;
    പൂർണ്ണസംഖ്യ		sort_what = 0;
    പൂർണ്ണസംഖ്യ		format_found = 0;
    പൂർണ്ണസംഖ്യ		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    എങ്കിൽ (എണ്ണം <= 1)
	തിരികെ;

    എങ്കിൽ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	തിരികെ;
    sortbuf1 = ശൂന്യ;
    sortbuf2 = ശൂന്യ;
    regmatch.regprog = ശൂന്യ;
    nrs = ALLOC_MULT(sorti_T, എണ്ണം);
    എങ്കിൽ (nrs == ശൂന്യ)
	പോകുക sortend;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    വേണ്ടി (p = eap->arg; *p != NUL; ++p)
    {
	എങ്കിൽ (VIM_ISWHITE(*p))
	    ;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'i')
	    sort_ic = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'l')
	    sort_lc = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'r')
	    sort_rx = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'n')
	{
	    sort_nr = 1;
	    ++format_found;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'f')
	{
	    sort_flt = 1;
	    ++format_found;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'b')
	{
	    sort_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_found;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'o')
	{
	    sort_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_found;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'x')
	{
	    sort_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_found;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'u')
	    unique = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"')	// comment start
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (eap->nextcmd == ശൂന്യ && check_nextcmd(p) != ശൂന്യ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!ASCII_ISALPHA(*p) && regmatch.regprog == ശൂന്യ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    എങ്കിൽ (s == p + 1)
	    {
		എങ്കിൽ (last_തിരയൽ_pat() == ശൂന്യ)
		{
		    emsg(_(e_no_previous_regular_expression));
		    പോകുക sortend;
		}
		regmatch.regprog = vim_regcomp(last_തിരയൽ_pat(), RE_MAGIC);
	    }
	    അല്ലെങ്കിൽ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		പോകുക sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_argument_str), p);
	    പോകുക sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    എങ്കിൽ (format_found > 1)
    {
	emsg(_(e_invalid_argument));
	പോകുക sortend;
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
    വേണ്ടി (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	എങ്കിൽ (maxlen < len)
	    maxlen = len;

	start_col = 0;
	end_col = len;
	എങ്കിൽ (regmatch.regprog != ശൂന്യ && vim_regexec(&regmatch, s, 0))
	{
	    എങ്കിൽ (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    അല്ലെങ്കിൽ
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	അല്ലെങ്കിൽ
	    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
		end_col = 0;

	എങ്കിൽ (sort_nr || sort_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + end_col;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + start_col;
	    എങ്കിൽ (sort_nr)
	    {
		എങ്കിൽ (sort_what & STR2NR_HEX)
		    s = skiptohex(p);
		അല്ലെങ്കിൽ എങ്കിൽ (sort_what & STR2NR_BIN)
		    s = skiptobin(p);
		അല്ലെങ്കിൽ
		    s = skiptodigit(p);
		എങ്കിൽ (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		എങ്കിൽ (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.അക്കം.is_സംഖ്യ = FALSE;
		    nrs[lnum - eap->line1].st_u.അക്കം.മൂല്യം = 0;
		}
		അല്ലെങ്കിൽ
		{
		    nrs[lnum - eap->line1].st_u.അക്കം.is_സംഖ്യ = TRUE;
		    vim_str2nr(s, ശൂന്യ, ശൂന്യ, sort_what,
			&nrs[lnum - eap->line1].st_u.അക്കം.മൂല്യം,
			ശൂന്യ, 0, FALSE, ശൂന്യ);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		s = skipwhite(p);
		എങ്കിൽ (*s == '+')
		    s = skipwhite(s + 1);

		എങ്കിൽ (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.മൂല്യം_flt = -DBL_MAX;
		അല്ലെങ്കിൽ
		    nrs[lnum - eap->line1].st_u.മൂല്യം_flt =
						      strtod((അക്ഷരം *)s, ശൂന്യ);
	    }
	    *s2 = c;
	}
	അല്ലെങ്കിൽ
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.വരി.start_col_nr = start_col;
	    nrs[lnum - eap->line1].st_u.വരി.end_col_nr = end_col;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	എങ്കിൽ (regmatch.regprog != ശൂന്യ)
	    fast_breakcheck();
	എങ്കിൽ (got_int)
	    പോകുക sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    എങ്കിൽ (sortbuf1 == ശൂന്യ)
	പോകുക sortend;
    sortbuf2 = alloc(maxlen + 1);
    എങ്കിൽ (sortbuf2 == ശൂന്യ)
	പോകുക sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((ശൂന്യം *)nrs, എണ്ണം, വലുപ്പം(sorti_T), sort_compare);

    എങ്കിൽ (sort_abort)
	പോകുക sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? എണ്ണം - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	എങ്കിൽ (get_lnum + ((linenr_T)എണ്ണം - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	എങ്കിൽ (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    എങ്കിൽ (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		നിർത്തുക;
	}
	fast_breakcheck();
	എങ്കിൽ (got_int)
	    പോകുക sortend;
    }

    // delete the original lines if appending worked
    എങ്കിൽ (i == എണ്ണം)
	വേണ്ടി (i = 0; i < എണ്ണം; ++i)
	    ml_delete(eap->line1);
    അല്ലെങ്കിൽ
	എണ്ണം = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (long)(എണ്ണം - (lnum - eap->line2));
    എങ്കിൽ (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    എങ്കിൽ (change_occurred || deleted != 0)
	changed_വരികൾ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    എങ്കിൽ (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    ശൂന്യം
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		len;
    linenr_T	lnum;
    long	maxlen = 0;
    linenr_T	എണ്ണം = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    പൂർണ്ണസംഖ്യ		keep_only_unique = FALSE;
    പൂർണ്ണസംഖ്യ		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    long	deleted = 0;
    colnr_T	start_col;
    colnr_T	end_col;
    പൂർണ്ണസംഖ്യ		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    എങ്കിൽ (എണ്ണം <= 1)
	തിരികെ;

    എങ്കിൽ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	തിരികെ;
    sortbuf1 = ശൂന്യ;
    regmatch.regprog = ശൂന്യ;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    വേണ്ടി (p = eap->arg; *p != NUL; ++p)
    {
	എങ്കിൽ (VIM_ISWHITE(*p))
	    ;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'i')
	    sort_ic = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'l')
	    sort_lc = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'r')
	    sort_rx = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    എങ്കിൽ (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"')	// comment start
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (eap->nextcmd == ശൂന്യ && check_nextcmd(p) != ശൂന്യ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!ASCII_ISALPHA(*p) && regmatch.regprog == ശൂന്യ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    എങ്കിൽ (s == p + 1)
	    {
		എങ്കിൽ (last_തിരയൽ_pat() == ശൂന്യ)
		{
		    emsg(_(e_no_previous_regular_expression));
		    പോകുക uniqend;
		}
		regmatch.regprog = vim_regcomp(last_തിരയൽ_pat(), RE_MAGIC);
	    }
	    അല്ലെങ്കിൽ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		പോകുക uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_argument_str), p);
	    പോകുക uniqend;
	}
    }

    // Find the length of the longest line.
    വേണ്ടി (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	എങ്കിൽ (maxlen < len)
	    maxlen = len;

	എങ്കിൽ (got_int)
	    പോകുക uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    എങ്കിൽ (sortbuf1 == ശൂന്യ)
	പോകുക uniqend;

    // Delete lines according to options.
    പൂർണ്ണസംഖ്യ match_continue = FALSE;
    പൂർണ്ണസംഖ്യ next_is_unmatch = FALSE;
    പൂർണ്ണസംഖ്യ is_match;
    linenr_T done_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    വേണ്ടി (linenr_T i = 0; i < എണ്ണം; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	start_col = 0;
	end_col = len;
	എങ്കിൽ (regmatch.regprog != ശൂന്യ && vim_regexec(&regmatch, s, 0))
	{
	    എങ്കിൽ (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    അല്ലെങ്കിൽ
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	അല്ലെങ്കിൽ
	    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
		end_col = 0;
	എങ്കിൽ (end_col > 0)
	{
	    save_c = s[end_col];
	    s[end_col] = NUL;
	}

	is_match = i > 0 ? !string_compare(&s[start_col], sortbuf1) : FALSE;
	delete_lnum = 0;
	എങ്കിൽ (next_is_unmatch)
	{
	    is_match = FALSE;
	    next_is_unmatch = FALSE;
	}

	എങ്കിൽ (!keep_only_unique && !keep_only_not_unique)
	{
	    എങ്കിൽ (is_match)
		delete_lnum = get_lnum;
	    അല്ലെങ്കിൽ
		STRCPY(sortbuf1, &s[start_col]);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (keep_only_not_unique)
	{
	    എങ്കിൽ (is_match)
	    {
		done_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (i > 0 && !match_continue && get_lnum - 1 > done_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    next_is_unmatch = TRUE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (i >= എണ്ണം - 1)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}
	അല്ലെങ്കിൽ // keep_only_unique
	{
	    എങ്കിൽ (is_match)
	    {
		എങ്കിൽ (!match_continue)
		    delete_lnum = get_lnum - 1;
		അല്ലെങ്കിൽ
		    delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (i == 0 && match_continue)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}

	എങ്കിൽ (end_col > 0)
	    s[end_col] = save_c;

	എങ്കിൽ (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    എണ്ണം--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	എങ്കിൽ (got_int)
	    പോകുക uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
    msgmore(-deleted);

    എങ്കിൽ (change_occurred)
	changed_വരികൾ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    എങ്കിൽ (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	അക്കം_വരികൾ;  // Num lines moved
    linenr_T	last_വരി;  // Last line in file after adding new text
#ifdef FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#endif

    എങ്കിൽ (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_of_വരികൾ_into_itself));
	തിരികെ FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    എങ്കിൽ (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	എങ്കിൽ (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	അല്ലെങ്കിൽ
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	തിരികെ OK;
    }

    അക്കം_വരികൾ = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    എങ്കിൽ (u_save(dest, dest + 1) == FAIL)
	തിരികെ FAIL;
    വേണ്ടി (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	എങ്കിൽ (str != ശൂന്യ)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    എങ്കിൽ (dest < line1)
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
    last_വരി = curbuf->b_ml.ml_വരി_എണ്ണം;
    mark_adjust_nofold(line1, line2, last_വരി - line2, 0L);
    എങ്കിൽ (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -അക്കം_വരികൾ, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    എങ്കിൽ (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#endif
	എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest - അക്കം_വരികൾ + 1;
	    curbuf->b_op_end.lnum = dest;
	}
    }
    അല്ലെങ്കിൽ
    {
	mark_adjust_nofold(dest + 1, line1 - 1, അക്കം_വരികൾ, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    എങ്കിൽ (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#endif
	എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest + 1;
	    curbuf->b_op_end.lnum = dest + അക്കം_വരികൾ;
	}
    }
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust_nofold(last_വരി - അക്കം_വരികൾ + 1, last_വരി,
					     -(last_വരി - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    എങ്കിൽ (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	തിരികെ FAIL;

    വേണ്ടി (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_MESSAGE);

    എങ്കിൽ (!global_busy && അക്കം_വരികൾ > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", അക്കം_വരികൾ),
			(long)അക്കം_വരികൾ);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    എങ്കിൽ (dest >= line1)
	curwin->w_cursor.lnum = dest;
    അല്ലെങ്കിൽ
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    എങ്കിൽ (line1 < dest)
    {
	dest += അക്കം_വരികൾ + 1;
	last_വരി = curbuf->b_ml.ml_വരി_എണ്ണം;
	എങ്കിൽ (dest > last_വരി + 1)
	    dest = last_വരി + 1;
	changed_വരികൾ(line1, 0, dest, 0L);
    }
    അല്ലെങ്കിൽ
	changed_വരികൾ(dest + 1, 0, line1 + അക്കം_വരികൾ, 0L);

    തിരികെ OK;
}

/*
 * ":copy"
 */
    ശൂന്യം
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	എണ്ണം;
    char_u	*p;

    എണ്ണം = line2 - line1 + 1;
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = n + 1;
	curbuf->b_op_end.lnum = n + എണ്ണം;
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
    എങ്കിൽ (u_save(n, n + 1) == FAIL)
	തിരികെ;

    curwin->w_cursor.lnum = n;
    ആകുമ്പോൾ (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	എങ്കിൽ (p != ശൂന്യ)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	എങ്കിൽ (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	എങ്കിൽ (curwin->w_cursor.lnum < line1)
	    ++line1;
	എങ്കിൽ (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_വരികൾ_mark(n, എണ്ണം);
    എങ്കിൽ (VIsual_active)
	check_pos(curbuf, &VIsual);

    msgmore((long)എണ്ണം);
}

സ്ഥിതം char_u	*prevcmd = ശൂന്യ;	// the previous command

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_prev_shellcmd(ശൂന്യം)
{
    vim_free(prevcmd);
}
#endif

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prevcmd_is_set(ശൂന്യം)
{
    എങ്കിൽ (prevcmd == ശൂന്യ)
    {
	emsg(_(e_no_previous_command));
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    ശൂന്യം
do_bang(
    പൂർണ്ണസംഖ്യ		addr_എണ്ണം,
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		forceit,
    പൂർണ്ണസംഖ്യ		do_in,
    പൂർണ്ണസംഖ്യ		do_out)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = ശൂന്യ;		// the new command
    പൂർണ്ണസംഖ്യ			free_newcmd = FALSE;    // need to free() newcmd
    പൂർണ്ണസംഖ്യ			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    പൂർണ്ണസംഖ്യ			len;
    പൂർണ്ണസംഖ്യ			scroll_save = സന്ദേശം_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (addr_എണ്ണം == 0)		// :!
    {
	സന്ദേശം_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	സന്ദേശം_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    ചെയ്യുക
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(trailarg) + 1;
	എങ്കിൽ (newcmd != ശൂന്യ)
	    len += (പൂർണ്ണസംഖ്യ)STRLEN(newcmd);
	എങ്കിൽ (ins_prevcmd)
	{
	    എങ്കിൽ (!prevcmd_is_set())
	    {
		vim_free(newcmd);
		തിരികെ;
	    }
	    len += (പൂർണ്ണസംഖ്യ)STRLEN(prevcmd);
	}
	എങ്കിൽ ((t = alloc(len)) == ശൂന്യ)
	{
	    vim_free(newcmd);
	    തിരികെ;
	}
	*t = NUL;
	എങ്കിൽ (newcmd != ശൂന്യ)
	    STRCAT(t, newcmd);
	എങ്കിൽ (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = ശൂന്യ;
	ആകുമ്പോൾ (*p)
	{
	    എങ്കിൽ (*p == '!')
	    {
		എങ്കിൽ (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		അല്ലെങ്കിൽ
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    നിർത്തുക;
		}
	    }
	    ++p;
	}
    } ആകുമ്പോൾ (trailarg != ശൂന്യ);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    എങ്കിൽ (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    അല്ലെങ്കിൽ
	free_newcmd = TRUE;

    എങ്കിൽ (bangredo)	    // put cmd in redo buffer for ! command
    {
	എങ്കിൽ (!prevcmd_is_set())
	    പോകുക theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	എങ്കിൽ (cmd != ശൂന്യ)
	{
	    AppendToRedobuffLit(cmd, -1);
	    vim_free(cmd);
	}
	അല്ലെങ്കിൽ
	    AppendToRedobuffLit(prevcmd, -1);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    എങ്കിൽ (*p_shq != NUL)
    {
	എങ്കിൽ (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	എങ്കിൽ (newcmd == ശൂന്യ)
	    തിരികെ;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    എങ്കിൽ (addr_എണ്ണം == 0)		// :!
    {
	// echo the command
	സന്ദേശം_start();
	സന്ദേശം_putchar(':');
	സന്ദേശം_putchar('!');
	സന്ദേശം_outtrans(newcmd);
	സന്ദേശം_clr_eos();
	windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);

	do_shell(newcmd, 0);
    }
    അല്ലെങ്കിൽ				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
	apply_autocmds(EVENT_SHELLFILTERPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    }

theend:
    എങ്കിൽ (free_newcmd)
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
    സ്ഥിതം ശൂന്യം
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		do_in,
    പൂർണ്ണസംഖ്യ		do_out)
{
    char_u	*itmp = ശൂന്യ;
    char_u	*otmp = ശൂന്യ;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    പൂർണ്ണസംഖ്യ		shell_flags = 0;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    പൂർണ്ണസംഖ്യ		save_cmod_flags = cmdmod.cmod_flags;
#ifdef FEAT_FILTERPIPE
    പൂർണ്ണസംഖ്യ		stmp = p_stmp;
#endif

    എങ്കിൽ (*cmd == NUL)	    // no filter command
	തിരികെ;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_വരി_abv_curs();
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

    എങ്കിൽ (do_out)
	shell_flags |= SHELL_DOOUT;

#ifdef FEAT_FILTERPIPE
# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    എങ്കിൽ (!do_in && do_out && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (do_in && !do_out && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (do_in && do_out && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ ((do_in && (itmp = vim_tempname('i', FALSE)) == ശൂന്യ)
		|| (do_out && (otmp = vim_tempname('o', FALSE)) == ശൂന്യ))
	{
	    emsg(_(e_cant_get_temp_file_name));
	    പോകുക filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    എങ്കിൽ (itmp != ശൂന്യ && buf_write(curbuf, itmp, ശൂന്യ, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	സന്ദേശം_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (!aborting())
#endif
	    // will call wait_return()
	    (ശൂന്യം)semsg(_(e_cant_create_file_str), itmp);
	പോകുക filterend;
    }
    എങ്കിൽ (curbuf != old_curbuf)
	പോകുക filterend;

    എങ്കിൽ (!do_out)
	സന്ദേശം_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    എങ്കിൽ (cmd_buf == ശൂന്യ)
	പോകുക filterend;

    windgoto((പൂർണ്ണസംഖ്യ)Rows - 1, cmdline_col_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    എങ്കിൽ (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	redraw_later_clear();

    എങ്കിൽ (do_out)
    {
	എങ്കിൽ (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    പോകുക error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_വരി_എണ്ണം;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    എങ്കിൽ (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    എങ്കിൽ (do_out)
    {
	എങ്കിൽ (otmp != ശൂന്യ)
	{
	    എങ്കിൽ (readfile(otmp, ശൂന്യ, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#എങ്കിൽ defined(FEAT_EVAL)
		എങ്കിൽ (!aborting())
#endif
		{
		    സന്ദേശം_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		പോകുക error;
	    }
	    എങ്കിൽ (curbuf != old_curbuf)
		പോകുക filterend;
	}

	read_linecount = curbuf->b_ml.ml_വരി_എണ്ണം - read_linecount;

	എങ്കിൽ (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_start.lnum = line2 + 1;
	    curbuf->b_op_end.lnum = curwin->w_cursor.lnum;
	    appended_വരികൾ_mark(line2, read_linecount);
	}

	എങ്കിൽ (do_in)
	{
	    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == ശൂന്യ)
	    {
		എങ്കിൽ (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		അല്ലെങ്കിൽ എങ്കിൽ (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		അല്ലെങ്കിൽ
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
	    del_വരികൾ(linecount, TRUE);
	    എങ്കിൽ (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_start.lnum = curbuf->b_op_end.lnum =
					MIN(line1, curbuf->b_ml.ml_വരി_എണ്ണം);
		curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	    }
	    അല്ലെങ്കിൽ
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
	അല്ലെങ്കിൽ
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	എങ്കിൽ (linecount > p_report)
	{
	    എങ്കിൽ (do_in)
	    {
		vim_snprintf(സന്ദേശം_buf, വലുപ്പം(സന്ദേശം_buf),
				    _("%ld lines filtered"), (long)linecount);
		എങ്കിൽ (സന്ദേശം(സന്ദേശം_buf) && !സന്ദേശം_scroll)
		    // save message to display it after redraw
		    set_keep_സന്ദേശം((char_u *)സന്ദേശം_buf, 0);
	    }
	    അല്ലെങ്കിൽ
		msgmore((long)linecount);
	}
    }
    അല്ലെങ്കിൽ
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    എങ്കിൽ (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_buffer));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

    എങ്കിൽ (itmp != ശൂന്യ)
	mch_remove(itmp);
    എങ്കിൽ (otmp != ശൂന്യ)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    ശൂന്യം
do_shell(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    പൂർണ്ണസംഖ്യ		save_nwr;
#endif
#ifdef MSWIN
    പൂർണ്ണസംഖ്യ		winstart = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    എങ്കിൽ (check_restricted() || check_secure())
    {
	സന്ദേശം_end();
	തിരികെ;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    എങ്കിൽ (cmd != ശൂന്യ)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    എങ്കിൽ (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ശൂന്യ)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    സന്ദേശം_putchar('\r');			// put cursor at start of line
    എങ്കിൽ (!autocmd_busy)
    {
	എങ്കിൽ (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    എങ്കിൽ (!winstart)
#endif
	സന്ദേശം_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    എങ്കിൽ (p_warn && !autocmd_busy && സന്ദേശം_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    എങ്കിൽ (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		എങ്കിൽ (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		സന്ദേശം_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		എങ്കിൽ (!keep_termcap)
		    stoptermcap();
#endif
		നിർത്തുക;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    എങ്കിൽ (!swapping_screen())
	windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
    cursor_on();
    (ശൂന്യം)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    എങ്കിൽ (!swapping_screen())
    {
	സന്ദേശം_row = Rows - 1;
	സന്ദേശം_col = 0;
    }

    എങ്കിൽ (autocmd_busy)
    {
	എങ്കിൽ (സന്ദേശം_silent == 0)
	    redraw_later_clear();
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	എങ്കിൽ (!gui.in_use)
# endif
	{
	    എങ്കിൽ (cmd == ശൂന്യ
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		എങ്കിൽ (സന്ദേശം_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		എങ്കിൽ (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		പൂർണ്ണസംഖ്യ redraw = സന്ദേശം_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		എങ്കിൽ (gui.in_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(term_console ? -1 : redraw); // see below
# അല്ലെങ്കിൽ
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	എങ്കിൽ (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	എങ്കിൽ (skip_redraw)		// ':' hit in wait_return()
	{
	    എങ്കിൽ (സന്ദേശം_silent == 0)
		redraw_later_clear();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    എങ്കിൽ (got_int && സന്ദേശം_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    അല്ലെങ്കിൽ
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
}

#എങ്കിൽ !defined(UNIX)
    സ്ഥിതം char_u *
find_pipe(char_u *cmd)
{
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    inquote = FALSE;

    വേണ്ടി (p = cmd; *p != NUL; ++p)
    {
	എങ്കിൽ (!inquote && *p == '|')
	    തിരികെ p;
	എങ്കിൽ (*p == '"')
	    inquote = !inquote;
	അല്ലെങ്കിൽ എങ്കിൽ (rem_backslash(p))
	    ++p;
    }
    തിരികെ ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ		is_powershell = FALSE;
#ifdef UNIX
    പൂർണ്ണസംഖ്യ		is_fish_shell;
#endif

    char_u *shell_name = get_isolated_shell_name();
    എങ്കിൽ (shell_name == ശൂന്യ)
	തിരികെ ശൂന്യ;

#എങ്കിൽ defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    എങ്കിൽ (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    അല്ലെങ്കിൽ
#endif
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	എങ്കിൽ (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	അല്ലെങ്കിൽ
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    എങ്കിൽ (itmp != ശൂന്യ)
    {
	എങ്കിൽ (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	അല്ലെങ്കിൽ
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    എങ്കിൽ (otmp != ശൂന്യ)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (is_powershell)
    {
	എങ്കിൽ (itmp != ശൂന്യ)
	    vim_snprintf((അക്ഷരം *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	അല്ലെങ്കിൽ
	    vim_snprintf((അക്ഷരം *)buf, len, "& { %s }", cmd);
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	എങ്കിൽ (itmp != ശൂന്യ || otmp != ശൂന്യ)
	{
	    എങ്കിൽ (is_fish_shell)
		vim_snprintf((അക്ഷരം *)buf, len, "begin; %s; end", (അക്ഷരം *)cmd);
	    അല്ലെങ്കിൽ
		vim_snprintf((അക്ഷരം *)buf, len, "(%s)", (അക്ഷരം *)cmd);
	}
	അല്ലെങ്കിൽ
	    STRCPY(buf, cmd);
	എങ്കിൽ (itmp != ശൂന്യ)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#അല്ലെങ്കിൽ
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	എങ്കിൽ (*p_sxe != NUL && *p_sxq == '(')
	{
	    എങ്കിൽ (itmp != ശൂന്യ || otmp != ശൂന്യ)
		vim_snprintf((അക്ഷരം *)buf, len, "(%s)", (അക്ഷരം *)cmd);
	    അല്ലെങ്കിൽ
		STRCPY(buf, cmd);
	    എങ്കിൽ (itmp != ശൂന്യ)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    STRCPY(buf, cmd);
	    എങ്കിൽ (itmp != ശൂന്യ)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		എങ്കിൽ (*p_shq == NUL)
		{
		    p = find_pipe(buf);
		    എങ്കിൽ (p != ശൂന്യ)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		എങ്കിൽ (*p_shq == NUL)
		{
		    p = find_pipe(cmd);
		    എങ്കിൽ (p != ശൂന്യ)
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
    എങ്കിൽ (otmp != ശൂന്യ)
	append_redir(buf, (പൂർണ്ണസംഖ്യ)len, p_srr, otmp);

    തിരികെ buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    ശൂന്യം
append_redir(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*end;

    end = buf + STRLEN(buf);
    // find "%s"
    വേണ്ടി (p = opt; (p = vim_strchr(p, '%')) != ശൂന്യ; ++p)
    {
	എങ്കിൽ (p[1] == 's') // found %s
	    നിർത്തുക;
	എങ്കിൽ (p[1] == '%') // skip %%
	    ++p;
    }
    എങ്കിൽ (p != ശൂന്യ)
    {
#ifdef MSWIN
	*end++ = ' '; // not really needed? Not with sh, ksh or bash
#endif
	vim_snprintf((അക്ഷരം *)end, (size_t)(buflen - (end - buf)),
						  (അക്ഷരം *)opt, (അക്ഷരം *)fname);
    }
    അല്ലെങ്കിൽ
	vim_snprintf((അക്ഷരം *)end, (size_t)(buflen - (end - buf)), " %s %s",
		(അക്ഷരം *)opt, (അക്ഷരം *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    ശൂന്യം
do_fixdel(exarg_T *eap UNUSED)
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != ശൂന്യ
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    ശൂന്യം
print_വരി_no_prefix(
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		use_സംഖ്യ,
    പൂർണ്ണസംഖ്യ		list)
{
    അക്ഷരം	numbuf[30];

    എങ്കിൽ (curwin->w_p_nu || use_സംഖ്യ)
    {
	vim_snprintf(numbuf, വലുപ്പം(numbuf),
				   "%*ld ", സംഖ്യ_width(curwin), (long)lnum);
	സന്ദേശം_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    സന്ദേശം_prt_വരി(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    ശൂന്യം
print_വരി(linenr_T lnum, പൂർണ്ണസംഖ്യ use_സംഖ്യ, പൂർണ്ണസംഖ്യ list)
{
    പൂർണ്ണസംഖ്യ		save_silent = silent_mode;

    // apply :filter /pat/
    എങ്കിൽ (message_filtered(ml_get(lnum)))
	തിരികെ;

    സന്ദേശം_start();
    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()
    print_വരി_no_prefix(lnum, use_സംഖ്യ, list);
    എങ്കിൽ (save_silent)
    {
	സന്ദേശം_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = save_silent;
    }
    info_message = FALSE;
}

    പൂർണ്ണസംഖ്യ
rename_buffer(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    // buffer changed, don't change name now
    എങ്കിൽ (buf != curbuf)
	തിരികെ FAIL;
#ifdef FEAT_EVAL
    എങ്കിൽ (aborting())	    // autocmds may abort script processing
	തിരികെ FAIL;
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
    curbuf->b_ffname = ശൂന്യ;
    curbuf->b_sfname = ശൂന്യ;
    എങ്കിൽ (setfname(curbuf, new_fname, ശൂന്യ, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	തിരികെ FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    എങ്കിൽ (xfname != ശൂന്യ && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	എങ്കിൽ (buf != ശൂന്യ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    തിരികെ OK;
}

/*
 * ":file[!] [fname]".
 */
    ശൂന്യം
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    എങ്കിൽ (eap->addr_എണ്ണം > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_എണ്ണം > 1))
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }

    എങ്കിൽ (*eap->arg != NUL || eap->addr_എണ്ണം == 1)
    {
	എങ്കിൽ (rename_buffer(eap->arg) == FAIL)
	    തിരികെ;
	redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    എങ്കിൽ (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    ശൂന്യം
ex_update(exarg_T *eap)
{
    എങ്കിൽ (curbufIsChanged())
	(ശൂന്യം)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    ശൂന്യം
ex_write(exarg_T *eap)
{
    എങ്കിൽ (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
    }

    എങ്കിൽ (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    അല്ലെങ്കിൽ
	(ശൂന്യം)do_write(eap);
}

#ifdef UNIX
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_writable(char_u *fname)
{
    എങ്കിൽ (mch_nodetype(fname) == കണ്ണി_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	തിരികെ FAIL;
    }
    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ
do_write(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		other;
    char_u	*fname = ശൂന്യ;		// init to shut up gcc
    char_u	*ffname;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    char_u	*free_fname = ശൂന്യ;
#ifdef FEAT_BROWSE
    char_u	*browse_file = ശൂന്യ;
#endif
    buf_T	*alt_buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		name_was_missing;

    എങ്കിൽ (not_writing())		// check 'write' option
	തിരികെ FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    ശൂന്യ, ശൂന്യ, ശൂന്യ, curbuf);
	എങ്കിൽ (browse_file == ശൂന്യ)
	    പോകുക theend;
	ffname = browse_file;
    }
#endif
    എങ്കിൽ (*ffname == NUL)
    {
	എങ്കിൽ (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_argument_required));
	    പോകുക theend;
	}
	other = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	എങ്കിൽ (free_fname != ശൂന്യ)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    എങ്കിൽ (other)
    {
	എങ്കിൽ (vim_strchr(p_cpo, CPO_ALTWRITE) != ശൂന്യ
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	അല്ലെങ്കിൽ
	    alt_buf = buflist_findname(ffname);
	എങ്കിൽ (alt_buf != ശൂന്യ && alt_buf->b_ml.ml_mfp != ശൂന്യ)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    പോകുക theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    എങ്കിൽ (!other && (bt_dontwrite_സന്ദേശം(curbuf) || check_fname() == FAIL
#ifdef UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#endif
		|| check_readonly(&eap->forceit, curbuf)))
	പോകുക theend;

    എങ്കിൽ (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	എങ്കിൽ (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_വരി_എണ്ണം)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    എങ്കിൽ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		എങ്കിൽ (vim_dialog_yesno(VIM_QUESTION, ശൂന്യ,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    പോകുക theend;
		eap->forceit = TRUE;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		emsg(_(e_use_bang_to_write_partial_buffer));
		പോകുക theend;
	    }
	}
    }

    എങ്കിൽ (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	എങ്കിൽ (eap->cmdidx == CMD_saveas && alt_buf != ശൂന്യ)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, ശൂന്യ, ശൂന്യ, FALSE, alt_buf);
#ifdef FEAT_EVAL
	    എങ്കിൽ (curbuf != was_curbuf || aborting())
#അല്ലെങ്കിൽ
	    എങ്കിൽ (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		പോകുക theend;
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

	    apply_autocmds(EVENT_BUFFILEPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, ശൂന്യ, ശൂന്യ, FALSE, alt_buf);
	    എങ്കിൽ (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, ശൂന്യ, ശൂന്യ, FALSE, alt_buf);
	    }
#ifdef FEAT_EVAL
	    എങ്കിൽ (curbuf != was_curbuf || aborting())
#അല്ലെങ്കിൽ
	    എങ്കിൽ (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		പോകുക theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    എങ്കിൽ (*curbuf->b_p_ft == NUL)
	    {
		എങ്കിൽ (au_has_group((char_u *)"filetypedetect"))
		    (ശൂന്യം)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, ശൂന്യ);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == ശൂന്യ;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	എങ്കിൽ (eap->cmdidx == CMD_saveas)
	{
	    എങ്കിൽ (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	എങ്കിൽ (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    തിരികെ retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    പൂർണ്ണസംഖ്യ
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    പൂർണ്ണസംഖ്യ		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    എങ്കിൽ (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == ശൂന്യ)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	എങ്കിൽ (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    എങ്കിൽ (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		തിരികെ FAIL;
	    }
#endif
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    എങ്കിൽ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_സന്ദേശം_SIZE];

		dialog_സന്ദേശം(buff, _("Overwrite existing file \"%s\"?"), fname);
		എങ്കിൽ (vim_dialog_yesno(VIM_QUESTION, ശൂന്യ, buff, 2) != VIM_YES)
		    തിരികെ FAIL;
		eap->forceit = TRUE;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		emsg(_(e_file_exists));
		തിരികെ FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	എങ്കിൽ (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    പൂർണ്ണസംഖ്യ		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    എങ്കിൽ (*p_dir == NUL)
	    {
		dir = alloc(5);
		എങ്കിൽ (dir == ശൂന്യ)
		    തിരികെ FAIL;
		STRCPY(dir, ".");
	    }
	    അല്ലെങ്കിൽ
	    {
		dir = alloc(MAXPATHL);
		എങ്കിൽ (dir == ശൂന്യ)
		    തിരികെ FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    എങ്കിൽ (r)
	    {
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		എങ്കിൽ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_സന്ദേശം_SIZE];

		    dialog_സന്ദേശം(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    എങ്കിൽ (vim_dialog_yesno(VIM_QUESTION, ശൂന്യ, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			തിരികെ FAIL;
		    }
		    eap->forceit = TRUE;
		}
		അല്ലെങ്കിൽ
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    തിരികെ FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    തിരികെ OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    ശൂന്യം
ex_wnext(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (പൂർണ്ണസംഖ്യ)eap->line2;
    അല്ലെങ്കിൽ
	i = curwin->w_arg_idx - (പൂർണ്ണസംഖ്യ)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
    എങ്കിൽ (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    ശൂന്യം
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		error = 0;
    പൂർണ്ണസംഖ്യ		save_forceit = eap->forceit;
    പൂർണ്ണസംഖ്യ		save_exiting = exiting;

    എങ്കിൽ (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	എങ്കിൽ (before_quit_all(eap) == FAIL)
	    തിരികെ;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ifdef FEAT_TERMINAL
	എങ്കിൽ (exiting && !eap->forceit && term_job_running(buf->b_term))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);
	    എങ്കിൽ (term_try_stop_job(buf) == FAIL)
	    {
		no_write_message_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    അല്ലെങ്കിൽ എങ്കിൽ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (bufIsChanged(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    എങ്കിൽ (not_writing())
	    {
		++error;
		നിർത്തുക;
	    }
#ifdef FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    എങ്കിൽ (buf->b_ffname == ശൂന്യ && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#endif
	    എങ്കിൽ (buf->b_ffname == ശൂന്യ)
	    {
		semsg(_(e_no_file_name_for_buffer_nr), (long)buf->b_fnum);
		++error;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    അല്ലെങ്കിൽ
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		എങ്കിൽ (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		എങ്കിൽ (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    എങ്കിൽ (exiting)
    {
	എങ്കിൽ (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
not_writing(ശൂന്യം)
{
    എങ്കിൽ (p_write)
	തിരികെ FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    തിരികെ TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_readonly(പൂർണ്ണസംഖ്യ *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    എങ്കിൽ (!*forceit && (buf->b_p_ro
		|| (mch_stat((അക്ഷരം *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	എങ്കിൽ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != ശൂന്യ)
	{
	    char_u	buff[DIALOG_സന്ദേശം_SIZE];

	    എങ്കിൽ (buf->b_p_ro)
		dialog_സന്ദേശം(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    അല്ലെങ്കിൽ
		dialog_സന്ദേശം(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    എങ്കിൽ (vim_dialog_yesno(VIM_QUESTION, ശൂന്യ, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		തിരികെ FALSE;
	    }
	    അല്ലെങ്കിൽ
		തിരികെ TRUE;
	}
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_set_add_bang_to_override));
	അല്ലെങ്കിൽ
	    semsg(_(e_str_is_read_only_add_bang_to_override), buf->b_fname);
	തിരികെ TRUE;
    }

    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
getfile(
    പൂർണ്ണസംഖ്യ		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    പൂർണ്ണസംഖ്യ		setpm,
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    പൂർണ്ണസംഖ്യ		other;
    പൂർണ്ണസംഖ്യ		retval;
    char_u	*free_me = ശൂന്യ;

    എങ്കിൽ (!check_can_set_curbuf_forceit(forceit))
	തിരികെ GETFILE_ERROR;

    എങ്കിൽ (text_locked())
	തിരികെ GETFILE_ERROR;
    എങ്കിൽ (curbuf_locked())
	തിരികെ GETFILE_ERROR;

    എങ്കിൽ (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    അല്ലെങ്കിൽ
	other = (fnum != curbuf->b_fnum);

    എങ്കിൽ (other)
	++no_wait_return;	    // don't wait for autowrite message
    എങ്കിൽ (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	എങ്കിൽ (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	എങ്കിൽ (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    പോകുക theend;
	}
    }
    എങ്കിൽ (other)
	--no_wait_return;
    എങ്കിൽ (setpm)
	setpcmark();
    എങ്കിൽ (!other)
    {
	എങ്കിൽ (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    അല്ലെങ്കിൽ എങ്കിൽ (do_ecmd(fnum, ffname, sfname, ശൂന്യ, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    അല്ലെങ്കിൽ
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    തിരികെ retval;
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
    പൂർണ്ണസംഖ്യ
do_ecmd(
    പൂർണ്ണസംഖ്യ		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    പൂർണ്ണസംഖ്യ		flags,
    win_T	*oldwin)
{
    പൂർണ്ണസംഖ്യ		other_file;		// TRUE if editing another file
    പൂർണ്ണസംഖ്യ		oldbuf;			// TRUE if using existing buffer
    പൂർണ്ണസംഖ്യ		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = ശൂന്യ;
#എങ്കിൽ defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = ശൂന്യ;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = ശൂന്യ;
#endif
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    പൂർണ്ണസംഖ്യ		newcol = -1;
    പൂർണ്ണസംഖ്യ		solcol = -1;
    pos_T	*pos;
    char_u	*command = ശൂന്യ;
#ifdef FEAT_SPELL
    പൂർണ്ണസംഖ്യ		did_get_winopts = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		readfile_flags = 0;
    പൂർണ്ണസംഖ്യ		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (ERROR_IF_TERM_POPUP_WINDOW)
	തിരികെ FAIL;
#endif

    എങ്കിൽ (eap != ശൂന്യ)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    എങ്കിൽ (fnum != 0)
    {
	എങ്കിൽ (fnum == curbuf->b_fnum)	// file is already being edited
	    തിരികെ OK;			// nothing to do
	other_file = TRUE;
    }
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_BROWSE
	എങ്കിൽ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    എങ്കിൽ (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		എങ്കിൽ (ffname == ശൂന്യ || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    അല്ലെങ്കിൽ
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    ശൂന്യ, ശൂന്യ, ശൂന്യ, curbuf);
		എങ്കിൽ (browse_file == ശൂന്യ)
		    പോകുക theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	എങ്കിൽ (sfname == ശൂന്യ)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	എങ്കിൽ (sfname != ശൂന്യ)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	എങ്കിൽ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == ശൂന്യ || *ffname == NUL))
	    പോകുക theend;

	എങ്കിൽ (ffname == ശൂന്യ)
	    other_file = TRUE;
					    // there is no file name
	അല്ലെങ്കിൽ എങ്കിൽ (*ffname == NUL && curbuf->b_ffname == ശൂന്യ)
	    other_file = FALSE;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    എങ്കിൽ (free_fname != ശൂന്യ)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    എങ്കിൽ (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == ശൂന്യ ? 0 : CCGD_EXCMD)))
    {
	എങ്കിൽ (fnum == 0 && other_file && ffname != ശൂന്യ)
	    (ശൂന്യം)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	പോകുക theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    എങ്കിൽ (oldwin != ശൂന്യ && !win_valid(oldwin))
	oldwin = ശൂന്യ;

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ ((command != ശൂന്യ || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	എങ്കിൽ (command != ശൂന്യ)
	    valsize = (പൂർണ്ണസംഖ്യ)STRLEN(command) + 3;
	അല്ലെങ്കിൽ
	    valsize = 30;
	val.string = alloc(valsize);
	എങ്കിൽ (val.string != ശൂന്യ)
	{
	    എങ്കിൽ (command != ശൂന്യ)
		val.length = vim_snprintf_safelen((അക്ഷരം *)val.string,
		    valsize, ":%s\r", command);
	    അല്ലെങ്കിൽ
		val.length = vim_snprintf_safelen((അക്ഷരം *)val.string,
		    valsize, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, val.string, (പൂർണ്ണസംഖ്യ)val.length);
	    did_set_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    എങ്കിൽ (other_file)
    {
	പൂർണ്ണസംഖ്യ prev_alt_fnum = curwin->w_alt_fnum;

	എങ്കിൽ (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    എങ്കിൽ (oldwin != ശൂന്യ)
		buflist_altfpos(oldwin);
	}

	എങ്കിൽ (fnum)
	    buf = buflist_findnr(fnum);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		എങ്കിൽ (command != ശൂന്യ)
		{
		    tlnum = atol((അക്ഷരം *)command);
		    എങ്കിൽ (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		എങ്കിൽ (newbuf != ശൂന്യ)
		{
		    എങ്കിൽ (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    എങ്കിൽ (tlnum > 0)
			newbuf->b_last_cursor.lnum = tlnum;
		}
		പോകുക theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    എങ്കിൽ (oldwin != ശൂന്യ)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	എങ്കിൽ (buf == ശൂന്യ)
	    പോകുക theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	എങ്കിൽ (buf->b_locked_split)
	{
	    emsg(_(e_cannot_switch_to_a_closing_buffer));
	    പോകുക theend;
	}
	എങ്കിൽ (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	അല്ലെങ്കിൽ					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (ശൂന്യം)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    എങ്കിൽ (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		പോകുക theend;
#ifdef FEAT_EVAL
	    എങ്കിൽ (aborting())	    // autocmds may abort script processing
		പോകുക theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	എങ്കിൽ ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
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
	എങ്കിൽ (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    പൂർണ്ണസംഖ്യ		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    പ്രതിജ്ഞാ(cmdwin_buf == ശൂന്യ);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = ശൂന്യ;

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
	    എങ്കിൽ (buf->b_fname != ശൂന്യ)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    എങ്കിൽ (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_സന്ദേശം(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		പോകുക theend;
	    }
#ifdef FEAT_EVAL
	    എങ്കിൽ (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		പോകുക theend;
	    }
#endif
	    എങ്കിൽ (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    അല്ലെങ്കിൽ
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		എങ്കിൽ (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_buffer(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != ശൂന്യ);

		// Autocommands may have closed the window.
		എങ്കിൽ (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		എങ്കിൽ (aborting() && curwin->w_buffer != ശൂന്യ)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    പോകുക theend;
		}
#endif
		// Be careful again, like above.
		എങ്കിൽ (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    എങ്കിൽ (curwin->w_buffer == ശൂന്യ)
			buf = lastbuf;
		    അല്ലെങ്കിൽ
		    {
			delbuf_സന്ദേശം(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			പോകുക theend;
		    }
		}
		എങ്കിൽ (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		അല്ലെങ്കിൽ
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    എങ്കിൽ (curwin->w_buffer == ശൂന്യ
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    എങ്കിൽ (curwin->w_buffer != ശൂന്യ)
			--curwin->w_buffer->b_nwindows;

		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    എങ്കിൽ (!oldbuf && eap != ശൂന്യ)
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
    അല്ലെങ്കിൽ // !other_file
    {
	എങ്കിൽ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    പോകുക theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    എങ്കിൽ ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    അല്ലെങ്കിൽ
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	എങ്കിൽ (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    എങ്കിൽ (buf != curbuf)
	പോകുക theend;
#ifdef FEAT_EVAL
    എങ്കിൽ (aborting())	    // autocmds may abort script processing
	പോകുക theend;
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
    എങ്കിൽ (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	എങ്കിൽ (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	എങ്കിൽ (buf->b_fname != ശൂന്യ)
	    new_name = vim_strsave(buf->b_fname);
	അല്ലെങ്കിൽ
	    new_name = ശൂന്യ;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	എങ്കിൽ (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_വരി_എണ്ണം <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    എങ്കിൽ (u_savecommon(0, curbuf->b_ml.ml_വരി_എണ്ണം + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		പോകുക theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	അല്ലെങ്കിൽ
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	എങ്കിൽ (!bufref_valid(&bufref))
	{
	    delbuf_സന്ദേശം(new_name);	// frees new_name
	    പോകുക theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	എങ്കിൽ (buf != curbuf)
	    പോകുക theend;
#ifdef FEAT_EVAL
	എങ്കിൽ (aborting())	    // autocmds may abort script processing
	    പോകുക theend;
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
    എങ്കിൽ (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    എങ്കിൽ (!auto_buf)
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
		എങ്കിൽ (win->w_buffer == curbuf)
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
	എങ്കിൽ (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    എങ്കിൽ (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    എങ്കിൽ (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#എങ്കിൽ defined(FEAT_EVAL)
	    എങ്കിൽ (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#അല്ലെങ്കിൽ
	    (ശൂന്യം)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    എങ്കിൽ (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	അല്ലെങ്കിൽ
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE,
							      curbuf, &retval);
	    എങ്കിൽ ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, ശൂന്യ, ശൂന്യ, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	എങ്കിൽ (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    എങ്കിൽ (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (പൂർണ്ണസംഖ്യ)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	എങ്കിൽ (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_വരി_abv_curs();

	maketitle();
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	എങ്കിൽ (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    എങ്കിൽ (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    എങ്കിൽ (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ശൂന്യം)parse_spelllang(curwin);
#endif

    എങ്കിൽ (command == ശൂന്യ)
    {
	എങ്കിൽ (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    എങ്കിൽ (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = true;
	    }
	    അല്ലെങ്കിൽ
		beginline(BL_SOL | BL_FIX);
	}
	അല്ലെങ്കിൽ			// no line number, go to last line in Ex mode
	{
	    എങ്കിൽ (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    എങ്കിൽ (oldbuf && !auto_buf)
    {
	പൂർണ്ണസംഖ്യ	സന്ദേശം_scroll_save = സന്ദേശം_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	എങ്കിൽ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    സന്ദേശം_scroll = FALSE;
	എങ്കിൽ (!സന്ദേശം_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	സന്ദേശം_start();
	സന്ദേശം_scroll = സന്ദേശം_scroll_save;
	സന്ദേശം_scrolled_ign = TRUE;

	എങ്കിൽ (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	സന്ദേശം_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    എങ്കിൽ (command != ശൂന്യ)
	do_cmdline(command, ശൂന്യ, ശൂന്യ, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    എങ്കിൽ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ശൂന്യം)keymap_init();
#endif

    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    എങ്കിൽ (!skip_redraw)
    {
	n = *so_ptr;
	എങ്കിൽ (topline == 0 && command == ശൂന്യ)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    എങ്കിൽ (p_im && (State & MODE_തിരുകുക) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    എങ്കിൽ (p_acd && curbuf->b_ffname != ശൂന്യ)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	എങ്കിൽ (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#എങ്കിൽ defined(FEAT_NETBEANS_INTG)
    എങ്കിൽ (curbuf->b_ffname != ശൂന്യ)
    {
# ifdef FEAT_NETBEANS_INTG
	എങ്കിൽ ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    എങ്കിൽ (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, ശൂന്യ, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    തിരികെ retval;
}

    സ്ഥിതം ശൂന്യം
delbuf_സന്ദേശം(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_buffer_str),
					   name == ശൂന്യ ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = ശൂന്യ;
    au_new_curbuf.br_buf_free_എണ്ണം = 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    സ്ഥിതം char_u *
get_append_വരി(exarg_T *eap, പൂർണ്ണസംഖ്യ indent)
{
    char_u	*theline;
    char_u	*p;

    എങ്കിൽ (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->ea_getline == ശൂന്യ)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	എങ്കിൽ (eap->nextcmd == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	p = vim_strchr(eap->nextcmd, NL);
	എങ്കിൽ (p == ശൂന്യ)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	എങ്കിൽ (*p != NUL)
	    ++p;
	അല്ലെങ്കിൽ
	    p = ശൂന്യ;
	eap->nextcmd = p;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ save_State = State;

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
    തിരികെ theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    ശൂന്യം
ex_append(exarg_T *eap)
{
    char_u	*theline;
    പൂർണ്ണസംഖ്യ		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    പൂർണ്ണസംഖ്യ		indent = 0;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		vcol;
    പൂർണ്ണസംഖ്യ		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    എങ്കിൽ (not_in_vim9(eap, eap->skip) == FAIL)
	തിരികെ;
#endif
    എങ്കിൽ (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	ആകുമ്പോൾ ((theline = get_append_വരി(eap, 0)) != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ end = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    എങ്കിൽ (end)
		നിർത്തുക;
	}
	തിരികെ;
    }

    // the ! flag toggles autoindent
    എങ്കിൽ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    എങ്കിൽ (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    എങ്കിൽ (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    എങ്കിൽ (empty && lnum == 1)
	lnum = 0;

    State = MODE_തിരുകുക;		    // behave like in Insert mode
    എങ്കിൽ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    വേണ്ടി (;;)
    {
	സന്ദേശം_scroll = TRUE;
	need_wait_return = FALSE;
	എങ്കിൽ (curbuf->b_p_ai)
	{
	    എങ്കിൽ (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_വരി(eap, indent);
	വരികൾ_ഇടത് = Rows - 1;
	എങ്കിൽ (theline == ശൂന്യ)
	    നിർത്തുക;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	എങ്കിൽ (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	വേണ്ടി (p = theline; indent > vcol; ++p)
	{
	    എങ്കിൽ (*p == ' ')
		++vcol;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == TAB)
		vcol += 8 - vcol % 8;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}
	എങ്കിൽ ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    നിർത്തുക;
	}

	// don't use autoindent if nothing was typed.
	എങ്കിൽ (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	എങ്കിൽ (empty)
	    // there are no marks below the inserted lines
	    appended_വരികൾ(lnum, 1L);
	അല്ലെങ്കിൽ
	    appended_വരികൾ_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	എങ്കിൽ (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    എങ്കിൽ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_വരി_എണ്ണം) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_വരി_എണ്ണം;
	എങ്കിൽ (eap->cmdidx != CMD_append)
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
    ശൂന്യം
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ifdef FEAT_EVAL
    എങ്കിൽ (not_in_vim9(eap, eap->skip) == FAIL)
	തിരികെ;
#endif
    എങ്കിൽ (eap->skip)
    {
	ex_append(eap);
	തിരികെ;
    }
    എങ്കിൽ (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	തിരികെ;

    // the ! flag toggles autoindent
    എങ്കിൽ (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    വേണ്ടി (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    നിർത്തുക;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_വരികൾ_mark(eap->line1, (long)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    ശൂന്യം
ex_z(exarg_T *eap)
{
    char_u	*x;
    long	bigness;
    char_u	*kind;
    പൂർണ്ണസംഖ്യ		minus = 0;
    linenr_T	start, end, curs, i;
    പൂർണ്ണസംഖ്യ		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    എങ്കിൽ (eap->forceit)
	bigness = Rows - 1;
    അല്ലെങ്കിൽ എങ്കിൽ (!ONE_WINDOW)
	bigness = curwin->w_height - 3;
    അല്ലെങ്കിൽ
	bigness = curwin->w_p_scr * 2;
    എങ്കിൽ (bigness < 1)
	bigness = 1;

    x = eap->arg;
    kind = x;
    എങ്കിൽ (*kind == '-' || *kind == '+' || *kind == '='
					      || *kind == '^' || *kind == '.')
	++x;
    ആകുമ്പോൾ (*x == '-' || *x == '+')
	++x;

    എങ്കിൽ (*x != 0)
    {
	എങ്കിൽ (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_argument_to_z));
	    തിരികെ;
	}
	അല്ലെങ്കിൽ
	{
	    bigness = atol((അക്ഷരം *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    എങ്കിൽ (bigness > 2 * curbuf->b_ml.ml_വരി_എണ്ണം || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_വരി_എണ്ണം;

	    p_window = bigness;
	    എങ്കിൽ (*kind == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    എങ്കിൽ (*kind == '-' || *kind == '+')
	വേണ്ടി (x = kind + 1; *x == *kind; ++x)
	    ;

    തിരഞ്ഞെടുപ്പ് (*kind)
    {
	അവസ്ഥ '-':
	    start = lnum - bigness * (linenr_T)(x - kind) + 1;
	    end = start + bigness - 1;
	    curs = end;
	    നിർത്തുക;

	അവസ്ഥ '=':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    നിർത്തുക;

	അവസ്ഥ '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    നിർത്തുക;

	അവസ്ഥ '.':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = end;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:  // '+'
	    start = lnum;
	    എങ്കിൽ (*kind == '+')
		start += bigness * (linenr_T)(x - kind - 1) + 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (eap->addr_എണ്ണം == 0)
		++start;
	    end = start + bigness - 1;
	    curs = end;
	    നിർത്തുക;
    }

    എങ്കിൽ (start < 1)
	start = 1;

    എങ്കിൽ (end > curbuf->b_ml.ml_വരി_എണ്ണം)
	end = curbuf->b_ml.ml_വരി_എണ്ണം;

    എങ്കിൽ (curs > curbuf->b_ml.ml_വരി_എണ്ണം)
	curs = curbuf->b_ml.ml_വരി_എണ്ണം;
    അല്ലെങ്കിൽ എങ്കിൽ (curs < 1)
	curs = 1;

    വേണ്ടി (i = start; i <= end; i++)
    {
	എങ്കിൽ (minus && i == lnum)
	{
	    സന്ദേശം_putchar('\n');

	    വേണ്ടി (j = 1; j < Columns; j++)
		സന്ദേശം_putchar('-');
	}

	print_വരി(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	എങ്കിൽ (minus && i == lnum)
	{
	    സന്ദേശം_putchar('\n');

	    വേണ്ടി (j = 1; j < Columns; j++)
		സന്ദേശം_putchar('-');
	}
    }

    എങ്കിൽ (curwin->w_cursor.lnum != curs)
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
    പൂർണ്ണസംഖ്യ
check_restricted(ശൂന്യം)
{
    എങ്കിൽ (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_in_rvim));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    പൂർണ്ണസംഖ്യ
check_secure(ശൂന്യം)
{
    എങ്കിൽ (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_തിരയൽ));
	തിരികെ TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    എങ്കിൽ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	തിരികെ TRUE;
    }
#endif
    തിരികെ FALSE;
}

സ്ഥിതം char_u	*old_sub = ശൂന്യ;	// previous substitute pattern
സ്ഥിതം പൂർണ്ണസംഖ്യ	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ	do_all;		// do multiple substitutions per line
    പൂർണ്ണസംഖ്യ	do_ask;		// ask for confirmation
    പൂർണ്ണസംഖ്യ	do_എണ്ണം;	// count only
    പൂർണ്ണസംഖ്യ	do_error;	// if false, ignore errors
    പൂർണ്ണസംഖ്യ	do_print;	// print last line with subs.
    പൂർണ്ണസംഖ്യ	do_list;	// list last line with subs.
    പൂർണ്ണസംഖ്യ	do_സംഖ്യ;	// list last line with line nr
    പൂർണ്ണസംഖ്യ	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *start, പൂർണ്ണസംഖ്യ delimiter)
{
    char_u *p = start;

    ആകുമ്പോൾ (p[0])
    {
	എങ്കിൽ (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    നിർത്തുക;
	}
	എങ്കിൽ (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    തിരികെ p;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_regexp_delim(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (SAFE_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    ശൂന്യം
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    long	i = 0;
    regmmatch_T regmatch;
    സ്ഥിതം subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ifdef FEAT_EVAL
    subflags_T	subflags_save;
#endif
    പൂർണ്ണസംഖ്യ		save_do_all;		// remember user specified 'g' flag
    പൂർണ്ണസംഖ്യ		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = ശൂന്യ;		// init for GCC
    string_T	pat = {ശൂന്യ, 0};
    പൂർണ്ണസംഖ്യ		delimiter;
    size_t	sublen;
    പൂർണ്ണസംഖ്യ		got_quit = FALSE;
    പൂർണ്ണസംഖ്യ		got_match = FALSE;
    പൂർണ്ണസംഖ്യ		which_pat;
    char_u	*cmd;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		save_State;
    linenr_T	first_വരി = 0;		// first changed line
    linenr_T	last_വരി = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;
    linenr_T	line2;
    long	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    പൂർണ്ണസംഖ്യ		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    പൂർണ്ണസംഖ്യ		start_nsubs;
    പൂർണ്ണസംഖ്യ		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		save_ma = 0;
    പൂർണ്ണസംഖ്യ		save_sandbox = 0;
#endif
#ifdef FEAT_PROP_POPUP
    textprop_T	*text_props = ശൂന്യ;
#endif

    cmd = eap->arg;
    എങ്കിൽ (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    start_nsubs = sub_nsubs;

    എങ്കിൽ (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	// use last used regexp
    അല്ലെങ്കിൽ
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    എങ്കിൽ (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == ശൂന്യ)
    {
	// don't accept alphanumeric for separator
	എങ്കിൽ (check_regexp_delim(*cmd) == FAIL)
	    തിരികെ;
#ifdef FEAT_EVAL
	എങ്കിൽ (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    തിരികെ;
#endif

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	എങ്കിൽ (*cmd == '\\')
	{
	    എങ്കിൽ (in_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_in_vim9_script));
		തിരികെ;
	    }
	    ++cmd;
	    എങ്കിൽ (vim_strchr((char_u *)"/?&", *cmd) == ശൂന്യ)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		തിരികെ;
	    }
	    എങ്കിൽ (*cmd != '&')
		which_pat = RE_തിരയൽ;	    // use last '/' pattern
	    STR_LITERAL_SET(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	അല്ലെങ്കിൽ		// find the end of the regexp
	{
	    which_pat = RE_LAST;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, ശൂന്യ, ശൂന്യ);
	    pat.length = (size_t)(cmd - pat.string);
	    എങ്കിൽ (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	എങ്കിൽ (sub == ശൂന്യ)
	    // out of memory
	    തിരികെ;

	എങ്കിൽ (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    എങ്കിൽ (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != ശൂന്യ)
	    {
		എങ്കിൽ (old_sub == ശൂന്യ)	// there is no previous command
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    vim_free(sub);
		    തിരികെ;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		എങ്കിൽ (sub == ശൂന്യ)
		    // out of memory
		    തിരികെ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		എങ്കിൽ (old_sub == ശൂന്യ)
		{
		    // out of memory
		    vim_free(sub);
		    തിരികെ;
		}
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!eap->skip)	// use previous pattern and substitution
    {
	എങ്കിൽ (old_sub == ശൂന്യ)	// there is no previous command
	{
	    emsg(_(e_no_previous_substitute_regular_expression));
	    തിരികെ;
	}
	pat.string = ശൂന്യ;		// search_regcomp() will use previous pattern
	pat.length = 0;
	sub = vim_strsave(old_sub);
	എങ്കിൽ (sub == ശൂന്യ)
	    തിരികെ;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    എങ്കിൽ (pat.string != ശൂന്യ && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_വരികൾ_എണ്ണം;

	എങ്കിൽ (eap->skip)
	{
	    vim_free(sub);
	    തിരികെ;
	}
	curwin->w_cursor.lnum = eap->line1;
	എങ്കിൽ (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_വരികൾ_എണ്ണം = eap->line2 - eap->line1 + 1;
	എങ്കിൽ (eap->line2 < curbuf->b_ml.ml_വരി_എണ്ണം)
	    ++joined_വരികൾ_എണ്ണം;
	എങ്കിൽ (joined_വരികൾ_എണ്ണം > 1)
	{
	    (ശൂന്യം)do_join(joined_വരികൾ_എണ്ണം, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_വരികൾ_എണ്ണം - 1;
	    sub_nlines = 1;
	    (ശൂന്യം)do_sub_സന്ദേശം(FALSE);
	    ex_may_print(eap);
	}

	എങ്കിൽ (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.length, magic_isset());
	// put pattern in history
	add_to_history(HIST_തിരയൽ, pat.string, pat.length, TRUE, NUL);
	vim_free(sub);

	തിരികെ;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    എങ്കിൽ (*cmd == '&')
	++cmd;
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_EVAL
	എങ്കിൽ (in_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (!p_ed)
	{
	    എങ്കിൽ (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    അല്ലെങ്കിൽ
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_എണ്ണം = FALSE;
	subflags.do_സംഖ്യ = FALSE;
	subflags.do_ic = 0;
    }
    ആകുമ്പോൾ (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	എങ്കിൽ (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'n')
	    subflags.do_എണ്ണം = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_LAST;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'p')
	    subflags.do_print = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_സംഖ്യ = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	അല്ലെങ്കിൽ
	    നിർത്തുക;
	++cmd;
    }
    എങ്കിൽ (subflags.do_എണ്ണം)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    എങ്കിൽ (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	എങ്കിൽ (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_positive_എണ്ണം_required));
	    vim_free(sub);
	    തിരികെ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (i >= INT_MAX)
	{
	    അക്ഷരം	buf[20];
	    vim_snprintf(buf, വലുപ്പം(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    തിരികെ;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	എങ്കിൽ (eap->line2 > curbuf->b_ml.ml_വരി_എണ്ണം)
	    eap->line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    എങ്കിൽ (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	set_nextcmd(eap, cmd);
	എങ്കിൽ (eap->nextcmd == ശൂന്യ)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    തിരികെ;
	}
    }

    എങ്കിൽ (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	തിരികെ;
    }

    എങ്കിൽ (!subflags.do_എണ്ണം && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	തിരികെ;
    }

    എങ്കിൽ (തിരയൽ_regcomp(pat.string, pat.length, ശൂന്യ, RE_SUBST, which_pat, തിരയൽ_HIS, &regmatch) == FAIL)
    {
	എങ്കിൽ (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	തിരികെ;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    എങ്കിൽ (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = ശൂന്യ;
    sub_firstline.length = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    എങ്കിൽ (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ;
	sub = p;
    }
    അല്ലെങ്കിൽ
    {
	p = regtilde(sub, magic_isset());

	എങ്കിൽ (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    വേണ്ടി (lnum = eap->line1; lnum <= line2 && !(got_quit
#എങ്കിൽ defined(FEAT_EVAL)
		|| aborting()
#endif
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ശൂന്യ);
	എങ്കിൽ (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_start = {ശൂന്യ, 0};
	    size_t	new_start_size = 0;
	    char_u	*new_end;
	    പൂർണ്ണസംഖ്യ		did_sub = FALSE;
	    പൂർണ്ണസംഖ്യ		lastone;
	    size_t	copy_len;
	    size_t	needed_size;
	    long	nmatch_tl = 0;	// nr of lines matched below lnum
	    പൂർണ്ണസംഖ്യ		do_again;	// do it again after joining lines
	    പൂർണ്ണസംഖ്യ		skip_match = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_split = false;	// "\r" split the line
#ifdef FEAT_PROP_POPUP
	    പൂർണ്ണസംഖ്യ		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	total_added =  0;
	    പൂർണ്ണസംഖ്യ		text_prop_എണ്ണം = 0;
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
	    എങ്കിൽ (!got_match)
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
	    വേണ്ടി (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		എങ്കിൽ (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
		    നിർത്തുക;

		എങ്കിൽ (sub_firstline.string == ശൂന്യ)
		{
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    എങ്കിൽ (sub_firstline.string == ശൂന്യ)
		    {
			vim_free(new_start.string);
			പോകുക outofmem;
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
		എങ്കിൽ (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    എങ്കിൽ (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_match = TRUE;
		    അല്ലെങ്കിൽ
		    {
			 // search for a match at next column
			എങ്കിൽ (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			അല്ലെങ്കിൽ
			    ++matchcol;
		    }
		    പോകുക skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		എങ്കിൽ (subflags.do_എണ്ണം)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    എങ്കിൽ (nmatch > 1)
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
		    എങ്കിൽ (!(sub[0] == '\\' && sub[1] == '='))
#endif
			പോകുക skip;
		}

		എങ്കിൽ (subflags.do_ask)
		{
		    പൂർണ്ണസംഖ്യ typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = MODE_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.col = regmatch.startpos[0].col;
		    എങ്കിൽ (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    എങ്കിൽ (vim_strchr(p_cpo, CPO_UNDO) != ശൂന്യ)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    ആകുമ്പോൾ (subflags.do_ask)
		    {
			എങ്കിൽ (exmode_active)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_വരി_no_prefix(lnum,
					 subflags.do_സംഖ്യ, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, ശൂന്യ, ശൂന്യ, 0);
			    curwin->w_cursor.col = regmatch.endpos[0].col - 1;
			    എങ്കിൽ (curwin->w_cursor.col < 0)
				curwin->w_cursor.col = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   ശൂന്യ, ശൂന്യ, &ec, 0);
			    curwin->w_cursor.col = regmatch.startpos[0].col;
			    എങ്കിൽ (subflags.do_സംഖ്യ || curwin->w_p_nu)
			    {
				പൂർണ്ണസംഖ്യ numw = സംഖ്യ_width(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    സന്ദേശം_start();
			    വേണ്ടി (i = 0; i < (long)sc; ++i)
				സന്ദേശം_putchar(' ');
			    വേണ്ടി ( ; i <= (long)ec; ++i)
				സന്ദേശം_putchar('^');

			    resp = getexmodeline('?', ശൂന്യ, 0, TRUE);
			    എങ്കിൽ (resp != ശൂന്യ)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				എങ്കിൽ (ex_normal_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			അല്ലെങ്കിൽ
			{
			    string_T orig_വരി = {ശൂന്യ, 0};
			    പൂർണ്ണസംഖ്യ    len_change = 0;
			    പൂർണ്ണസംഖ്യ	   save_p_lz = p_lz;
#ifdef FEAT_FOLDING
			    പൂർണ്ണസംഖ്യ save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#endif
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    എങ്കിൽ (new_start.string != ശൂന്യ)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_വരി.length = ml_get_len(lnum);
				orig_വരി.string = vim_strnsave(ml_get(lnum), orig_വരി.length);
				എങ്കിൽ (orig_വരി.string != ശൂന്യ)
				{
				    string_T new_വരി;

				    new_വരി.string = concat_str(new_start.string,
						     sub_firstline.string + copycol);
				    എങ്കിൽ (new_വരി.string == ശൂന്യ)
					VIM_CLEAR_STRING(orig_വരി);
				    അല്ലെങ്കിൽ
				    {
					new_വരി.length =
					    new_start.length + (sub_firstline.length - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (പൂർണ്ണസംഖ്യ)new_വരി.length - (പൂർണ്ണസംഖ്യ)orig_വരി.length;
					curwin->w_cursor.col += len_change;
					ml_replace(lnum, new_വരി.string, FALSE);
				    }
				}
			    }

			    തിരയൽ_match_വരികൾ = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    തിരയൽ_match_endcol = regmatch.endpos[0].col
								 + len_change;
			    എങ്കിൽ (തിരയൽ_match_വരികൾ == 0
						   && തിരയൽ_match_endcol == 0)
				// highlight at least one character for /^/
				തിരയൽ_match_endcol = 1;
			    highlight_match = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_match = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ifdef FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#endif
			    എങ്കിൽ (സന്ദേശം_row == Rows - 1)
				സന്ദേശം_didout = FALSE;	// avoid a scroll-up
			    സന്ദേശം_starthere();
			    i = സന്ദേശം_scroll;
			    സന്ദേശം_scroll = 0;		// truncate msg when
							// needed
			    സന്ദേശം_no_more = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    സന്ദേശം_no_more = FALSE;
			    സന്ദേശം_scroll = i;
			    showruler(TRUE);
			    windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
			    RedrawingDisabled = save_RedrawingDisabled;

#ifdef USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#endif
			    ++no_mapping;	// don't map this key
			    ++allow_താക്കോലുകൾ;	// allow special keys
			    typed = plain_vgetc();
			    --allow_താക്കോലുകൾ;
			    --no_mapping;

			    // clear the question
			    സന്ദേശം_didout = FALSE;	// don't scroll up
			    സന്ദേശം_col = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    എങ്കിൽ (orig_വരി.string != ശൂന്യ)
				ml_replace(lnum, orig_വരി.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			എങ്കിൽ (typed == 'q' || typed == ESC || typed == Ctrl_C
#ifdef UNIX
				|| typed == intr_char
#endif
				)
			{
			    got_quit = TRUE;
			    നിർത്തുക;
			}
			എങ്കിൽ (typed == 'n')
			    നിർത്തുക;
			എങ്കിൽ (typed == 'y')
			    നിർത്തുക;
			എങ്കിൽ (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    നിർത്തുക;
			}
			എങ്കിൽ (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    നിർത്തുക;
			}
			എങ്കിൽ (typed == Ctrl_E)
			    scrollup_clamp();
			അല്ലെങ്കിൽ എങ്കിൽ (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    എങ്കിൽ (vim_strchr(p_cpo, CPO_UNDO) != ശൂന്യ)
			--no_u_sync;

		    എങ്കിൽ (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			എങ്കിൽ (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.length;
			    skip_match = TRUE;
			}
			പോകുക skip;
		    }
		    എങ്കിൽ (got_quit)
			പോകുക skip;
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
		എങ്കിൽ (subflags.do_എണ്ണം)
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
		എങ്കിൽ (sublen == 0 || aborting() || subflags.do_എണ്ണം)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    പോകുക skip;
		}
#endif

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		എങ്കിൽ (nmatch > curbuf->b_ml.ml_വരി_എണ്ണം - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_വരി_എണ്ണം - sub_firstlnum + 1;
		    skip_match = TRUE;
		    // safety check
		    എങ്കിൽ (nmatch < 0)
			പോകുക skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		എങ്കിൽ (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.length = sub_firstline.length;
#ifdef FEAT_PROP_POPUP
		    എങ്കിൽ (curbuf->b_has_textprop)
		    {
			പൂർണ്ണസംഖ്യ bytes_added = (പൂർണ്ണസംഖ്യ)sublen - 1 - (regmatch.endpos[0].col
						   - regmatch.startpos[0].col);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			എങ്കിൽ (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       bytes_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			total_added += (colnr_T)bytes_added;
		    }
#endif
		}
		അല്ലെങ്കിൽ
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    എങ്കിൽ (curbuf->b_has_textprop)
		    {
			char_u	*prop_start;

			// Props in the first line may be shortened or deleted
			എങ്കിൽ (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			total_added -=
			    (colnr_T)(sub_firstline.length - regmatch.startpos[0].col);

			// Props in the last line may be moved or deleted
			എങ്കിൽ (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].col, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			text_prop_എണ്ണം = get_text_props(curbuf, lastlnum,
							   &prop_start, FALSE);
			എങ്കിൽ (text_prop_എണ്ണം > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(text_props);
			    text_props = ALLOC_MULT(textprop_T,
							      text_prop_എണ്ണം);
			    എങ്കിൽ (text_props != ശൂന്യ)
			    {
				mch_memmove(text_props, prop_start,
					 text_prop_എണ്ണം * വലുപ്പം(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				പൂർണ്ണസംഖ്യ wi = 0;
				വേണ്ടി (പൂർണ്ണസംഖ്യ pi = 0; pi < text_prop_എണ്ണം; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    എങ്കിൽ (text_props[pi].tp_id < 0
					    || (text_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					തുടരുക;
				    text_props[wi] = text_props[pi];
				    text_props[wi].tp_col +=
					regmatch.startpos[0].col + (colnr_T)sublen - 1;
				    text_props[wi].u.tp_text = ശൂന്യ;
				    ++wi;
				}
				text_prop_എണ്ണം = wi;
			    }
			}
		    }
#endif
		    tmp.string = ml_get(lastlnum);
		    tmp.length = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    എങ്കിൽ (curbuf->b_has_textprop)
			total_added +=
			    (colnr_T)(tmp.length - regmatch.endpos[0].col);
#endif
		}

		copy_len = (size_t)(regmatch.startpos[0].col - copycol);
		needed_size = copy_len
		    + (tmp.length - (size_t)regmatch.endpos[0].col) + sublen + 1;

		എങ്കിൽ (new_start.string == ശൂന്യ)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_size = needed_size + 50;
		    എങ്കിൽ ((new_start.string = alloc_clear(new_start_size)) == ശൂന്യ)
			പോകുക outofmem;
		    *new_start.string = NUL;
		    new_start.length = 0;
		}
		അല്ലെങ്കിൽ
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_size += new_start.length;
		    എങ്കിൽ (needed_size > new_start_size)
		    {
			new_start_size = needed_size + 50;
			എങ്കിൽ ((p1 = alloc_clear(new_start_size)) == ശൂന്യ)
			{
			    vim_free(new_start.string);
			    പോകുക outofmem;
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

		എങ്കിൽ (new_start_size - copy_len < sublen)
		    sublen = new_start_size - copy_len - 1;

#ifdef FEAT_EVAL
		++textlock;
#endif
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_end, (പൂർണ്ണസംഖ്യ)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		എങ്കിൽ (n > 0)
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
		എങ്കിൽ (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    എങ്കിൽ (sub_firstline.string == ശൂന്യ)
		    {
			vim_free(new_start.string);
			പോകുക outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    എങ്കിൽ (sub_firstlnum <= line2)
			do_again = TRUE;
		    അല്ലെങ്കിൽ
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].col;

		എങ്കിൽ (skip_match)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    എങ്കിൽ (sub_firstline.string == ശൂന്യ)
		    {
			vim_free(new_start.string);
			പോകുക outofmem;
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
		വേണ്ടി (p1 = new_end; *p1; ++p1)
		{
		    എങ്കിൽ (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_start.length - (p1 - new_start.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_start.length;
#ifdef FEAT_PROP_POPUP
			എങ്കിൽ (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    എങ്കിൽ (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_start.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#endif
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*p1 == CAR)
		    {
			എങ്കിൽ (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_start.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_start.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    എങ്കിൽ (subflags.do_ask)
				appended_വരികൾ(lnum - 1, 1L);
			    അല്ലെങ്കിൽ
			    {
				എങ്കിൽ (first_വരി == 0)
				    first_വരി = lnum;
				last_വരി = lnum + 1;
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
		    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
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
		എങ്കിൽ (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_split)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, ശൂന്യ)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    എങ്കിൽ (new_start.string != ശൂന്യ)
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

			എങ്കിൽ (u_savesub(lnum) != OK)
			    നിർത്തുക;
			ml_replace(lnum, new_start.string, TRUE);
#ifdef FEAT_PROP_POPUP
			എങ്കിൽ (text_props != ശൂന്യ)
			    add_text_props(lnum, text_props, text_prop_എണ്ണം);
#endif
			എങ്കിൽ (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    എങ്കിൽ (u_savedel(lnum, nmatch_tl) != OK)
				നിർത്തുക;
			    വേണ്ടി (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    എങ്കിൽ (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    അല്ലെങ്കിൽ
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    എങ്കിൽ (subflags.do_ask)
				deleted_വരികൾ(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			എങ്കിൽ (subflags.do_ask)
			    changed_bytes(lnum, 0);
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (first_വരി == 0)
				first_വരി = lnum;
			    last_വരി = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_start.string;
			sub_firstline.length = new_start.length;
			new_start.string = ശൂന്യ;
			new_start.length = 0;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);
			copycol = 0;
			did_split = false;
		    }
		    എങ്കിൽ (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, ശൂന്യ);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    എങ്കിൽ (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			എങ്കിൽ (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			നിർത്തുക;
		    }
		}

		വരി_breakcheck();
	    }

	    എങ്കിൽ (did_sub)
		++sub_nlines;
	    vim_free(new_start.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	വരി_breakcheck();
    }

    എങ്കിൽ (first_വരി != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_വരി_എണ്ണം - old_വരി_എണ്ണം;
	changed_വരികൾ(first_വരി, 0, last_വരി - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ifdef FEAT_PROP_POPUP
    vim_free(text_props);
#endif

    // ":s/pat//n" doesn't move the cursor
    എങ്കിൽ (subflags.do_എണ്ണം)
	curwin->w_cursor = old_cursor;

    എങ്കിൽ (sub_nsubs > start_nsubs)
    {
	എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_start.lnum = eap->line1;
	    curbuf->b_op_end.lnum = line2;
	    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	}

	എങ്കിൽ (!global_busy)
	{
	    // when interactive leave cursor on the match
	    എങ്കിൽ (!subflags.do_ask)
	    {
		എങ്കിൽ (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		അല്ലെങ്കിൽ
		    beginline(BL_WHITE | BL_FIX);
	    }
	    എങ്കിൽ (!do_sub_സന്ദേശം(subflags.do_എണ്ണം) && subflags.do_ask)
		സന്ദേശം("");
	}
	അല്ലെങ്കിൽ
	    global_need_beginline = TRUE;
	എങ്കിൽ (subflags.do_print)
	    print_വരി(curwin->w_cursor.lnum,
					 subflags.do_സംഖ്യ, subflags.do_list);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!global_busy)
    {
	എങ്കിൽ (got_int)		// interrupted
	    emsg(_(e_interrupted));
	അല്ലെങ്കിൽ എങ്കിൽ (got_match)	// did find something but nothing substituted
	    സന്ദേശം("");
	അല്ലെങ്കിൽ എങ്കിൽ (subflags.do_error)	// nothing found
	    semsg(_(e_pattern_not_found_str), get_തിരയൽ_pat());
    }

#ifdef FEAT_FOLDING
    എങ്കിൽ (subflags.do_ask && hasAnyFolding(curwin))
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
    പൂർണ്ണസംഖ്യ
do_sub_സന്ദേശം(
    പൂർണ്ണസംഖ്യ	    എണ്ണം_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    എങ്കിൽ (((sub_nsubs > p_report && (താക്കോൽ_Typed || sub_nlines > 1 || p_report < 1))
		|| എണ്ണം_only)
	    && messaging())
    {
	അക്ഷരം	*സന്ദേശം_single;
	അക്ഷരം	*സന്ദേശം_plural;

	എങ്കിൽ (got_int)
	    STRCPY(സന്ദേശം_buf, _("(Interrupted) "));
	അല്ലെങ്കിൽ
	    *സന്ദേശം_buf = NUL;

	സന്ദേശം_single = എണ്ണം_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	സന്ദേശം_plural = എണ്ണം_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_add(സന്ദേശം_buf, വലുപ്പം(സന്ദേശം_buf),
				 NGETTEXT(സന്ദേശം_single, സന്ദേശം_plural, sub_nlines),
				 sub_nsubs, (long)sub_nlines);

	എങ്കിൽ (സന്ദേശം(സന്ദേശം_buf))
	    // save message to display it after redraw
	    set_keep_സന്ദേശം((char_u *)സന്ദേശം_buf, 0);
	തിരികെ TRUE;
    }
    എങ്കിൽ (got_int)
    {
	emsg(_(e_interrupted));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം ശൂന്യം
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    എങ്കിൽ (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", ശൂന്യ, ശൂന്യ, DOCMD_NOWAIT);
    അല്ലെങ്കിൽ
	do_cmdline(cmd, ശൂന്യ, ശൂന്യ, DOCMD_NOWAIT);
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
    ശൂന്യം
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    പൂർണ്ണസംഖ്യ		ndone = 0;
    പൂർണ്ണസംഖ്യ		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*used_pat;
    regmmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		match;
    പൂർണ്ണസംഖ്യ		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    എങ്കിൽ (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_വരി_എണ്ണം))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	തിരികെ;
    }

    എങ്കിൽ (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    അല്ലെങ്കിൽ
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    എങ്കിൽ (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	തിരികെ;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    എങ്കിൽ (*cmd == '\\')
    {
	++cmd;
	എങ്കിൽ (vim_strchr((char_u *)"/?&", *cmd) == ശൂന്യ)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    തിരികെ;
	}
	എങ്കിൽ (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	അല്ലെങ്കിൽ
	    which_pat = RE_തിരയൽ;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (check_regexp_delim(*cmd) == FAIL)
    {
	തിരികെ;
    }
    അല്ലെങ്കിൽ
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    എങ്കിൽ (തിരയൽ_regcomp(pat, patlen, &used_pat, RE_BOTH, which_pat, തിരയൽ_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	തിരികെ;
    }

    എങ്കിൽ (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ശൂന്യ);
	എങ്കിൽ ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	വേണ്ടി (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ശൂന്യ);
	    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		നിർത്തുക;  // re-compiling regprog failed
	    എങ്കിൽ ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    വരി_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	എങ്കിൽ (got_int)
	    സന്ദേശം(_(e_interrupted));
	അല്ലെങ്കിൽ എങ്കിൽ (ndone == 0)
	{
	    എങ്കിൽ (type == 'v')
	    {
		എങ്കിൽ (in_vim9script())
		    semsg(_(e_pattern_found_in_every_വരി_str), used_pat);
		അല്ലെങ്കിൽ
		    smsg(_("Pattern found in every line: %s"), used_pat);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (in_vim9script())
		    semsg(_(e_pattern_not_found_str), used_pat);
		അല്ലെങ്കിൽ
		    smsg(_("Pattern not found: %s"), used_pat);
	    }
	}
	അല്ലെങ്കിൽ
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
    ശൂന്യം
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
    സന്ദേശം_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_വരി_എണ്ണം;
    ആകുമ്പോൾ (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    എങ്കിൽ (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    അല്ലെങ്കിൽ
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    changed_വരി_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    എങ്കിൽ (സന്ദേശം_col == 0 && സന്ദേശം_scrolled == 0)
	സന്ദേശം_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    എങ്കിൽ (!do_sub_സന്ദേശം(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_വരി_എണ്ണം - old_lcount);
}

#ifdef FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(ശൂന്യം)
{
    തിരികെ old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    ശൂന്യം
set_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#endif // FEAT_VIMINFO

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_old_sub(ശൂന്യം)
{
    vim_free(old_sub);
}
#endif

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    പൂർണ്ണസംഖ്യ
prepare_tagpreview(
    പൂർണ്ണസംഖ്യ		undo_sync,	    // sync undo when leaving the window
    പൂർണ്ണസംഖ്യ		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    എങ്കിൽ (curwin->w_p_pvw)
	തിരികെ FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    എങ്കിൽ (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_find_preview_window();
	എങ്കിൽ (wp != ശൂന്യ)
	    popup_set_wantpos_cursor(wp, wp->w_minwidth, ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (use_popup != USEPOPUP_NONE)
    {
	wp = popup_find_info_window();
	എങ്കിൽ (wp != ശൂന്യ)
	{
	    എങ്കിൽ (use_popup == USEPOPUP_NORMAL)
		popup_show(wp);
	    അല്ലെങ്കിൽ
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    അല്ലെങ്കിൽ
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (wp->w_p_pvw)
		നിർത്തുക;
    }
    എങ്കിൽ (wp != ശൂന്യ)
    {
	win_enter(wp, undo_sync);
	തിരികെ FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    എങ്കിൽ ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	തിരികെ popup_create_preview_window(use_popup != USEPOPUP_NONE);
# endif
    എങ്കിൽ (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	തിരികെ FALSE;
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
    തിരികെ TRUE;
}

#endif

/*
 * Make the user happy.
 */
    ശൂന്യം
ex_smile(exarg_T *eap UNUSED)
{
    സ്ഥിതം അക്ഷരം *code[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    അക്ഷരം *p;
    പൂർണ്ണസംഖ്യ n;
    പൂർണ്ണസംഖ്യ i;

    സന്ദേശം_start();
    സന്ദേശം_putchar('\n');
    വേണ്ടി (i = 0; i < 2; ++i)
	വേണ്ടി (p = code[i]; *p != NUL; ++p)
	    എങ്കിൽ (*p == 'x')
		സന്ദേശം_putchar('\n');
	    അല്ലെങ്കിൽ
		വേണ്ടി (n = *p++; n > 0; --n)
		    എങ്കിൽ (*p == 'o' || *p == '$')
			സന്ദേശം_putchar_attr(*p, HL_ATTR(HLF_L));
		    അല്ലെങ്കിൽ
			സന്ദേശം_putchar(*p);
    സന്ദേശം_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    ശൂന്യം
ex_drop(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		split = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    എങ്കിൽ (ERROR_IF_POPUP_WINDOW || ERROR_IF_TERM_POPUP_WINDOW)
	തിരികെ;

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
    എങ്കിൽ (ARGCOUNT == 0)
	തിരികെ;

    എങ്കിൽ (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	തിരികെ;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	എങ്കിൽ (wp->w_buffer == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    എങ്കിൽ (!bufIsChanged(curbuf))
	    {
		പൂർണ്ണസംഖ്യ save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
		ex_rewind(eap);
	    തിരികെ;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    എങ്കിൽ (!buf_hide(curbuf))
    {
	++emsg_off;
	split = check_changed(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    എങ്കിൽ (split)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    അല്ലെങ്കിൽ
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
skip_vimgrep_pat(char_u *p, char_u **s, പൂർണ്ണസംഖ്യ *flags)
{
    തിരികെ skip_vimgrep_pat_ext(p, s, flags, ശൂന്യ, ശൂന്യ);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, പൂർണ്ണസംഖ്യ *flags, char_u **nulp, പൂർണ്ണസംഖ്യ *cp)
{
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	എങ്കിൽ (s != ശൂന്യ)
	    *s = p;
	p = skiptowhite(p);
	എങ്കിൽ (s != ശൂന്യ && *p != NUL)
	{
	    എങ്കിൽ (nulp != ശൂന്യ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    അല്ലെങ്കിൽ
    {
	// ":vimgrep /pattern/[g][j] fname"
	എങ്കിൽ (s != ശൂന്യ)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	എങ്കിൽ (*p != c)
	    തിരികെ ശൂന്യ;

	// Truncate the pattern.
	എങ്കിൽ (s != ശൂന്യ)
	{
	    എങ്കിൽ (nulp != ശൂന്യ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	ആകുമ്പോൾ (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    എങ്കിൽ (flags != ശൂന്യ)
	    {
		എങ്കിൽ (*p == 'g')
		    *flags |= VGR_GLOBAL;
		അല്ലെങ്കിൽ എങ്കിൽ (*p == 'j')
		    *flags |= VGR_NOJUMP;
		അല്ലെങ്കിൽ
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    തിരികെ p;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    ശൂന്യം
ex_oldfiles(exarg_T *eap UNUSED)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    പൂർണ്ണസംഖ്യ		matches = -1;

    എങ്കിൽ (l == ശൂന്യ)
    {
	സന്ദേശം(_("No old files"));
	തിരികെ;
    }

    സന്ദേശം_start();
    സന്ദേശം_scroll = TRUE;
    വേണ്ടി (li = l->lv_first; li != ശൂന്യ && !got_int; li = li->li_next)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	എങ്കിൽ (!message_filtered(fname))
	{
	    എങ്കിൽ (matches < 0)
		matches = nr;
	    അല്ലെങ്കിൽ
		matches = 0;
	    സന്ദേശം_outnum((long)nr);
	    സന്ദേശം_puts(": ");
	    സന്ദേശം_outtrans(fname);
	    സന്ദേശം_clr_eos();
	    സന്ദേശം_putchar('\n');
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_more = FALSE;
	// we only need to prompt if there is more than 1 match
	എങ്കിൽ (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	അല്ലെങ്കിൽ
	    nr = prompt_for_സംഖ്യ(FALSE);
	സന്ദേശം_starthere();
	എങ്കിൽ (nr > 0)
	{
	    char_u *p = list_find_str(get_vim_var_list(VV_OLDFILES),
		    (long)nr);

	    എങ്കിൽ (p != ശൂന്യ)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, ശൂന്യ);
		vim_free(p);
	    }
	}
    }
# endif
}
#endif
