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

#شامل "vim.h"
#شامل "version.h"

#شامل <float.h>

ساکن صحیح linelen(صحیح *has_tab);
ساکن خالی do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, صحیح do_اندر, صحیح do_برآمد);
ساکن صحیح not_writing(خالی);
ساکن صحیح check_readonly(صحیح *forceit, buf_T *buf);
ساکن خالی delbuf_پیغام(char_u *name);

/*
 * ":ascii" and "ga".
 */
    خالی
do_ascii(exarg_T *eap غیر_مستعمل)
{
    صحیح		c;
    صحیح		cval;
    حرف	buf1[20];
    حرف	buf2[20];
    char_u	buf3[7];
#اگر_تعریف_شدہ FEAT_DIGRAPHS
    char_u      *dig;
#اگر_ختم
    صحیح		cc[MAX_MCO];
    صحیح		ci = 0;
    صحیح		len;

    اگر (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    ورنہ
	c = gchar_cursor();
    اگر (c == NUL)
    {
	پیغام("NUL");
	واپس;
    }

    IObuff[0] = NUL;
    اگر (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	اگر (c == NL)	    // NUL is stored as NL
	    c = NUL;
	اگر (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	ورنہ
	    cval = c;
	اگر (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, حجم(buf1), "  <%s>", (حرف *)buf3);
	}
	ورنہ
	    buf1[0] = NUL;
	اگر (c >= 0x80)
	    vim_snprintf(buf2, حجم(buf2), "  <M-%s>",
						 (حرف *)transchar(c & 0x7f));
	ورنہ
	    buf2[0] = NUL;
#اگر_تعریف_شدہ FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	اگر (dig != تہی)
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	ورنہ
#اگر_ختم
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	اگر (enc_utf8)
	    c = cc[ci++];
	ورنہ
	    c = 0;
    }

    // Repeat for combining characters.
    جب (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (صحیح)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	اگر (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	اگر (enc_utf8 && utf_iscomposing(c)
#اگر_تعریف_شدہ USE_GUI
		&& !gui.اندر_use
#اگر_ختم
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#اگر_تعریف_شدہ FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	اگر (dig != تہی)
	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	ورنہ
#اگر_ختم
	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	اگر (ci == MAX_MCO)
	    توڑو;
	اگر (enc_utf8)
	    c = cc[ci++];
	ورنہ
	    c = 0;
    }

    پیغام((حرف *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    خالی
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    صحیح		len;
    صحیح		indent = 0;
    صحیح		new_indent;
    صحیح		has_tab;
    صحیح		چوڑائی;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl)
    {
	// switch left and right aligning
	اگر (eap->cmdidx == CMD_دایاں)
	    eap->cmdidx = CMD_بایاں;
	ورنہ اگر (eap->cmdidx == CMD_بایاں)
	    eap->cmdidx = CMD_دایاں;
    }
#اگر_ختم

    چوڑائی = atoi((حرف *)eap->arg);
    save_curpos = curwin->w_cursor;
    اگر (eap->cmdidx == CMD_بایاں)    // width is used for new indent
    {
	اگر (چوڑائی >= 0)
	    indent = چوڑائی;
    }
    ورنہ
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	اگر (چوڑائی <= 0)
	    چوڑائی = curbuf->b_p_tw;
	اگر (چوڑائی == 0 && curbuf->b_p_wm > 0)
	    چوڑائی = curwin->w_چوڑائی - curbuf->b_p_wm;
	اگر (چوڑائی <= 0)
	    چوڑائی = 80;
    }

    اگر (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	واپس;

    برائے (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	اگر (eap->cmdidx == CMD_بایاں)		// left align
	    new_indent = indent;
	ورنہ
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_دایاں ? &has_tab
						   : تہی) - get_indent();

	    اگر (len <= 0)			// skip blank lines
		جاری;

	    اگر (eap->cmdidx == CMD_center)
		new_indent = (چوڑائی - len) / 2;
	    ورنہ
	    {
		new_indent = چوڑائی - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		اگر (has_tab)
		    جب (new_indent > 0)
		    {
			(خالی)سیٹ_indent(new_indent, 0);
			اگر (linelen(تہی) <= چوڑائی)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    کرو
				(خالی)سیٹ_indent(++new_indent, 0);
			    جب (linelen(تہی) <= چوڑائی);
			    --new_indent;
			    توڑو;
			}
			--new_indent;
		    }
	    }
	}
	اگر (new_indent < 0)
	    new_indent = 0;
	(خالی)سیٹ_indent(new_indent, 0);		// set indent
    }
    بدلا_سطریں(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    ساکن صحیح
linelen(صحیح *has_tab)
{
    char_u  *سطر;
    char_u  *پہلا;
    char_u  *آخری;
    صحیح	    save;
    صحیح	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    سطر = ml_get_curline();
    اگر (*سطر == NUL)
	واپس 0;

    // find the first non-blank character
    پہلا = skipwhite(سطر);

    // find the character after the last non-blank character
    برائے (آخری = پہلا + STRLEN(پہلا);
				آخری > پہلا && VIM_ISWHITE(آخری[-1]); --آخری)
	;
    save = *آخری;
    *آخری = NUL;
    len = linetabsize_str(سطر);	// get line length on screen
    اگر (has_tab != تہی)		// check for embedded TAB
	*has_tab = (vim_strchr(پہلا, TAB) != تہی);
    *آخری = save;

    واپس len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
ساکن char_u	*sortbuf1;
ساکن char_u	*sortbuf2;

ساکن صحیح	ترتیب_دو_lc;	// sort using locale
ساکن صحیح	ترتیب_دو_ic;	// ignore case
ساکن صحیح	ترتیب_دو_nr;	// sort on number
ساکن صحیح	ترتیب_دو_rx;	// sort on regex instead of skipping it
ساکن صحیح	ترتیب_دو_flt;	// sort on floating number

ساکن صحیح	ترتیب_دو_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
قسمتعریف ساخت
{
    linenr_T	lnum;			// line number
    اتحاد {
	ساخت
	{
	    varnumber_T	آغاز_ستون_nr;	// starting column number
	    varnumber_T	انتہا_ستون_nr;	// ending column number
	} سطر;
	ساخت
	{
	    varnumber_T	قدر;		// value if sorting by integer
	    صحیح is_نمبر;		// TRUE when line contains a number
	} عدد;
	float_T قدر_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    ساکن صحیح
string_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    اگر (ترتیب_دو_lc)
	واپس strcoll((حرف *)s1, (حرف *)s2);
    واپس ترتیب_دو_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    ساکن صحیح
ترتیب_دو_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    صحیح		نتیجہ = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    اگر (ترتیب_دو_abort)
	واپس 0;
    fast_breakcheck();
    اگر (got_int)
	ترتیب_دو_abort = TRUE;

    اگر (ترتیب_دو_nr)
    {
	اگر (l1.st_u.عدد.is_نمبر != l2.st_u.عدد.is_نمبر)
	    نتیجہ = l1.st_u.عدد.is_نمبر > l2.st_u.عدد.is_نمبر ? 1 : -1;
	ورنہ
	    نتیجہ = l1.st_u.عدد.قدر == l2.st_u.عدد.قدر ? 0
			     : l1.st_u.عدد.قدر > l2.st_u.عدد.قدر ? 1 : -1;
    }
    ورنہ اگر (ترتیب_دو_flt)
	نتیجہ = l1.st_u.قدر_flt == l2.st_u.قدر_flt ? 0
			     : l1.st_u.قدر_flt > l2.st_u.قدر_flt ? 1 : -1;
    ورنہ
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.سطر.آغاز_ستون_nr,
		     l1.st_u.سطر.انتہا_ستون_nr - l1.st_u.سطر.آغاز_ستون_nr + 1);
	sortbuf1[l1.st_u.سطر.انتہا_ستون_nr - l1.st_u.سطر.آغاز_ستون_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.سطر.آغاز_ستون_nr,
		     l2.st_u.سطر.انتہا_ستون_nr - l2.st_u.سطر.آغاز_ستون_nr + 1);
	sortbuf2[l2.st_u.سطر.انتہا_ستون_nr - l2.st_u.سطر.آغاز_ستون_nr] = 0;

	نتیجہ = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    اگر (نتیجہ == 0)
	واپس (صحیح)(l1.lnum - l2.lnum);
    واپس نتیجہ;
}

/*
 * ":sort".
 */
    خالی
ex_ترتیب_دو(exarg_T *eap)
{
    regmatch_T	regmatch;
    صحیح		len;
    linenr_T	lnum;
    طویل	maxlen = 0;
    sorti_T	*nrs;
    size_t	گنتی = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    صحیح		unique = FALSE;
    طویل	deleted;
    colnr_T	آغاز_ستون;
    colnr_T	انتہا_ستون;
    صحیح		ترتیب_دو_what = 0;
    صحیح		format_ملا = 0;
    صحیح		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    اگر (گنتی <= 1)
	واپس;

    اگر (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	واپس;
    sortbuf1 = تہی;
    sortbuf2 = تہی;
    regmatch.regprog = تہی;
    nrs = ALLOC_MULT(sorti_T, گنتی);
    اگر (nrs == تہی)
	جاؤ sortend;

    ترتیب_دو_abort = ترتیب_دو_ic = ترتیب_دو_lc = ترتیب_دو_rx = ترتیب_دو_nr = 0;
    ترتیب_دو_flt = 0;

    برائے (p = eap->arg; *p != NUL; ++p)
    {
	اگر (VIM_ISWHITE(*p))
	    ;
	ورنہ اگر (*p == 'i')
	    ترتیب_دو_ic = TRUE;
	ورنہ اگر (*p == 'l')
	    ترتیب_دو_lc = TRUE;
	ورنہ اگر (*p == 'r')
	    ترتیب_دو_rx = TRUE;
	ورنہ اگر (*p == 'n')
	{
	    ترتیب_دو_nr = 1;
	    ++format_ملا;
	}
	ورنہ اگر (*p == 'f')
	{
	    ترتیب_دو_flt = 1;
	    ++format_ملا;
	}
	ورنہ اگر (*p == 'b')
	{
	    ترتیب_دو_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_ملا;
	}
	ورنہ اگر (*p == 'o')
	{
	    ترتیب_دو_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_ملا;
	}
	ورنہ اگر (*p == 'x')
	{
	    ترتیب_دو_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_ملا;
	}
	ورنہ اگر (*p == 'u')
	    unique = TRUE;
	ورنہ اگر (*p == '"')	// comment start
	    توڑو;
	ورنہ اگر (eap->nextcmd == تہی && check_nextcmd(p) != تہی)
	{
	    eap->nextcmd = check_nextcmd(p);
	    توڑو;
	}
	ورنہ اگر (!ASCII_ISALPHA(*p) && regmatch.regprog == تہی)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    اگر (s == تہی)
		جاؤ sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    اگر (s == p + 1)
	    {
		اگر (آخری_تلاش_pat() == تہی)
		{
		    emsg(_(e_no_پچھلا_regular_عبارت));
		    جاؤ sortend;
		}
		regmatch.regprog = vim_regcomp(آخری_تلاش_pat(), RE_MAGIC);
	    }
	    ورنہ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    اگر (regmatch.regprog == تہی)
		جاؤ sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ورنہ
	{
	    semsg(_(e_invalid_دلیل_str), p);
	    جاؤ sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    اگر (format_ملا > 1)
    {
	emsg(_(e_invalid_دلیل));
	جاؤ sortend;
    }

    // From here on "sort_nr" is used as a flag for any integer number
    // sorting.
    ترتیب_دو_nr += ترتیب_دو_what;

    /*
     * Make an array with all line numbers.  This avoids having to copy all
     * the lines into allocated memory.
     * When sorting on strings "start_col_nr" is the offset in the line, for
     * numbers sorting it's the number to sort on.  This means the pattern
     * matching and number conversion only has to be done once per line.
     * Also get the longest line length for allocating "sortbuf".
     */
    برائے (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	اگر (maxlen < len)
	    maxlen = len;

	آغاز_ستون = 0;
	انتہا_ستون = len;
	اگر (regmatch.regprog != تہی && vim_regexec(&regmatch, s, 0))
	{
	    اگر (ترتیب_دو_rx)
	    {
		آغاز_ستون = (colnr_T)(regmatch.startp[0] - s);
		انتہا_ستون = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ورنہ
		آغاز_ستون = (colnr_T)(regmatch.endp[0] - s);
	}
	ورنہ
	    اگر (regmatch.regprog != تہی)
		انتہا_ستون = 0;

	اگر (ترتیب_دو_nr || ترتیب_دو_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + انتہا_ستون;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + آغاز_ستون;
	    اگر (ترتیب_دو_nr)
	    {
		اگر (ترتیب_دو_what & STR2NR_HEX)
		    s = skiptohex(p);
		ورنہ اگر (ترتیب_دو_what & STR2NR_BIN)
		    s = skiptobin(p);
		ورنہ
		    s = skiptodigit(p);
		اگر (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		اگر (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.عدد.is_نمبر = FALSE;
		    nrs[lnum - eap->line1].st_u.عدد.قدر = 0;
		}
		ورنہ
		{
		    nrs[lnum - eap->line1].st_u.عدد.is_نمبر = TRUE;
		    vim_str2nr(s, تہی, تہی, ترتیب_دو_what,
			&nrs[lnum - eap->line1].st_u.عدد.قدر,
			تہی, 0, FALSE, تہی);
		}
	    }
	    ورنہ
	    {
		s = skipwhite(p);
		اگر (*s == '+')
		    s = skipwhite(s + 1);

		اگر (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.قدر_flt = -DBL_MAX;
		ورنہ
		    nrs[lnum - eap->line1].st_u.قدر_flt =
						      strtod((حرف *)s, تہی);
	    }
	    *s2 = c;
	}
	ورنہ
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.سطر.آغاز_ستون_nr = آغاز_ستون;
	    nrs[lnum - eap->line1].st_u.سطر.انتہا_ستون_nr = انتہا_ستون;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	اگر (regmatch.regprog != تہی)
	    fast_breakcheck();
	اگر (got_int)
	    جاؤ sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    اگر (sortbuf1 == تہی)
	جاؤ sortend;
    sortbuf2 = alloc(maxlen + 1);
    اگر (sortbuf2 == تہی)
	جاؤ sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((خالی *)nrs, گنتی, حجم(sorti_T), ترتیب_دو_compare);

    اگر (ترتیب_دو_abort)
	جاؤ sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    برائے (i = 0; i < گنتی; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? گنتی - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	اگر (get_lnum + ((linenr_T)گنتی - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	اگر (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    اگر (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		توڑو;
	}
	fast_breakcheck();
	اگر (got_int)
	    جاؤ sortend;
    }

    // delete the original lines if appending worked
    اگر (i == گنتی)
	برائے (i = 0; i < گنتی; ++i)
	    ml_delete(eap->line1);
    ورنہ
	گنتی = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (طویل)(گنتی - (lnum - eap->line2));
    اگر (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (طویل)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    ورنہ اگر (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    اگر (change_occurred || deleted != 0)
	بدلا_سطریں(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    اگر (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    خالی
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    صحیح		len;
    linenr_T	lnum;
    طویل	maxlen = 0;
    linenr_T	گنتی = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    صحیح		keep_only_unique = FALSE;
    صحیح		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    طویل	deleted = 0;
    colnr_T	آغاز_ستون;
    colnr_T	انتہا_ستون;
    صحیح		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    اگر (گنتی <= 1)
	واپس;

    اگر (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	واپس;
    sortbuf1 = تہی;
    regmatch.regprog = تہی;

    ترتیب_دو_abort = ترتیب_دو_ic = ترتیب_دو_lc = ترتیب_دو_rx = ترتیب_دو_nr = 0;
    ترتیب_دو_flt = 0;

    برائے (p = eap->arg; *p != NUL; ++p)
    {
	اگر (VIM_ISWHITE(*p))
	    ;
	ورنہ اگر (*p == 'i')
	    ترتیب_دو_ic = TRUE;
	ورنہ اگر (*p == 'l')
	    ترتیب_دو_lc = TRUE;
	ورنہ اگر (*p == 'r')
	    ترتیب_دو_rx = TRUE;
	ورنہ اگر (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    اگر (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	ورنہ اگر (*p == '"')	// comment start
	    توڑو;
	ورنہ اگر (eap->nextcmd == تہی && check_nextcmd(p) != تہی)
	{
	    eap->nextcmd = check_nextcmd(p);
	    توڑو;
	}
	ورنہ اگر (!ASCII_ISALPHA(*p) && regmatch.regprog == تہی)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    اگر (s == تہی)
		جاؤ uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    اگر (s == p + 1)
	    {
		اگر (آخری_تلاش_pat() == تہی)
		{
		    emsg(_(e_no_پچھلا_regular_عبارت));
		    جاؤ uniqend;
		}
		regmatch.regprog = vim_regcomp(آخری_تلاش_pat(), RE_MAGIC);
	    }
	    ورنہ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    اگر (regmatch.regprog == تہی)
		جاؤ uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ورنہ
	{
	    semsg(_(e_invalid_دلیل_str), p);
	    جاؤ uniqend;
	}
    }

    // Find the length of the longest line.
    برائے (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	اگر (maxlen < len)
	    maxlen = len;

	اگر (got_int)
	    جاؤ uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    اگر (sortbuf1 == تہی)
	جاؤ uniqend;

    // Delete lines according to options.
    صحیح مطابقت_continue = FALSE;
    صحیح اگلا_is_unmatch = FALSE;
    صحیح is_مطابقت;
    linenr_T مکمل_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    برائے (linenr_T i = 0; i < گنتی; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	آغاز_ستون = 0;
	انتہا_ستون = len;
	اگر (regmatch.regprog != تہی && vim_regexec(&regmatch, s, 0))
	{
	    اگر (ترتیب_دو_rx)
	    {
		آغاز_ستون = (colnr_T)(regmatch.startp[0] - s);
		انتہا_ستون = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ورنہ
		آغاز_ستون = (colnr_T)(regmatch.endp[0] - s);
	}
	ورنہ
	    اگر (regmatch.regprog != تہی)
		انتہا_ستون = 0;
	اگر (انتہا_ستون > 0)
	{
	    save_c = s[انتہا_ستون];
	    s[انتہا_ستون] = NUL;
	}

	is_مطابقت = i > 0 ? !string_compare(&s[آغاز_ستون], sortbuf1) : FALSE;
	delete_lnum = 0;
	اگر (اگلا_is_unmatch)
	{
	    is_مطابقت = FALSE;
	    اگلا_is_unmatch = FALSE;
	}

	اگر (!keep_only_unique && !keep_only_not_unique)
	{
	    اگر (is_مطابقت)
		delete_lnum = get_lnum;
	    ورنہ
		STRCPY(sortbuf1, &s[آغاز_ستون]);
	}
	ورنہ اگر (keep_only_not_unique)
	{
	    اگر (is_مطابقت)
	    {
		مکمل_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		مطابقت_continue = TRUE;
	    }
	    ورنہ
	    {
		اگر (i > 0 && !مطابقت_continue && get_lnum - 1 > مکمل_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    اگلا_is_unmatch = TRUE;
		}
		ورنہ اگر (i >= گنتی - 1)
		    delete_lnum = get_lnum;
		مطابقت_continue = FALSE;
		STRCPY(sortbuf1, &s[آغاز_ستون]);
	    }
	}
	ورنہ // keep_only_unique
	{
	    اگر (is_مطابقت)
	    {
		اگر (!مطابقت_continue)
		    delete_lnum = get_lnum - 1;
		ورنہ
		    delete_lnum = get_lnum;
		مطابقت_continue = TRUE;
	    }
	    ورنہ
	    {
		اگر (i == 0 && مطابقت_continue)
		    delete_lnum = get_lnum;
		مطابقت_continue = FALSE;
		STRCPY(sortbuf1, &s[آغاز_ستون]);
	    }
	}

	اگر (انتہا_ستون > 0)
	    s[انتہا_ستون] = save_c;

	اگر (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    گنتی--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	اگر (got_int)
	    جاؤ uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (طویل)MAXLNUM, -deleted);
    msgmore(-deleted);

    اگر (change_occurred)
	بدلا_سطریں(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    اگر (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	عدد_سطریں;  // Num lines moved
    linenr_T	آخری_سطر;  // Last line in file after adding new text
#اگر_تعریف_شدہ FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#اگر_ختم

    اگر (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_کا_سطریں_into_itself));
	واپس FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    اگر (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	اگر (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	ورنہ
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	واپس OK;
    }

    عدد_سطریں = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    اگر (u_save(dest, dest + 1) == FAIL)
	واپس FAIL;
    برائے (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	اگر (str != تہی)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    اگر (dest < line1)
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
    آخری_سطر = curbuf->b_ml.ml_سطر_گنتی;
    mark_adjust_nofold(line1, line2, آخری_سطر - line2, 0L);
    اگر (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -عدد_سطریں, 0L);
#اگر_تعریف_شدہ FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    اگر (win->w_بفر == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#اگر_ختم
	اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_آغاز.lnum = dest - عدد_سطریں + 1;
	    curbuf->b_op_انتہا.lnum = dest;
	}
    }
    ورنہ
    {
	mark_adjust_nofold(dest + 1, line1 - 1, عدد_سطریں, 0L);
#اگر_تعریف_شدہ FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    اگر (win->w_بفر == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#اگر_ختم
	اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_آغاز.lnum = dest + 1;
	    curbuf->b_op_انتہا.lnum = dest + عدد_سطریں;
	}
    }
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_آغاز.ستون = curbuf->b_op_انتہا.ستون = 0;
    mark_adjust_nofold(آخری_سطر - عدد_سطریں + 1, آخری_سطر,
					     -(آخری_سطر - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    اگر (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	واپس FAIL;

    برائے (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_پیغام);

    اگر (!global_busy && عدد_سطریں > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", عدد_سطریں),
			(طویل)عدد_سطریں);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    اگر (dest >= line1)
	curwin->w_cursor.lnum = dest;
    ورنہ
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    اگر (line1 < dest)
    {
	dest += عدد_سطریں + 1;
	آخری_سطر = curbuf->b_ml.ml_سطر_گنتی;
	اگر (dest > آخری_سطر + 1)
	    dest = آخری_سطر + 1;
	بدلا_سطریں(line1, 0, dest, 0L);
    }
    ورنہ
	بدلا_سطریں(dest + 1, 0, line1 + عدد_سطریں, 0L);

    واپس OK;
}

/*
 * ":copy"
 */
    خالی
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	گنتی;
    char_u	*p;

    گنتی = line2 - line1 + 1;
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_آغاز.lnum = n + 1;
	curbuf->b_op_انتہا.lnum = n + گنتی;
	curbuf->b_op_آغاز.ستون = curbuf->b_op_انتہا.ستون = 0;
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
    اگر (u_save(n, n + 1) == FAIL)
	واپس;

    curwin->w_cursor.lnum = n;
    جب (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	اگر (p != تہی)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	اگر (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	اگر (curwin->w_cursor.lnum < line1)
	    ++line1;
	اگر (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_سطریں_mark(n, گنتی);
    اگر (VIsual_فعال)
	check_pos(curbuf, &VIsual);

    msgmore((طویل)گنتی);
}

ساکن char_u	*prevcmd = تہی;	// the previous command

#اگر defined(EXITFREE)
    خالی
free_پچھلا_shellcmd(خالی)
{
    vim_free(prevcmd);
}
#اگر_ختم

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    ساکن صحیح
prevcmd_is_سیٹ(خالی)
{
    اگر (prevcmd == تہی)
    {
	emsg(_(e_no_پچھلا_command));
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    خالی
do_bang(
    صحیح		addr_گنتی,
    exarg_T	*eap,
    صحیح		forceit,
    صحیح		do_اندر,
    صحیح		do_برآمد)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = تہی;		// the new command
    صحیح			free_newcmd = FALSE;    // need to free() newcmd
    صحیح			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    صحیح			len;
    صحیح			scroll_save = پیغام_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (addr_گنتی == 0)		// :!
    {
	پیغام_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	پیغام_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    کرو
    {
	len = (صحیح)STRLEN(trailarg) + 1;
	اگر (newcmd != تہی)
	    len += (صحیح)STRLEN(newcmd);
	اگر (ins_prevcmd)
	{
	    اگر (!prevcmd_is_سیٹ())
	    {
		vim_free(newcmd);
		واپس;
	    }
	    len += (صحیح)STRLEN(prevcmd);
	}
	اگر ((t = alloc(len)) == تہی)
	{
	    vim_free(newcmd);
	    واپس;
	}
	*t = NUL;
	اگر (newcmd != تہی)
	    STRCAT(t, newcmd);
	اگر (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = تہی;
	جب (*p)
	{
	    اگر (*p == '!')
	    {
		اگر (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		ورنہ
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    توڑو;
		}
	    }
	    ++p;
	}
    } جب (trailarg != تہی);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    اگر (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    ورنہ
	free_newcmd = TRUE;

    اگر (bangredo)	    // put cmd in redo buffer for ! command
    {
	اگر (!prevcmd_is_سیٹ())
	    جاؤ theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	اگر (cmd != تہی)
	{
	    Append_مقصد_Redobuff_Lit(cmd, -1);
	    vim_free(cmd);
	}
	ورنہ
	    Append_مقصد_Redobuff_Lit(prevcmd, -1);
	Append_مقصد_Redobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    اگر (*p_shq != NUL)
    {
	اگر (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	اگر (newcmd == تہی)
	    واپس;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    اگر (addr_گنتی == 0)		// :!
    {
	// echo the command
	پیغام_آغاز();
	پیغام_putchar(':');
	پیغام_putchar('!');
	پیغام_outtrans(newcmd);
	پیغام_clr_eos();
	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);

	do_shell(newcmd, 0);
    }
    ورنہ				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_اندر, do_برآمد);
	apply_autocmds(EVENT_SHELLFILTERPOST, تہی, تہی, FALSE, curbuf);
    }

theend:
    اگر (free_newcmd)
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
    ساکن خالی
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    صحیح		do_اندر,
    صحیح		do_برآمد)
{
    char_u	*itmp = تہی;
    char_u	*otmp = تہی;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    صحیح		shell_flags = 0;
    pos_T	orig_آغاز = curbuf->b_op_آغاز;
    pos_T	orig_انتہا = curbuf->b_op_انتہا;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
#اگر_تعریف_شدہ FEAT_FILTERPIPE
    صحیح		stmp = p_stmp;
#اگر_ختم

    اگر (*cmd == NUL)	    // no filter command
	واپس;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.ستون = 0;
    بدلا_سطر_abv_curs();
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

    اگر (do_برآمد)
	shell_flags |= SHELL_DOOUT;

#اگر_تعریف_شدہ FEAT_FILTERPIPE
# ifdef VIMDLL
    اگر (!gui.اندر_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    اگر (!do_اندر && do_برآمد && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    ورنہ اگر (do_اندر && !do_برآمد && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_آغاز.lnum = line1;
	curbuf->b_op_انتہا.lnum = line2;
    }
    ورنہ اگر (do_اندر && do_برآمد && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_آغاز.lnum = line1;
	curbuf->b_op_انتہا.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    ورنہ
#اگر_ختم
	اگر ((do_اندر && (itmp = vim_tempname('i', FALSE)) == تہی)
		|| (do_برآمد && (otmp = vim_tempname('o', FALSE)) == تہی))
	{
	    emsg(_(e_cant_get_عارضی_file_name));
	    جاؤ filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    اگر (itmp != تہی && buf_write(curbuf, itmp, تہی, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	پیغام_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#اگر defined(FEAT_EVAL)
	اگر (!aborting())
#اگر_ختم
	    // will call wait_return()
	    (خالی)semsg(_(e_cant_create_file_str), itmp);
	جاؤ filterend;
    }
    اگر (curbuf != old_curbuf)
	جاؤ filterend;

    اگر (!do_برآمد)
	پیغام_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    اگر (cmd_buf == تہی)
	جاؤ filterend;

    windgoto((صحیح)صفیں - 1, cmdline_ستون_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    اگر (!do_برآمد || STRCMP(p_srr, ">") == 0 || !do_اندر)
	redraw_later_clear();

    اگر (do_برآمد)
    {
	اگر (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    جاؤ error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_سطر_گنتی;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    اگر (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    اگر (do_برآمد)
    {
	اگر (otmp != تہی)
	{
	    اگر (readfile(otmp, تہی, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#اگر defined(FEAT_EVAL)
		اگر (!aborting())
#اگر_ختم
		{
		    پیغام_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		جاؤ error;
	    }
	    اگر (curbuf != old_curbuf)
		جاؤ filterend;
	}

	read_linecount = curbuf->b_ml.ml_سطر_گنتی - read_linecount;

	اگر (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_آغاز.lnum = line2 + 1;
	    curbuf->b_op_انتہا.lnum = curwin->w_cursor.lnum;
	    appended_سطریں_mark(line2, read_linecount);
	}

	اگر (do_اندر)
	{
	    اگر ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == تہی)
	    {
		اگر (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		ورنہ اگر (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		ورنہ
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
	    del_سطریں(linecount, TRUE);
	    اگر (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_آغاز.lnum = curbuf->b_op_انتہا.lnum =
					MIN(line1, curbuf->b_ml.ml_سطر_گنتی);
		curbuf->b_op_آغاز.ستون = curbuf->b_op_انتہا.ستون = 0;
	    }
	    ورنہ
	    {
		curbuf->b_op_آغاز.lnum -= linecount;	// adjust '[
		curbuf->b_op_انتہا.lnum -= linecount;	// adjust ']
	    }
	    write_lnum_adjust(-linecount);		// adjust last line
							// for next write
#اگر_تعریف_شدہ FEAT_FOLDING
	    foldUpdate(curwin, curbuf->b_op_آغاز.lnum, curbuf->b_op_انتہا.lnum);
#اگر_ختم
	}
	ورنہ
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_انتہا.lnum - curbuf->b_op_آغاز.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_انتہا.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	اگر (linecount > p_report)
	{
	    اگر (do_اندر)
	    {
		vim_snprintf(پیغام_buf, حجم(پیغام_buf),
				    _("%ld lines filtered"), (طویل)linecount);
		اگر (پیغام(پیغام_buf) && !پیغام_scroll)
		    // save message to display it after redraw
		    سیٹ_keep_پیغام((char_u *)پیغام_buf, 0);
	    }
	    ورنہ
		msgmore((طویل)linecount);
	}
    }
    ورنہ
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    اگر (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_بفر));
    }
    ورنہ اگر (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_آغاز = orig_آغاز;
	curbuf->b_op_انتہا = orig_انتہا;
    }

    اگر (itmp != تہی)
	mch_remove(itmp);
    اگر (otmp != تہی)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    خالی
do_shell(
    char_u	*cmd,
    صحیح		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    صحیح		save_nwr;
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    صحیح		winstart = FALSE;
#اگر_ختم
    صحیح		keep_termcap = !termcap_فعال;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    اگر (check_restricted() || check_secure())
    {
	پیغام_انتہا();
	واپس;
    }

#اگر_تعریف_شدہ MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    اگر (cmd != تہی)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    اگر (gui.اندر_use && vim_strchr(p_go, GO_TERMINAL) != تہی)
	keep_termcap = TRUE;
# endif
#اگر_ختم

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    پیغام_putchar('\r');			// put cursor at start of line
    اگر (!autocmd_busy)
    {
	اگر (!keep_termcap)
	    stoptermcap();
    }
#اگر_تعریف_شدہ MSWIN
    اگر (!winstart)
#اگر_ختم
	پیغام_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    اگر (p_warn && !autocmd_busy && پیغام_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    اگر (buf_Is_بدلا_Not_رکن(buf))
	    {
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
		اگر (!keep_termcap)
		    starttermcap();	// don't want a message box here
#اگر_ختم
		پیغام_puts(_("[No write since last change]\n"));
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
		اگر (!keep_termcap)
		    stoptermcap();
#اگر_ختم
		توڑو;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    اگر (!swapping_screen())
	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
    cursor_on();
    (خالی)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    اگر (!swapping_screen())
    {
	پیغام_صف = صفیں - 1;
	پیغام_ستون = 0;
    }

    اگر (autocmd_busy)
    {
	اگر (پیغام_silent == 0)
	    redraw_later_clear();
    }
    ورنہ
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	اگر (!gui.اندر_use)
# endif
	{
	    اگر (cmd == تہی
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		اگر (پیغام_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    ورنہ
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		اگر (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		صحیح redraw = پیغام_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		اگر (gui.اندر_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(رکن_console ? -1 : redraw); // see below
# ورنہ
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#اگر_ختم // FEAT_GUI_MSWIN

	اگر (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#اگر_تعریف_شدہ AMIGA
	اگر (skip_redraw)		// ':' hit in wait_return()
	{
	    اگر (پیغام_silent == 0)
		redraw_later_clear();
	}
	ورنہ اگر (رکن_console)
	{
	    برآمد_STR("\033[0 q");	// get window size
	    اگر (got_int && پیغام_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    ورنہ
		must_redraw = 0;	// no extra redraw needed
	}
#اگر_ختم
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, تہی, تہی, FALSE, curbuf);
}

#اگر !defined(UNIX)
    ساکن char_u *
ڈھونڈو_pipe(char_u *cmd)
{
    char_u  *p;
    صحیح	    inquote = FALSE;

    برائے (p = cmd; *p != NUL; ++p)
    {
	اگر (!inquote && *p == '|')
	    واپس p;
	اگر (*p == '"')
	    inquote = !inquote;
	ورنہ اگر (rem_backslash(p))
	    ++p;
    }
    واپس تہی;
}
#اگر_ختم

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
    صحیح		is_powershell = FALSE;
#اگر_تعریف_شدہ UNIX
    صحیح		is_fish_shell;
#اگر_ختم

    char_u *shell_name = get_isolated_shell_name();
    اگر (shell_name == تہی)
	واپس تہی;

#اگر defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    اگر (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    ورنہ
#اگر_ختم
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	اگر (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	ورنہ
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    اگر (itmp != تہی)
    {
	اگر (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	ورنہ
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    اگر (otmp != تہی)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    اگر (buf == تہی)
	واپس تہی;

    اگر (is_powershell)
    {
	اگر (itmp != تہی)
	    vim_snprintf((حرف *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	ورنہ
	    vim_snprintf((حرف *)buf, len, "& { %s }", cmd);
    }
    ورنہ
    {
#اگر defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	اگر (itmp != تہی || otmp != تہی)
	{
	    اگر (is_fish_shell)
		vim_snprintf((حرف *)buf, len, "begin; %s; end", (حرف *)cmd);
	    ورنہ
		vim_snprintf((حرف *)buf, len, "(%s)", (حرف *)cmd);
	}
	ورنہ
	    STRCPY(buf, cmd);
	اگر (itmp != تہی)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#ورنہ
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	اگر (*p_sxe != NUL && *p_sxq == '(')
	{
	    اگر (itmp != تہی || otmp != تہی)
		vim_snprintf((حرف *)buf, len, "(%s)", (حرف *)cmd);
	    ورنہ
		STRCPY(buf, cmd);
	    اگر (itmp != تہی)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	ورنہ
	{
	    STRCPY(buf, cmd);
	    اگر (itmp != تہی)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		اگر (*p_shq == NUL)
		{
		    p = ڈھونڈو_pipe(buf);
		    اگر (p != تہی)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		اگر (*p_shq == NUL)
		{
		    p = ڈھونڈو_pipe(cmd);
		    اگر (p != تہی)
		    {
			// insert a space before the '|' for DOS
			STRCAT(buf, " ");
			STRCAT(buf, p);
		    }
		}
	    }
	}
#اگر_ختم
    }
    اگر (otmp != تہی)
	append_redir(buf, (صحیح)len, p_srr, otmp);

    واپس buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    خالی
append_redir(
    char_u	*buf,
    صحیح		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*انتہا;

    انتہا = buf + STRLEN(buf);
    // find "%s"
    برائے (p = opt; (p = vim_strchr(p, '%')) != تہی; ++p)
    {
	اگر (p[1] == 's') // found %s
	    توڑو;
	اگر (p[1] == '%') // skip %%
	    ++p;
    }
    اگر (p != تہی)
    {
#اگر_تعریف_شدہ MSWIN
	*انتہا++ = ' '; // not really needed? Not with sh, ksh or bash
#اگر_ختم
	vim_snprintf((حرف *)انتہا, (size_t)(buflen - (انتہا - buf)),
						  (حرف *)opt, (حرف *)fname);
    }
    ورنہ
	vim_snprintf((حرف *)انتہا, (size_t)(buflen - (انتہا - buf)), " %s %s",
		(حرف *)opt, (حرف *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    خالی
do_fixdel(exarg_T *eap غیر_مستعمل)
{
    char_u  *p;

    p = ڈھونڈو_termcode((char_u *)"kb");
    جوڑو_termcode((char_u *)"kD", p != تہی
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    خالی
print_سطر_no_سابقہ(
    linenr_T	lnum,
    صحیح		use_نمبر,
    صحیح		list)
{
    حرف	numbuf[30];

    اگر (curwin->w_p_nu || use_نمبر)
    {
	vim_snprintf(numbuf, حجم(numbuf),
				   "%*ld ", نمبر_چوڑائی(curwin), (طویل)lnum);
	پیغام_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    پیغام_prt_سطر(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    خالی
print_سطر(linenr_T lnum, صحیح use_نمبر, صحیح list)
{
    صحیح		save_silent = silent_منوال;

    // apply :filter /pat/
    اگر (پیغام_filtered(ml_get(lnum)))
	واپس;

    پیغام_آغاز();
    silent_منوال = FALSE;
    info_پیغام = TRUE;	// use mch_msg(), not mch_errmsg()
    print_سطر_no_سابقہ(lnum, use_نمبر, list);
    اگر (save_silent)
    {
	پیغام_putchar('\n');
	cursor_on();		// msg_start() switches it off
	برآمد_flush();
	silent_منوال = save_silent;
    }
    info_پیغام = FALSE;
}

    صحیح
rename_بفر(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, تہی, تہی, FALSE, curbuf);
    // buffer changed, don't change name now
    اگر (buf != curbuf)
	واپس FAIL;
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (aborting())	    // autocmds may abort script processing
	واپس FAIL;
#اگر_ختم
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
    curbuf->b_ffname = تہی;
    curbuf->b_sfname = تہی;
    اگر (setfname(curbuf, new_fname, تہی, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	واپس FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    اگر (xfname != تہی && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	اگر (buf != تہی && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, تہی, تہی, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    واپس OK;
}

/*
 * ":file[!] [fname]".
 */
    خالی
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    اگر (eap->addr_گنتی > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_گنتی > 1))
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }

    اگر (*eap->arg != NUL || eap->addr_گنتی == 1)
    {
	اگر (rename_بفر(eap->arg) == FAIL)
	    واپس;
	redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#اگر_ختم
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    اگر (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    خالی
ex_update(exarg_T *eap)
{
    اگر (curbuf_Is_بدلا())
	(خالی)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    خالی
ex_write(exarg_T *eap)
{
    اگر (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
    }

    اگر (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    ورنہ
	(خالی)do_write(eap);
}

#اگر_تعریف_شدہ UNIX
    ساکن صحیح
check_writable(char_u *fname)
{
    اگر (mch_nodetype(fname) == گرہ_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	واپس FAIL;
    }
    واپس OK;
}
#اگر_ختم

/*
 * Write the current buffer to file "eap->arg".
 * If "eap->append" is TRUE, append to the file.
 *
 * If "*eap->arg == NUL" write to current file.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
do_write(exarg_T *eap)
{
    صحیح		other;
    char_u	*fname = تہی;		// init to shut up gcc
    char_u	*ffname;
    صحیح		retval = FAIL;
    char_u	*free_fname = تہی;
#اگر_تعریف_شدہ FEAT_BROWSE
    char_u	*browse_file = تہی;
#اگر_ختم
    buf_T	*alt_buf = تہی;
    صحیح		name_was_missing;

    اگر (not_writing())		// check 'write' option
	واپس FAIL;

    ffname = eap->arg;
#اگر_تعریف_شدہ FEAT_BROWSE
    اگر ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    تہی, تہی, تہی, curbuf);
	اگر (browse_file == تہی)
	    جاؤ theend;
	ffname = browse_file;
    }
#اگر_ختم
    اگر (*ffname == NUL)
    {
	اگر (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_دلیل_required));
	    جاؤ theend;
	}
	other = FALSE;
    }
    ورنہ
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	اگر (free_fname != تہی)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    اگر (other)
    {
	اگر (vim_strchr(p_cpo, CPO_ALTWRITE) != تہی
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	ورنہ
	    alt_buf = buflist_findname(ffname);
	اگر (alt_buf != تہی && alt_buf->b_ml.ml_mfp != تہی)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_اندر_another_بفر));
	    جاؤ theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    اگر (!other && (bt_dontwrite_پیغام(curbuf) || check_fname() == FAIL
#اگر_تعریف_شدہ UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#اگر_ختم
		|| check_readonly(&eap->forceit, curbuf)))
	جاؤ theend;

    اگر (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	اگر (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_سطر_گنتی)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		اگر (vim_dialog_yesno(VIM_QUESTION, تہی,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    جاؤ theend;
		eap->forceit = TRUE;
	    }
	    ورنہ
#اگر_ختم
	    {
		emsg(_(e_use_bang_مقصد_write_partial_بفر));
		جاؤ theend;
	    }
	}
    }

    اگر (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	اگر (eap->cmdidx == CMD_saveas && alt_buf != تہی)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, تہی, تہی, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, تہی, تہی, FALSE, alt_buf);
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (curbuf != was_curbuf || aborting())
#ورنہ
	    اگر (curbuf != was_curbuf)
#اگر_ختم
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		جاؤ theend;
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
	    buf_name_بدلا(curbuf);

	    apply_autocmds(EVENT_BUFFILEPOST, تہی, تہی, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, تہی, تہی, FALSE, alt_buf);
	    اگر (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, تہی, تہی, FALSE, alt_buf);
	    }
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (curbuf != was_curbuf || aborting())
#ورنہ
	    اگر (curbuf != was_curbuf)
#اگر_ختم
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		جاؤ theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    اگر (*curbuf->b_p_ft == NUL)
	    {
		اگر (au_has_گروہ((char_u *)"filetypedetect"))
		    (خالی)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, تہی);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == تہی;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	اگر (eap->cmdidx == CMD_saveas)
	{
	    اگر (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#اگر_ختم
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	اگر (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#اگر_تعریف_شدہ FEAT_BROWSE
    vim_free(browse_file);
#اگر_ختم
    vim_free(free_fname);
    واپس retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    صحیح
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    صحیح		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    اگر (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == تہی)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	اگر (!eap->forceit && !eap->append)
	{
#اگر_تعریف_شدہ UNIX
	    // with UNIX it is possible to open a directory
	    اگر (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		واپس FAIL;
	    }
#اگر_ختم
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_پیغام_سائز];

		dialog_پیغام(buff, _("Overwrite existing file \"%s\"?"), fname);
		اگر (vim_dialog_yesno(VIM_QUESTION, تہی, buff, 2) != VIM_YES)
		    واپس FAIL;
		eap->forceit = TRUE;
	    }
	    ورنہ
#اگر_ختم
	    {
		emsg(_(e_file_exists));
		واپس FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	اگر (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    صحیح		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    اگر (*p_dir == NUL)
	    {
		dir = alloc(5);
		اگر (dir == تہی)
		    واپس FAIL;
		STRCPY(dir, ".");
	    }
	    ورنہ
	    {
		dir = alloc(MAXPATHL);
		اگر (dir == تہی)
		    واپس FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    اگر (r)
	    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		اگر (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_پیغام_سائز];

		    dialog_پیغام(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    اگر (vim_dialog_yesno(VIM_QUESTION, تہی, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			واپس FAIL;
		    }
		    eap->forceit = TRUE;
		}
		ورنہ
#اگر_ختم
		{
		    semsg(_(e_تبادلہ_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    واپس FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    واپس OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    خالی
ex_wnext(exarg_T *eap)
{
    صحیح		i;

    اگر (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (صحیح)eap->line2;
    ورنہ
	i = curwin->w_arg_idx - (صحیح)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
    اگر (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    خالی
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    صحیح		error = 0;
    صحیح		save_forceit = eap->forceit;
    صحیح		save_exiting = exiting;

    اگر (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	اگر (before_quit_all(eap) == FAIL)
	    واپس;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#اگر_تعریف_شدہ FEAT_TERMINAL
	اگر (exiting && !eap->forceit && رکن_کام_running(buf->b_رکن))
	{
	    bufref_T	bufref;

	    سیٹ_bufref(&bufref, buf);
	    اگر (رکن_try_stop_کام(buf) == FAIL)
	    {
		no_write_پیغام_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    ورنہ اگر (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	ورنہ
#اگر_ختم
	اگر (buf_Is_بدلا(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    اگر (not_writing())
	    {
		++error;
		توڑو;
	    }
#اگر_تعریف_شدہ FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    اگر (buf->b_ffname == تہی && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#اگر_ختم
	    اگر (buf->b_ffname == تہی)
	    {
		semsg(_(e_no_file_name_for_بفر_nr), (طویل)buf->b_fnum);
		++error;
	    }
	    ورنہ اگر (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    ورنہ
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf);
		اگر (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		اگر (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    اگر (exiting)
    {
	اگر (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    ساکن صحیح
not_writing(خالی)
{
    اگر (p_write)
	واپس FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    واپس TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    ساکن صحیح
check_readonly(صحیح *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    اگر (!*forceit && (buf->b_p_ro
		|| (mch_stat((حرف *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != تہی)
	{
	    char_u	buff[DIALOG_پیغام_سائز];

	    اگر (buf->b_p_ro)
		dialog_پیغام(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    ورنہ
		dialog_پیغام(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    اگر (vim_dialog_yesno(VIM_QUESTION, تہی, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		واپس FALSE;
	    }
	    ورنہ
		واپس TRUE;
	}
	ورنہ
#اگر_ختم
	اگر (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_سیٹ_جوڑو_bang_مقصد_override));
	ورنہ
	    semsg(_(e_str_is_read_only_جوڑو_bang_مقصد_override), buf->b_fname);
	واپس TRUE;
    }

    واپس FALSE;
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
    صحیح
getfile(
    صحیح		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    صحیح		setpm,
    linenr_T	lnum,
    صحیح		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    صحیح		other;
    صحیح		retval;
    char_u	*free_me = تہی;

    اگر (!check_can_سیٹ_curbuf_forceit(forceit))
	واپس GETFILE_ERROR;

    اگر (متن_locked())
	واپس GETFILE_ERROR;
    اگر (curbuf_locked())
	واپس GETFILE_ERROR;

    اگر (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    ورنہ
	other = (fnum != curbuf->b_fnum);

    اگر (other)
	++no_wait_return;	    // don't wait for autowrite message
    اگر (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbuf_Is_بدلا() && autowrite(curbuf, forceit) == FAIL)
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر (p_confirm && p_write)
	    dialog_بدلا(curbuf, FALSE);
	اگر (curbuf_Is_بدلا())
#اگر_ختم
	{
	    --no_wait_return;
	    no_write_پیغام();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    جاؤ theend;
	}
    }
    اگر (other)
	--no_wait_return;
    اگر (setpm)
	setpcmark();
    اگر (!other)
    {
	اگر (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    ورنہ اگر (do_ecmd(fnum, ffname, sfname, تہی, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    ورنہ
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    واپس retval;
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
    صحیح
do_ecmd(
    صحیح		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    صحیح		flags,
    win_T	*oldwin)
{
    صحیح		other_file;		// TRUE if editing another file
    صحیح		oldbuf;			// TRUE if using existing buffer
    صحیح		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = تہی;
#اگر defined(FEAT_EVAL)
    صحیح		did_سیٹ_swapcommand = FALSE;
#اگر_ختم
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = تہی;
#اگر_تعریف_شدہ FEAT_BROWSE
    char_u	dot_راستہ[] = ".";
    char_u	*browse_file = تہی;
#اگر_ختم
    صحیح		retval = FAIL;
    طویل	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    صحیح		newcol = -1;
    صحیح		solcol = -1;
    pos_T	*pos;
    char_u	*command = تہی;
#اگر_تعریف_شدہ FEAT_SPELL
    صحیح		did_get_winopts = FALSE;
#اگر_ختم
    صحیح		readfile_flags = 0;
    صحیح		did_inc_redrawing_disabled = FALSE;
    طویل	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (ERROR_IF_رکن_POPUP_کھڑکی)
	واپس FAIL;
#اگر_ختم

    اگر (eap != تہی)
	command = eap->do_ecmd_cmd;
    سیٹ_bufref(&old_curbuf, curbuf);

    اگر (fnum != 0)
    {
	اگر (fnum == curbuf->b_fnum)	// file is already being edited
	    واپس OK;			// nothing to do
	other_file = TRUE;
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_BROWSE
	اگر ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    اگر (
# ifdef FEAT_GUI
		!gui.اندر_use &&
# endif
		    au_has_گروہ((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		اگر (ffname == تہی || !mch_isdir(ffname))
		    ffname = dot_راستہ;
	    }
	    ورنہ
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    تہی, تہی, تہی, curbuf);
		اگر (browse_file == تہی)
		    جاؤ theend;
		ffname = browse_file;
	    }
	}
#اگر_ختم
	// if no short name given, use ffname for short name
	اگر (sfname == تہی)
	    sfname = ffname;
#اگر_تعریف_شدہ USE_FNAME_CASE
	اگر (sfname != تہی)
	    fname_case(sfname, 0);   // set correct case for sfname
#اگر_ختم

	اگر ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == تہی || *ffname == NUL))
	    جاؤ theend;

	اگر (ffname == تہی)
	    other_file = TRUE;
					    // there is no file name
	ورنہ اگر (*ffname == NUL && curbuf->b_ffname == تہی)
	    other_file = FALSE;
	ورنہ
	{
	    اگر (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    اگر (free_fname != تہی)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    اگر (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_بدلا(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == تہی ? 0 : CCGD_EXCMD)))
    {
	اگر (fnum == 0 && other_file && ffname != تہی)
	    (خالی)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	جاؤ theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    اگر (oldwin != تہی && !win_valid(oldwin))
	oldwin = تہی;

#اگر defined(FEAT_EVAL)
    اگر ((command != تہی || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	اگر (command != تہی)
	    valsize = (صحیح)STRLEN(command) + 3;
	ورنہ
	    valsize = 30;
	val.string = alloc(valsize);
	اگر (val.string != تہی)
	{
	    اگر (command != تہی)
		val.لمبائی = vim_snprintf_safelen((حرف *)val.string,
		    valsize, ":%s\r", command);
	    ورنہ
		val.لمبائی = vim_snprintf_safelen((حرف *)val.string,
		    valsize, "%ldG", (طویل)newlnum);
	    سیٹ_vim_var_string(VV_SWAPCOMMAND, val.string, (صحیح)val.لمبائی);
	    did_سیٹ_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#اگر_ختم

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    اگر (other_file)
    {
	صحیح پچھلا_alt_fnum = curwin->w_alt_fnum;

	اگر (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    اگر ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    اگر (oldwin != تہی)
		buflist_altfpos(oldwin);
	}

	اگر (fnum)
	    buf = buflist_findnr(fnum);
	ورنہ
	{
	    اگر (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		اگر (command != تہی)
		{
		    tlnum = atol((حرف *)command);
		    اگر (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		اگر (newbuf != تہی)
		{
		    اگر (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    اگر (tlnum > 0)
			newbuf->b_آخری_cursor.lnum = tlnum;
		}
		جاؤ theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_سیٹ_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    اگر (oldwin != تہی)
		oldwin = curwin;
	    سیٹ_bufref(&old_curbuf, curbuf);
	}
	اگر (buf == تہی)
	    جاؤ theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	اگر (buf->b_locked_بانٹو)
	{
	    emsg(_(e_cannot_switch_مقصد_a_closing_بفر));
	    جاؤ theend;
	}
	اگر (curwin->w_alt_fnum == buf->b_fnum && پچھلا_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = پچھلا_alt_fnum;

	اگر (buf->b_ml.ml_mfp == تہی)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	ورنہ					// existing memfile
	{
	    oldbuf = TRUE;
	    سیٹ_bufref(&bufref, buf);
	    (خالی)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    اگر (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		جاؤ theend;
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (aborting())	    // autocmds may abort script processing
		جاؤ theend;
#اگر_ختم
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	اگر ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_آخری)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->ستون;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	اگر (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    صحیح		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    أكد(cmdwin_buf == تہی);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = تہی;

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
	    اگر (buf->b_fname != تہی)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    سیٹ_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, تہی, تہی, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    اگر (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_پیغام(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		جاؤ theend;
	    }
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		جاؤ theend;
	    }
#اگر_ختم
	    اگر (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    ورنہ
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		اگر (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_بفر(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != تہی);

		// Autocommands may have closed the window.
		اگر (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#اگر_تعریف_شدہ FEAT_EVAL
		// autocmds may abort script processing
		اگر (aborting() && curwin->w_بفر != تہی)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    جاؤ theend;
		}
#اگر_ختم
		// Be careful again, like above.
		اگر (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    اگر (curwin->w_بفر == تہی)
			buf = lastbuf;
		    ورنہ
		    {
			delbuf_پیغام(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			جاؤ theend;
		    }
		}
		اگر (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		ورنہ
		{
#اگر_تعریف_شدہ FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    اگر (curwin->w_بفر == تہی
			    || curwin->w_s == &(curwin->w_بفر->b_s))
			curwin->w_s = &(buf->b_s);
#اگر_ختم
		    اگر (curwin->w_بفر != تہی)
			--curwin->w_بفر->b_nwindows;

		    curwin->w_بفر = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    اگر (!oldbuf && eap != تہی)
		    {
			سیٹ_file_options(TRUE, eap);
			سیٹ_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#اگر_تعریف_شدہ FEAT_SPELL
		did_get_winopts = TRUE;
#اگر_ختم
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.ستون = 0;
    }
    ورنہ // !other_file
    {
	اگر ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    جاؤ theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    اگر ((flags & ECMD_سیٹ_HELP) || keep_help_flag)
    {
	prepare_help_بفر();
    }
    ورنہ
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	اگر (!curbuf->b_help)
	    سیٹ_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    اگر (buf != curbuf)
	جاؤ theend;
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (aborting())	    // autocmds may abort script processing
	جاؤ theend;
#اگر_ختم

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
    اگر (!other_file && !oldbuf)		// re-use the buffer
    {
	سیٹ_آخری_cursor(curwin);	// may set b_last_cursor
	اگر (newlnum == ECMD_آخری || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.ستون;
	}
	buf = curbuf;
	اگر (buf->b_fname != تہی)
	    new_name = vim_strsave(buf->b_fname);
	ورنہ
	    new_name = تہی;
	سیٹ_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	اگر (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_سطر_گنتی <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    اگر (u_savecommon(0, curbuf->b_ml.ml_سطر_گنتی + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		جاؤ theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	ورنہ
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	اگر (!bufref_valid(&bufref))
	{
	    delbuf_پیغام(new_name);	// frees new_name
	    جاؤ theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	اگر (buf != curbuf)
	    جاؤ theend;
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	    جاؤ theend;
#اگر_ختم
	buf_clear_file(curbuf);
	curbuf->b_op_آغاز.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_انتہا.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    // If the file name was changed, reset the not-edit flag so that ":write"
    // works.
    اگر (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    اگر (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#اگر_تعریف_شدہ FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		اگر (win->w_بفر == curbuf)
		    foldUpdateAll(win);
	}
#اگر_ختم

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	اگر (!oldbuf)			    // need to read the file
	{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    اگر (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#اگر_ختم
	    تبادلہ_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    اگر (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#اگر defined(FEAT_EVAL)
	    اگر (should_abort(open_بفر(FALSE, eap, readfile_flags)))
		retval = FAIL;
#ورنہ
	    (خالی)open_بفر(FALSE, eap, readfile_flags);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#اگر_ختم
	    اگر (تبادلہ_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_تبادلہ_exists(&old_curbuf);
	}
	ورنہ
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, تہی, تہی, FALSE,
							      curbuf, &retval);
	    اگر ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, تہی, تہی, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	اگر (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *متن = ml_get_curline();

	    اگر (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.ستون != (صحیح)(skipwhite(متن) - متن))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.ستون;
	    }
	}
	اگر (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	بدلا_سطر_abv_curs();

	maketitle();
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	اگر (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_سیٹ_title(curwin);
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    اگر (curwin->w_p_diff)
    {
	diff_buf_جوڑو(curbuf);
	diff_invalidate(curbuf);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    اگر (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(خالی)parse_spelllang(curwin);
#اگر_ختم

    اگر (command == تہی)
    {
	اگر (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.ستون = newcol;
	    check_cursor();
	}
	ورنہ اگر (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    اگر (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.ستون = solcol;
		check_cursor_ستون();
		curwin->w_cursor.coladd = 0;
		curwin->w_سیٹ_curswant = true;
	    }
	    ورنہ
		beginline(BL_SOL | BL_FIX);
	}
	ورنہ			// no line number, go to last line in Ex mode
	{
	    اگر (exmode_فعال)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    اگر (oldbuf && !auto_buf)
    {
	صحیح	پیغام_scroll_save = پیغام_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	اگر (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    پیغام_scroll = FALSE;
	اگر (!پیغام_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	پیغام_آغاز();
	پیغام_scroll = پیغام_scroll_save;
	پیغام_scrolled_ign = TRUE;

	اگر (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	پیغام_scrolled_ign = FALSE;
    }

#اگر_تعریف_شدہ FEAT_VIMINFO
    curbuf->b_آخری_مستعمل = vim_وقت();
#اگر_ختم

    اگر (command != تہی)
	do_cmdline(command, تہی, تہی, DOCMD_VERBOSE|DOCMD_RANGEOK);

#اگر_تعریف_شدہ FEAT_KEYMAP
    اگر (curbuf->b_kmap_state & KEYMAP_INIT)
	(خالی)keymap_init();
#اگر_ختم

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    اگر (!skip_redraw)
    {
	n = *so_ptr;
	اگر (topline == 0 && command == تہی)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    اگر (p_im && (State & منوال_داخل_کرو) == 0)
	need_آغاز_insertmode = TRUE;

#اگر_تعریف_شدہ FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    اگر (p_acd && curbuf->b_ffname != تہی)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	اگر (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#اگر_ختم

#اگر defined(FEAT_NETBEANS_INTG)
    اگر (curbuf->b_ffname != تہی)
    {
# ifdef FEAT_NETBEANS_INTG
	اگر ((flags & ECMD_سیٹ_HELP) != ECMD_سیٹ_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#اگر_ختم

theend:
    اگر (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#اگر defined(FEAT_EVAL)
    اگر (did_سیٹ_swapcommand)
	سیٹ_vim_var_string(VV_SWAPCOMMAND, تہی, -1);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BROWSE
    vim_free(browse_file);
#اگر_ختم
    vim_free(free_fname);
    واپس retval;
}

    ساکن خالی
delbuf_پیغام(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_بفر_str),
					   name == تہی ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = تہی;
    au_new_curbuf.br_buf_free_گنتی = 0;
}

ساکن صحیح append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    ساکن char_u *
get_append_سطر(exarg_T *eap, صحیح indent)
{
    char_u	*theline;
    char_u	*p;

    اگر (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    ورنہ اگر (eap->ea_getline == تہی)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	اگر (eap->nextcmd == تہی)
	    واپس تہی;
	p = vim_strchr(eap->nextcmd, NL);
	اگر (p == تہی)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	اگر (*p != NUL)
	    ++p;
	ورنہ
	    p = تہی;
	eap->nextcmd = p;
    }
    ورنہ
    {
	صحیح save_State = State;

	// Set State to avoid the cursor shape to be set to MODE_INSERT
	// state when getline() returns.
	State = منوال_CMDLINE;
	theline = eap->ea_getline(
#اگر_تعریف_شدہ FEAT_EVAL
		eap->cstack->cs_looplevel > 0 ? -1 :
#اگر_ختم
		NUL, eap->cookie, indent, TRUE);
	State = save_State;
    }
    واپس theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    خالی
ex_append(exarg_T *eap)
{
    char_u	*theline;
    صحیح		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    صحیح		indent = 0;
    char_u	*p;
    صحیح		vcol;
    صحیح		خالی_خانے_2 = (curbuf->b_ml.ml_flags & ML_خالی_خانے);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (not_اندر_vim9(eap, eap->skip) == FAIL)
	واپس;
#اگر_ختم
    اگر (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	جب ((theline = get_append_سطر(eap, 0)) != تہی)
	{
	    صحیح انتہا = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    اگر (انتہا)
		توڑو;
	}
	واپس;
    }

    // the ! flag toggles autoindent
    اگر (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    اگر (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    اگر (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    اگر (خالی_خانے_2 && lnum == 1)
	lnum = 0;

    State = منوال_داخل_کرو;		    // behave like in Insert mode
    اگر (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= منوال_LANGMAP;

    برائے (;;)
    {
	پیغام_scroll = TRUE;
	need_wait_return = FALSE;
	اگر (curbuf->b_p_ai)
	{
	    اگر (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    ورنہ اگر (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_سطر(eap, indent);
	سطریں_بایاں = صفیں - 1;
	اگر (theline == تہی)
	    توڑو;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	اگر (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	برائے (p = theline; indent > vcol; ++p)
	{
	    اگر (*p == ' ')
		++vcol;
	    ورنہ اگر (*p == TAB)
		vcol += 8 - vcol % 8;
	    ورنہ
		توڑو;
	}
	اگر ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (خالی_خانے_2 ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    توڑو;
	}

	// don't use autoindent if nothing was typed.
	اگر (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	اگر (خالی_خانے_2)
	    // there are no marks below the inserted lines
	    appended_سطریں(lnum, 1L);
	ورنہ
	    appended_سطریں_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	اگر (خالی_خانے_2)
	{
	    ml_delete(2L);
	    خالی_خانے_2 = FALSE;
	}
    }
    State = منوال_معمول;

    اگر (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_آغاز.lnum = (eap->line2 < curbuf->b_ml.ml_سطر_گنتی) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_سطر_گنتی;
	اگر (eap->cmdidx != CMD_append)
	    --curbuf->b_op_آغاز.lnum;
	curbuf->b_op_انتہا.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_آغاز.lnum;
	curbuf->b_op_آغاز.ستون = curbuf->b_op_انتہا.ستون = 0;
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
    خالی
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (not_اندر_vim9(eap, eap->skip) == FAIL)
	واپس;
#اگر_ختم
    اگر (eap->skip)
    {
	ex_append(eap);
	واپس;
    }
    اگر (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	واپس;

    // the ! flag toggles autoindent
    اگر (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    برائے (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)	    // nothing to delete
	    توڑو;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_سطریں_mark(eap->line1, (طویل)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    خالی
ex_z(exarg_T *eap)
{
    char_u	*x;
    طویل	bigness;
    char_u	*قسم;
    صحیح		minus = 0;
    linenr_T	آغاز, انتہا, curs, i;
    صحیح		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    اگر (eap->forceit)
	bigness = صفیں - 1;
    ورنہ اگر (!ONE_کھڑکی)
	bigness = curwin->w_اونچائی - 3;
    ورنہ
	bigness = curwin->w_p_scr * 2;
    اگر (bigness < 1)
	bigness = 1;

    x = eap->arg;
    قسم = x;
    اگر (*قسم == '-' || *قسم == '+' || *قسم == '='
					      || *قسم == '^' || *قسم == '.')
	++x;
    جب (*x == '-' || *x == '+')
	++x;

    اگر (*x != 0)
    {
	اگر (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_دلیل_مقصد_z));
	    واپس;
	}
	ورنہ
	{
	    bigness = atol((حرف *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    اگر (bigness > 2 * curbuf->b_ml.ml_سطر_گنتی || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_سطر_گنتی;

	    p_کھڑکی = bigness;
	    اگر (*قسم == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    اگر (*قسم == '-' || *قسم == '+')
	برائے (x = قسم + 1; *x == *قسم; ++x)
	    ;

    انتخاب (*قسم)
    {
	حالت '-':
	    آغاز = lnum - bigness * (linenr_T)(x - قسم) + 1;
	    انتہا = آغاز + bigness - 1;
	    curs = انتہا;
	    توڑو;

	حالت '=':
	    آغاز = lnum - (bigness + 1) / 2 + 1;
	    انتہا = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    توڑو;

	حالت '^':
	    آغاز = lnum - bigness * 2;
	    انتہا = lnum - bigness;
	    curs = lnum - bigness;
	    توڑو;

	حالت '.':
	    آغاز = lnum - (bigness + 1) / 2 + 1;
	    انتہا = lnum + (bigness + 1) / 2 - 1;
	    curs = انتہا;
	    توڑو;

	طےشدہ:  // '+'
	    آغاز = lnum;
	    اگر (*قسم == '+')
		آغاز += bigness * (linenr_T)(x - قسم - 1) + 1;
	    ورنہ اگر (eap->addr_گنتی == 0)
		++آغاز;
	    انتہا = آغاز + bigness - 1;
	    curs = انتہا;
	    توڑو;
    }

    اگر (آغاز < 1)
	آغاز = 1;

    اگر (انتہا > curbuf->b_ml.ml_سطر_گنتی)
	انتہا = curbuf->b_ml.ml_سطر_گنتی;

    اگر (curs > curbuf->b_ml.ml_سطر_گنتی)
	curs = curbuf->b_ml.ml_سطر_گنتی;
    ورنہ اگر (curs < 1)
	curs = 1;

    برائے (i = آغاز; i <= انتہا; i++)
    {
	اگر (minus && i == lnum)
	{
	    پیغام_putchar('\n');

	    برائے (j = 1; j < Columns; j++)
		پیغام_putchar('-');
	}

	print_سطر(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	اگر (minus && i == lnum)
	{
	    پیغام_putchar('\n');

	    برائے (j = 1; j < Columns; j++)
		پیغام_putchar('-');
	}
    }

    اگر (curwin->w_cursor.lnum != curs)
    {
	curwin->w_cursor.lnum = curs;
	curwin->w_cursor.ستون = 0;
    }
    ex_no_reprint = TRUE;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    صحیح
check_restricted(خالی)
{
    اگر (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_اندر_rvim));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    صحیح
check_secure(خالی)
{
    اگر (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_اندر_current_dir_or_لیبل_تلاش));
	واپس TRUE;
    }
#اگر_تعریف_شدہ HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندر_sandbox));
	واپس TRUE;
    }
#اگر_ختم
    واپس FALSE;
}

ساکن char_u	*old_sub = تہی;	// previous substitute pattern
ساکن صحیح	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
قسمتعریف ساخت {
    صحیح	do_all;		// do multiple substitutions per line
    صحیح	do_ask;		// ask for confirmation
    صحیح	do_گنتی;	// count only
    صحیح	do_error;	// if false, ignore errors
    صحیح	do_print;	// print last line with subs.
    صحیح	do_list;	// list last line with subs.
    صحیح	do_نمبر;	// list last line with line nr
    صحیح	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *آغاز, صحیح delimiter)
{
    char_u *p = آغاز;

    جب (p[0])
    {
	اگر (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    توڑو;
	}
	اگر (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    واپس p;
}

    ساکن صحیح
check_regexp_delim(صحیح c)
{
    اگر (محفوظ_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    خالی
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    طویل	i = 0;
    regmmatch_T regmatch;
    ساکن subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#اگر_تعریف_شدہ FEAT_EVAL
    subflags_T	subflags_save;
#اگر_ختم
    صحیح		save_do_all;		// remember user specified 'g' flag
    صحیح		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = تہی;		// init for GCC
    string_T	pat = {تہی, 0};
    صحیح		delimiter;
    size_t	sublen;
    صحیح		got_quit = FALSE;
    صحیح		got_مطابقت = FALSE;
    صحیح		which_pat;
    char_u	*cmd;
    char_u	*p;
    صحیح		save_State;
    linenr_T	پہلا_سطر = 0;		// first changed line
    linenr_T	آخری_سطر = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
    linenr_T	line2;
    طویل	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    صحیح		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    صحیح		آغاز_nsubs;
    صحیح		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		save_ma = 0;
    صحیح		save_sandbox = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    textprop_T	*متن_props = تہی;
#اگر_ختم

    cmd = eap->arg;
    اگر (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    آغاز_nsubs = sub_nsubs;

    اگر (eap->cmdidx == CMD_tilde)
	which_pat = RE_آخری;	// use last used regexp
    ورنہ
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    اگر (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == تہی)
    {
	// don't accept alphanumeric for separator
	اگر (check_regexp_delim(*cmd) == FAIL)
	    واپس;
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (اندر_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    واپس;
#اگر_ختم

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	اگر (*cmd == '\\')
	{
	    اگر (اندر_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_اندر_vim9_script));
		واپس;
	    }
	    ++cmd;
	    اگر (vim_strchr((char_u *)"/?&", *cmd) == تہی)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		واپس;
	    }
	    اگر (*cmd != '&')
		which_pat = RE_تلاش;	    // use last '/' pattern
	    STR_LITERAL_سیٹ(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	ورنہ		// find the end of the regexp
	{
	    which_pat = RE_آخری;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, تہی, تہی);
	    pat.لمبائی = (size_t)(cmd - pat.string);
	    اگر (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	اگر (sub == تہی)
	    // out of memory
	    واپس;

	اگر (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    اگر (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != تہی)
	    {
		اگر (old_sub == تہی)	// there is no previous command
		{
		    emsg(_(e_no_پچھلا_substitute_regular_عبارت));
		    vim_free(sub);
		    واپس;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		اگر (sub == تہی)
		    // out of memory
		    واپس;
	    }
	    ورنہ اگر (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		اگر (old_sub == تہی)
		{
		    // out of memory
		    vim_free(sub);
		    واپس;
		}
	    }
	}
    }
    ورنہ اگر (!eap->skip)	// use previous pattern and substitution
    {
	اگر (old_sub == تہی)	// there is no previous command
	{
	    emsg(_(e_no_پچھلا_substitute_regular_عبارت));
	    واپس;
	}
	pat.string = تہی;		// search_regcomp() will use previous pattern
	pat.لمبائی = 0;
	sub = vim_strsave(old_sub);
	اگر (sub == تہی)
	    واپس;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    اگر (pat.string != تہی && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_سطریں_گنتی;

	اگر (eap->skip)
	{
	    vim_free(sub);
	    واپس;
	}
	curwin->w_cursor.lnum = eap->line1;
	اگر (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	ورنہ اگر (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	ورنہ اگر (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_سطریں_گنتی = eap->line2 - eap->line1 + 1;
	اگر (eap->line2 < curbuf->b_ml.ml_سطر_گنتی)
	    ++joined_سطریں_گنتی;
	اگر (joined_سطریں_گنتی > 1)
	{
	    (خالی)do_join(joined_سطریں_گنتی, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_سطریں_گنتی - 1;
	    sub_nlines = 1;
	    (خالی)do_sub_پیغام(FALSE);
	    ex_may_print(eap);
	}

	اگر (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.لمبائی, magic_isset());
	// put pattern in history
	جوڑو_مقصد_history(HIST_تلاش, pat.string, pat.لمبائی, TRUE, NUL);
	vim_free(sub);

	واپس;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    اگر (*cmd == '&')
	++cmd;
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (اندر_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	ورنہ
#اگر_ختم
	اگر (!p_ed)
	{
	    اگر (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    ورنہ
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_گنتی = FALSE;
	subflags.do_نمبر = FALSE;
	subflags.do_ic = 0;
    }
    جب (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	اگر (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	ورنہ اگر (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	ورنہ اگر (*cmd == 'n')
	    subflags.do_گنتی = TRUE;
	ورنہ اگر (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	ورنہ اگر (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_آخری;
	ورنہ اگر (*cmd == 'p')
	    subflags.do_print = TRUE;
	ورنہ اگر (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_نمبر = TRUE;
	}
	ورنہ اگر (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	ورنہ اگر (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	ورنہ اگر (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	ورنہ
	    توڑو;
	++cmd;
    }
    اگر (subflags.do_گنتی)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    اگر (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	اگر (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_مثبت_گنتی_required));
	    vim_free(sub);
	    واپس;
	}
	ورنہ اگر (i >= زیادہ_سے_زیادہ_عدد)
	{
	    حرف	buf[20];
	    vim_snprintf(buf, حجم(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    واپس;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	اگر (eap->line2 > curbuf->b_ml.ml_سطر_گنتی)
	    eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    اگر (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	سیٹ_nextcmd(eap, cmd);
	اگر (eap->nextcmd == تہی)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    واپس;
	}
    }

    اگر (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	واپس;
    }

    اگر (!subflags.do_گنتی && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	واپس;
    }

    اگر (تلاش_regcomp(pat.string, pat.لمبائی, تہی, RE_SUBST, which_pat, تلاش_HIS, &regmatch) == FAIL)
    {
	اگر (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	واپس;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    اگر (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    ورنہ اگر (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = تہی;
    sub_firstline.لمبائی = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    اگر (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	اگر (p == تہی)
	    واپس;
	sub = p;
    }
    ورنہ
    {
	p = regtilde(sub, magic_isset());

	اگر (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    برائے (lnum = eap->line1; lnum <= line2 && !(got_quit
#اگر defined(FEAT_EVAL)
		|| aborting()
#اگر_ختم
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, تہی);
	اگر (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	پچھلا_matchcol = MAXCOL;
	    string_T	new_آغاز = {تہی, 0};
	    size_t	new_آغاز_سائز = 0;
	    char_u	*new_انتہا;
	    صحیح		did_sub = FALSE;
	    صحیح		lastone;
	    size_t	copy_len;
	    size_t	needed_سائز;
	    طویل	nmatch_tl = 0;	// nr of lines matched below lnum
	    صحیح		do_again;	// do it again after joining lines
	    صحیح		skip_مطابقت = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_بانٹو = false;	// "\r" split the line
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    صحیح		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	کل_added =  0;
	    صحیح		متن_prop_گنتی = 0;
#اگر_ختم

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
	    اگر (!got_مطابقت)
	    {
		setpcmark();
		got_مطابقت = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    برائے (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		اگر (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
		    توڑو;

		اگر (sub_firstline.string == تہی)
		{
		    sub_firstline.لمبائی = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.لمبائی);
		    اگر (sub_firstline.string == تہی)
		    {
			vim_free(new_آغاز.string);
			جاؤ outofmem;
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
		اگر (matchcol == پچھلا_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].ستون)
		{
		    اگر (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_مطابقت = TRUE;
		    ورنہ
		    {
			 // search for a match at next column
			اگر (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			ورنہ
			    ++matchcol;
		    }
		    جاؤ skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].ستون;
		پچھلا_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		اگر (subflags.do_گنتی)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    اگر (nmatch > 1)
		    {
			matchcol = (colnr_T)sub_firstline.لمبائی;
			nmatch = 1;
			skip_مطابقت = TRUE;
		    }
		    sub_nsubs++;
		    did_sub = TRUE;
#اگر_تعریف_شدہ FEAT_EVAL
		    // Skip the substitution, unless an expression is used,
		    // then it is evaluated in the sandbox.
		    اگر (!(sub[0] == '\\' && sub[1] == '='))
#اگر_ختم
			جاؤ skip;
		}

		اگر (subflags.do_ask)
		{
		    صحیح typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = منوال_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.ستون = regmatch.startpos[0].ستون;
		    اگر (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    اگر (vim_strchr(p_cpo, CPO_UNDO) != تہی)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    جب (subflags.do_ask)
		    {
			اگر (exmode_فعال)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_سطر_no_سابقہ(lnum,
					 subflags.do_نمبر, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, تہی, تہی, 0);
			    curwin->w_cursor.ستون = regmatch.endpos[0].ستون - 1;
			    اگر (curwin->w_cursor.ستون < 0)
				curwin->w_cursor.ستون = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   تہی, تہی, &ec, 0);
			    curwin->w_cursor.ستون = regmatch.startpos[0].ستون;
			    اگر (subflags.do_نمبر || curwin->w_p_nu)
			    {
				صحیح numw = نمبر_چوڑائی(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    پیغام_آغاز();
			    برائے (i = 0; i < (طویل)sc; ++i)
				پیغام_putchar(' ');
			    برائے ( ; i <= (طویل)ec; ++i)
				پیغام_putchar('^');

			    resp = getexmodeline('?', تہی, 0, TRUE);
			    اگر (resp != تہی)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				اگر (ex_معمول_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			ورنہ
			{
			    string_T orig_سطر = {تہی, 0};
			    صحیح    len_change = 0;
			    صحیح	   save_p_lz = p_lz;
#اگر_تعریف_شدہ FEAT_FOLDING
			    صحیح save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#اگر_ختم
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    صحیح save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    اگر (new_آغاز.string != تہی)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_سطر.لمبائی = ml_get_len(lnum);
				orig_سطر.string = vim_strnsave(ml_get(lnum), orig_سطر.لمبائی);
				اگر (orig_سطر.string != تہی)
				{
				    string_T new_سطر;

				    new_سطر.string = concat_str(new_آغاز.string,
						     sub_firstline.string + copycol);
				    اگر (new_سطر.string == تہی)
					VIM_CLEAR_STRING(orig_سطر);
				    ورنہ
				    {
					new_سطر.لمبائی =
					    new_آغاز.لمبائی + (sub_firstline.لمبائی - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (صحیح)new_سطر.لمبائی - (صحیح)orig_سطر.لمبائی;
					curwin->w_cursor.ستون += len_change;
					ml_replace(lnum, new_سطر.string, FALSE);
				    }
				}
			    }

			    تلاش_مطابقت_سطریں = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    تلاش_مطابقت_endcol = regmatch.endpos[0].ستون
								 + len_change;
			    اگر (تلاش_مطابقت_سطریں == 0
						   && تلاش_مطابقت_endcol == 0)
				// highlight at least one character for /^/
				تلاش_مطابقت_endcol = 1;
			    highlight_مطابقت = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_مطابقت = FALSE;
			    redraw_later(UPD_SOME_VALID);

#اگر_تعریف_شدہ FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#اگر_ختم
			    اگر (پیغام_صف == صفیں - 1)
				پیغام_didout = FALSE;	// avoid a scroll-up
			    پیغام_starthere();
			    i = پیغام_scroll;
			    پیغام_scroll = 0;		// truncate msg when
							// needed
			    پیغام_no_مزید = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    پیغام_no_مزید = FALSE;
			    پیغام_scroll = i;
			    showruler(TRUE);
			    windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
			    RedrawingDisabled = save_RedrawingDisabled;

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#اگر_ختم
			    ++no_mapping;	// don't map this key
			    ++allow_کلیدیں;	// allow special keys
			    typed = plain_vgetc();
			    --allow_کلیدیں;
			    --no_mapping;

			    // clear the question
			    پیغام_didout = FALSE;	// don't scroll up
			    پیغام_ستون = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    اگر (orig_سطر.string != تہی)
				ml_replace(lnum, orig_سطر.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			اگر (typed == 'q' || typed == ESC || typed == Ctrl_C
#اگر_تعریف_شدہ UNIX
				|| typed == intr_char
#اگر_ختم
				)
			{
			    got_quit = TRUE;
			    توڑو;
			}
			اگر (typed == 'n')
			    توڑو;
			اگر (typed == 'y')
			    توڑو;
			اگر (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    توڑو;
			}
			اگر (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    توڑو;
			}
			اگر (typed == Ctrl_E)
			    scrollup_clamp();
			ورنہ اگر (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    اگر (vim_strchr(p_cpo, CPO_UNDO) != تہی)
			--no_u_sync;

		    اگر (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			اگر (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.لمبائی;
			    skip_مطابقت = TRUE;
			}
			جاؤ skip;
		    }
		    اگر (got_quit)
			جاؤ skip;
		}

		// Move the cursor to the start of the match, so that we can
		// use "\=col(".").
		curwin->w_cursor.ستون = regmatch.startpos[0].ستون;

		/*
		 * 3. substitute the string.
		 */
#اگر_تعریف_شدہ FEAT_EVAL
		save_ma = curbuf->b_p_ma;
		save_sandbox = sandbox;
		اگر (subflags.do_گنتی)
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
#اگر_ختم
		// Get length of substitution part, including the NUL.
		// When it fails sublen is zero.
		sublen = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, sub_firstline.string, 0,
		    REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));
#اگر_تعریف_شدہ FEAT_EVAL
		--textlock;

		// If getting the substitute string caused an error, don't do
		// the replacement.
		// Don't keep flags set by a recursive call.
		subflags = subflags_save;
		اگر (sublen == 0 || aborting() || subflags.do_گنتی)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    جاؤ skip;
		}
#اگر_ختم

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		اگر (nmatch > curbuf->b_ml.ml_سطر_گنتی - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_سطر_گنتی - sub_firstlnum + 1;
		    skip_مطابقت = TRUE;
		    // safety check
		    اگر (nmatch < 0)
			جاؤ skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		اگر (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.لمبائی = sub_firstline.لمبائی;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    اگر (curbuf->b_has_textprop)
		    {
			صحیح بائٹیں_added = (صحیح)sublen - 1 - (regmatch.endpos[0].ستون
						   - regmatch.startpos[0].ستون);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			اگر (adjust_prop_columns(lnum,
					کل_added + regmatch.startpos[0].ستون,
						       بائٹیں_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			کل_added += (colnr_T)بائٹیں_added;
		    }
#اگر_ختم
		}
		ورنہ
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    اگر (curbuf->b_has_textprop)
		    {
			char_u	*prop_آغاز;

			// Props in the first line may be shortened or deleted
			اگر (adjust_prop_columns(lnum,
					کل_added + regmatch.startpos[0].ستون,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			کل_added -=
			    (colnr_T)(sub_firstline.لمبائی - regmatch.startpos[0].ستون);

			// Props in the last line may be moved or deleted
			اگر (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].ستون, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			متن_prop_گنتی = get_متن_props(curbuf, lastlnum,
							   &prop_آغاز, FALSE);
			اگر (متن_prop_گنتی > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(متن_props);
			    متن_props = ALLOC_MULT(textprop_T,
							      متن_prop_گنتی);
			    اگر (متن_props != تہی)
			    {
				mch_memmove(متن_props, prop_آغاز,
					 متن_prop_گنتی * حجم(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				صحیح wi = 0;
				برائے (صحیح pi = 0; pi < متن_prop_گنتی; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    اگر (متن_props[pi].tp_id < 0
					    || (متن_props[pi].tp_flags
							& TP_FLAG_CONT_پچھلا))
					جاری;
				    متن_props[wi] = متن_props[pi];
				    متن_props[wi].tp_ستون +=
					regmatch.startpos[0].ستون + (colnr_T)sublen - 1;
				    متن_props[wi].u.tp_متن = تہی;
				    ++wi;
				}
				متن_prop_گنتی = wi;
			    }
			}
		    }
#اگر_ختم
		    tmp.string = ml_get(lastlnum);
		    tmp.لمبائی = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    اگر (curbuf->b_has_textprop)
			کل_added +=
			    (colnr_T)(tmp.لمبائی - regmatch.endpos[0].ستون);
#اگر_ختم
		}

		copy_len = (size_t)(regmatch.startpos[0].ستون - copycol);
		needed_سائز = copy_len
		    + (tmp.لمبائی - (size_t)regmatch.endpos[0].ستون) + sublen + 1;

		اگر (new_آغاز.string == تہی)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_آغاز_سائز = needed_سائز + 50;
		    اگر ((new_آغاز.string = alloc_clear(new_آغاز_سائز)) == تہی)
			جاؤ outofmem;
		    *new_آغاز.string = NUL;
		    new_آغاز.لمبائی = 0;
		}
		ورنہ
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_سائز += new_آغاز.لمبائی;
		    اگر (needed_سائز > new_آغاز_سائز)
		    {
			new_آغاز_سائز = needed_سائز + 50;
			اگر ((p1 = alloc_clear(new_آغاز_سائز)) == تہی)
			{
			    vim_free(new_آغاز.string);
			    جاؤ outofmem;
			}
			mch_memmove(p1, new_آغاز.string, new_آغاز.لمبائی + 1);
			vim_free(new_آغاز.string);
			new_آغاز.string = p1;
		    }
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_آغاز.string + new_آغاز.لمبائی,
		    sub_firstline.string + copycol, copy_len);
		new_آغاز.لمبائی += copy_len;

		// new text added here
		new_انتہا = new_آغاز.string + new_آغاز.لمبائی;

		اگر (new_آغاز_سائز - copy_len < sublen)
		    sublen = new_آغاز_سائز - copy_len - 1;

#اگر_تعریف_شدہ FEAT_EVAL
		++textlock;
#اگر_ختم
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_انتہا, (صحیح)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		اگر (n > 0)
		    new_آغاز.لمبائی += n - 1;	    // remove 1 for the NUL

#اگر_تعریف_شدہ FEAT_EVAL
		--textlock;
#اگر_ختم
		sub_nsubs++;
		did_sub = TRUE;

		// Move the cursor to the start of the line, to avoid that it
		// is beyond the end of the line after the substitution.
		curwin->w_cursor.ستون = 0;

		// For a multi-line match, make a copy of the last matched
		// line and continue in that one.
		اگر (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.لمبائی = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.لمبائی);
		    اگر (sub_firstline.string == تہی)
		    {
			vim_free(new_آغاز.string);
			جاؤ outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    اگر (sub_firstlnum <= line2)
			do_again = TRUE;
		    ورنہ
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].ستون;

		اگر (skip_مطابقت)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    اگر (sub_firstline.string == تہی)
		    {
			vim_free(new_آغاز.string);
			جاؤ outofmem;
		    }
		    sub_firstline.لمبائی = 0;
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
		برائے (p1 = new_انتہا; *p1; ++p1)
		{
		    اگر (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_آغاز.لمبائی - (p1 - new_آغاز.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_آغاز.لمبائی;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			اگر (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    اگر (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_آغاز.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#اگر_ختم
		    }
		    ورنہ اگر (*p1 == CAR)
		    {
			اگر (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_آغاز.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_آغاز.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    اگر (subflags.do_ask)
				appended_سطریں(lnum - 1, 1L);
			    ورنہ
			    {
				اگر (پہلا_سطر == 0)
				    پہلا_سطر = lnum;
				آخری_سطر = lnum + 1;
			    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			    adjust_props_for_بانٹو(lnum + 1, lnum,
							       plen, 1, FALSE);
#اگر_ختم
			    // all line numbers increase
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    did_بانٹو = true;
			    // move the cursor to the new line, like Vi
			    ++curwin->w_cursor.lnum;
			    // copy the rest
			    n = new_آغاز.لمبائی - (size_t)plen;
			    mch_memmove(new_آغاز.string, p1 + 1, n + 1);
			    new_آغاز.لمبائی = n;
			    p1 = new_آغاز.string - 1;
			}
		    }
		    ورنہ اگر (has_mbyte)
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
		lastone = (skip_مطابقت
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
		اگر (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_بانٹو)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, تہی)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    اگر (new_آغاز.string != تہی)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * "matchcol" has to be adjusted, we use the end of
			 * the line as reference, because the substitute may
			 * have changed the number of characters.  Same for
			 * "prev_matchcol".
			 */
			STRCPY(new_آغاز.string + new_آغاز.لمبائی,
			    sub_firstline.string + copycol);
			new_آغاز.لمبائی += sub_firstline.لمبائی - (size_t)copycol;
			matchcol = (colnr_T)(sub_firstline.لمبائی - matchcol);
			پچھلا_matchcol = (colnr_T)(sub_firstline.لمبائی
							      - پچھلا_matchcol);

			اگر (u_savesub(lnum) != OK)
			    توڑو;
			ml_replace(lnum, new_آغاز.string, TRUE);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			اگر (متن_props != تہی)
			    جوڑو_متن_props(lnum, متن_props, متن_prop_گنتی);
#اگر_ختم
			اگر (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    اگر (u_savedel(lnum, nmatch_tl) != OK)
				توڑو;
			    برائے (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    اگر (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (طویل)MAXLNUM, -nmatch_tl);
			    ورنہ
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (طویل)MAXLNUM, -nmatch_tl);
			    اگر (subflags.do_ask)
				deleted_سطریں(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			اگر (subflags.do_ask)
			    بدلا_بائٹیں(lnum, 0);
			ورنہ
			{
			    اگر (پہلا_سطر == 0)
				پہلا_سطر = lnum;
			    آخری_سطر = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_آغاز.string;
			sub_firstline.لمبائی = new_آغاز.لمبائی;
			new_آغاز.string = تہی;
			new_آغاز.لمبائی = 0;
			matchcol = (colnr_T)(sub_firstline.لمبائی - matchcol);
			پچھلا_matchcol = (colnr_T)(sub_firstline.لمبائی
							      - پچھلا_matchcol);
			copycol = 0;
			did_بانٹو = false;
		    }
		    اگر (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, تہی);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    اگر (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			اگر (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			توڑو;
		    }
		}

		سطر_breakcheck();
	    }

	    اگر (did_sub)
		++sub_nlines;
	    vim_free(new_آغاز.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	سطر_breakcheck();
    }

    اگر (پہلا_سطر != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_سطر_گنتی - old_سطر_گنتی;
	بدلا_سطریں(پہلا_سطر, 0, آخری_سطر - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    vim_free(متن_props);
#اگر_ختم

    // ":s/pat//n" doesn't move the cursor
    اگر (subflags.do_گنتی)
	curwin->w_cursor = old_cursor;

    اگر (sub_nsubs > آغاز_nsubs)
    {
	اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_آغاز.lnum = eap->line1;
	    curbuf->b_op_انتہا.lnum = line2;
	    curbuf->b_op_آغاز.ستون = curbuf->b_op_انتہا.ستون = 0;
	}

	اگر (!global_busy)
	{
	    // when interactive leave cursor on the match
	    اگر (!subflags.do_ask)
	    {
		اگر (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		ورنہ
		    beginline(BL_WHITE | BL_FIX);
	    }
	    اگر (!do_sub_پیغام(subflags.do_گنتی) && subflags.do_ask)
		پیغام("");
	}
	ورنہ
	    global_need_beginline = TRUE;
	اگر (subflags.do_print)
	    print_سطر(curwin->w_cursor.lnum,
					 subflags.do_نمبر, subflags.do_list);
    }
    ورنہ اگر (!global_busy)
    {
	اگر (got_int)		// interrupted
	    emsg(_(e_interrupted));
	ورنہ اگر (got_مطابقت)	// did find something but nothing substituted
	    پیغام("");
	ورنہ اگر (subflags.do_error)	// nothing found
	    semsg(_(e_نمونہ_not_ملا_str), get_تلاش_pat());
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (subflags.do_ask && hasAnyFolding(curwin))
	// Cursor position may require updating
	بدلا_کھڑکی_setting();
#اگر_ختم

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
    صحیح
do_sub_پیغام(
    صحیح	    گنتی_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    اگر (((sub_nsubs > p_report && (کلید_Typed || sub_nlines > 1 || p_report < 1))
		|| گنتی_only)
	    && messaging())
    {
	حرف	*پیغام_single;
	حرف	*پیغام_plural;

	اگر (got_int)
	    STRCPY(پیغام_buf, _("(Interrupted) "));
	ورنہ
	    *پیغام_buf = NUL;

	پیغام_single = گنتی_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	پیغام_plural = گنتی_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_جوڑو(پیغام_buf, حجم(پیغام_buf),
				 NGETTEXT(پیغام_single, پیغام_plural, sub_nlines),
				 sub_nsubs, (طویل)sub_nlines);

	اگر (پیغام(پیغام_buf))
	    // save message to display it after redraw
	    سیٹ_keep_پیغام((char_u *)پیغام_buf, 0);
	واپس TRUE;
    }
    اگر (got_int)
    {
	emsg(_(e_interrupted));
	واپس TRUE;
    }
    واپس FALSE;
}

    ساکن خالی
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ستون = 0;
    اگر (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", تہی, تہی, DOCMD_NOWAIT);
    ورنہ
	do_cmdline(cmd, تہی, تہی, DOCMD_NOWAIT);
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
    خالی
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    صحیح		ndone = 0;
    صحیح		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*مستعمل_pat;
    regmmatch_T	regmatch;
    صحیح		مطابقت;
    صحیح		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    اگر (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_سطر_گنتی))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	واپس;
    }

    اگر (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    ورنہ
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_آخری;	    // default: use last used regexp

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (اندر_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	واپس;
#اگر_ختم

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    اگر (*cmd == '\\')
    {
	++cmd;
	اگر (vim_strchr((char_u *)"/?&", *cmd) == تہی)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    واپس;
	}
	اگر (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	ورنہ
	    which_pat = RE_تلاش;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    ورنہ اگر (*cmd == NUL)
    {
	emsg(_(e_regular_عبارت_missing_from_global));
	واپس;
    }
    ورنہ اگر (check_regexp_delim(*cmd) == FAIL)
    {
	واپس;
    }
    ورنہ
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, تہی, تہی);
	اگر (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    اگر (تلاش_regcomp(pat, patlen, &مستعمل_pat, RE_BOTH, which_pat, تلاش_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	واپس;
    }

    اگر (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	مطابقت = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, تہی);
	اگر ((type == 'g' && مطابقت) || (type == 'v' && !مطابقت))
	    global_exe_one(cmd, lnum);
    }
    ورنہ
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	برائے (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    مطابقت = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, تہی);
	    اگر (regmatch.regprog == تہی)
		توڑو;  // re-compiling regprog failed
	    اگر ((type == 'g' && مطابقت) || (type == 'v' && !مطابقت))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    سطر_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	اگر (got_int)
	    پیغام(_(e_interrupted));
	ورنہ اگر (ndone == 0)
	{
	    اگر (type == 'v')
	    {
		اگر (اندر_vim9script())
		    semsg(_(e_نمونہ_ملا_اندر_every_سطر_str), مستعمل_pat);
		ورنہ
		    smsg(_("Pattern found in every line: %s"), مستعمل_pat);
	    }
	    ورنہ
	    {
		اگر (اندر_vim9script())
		    semsg(_(e_نمونہ_not_ملا_str), مستعمل_pat);
		ورنہ
		    smsg(_("Pattern not found: %s"), مستعمل_pat);
	    }
	}
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	    inc_clip_provider();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	    آغاز_global_changes();
#اگر_ختم
	    global_exe(cmd);
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	    انتہا_global_changes();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	    dec_clip_provider();
#اگر_ختم
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Execute "cmd" on lines marked with ml_setmarked().
 */
    خالی
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
    پیغام_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_سطر_گنتی;
    جب (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    اگر (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    ورنہ
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    بدلا_سطر_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    اگر (پیغام_ستون == 0 && پیغام_scrolled == 0)
	پیغام_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    اگر (!do_sub_پیغام(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_سطر_گنتی - old_lcount);
}

#اگر_تعریف_شدہ FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(خالی)
{
    واپس old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    خالی
سیٹ_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#اگر_ختم // FEAT_VIMINFO

#اگر defined(EXITFREE)
    خالی
free_old_sub(خالی)
{
    vim_free(old_sub);
}
#اگر_ختم

#اگر defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    صحیح
prepare_tagpreview(
    صحیح		undo_sync,	    // sync undo when leaving the window
    صحیح		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    اگر (curwin->w_p_pvw)
	واپس FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    اگر (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_ڈھونڈو_preview_کھڑکی();
	اگر (wp != تہی)
	    popup_سیٹ_wantpos_cursor(wp, wp->w_minwidth, تہی);
    }
    ورنہ اگر (use_popup != USEPOPUP_NONE)
    {
	wp = popup_ڈھونڈو_info_کھڑکی();
	اگر (wp != تہی)
	{
	    اگر (use_popup == USEPOPUP_معمول)
		popup_show(wp);
	    ورنہ
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ورنہ
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_p_pvw)
		توڑو;
    }
    اگر (wp != تہی)
    {
	win_enter(wp, undo_sync);
	واپس FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    اگر ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	واپس popup_create_preview_کھڑکی(use_popup != USEPOPUP_NONE);
# endif
    اگر (win_بانٹو(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	واپس FALSE;
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
    واپس TRUE;
}

#اگر_ختم

/*
 * Make the user happy.
 */
    خالی
ex_smile(exarg_T *eap غیر_مستعمل)
{
    ساکن حرف *رمز[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    حرف *p;
    صحیح n;
    صحیح i;

    پیغام_آغاز();
    پیغام_putchar('\n');
    برائے (i = 0; i < 2; ++i)
	برائے (p = رمز[i]; *p != NUL; ++p)
	    اگر (*p == 'x')
		پیغام_putchar('\n');
	    ورنہ
		برائے (n = *p++; n > 0; --n)
		    اگر (*p == 'o' || *p == '$')
			پیغام_putchar_attr(*p, HL_ATTR(HLF_L));
		    ورنہ
			پیغام_putchar(*p);
    پیغام_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    خالی
ex_drop(exarg_T *eap)
{
    صحیح		بانٹو = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    اگر (ERROR_IF_POPUP_کھڑکی || ERROR_IF_رکن_POPUP_کھڑکی)
	واپس;

    /*
     * Check if the first argument is already being edited in a window.  If
     * so, jump to that window.
     * We would actually need to check all arguments, but that's complicated
     * and mostly only one file is dropped.
     * This also ignores wildcards, since it is very unlikely the user is
     * editing a file name with a wildcard character.
     */
    سیٹ_arglist(eap->arg);

    /*
     * Expanding wildcards may result in an empty argument list.  E.g. when
     * editing "foo.pyc" and ".pyc" is in 'wildignore'.  Assume that we
     * already did an error message for this.
     */
    اگر (ARGCOUNT == 0)
	واپس;

    اگر (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	واپس;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (wp->w_بفر == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    اگر (!buf_Is_بدلا(curbuf))
	    {
		صحیح save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
		ex_rewind(eap);
	    واپس;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    اگر (!buf_hide(curbuf))
    {
	++emsg_off;
	بانٹو = check_بدلا(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    اگر (بانٹو)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    ورنہ
	eap->cmdidx = CMD_پہلا;
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
skip_vimgrep_pat(char_u *p, char_u **s, صحیح *flags)
{
    واپس skip_vimgrep_pat_ext(p, s, flags, تہی, تہی);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, صحیح *flags, char_u **nulp, صحیح *cp)
{
    صحیح		c;

    اگر (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	اگر (s != تہی)
	    *s = p;
	p = skiptowhite(p);
	اگر (s != تہی && *p != NUL)
	{
	    اگر (nulp != تہی)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    ورنہ
    {
	// ":vimgrep /pattern/[g][j] fname"
	اگر (s != تہی)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	اگر (*p != c)
	    واپس تہی;

	// Truncate the pattern.
	اگر (s != تہی)
	{
	    اگر (nulp != تہی)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	جب (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    اگر (flags != تہی)
	    {
		اگر (*p == 'g')
		    *flags |= VGR_GLOBAL;
		ورنہ اگر (*p == 'j')
		    *flags |= VGR_NOJUMP;
		ورنہ
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    واپس p;
}

#اگر defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    خالی
ex_oldfiles(exarg_T *eap غیر_مستعمل)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    صحیح		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    صحیح		matches = -1;

    اگر (l == تہی)
    {
	پیغام(_("No old files"));
	واپس;
    }

    پیغام_آغاز();
    پیغام_scroll = TRUE;
    برائے (li = l->lv_پہلا; li != تہی && !got_int; li = li->li_اگلا)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	اگر (!پیغام_filtered(fname))
	{
	    اگر (matches < 0)
		matches = nr;
	    ورنہ
		matches = 0;
	    پیغام_outnum((طویل)nr);
	    پیغام_puts(": ");
	    پیغام_outtrans(fname);
	    پیغام_clr_eos();
	    پیغام_putchar('\n');
	    برآمد_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_مزید = FALSE;
	// we only need to prompt if there is more than 1 match
	اگر (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	ورنہ
	    nr = prompt_for_نمبر(FALSE);
	پیغام_starthere();
	اگر (nr > 0)
	{
	    char_u *p = list_ڈھونڈو_str(get_vim_var_list(VV_OLDFILES),
		    (طویل)nr);

	    اگر (p != تہی)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, تہی);
		vim_free(p);
	    }
	}
    }
# endif
}
#اگر_ختم
