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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#అన్తర్భావ <float.h>

స్థిర పూర్ణాంకం linelen(పూర్ణాంకం *has_tab);
స్థిర శూన్య do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, పూర్ణాంకం do_లోపల, పూర్ణాంకం do_బహిర్గతం);
స్థిర పూర్ణాంకం not_writing(శూన్య);
స్థిర పూర్ణాంకం check_readonly(పూర్ణాంకం *forceit, buf_T *buf);
స్థిర శూన్య delbuf_సందేశం(char_u *name);

/*
 * ":ascii" and "ga".
 */
    శూన్య
do_ascii(exarg_T *eap వాడని)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		cval;
    అక్షరం	buf1[20];
    అక్షరం	buf2[20];
    char_u	buf3[7];
#నిర్వచితమైతే FEAT_DIGRAPHS
    char_u      *dig;
#అయితే_ముగింపు
    పూర్ణాంకం		cc[MAX_MCO];
    పూర్ణాంకం		ci = 0;
    పూర్ణాంకం		len;

    ఒకవేళ (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    లేకపోతే
	c = gchar_cursor();
    ఒకవేళ (c == NUL)
    {
	సందేశం("NUL");
	తిరిగి;
    }

    IObuff[0] = NUL;
    ఒకవేళ (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	ఒకవేళ (c == NL)	    // NUL is stored as NL
	    c = NUL;
	ఒకవేళ (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	లేకపోతే
	    cval = c;
	ఒకవేళ (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, పరిమాణం(buf1), "  <%s>", (అక్షరం *)buf3);
	}
	లేకపోతే
	    buf1[0] = NUL;
	ఒకవేళ (c >= 0x80)
	    vim_snprintf(buf2, పరిమాణం(buf2), "  <M-%s>",
						 (అక్షరం *)transchar(c & 0x7f));
	లేకపోతే
	    buf2[0] = NUL;
#నిర్వచితమైతే FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	ఒకవేళ (dig != ఖాళీ)
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	లేకపోతే
#అయితే_ముగింపు
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	ఒకవేళ (enc_utf8)
	    c = cc[ci++];
	లేకపోతే
	    c = 0;
    }

    // Repeat for combining characters.
    అయితే (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (పూర్ణాంకం)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	ఒకవేళ (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	ఒకవేళ (enc_utf8 && utf_iscomposing(c)
#నిర్వచితమైతే USE_GUI
		&& !gui.లోపల_use
#అయితే_ముగింపు
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#నిర్వచితమైతే FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	ఒకవేళ (dig != ఖాళీ)
	    vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	లేకపోతే
#అయితే_ముగింపు
	    vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	ఒకవేళ (ci == MAX_MCO)
	    ఆపు;
	ఒకవేళ (enc_utf8)
	    c = cc[ci++];
	లేకపోతే
	    c = 0;
    }

    సందేశం((అక్షరం *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    శూన్య
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    పూర్ణాంకం		len;
    పూర్ణాంకం		indent = 0;
    పూర్ణాంకం		new_indent;
    పూర్ణాంకం		has_tab;
    పూర్ణాంకం		వెడల్పు;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl)
    {
	// switch left and right aligning
	ఒకవేళ (eap->cmdidx == CMD_కుడి)
	    eap->cmdidx = CMD_ఎడమ;
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_ఎడమ)
	    eap->cmdidx = CMD_కుడి;
    }
#అయితే_ముగింపు

    వెడల్పు = atoi((అక్షరం *)eap->arg);
    save_curpos = curwin->w_cursor;
    ఒకవేళ (eap->cmdidx == CMD_ఎడమ)    // width is used for new indent
    {
	ఒకవేళ (వెడల్పు >= 0)
	    indent = వెడల్పు;
    }
    లేకపోతే
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	ఒకవేళ (వెడల్పు <= 0)
	    వెడల్పు = curbuf->b_p_tw;
	ఒకవేళ (వెడల్పు == 0 && curbuf->b_p_wm > 0)
	    వెడల్పు = curwin->w_వెడల్పు - curbuf->b_p_wm;
	ఒకవేళ (వెడల్పు <= 0)
	    వెడల్పు = 80;
    }

    ఒకవేళ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	తిరిగి;

    కొరకు (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	ఒకవేళ (eap->cmdidx == CMD_ఎడమ)		// left align
	    new_indent = indent;
	లేకపోతే
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_కుడి ? &has_tab
						   : ఖాళీ) - get_indent();

	    ఒకవేళ (len <= 0)			// skip blank lines
		కొనసాగు;

	    ఒకవేళ (eap->cmdidx == CMD_center)
		new_indent = (వెడల్పు - len) / 2;
	    లేకపోతే
	    {
		new_indent = వెడల్పు - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		ఒకవేళ (has_tab)
		    అయితే (new_indent > 0)
		    {
			(శూన్య)సమితి_indent(new_indent, 0);
			ఒకవేళ (linelen(ఖాళీ) <= వెడల్పు)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    చేయి
				(శూన్య)సమితి_indent(++new_indent, 0);
			    అయితే (linelen(ఖాళీ) <= వెడల్పు);
			    --new_indent;
			    ఆపు;
			}
			--new_indent;
		    }
	    }
	}
	ఒకవేళ (new_indent < 0)
	    new_indent = 0;
	(శూన్య)సమితి_indent(new_indent, 0);		// set indent
    }
    మారింది_పంక్తులు(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    స్థిర పూర్ణాంకం
linelen(పూర్ణాంకం *has_tab)
{
    char_u  *పంక్తి;
    char_u  *మొదటి;
    char_u  *చివరి;
    పూర్ణాంకం	    save;
    పూర్ణాంకం	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    పంక్తి = ml_get_curline();
    ఒకవేళ (*పంక్తి == NUL)
	తిరిగి 0;

    // find the first non-blank character
    మొదటి = skipwhite(పంక్తి);

    // find the character after the last non-blank character
    కొరకు (చివరి = మొదటి + STRLEN(మొదటి);
				చివరి > మొదటి && VIM_ISWHITE(చివరి[-1]); --చివరి)
	;
    save = *చివరి;
    *చివరి = NUL;
    len = linetabsize_str(పంక్తి);	// get line length on screen
    ఒకవేళ (has_tab != ఖాళీ)		// check for embedded TAB
	*has_tab = (vim_strchr(మొదటి, TAB) != ఖాళీ);
    *చివరి = save;

    తిరిగి len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
స్థిర char_u	*sortbuf1;
స్థిర char_u	*sortbuf2;

స్థిర పూర్ణాంకం	క్రమీకరించు_lc;	// sort using locale
స్థిర పూర్ణాంకం	క్రమీకరించు_ic;	// ignore case
స్థిర పూర్ణాంకం	క్రమీకరించు_nr;	// sort on number
స్థిర పూర్ణాంకం	క్రమీకరించు_rx;	// sort on regex instead of skipping it
స్థిర పూర్ణాంకం	క్రమీకరించు_flt;	// sort on floating number

స్థిర పూర్ణాంకం	క్రమీకరించు_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
రకనిర్వచనం నిర్మాణం
{
    linenr_T	lnum;			// line number
    సంఘం {
	నిర్మాణం
	{
	    varnumber_T	ప్రారంభం_నిలువు_nr;	// starting column number
	    varnumber_T	చివర_నిలువు_nr;	// ending column number
	} పంక్తి;
	నిర్మాణం
	{
	    varnumber_T	విలువ;		// value if sorting by integer
	    పూర్ణాంకం is_సంఖ్య;		// TRUE when line contains a number
	} అంకె;
	float_T విలువ_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    స్థిర పూర్ణాంకం
string_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    ఒకవేళ (క్రమీకరించు_lc)
	తిరిగి strcoll((అక్షరం *)s1, (అక్షరం *)s2);
    తిరిగి క్రమీకరించు_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    స్థిర పూర్ణాంకం
క్రమీకరించు_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    పూర్ణాంకం		ఫలితం = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    ఒకవేళ (క్రమీకరించు_abort)
	తిరిగి 0;
    fast_breakcheck();
    ఒకవేళ (got_int)
	క్రమీకరించు_abort = TRUE;

    ఒకవేళ (క్రమీకరించు_nr)
    {
	ఒకవేళ (l1.st_u.అంకె.is_సంఖ్య != l2.st_u.అంకె.is_సంఖ్య)
	    ఫలితం = l1.st_u.అంకె.is_సంఖ్య > l2.st_u.అంకె.is_సంఖ్య ? 1 : -1;
	లేకపోతే
	    ఫలితం = l1.st_u.అంకె.విలువ == l2.st_u.అంకె.విలువ ? 0
			     : l1.st_u.అంకె.విలువ > l2.st_u.అంకె.విలువ ? 1 : -1;
    }
    లేకపోతే ఒకవేళ (క్రమీకరించు_flt)
	ఫలితం = l1.st_u.విలువ_flt == l2.st_u.విలువ_flt ? 0
			     : l1.st_u.విలువ_flt > l2.st_u.విలువ_flt ? 1 : -1;
    లేకపోతే
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.పంక్తి.ప్రారంభం_నిలువు_nr,
		     l1.st_u.పంక్తి.చివర_నిలువు_nr - l1.st_u.పంక్తి.ప్రారంభం_నిలువు_nr + 1);
	sortbuf1[l1.st_u.పంక్తి.చివర_నిలువు_nr - l1.st_u.పంక్తి.ప్రారంభం_నిలువు_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.పంక్తి.ప్రారంభం_నిలువు_nr,
		     l2.st_u.పంక్తి.చివర_నిలువు_nr - l2.st_u.పంక్తి.ప్రారంభం_నిలువు_nr + 1);
	sortbuf2[l2.st_u.పంక్తి.చివర_నిలువు_nr - l2.st_u.పంక్తి.ప్రారంభం_నిలువు_nr] = 0;

	ఫలితం = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    ఒకవేళ (ఫలితం == 0)
	తిరిగి (పూర్ణాంకం)(l1.lnum - l2.lnum);
    తిరిగి ఫలితం;
}

/*
 * ":sort".
 */
    శూన్య
ex_క్రమీకరించు(exarg_T *eap)
{
    regmatch_T	regmatch;
    పూర్ణాంకం		len;
    linenr_T	lnum;
    దీర్ఘ	maxlen = 0;
    sorti_T	*nrs;
    size_t	లెక్క = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    పూర్ణాంకం		unique = FALSE;
    దీర్ఘ	deleted;
    colnr_T	ప్రారంభం_నిలువు;
    colnr_T	చివర_నిలువు;
    పూర్ణాంకం		క్రమీకరించు_what = 0;
    పూర్ణాంకం		format_దొరికింది = 0;
    పూర్ణాంకం		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    ఒకవేళ (లెక్క <= 1)
	తిరిగి;

    ఒకవేళ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	తిరిగి;
    sortbuf1 = ఖాళీ;
    sortbuf2 = ఖాళీ;
    regmatch.regprog = ఖాళీ;
    nrs = ALLOC_MULT(sorti_T, లెక్క);
    ఒకవేళ (nrs == ఖాళీ)
	వెళ్ళు sortend;

    క్రమీకరించు_abort = క్రమీకరించు_ic = క్రమీకరించు_lc = క్రమీకరించు_rx = క్రమీకరించు_nr = 0;
    క్రమీకరించు_flt = 0;

    కొరకు (p = eap->arg; *p != NUL; ++p)
    {
	ఒకవేళ (VIM_ISWHITE(*p))
	    ;
	లేకపోతే ఒకవేళ (*p == 'i')
	    క్రమీకరించు_ic = TRUE;
	లేకపోతే ఒకవేళ (*p == 'l')
	    క్రమీకరించు_lc = TRUE;
	లేకపోతే ఒకవేళ (*p == 'r')
	    క్రమీకరించు_rx = TRUE;
	లేకపోతే ఒకవేళ (*p == 'n')
	{
	    క్రమీకరించు_nr = 1;
	    ++format_దొరికింది;
	}
	లేకపోతే ఒకవేళ (*p == 'f')
	{
	    క్రమీకరించు_flt = 1;
	    ++format_దొరికింది;
	}
	లేకపోతే ఒకవేళ (*p == 'b')
	{
	    క్రమీకరించు_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_దొరికింది;
	}
	లేకపోతే ఒకవేళ (*p == 'o')
	{
	    క్రమీకరించు_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_దొరికింది;
	}
	లేకపోతే ఒకవేళ (*p == 'x')
	{
	    క్రమీకరించు_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_దొరికింది;
	}
	లేకపోతే ఒకవేళ (*p == 'u')
	    unique = TRUE;
	లేకపోతే ఒకవేళ (*p == '"')	// comment start
	    ఆపు;
	లేకపోతే ఒకవేళ (eap->nextcmd == ఖాళీ && check_nextcmd(p) != ఖాళీ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (!ASCII_ISALPHA(*p) && regmatch.regprog == ఖాళీ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    ఒకవేళ (s == p + 1)
	    {
		ఒకవేళ (చివరి_శోధన_pat() == ఖాళీ)
		{
		    emsg(_(e_no_మునుపటి_regular_వ్యక్తీకరణ));
		    వెళ్ళు sortend;
		}
		regmatch.regprog = vim_regcomp(చివరి_శోధన_pat(), RE_MAGIC);
	    }
	    లేకపోతే
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ఒకవేళ (regmatch.regprog == ఖాళీ)
		వెళ్ళు sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_వాదం_str), p);
	    వెళ్ళు sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    ఒకవేళ (format_దొరికింది > 1)
    {
	emsg(_(e_invalid_వాదం));
	వెళ్ళు sortend;
    }

    // From here on "sort_nr" is used as a flag for any integer number
    // sorting.
    క్రమీకరించు_nr += క్రమీకరించు_what;

    /*
     * Make an array with all line numbers.  This avoids having to copy all
     * the lines into allocated memory.
     * When sorting on strings "start_col_nr" is the offset in the line, for
     * numbers sorting it's the number to sort on.  This means the pattern
     * matching and number conversion only has to be done once per line.
     * Also get the longest line length for allocating "sortbuf".
     */
    కొరకు (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	ఒకవేళ (maxlen < len)
	    maxlen = len;

	ప్రారంభం_నిలువు = 0;
	చివర_నిలువు = len;
	ఒకవేళ (regmatch.regprog != ఖాళీ && vim_regexec(&regmatch, s, 0))
	{
	    ఒకవేళ (క్రమీకరించు_rx)
	    {
		ప్రారంభం_నిలువు = (colnr_T)(regmatch.startp[0] - s);
		చివర_నిలువు = (colnr_T)(regmatch.endp[0] - s);
	    }
	    లేకపోతే
		ప్రారంభం_నిలువు = (colnr_T)(regmatch.endp[0] - s);
	}
	లేకపోతే
	    ఒకవేళ (regmatch.regprog != ఖాళీ)
		చివర_నిలువు = 0;

	ఒకవేళ (క్రమీకరించు_nr || క్రమీకరించు_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + చివర_నిలువు;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + ప్రారంభం_నిలువు;
	    ఒకవేళ (క్రమీకరించు_nr)
	    {
		ఒకవేళ (క్రమీకరించు_what & STR2NR_HEX)
		    s = skiptohex(p);
		లేకపోతే ఒకవేళ (క్రమీకరించు_what & STR2NR_BIN)
		    s = skiptobin(p);
		లేకపోతే
		    s = skiptodigit(p);
		ఒకవేళ (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		ఒకవేళ (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.అంకె.is_సంఖ్య = FALSE;
		    nrs[lnum - eap->line1].st_u.అంకె.విలువ = 0;
		}
		లేకపోతే
		{
		    nrs[lnum - eap->line1].st_u.అంకె.is_సంఖ్య = TRUE;
		    vim_str2nr(s, ఖాళీ, ఖాళీ, క్రమీకరించు_what,
			&nrs[lnum - eap->line1].st_u.అంకె.విలువ,
			ఖాళీ, 0, FALSE, ఖాళీ);
		}
	    }
	    లేకపోతే
	    {
		s = skipwhite(p);
		ఒకవేళ (*s == '+')
		    s = skipwhite(s + 1);

		ఒకవేళ (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.విలువ_flt = -DBL_MAX;
		లేకపోతే
		    nrs[lnum - eap->line1].st_u.విలువ_flt =
						      strtod((అక్షరం *)s, ఖాళీ);
	    }
	    *s2 = c;
	}
	లేకపోతే
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.పంక్తి.ప్రారంభం_నిలువు_nr = ప్రారంభం_నిలువు;
	    nrs[lnum - eap->line1].st_u.పంక్తి.చివర_నిలువు_nr = చివర_నిలువు;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	ఒకవేళ (regmatch.regprog != ఖాళీ)
	    fast_breakcheck();
	ఒకవేళ (got_int)
	    వెళ్ళు sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ఒకవేళ (sortbuf1 == ఖాళీ)
	వెళ్ళు sortend;
    sortbuf2 = alloc(maxlen + 1);
    ఒకవేళ (sortbuf2 == ఖాళీ)
	వెళ్ళు sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((శూన్య *)nrs, లెక్క, పరిమాణం(sorti_T), క్రమీకరించు_compare);

    ఒకవేళ (క్రమీకరించు_abort)
	వెళ్ళు sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? లెక్క - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	ఒకవేళ (get_lnum + ((linenr_T)లెక్క - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	ఒకవేళ (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    ఒకవేళ (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		ఆపు;
	}
	fast_breakcheck();
	ఒకవేళ (got_int)
	    వెళ్ళు sortend;
    }

    // delete the original lines if appending worked
    ఒకవేళ (i == లెక్క)
	కొరకు (i = 0; i < లెక్క; ++i)
	    ml_delete(eap->line1);
    లేకపోతే
	లెక్క = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (దీర్ఘ)(లెక్క - (lnum - eap->line2));
    ఒకవేళ (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (దీర్ఘ)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    లేకపోతే ఒకవేళ (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    ఒకవేళ (change_occurred || deleted != 0)
	మారింది_పంక్తులు(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    ఒకవేళ (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    శూన్య
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    పూర్ణాంకం		len;
    linenr_T	lnum;
    దీర్ఘ	maxlen = 0;
    linenr_T	లెక్క = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    పూర్ణాంకం		keep_only_unique = FALSE;
    పూర్ణాంకం		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    దీర్ఘ	deleted = 0;
    colnr_T	ప్రారంభం_నిలువు;
    colnr_T	చివర_నిలువు;
    పూర్ణాంకం		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    ఒకవేళ (లెక్క <= 1)
	తిరిగి;

    ఒకవేళ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	తిరిగి;
    sortbuf1 = ఖాళీ;
    regmatch.regprog = ఖాళీ;

    క్రమీకరించు_abort = క్రమీకరించు_ic = క్రమీకరించు_lc = క్రమీకరించు_rx = క్రమీకరించు_nr = 0;
    క్రమీకరించు_flt = 0;

    కొరకు (p = eap->arg; *p != NUL; ++p)
    {
	ఒకవేళ (VIM_ISWHITE(*p))
	    ;
	లేకపోతే ఒకవేళ (*p == 'i')
	    క్రమీకరించు_ic = TRUE;
	లేకపోతే ఒకవేళ (*p == 'l')
	    క్రమీకరించు_lc = TRUE;
	లేకపోతే ఒకవేళ (*p == 'r')
	    క్రమీకరించు_rx = TRUE;
	లేకపోతే ఒకవేళ (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    ఒకవేళ (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	లేకపోతే ఒకవేళ (*p == '"')	// comment start
	    ఆపు;
	లేకపోతే ఒకవేళ (eap->nextcmd == ఖాళీ && check_nextcmd(p) != ఖాళీ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (!ASCII_ISALPHA(*p) && regmatch.regprog == ఖాళీ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    ఒకవేళ (s == p + 1)
	    {
		ఒకవేళ (చివరి_శోధన_pat() == ఖాళీ)
		{
		    emsg(_(e_no_మునుపటి_regular_వ్యక్తీకరణ));
		    వెళ్ళు uniqend;
		}
		regmatch.regprog = vim_regcomp(చివరి_శోధన_pat(), RE_MAGIC);
	    }
	    లేకపోతే
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ఒకవేళ (regmatch.regprog == ఖాళీ)
		వెళ్ళు uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_వాదం_str), p);
	    వెళ్ళు uniqend;
	}
    }

    // Find the length of the longest line.
    కొరకు (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	ఒకవేళ (maxlen < len)
	    maxlen = len;

	ఒకవేళ (got_int)
	    వెళ్ళు uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ఒకవేళ (sortbuf1 == ఖాళీ)
	వెళ్ళు uniqend;

    // Delete lines according to options.
    పూర్ణాంకం సరిపోలిక_continue = FALSE;
    పూర్ణాంకం తదుపరి_is_unmatch = FALSE;
    పూర్ణాంకం is_సరిపోలిక;
    linenr_T పూర్తి_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    కొరకు (linenr_T i = 0; i < లెక్క; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	ప్రారంభం_నిలువు = 0;
	చివర_నిలువు = len;
	ఒకవేళ (regmatch.regprog != ఖాళీ && vim_regexec(&regmatch, s, 0))
	{
	    ఒకవేళ (క్రమీకరించు_rx)
	    {
		ప్రారంభం_నిలువు = (colnr_T)(regmatch.startp[0] - s);
		చివర_నిలువు = (colnr_T)(regmatch.endp[0] - s);
	    }
	    లేకపోతే
		ప్రారంభం_నిలువు = (colnr_T)(regmatch.endp[0] - s);
	}
	లేకపోతే
	    ఒకవేళ (regmatch.regprog != ఖాళీ)
		చివర_నిలువు = 0;
	ఒకవేళ (చివర_నిలువు > 0)
	{
	    save_c = s[చివర_నిలువు];
	    s[చివర_నిలువు] = NUL;
	}

	is_సరిపోలిక = i > 0 ? !string_compare(&s[ప్రారంభం_నిలువు], sortbuf1) : FALSE;
	delete_lnum = 0;
	ఒకవేళ (తదుపరి_is_unmatch)
	{
	    is_సరిపోలిక = FALSE;
	    తదుపరి_is_unmatch = FALSE;
	}

	ఒకవేళ (!keep_only_unique && !keep_only_not_unique)
	{
	    ఒకవేళ (is_సరిపోలిక)
		delete_lnum = get_lnum;
	    లేకపోతే
		STRCPY(sortbuf1, &s[ప్రారంభం_నిలువు]);
	}
	లేకపోతే ఒకవేళ (keep_only_not_unique)
	{
	    ఒకవేళ (is_సరిపోలిక)
	    {
		పూర్తి_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		సరిపోలిక_continue = TRUE;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (i > 0 && !సరిపోలిక_continue && get_lnum - 1 > పూర్తి_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    తదుపరి_is_unmatch = TRUE;
		}
		లేకపోతే ఒకవేళ (i >= లెక్క - 1)
		    delete_lnum = get_lnum;
		సరిపోలిక_continue = FALSE;
		STRCPY(sortbuf1, &s[ప్రారంభం_నిలువు]);
	    }
	}
	లేకపోతే // keep_only_unique
	{
	    ఒకవేళ (is_సరిపోలిక)
	    {
		ఒకవేళ (!సరిపోలిక_continue)
		    delete_lnum = get_lnum - 1;
		లేకపోతే
		    delete_lnum = get_lnum;
		సరిపోలిక_continue = TRUE;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (i == 0 && సరిపోలిక_continue)
		    delete_lnum = get_lnum;
		సరిపోలిక_continue = FALSE;
		STRCPY(sortbuf1, &s[ప్రారంభం_నిలువు]);
	    }
	}

	ఒకవేళ (చివర_నిలువు > 0)
	    s[చివర_నిలువు] = save_c;

	ఒకవేళ (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    లెక్క--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	ఒకవేళ (got_int)
	    వెళ్ళు uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (దీర్ఘ)MAXLNUM, -deleted);
    msgmore(-deleted);

    ఒకవేళ (change_occurred)
	మారింది_పంక్తులు(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    ఒకవేళ (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	అంకె_పంక్తులు;  // Num lines moved
    linenr_T	చివరి_పంక్తి;  // Last line in file after adding new text
#నిర్వచితమైతే FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#అయితే_ముగింపు

    ఒకవేళ (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_యొక్క_పంక్తులు_into_itself));
	తిరిగి FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    ఒకవేళ (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	ఒకవేళ (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	లేకపోతే
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	తిరిగి OK;
    }

    అంకె_పంక్తులు = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    ఒకవేళ (u_save(dest, dest + 1) == FAIL)
	తిరిగి FAIL;
    కొరకు (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	ఒకవేళ (str != ఖాళీ)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    ఒకవేళ (dest < line1)
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
    చివరి_పంక్తి = curbuf->b_ml.ml_పంక్తి_లెక్క;
    mark_adjust_nofold(line1, line2, చివరి_పంక్తి - line2, 0L);
    ఒకవేళ (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -అంకె_పంక్తులు, 0L);
#నిర్వచితమైతే FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ఒకవేళ (win->w_బఫరు == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#అయితే_ముగింపు
	ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_ప్రారంభం.lnum = dest - అంకె_పంక్తులు + 1;
	    curbuf->b_op_చివర.lnum = dest;
	}
    }
    లేకపోతే
    {
	mark_adjust_nofold(dest + 1, line1 - 1, అంకె_పంక్తులు, 0L);
#నిర్వచితమైతే FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ఒకవేళ (win->w_బఫరు == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#అయితే_ముగింపు
	ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_ప్రారంభం.lnum = dest + 1;
	    curbuf->b_op_చివర.lnum = dest + అంకె_పంక్తులు;
	}
    }
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_ప్రారంభం.నిలువు = curbuf->b_op_చివర.నిలువు = 0;
    mark_adjust_nofold(చివరి_పంక్తి - అంకె_పంక్తులు + 1, చివరి_పంక్తి,
					     -(చివరి_పంక్తి - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    ఒకవేళ (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	తిరిగి FAIL;

    కొరకు (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_సందేశం);

    ఒకవేళ (!global_busy && అంకె_పంక్తులు > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", అంకె_పంక్తులు),
			(దీర్ఘ)అంకె_పంక్తులు);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    ఒకవేళ (dest >= line1)
	curwin->w_cursor.lnum = dest;
    లేకపోతే
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    ఒకవేళ (line1 < dest)
    {
	dest += అంకె_పంక్తులు + 1;
	చివరి_పంక్తి = curbuf->b_ml.ml_పంక్తి_లెక్క;
	ఒకవేళ (dest > చివరి_పంక్తి + 1)
	    dest = చివరి_పంక్తి + 1;
	మారింది_పంక్తులు(line1, 0, dest, 0L);
    }
    లేకపోతే
	మారింది_పంక్తులు(dest + 1, 0, line1 + అంకె_పంక్తులు, 0L);

    తిరిగి OK;
}

/*
 * ":copy"
 */
    శూన్య
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	లెక్క;
    char_u	*p;

    లెక్క = line2 - line1 + 1;
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_ప్రారంభం.lnum = n + 1;
	curbuf->b_op_చివర.lnum = n + లెక్క;
	curbuf->b_op_ప్రారంభం.నిలువు = curbuf->b_op_చివర.నిలువు = 0;
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
    ఒకవేళ (u_save(n, n + 1) == FAIL)
	తిరిగి;

    curwin->w_cursor.lnum = n;
    అయితే (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	ఒకవేళ (p != ఖాళీ)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	ఒకవేళ (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	ఒకవేళ (curwin->w_cursor.lnum < line1)
	    ++line1;
	ఒకవేళ (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_పంక్తులు_mark(n, లెక్క);
    ఒకవేళ (VIsual_చురుకైన)
	check_pos(curbuf, &VIsual);

    msgmore((దీర్ఘ)లెక్క);
}

స్థిర char_u	*prevcmd = ఖాళీ;	// the previous command

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_prev_shellcmd(శూన్య)
{
    vim_free(prevcmd);
}
#అయితే_ముగింపు

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    స్థిర పూర్ణాంకం
prevcmd_is_సమితి(శూన్య)
{
    ఒకవేళ (prevcmd == ఖాళీ)
    {
	emsg(_(e_no_మునుపటి_command));
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    శూన్య
do_bang(
    పూర్ణాంకం		addr_లెక్క,
    exarg_T	*eap,
    పూర్ణాంకం		forceit,
    పూర్ణాంకం		do_లోపల,
    పూర్ణాంకం		do_బహిర్గతం)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = ఖాళీ;		// the new command
    పూర్ణాంకం			free_newcmd = FALSE;    // need to free() newcmd
    పూర్ణాంకం			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    పూర్ణాంకం			len;
    పూర్ణాంకం			scroll_save = సందేశం_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (addr_లెక్క == 0)		// :!
    {
	సందేశం_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	సందేశం_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    చేయి
    {
	len = (పూర్ణాంకం)STRLEN(trailarg) + 1;
	ఒకవేళ (newcmd != ఖాళీ)
	    len += (పూర్ణాంకం)STRLEN(newcmd);
	ఒకవేళ (ins_prevcmd)
	{
	    ఒకవేళ (!prevcmd_is_సమితి())
	    {
		vim_free(newcmd);
		తిరిగి;
	    }
	    len += (పూర్ణాంకం)STRLEN(prevcmd);
	}
	ఒకవేళ ((t = alloc(len)) == ఖాళీ)
	{
	    vim_free(newcmd);
	    తిరిగి;
	}
	*t = NUL;
	ఒకవేళ (newcmd != ఖాళీ)
	    STRCAT(t, newcmd);
	ఒకవేళ (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = ఖాళీ;
	అయితే (*p)
	{
	    ఒకవేళ (*p == '!')
	    {
		ఒకవేళ (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		లేకపోతే
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    ఆపు;
		}
	    }
	    ++p;
	}
    } అయితే (trailarg != ఖాళీ);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    ఒకవేళ (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    లేకపోతే
	free_newcmd = TRUE;

    ఒకవేళ (bangredo)	    // put cmd in redo buffer for ! command
    {
	ఒకవేళ (!prevcmd_is_సమితి())
	    వెళ్ళు theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	ఒకవేళ (cmd != ఖాళీ)
	{
	    Append_గమ్యం_Redobuff_Lit(cmd, -1);
	    vim_free(cmd);
	}
	లేకపోతే
	    Append_గమ్యం_Redobuff_Lit(prevcmd, -1);
	Append_గమ్యం_Redobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    ఒకవేళ (*p_shq != NUL)
    {
	ఒకవేళ (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	ఒకవేళ (newcmd == ఖాళీ)
	    తిరిగి;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    ఒకవేళ (addr_లెక్క == 0)		// :!
    {
	// echo the command
	సందేశం_ప్రారంభం();
	సందేశం_putchar(':');
	సందేశం_putchar('!');
	సందేశం_outtrans(newcmd);
	సందేశం_clr_eos();
	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);

	do_shell(newcmd, 0);
    }
    లేకపోతే				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_లోపల, do_బహిర్గతం);
	apply_autocmds(EVENT_SHELLFILTERPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
    }

theend:
    ఒకవేళ (free_newcmd)
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
    స్థిర శూన్య
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    పూర్ణాంకం		do_లోపల,
    పూర్ణాంకం		do_బహిర్గతం)
{
    char_u	*itmp = ఖాళీ;
    char_u	*otmp = ఖాళీ;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    పూర్ణాంకం		shell_flags = 0;
    pos_T	orig_ప్రారంభం = curbuf->b_op_ప్రారంభం;
    pos_T	orig_చివర = curbuf->b_op_చివర;
    పూర్ణాంకం		save_cmod_flags = cmdmod.cmod_flags;
#నిర్వచితమైతే FEAT_FILTERPIPE
    పూర్ణాంకం		stmp = p_stmp;
#అయితే_ముగింపు

    ఒకవేళ (*cmd == NUL)	    // no filter command
	తిరిగి;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.నిలువు = 0;
    మారింది_పంక్తి_abv_curs();
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

    ఒకవేళ (do_బహిర్గతం)
	shell_flags |= SHELL_DOOUT;

#నిర్వచితమైతే FEAT_FILTERPIPE
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    ఒకవేళ (!do_లోపల && do_బహిర్గతం && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    లేకపోతే ఒకవేళ (do_లోపల && !do_బహిర్గతం && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_ప్రారంభం.lnum = line1;
	curbuf->b_op_చివర.lnum = line2;
    }
    లేకపోతే ఒకవేళ (do_లోపల && do_బహిర్గతం && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_ప్రారంభం.lnum = line1;
	curbuf->b_op_చివర.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ ((do_లోపల && (itmp = vim_tempname('i', FALSE)) == ఖాళీ)
		|| (do_బహిర్గతం && (otmp = vim_tempname('o', FALSE)) == ఖాళీ))
	{
	    emsg(_(e_cant_get_తాత్కాలిక_file_name));
	    వెళ్ళు filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    ఒకవేళ (itmp != ఖాళీ && buf_write(curbuf, itmp, ఖాళీ, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	సందేశం_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (!aborting())
#అయితే_ముగింపు
	    // will call wait_return()
	    (శూన్య)semsg(_(e_cant_create_file_str), itmp);
	వెళ్ళు filterend;
    }
    ఒకవేళ (curbuf != old_curbuf)
	వెళ్ళు filterend;

    ఒకవేళ (!do_బహిర్గతం)
	సందేశం_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    ఒకవేళ (cmd_buf == ఖాళీ)
	వెళ్ళు filterend;

    windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    ఒకవేళ (!do_బహిర్గతం || STRCMP(p_srr, ">") == 0 || !do_లోపల)
	redraw_later_clear();

    ఒకవేళ (do_బహిర్గతం)
    {
	ఒకవేళ (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    వెళ్ళు error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_పంక్తి_లెక్క;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    ఒకవేళ (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    ఒకవేళ (do_బహిర్గతం)
    {
	ఒకవేళ (otmp != ఖాళీ)
	{
	    ఒకవేళ (readfile(otmp, ఖాళీ, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#ఒకవేళ defined(FEAT_EVAL)
		ఒకవేళ (!aborting())
#అయితే_ముగింపు
		{
		    సందేశం_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		వెళ్ళు error;
	    }
	    ఒకవేళ (curbuf != old_curbuf)
		వెళ్ళు filterend;
	}

	read_linecount = curbuf->b_ml.ml_పంక్తి_లెక్క - read_linecount;

	ఒకవేళ (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_ప్రారంభం.lnum = line2 + 1;
	    curbuf->b_op_చివర.lnum = curwin->w_cursor.lnum;
	    appended_పంక్తులు_mark(line2, read_linecount);
	}

	ఒకవేళ (do_లోపల)
	{
	    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == ఖాళీ)
	    {
		ఒకవేళ (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		లేకపోతే ఒకవేళ (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		లేకపోతే
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
	    del_పంక్తులు(linecount, TRUE);
	    ఒకవేళ (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_ప్రారంభం.lnum = curbuf->b_op_చివర.lnum =
					MIN(line1, curbuf->b_ml.ml_పంక్తి_లెక్క);
		curbuf->b_op_ప్రారంభం.నిలువు = curbuf->b_op_చివర.నిలువు = 0;
	    }
	    లేకపోతే
	    {
		curbuf->b_op_ప్రారంభం.lnum -= linecount;	// adjust '[
		curbuf->b_op_చివర.lnum -= linecount;	// adjust ']
	    }
	    write_lnum_adjust(-linecount);		// adjust last line
							// for next write
#నిర్వచితమైతే FEAT_FOLDING
	    foldUpdate(curwin, curbuf->b_op_ప్రారంభం.lnum, curbuf->b_op_చివర.lnum);
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_చివర.lnum - curbuf->b_op_ప్రారంభం.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_చివర.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	ఒకవేళ (linecount > p_report)
	{
	    ఒకవేళ (do_లోపల)
	    {
		vim_snprintf(సందేశం_buf, పరిమాణం(సందేశం_buf),
				    _("%ld lines filtered"), (దీర్ఘ)linecount);
		ఒకవేళ (సందేశం(సందేశం_buf) && !సందేశం_scroll)
		    // save message to display it after redraw
		    సమితి_keep_సందేశం((char_u *)సందేశం_buf, 0);
	    }
	    లేకపోతే
		msgmore((దీర్ఘ)linecount);
	}
    }
    లేకపోతే
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    ఒకవేళ (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_బఫరు));
    }
    లేకపోతే ఒకవేళ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_ప్రారంభం = orig_ప్రారంభం;
	curbuf->b_op_చివర = orig_చివర;
    }

    ఒకవేళ (itmp != ఖాళీ)
	mch_remove(itmp);
    ఒకవేళ (otmp != ఖాళీ)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    శూన్య
do_shell(
    char_u	*cmd,
    పూర్ణాంకం		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    పూర్ణాంకం		save_nwr;
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    పూర్ణాంకం		winstart = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		keep_termcap = !termcap_చురుకైన;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ఒకవేళ (check_restricted() || check_secure())
    {
	సందేశం_చివర();
	తిరిగి;
    }

#నిర్వచితమైతే MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    ఒకవేళ (cmd != ఖాళీ)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# ఒకవేళ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    ఒకవేళ (gui.లోపల_use && vim_strchr(p_go, GO_TERMINAL) != ఖాళీ)
	keep_termcap = TRUE;
# endif
#అయితే_ముగింపు

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    సందేశం_putchar('\r');			// put cursor at start of line
    ఒకవేళ (!autocmd_busy)
    {
	ఒకవేళ (!keep_termcap)
	    stoptermcap();
    }
#నిర్వచితమైతే MSWIN
    ఒకవేళ (!winstart)
#అయితే_ముగింపు
	సందేశం_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    ఒకవేళ (p_warn && !autocmd_busy && సందేశం_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    ఒకవేళ (buf_Is_మారింది_Not_పదం(buf))
	    {
#నిర్వచితమైతే FEAT_GUI_MSWIN
		ఒకవేళ (!keep_termcap)
		    starttermcap();	// don't want a message box here
#అయితే_ముగింపు
		సందేశం_puts(_("[No write since last change]\n"));
#నిర్వచితమైతే FEAT_GUI_MSWIN
		ఒకవేళ (!keep_termcap)
		    stoptermcap();
#అయితే_ముగింపు
		ఆపు;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    ఒకవేళ (!swapping_screen())
	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
    cursor_on();
    (శూన్య)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    ఒకవేళ (!swapping_screen())
    {
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
	సందేశం_నిలువు = 0;
    }

    ఒకవేళ (autocmd_busy)
    {
	ఒకవేళ (సందేశం_silent == 0)
	    redraw_later_clear();
    }
    లేకపోతే
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	ఒకవేళ (!gui.లోపల_use)
# endif
	{
	    ఒకవేళ (cmd == ఖాళీ
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		ఒకవేళ (సందేశం_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    లేకపోతే
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		ఒకవేళ (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		పూర్ణాంకం redraw = సందేశం_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		ఒకవేళ (gui.లోపల_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(పదం_console ? -1 : redraw); // see below
# లేకపోతే
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#అయితే_ముగింపు // FEAT_GUI_MSWIN

	ఒకవేళ (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#నిర్వచితమైతే AMIGA
	ఒకవేళ (skip_redraw)		// ':' hit in wait_return()
	{
	    ఒకవేళ (సందేశం_silent == 0)
		redraw_later_clear();
	}
	లేకపోతే ఒకవేళ (పదం_console)
	{
	    బహిర్గతం_STR("\033[0 q");	// get window size
	    ఒకవేళ (got_int && సందేశం_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    లేకపోతే
		must_redraw = 0;	// no extra redraw needed
	}
#అయితే_ముగింపు
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
}

#ఒకవేళ !defined(UNIX)
    స్థిర char_u *
కనుగొను_pipe(char_u *cmd)
{
    char_u  *p;
    పూర్ణాంకం	    inquote = FALSE;

    కొరకు (p = cmd; *p != NUL; ++p)
    {
	ఒకవేళ (!inquote && *p == '|')
	    తిరిగి p;
	ఒకవేళ (*p == '"')
	    inquote = !inquote;
	లేకపోతే ఒకవేళ (rem_backslash(p))
	    ++p;
    }
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

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
    పూర్ణాంకం		is_powershell = FALSE;
#నిర్వచితమైతే UNIX
    పూర్ణాంకం		is_fish_shell;
#అయితే_ముగింపు

    char_u *shell_name = get_isolated_shell_name();
    ఒకవేళ (shell_name == ఖాళీ)
	తిరిగి ఖాళీ;

#ఒకవేళ defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    ఒకవేళ (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    లేకపోతే
#అయితే_ముగింపు
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	ఒకవేళ (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	లేకపోతే
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    ఒకవేళ (itmp != ఖాళీ)
    {
	ఒకవేళ (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	లేకపోతే
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    ఒకవేళ (otmp != ఖాళీ)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (is_powershell)
    {
	ఒకవేళ (itmp != ఖాళీ)
	    vim_snprintf((అక్షరం *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	లేకపోతే
	    vim_snprintf((అక్షరం *)buf, len, "& { %s }", cmd);
    }
    లేకపోతే
    {
#ఒకవేళ defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	ఒకవేళ (itmp != ఖాళీ || otmp != ఖాళీ)
	{
	    ఒకవేళ (is_fish_shell)
		vim_snprintf((అక్షరం *)buf, len, "begin; %s; end", (అక్షరం *)cmd);
	    లేకపోతే
		vim_snprintf((అక్షరం *)buf, len, "(%s)", (అక్షరం *)cmd);
	}
	లేకపోతే
	    STRCPY(buf, cmd);
	ఒకవేళ (itmp != ఖాళీ)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#లేకపోతే
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	ఒకవేళ (*p_sxe != NUL && *p_sxq == '(')
	{
	    ఒకవేళ (itmp != ఖాళీ || otmp != ఖాళీ)
		vim_snprintf((అక్షరం *)buf, len, "(%s)", (అక్షరం *)cmd);
	    లేకపోతే
		STRCPY(buf, cmd);
	    ఒకవేళ (itmp != ఖాళీ)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	లేకపోతే
	{
	    STRCPY(buf, cmd);
	    ఒకవేళ (itmp != ఖాళీ)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		ఒకవేళ (*p_shq == NUL)
		{
		    p = కనుగొను_pipe(buf);
		    ఒకవేళ (p != ఖాళీ)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		ఒకవేళ (*p_shq == NUL)
		{
		    p = కనుగొను_pipe(cmd);
		    ఒకవేళ (p != ఖాళీ)
		    {
			// insert a space before the '|' for DOS
			STRCAT(buf, " ");
			STRCAT(buf, p);
		    }
		}
	    }
	}
#అయితే_ముగింపు
    }
    ఒకవేళ (otmp != ఖాళీ)
	append_redir(buf, (పూర్ణాంకం)len, p_srr, otmp);

    తిరిగి buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    శూన్య
append_redir(
    char_u	*buf,
    పూర్ణాంకం		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*చివర;

    చివర = buf + STRLEN(buf);
    // find "%s"
    కొరకు (p = opt; (p = vim_strchr(p, '%')) != ఖాళీ; ++p)
    {
	ఒకవేళ (p[1] == 's') // found %s
	    ఆపు;
	ఒకవేళ (p[1] == '%') // skip %%
	    ++p;
    }
    ఒకవేళ (p != ఖాళీ)
    {
#నిర్వచితమైతే MSWIN
	*చివర++ = ' '; // not really needed? Not with sh, ksh or bash
#అయితే_ముగింపు
	vim_snprintf((అక్షరం *)చివర, (size_t)(buflen - (చివర - buf)),
						  (అక్షరం *)opt, (అక్షరం *)fname);
    }
    లేకపోతే
	vim_snprintf((అక్షరం *)చివర, (size_t)(buflen - (చివర - buf)), " %s %s",
		(అక్షరం *)opt, (అక్షరం *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    శూన్య
do_fixdel(exarg_T *eap వాడని)
{
    char_u  *p;

    p = కనుగొను_termcode((char_u *)"kb");
    కూడు_termcode((char_u *)"kD", p != ఖాళీ
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    శూన్య
print_పంక్తి_no_ఉపసర్గ(
    linenr_T	lnum,
    పూర్ణాంకం		use_సంఖ్య,
    పూర్ణాంకం		list)
{
    అక్షరం	numbuf[30];

    ఒకవేళ (curwin->w_p_nu || use_సంఖ్య)
    {
	vim_snprintf(numbuf, పరిమాణం(numbuf),
				   "%*ld ", సంఖ్య_వెడల్పు(curwin), (దీర్ఘ)lnum);
	సందేశం_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    సందేశం_prt_పంక్తి(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    శూన్య
print_పంక్తి(linenr_T lnum, పూర్ణాంకం use_సంఖ్య, పూర్ణాంకం list)
{
    పూర్ణాంకం		save_silent = silent_బహుళకం;

    // apply :filter /pat/
    ఒకవేళ (సందేశం_filtered(ml_get(lnum)))
	తిరిగి;

    సందేశం_ప్రారంభం();
    silent_బహుళకం = FALSE;
    info_సందేశం = TRUE;	// use mch_msg(), not mch_errmsg()
    print_పంక్తి_no_ఉపసర్గ(lnum, use_సంఖ్య, list);
    ఒకవేళ (save_silent)
    {
	సందేశం_putchar('\n');
	cursor_on();		// msg_start() switches it off
	బహిర్గతం_flush();
	silent_బహుళకం = save_silent;
    }
    info_సందేశం = FALSE;
}

    పూర్ణాంకం
rename_బఫరు(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, ఖాళీ, ఖాళీ, FALSE, curbuf);
    // buffer changed, don't change name now
    ఒకవేళ (buf != curbuf)
	తిరిగి FAIL;
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (aborting())	    // autocmds may abort script processing
	తిరిగి FAIL;
#అయితే_ముగింపు
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
    curbuf->b_ffname = ఖాళీ;
    curbuf->b_sfname = ఖాళీ;
    ఒకవేళ (setfname(curbuf, new_fname, ఖాళీ, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	తిరిగి FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    ఒకవేళ (xfname != ఖాళీ && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	ఒకవేళ (buf != ఖాళీ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    తిరిగి OK;
}

/*
 * ":file[!] [fname]".
 */
    శూన్య
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    ఒకవేళ (eap->addr_లెక్క > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_లెక్క > 1))
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }

    ఒకవేళ (*eap->arg != NUL || eap->addr_లెక్క == 1)
    {
	ఒకవేళ (rename_బఫరు(eap->arg) == FAIL)
	    తిరిగి;
	redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#అయితే_ముగింపు
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    ఒకవేళ (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    శూన్య
ex_update(exarg_T *eap)
{
    ఒకవేళ (curbuf_Is_మారింది())
	(శూన్య)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    శూన్య
ex_write(exarg_T *eap)
{
    ఒకవేళ (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
    }

    ఒకవేళ (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    లేకపోతే
	(శూన్య)do_write(eap);
}

#నిర్వచితమైతే UNIX
    స్థిర పూర్ణాంకం
check_writable(char_u *fname)
{
    ఒకవేళ (mch_nodetype(fname) == కణుపు_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Write the current buffer to file "eap->arg".
 * If "eap->append" is TRUE, append to the file.
 *
 * If "*eap->arg == NUL" write to current file.
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
do_write(exarg_T *eap)
{
    పూర్ణాంకం		other;
    char_u	*fname = ఖాళీ;		// init to shut up gcc
    char_u	*ffname;
    పూర్ణాంకం		retval = FAIL;
    char_u	*free_fname = ఖాళీ;
#నిర్వచితమైతే FEAT_BROWSE
    char_u	*browse_file = ఖాళీ;
#అయితే_ముగింపు
    buf_T	*alt_buf = ఖాళీ;
    పూర్ణాంకం		name_was_missing;

    ఒకవేళ (not_writing())		// check 'write' option
	తిరిగి FAIL;

    ffname = eap->arg;
#నిర్వచితమైతే FEAT_BROWSE
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    ఖాళీ, ఖాళీ, ఖాళీ, curbuf);
	ఒకవేళ (browse_file == ఖాళీ)
	    వెళ్ళు theend;
	ffname = browse_file;
    }
#అయితే_ముగింపు
    ఒకవేళ (*ffname == NUL)
    {
	ఒకవేళ (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_వాదం_required));
	    వెళ్ళు theend;
	}
	other = FALSE;
    }
    లేకపోతే
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	ఒకవేళ (free_fname != ఖాళీ)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    ఒకవేళ (other)
    {
	ఒకవేళ (vim_strchr(p_cpo, CPO_ALTWRITE) != ఖాళీ
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	లేకపోతే
	    alt_buf = buflist_findname(ffname);
	ఒకవేళ (alt_buf != ఖాళీ && alt_buf->b_ml.ml_mfp != ఖాళీ)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_లోపల_another_బఫరు));
	    వెళ్ళు theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    ఒకవేళ (!other && (bt_dontwrite_సందేశం(curbuf) || check_fname() == FAIL
#నిర్వచితమైతే UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#అయితే_ముగింపు
		|| check_readonly(&eap->forceit, curbuf)))
	వెళ్ళు theend;

    ఒకవేళ (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	ఒకవేళ (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_పంక్తి_లెక్క)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ఒకవేళ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		ఒకవేళ (vim_dialog_yesno(VIM_QUESTION, ఖాళీ,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    వెళ్ళు theend;
		eap->forceit = TRUE;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		emsg(_(e_use_bang_గమ్యం_write_partial_బఫరు));
		వెళ్ళు theend;
	    }
	}
    }

    ఒకవేళ (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	ఒకవేళ (eap->cmdidx == CMD_saveas && alt_buf != ఖాళీ)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, ఖాళీ, ఖాళీ, FALSE, alt_buf);
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (curbuf != was_curbuf || aborting())
#లేకపోతే
	    ఒకవేళ (curbuf != was_curbuf)
#అయితే_ముగింపు
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		వెళ్ళు theend;
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
	    buf_name_మారింది(curbuf);

	    apply_autocmds(EVENT_BUFFILEPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, ఖాళీ, ఖాళీ, FALSE, alt_buf);
	    ఒకవేళ (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, ఖాళీ, ఖాళీ, FALSE, alt_buf);
	    }
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (curbuf != was_curbuf || aborting())
#లేకపోతే
	    ఒకవేళ (curbuf != was_curbuf)
#అయితే_ముగింపు
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		వెళ్ళు theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    ఒకవేళ (*curbuf->b_p_ft == NUL)
	    {
		ఒకవేళ (au_has_సమూహం((char_u *)"filetypedetect"))
		    (శూన్య)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, ఖాళీ);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == ఖాళీ;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	ఒకవేళ (eap->cmdidx == CMD_saveas)
	{
	    ఒకవేళ (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#అయితే_ముగింపు
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	ఒకవేళ (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#నిర్వచితమైతే FEAT_BROWSE
    vim_free(browse_file);
#అయితే_ముగింపు
    vim_free(free_fname);
    తిరిగి retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    పూర్ణాంకం
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    పూర్ణాంకం		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    ఒకవేళ (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == ఖాళీ)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	ఒకవేళ (!eap->forceit && !eap->append)
	{
#నిర్వచితమైతే UNIX
	    // with UNIX it is possible to open a directory
	    ఒకవేళ (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		తిరిగి FAIL;
	    }
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ఒకవేళ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_సందేశం_కొలత];

		dialog_సందేశం(buff, _("Overwrite existing file \"%s\"?"), fname);
		ఒకవేళ (vim_dialog_yesno(VIM_QUESTION, ఖాళీ, buff, 2) != VIM_YES)
		    తిరిగి FAIL;
		eap->forceit = TRUE;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		emsg(_(e_file_exists));
		తిరిగి FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	ఒకవేళ (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    పూర్ణాంకం		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    ఒకవేళ (*p_dir == NUL)
	    {
		dir = alloc(5);
		ఒకవేళ (dir == ఖాళీ)
		    తిరిగి FAIL;
		STRCPY(dir, ".");
	    }
	    లేకపోతే
	    {
		dir = alloc(MAXPATHL);
		ఒకవేళ (dir == ఖాళీ)
		    తిరిగి FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    ఒకవేళ (r)
	    {
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		ఒకవేళ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_సందేశం_కొలత];

		    dialog_సందేశం(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    ఒకవేళ (vim_dialog_yesno(VIM_QUESTION, ఖాళీ, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			తిరిగి FAIL;
		    }
		    eap->forceit = TRUE;
		}
		లేకపోతే
#అయితే_ముగింపు
		{
		    semsg(_(e_మార్పిడి_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    తిరిగి FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    తిరిగి OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    శూన్య
ex_wnext(exarg_T *eap)
{
    పూర్ణాంకం		i;

    ఒకవేళ (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (పూర్ణాంకం)eap->line2;
    లేకపోతే
	i = curwin->w_arg_idx - (పూర్ణాంకం)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    శూన్య
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    పూర్ణాంకం		error = 0;
    పూర్ణాంకం		save_forceit = eap->forceit;
    పూర్ణాంకం		save_exiting = exiting;

    ఒకవేళ (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	ఒకవేళ (before_quit_all(eap) == FAIL)
	    తిరిగి;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#నిర్వచితమైతే FEAT_TERMINAL
	ఒకవేళ (exiting && !eap->forceit && పదం_పని_running(buf->b_పదం))
	{
	    bufref_T	bufref;

	    సమితి_bufref(&bufref, buf);
	    ఒకవేళ (పదం_try_stop_పని(buf) == FAIL)
	    {
		no_write_సందేశం_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    లేకపోతే ఒకవేళ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (buf_Is_మారింది(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    ఒకవేళ (not_writing())
	    {
		++error;
		ఆపు;
	    }
#నిర్వచితమైతే FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    ఒకవేళ (buf->b_ffname == ఖాళీ && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#అయితే_ముగింపు
	    ఒకవేళ (buf->b_ffname == ఖాళీ)
	    {
		semsg(_(e_no_file_name_for_బఫరు_nr), (దీర్ఘ)buf->b_fnum);
		++error;
	    }
	    లేకపోతే ఒకవేళ (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    లేకపోతే
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf);
		ఒకవేళ (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		ఒకవేళ (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    ఒకవేళ (exiting)
    {
	ఒకవేళ (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    స్థిర పూర్ణాంకం
not_writing(శూన్య)
{
    ఒకవేళ (p_write)
	తిరిగి FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    తిరిగి TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    స్థిర పూర్ణాంకం
check_readonly(పూర్ణాంకం *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    ఒకవేళ (!*forceit && (buf->b_p_ro
		|| (mch_stat((అక్షరం *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ఒకవేళ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != ఖాళీ)
	{
	    char_u	buff[DIALOG_సందేశం_కొలత];

	    ఒకవేళ (buf->b_p_ro)
		dialog_సందేశం(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    లేకపోతే
		dialog_సందేశం(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    ఒకవేళ (vim_dialog_yesno(VIM_QUESTION, ఖాళీ, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		తిరిగి FALSE;
	    }
	    లేకపోతే
		తిరిగి TRUE;
	}
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_సమితి_కూడు_bang_గమ్యం_override));
	లేకపోతే
	    semsg(_(e_str_is_read_only_కూడు_bang_గమ్యం_override), buf->b_fname);
	తిరిగి TRUE;
    }

    తిరిగి FALSE;
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
    పూర్ణాంకం
getfile(
    పూర్ణాంకం		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    పూర్ణాంకం		setpm,
    linenr_T	lnum,
    పూర్ణాంకం		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    పూర్ణాంకం		other;
    పూర్ణాంకం		retval;
    char_u	*free_me = ఖాళీ;

    ఒకవేళ (!check_can_సమితి_curbuf_forceit(forceit))
	తిరిగి GETFILE_ERROR;

    ఒకవేళ (పాఠ్యం_locked())
	తిరిగి GETFILE_ERROR;
    ఒకవేళ (curbuf_locked())
	తిరిగి GETFILE_ERROR;

    ఒకవేళ (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    లేకపోతే
	other = (fnum != curbuf->b_fnum);

    ఒకవేళ (other)
	++no_wait_return;	    // don't wait for autowrite message
    ఒకవేళ (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbuf_Is_మారింది() && autowrite(curbuf, forceit) == FAIL)
    {
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ఒకవేళ (p_confirm && p_write)
	    dialog_మారింది(curbuf, FALSE);
	ఒకవేళ (curbuf_Is_మారింది())
#అయితే_ముగింపు
	{
	    --no_wait_return;
	    no_write_సందేశం();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    వెళ్ళు theend;
	}
    }
    ఒకవేళ (other)
	--no_wait_return;
    ఒకవేళ (setpm)
	setpcmark();
    ఒకవేళ (!other)
    {
	ఒకవేళ (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    లేకపోతే ఒకవేళ (do_ecmd(fnum, ffname, sfname, ఖాళీ, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    లేకపోతే
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    తిరిగి retval;
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
    పూర్ణాంకం
do_ecmd(
    పూర్ణాంకం		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    పూర్ణాంకం		flags,
    win_T	*oldwin)
{
    పూర్ణాంకం		other_file;		// TRUE if editing another file
    పూర్ణాంకం		oldbuf;			// TRUE if using existing buffer
    పూర్ణాంకం		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = ఖాళీ;
#ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం		did_సమితి_swapcommand = FALSE;
#అయితే_ముగింపు
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = ఖాళీ;
#నిర్వచితమైతే FEAT_BROWSE
    char_u	dot_మార్గం[] = ".";
    char_u	*browse_file = ఖాళీ;
#అయితే_ముగింపు
    పూర్ణాంకం		retval = FAIL;
    దీర్ఘ	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    పూర్ణాంకం		newcol = -1;
    పూర్ణాంకం		solcol = -1;
    pos_T	*pos;
    char_u	*command = ఖాళీ;
#నిర్వచితమైతే FEAT_SPELL
    పూర్ణాంకం		did_get_winopts = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		readfile_flags = 0;
    పూర్ణాంకం		did_inc_redrawing_disabled = FALSE;
    దీర్ఘ	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (ERROR_IF_పదం_POPUP_కిటికీ)
	తిరిగి FAIL;
#అయితే_ముగింపు

    ఒకవేళ (eap != ఖాళీ)
	command = eap->do_ecmd_cmd;
    సమితి_bufref(&old_curbuf, curbuf);

    ఒకవేళ (fnum != 0)
    {
	ఒకవేళ (fnum == curbuf->b_fnum)	// file is already being edited
	    తిరిగి OK;			// nothing to do
	other_file = TRUE;
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_BROWSE
	ఒకవేళ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    ఒకవేళ (
# ifdef FEAT_GUI
		!gui.లోపల_use &&
# endif
		    au_has_సమూహం((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		ఒకవేళ (ffname == ఖాళీ || !mch_isdir(ffname))
		    ffname = dot_మార్గం;
	    }
	    లేకపోతే
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    ఖాళీ, ఖాళీ, ఖాళీ, curbuf);
		ఒకవేళ (browse_file == ఖాళీ)
		    వెళ్ళు theend;
		ffname = browse_file;
	    }
	}
#అయితే_ముగింపు
	// if no short name given, use ffname for short name
	ఒకవేళ (sfname == ఖాళీ)
	    sfname = ffname;
#నిర్వచితమైతే USE_FNAME_CASE
	ఒకవేళ (sfname != ఖాళీ)
	    fname_case(sfname, 0);   // set correct case for sfname
#అయితే_ముగింపు

	ఒకవేళ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == ఖాళీ || *ffname == NUL))
	    వెళ్ళు theend;

	ఒకవేళ (ffname == ఖాళీ)
	    other_file = TRUE;
					    // there is no file name
	లేకపోతే ఒకవేళ (*ffname == NUL && curbuf->b_ffname == ఖాళీ)
	    other_file = FALSE;
	లేకపోతే
	{
	    ఒకవేళ (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    ఒకవేళ (free_fname != ఖాళీ)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    ఒకవేళ (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_మారింది(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == ఖాళీ ? 0 : CCGD_EXCMD)))
    {
	ఒకవేళ (fnum == 0 && other_file && ffname != ఖాళీ)
	    (శూన్య)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	వెళ్ళు theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    ఒకవేళ (oldwin != ఖాళీ && !win_valid(oldwin))
	oldwin = ఖాళీ;

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ ((command != ఖాళీ || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	ఒకవేళ (command != ఖాళీ)
	    valsize = (పూర్ణాంకం)STRLEN(command) + 3;
	లేకపోతే
	    valsize = 30;
	val.string = alloc(valsize);
	ఒకవేళ (val.string != ఖాళీ)
	{
	    ఒకవేళ (command != ఖాళీ)
		val.నిడివి = vim_snprintf_safelen((అక్షరం *)val.string,
		    valsize, ":%s\r", command);
	    లేకపోతే
		val.నిడివి = vim_snprintf_safelen((అక్షరం *)val.string,
		    valsize, "%ldG", (దీర్ఘ)newlnum);
	    సమితి_vim_var_string(VV_SWAPCOMMAND, val.string, (పూర్ణాంకం)val.నిడివి);
	    did_సమితి_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#అయితే_ముగింపు

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    ఒకవేళ (other_file)
    {
	పూర్ణాంకం prev_alt_fnum = curwin->w_alt_fnum;

	ఒకవేళ (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    ఒకవేళ (oldwin != ఖాళీ)
		buflist_altfpos(oldwin);
	}

	ఒకవేళ (fnum)
	    buf = buflist_findnr(fnum);
	లేకపోతే
	{
	    ఒకవేళ (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		ఒకవేళ (command != ఖాళీ)
		{
		    tlnum = atol((అక్షరం *)command);
		    ఒకవేళ (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		ఒకవేళ (newbuf != ఖాళీ)
		{
		    ఒకవేళ (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    ఒకవేళ (tlnum > 0)
			newbuf->b_చివరి_cursor.lnum = tlnum;
		}
		వెళ్ళు theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_సమితి_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    ఒకవేళ (oldwin != ఖాళీ)
		oldwin = curwin;
	    సమితి_bufref(&old_curbuf, curbuf);
	}
	ఒకవేళ (buf == ఖాళీ)
	    వెళ్ళు theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	ఒకవేళ (buf->b_locked_విభజించు)
	{
	    emsg(_(e_cannot_switch_గమ్యం_a_closing_బఫరు));
	    వెళ్ళు theend;
	}
	ఒకవేళ (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	లేకపోతే					// existing memfile
	{
	    oldbuf = TRUE;
	    సమితి_bufref(&bufref, buf);
	    (శూన్య)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    ఒకవేళ (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		వెళ్ళు theend;
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (aborting())	    // autocmds may abort script processing
		వెళ్ళు theend;
#అయితే_ముగింపు
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	ఒకవేళ ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_చివరి)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->నిలువు;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	ఒకవేళ (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    పూర్ణాంకం		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    ప్రతిజ్ఞా(cmdwin_buf == ఖాళీ);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = ఖాళీ;

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
	    ఒకవేళ (buf->b_fname != ఖాళీ)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    సమితి_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    ఒకవేళ (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_సందేశం(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		వెళ్ళు theend;
	    }
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		వెళ్ళు theend;
	    }
#అయితే_ముగింపు
	    ఒకవేళ (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    లేకపోతే
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		ఒకవేళ (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_బఫరు(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != ఖాళీ);

		// Autocommands may have closed the window.
		ఒకవేళ (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#నిర్వచితమైతే FEAT_EVAL
		// autocmds may abort script processing
		ఒకవేళ (aborting() && curwin->w_బఫరు != ఖాళీ)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    వెళ్ళు theend;
		}
#అయితే_ముగింపు
		// Be careful again, like above.
		ఒకవేళ (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    ఒకవేళ (curwin->w_బఫరు == ఖాళీ)
			buf = lastbuf;
		    లేకపోతే
		    {
			delbuf_సందేశం(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			వెళ్ళు theend;
		    }
		}
		ఒకవేళ (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		లేకపోతే
		{
#నిర్వచితమైతే FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    ఒకవేళ (curwin->w_బఫరు == ఖాళీ
			    || curwin->w_s == &(curwin->w_బఫరు->b_s))
			curwin->w_s = &(buf->b_s);
#అయితే_ముగింపు
		    ఒకవేళ (curwin->w_బఫరు != ఖాళీ)
			--curwin->w_బఫరు->b_nwindows;

		    curwin->w_బఫరు = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    ఒకవేళ (!oldbuf && eap != ఖాళీ)
		    {
			సమితి_file_options(TRUE, eap);
			సమితి_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#నిర్వచితమైతే FEAT_SPELL
		did_get_winopts = TRUE;
#అయితే_ముగింపు
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.నిలువు = 0;
    }
    లేకపోతే // !other_file
    {
	ఒకవేళ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    వెళ్ళు theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    ఒకవేళ ((flags & ECMD_సమితి_HELP) || keep_help_flag)
    {
	prepare_help_బఫరు();
    }
    లేకపోతే
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	ఒకవేళ (!curbuf->b_help)
	    సమితి_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    ఒకవేళ (buf != curbuf)
	వెళ్ళు theend;
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (aborting())	    // autocmds may abort script processing
	వెళ్ళు theend;
#అయితే_ముగింపు

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
    ఒకవేళ (!other_file && !oldbuf)		// re-use the buffer
    {
	సమితి_చివరి_cursor(curwin);	// may set b_last_cursor
	ఒకవేళ (newlnum == ECMD_చివరి || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.నిలువు;
	}
	buf = curbuf;
	ఒకవేళ (buf->b_fname != ఖాళీ)
	    new_name = vim_strsave(buf->b_fname);
	లేకపోతే
	    new_name = ఖాళీ;
	సమితి_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	ఒకవేళ (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_పంక్తి_లెక్క <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    ఒకవేళ (u_savecommon(0, curbuf->b_ml.ml_పంక్తి_లెక్క + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		వెళ్ళు theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	లేకపోతే
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	ఒకవేళ (!bufref_valid(&bufref))
	{
	    delbuf_సందేశం(new_name);	// frees new_name
	    వెళ్ళు theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	ఒకవేళ (buf != curbuf)
	    వెళ్ళు theend;
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (aborting())	    // autocmds may abort script processing
	    వెళ్ళు theend;
#అయితే_ముగింపు
	buf_clear_file(curbuf);
	curbuf->b_op_ప్రారంభం.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_చివర.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    // If the file name was changed, reset the not-edit flag so that ":write"
    // works.
    ఒకవేళ (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    ఒకవేళ (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#నిర్వచితమైతే FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		ఒకవేళ (win->w_బఫరు == curbuf)
		    foldUpdateAll(win);
	}
#అయితే_ముగింపు

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	ఒకవేళ (!oldbuf)			    // need to read the file
	{
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    ఒకవేళ (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#అయితే_ముగింపు
	    మార్పిడి_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    ఒకవేళ (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#ఒకవేళ defined(FEAT_EVAL)
	    ఒకవేళ (should_abort(open_బఫరు(FALSE, eap, readfile_flags)))
		retval = FAIL;
#లేకపోతే
	    (శూన్య)open_బఫరు(FALSE, eap, readfile_flags);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#అయితే_ముగింపు
	    ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_మార్పిడి_exists(&old_curbuf);
	}
	లేకపోతే
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE,
							      curbuf, &retval);
	    ఒకవేళ ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, ఖాళీ, ఖాళీ, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	ఒకవేళ (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *పాఠ్యం = ml_get_curline();

	    ఒకవేళ (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.నిలువు != (పూర్ణాంకం)(skipwhite(పాఠ్యం) - పాఠ్యం))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.నిలువు;
	    }
	}
	ఒకవేళ (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	మారింది_పంక్తి_abv_curs();

	maketitle();
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	ఒకవేళ (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_సమితి_title(curwin);
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    ఒకవేళ (curwin->w_p_diff)
    {
	diff_buf_కూడు(curbuf);
	diff_invalidate(curbuf);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    ఒకవేళ (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(శూన్య)parse_spelllang(curwin);
#అయితే_ముగింపు

    ఒకవేళ (command == ఖాళీ)
    {
	ఒకవేళ (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.నిలువు = newcol;
	    check_cursor();
	}
	లేకపోతే ఒకవేళ (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    ఒకవేళ (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.నిలువు = solcol;
		check_cursor_నిలువు();
		curwin->w_cursor.coladd = 0;
		curwin->w_సమితి_curswant = true;
	    }
	    లేకపోతే
		beginline(BL_SOL | BL_FIX);
	}
	లేకపోతే			// no line number, go to last line in Ex mode
	{
	    ఒకవేళ (exmode_చురుకైన)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    ఒకవేళ (oldbuf && !auto_buf)
    {
	పూర్ణాంకం	సందేశం_scroll_save = సందేశం_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	ఒకవేళ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    సందేశం_scroll = FALSE;
	ఒకవేళ (!సందేశం_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	సందేశం_ప్రారంభం();
	సందేశం_scroll = సందేశం_scroll_save;
	సందేశం_scrolled_ign = TRUE;

	ఒకవేళ (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	సందేశం_scrolled_ign = FALSE;
    }

#నిర్వచితమైతే FEAT_VIMINFO
    curbuf->b_చివరి_వాడిన = vim_సమయం();
#అయితే_ముగింపు

    ఒకవేళ (command != ఖాళీ)
	do_cmdline(command, ఖాళీ, ఖాళీ, DOCMD_VERBOSE|DOCMD_RANGEOK);

#నిర్వచితమైతే FEAT_KEYMAP
    ఒకవేళ (curbuf->b_kmap_state & KEYMAP_INIT)
	(శూన్య)keymap_init();
#అయితే_ముగింపు

    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    ఒకవేళ (!skip_redraw)
    {
	n = *so_ptr;
	ఒకవేళ (topline == 0 && command == ఖాళీ)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    ఒకవేళ (p_im && (State & బహుళకం_చొప్పించు) == 0)
	need_ప్రారంభం_insertmode = TRUE;

#నిర్వచితమైతే FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    ఒకవేళ (p_acd && curbuf->b_ffname != ఖాళీ)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	ఒకవేళ (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_NETBEANS_INTG)
    ఒకవేళ (curbuf->b_ffname != ఖాళీ)
    {
# ifdef FEAT_NETBEANS_INTG
	ఒకవేళ ((flags & ECMD_సమితి_HELP) != ECMD_సమితి_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#అయితే_ముగింపు

theend:
    ఒకవేళ (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (did_సమితి_swapcommand)
	సమితి_vim_var_string(VV_SWAPCOMMAND, ఖాళీ, -1);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BROWSE
    vim_free(browse_file);
#అయితే_ముగింపు
    vim_free(free_fname);
    తిరిగి retval;
}

    స్థిర శూన్య
delbuf_సందేశం(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_బఫరు_str),
					   name == ఖాళీ ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = ఖాళీ;
    au_new_curbuf.br_buf_free_లెక్క = 0;
}

స్థిర పూర్ణాంకం append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    స్థిర char_u *
get_append_పంక్తి(exarg_T *eap, పూర్ణాంకం indent)
{
    char_u	*theline;
    char_u	*p;

    ఒకవేళ (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    లేకపోతే ఒకవేళ (eap->ea_getline == ఖాళీ)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	ఒకవేళ (eap->nextcmd == ఖాళీ)
	    తిరిగి ఖాళీ;
	p = vim_strchr(eap->nextcmd, NL);
	ఒకవేళ (p == ఖాళీ)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	ఒకవేళ (*p != NUL)
	    ++p;
	లేకపోతే
	    p = ఖాళీ;
	eap->nextcmd = p;
    }
    లేకపోతే
    {
	పూర్ణాంకం save_State = State;

	// Set State to avoid the cursor shape to be set to MODE_INSERT
	// state when getline() returns.
	State = బహుళకం_CMDLINE;
	theline = eap->ea_getline(
#నిర్వచితమైతే FEAT_EVAL
		eap->cstack->cs_looplevel > 0 ? -1 :
#అయితే_ముగింపు
		NUL, eap->cookie, indent, TRUE);
	State = save_State;
    }
    తిరిగి theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    శూన్య
ex_append(exarg_T *eap)
{
    char_u	*theline;
    పూర్ణాంకం		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    పూర్ణాంకం		indent = 0;
    char_u	*p;
    పూర్ణాంకం		vcol;
    పూర్ణాంకం		ఖాళీలు_2 = (curbuf->b_ml.ml_flags & ML_ఖాళీలు);

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (not_లోపల_vim9(eap, eap->skip) == FAIL)
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	అయితే ((theline = get_append_పంక్తి(eap, 0)) != ఖాళీ)
	{
	    పూర్ణాంకం చివర = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    ఒకవేళ (చివర)
		ఆపు;
	}
	తిరిగి;
    }

    // the ! flag toggles autoindent
    ఒకవేళ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    ఒకవేళ (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    ఒకవేళ (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    ఒకవేళ (ఖాళీలు_2 && lnum == 1)
	lnum = 0;

    State = బహుళకం_చొప్పించు;		    // behave like in Insert mode
    ఒకవేళ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= బహుళకం_LANGMAP;

    కొరకు (;;)
    {
	సందేశం_scroll = TRUE;
	need_wait_return = FALSE;
	ఒకవేళ (curbuf->b_p_ai)
	{
	    ఒకవేళ (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    లేకపోతే ఒకవేళ (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_పంక్తి(eap, indent);
	పంక్తులు_ఎడమ = అడ్డువరుసలు - 1;
	ఒకవేళ (theline == ఖాళీ)
	    ఆపు;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	ఒకవేళ (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	కొరకు (p = theline; indent > vcol; ++p)
	{
	    ఒకవేళ (*p == ' ')
		++vcol;
	    లేకపోతే ఒకవేళ (*p == TAB)
		vcol += 8 - vcol % 8;
	    లేకపోతే
		ఆపు;
	}
	ఒకవేళ ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (ఖాళీలు_2 ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    ఆపు;
	}

	// don't use autoindent if nothing was typed.
	ఒకవేళ (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	ఒకవేళ (ఖాళీలు_2)
	    // there are no marks below the inserted lines
	    appended_పంక్తులు(lnum, 1L);
	లేకపోతే
	    appended_పంక్తులు_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	ఒకవేళ (ఖాళీలు_2)
	{
	    ml_delete(2L);
	    ఖాళీలు_2 = FALSE;
	}
    }
    State = బహుళకం_సాధారణ;

    ఒకవేళ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_ప్రారంభం.lnum = (eap->line2 < curbuf->b_ml.ml_పంక్తి_లెక్క) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_పంక్తి_లెక్క;
	ఒకవేళ (eap->cmdidx != CMD_append)
	    --curbuf->b_op_ప్రారంభం.lnum;
	curbuf->b_op_చివర.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_ప్రారంభం.lnum;
	curbuf->b_op_ప్రారంభం.నిలువు = curbuf->b_op_చివర.నిలువు = 0;
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
    శూన్య
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (not_లోపల_vim9(eap, eap->skip) == FAIL)
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (eap->skip)
    {
	ex_append(eap);
	తిరిగి;
    }
    ఒకవేళ (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	తిరిగి;

    // the ! flag toggles autoindent
    ఒకవేళ (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    కొరకు (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)	    // nothing to delete
	    ఆపు;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_పంక్తులు_mark(eap->line1, (దీర్ఘ)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    శూన్య
ex_z(exarg_T *eap)
{
    char_u	*x;
    దీర్ఘ	bigness;
    char_u	*రకం;
    పూర్ణాంకం		minus = 0;
    linenr_T	ప్రారంభం, చివర, curs, i;
    పూర్ణాంకం		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    ఒకవేళ (eap->forceit)
	bigness = అడ్డువరుసలు - 1;
    లేకపోతే ఒకవేళ (!ONE_కిటికీ)
	bigness = curwin->w_ఎత్తు - 3;
    లేకపోతే
	bigness = curwin->w_p_scr * 2;
    ఒకవేళ (bigness < 1)
	bigness = 1;

    x = eap->arg;
    రకం = x;
    ఒకవేళ (*రకం == '-' || *రకం == '+' || *రకం == '='
					      || *రకం == '^' || *రకం == '.')
	++x;
    అయితే (*x == '-' || *x == '+')
	++x;

    ఒకవేళ (*x != 0)
    {
	ఒకవేళ (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_వాదం_గమ్యం_z));
	    తిరిగి;
	}
	లేకపోతే
	{
	    bigness = atol((అక్షరం *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    ఒకవేళ (bigness > 2 * curbuf->b_ml.ml_పంక్తి_లెక్క || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_పంక్తి_లెక్క;

	    p_కిటికీ = bigness;
	    ఒకవేళ (*రకం == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    ఒకవేళ (*రకం == '-' || *రకం == '+')
	కొరకు (x = రకం + 1; *x == *రకం; ++x)
	    ;

    ఎంపిక (*రకం)
    {
	స్థితి '-':
	    ప్రారంభం = lnum - bigness * (linenr_T)(x - రకం) + 1;
	    చివర = ప్రారంభం + bigness - 1;
	    curs = చివర;
	    ఆపు;

	స్థితి '=':
	    ప్రారంభం = lnum - (bigness + 1) / 2 + 1;
	    చివర = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    ఆపు;

	స్థితి '^':
	    ప్రారంభం = lnum - bigness * 2;
	    చివర = lnum - bigness;
	    curs = lnum - bigness;
	    ఆపు;

	స్థితి '.':
	    ప్రారంభం = lnum - (bigness + 1) / 2 + 1;
	    చివర = lnum + (bigness + 1) / 2 - 1;
	    curs = చివర;
	    ఆపు;

	అప్రమేయ:  // '+'
	    ప్రారంభం = lnum;
	    ఒకవేళ (*రకం == '+')
		ప్రారంభం += bigness * (linenr_T)(x - రకం - 1) + 1;
	    లేకపోతే ఒకవేళ (eap->addr_లెక్క == 0)
		++ప్రారంభం;
	    చివర = ప్రారంభం + bigness - 1;
	    curs = చివర;
	    ఆపు;
    }

    ఒకవేళ (ప్రారంభం < 1)
	ప్రారంభం = 1;

    ఒకవేళ (చివర > curbuf->b_ml.ml_పంక్తి_లెక్క)
	చివర = curbuf->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (curs > curbuf->b_ml.ml_పంక్తి_లెక్క)
	curs = curbuf->b_ml.ml_పంక్తి_లెక్క;
    లేకపోతే ఒకవేళ (curs < 1)
	curs = 1;

    కొరకు (i = ప్రారంభం; i <= చివర; i++)
    {
	ఒకవేళ (minus && i == lnum)
	{
	    సందేశం_putchar('\n');

	    కొరకు (j = 1; j < Columns; j++)
		సందేశం_putchar('-');
	}

	print_పంక్తి(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	ఒకవేళ (minus && i == lnum)
	{
	    సందేశం_putchar('\n');

	    కొరకు (j = 1; j < Columns; j++)
		సందేశం_putchar('-');
	}
    }

    ఒకవేళ (curwin->w_cursor.lnum != curs)
    {
	curwin->w_cursor.lnum = curs;
	curwin->w_cursor.నిలువు = 0;
    }
    ex_no_reprint = TRUE;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    పూర్ణాంకం
check_restricted(శూన్య)
{
    ఒకవేళ (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_లోపల_rvim));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    పూర్ణాంకం
check_secure(శూన్య)
{
    ఒకవేళ (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_లోపల_current_dir_or_ట్యాగ్_శోధన));
	తిరిగి TRUE;
    }
#నిర్వచితమైతే HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    ఒకవేళ (sandbox != 0)
    {
	emsg(_(e_not_allowed_లోపల_sandbox));
	తిరిగి TRUE;
    }
#అయితే_ముగింపు
    తిరిగి FALSE;
}

స్థిర char_u	*old_sub = ఖాళీ;	// previous substitute pattern
స్థిర పూర్ణాంకం	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం	do_all;		// do multiple substitutions per line
    పూర్ణాంకం	do_ask;		// ask for confirmation
    పూర్ణాంకం	do_లెక్క;	// count only
    పూర్ణాంకం	do_error;	// if false, ignore errors
    పూర్ణాంకం	do_print;	// print last line with subs.
    పూర్ణాంకం	do_list;	// list last line with subs.
    పూర్ణాంకం	do_సంఖ్య;	// list last line with line nr
    పూర్ణాంకం	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *ప్రారంభం, పూర్ణాంకం delimiter)
{
    char_u *p = ప్రారంభం;

    అయితే (p[0])
    {
	ఒకవేళ (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    ఆపు;
	}
	ఒకవేళ (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    తిరిగి p;
}

    స్థిర పూర్ణాంకం
check_regexp_delim(పూర్ణాంకం c)
{
    ఒకవేళ (సురక్షిత_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    శూన్య
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    దీర్ఘ	i = 0;
    regmmatch_T regmatch;
    స్థిర subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#నిర్వచితమైతే FEAT_EVAL
    subflags_T	subflags_save;
#అయితే_ముగింపు
    పూర్ణాంకం		save_do_all;		// remember user specified 'g' flag
    పూర్ణాంకం		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = ఖాళీ;		// init for GCC
    string_T	pat = {ఖాళీ, 0};
    పూర్ణాంకం		delimiter;
    size_t	sublen;
    పూర్ణాంకం		got_quit = FALSE;
    పూర్ణాంకం		got_సరిపోలిక = FALSE;
    పూర్ణాంకం		which_pat;
    char_u	*cmd;
    char_u	*p;
    పూర్ణాంకం		save_State;
    linenr_T	మొదటి_పంక్తి = 0;		// first changed line
    linenr_T	చివరి_పంక్తి = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;
    linenr_T	line2;
    దీర్ఘ	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    పూర్ణాంకం		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    పూర్ణాంకం		ప్రారంభం_nsubs;
    పూర్ణాంకం		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		save_ma = 0;
    పూర్ణాంకం		save_sandbox = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    textprop_T	*పాఠ్యం_props = ఖాళీ;
#అయితే_ముగింపు

    cmd = eap->arg;
    ఒకవేళ (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    ప్రారంభం_nsubs = sub_nsubs;

    ఒకవేళ (eap->cmdidx == CMD_tilde)
	which_pat = RE_చివరి;	// use last used regexp
    లేకపోతే
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    ఒకవేళ (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == ఖాళీ)
    {
	// don't accept alphanumeric for separator
	ఒకవేళ (check_regexp_delim(*cmd) == FAIL)
	    తిరిగి;
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (లోపల_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    తిరిగి;
#అయితే_ముగింపు

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	ఒకవేళ (*cmd == '\\')
	{
	    ఒకవేళ (లోపల_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_లోపల_vim9_script));
		తిరిగి;
	    }
	    ++cmd;
	    ఒకవేళ (vim_strchr((char_u *)"/?&", *cmd) == ఖాళీ)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		తిరిగి;
	    }
	    ఒకవేళ (*cmd != '&')
		which_pat = RE_శోధన;	    // use last '/' pattern
	    STR_LITERAL_సమితి(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	లేకపోతే		// find the end of the regexp
	{
	    which_pat = RE_చివరి;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, ఖాళీ, ఖాళీ);
	    pat.నిడివి = (size_t)(cmd - pat.string);
	    ఒకవేళ (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	ఒకవేళ (sub == ఖాళీ)
	    // out of memory
	    తిరిగి;

	ఒకవేళ (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    ఒకవేళ (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != ఖాళీ)
	    {
		ఒకవేళ (old_sub == ఖాళీ)	// there is no previous command
		{
		    emsg(_(e_no_మునుపటి_substitute_regular_వ్యక్తీకరణ));
		    vim_free(sub);
		    తిరిగి;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		ఒకవేళ (sub == ఖాళీ)
		    // out of memory
		    తిరిగి;
	    }
	    లేకపోతే ఒకవేళ (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		ఒకవేళ (old_sub == ఖాళీ)
		{
		    // out of memory
		    vim_free(sub);
		    తిరిగి;
		}
	    }
	}
    }
    లేకపోతే ఒకవేళ (!eap->skip)	// use previous pattern and substitution
    {
	ఒకవేళ (old_sub == ఖాళీ)	// there is no previous command
	{
	    emsg(_(e_no_మునుపటి_substitute_regular_వ్యక్తీకరణ));
	    తిరిగి;
	}
	pat.string = ఖాళీ;		// search_regcomp() will use previous pattern
	pat.నిడివి = 0;
	sub = vim_strsave(old_sub);
	ఒకవేళ (sub == ఖాళీ)
	    తిరిగి;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    ఒకవేళ (pat.string != ఖాళీ && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_పంక్తులు_లెక్క;

	ఒకవేళ (eap->skip)
	{
	    vim_free(sub);
	    తిరిగి;
	}
	curwin->w_cursor.lnum = eap->line1;
	ఒకవేళ (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	లేకపోతే ఒకవేళ (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	లేకపోతే ఒకవేళ (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_పంక్తులు_లెక్క = eap->line2 - eap->line1 + 1;
	ఒకవేళ (eap->line2 < curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ++joined_పంక్తులు_లెక్క;
	ఒకవేళ (joined_పంక్తులు_లెక్క > 1)
	{
	    (శూన్య)do_join(joined_పంక్తులు_లెక్క, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_పంక్తులు_లెక్క - 1;
	    sub_nlines = 1;
	    (శూన్య)do_sub_సందేశం(FALSE);
	    ex_may_print(eap);
	}

	ఒకవేళ (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.నిడివి, magic_isset());
	// put pattern in history
	కూడు_గమ్యం_history(HIST_శోధన, pat.string, pat.నిడివి, TRUE, NUL);
	vim_free(sub);

	తిరిగి;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    ఒకవేళ (*cmd == '&')
	++cmd;
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (లోపల_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (!p_ed)
	{
	    ఒకవేళ (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    లేకపోతే
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_లెక్క = FALSE;
	subflags.do_సంఖ్య = FALSE;
	subflags.do_ic = 0;
    }
    అయితే (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	ఒకవేళ (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	లేకపోతే ఒకవేళ (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	లేకపోతే ఒకవేళ (*cmd == 'n')
	    subflags.do_లెక్క = TRUE;
	లేకపోతే ఒకవేళ (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	లేకపోతే ఒకవేళ (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_చివరి;
	లేకపోతే ఒకవేళ (*cmd == 'p')
	    subflags.do_print = TRUE;
	లేకపోతే ఒకవేళ (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_సంఖ్య = TRUE;
	}
	లేకపోతే ఒకవేళ (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	లేకపోతే ఒకవేళ (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	లేకపోతే ఒకవేళ (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	లేకపోతే
	    ఆపు;
	++cmd;
    }
    ఒకవేళ (subflags.do_లెక్క)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    ఒకవేళ (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	ఒకవేళ (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_ధనాత్మక_లెక్క_required));
	    vim_free(sub);
	    తిరిగి;
	}
	లేకపోతే ఒకవేళ (i >= గరిష్ఠ_పూర్ణాంకం)
	{
	    అక్షరం	buf[20];
	    vim_snprintf(buf, పరిమాణం(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    తిరిగి;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	ఒకవేళ (eap->line2 > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    eap->line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    ఒకవేళ (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	సమితి_nextcmd(eap, cmd);
	ఒకవేళ (eap->nextcmd == ఖాళీ)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    తిరిగి;
	}
    }

    ఒకవేళ (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	తిరిగి;
    }

    ఒకవేళ (!subflags.do_లెక్క && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	తిరిగి;
    }

    ఒకవేళ (శోధన_regcomp(pat.string, pat.నిడివి, ఖాళీ, RE_SUBST, which_pat, శోధన_HIS, &regmatch) == FAIL)
    {
	ఒకవేళ (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	తిరిగి;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    ఒకవేళ (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    లేకపోతే ఒకవేళ (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = ఖాళీ;
    sub_firstline.నిడివి = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    ఒకవేళ (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి;
	sub = p;
    }
    లేకపోతే
    {
	p = regtilde(sub, magic_isset());

	ఒకవేళ (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    కొరకు (lnum = eap->line1; lnum <= line2 && !(got_quit
#ఒకవేళ defined(FEAT_EVAL)
		|| aborting()
#అయితే_ముగింపు
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ఖాళీ);
	ఒకవేళ (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_ప్రారంభం = {ఖాళీ, 0};
	    size_t	new_ప్రారంభం_కొలత = 0;
	    char_u	*new_చివర;
	    పూర్ణాంకం		did_sub = FALSE;
	    పూర్ణాంకం		lastone;
	    size_t	copy_len;
	    size_t	needed_కొలత;
	    దీర్ఘ	nmatch_tl = 0;	// nr of lines matched below lnum
	    పూర్ణాంకం		do_again;	// do it again after joining lines
	    పూర్ణాంకం		skip_సరిపోలిక = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_విభజించు = false;	// "\r" split the line
#నిర్వచితమైతే FEAT_PROP_POPUP
	    పూర్ణాంకం		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	మొత్తం_added =  0;
	    పూర్ణాంకం		పాఠ్యం_prop_లెక్క = 0;
#అయితే_ముగింపు

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
	    ఒకవేళ (!got_సరిపోలిక)
	    {
		setpcmark();
		got_సరిపోలిక = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    కొరకు (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		ఒకవేళ (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
		    ఆపు;

		ఒకవేళ (sub_firstline.string == ఖాళీ)
		{
		    sub_firstline.నిడివి = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.నిడివి);
		    ఒకవేళ (sub_firstline.string == ఖాళీ)
		    {
			vim_free(new_ప్రారంభం.string);
			వెళ్ళు outofmem;
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
		ఒకవేళ (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].నిలువు)
		{
		    ఒకవేళ (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_సరిపోలిక = TRUE;
		    లేకపోతే
		    {
			 // search for a match at next column
			ఒకవేళ (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			లేకపోతే
			    ++matchcol;
		    }
		    వెళ్ళు skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].నిలువు;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		ఒకవేళ (subflags.do_లెక్క)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    ఒకవేళ (nmatch > 1)
		    {
			matchcol = (colnr_T)sub_firstline.నిడివి;
			nmatch = 1;
			skip_సరిపోలిక = TRUE;
		    }
		    sub_nsubs++;
		    did_sub = TRUE;
#నిర్వచితమైతే FEAT_EVAL
		    // Skip the substitution, unless an expression is used,
		    // then it is evaluated in the sandbox.
		    ఒకవేళ (!(sub[0] == '\\' && sub[1] == '='))
#అయితే_ముగింపు
			వెళ్ళు skip;
		}

		ఒకవేళ (subflags.do_ask)
		{
		    పూర్ణాంకం typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = బహుళకం_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.నిలువు = regmatch.startpos[0].నిలువు;
		    ఒకవేళ (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    ఒకవేళ (vim_strchr(p_cpo, CPO_UNDO) != ఖాళీ)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    అయితే (subflags.do_ask)
		    {
			ఒకవేళ (exmode_చురుకైన)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_పంక్తి_no_ఉపసర్గ(lnum,
					 subflags.do_సంఖ్య, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, ఖాళీ, ఖాళీ, 0);
			    curwin->w_cursor.నిలువు = regmatch.endpos[0].నిలువు - 1;
			    ఒకవేళ (curwin->w_cursor.నిలువు < 0)
				curwin->w_cursor.నిలువు = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   ఖాళీ, ఖాళీ, &ec, 0);
			    curwin->w_cursor.నిలువు = regmatch.startpos[0].నిలువు;
			    ఒకవేళ (subflags.do_సంఖ్య || curwin->w_p_nu)
			    {
				పూర్ణాంకం numw = సంఖ్య_వెడల్పు(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    సందేశం_ప్రారంభం();
			    కొరకు (i = 0; i < (దీర్ఘ)sc; ++i)
				సందేశం_putchar(' ');
			    కొరకు ( ; i <= (దీర్ఘ)ec; ++i)
				సందేశం_putchar('^');

			    resp = getexmodeline('?', ఖాళీ, 0, TRUE);
			    ఒకవేళ (resp != ఖాళీ)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				ఒకవేళ (ex_సాధారణ_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			లేకపోతే
			{
			    string_T orig_పంక్తి = {ఖాళీ, 0};
			    పూర్ణాంకం    len_change = 0;
			    పూర్ణాంకం	   save_p_lz = p_lz;
#నిర్వచితమైతే FEAT_FOLDING
			    పూర్ణాంకం save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#అయితే_ముగింపు
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    ఒకవేళ (new_ప్రారంభం.string != ఖాళీ)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_పంక్తి.నిడివి = ml_get_len(lnum);
				orig_పంక్తి.string = vim_strnsave(ml_get(lnum), orig_పంక్తి.నిడివి);
				ఒకవేళ (orig_పంక్తి.string != ఖాళీ)
				{
				    string_T new_పంక్తి;

				    new_పంక్తి.string = concat_str(new_ప్రారంభం.string,
						     sub_firstline.string + copycol);
				    ఒకవేళ (new_పంక్తి.string == ఖాళీ)
					VIM_CLEAR_STRING(orig_పంక్తి);
				    లేకపోతే
				    {
					new_పంక్తి.నిడివి =
					    new_ప్రారంభం.నిడివి + (sub_firstline.నిడివి - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (పూర్ణాంకం)new_పంక్తి.నిడివి - (పూర్ణాంకం)orig_పంక్తి.నిడివి;
					curwin->w_cursor.నిలువు += len_change;
					ml_replace(lnum, new_పంక్తి.string, FALSE);
				    }
				}
			    }

			    శోధన_సరిపోలిక_పంక్తులు = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    శోధన_సరిపోలిక_endcol = regmatch.endpos[0].నిలువు
								 + len_change;
			    ఒకవేళ (శోధన_సరిపోలిక_పంక్తులు == 0
						   && శోధన_సరిపోలిక_endcol == 0)
				// highlight at least one character for /^/
				శోధన_సరిపోలిక_endcol = 1;
			    highlight_సరిపోలిక = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_సరిపోలిక = FALSE;
			    redraw_later(UPD_SOME_VALID);

#నిర్వచితమైతే FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#అయితే_ముగింపు
			    ఒకవేళ (సందేశం_అడ్డువరుస == అడ్డువరుసలు - 1)
				సందేశం_didout = FALSE;	// avoid a scroll-up
			    సందేశం_starthere();
			    i = సందేశం_scroll;
			    సందేశం_scroll = 0;		// truncate msg when
							// needed
			    సందేశం_no_మరిన్ని = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    సందేశం_no_మరిన్ని = FALSE;
			    సందేశం_scroll = i;
			    showruler(TRUE);
			    windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
			    RedrawingDisabled = save_RedrawingDisabled;

#నిర్వచితమైతే USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#అయితే_ముగింపు
			    ++no_mapping;	// don't map this key
			    ++allow_కీలకాలు;	// allow special keys
			    typed = plain_vgetc();
			    --allow_కీలకాలు;
			    --no_mapping;

			    // clear the question
			    సందేశం_didout = FALSE;	// don't scroll up
			    సందేశం_నిలువు = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    ఒకవేళ (orig_పంక్తి.string != ఖాళీ)
				ml_replace(lnum, orig_పంక్తి.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			ఒకవేళ (typed == 'q' || typed == ESC || typed == Ctrl_C
#నిర్వచితమైతే UNIX
				|| typed == intr_char
#అయితే_ముగింపు
				)
			{
			    got_quit = TRUE;
			    ఆపు;
			}
			ఒకవేళ (typed == 'n')
			    ఆపు;
			ఒకవేళ (typed == 'y')
			    ఆపు;
			ఒకవేళ (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    ఆపు;
			}
			ఒకవేళ (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    ఆపు;
			}
			ఒకవేళ (typed == Ctrl_E)
			    scrollup_clamp();
			లేకపోతే ఒకవేళ (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    ఒకవేళ (vim_strchr(p_cpo, CPO_UNDO) != ఖాళీ)
			--no_u_sync;

		    ఒకవేళ (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			ఒకవేళ (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.నిడివి;
			    skip_సరిపోలిక = TRUE;
			}
			వెళ్ళు skip;
		    }
		    ఒకవేళ (got_quit)
			వెళ్ళు skip;
		}

		// Move the cursor to the start of the match, so that we can
		// use "\=col(".").
		curwin->w_cursor.నిలువు = regmatch.startpos[0].నిలువు;

		/*
		 * 3. substitute the string.
		 */
#నిర్వచితమైతే FEAT_EVAL
		save_ma = curbuf->b_p_ma;
		save_sandbox = sandbox;
		ఒకవేళ (subflags.do_లెక్క)
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
#అయితే_ముగింపు
		// Get length of substitution part, including the NUL.
		// When it fails sublen is zero.
		sublen = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, sub_firstline.string, 0,
		    REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));
#నిర్వచితమైతే FEAT_EVAL
		--textlock;

		// If getting the substitute string caused an error, don't do
		// the replacement.
		// Don't keep flags set by a recursive call.
		subflags = subflags_save;
		ఒకవేళ (sublen == 0 || aborting() || subflags.do_లెక్క)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    వెళ్ళు skip;
		}
#అయితే_ముగింపు

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		ఒకవేళ (nmatch > curbuf->b_ml.ml_పంక్తి_లెక్క - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_పంక్తి_లెక్క - sub_firstlnum + 1;
		    skip_సరిపోలిక = TRUE;
		    // safety check
		    ఒకవేళ (nmatch < 0)
			వెళ్ళు skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		ఒకవేళ (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.నిడివి = sub_firstline.నిడివి;
#నిర్వచితమైతే FEAT_PROP_POPUP
		    ఒకవేళ (curbuf->b_has_textprop)
		    {
			పూర్ణాంకం బైట్లు_added = (పూర్ణాంకం)sublen - 1 - (regmatch.endpos[0].నిలువు
						   - regmatch.startpos[0].నిలువు);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			ఒకవేళ (adjust_prop_columns(lnum,
					మొత్తం_added + regmatch.startpos[0].నిలువు,
						       బైట్లు_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			మొత్తం_added += (colnr_T)బైట్లు_added;
		    }
#అయితే_ముగింపు
		}
		లేకపోతే
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#నిర్వచితమైతే FEAT_PROP_POPUP
		    ఒకవేళ (curbuf->b_has_textprop)
		    {
			char_u	*prop_ప్రారంభం;

			// Props in the first line may be shortened or deleted
			ఒకవేళ (adjust_prop_columns(lnum,
					మొత్తం_added + regmatch.startpos[0].నిలువు,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			మొత్తం_added -=
			    (colnr_T)(sub_firstline.నిడివి - regmatch.startpos[0].నిలువు);

			// Props in the last line may be moved or deleted
			ఒకవేళ (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].నిలువు, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			పాఠ్యం_prop_లెక్క = get_పాఠ్యం_props(curbuf, lastlnum,
							   &prop_ప్రారంభం, FALSE);
			ఒకవేళ (పాఠ్యం_prop_లెక్క > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(పాఠ్యం_props);
			    పాఠ్యం_props = ALLOC_MULT(textprop_T,
							      పాఠ్యం_prop_లెక్క);
			    ఒకవేళ (పాఠ్యం_props != ఖాళీ)
			    {
				mch_memmove(పాఠ్యం_props, prop_ప్రారంభం,
					 పాఠ్యం_prop_లెక్క * పరిమాణం(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				పూర్ణాంకం wi = 0;
				కొరకు (పూర్ణాంకం pi = 0; pi < పాఠ్యం_prop_లెక్క; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    ఒకవేళ (పాఠ్యం_props[pi].tp_id < 0
					    || (పాఠ్యం_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					కొనసాగు;
				    పాఠ్యం_props[wi] = పాఠ్యం_props[pi];
				    పాఠ్యం_props[wi].tp_నిలువు +=
					regmatch.startpos[0].నిలువు + (colnr_T)sublen - 1;
				    పాఠ్యం_props[wi].u.tp_పాఠ్యం = ఖాళీ;
				    ++wi;
				}
				పాఠ్యం_prop_లెక్క = wi;
			    }
			}
		    }
#అయితే_ముగింపు
		    tmp.string = ml_get(lastlnum);
		    tmp.నిడివి = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#నిర్వచితమైతే FEAT_PROP_POPUP
		    ఒకవేళ (curbuf->b_has_textprop)
			మొత్తం_added +=
			    (colnr_T)(tmp.నిడివి - regmatch.endpos[0].నిలువు);
#అయితే_ముగింపు
		}

		copy_len = (size_t)(regmatch.startpos[0].నిలువు - copycol);
		needed_కొలత = copy_len
		    + (tmp.నిడివి - (size_t)regmatch.endpos[0].నిలువు) + sublen + 1;

		ఒకవేళ (new_ప్రారంభం.string == ఖాళీ)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_ప్రారంభం_కొలత = needed_కొలత + 50;
		    ఒకవేళ ((new_ప్రారంభం.string = alloc_clear(new_ప్రారంభం_కొలత)) == ఖాళీ)
			వెళ్ళు outofmem;
		    *new_ప్రారంభం.string = NUL;
		    new_ప్రారంభం.నిడివి = 0;
		}
		లేకపోతే
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_కొలత += new_ప్రారంభం.నిడివి;
		    ఒకవేళ (needed_కొలత > new_ప్రారంభం_కొలత)
		    {
			new_ప్రారంభం_కొలత = needed_కొలత + 50;
			ఒకవేళ ((p1 = alloc_clear(new_ప్రారంభం_కొలత)) == ఖాళీ)
			{
			    vim_free(new_ప్రారంభం.string);
			    వెళ్ళు outofmem;
			}
			mch_memmove(p1, new_ప్రారంభం.string, new_ప్రారంభం.నిడివి + 1);
			vim_free(new_ప్రారంభం.string);
			new_ప్రారంభం.string = p1;
		    }
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_ప్రారంభం.string + new_ప్రారంభం.నిడివి,
		    sub_firstline.string + copycol, copy_len);
		new_ప్రారంభం.నిడివి += copy_len;

		// new text added here
		new_చివర = new_ప్రారంభం.string + new_ప్రారంభం.నిడివి;

		ఒకవేళ (new_ప్రారంభం_కొలత - copy_len < sublen)
		    sublen = new_ప్రారంభం_కొలత - copy_len - 1;

#నిర్వచితమైతే FEAT_EVAL
		++textlock;
#అయితే_ముగింపు
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_చివర, (పూర్ణాంకం)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		ఒకవేళ (n > 0)
		    new_ప్రారంభం.నిడివి += n - 1;	    // remove 1 for the NUL

#నిర్వచితమైతే FEAT_EVAL
		--textlock;
#అయితే_ముగింపు
		sub_nsubs++;
		did_sub = TRUE;

		// Move the cursor to the start of the line, to avoid that it
		// is beyond the end of the line after the substitution.
		curwin->w_cursor.నిలువు = 0;

		// For a multi-line match, make a copy of the last matched
		// line and continue in that one.
		ఒకవేళ (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.నిడివి = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.నిడివి);
		    ఒకవేళ (sub_firstline.string == ఖాళీ)
		    {
			vim_free(new_ప్రారంభం.string);
			వెళ్ళు outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    ఒకవేళ (sub_firstlnum <= line2)
			do_again = TRUE;
		    లేకపోతే
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].నిలువు;

		ఒకవేళ (skip_సరిపోలిక)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    ఒకవేళ (sub_firstline.string == ఖాళీ)
		    {
			vim_free(new_ప్రారంభం.string);
			వెళ్ళు outofmem;
		    }
		    sub_firstline.నిడివి = 0;
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
		కొరకు (p1 = new_చివర; *p1; ++p1)
		{
		    ఒకవేళ (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_ప్రారంభం.నిడివి - (p1 - new_ప్రారంభం.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_ప్రారంభం.నిడివి;
#నిర్వచితమైతే FEAT_PROP_POPUP
			ఒకవేళ (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    ఒకవేళ (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_ప్రారంభం.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#అయితే_ముగింపు
		    }
		    లేకపోతే ఒకవేళ (*p1 == CAR)
		    {
			ఒకవేళ (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_ప్రారంభం.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_ప్రారంభం.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    ఒకవేళ (subflags.do_ask)
				appended_పంక్తులు(lnum - 1, 1L);
			    లేకపోతే
			    {
				ఒకవేళ (మొదటి_పంక్తి == 0)
				    మొదటి_పంక్తి = lnum;
				చివరి_పంక్తి = lnum + 1;
			    }
#నిర్వచితమైతే FEAT_PROP_POPUP
			    adjust_props_for_విభజించు(lnum + 1, lnum,
							       plen, 1, FALSE);
#అయితే_ముగింపు
			    // all line numbers increase
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    did_విభజించు = true;
			    // move the cursor to the new line, like Vi
			    ++curwin->w_cursor.lnum;
			    // copy the rest
			    n = new_ప్రారంభం.నిడివి - (size_t)plen;
			    mch_memmove(new_ప్రారంభం.string, p1 + 1, n + 1);
			    new_ప్రారంభం.నిడివి = n;
			    p1 = new_ప్రారంభం.string - 1;
			}
		    }
		    లేకపోతే ఒకవేళ (has_mbyte)
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
		lastone = (skip_సరిపోలిక
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
		ఒకవేళ (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_విభజించు)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, ఖాళీ)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    ఒకవేళ (new_ప్రారంభం.string != ఖాళీ)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * "matchcol" has to be adjusted, we use the end of
			 * the line as reference, because the substitute may
			 * have changed the number of characters.  Same for
			 * "prev_matchcol".
			 */
			STRCPY(new_ప్రారంభం.string + new_ప్రారంభం.నిడివి,
			    sub_firstline.string + copycol);
			new_ప్రారంభం.నిడివి += sub_firstline.నిడివి - (size_t)copycol;
			matchcol = (colnr_T)(sub_firstline.నిడివి - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.నిడివి
							      - prev_matchcol);

			ఒకవేళ (u_savesub(lnum) != OK)
			    ఆపు;
			ml_replace(lnum, new_ప్రారంభం.string, TRUE);
#నిర్వచితమైతే FEAT_PROP_POPUP
			ఒకవేళ (పాఠ్యం_props != ఖాళీ)
			    కూడు_పాఠ్యం_props(lnum, పాఠ్యం_props, పాఠ్యం_prop_లెక్క);
#అయితే_ముగింపు
			ఒకవేళ (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    ఒకవేళ (u_savedel(lnum, nmatch_tl) != OK)
				ఆపు;
			    కొరకు (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    ఒకవేళ (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (దీర్ఘ)MAXLNUM, -nmatch_tl);
			    లేకపోతే
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (దీర్ఘ)MAXLNUM, -nmatch_tl);
			    ఒకవేళ (subflags.do_ask)
				deleted_పంక్తులు(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			ఒకవేళ (subflags.do_ask)
			    మారింది_బైట్లు(lnum, 0);
			లేకపోతే
			{
			    ఒకవేళ (మొదటి_పంక్తి == 0)
				మొదటి_పంక్తి = lnum;
			    చివరి_పంక్తి = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_ప్రారంభం.string;
			sub_firstline.నిడివి = new_ప్రారంభం.నిడివి;
			new_ప్రారంభం.string = ఖాళీ;
			new_ప్రారంభం.నిడివి = 0;
			matchcol = (colnr_T)(sub_firstline.నిడివి - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.నిడివి
							      - prev_matchcol);
			copycol = 0;
			did_విభజించు = false;
		    }
		    ఒకవేళ (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, ఖాళీ);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    ఒకవేళ (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			ఒకవేళ (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			ఆపు;
		    }
		}

		పంక్తి_breakcheck();
	    }

	    ఒకవేళ (did_sub)
		++sub_nlines;
	    vim_free(new_ప్రారంభం.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	పంక్తి_breakcheck();
    }

    ఒకవేళ (మొదటి_పంక్తి != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_పంక్తి_లెక్క - old_పంక్తి_లెక్క;
	మారింది_పంక్తులు(మొదటి_పంక్తి, 0, చివరి_పంక్తి - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#నిర్వచితమైతే FEAT_PROP_POPUP
    vim_free(పాఠ్యం_props);
#అయితే_ముగింపు

    // ":s/pat//n" doesn't move the cursor
    ఒకవేళ (subflags.do_లెక్క)
	curwin->w_cursor = old_cursor;

    ఒకవేళ (sub_nsubs > ప్రారంభం_nsubs)
    {
	ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_ప్రారంభం.lnum = eap->line1;
	    curbuf->b_op_చివర.lnum = line2;
	    curbuf->b_op_ప్రారంభం.నిలువు = curbuf->b_op_చివర.నిలువు = 0;
	}

	ఒకవేళ (!global_busy)
	{
	    // when interactive leave cursor on the match
	    ఒకవేళ (!subflags.do_ask)
	    {
		ఒకవేళ (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		లేకపోతే
		    beginline(BL_WHITE | BL_FIX);
	    }
	    ఒకవేళ (!do_sub_సందేశం(subflags.do_లెక్క) && subflags.do_ask)
		సందేశం("");
	}
	లేకపోతే
	    global_need_beginline = TRUE;
	ఒకవేళ (subflags.do_print)
	    print_పంక్తి(curwin->w_cursor.lnum,
					 subflags.do_సంఖ్య, subflags.do_list);
    }
    లేకపోతే ఒకవేళ (!global_busy)
    {
	ఒకవేళ (got_int)		// interrupted
	    emsg(_(e_interrupted));
	లేకపోతే ఒకవేళ (got_సరిపోలిక)	// did find something but nothing substituted
	    సందేశం("");
	లేకపోతే ఒకవేళ (subflags.do_error)	// nothing found
	    semsg(_(e_నమూనా_not_దొరికింది_str), get_శోధన_pat());
    }

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (subflags.do_ask && hasAnyFolding(curwin))
	// Cursor position may require updating
	మారింది_కిటికీ_setting();
#అయితే_ముగింపు

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
    పూర్ణాంకం
do_sub_సందేశం(
    పూర్ణాంకం	    లెక్క_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    ఒకవేళ (((sub_nsubs > p_report && (కీలకం_Typed || sub_nlines > 1 || p_report < 1))
		|| లెక్క_only)
	    && messaging())
    {
	అక్షరం	*సందేశం_single;
	అక్షరం	*సందేశం_plural;

	ఒకవేళ (got_int)
	    STRCPY(సందేశం_buf, _("(Interrupted) "));
	లేకపోతే
	    *సందేశం_buf = NUL;

	సందేశం_single = లెక్క_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	సందేశం_plural = లెక్క_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_కూడు(సందేశం_buf, పరిమాణం(సందేశం_buf),
				 NGETTEXT(సందేశం_single, సందేశం_plural, sub_nlines),
				 sub_nsubs, (దీర్ఘ)sub_nlines);

	ఒకవేళ (సందేశం(సందేశం_buf))
	    // save message to display it after redraw
	    సమితి_keep_సందేశం((char_u *)సందేశం_buf, 0);
	తిరిగి TRUE;
    }
    ఒకవేళ (got_int)
    {
	emsg(_(e_interrupted));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    స్థిర శూన్య
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.నిలువు = 0;
    ఒకవేళ (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", ఖాళీ, ఖాళీ, DOCMD_NOWAIT);
    లేకపోతే
	do_cmdline(cmd, ఖాళీ, ఖాళీ, DOCMD_NOWAIT);
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
    శూన్య
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    పూర్ణాంకం		ndone = 0;
    పూర్ణాంకం		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*వాడిన_pat;
    regmmatch_T	regmatch;
    పూర్ణాంకం		సరిపోలిక;
    పూర్ణాంకం		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    ఒకవేళ (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_పంక్తి_లెక్క))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	తిరిగి;
    }

    ఒకవేళ (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    లేకపోతే
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_చివరి;	    // default: use last used regexp

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (లోపల_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	తిరిగి;
#అయితే_ముగింపు

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    ఒకవేళ (*cmd == '\\')
    {
	++cmd;
	ఒకవేళ (vim_strchr((char_u *)"/?&", *cmd) == ఖాళీ)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    తిరిగి;
	}
	ఒకవేళ (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	లేకపోతే
	    which_pat = RE_శోధన;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    లేకపోతే ఒకవేళ (*cmd == NUL)
    {
	emsg(_(e_regular_వ్యక్తీకరణ_missing_from_global));
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (check_regexp_delim(*cmd) == FAIL)
    {
	తిరిగి;
    }
    లేకపోతే
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, ఖాళీ, ఖాళీ);
	ఒకవేళ (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    ఒకవేళ (శోధన_regcomp(pat, patlen, &వాడిన_pat, RE_BOTH, which_pat, శోధన_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	తిరిగి;
    }

    ఒకవేళ (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	సరిపోలిక = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ఖాళీ);
	ఒకవేళ ((type == 'g' && సరిపోలిక) || (type == 'v' && !సరిపోలిక))
	    global_exe_one(cmd, lnum);
    }
    లేకపోతే
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	కొరకు (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    సరిపోలిక = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ఖాళీ);
	    ఒకవేళ (regmatch.regprog == ఖాళీ)
		ఆపు;  // re-compiling regprog failed
	    ఒకవేళ ((type == 'g' && సరిపోలిక) || (type == 'v' && !సరిపోలిక))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    పంక్తి_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	ఒకవేళ (got_int)
	    సందేశం(_(e_interrupted));
	లేకపోతే ఒకవేళ (ndone == 0)
	{
	    ఒకవేళ (type == 'v')
	    {
		ఒకవేళ (లోపల_vim9script())
		    semsg(_(e_నమూనా_దొరికింది_లోపల_every_పంక్తి_str), వాడిన_pat);
		లేకపోతే
		    smsg(_("Pattern found in every line: %s"), వాడిన_pat);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (లోపల_vim9script())
		    semsg(_(e_నమూనా_not_దొరికింది_str), వాడిన_pat);
		లేకపోతే
		    smsg(_("Pattern not found: %s"), వాడిన_pat);
	    }
	}
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	    inc_clip_provider();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
	    ప్రారంభం_global_changes();
#అయితే_ముగింపు
	    global_exe(cmd);
#నిర్వచితమైతే FEAT_CLIPBOARD
	    చివర_global_changes();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	    dec_clip_provider();
#అయితే_ముగింపు
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Execute "cmd" on lines marked with ml_setmarked().
 */
    శూన్య
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
    సందేశం_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_పంక్తి_లెక్క;
    అయితే (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    ఒకవేళ (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    లేకపోతే
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    మారింది_పంక్తి_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    ఒకవేళ (సందేశం_నిలువు == 0 && సందేశం_scrolled == 0)
	సందేశం_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    ఒకవేళ (!do_sub_సందేశం(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_పంక్తి_లెక్క - old_lcount);
}

#నిర్వచితమైతే FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(శూన్య)
{
    తిరిగి old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    శూన్య
సమితి_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#అయితే_ముగింపు // FEAT_VIMINFO

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_old_sub(శూన్య)
{
    vim_free(old_sub);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    పూర్ణాంకం
prepare_tagpreview(
    పూర్ణాంకం		undo_sync,	    // sync undo when leaving the window
    పూర్ణాంకం		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    ఒకవేళ (curwin->w_p_pvw)
	తిరిగి FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    ఒకవేళ (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_కనుగొను_preview_కిటికీ();
	ఒకవేళ (wp != ఖాళీ)
	    popup_సమితి_wantpos_cursor(wp, wp->w_minwidth, ఖాళీ);
    }
    లేకపోతే ఒకవేళ (use_popup != USEPOPUP_NONE)
    {
	wp = popup_కనుగొను_info_కిటికీ();
	ఒకవేళ (wp != ఖాళీ)
	{
	    ఒకవేళ (use_popup == USEPOPUP_సాధారణ)
		popup_show(wp);
	    లేకపోతే
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    లేకపోతే
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_p_pvw)
		ఆపు;
    }
    ఒకవేళ (wp != ఖాళీ)
    {
	win_enter(wp, undo_sync);
	తిరిగి FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    ఒకవేళ ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	తిరిగి popup_create_preview_కిటికీ(use_popup != USEPOPUP_NONE);
# endif
    ఒకవేళ (win_విభజించు(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	తిరిగి FALSE;
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
    తిరిగి TRUE;
}

#అయితే_ముగింపు

/*
 * Make the user happy.
 */
    శూన్య
ex_smile(exarg_T *eap వాడని)
{
    స్థిర అక్షరం *సంకేతం_2[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    అక్షరం *p;
    పూర్ణాంకం n;
    పూర్ణాంకం i;

    సందేశం_ప్రారంభం();
    సందేశం_putchar('\n');
    కొరకు (i = 0; i < 2; ++i)
	కొరకు (p = సంకేతం_2[i]; *p != NUL; ++p)
	    ఒకవేళ (*p == 'x')
		సందేశం_putchar('\n');
	    లేకపోతే
		కొరకు (n = *p++; n > 0; --n)
		    ఒకవేళ (*p == 'o' || *p == '$')
			సందేశం_putchar_attr(*p, HL_ATTR(HLF_L));
		    లేకపోతే
			సందేశం_putchar(*p);
    సందేశం_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    శూన్య
ex_drop(exarg_T *eap)
{
    పూర్ణాంకం		విభజించు = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    ఒకవేళ (ERROR_IF_POPUP_కిటికీ || ERROR_IF_పదం_POPUP_కిటికీ)
	తిరిగి;

    /*
     * Check if the first argument is already being edited in a window.  If
     * so, jump to that window.
     * We would actually need to check all arguments, but that's complicated
     * and mostly only one file is dropped.
     * This also ignores wildcards, since it is very unlikely the user is
     * editing a file name with a wildcard character.
     */
    సమితి_arglist(eap->arg);

    /*
     * Expanding wildcards may result in an empty argument list.  E.g. when
     * editing "foo.pyc" and ".pyc" is in 'wildignore'.  Assume that we
     * already did an error message for this.
     */
    ఒకవేళ (ARGCOUNT == 0)
	తిరిగి;

    ఒకవేళ (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	తిరిగి;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ఒకవేళ (wp->w_బఫరు == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    ఒకవేళ (!buf_Is_మారింది(curbuf))
	    {
		పూర్ణాంకం save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
		ex_rewind(eap);
	    తిరిగి;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    ఒకవేళ (!buf_hide(curbuf))
    {
	++emsg_off;
	విభజించు = check_మారింది(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    ఒకవేళ (విభజించు)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    లేకపోతే
	eap->cmdidx = CMD_మొదటి;
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
skip_vimgrep_pat(char_u *p, char_u **s, పూర్ణాంకం *flags)
{
    తిరిగి skip_vimgrep_pat_ext(p, s, flags, ఖాళీ, ఖాళీ);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, పూర్ణాంకం *flags, char_u **nulp, పూర్ణాంకం *cp)
{
    పూర్ణాంకం		c;

    ఒకవేళ (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	ఒకవేళ (s != ఖాళీ)
	    *s = p;
	p = skiptowhite(p);
	ఒకవేళ (s != ఖాళీ && *p != NUL)
	{
	    ఒకవేళ (nulp != ఖాళీ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    లేకపోతే
    {
	// ":vimgrep /pattern/[g][j] fname"
	ఒకవేళ (s != ఖాళీ)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	ఒకవేళ (*p != c)
	    తిరిగి ఖాళీ;

	// Truncate the pattern.
	ఒకవేళ (s != ఖాళీ)
	{
	    ఒకవేళ (nulp != ఖాళీ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	అయితే (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    ఒకవేళ (flags != ఖాళీ)
	    {
		ఒకవేళ (*p == 'g')
		    *flags |= VGR_GLOBAL;
		లేకపోతే ఒకవేళ (*p == 'j')
		    *flags |= VGR_NOJUMP;
		లేకపోతే
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    తిరిగి p;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    శూన్య
ex_oldfiles(exarg_T *eap వాడని)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    పూర్ణాంకం		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    పూర్ణాంకం		matches = -1;

    ఒకవేళ (l == ఖాళీ)
    {
	సందేశం(_("No old files"));
	తిరిగి;
    }

    సందేశం_ప్రారంభం();
    సందేశం_scroll = TRUE;
    కొరకు (li = l->lv_మొదటి; li != ఖాళీ && !got_int; li = li->li_తదుపరి)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	ఒకవేళ (!సందేశం_filtered(fname))
	{
	    ఒకవేళ (matches < 0)
		matches = nr;
	    లేకపోతే
		matches = 0;
	    సందేశం_outnum((దీర్ఘ)nr);
	    సందేశం_puts(": ");
	    సందేశం_outtrans(fname);
	    సందేశం_clr_eos();
	    సందేశం_putchar('\n');
	    బహిర్గతం_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_మరిన్ని = FALSE;
	// we only need to prompt if there is more than 1 match
	ఒకవేళ (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	లేకపోతే
	    nr = prompt_for_సంఖ్య(FALSE);
	సందేశం_starthere();
	ఒకవేళ (nr > 0)
	{
	    char_u *p = list_కనుగొను_str(get_vim_var_list(VV_OLDFILES),
		    (దీర్ఘ)nr);

	    ఒకవేళ (p != ఖాళీ)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, ఖాళీ);
		vim_free(p);
	    }
	}
    }
# endif
}
#అయితే_ముగింపు
