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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ଅନ୍ତର୍ଭାଵ <float.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା linelen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *has_tab);
ସ୍ଥିର ଶୂନ୍ୟ do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_out);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_writing(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_readonly(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *forceit, buf_T *buf);
ସ୍ଥିର ଶୂନ୍ୟ delbuf_ସନ୍ଦେଶ(char_u *name);

/*
 * ":ascii" and "ga".
 */
    ଶୂନ୍ୟ
do_ascii(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cval;
    ଅକ୍ଷର	buf1[20];
    ଅକ୍ଷର	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc[MAX_MCO];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ci = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    ଅନ୍ୟଥା
	c = gchar_cursor();
    ଯଦି (c == NUL)
    {
	ସନ୍ଦେଶ("NUL");
	ଫେରନ୍ତୁ;
    }

    IObuff[0] = NUL;
    ଯଦି (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	ଯଦି (c == NL)	    // NUL is stored as NL
	    c = NUL;
	ଯଦି (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	ଅନ୍ୟଥା
	    cval = c;
	ଯଦି (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, ଆକାର(buf1), "  <%s>", (ଅକ୍ଷର *)buf3);
	}
	ଅନ୍ୟଥା
	    buf1[0] = NUL;
	ଯଦି (c >= 0x80)
	    vim_snprintf(buf2, ଆକାର(buf2), "  <M-%s>",
						 (ଅକ୍ଷର *)transchar(c & 0x7f));
	ଅନ୍ୟଥା
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	ଯଦି (dig != ଖାଲି)
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	ଅନ୍ୟଥା
#endif
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	ଯଦି (enc_utf8)
	    c = cc[ci++];
	ଅନ୍ୟଥା
	    c = 0;
    }

    // Repeat for combining characters.
    ଯେତେବେଳେ (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	ଯଦି (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	ଯଦି (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	ଯଦି (dig != ଖାଲି)
	    vim_snprintf((ଅକ୍ଷର *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	ଅନ୍ୟଥା
#endif
	    vim_snprintf((ଅକ୍ଷର *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	ଯଦି (ci == MAX_MCO)
	    ଭାଙ୍ଗ;
	ଯଦି (enc_utf8)
	    c = cc[ci++];
	ଅନ୍ୟଥା
	    c = 0;
    }

    ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    ଶୂନ୍ୟ
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_indent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_tab;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;

#ifdef FEAT_RIGHTLEFT
    ଯଦି (curwin->w_p_rl)
    {
	// switch left and right aligning
	ଯଦି (eap->cmdidx == CMD_ଡାହାଣ)
	    eap->cmdidx = CMD_ବାମ;
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_ବାମ)
	    eap->cmdidx = CMD_ଡାହାଣ;
    }
#endif

    width = atoi((ଅକ୍ଷର *)eap->arg);
    save_curpos = curwin->w_cursor;
    ଯଦି (eap->cmdidx == CMD_ବାମ)    // width is used for new indent
    {
	ଯଦି (width >= 0)
	    indent = width;
    }
    ଅନ୍ୟଥା
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	ଯଦି (width <= 0)
	    width = curbuf->b_p_tw;
	ଯଦି (width == 0 && curbuf->b_p_wm > 0)
	    width = curwin->w_width - curbuf->b_p_wm;
	ଯଦି (width <= 0)
	    width = 80;
    }

    ଯଦି (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ଫେରନ୍ତୁ;

    ପାଇଁ (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	ଯଦି (eap->cmdidx == CMD_ବାମ)		// left align
	    new_indent = indent;
	ଅନ୍ୟଥା
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_ଡାହାଣ ? &has_tab
						   : ଖାଲି) - get_indent();

	    ଯଦି (len <= 0)			// skip blank lines
		ଜାରି;

	    ଯଦି (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    ଅନ୍ୟଥା
	    {
		new_indent = width - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		ଯଦି (has_tab)
		    ଯେତେବେଳେ (new_indent > 0)
		    {
			(ଶୂନ୍ୟ)set_indent(new_indent, 0);
			ଯଦି (linelen(ଖାଲି) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    କର
				(ଶୂନ୍ୟ)set_indent(++new_indent, 0);
			    ଯେତେବେଳେ (linelen(ଖାଲି) <= width);
			    --new_indent;
			    ଭାଙ୍ଗ;
			}
			--new_indent;
		    }
	    }
	}
	ଯଦି (new_indent < 0)
	    new_indent = 0;
	(ଶୂନ୍ୟ)set_indent(new_indent, 0);		// set indent
    }
    changed_ଧାଡ଼ିଗୁଡ଼ିକ(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linelen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *has_tab)
{
    char_u  *ଧାଡ଼ି;
    char_u  *first;
    char_u  *last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    ଧାଡ଼ି = ml_get_curline();
    ଯଦି (*ଧାଡ଼ି == NUL)
	ଫେରନ୍ତୁ 0;

    // find the first non-blank character
    first = skipwhite(ଧାଡ଼ି);

    // find the character after the last non-blank character
    ପାଇଁ (last = first + STRLEN(first);
				last > first && VIM_ISWHITE(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize_str(ଧାଡ଼ି);	// get line length on screen
    ଯଦି (has_tab != ଖାଲି)		// check for embedded TAB
	*has_tab = (vim_strchr(first, TAB) != ଖାଲି);
    *last = save;

    ଫେରନ୍ତୁ len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
ସ୍ଥିର char_u	*sortbuf1;
ସ୍ଥିର char_u	*sortbuf2;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_lc;	// sort using locale
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_ic;	// ignore case
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_nr;	// sort on number
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_rx;	// sort on regex instead of skipping it
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_flt;	// sort on floating number

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    linenr_T	lnum;			// line number
    ସଂଘ {
	ସଂରଚନା
	{
	    varnumber_T	start_col_nr;	// starting column number
	    varnumber_T	end_col_nr;	// ending column number
	} ଧାଡ଼ି;
	ସଂରଚନା
	{
	    varnumber_T	ମୂଲ୍ୟ;		// value if sorting by integer
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_ସଂଖ୍ୟା;		// TRUE when line contains a number
	} ଅଙ୍କ;
	float_T ମୂଲ୍ୟ_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
string_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ଯଦି (sort_lc)
	ଫେରନ୍ତୁ strcoll((ଅକ୍ଷର *)s1, (ଅକ୍ଷର *)s2);
    ଫେରନ୍ତୁ sort_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sort_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    ଯଦି (sort_abort)
	ଫେରନ୍ତୁ 0;
    fast_breakcheck();
    ଯଦି (got_int)
	sort_abort = TRUE;

    ଯଦି (sort_nr)
    {
	ଯଦି (l1.st_u.ଅଙ୍କ.is_ସଂଖ୍ୟା != l2.st_u.ଅଙ୍କ.is_ସଂଖ୍ୟା)
	    result = l1.st_u.ଅଙ୍କ.is_ସଂଖ୍ୟା > l2.st_u.ଅଙ୍କ.is_ସଂଖ୍ୟା ? 1 : -1;
	ଅନ୍ୟଥା
	    result = l1.st_u.ଅଙ୍କ.ମୂଲ୍ୟ == l2.st_u.ଅଙ୍କ.ମୂଲ୍ୟ ? 0
			     : l1.st_u.ଅଙ୍କ.ମୂଲ୍ୟ > l2.st_u.ଅଙ୍କ.ମୂଲ୍ୟ ? 1 : -1;
    }
    ଅନ୍ୟଥା ଯଦି (sort_flt)
	result = l1.st_u.ମୂଲ୍ୟ_flt == l2.st_u.ମୂଲ୍ୟ_flt ? 0
			     : l1.st_u.ମୂଲ୍ୟ_flt > l2.st_u.ମୂଲ୍ୟ_flt ? 1 : -1;
    ଅନ୍ୟଥା
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.ଧାଡ଼ି.start_col_nr,
		     l1.st_u.ଧାଡ଼ି.end_col_nr - l1.st_u.ଧାଡ଼ି.start_col_nr + 1);
	sortbuf1[l1.st_u.ଧାଡ଼ି.end_col_nr - l1.st_u.ଧାଡ଼ି.start_col_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.ଧାଡ଼ି.start_col_nr,
		     l2.st_u.ଧାଡ଼ି.end_col_nr - l2.st_u.ଧାଡ଼ି.start_col_nr + 1);
	sortbuf2[l2.st_u.ଧାଡ଼ି.end_col_nr - l2.st_u.ଧାଡ଼ି.start_col_nr] = 0;

	result = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    ଯଦି (result == 0)
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(l1.lnum - l2.lnum);
    ଫେରନ୍ତୁ result;
}

/*
 * ":sort".
 */
    ଶୂନ୍ୟ
ex_sort(exarg_T *eap)
{
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    linenr_T	lnum;
    long	maxlen = 0;
    sorti_T	*nrs;
    size_t	ଗଣନା_2 = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unique = FALSE;
    long	deleted;
    colnr_T	start_col;
    colnr_T	end_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sort_what = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		format_found = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    ଯଦି (ଗଣନା_2 <= 1)
	ଫେରନ୍ତୁ;

    ଯଦି (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ଫେରନ୍ତୁ;
    sortbuf1 = ଖାଲି;
    sortbuf2 = ଖାଲି;
    regmatch.regprog = ଖାଲି;
    nrs = ALLOC_MULT(sorti_T, ଗଣନା_2);
    ଯଦି (nrs == ଖାଲି)
	ଯାଅ sortend;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    ପାଇଁ (p = eap->arg; *p != NUL; ++p)
    {
	ଯଦି (VIM_ISWHITE(*p))
	    ;
	ଅନ୍ୟଥା ଯଦି (*p == 'i')
	    sort_ic = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == 'l')
	    sort_lc = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == 'r')
	    sort_rx = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == 'n')
	{
	    sort_nr = 1;
	    ++format_found;
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'f')
	{
	    sort_flt = 1;
	    ++format_found;
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'b')
	{
	    sort_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_found;
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'o')
	{
	    sort_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_found;
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'x')
	{
	    sort_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_found;
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'u')
	    unique = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == '"')	// comment start
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା ଯଦି (eap->nextcmd == ଖାଲି && check_nextcmd(p) != ଖାଲି)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (!ASCII_ISALPHA(*p) && regmatch.regprog == ଖାଲି)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    ଯଦି (s == p + 1)
	    {
		ଯଦି (last_ସନ୍ଧାନ_pat() == ଖାଲି)
		{
		    emsg(_(e_no_previous_regular_expression));
		    ଯାଅ sortend;
		}
		regmatch.regprog = vim_regcomp(last_ସନ୍ଧାନ_pat(), RE_MAGIC);
	    }
	    ଅନ୍ୟଥା
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ଯଦି (regmatch.regprog == ଖାଲି)
		ଯାଅ sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_argument_str), p);
	    ଯାଅ sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    ଯଦି (format_found > 1)
    {
	emsg(_(e_invalid_argument));
	ଯାଅ sortend;
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
    ପାଇଁ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	ଯଦି (maxlen < len)
	    maxlen = len;

	start_col = 0;
	end_col = len;
	ଯଦି (regmatch.regprog != ଖାଲି && vim_regexec(&regmatch, s, 0))
	{
	    ଯଦି (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ଅନ୍ୟଥା
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	ଅନ୍ୟଥା
	    ଯଦି (regmatch.regprog != ଖାଲି)
		end_col = 0;

	ଯଦି (sort_nr || sort_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + end_col;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + start_col;
	    ଯଦି (sort_nr)
	    {
		ଯଦି (sort_what & STR2NR_HEX)
		    s = skiptohex(p);
		ଅନ୍ୟଥା ଯଦି (sort_what & STR2NR_BIN)
		    s = skiptobin(p);
		ଅନ୍ୟଥା
		    s = skiptodigit(p);
		ଯଦି (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		ଯଦି (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.ଅଙ୍କ.is_ସଂଖ୍ୟା = FALSE;
		    nrs[lnum - eap->line1].st_u.ଅଙ୍କ.ମୂଲ୍ୟ = 0;
		}
		ଅନ୍ୟଥା
		{
		    nrs[lnum - eap->line1].st_u.ଅଙ୍କ.is_ସଂଖ୍ୟା = TRUE;
		    vim_str2nr(s, ଖାଲି, ଖାଲି, sort_what,
			&nrs[lnum - eap->line1].st_u.ଅଙ୍କ.ମୂଲ୍ୟ,
			ଖାଲି, 0, FALSE, ଖାଲି);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		s = skipwhite(p);
		ଯଦି (*s == '+')
		    s = skipwhite(s + 1);

		ଯଦି (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.ମୂଲ୍ୟ_flt = -DBL_MAX;
		ଅନ୍ୟଥା
		    nrs[lnum - eap->line1].st_u.ମୂଲ୍ୟ_flt =
						      strtod((ଅକ୍ଷର *)s, ଖାଲି);
	    }
	    *s2 = c;
	}
	ଅନ୍ୟଥା
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.ଧାଡ଼ି.start_col_nr = start_col;
	    nrs[lnum - eap->line1].st_u.ଧାଡ଼ି.end_col_nr = end_col;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	ଯଦି (regmatch.regprog != ଖାଲି)
	    fast_breakcheck();
	ଯଦି (got_int)
	    ଯାଅ sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ଯଦି (sortbuf1 == ଖାଲି)
	ଯାଅ sortend;
    sortbuf2 = alloc(maxlen + 1);
    ଯଦି (sortbuf2 == ଖାଲି)
	ଯାଅ sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((ଶୂନ୍ୟ *)nrs, ଗଣନା_2, ଆକାର(sorti_T), sort_compare);

    ଯଦି (sort_abort)
	ଯାଅ sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? ଗଣନା_2 - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	ଯଦି (get_lnum + ((linenr_T)ଗଣନା_2 - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	ଯଦି (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    ଯଦି (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		ଭାଙ୍ଗ;
	}
	fast_breakcheck();
	ଯଦି (got_int)
	    ଯାଅ sortend;
    }

    // delete the original lines if appending worked
    ଯଦି (i == ଗଣନା_2)
	ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
	    ml_delete(eap->line1);
    ଅନ୍ୟଥା
	ଗଣନା_2 = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (long)(ଗଣନା_2 - (lnum - eap->line2));
    ଯଦି (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    ଅନ୍ୟଥା ଯଦି (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    ଯଦି (change_occurred || deleted != 0)
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    ଯଦି (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    ଶୂନ୍ୟ
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    linenr_T	lnum;
    long	maxlen = 0;
    linenr_T	ଗଣନା_2 = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_only_unique = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    long	deleted = 0;
    colnr_T	start_col;
    colnr_T	end_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    ଯଦି (ଗଣନା_2 <= 1)
	ଫେରନ୍ତୁ;

    ଯଦି (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ଫେରନ୍ତୁ;
    sortbuf1 = ଖାଲି;
    regmatch.regprog = ଖାଲି;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    ପାଇଁ (p = eap->arg; *p != NUL; ++p)
    {
	ଯଦି (VIM_ISWHITE(*p))
	    ;
	ଅନ୍ୟଥା ଯଦି (*p == 'i')
	    sort_ic = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == 'l')
	    sort_lc = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == 'r')
	    sort_rx = TRUE;
	ଅନ୍ୟଥା ଯଦି (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    ଯଦି (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '"')	// comment start
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା ଯଦି (eap->nextcmd == ଖାଲି && check_nextcmd(p) != ଖାଲି)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (!ASCII_ISALPHA(*p) && regmatch.regprog == ଖାଲି)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    ଯଦି (s == p + 1)
	    {
		ଯଦି (last_ସନ୍ଧାନ_pat() == ଖାଲି)
		{
		    emsg(_(e_no_previous_regular_expression));
		    ଯାଅ uniqend;
		}
		regmatch.regprog = vim_regcomp(last_ସନ୍ଧାନ_pat(), RE_MAGIC);
	    }
	    ଅନ୍ୟଥା
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ଯଦି (regmatch.regprog == ଖାଲି)
		ଯାଅ uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_argument_str), p);
	    ଯାଅ uniqend;
	}
    }

    // Find the length of the longest line.
    ପାଇଁ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	ଯଦି (maxlen < len)
	    maxlen = len;

	ଯଦି (got_int)
	    ଯାଅ uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ଯଦି (sortbuf1 == ଖାଲି)
	ଯାଅ uniqend;

    // Delete lines according to options.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା match_continue = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_is_unmatch = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_match;
    linenr_T done_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    ପାଇଁ (linenr_T i = 0; i < ଗଣନା_2; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	start_col = 0;
	end_col = len;
	ଯଦି (regmatch.regprog != ଖାଲି && vim_regexec(&regmatch, s, 0))
	{
	    ଯଦି (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ଅନ୍ୟଥା
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	ଅନ୍ୟଥା
	    ଯଦି (regmatch.regprog != ଖାଲି)
		end_col = 0;
	ଯଦି (end_col > 0)
	{
	    save_c = s[end_col];
	    s[end_col] = NUL;
	}

	is_match = i > 0 ? !string_compare(&s[start_col], sortbuf1) : FALSE;
	delete_lnum = 0;
	ଯଦି (next_is_unmatch)
	{
	    is_match = FALSE;
	    next_is_unmatch = FALSE;
	}

	ଯଦି (!keep_only_unique && !keep_only_not_unique)
	{
	    ଯଦି (is_match)
		delete_lnum = get_lnum;
	    ଅନ୍ୟଥା
		STRCPY(sortbuf1, &s[start_col]);
	}
	ଅନ୍ୟଥା ଯଦି (keep_only_not_unique)
	{
	    ଯଦି (is_match)
	    {
		done_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (i > 0 && !match_continue && get_lnum - 1 > done_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    next_is_unmatch = TRUE;
		}
		ଅନ୍ୟଥା ଯଦି (i >= ଗଣନା_2 - 1)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}
	ଅନ୍ୟଥା // keep_only_unique
	{
	    ଯଦି (is_match)
	    {
		ଯଦି (!match_continue)
		    delete_lnum = get_lnum - 1;
		ଅନ୍ୟଥା
		    delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (i == 0 && match_continue)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}

	ଯଦି (end_col > 0)
	    s[end_col] = save_c;

	ଯଦି (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    ଗଣନା_2--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	ଯଦି (got_int)
	    ଯାଅ uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
    msgmore(-deleted);

    ଯଦି (change_occurred)
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    ଯଦି (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ;  // Num lines moved
    linenr_T	last_ଧାଡ଼ି;  // Last line in file after adding new text
#ifdef FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#endif

    ଯଦି (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_of_ଧାଡ଼ିଗୁଡ଼ିକ_into_itself));
	ଫେରନ୍ତୁ FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    ଯଦି (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	ଯଦି (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	ଅନ୍ୟଥା
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	ଫେରନ୍ତୁ OK;
    }

    ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    ଯଦି (u_save(dest, dest + 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ପାଇଁ (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	ଯଦି (str != ଖାଲି)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    ଯଦି (dest < line1)
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
    last_ଧାଡ଼ି = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    mark_adjust_nofold(line1, line2, last_ଧାଡ଼ି - line2, 0L);
    ଯଦି (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ଯଦି (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#endif
	ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1;
	    curbuf->b_op_end.lnum = dest;
	}
    }
    ଅନ୍ୟଥା
    {
	mark_adjust_nofold(dest + 1, line1 - 1, ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ଯଦି (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#endif
	ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest + 1;
	    curbuf->b_op_end.lnum = dest + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ;
	}
    }
    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust_nofold(last_ଧାଡ଼ି - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1, last_ଧାଡ଼ି,
					     -(last_ଧାଡ଼ି - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    ଯଦି (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_MESSAGE);

    ଯଦି (!global_busy && ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ),
			(long)ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    ଯଦି (dest >= line1)
	curwin->w_cursor.lnum = dest;
    ଅନ୍ୟଥା
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    ଯଦି (line1 < dest)
    {
	dest += ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1;
	last_ଧାଡ଼ି = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	ଯଦି (dest > last_ଧାଡ଼ି + 1)
	    dest = last_ଧାଡ଼ି + 1;
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(line1, 0, dest, 0L);
    }
    ଅନ୍ୟଥା
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(dest + 1, 0, line1 + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ, 0L);

    ଫେରନ୍ତୁ OK;
}

/*
 * ":copy"
 */
    ଶୂନ୍ୟ
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	ଗଣନା_2;
    char_u	*p;

    ଗଣନା_2 = line2 - line1 + 1;
    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = n + 1;
	curbuf->b_op_end.lnum = n + ଗଣନା_2;
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
    ଯଦି (u_save(n, n + 1) == FAIL)
	ଫେରନ୍ତୁ;

    curwin->w_cursor.lnum = n;
    ଯେତେବେଳେ (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	ଯଦି (p != ଖାଲି)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	ଯଦି (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	ଯଦି (curwin->w_cursor.lnum < line1)
	    ++line1;
	ଯଦି (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(n, ଗଣନା_2);
    ଯଦି (VIsual_active)
	check_pos(curbuf, &VIsual);

    msgmore((long)ଗଣନା_2);
}

ସ୍ଥିର char_u	*prevcmd = ଖାଲି;	// the previous command

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_prev_shellcmd(ଶୂନ୍ୟ)
{
    vim_free(prevcmd);
}
#endif

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prevcmd_is_set(ଶୂନ୍ୟ)
{
    ଯଦି (prevcmd == ଖାଲି)
    {
	emsg(_(e_no_previous_command));
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    ଶୂନ୍ୟ
do_bang(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		addr_ଗଣନା,
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_in,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_out)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = ଖାଲି;		// the new command
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			free_newcmd = FALSE;    // need to free() newcmd
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			scroll_save = ସନ୍ଦେଶ_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (addr_ଗଣନା == 0)		// :!
    {
	ସନ୍ଦେଶ_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	ସନ୍ଦେଶ_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    କର
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(trailarg) + 1;
	ଯଦି (newcmd != ଖାଲି)
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(newcmd);
	ଯଦି (ins_prevcmd)
	{
	    ଯଦି (!prevcmd_is_set())
	    {
		vim_free(newcmd);
		ଫେରନ୍ତୁ;
	    }
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prevcmd);
	}
	ଯଦି ((t = alloc(len)) == ଖାଲି)
	{
	    vim_free(newcmd);
	    ଫେରନ୍ତୁ;
	}
	*t = NUL;
	ଯଦି (newcmd != ଖାଲି)
	    STRCAT(t, newcmd);
	ଯଦି (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = ଖାଲି;
	ଯେତେବେଳେ (*p)
	{
	    ଯଦି (*p == '!')
	    {
		ଯଦି (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		ଅନ୍ୟଥା
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ++p;
	}
    } ଯେତେବେଳେ (trailarg != ଖାଲି);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    ଯଦି (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    ଅନ୍ୟଥା
	free_newcmd = TRUE;

    ଯଦି (bangredo)	    // put cmd in redo buffer for ! command
    {
	ଯଦି (!prevcmd_is_set())
	    ଯାଅ theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	ଯଦି (cmd != ଖାଲି)
	{
	    AppendToRedobuffLit(cmd, -1);
	    vim_free(cmd);
	}
	ଅନ୍ୟଥା
	    AppendToRedobuffLit(prevcmd, -1);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    ଯଦି (*p_shq != NUL)
    {
	ଯଦି (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	ଯଦି (newcmd == ଖାଲି)
	    ଫେରନ୍ତୁ;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    ଯଦି (addr_ଗଣନା == 0)		// :!
    {
	// echo the command
	ସନ୍ଦେଶ_start();
	ସନ୍ଦେଶ_putchar(':');
	ସନ୍ଦେଶ_putchar('!');
	ସନ୍ଦେଶ_outtrans(newcmd);
	ସନ୍ଦେଶ_clr_eos();
	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);

	do_shell(newcmd, 0);
    }
    ଅନ୍ୟଥା				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
	apply_autocmds(EVENT_SHELLFILTERPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
    }

theend:
    ଯଦି (free_newcmd)
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
    ସ୍ଥିର ଶୂନ୍ୟ
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_in,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_out)
{
    char_u	*itmp = ଖାଲି;
    char_u	*otmp = ଖାଲି;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shell_flags = 0;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags = cmdmod.cmod_flags;
#ifdef FEAT_FILTERPIPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stmp = p_stmp;
#endif

    ଯଦି (*cmd == NUL)	    // no filter command
	ଫେରନ୍ତୁ;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_ଧାଡ଼ି_abv_curs();
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

    ଯଦି (do_out)
	shell_flags |= SHELL_DOOUT;

#ifdef FEAT_FILTERPIPE
# ifdef VIMDLL
    ଯଦି (!gui.in_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    ଯଦି (!do_in && do_out && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    ଅନ୍ୟଥା ଯଦି (do_in && !do_out && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
    }
    ଅନ୍ୟଥା ଯଦି (do_in && do_out && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    ଅନ୍ୟଥା
#endif
	ଯଦି ((do_in && (itmp = vim_tempname('i', FALSE)) == ଖାଲି)
		|| (do_out && (otmp = vim_tempname('o', FALSE)) == ଖାଲି))
	{
	    emsg(_(e_cant_get_temp_file_name));
	    ଯାଅ filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    ଯଦି (itmp != ଖାଲି && buf_write(curbuf, itmp, ଖାଲି, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	ସନ୍ଦେଶ_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#ଯଦି defined(FEAT_EVAL)
	ଯଦି (!aborting())
#endif
	    // will call wait_return()
	    (ଶୂନ୍ୟ)semsg(_(e_cant_create_file_str), itmp);
	ଯାଅ filterend;
    }
    ଯଦି (curbuf != old_curbuf)
	ଯାଅ filterend;

    ଯଦି (!do_out)
	ସନ୍ଦେଶ_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    ଯଦି (cmd_buf == ଖାଲି)
	ଯାଅ filterend;

    windgoto((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    ଯଦି (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	redraw_later_clear();

    ଯଦି (do_out)
    {
	ଯଦି (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    ଯାଅ error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    ଯଦି (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    ଯଦି (do_out)
    {
	ଯଦି (otmp != ଖାଲି)
	{
	    ଯଦି (readfile(otmp, ଖାଲି, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#ଯଦି defined(FEAT_EVAL)
		ଯଦି (!aborting())
#endif
		{
		    ସନ୍ଦେଶ_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		ଯାଅ error;
	    }
	    ଯଦି (curbuf != old_curbuf)
		ଯାଅ filterend;
	}

	read_linecount = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - read_linecount;

	ଯଦି (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_start.lnum = line2 + 1;
	    curbuf->b_op_end.lnum = curwin->w_cursor.lnum;
	    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(line2, read_linecount);
	}

	ଯଦି (do_in)
	{
	    ଯଦି ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == ଖାଲି)
	    {
		ଯଦି (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		ଅନ୍ୟଥା ଯଦି (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		ଅନ୍ୟଥା
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
	    del_ଧାଡ଼ିଗୁଡ଼ିକ(linecount, TRUE);
	    ଯଦି (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_start.lnum = curbuf->b_op_end.lnum =
					MIN(line1, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
		curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	    }
	    ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	ଯଦି (linecount > p_report)
	{
	    ଯଦି (do_in)
	    {
		vim_snprintf(ସନ୍ଦେଶ_buf, ଆକାର(ସନ୍ଦେଶ_buf),
				    _("%ld lines filtered"), (long)linecount);
		ଯଦି (ସନ୍ଦେଶ(ସନ୍ଦେଶ_buf) && !ସନ୍ଦେଶ_scroll)
		    // save message to display it after redraw
		    set_keep_ସନ୍ଦେଶ((char_u *)ସନ୍ଦେଶ_buf, 0);
	    }
	    ଅନ୍ୟଥା
		msgmore((long)linecount);
	}
    }
    ଅନ୍ୟଥା
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    ଯଦି (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_buffer));
    }
    ଅନ୍ୟଥା ଯଦି (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

    ଯଦି (itmp != ଖାଲି)
	mch_remove(itmp);
    ଯଦି (otmp != ଖାଲି)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    ଶୂନ୍ୟ
do_shell(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_nwr;
#endif
#ifdef MSWIN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winstart = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ଯଦି (check_restricted() || check_secure())
    {
	ସନ୍ଦେଶ_end();
	ଫେରନ୍ତୁ;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    ଯଦି (cmd != ଖାଲି)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# ଯଦି defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    ଯଦି (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ଖାଲି)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    ସନ୍ଦେଶ_putchar('\r');			// put cursor at start of line
    ଯଦି (!autocmd_busy)
    {
	ଯଦି (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    ଯଦି (!winstart)
#endif
	ସନ୍ଦେଶ_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    ଯଦି (p_warn && !autocmd_busy && ସନ୍ଦେଶ_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    ଯଦି (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		ଯଦି (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		ସନ୍ଦେଶ_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		ଯଦି (!keep_termcap)
		    stoptermcap();
#endif
		ଭାଙ୍ଗ;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    ଯଦି (!swapping_screen())
	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
    cursor_on();
    (ଶୂନ୍ୟ)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    ଯଦି (!swapping_screen())
    {
	ସନ୍ଦେଶ_row = Rows - 1;
	ସନ୍ଦେଶ_col = 0;
    }

    ଯଦି (autocmd_busy)
    {
	ଯଦି (ସନ୍ଦେଶ_silent == 0)
	    redraw_later_clear();
    }
    ଅନ୍ୟଥା
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	ଯଦି (!gui.in_use)
# endif
	{
	    ଯଦି (cmd == ଖାଲି
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		ଯଦି (ସନ୍ଦେଶ_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		ଯଦି (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା redraw = ସନ୍ଦେଶ_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		ଯଦି (gui.in_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(term_console ? -1 : redraw); // see below
# ଅନ୍ୟଥା
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	ଯଦି (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	ଯଦି (skip_redraw)		// ':' hit in wait_return()
	{
	    ଯଦି (ସନ୍ଦେଶ_silent == 0)
		redraw_later_clear();
	}
	ଅନ୍ୟଥା ଯଦି (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    ଯଦି (got_int && ସନ୍ଦେଶ_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    ଅନ୍ୟଥା
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
}

#ଯଦି !defined(UNIX)
    ସ୍ଥିର char_u *
find_pipe(char_u *cmd)
{
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    inquote = FALSE;

    ପାଇଁ (p = cmd; *p != NUL; ++p)
    {
	ଯଦି (!inquote && *p == '|')
	    ଫେରନ୍ତୁ p;
	ଯଦି (*p == '"')
	    inquote = !inquote;
	ଅନ୍ୟଥା ଯଦି (rem_backslash(p))
	    ++p;
    }
    ଫେରନ୍ତୁ ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_powershell = FALSE;
#ifdef UNIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_fish_shell;
#endif

    char_u *shell_name = get_isolated_shell_name();
    ଯଦି (shell_name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

#ଯଦି defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    ଯଦି (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    ଅନ୍ୟଥା
#endif
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	ଯଦି (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	ଅନ୍ୟଥା
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    ଯଦି (itmp != ଖାଲି)
    {
	ଯଦି (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	ଅନ୍ୟଥା
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    ଯଦି (otmp != ଖାଲି)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (is_powershell)
    {
	ଯଦି (itmp != ଖାଲି)
	    vim_snprintf((ଅକ୍ଷର *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	ଅନ୍ୟଥା
	    vim_snprintf((ଅକ୍ଷର *)buf, len, "& { %s }", cmd);
    }
    ଅନ୍ୟଥା
    {
#ଯଦି defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	ଯଦି (itmp != ଖାଲି || otmp != ଖାଲି)
	{
	    ଯଦି (is_fish_shell)
		vim_snprintf((ଅକ୍ଷର *)buf, len, "begin; %s; end", (ଅକ୍ଷର *)cmd);
	    ଅନ୍ୟଥା
		vim_snprintf((ଅକ୍ଷର *)buf, len, "(%s)", (ଅକ୍ଷର *)cmd);
	}
	ଅନ୍ୟଥା
	    STRCPY(buf, cmd);
	ଯଦି (itmp != ଖାଲି)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#ଅନ୍ୟଥା
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	ଯଦି (*p_sxe != NUL && *p_sxq == '(')
	{
	    ଯଦି (itmp != ଖାଲି || otmp != ଖାଲି)
		vim_snprintf((ଅକ୍ଷର *)buf, len, "(%s)", (ଅକ୍ଷର *)cmd);
	    ଅନ୍ୟଥା
		STRCPY(buf, cmd);
	    ଯଦି (itmp != ଖାଲି)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	ଅନ୍ୟଥା
	{
	    STRCPY(buf, cmd);
	    ଯଦି (itmp != ଖାଲି)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		ଯଦି (*p_shq == NUL)
		{
		    p = find_pipe(buf);
		    ଯଦି (p != ଖାଲି)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		ଯଦି (*p_shq == NUL)
		{
		    p = find_pipe(cmd);
		    ଯଦି (p != ଖାଲି)
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
    ଯଦି (otmp != ଖାଲି)
	append_redir(buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, p_srr, otmp);

    ଫେରନ୍ତୁ buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    ଶୂନ୍ୟ
append_redir(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*end;

    end = buf + STRLEN(buf);
    // find "%s"
    ପାଇଁ (p = opt; (p = vim_strchr(p, '%')) != ଖାଲି; ++p)
    {
	ଯଦି (p[1] == 's') // found %s
	    ଭାଙ୍ଗ;
	ଯଦି (p[1] == '%') // skip %%
	    ++p;
    }
    ଯଦି (p != ଖାଲି)
    {
#ifdef MSWIN
	*end++ = ' '; // not really needed? Not with sh, ksh or bash
#endif
	vim_snprintf((ଅକ୍ଷର *)end, (size_t)(buflen - (end - buf)),
						  (ଅକ୍ଷର *)opt, (ଅକ୍ଷର *)fname);
    }
    ଅନ୍ୟଥା
	vim_snprintf((ଅକ୍ଷର *)end, (size_t)(buflen - (end - buf)), " %s %s",
		(ଅକ୍ଷର *)opt, (ଅକ୍ଷର *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    ଶୂନ୍ୟ
do_fixdel(exarg_T *eap UNUSED)
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != ଖାଲି
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    ଶୂନ୍ୟ
print_ଧାଡ଼ି_no_prefix(
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_ସଂଖ୍ୟା,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		list)
{
    ଅକ୍ଷର	numbuf[30];

    ଯଦି (curwin->w_p_nu || use_ସଂଖ୍ୟା)
    {
	vim_snprintf(numbuf, ଆକାର(numbuf),
				   "%*ld ", ସଂଖ୍ୟା_width(curwin), (long)lnum);
	ସନ୍ଦେଶ_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    ସନ୍ଦେଶ_prt_ଧାଡ଼ି(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    ଶୂନ୍ୟ
print_ଧାଡ଼ି(linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_ସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_silent = silent_mode;

    // apply :filter /pat/
    ଯଦି (message_filtered(ml_get(lnum)))
	ଫେରନ୍ତୁ;

    ସନ୍ଦେଶ_start();
    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()
    print_ଧାଡ଼ି_no_prefix(lnum, use_ସଂଖ୍ୟା, list);
    ଯଦି (save_silent)
    {
	ସନ୍ଦେଶ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = save_silent;
    }
    info_message = FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rename_buffer(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, ଖାଲି, ଖାଲି, FALSE, curbuf);
    // buffer changed, don't change name now
    ଯଦି (buf != curbuf)
	ଫେରନ୍ତୁ FAIL;
#ifdef FEAT_EVAL
    ଯଦି (aborting())	    // autocmds may abort script processing
	ଫେରନ୍ତୁ FAIL;
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
    curbuf->b_ffname = ଖାଲି;
    curbuf->b_sfname = ଖାଲି;
    ଯଦି (setfname(curbuf, new_fname, ଖାଲି, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	ଫେରନ୍ତୁ FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    ଯଦି (xfname != ଖାଲି && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	ଯଦି (buf != ଖାଲି && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    ଫେରନ୍ତୁ OK;
}

/*
 * ":file[!] [fname]".
 */
    ଶୂନ୍ୟ
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    ଯଦି (eap->addr_ଗଣନା > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_ଗଣନା > 1))
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*eap->arg != NUL || eap->addr_ଗଣନା == 1)
    {
	ଯଦି (rename_buffer(eap->arg) == FAIL)
	    ଫେରନ୍ତୁ;
	redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    ଯଦି (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    ଶୂନ୍ୟ
ex_update(exarg_T *eap)
{
    ଯଦି (curbufIsChanged())
	(ଶୂନ୍ୟ)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    ଶୂନ୍ୟ
ex_write(exarg_T *eap)
{
    ଯଦି (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    }

    ଯଦି (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)do_write(eap);
}

#ifdef UNIX
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_writable(char_u *fname)
{
    ଯଦି (mch_nodetype(fname) == ଗଣ୍ଠି_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_write(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other;
    char_u	*fname = ଖାଲି;		// init to shut up gcc
    char_u	*ffname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    char_u	*free_fname = ଖାଲି;
#ifdef FEAT_BROWSE
    char_u	*browse_file = ଖାଲି;
#endif
    buf_T	*alt_buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name_was_missing;

    ଯଦି (not_writing())		// check 'write' option
	ଫେରନ୍ତୁ FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    ଯଦି ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    ଖାଲି, ଖାଲି, ଖାଲି, curbuf);
	ଯଦି (browse_file == ଖାଲି)
	    ଯାଅ theend;
	ffname = browse_file;
    }
#endif
    ଯଦି (*ffname == NUL)
    {
	ଯଦି (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_argument_required));
	    ଯାଅ theend;
	}
	other = FALSE;
    }
    ଅନ୍ୟଥା
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	ଯଦି (free_fname != ଖାଲି)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    ଯଦି (other)
    {
	ଯଦି (vim_strchr(p_cpo, CPO_ALTWRITE) != ଖାଲି
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	ଅନ୍ୟଥା
	    alt_buf = buflist_findname(ffname);
	ଯଦି (alt_buf != ଖାଲି && alt_buf->b_ml.ml_mfp != ଖାଲି)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    ଯାଅ theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    ଯଦି (!other && (bt_dontwrite_ସନ୍ଦେଶ(curbuf) || check_fname() == FAIL
#ifdef UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#endif
		|| check_readonly(&eap->forceit, curbuf)))
	ଯାଅ theend;

    ଯଦି (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	ଯଦି (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ଯଦି (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		ଯଦି (vim_dialog_yesno(VIM_QUESTION, ଖାଲି,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    ଯାଅ theend;
		eap->forceit = TRUE;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		emsg(_(e_use_bang_to_write_partial_buffer));
		ଯାଅ theend;
	    }
	}
    }

    ଯଦି (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	ଯଦି (eap->cmdidx == CMD_saveas && alt_buf != ଖାଲି)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, ଖାଲି, ଖାଲି, FALSE, alt_buf);
#ifdef FEAT_EVAL
	    ଯଦି (curbuf != was_curbuf || aborting())
#ଅନ୍ୟଥା
	    ଯଦି (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		ଯାଅ theend;
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

	    apply_autocmds(EVENT_BUFFILEPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, ଖାଲି, ଖାଲି, FALSE, alt_buf);
	    ଯଦି (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, ଖାଲି, ଖାଲି, FALSE, alt_buf);
	    }
#ifdef FEAT_EVAL
	    ଯଦି (curbuf != was_curbuf || aborting())
#ଅନ୍ୟଥା
	    ଯଦି (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		ଯାଅ theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    ଯଦି (*curbuf->b_p_ft == NUL)
	    {
		ଯଦି (au_has_group((char_u *)"filetypedetect"))
		    (ଶୂନ୍ୟ)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, ଖାଲି);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == ଖାଲି;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	ଯଦି (eap->cmdidx == CMD_saveas)
	{
	    ଯଦି (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	ଯଦି (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    ଫେରନ୍ତୁ retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    ଯଦି (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == ଖାଲି)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	ଯଦି (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    ଯଦି (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		ଫେରନ୍ତୁ FAIL;
	    }
#endif
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ଯଦି (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_ସନ୍ଦେଶ_SIZE];

		dialog_ସନ୍ଦେଶ(buff, _("Overwrite existing file \"%s\"?"), fname);
		ଯଦି (vim_dialog_yesno(VIM_QUESTION, ଖାଲି, buff, 2) != VIM_YES)
		    ଫେରନ୍ତୁ FAIL;
		eap->forceit = TRUE;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		emsg(_(e_file_exists));
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	ଯଦି (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    ଯଦି (*p_dir == NUL)
	    {
		dir = alloc(5);
		ଯଦି (dir == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		STRCPY(dir, ".");
	    }
	    ଅନ୍ୟଥା
	    {
		dir = alloc(MAXPATHL);
		ଯଦି (dir == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    ଯଦି (r)
	    {
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		ଯଦି (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_ସନ୍ଦେଶ_SIZE];

		    dialog_ସନ୍ଦେଶ(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    ଯଦି (vim_dialog_yesno(VIM_QUESTION, ଖାଲି, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			ଫେରନ୍ତୁ FAIL;
		    }
		    eap->forceit = TRUE;
		}
		ଅନ୍ୟଥା
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    ଶୂନ୍ୟ
ex_wnext(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2;
    ଅନ୍ୟଥା
	i = curwin->w_arg_idx - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଯଦି (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    ଶୂନ୍ୟ
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_forceit = eap->forceit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_exiting = exiting;

    ଯଦି (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	ଯଦି (before_quit_all(eap) == FAIL)
	    ଫେରନ୍ତୁ;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ifdef FEAT_TERMINAL
	ଯଦି (exiting && !eap->forceit && term_job_running(buf->b_term))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);
	    ଯଦି (term_try_stop_job(buf) == FAIL)
	    {
		no_write_message_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    ଅନ୍ୟଥା ଯଦି (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	ଅନ୍ୟଥା
#endif
	ଯଦି (bufIsChanged(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    ଯଦି (not_writing())
	    {
		++error;
		ଭାଙ୍ଗ;
	    }
#ifdef FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    ଯଦି (buf->b_ffname == ଖାଲି && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#endif
	    ଯଦି (buf->b_ffname == ଖାଲି)
	    {
		semsg(_(e_no_file_name_for_buffer_nr), (long)buf->b_fnum);
		++error;
	    }
	    ଅନ୍ୟଥା ଯଦି (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    ଅନ୍ୟଥା
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		ଯଦି (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		ଯଦି (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    ଯଦି (exiting)
    {
	ଯଦି (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
not_writing(ଶୂନ୍ୟ)
{
    ଯଦି (p_write)
	ଫେରନ୍ତୁ FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_readonly(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    ଯଦି (!*forceit && (buf->b_p_ro
		|| (mch_stat((ଅକ୍ଷର *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ଯଦି ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != ଖାଲି)
	{
	    char_u	buff[DIALOG_ସନ୍ଦେଶ_SIZE];

	    ଯଦି (buf->b_p_ro)
		dialog_ସନ୍ଦେଶ(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    ଅନ୍ୟଥା
		dialog_ସନ୍ଦେଶ(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    ଯଦି (vim_dialog_yesno(VIM_QUESTION, ଖାଲି, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		ଫେରନ୍ତୁ FALSE;
	    }
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ TRUE;
	}
	ଅନ୍ୟଥା
#endif
	ଯଦି (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_set_add_bang_to_override));
	ଅନ୍ୟଥା
	    semsg(_(e_str_is_read_only_add_bang_to_override), buf->b_fname);
	ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getfile(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		setpm,
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    char_u	*free_me = ଖାଲି;

    ଯଦି (!check_can_set_curbuf_forceit(forceit))
	ଫେରନ୍ତୁ GETFILE_ERROR;

    ଯଦି (text_locked())
	ଫେରନ୍ତୁ GETFILE_ERROR;
    ଯଦି (curbuf_locked())
	ଫେରନ୍ତୁ GETFILE_ERROR;

    ଯଦି (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    ଅନ୍ୟଥା
	other = (fnum != curbuf->b_fnum);

    ଯଦି (other)
	++no_wait_return;	    // don't wait for autowrite message
    ଯଦି (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ଯଦି (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	ଯଦି (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    ଯାଅ theend;
	}
    }
    ଯଦି (other)
	--no_wait_return;
    ଯଦି (setpm)
	setpcmark();
    ଯଦି (!other)
    {
	ଯଦି (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    ଅନ୍ୟଥା ଯଦି (do_ecmd(fnum, ffname, sfname, ଖାଲି, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    ଅନ୍ୟଥା
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    ଫେରନ୍ତୁ retval;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_ecmd(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    win_T	*oldwin)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other_file;		// TRUE if editing another file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldbuf;			// TRUE if using existing buffer
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = ଖାଲି;
#ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = ଖାଲି;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = ଖାଲି;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newcol = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		solcol = -1;
    pos_T	*pos;
    char_u	*command = ଖାଲି;
#ifdef FEAT_SPELL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_get_winopts = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		readfile_flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    ଯଦି (ERROR_IF_TERM_POPUP_WINDOW)
	ଫେରନ୍ତୁ FAIL;
#endif

    ଯଦି (eap != ଖାଲି)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    ଯଦି (fnum != 0)
    {
	ଯଦି (fnum == curbuf->b_fnum)	// file is already being edited
	    ଫେରନ୍ତୁ OK;			// nothing to do
	other_file = TRUE;
    }
    ଅନ୍ୟଥା
    {
#ifdef FEAT_BROWSE
	ଯଦି ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    ଯଦି (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		ଯଦି (ffname == ଖାଲି || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    ଅନ୍ୟଥା
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    ଖାଲି, ଖାଲି, ଖାଲି, curbuf);
		ଯଦି (browse_file == ଖାଲି)
		    ଯାଅ theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	ଯଦି (sfname == ଖାଲି)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	ଯଦି (sfname != ଖାଲି)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	ଯଦି ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == ଖାଲି || *ffname == NUL))
	    ଯାଅ theend;

	ଯଦି (ffname == ଖାଲି)
	    other_file = TRUE;
					    // there is no file name
	ଅନ୍ୟଥା ଯଦି (*ffname == NUL && curbuf->b_ffname == ଖାଲି)
	    other_file = FALSE;
	ଅନ୍ୟଥା
	{
	    ଯଦି (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    ଯଦି (free_fname != ଖାଲି)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    ଯଦି (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == ଖାଲି ? 0 : CCGD_EXCMD)))
    {
	ଯଦି (fnum == 0 && other_file && ffname != ଖାଲି)
	    (ଶୂନ୍ୟ)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	ଯାଅ theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    ଯଦି (oldwin != ଖାଲି && !win_valid(oldwin))
	oldwin = ଖାଲି;

#ଯଦି defined(FEAT_EVAL)
    ଯଦି ((command != ଖାଲି || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	ଯଦି (command != ଖାଲି)
	    valsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(command) + 3;
	ଅନ୍ୟଥା
	    valsize = 30;
	val.string = alloc(valsize);
	ଯଦି (val.string != ଖାଲି)
	{
	    ଯଦି (command != ଖାଲି)
		val.length = vim_snprintf_safelen((ଅକ୍ଷର *)val.string,
		    valsize, ":%s\r", command);
	    ଅନ୍ୟଥା
		val.length = vim_snprintf_safelen((ଅକ୍ଷର *)val.string,
		    valsize, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, val.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val.length);
	    did_set_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    ଯଦି (other_file)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_alt_fnum = curwin->w_alt_fnum;

	ଯଦି (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    ଯଦି ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    ଯଦି (oldwin != ଖାଲି)
		buflist_altfpos(oldwin);
	}

	ଯଦି (fnum)
	    buf = buflist_findnr(fnum);
	ଅନ୍ୟଥା
	{
	    ଯଦି (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		ଯଦି (command != ଖାଲି)
		{
		    tlnum = atol((ଅକ୍ଷର *)command);
		    ଯଦି (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		ଯଦି (newbuf != ଖାଲି)
		{
		    ଯଦି (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    ଯଦି (tlnum > 0)
			newbuf->b_last_cursor.lnum = tlnum;
		}
		ଯାଅ theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    ଯଦି (oldwin != ଖାଲି)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	ଯଦି (buf == ଖାଲି)
	    ଯାଅ theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	ଯଦି (buf->b_locked_split)
	{
	    emsg(_(e_cannot_switch_to_a_closing_buffer));
	    ଯାଅ theend;
	}
	ଯଦି (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	ଅନ୍ୟଥା					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (ଶୂନ୍ୟ)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    ଯଦି (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		ଯାଅ theend;
#ifdef FEAT_EVAL
	    ଯଦି (aborting())	    // autocmds may abort script processing
		ଯାଅ theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	ଯଦି ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
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
	ଯଦି (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    ପ୍ରତିଜ୍ଞା(cmdwin_buf == ଖାଲି);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = ଖାଲି;

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
	    ଯଦି (buf->b_fname != ଖାଲି)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    ଯଦି (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_ସନ୍ଦେଶ(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		ଯାଅ theend;
	    }
#ifdef FEAT_EVAL
	    ଯଦି (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		ଯାଅ theend;
	    }
#endif
	    ଯଦି (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    ଅନ୍ୟଥା
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		ଯଦି (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_buffer(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != ଖାଲି);

		// Autocommands may have closed the window.
		ଯଦି (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		ଯଦି (aborting() && curwin->w_buffer != ଖାଲି)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    ଯାଅ theend;
		}
#endif
		// Be careful again, like above.
		ଯଦି (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    ଯଦି (curwin->w_buffer == ଖାଲି)
			buf = lastbuf;
		    ଅନ୍ୟଥା
		    {
			delbuf_ସନ୍ଦେଶ(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			ଯାଅ theend;
		    }
		}
		ଯଦି (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		ଅନ୍ୟଥା
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    ଯଦି (curwin->w_buffer == ଖାଲି
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    ଯଦି (curwin->w_buffer != ଖାଲି)
			--curwin->w_buffer->b_nwindows;

		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    ଯଦି (!oldbuf && eap != ଖାଲି)
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
    ଅନ୍ୟଥା // !other_file
    {
	ଯଦି ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    ଯାଅ theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    ଯଦି ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    ଅନ୍ୟଥା
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	ଯଦି (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    ଯଦି (buf != curbuf)
	ଯାଅ theend;
#ifdef FEAT_EVAL
    ଯଦି (aborting())	    // autocmds may abort script processing
	ଯାଅ theend;
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
    ଯଦି (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	ଯଦି (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	ଯଦି (buf->b_fname != ଖାଲି)
	    new_name = vim_strsave(buf->b_fname);
	ଅନ୍ୟଥା
	    new_name = ଖାଲି;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	ଯଦି (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    ଯଦି (u_savecommon(0, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		ଯାଅ theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	ଅନ୍ୟଥା
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	ଯଦି (!bufref_valid(&bufref))
	{
	    delbuf_ସନ୍ଦେଶ(new_name);	// frees new_name
	    ଯାଅ theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	ଯଦି (buf != curbuf)
	    ଯାଅ theend;
#ifdef FEAT_EVAL
	ଯଦି (aborting())	    // autocmds may abort script processing
	    ଯାଅ theend;
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
    ଯଦି (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    ଯଦି (!auto_buf)
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
		ଯଦି (win->w_buffer == curbuf)
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
	ଯଦି (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    ଯଦି (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    ଯଦି (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#ଯଦି defined(FEAT_EVAL)
	    ଯଦି (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#ଅନ୍ୟଥା
	    (ଶୂନ୍ୟ)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    ଯଦି (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	ଅନ୍ୟଥା
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE,
							      curbuf, &retval);
	    ଯଦି ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, ଖାଲି, ଖାଲି, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	ଯଦି (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    ଯଦି (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	ଯଦି (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_ଧାଡ଼ି_abv_curs();

	maketitle();
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	ଯଦି (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    ଯଦି (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    ଯଦି (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ଶୂନ୍ୟ)parse_spelllang(curwin);
#endif

    ଯଦି (command == ଖାଲି)
    {
	ଯଦି (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	ଅନ୍ୟଥା ଯଦି (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    ଯଦି (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = true;
	    }
	    ଅନ୍ୟଥା
		beginline(BL_SOL | BL_FIX);
	}
	ଅନ୍ୟଥା			// no line number, go to last line in Ex mode
	{
	    ଯଦି (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    ଯଦି (oldbuf && !auto_buf)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ସନ୍ଦେଶ_scroll_save = ସନ୍ଦେଶ_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	ଯଦି (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    ସନ୍ଦେଶ_scroll = FALSE;
	ଯଦି (!ସନ୍ଦେଶ_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	ସନ୍ଦେଶ_start();
	ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll_save;
	ସନ୍ଦେଶ_scrolled_ign = TRUE;

	ଯଦି (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	ସନ୍ଦେଶ_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    ଯଦି (command != ଖାଲି)
	do_cmdline(command, ଖାଲି, ଖାଲି, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    ଯଦି (curbuf->b_kmap_state & KEYMAP_INIT)
	(ଶୂନ୍ୟ)keymap_init();
#endif

    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    ଯଦି (!skip_redraw)
    {
	n = *so_ptr;
	ଯଦି (topline == 0 && command == ଖାଲି)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    ଯଦି (p_im && (State & MODE_ଭର୍ତ୍ତି) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    ଯଦି (p_acd && curbuf->b_ffname != ଖାଲି)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	ଯଦି (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#ଯଦି defined(FEAT_NETBEANS_INTG)
    ଯଦି (curbuf->b_ffname != ଖାଲି)
    {
# ifdef FEAT_NETBEANS_INTG
	ଯଦି ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    ଯଦି (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#ଯଦି defined(FEAT_EVAL)
    ଯଦି (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, ଖାଲି, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ଶୂନ୍ୟ
delbuf_ସନ୍ଦେଶ(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_buffer_str),
					   name == ଖାଲି ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = ଖାଲି;
    au_new_curbuf.br_buf_free_ଗଣନା = 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    ସ୍ଥିର char_u *
get_append_ଧାଡ଼ି(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent)
{
    char_u	*theline;
    char_u	*p;

    ଯଦି (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    ଅନ୍ୟଥା ଯଦି (eap->ea_getline == ଖାଲି)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	ଯଦି (eap->nextcmd == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	p = vim_strchr(eap->nextcmd, NL);
	ଯଦି (p == ଖାଲି)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	ଯଦି (*p != NUL)
	    ++p;
	ଅନ୍ୟଥା
	    p = ଖାଲି;
	eap->nextcmd = p;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_State = State;

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
    ଫେରନ୍ତୁ theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    ଶୂନ୍ୟ
ex_append(exarg_T *eap)
{
    char_u	*theline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = 0;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    ଯଦି (not_in_vim9(eap, eap->skip) == FAIL)
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	ଯେତେବେଳେ ((theline = get_append_ଧାଡ଼ି(eap, 0)) != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    ଯଦି (end)
		ଭାଙ୍ଗ;
	}
	ଫେରନ୍ତୁ;
    }

    // the ! flag toggles autoindent
    ଯଦି (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    ଯଦି (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    ଯଦି (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    ଯଦି (empty && lnum == 1)
	lnum = 0;

    State = MODE_ଭର୍ତ୍ତି;		    // behave like in Insert mode
    ଯଦି (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    ପାଇଁ (;;)
    {
	ସନ୍ଦେଶ_scroll = TRUE;
	need_wait_return = FALSE;
	ଯଦି (curbuf->b_p_ai)
	{
	    ଯଦି (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    ଅନ୍ୟଥା ଯଦି (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_ଧାଡ଼ି(eap, indent);
	ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows - 1;
	ଯଦି (theline == ଖାଲି)
	    ଭାଙ୍ଗ;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	ଯଦି (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	ପାଇଁ (p = theline; indent > vcol; ++p)
	{
	    ଯଦି (*p == ' ')
		++vcol;
	    ଅନ୍ୟଥା ଯଦି (*p == TAB)
		vcol += 8 - vcol % 8;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}
	ଯଦି ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    ଭାଙ୍ଗ;
	}

	// don't use autoindent if nothing was typed.
	ଯଦି (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	ଯଦି (empty)
	    // there are no marks below the inserted lines
	    appended_ଧାଡ଼ିଗୁଡ଼ିକ(lnum, 1L);
	ଅନ୍ୟଥା
	    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	ଯଦି (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    ଯଦି (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	ଯଦି (eap->cmdidx != CMD_append)
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
    ଶୂନ୍ୟ
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ifdef FEAT_EVAL
    ଯଦି (not_in_vim9(eap, eap->skip) == FAIL)
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (eap->skip)
    {
	ex_append(eap);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	ଫେରନ୍ତୁ;

    // the ! flag toggles autoindent
    ଯଦି (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    ପାଇଁ (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    ଭାଙ୍ଗ;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark(eap->line1, (long)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    ଶୂନ୍ୟ
ex_z(exarg_T *eap)
{
    char_u	*x;
    long	bigness;
    char_u	*kind;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		minus = 0;
    linenr_T	start, end, curs, i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    ଯଦି (eap->forceit)
	bigness = Rows - 1;
    ଅନ୍ୟଥା ଯଦି (!ONE_WINDOW)
	bigness = curwin->w_height - 3;
    ଅନ୍ୟଥା
	bigness = curwin->w_p_scr * 2;
    ଯଦି (bigness < 1)
	bigness = 1;

    x = eap->arg;
    kind = x;
    ଯଦି (*kind == '-' || *kind == '+' || *kind == '='
					      || *kind == '^' || *kind == '.')
	++x;
    ଯେତେବେଳେ (*x == '-' || *x == '+')
	++x;

    ଯଦି (*x != 0)
    {
	ଯଦି (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_argument_to_z));
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା
	{
	    bigness = atol((ଅକ୍ଷର *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    ଯଦି (bigness > 2 * curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

	    p_window = bigness;
	    ଯଦି (*kind == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    ଯଦି (*kind == '-' || *kind == '+')
	ପାଇଁ (x = kind + 1; *x == *kind; ++x)
	    ;

    ବାଛ (*kind)
    {
	ସ୍ଥିତି '-':
	    start = lnum - bigness * (linenr_T)(x - kind) + 1;
	    end = start + bigness - 1;
	    curs = end;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି '=':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି '.':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = end;
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // '+'
	    start = lnum;
	    ଯଦି (*kind == '+')
		start += bigness * (linenr_T)(x - kind - 1) + 1;
	    ଅନ୍ୟଥା ଯଦି (eap->addr_ଗଣନା == 0)
		++start;
	    end = start + bigness - 1;
	    curs = end;
	    ଭାଙ୍ଗ;
    }

    ଯଦି (start < 1)
	start = 1;

    ଯଦି (end > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	end = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ଯଦି (curs > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	curs = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଅନ୍ୟଥା ଯଦି (curs < 1)
	curs = 1;

    ପାଇଁ (i = start; i <= end; i++)
    {
	ଯଦି (minus && i == lnum)
	{
	    ସନ୍ଦେଶ_putchar('\n');

	    ପାଇଁ (j = 1; j < Columns; j++)
		ସନ୍ଦେଶ_putchar('-');
	}

	print_ଧାଡ଼ି(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	ଯଦି (minus && i == lnum)
	{
	    ସନ୍ଦେଶ_putchar('\n');

	    ପାଇଁ (j = 1; j < Columns; j++)
		ସନ୍ଦେଶ_putchar('-');
	}
    }

    ଯଦି (curwin->w_cursor.lnum != curs)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_restricted(ଶୂନ୍ୟ)
{
    ଯଦି (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_in_rvim));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_secure(ଶୂନ୍ୟ)
{
    ଯଦି (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_ସନ୍ଧାନ));
	ଫେରନ୍ତୁ TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    ଯଦି (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ଫେରନ୍ତୁ TRUE;
    }
#endif
    ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର char_u	*old_sub = ଖାଲି;	// previous substitute pattern
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_all;		// do multiple substitutions per line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_ask;		// ask for confirmation
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_ଗଣନା;	// count only
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_error;	// if false, ignore errors
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_print;	// print last line with subs.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_list;	// list last line with subs.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_ସଂଖ୍ୟା;	// list last line with line nr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା delimiter)
{
    char_u *p = start;

    ଯେତେବେଳେ (p[0])
    {
	ଯଦି (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    ଭାଙ୍ଗ;
	}
	ଯଦି (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    ଫେରନ୍ତୁ p;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_regexp_delim(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (SAFE_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    ଶୂନ୍ୟ
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    long	i = 0;
    regmmatch_T regmatch;
    ସ୍ଥିର subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ifdef FEAT_EVAL
    subflags_T	subflags_save;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_do_all;		// remember user specified 'g' flag
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = ଖାଲି;		// init for GCC
    string_T	pat = {ଖାଲି, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delimiter;
    size_t	sublen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_quit = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_match = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		which_pat;
    char_u	*cmd;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State;
    linenr_T	first_ଧାଡ଼ି = 0;		// first changed line
    linenr_T	last_ଧାଡ଼ି = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    linenr_T	line2;
    long	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_nsubs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ma = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sandbox = 0;
#endif
#ifdef FEAT_PROP_POPUP
    textprop_T	*text_props = ଖାଲି;
#endif

    cmd = eap->arg;
    ଯଦି (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    start_nsubs = sub_nsubs;

    ଯଦି (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	// use last used regexp
    ଅନ୍ୟଥା
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    ଯଦି (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == ଖାଲି)
    {
	// don't accept alphanumeric for separator
	ଯଦି (check_regexp_delim(*cmd) == FAIL)
	    ଫେରନ୍ତୁ;
#ifdef FEAT_EVAL
	ଯଦି (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    ଫେରନ୍ତୁ;
#endif

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	ଯଦି (*cmd == '\\')
	{
	    ଯଦି (in_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_in_vim9_script));
		ଫେରନ୍ତୁ;
	    }
	    ++cmd;
	    ଯଦି (vim_strchr((char_u *)"/?&", *cmd) == ଖାଲି)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (*cmd != '&')
		which_pat = RE_ସନ୍ଧାନ;	    // use last '/' pattern
	    STR_LITERAL_SET(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	ଅନ୍ୟଥା		// find the end of the regexp
	{
	    which_pat = RE_LAST;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, ଖାଲି, ଖାଲି);
	    pat.length = (size_t)(cmd - pat.string);
	    ଯଦି (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	ଯଦି (sub == ଖାଲି)
	    // out of memory
	    ଫେରନ୍ତୁ;

	ଯଦି (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    ଯଦି (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != ଖାଲି)
	    {
		ଯଦି (old_sub == ଖାଲି)	// there is no previous command
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    vim_free(sub);
		    ଫେରନ୍ତୁ;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		ଯଦି (sub == ଖାଲି)
		    // out of memory
		    ଫେରନ୍ତୁ;
	    }
	    ଅନ୍ୟଥା ଯଦି (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		ଯଦି (old_sub == ଖାଲି)
		{
		    // out of memory
		    vim_free(sub);
		    ଫେରନ୍ତୁ;
		}
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (!eap->skip)	// use previous pattern and substitution
    {
	ଯଦି (old_sub == ଖାଲି)	// there is no previous command
	{
	    emsg(_(e_no_previous_substitute_regular_expression));
	    ଫେରନ୍ତୁ;
	}
	pat.string = ଖାଲି;		// search_regcomp() will use previous pattern
	pat.length = 0;
	sub = vim_strsave(old_sub);
	ଯଦି (sub == ଖାଲି)
	    ଫେରନ୍ତୁ;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    ଯଦି (pat.string != ଖାଲି && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_ଧାଡ଼ିଗୁଡ଼ିକ_ଗଣନା;

	ଯଦି (eap->skip)
	{
	    vim_free(sub);
	    ଫେରନ୍ତୁ;
	}
	curwin->w_cursor.lnum = eap->line1;
	ଯଦି (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	ଅନ୍ୟଥା ଯଦି (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	ଅନ୍ୟଥା ଯଦି (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_ଧାଡ଼ିଗୁଡ଼ିକ_ଗଣନା = eap->line2 - eap->line1 + 1;
	ଯଦି (eap->line2 < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ++joined_ଧାଡ଼ିଗୁଡ଼ିକ_ଗଣନା;
	ଯଦି (joined_ଧାଡ଼ିଗୁଡ଼ିକ_ଗଣନା > 1)
	{
	    (ଶୂନ୍ୟ)do_join(joined_ଧାଡ଼ିଗୁଡ଼ିକ_ଗଣନା, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_ଧାଡ଼ିଗୁଡ଼ିକ_ଗଣନା - 1;
	    sub_nlines = 1;
	    (ଶୂନ୍ୟ)do_sub_ସନ୍ଦେଶ(FALSE);
	    ex_may_print(eap);
	}

	ଯଦି (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.length, magic_isset());
	// put pattern in history
	add_to_history(HIST_ସନ୍ଧାନ, pat.string, pat.length, TRUE, NUL);
	vim_free(sub);

	ଫେରନ୍ତୁ;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    ଯଦି (*cmd == '&')
	++cmd;
    ଅନ୍ୟଥା
    {
#ifdef FEAT_EVAL
	ଯଦି (in_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	ଅନ୍ୟଥା
#endif
	ଯଦି (!p_ed)
	{
	    ଯଦି (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    ଅନ୍ୟଥା
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_ଗଣନା = FALSE;
	subflags.do_ସଂଖ୍ୟା = FALSE;
	subflags.do_ic = 0;
    }
    ଯେତେବେଳେ (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	ଯଦି (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	ଅନ୍ୟଥା ଯଦି (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	ଅନ୍ୟଥା ଯଦି (*cmd == 'n')
	    subflags.do_ଗଣନା = TRUE;
	ଅନ୍ୟଥା ଯଦି (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	ଅନ୍ୟଥା ଯଦି (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_LAST;
	ଅନ୍ୟଥା ଯଦି (*cmd == 'p')
	    subflags.do_print = TRUE;
	ଅନ୍ୟଥା ଯଦି (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_ସଂଖ୍ୟା = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	ଅନ୍ୟଥା ଯଦି (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
	++cmd;
    }
    ଯଦି (subflags.do_ଗଣନା)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    ଯଦି (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	ଯଦି (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_positive_ଗଣନା_required));
	    vim_free(sub);
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା ଯଦି (i >= INT_MAX)
	{
	    ଅକ୍ଷର	buf[20];
	    vim_snprintf(buf, ଆକାର(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    ଫେରନ୍ତୁ;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	ଯଦି (eap->line2 > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    eap->line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    ଯଦି (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	set_nextcmd(eap, cmd);
	ଯଦି (eap->nextcmd == ଖାଲି)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!subflags.do_ଗଣନା && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ସନ୍ଧାନ_regcomp(pat.string, pat.length, ଖାଲି, RE_SUBST, which_pat, ସନ୍ଧାନ_HIS, &regmatch) == FAIL)
    {
	ଯଦି (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	ଫେରନ୍ତୁ;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    ଯଦି (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    ଅନ୍ୟଥା ଯଦି (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = ଖାଲି;
    sub_firstline.length = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    ଯଦି (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ;
	sub = p;
    }
    ଅନ୍ୟଥା
    {
	p = regtilde(sub, magic_isset());

	ଯଦି (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    ପାଇଁ (lnum = eap->line1; lnum <= line2 && !(got_quit
#ଯଦି defined(FEAT_EVAL)
		|| aborting()
#endif
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ଖାଲି);
	ଯଦି (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_start = {ଖାଲି, 0};
	    size_t	new_start_size = 0;
	    char_u	*new_end;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_sub = FALSE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lastone;
	    size_t	copy_len;
	    size_t	needed_size;
	    long	nmatch_tl = 0;	// nr of lines matched below lnum
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_again;	// do it again after joining lines
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_match = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_split = false;	// "\r" split the line
#ifdef FEAT_PROP_POPUP
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	total_added =  0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_ଗଣନା = 0;
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
	    ଯଦି (!got_match)
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
	    ପାଇଁ (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		ଯଦି (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		    ଭାଙ୍ଗ;

		ଯଦି (sub_firstline.string == ଖାଲି)
		{
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    ଯଦି (sub_firstline.string == ଖାଲି)
		    {
			vim_free(new_start.string);
			ଯାଅ outofmem;
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
		ଯଦି (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    ଯଦି (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_match = TRUE;
		    ଅନ୍ୟଥା
		    {
			 // search for a match at next column
			ଯଦି (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			ଅନ୍ୟଥା
			    ++matchcol;
		    }
		    ଯାଅ skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		ଯଦି (subflags.do_ଗଣନା)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    ଯଦି (nmatch > 1)
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
		    ଯଦି (!(sub[0] == '\\' && sub[1] == '='))
#endif
			ଯାଅ skip;
		}

		ଯଦି (subflags.do_ask)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = MODE_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.col = regmatch.startpos[0].col;
		    ଯଦି (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    ଯଦି (vim_strchr(p_cpo, CPO_UNDO) != ଖାଲି)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    ଯେତେବେଳେ (subflags.do_ask)
		    {
			ଯଦି (exmode_active)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_ଧାଡ଼ି_no_prefix(lnum,
					 subflags.do_ସଂଖ୍ୟା, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, ଖାଲି, ଖାଲି, 0);
			    curwin->w_cursor.col = regmatch.endpos[0].col - 1;
			    ଯଦି (curwin->w_cursor.col < 0)
				curwin->w_cursor.col = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   ଖାଲି, ଖାଲି, &ec, 0);
			    curwin->w_cursor.col = regmatch.startpos[0].col;
			    ଯଦି (subflags.do_ସଂଖ୍ୟା || curwin->w_p_nu)
			    {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା numw = ସଂଖ୍ୟା_width(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    ସନ୍ଦେଶ_start();
			    ପାଇଁ (i = 0; i < (long)sc; ++i)
				ସନ୍ଦେଶ_putchar(' ');
			    ପାଇଁ ( ; i <= (long)ec; ++i)
				ସନ୍ଦେଶ_putchar('^');

			    resp = getexmodeline('?', ଖାଲି, 0, TRUE);
			    ଯଦି (resp != ଖାଲି)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				ଯଦି (ex_normal_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			ଅନ୍ୟଥା
			{
			    string_T orig_ଧାଡ଼ି = {ଖାଲି, 0};
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    len_change = 0;
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   save_p_lz = p_lz;
#ifdef FEAT_FOLDING
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#endif
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    ଯଦି (new_start.string != ଖାଲି)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_ଧାଡ଼ି.length = ml_get_len(lnum);
				orig_ଧାଡ଼ି.string = vim_strnsave(ml_get(lnum), orig_ଧାଡ଼ି.length);
				ଯଦି (orig_ଧାଡ଼ି.string != ଖାଲି)
				{
				    string_T new_ଧାଡ଼ି;

				    new_ଧାଡ଼ି.string = concat_str(new_start.string,
						     sub_firstline.string + copycol);
				    ଯଦି (new_ଧାଡ଼ି.string == ଖାଲି)
					VIM_CLEAR_STRING(orig_ଧାଡ଼ି);
				    ଅନ୍ୟଥା
				    {
					new_ଧାଡ଼ି.length =
					    new_start.length + (sub_firstline.length - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)new_ଧାଡ଼ି.length - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)orig_ଧାଡ଼ି.length;
					curwin->w_cursor.col += len_change;
					ml_replace(lnum, new_ଧାଡ଼ି.string, FALSE);
				    }
				}
			    }

			    ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    ସନ୍ଧାନ_match_endcol = regmatch.endpos[0].col
								 + len_change;
			    ଯଦି (ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ == 0
						   && ସନ୍ଧାନ_match_endcol == 0)
				// highlight at least one character for /^/
				ସନ୍ଧାନ_match_endcol = 1;
			    highlight_match = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_match = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ifdef FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#endif
			    ଯଦି (ସନ୍ଦେଶ_row == Rows - 1)
				ସନ୍ଦେଶ_didout = FALSE;	// avoid a scroll-up
			    ସନ୍ଦେଶ_starthere();
			    i = ସନ୍ଦେଶ_scroll;
			    ସନ୍ଦେଶ_scroll = 0;		// truncate msg when
							// needed
			    ସନ୍ଦେଶ_no_more = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    ସନ୍ଦେଶ_no_more = FALSE;
			    ସନ୍ଦେଶ_scroll = i;
			    showruler(TRUE);
			    windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
			    RedrawingDisabled = save_RedrawingDisabled;

#ifdef USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#endif
			    ++no_mapping;	// don't map this key
			    ++allow_ଚାବିଗୁଡ଼ିକ;	// allow special keys
			    typed = plain_vgetc();
			    --allow_ଚାବିଗୁଡ଼ିକ;
			    --no_mapping;

			    // clear the question
			    ସନ୍ଦେଶ_didout = FALSE;	// don't scroll up
			    ସନ୍ଦେଶ_col = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    ଯଦି (orig_ଧାଡ଼ି.string != ଖାଲି)
				ml_replace(lnum, orig_ଧାଡ଼ି.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			ଯଦି (typed == 'q' || typed == ESC || typed == Ctrl_C
#ifdef UNIX
				|| typed == intr_char
#endif
				)
			{
			    got_quit = TRUE;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (typed == 'n')
			    ଭାଙ୍ଗ;
			ଯଦି (typed == 'y')
			    ଭାଙ୍ଗ;
			ଯଦି (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (typed == Ctrl_E)
			    scrollup_clamp();
			ଅନ୍ୟଥା ଯଦି (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    ଯଦି (vim_strchr(p_cpo, CPO_UNDO) != ଖାଲି)
			--no_u_sync;

		    ଯଦି (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			ଯଦି (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.length;
			    skip_match = TRUE;
			}
			ଯାଅ skip;
		    }
		    ଯଦି (got_quit)
			ଯାଅ skip;
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
		ଯଦି (subflags.do_ଗଣନା)
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
		ଯଦି (sublen == 0 || aborting() || subflags.do_ଗଣନା)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    ଯାଅ skip;
		}
#endif

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		ଯଦି (nmatch > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - sub_firstlnum + 1;
		    skip_match = TRUE;
		    // safety check
		    ଯଦି (nmatch < 0)
			ଯାଅ skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		ଯଦି (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.length = sub_firstline.length;
#ifdef FEAT_PROP_POPUP
		    ଯଦି (curbuf->b_has_textprop)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes_added = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sublen - 1 - (regmatch.endpos[0].col
						   - regmatch.startpos[0].col);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			ଯଦି (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       bytes_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			total_added += (colnr_T)bytes_added;
		    }
#endif
		}
		ଅନ୍ୟଥା
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    ଯଦି (curbuf->b_has_textprop)
		    {
			char_u	*prop_start;

			// Props in the first line may be shortened or deleted
			ଯଦି (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			total_added -=
			    (colnr_T)(sub_firstline.length - regmatch.startpos[0].col);

			// Props in the last line may be moved or deleted
			ଯଦି (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].col, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			text_prop_ଗଣନା = get_text_props(curbuf, lastlnum,
							   &prop_start, FALSE);
			ଯଦି (text_prop_ଗଣନା > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(text_props);
			    text_props = ALLOC_MULT(textprop_T,
							      text_prop_ଗଣନା);
			    ଯଦି (text_props != ଖାଲି)
			    {
				mch_memmove(text_props, prop_start,
					 text_prop_ଗଣନା * ଆକାର(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା wi = 0;
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା pi = 0; pi < text_prop_ଗଣନା; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    ଯଦି (text_props[pi].tp_id < 0
					    || (text_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					ଜାରି;
				    text_props[wi] = text_props[pi];
				    text_props[wi].tp_col +=
					regmatch.startpos[0].col + (colnr_T)sublen - 1;
				    text_props[wi].u.tp_text = ଖାଲି;
				    ++wi;
				}
				text_prop_ଗଣନା = wi;
			    }
			}
		    }
#endif
		    tmp.string = ml_get(lastlnum);
		    tmp.length = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    ଯଦି (curbuf->b_has_textprop)
			total_added +=
			    (colnr_T)(tmp.length - regmatch.endpos[0].col);
#endif
		}

		copy_len = (size_t)(regmatch.startpos[0].col - copycol);
		needed_size = copy_len
		    + (tmp.length - (size_t)regmatch.endpos[0].col) + sublen + 1;

		ଯଦି (new_start.string == ଖାଲି)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_size = needed_size + 50;
		    ଯଦି ((new_start.string = alloc_clear(new_start_size)) == ଖାଲି)
			ଯାଅ outofmem;
		    *new_start.string = NUL;
		    new_start.length = 0;
		}
		ଅନ୍ୟଥା
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_size += new_start.length;
		    ଯଦି (needed_size > new_start_size)
		    {
			new_start_size = needed_size + 50;
			ଯଦି ((p1 = alloc_clear(new_start_size)) == ଖାଲି)
			{
			    vim_free(new_start.string);
			    ଯାଅ outofmem;
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

		ଯଦି (new_start_size - copy_len < sublen)
		    sublen = new_start_size - copy_len - 1;

#ifdef FEAT_EVAL
		++textlock;
#endif
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_end, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		ଯଦି (n > 0)
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
		ଯଦି (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    ଯଦି (sub_firstline.string == ଖାଲି)
		    {
			vim_free(new_start.string);
			ଯାଅ outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    ଯଦି (sub_firstlnum <= line2)
			do_again = TRUE;
		    ଅନ୍ୟଥା
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].col;

		ଯଦି (skip_match)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    ଯଦି (sub_firstline.string == ଖାଲି)
		    {
			vim_free(new_start.string);
			ଯାଅ outofmem;
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
		ପାଇଁ (p1 = new_end; *p1; ++p1)
		{
		    ଯଦି (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_start.length - (p1 - new_start.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_start.length;
#ifdef FEAT_PROP_POPUP
			ଯଦି (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    ଯଦି (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_start.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#endif
		    }
		    ଅନ୍ୟଥା ଯଦି (*p1 == CAR)
		    {
			ଯଦି (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_start.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_start.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    ଯଦି (subflags.do_ask)
				appended_ଧାଡ଼ିଗୁଡ଼ିକ(lnum - 1, 1L);
			    ଅନ୍ୟଥା
			    {
				ଯଦି (first_ଧାଡ଼ି == 0)
				    first_ଧାଡ଼ି = lnum;
				last_ଧାଡ଼ି = lnum + 1;
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
		    ଅନ୍ୟଥା ଯଦି (has_mbyte)
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
		ଯଦି (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_split)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, ଖାଲି)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    ଯଦି (new_start.string != ଖାଲି)
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

			ଯଦି (u_savesub(lnum) != OK)
			    ଭାଙ୍ଗ;
			ml_replace(lnum, new_start.string, TRUE);
#ifdef FEAT_PROP_POPUP
			ଯଦି (text_props != ଖାଲି)
			    add_text_props(lnum, text_props, text_prop_ଗଣନା);
#endif
			ଯଦି (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    ଯଦି (u_savedel(lnum, nmatch_tl) != OK)
				ଭାଙ୍ଗ;
			    ପାଇଁ (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    ଯଦି (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    ଅନ୍ୟଥା
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    ଯଦି (subflags.do_ask)
				deleted_ଧାଡ଼ିଗୁଡ଼ିକ(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			ଯଦି (subflags.do_ask)
			    changed_bytes(lnum, 0);
			ଅନ୍ୟଥା
			{
			    ଯଦି (first_ଧାଡ଼ି == 0)
				first_ଧାଡ଼ି = lnum;
			    last_ଧାଡ଼ି = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_start.string;
			sub_firstline.length = new_start.length;
			new_start.string = ଖାଲି;
			new_start.length = 0;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);
			copycol = 0;
			did_split = false;
		    }
		    ଯଦି (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, ଖାଲି);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    ଯଦି (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			ଯଦି (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			ଭାଙ୍ଗ;
		    }
		}

		ଧାଡ଼ି_breakcheck();
	    }

	    ଯଦି (did_sub)
		++sub_nlines;
	    vim_free(new_start.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	ଧାଡ଼ି_breakcheck();
    }

    ଯଦି (first_ଧାଡ଼ି != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - old_ଧାଡ଼ି_ଗଣନା;
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(first_ଧାଡ଼ି, 0, last_ଧାଡ଼ି - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ifdef FEAT_PROP_POPUP
    vim_free(text_props);
#endif

    // ":s/pat//n" doesn't move the cursor
    ଯଦି (subflags.do_ଗଣନା)
	curwin->w_cursor = old_cursor;

    ଯଦି (sub_nsubs > start_nsubs)
    {
	ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_start.lnum = eap->line1;
	    curbuf->b_op_end.lnum = line2;
	    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	}

	ଯଦି (!global_busy)
	{
	    // when interactive leave cursor on the match
	    ଯଦି (!subflags.do_ask)
	    {
		ଯଦି (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		ଅନ୍ୟଥା
		    beginline(BL_WHITE | BL_FIX);
	    }
	    ଯଦି (!do_sub_ସନ୍ଦେଶ(subflags.do_ଗଣନା) && subflags.do_ask)
		ସନ୍ଦେଶ("");
	}
	ଅନ୍ୟଥା
	    global_need_beginline = TRUE;
	ଯଦି (subflags.do_print)
	    print_ଧାଡ଼ି(curwin->w_cursor.lnum,
					 subflags.do_ସଂଖ୍ୟା, subflags.do_list);
    }
    ଅନ୍ୟଥା ଯଦି (!global_busy)
    {
	ଯଦି (got_int)		// interrupted
	    emsg(_(e_interrupted));
	ଅନ୍ୟଥା ଯଦି (got_match)	// did find something but nothing substituted
	    ସନ୍ଦେଶ("");
	ଅନ୍ୟଥା ଯଦି (subflags.do_error)	// nothing found
	    semsg(_(e_pattern_not_found_str), get_ସନ୍ଧାନ_pat());
    }

#ifdef FEAT_FOLDING
    ଯଦି (subflags.do_ask && hasAnyFolding(curwin))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_sub_ସନ୍ଦେଶ(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    ଯଦି (((sub_nsubs > p_report && (ଚାବି_Typed || sub_nlines > 1 || p_report < 1))
		|| ଗଣନା_only)
	    && messaging())
    {
	ଅକ୍ଷର	*ସନ୍ଦେଶ_single;
	ଅକ୍ଷର	*ସନ୍ଦେଶ_plural;

	ଯଦି (got_int)
	    STRCPY(ସନ୍ଦେଶ_buf, _("(Interrupted) "));
	ଅନ୍ୟଥା
	    *ସନ୍ଦେଶ_buf = NUL;

	ସନ୍ଦେଶ_single = ଗଣନା_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	ସନ୍ଦେଶ_plural = ଗଣନା_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_add(ସନ୍ଦେଶ_buf, ଆକାର(ସନ୍ଦେଶ_buf),
				 NGETTEXT(ସନ୍ଦେଶ_single, ସନ୍ଦେଶ_plural, sub_nlines),
				 sub_nsubs, (long)sub_nlines);

	ଯଦି (ସନ୍ଦେଶ(ସନ୍ଦେଶ_buf))
	    // save message to display it after redraw
	    set_keep_ସନ୍ଦେଶ((char_u *)ସନ୍ଦେଶ_buf, 0);
	ଫେରନ୍ତୁ TRUE;
    }
    ଯଦି (got_int)
    {
	emsg(_(e_interrupted));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    ଯଦି (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", ଖାଲି, ଖାଲି, DOCMD_NOWAIT);
    ଅନ୍ୟଥା
	do_cmdline(cmd, ଖାଲି, ଖାଲି, DOCMD_NOWAIT);
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
    ଶୂନ୍ୟ
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ndone = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*used_pat;
    regmmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    ଯଦି (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    ଅନ୍ୟଥା
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    ଯଦି (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	ଫେରନ୍ତୁ;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    ଯଦି (*cmd == '\\')
    {
	++cmd;
	ଯଦି (vim_strchr((char_u *)"/?&", *cmd) == ଖାଲି)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	ଅନ୍ୟଥା
	    which_pat = RE_ସନ୍ଧାନ;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    ଅନ୍ୟଥା ଯଦି (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (check_regexp_delim(*cmd) == FAIL)
    {
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, ଖାଲି, ଖାଲି);
	ଯଦି (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    ଯଦି (ସନ୍ଧାନ_regcomp(pat, patlen, &used_pat, RE_BOTH, which_pat, ସନ୍ଧାନ_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ଖାଲି);
	ଯଦି ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	ପାଇଁ (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ଖାଲି);
	    ଯଦି (regmatch.regprog == ଖାଲି)
		ଭାଙ୍ଗ;  // re-compiling regprog failed
	    ଯଦି ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    ଧାଡ଼ି_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	ଯଦି (got_int)
	    ସନ୍ଦେଶ(_(e_interrupted));
	ଅନ୍ୟଥା ଯଦି (ndone == 0)
	{
	    ଯଦି (type == 'v')
	    {
		ଯଦି (in_vim9script())
		    semsg(_(e_pattern_found_in_every_ଧାଡ଼ି_str), used_pat);
		ଅନ୍ୟଥା
		    smsg(_("Pattern found in every line: %s"), used_pat);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (in_vim9script())
		    semsg(_(e_pattern_not_found_str), used_pat);
		ଅନ୍ୟଥା
		    smsg(_("Pattern not found: %s"), used_pat);
	    }
	}
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
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
    ସନ୍ଦେଶ_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଯେତେବେଳେ (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    ଯଦି (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    ଅନ୍ୟଥା
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    changed_ଧାଡ଼ି_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    ଯଦି (ସନ୍ଦେଶ_col == 0 && ସନ୍ଦେଶ_scrolled == 0)
	ସନ୍ଦେଶ_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    ଯଦି (!do_sub_ସନ୍ଦେଶ(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - old_lcount);
}

#ifdef FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    ଶୂନ୍ୟ
set_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#endif // FEAT_VIMINFO

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_old_sub(ଶୂନ୍ୟ)
{
    vim_free(old_sub);
}
#endif

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prepare_tagpreview(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		undo_sync,	    // sync undo when leaving the window
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    ଯଦି (curwin->w_p_pvw)
	ଫେରନ୍ତୁ FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    ଯଦି (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_find_preview_window();
	ଯଦି (wp != ଖାଲି)
	    popup_set_wantpos_cursor(wp, wp->w_minwidth, ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (use_popup != USEPOPUP_NONE)
    {
	wp = popup_find_info_window();
	ଯଦି (wp != ଖାଲି)
	{
	    ଯଦି (use_popup == USEPOPUP_NORMAL)
		popup_show(wp);
	    ଅନ୍ୟଥା
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ଅନ୍ୟଥା
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    ଯଦି (wp->w_p_pvw)
		ଭାଙ୍ଗ;
    }
    ଯଦି (wp != ଖାଲି)
    {
	win_enter(wp, undo_sync);
	ଫେରନ୍ତୁ FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    ଯଦି ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	ଫେରନ୍ତୁ popup_create_preview_window(use_popup != USEPOPUP_NONE);
# endif
    ଯଦି (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	ଫେରନ୍ତୁ FALSE;
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
    ଫେରନ୍ତୁ TRUE;
}

#endif

/*
 * Make the user happy.
 */
    ଶୂନ୍ୟ
ex_smile(exarg_T *eap UNUSED)
{
    ସ୍ଥିର ଅକ୍ଷର *code[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    ଅକ୍ଷର *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_putchar('\n');
    ପାଇଁ (i = 0; i < 2; ++i)
	ପାଇଁ (p = code[i]; *p != NUL; ++p)
	    ଯଦି (*p == 'x')
		ସନ୍ଦେଶ_putchar('\n');
	    ଅନ୍ୟଥା
		ପାଇଁ (n = *p++; n > 0; --n)
		    ଯଦି (*p == 'o' || *p == '$')
			ସନ୍ଦେଶ_putchar_attr(*p, HL_ATTR(HLF_L));
		    ଅନ୍ୟଥା
			ସନ୍ଦେଶ_putchar(*p);
    ସନ୍ଦେଶ_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    ଶୂନ୍ୟ
ex_drop(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		split = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    ଯଦି (ERROR_IF_POPUP_WINDOW || ERROR_IF_TERM_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

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
    ଯଦି (ARGCOUNT == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	ଫେରନ୍ତୁ;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ଯଦି (wp->w_buffer == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    ଯଦି (!bufIsChanged(curbuf))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
		ex_rewind(eap);
	    ଫେରନ୍ତୁ;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    ଯଦି (!buf_hide(curbuf))
    {
	++emsg_off;
	split = check_changed(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    ଯଦି (split)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    ଅନ୍ୟଥା
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
skip_vimgrep_pat(char_u *p, char_u **s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flags)
{
    ଫେରନ୍ତୁ skip_vimgrep_pat_ext(p, s, flags, ଖାଲି, ଖାଲି);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flags, char_u **nulp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	ଯଦି (s != ଖାଲି)
	    *s = p;
	p = skiptowhite(p);
	ଯଦି (s != ଖାଲି && *p != NUL)
	{
	    ଯଦି (nulp != ଖାଲି)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    ଅନ୍ୟଥା
    {
	// ":vimgrep /pattern/[g][j] fname"
	ଯଦି (s != ଖାଲି)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	ଯଦି (*p != c)
	    ଫେରନ୍ତୁ ଖାଲି;

	// Truncate the pattern.
	ଯଦି (s != ଖାଲି)
	{
	    ଯଦି (nulp != ଖାଲି)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	ଯେତେବେଳେ (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    ଯଦି (flags != ଖାଲି)
	    {
		ଯଦି (*p == 'g')
		    *flags |= VGR_GLOBAL;
		ଅନ୍ୟଥା ଯଦି (*p == 'j')
		    *flags |= VGR_NOJUMP;
		ଅନ୍ୟଥା
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    ଫେରନ୍ତୁ p;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    ଶୂନ୍ୟ
ex_oldfiles(exarg_T *eap UNUSED)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matches = -1;

    ଯଦି (l == ଖାଲି)
    {
	ସନ୍ଦେଶ(_("No old files"));
	ଫେରନ୍ତୁ;
    }

    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_scroll = TRUE;
    ପାଇଁ (li = l->lv_first; li != ଖାଲି && !got_int; li = li->li_next)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	ଯଦି (!message_filtered(fname))
	{
	    ଯଦି (matches < 0)
		matches = nr;
	    ଅନ୍ୟଥା
		matches = 0;
	    ସନ୍ଦେଶ_outnum((long)nr);
	    ସନ୍ଦେଶ_puts(": ");
	    ସନ୍ଦେଶ_outtrans(fname);
	    ସନ୍ଦେଶ_clr_eos();
	    ସନ୍ଦେଶ_putchar('\n');
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_more = FALSE;
	// we only need to prompt if there is more than 1 match
	ଯଦି (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	ଅନ୍ୟଥା
	    nr = prompt_for_ସଂଖ୍ୟା(FALSE);
	ସନ୍ଦେଶ_starthere();
	ଯଦି (nr > 0)
	{
	    char_u *p = list_find_str(get_vim_var_list(VV_OLDFILES),
		    (long)nr);

	    ଯଦି (p != ଖାଲି)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, ଖାଲି);
		vim_free(p);
	    }
	}
    }
# endif
}
#endif
