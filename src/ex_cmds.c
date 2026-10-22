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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#अन्तर्भाव <float.h>

स्थिर पूर्णाङ्क linelen(पूर्णाङ्क *has_tab);
स्थिर शून्य do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, पूर्णाङ्क do_in, पूर्णाङ्क do_out);
स्थिर पूर्णाङ्क not_writing(शून्य);
स्थिर पूर्णाङ्क check_readonly(पूर्णाङ्क *forceit, buf_T *buf);
स्थिर शून्य delbuf_संदेश(char_u *name);

/*
 * ":ascii" and "ga".
 */
    शून्य
do_ascii(exarg_T *eap UNUSED)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		cval;
    वर्ण	buf1[20];
    वर्ण	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    पूर्णाङ्क		cc[MAX_MCO];
    पूर्णाङ्क		ci = 0;
    पूर्णाङ्क		len;

    यदि (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    अन्यथा
	c = gchar_cursor();
    यदि (c == NUL)
    {
	संदेश("NUL");
	प्रत्यागम;
    }

    IObuff[0] = NUL;
    यदि (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	यदि (c == NL)	    // NUL is stored as NL
	    c = NUL;
	यदि (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	अन्यथा
	    cval = c;
	यदि (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, परिमाण(buf1), "  <%s>", (वर्ण *)buf3);
	}
	अन्यथा
	    buf1[0] = NUL;
	यदि (c >= 0x80)
	    vim_snprintf(buf2, परिमाण(buf2), "  <M-%s>",
						 (वर्ण *)transchar(c & 0x7f));
	अन्यथा
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	यदि (dig != रिक्त)
	    vim_snprintf((वर्ण *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	अन्यथा
#endif
	    vim_snprintf((वर्ण *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	यदि (enc_utf8)
	    c = cc[ci++];
	अन्यथा
	    c = 0;
    }

    // Repeat for combining characters.
    यावत् (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (पूर्णाङ्क)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	यदि (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	यदि (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	यदि (dig != रिक्त)
	    vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	अन्यथा
#endif
	    vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	यदि (ci == MAX_MCO)
	    भङ्ग;
	यदि (enc_utf8)
	    c = cc[ci++];
	अन्यथा
	    c = 0;
    }

    संदेश((वर्ण *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    शून्य
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    पूर्णाङ्क		len;
    पूर्णाङ्क		indent = 0;
    पूर्णाङ्क		new_indent;
    पूर्णाङ्क		has_tab;
    पूर्णाङ्क		width;

#ifdef FEAT_RIGHTLEFT
    यदि (curwin->w_p_rl)
    {
	// switch left and right aligning
	यदि (eap->cmdidx == CMD_उजवा)
	    eap->cmdidx = CMD_डावा;
	अन्यथा यदि (eap->cmdidx == CMD_डावा)
	    eap->cmdidx = CMD_उजवा;
    }
#endif

    width = atoi((वर्ण *)eap->arg);
    save_curpos = curwin->w_cursor;
    यदि (eap->cmdidx == CMD_डावा)    // width is used for new indent
    {
	यदि (width >= 0)
	    indent = width;
    }
    अन्यथा
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	यदि (width <= 0)
	    width = curbuf->b_p_tw;
	यदि (width == 0 && curbuf->b_p_wm > 0)
	    width = curwin->w_width - curbuf->b_p_wm;
	यदि (width <= 0)
	    width = 80;
    }

    यदि (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	प्रत्यागम;

    प्रति (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	यदि (eap->cmdidx == CMD_डावा)		// left align
	    new_indent = indent;
	अन्यथा
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_उजवा ? &has_tab
						   : रिक्त) - get_indent();

	    यदि (len <= 0)			// skip blank lines
		अनुवर्त;

	    यदि (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    अन्यथा
	    {
		new_indent = width - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		यदि (has_tab)
		    यावत् (new_indent > 0)
		    {
			(शून्य)set_indent(new_indent, 0);
			यदि (linelen(रिक्त) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    कुरु
				(शून्य)set_indent(++new_indent, 0);
			    यावत् (linelen(रिक्त) <= width);
			    --new_indent;
			    भङ्ग;
			}
			--new_indent;
		    }
	    }
	}
	यदि (new_indent < 0)
	    new_indent = 0;
	(शून्य)set_indent(new_indent, 0);		// set indent
    }
    changed_ओळी(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    स्थिर पूर्णाङ्क
linelen(पूर्णाङ्क *has_tab)
{
    char_u  *ओळ;
    char_u  *first;
    char_u  *last;
    पूर्णाङ्क	    save;
    पूर्णाङ्क	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    ओळ = ml_get_curline();
    यदि (*ओळ == NUL)
	प्रत्यागम 0;

    // find the first non-blank character
    first = skipwhite(ओळ);

    // find the character after the last non-blank character
    प्रति (last = first + STRLEN(first);
				last > first && VIM_ISWHITE(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize_str(ओळ);	// get line length on screen
    यदि (has_tab != रिक्त)		// check for embedded TAB
	*has_tab = (vim_strchr(first, TAB) != रिक्त);
    *last = save;

    प्रत्यागम len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
स्थिर char_u	*sortbuf1;
स्थिर char_u	*sortbuf2;

स्थिर पूर्णाङ्क	sort_lc;	// sort using locale
स्थिर पूर्णाङ्क	sort_ic;	// ignore case
स्थिर पूर्णाङ्क	sort_nr;	// sort on number
स्थिर पूर्णाङ्क	sort_rx;	// sort on regex instead of skipping it
स्थिर पूर्णाङ्क	sort_flt;	// sort on floating number

स्थिर पूर्णाङ्क	sort_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
प्रकारनिर्देश संरचना
{
    linenr_T	lnum;			// line number
    सङ्घ {
	संरचना
	{
	    varnumber_T	start_col_nr;	// starting column number
	    varnumber_T	end_col_nr;	// ending column number
	} ओळ;
	संरचना
	{
	    varnumber_T	मूल्य;		// value if sorting by integer
	    पूर्णाङ्क is_संख्या;		// TRUE when line contains a number
	} अंक;
	float_T मूल्य_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    स्थिर पूर्णाङ्क
string_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    यदि (sort_lc)
	प्रत्यागम strcoll((वर्ण *)s1, (वर्ण *)s2);
    प्रत्यागम sort_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    स्थिर पूर्णाङ्क
sort_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    पूर्णाङ्क		result = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    यदि (sort_abort)
	प्रत्यागम 0;
    fast_breakcheck();
    यदि (got_int)
	sort_abort = TRUE;

    यदि (sort_nr)
    {
	यदि (l1.st_u.अंक.is_संख्या != l2.st_u.अंक.is_संख्या)
	    result = l1.st_u.अंक.is_संख्या > l2.st_u.अंक.is_संख्या ? 1 : -1;
	अन्यथा
	    result = l1.st_u.अंक.मूल्य == l2.st_u.अंक.मूल्य ? 0
			     : l1.st_u.अंक.मूल्य > l2.st_u.अंक.मूल्य ? 1 : -1;
    }
    अन्यथा यदि (sort_flt)
	result = l1.st_u.मूल्य_flt == l2.st_u.मूल्य_flt ? 0
			     : l1.st_u.मूल्य_flt > l2.st_u.मूल्य_flt ? 1 : -1;
    अन्यथा
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.ओळ.start_col_nr,
		     l1.st_u.ओळ.end_col_nr - l1.st_u.ओळ.start_col_nr + 1);
	sortbuf1[l1.st_u.ओळ.end_col_nr - l1.st_u.ओळ.start_col_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.ओळ.start_col_nr,
		     l2.st_u.ओळ.end_col_nr - l2.st_u.ओळ.start_col_nr + 1);
	sortbuf2[l2.st_u.ओळ.end_col_nr - l2.st_u.ओळ.start_col_nr] = 0;

	result = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    यदि (result == 0)
	प्रत्यागम (पूर्णाङ्क)(l1.lnum - l2.lnum);
    प्रत्यागम result;
}

/*
 * ":sort".
 */
    शून्य
ex_sort(exarg_T *eap)
{
    regmatch_T	regmatch;
    पूर्णाङ्क		len;
    linenr_T	lnum;
    long	maxlen = 0;
    sorti_T	*nrs;
    size_t	मोजणी = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    पूर्णाङ्क		unique = FALSE;
    long	deleted;
    colnr_T	start_col;
    colnr_T	end_col;
    पूर्णाङ्क		sort_what = 0;
    पूर्णाङ्क		format_found = 0;
    पूर्णाङ्क		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    यदि (मोजणी <= 1)
	प्रत्यागम;

    यदि (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	प्रत्यागम;
    sortbuf1 = रिक्त;
    sortbuf2 = रिक्त;
    regmatch.regprog = रिक्त;
    nrs = ALLOC_MULT(sorti_T, मोजणी);
    यदि (nrs == रिक्त)
	गच्छ sortend;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    प्रति (p = eap->arg; *p != NUL; ++p)
    {
	यदि (VIM_ISWHITE(*p))
	    ;
	अन्यथा यदि (*p == 'i')
	    sort_ic = TRUE;
	अन्यथा यदि (*p == 'l')
	    sort_lc = TRUE;
	अन्यथा यदि (*p == 'r')
	    sort_rx = TRUE;
	अन्यथा यदि (*p == 'n')
	{
	    sort_nr = 1;
	    ++format_found;
	}
	अन्यथा यदि (*p == 'f')
	{
	    sort_flt = 1;
	    ++format_found;
	}
	अन्यथा यदि (*p == 'b')
	{
	    sort_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_found;
	}
	अन्यथा यदि (*p == 'o')
	{
	    sort_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_found;
	}
	अन्यथा यदि (*p == 'x')
	{
	    sort_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_found;
	}
	अन्यथा यदि (*p == 'u')
	    unique = TRUE;
	अन्यथा यदि (*p == '"')	// comment start
	    भङ्ग;
	अन्यथा यदि (eap->nextcmd == रिक्त && check_nextcmd(p) != रिक्त)
	{
	    eap->nextcmd = check_nextcmd(p);
	    भङ्ग;
	}
	अन्यथा यदि (!ASCII_ISALPHA(*p) && regmatch.regprog == रिक्त)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    यदि (s == रिक्त)
		गच्छ sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    यदि (s == p + 1)
	    {
		यदि (last_शोध_pat() == रिक्त)
		{
		    emsg(_(e_no_previous_regular_expression));
		    गच्छ sortend;
		}
		regmatch.regprog = vim_regcomp(last_शोध_pat(), RE_MAGIC);
	    }
	    अन्यथा
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    यदि (regmatch.regprog == रिक्त)
		गच्छ sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	अन्यथा
	{
	    semsg(_(e_invalid_argument_str), p);
	    गच्छ sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    यदि (format_found > 1)
    {
	emsg(_(e_invalid_argument));
	गच्छ sortend;
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
    प्रति (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	यदि (maxlen < len)
	    maxlen = len;

	start_col = 0;
	end_col = len;
	यदि (regmatch.regprog != रिक्त && vim_regexec(&regmatch, s, 0))
	{
	    यदि (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    अन्यथा
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	अन्यथा
	    यदि (regmatch.regprog != रिक्त)
		end_col = 0;

	यदि (sort_nr || sort_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + end_col;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + start_col;
	    यदि (sort_nr)
	    {
		यदि (sort_what & STR2NR_HEX)
		    s = skiptohex(p);
		अन्यथा यदि (sort_what & STR2NR_BIN)
		    s = skiptobin(p);
		अन्यथा
		    s = skiptodigit(p);
		यदि (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		यदि (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.अंक.is_संख्या = FALSE;
		    nrs[lnum - eap->line1].st_u.अंक.मूल्य = 0;
		}
		अन्यथा
		{
		    nrs[lnum - eap->line1].st_u.अंक.is_संख्या = TRUE;
		    vim_str2nr(s, रिक्त, रिक्त, sort_what,
			&nrs[lnum - eap->line1].st_u.अंक.मूल्य,
			रिक्त, 0, FALSE, रिक्त);
		}
	    }
	    अन्यथा
	    {
		s = skipwhite(p);
		यदि (*s == '+')
		    s = skipwhite(s + 1);

		यदि (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.मूल्य_flt = -DBL_MAX;
		अन्यथा
		    nrs[lnum - eap->line1].st_u.मूल्य_flt =
						      strtod((वर्ण *)s, रिक्त);
	    }
	    *s2 = c;
	}
	अन्यथा
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.ओळ.start_col_nr = start_col;
	    nrs[lnum - eap->line1].st_u.ओळ.end_col_nr = end_col;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	यदि (regmatch.regprog != रिक्त)
	    fast_breakcheck();
	यदि (got_int)
	    गच्छ sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    यदि (sortbuf1 == रिक्त)
	गच्छ sortend;
    sortbuf2 = alloc(maxlen + 1);
    यदि (sortbuf2 == रिक्त)
	गच्छ sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((शून्य *)nrs, मोजणी, परिमाण(sorti_T), sort_compare);

    यदि (sort_abort)
	गच्छ sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    प्रति (i = 0; i < मोजणी; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? मोजणी - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	यदि (get_lnum + ((linenr_T)मोजणी - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	यदि (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    यदि (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		भङ्ग;
	}
	fast_breakcheck();
	यदि (got_int)
	    गच्छ sortend;
    }

    // delete the original lines if appending worked
    यदि (i == मोजणी)
	प्रति (i = 0; i < मोजणी; ++i)
	    ml_delete(eap->line1);
    अन्यथा
	मोजणी = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (long)(मोजणी - (lnum - eap->line2));
    यदि (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    अन्यथा यदि (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    यदि (change_occurred || deleted != 0)
	changed_ओळी(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    यदि (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    शून्य
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    पूर्णाङ्क		len;
    linenr_T	lnum;
    long	maxlen = 0;
    linenr_T	मोजणी = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    पूर्णाङ्क		keep_only_unique = FALSE;
    पूर्णाङ्क		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    long	deleted = 0;
    colnr_T	start_col;
    colnr_T	end_col;
    पूर्णाङ्क		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    यदि (मोजणी <= 1)
	प्रत्यागम;

    यदि (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	प्रत्यागम;
    sortbuf1 = रिक्त;
    regmatch.regprog = रिक्त;

    sort_abort = sort_ic = sort_lc = sort_rx = sort_nr = 0;
    sort_flt = 0;

    प्रति (p = eap->arg; *p != NUL; ++p)
    {
	यदि (VIM_ISWHITE(*p))
	    ;
	अन्यथा यदि (*p == 'i')
	    sort_ic = TRUE;
	अन्यथा यदि (*p == 'l')
	    sort_lc = TRUE;
	अन्यथा यदि (*p == 'r')
	    sort_rx = TRUE;
	अन्यथा यदि (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    यदि (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	अन्यथा यदि (*p == '"')	// comment start
	    भङ्ग;
	अन्यथा यदि (eap->nextcmd == रिक्त && check_nextcmd(p) != रिक्त)
	{
	    eap->nextcmd = check_nextcmd(p);
	    भङ्ग;
	}
	अन्यथा यदि (!ASCII_ISALPHA(*p) && regmatch.regprog == रिक्त)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    यदि (s == रिक्त)
		गच्छ uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    यदि (s == p + 1)
	    {
		यदि (last_शोध_pat() == रिक्त)
		{
		    emsg(_(e_no_previous_regular_expression));
		    गच्छ uniqend;
		}
		regmatch.regprog = vim_regcomp(last_शोध_pat(), RE_MAGIC);
	    }
	    अन्यथा
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    यदि (regmatch.regprog == रिक्त)
		गच्छ uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	अन्यथा
	{
	    semsg(_(e_invalid_argument_str), p);
	    गच्छ uniqend;
	}
    }

    // Find the length of the longest line.
    प्रति (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	यदि (maxlen < len)
	    maxlen = len;

	यदि (got_int)
	    गच्छ uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    यदि (sortbuf1 == रिक्त)
	गच्छ uniqend;

    // Delete lines according to options.
    पूर्णाङ्क match_continue = FALSE;
    पूर्णाङ्क next_is_unmatch = FALSE;
    पूर्णाङ्क is_match;
    linenr_T done_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    प्रति (linenr_T i = 0; i < मोजणी; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	start_col = 0;
	end_col = len;
	यदि (regmatch.regprog != रिक्त && vim_regexec(&regmatch, s, 0))
	{
	    यदि (sort_rx)
	    {
		start_col = (colnr_T)(regmatch.startp[0] - s);
		end_col = (colnr_T)(regmatch.endp[0] - s);
	    }
	    अन्यथा
		start_col = (colnr_T)(regmatch.endp[0] - s);
	}
	अन्यथा
	    यदि (regmatch.regprog != रिक्त)
		end_col = 0;
	यदि (end_col > 0)
	{
	    save_c = s[end_col];
	    s[end_col] = NUL;
	}

	is_match = i > 0 ? !string_compare(&s[start_col], sortbuf1) : FALSE;
	delete_lnum = 0;
	यदि (next_is_unmatch)
	{
	    is_match = FALSE;
	    next_is_unmatch = FALSE;
	}

	यदि (!keep_only_unique && !keep_only_not_unique)
	{
	    यदि (is_match)
		delete_lnum = get_lnum;
	    अन्यथा
		STRCPY(sortbuf1, &s[start_col]);
	}
	अन्यथा यदि (keep_only_not_unique)
	{
	    यदि (is_match)
	    {
		done_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    अन्यथा
	    {
		यदि (i > 0 && !match_continue && get_lnum - 1 > done_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    next_is_unmatch = TRUE;
		}
		अन्यथा यदि (i >= मोजणी - 1)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}
	अन्यथा // keep_only_unique
	{
	    यदि (is_match)
	    {
		यदि (!match_continue)
		    delete_lnum = get_lnum - 1;
		अन्यथा
		    delete_lnum = get_lnum;
		match_continue = TRUE;
	    }
	    अन्यथा
	    {
		यदि (i == 0 && match_continue)
		    delete_lnum = get_lnum;
		match_continue = FALSE;
		STRCPY(sortbuf1, &s[start_col]);
	    }
	}

	यदि (end_col > 0)
	    s[end_col] = save_c;

	यदि (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    मोजणी--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	यदि (got_int)
	    गच्छ uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (long)MAXLNUM, -deleted);
    msgmore(-deleted);

    यदि (change_occurred)
	changed_ओळी(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    यदि (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	अंक_ओळी;  // Num lines moved
    linenr_T	last_ओळ;  // Last line in file after adding new text
#ifdef FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#endif

    यदि (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_of_ओळी_into_itself));
	प्रत्यागम FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    यदि (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	यदि (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	अन्यथा
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	प्रत्यागम OK;
    }

    अंक_ओळी = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    यदि (u_save(dest, dest + 1) == FAIL)
	प्रत्यागम FAIL;
    प्रति (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	यदि (str != रिक्त)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    यदि (dest < line1)
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
    last_ओळ = curbuf->b_ml.ml_ओळ_मोजणी;
    mark_adjust_nofold(line1, line2, last_ओळ - line2, 0L);
    यदि (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -अंक_ओळी, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    यदि (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#endif
	यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest - अंक_ओळी + 1;
	    curbuf->b_op_end.lnum = dest;
	}
    }
    अन्यथा
    {
	mark_adjust_nofold(dest + 1, line1 - 1, अंक_ओळी, 0L);
#ifdef FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    यदि (win->w_buffer == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#endif
	यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_start.lnum = dest + 1;
	    curbuf->b_op_end.lnum = dest + अंक_ओळी;
	}
    }
    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust_nofold(last_ओळ - अंक_ओळी + 1, last_ओळ,
					     -(last_ओळ - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    यदि (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	प्रत्यागम FAIL;

    प्रति (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_MESSAGE);

    यदि (!global_busy && अंक_ओळी > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", अंक_ओळी),
			(long)अंक_ओळी);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    यदि (dest >= line1)
	curwin->w_cursor.lnum = dest;
    अन्यथा
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    यदि (line1 < dest)
    {
	dest += अंक_ओळी + 1;
	last_ओळ = curbuf->b_ml.ml_ओळ_मोजणी;
	यदि (dest > last_ओळ + 1)
	    dest = last_ओळ + 1;
	changed_ओळी(line1, 0, dest, 0L);
    }
    अन्यथा
	changed_ओळी(dest + 1, 0, line1 + अंक_ओळी, 0L);

    प्रत्यागम OK;
}

/*
 * ":copy"
 */
    शून्य
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	मोजणी;
    char_u	*p;

    मोजणी = line2 - line1 + 1;
    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = n + 1;
	curbuf->b_op_end.lnum = n + मोजणी;
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
    यदि (u_save(n, n + 1) == FAIL)
	प्रत्यागम;

    curwin->w_cursor.lnum = n;
    यावत् (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	यदि (p != रिक्त)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	यदि (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	यदि (curwin->w_cursor.lnum < line1)
	    ++line1;
	यदि (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_ओळी_mark(n, मोजणी);
    यदि (VIsual_active)
	check_pos(curbuf, &VIsual);

    msgmore((long)मोजणी);
}

स्थिर char_u	*prevcmd = रिक्त;	// the previous command

#यदि defined(EXITFREE)
    शून्य
free_prev_shellcmd(शून्य)
{
    vim_free(prevcmd);
}
#endif

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    स्थिर पूर्णाङ्क
prevcmd_is_set(शून्य)
{
    यदि (prevcmd == रिक्त)
    {
	emsg(_(e_no_previous_command));
	प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    शून्य
do_bang(
    पूर्णाङ्क		addr_मोजणी,
    exarg_T	*eap,
    पूर्णाङ्क		forceit,
    पूर्णाङ्क		do_in,
    पूर्णाङ्क		do_out)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = रिक्त;		// the new command
    पूर्णाङ्क			free_newcmd = FALSE;    // need to free() newcmd
    पूर्णाङ्क			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    पूर्णाङ्क			len;
    पूर्णाङ्क			scroll_save = संदेश_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (addr_मोजणी == 0)		// :!
    {
	संदेश_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	संदेश_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    कुरु
    {
	len = (पूर्णाङ्क)STRLEN(trailarg) + 1;
	यदि (newcmd != रिक्त)
	    len += (पूर्णाङ्क)STRLEN(newcmd);
	यदि (ins_prevcmd)
	{
	    यदि (!prevcmd_is_set())
	    {
		vim_free(newcmd);
		प्रत्यागम;
	    }
	    len += (पूर्णाङ्क)STRLEN(prevcmd);
	}
	यदि ((t = alloc(len)) == रिक्त)
	{
	    vim_free(newcmd);
	    प्रत्यागम;
	}
	*t = NUL;
	यदि (newcmd != रिक्त)
	    STRCAT(t, newcmd);
	यदि (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = रिक्त;
	यावत् (*p)
	{
	    यदि (*p == '!')
	    {
		यदि (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		अन्यथा
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    भङ्ग;
		}
	    }
	    ++p;
	}
    } यावत् (trailarg != रिक्त);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    यदि (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    अन्यथा
	free_newcmd = TRUE;

    यदि (bangredo)	    // put cmd in redo buffer for ! command
    {
	यदि (!prevcmd_is_set())
	    गच्छ theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	यदि (cmd != रिक्त)
	{
	    AppendToRedobuffLit(cmd, -1);
	    vim_free(cmd);
	}
	अन्यथा
	    AppendToRedobuffLit(prevcmd, -1);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    यदि (*p_shq != NUL)
    {
	यदि (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	यदि (newcmd == रिक्त)
	    प्रत्यागम;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    यदि (addr_मोजणी == 0)		// :!
    {
	// echo the command
	संदेश_start();
	संदेश_putchar(':');
	संदेश_putchar('!');
	संदेश_outtrans(newcmd);
	संदेश_clr_eos();
	windgoto(संदेश_row, cmdline_col_off + संदेश_col);

	do_shell(newcmd, 0);
    }
    अन्यथा				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
	apply_autocmds(EVENT_SHELLFILTERPOST, रिक्त, रिक्त, FALSE, curbuf);
    }

theend:
    यदि (free_newcmd)
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
    स्थिर शून्य
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    पूर्णाङ्क		do_in,
    पूर्णाङ्क		do_out)
{
    char_u	*itmp = रिक्त;
    char_u	*otmp = रिक्त;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    पूर्णाङ्क		shell_flags = 0;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    पूर्णाङ्क		save_cmod_flags = cmdmod.cmod_flags;
#ifdef FEAT_FILTERPIPE
    पूर्णाङ्क		stmp = p_stmp;
#endif

    यदि (*cmd == NUL)	    // no filter command
	प्रत्यागम;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_ओळ_abv_curs();
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

    यदि (do_out)
	shell_flags |= SHELL_DOOUT;

#ifdef FEAT_FILTERPIPE
# ifdef VIMDLL
    यदि (!gui.in_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    यदि (!do_in && do_out && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    अन्यथा यदि (do_in && !do_out && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
    }
    अन्यथा यदि (do_in && do_out && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_start.lnum = line1;
	curbuf->b_op_end.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    अन्यथा
#endif
	यदि ((do_in && (itmp = vim_tempname('i', FALSE)) == रिक्त)
		|| (do_out && (otmp = vim_tempname('o', FALSE)) == रिक्त))
	{
	    emsg(_(e_cant_get_temp_file_name));
	    गच्छ filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    यदि (itmp != रिक्त && buf_write(curbuf, itmp, रिक्त, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	संदेश_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#यदि defined(FEAT_EVAL)
	यदि (!aborting())
#endif
	    // will call wait_return()
	    (शून्य)semsg(_(e_cant_create_file_str), itmp);
	गच्छ filterend;
    }
    यदि (curbuf != old_curbuf)
	गच्छ filterend;

    यदि (!do_out)
	संदेश_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    यदि (cmd_buf == रिक्त)
	गच्छ filterend;

    windgoto((पूर्णाङ्क)Rows - 1, cmdline_col_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    यदि (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	redraw_later_clear();

    यदि (do_out)
    {
	यदि (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    गच्छ error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_ओळ_मोजणी;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    यदि (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    यदि (do_out)
    {
	यदि (otmp != रिक्त)
	{
	    यदि (readfile(otmp, रिक्त, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#यदि defined(FEAT_EVAL)
		यदि (!aborting())
#endif
		{
		    संदेश_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		गच्छ error;
	    }
	    यदि (curbuf != old_curbuf)
		गच्छ filterend;
	}

	read_linecount = curbuf->b_ml.ml_ओळ_मोजणी - read_linecount;

	यदि (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_start.lnum = line2 + 1;
	    curbuf->b_op_end.lnum = curwin->w_cursor.lnum;
	    appended_ओळी_mark(line2, read_linecount);
	}

	यदि (do_in)
	{
	    यदि ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == रिक्त)
	    {
		यदि (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		अन्यथा यदि (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		अन्यथा
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
	    del_ओळी(linecount, TRUE);
	    यदि (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_start.lnum = curbuf->b_op_end.lnum =
					MIN(line1, curbuf->b_ml.ml_ओळ_मोजणी);
		curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	    }
	    अन्यथा
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
	अन्यथा
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	यदि (linecount > p_report)
	{
	    यदि (do_in)
	    {
		vim_snprintf(संदेश_buf, परिमाण(संदेश_buf),
				    _("%ld lines filtered"), (long)linecount);
		यदि (संदेश(संदेश_buf) && !संदेश_scroll)
		    // save message to display it after redraw
		    set_keep_संदेश((char_u *)संदेश_buf, 0);
	    }
	    अन्यथा
		msgmore((long)linecount);
	}
    }
    अन्यथा
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    यदि (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_buffer));
    }
    अन्यथा यदि (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_start = orig_start;
	curbuf->b_op_end = orig_end;
    }

    यदि (itmp != रिक्त)
	mch_remove(itmp);
    यदि (otmp != रिक्त)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    शून्य
do_shell(
    char_u	*cmd,
    पूर्णाङ्क		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#यदि !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    पूर्णाङ्क		save_nwr;
#endif
#ifdef MSWIN
    पूर्णाङ्क		winstart = FALSE;
#endif
    पूर्णाङ्क		keep_termcap = !termcap_active;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    यदि (check_restricted() || check_secure())
    {
	संदेश_end();
	प्रत्यागम;
    }

#ifdef MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    यदि (cmd != रिक्त)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# यदि defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    यदि (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != रिक्त)
	keep_termcap = TRUE;
# endif
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    संदेश_putchar('\r');			// put cursor at start of line
    यदि (!autocmd_busy)
    {
	यदि (!keep_termcap)
	    stoptermcap();
    }
#ifdef MSWIN
    यदि (!winstart)
#endif
	संदेश_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    यदि (p_warn && !autocmd_busy && संदेश_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    यदि (bufIsChangedNotTerm(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		यदि (!keep_termcap)
		    starttermcap();	// don't want a message box here
#endif
		संदेश_puts(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		यदि (!keep_termcap)
		    stoptermcap();
#endif
		भङ्ग;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    यदि (!swapping_screen())
	windgoto(संदेश_row, cmdline_col_off + संदेश_col);
    cursor_on();
    (शून्य)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    यदि (!swapping_screen())
    {
	संदेश_row = Rows - 1;
	संदेश_col = 0;
    }

    यदि (autocmd_busy)
    {
	यदि (संदेश_silent == 0)
	    redraw_later_clear();
    }
    अन्यथा
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#यदि !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	यदि (!gui.in_use)
# endif
	{
	    यदि (cmd == रिक्त
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		यदि (संदेश_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    अन्यथा
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		यदि (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		पूर्णाङ्क redraw = संदेश_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		यदि (gui.in_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(term_console ? -1 : redraw); // see below
# अन्यथा
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#endif // FEAT_GUI_MSWIN

	यदि (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	यदि (skip_redraw)		// ':' hit in wait_return()
	{
	    यदि (संदेश_silent == 0)
		redraw_later_clear();
	}
	अन्यथा यदि (term_console)
	{
	    OUT_STR("\033[0 q");	// get window size
	    यदि (got_int && संदेश_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    अन्यथा
		must_redraw = 0;	// no extra redraw needed
	}
#endif
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, रिक्त, रिक्त, FALSE, curbuf);
}

#यदि !defined(UNIX)
    स्थिर char_u *
find_pipe(char_u *cmd)
{
    char_u  *p;
    पूर्णाङ्क	    inquote = FALSE;

    प्रति (p = cmd; *p != NUL; ++p)
    {
	यदि (!inquote && *p == '|')
	    प्रत्यागम p;
	यदि (*p == '"')
	    inquote = !inquote;
	अन्यथा यदि (rem_backslash(p))
	    ++p;
    }
    प्रत्यागम रिक्त;
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
    पूर्णाङ्क		is_powershell = FALSE;
#ifdef UNIX
    पूर्णाङ्क		is_fish_shell;
#endif

    char_u *shell_name = get_isolated_shell_name();
    यदि (shell_name == रिक्त)
	प्रत्यागम रिक्त;

#यदि defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    यदि (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    अन्यथा
#endif
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	यदि (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	अन्यथा
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    यदि (itmp != रिक्त)
    {
	यदि (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	अन्यथा
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    यदि (otmp != रिक्त)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    यदि (buf == रिक्त)
	प्रत्यागम रिक्त;

    यदि (is_powershell)
    {
	यदि (itmp != रिक्त)
	    vim_snprintf((वर्ण *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	अन्यथा
	    vim_snprintf((वर्ण *)buf, len, "& { %s }", cmd);
    }
    अन्यथा
    {
#यदि defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	यदि (itmp != रिक्त || otmp != रिक्त)
	{
	    यदि (is_fish_shell)
		vim_snprintf((वर्ण *)buf, len, "begin; %s; end", (वर्ण *)cmd);
	    अन्यथा
		vim_snprintf((वर्ण *)buf, len, "(%s)", (वर्ण *)cmd);
	}
	अन्यथा
	    STRCPY(buf, cmd);
	यदि (itmp != रिक्त)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#अन्यथा
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	यदि (*p_sxe != NUL && *p_sxq == '(')
	{
	    यदि (itmp != रिक्त || otmp != रिक्त)
		vim_snprintf((वर्ण *)buf, len, "(%s)", (वर्ण *)cmd);
	    अन्यथा
		STRCPY(buf, cmd);
	    यदि (itmp != रिक्त)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	अन्यथा
	{
	    STRCPY(buf, cmd);
	    यदि (itmp != रिक्त)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		यदि (*p_shq == NUL)
		{
		    p = find_pipe(buf);
		    यदि (p != रिक्त)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		यदि (*p_shq == NUL)
		{
		    p = find_pipe(cmd);
		    यदि (p != रिक्त)
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
    यदि (otmp != रिक्त)
	append_redir(buf, (पूर्णाङ्क)len, p_srr, otmp);

    प्रत्यागम buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    शून्य
append_redir(
    char_u	*buf,
    पूर्णाङ्क		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*end;

    end = buf + STRLEN(buf);
    // find "%s"
    प्रति (p = opt; (p = vim_strchr(p, '%')) != रिक्त; ++p)
    {
	यदि (p[1] == 's') // found %s
	    भङ्ग;
	यदि (p[1] == '%') // skip %%
	    ++p;
    }
    यदि (p != रिक्त)
    {
#ifdef MSWIN
	*end++ = ' '; // not really needed? Not with sh, ksh or bash
#endif
	vim_snprintf((वर्ण *)end, (size_t)(buflen - (end - buf)),
						  (वर्ण *)opt, (वर्ण *)fname);
    }
    अन्यथा
	vim_snprintf((वर्ण *)end, (size_t)(buflen - (end - buf)), " %s %s",
		(वर्ण *)opt, (वर्ण *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    शून्य
do_fixdel(exarg_T *eap UNUSED)
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != रिक्त
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    शून्य
print_ओळ_no_prefix(
    linenr_T	lnum,
    पूर्णाङ्क		use_संख्या,
    पूर्णाङ्क		list)
{
    वर्ण	numbuf[30];

    यदि (curwin->w_p_nu || use_संख्या)
    {
	vim_snprintf(numbuf, परिमाण(numbuf),
				   "%*ld ", संख्या_width(curwin), (long)lnum);
	संदेश_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    संदेश_prt_ओळ(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    शून्य
print_ओळ(linenr_T lnum, पूर्णाङ्क use_संख्या, पूर्णाङ्क list)
{
    पूर्णाङ्क		save_silent = silent_mode;

    // apply :filter /pat/
    यदि (message_filtered(ml_get(lnum)))
	प्रत्यागम;

    संदेश_start();
    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()
    print_ओळ_no_prefix(lnum, use_संख्या, list);
    यदि (save_silent)
    {
	संदेश_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = save_silent;
    }
    info_message = FALSE;
}

    पूर्णाङ्क
rename_buffer(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, रिक्त, रिक्त, FALSE, curbuf);
    // buffer changed, don't change name now
    यदि (buf != curbuf)
	प्रत्यागम FAIL;
#ifdef FEAT_EVAL
    यदि (aborting())	    // autocmds may abort script processing
	प्रत्यागम FAIL;
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
    curbuf->b_ffname = रिक्त;
    curbuf->b_sfname = रिक्त;
    यदि (setfname(curbuf, new_fname, रिक्त, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	प्रत्यागम FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    यदि (xfname != रिक्त && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	यदि (buf != रिक्त && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, रिक्त, रिक्त, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    प्रत्यागम OK;
}

/*
 * ":file[!] [fname]".
 */
    शून्य
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    यदि (eap->addr_मोजणी > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_मोजणी > 1))
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }

    यदि (*eap->arg != NUL || eap->addr_मोजणी == 1)
    {
	यदि (rename_buffer(eap->arg) == FAIL)
	    प्रत्यागम;
	redraw_tabline = TRUE;
#यदि defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#endif
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    यदि (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    शून्य
ex_update(exarg_T *eap)
{
    यदि (curbufIsChanged())
	(शून्य)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    शून्य
ex_write(exarg_T *eap)
{
    यदि (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_ओळ_मोजणी;
    }

    यदि (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    अन्यथा
	(शून्य)do_write(eap);
}

#ifdef UNIX
    स्थिर पूर्णाङ्क
check_writable(char_u *fname)
{
    यदि (mch_nodetype(fname) == गाठ_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
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
    पूर्णाङ्क
do_write(exarg_T *eap)
{
    पूर्णाङ्क		other;
    char_u	*fname = रिक्त;		// init to shut up gcc
    char_u	*ffname;
    पूर्णाङ्क		retval = FAIL;
    char_u	*free_fname = रिक्त;
#ifdef FEAT_BROWSE
    char_u	*browse_file = रिक्त;
#endif
    buf_T	*alt_buf = रिक्त;
    पूर्णाङ्क		name_was_missing;

    यदि (not_writing())		// check 'write' option
	प्रत्यागम FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    यदि ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    रिक्त, रिक्त, रिक्त, curbuf);
	यदि (browse_file == रिक्त)
	    गच्छ theend;
	ffname = browse_file;
    }
#endif
    यदि (*ffname == NUL)
    {
	यदि (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_argument_required));
	    गच्छ theend;
	}
	other = FALSE;
    }
    अन्यथा
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	यदि (free_fname != रिक्त)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    यदि (other)
    {
	यदि (vim_strchr(p_cpo, CPO_ALTWRITE) != रिक्त
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	अन्यथा
	    alt_buf = buflist_findname(ffname);
	यदि (alt_buf != रिक्त && alt_buf->b_ml.ml_mfp != रिक्त)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    गच्छ theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    यदि (!other && (bt_dontwrite_संदेश(curbuf) || check_fname() == FAIL
#ifdef UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#endif
		|| check_readonly(&eap->forceit, curbuf)))
	गच्छ theend;

    यदि (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	यदि (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_ओळ_मोजणी)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    यदि (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		यदि (vim_dialog_yesno(VIM_QUESTION, रिक्त,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    गच्छ theend;
		eap->forceit = TRUE;
	    }
	    अन्यथा
#endif
	    {
		emsg(_(e_use_bang_to_write_partial_buffer));
		गच्छ theend;
	    }
	}
    }

    यदि (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	यदि (eap->cmdidx == CMD_saveas && alt_buf != रिक्त)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, रिक्त, रिक्त, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, रिक्त, रिक्त, FALSE, alt_buf);
#ifdef FEAT_EVAL
	    यदि (curbuf != was_curbuf || aborting())
#अन्यथा
	    यदि (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		गच्छ theend;
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

	    apply_autocmds(EVENT_BUFFILEPOST, रिक्त, रिक्त, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, रिक्त, रिक्त, FALSE, alt_buf);
	    यदि (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, alt_buf);
	    }
#ifdef FEAT_EVAL
	    यदि (curbuf != was_curbuf || aborting())
#अन्यथा
	    यदि (curbuf != was_curbuf)
#endif
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		गच्छ theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    यदि (*curbuf->b_p_ft == NUL)
	    {
		यदि (au_has_group((char_u *)"filetypedetect"))
		    (शून्य)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, रिक्त);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == रिक्त;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	यदि (eap->cmdidx == CMD_saveas)
	{
	    यदि (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#यदि defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	यदि (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    प्रत्यागम retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    पूर्णाङ्क
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    पूर्णाङ्क		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    यदि (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == रिक्त)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	यदि (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    यदि (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		प्रत्यागम FAIL;
	    }
#endif
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    यदि (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_संदेश_SIZE];

		dialog_संदेश(buff, _("Overwrite existing file \"%s\"?"), fname);
		यदि (vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 2) != VIM_YES)
		    प्रत्यागम FAIL;
		eap->forceit = TRUE;
	    }
	    अन्यथा
#endif
	    {
		emsg(_(e_file_exists));
		प्रत्यागम FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	यदि (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    पूर्णाङ्क		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    यदि (*p_dir == NUL)
	    {
		dir = alloc(5);
		यदि (dir == रिक्त)
		    प्रत्यागम FAIL;
		STRCPY(dir, ".");
	    }
	    अन्यथा
	    {
		dir = alloc(MAXPATHL);
		यदि (dir == रिक्त)
		    प्रत्यागम FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    यदि (r)
	    {
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		यदि (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_संदेश_SIZE];

		    dialog_संदेश(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    यदि (vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			प्रत्यागम FAIL;
		    }
		    eap->forceit = TRUE;
		}
		अन्यथा
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    प्रत्यागम FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    प्रत्यागम OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    शून्य
ex_wnext(exarg_T *eap)
{
    पूर्णाङ्क		i;

    यदि (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (पूर्णाङ्क)eap->line2;
    अन्यथा
	i = curwin->w_arg_idx - (पूर्णाङ्क)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_ओळ_मोजणी;
    यदि (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    शून्य
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    पूर्णाङ्क		error = 0;
    पूर्णाङ्क		save_forceit = eap->forceit;
    पूर्णाङ्क		save_exiting = exiting;

    यदि (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	यदि (before_quit_all(eap) == FAIL)
	    प्रत्यागम;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ifdef FEAT_TERMINAL
	यदि (exiting && !eap->forceit && term_job_running(buf->b_term))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);
	    यदि (term_try_stop_job(buf) == FAIL)
	    {
		no_write_message_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    अन्यथा यदि (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	अन्यथा
#endif
	यदि (bufIsChanged(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    यदि (not_writing())
	    {
		++error;
		भङ्ग;
	    }
#ifdef FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    यदि (buf->b_ffname == रिक्त && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#endif
	    यदि (buf->b_ffname == रिक्त)
	    {
		semsg(_(e_no_file_name_for_buffer_nr), (long)buf->b_fnum);
		++error;
	    }
	    अन्यथा यदि (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    अन्यथा
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		यदि (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		यदि (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    यदि (exiting)
    {
	यदि (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    स्थिर पूर्णाङ्क
not_writing(शून्य)
{
    यदि (p_write)
	प्रत्यागम FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    प्रत्यागम TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    स्थिर पूर्णाङ्क
check_readonly(पूर्णाङ्क *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    यदि (!*forceit && (buf->b_p_ro
		|| (mch_stat((वर्ण *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	यदि ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != रिक्त)
	{
	    char_u	buff[DIALOG_संदेश_SIZE];

	    यदि (buf->b_p_ro)
		dialog_संदेश(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    अन्यथा
		dialog_संदेश(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    यदि (vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		प्रत्यागम FALSE;
	    }
	    अन्यथा
		प्रत्यागम TRUE;
	}
	अन्यथा
#endif
	यदि (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_set_add_bang_to_override));
	अन्यथा
	    semsg(_(e_str_is_read_only_add_bang_to_override), buf->b_fname);
	प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
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
    पूर्णाङ्क
getfile(
    पूर्णाङ्क		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    पूर्णाङ्क		setpm,
    linenr_T	lnum,
    पूर्णाङ्क		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    पूर्णाङ्क		other;
    पूर्णाङ्क		retval;
    char_u	*free_me = रिक्त;

    यदि (!check_can_set_curbuf_forceit(forceit))
	प्रत्यागम GETFILE_ERROR;

    यदि (text_locked())
	प्रत्यागम GETFILE_ERROR;
    यदि (curbuf_locked())
	प्रत्यागम GETFILE_ERROR;

    यदि (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    अन्यथा
	other = (fnum != curbuf->b_fnum);

    यदि (other)
	++no_wait_return;	    // don't wait for autowrite message
    यदि (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	यदि (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	यदि (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    गच्छ theend;
	}
    }
    यदि (other)
	--no_wait_return;
    यदि (setpm)
	setpcmark();
    यदि (!other)
    {
	यदि (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    अन्यथा यदि (do_ecmd(fnum, ffname, sfname, रिक्त, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    अन्यथा
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    प्रत्यागम retval;
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
    पूर्णाङ्क
do_ecmd(
    पूर्णाङ्क		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    पूर्णाङ्क		flags,
    win_T	*oldwin)
{
    पूर्णाङ्क		other_file;		// TRUE if editing another file
    पूर्णाङ्क		oldbuf;			// TRUE if using existing buffer
    पूर्णाङ्क		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = रिक्त;
#यदि defined(FEAT_EVAL)
    पूर्णाङ्क		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = रिक्त;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = रिक्त;
#endif
    पूर्णाङ्क		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    पूर्णाङ्क		newcol = -1;
    पूर्णाङ्क		solcol = -1;
    pos_T	*pos;
    char_u	*command = रिक्त;
#ifdef FEAT_SPELL
    पूर्णाङ्क		did_get_winopts = FALSE;
#endif
    पूर्णाङ्क		readfile_flags = 0;
    पूर्णाङ्क		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    यदि (ERROR_IF_TERM_POPUP_WINDOW)
	प्रत्यागम FAIL;
#endif

    यदि (eap != रिक्त)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    यदि (fnum != 0)
    {
	यदि (fnum == curbuf->b_fnum)	// file is already being edited
	    प्रत्यागम OK;			// nothing to do
	other_file = TRUE;
    }
    अन्यथा
    {
#ifdef FEAT_BROWSE
	यदि ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    यदि (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		यदि (ffname == रिक्त || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    अन्यथा
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    रिक्त, रिक्त, रिक्त, curbuf);
		यदि (browse_file == रिक्त)
		    गच्छ theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	यदि (sfname == रिक्त)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	यदि (sfname != रिक्त)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	यदि ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == रिक्त || *ffname == NUL))
	    गच्छ theend;

	यदि (ffname == रिक्त)
	    other_file = TRUE;
					    // there is no file name
	अन्यथा यदि (*ffname == NUL && curbuf->b_ffname == रिक्त)
	    other_file = FALSE;
	अन्यथा
	{
	    यदि (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    यदि (free_fname != रिक्त)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    यदि (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == रिक्त ? 0 : CCGD_EXCMD)))
    {
	यदि (fnum == 0 && other_file && ffname != रिक्त)
	    (शून्य)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	गच्छ theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    यदि (oldwin != रिक्त && !win_valid(oldwin))
	oldwin = रिक्त;

#यदि defined(FEAT_EVAL)
    यदि ((command != रिक्त || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	यदि (command != रिक्त)
	    valsize = (पूर्णाङ्क)STRLEN(command) + 3;
	अन्यथा
	    valsize = 30;
	val.string = alloc(valsize);
	यदि (val.string != रिक्त)
	{
	    यदि (command != रिक्त)
		val.length = vim_snprintf_safelen((वर्ण *)val.string,
		    valsize, ":%s\r", command);
	    अन्यथा
		val.length = vim_snprintf_safelen((वर्ण *)val.string,
		    valsize, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, val.string, (पूर्णाङ्क)val.length);
	    did_set_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    यदि (other_file)
    {
	पूर्णाङ्क prev_alt_fnum = curwin->w_alt_fnum;

	यदि (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    यदि ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    यदि (oldwin != रिक्त)
		buflist_altfpos(oldwin);
	}

	यदि (fnum)
	    buf = buflist_findnr(fnum);
	अन्यथा
	{
	    यदि (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		यदि (command != रिक्त)
		{
		    tlnum = atol((वर्ण *)command);
		    यदि (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		यदि (newbuf != रिक्त)
		{
		    यदि (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    यदि (tlnum > 0)
			newbuf->b_last_cursor.lnum = tlnum;
		}
		गच्छ theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    यदि (oldwin != रिक्त)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	यदि (buf == रिक्त)
	    गच्छ theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	यदि (buf->b_locked_split)
	{
	    emsg(_(e_cannot_switch_to_a_closing_buffer));
	    गच्छ theend;
	}
	यदि (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	यदि (buf->b_ml.ml_mfp == रिक्त)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	अन्यथा					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (शून्य)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    यदि (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		गच्छ theend;
#ifdef FEAT_EVAL
	    यदि (aborting())	    // autocmds may abort script processing
		गच्छ theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	यदि ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
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
	यदि (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    पूर्णाङ्क		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    प्रतिज्ञा(cmdwin_buf == रिक्त);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = रिक्त;

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
	    यदि (buf->b_fname != रिक्त)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    यदि (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_संदेश(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		गच्छ theend;
	    }
#ifdef FEAT_EVAL
	    यदि (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		गच्छ theend;
	    }
#endif
	    यदि (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    अन्यथा
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		यदि (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_buffer(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != रिक्त);

		// Autocommands may have closed the window.
		यदि (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		यदि (aborting() && curwin->w_buffer != रिक्त)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    गच्छ theend;
		}
#endif
		// Be careful again, like above.
		यदि (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    यदि (curwin->w_buffer == रिक्त)
			buf = lastbuf;
		    अन्यथा
		    {
			delbuf_संदेश(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			गच्छ theend;
		    }
		}
		यदि (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		अन्यथा
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    यदि (curwin->w_buffer == रिक्त
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    यदि (curwin->w_buffer != रिक्त)
			--curwin->w_buffer->b_nwindows;

		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    यदि (!oldbuf && eap != रिक्त)
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
    अन्यथा // !other_file
    {
	यदि ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    गच्छ theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    यदि ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    अन्यथा
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	यदि (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    यदि (buf != curbuf)
	गच्छ theend;
#ifdef FEAT_EVAL
    यदि (aborting())	    // autocmds may abort script processing
	गच्छ theend;
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
    यदि (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	यदि (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	यदि (buf->b_fname != रिक्त)
	    new_name = vim_strsave(buf->b_fname);
	अन्यथा
	    new_name = रिक्त;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	यदि (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_ओळ_मोजणी <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    यदि (u_savecommon(0, curbuf->b_ml.ml_ओळ_मोजणी + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		गच्छ theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	अन्यथा
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	यदि (!bufref_valid(&bufref))
	{
	    delbuf_संदेश(new_name);	// frees new_name
	    गच्छ theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	यदि (buf != curbuf)
	    गच्छ theend;
#ifdef FEAT_EVAL
	यदि (aborting())	    // autocmds may abort script processing
	    गच्छ theend;
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
    यदि (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    यदि (!auto_buf)
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
		यदि (win->w_buffer == curbuf)
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
	यदि (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    यदि (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    यदि (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#यदि defined(FEAT_EVAL)
	    यदि (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#अन्यथा
	    (शून्य)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    यदि (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	अन्यथा
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, रिक्त, रिक्त, FALSE,
							      curbuf, &retval);
	    यदि ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, रिक्त, रिक्त, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	यदि (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    यदि (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (पूर्णाङ्क)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	यदि (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_ओळ_abv_curs();

	maketitle();
#यदि defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	यदि (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    यदि (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    यदि (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(शून्य)parse_spelllang(curwin);
#endif

    यदि (command == रिक्त)
    {
	यदि (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	अन्यथा यदि (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    यदि (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = true;
	    }
	    अन्यथा
		beginline(BL_SOL | BL_FIX);
	}
	अन्यथा			// no line number, go to last line in Ex mode
	{
	    यदि (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    यदि (oldbuf && !auto_buf)
    {
	पूर्णाङ्क	संदेश_scroll_save = संदेश_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	यदि (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    संदेश_scroll = FALSE;
	यदि (!संदेश_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	संदेश_start();
	संदेश_scroll = संदेश_scroll_save;
	संदेश_scrolled_ign = TRUE;

	यदि (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	संदेश_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    यदि (command != रिक्त)
	do_cmdline(command, रिक्त, रिक्त, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    यदि (curbuf->b_kmap_state & KEYMAP_INIT)
	(शून्य)keymap_init();
#endif

    यदि (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    यदि (!skip_redraw)
    {
	n = *so_ptr;
	यदि (topline == 0 && command == रिक्त)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    यदि (p_im && (State & MODE_घाला) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    यदि (p_acd && curbuf->b_ffname != रिक्त)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	यदि (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#यदि defined(FEAT_NETBEANS_INTG)
    यदि (curbuf->b_ffname != रिक्त)
    {
# ifdef FEAT_NETBEANS_INTG
	यदि ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    यदि (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#यदि defined(FEAT_EVAL)
    यदि (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, रिक्त, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    प्रत्यागम retval;
}

    स्थिर शून्य
delbuf_संदेश(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_buffer_str),
					   name == रिक्त ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = रिक्त;
    au_new_curbuf.br_buf_free_मोजणी = 0;
}

स्थिर पूर्णाङ्क append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    स्थिर char_u *
get_append_ओळ(exarg_T *eap, पूर्णाङ्क indent)
{
    char_u	*theline;
    char_u	*p;

    यदि (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    अन्यथा यदि (eap->ea_getline == रिक्त)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	यदि (eap->nextcmd == रिक्त)
	    प्रत्यागम रिक्त;
	p = vim_strchr(eap->nextcmd, NL);
	यदि (p == रिक्त)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	यदि (*p != NUL)
	    ++p;
	अन्यथा
	    p = रिक्त;
	eap->nextcmd = p;
    }
    अन्यथा
    {
	पूर्णाङ्क save_State = State;

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
    प्रत्यागम theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    शून्य
ex_append(exarg_T *eap)
{
    char_u	*theline;
    पूर्णाङ्क		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    पूर्णाङ्क		indent = 0;
    char_u	*p;
    पूर्णाङ्क		vcol;
    पूर्णाङ्क		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    यदि (not_in_vim9(eap, eap->skip) == FAIL)
	प्रत्यागम;
#endif
    यदि (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	यावत् ((theline = get_append_ओळ(eap, 0)) != रिक्त)
	{
	    पूर्णाङ्क end = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    यदि (end)
		भङ्ग;
	}
	प्रत्यागम;
    }

    // the ! flag toggles autoindent
    यदि (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    यदि (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    यदि (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    यदि (empty && lnum == 1)
	lnum = 0;

    State = MODE_घाला;		    // behave like in Insert mode
    यदि (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    प्रति (;;)
    {
	संदेश_scroll = TRUE;
	need_wait_return = FALSE;
	यदि (curbuf->b_p_ai)
	{
	    यदि (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    अन्यथा यदि (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_ओळ(eap, indent);
	ओळी_डावा = Rows - 1;
	यदि (theline == रिक्त)
	    भङ्ग;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	यदि (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	प्रति (p = theline; indent > vcol; ++p)
	{
	    यदि (*p == ' ')
		++vcol;
	    अन्यथा यदि (*p == TAB)
		vcol += 8 - vcol % 8;
	    अन्यथा
		भङ्ग;
	}
	यदि ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    भङ्ग;
	}

	// don't use autoindent if nothing was typed.
	यदि (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	यदि (empty)
	    // there are no marks below the inserted lines
	    appended_ओळी(lnum, 1L);
	अन्यथा
	    appended_ओळी_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	यदि (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    यदि (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_ओळ_मोजणी) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_ओळ_मोजणी;
	यदि (eap->cmdidx != CMD_append)
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
    शून्य
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ifdef FEAT_EVAL
    यदि (not_in_vim9(eap, eap->skip) == FAIL)
	प्रत्यागम;
#endif
    यदि (eap->skip)
    {
	ex_append(eap);
	प्रत्यागम;
    }
    यदि (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	प्रत्यागम;

    // the ! flag toggles autoindent
    यदि (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    प्रति (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	यदि (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to delete
	    भङ्ग;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_ओळी_mark(eap->line1, (long)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    शून्य
ex_z(exarg_T *eap)
{
    char_u	*x;
    long	bigness;
    char_u	*kind;
    पूर्णाङ्क		minus = 0;
    linenr_T	start, end, curs, i;
    पूर्णाङ्क		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    यदि (eap->forceit)
	bigness = Rows - 1;
    अन्यथा यदि (!ONE_WINDOW)
	bigness = curwin->w_height - 3;
    अन्यथा
	bigness = curwin->w_p_scr * 2;
    यदि (bigness < 1)
	bigness = 1;

    x = eap->arg;
    kind = x;
    यदि (*kind == '-' || *kind == '+' || *kind == '='
					      || *kind == '^' || *kind == '.')
	++x;
    यावत् (*x == '-' || *x == '+')
	++x;

    यदि (*x != 0)
    {
	यदि (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_argument_to_z));
	    प्रत्यागम;
	}
	अन्यथा
	{
	    bigness = atol((वर्ण *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    यदि (bigness > 2 * curbuf->b_ml.ml_ओळ_मोजणी || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_ओळ_मोजणी;

	    p_window = bigness;
	    यदि (*kind == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    यदि (*kind == '-' || *kind == '+')
	प्रति (x = kind + 1; *x == *kind; ++x)
	    ;

    विकल्प (*kind)
    {
	पक्ष '-':
	    start = lnum - bigness * (linenr_T)(x - kind) + 1;
	    end = start + bigness - 1;
	    curs = end;
	    भङ्ग;

	पक्ष '=':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    भङ्ग;

	पक्ष '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    भङ्ग;

	पक्ष '.':
	    start = lnum - (bigness + 1) / 2 + 1;
	    end = lnum + (bigness + 1) / 2 - 1;
	    curs = end;
	    भङ्ग;

	स्वभाव:  // '+'
	    start = lnum;
	    यदि (*kind == '+')
		start += bigness * (linenr_T)(x - kind - 1) + 1;
	    अन्यथा यदि (eap->addr_मोजणी == 0)
		++start;
	    end = start + bigness - 1;
	    curs = end;
	    भङ्ग;
    }

    यदि (start < 1)
	start = 1;

    यदि (end > curbuf->b_ml.ml_ओळ_मोजणी)
	end = curbuf->b_ml.ml_ओळ_मोजणी;

    यदि (curs > curbuf->b_ml.ml_ओळ_मोजणी)
	curs = curbuf->b_ml.ml_ओळ_मोजणी;
    अन्यथा यदि (curs < 1)
	curs = 1;

    प्रति (i = start; i <= end; i++)
    {
	यदि (minus && i == lnum)
	{
	    संदेश_putchar('\n');

	    प्रति (j = 1; j < Columns; j++)
		संदेश_putchar('-');
	}

	print_ओळ(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	यदि (minus && i == lnum)
	{
	    संदेश_putchar('\n');

	    प्रति (j = 1; j < Columns; j++)
		संदेश_putchar('-');
	}
    }

    यदि (curwin->w_cursor.lnum != curs)
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
    पूर्णाङ्क
check_restricted(शून्य)
{
    यदि (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_in_rvim));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    पूर्णाङ्क
check_secure(शून्य)
{
    यदि (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_शोध));
	प्रत्यागम TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    यदि (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	प्रत्यागम TRUE;
    }
#endif
    प्रत्यागम FALSE;
}

स्थिर char_u	*old_sub = रिक्त;	// previous substitute pattern
स्थिर पूर्णाङ्क	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
प्रकारनिर्देश संरचना {
    पूर्णाङ्क	do_all;		// do multiple substitutions per line
    पूर्णाङ्क	do_ask;		// ask for confirmation
    पूर्णाङ्क	do_मोजणी;	// count only
    पूर्णाङ्क	do_error;	// if false, ignore errors
    पूर्णाङ्क	do_print;	// print last line with subs.
    पूर्णाङ्क	do_list;	// list last line with subs.
    पूर्णाङ्क	do_संख्या;	// list last line with line nr
    पूर्णाङ्क	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *start, पूर्णाङ्क delimiter)
{
    char_u *p = start;

    यावत् (p[0])
    {
	यदि (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    भङ्ग;
	}
	यदि (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    प्रत्यागम p;
}

    स्थिर पूर्णाङ्क
check_regexp_delim(पूर्णाङ्क c)
{
    यदि (SAFE_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    शून्य
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    long	i = 0;
    regmmatch_T regmatch;
    स्थिर subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ifdef FEAT_EVAL
    subflags_T	subflags_save;
#endif
    पूर्णाङ्क		save_do_all;		// remember user specified 'g' flag
    पूर्णाङ्क		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = रिक्त;		// init for GCC
    string_T	pat = {रिक्त, 0};
    पूर्णाङ्क		delimiter;
    size_t	sublen;
    पूर्णाङ्क		got_quit = FALSE;
    पूर्णाङ्क		got_match = FALSE;
    पूर्णाङ्क		which_pat;
    char_u	*cmd;
    char_u	*p;
    पूर्णाङ्क		save_State;
    linenr_T	first_ओळ = 0;		// first changed line
    linenr_T	last_ओळ = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
    linenr_T	line2;
    long	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    पूर्णाङ्क		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    पूर्णाङ्क		start_nsubs;
    पूर्णाङ्क		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ifdef FEAT_EVAL
    पूर्णाङ्क		save_ma = 0;
    पूर्णाङ्क		save_sandbox = 0;
#endif
#ifdef FEAT_PROP_POPUP
    textprop_T	*text_props = रिक्त;
#endif

    cmd = eap->arg;
    यदि (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    start_nsubs = sub_nsubs;

    यदि (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	// use last used regexp
    अन्यथा
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    यदि (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == रिक्त)
    {
	// don't accept alphanumeric for separator
	यदि (check_regexp_delim(*cmd) == FAIL)
	    प्रत्यागम;
#ifdef FEAT_EVAL
	यदि (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    प्रत्यागम;
#endif

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	यदि (*cmd == '\\')
	{
	    यदि (in_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_in_vim9_script));
		प्रत्यागम;
	    }
	    ++cmd;
	    यदि (vim_strchr((char_u *)"/?&", *cmd) == रिक्त)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		प्रत्यागम;
	    }
	    यदि (*cmd != '&')
		which_pat = RE_शोध;	    // use last '/' pattern
	    STR_LITERAL_SET(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	अन्यथा		// find the end of the regexp
	{
	    which_pat = RE_LAST;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, रिक्त, रिक्त);
	    pat.length = (size_t)(cmd - pat.string);
	    यदि (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	यदि (sub == रिक्त)
	    // out of memory
	    प्रत्यागम;

	यदि (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    यदि (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != रिक्त)
	    {
		यदि (old_sub == रिक्त)	// there is no previous command
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    vim_free(sub);
		    प्रत्यागम;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		यदि (sub == रिक्त)
		    // out of memory
		    प्रत्यागम;
	    }
	    अन्यथा यदि (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		यदि (old_sub == रिक्त)
		{
		    // out of memory
		    vim_free(sub);
		    प्रत्यागम;
		}
	    }
	}
    }
    अन्यथा यदि (!eap->skip)	// use previous pattern and substitution
    {
	यदि (old_sub == रिक्त)	// there is no previous command
	{
	    emsg(_(e_no_previous_substitute_regular_expression));
	    प्रत्यागम;
	}
	pat.string = रिक्त;		// search_regcomp() will use previous pattern
	pat.length = 0;
	sub = vim_strsave(old_sub);
	यदि (sub == रिक्त)
	    प्रत्यागम;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    यदि (pat.string != रिक्त && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_ओळी_मोजणी;

	यदि (eap->skip)
	{
	    vim_free(sub);
	    प्रत्यागम;
	}
	curwin->w_cursor.lnum = eap->line1;
	यदि (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	अन्यथा यदि (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	अन्यथा यदि (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_ओळी_मोजणी = eap->line2 - eap->line1 + 1;
	यदि (eap->line2 < curbuf->b_ml.ml_ओळ_मोजणी)
	    ++joined_ओळी_मोजणी;
	यदि (joined_ओळी_मोजणी > 1)
	{
	    (शून्य)do_join(joined_ओळी_मोजणी, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_ओळी_मोजणी - 1;
	    sub_nlines = 1;
	    (शून्य)do_sub_संदेश(FALSE);
	    ex_may_print(eap);
	}

	यदि (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.length, magic_isset());
	// put pattern in history
	add_to_history(HIST_शोध, pat.string, pat.length, TRUE, NUL);
	vim_free(sub);

	प्रत्यागम;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    यदि (*cmd == '&')
	++cmd;
    अन्यथा
    {
#ifdef FEAT_EVAL
	यदि (in_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	अन्यथा
#endif
	यदि (!p_ed)
	{
	    यदि (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    अन्यथा
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_मोजणी = FALSE;
	subflags.do_संख्या = FALSE;
	subflags.do_ic = 0;
    }
    यावत् (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	यदि (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	अन्यथा यदि (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	अन्यथा यदि (*cmd == 'n')
	    subflags.do_मोजणी = TRUE;
	अन्यथा यदि (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	अन्यथा यदि (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_LAST;
	अन्यथा यदि (*cmd == 'p')
	    subflags.do_print = TRUE;
	अन्यथा यदि (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_संख्या = TRUE;
	}
	अन्यथा यदि (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	अन्यथा यदि (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	अन्यथा यदि (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	अन्यथा
	    भङ्ग;
	++cmd;
    }
    यदि (subflags.do_मोजणी)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    यदि (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	यदि (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_positive_मोजणी_required));
	    vim_free(sub);
	    प्रत्यागम;
	}
	अन्यथा यदि (i >= INT_MAX)
	{
	    वर्ण	buf[20];
	    vim_snprintf(buf, परिमाण(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    प्रत्यागम;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	यदि (eap->line2 > curbuf->b_ml.ml_ओळ_मोजणी)
	    eap->line2 = curbuf->b_ml.ml_ओळ_मोजणी;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    यदि (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	set_nextcmd(eap, cmd);
	यदि (eap->nextcmd == रिक्त)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    प्रत्यागम;
	}
    }

    यदि (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	प्रत्यागम;
    }

    यदि (!subflags.do_मोजणी && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	प्रत्यागम;
    }

    यदि (शोध_regcomp(pat.string, pat.length, रिक्त, RE_SUBST, which_pat, शोध_HIS, &regmatch) == FAIL)
    {
	यदि (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	प्रत्यागम;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    यदि (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    अन्यथा यदि (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = रिक्त;
    sub_firstline.length = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    यदि (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	यदि (p == रिक्त)
	    प्रत्यागम;
	sub = p;
    }
    अन्यथा
    {
	p = regtilde(sub, magic_isset());

	यदि (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    प्रति (lnum = eap->line1; lnum <= line2 && !(got_quit
#यदि defined(FEAT_EVAL)
		|| aborting()
#endif
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, रिक्त);
	यदि (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_start = {रिक्त, 0};
	    size_t	new_start_size = 0;
	    char_u	*new_end;
	    पूर्णाङ्क		did_sub = FALSE;
	    पूर्णाङ्क		lastone;
	    size_t	copy_len;
	    size_t	needed_size;
	    long	nmatch_tl = 0;	// nr of lines matched below lnum
	    पूर्णाङ्क		do_again;	// do it again after joining lines
	    पूर्णाङ्क		skip_match = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_split = false;	// "\r" split the line
#ifdef FEAT_PROP_POPUP
	    पूर्णाङ्क		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	total_added =  0;
	    पूर्णाङ्क		text_prop_मोजणी = 0;
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
	    यदि (!got_match)
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
	    प्रति (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		यदि (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		यदि (lnum > curbuf->b_ml.ml_ओळ_मोजणी)
		    भङ्ग;

		यदि (sub_firstline.string == रिक्त)
		{
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    यदि (sub_firstline.string == रिक्त)
		    {
			vim_free(new_start.string);
			गच्छ outofmem;
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
		यदि (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    यदि (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_match = TRUE;
		    अन्यथा
		    {
			 // search for a match at next column
			यदि (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			अन्यथा
			    ++matchcol;
		    }
		    गच्छ skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		यदि (subflags.do_मोजणी)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    यदि (nmatch > 1)
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
		    यदि (!(sub[0] == '\\' && sub[1] == '='))
#endif
			गच्छ skip;
		}

		यदि (subflags.do_ask)
		{
		    पूर्णाङ्क typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = MODE_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.col = regmatch.startpos[0].col;
		    यदि (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    यदि (vim_strchr(p_cpo, CPO_UNDO) != रिक्त)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    यावत् (subflags.do_ask)
		    {
			यदि (exmode_active)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_ओळ_no_prefix(lnum,
					 subflags.do_संख्या, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, रिक्त, रिक्त, 0);
			    curwin->w_cursor.col = regmatch.endpos[0].col - 1;
			    यदि (curwin->w_cursor.col < 0)
				curwin->w_cursor.col = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   रिक्त, रिक्त, &ec, 0);
			    curwin->w_cursor.col = regmatch.startpos[0].col;
			    यदि (subflags.do_संख्या || curwin->w_p_nu)
			    {
				पूर्णाङ्क numw = संख्या_width(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    संदेश_start();
			    प्रति (i = 0; i < (long)sc; ++i)
				संदेश_putchar(' ');
			    प्रति ( ; i <= (long)ec; ++i)
				संदेश_putchar('^');

			    resp = getexmodeline('?', रिक्त, 0, TRUE);
			    यदि (resp != रिक्त)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				यदि (ex_normal_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			अन्यथा
			{
			    string_T orig_ओळ = {रिक्त, 0};
			    पूर्णाङ्क    len_change = 0;
			    पूर्णाङ्क	   save_p_lz = p_lz;
#ifdef FEAT_FOLDING
			    पूर्णाङ्क save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#endif
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    यदि (new_start.string != रिक्त)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_ओळ.length = ml_get_len(lnum);
				orig_ओळ.string = vim_strnsave(ml_get(lnum), orig_ओळ.length);
				यदि (orig_ओळ.string != रिक्त)
				{
				    string_T new_ओळ;

				    new_ओळ.string = concat_str(new_start.string,
						     sub_firstline.string + copycol);
				    यदि (new_ओळ.string == रिक्त)
					VIM_CLEAR_STRING(orig_ओळ);
				    अन्यथा
				    {
					new_ओळ.length =
					    new_start.length + (sub_firstline.length - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (पूर्णाङ्क)new_ओळ.length - (पूर्णाङ्क)orig_ओळ.length;
					curwin->w_cursor.col += len_change;
					ml_replace(lnum, new_ओळ.string, FALSE);
				    }
				}
			    }

			    शोध_match_ओळी = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    शोध_match_endcol = regmatch.endpos[0].col
								 + len_change;
			    यदि (शोध_match_ओळी == 0
						   && शोध_match_endcol == 0)
				// highlight at least one character for /^/
				शोध_match_endcol = 1;
			    highlight_match = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_match = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ifdef FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#endif
			    यदि (संदेश_row == Rows - 1)
				संदेश_didout = FALSE;	// avoid a scroll-up
			    संदेश_starthere();
			    i = संदेश_scroll;
			    संदेश_scroll = 0;		// truncate msg when
							// needed
			    संदेश_no_more = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    संदेश_no_more = FALSE;
			    संदेश_scroll = i;
			    showruler(TRUE);
			    windgoto(संदेश_row, cmdline_col_off + संदेश_col);
			    RedrawingDisabled = save_RedrawingDisabled;

#ifdef USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#endif
			    ++no_mapping;	// don't map this key
			    ++allow_किल्ल्या;	// allow special keys
			    typed = plain_vgetc();
			    --allow_किल्ल्या;
			    --no_mapping;

			    // clear the question
			    संदेश_didout = FALSE;	// don't scroll up
			    संदेश_col = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    यदि (orig_ओळ.string != रिक्त)
				ml_replace(lnum, orig_ओळ.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			यदि (typed == 'q' || typed == ESC || typed == Ctrl_C
#ifdef UNIX
				|| typed == intr_char
#endif
				)
			{
			    got_quit = TRUE;
			    भङ्ग;
			}
			यदि (typed == 'n')
			    भङ्ग;
			यदि (typed == 'y')
			    भङ्ग;
			यदि (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    भङ्ग;
			}
			यदि (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    भङ्ग;
			}
			यदि (typed == Ctrl_E)
			    scrollup_clamp();
			अन्यथा यदि (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    यदि (vim_strchr(p_cpo, CPO_UNDO) != रिक्त)
			--no_u_sync;

		    यदि (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			यदि (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.length;
			    skip_match = TRUE;
			}
			गच्छ skip;
		    }
		    यदि (got_quit)
			गच्छ skip;
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
		यदि (subflags.do_मोजणी)
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
		यदि (sublen == 0 || aborting() || subflags.do_मोजणी)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    गच्छ skip;
		}
#endif

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		यदि (nmatch > curbuf->b_ml.ml_ओळ_मोजणी - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_ओळ_मोजणी - sub_firstlnum + 1;
		    skip_match = TRUE;
		    // safety check
		    यदि (nmatch < 0)
			गच्छ skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		यदि (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.length = sub_firstline.length;
#ifdef FEAT_PROP_POPUP
		    यदि (curbuf->b_has_textprop)
		    {
			पूर्णाङ्क bytes_added = (पूर्णाङ्क)sublen - 1 - (regmatch.endpos[0].col
						   - regmatch.startpos[0].col);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			यदि (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       bytes_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			total_added += (colnr_T)bytes_added;
		    }
#endif
		}
		अन्यथा
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    यदि (curbuf->b_has_textprop)
		    {
			char_u	*prop_start;

			// Props in the first line may be shortened or deleted
			यदि (adjust_prop_columns(lnum,
					total_added + regmatch.startpos[0].col,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			total_added -=
			    (colnr_T)(sub_firstline.length - regmatch.startpos[0].col);

			// Props in the last line may be moved or deleted
			यदि (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].col, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			text_prop_मोजणी = get_text_props(curbuf, lastlnum,
							   &prop_start, FALSE);
			यदि (text_prop_मोजणी > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(text_props);
			    text_props = ALLOC_MULT(textprop_T,
							      text_prop_मोजणी);
			    यदि (text_props != रिक्त)
			    {
				mch_memmove(text_props, prop_start,
					 text_prop_मोजणी * परिमाण(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				पूर्णाङ्क wi = 0;
				प्रति (पूर्णाङ्क pi = 0; pi < text_prop_मोजणी; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    यदि (text_props[pi].tp_id < 0
					    || (text_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					अनुवर्त;
				    text_props[wi] = text_props[pi];
				    text_props[wi].tp_col +=
					regmatch.startpos[0].col + (colnr_T)sublen - 1;
				    text_props[wi].u.tp_text = रिक्त;
				    ++wi;
				}
				text_prop_मोजणी = wi;
			    }
			}
		    }
#endif
		    tmp.string = ml_get(lastlnum);
		    tmp.length = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ifdef FEAT_PROP_POPUP
		    यदि (curbuf->b_has_textprop)
			total_added +=
			    (colnr_T)(tmp.length - regmatch.endpos[0].col);
#endif
		}

		copy_len = (size_t)(regmatch.startpos[0].col - copycol);
		needed_size = copy_len
		    + (tmp.length - (size_t)regmatch.endpos[0].col) + sublen + 1;

		यदि (new_start.string == रिक्त)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_size = needed_size + 50;
		    यदि ((new_start.string = alloc_clear(new_start_size)) == रिक्त)
			गच्छ outofmem;
		    *new_start.string = NUL;
		    new_start.length = 0;
		}
		अन्यथा
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_size += new_start.length;
		    यदि (needed_size > new_start_size)
		    {
			new_start_size = needed_size + 50;
			यदि ((p1 = alloc_clear(new_start_size)) == रिक्त)
			{
			    vim_free(new_start.string);
			    गच्छ outofmem;
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

		यदि (new_start_size - copy_len < sublen)
		    sublen = new_start_size - copy_len - 1;

#ifdef FEAT_EVAL
		++textlock;
#endif
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_end, (पूर्णाङ्क)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		यदि (n > 0)
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
		यदि (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.length = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.length);
		    यदि (sub_firstline.string == रिक्त)
		    {
			vim_free(new_start.string);
			गच्छ outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    यदि (sub_firstlnum <= line2)
			do_again = TRUE;
		    अन्यथा
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].col;

		यदि (skip_match)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    यदि (sub_firstline.string == रिक्त)
		    {
			vim_free(new_start.string);
			गच्छ outofmem;
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
		प्रति (p1 = new_end; *p1; ++p1)
		{
		    यदि (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_start.length - (p1 - new_start.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_start.length;
#ifdef FEAT_PROP_POPUP
			यदि (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    यदि (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_start.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#endif
		    }
		    अन्यथा यदि (*p1 == CAR)
		    {
			यदि (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_start.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_start.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    यदि (subflags.do_ask)
				appended_ओळी(lnum - 1, 1L);
			    अन्यथा
			    {
				यदि (first_ओळ == 0)
				    first_ओळ = lnum;
				last_ओळ = lnum + 1;
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
		    अन्यथा यदि (has_mbyte)
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
		यदि (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_split)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, रिक्त)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    यदि (new_start.string != रिक्त)
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

			यदि (u_savesub(lnum) != OK)
			    भङ्ग;
			ml_replace(lnum, new_start.string, TRUE);
#ifdef FEAT_PROP_POPUP
			यदि (text_props != रिक्त)
			    add_text_props(lnum, text_props, text_prop_मोजणी);
#endif
			यदि (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    यदि (u_savedel(lnum, nmatch_tl) != OK)
				भङ्ग;
			    प्रति (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    यदि (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    अन्यथा
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    यदि (subflags.do_ask)
				deleted_ओळी(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			यदि (subflags.do_ask)
			    changed_bytes(lnum, 0);
			अन्यथा
			{
			    यदि (first_ओळ == 0)
				first_ओळ = lnum;
			    last_ओळ = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_start.string;
			sub_firstline.length = new_start.length;
			new_start.string = रिक्त;
			new_start.length = 0;
			matchcol = (colnr_T)(sub_firstline.length - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.length
							      - prev_matchcol);
			copycol = 0;
			did_split = false;
		    }
		    यदि (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, रिक्त);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    यदि (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			यदि (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			भङ्ग;
		    }
		}

		ओळ_breakcheck();
	    }

	    यदि (did_sub)
		++sub_nlines;
	    vim_free(new_start.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	ओळ_breakcheck();
    }

    यदि (first_ओळ != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_ओळ_मोजणी - old_ओळ_मोजणी;
	changed_ओळी(first_ओळ, 0, last_ओळ - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ifdef FEAT_PROP_POPUP
    vim_free(text_props);
#endif

    // ":s/pat//n" doesn't move the cursor
    यदि (subflags.do_मोजणी)
	curwin->w_cursor = old_cursor;

    यदि (sub_nsubs > start_nsubs)
    {
	यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_start.lnum = eap->line1;
	    curbuf->b_op_end.lnum = line2;
	    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
	}

	यदि (!global_busy)
	{
	    // when interactive leave cursor on the match
	    यदि (!subflags.do_ask)
	    {
		यदि (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		अन्यथा
		    beginline(BL_WHITE | BL_FIX);
	    }
	    यदि (!do_sub_संदेश(subflags.do_मोजणी) && subflags.do_ask)
		संदेश("");
	}
	अन्यथा
	    global_need_beginline = TRUE;
	यदि (subflags.do_print)
	    print_ओळ(curwin->w_cursor.lnum,
					 subflags.do_संख्या, subflags.do_list);
    }
    अन्यथा यदि (!global_busy)
    {
	यदि (got_int)		// interrupted
	    emsg(_(e_interrupted));
	अन्यथा यदि (got_match)	// did find something but nothing substituted
	    संदेश("");
	अन्यथा यदि (subflags.do_error)	// nothing found
	    semsg(_(e_pattern_not_found_str), get_शोध_pat());
    }

#ifdef FEAT_FOLDING
    यदि (subflags.do_ask && hasAnyFolding(curwin))
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
    पूर्णाङ्क
do_sub_संदेश(
    पूर्णाङ्क	    मोजणी_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    यदि (((sub_nsubs > p_report && (किल्ली_Typed || sub_nlines > 1 || p_report < 1))
		|| मोजणी_only)
	    && messaging())
    {
	वर्ण	*संदेश_single;
	वर्ण	*संदेश_plural;

	यदि (got_int)
	    STRCPY(संदेश_buf, _("(Interrupted) "));
	अन्यथा
	    *संदेश_buf = NUL;

	संदेश_single = मोजणी_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	संदेश_plural = मोजणी_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_add(संदेश_buf, परिमाण(संदेश_buf),
				 NGETTEXT(संदेश_single, संदेश_plural, sub_nlines),
				 sub_nsubs, (long)sub_nlines);

	यदि (संदेश(संदेश_buf))
	    // save message to display it after redraw
	    set_keep_संदेश((char_u *)संदेश_buf, 0);
	प्रत्यागम TRUE;
    }
    यदि (got_int)
    {
	emsg(_(e_interrupted));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

    स्थिर शून्य
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    यदि (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", रिक्त, रिक्त, DOCMD_NOWAIT);
    अन्यथा
	do_cmdline(cmd, रिक्त, रिक्त, DOCMD_NOWAIT);
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
    शून्य
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    पूर्णाङ्क		ndone = 0;
    पूर्णाङ्क		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*used_pat;
    regmmatch_T	regmatch;
    पूर्णाङ्क		match;
    पूर्णाङ्क		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    यदि (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_ओळ_मोजणी))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	प्रत्यागम;
    }

    यदि (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    अन्यथा
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    यदि (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	प्रत्यागम;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    यदि (*cmd == '\\')
    {
	++cmd;
	यदि (vim_strchr((char_u *)"/?&", *cmd) == रिक्त)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    प्रत्यागम;
	}
	यदि (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	अन्यथा
	    which_pat = RE_शोध;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    अन्यथा यदि (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	प्रत्यागम;
    }
    अन्यथा यदि (check_regexp_delim(*cmd) == FAIL)
    {
	प्रत्यागम;
    }
    अन्यथा
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, रिक्त, रिक्त);
	यदि (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    यदि (शोध_regcomp(pat, patlen, &used_pat, RE_BOTH, which_pat, शोध_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	प्रत्यागम;
    }

    यदि (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, रिक्त);
	यदि ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    अन्यथा
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	प्रति (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, रिक्त);
	    यदि (regmatch.regprog == रिक्त)
		भङ्ग;  // re-compiling regprog failed
	    यदि ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    ओळ_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	यदि (got_int)
	    संदेश(_(e_interrupted));
	अन्यथा यदि (ndone == 0)
	{
	    यदि (type == 'v')
	    {
		यदि (in_vim9script())
		    semsg(_(e_pattern_found_in_every_ओळ_str), used_pat);
		अन्यथा
		    smsg(_("Pattern found in every line: %s"), used_pat);
	    }
	    अन्यथा
	    {
		यदि (in_vim9script())
		    semsg(_(e_pattern_not_found_str), used_pat);
		अन्यथा
		    smsg(_("Pattern not found: %s"), used_pat);
	    }
	}
	अन्यथा
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
    शून्य
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
    संदेश_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_ओळ_मोजणी;
    यावत् (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    यदि (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    अन्यथा
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    changed_ओळ_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    यदि (संदेश_col == 0 && संदेश_scrolled == 0)
	संदेश_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    यदि (!do_sub_संदेश(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_ओळ_मोजणी - old_lcount);
}

#ifdef FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(शून्य)
{
    प्रत्यागम old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    शून्य
set_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#endif // FEAT_VIMINFO

#यदि defined(EXITFREE)
    शून्य
free_old_sub(शून्य)
{
    vim_free(old_sub);
}
#endif

#यदि defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    पूर्णाङ्क
prepare_tagpreview(
    पूर्णाङ्क		undo_sync,	    // sync undo when leaving the window
    पूर्णाङ्क		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    यदि (curwin->w_p_pvw)
	प्रत्यागम FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    यदि (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_find_preview_window();
	यदि (wp != रिक्त)
	    popup_set_wantpos_cursor(wp, wp->w_minwidth, रिक्त);
    }
    अन्यथा यदि (use_popup != USEPOPUP_NONE)
    {
	wp = popup_find_info_window();
	यदि (wp != रिक्त)
	{
	    यदि (use_popup == USEPOPUP_NORMAL)
		popup_show(wp);
	    अन्यथा
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    अन्यथा
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    यदि (wp->w_p_pvw)
		भङ्ग;
    }
    यदि (wp != रिक्त)
    {
	win_enter(wp, undo_sync);
	प्रत्यागम FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    यदि ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	प्रत्यागम popup_create_preview_window(use_popup != USEPOPUP_NONE);
# endif
    यदि (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	प्रत्यागम FALSE;
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
    प्रत्यागम TRUE;
}

#endif

/*
 * Make the user happy.
 */
    शून्य
ex_smile(exarg_T *eap UNUSED)
{
    स्थिर वर्ण *code[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    वर्ण *p;
    पूर्णाङ्क n;
    पूर्णाङ्क i;

    संदेश_start();
    संदेश_putchar('\n');
    प्रति (i = 0; i < 2; ++i)
	प्रति (p = code[i]; *p != NUL; ++p)
	    यदि (*p == 'x')
		संदेश_putchar('\n');
	    अन्यथा
		प्रति (n = *p++; n > 0; --n)
		    यदि (*p == 'o' || *p == '$')
			संदेश_putchar_attr(*p, HL_ATTR(HLF_L));
		    अन्यथा
			संदेश_putchar(*p);
    संदेश_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    शून्य
ex_drop(exarg_T *eap)
{
    पूर्णाङ्क		split = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    यदि (ERROR_IF_POPUP_WINDOW || ERROR_IF_TERM_POPUP_WINDOW)
	प्रत्यागम;

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
    यदि (ARGCOUNT == 0)
	प्रत्यागम;

    यदि (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	प्रत्यागम;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	यदि (wp->w_buffer == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    यदि (!bufIsChanged(curbuf))
	    {
		पूर्णाङ्क save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    यदि (curbuf->b_ml.ml_flags & ML_EMPTY)
		ex_rewind(eap);
	    प्रत्यागम;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    यदि (!buf_hide(curbuf))
    {
	++emsg_off;
	split = check_changed(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    यदि (split)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    अन्यथा
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
skip_vimgrep_pat(char_u *p, char_u **s, पूर्णाङ्क *flags)
{
    प्रत्यागम skip_vimgrep_pat_ext(p, s, flags, रिक्त, रिक्त);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, पूर्णाङ्क *flags, char_u **nulp, पूर्णाङ्क *cp)
{
    पूर्णाङ्क		c;

    यदि (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	यदि (s != रिक्त)
	    *s = p;
	p = skiptowhite(p);
	यदि (s != रिक्त && *p != NUL)
	{
	    यदि (nulp != रिक्त)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    अन्यथा
    {
	// ":vimgrep /pattern/[g][j] fname"
	यदि (s != रिक्त)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	यदि (*p != c)
	    प्रत्यागम रिक्त;

	// Truncate the pattern.
	यदि (s != रिक्त)
	{
	    यदि (nulp != रिक्त)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	यावत् (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    यदि (flags != रिक्त)
	    {
		यदि (*p == 'g')
		    *flags |= VGR_GLOBAL;
		अन्यथा यदि (*p == 'j')
		    *flags |= VGR_NOJUMP;
		अन्यथा
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    प्रत्यागम p;
}

#यदि defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    शून्य
ex_oldfiles(exarg_T *eap UNUSED)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    पूर्णाङ्क		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    पूर्णाङ्क		matches = -1;

    यदि (l == रिक्त)
    {
	संदेश(_("No old files"));
	प्रत्यागम;
    }

    संदेश_start();
    संदेश_scroll = TRUE;
    प्रति (li = l->lv_first; li != रिक्त && !got_int; li = li->li_next)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	यदि (!message_filtered(fname))
	{
	    यदि (matches < 0)
		matches = nr;
	    अन्यथा
		matches = 0;
	    संदेश_outnum((long)nr);
	    संदेश_puts(": ");
	    संदेश_outtrans(fname);
	    संदेश_clr_eos();
	    संदेश_putchar('\n');
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    यदि (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_more = FALSE;
	// we only need to prompt if there is more than 1 match
	यदि (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	अन्यथा
	    nr = prompt_for_संख्या(FALSE);
	संदेश_starthere();
	यदि (nr > 0)
	{
	    char_u *p = list_find_str(get_vim_var_list(VV_OLDFILES),
		    (long)nr);

	    यदि (p != रिक्त)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, रिक्त);
		vim_free(p);
	    }
	}
    }
# endif
}
#endif
