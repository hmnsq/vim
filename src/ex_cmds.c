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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

ਸਥਿਰ ਪੂਰਨ linelen(ਪੂਰਨ *has_tab);
ਸਥਿਰ ਖਾਲੀ do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, ਪੂਰਨ do_ਵਿੱਚ, ਪੂਰਨ do_ਬਾਹਰਲਾ);
ਸਥਿਰ ਪੂਰਨ not_writing(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ check_readonly(ਪੂਰਨ *forceit, buf_T *buf);
ਸਥਿਰ ਖਾਲੀ delbuf_ਸੁਨੇਹਾ(char_u *name);

/*
 * ":ascii" and "ga".
 */
    ਖਾਲੀ
do_ascii(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		cval;
    ਅੱਖਰ	buf1[20];
    ਅੱਖਰ	buf2[20];
    char_u	buf3[7];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
    char_u      *dig;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		cc[MAX_MCO];
    ਪੂਰਨ		ci = 0;
    ਪੂਰਨ		len;

    ਜੇ (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    ਨਹੀਂਤਾਂ
	c = gchar_cursor();
    ਜੇ (c == NUL)
    {
	ਸੁਨੇਹਾ("NUL");
	ਵਾਪਸ;
    }

    IObuff[0] = NUL;
    ਜੇ (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	ਜੇ (c == NL)	    // NUL is stored as NL
	    c = NUL;
	ਜੇ (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	ਨਹੀਂਤਾਂ
	    cval = c;
	ਜੇ (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, ਆਕਾਰ(buf1), "  <%s>", (ਅੱਖਰ *)buf3);
	}
	ਨਹੀਂਤਾਂ
	    buf1[0] = NUL;
	ਜੇ (c >= 0x80)
	    vim_snprintf(buf2, ਆਕਾਰ(buf2), "  <M-%s>",
						 (ਅੱਖਰ *)transchar(c & 0x7f));
	ਨਹੀਂਤਾਂ
	    buf2[0] = NUL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	ਜੇ (dig != ਨਿਰਾ)
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	ਜੇ (enc_utf8)
	    c = cc[ci++];
	ਨਹੀਂਤਾਂ
	    c = 0;
    }

    // Repeat for combining characters.
    ਜਦੋਂ (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (ਪੂਰਨ)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	ਜੇ (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	ਜੇ (enc_utf8 && utf_iscomposing(c)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GUI
		&& !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	ਜੇ (dig != ਨਿਰਾ)
	    vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	ਜੇ (ci == MAX_MCO)
	    ਰੋਕੋ;
	ਜੇ (enc_utf8)
	    c = cc[ci++];
	ਨਹੀਂਤਾਂ
	    c = 0;
    }

    ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    ਖਾਲੀ
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    ਪੂਰਨ		len;
    ਪੂਰਨ		indent = 0;
    ਪੂਰਨ		new_indent;
    ਪੂਰਨ		has_tab;
    ਪੂਰਨ		ਚੌੜਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl)
    {
	// switch left and right aligning
	ਜੇ (eap->cmdidx == CMD_ਸੱਜਾ)
	    eap->cmdidx = CMD_ਖੱਬਾ;
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_ਖੱਬਾ)
	    eap->cmdidx = CMD_ਸੱਜਾ;
    }
#ਜੇ_ਅੰਤ

    ਚੌੜਾਈ = atoi((ਅੱਖਰ *)eap->arg);
    save_curpos = curwin->w_cursor;
    ਜੇ (eap->cmdidx == CMD_ਖੱਬਾ)    // width is used for new indent
    {
	ਜੇ (ਚੌੜਾਈ >= 0)
	    indent = ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	ਜੇ (ਚੌੜਾਈ <= 0)
	    ਚੌੜਾਈ = curbuf->b_p_tw;
	ਜੇ (ਚੌੜਾਈ == 0 && curbuf->b_p_wm > 0)
	    ਚੌੜਾਈ = curwin->w_ਚੌੜਾਈ - curbuf->b_p_wm;
	ਜੇ (ਚੌੜਾਈ <= 0)
	    ਚੌੜਾਈ = 80;
    }

    ਜੇ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ਵਾਪਸ;

    ਲਈ (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	ਜੇ (eap->cmdidx == CMD_ਖੱਬਾ)		// left align
	    new_indent = indent;
	ਨਹੀਂਤਾਂ
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_ਸੱਜਾ ? &has_tab
						   : ਨਿਰਾ) - get_indent();

	    ਜੇ (len <= 0)			// skip blank lines
		ਜਾਰੀ;

	    ਜੇ (eap->cmdidx == CMD_center)
		new_indent = (ਚੌੜਾਈ - len) / 2;
	    ਨਹੀਂਤਾਂ
	    {
		new_indent = ਚੌੜਾਈ - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		ਜੇ (has_tab)
		    ਜਦੋਂ (new_indent > 0)
		    {
			(ਖਾਲੀ)ਸੈੱਟ_indent(new_indent, 0);
			ਜੇ (linelen(ਨਿਰਾ) <= ਚੌੜਾਈ)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    ਕਰੋ
				(ਖਾਲੀ)ਸੈੱਟ_indent(++new_indent, 0);
			    ਜਦੋਂ (linelen(ਨਿਰਾ) <= ਚੌੜਾਈ);
			    --new_indent;
			    ਰੋਕੋ;
			}
			--new_indent;
		    }
	    }
	}
	ਜੇ (new_indent < 0)
	    new_indent = 0;
	(ਖਾਲੀ)ਸੈੱਟ_indent(new_indent, 0);		// set indent
    }
    ਬਦਲਿਆ_ਸਤਰਾਂ(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    ਸਥਿਰ ਪੂਰਨ
linelen(ਪੂਰਨ *has_tab)
{
    char_u  *ਸਤਰ;
    char_u  *ਪਹਿਲਾ;
    char_u  *ਆਖਰੀ;
    ਪੂਰਨ	    save;
    ਪੂਰਨ	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    ਸਤਰ = ml_get_curline();
    ਜੇ (*ਸਤਰ == NUL)
	ਵਾਪਸ 0;

    // find the first non-blank character
    ਪਹਿਲਾ = skipwhite(ਸਤਰ);

    // find the character after the last non-blank character
    ਲਈ (ਆਖਰੀ = ਪਹਿਲਾ + STRLEN(ਪਹਿਲਾ);
				ਆਖਰੀ > ਪਹਿਲਾ && VIM_ISWHITE(ਆਖਰੀ[-1]); --ਆਖਰੀ)
	;
    save = *ਆਖਰੀ;
    *ਆਖਰੀ = NUL;
    len = linetabsize_str(ਸਤਰ);	// get line length on screen
    ਜੇ (has_tab != ਨਿਰਾ)		// check for embedded TAB
	*has_tab = (vim_strchr(ਪਹਿਲਾ, TAB) != ਨਿਰਾ);
    *ਆਖਰੀ = save;

    ਵਾਪਸ len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
ਸਥਿਰ char_u	*sortbuf1;
ਸਥਿਰ char_u	*sortbuf2;

ਸਥਿਰ ਪੂਰਨ	ਛਾਂਟੋ_lc;	// sort using locale
ਸਥਿਰ ਪੂਰਨ	ਛਾਂਟੋ_ic;	// ignore case
ਸਥਿਰ ਪੂਰਨ	ਛਾਂਟੋ_nr;	// sort on number
ਸਥਿਰ ਪੂਰਨ	ਛਾਂਟੋ_rx;	// sort on regex instead of skipping it
ਸਥਿਰ ਪੂਰਨ	ਛਾਂਟੋ_flt;	// sort on floating number

ਸਥਿਰ ਪੂਰਨ	ਛਾਂਟੋ_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	lnum;			// line number
    ਸੰਘ {
	ਬਣਤਰ
	{
	    varnumber_T	ਸ਼ੁਰੂ_ਖਾਨਾ_nr;	// starting column number
	    varnumber_T	ਸਿਰਾ_ਖਾਨਾ_nr;	// ending column number
	} ਸਤਰ;
	ਬਣਤਰ
	{
	    varnumber_T	ਮੁੱਲ;		// value if sorting by integer
	    ਪੂਰਨ is_ਸੰਖਿਆ;		// TRUE when line contains a number
	} ਅੰਕ;
	float_T ਮੁੱਲ_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    ਸਥਿਰ ਪੂਰਨ
string_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਜੇ (ਛਾਂਟੋ_lc)
	ਵਾਪਸ strcoll((ਅੱਖਰ *)s1, (ਅੱਖਰ *)s2);
    ਵਾਪਸ ਛਾਂਟੋ_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    ਸਥਿਰ ਪੂਰਨ
ਛਾਂਟੋ_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    ਪੂਰਨ		ਨਤੀਜਾ = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    ਜੇ (ਛਾਂਟੋ_abort)
	ਵਾਪਸ 0;
    fast_breakcheck();
    ਜੇ (got_int)
	ਛਾਂਟੋ_abort = TRUE;

    ਜੇ (ਛਾਂਟੋ_nr)
    {
	ਜੇ (l1.st_u.ਅੰਕ.is_ਸੰਖਿਆ != l2.st_u.ਅੰਕ.is_ਸੰਖਿਆ)
	    ਨਤੀਜਾ = l1.st_u.ਅੰਕ.is_ਸੰਖਿਆ > l2.st_u.ਅੰਕ.is_ਸੰਖਿਆ ? 1 : -1;
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = l1.st_u.ਅੰਕ.ਮੁੱਲ == l2.st_u.ਅੰਕ.ਮੁੱਲ ? 0
			     : l1.st_u.ਅੰਕ.ਮੁੱਲ > l2.st_u.ਅੰਕ.ਮੁੱਲ ? 1 : -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਛਾਂਟੋ_flt)
	ਨਤੀਜਾ = l1.st_u.ਮੁੱਲ_flt == l2.st_u.ਮੁੱਲ_flt ? 0
			     : l1.st_u.ਮੁੱਲ_flt > l2.st_u.ਮੁੱਲ_flt ? 1 : -1;
    ਨਹੀਂਤਾਂ
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr,
		     l1.st_u.ਸਤਰ.ਸਿਰਾ_ਖਾਨਾ_nr - l1.st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr + 1);
	sortbuf1[l1.st_u.ਸਤਰ.ਸਿਰਾ_ਖਾਨਾ_nr - l1.st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr,
		     l2.st_u.ਸਤਰ.ਸਿਰਾ_ਖਾਨਾ_nr - l2.st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr + 1);
	sortbuf2[l2.st_u.ਸਤਰ.ਸਿਰਾ_ਖਾਨਾ_nr - l2.st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr] = 0;

	ਨਤੀਜਾ = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    ਜੇ (ਨਤੀਜਾ == 0)
	ਵਾਪਸ (ਪੂਰਨ)(l1.lnum - l2.lnum);
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * ":sort".
 */
    ਖਾਲੀ
ex_ਛਾਂਟੋ(exarg_T *eap)
{
    regmatch_T	regmatch;
    ਪੂਰਨ		len;
    linenr_T	lnum;
    ਲੰਮਾ	maxlen = 0;
    sorti_T	*nrs;
    size_t	ਗਿਣਤੀ_2 = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    ਪੂਰਨ		unique = FALSE;
    ਲੰਮਾ	deleted;
    colnr_T	ਸ਼ੁਰੂ_ਖਾਨਾ;
    colnr_T	ਸਿਰਾ_ਖਾਨਾ;
    ਪੂਰਨ		ਛਾਂਟੋ_what = 0;
    ਪੂਰਨ		format_ਮਿਲਿਆ = 0;
    ਪੂਰਨ		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    ਜੇ (ਗਿਣਤੀ_2 <= 1)
	ਵਾਪਸ;

    ਜੇ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ਵਾਪਸ;
    sortbuf1 = ਨਿਰਾ;
    sortbuf2 = ਨਿਰਾ;
    regmatch.regprog = ਨਿਰਾ;
    nrs = ALLOC_MULT(sorti_T, ਗਿਣਤੀ_2);
    ਜੇ (nrs == ਨਿਰਾ)
	ਜਾਓ sortend;

    ਛਾਂਟੋ_abort = ਛਾਂਟੋ_ic = ਛਾਂਟੋ_lc = ਛਾਂਟੋ_rx = ਛਾਂਟੋ_nr = 0;
    ਛਾਂਟੋ_flt = 0;

    ਲਈ (p = eap->arg; *p != NUL; ++p)
    {
	ਜੇ (VIM_ISWHITE(*p))
	    ;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'i')
	    ਛਾਂਟੋ_ic = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'l')
	    ਛਾਂਟੋ_lc = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'r')
	    ਛਾਂਟੋ_rx = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'n')
	{
	    ਛਾਂਟੋ_nr = 1;
	    ++format_ਮਿਲਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'f')
	{
	    ਛਾਂਟੋ_flt = 1;
	    ++format_ਮਿਲਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'b')
	{
	    ਛਾਂਟੋ_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_ਮਿਲਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'o')
	{
	    ਛਾਂਟੋ_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_ਮਿਲਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'x')
	{
	    ਛਾਂਟੋ_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_ਮਿਲਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'u')
	    unique = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"')	// comment start
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (eap->nextcmd == ਨਿਰਾ && check_nextcmd(p) != ਨਿਰਾ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!ASCII_ISALPHA(*p) && regmatch.regprog == ਨਿਰਾ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    ਜੇ (s == p + 1)
	    {
		ਜੇ (ਆਖਰੀ_ਖੋਜ_pat() == ਨਿਰਾ)
		{
		    emsg(_(e_no_ਪਿਛਲਾ_regular_ਵਿਅੰਜਕ));
		    ਜਾਓ sortend;
		}
		regmatch.regprog = vim_regcomp(ਆਖਰੀ_ਖੋਜ_pat(), RE_MAGIC);
	    }
	    ਨਹੀਂਤਾਂ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ਜੇ (regmatch.regprog == ਨਿਰਾ)
		ਜਾਓ sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), p);
	    ਜਾਓ sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    ਜੇ (format_ਮਿਲਿਆ > 1)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਜਾਓ sortend;
    }

    // From here on "sort_nr" is used as a flag for any integer number
    // sorting.
    ਛਾਂਟੋ_nr += ਛਾਂਟੋ_what;

    /*
     * Make an array with all line numbers.  This avoids having to copy all
     * the lines into allocated memory.
     * When sorting on strings "start_col_nr" is the offset in the line, for
     * numbers sorting it's the number to sort on.  This means the pattern
     * matching and number conversion only has to be done once per line.
     * Also get the longest line length for allocating "sortbuf".
     */
    ਲਈ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	ਜੇ (maxlen < len)
	    maxlen = len;

	ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
	ਸਿਰਾ_ਖਾਨਾ = len;
	ਜੇ (regmatch.regprog != ਨਿਰਾ && vim_regexec(&regmatch, s, 0))
	{
	    ਜੇ (ਛਾਂਟੋ_rx)
	    {
		ਸ਼ੁਰੂ_ਖਾਨਾ = (colnr_T)(regmatch.startp[0] - s);
		ਸਿਰਾ_ਖਾਨਾ = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ਨਹੀਂਤਾਂ
		ਸ਼ੁਰੂ_ਖਾਨਾ = (colnr_T)(regmatch.endp[0] - s);
	}
	ਨਹੀਂਤਾਂ
	    ਜੇ (regmatch.regprog != ਨਿਰਾ)
		ਸਿਰਾ_ਖਾਨਾ = 0;

	ਜੇ (ਛਾਂਟੋ_nr || ਛਾਂਟੋ_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + ਸਿਰਾ_ਖਾਨਾ;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + ਸ਼ੁਰੂ_ਖਾਨਾ;
	    ਜੇ (ਛਾਂਟੋ_nr)
	    {
		ਜੇ (ਛਾਂਟੋ_what & STR2NR_HEX)
		    s = skiptohex(p);
		ਨਹੀਂਤਾਂ ਜੇ (ਛਾਂਟੋ_what & STR2NR_BIN)
		    s = skiptobin(p);
		ਨਹੀਂਤਾਂ
		    s = skiptodigit(p);
		ਜੇ (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		ਜੇ (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.ਅੰਕ.is_ਸੰਖਿਆ = FALSE;
		    nrs[lnum - eap->line1].st_u.ਅੰਕ.ਮੁੱਲ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    nrs[lnum - eap->line1].st_u.ਅੰਕ.is_ਸੰਖਿਆ = TRUE;
		    vim_str2nr(s, ਨਿਰਾ, ਨਿਰਾ, ਛਾਂਟੋ_what,
			&nrs[lnum - eap->line1].st_u.ਅੰਕ.ਮੁੱਲ,
			ਨਿਰਾ, 0, FALSE, ਨਿਰਾ);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		s = skipwhite(p);
		ਜੇ (*s == '+')
		    s = skipwhite(s + 1);

		ਜੇ (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.ਮੁੱਲ_flt = -DBL_MAX;
		ਨਹੀਂਤਾਂ
		    nrs[lnum - eap->line1].st_u.ਮੁੱਲ_flt =
						      strtod((ਅੱਖਰ *)s, ਨਿਰਾ);
	    }
	    *s2 = c;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.ਸਤਰ.ਸ਼ੁਰੂ_ਖਾਨਾ_nr = ਸ਼ੁਰੂ_ਖਾਨਾ;
	    nrs[lnum - eap->line1].st_u.ਸਤਰ.ਸਿਰਾ_ਖਾਨਾ_nr = ਸਿਰਾ_ਖਾਨਾ;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	ਜੇ (regmatch.regprog != ਨਿਰਾ)
	    fast_breakcheck();
	ਜੇ (got_int)
	    ਜਾਓ sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ਜੇ (sortbuf1 == ਨਿਰਾ)
	ਜਾਓ sortend;
    sortbuf2 = alloc(maxlen + 1);
    ਜੇ (sortbuf2 == ਨਿਰਾ)
	ਜਾਓ sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((ਖਾਲੀ *)nrs, ਗਿਣਤੀ_2, ਆਕਾਰ(sorti_T), ਛਾਂਟੋ_compare);

    ਜੇ (ਛਾਂਟੋ_abort)
	ਜਾਓ sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? ਗਿਣਤੀ_2 - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	ਜੇ (get_lnum + ((linenr_T)ਗਿਣਤੀ_2 - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	ਜੇ (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    ਜੇ (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		ਰੋਕੋ;
	}
	fast_breakcheck();
	ਜੇ (got_int)
	    ਜਾਓ sortend;
    }

    // delete the original lines if appending worked
    ਜੇ (i == ਗਿਣਤੀ_2)
	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	    ml_delete(eap->line1);
    ਨਹੀਂਤਾਂ
	ਗਿਣਤੀ_2 = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (ਲੰਮਾ)(ਗਿਣਤੀ_2 - (lnum - eap->line2));
    ਜੇ (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (ਲੰਮਾ)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    ਨਹੀਂਤਾਂ ਜੇ (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    ਜੇ (change_occurred || deleted != 0)
	ਬਦਲਿਆ_ਸਤਰਾਂ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    ਜੇ (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    ਖਾਲੀ
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    ਪੂਰਨ		len;
    linenr_T	lnum;
    ਲੰਮਾ	maxlen = 0;
    linenr_T	ਗਿਣਤੀ_2 = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    ਪੂਰਨ		keep_only_unique = FALSE;
    ਪੂਰਨ		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    ਲੰਮਾ	deleted = 0;
    colnr_T	ਸ਼ੁਰੂ_ਖਾਨਾ;
    colnr_T	ਸਿਰਾ_ਖਾਨਾ;
    ਪੂਰਨ		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    ਜੇ (ਗਿਣਤੀ_2 <= 1)
	ਵਾਪਸ;

    ਜੇ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ਵਾਪਸ;
    sortbuf1 = ਨਿਰਾ;
    regmatch.regprog = ਨਿਰਾ;

    ਛਾਂਟੋ_abort = ਛਾਂਟੋ_ic = ਛਾਂਟੋ_lc = ਛਾਂਟੋ_rx = ਛਾਂਟੋ_nr = 0;
    ਛਾਂਟੋ_flt = 0;

    ਲਈ (p = eap->arg; *p != NUL; ++p)
    {
	ਜੇ (VIM_ISWHITE(*p))
	    ;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'i')
	    ਛਾਂਟੋ_ic = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'l')
	    ਛਾਂਟੋ_lc = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'r')
	    ਛਾਂਟੋ_rx = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    ਜੇ (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"')	// comment start
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (eap->nextcmd == ਨਿਰਾ && check_nextcmd(p) != ਨਿਰਾ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!ASCII_ISALPHA(*p) && regmatch.regprog == ਨਿਰਾ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    ਜੇ (s == p + 1)
	    {
		ਜੇ (ਆਖਰੀ_ਖੋਜ_pat() == ਨਿਰਾ)
		{
		    emsg(_(e_no_ਪਿਛਲਾ_regular_ਵਿਅੰਜਕ));
		    ਜਾਓ uniqend;
		}
		regmatch.regprog = vim_regcomp(ਆਖਰੀ_ਖੋਜ_pat(), RE_MAGIC);
	    }
	    ਨਹੀਂਤਾਂ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ਜੇ (regmatch.regprog == ਨਿਰਾ)
		ਜਾਓ uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), p);
	    ਜਾਓ uniqend;
	}
    }

    // Find the length of the longest line.
    ਲਈ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	ਜੇ (maxlen < len)
	    maxlen = len;

	ਜੇ (got_int)
	    ਜਾਓ uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ਜੇ (sortbuf1 == ਨਿਰਾ)
	ਜਾਓ uniqend;

    // Delete lines according to options.
    ਪੂਰਨ ਮੇਲ_continue = FALSE;
    ਪੂਰਨ ਅਗਲਾ_is_unmatch = FALSE;
    ਪੂਰਨ is_ਮੇਲ;
    linenr_T ਪੂਰਾ_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    ਲਈ (linenr_T i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
	ਸਿਰਾ_ਖਾਨਾ = len;
	ਜੇ (regmatch.regprog != ਨਿਰਾ && vim_regexec(&regmatch, s, 0))
	{
	    ਜੇ (ਛਾਂਟੋ_rx)
	    {
		ਸ਼ੁਰੂ_ਖਾਨਾ = (colnr_T)(regmatch.startp[0] - s);
		ਸਿਰਾ_ਖਾਨਾ = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ਨਹੀਂਤਾਂ
		ਸ਼ੁਰੂ_ਖਾਨਾ = (colnr_T)(regmatch.endp[0] - s);
	}
	ਨਹੀਂਤਾਂ
	    ਜੇ (regmatch.regprog != ਨਿਰਾ)
		ਸਿਰਾ_ਖਾਨਾ = 0;
	ਜੇ (ਸਿਰਾ_ਖਾਨਾ > 0)
	{
	    save_c = s[ਸਿਰਾ_ਖਾਨਾ];
	    s[ਸਿਰਾ_ਖਾਨਾ] = NUL;
	}

	is_ਮੇਲ = i > 0 ? !string_compare(&s[ਸ਼ੁਰੂ_ਖਾਨਾ], sortbuf1) : FALSE;
	delete_lnum = 0;
	ਜੇ (ਅਗਲਾ_is_unmatch)
	{
	    is_ਮੇਲ = FALSE;
	    ਅਗਲਾ_is_unmatch = FALSE;
	}

	ਜੇ (!keep_only_unique && !keep_only_not_unique)
	{
	    ਜੇ (is_ਮੇਲ)
		delete_lnum = get_lnum;
	    ਨਹੀਂਤਾਂ
		STRCPY(sortbuf1, &s[ਸ਼ੁਰੂ_ਖਾਨਾ]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (keep_only_not_unique)
	{
	    ਜੇ (is_ਮੇਲ)
	    {
		ਪੂਰਾ_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		ਮੇਲ_continue = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (i > 0 && !ਮੇਲ_continue && get_lnum - 1 > ਪੂਰਾ_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    ਅਗਲਾ_is_unmatch = TRUE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (i >= ਗਿਣਤੀ_2 - 1)
		    delete_lnum = get_lnum;
		ਮੇਲ_continue = FALSE;
		STRCPY(sortbuf1, &s[ਸ਼ੁਰੂ_ਖਾਨਾ]);
	    }
	}
	ਨਹੀਂਤਾਂ // keep_only_unique
	{
	    ਜੇ (is_ਮੇਲ)
	    {
		ਜੇ (!ਮੇਲ_continue)
		    delete_lnum = get_lnum - 1;
		ਨਹੀਂਤਾਂ
		    delete_lnum = get_lnum;
		ਮੇਲ_continue = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (i == 0 && ਮੇਲ_continue)
		    delete_lnum = get_lnum;
		ਮੇਲ_continue = FALSE;
		STRCPY(sortbuf1, &s[ਸ਼ੁਰੂ_ਖਾਨਾ]);
	    }
	}

	ਜੇ (ਸਿਰਾ_ਖਾਨਾ > 0)
	    s[ਸਿਰਾ_ਖਾਨਾ] = save_c;

	ਜੇ (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    ਗਿਣਤੀ_2--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	ਜੇ (got_int)
	    ਜਾਓ uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (ਲੰਮਾ)MAXLNUM, -deleted);
    msgmore(-deleted);

    ਜੇ (change_occurred)
	ਬਦਲਿਆ_ਸਤਰਾਂ(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    ਜੇ (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	ਅੰਕ_ਸਤਰਾਂ;  // Num lines moved
    linenr_T	ਆਖਰੀ_ਸਤਰ;  // Last line in file after adding new text
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#ਜੇ_ਅੰਤ

    ਜੇ (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_ਦਾ_ਸਤਰਾਂ_into_itself));
	ਵਾਪਸ FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    ਜੇ (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	ਜੇ (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	ਨਹੀਂਤਾਂ
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	ਵਾਪਸ OK;
    }

    ਅੰਕ_ਸਤਰਾਂ = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    ਜੇ (u_save(dest, dest + 1) == FAIL)
	ਵਾਪਸ FAIL;
    ਲਈ (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	ਜੇ (str != ਨਿਰਾ)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    ਜੇ (dest < line1)
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
    ਆਖਰੀ_ਸਤਰ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    mark_adjust_nofold(line1, line2, ਆਖਰੀ_ਸਤਰ - line2, 0L);
    ਜੇ (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -ਅੰਕ_ਸਤਰਾਂ, 0L);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ਜੇ (win->w_ਬਫਰ == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#ਜੇ_ਅੰਤ
	ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_ਸ਼ੁਰੂ.lnum = dest - ਅੰਕ_ਸਤਰਾਂ + 1;
	    curbuf->b_op_ਸਿਰਾ.lnum = dest;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	mark_adjust_nofold(dest + 1, line1 - 1, ਅੰਕ_ਸਤਰਾਂ, 0L);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ਜੇ (win->w_ਬਫਰ == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#ਜੇ_ਅੰਤ
	ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_ਸ਼ੁਰੂ.lnum = dest + 1;
	    curbuf->b_op_ਸਿਰਾ.lnum = dest + ਅੰਕ_ਸਤਰਾਂ;
	}
    }
    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
    mark_adjust_nofold(ਆਖਰੀ_ਸਤਰ - ਅੰਕ_ਸਤਰਾਂ + 1, ਆਖਰੀ_ਸਤਰ,
					     -(ਆਖਰੀ_ਸਤਰ - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    ਜੇ (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	ਵਾਪਸ FAIL;

    ਲਈ (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_ਸੁਨੇਹਾ);

    ਜੇ (!global_busy && ਅੰਕ_ਸਤਰਾਂ > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", ਅੰਕ_ਸਤਰਾਂ),
			(ਲੰਮਾ)ਅੰਕ_ਸਤਰਾਂ);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    ਜੇ (dest >= line1)
	curwin->w_cursor.lnum = dest;
    ਨਹੀਂਤਾਂ
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    ਜੇ (line1 < dest)
    {
	dest += ਅੰਕ_ਸਤਰਾਂ + 1;
	ਆਖਰੀ_ਸਤਰ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਜੇ (dest > ਆਖਰੀ_ਸਤਰ + 1)
	    dest = ਆਖਰੀ_ਸਤਰ + 1;
	ਬਦਲਿਆ_ਸਤਰਾਂ(line1, 0, dest, 0L);
    }
    ਨਹੀਂਤਾਂ
	ਬਦਲਿਆ_ਸਤਰਾਂ(dest + 1, 0, line1 + ਅੰਕ_ਸਤਰਾਂ, 0L);

    ਵਾਪਸ OK;
}

/*
 * ":copy"
 */
    ਖਾਲੀ
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	ਗਿਣਤੀ_2;
    char_u	*p;

    ਗਿਣਤੀ_2 = line2 - line1 + 1;
    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = n + 1;
	curbuf->b_op_ਸਿਰਾ.lnum = n + ਗਿਣਤੀ_2;
	curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
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
    ਜੇ (u_save(n, n + 1) == FAIL)
	ਵਾਪਸ;

    curwin->w_cursor.lnum = n;
    ਜਦੋਂ (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	ਜੇ (p != ਨਿਰਾ)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	ਜੇ (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	ਜੇ (curwin->w_cursor.lnum < line1)
	    ++line1;
	ਜੇ (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_ਸਤਰਾਂ_mark(n, ਗਿਣਤੀ_2);
    ਜੇ (VIsual_ਸਰਗਰਮ)
	check_pos(curbuf, &VIsual);

    msgmore((ਲੰਮਾ)ਗਿਣਤੀ_2);
}

ਸਥਿਰ char_u	*prevcmd = ਨਿਰਾ;	// the previous command

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_prev_shellcmd(ਖਾਲੀ)
{
    vim_free(prevcmd);
}
#ਜੇ_ਅੰਤ

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
prevcmd_is_ਸੈੱਟ(ਖਾਲੀ)
{
    ਜੇ (prevcmd == ਨਿਰਾ)
    {
	emsg(_(e_no_ਪਿਛਲਾ_command));
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    ਖਾਲੀ
do_bang(
    ਪੂਰਨ		addr_ਗਿਣਤੀ,
    exarg_T	*eap,
    ਪੂਰਨ		forceit,
    ਪੂਰਨ		do_ਵਿੱਚ,
    ਪੂਰਨ		do_ਬਾਹਰਲਾ)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = ਨਿਰਾ;		// the new command
    ਪੂਰਨ			free_newcmd = FALSE;    // need to free() newcmd
    ਪੂਰਨ			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    ਪੂਰਨ			len;
    ਪੂਰਨ			scroll_save = ਸੁਨੇਹਾ_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (addr_ਗਿਣਤੀ == 0)		// :!
    {
	ਸੁਨੇਹਾ_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	ਸੁਨੇਹਾ_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    ਕਰੋ
    {
	len = (ਪੂਰਨ)STRLEN(trailarg) + 1;
	ਜੇ (newcmd != ਨਿਰਾ)
	    len += (ਪੂਰਨ)STRLEN(newcmd);
	ਜੇ (ins_prevcmd)
	{
	    ਜੇ (!prevcmd_is_ਸੈੱਟ())
	    {
		vim_free(newcmd);
		ਵਾਪਸ;
	    }
	    len += (ਪੂਰਨ)STRLEN(prevcmd);
	}
	ਜੇ ((t = alloc(len)) == ਨਿਰਾ)
	{
	    vim_free(newcmd);
	    ਵਾਪਸ;
	}
	*t = NUL;
	ਜੇ (newcmd != ਨਿਰਾ)
	    STRCAT(t, newcmd);
	ਜੇ (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = ਨਿਰਾ;
	ਜਦੋਂ (*p)
	{
	    ਜੇ (*p == '!')
	    {
		ਜੇ (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		ਨਹੀਂਤਾਂ
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ++p;
	}
    } ਜਦੋਂ (trailarg != ਨਿਰਾ);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    ਜੇ (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    ਨਹੀਂਤਾਂ
	free_newcmd = TRUE;

    ਜੇ (bangredo)	    // put cmd in redo buffer for ! command
    {
	ਜੇ (!prevcmd_is_ਸੈੱਟ())
	    ਜਾਓ theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	ਜੇ (cmd != ਨਿਰਾ)
	{
	    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(cmd, -1);
	    vim_free(cmd);
	}
	ਨਹੀਂਤਾਂ
	    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(prevcmd, -1);
	Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    ਜੇ (*p_shq != NUL)
    {
	ਜੇ (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	ਜੇ (newcmd == ਨਿਰਾ)
	    ਵਾਪਸ;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    ਜੇ (addr_ਗਿਣਤੀ == 0)		// :!
    {
	// echo the command
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_putchar(':');
	ਸੁਨੇਹਾ_putchar('!');
	ਸੁਨੇਹਾ_outtrans(newcmd);
	ਸੁਨੇਹਾ_clr_eos();
	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);

	do_shell(newcmd, 0);
    }
    ਨਹੀਂਤਾਂ				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_ਵਿੱਚ, do_ਬਾਹਰਲਾ);
	apply_autocmds(EVENT_SHELLFILTERPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    }

theend:
    ਜੇ (free_newcmd)
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
    ਸਥਿਰ ਖਾਲੀ
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    ਪੂਰਨ		do_ਵਿੱਚ,
    ਪੂਰਨ		do_ਬਾਹਰਲਾ)
{
    char_u	*itmp = ਨਿਰਾ;
    char_u	*otmp = ਨਿਰਾ;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    ਪੂਰਨ		shell_flags = 0;
    pos_T	orig_ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ;
    pos_T	orig_ਸਿਰਾ = curbuf->b_op_ਸਿਰਾ;
    ਪੂਰਨ		save_cmod_flags = cmdmod.cmod_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FILTERPIPE
    ਪੂਰਨ		stmp = p_stmp;
#ਜੇ_ਅੰਤ

    ਜੇ (*cmd == NUL)	    // no filter command
	ਵਾਪਸ;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.ਖਾਨਾ = 0;
    ਬਦਲਿਆ_ਸਤਰ_abv_curs();
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

    ਜੇ (do_ਬਾਹਰਲਾ)
	shell_flags |= SHELL_DOOUT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FILTERPIPE
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    ਜੇ (!do_ਵਿੱਚ && do_ਬਾਹਰਲਾ && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (do_ਵਿੱਚ && !do_ਬਾਹਰਲਾ && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = line1;
	curbuf->b_op_ਸਿਰਾ.lnum = line2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (do_ਵਿੱਚ && do_ਬਾਹਰਲਾ && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = line1;
	curbuf->b_op_ਸਿਰਾ.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ ((do_ਵਿੱਚ && (itmp = vim_tempname('i', FALSE)) == ਨਿਰਾ)
		|| (do_ਬਾਹਰਲਾ && (otmp = vim_tempname('o', FALSE)) == ਨਿਰਾ))
	{
	    emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
	    ਜਾਓ filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    ਜੇ (itmp != ਨਿਰਾ && buf_write(curbuf, itmp, ਨਿਰਾ, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	ਸੁਨੇਹਾ_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#ਜੇ defined(FEAT_EVAL)
	ਜੇ (!aborting())
#ਜੇ_ਅੰਤ
	    // will call wait_return()
	    (ਖਾਲੀ)semsg(_(e_cant_create_file_str), itmp);
	ਜਾਓ filterend;
    }
    ਜੇ (curbuf != old_curbuf)
	ਜਾਓ filterend;

    ਜੇ (!do_ਬਾਹਰਲਾ)
	ਸੁਨੇਹਾ_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    ਜੇ (cmd_buf == ਨਿਰਾ)
	ਜਾਓ filterend;

    windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    ਜੇ (!do_ਬਾਹਰਲਾ || STRCMP(p_srr, ">") == 0 || !do_ਵਿੱਚ)
	redraw_later_clear();

    ਜੇ (do_ਬਾਹਰਲਾ)
    {
	ਜੇ (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    ਜਾਓ error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    ਜੇ (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    ਜੇ (do_ਬਾਹਰਲਾ)
    {
	ਜੇ (otmp != ਨਿਰਾ)
	{
	    ਜੇ (readfile(otmp, ਨਿਰਾ, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#ਜੇ defined(FEAT_EVAL)
		ਜੇ (!aborting())
#ਜੇ_ਅੰਤ
		{
		    ਸੁਨੇਹਾ_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		ਜਾਓ error;
	    }
	    ਜੇ (curbuf != old_curbuf)
		ਜਾਓ filterend;
	}

	read_linecount = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - read_linecount;

	ਜੇ (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_ਸ਼ੁਰੂ.lnum = line2 + 1;
	    curbuf->b_op_ਸਿਰਾ.lnum = curwin->w_cursor.lnum;
	    appended_ਸਤਰਾਂ_mark(line2, read_linecount);
	}

	ਜੇ (do_ਵਿੱਚ)
	{
	    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == ਨਿਰਾ)
	    {
		ਜੇ (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		ਨਹੀਂਤਾਂ ਜੇ (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		ਨਹੀਂਤਾਂ
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
	    del_ਸਤਰਾਂ(linecount, TRUE);
	    ਜੇ (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_ਸ਼ੁਰੂ.lnum = curbuf->b_op_ਸਿਰਾ.lnum =
					MIN(line1, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
		curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		curbuf->b_op_ਸ਼ੁਰੂ.lnum -= linecount;	// adjust '[
		curbuf->b_op_ਸਿਰਾ.lnum -= linecount;	// adjust ']
	    }
	    write_lnum_adjust(-linecount);		// adjust last line
							// for next write
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    foldUpdate(curwin, curbuf->b_op_ਸ਼ੁਰੂ.lnum, curbuf->b_op_ਸਿਰਾ.lnum);
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_ਸਿਰਾ.lnum - curbuf->b_op_ਸ਼ੁਰੂ.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_ਸਿਰਾ.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	ਜੇ (linecount > p_report)
	{
	    ਜੇ (do_ਵਿੱਚ)
	    {
		vim_snprintf(ਸੁਨੇਹਾ_buf, ਆਕਾਰ(ਸੁਨੇਹਾ_buf),
				    _("%ld lines filtered"), (ਲੰਮਾ)linecount);
		ਜੇ (ਸੁਨੇਹਾ(ਸੁਨੇਹਾ_buf) && !ਸੁਨੇਹਾ_scroll)
		    // save message to display it after redraw
		    ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)ਸੁਨੇਹਾ_buf, 0);
	    }
	    ਨਹੀਂਤਾਂ
		msgmore((ਲੰਮਾ)linecount);
	}
    }
    ਨਹੀਂਤਾਂ
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    ਜੇ (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_ਬਫਰ));
    }
    ਨਹੀਂਤਾਂ ਜੇ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = orig_ਸਿਰਾ;
    }

    ਜੇ (itmp != ਨਿਰਾ)
	mch_remove(itmp);
    ਜੇ (otmp != ਨਿਰਾ)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    ਖਾਲੀ
do_shell(
    char_u	*cmd,
    ਪੂਰਨ		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ਪੂਰਨ		save_nwr;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਪੂਰਨ		winstart = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		keep_termcap = !termcap_ਸਰਗਰਮ;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ਜੇ (check_restricted() || check_secure())
    {
	ਸੁਨੇਹਾ_ਸਿਰਾ();
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    ਜੇ (cmd != ਨਿਰਾ)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# ਜੇ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    ਜੇ (gui.ਵਿੱਚ_use && vim_strchr(p_go, GO_TERMINAL) != ਨਿਰਾ)
	keep_termcap = TRUE;
# endif
#ਜੇ_ਅੰਤ

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    ਸੁਨੇਹਾ_putchar('\r');			// put cursor at start of line
    ਜੇ (!autocmd_busy)
    {
	ਜੇ (!keep_termcap)
	    stoptermcap();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (!winstart)
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    ਜੇ (p_warn && !autocmd_busy && ਸੁਨੇਹਾ_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    ਜੇ (buf_Is_ਬਦਲਿਆ_Not_ਪਦ(buf))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
		ਜੇ (!keep_termcap)
		    starttermcap();	// don't want a message box here
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_puts(_("[No write since last change]\n"));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
		ਜੇ (!keep_termcap)
		    stoptermcap();
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    ਜੇ (!swapping_screen())
	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
    cursor_on();
    (ਖਾਲੀ)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    ਜੇ (!swapping_screen())
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    }

    ਜੇ (autocmd_busy)
    {
	ਜੇ (ਸੁਨੇਹਾ_silent == 0)
	    redraw_later_clear();
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	{
	    ਜੇ (cmd == ਨਿਰਾ
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		ਜੇ (ਸੁਨੇਹਾ_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		ਜੇ (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		ਪੂਰਨ redraw = ਸੁਨੇਹਾ_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		ਜੇ (gui.ਵਿੱਚ_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(ਪਦ_console ? -1 : redraw); // see below
# ਨਹੀਂਤਾਂ
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#ਜੇ_ਅੰਤ // FEAT_GUI_MSWIN

	ਜੇ (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
	ਜੇ (skip_redraw)		// ':' hit in wait_return()
	{
	    ਜੇ (ਸੁਨੇਹਾ_silent == 0)
		redraw_later_clear();
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_console)
	{
	    ਬਾਹਰਲਾ_STR("\033[0 q");	// get window size
	    ਜੇ (got_int && ਸੁਨੇਹਾ_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    ਨਹੀਂਤਾਂ
		must_redraw = 0;	// no extra redraw needed
	}
#ਜੇ_ਅੰਤ
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
}

#ਜੇ !defined(UNIX)
    ਸਥਿਰ char_u *
ਲੱਭੋ_pipe(char_u *cmd)
{
    char_u  *p;
    ਪੂਰਨ	    inquote = FALSE;

    ਲਈ (p = cmd; *p != NUL; ++p)
    {
	ਜੇ (!inquote && *p == '|')
	    ਵਾਪਸ p;
	ਜੇ (*p == '"')
	    inquote = !inquote;
	ਨਹੀਂਤਾਂ ਜੇ (rem_backslash(p))
	    ++p;
    }
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ		is_powershell = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਪੂਰਨ		is_fish_shell;
#ਜੇ_ਅੰਤ

    char_u *shell_name = get_isolated_shell_name();
    ਜੇ (shell_name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    ਜੇ (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	ਜੇ (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	ਨਹੀਂਤਾਂ
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    ਜੇ (itmp != ਨਿਰਾ)
    {
	ਜੇ (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	ਨਹੀਂਤਾਂ
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    ਜੇ (otmp != ਨਿਰਾ)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (is_powershell)
    {
	ਜੇ (itmp != ਨਿਰਾ)
	    vim_snprintf((ਅੱਖਰ *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	ਨਹੀਂਤਾਂ
	    vim_snprintf((ਅੱਖਰ *)buf, len, "& { %s }", cmd);
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	ਜੇ (itmp != ਨਿਰਾ || otmp != ਨਿਰਾ)
	{
	    ਜੇ (is_fish_shell)
		vim_snprintf((ਅੱਖਰ *)buf, len, "begin; %s; end", (ਅੱਖਰ *)cmd);
	    ਨਹੀਂਤਾਂ
		vim_snprintf((ਅੱਖਰ *)buf, len, "(%s)", (ਅੱਖਰ *)cmd);
	}
	ਨਹੀਂਤਾਂ
	    STRCPY(buf, cmd);
	ਜੇ (itmp != ਨਿਰਾ)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#ਨਹੀਂਤਾਂ
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	ਜੇ (*p_sxe != NUL && *p_sxq == '(')
	{
	    ਜੇ (itmp != ਨਿਰਾ || otmp != ਨਿਰਾ)
		vim_snprintf((ਅੱਖਰ *)buf, len, "(%s)", (ਅੱਖਰ *)cmd);
	    ਨਹੀਂਤਾਂ
		STRCPY(buf, cmd);
	    ਜੇ (itmp != ਨਿਰਾ)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    STRCPY(buf, cmd);
	    ਜੇ (itmp != ਨਿਰਾ)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		ਜੇ (*p_shq == NUL)
		{
		    p = ਲੱਭੋ_pipe(buf);
		    ਜੇ (p != ਨਿਰਾ)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		ਜੇ (*p_shq == NUL)
		{
		    p = ਲੱਭੋ_pipe(cmd);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			// insert a space before the '|' for DOS
			STRCAT(buf, " ");
			STRCAT(buf, p);
		    }
		}
	    }
	}
#ਜੇ_ਅੰਤ
    }
    ਜੇ (otmp != ਨਿਰਾ)
	append_redir(buf, (ਪੂਰਨ)len, p_srr, otmp);

    ਵਾਪਸ buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    ਖਾਲੀ
append_redir(
    char_u	*buf,
    ਪੂਰਨ		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*ਸਿਰਾ;

    ਸਿਰਾ = buf + STRLEN(buf);
    // find "%s"
    ਲਈ (p = opt; (p = vim_strchr(p, '%')) != ਨਿਰਾ; ++p)
    {
	ਜੇ (p[1] == 's') // found %s
	    ਰੋਕੋ;
	ਜੇ (p[1] == '%') // skip %%
	    ++p;
    }
    ਜੇ (p != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	*ਸਿਰਾ++ = ' '; // not really needed? Not with sh, ksh or bash
#ਜੇ_ਅੰਤ
	vim_snprintf((ਅੱਖਰ *)ਸਿਰਾ, (size_t)(buflen - (ਸਿਰਾ - buf)),
						  (ਅੱਖਰ *)opt, (ਅੱਖਰ *)fname);
    }
    ਨਹੀਂਤਾਂ
	vim_snprintf((ਅੱਖਰ *)ਸਿਰਾ, (size_t)(buflen - (ਸਿਰਾ - buf)), " %s %s",
		(ਅੱਖਰ *)opt, (ਅੱਖਰ *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    ਖਾਲੀ
do_fixdel(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    char_u  *p;

    p = ਲੱਭੋ_termcode((char_u *)"kb");
    ਜੋੜੋ_termcode((char_u *)"kD", p != ਨਿਰਾ
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    ਖਾਲੀ
print_ਸਤਰ_no_ਅਗੇਤਰ(
    linenr_T	lnum,
    ਪੂਰਨ		use_ਸੰਖਿਆ,
    ਪੂਰਨ		list)
{
    ਅੱਖਰ	numbuf[30];

    ਜੇ (curwin->w_p_nu || use_ਸੰਖਿਆ)
    {
	vim_snprintf(numbuf, ਆਕਾਰ(numbuf),
				   "%*ld ", ਸੰਖਿਆ_ਚੌੜਾਈ(curwin), (ਲੰਮਾ)lnum);
	ਸੁਨੇਹਾ_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    ਸੁਨੇਹਾ_prt_ਸਤਰ(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    ਖਾਲੀ
print_ਸਤਰ(linenr_T lnum, ਪੂਰਨ use_ਸੰਖਿਆ, ਪੂਰਨ list)
{
    ਪੂਰਨ		save_silent = silent_ਬਹੁਲਕ;

    // apply :filter /pat/
    ਜੇ (ਸੁਨੇਹਾ_filtered(ml_get(lnum)))
	ਵਾਪਸ;

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    silent_ਬਹੁਲਕ = FALSE;
    info_ਸੁਨੇਹਾ = TRUE;	// use mch_msg(), not mch_errmsg()
    print_ਸਤਰ_no_ਅਗੇਤਰ(lnum, use_ਸੰਖਿਆ, list);
    ਜੇ (save_silent)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	ਬਾਹਰਲਾ_flush();
	silent_ਬਹੁਲਕ = save_silent;
    }
    info_ਸੁਨੇਹਾ = FALSE;
}

    ਪੂਰਨ
rename_ਬਫਰ(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    // buffer changed, don't change name now
    ਜੇ (buf != curbuf)
	ਵਾਪਸ FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (aborting())	    // autocmds may abort script processing
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
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
    curbuf->b_ffname = ਨਿਰਾ;
    curbuf->b_sfname = ਨਿਰਾ;
    ਜੇ (setfname(curbuf, new_fname, ਨਿਰਾ, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	ਵਾਪਸ FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    ਜੇ (xfname != ਨਿਰਾ && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	ਜੇ (buf != ਨਿਰਾ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    ਵਾਪਸ OK;
}

/*
 * ":file[!] [fname]".
 */
    ਖਾਲੀ
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    ਜੇ (eap->addr_ਗਿਣਤੀ > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_ਗਿਣਤੀ > 1))
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    ਜੇ (*eap->arg != NUL || eap->addr_ਗਿਣਤੀ == 1)
    {
	ਜੇ (rename_ਬਫਰ(eap->arg) == FAIL)
	    ਵਾਪਸ;
	redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    ਜੇ (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    ਖਾਲੀ
ex_update(exarg_T *eap)
{
    ਜੇ (curbuf_Is_ਬਦਲਿਆ())
	(ਖਾਲੀ)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    ਖਾਲੀ
ex_write(exarg_T *eap)
{
    ਜੇ (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    }

    ਜੇ (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)do_write(eap);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਸਥਿਰ ਪੂਰਨ
check_writable(char_u *fname)
{
    ਜੇ (mch_nodetype(fname) == ਗੰਢ_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Write the current buffer to file "eap->arg".
 * If "eap->append" is TRUE, append to the file.
 *
 * If "*eap->arg == NUL" write to current file.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
do_write(exarg_T *eap)
{
    ਪੂਰਨ		other;
    char_u	*fname = ਨਿਰਾ;		// init to shut up gcc
    char_u	*ffname;
    ਪੂਰਨ		retval = FAIL;
    char_u	*free_fname = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    char_u	*browse_file = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    buf_T	*alt_buf = ਨਿਰਾ;
    ਪੂਰਨ		name_was_missing;

    ਜੇ (not_writing())		// check 'write' option
	ਵਾਪਸ FAIL;

    ffname = eap->arg;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    ਜੇ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, curbuf);
	ਜੇ (browse_file == ਨਿਰਾ)
	    ਜਾਓ theend;
	ffname = browse_file;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (*ffname == NUL)
    {
	ਜੇ (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_ਦਲੀਲ_required));
	    ਜਾਓ theend;
	}
	other = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	ਜੇ (free_fname != ਨਿਰਾ)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    ਜੇ (other)
    {
	ਜੇ (vim_strchr(p_cpo, CPO_ALTWRITE) != ਨਿਰਾ
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	ਨਹੀਂਤਾਂ
	    alt_buf = buflist_findname(ffname);
	ਜੇ (alt_buf != ਨਿਰਾ && alt_buf->b_ml.ml_mfp != ਨਿਰਾ)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_ਵਿੱਚ_another_ਬਫਰ));
	    ਜਾਓ theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    ਜੇ (!other && (bt_dontwrite_ਸੁਨੇਹਾ(curbuf) || check_fname() == FAIL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#ਜੇ_ਅੰਤ
		|| check_readonly(&eap->forceit, curbuf)))
	ਜਾਓ theend;

    ਜੇ (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	ਜੇ (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ਜੇ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		ਜੇ (vim_dialog_yesno(VIM_QUESTION, ਨਿਰਾ,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    ਜਾਓ theend;
		eap->forceit = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		emsg(_(e_use_bang_ਮੰਜ਼ਿਲ_write_partial_ਬਫਰ));
		ਜਾਓ theend;
	    }
	}
    }

    ਜੇ (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	ਜੇ (eap->cmdidx == CMD_saveas && alt_buf != ਨਿਰਾ)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, alt_buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (curbuf != was_curbuf || aborting())
#ਨਹੀਂਤਾਂ
	    ਜੇ (curbuf != was_curbuf)
#ਜੇ_ਅੰਤ
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		ਜਾਓ theend;
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
	    buf_name_ਬਦਲਿਆ(curbuf);

	    apply_autocmds(EVENT_BUFFILEPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, alt_buf);
	    ਜੇ (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, ਨਿਰਾ, ਨਿਰਾ, FALSE, alt_buf);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (curbuf != was_curbuf || aborting())
#ਨਹੀਂਤਾਂ
	    ਜੇ (curbuf != was_curbuf)
#ਜੇ_ਅੰਤ
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		ਜਾਓ theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    ਜੇ (*curbuf->b_p_ft == NUL)
	    {
		ਜੇ (au_has_ਸਮੂਹ((char_u *)"filetypedetect"))
		    (ਖਾਲੀ)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, ਨਿਰਾ);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == ਨਿਰਾ;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	ਜੇ (eap->cmdidx == CMD_saveas)
	{
	    ਜੇ (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	ਜੇ (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    vim_free(browse_file);
#ਜੇ_ਅੰਤ
    vim_free(free_fname);
    ਵਾਪਸ retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    ਪੂਰਨ
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    ਪੂਰਨ		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    ਜੇ (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == ਨਿਰਾ)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	ਜੇ (!eap->forceit && !eap->append)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    // with UNIX it is possible to open a directory
	    ਜੇ (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		ਵਾਪਸ FAIL;
	    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ਜੇ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_ਸੁਨੇਹਾ_ਮਾਪ];

		dialog_ਸੁਨੇਹਾ(buff, _("Overwrite existing file \"%s\"?"), fname);
		ਜੇ (vim_dialog_yesno(VIM_QUESTION, ਨਿਰਾ, buff, 2) != VIM_YES)
		    ਵਾਪਸ FAIL;
		eap->forceit = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		emsg(_(e_file_exists));
		ਵਾਪਸ FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	ਜੇ (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    ਪੂਰਨ		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    ਜੇ (*p_dir == NUL)
	    {
		dir = alloc(5);
		ਜੇ (dir == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		STRCPY(dir, ".");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		dir = alloc(MAXPATHL);
		ਜੇ (dir == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    ਜੇ (r)
	    {
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		ਜੇ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_ਸੁਨੇਹਾ_ਮਾਪ];

		    dialog_ਸੁਨੇਹਾ(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    ਜੇ (vim_dialog_yesno(VIM_QUESTION, ਨਿਰਾ, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			ਵਾਪਸ FAIL;
		    }
		    eap->forceit = TRUE;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    semsg(_(e_ਅਦਲਾ_ਬਦਲੀ_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    ਵਾਪਸ FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    ਖਾਲੀ
ex_wnext(exarg_T *eap)
{
    ਪੂਰਨ		i;

    ਜੇ (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (ਪੂਰਨ)eap->line2;
    ਨਹੀਂਤਾਂ
	i = curwin->w_arg_idx - (ਪੂਰਨ)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    ਖਾਲੀ
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    ਪੂਰਨ		error = 0;
    ਪੂਰਨ		save_forceit = eap->forceit;
    ਪੂਰਨ		save_exiting = exiting;

    ਜੇ (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	ਜੇ (before_quit_all(eap) == FAIL)
	    ਵਾਪਸ;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਜੇ (exiting && !eap->forceit && ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	{
	    bufref_T	bufref;

	    ਸੈੱਟ_bufref(&bufref, buf);
	    ਜੇ (ਪਦ_try_stop_ਕੰਮ(buf) == FAIL)
	    {
		no_write_ਸੁਨੇਹਾ_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    ਨਹੀਂਤਾਂ ਜੇ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (buf_Is_ਬਦਲਿਆ(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    ਜੇ (not_writing())
	    {
		++error;
		ਰੋਕੋ;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    ਜੇ (buf->b_ffname == ਨਿਰਾ && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#ਜੇ_ਅੰਤ
	    ਜੇ (buf->b_ffname == ਨਿਰਾ)
	    {
		semsg(_(e_no_file_name_for_ਬਫਰ_nr), (ਲੰਮਾ)buf->b_fnum);
		++error;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf);
		ਜੇ (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		ਜੇ (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    ਜੇ (exiting)
    {
	ਜੇ (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    ਸਥਿਰ ਪੂਰਨ
not_writing(ਖਾਲੀ)
{
    ਜੇ (p_write)
	ਵਾਪਸ FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    ਵਾਪਸ TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    ਸਥਿਰ ਪੂਰਨ
check_readonly(ਪੂਰਨ *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    ਜੇ (!*forceit && (buf->b_p_ro
		|| (mch_stat((ਅੱਖਰ *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ਜੇ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != ਨਿਰਾ)
	{
	    char_u	buff[DIALOG_ਸੁਨੇਹਾ_ਮਾਪ];

	    ਜੇ (buf->b_p_ro)
		dialog_ਸੁਨੇਹਾ(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    ਨਹੀਂਤਾਂ
		dialog_ਸੁਨੇਹਾ(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    ਜੇ (vim_dialog_yesno(VIM_QUESTION, ਨਿਰਾ, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		ਵਾਪਸ FALSE;
	    }
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ TRUE;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_ਸੈੱਟ_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_str_is_read_only_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override), buf->b_fname);
	ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ
getfile(
    ਪੂਰਨ		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ਪੂਰਨ		setpm,
    linenr_T	lnum,
    ਪੂਰਨ		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    ਪੂਰਨ		other;
    ਪੂਰਨ		retval;
    char_u	*free_me = ਨਿਰਾ;

    ਜੇ (!check_can_ਸੈੱਟ_curbuf_forceit(forceit))
	ਵਾਪਸ GETFILE_ERROR;

    ਜੇ (ਪਾਠ_locked())
	ਵਾਪਸ GETFILE_ERROR;
    ਜੇ (curbuf_locked())
	ਵਾਪਸ GETFILE_ERROR;

    ਜੇ (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    ਨਹੀਂਤਾਂ
	other = (fnum != curbuf->b_fnum);

    ਜੇ (other)
	++no_wait_return;	    // don't wait for autowrite message
    ਜੇ (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbuf_Is_ਬਦਲਿਆ() && autowrite(curbuf, forceit) == FAIL)
    {
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ਜੇ (p_confirm && p_write)
	    dialog_ਬਦਲਿਆ(curbuf, FALSE);
	ਜੇ (curbuf_Is_ਬਦਲਿਆ())
#ਜੇ_ਅੰਤ
	{
	    --no_wait_return;
	    no_write_ਸੁਨੇਹਾ();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    ਜਾਓ theend;
	}
    }
    ਜੇ (other)
	--no_wait_return;
    ਜੇ (setpm)
	setpcmark();
    ਜੇ (!other)
    {
	ਜੇ (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    ਨਹੀਂਤਾਂ ਜੇ (do_ecmd(fnum, ffname, sfname, ਨਿਰਾ, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    ਨਹੀਂਤਾਂ
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    ਵਾਪਸ retval;
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
    ਪੂਰਨ
do_ecmd(
    ਪੂਰਨ		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    ਪੂਰਨ		flags,
    win_T	*oldwin)
{
    ਪੂਰਨ		other_file;		// TRUE if editing another file
    ਪੂਰਨ		oldbuf;			// TRUE if using existing buffer
    ਪੂਰਨ		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = ਨਿਰਾ;
#ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ		did_ਸੈੱਟ_swapcommand = FALSE;
#ਜੇ_ਅੰਤ
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    char_u	dot_ਰਸਤਾ[] = ".";
    char_u	*browse_file = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		retval = FAIL;
    ਲੰਮਾ	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    ਪੂਰਨ		newcol = -1;
    ਪੂਰਨ		solcol = -1;
    pos_T	*pos;
    char_u	*command = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਪੂਰਨ		did_get_winopts = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		readfile_flags = 0;
    ਪੂਰਨ		did_inc_redrawing_disabled = FALSE;
    ਲੰਮਾ	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    ਜੇ (eap != ਨਿਰਾ)
	command = eap->do_ecmd_cmd;
    ਸੈੱਟ_bufref(&old_curbuf, curbuf);

    ਜੇ (fnum != 0)
    {
	ਜੇ (fnum == curbuf->b_fnum)	// file is already being edited
	    ਵਾਪਸ OK;			// nothing to do
	other_file = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
	ਜੇ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    ਜੇ (
# ifdef FEAT_GUI
		!gui.ਵਿੱਚ_use &&
# endif
		    au_has_ਸਮੂਹ((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		ਜੇ (ffname == ਨਿਰਾ || !mch_isdir(ffname))
		    ffname = dot_ਰਸਤਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, curbuf);
		ਜੇ (browse_file == ਨਿਰਾ)
		    ਜਾਓ theend;
		ffname = browse_file;
	    }
	}
#ਜੇ_ਅੰਤ
	// if no short name given, use ffname for short name
	ਜੇ (sfname == ਨਿਰਾ)
	    sfname = ffname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FNAME_CASE
	ਜੇ (sfname != ਨਿਰਾ)
	    fname_case(sfname, 0);   // set correct case for sfname
#ਜੇ_ਅੰਤ

	ਜੇ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == ਨਿਰਾ || *ffname == NUL))
	    ਜਾਓ theend;

	ਜੇ (ffname == ਨਿਰਾ)
	    other_file = TRUE;
					    // there is no file name
	ਨਹੀਂਤਾਂ ਜੇ (*ffname == NUL && curbuf->b_ffname == ਨਿਰਾ)
	    other_file = FALSE;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    ਜੇ (free_fname != ਨਿਰਾ)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    ਜੇ (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_ਬਦਲਿਆ(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == ਨਿਰਾ ? 0 : CCGD_EXCMD)))
    {
	ਜੇ (fnum == 0 && other_file && ffname != ਨਿਰਾ)
	    (ਖਾਲੀ)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	ਜਾਓ theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    ਜੇ (oldwin != ਨਿਰਾ && !win_valid(oldwin))
	oldwin = ਨਿਰਾ;

#ਜੇ defined(FEAT_EVAL)
    ਜੇ ((command != ਨਿਰਾ || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	ਜੇ (command != ਨਿਰਾ)
	    valsize = (ਪੂਰਨ)STRLEN(command) + 3;
	ਨਹੀਂਤਾਂ
	    valsize = 30;
	val.string = alloc(valsize);
	ਜੇ (val.string != ਨਿਰਾ)
	{
	    ਜੇ (command != ਨਿਰਾ)
		val.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)val.string,
		    valsize, ":%s\r", command);
	    ਨਹੀਂਤਾਂ
		val.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)val.string,
		    valsize, "%ldG", (ਲੰਮਾ)newlnum);
	    ਸੈੱਟ_vim_var_string(VV_SWAPCOMMAND, val.string, (ਪੂਰਨ)val.ਲੰਮਾਈ);
	    did_ਸੈੱਟ_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#ਜੇ_ਅੰਤ

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    ਜੇ (other_file)
    {
	ਪੂਰਨ prev_alt_fnum = curwin->w_alt_fnum;

	ਜੇ (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    ਜੇ (oldwin != ਨਿਰਾ)
		buflist_altfpos(oldwin);
	}

	ਜੇ (fnum)
	    buf = buflist_findnr(fnum);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		ਜੇ (command != ਨਿਰਾ)
		{
		    tlnum = atol((ਅੱਖਰ *)command);
		    ਜੇ (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		ਜੇ (newbuf != ਨਿਰਾ)
		{
		    ਜੇ (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    ਜੇ (tlnum > 0)
			newbuf->b_ਆਖਰੀ_cursor.lnum = tlnum;
		}
		ਜਾਓ theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_ਸੈੱਟ_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    ਜੇ (oldwin != ਨਿਰਾ)
		oldwin = curwin;
	    ਸੈੱਟ_bufref(&old_curbuf, curbuf);
	}
	ਜੇ (buf == ਨਿਰਾ)
	    ਜਾਓ theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	ਜੇ (buf->b_locked_ਵੰਡੋ)
	{
	    emsg(_(e_cannot_switch_ਮੰਜ਼ਿਲ_a_closing_ਬਫਰ));
	    ਜਾਓ theend;
	}
	ਜੇ (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	ਨਹੀਂਤਾਂ					// existing memfile
	{
	    oldbuf = TRUE;
	    ਸੈੱਟ_bufref(&bufref, buf);
	    (ਖਾਲੀ)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    ਜੇ (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		ਜਾਓ theend;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (aborting())	    // autocmds may abort script processing
		ਜਾਓ theend;
#ਜੇ_ਅੰਤ
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	ਜੇ ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_ਆਖਰੀ)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->ਖਾਨਾ;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	ਜੇ (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    ਪੂਰਨ		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    ਪ੍ਰਤਿਜ੍ਞਾ(cmdwin_buf == ਨਿਰਾ);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = ਨਿਰਾ;

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
	    ਜੇ (buf->b_fname != ਨਿਰਾ)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    ਸੈੱਟ_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    ਜੇ (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_ਸੁਨੇਹਾ(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		ਜਾਓ theend;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		ਜਾਓ theend;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		ਜੇ (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_ਬਫਰ(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != ਨਿਰਾ);

		// Autocommands may have closed the window.
		ਜੇ (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		// autocmds may abort script processing
		ਜੇ (aborting() && curwin->w_ਬਫਰ != ਨਿਰਾ)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    ਜਾਓ theend;
		}
#ਜੇ_ਅੰਤ
		// Be careful again, like above.
		ਜੇ (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    ਜੇ (curwin->w_ਬਫਰ == ਨਿਰਾ)
			buf = lastbuf;
		    ਨਹੀਂਤਾਂ
		    {
			delbuf_ਸੁਨੇਹਾ(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			ਜਾਓ theend;
		    }
		}
		ਜੇ (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		ਨਹੀਂਤਾਂ
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    ਜੇ (curwin->w_ਬਫਰ == ਨਿਰਾ
			    || curwin->w_s == &(curwin->w_ਬਫਰ->b_s))
			curwin->w_s = &(buf->b_s);
#ਜੇ_ਅੰਤ
		    ਜੇ (curwin->w_ਬਫਰ != ਨਿਰਾ)
			--curwin->w_ਬਫਰ->b_nwindows;

		    curwin->w_ਬਫਰ = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    ਜੇ (!oldbuf && eap != ਨਿਰਾ)
		    {
			ਸੈੱਟ_file_options(TRUE, eap);
			ਸੈੱਟ_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
		did_get_winopts = TRUE;
#ਜੇ_ਅੰਤ
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.ਖਾਨਾ = 0;
    }
    ਨਹੀਂਤਾਂ // !other_file
    {
	ਜੇ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    ਜਾਓ theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    ਜੇ ((flags & ECMD_ਸੈੱਟ_HELP) || keep_help_flag)
    {
	prepare_help_ਬਫਰ();
    }
    ਨਹੀਂਤਾਂ
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	ਜੇ (!curbuf->b_help)
	    ਸੈੱਟ_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    ਜੇ (buf != curbuf)
	ਜਾਓ theend;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (aborting())	    // autocmds may abort script processing
	ਜਾਓ theend;
#ਜੇ_ਅੰਤ

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
    ਜੇ (!other_file && !oldbuf)		// re-use the buffer
    {
	ਸੈੱਟ_ਆਖਰੀ_cursor(curwin);	// may set b_last_cursor
	ਜੇ (newlnum == ECMD_ਆਖਰੀ || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.ਖਾਨਾ;
	}
	buf = curbuf;
	ਜੇ (buf->b_fname != ਨਿਰਾ)
	    new_name = vim_strsave(buf->b_fname);
	ਨਹੀਂਤਾਂ
	    new_name = ਨਿਰਾ;
	ਸੈੱਟ_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	ਜੇ (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    ਜੇ (u_savecommon(0, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		ਜਾਓ theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	ਨਹੀਂਤਾਂ
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	ਜੇ (!bufref_valid(&bufref))
	{
	    delbuf_ਸੁਨੇਹਾ(new_name);	// frees new_name
	    ਜਾਓ theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	ਜੇ (buf != curbuf)
	    ਜਾਓ theend;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (aborting())	    // autocmds may abort script processing
	    ਜਾਓ theend;
#ਜੇ_ਅੰਤ
	buf_clear_file(curbuf);
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_ਸਿਰਾ.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    // If the file name was changed, reset the not-edit flag so that ":write"
    // works.
    ਜੇ (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    ਜੇ (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		ਜੇ (win->w_ਬਫਰ == curbuf)
		    foldUpdateAll(win);
	}
#ਜੇ_ਅੰਤ

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	ਜੇ (!oldbuf)			    // need to read the file
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    ਜੇ (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#ਜੇ_ਅੰਤ
	    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    ਜੇ (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#ਜੇ defined(FEAT_EVAL)
	    ਜੇ (should_abort(open_ਬਫਰ(FALSE, eap, readfile_flags)))
		retval = FAIL;
#ਨਹੀਂਤਾਂ
	    (ਖਾਲੀ)open_ਬਫਰ(FALSE, eap, readfile_flags);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#ਜੇ_ਅੰਤ
	    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_ਅਦਲਾ_ਬਦਲੀ_exists(&old_curbuf);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE,
							      curbuf, &retval);
	    ਜੇ ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	ਜੇ (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *ਪਾਠ = ml_get_curline();

	    ਜੇ (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.ਖਾਨਾ != (ਪੂਰਨ)(skipwhite(ਪਾਠ) - ਪਾਠ))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.ਖਾਨਾ;
	    }
	}
	ਜੇ (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	ਬਦਲਿਆ_ਸਤਰ_abv_curs();

	maketitle();
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	ਜੇ (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_ਸੈੱਟ_title(curwin);
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    ਜੇ (curwin->w_p_diff)
    {
	diff_buf_ਜੋੜੋ(curbuf);
	diff_invalidate(curbuf);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    ਜੇ (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ਖਾਲੀ)parse_spelllang(curwin);
#ਜੇ_ਅੰਤ

    ਜੇ (command == ਨਿਰਾ)
    {
	ਜੇ (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.ਖਾਨਾ = newcol;
	    check_cursor();
	}
	ਨਹੀਂਤਾਂ ਜੇ (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    ਜੇ (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.ਖਾਨਾ = solcol;
		check_cursor_ਖਾਨਾ();
		curwin->w_cursor.coladd = 0;
		curwin->w_ਸੈੱਟ_curswant = true;
	    }
	    ਨਹੀਂਤਾਂ
		beginline(BL_SOL | BL_FIX);
	}
	ਨਹੀਂਤਾਂ			// no line number, go to last line in Ex mode
	{
	    ਜੇ (exmode_ਸਰਗਰਮ)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    ਜੇ (oldbuf && !auto_buf)
    {
	ਪੂਰਨ	ਸੁਨੇਹਾ_scroll_save = ਸੁਨੇਹਾ_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	ਜੇ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    ਸੁਨੇਹਾ_scroll = FALSE;
	ਜੇ (!ਸੁਨੇਹਾ_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll_save;
	ਸੁਨੇਹਾ_scrolled_ign = TRUE;

	ਜੇ (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	ਸੁਨੇਹਾ_scrolled_ign = FALSE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    curbuf->b_ਆਖਰੀ_ਵਰਤਿਆ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    ਜੇ (command != ਨਿਰਾ)
	do_cmdline(command, ਨਿਰਾ, ਨਿਰਾ, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    ਜੇ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ਖਾਲੀ)keymap_init();
#ਜੇ_ਅੰਤ

    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    ਜੇ (!skip_redraw)
    {
	n = *so_ptr;
	ਜੇ (topline == 0 && command == ਨਿਰਾ)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    ਜੇ (p_im && (State & ਬਹੁਲਕ_ਪਾਓ) == 0)
	need_ਸ਼ੁਰੂ_insertmode = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    ਜੇ (p_acd && curbuf->b_ffname != ਨਿਰਾ)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	ਜੇ (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_NETBEANS_INTG)
    ਜੇ (curbuf->b_ffname != ਨਿਰਾ)
    {
# ifdef FEAT_NETBEANS_INTG
	ਜੇ ((flags & ECMD_ਸੈੱਟ_HELP) != ECMD_ਸੈੱਟ_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#ਜੇ_ਅੰਤ

theend:
    ਜੇ (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#ਜੇ defined(FEAT_EVAL)
    ਜੇ (did_ਸੈੱਟ_swapcommand)
	ਸੈੱਟ_vim_var_string(VV_SWAPCOMMAND, ਨਿਰਾ, -1);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    vim_free(browse_file);
#ਜੇ_ਅੰਤ
    vim_free(free_fname);
    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਖਾਲੀ
delbuf_ਸੁਨੇਹਾ(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_ਬਫਰ_str),
					   name == ਨਿਰਾ ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = ਨਿਰਾ;
    au_new_curbuf.br_buf_free_ਗਿਣਤੀ = 0;
}

ਸਥਿਰ ਪੂਰਨ append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    ਸਥਿਰ char_u *
get_append_ਸਤਰ(exarg_T *eap, ਪੂਰਨ indent)
{
    char_u	*theline;
    char_u	*p;

    ਜੇ (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap->ea_getline == ਨਿਰਾ)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	ਜੇ (eap->nextcmd == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	p = vim_strchr(eap->nextcmd, NL);
	ਜੇ (p == ਨਿਰਾ)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	ਜੇ (*p != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ
	    p = ਨਿਰਾ;
	eap->nextcmd = p;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ save_State = State;

	// Set State to avoid the cursor shape to be set to MODE_INSERT
	// state when getline() returns.
	State = ਬਹੁਲਕ_CMDLINE;
	theline = eap->ea_getline(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		eap->cstack->cs_looplevel > 0 ? -1 :
#ਜੇ_ਅੰਤ
		NUL, eap->cookie, indent, TRUE);
	State = save_State;
    }
    ਵਾਪਸ theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    ਖਾਲੀ
ex_append(exarg_T *eap)
{
    char_u	*theline;
    ਪੂਰਨ		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    ਪੂਰਨ		indent = 0;
    char_u	*p;
    ਪੂਰਨ		vcol;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2 = (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (not_ਵਿੱਚ_vim9(eap, eap->skip) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	ਜਦੋਂ ((theline = get_append_ਸਤਰ(eap, 0)) != ਨਿਰਾ)
	{
	    ਪੂਰਨ ਸਿਰਾ = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    ਜੇ (ਸਿਰਾ)
		ਰੋਕੋ;
	}
	ਵਾਪਸ;
    }

    // the ! flag toggles autoindent
    ਜੇ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    ਜੇ (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    ਜੇ (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2 && lnum == 1)
	lnum = 0;

    State = ਬਹੁਲਕ_ਪਾਓ;		    // behave like in Insert mode
    ਜੇ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= ਬਹੁਲਕ_LANGMAP;

    ਲਈ (;;)
    {
	ਸੁਨੇਹਾ_scroll = TRUE;
	need_wait_return = FALSE;
	ਜੇ (curbuf->b_p_ai)
	{
	    ਜੇ (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_ਸਤਰ(eap, indent);
	ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ - 1;
	ਜੇ (theline == ਨਿਰਾ)
	    ਰੋਕੋ;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	ਜੇ (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	ਲਈ (p = theline; indent > vcol; ++p)
	{
	    ਜੇ (*p == ' ')
		++vcol;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == TAB)
		vcol += 8 - vcol % 8;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
	ਜੇ ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (ਖਾਲੀ_ਥਾਵਾਂ_2 ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    ਰੋਕੋ;
	}

	// don't use autoindent if nothing was typed.
	ਜੇ (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2)
	    // there are no marks below the inserted lines
	    appended_ਸਤਰਾਂ(lnum, 1L);
	ਨਹੀਂਤਾਂ
	    appended_ਸਤਰਾਂ_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2)
	{
	    ml_delete(2L);
	    ਖਾਲੀ_ਥਾਵਾਂ_2 = FALSE;
	}
    }
    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;

    ਜੇ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_ਸ਼ੁਰੂ.lnum = (eap->line2 < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਜੇ (eap->cmdidx != CMD_append)
	    --curbuf->b_op_ਸ਼ੁਰੂ.lnum;
	curbuf->b_op_ਸਿਰਾ.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_ਸ਼ੁਰੂ.lnum;
	curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
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
    ਖਾਲੀ
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (not_ਵਿੱਚ_vim9(eap, eap->skip) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (eap->skip)
    {
	ex_append(eap);
	ਵਾਪਸ;
    }
    ਜੇ (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	ਵਾਪਸ;

    // the ! flag toggles autoindent
    ਜੇ (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    ਲਈ (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)	    // nothing to delete
	    ਰੋਕੋ;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_ਸਤਰਾਂ_mark(eap->line1, (ਲੰਮਾ)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    ਖਾਲੀ
ex_z(exarg_T *eap)
{
    char_u	*x;
    ਲੰਮਾ	bigness;
    char_u	*ਕਿਸਮ;
    ਪੂਰਨ		minus = 0;
    linenr_T	ਸ਼ੁਰੂ, ਸਿਰਾ, curs, i;
    ਪੂਰਨ		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    ਜੇ (eap->forceit)
	bigness = ਪੰਕਤੀਆਂ - 1;
    ਨਹੀਂਤਾਂ ਜੇ (!ONE_ਖਿੜਕੀ)
	bigness = curwin->w_ਉਚਾਈ - 3;
    ਨਹੀਂਤਾਂ
	bigness = curwin->w_p_scr * 2;
    ਜੇ (bigness < 1)
	bigness = 1;

    x = eap->arg;
    ਕਿਸਮ = x;
    ਜੇ (*ਕਿਸਮ == '-' || *ਕਿਸਮ == '+' || *ਕਿਸਮ == '='
					      || *ਕਿਸਮ == '^' || *ਕਿਸਮ == '.')
	++x;
    ਜਦੋਂ (*x == '-' || *x == '+')
	++x;

    ਜੇ (*x != 0)
    {
	ਜੇ (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_ਦਲੀਲ_ਮੰਜ਼ਿਲ_z));
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ
	{
	    bigness = atol((ਅੱਖਰ *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    ਜੇ (bigness > 2 * curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

	    p_ਖਿੜਕੀ = bigness;
	    ਜੇ (*ਕਿਸਮ == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    ਜੇ (*ਕਿਸਮ == '-' || *ਕਿਸਮ == '+')
	ਲਈ (x = ਕਿਸਮ + 1; *x == *ਕਿਸਮ; ++x)
	    ;

    ਚੋਣ (*ਕਿਸਮ)
    {
	ਹਾਲਤ '-':
	    ਸ਼ੁਰੂ = lnum - bigness * (linenr_T)(x - ਕਿਸਮ) + 1;
	    ਸਿਰਾ = ਸ਼ੁਰੂ + bigness - 1;
	    curs = ਸਿਰਾ;
	    ਰੋਕੋ;

	ਹਾਲਤ '=':
	    ਸ਼ੁਰੂ = lnum - (bigness + 1) / 2 + 1;
	    ਸਿਰਾ = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    ਰੋਕੋ;

	ਹਾਲਤ '^':
	    ਸ਼ੁਰੂ = lnum - bigness * 2;
	    ਸਿਰਾ = lnum - bigness;
	    curs = lnum - bigness;
	    ਰੋਕੋ;

	ਹਾਲਤ '.':
	    ਸ਼ੁਰੂ = lnum - (bigness + 1) / 2 + 1;
	    ਸਿਰਾ = lnum + (bigness + 1) / 2 - 1;
	    curs = ਸਿਰਾ;
	    ਰੋਕੋ;

	ਮੂਲ:  // '+'
	    ਸ਼ੁਰੂ = lnum;
	    ਜੇ (*ਕਿਸਮ == '+')
		ਸ਼ੁਰੂ += bigness * (linenr_T)(x - ਕਿਸਮ - 1) + 1;
	    ਨਹੀਂਤਾਂ ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
		++ਸ਼ੁਰੂ;
	    ਸਿਰਾ = ਸ਼ੁਰੂ + bigness - 1;
	    curs = ਸਿਰਾ;
	    ਰੋਕੋ;
    }

    ਜੇ (ਸ਼ੁਰੂ < 1)
	ਸ਼ੁਰੂ = 1;

    ਜੇ (ਸਿਰਾ > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਸਿਰਾ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (curs > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	curs = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਨਹੀਂਤਾਂ ਜੇ (curs < 1)
	curs = 1;

    ਲਈ (i = ਸ਼ੁਰੂ; i <= ਸਿਰਾ; i++)
    {
	ਜੇ (minus && i == lnum)
	{
	    ਸੁਨੇਹਾ_putchar('\n');

	    ਲਈ (j = 1; j < Columns; j++)
		ਸੁਨੇਹਾ_putchar('-');
	}

	print_ਸਤਰ(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	ਜੇ (minus && i == lnum)
	{
	    ਸੁਨੇਹਾ_putchar('\n');

	    ਲਈ (j = 1; j < Columns; j++)
		ਸੁਨੇਹਾ_putchar('-');
	}
    }

    ਜੇ (curwin->w_cursor.lnum != curs)
    {
	curwin->w_cursor.lnum = curs;
	curwin->w_cursor.ਖਾਨਾ = 0;
    }
    ex_no_reprint = TRUE;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    ਪੂਰਨ
check_restricted(ਖਾਲੀ)
{
    ਜੇ (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_ਵਿੱਚ_rvim));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    ਪੂਰਨ
check_secure(ਖਾਲੀ)
{
    ਜੇ (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_ਵਿੱਚ_current_dir_or_ਟੈਗ_ਖੋਜ));
	ਵਾਪਸ TRUE;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    ਜੇ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ਵਿੱਚ_sandbox));
	ਵਾਪਸ TRUE;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FALSE;
}

ਸਥਿਰ char_u	*old_sub = ਨਿਰਾ;	// previous substitute pattern
ਸਥਿਰ ਪੂਰਨ	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ	do_all;		// do multiple substitutions per line
    ਪੂਰਨ	do_ask;		// ask for confirmation
    ਪੂਰਨ	do_ਗਿਣਤੀ;	// count only
    ਪੂਰਨ	do_error;	// if false, ignore errors
    ਪੂਰਨ	do_print;	// print last line with subs.
    ਪੂਰਨ	do_list;	// list last line with subs.
    ਪੂਰਨ	do_ਸੰਖਿਆ;	// list last line with line nr
    ਪੂਰਨ	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *ਸ਼ੁਰੂ, ਪੂਰਨ delimiter)
{
    char_u *p = ਸ਼ੁਰੂ;

    ਜਦੋਂ (p[0])
    {
	ਜੇ (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    ਰੋਕੋ;
	}
	ਜੇ (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    ਵਾਪਸ p;
}

    ਸਥਿਰ ਪੂਰਨ
check_regexp_delim(ਪੂਰਨ c)
{
    ਜੇ (ਸੁਰੱਖਿਅਤ_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    ਖਾਲੀ
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    ਲੰਮਾ	i = 0;
    regmmatch_T regmatch;
    ਸਥਿਰ subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    subflags_T	subflags_save;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		save_do_all;		// remember user specified 'g' flag
    ਪੂਰਨ		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = ਨਿਰਾ;		// init for GCC
    string_T	pat = {ਨਿਰਾ, 0};
    ਪੂਰਨ		delimiter;
    size_t	sublen;
    ਪੂਰਨ		got_quit = FALSE;
    ਪੂਰਨ		got_ਮੇਲ = FALSE;
    ਪੂਰਨ		which_pat;
    char_u	*cmd;
    char_u	*p;
    ਪੂਰਨ		save_State;
    linenr_T	ਪਹਿਲਾ_ਸਤਰ = 0;		// first changed line
    linenr_T	ਆਖਰੀ_ਸਤਰ = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    linenr_T	line2;
    ਲੰਮਾ	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    ਪੂਰਨ		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    ਪੂਰਨ		ਸ਼ੁਰੂ_nsubs;
    ਪੂਰਨ		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		save_ma = 0;
    ਪੂਰਨ		save_sandbox = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    textprop_T	*ਪਾਠ_props = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    cmd = eap->arg;
    ਜੇ (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    ਸ਼ੁਰੂ_nsubs = sub_nsubs;

    ਜੇ (eap->cmdidx == CMD_tilde)
	which_pat = RE_ਆਖਰੀ;	// use last used regexp
    ਨਹੀਂਤਾਂ
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    ਜੇ (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == ਨਿਰਾ)
    {
	// don't accept alphanumeric for separator
	ਜੇ (check_regexp_delim(*cmd) == FAIL)
	    ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (ਵਿੱਚ_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    ਵਾਪਸ;
#ਜੇ_ਅੰਤ

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	ਜੇ (*cmd == '\\')
	{
	    ਜੇ (ਵਿੱਚ_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_ਵਿੱਚ_vim9_script));
		ਵਾਪਸ;
	    }
	    ++cmd;
	    ਜੇ (vim_strchr((char_u *)"/?&", *cmd) == ਨਿਰਾ)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		ਵਾਪਸ;
	    }
	    ਜੇ (*cmd != '&')
		which_pat = RE_ਖੋਜ;	    // use last '/' pattern
	    STR_LITERAL_ਸੈੱਟ(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	ਨਹੀਂਤਾਂ		// find the end of the regexp
	{
	    which_pat = RE_ਆਖਰੀ;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, ਨਿਰਾ, ਨਿਰਾ);
	    pat.ਲੰਮਾਈ = (size_t)(cmd - pat.string);
	    ਜੇ (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	ਜੇ (sub == ਨਿਰਾ)
	    // out of memory
	    ਵਾਪਸ;

	ਜੇ (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    ਜੇ (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != ਨਿਰਾ)
	    {
		ਜੇ (old_sub == ਨਿਰਾ)	// there is no previous command
		{
		    emsg(_(e_no_ਪਿਛਲਾ_substitute_regular_ਵਿਅੰਜਕ));
		    vim_free(sub);
		    ਵਾਪਸ;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		ਜੇ (sub == ਨਿਰਾ)
		    // out of memory
		    ਵਾਪਸ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		ਜੇ (old_sub == ਨਿਰਾ)
		{
		    // out of memory
		    vim_free(sub);
		    ਵਾਪਸ;
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!eap->skip)	// use previous pattern and substitution
    {
	ਜੇ (old_sub == ਨਿਰਾ)	// there is no previous command
	{
	    emsg(_(e_no_ਪਿਛਲਾ_substitute_regular_ਵਿਅੰਜਕ));
	    ਵਾਪਸ;
	}
	pat.string = ਨਿਰਾ;		// search_regcomp() will use previous pattern
	pat.ਲੰਮਾਈ = 0;
	sub = vim_strsave(old_sub);
	ਜੇ (sub == ਨਿਰਾ)
	    ਵਾਪਸ;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    ਜੇ (pat.string != ਨਿਰਾ && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_ਸਤਰਾਂ_ਗਿਣਤੀ;

	ਜੇ (eap->skip)
	{
	    vim_free(sub);
	    ਵਾਪਸ;
	}
	curwin->w_cursor.lnum = eap->line1;
	ਜੇ (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_ਸਤਰਾਂ_ਗਿਣਤੀ = eap->line2 - eap->line1 + 1;
	ਜੇ (eap->line2 < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ++joined_ਸਤਰਾਂ_ਗਿਣਤੀ;
	ਜੇ (joined_ਸਤਰਾਂ_ਗਿਣਤੀ > 1)
	{
	    (ਖਾਲੀ)do_join(joined_ਸਤਰਾਂ_ਗਿਣਤੀ, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_ਸਤਰਾਂ_ਗਿਣਤੀ - 1;
	    sub_nlines = 1;
	    (ਖਾਲੀ)do_sub_ਸੁਨੇਹਾ(FALSE);
	    ex_may_print(eap);
	}

	ਜੇ (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.ਲੰਮਾਈ, magic_isset());
	// put pattern in history
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_history(HIST_ਖੋਜ, pat.string, pat.ਲੰਮਾਈ, TRUE, NUL);
	vim_free(sub);

	ਵਾਪਸ;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    ਜੇ (*cmd == '&')
	++cmd;
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (ਵਿੱਚ_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (!p_ed)
	{
	    ਜੇ (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    ਨਹੀਂਤਾਂ
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_ਗਿਣਤੀ = FALSE;
	subflags.do_ਸੰਖਿਆ = FALSE;
	subflags.do_ic = 0;
    }
    ਜਦੋਂ (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	ਜੇ (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'n')
	    subflags.do_ਗਿਣਤੀ = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_ਆਖਰੀ;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'p')
	    subflags.do_print = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_ਸੰਖਿਆ = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
	++cmd;
    }
    ਜੇ (subflags.do_ਗਿਣਤੀ)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    ਜੇ (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	ਜੇ (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_ਸਕਾਰਾਤਮਕ_ਗਿਣਤੀ_required));
	    vim_free(sub);
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (i >= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	{
	    ਅੱਖਰ	buf[20];
	    vim_snprintf(buf, ਆਕਾਰ(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    ਵਾਪਸ;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	ਜੇ (eap->line2 > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    eap->line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    ਜੇ (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	ਸੈੱਟ_nextcmd(eap, cmd);
	ਜੇ (eap->nextcmd == ਨਿਰਾ)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	ਵਾਪਸ;
    }

    ਜੇ (!subflags.do_ਗਿਣਤੀ && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	ਵਾਪਸ;
    }

    ਜੇ (ਖੋਜ_regcomp(pat.string, pat.ਲੰਮਾਈ, ਨਿਰਾ, RE_SUBST, which_pat, ਖੋਜ_HIS, &regmatch) == FAIL)
    {
	ਜੇ (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	ਵਾਪਸ;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    ਜੇ (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = ਨਿਰਾ;
    sub_firstline.ਲੰਮਾਈ = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    ਜੇ (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ;
	sub = p;
    }
    ਨਹੀਂਤਾਂ
    {
	p = regtilde(sub, magic_isset());

	ਜੇ (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    ਲਈ (lnum = eap->line1; lnum <= line2 && !(got_quit
#ਜੇ defined(FEAT_EVAL)
		|| aborting()
#ਜੇ_ਅੰਤ
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ਨਿਰਾ);
	ਜੇ (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_ਸ਼ੁਰੂ = {ਨਿਰਾ, 0};
	    size_t	new_ਸ਼ੁਰੂ_ਮਾਪ = 0;
	    char_u	*new_ਸਿਰਾ;
	    ਪੂਰਨ		did_sub = FALSE;
	    ਪੂਰਨ		lastone;
	    size_t	copy_len;
	    size_t	needed_ਮਾਪ;
	    ਲੰਮਾ	nmatch_tl = 0;	// nr of lines matched below lnum
	    ਪੂਰਨ		do_again;	// do it again after joining lines
	    ਪੂਰਨ		skip_ਮੇਲ = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_ਵੰਡੋ = false;	// "\r" split the line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਪੂਰਨ		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	ਕੁੱਲ_added =  0;
	    ਪੂਰਨ		ਪਾਠ_prop_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

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
	    ਜੇ (!got_ਮੇਲ)
	    {
		setpcmark();
		got_ਮੇਲ = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    ਲਈ (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		ਜੇ (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    ਰੋਕੋ;

		ਜੇ (sub_firstline.string == ਨਿਰਾ)
		{
		    sub_firstline.ਲੰਮਾਈ = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.ਲੰਮਾਈ);
		    ਜੇ (sub_firstline.string == ਨਿਰਾ)
		    {
			vim_free(new_ਸ਼ੁਰੂ.string);
			ਜਾਓ outofmem;
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
		ਜੇ (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].ਖਾਨਾ)
		{
		    ਜੇ (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_ਮੇਲ = TRUE;
		    ਨਹੀਂਤਾਂ
		    {
			 // search for a match at next column
			ਜੇ (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			ਨਹੀਂਤਾਂ
			    ++matchcol;
		    }
		    ਜਾਓ skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].ਖਾਨਾ;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		ਜੇ (subflags.do_ਗਿਣਤੀ)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    ਜੇ (nmatch > 1)
		    {
			matchcol = (colnr_T)sub_firstline.ਲੰਮਾਈ;
			nmatch = 1;
			skip_ਮੇਲ = TRUE;
		    }
		    sub_nsubs++;
		    did_sub = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    // Skip the substitution, unless an expression is used,
		    // then it is evaluated in the sandbox.
		    ਜੇ (!(sub[0] == '\\' && sub[1] == '='))
#ਜੇ_ਅੰਤ
			ਜਾਓ skip;
		}

		ਜੇ (subflags.do_ask)
		{
		    ਪੂਰਨ typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = ਬਹੁਲਕ_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.ਖਾਨਾ = regmatch.startpos[0].ਖਾਨਾ;
		    ਜੇ (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    ਜੇ (vim_strchr(p_cpo, CPO_UNDO) != ਨਿਰਾ)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    ਜਦੋਂ (subflags.do_ask)
		    {
			ਜੇ (exmode_ਸਰਗਰਮ)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_ਸਤਰ_no_ਅਗੇਤਰ(lnum,
					 subflags.do_ਸੰਖਿਆ, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, ਨਿਰਾ, ਨਿਰਾ, 0);
			    curwin->w_cursor.ਖਾਨਾ = regmatch.endpos[0].ਖਾਨਾ - 1;
			    ਜੇ (curwin->w_cursor.ਖਾਨਾ < 0)
				curwin->w_cursor.ਖਾਨਾ = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   ਨਿਰਾ, ਨਿਰਾ, &ec, 0);
			    curwin->w_cursor.ਖਾਨਾ = regmatch.startpos[0].ਖਾਨਾ;
			    ਜੇ (subflags.do_ਸੰਖਿਆ || curwin->w_p_nu)
			    {
				ਪੂਰਨ numw = ਸੰਖਿਆ_ਚੌੜਾਈ(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
			    ਲਈ (i = 0; i < (ਲੰਮਾ)sc; ++i)
				ਸੁਨੇਹਾ_putchar(' ');
			    ਲਈ ( ; i <= (ਲੰਮਾ)ec; ++i)
				ਸੁਨੇਹਾ_putchar('^');

			    resp = getexmodeline('?', ਨਿਰਾ, 0, TRUE);
			    ਜੇ (resp != ਨਿਰਾ)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				ਜੇ (ex_ਸਾਧਾਰਨ_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    string_T orig_ਸਤਰ = {ਨਿਰਾ, 0};
			    ਪੂਰਨ    len_change = 0;
			    ਪੂਰਨ	   save_p_lz = p_lz;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    ਪੂਰਨ save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#ਜੇ_ਅੰਤ
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    ਜੇ (new_ਸ਼ੁਰੂ.string != ਨਿਰਾ)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_ਸਤਰ.ਲੰਮਾਈ = ml_get_len(lnum);
				orig_ਸਤਰ.string = vim_strnsave(ml_get(lnum), orig_ਸਤਰ.ਲੰਮਾਈ);
				ਜੇ (orig_ਸਤਰ.string != ਨਿਰਾ)
				{
				    string_T new_ਸਤਰ;

				    new_ਸਤਰ.string = concat_str(new_ਸ਼ੁਰੂ.string,
						     sub_firstline.string + copycol);
				    ਜੇ (new_ਸਤਰ.string == ਨਿਰਾ)
					VIM_CLEAR_STRING(orig_ਸਤਰ);
				    ਨਹੀਂਤਾਂ
				    {
					new_ਸਤਰ.ਲੰਮਾਈ =
					    new_ਸ਼ੁਰੂ.ਲੰਮਾਈ + (sub_firstline.ਲੰਮਾਈ - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (ਪੂਰਨ)new_ਸਤਰ.ਲੰਮਾਈ - (ਪੂਰਨ)orig_ਸਤਰ.ਲੰਮਾਈ;
					curwin->w_cursor.ਖਾਨਾ += len_change;
					ml_replace(lnum, new_ਸਤਰ.string, FALSE);
				    }
				}
			    }

			    ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    ਖੋਜ_ਮੇਲ_endcol = regmatch.endpos[0].ਖਾਨਾ
								 + len_change;
			    ਜੇ (ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ == 0
						   && ਖੋਜ_ਮੇਲ_endcol == 0)
				// highlight at least one character for /^/
				ਖੋਜ_ਮੇਲ_endcol = 1;
			    highlight_ਮੇਲ = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_ਮੇਲ = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#ਜੇ_ਅੰਤ
			    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1)
				ਸੁਨੇਹਾ_didout = FALSE;	// avoid a scroll-up
			    ਸੁਨੇਹਾ_starthere();
			    i = ਸੁਨੇਹਾ_scroll;
			    ਸੁਨੇਹਾ_scroll = 0;		// truncate msg when
							// needed
			    ਸੁਨੇਹਾ_no_ਹੋਰ = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    ਸੁਨੇਹਾ_no_ਹੋਰ = FALSE;
			    ਸੁਨੇਹਾ_scroll = i;
			    showruler(TRUE);
			    windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
			    RedrawingDisabled = save_RedrawingDisabled;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#ਜੇ_ਅੰਤ
			    ++no_mapping;	// don't map this key
			    ++allow_ਕੁੰਜੀਆਂ;	// allow special keys
			    typed = plain_vgetc();
			    --allow_ਕੁੰਜੀਆਂ;
			    --no_mapping;

			    // clear the question
			    ਸੁਨੇਹਾ_didout = FALSE;	// don't scroll up
			    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    ਜੇ (orig_ਸਤਰ.string != ਨਿਰਾ)
				ml_replace(lnum, orig_ਸਤਰ.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			ਜੇ (typed == 'q' || typed == ESC || typed == Ctrl_C
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
				|| typed == intr_char
#ਜੇ_ਅੰਤ
				)
			{
			    got_quit = TRUE;
			    ਰੋਕੋ;
			}
			ਜੇ (typed == 'n')
			    ਰੋਕੋ;
			ਜੇ (typed == 'y')
			    ਰੋਕੋ;
			ਜੇ (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    ਰੋਕੋ;
			}
			ਜੇ (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    ਰੋਕੋ;
			}
			ਜੇ (typed == Ctrl_E)
			    scrollup_clamp();
			ਨਹੀਂਤਾਂ ਜੇ (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    ਜੇ (vim_strchr(p_cpo, CPO_UNDO) != ਨਿਰਾ)
			--no_u_sync;

		    ਜੇ (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			ਜੇ (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.ਲੰਮਾਈ;
			    skip_ਮੇਲ = TRUE;
			}
			ਜਾਓ skip;
		    }
		    ਜੇ (got_quit)
			ਜਾਓ skip;
		}

		// Move the cursor to the start of the match, so that we can
		// use "\=col(".").
		curwin->w_cursor.ਖਾਨਾ = regmatch.startpos[0].ਖਾਨਾ;

		/*
		 * 3. substitute the string.
		 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		save_ma = curbuf->b_p_ma;
		save_sandbox = sandbox;
		ਜੇ (subflags.do_ਗਿਣਤੀ)
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
#ਜੇ_ਅੰਤ
		// Get length of substitution part, including the NUL.
		// When it fails sublen is zero.
		sublen = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, sub_firstline.string, 0,
		    REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		--textlock;

		// If getting the substitute string caused an error, don't do
		// the replacement.
		// Don't keep flags set by a recursive call.
		subflags = subflags_save;
		ਜੇ (sublen == 0 || aborting() || subflags.do_ਗਿਣਤੀ)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    ਜਾਓ skip;
		}
#ਜੇ_ਅੰਤ

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		ਜੇ (nmatch > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - sub_firstlnum + 1;
		    skip_ਮੇਲ = TRUE;
		    // safety check
		    ਜੇ (nmatch < 0)
			ਜਾਓ skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		ਜੇ (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.ਲੰਮਾਈ = sub_firstline.ਲੰਮਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    ਜੇ (curbuf->b_has_textprop)
		    {
			ਪੂਰਨ ਬਾਈਟਾਂ_added = (ਪੂਰਨ)sublen - 1 - (regmatch.endpos[0].ਖਾਨਾ
						   - regmatch.startpos[0].ਖਾਨਾ);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			ਜੇ (adjust_prop_columns(lnum,
					ਕੁੱਲ_added + regmatch.startpos[0].ਖਾਨਾ,
						       ਬਾਈਟਾਂ_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			ਕੁੱਲ_added += (colnr_T)ਬਾਈਟਾਂ_added;
		    }
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    ਜੇ (curbuf->b_has_textprop)
		    {
			char_u	*prop_ਸ਼ੁਰੂ;

			// Props in the first line may be shortened or deleted
			ਜੇ (adjust_prop_columns(lnum,
					ਕੁੱਲ_added + regmatch.startpos[0].ਖਾਨਾ,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			ਕੁੱਲ_added -=
			    (colnr_T)(sub_firstline.ਲੰਮਾਈ - regmatch.startpos[0].ਖਾਨਾ);

			// Props in the last line may be moved or deleted
			ਜੇ (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].ਖਾਨਾ, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			ਪਾਠ_prop_ਗਿਣਤੀ = get_ਪਾਠ_props(curbuf, lastlnum,
							   &prop_ਸ਼ੁਰੂ, FALSE);
			ਜੇ (ਪਾਠ_prop_ਗਿਣਤੀ > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(ਪਾਠ_props);
			    ਪਾਠ_props = ALLOC_MULT(textprop_T,
							      ਪਾਠ_prop_ਗਿਣਤੀ);
			    ਜੇ (ਪਾਠ_props != ਨਿਰਾ)
			    {
				mch_memmove(ਪਾਠ_props, prop_ਸ਼ੁਰੂ,
					 ਪਾਠ_prop_ਗਿਣਤੀ * ਆਕਾਰ(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				ਪੂਰਨ wi = 0;
				ਲਈ (ਪੂਰਨ pi = 0; pi < ਪਾਠ_prop_ਗਿਣਤੀ; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    ਜੇ (ਪਾਠ_props[pi].tp_id < 0
					    || (ਪਾਠ_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					ਜਾਰੀ;
				    ਪਾਠ_props[wi] = ਪਾਠ_props[pi];
				    ਪਾਠ_props[wi].tp_ਖਾਨਾ +=
					regmatch.startpos[0].ਖਾਨਾ + (colnr_T)sublen - 1;
				    ਪਾਠ_props[wi].u.tp_ਪਾਠ = ਨਿਰਾ;
				    ++wi;
				}
				ਪਾਠ_prop_ਗਿਣਤੀ = wi;
			    }
			}
		    }
#ਜੇ_ਅੰਤ
		    tmp.string = ml_get(lastlnum);
		    tmp.ਲੰਮਾਈ = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    ਜੇ (curbuf->b_has_textprop)
			ਕੁੱਲ_added +=
			    (colnr_T)(tmp.ਲੰਮਾਈ - regmatch.endpos[0].ਖਾਨਾ);
#ਜੇ_ਅੰਤ
		}

		copy_len = (size_t)(regmatch.startpos[0].ਖਾਨਾ - copycol);
		needed_ਮਾਪ = copy_len
		    + (tmp.ਲੰਮਾਈ - (size_t)regmatch.endpos[0].ਖਾਨਾ) + sublen + 1;

		ਜੇ (new_ਸ਼ੁਰੂ.string == ਨਿਰਾ)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_ਸ਼ੁਰੂ_ਮਾਪ = needed_ਮਾਪ + 50;
		    ਜੇ ((new_ਸ਼ੁਰੂ.string = alloc_clear(new_ਸ਼ੁਰੂ_ਮਾਪ)) == ਨਿਰਾ)
			ਜਾਓ outofmem;
		    *new_ਸ਼ੁਰੂ.string = NUL;
		    new_ਸ਼ੁਰੂ.ਲੰਮਾਈ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_ਮਾਪ += new_ਸ਼ੁਰੂ.ਲੰਮਾਈ;
		    ਜੇ (needed_ਮਾਪ > new_ਸ਼ੁਰੂ_ਮਾਪ)
		    {
			new_ਸ਼ੁਰੂ_ਮਾਪ = needed_ਮਾਪ + 50;
			ਜੇ ((p1 = alloc_clear(new_ਸ਼ੁਰੂ_ਮਾਪ)) == ਨਿਰਾ)
			{
			    vim_free(new_ਸ਼ੁਰੂ.string);
			    ਜਾਓ outofmem;
			}
			mch_memmove(p1, new_ਸ਼ੁਰੂ.string, new_ਸ਼ੁਰੂ.ਲੰਮਾਈ + 1);
			vim_free(new_ਸ਼ੁਰੂ.string);
			new_ਸ਼ੁਰੂ.string = p1;
		    }
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_ਸ਼ੁਰੂ.string + new_ਸ਼ੁਰੂ.ਲੰਮਾਈ,
		    sub_firstline.string + copycol, copy_len);
		new_ਸ਼ੁਰੂ.ਲੰਮਾਈ += copy_len;

		// new text added here
		new_ਸਿਰਾ = new_ਸ਼ੁਰੂ.string + new_ਸ਼ੁਰੂ.ਲੰਮਾਈ;

		ਜੇ (new_ਸ਼ੁਰੂ_ਮਾਪ - copy_len < sublen)
		    sublen = new_ਸ਼ੁਰੂ_ਮਾਪ - copy_len - 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		++textlock;
#ਜੇ_ਅੰਤ
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_ਸਿਰਾ, (ਪੂਰਨ)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		ਜੇ (n > 0)
		    new_ਸ਼ੁਰੂ.ਲੰਮਾਈ += n - 1;	    // remove 1 for the NUL

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		--textlock;
#ਜੇ_ਅੰਤ
		sub_nsubs++;
		did_sub = TRUE;

		// Move the cursor to the start of the line, to avoid that it
		// is beyond the end of the line after the substitution.
		curwin->w_cursor.ਖਾਨਾ = 0;

		// For a multi-line match, make a copy of the last matched
		// line and continue in that one.
		ਜੇ (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.ਲੰਮਾਈ = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.ਲੰਮਾਈ);
		    ਜੇ (sub_firstline.string == ਨਿਰਾ)
		    {
			vim_free(new_ਸ਼ੁਰੂ.string);
			ਜਾਓ outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    ਜੇ (sub_firstlnum <= line2)
			do_again = TRUE;
		    ਨਹੀਂਤਾਂ
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].ਖਾਨਾ;

		ਜੇ (skip_ਮੇਲ)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    ਜੇ (sub_firstline.string == ਨਿਰਾ)
		    {
			vim_free(new_ਸ਼ੁਰੂ.string);
			ਜਾਓ outofmem;
		    }
		    sub_firstline.ਲੰਮਾਈ = 0;
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
		ਲਈ (p1 = new_ਸਿਰਾ; *p1; ++p1)
		{
		    ਜੇ (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_ਸ਼ੁਰੂ.ਲੰਮਾਈ - (p1 - new_ਸ਼ੁਰੂ.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_ਸ਼ੁਰੂ.ਲੰਮਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			ਜੇ (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    ਜੇ (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_ਸ਼ੁਰੂ.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#ਜੇ_ਅੰਤ
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*p1 == CAR)
		    {
			ਜੇ (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_ਸ਼ੁਰੂ.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_ਸ਼ੁਰੂ.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    ਜੇ (subflags.do_ask)
				appended_ਸਤਰਾਂ(lnum - 1, 1L);
			    ਨਹੀਂਤਾਂ
			    {
				ਜੇ (ਪਹਿਲਾ_ਸਤਰ == 0)
				    ਪਹਿਲਾ_ਸਤਰ = lnum;
				ਆਖਰੀ_ਸਤਰ = lnum + 1;
			    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			    adjust_props_for_ਵੰਡੋ(lnum + 1, lnum,
							       plen, 1, FALSE);
#ਜੇ_ਅੰਤ
			    // all line numbers increase
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    did_ਵੰਡੋ = true;
			    // move the cursor to the new line, like Vi
			    ++curwin->w_cursor.lnum;
			    // copy the rest
			    n = new_ਸ਼ੁਰੂ.ਲੰਮਾਈ - (size_t)plen;
			    mch_memmove(new_ਸ਼ੁਰੂ.string, p1 + 1, n + 1);
			    new_ਸ਼ੁਰੂ.ਲੰਮਾਈ = n;
			    p1 = new_ਸ਼ੁਰੂ.string - 1;
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
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
		lastone = (skip_ਮੇਲ
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
		ਜੇ (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_ਵੰਡੋ)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, ਨਿਰਾ)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    ਜੇ (new_ਸ਼ੁਰੂ.string != ਨਿਰਾ)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * "matchcol" has to be adjusted, we use the end of
			 * the line as reference, because the substitute may
			 * have changed the number of characters.  Same for
			 * "prev_matchcol".
			 */
			STRCPY(new_ਸ਼ੁਰੂ.string + new_ਸ਼ੁਰੂ.ਲੰਮਾਈ,
			    sub_firstline.string + copycol);
			new_ਸ਼ੁਰੂ.ਲੰਮਾਈ += sub_firstline.ਲੰਮਾਈ - (size_t)copycol;
			matchcol = (colnr_T)(sub_firstline.ਲੰਮਾਈ - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.ਲੰਮਾਈ
							      - prev_matchcol);

			ਜੇ (u_savesub(lnum) != OK)
			    ਰੋਕੋ;
			ml_replace(lnum, new_ਸ਼ੁਰੂ.string, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			ਜੇ (ਪਾਠ_props != ਨਿਰਾ)
			    ਜੋੜੋ_ਪਾਠ_props(lnum, ਪਾਠ_props, ਪਾਠ_prop_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
			ਜੇ (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    ਜੇ (u_savedel(lnum, nmatch_tl) != OK)
				ਰੋਕੋ;
			    ਲਈ (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    ਜੇ (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (ਲੰਮਾ)MAXLNUM, -nmatch_tl);
			    ਨਹੀਂਤਾਂ
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (ਲੰਮਾ)MAXLNUM, -nmatch_tl);
			    ਜੇ (subflags.do_ask)
				deleted_ਸਤਰਾਂ(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			ਜੇ (subflags.do_ask)
			    ਬਦਲਿਆ_ਬਾਈਟਾਂ(lnum, 0);
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (ਪਹਿਲਾ_ਸਤਰ == 0)
				ਪਹਿਲਾ_ਸਤਰ = lnum;
			    ਆਖਰੀ_ਸਤਰ = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_ਸ਼ੁਰੂ.string;
			sub_firstline.ਲੰਮਾਈ = new_ਸ਼ੁਰੂ.ਲੰਮਾਈ;
			new_ਸ਼ੁਰੂ.string = ਨਿਰਾ;
			new_ਸ਼ੁਰੂ.ਲੰਮਾਈ = 0;
			matchcol = (colnr_T)(sub_firstline.ਲੰਮਾਈ - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.ਲੰਮਾਈ
							      - prev_matchcol);
			copycol = 0;
			did_ਵੰਡੋ = false;
		    }
		    ਜੇ (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, ਨਿਰਾ);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    ਜੇ (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			ਜੇ (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			ਰੋਕੋ;
		    }
		}

		ਸਤਰ_breakcheck();
	    }

	    ਜੇ (did_sub)
		++sub_nlines;
	    vim_free(new_ਸ਼ੁਰੂ.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	ਸਤਰ_breakcheck();
    }

    ਜੇ (ਪਹਿਲਾ_ਸਤਰ != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - old_ਸਤਰ_ਗਿਣਤੀ;
	ਬਦਲਿਆ_ਸਤਰਾਂ(ਪਹਿਲਾ_ਸਤਰ, 0, ਆਖਰੀ_ਸਤਰ - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    vim_free(ਪਾਠ_props);
#ਜੇ_ਅੰਤ

    // ":s/pat//n" doesn't move the cursor
    ਜੇ (subflags.do_ਗਿਣਤੀ)
	curwin->w_cursor = old_cursor;

    ਜੇ (sub_nsubs > ਸ਼ੁਰੂ_nsubs)
    {
	ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_ਸ਼ੁਰੂ.lnum = eap->line1;
	    curbuf->b_op_ਸਿਰਾ.lnum = line2;
	    curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;
	}

	ਜੇ (!global_busy)
	{
	    // when interactive leave cursor on the match
	    ਜੇ (!subflags.do_ask)
	    {
		ਜੇ (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		ਨਹੀਂਤਾਂ
		    beginline(BL_WHITE | BL_FIX);
	    }
	    ਜੇ (!do_sub_ਸੁਨੇਹਾ(subflags.do_ਗਿਣਤੀ) && subflags.do_ask)
		ਸੁਨੇਹਾ("");
	}
	ਨਹੀਂਤਾਂ
	    global_need_beginline = TRUE;
	ਜੇ (subflags.do_print)
	    print_ਸਤਰ(curwin->w_cursor.lnum,
					 subflags.do_ਸੰਖਿਆ, subflags.do_list);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!global_busy)
    {
	ਜੇ (got_int)		// interrupted
	    emsg(_(e_interrupted));
	ਨਹੀਂਤਾਂ ਜੇ (got_ਮੇਲ)	// did find something but nothing substituted
	    ਸੁਨੇਹਾ("");
	ਨਹੀਂਤਾਂ ਜੇ (subflags.do_error)	// nothing found
	    semsg(_(e_ਨਮੂਨਾ_not_ਮਿਲਿਆ_str), get_ਖੋਜ_pat());
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (subflags.do_ask && hasAnyFolding(curwin))
	// Cursor position may require updating
	ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
do_sub_ਸੁਨੇਹਾ(
    ਪੂਰਨ	    ਗਿਣਤੀ_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    ਜੇ (((sub_nsubs > p_report && (ਕੁੰਜੀ_Typed || sub_nlines > 1 || p_report < 1))
		|| ਗਿਣਤੀ_only)
	    && messaging())
    {
	ਅੱਖਰ	*ਸੁਨੇਹਾ_single;
	ਅੱਖਰ	*ਸੁਨੇਹਾ_plural;

	ਜੇ (got_int)
	    STRCPY(ਸੁਨੇਹਾ_buf, _("(Interrupted) "));
	ਨਹੀਂਤਾਂ
	    *ਸੁਨੇਹਾ_buf = NUL;

	ਸੁਨੇਹਾ_single = ਗਿਣਤੀ_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	ਸੁਨੇਹਾ_plural = ਗਿਣਤੀ_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_ਜੋੜੋ(ਸੁਨੇਹਾ_buf, ਆਕਾਰ(ਸੁਨੇਹਾ_buf),
				 NGETTEXT(ਸੁਨੇਹਾ_single, ਸੁਨੇਹਾ_plural, sub_nlines),
				 sub_nsubs, (ਲੰਮਾ)sub_nlines);

	ਜੇ (ਸੁਨੇਹਾ(ਸੁਨੇਹਾ_buf))
	    // save message to display it after redraw
	    ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)ਸੁਨੇਹਾ_buf, 0);
	ਵਾਪਸ TRUE;
    }
    ਜੇ (got_int)
    {
	emsg(_(e_interrupted));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ਖਾਨਾ = 0;
    ਜੇ (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", ਨਿਰਾ, ਨਿਰਾ, DOCMD_NOWAIT);
    ਨਹੀਂਤਾਂ
	do_cmdline(cmd, ਨਿਰਾ, ਨਿਰਾ, DOCMD_NOWAIT);
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
    ਖਾਲੀ
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    ਪੂਰਨ		ndone = 0;
    ਪੂਰਨ		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*ਵਰਤਿਆ_pat;
    regmmatch_T	regmatch;
    ਪੂਰਨ		ਮੇਲ;
    ਪੂਰਨ		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    ਜੇ (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	ਵਾਪਸ;
    }

    ਜੇ (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    ਨਹੀਂਤਾਂ
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_ਆਖਰੀ;	    // default: use last used regexp

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ਵਿੱਚ_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    ਜੇ (*cmd == '\\')
    {
	++cmd;
	ਜੇ (vim_strchr((char_u *)"/?&", *cmd) == ਨਿਰਾ)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    ਵਾਪਸ;
	}
	ਜੇ (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	ਨਹੀਂਤਾਂ
	    which_pat = RE_ਖੋਜ;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*cmd == NUL)
    {
	emsg(_(e_regular_ਵਿਅੰਜਕ_missing_from_global));
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (check_regexp_delim(*cmd) == FAIL)
    {
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    ਜੇ (ਖੋਜ_regcomp(pat, patlen, &ਵਰਤਿਆ_pat, RE_BOTH, which_pat, ਖੋਜ_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	ਵਾਪਸ;
    }

    ਜੇ (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	ਮੇਲ = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ਨਿਰਾ);
	ਜੇ ((type == 'g' && ਮੇਲ) || (type == 'v' && !ਮੇਲ))
	    global_exe_one(cmd, lnum);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	ਲਈ (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    ਮੇਲ = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ਨਿਰਾ);
	    ਜੇ (regmatch.regprog == ਨਿਰਾ)
		ਰੋਕੋ;  // re-compiling regprog failed
	    ਜੇ ((type == 'g' && ਮੇਲ) || (type == 'v' && !ਮੇਲ))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    ਸਤਰ_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	ਜੇ (got_int)
	    ਸੁਨੇਹਾ(_(e_interrupted));
	ਨਹੀਂਤਾਂ ਜੇ (ndone == 0)
	{
	    ਜੇ (type == 'v')
	    {
		ਜੇ (ਵਿੱਚ_vim9script())
		    semsg(_(e_ਨਮੂਨਾ_ਮਿਲਿਆ_ਵਿੱਚ_every_ਸਤਰ_str), ਵਰਤਿਆ_pat);
		ਨਹੀਂਤਾਂ
		    smsg(_("Pattern found in every line: %s"), ਵਰਤਿਆ_pat);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਵਿੱਚ_vim9script())
		    semsg(_(e_ਨਮੂਨਾ_not_ਮਿਲਿਆ_str), ਵਰਤਿਆ_pat);
		ਨਹੀਂਤਾਂ
		    smsg(_("Pattern not found: %s"), ਵਰਤਿਆ_pat);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	    inc_clip_provider();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	    ਸ਼ੁਰੂ_global_changes();
#ਜੇ_ਅੰਤ
	    global_exe(cmd);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	    ਸਿਰਾ_global_changes();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	    dec_clip_provider();
#ਜੇ_ਅੰਤ
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Execute "cmd" on lines marked with ml_setmarked().
 */
    ਖਾਲੀ
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
    ਸੁਨੇਹਾ_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜਦੋਂ (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    ਜੇ (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    ਨਹੀਂਤਾਂ
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    ਬਦਲਿਆ_ਸਤਰ_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ == 0 && ਸੁਨੇਹਾ_scrolled == 0)
	ਸੁਨੇਹਾ_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    ਜੇ (!do_sub_ਸੁਨੇਹਾ(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - old_lcount);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(ਖਾਲੀ)
{
    ਵਾਪਸ old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    ਖਾਲੀ
ਸੈੱਟ_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#ਜੇ_ਅੰਤ // FEAT_VIMINFO

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_old_sub(ਖਾਲੀ)
{
    vim_free(old_sub);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    ਪੂਰਨ
prepare_tagpreview(
    ਪੂਰਨ		undo_sync,	    // sync undo when leaving the window
    ਪੂਰਨ		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    ਜੇ (curwin->w_p_pvw)
	ਵਾਪਸ FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    ਜੇ (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_ਲੱਭੋ_preview_ਖਿੜਕੀ();
	ਜੇ (wp != ਨਿਰਾ)
	    popup_ਸੈੱਟ_wantpos_cursor(wp, wp->w_minwidth, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (use_popup != USEPOPUP_NONE)
    {
	wp = popup_ਲੱਭੋ_info_ਖਿੜਕੀ();
	ਜੇ (wp != ਨਿਰਾ)
	{
	    ਜੇ (use_popup == USEPOPUP_ਸਾਧਾਰਨ)
		popup_show(wp);
	    ਨਹੀਂਤਾਂ
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_p_pvw)
		ਰੋਕੋ;
    }
    ਜੇ (wp != ਨਿਰਾ)
    {
	win_enter(wp, undo_sync);
	ਵਾਪਸ FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    ਜੇ ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	ਵਾਪਸ popup_create_preview_ਖਿੜਕੀ(use_popup != USEPOPUP_NONE);
# endif
    ਜੇ (win_ਵੰਡੋ(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	ਵਾਪਸ FALSE;
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
    ਵਾਪਸ TRUE;
}

#ਜੇ_ਅੰਤ

/*
 * Make the user happy.
 */
    ਖਾਲੀ
ex_smile(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ ਅੱਖਰ *ਕੋਡ[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    ਅੱਖਰ *p;
    ਪੂਰਨ n;
    ਪੂਰਨ i;

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_putchar('\n');
    ਲਈ (i = 0; i < 2; ++i)
	ਲਈ (p = ਕੋਡ[i]; *p != NUL; ++p)
	    ਜੇ (*p == 'x')
		ਸੁਨੇਹਾ_putchar('\n');
	    ਨਹੀਂਤਾਂ
		ਲਈ (n = *p++; n > 0; --n)
		    ਜੇ (*p == 'o' || *p == '$')
			ਸੁਨੇਹਾ_putchar_attr(*p, HL_ATTR(HLF_L));
		    ਨਹੀਂਤਾਂ
			ਸੁਨੇਹਾ_putchar(*p);
    ਸੁਨੇਹਾ_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    ਖਾਲੀ
ex_drop(exarg_T *eap)
{
    ਪੂਰਨ		ਵੰਡੋ = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    ਜੇ (ERROR_IF_POPUP_ਖਿੜਕੀ || ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    /*
     * Check if the first argument is already being edited in a window.  If
     * so, jump to that window.
     * We would actually need to check all arguments, but that's complicated
     * and mostly only one file is dropped.
     * This also ignores wildcards, since it is very unlikely the user is
     * editing a file name with a wildcard character.
     */
    ਸੈੱਟ_arglist(eap->arg);

    /*
     * Expanding wildcards may result in an empty argument list.  E.g. when
     * editing "foo.pyc" and ".pyc" is in 'wildignore'.  Assume that we
     * already did an error message for this.
     */
    ਜੇ (ARGCOUNT == 0)
	ਵਾਪਸ;

    ਜੇ (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	ਵਾਪਸ;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ਜੇ (wp->w_ਬਫਰ == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    ਜੇ (!buf_Is_ਬਦਲਿਆ(curbuf))
	    {
		ਪੂਰਨ save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
		ex_rewind(eap);
	    ਵਾਪਸ;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    ਜੇ (!buf_hide(curbuf))
    {
	++emsg_off;
	ਵੰਡੋ = check_ਬਦਲਿਆ(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    ਜੇ (ਵੰਡੋ)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    ਨਹੀਂਤਾਂ
	eap->cmdidx = CMD_ਪਹਿਲਾ;
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
skip_vimgrep_pat(char_u *p, char_u **s, ਪੂਰਨ *flags)
{
    ਵਾਪਸ skip_vimgrep_pat_ext(p, s, flags, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, ਪੂਰਨ *flags, char_u **nulp, ਪੂਰਨ *cp)
{
    ਪੂਰਨ		c;

    ਜੇ (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	ਜੇ (s != ਨਿਰਾ)
	    *s = p;
	p = skiptowhite(p);
	ਜੇ (s != ਨਿਰਾ && *p != NUL)
	{
	    ਜੇ (nulp != ਨਿਰਾ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// ":vimgrep /pattern/[g][j] fname"
	ਜੇ (s != ਨਿਰਾ)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	ਜੇ (*p != c)
	    ਵਾਪਸ ਨਿਰਾ;

	// Truncate the pattern.
	ਜੇ (s != ਨਿਰਾ)
	{
	    ਜੇ (nulp != ਨਿਰਾ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	ਜਦੋਂ (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    ਜੇ (flags != ਨਿਰਾ)
	    {
		ਜੇ (*p == 'g')
		    *flags |= VGR_GLOBAL;
		ਨਹੀਂਤਾਂ ਜੇ (*p == 'j')
		    *flags |= VGR_NOJUMP;
		ਨਹੀਂਤਾਂ
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    ਵਾਪਸ p;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    ਖਾਲੀ
ex_oldfiles(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    ਪੂਰਨ		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    ਪੂਰਨ		matches = -1;

    ਜੇ (l == ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ(_("No old files"));
	ਵਾਪਸ;
    }

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_scroll = TRUE;
    ਲਈ (li = l->lv_ਪਹਿਲਾ; li != ਨਿਰਾ && !got_int; li = li->li_ਅਗਲਾ)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	ਜੇ (!ਸੁਨੇਹਾ_filtered(fname))
	{
	    ਜੇ (matches < 0)
		matches = nr;
	    ਨਹੀਂਤਾਂ
		matches = 0;
	    ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)nr);
	    ਸੁਨੇਹਾ_puts(": ");
	    ਸੁਨੇਹਾ_outtrans(fname);
	    ਸੁਨੇਹਾ_clr_eos();
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਬਾਹਰਲਾ_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_ਹੋਰ = FALSE;
	// we only need to prompt if there is more than 1 match
	ਜੇ (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    nr = prompt_for_ਸੰਖਿਆ(FALSE);
	ਸੁਨੇਹਾ_starthere();
	ਜੇ (nr > 0)
	{
	    char_u *p = list_ਲੱਭੋ_str(get_vim_var_list(VV_OLDFILES),
		    (ਲੰਮਾ)nr);

	    ਜੇ (p != ਨਿਰਾ)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, ਨਿਰਾ);
		vim_free(p);
	    }
	}
    }
# endif
}
#ਜੇ_ਅੰਤ
