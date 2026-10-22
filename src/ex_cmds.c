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

#ضمن "vim.h"
#ضمن "version.h"

#ضمن <float.h>

ساكن صحيح linelen(صحيح *has_tab);
ساكن فراغ do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, صحيح do_اندريون, صحيح do_ٻاهريون);
ساكن صحيح not_writing(فراغ);
ساكن صحيح check_readonly(صحيح *forceit, buf_T *buf);
ساكن فراغ delbuf_نياپو(char_u *name);

/*
 * ":ascii" and "ga".
 */
    فراغ
do_ascii(exarg_T *eap اڻ_استعمال_ٿيل)
{
    صحيح		c;
    صحيح		cval;
    حرف	buf1[20];
    حرف	buf2[20];
    char_u	buf3[7];
#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
    char_u      *dig;
#جيڪڏهن_پورو
    صحيح		cc[MAX_MCO];
    صحيح		ci = 0;
    صحيح		len;

    إذا (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    وإلا
	c = gchar_cursor();
    إذا (c == NUL)
    {
	نياپو("NUL");
	أرجع;
    }

    IObuff[0] = NUL;
    إذا (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	إذا (c == NL)	    // NUL is stored as NL
	    c = NUL;
	إذا (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	وإلا
	    cval = c;
	إذا (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, حجم(buf1), "  <%s>", (حرف *)buf3);
	}
	وإلا
	    buf1[0] = NUL;
	إذا (c >= 0x80)
	    vim_snprintf(buf2, حجم(buf2), "  <M-%s>",
						 (حرف *)transchar(c & 0x7f));
	وإلا
	    buf2[0] = NUL;
#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	إذا (dig != عدم)
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	وإلا
#جيڪڏهن_پورو
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	إذا (enc_utf8)
	    c = cc[ci++];
	وإلا
	    c = 0;
    }

    // Repeat for combining characters.
    طالما (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (صحيح)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	إذا (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	إذا (enc_utf8 && utf_iscomposing(c)
#جيڪڏهن_وصف_ٿيل USE_GUI
		&& !gui.اندريون_use
#جيڪڏهن_پورو
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	إذا (dig != عدم)
	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	وإلا
#جيڪڏهن_پورو
	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	إذا (ci == MAX_MCO)
	    اكسر;
	إذا (enc_utf8)
	    c = cc[ci++];
	وإلا
	    c = 0;
    }

    نياپو((حرف *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    فراغ
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    صحيح		len;
    صحيح		indent = 0;
    صحيح		new_indent;
    صحيح		has_tab;
    صحيح		ويڪر;

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl)
    {
	// switch left and right aligning
	إذا (eap->cmdidx == CMD_ساڄو)
	    eap->cmdidx = CMD_کاٻو;
	وإلا إذا (eap->cmdidx == CMD_کاٻو)
	    eap->cmdidx = CMD_ساڄو;
    }
#جيڪڏهن_پورو

    ويڪر = atoi((حرف *)eap->arg);
    save_curpos = curwin->w_cursor;
    إذا (eap->cmdidx == CMD_کاٻو)    // width is used for new indent
    {
	إذا (ويڪر >= 0)
	    indent = ويڪر;
    }
    وإلا
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	إذا (ويڪر <= 0)
	    ويڪر = curbuf->b_p_tw;
	إذا (ويڪر == 0 && curbuf->b_p_wm > 0)
	    ويڪر = curwin->w_ويڪر - curbuf->b_p_wm;
	إذا (ويڪر <= 0)
	    ويڪر = 80;
    }

    إذا (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	أرجع;

    لكل (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	إذا (eap->cmdidx == CMD_کاٻو)		// left align
	    new_indent = indent;
	وإلا
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_ساڄو ? &has_tab
						   : عدم) - get_indent();

	    إذا (len <= 0)			// skip blank lines
		استمر;

	    إذا (eap->cmdidx == CMD_center)
		new_indent = (ويڪر - len) / 2;
	    وإلا
	    {
		new_indent = ويڪر - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		إذا (has_tab)
		    طالما (new_indent > 0)
		    {
			(فراغ)سيٽ_indent(new_indent, 0);
			إذا (linelen(عدم) <= ويڪر)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    افعل
				(فراغ)سيٽ_indent(++new_indent, 0);
			    طالما (linelen(عدم) <= ويڪر);
			    --new_indent;
			    اكسر;
			}
			--new_indent;
		    }
	    }
	}
	إذا (new_indent < 0)
	    new_indent = 0;
	(فراغ)سيٽ_indent(new_indent, 0);		// set indent
    }
    بدليو_سٽون(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    ساكن صحيح
linelen(صحيح *has_tab)
{
    char_u  *سٽ;
    char_u  *پهريون;
    char_u  *آخري;
    صحيح	    save;
    صحيح	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    سٽ = ml_get_curline();
    إذا (*سٽ == NUL)
	أرجع 0;

    // find the first non-blank character
    پهريون = skipwhite(سٽ);

    // find the character after the last non-blank character
    لكل (آخري = پهريون + STRLEN(پهريون);
				آخري > پهريون && VIM_ISWHITE(آخري[-1]); --آخري)
	;
    save = *آخري;
    *آخري = NUL;
    len = linetabsize_str(سٽ);	// get line length on screen
    إذا (has_tab != عدم)		// check for embedded TAB
	*has_tab = (vim_strchr(پهريون, TAB) != عدم);
    *آخري = save;

    أرجع len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
ساكن char_u	*sortbuf1;
ساكن char_u	*sortbuf2;

ساكن صحيح	ترتيب_ڏيو_lc;	// sort using locale
ساكن صحيح	ترتيب_ڏيو_ic;	// ignore case
ساكن صحيح	ترتيب_ڏيو_nr;	// sort on number
ساكن صحيح	ترتيب_ڏيو_rx;	// sort on regex instead of skipping it
ساكن صحيح	ترتيب_ڏيو_flt;	// sort on floating number

ساكن صحيح	ترتيب_ڏيو_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
تعريفنوع بنية
{
    linenr_T	lnum;			// line number
    اتحاد {
	بنية
	{
	    varnumber_T	شروعات_کانو_nr;	// starting column number
	    varnumber_T	پڇاڙي_کانو_nr;	// ending column number
	} سٽ;
	بنية
	{
	    varnumber_T	قدر;		// value if sorting by integer
	    صحيح is_عدد;		// TRUE when line contains a number
	} انگ;
	float_T قدر_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    ساكن صحيح
string_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    إذا (ترتيب_ڏيو_lc)
	أرجع strcoll((حرف *)s1, (حرف *)s2);
    أرجع ترتيب_ڏيو_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    ساكن صحيح
ترتيب_ڏيو_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    صحيح		نتيجو = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    إذا (ترتيب_ڏيو_abort)
	أرجع 0;
    fast_breakcheck();
    إذا (got_int)
	ترتيب_ڏيو_abort = TRUE;

    إذا (ترتيب_ڏيو_nr)
    {
	إذا (l1.st_u.انگ.is_عدد != l2.st_u.انگ.is_عدد)
	    نتيجو = l1.st_u.انگ.is_عدد > l2.st_u.انگ.is_عدد ? 1 : -1;
	وإلا
	    نتيجو = l1.st_u.انگ.قدر == l2.st_u.انگ.قدر ? 0
			     : l1.st_u.انگ.قدر > l2.st_u.انگ.قدر ? 1 : -1;
    }
    وإلا إذا (ترتيب_ڏيو_flt)
	نتيجو = l1.st_u.قدر_flt == l2.st_u.قدر_flt ? 0
			     : l1.st_u.قدر_flt > l2.st_u.قدر_flt ? 1 : -1;
    وإلا
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.سٽ.شروعات_کانو_nr,
		     l1.st_u.سٽ.پڇاڙي_کانو_nr - l1.st_u.سٽ.شروعات_کانو_nr + 1);
	sortbuf1[l1.st_u.سٽ.پڇاڙي_کانو_nr - l1.st_u.سٽ.شروعات_کانو_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.سٽ.شروعات_کانو_nr,
		     l2.st_u.سٽ.پڇاڙي_کانو_nr - l2.st_u.سٽ.شروعات_کانو_nr + 1);
	sortbuf2[l2.st_u.سٽ.پڇاڙي_کانو_nr - l2.st_u.سٽ.شروعات_کانو_nr] = 0;

	نتيجو = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    إذا (نتيجو == 0)
	أرجع (صحيح)(l1.lnum - l2.lnum);
    أرجع نتيجو;
}

/*
 * ":sort".
 */
    فراغ
ex_ترتيب_ڏيو(exarg_T *eap)
{
    regmatch_T	regmatch;
    صحيح		len;
    linenr_T	lnum;
    ڊگهو	maxlen = 0;
    sorti_T	*nrs;
    size_t	ڳڻپ = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    صحيح		unique = FALSE;
    ڊگهو	deleted;
    colnr_T	شروعات_کانو;
    colnr_T	پڇاڙي_کانو;
    صحيح		ترتيب_ڏيو_what = 0;
    صحيح		format_مليو = 0;
    صحيح		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    إذا (ڳڻپ <= 1)
	أرجع;

    إذا (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	أرجع;
    sortbuf1 = عدم;
    sortbuf2 = عدم;
    regmatch.regprog = عدم;
    nrs = ALLOC_MULT(sorti_T, ڳڻپ);
    إذا (nrs == عدم)
	اذهب sortend;

    ترتيب_ڏيو_abort = ترتيب_ڏيو_ic = ترتيب_ڏيو_lc = ترتيب_ڏيو_rx = ترتيب_ڏيو_nr = 0;
    ترتيب_ڏيو_flt = 0;

    لكل (p = eap->arg; *p != NUL; ++p)
    {
	إذا (VIM_ISWHITE(*p))
	    ;
	وإلا إذا (*p == 'i')
	    ترتيب_ڏيو_ic = TRUE;
	وإلا إذا (*p == 'l')
	    ترتيب_ڏيو_lc = TRUE;
	وإلا إذا (*p == 'r')
	    ترتيب_ڏيو_rx = TRUE;
	وإلا إذا (*p == 'n')
	{
	    ترتيب_ڏيو_nr = 1;
	    ++format_مليو;
	}
	وإلا إذا (*p == 'f')
	{
	    ترتيب_ڏيو_flt = 1;
	    ++format_مليو;
	}
	وإلا إذا (*p == 'b')
	{
	    ترتيب_ڏيو_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_مليو;
	}
	وإلا إذا (*p == 'o')
	{
	    ترتيب_ڏيو_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_مليو;
	}
	وإلا إذا (*p == 'x')
	{
	    ترتيب_ڏيو_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_مليو;
	}
	وإلا إذا (*p == 'u')
	    unique = TRUE;
	وإلا إذا (*p == '"')	// comment start
	    اكسر;
	وإلا إذا (eap->nextcmd == عدم && check_nextcmd(p) != عدم)
	{
	    eap->nextcmd = check_nextcmd(p);
	    اكسر;
	}
	وإلا إذا (!ASCII_ISALPHA(*p) && regmatch.regprog == عدم)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    إذا (s == عدم)
		اذهب sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    إذا (s == p + 1)
	    {
		إذا (آخري_ڳولا_pat() == عدم)
		{
		    emsg(_(e_no_اڳيون_regular_اظهار));
		    اذهب sortend;
		}
		regmatch.regprog = vim_regcomp(آخري_ڳولا_pat(), RE_MAGIC);
	    }
	    وإلا
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    إذا (regmatch.regprog == عدم)
		اذهب sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	وإلا
	{
	    semsg(_(e_invalid_دليل_str), p);
	    اذهب sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    إذا (format_مليو > 1)
    {
	emsg(_(e_invalid_دليل));
	اذهب sortend;
    }

    // From here on "sort_nr" is used as a flag for any integer number
    // sorting.
    ترتيب_ڏيو_nr += ترتيب_ڏيو_what;

    /*
     * Make an array with all line numbers.  This avoids having to copy all
     * the lines into allocated memory.
     * When sorting on strings "start_col_nr" is the offset in the line, for
     * numbers sorting it's the number to sort on.  This means the pattern
     * matching and number conversion only has to be done once per line.
     * Also get the longest line length for allocating "sortbuf".
     */
    لكل (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	إذا (maxlen < len)
	    maxlen = len;

	شروعات_کانو = 0;
	پڇاڙي_کانو = len;
	إذا (regmatch.regprog != عدم && vim_regexec(&regmatch, s, 0))
	{
	    إذا (ترتيب_ڏيو_rx)
	    {
		شروعات_کانو = (colnr_T)(regmatch.startp[0] - s);
		پڇاڙي_کانو = (colnr_T)(regmatch.endp[0] - s);
	    }
	    وإلا
		شروعات_کانو = (colnr_T)(regmatch.endp[0] - s);
	}
	وإلا
	    إذا (regmatch.regprog != عدم)
		پڇاڙي_کانو = 0;

	إذا (ترتيب_ڏيو_nr || ترتيب_ڏيو_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + پڇاڙي_کانو;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + شروعات_کانو;
	    إذا (ترتيب_ڏيو_nr)
	    {
		إذا (ترتيب_ڏيو_what & STR2NR_HEX)
		    s = skiptohex(p);
		وإلا إذا (ترتيب_ڏيو_what & STR2NR_BIN)
		    s = skiptobin(p);
		وإلا
		    s = skiptodigit(p);
		إذا (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		إذا (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.انگ.is_عدد = FALSE;
		    nrs[lnum - eap->line1].st_u.انگ.قدر = 0;
		}
		وإلا
		{
		    nrs[lnum - eap->line1].st_u.انگ.is_عدد = TRUE;
		    vim_str2nr(s, عدم, عدم, ترتيب_ڏيو_what,
			&nrs[lnum - eap->line1].st_u.انگ.قدر,
			عدم, 0, FALSE, عدم);
		}
	    }
	    وإلا
	    {
		s = skipwhite(p);
		إذا (*s == '+')
		    s = skipwhite(s + 1);

		إذا (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.قدر_flt = -DBL_MAX;
		وإلا
		    nrs[lnum - eap->line1].st_u.قدر_flt =
						      strtod((حرف *)s, عدم);
	    }
	    *s2 = c;
	}
	وإلا
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.سٽ.شروعات_کانو_nr = شروعات_کانو;
	    nrs[lnum - eap->line1].st_u.سٽ.پڇاڙي_کانو_nr = پڇاڙي_کانو;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	إذا (regmatch.regprog != عدم)
	    fast_breakcheck();
	إذا (got_int)
	    اذهب sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    إذا (sortbuf1 == عدم)
	اذهب sortend;
    sortbuf2 = alloc(maxlen + 1);
    إذا (sortbuf2 == عدم)
	اذهب sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((فراغ *)nrs, ڳڻپ, حجم(sorti_T), ترتيب_ڏيو_compare);

    إذا (ترتيب_ڏيو_abort)
	اذهب sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    لكل (i = 0; i < ڳڻپ; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? ڳڻپ - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	إذا (get_lnum + ((linenr_T)ڳڻپ - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	إذا (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    إذا (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		اكسر;
	}
	fast_breakcheck();
	إذا (got_int)
	    اذهب sortend;
    }

    // delete the original lines if appending worked
    إذا (i == ڳڻپ)
	لكل (i = 0; i < ڳڻپ; ++i)
	    ml_delete(eap->line1);
    وإلا
	ڳڻپ = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (ڊگهو)(ڳڻپ - (lnum - eap->line2));
    إذا (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (ڊگهو)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    وإلا إذا (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    إذا (change_occurred || deleted != 0)
	بدليو_سٽون(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    إذا (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    فراغ
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    صحيح		len;
    linenr_T	lnum;
    ڊگهو	maxlen = 0;
    linenr_T	ڳڻپ = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    صحيح		keep_only_unique = FALSE;
    صحيح		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    ڊگهو	deleted = 0;
    colnr_T	شروعات_کانو;
    colnr_T	پڇاڙي_کانو;
    صحيح		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    إذا (ڳڻپ <= 1)
	أرجع;

    إذا (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	أرجع;
    sortbuf1 = عدم;
    regmatch.regprog = عدم;

    ترتيب_ڏيو_abort = ترتيب_ڏيو_ic = ترتيب_ڏيو_lc = ترتيب_ڏيو_rx = ترتيب_ڏيو_nr = 0;
    ترتيب_ڏيو_flt = 0;

    لكل (p = eap->arg; *p != NUL; ++p)
    {
	إذا (VIM_ISWHITE(*p))
	    ;
	وإلا إذا (*p == 'i')
	    ترتيب_ڏيو_ic = TRUE;
	وإلا إذا (*p == 'l')
	    ترتيب_ڏيو_lc = TRUE;
	وإلا إذا (*p == 'r')
	    ترتيب_ڏيو_rx = TRUE;
	وإلا إذا (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    إذا (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	وإلا إذا (*p == '"')	// comment start
	    اكسر;
	وإلا إذا (eap->nextcmd == عدم && check_nextcmd(p) != عدم)
	{
	    eap->nextcmd = check_nextcmd(p);
	    اكسر;
	}
	وإلا إذا (!ASCII_ISALPHA(*p) && regmatch.regprog == عدم)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    إذا (s == عدم)
		اذهب uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    إذا (s == p + 1)
	    {
		إذا (آخري_ڳولا_pat() == عدم)
		{
		    emsg(_(e_no_اڳيون_regular_اظهار));
		    اذهب uniqend;
		}
		regmatch.regprog = vim_regcomp(آخري_ڳولا_pat(), RE_MAGIC);
	    }
	    وإلا
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    إذا (regmatch.regprog == عدم)
		اذهب uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	وإلا
	{
	    semsg(_(e_invalid_دليل_str), p);
	    اذهب uniqend;
	}
    }

    // Find the length of the longest line.
    لكل (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	إذا (maxlen < len)
	    maxlen = len;

	إذا (got_int)
	    اذهب uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    إذا (sortbuf1 == عدم)
	اذهب uniqend;

    // Delete lines according to options.
    صحيح ميلاپ_continue = FALSE;
    صحيح ايندڙ_is_unmatch = FALSE;
    صحيح is_ميلاپ;
    linenr_T پورو_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    لكل (linenr_T i = 0; i < ڳڻپ; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	شروعات_کانو = 0;
	پڇاڙي_کانو = len;
	إذا (regmatch.regprog != عدم && vim_regexec(&regmatch, s, 0))
	{
	    إذا (ترتيب_ڏيو_rx)
	    {
		شروعات_کانو = (colnr_T)(regmatch.startp[0] - s);
		پڇاڙي_کانو = (colnr_T)(regmatch.endp[0] - s);
	    }
	    وإلا
		شروعات_کانو = (colnr_T)(regmatch.endp[0] - s);
	}
	وإلا
	    إذا (regmatch.regprog != عدم)
		پڇاڙي_کانو = 0;
	إذا (پڇاڙي_کانو > 0)
	{
	    save_c = s[پڇاڙي_کانو];
	    s[پڇاڙي_کانو] = NUL;
	}

	is_ميلاپ = i > 0 ? !string_compare(&s[شروعات_کانو], sortbuf1) : FALSE;
	delete_lnum = 0;
	إذا (ايندڙ_is_unmatch)
	{
	    is_ميلاپ = FALSE;
	    ايندڙ_is_unmatch = FALSE;
	}

	إذا (!keep_only_unique && !keep_only_not_unique)
	{
	    إذا (is_ميلاپ)
		delete_lnum = get_lnum;
	    وإلا
		STRCPY(sortbuf1, &s[شروعات_کانو]);
	}
	وإلا إذا (keep_only_not_unique)
	{
	    إذا (is_ميلاپ)
	    {
		پورو_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		ميلاپ_continue = TRUE;
	    }
	    وإلا
	    {
		إذا (i > 0 && !ميلاپ_continue && get_lnum - 1 > پورو_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    ايندڙ_is_unmatch = TRUE;
		}
		وإلا إذا (i >= ڳڻپ - 1)
		    delete_lnum = get_lnum;
		ميلاپ_continue = FALSE;
		STRCPY(sortbuf1, &s[شروعات_کانو]);
	    }
	}
	وإلا // keep_only_unique
	{
	    إذا (is_ميلاپ)
	    {
		إذا (!ميلاپ_continue)
		    delete_lnum = get_lnum - 1;
		وإلا
		    delete_lnum = get_lnum;
		ميلاپ_continue = TRUE;
	    }
	    وإلا
	    {
		إذا (i == 0 && ميلاپ_continue)
		    delete_lnum = get_lnum;
		ميلاپ_continue = FALSE;
		STRCPY(sortbuf1, &s[شروعات_کانو]);
	    }
	}

	إذا (پڇاڙي_کانو > 0)
	    s[پڇاڙي_کانو] = save_c;

	إذا (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    ڳڻپ--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	إذا (got_int)
	    اذهب uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (ڊگهو)MAXLNUM, -deleted);
    msgmore(-deleted);

    إذا (change_occurred)
	بدليو_سٽون(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    إذا (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	انگ_سٽون;  // Num lines moved
    linenr_T	آخري_سٽ;  // Last line in file after adding new text
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#جيڪڏهن_پورو

    إذا (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_جو_سٽون_into_itself));
	أرجع FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    إذا (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	إذا (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	وإلا
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	أرجع OK;
    }

    انگ_سٽون = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    إذا (u_save(dest, dest + 1) == FAIL)
	أرجع FAIL;
    لكل (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	إذا (str != عدم)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    إذا (dest < line1)
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
    آخري_سٽ = curbuf->b_ml.ml_سٽ_ڳڻپ;
    mark_adjust_nofold(line1, line2, آخري_سٽ - line2, 0L);
    إذا (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -انگ_سٽون, 0L);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    إذا (win->w_بفر == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#جيڪڏهن_پورو
	إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_شروعات.lnum = dest - انگ_سٽون + 1;
	    curbuf->b_op_پڇاڙي.lnum = dest;
	}
    }
    وإلا
    {
	mark_adjust_nofold(dest + 1, line1 - 1, انگ_سٽون, 0L);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    إذا (win->w_بفر == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#جيڪڏهن_پورو
	إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_شروعات.lnum = dest + 1;
	    curbuf->b_op_پڇاڙي.lnum = dest + انگ_سٽون;
	}
    }
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_شروعات.کانو = curbuf->b_op_پڇاڙي.کانو = 0;
    mark_adjust_nofold(آخري_سٽ - انگ_سٽون + 1, آخري_سٽ,
					     -(آخري_سٽ - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    إذا (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	أرجع FAIL;

    لكل (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_پيغام);

    إذا (!global_busy && انگ_سٽون > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", انگ_سٽون),
			(ڊگهو)انگ_سٽون);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    إذا (dest >= line1)
	curwin->w_cursor.lnum = dest;
    وإلا
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    إذا (line1 < dest)
    {
	dest += انگ_سٽون + 1;
	آخري_سٽ = curbuf->b_ml.ml_سٽ_ڳڻپ;
	إذا (dest > آخري_سٽ + 1)
	    dest = آخري_سٽ + 1;
	بدليو_سٽون(line1, 0, dest, 0L);
    }
    وإلا
	بدليو_سٽون(dest + 1, 0, line1 + انگ_سٽون, 0L);

    أرجع OK;
}

/*
 * ":copy"
 */
    فراغ
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	ڳڻپ;
    char_u	*p;

    ڳڻپ = line2 - line1 + 1;
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_شروعات.lnum = n + 1;
	curbuf->b_op_پڇاڙي.lnum = n + ڳڻپ;
	curbuf->b_op_شروعات.کانو = curbuf->b_op_پڇاڙي.کانو = 0;
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
    إذا (u_save(n, n + 1) == FAIL)
	أرجع;

    curwin->w_cursor.lnum = n;
    طالما (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	إذا (p != عدم)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	إذا (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	إذا (curwin->w_cursor.lnum < line1)
	    ++line1;
	إذا (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_سٽون_mark(n, ڳڻپ);
    إذا (VIsual_سرگرم)
	check_pos(curbuf, &VIsual);

    msgmore((ڊگهو)ڳڻپ);
}

ساكن char_u	*prevcmd = عدم;	// the previous command

#إذا defined(EXITFREE)
    فراغ
free_prev_shellcmd(فراغ)
{
    vim_free(prevcmd);
}
#جيڪڏهن_پورو

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    ساكن صحيح
prevcmd_is_سيٽ(فراغ)
{
    إذا (prevcmd == عدم)
    {
	emsg(_(e_no_اڳيون_command));
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    فراغ
do_bang(
    صحيح		addr_ڳڻپ,
    exarg_T	*eap,
    صحيح		forceit,
    صحيح		do_اندريون,
    صحيح		do_ٻاهريون)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = عدم;		// the new command
    صحيح			free_newcmd = FALSE;    // need to free() newcmd
    صحيح			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    صحيح			len;
    صحيح			scroll_save = نياپو_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (addr_ڳڻپ == 0)		// :!
    {
	نياپو_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	نياپو_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    افعل
    {
	len = (صحيح)STRLEN(trailarg) + 1;
	إذا (newcmd != عدم)
	    len += (صحيح)STRLEN(newcmd);
	إذا (ins_prevcmd)
	{
	    إذا (!prevcmd_is_سيٽ())
	    {
		vim_free(newcmd);
		أرجع;
	    }
	    len += (صحيح)STRLEN(prevcmd);
	}
	إذا ((t = alloc(len)) == عدم)
	{
	    vim_free(newcmd);
	    أرجع;
	}
	*t = NUL;
	إذا (newcmd != عدم)
	    STRCAT(t, newcmd);
	إذا (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = عدم;
	طالما (*p)
	{
	    إذا (*p == '!')
	    {
		إذا (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		وإلا
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    اكسر;
		}
	    }
	    ++p;
	}
    } طالما (trailarg != عدم);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    إذا (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    وإلا
	free_newcmd = TRUE;

    إذا (bangredo)	    // put cmd in redo buffer for ! command
    {
	إذا (!prevcmd_is_سيٽ())
	    اذهب theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	إذا (cmd != عدم)
	{
	    Append_منزل_Redobuff_Lit(cmd, -1);
	    vim_free(cmd);
	}
	وإلا
	    Append_منزل_Redobuff_Lit(prevcmd, -1);
	Append_منزل_Redobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    إذا (*p_shq != NUL)
    {
	إذا (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	إذا (newcmd == عدم)
	    أرجع;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    إذا (addr_ڳڻپ == 0)		// :!
    {
	// echo the command
	نياپو_شروعات();
	نياپو_putchar(':');
	نياپو_putchar('!');
	نياپو_outtrans(newcmd);
	نياپو_clr_eos();
	windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);

	do_shell(newcmd, 0);
    }
    وإلا				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_اندريون, do_ٻاهريون);
	apply_autocmds(EVENT_SHELLFILTERPOST, عدم, عدم, FALSE, curbuf);
    }

theend:
    إذا (free_newcmd)
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
    ساكن فراغ
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    صحيح		do_اندريون,
    صحيح		do_ٻاهريون)
{
    char_u	*itmp = عدم;
    char_u	*otmp = عدم;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    صحيح		shell_flags = 0;
    pos_T	orig_شروعات = curbuf->b_op_شروعات;
    pos_T	orig_پڇاڙي = curbuf->b_op_پڇاڙي;
    صحيح		save_cmod_flags = cmdmod.cmod_flags;
#جيڪڏهن_وصف_ٿيل FEAT_FILTERPIPE
    صحيح		stmp = p_stmp;
#جيڪڏهن_پورو

    إذا (*cmd == NUL)	    // no filter command
	أرجع;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.کانو = 0;
    بدليو_سٽ_abv_curs();
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

    إذا (do_ٻاهريون)
	shell_flags |= SHELL_DOOUT;

#جيڪڏهن_وصف_ٿيل FEAT_FILTERPIPE
# ifdef VIMDLL
    إذا (!gui.اندريون_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    إذا (!do_اندريون && do_ٻاهريون && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    وإلا إذا (do_اندريون && !do_ٻاهريون && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_شروعات.lnum = line1;
	curbuf->b_op_پڇاڙي.lnum = line2;
    }
    وإلا إذا (do_اندريون && do_ٻاهريون && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_شروعات.lnum = line1;
	curbuf->b_op_پڇاڙي.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    وإلا
#جيڪڏهن_پورو
	إذا ((do_اندريون && (itmp = vim_tempname('i', FALSE)) == عدم)
		|| (do_ٻاهريون && (otmp = vim_tempname('o', FALSE)) == عدم))
	{
	    emsg(_(e_cant_get_عارضي_file_name));
	    اذهب filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    إذا (itmp != عدم && buf_write(curbuf, itmp, عدم, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	نياپو_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#إذا defined(FEAT_EVAL)
	إذا (!aborting())
#جيڪڏهن_پورو
	    // will call wait_return()
	    (فراغ)semsg(_(e_cant_create_file_str), itmp);
	اذهب filterend;
    }
    إذا (curbuf != old_curbuf)
	اذهب filterend;

    إذا (!do_ٻاهريون)
	نياپو_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    إذا (cmd_buf == عدم)
	اذهب filterend;

    windgoto((صحيح)صفون - 1, cmdline_کانو_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    إذا (!do_ٻاهريون || STRCMP(p_srr, ">") == 0 || !do_اندريون)
	redraw_later_clear();

    إذا (do_ٻاهريون)
    {
	إذا (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    اذهب error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_سٽ_ڳڻپ;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    إذا (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    إذا (do_ٻاهريون)
    {
	إذا (otmp != عدم)
	{
	    إذا (readfile(otmp, عدم, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#إذا defined(FEAT_EVAL)
		إذا (!aborting())
#جيڪڏهن_پورو
		{
		    نياپو_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		اذهب error;
	    }
	    إذا (curbuf != old_curbuf)
		اذهب filterend;
	}

	read_linecount = curbuf->b_ml.ml_سٽ_ڳڻپ - read_linecount;

	إذا (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_شروعات.lnum = line2 + 1;
	    curbuf->b_op_پڇاڙي.lnum = curwin->w_cursor.lnum;
	    appended_سٽون_mark(line2, read_linecount);
	}

	إذا (do_اندريون)
	{
	    إذا ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == عدم)
	    {
		إذا (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		وإلا إذا (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		وإلا
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
	    del_سٽون(linecount, TRUE);
	    إذا (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_شروعات.lnum = curbuf->b_op_پڇاڙي.lnum =
					MIN(line1, curbuf->b_ml.ml_سٽ_ڳڻپ);
		curbuf->b_op_شروعات.کانو = curbuf->b_op_پڇاڙي.کانو = 0;
	    }
	    وإلا
	    {
		curbuf->b_op_شروعات.lnum -= linecount;	// adjust '[
		curbuf->b_op_پڇاڙي.lnum -= linecount;	// adjust ']
	    }
	    write_lnum_adjust(-linecount);		// adjust last line
							// for next write
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    foldUpdate(curwin, curbuf->b_op_شروعات.lnum, curbuf->b_op_پڇاڙي.lnum);
#جيڪڏهن_پورو
	}
	وإلا
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_پڇاڙي.lnum - curbuf->b_op_شروعات.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_پڇاڙي.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	إذا (linecount > p_report)
	{
	    إذا (do_اندريون)
	    {
		vim_snprintf(نياپو_buf, حجم(نياپو_buf),
				    _("%ld lines filtered"), (ڊگهو)linecount);
		إذا (نياپو(نياپو_buf) && !نياپو_scroll)
		    // save message to display it after redraw
		    سيٽ_keep_نياپو((char_u *)نياپو_buf, 0);
	    }
	    وإلا
		msgmore((ڊگهو)linecount);
	}
    }
    وإلا
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    إذا (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_بفر));
    }
    وإلا إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_شروعات = orig_شروعات;
	curbuf->b_op_پڇاڙي = orig_پڇاڙي;
    }

    إذا (itmp != عدم)
	mch_remove(itmp);
    إذا (otmp != عدم)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    فراغ
do_shell(
    char_u	*cmd,
    صحيح		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    صحيح		save_nwr;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
    صحيح		winstart = FALSE;
#جيڪڏهن_پورو
    صحيح		keep_termcap = !termcap_سرگرم;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    إذا (check_restricted() || check_secure())
    {
	نياپو_پڇاڙي();
	أرجع;
    }

#جيڪڏهن_وصف_ٿيل MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    إذا (cmd != عدم)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    إذا (gui.اندريون_use && vim_strchr(p_go, GO_TERMINAL) != عدم)
	keep_termcap = TRUE;
# endif
#جيڪڏهن_پورو

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    نياپو_putchar('\r');			// put cursor at start of line
    إذا (!autocmd_busy)
    {
	إذا (!keep_termcap)
	    stoptermcap();
    }
#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (!winstart)
#جيڪڏهن_پورو
	نياپو_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    إذا (p_warn && !autocmd_busy && نياپو_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    إذا (buf_Is_بدليو_Not_رڪن(buf))
	    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
		إذا (!keep_termcap)
		    starttermcap();	// don't want a message box here
#جيڪڏهن_پورو
		نياپو_puts(_("[No write since last change]\n"));
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
		إذا (!keep_termcap)
		    stoptermcap();
#جيڪڏهن_پورو
		اكسر;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    إذا (!swapping_screen())
	windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
    cursor_on();
    (فراغ)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    إذا (!swapping_screen())
    {
	نياپو_صف = صفون - 1;
	نياپو_کانو = 0;
    }

    إذا (autocmd_busy)
    {
	إذا (نياپو_silent == 0)
	    redraw_later_clear();
    }
    وإلا
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	إذا (!gui.اندريون_use)
# endif
	{
	    إذا (cmd == عدم
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		إذا (نياپو_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    وإلا
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		إذا (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		صحيح redraw = نياپو_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		إذا (gui.اندريون_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(رڪن_console ? -1 : redraw); // see below
# وإلا
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#جيڪڏهن_پورو // FEAT_GUI_MSWIN

	إذا (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#جيڪڏهن_وصف_ٿيل AMIGA
	إذا (skip_redraw)		// ':' hit in wait_return()
	{
	    إذا (نياپو_silent == 0)
		redraw_later_clear();
	}
	وإلا إذا (رڪن_console)
	{
	    ٻاهريون_STR("\033[0 q");	// get window size
	    إذا (got_int && نياپو_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    وإلا
		must_redraw = 0;	// no extra redraw needed
	}
#جيڪڏهن_پورو
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, عدم, عدم, FALSE, curbuf);
}

#إذا !defined(UNIX)
    ساكن char_u *
ڳوليو_pipe(char_u *cmd)
{
    char_u  *p;
    صحيح	    inquote = FALSE;

    لكل (p = cmd; *p != NUL; ++p)
    {
	إذا (!inquote && *p == '|')
	    أرجع p;
	إذا (*p == '"')
	    inquote = !inquote;
	وإلا إذا (rem_backslash(p))
	    ++p;
    }
    أرجع عدم;
}
#جيڪڏهن_پورو

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
    صحيح		is_powershell = FALSE;
#جيڪڏهن_وصف_ٿيل UNIX
    صحيح		is_fish_shell;
#جيڪڏهن_پورو

    char_u *shell_name = get_isolated_shell_name();
    إذا (shell_name == عدم)
	أرجع عدم;

#إذا defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    إذا (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    وإلا
#جيڪڏهن_پورو
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	إذا (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	وإلا
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    إذا (itmp != عدم)
    {
	إذا (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	وإلا
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    إذا (otmp != عدم)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    إذا (buf == عدم)
	أرجع عدم;

    إذا (is_powershell)
    {
	إذا (itmp != عدم)
	    vim_snprintf((حرف *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	وإلا
	    vim_snprintf((حرف *)buf, len, "& { %s }", cmd);
    }
    وإلا
    {
#إذا defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	إذا (itmp != عدم || otmp != عدم)
	{
	    إذا (is_fish_shell)
		vim_snprintf((حرف *)buf, len, "begin; %s; end", (حرف *)cmd);
	    وإلا
		vim_snprintf((حرف *)buf, len, "(%s)", (حرف *)cmd);
	}
	وإلا
	    STRCPY(buf, cmd);
	إذا (itmp != عدم)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#وإلا
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	إذا (*p_sxe != NUL && *p_sxq == '(')
	{
	    إذا (itmp != عدم || otmp != عدم)
		vim_snprintf((حرف *)buf, len, "(%s)", (حرف *)cmd);
	    وإلا
		STRCPY(buf, cmd);
	    إذا (itmp != عدم)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	وإلا
	{
	    STRCPY(buf, cmd);
	    إذا (itmp != عدم)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		إذا (*p_shq == NUL)
		{
		    p = ڳوليو_pipe(buf);
		    إذا (p != عدم)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		إذا (*p_shq == NUL)
		{
		    p = ڳوليو_pipe(cmd);
		    إذا (p != عدم)
		    {
			// insert a space before the '|' for DOS
			STRCAT(buf, " ");
			STRCAT(buf, p);
		    }
		}
	    }
	}
#جيڪڏهن_پورو
    }
    إذا (otmp != عدم)
	append_redir(buf, (صحيح)len, p_srr, otmp);

    أرجع buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    فراغ
append_redir(
    char_u	*buf,
    صحيح		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*پڇاڙي;

    پڇاڙي = buf + STRLEN(buf);
    // find "%s"
    لكل (p = opt; (p = vim_strchr(p, '%')) != عدم; ++p)
    {
	إذا (p[1] == 's') // found %s
	    اكسر;
	إذا (p[1] == '%') // skip %%
	    ++p;
    }
    إذا (p != عدم)
    {
#جيڪڏهن_وصف_ٿيل MSWIN
	*پڇاڙي++ = ' '; // not really needed? Not with sh, ksh or bash
#جيڪڏهن_پورو
	vim_snprintf((حرف *)پڇاڙي, (size_t)(buflen - (پڇاڙي - buf)),
						  (حرف *)opt, (حرف *)fname);
    }
    وإلا
	vim_snprintf((حرف *)پڇاڙي, (size_t)(buflen - (پڇاڙي - buf)), " %s %s",
		(حرف *)opt, (حرف *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    فراغ
do_fixdel(exarg_T *eap اڻ_استعمال_ٿيل)
{
    char_u  *p;

    p = ڳوليو_termcode((char_u *)"kb");
    جوڙيو_termcode((char_u *)"kD", p != عدم
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    فراغ
print_سٽ_no_اڳياڙي(
    linenr_T	lnum,
    صحيح		use_عدد,
    صحيح		list)
{
    حرف	numbuf[30];

    إذا (curwin->w_p_nu || use_عدد)
    {
	vim_snprintf(numbuf, حجم(numbuf),
				   "%*ld ", عدد_ويڪر(curwin), (ڊگهو)lnum);
	نياپو_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    نياپو_prt_سٽ(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    فراغ
print_سٽ(linenr_T lnum, صحيح use_عدد, صحيح list)
{
    صحيح		save_silent = silent_گهڻائي;

    // apply :filter /pat/
    إذا (پيغام_filtered(ml_get(lnum)))
	أرجع;

    نياپو_شروعات();
    silent_گهڻائي = FALSE;
    info_پيغام = TRUE;	// use mch_msg(), not mch_errmsg()
    print_سٽ_no_اڳياڙي(lnum, use_عدد, list);
    إذا (save_silent)
    {
	نياپو_putchar('\n');
	cursor_on();		// msg_start() switches it off
	ٻاهريون_flush();
	silent_گهڻائي = save_silent;
    }
    info_پيغام = FALSE;
}

    صحيح
rename_بفر(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);
    // buffer changed, don't change name now
    إذا (buf != curbuf)
	أرجع FAIL;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (aborting())	    // autocmds may abort script processing
	أرجع FAIL;
#جيڪڏهن_پورو
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
    curbuf->b_ffname = عدم;
    curbuf->b_sfname = عدم;
    إذا (setfname(curbuf, new_fname, عدم, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	أرجع FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    إذا (xfname != عدم && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	إذا (buf != عدم && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, عدم, عدم, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    أرجع OK;
}

/*
 * ":file[!] [fname]".
 */
    فراغ
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    إذا (eap->addr_ڳڻپ > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_ڳڻپ > 1))
    {
	emsg(_(e_invalid_دليل));
	أرجع;
    }

    إذا (*eap->arg != NUL || eap->addr_ڳڻپ == 1)
    {
	إذا (rename_بفر(eap->arg) == FAIL)
	    أرجع;
	redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#جيڪڏهن_پورو
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    إذا (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    فراغ
ex_update(exarg_T *eap)
{
    إذا (curbuf_Is_بدليو())
	(فراغ)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    فراغ
ex_write(exarg_T *eap)
{
    إذا (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_سٽ_ڳڻپ;
    }

    إذا (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    وإلا
	(فراغ)do_write(eap);
}

#جيڪڏهن_وصف_ٿيل UNIX
    ساكن صحيح
check_writable(char_u *fname)
{
    إذا (mch_nodetype(fname) == ڳنڍ_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	أرجع FAIL;
    }
    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Write the current buffer to file "eap->arg".
 * If "eap->append" is TRUE, append to the file.
 *
 * If "*eap->arg == NUL" write to current file.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
do_write(exarg_T *eap)
{
    صحيح		other;
    char_u	*fname = عدم;		// init to shut up gcc
    char_u	*ffname;
    صحيح		retval = FAIL;
    char_u	*free_fname = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
    char_u	*browse_file = عدم;
#جيڪڏهن_پورو
    buf_T	*alt_buf = عدم;
    صحيح		name_was_missing;

    إذا (not_writing())		// check 'write' option
	أرجع FAIL;

    ffname = eap->arg;
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
    إذا ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    عدم, عدم, عدم, curbuf);
	إذا (browse_file == عدم)
	    اذهب theend;
	ffname = browse_file;
    }
#جيڪڏهن_پورو
    إذا (*ffname == NUL)
    {
	إذا (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_دليل_required));
	    اذهب theend;
	}
	other = FALSE;
    }
    وإلا
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	إذا (free_fname != عدم)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    إذا (other)
    {
	إذا (vim_strchr(p_cpo, CPO_ALTWRITE) != عدم
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	وإلا
	    alt_buf = buflist_findname(ffname);
	إذا (alt_buf != عدم && alt_buf->b_ml.ml_mfp != عدم)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_اندريون_another_بفر));
	    اذهب theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    إذا (!other && (bt_dontwrite_نياپو(curbuf) || check_fname() == FAIL
#جيڪڏهن_وصف_ٿيل UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#جيڪڏهن_پورو
		|| check_readonly(&eap->forceit, curbuf)))
	اذهب theend;

    إذا (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	إذا (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_سٽ_ڳڻپ)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    إذا (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		إذا (vim_dialog_yesno(VIM_QUESTION, عدم,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    اذهب theend;
		eap->forceit = TRUE;
	    }
	    وإلا
#جيڪڏهن_پورو
	    {
		emsg(_(e_use_bang_منزل_write_partial_بفر));
		اذهب theend;
	    }
	}
    }

    إذا (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	إذا (eap->cmdidx == CMD_saveas && alt_buf != عدم)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, alt_buf);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (curbuf != was_curbuf || aborting())
#وإلا
	    إذا (curbuf != was_curbuf)
#جيڪڏهن_پورو
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		اذهب theend;
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
	    buf_name_بدليو(curbuf);

	    apply_autocmds(EVENT_BUFFILEPOST, عدم, عدم, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, عدم, عدم, FALSE, alt_buf);
	    إذا (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, عدم, عدم, FALSE, alt_buf);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (curbuf != was_curbuf || aborting())
#وإلا
	    إذا (curbuf != was_curbuf)
#جيڪڏهن_پورو
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		اذهب theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    إذا (*curbuf->b_p_ft == NUL)
	    {
		إذا (au_has_گروهه((char_u *)"filetypedetect"))
		    (فراغ)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, عدم);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == عدم;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	إذا (eap->cmdidx == CMD_saveas)
	{
	    إذا (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#جيڪڏهن_پورو
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	إذا (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
    vim_free(browse_file);
#جيڪڏهن_پورو
    vim_free(free_fname);
    أرجع retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    صحيح
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    صحيح		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    إذا (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == عدم)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	إذا (!eap->forceit && !eap->append)
	{
#جيڪڏهن_وصف_ٿيل UNIX
	    // with UNIX it is possible to open a directory
	    إذا (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		أرجع FAIL;
	    }
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    إذا (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_نياپو_سائيز];

		dialog_نياپو(buff, _("Overwrite existing file \"%s\"?"), fname);
		إذا (vim_dialog_yesno(VIM_QUESTION, عدم, buff, 2) != VIM_YES)
		    أرجع FAIL;
		eap->forceit = TRUE;
	    }
	    وإلا
#جيڪڏهن_پورو
	    {
		emsg(_(e_file_exists));
		أرجع FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	إذا (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    صحيح		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    إذا (*p_dir == NUL)
	    {
		dir = alloc(5);
		إذا (dir == عدم)
		    أرجع FAIL;
		STRCPY(dir, ".");
	    }
	    وإلا
	    {
		dir = alloc(MAXPATHL);
		إذا (dir == عدم)
		    أرجع FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    إذا (r)
	    {
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		إذا (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_نياپو_سائيز];

		    dialog_نياپو(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    إذا (vim_dialog_yesno(VIM_QUESTION, عدم, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			أرجع FAIL;
		    }
		    eap->forceit = TRUE;
		}
		وإلا
#جيڪڏهن_پورو
		{
		    semsg(_(e_مٽا_سٽا_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    أرجع FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    أرجع OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    فراغ
ex_wnext(exarg_T *eap)
{
    صحيح		i;

    إذا (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (صحيح)eap->line2;
    وإلا
	i = curwin->w_arg_idx - (صحيح)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_سٽ_ڳڻپ;
    إذا (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    فراغ
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    صحيح		error = 0;
    صحيح		save_forceit = eap->forceit;
    صحيح		save_exiting = exiting;

    إذا (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	إذا (before_quit_all(eap) == FAIL)
	    أرجع;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	إذا (exiting && !eap->forceit && رڪن_ڪم_running(buf->b_رڪن))
	{
	    bufref_T	bufref;

	    سيٽ_bufref(&bufref, buf);
	    إذا (رڪن_try_stop_ڪم(buf) == FAIL)
	    {
		no_write_پيغام_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    وإلا إذا (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	وإلا
#جيڪڏهن_پورو
	إذا (buf_Is_بدليو(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    إذا (not_writing())
	    {
		++error;
		اكسر;
	    }
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    إذا (buf->b_ffname == عدم && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#جيڪڏهن_پورو
	    إذا (buf->b_ffname == عدم)
	    {
		semsg(_(e_no_file_name_for_بفر_nr), (ڊگهو)buf->b_fnum);
		++error;
	    }
	    وإلا إذا (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    وإلا
	    {
		bufref_T bufref;

		سيٽ_bufref(&bufref, buf);
		إذا (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		إذا (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    إذا (exiting)
    {
	إذا (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    ساكن صحيح
not_writing(فراغ)
{
    إذا (p_write)
	أرجع FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    أرجع TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    ساكن صحيح
check_readonly(صحيح *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    إذا (!*forceit && (buf->b_p_ro
		|| (mch_stat((حرف *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	إذا ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != عدم)
	{
	    char_u	buff[DIALOG_نياپو_سائيز];

	    إذا (buf->b_p_ro)
		dialog_نياپو(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    وإلا
		dialog_نياپو(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    إذا (vim_dialog_yesno(VIM_QUESTION, عدم, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		أرجع FALSE;
	    }
	    وإلا
		أرجع TRUE;
	}
	وإلا
#جيڪڏهن_پورو
	إذا (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_سيٽ_جوڙيو_bang_منزل_override));
	وإلا
	    semsg(_(e_str_is_read_only_جوڙيو_bang_منزل_override), buf->b_fname);
	أرجع TRUE;
    }

    أرجع FALSE;
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
    صحيح
getfile(
    صحيح		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    صحيح		setpm,
    linenr_T	lnum,
    صحيح		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    صحيح		other;
    صحيح		retval;
    char_u	*free_me = عدم;

    إذا (!check_can_سيٽ_curbuf_forceit(forceit))
	أرجع GETFILE_ERROR;

    إذا (متن_locked())
	أرجع GETFILE_ERROR;
    إذا (curbuf_locked())
	أرجع GETFILE_ERROR;

    إذا (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    وإلا
	other = (fnum != curbuf->b_fnum);

    إذا (other)
	++no_wait_return;	    // don't wait for autowrite message
    إذا (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbuf_Is_بدليو() && autowrite(curbuf, forceit) == FAIL)
    {
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	إذا (p_confirm && p_write)
	    dialog_بدليو(curbuf, FALSE);
	إذا (curbuf_Is_بدليو())
#جيڪڏهن_پورو
	{
	    --no_wait_return;
	    no_write_پيغام();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    اذهب theend;
	}
    }
    إذا (other)
	--no_wait_return;
    إذا (setpm)
	setpcmark();
    إذا (!other)
    {
	إذا (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    وإلا إذا (do_ecmd(fnum, ffname, sfname, عدم, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    وإلا
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    أرجع retval;
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
    صحيح
do_ecmd(
    صحيح		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    صحيح		flags,
    win_T	*oldwin)
{
    صحيح		other_file;		// TRUE if editing another file
    صحيح		oldbuf;			// TRUE if using existing buffer
    صحيح		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = عدم;
#إذا defined(FEAT_EVAL)
    صحيح		did_سيٽ_swapcommand = FALSE;
#جيڪڏهن_پورو
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
    char_u	dot_رستو[] = ".";
    char_u	*browse_file = عدم;
#جيڪڏهن_پورو
    صحيح		retval = FAIL;
    ڊگهو	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    صحيح		newcol = -1;
    صحيح		solcol = -1;
    pos_T	*pos;
    char_u	*command = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    صحيح		did_get_winopts = FALSE;
#جيڪڏهن_پورو
    صحيح		readfile_flags = 0;
    صحيح		did_inc_redrawing_disabled = FALSE;
    ڊگهو	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (ERROR_IF_رڪن_POPUP_دري)
	أرجع FAIL;
#جيڪڏهن_پورو

    إذا (eap != عدم)
	command = eap->do_ecmd_cmd;
    سيٽ_bufref(&old_curbuf, curbuf);

    إذا (fnum != 0)
    {
	إذا (fnum == curbuf->b_fnum)	// file is already being edited
	    أرجع OK;			// nothing to do
	other_file = TRUE;
    }
    وإلا
    {
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
	إذا ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    إذا (
# ifdef FEAT_GUI
		!gui.اندريون_use &&
# endif
		    au_has_گروهه((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		إذا (ffname == عدم || !mch_isdir(ffname))
		    ffname = dot_رستو;
	    }
	    وإلا
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    عدم, عدم, عدم, curbuf);
		إذا (browse_file == عدم)
		    اذهب theend;
		ffname = browse_file;
	    }
	}
#جيڪڏهن_پورو
	// if no short name given, use ffname for short name
	إذا (sfname == عدم)
	    sfname = ffname;
#جيڪڏهن_وصف_ٿيل USE_FNAME_CASE
	إذا (sfname != عدم)
	    fname_case(sfname, 0);   // set correct case for sfname
#جيڪڏهن_پورو

	إذا ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == عدم || *ffname == NUL))
	    اذهب theend;

	إذا (ffname == عدم)
	    other_file = TRUE;
					    // there is no file name
	وإلا إذا (*ffname == NUL && curbuf->b_ffname == عدم)
	    other_file = FALSE;
	وإلا
	{
	    إذا (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    إذا (free_fname != عدم)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    إذا (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_بدليو(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == عدم ? 0 : CCGD_EXCMD)))
    {
	إذا (fnum == 0 && other_file && ffname != عدم)
	    (فراغ)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	اذهب theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    إذا (oldwin != عدم && !win_valid(oldwin))
	oldwin = عدم;

#إذا defined(FEAT_EVAL)
    إذا ((command != عدم || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	إذا (command != عدم)
	    valsize = (صحيح)STRLEN(command) + 3;
	وإلا
	    valsize = 30;
	val.string = alloc(valsize);
	إذا (val.string != عدم)
	{
	    إذا (command != عدم)
		val.ڊيگهائي = vim_snprintf_safelen((حرف *)val.string,
		    valsize, ":%s\r", command);
	    وإلا
		val.ڊيگهائي = vim_snprintf_safelen((حرف *)val.string,
		    valsize, "%ldG", (ڊگهو)newlnum);
	    سيٽ_vim_var_string(VV_SWAPCOMMAND, val.string, (صحيح)val.ڊيگهائي);
	    did_سيٽ_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#جيڪڏهن_پورو

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    إذا (other_file)
    {
	صحيح prev_alt_fnum = curwin->w_alt_fnum;

	إذا (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    إذا ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    إذا (oldwin != عدم)
		buflist_altfpos(oldwin);
	}

	إذا (fnum)
	    buf = buflist_findnr(fnum);
	وإلا
	{
	    إذا (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		إذا (command != عدم)
		{
		    tlnum = atol((حرف *)command);
		    إذا (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		إذا (newbuf != عدم)
		{
		    إذا (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    إذا (tlnum > 0)
			newbuf->b_آخري_cursor.lnum = tlnum;
		}
		اذهب theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_سيٽ_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    إذا (oldwin != عدم)
		oldwin = curwin;
	    سيٽ_bufref(&old_curbuf, curbuf);
	}
	إذا (buf == عدم)
	    اذهب theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	إذا (buf->b_locked_ورهايو)
	{
	    emsg(_(e_cannot_switch_منزل_a_closing_بفر));
	    اذهب theend;
	}
	إذا (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	إذا (buf->b_ml.ml_mfp == عدم)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	وإلا					// existing memfile
	{
	    oldbuf = TRUE;
	    سيٽ_bufref(&bufref, buf);
	    (فراغ)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    إذا (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		اذهب theend;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (aborting())	    // autocmds may abort script processing
		اذهب theend;
#جيڪڏهن_پورو
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	إذا ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_آخري)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->کانو;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	إذا (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    صحيح		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    أكد(cmdwin_buf == عدم);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = عدم;

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
	    إذا (buf->b_fname != عدم)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    سيٽ_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    إذا (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_نياپو(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		اذهب theend;
	    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		اذهب theend;
	    }
#جيڪڏهن_پورو
	    إذا (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    وإلا
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		إذا (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_بفر(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != عدم);

		// Autocommands may have closed the window.
		إذا (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		// autocmds may abort script processing
		إذا (aborting() && curwin->w_بفر != عدم)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    اذهب theend;
		}
#جيڪڏهن_پورو
		// Be careful again, like above.
		إذا (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    إذا (curwin->w_بفر == عدم)
			buf = lastbuf;
		    وإلا
		    {
			delbuf_نياپو(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			اذهب theend;
		    }
		}
		إذا (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		وإلا
		{
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    إذا (curwin->w_بفر == عدم
			    || curwin->w_s == &(curwin->w_بفر->b_s))
			curwin->w_s = &(buf->b_s);
#جيڪڏهن_پورو
		    إذا (curwin->w_بفر != عدم)
			--curwin->w_بفر->b_nwindows;

		    curwin->w_بفر = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    إذا (!oldbuf && eap != عدم)
		    {
			سيٽ_file_options(TRUE, eap);
			سيٽ_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
		did_get_winopts = TRUE;
#جيڪڏهن_پورو
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.کانو = 0;
    }
    وإلا // !other_file
    {
	إذا ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    اذهب theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    إذا ((flags & ECMD_سيٽ_HELP) || keep_help_flag)
    {
	prepare_help_بفر();
    }
    وإلا
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	إذا (!curbuf->b_help)
	    سيٽ_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    إذا (buf != curbuf)
	اذهب theend;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (aborting())	    // autocmds may abort script processing
	اذهب theend;
#جيڪڏهن_پورو

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
    إذا (!other_file && !oldbuf)		// re-use the buffer
    {
	سيٽ_آخري_cursor(curwin);	// may set b_last_cursor
	إذا (newlnum == ECMD_آخري || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.کانو;
	}
	buf = curbuf;
	إذا (buf->b_fname != عدم)
	    new_name = vim_strsave(buf->b_fname);
	وإلا
	    new_name = عدم;
	سيٽ_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	إذا (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_سٽ_ڳڻپ <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    إذا (u_savecommon(0, curbuf->b_ml.ml_سٽ_ڳڻپ + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		اذهب theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	وإلا
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	إذا (!bufref_valid(&bufref))
	{
	    delbuf_نياپو(new_name);	// frees new_name
	    اذهب theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	إذا (buf != curbuf)
	    اذهب theend;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (aborting())	    // autocmds may abort script processing
	    اذهب theend;
#جيڪڏهن_پورو
	buf_clear_file(curbuf);
	curbuf->b_op_شروعات.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_پڇاڙي.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    // If the file name was changed, reset the not-edit flag so that ":write"
    // works.
    إذا (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    إذا (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		إذا (win->w_بفر == curbuf)
		    foldUpdateAll(win);
	}
#جيڪڏهن_پورو

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	إذا (!oldbuf)			    // need to read the file
	{
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    إذا (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#جيڪڏهن_پورو
	    مٽا_سٽا_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    إذا (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#إذا defined(FEAT_EVAL)
	    إذا (should_abort(open_بفر(FALSE, eap, readfile_flags)))
		retval = FAIL;
#وإلا
	    (فراغ)open_بفر(FALSE, eap, readfile_flags);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#جيڪڏهن_پورو
	    إذا (مٽا_سٽا_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_مٽا_سٽا_exists(&old_curbuf);
	}
	وإلا
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, عدم, عدم, FALSE,
							      curbuf, &retval);
	    إذا ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, عدم, عدم, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	إذا (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *متن = ml_get_curline();

	    إذا (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.کانو != (صحيح)(skipwhite(متن) - متن))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.کانو;
	    }
	}
	إذا (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	بدليو_سٽ_abv_curs();

	maketitle();
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	إذا (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_سيٽ_title(curwin);
#جيڪڏهن_پورو
    }

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    إذا (curwin->w_p_diff)
    {
	diff_buf_جوڙيو(curbuf);
	diff_invalidate(curbuf);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    إذا (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(فراغ)parse_spelllang(curwin);
#جيڪڏهن_پورو

    إذا (command == عدم)
    {
	إذا (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.کانو = newcol;
	    check_cursor();
	}
	وإلا إذا (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    إذا (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.کانو = solcol;
		check_cursor_کانو();
		curwin->w_cursor.coladd = 0;
		curwin->w_سيٽ_curswant = true;
	    }
	    وإلا
		beginline(BL_SOL | BL_FIX);
	}
	وإلا			// no line number, go to last line in Ex mode
	{
	    إذا (exmode_سرگرم)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    إذا (oldbuf && !auto_buf)
    {
	صحيح	نياپو_scroll_save = نياپو_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	إذا (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    نياپو_scroll = FALSE;
	إذا (!نياپو_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	نياپو_شروعات();
	نياپو_scroll = نياپو_scroll_save;
	نياپو_scrolled_ign = TRUE;

	إذا (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	نياپو_scrolled_ign = FALSE;
    }

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    curbuf->b_آخري_استعمال_ٿيل = vim_وقت();
#جيڪڏهن_پورو

    إذا (command != عدم)
	do_cmdline(command, عدم, عدم, DOCMD_VERBOSE|DOCMD_RANGEOK);

#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
    إذا (curbuf->b_kmap_state & KEYMAP_INIT)
	(فراغ)keymap_init();
#جيڪڏهن_پورو

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    إذا (!skip_redraw)
    {
	n = *so_ptr;
	إذا (topline == 0 && command == عدم)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    إذا (p_im && (State & گهڻائي_وجهو) == 0)
	need_شروعات_insertmode = TRUE;

#جيڪڏهن_وصف_ٿيل FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    إذا (p_acd && curbuf->b_ffname != عدم)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	إذا (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#جيڪڏهن_پورو

#إذا defined(FEAT_NETBEANS_INTG)
    إذا (curbuf->b_ffname != عدم)
    {
# ifdef FEAT_NETBEANS_INTG
	إذا ((flags & ECMD_سيٽ_HELP) != ECMD_سيٽ_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#جيڪڏهن_پورو

theend:
    إذا (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#إذا defined(FEAT_EVAL)
    إذا (did_سيٽ_swapcommand)
	سيٽ_vim_var_string(VV_SWAPCOMMAND, عدم, -1);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
    vim_free(browse_file);
#جيڪڏهن_پورو
    vim_free(free_fname);
    أرجع retval;
}

    ساكن فراغ
delbuf_نياپو(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_بفر_str),
					   name == عدم ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = عدم;
    au_new_curbuf.br_buf_free_ڳڻپ = 0;
}

ساكن صحيح append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    ساكن char_u *
get_append_سٽ(exarg_T *eap, صحيح indent)
{
    char_u	*theline;
    char_u	*p;

    إذا (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    وإلا إذا (eap->ea_getline == عدم)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	إذا (eap->nextcmd == عدم)
	    أرجع عدم;
	p = vim_strchr(eap->nextcmd, NL);
	إذا (p == عدم)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	إذا (*p != NUL)
	    ++p;
	وإلا
	    p = عدم;
	eap->nextcmd = p;
    }
    وإلا
    {
	صحيح save_State = State;

	// Set State to avoid the cursor shape to be set to MODE_INSERT
	// state when getline() returns.
	State = گهڻائي_CMDLINE;
	theline = eap->ea_getline(
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		eap->cstack->cs_looplevel > 0 ? -1 :
#جيڪڏهن_پورو
		NUL, eap->cookie, indent, TRUE);
	State = save_State;
    }
    أرجع theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    فراغ
ex_append(exarg_T *eap)
{
    char_u	*theline;
    صحيح		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    صحيح		indent = 0;
    char_u	*p;
    صحيح		vcol;
    صحيح		خالي_جايون_2 = (curbuf->b_ml.ml_flags & ML_خالي_جايون);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (not_اندريون_vim9(eap, eap->skip) == FAIL)
	أرجع;
#جيڪڏهن_پورو
    إذا (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	طالما ((theline = get_append_سٽ(eap, 0)) != عدم)
	{
	    صحيح پڇاڙي = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    إذا (پڇاڙي)
		اكسر;
	}
	أرجع;
    }

    // the ! flag toggles autoindent
    إذا (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    إذا (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    إذا (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    إذا (خالي_جايون_2 && lnum == 1)
	lnum = 0;

    State = گهڻائي_وجهو;		    // behave like in Insert mode
    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= گهڻائي_LANGMAP;

    لكل (;;)
    {
	نياپو_scroll = TRUE;
	need_wait_return = FALSE;
	إذا (curbuf->b_p_ai)
	{
	    إذا (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    وإلا إذا (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_سٽ(eap, indent);
	سٽون_کاٻو = صفون - 1;
	إذا (theline == عدم)
	    اكسر;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	إذا (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	لكل (p = theline; indent > vcol; ++p)
	{
	    إذا (*p == ' ')
		++vcol;
	    وإلا إذا (*p == TAB)
		vcol += 8 - vcol % 8;
	    وإلا
		اكسر;
	}
	إذا ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (خالي_جايون_2 ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    اكسر;
	}

	// don't use autoindent if nothing was typed.
	إذا (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	إذا (خالي_جايون_2)
	    // there are no marks below the inserted lines
	    appended_سٽون(lnum, 1L);
	وإلا
	    appended_سٽون_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	إذا (خالي_جايون_2)
	{
	    ml_delete(2L);
	    خالي_جايون_2 = FALSE;
	}
    }
    State = گهڻائي_عام;

    إذا (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_شروعات.lnum = (eap->line2 < curbuf->b_ml.ml_سٽ_ڳڻپ) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_سٽ_ڳڻپ;
	إذا (eap->cmdidx != CMD_append)
	    --curbuf->b_op_شروعات.lnum;
	curbuf->b_op_پڇاڙي.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_شروعات.lnum;
	curbuf->b_op_شروعات.کانو = curbuf->b_op_پڇاڙي.کانو = 0;
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
    فراغ
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (not_اندريون_vim9(eap, eap->skip) == FAIL)
	أرجع;
#جيڪڏهن_پورو
    إذا (eap->skip)
    {
	ex_append(eap);
	أرجع;
    }
    إذا (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	أرجع;

    // the ! flag toggles autoindent
    إذا (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    لكل (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	إذا (curbuf->b_ml.ml_flags & ML_خالي_جايون)	    // nothing to delete
	    اكسر;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_سٽون_mark(eap->line1, (ڊگهو)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    فراغ
ex_z(exarg_T *eap)
{
    char_u	*x;
    ڊگهو	bigness;
    char_u	*قسم;
    صحيح		minus = 0;
    linenr_T	شروعات, پڇاڙي, curs, i;
    صحيح		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    إذا (eap->forceit)
	bigness = صفون - 1;
    وإلا إذا (!ONE_دري)
	bigness = curwin->w_اوچائي - 3;
    وإلا
	bigness = curwin->w_p_scr * 2;
    إذا (bigness < 1)
	bigness = 1;

    x = eap->arg;
    قسم = x;
    إذا (*قسم == '-' || *قسم == '+' || *قسم == '='
					      || *قسم == '^' || *قسم == '.')
	++x;
    طالما (*x == '-' || *x == '+')
	++x;

    إذا (*x != 0)
    {
	إذا (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_دليل_منزل_z));
	    أرجع;
	}
	وإلا
	{
	    bigness = atol((حرف *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    إذا (bigness > 2 * curbuf->b_ml.ml_سٽ_ڳڻپ || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_سٽ_ڳڻپ;

	    p_دري = bigness;
	    إذا (*قسم == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    إذا (*قسم == '-' || *قسم == '+')
	لكل (x = قسم + 1; *x == *قسم; ++x)
	    ;

    بدل (*قسم)
    {
	حالة '-':
	    شروعات = lnum - bigness * (linenr_T)(x - قسم) + 1;
	    پڇاڙي = شروعات + bigness - 1;
	    curs = پڇاڙي;
	    اكسر;

	حالة '=':
	    شروعات = lnum - (bigness + 1) / 2 + 1;
	    پڇاڙي = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    اكسر;

	حالة '^':
	    شروعات = lnum - bigness * 2;
	    پڇاڙي = lnum - bigness;
	    curs = lnum - bigness;
	    اكسر;

	حالة '.':
	    شروعات = lnum - (bigness + 1) / 2 + 1;
	    پڇاڙي = lnum + (bigness + 1) / 2 - 1;
	    curs = پڇاڙي;
	    اكسر;

	افتراضي:  // '+'
	    شروعات = lnum;
	    إذا (*قسم == '+')
		شروعات += bigness * (linenr_T)(x - قسم - 1) + 1;
	    وإلا إذا (eap->addr_ڳڻپ == 0)
		++شروعات;
	    پڇاڙي = شروعات + bigness - 1;
	    curs = پڇاڙي;
	    اكسر;
    }

    إذا (شروعات < 1)
	شروعات = 1;

    إذا (پڇاڙي > curbuf->b_ml.ml_سٽ_ڳڻپ)
	پڇاڙي = curbuf->b_ml.ml_سٽ_ڳڻپ;

    إذا (curs > curbuf->b_ml.ml_سٽ_ڳڻپ)
	curs = curbuf->b_ml.ml_سٽ_ڳڻپ;
    وإلا إذا (curs < 1)
	curs = 1;

    لكل (i = شروعات; i <= پڇاڙي; i++)
    {
	إذا (minus && i == lnum)
	{
	    نياپو_putchar('\n');

	    لكل (j = 1; j < Columns; j++)
		نياپو_putchar('-');
	}

	print_سٽ(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	إذا (minus && i == lnum)
	{
	    نياپو_putchar('\n');

	    لكل (j = 1; j < Columns; j++)
		نياپو_putchar('-');
	}
    }

    إذا (curwin->w_cursor.lnum != curs)
    {
	curwin->w_cursor.lnum = curs;
	curwin->w_cursor.کانو = 0;
    }
    ex_no_reprint = TRUE;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    صحيح
check_restricted(فراغ)
{
    إذا (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_اندريون_rvim));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    صحيح
check_secure(فراغ)
{
    إذا (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_اندريون_current_dir_or_ٽيگ_ڳولا));
	أرجع TRUE;
    }
#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    إذا (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندريون_sandbox));
	أرجع TRUE;
    }
#جيڪڏهن_پورو
    أرجع FALSE;
}

ساكن char_u	*old_sub = عدم;	// previous substitute pattern
ساكن صحيح	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
تعريفنوع بنية {
    صحيح	do_all;		// do multiple substitutions per line
    صحيح	do_ask;		// ask for confirmation
    صحيح	do_ڳڻپ;	// count only
    صحيح	do_error;	// if false, ignore errors
    صحيح	do_print;	// print last line with subs.
    صحيح	do_list;	// list last line with subs.
    صحيح	do_عدد;	// list last line with line nr
    صحيح	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *شروعات, صحيح delimiter)
{
    char_u *p = شروعات;

    طالما (p[0])
    {
	إذا (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    اكسر;
	}
	إذا (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    أرجع p;
}

    ساكن صحيح
check_regexp_delim(صحيح c)
{
    إذا (محفوظ_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    فراغ
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    ڊگهو	i = 0;
    regmmatch_T regmatch;
    ساكن subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    subflags_T	subflags_save;
#جيڪڏهن_پورو
    صحيح		save_do_all;		// remember user specified 'g' flag
    صحيح		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = عدم;		// init for GCC
    string_T	pat = {عدم, 0};
    صحيح		delimiter;
    size_t	sublen;
    صحيح		got_quit = FALSE;
    صحيح		got_ميلاپ = FALSE;
    صحيح		which_pat;
    char_u	*cmd;
    char_u	*p;
    صحيح		save_State;
    linenr_T	پهريون_سٽ = 0;		// first changed line
    linenr_T	آخري_سٽ = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_سٽ_ڳڻپ = curbuf->b_ml.ml_سٽ_ڳڻپ;
    linenr_T	line2;
    ڊگهو	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    صحيح		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    صحيح		شروعات_nsubs;
    صحيح		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		save_ma = 0;
    صحيح		save_sandbox = 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    textprop_T	*متن_props = عدم;
#جيڪڏهن_پورو

    cmd = eap->arg;
    إذا (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    شروعات_nsubs = sub_nsubs;

    إذا (eap->cmdidx == CMD_tilde)
	which_pat = RE_آخري;	// use last used regexp
    وإلا
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    إذا (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == عدم)
    {
	// don't accept alphanumeric for separator
	إذا (check_regexp_delim(*cmd) == FAIL)
	    أرجع;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (اندريون_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    أرجع;
#جيڪڏهن_پورو

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	إذا (*cmd == '\\')
	{
	    إذا (اندريون_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_اندريون_vim9_script));
		أرجع;
	    }
	    ++cmd;
	    إذا (vim_strchr((char_u *)"/?&", *cmd) == عدم)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		أرجع;
	    }
	    إذا (*cmd != '&')
		which_pat = RE_ڳولا;	    // use last '/' pattern
	    STR_LITERAL_سيٽ(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	وإلا		// find the end of the regexp
	{
	    which_pat = RE_آخري;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, عدم, عدم);
	    pat.ڊيگهائي = (size_t)(cmd - pat.string);
	    إذا (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	إذا (sub == عدم)
	    // out of memory
	    أرجع;

	إذا (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    إذا (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != عدم)
	    {
		إذا (old_sub == عدم)	// there is no previous command
		{
		    emsg(_(e_no_اڳيون_substitute_regular_اظهار));
		    vim_free(sub);
		    أرجع;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		إذا (sub == عدم)
		    // out of memory
		    أرجع;
	    }
	    وإلا إذا (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		إذا (old_sub == عدم)
		{
		    // out of memory
		    vim_free(sub);
		    أرجع;
		}
	    }
	}
    }
    وإلا إذا (!eap->skip)	// use previous pattern and substitution
    {
	إذا (old_sub == عدم)	// there is no previous command
	{
	    emsg(_(e_no_اڳيون_substitute_regular_اظهار));
	    أرجع;
	}
	pat.string = عدم;		// search_regcomp() will use previous pattern
	pat.ڊيگهائي = 0;
	sub = vim_strsave(old_sub);
	إذا (sub == عدم)
	    أرجع;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    إذا (pat.string != عدم && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_سٽون_ڳڻپ;

	إذا (eap->skip)
	{
	    vim_free(sub);
	    أرجع;
	}
	curwin->w_cursor.lnum = eap->line1;
	إذا (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	وإلا إذا (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	وإلا إذا (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_سٽون_ڳڻپ = eap->line2 - eap->line1 + 1;
	إذا (eap->line2 < curbuf->b_ml.ml_سٽ_ڳڻپ)
	    ++joined_سٽون_ڳڻپ;
	إذا (joined_سٽون_ڳڻپ > 1)
	{
	    (فراغ)do_join(joined_سٽون_ڳڻپ, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_سٽون_ڳڻپ - 1;
	    sub_nlines = 1;
	    (فراغ)do_sub_نياپو(FALSE);
	    ex_may_print(eap);
	}

	إذا (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.ڊيگهائي, magic_isset());
	// put pattern in history
	جوڙيو_منزل_history(HIST_ڳولا, pat.string, pat.ڊيگهائي, TRUE, NUL);
	vim_free(sub);

	أرجع;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    إذا (*cmd == '&')
	++cmd;
    وإلا
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (اندريون_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	وإلا
#جيڪڏهن_پورو
	إذا (!p_ed)
	{
	    إذا (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    وإلا
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_ڳڻپ = FALSE;
	subflags.do_عدد = FALSE;
	subflags.do_ic = 0;
    }
    طالما (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	إذا (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	وإلا إذا (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	وإلا إذا (*cmd == 'n')
	    subflags.do_ڳڻپ = TRUE;
	وإلا إذا (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	وإلا إذا (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_آخري;
	وإلا إذا (*cmd == 'p')
	    subflags.do_print = TRUE;
	وإلا إذا (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_عدد = TRUE;
	}
	وإلا إذا (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	وإلا إذا (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	وإلا إذا (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	وإلا
	    اكسر;
	++cmd;
    }
    إذا (subflags.do_ڳڻپ)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    إذا (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	إذا (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_مثبت_ڳڻپ_required));
	    vim_free(sub);
	    أرجع;
	}
	وإلا إذا (i >= وڌ_کان_وڌ_عدد)
	{
	    حرف	buf[20];
	    vim_snprintf(buf, حجم(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    أرجع;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	إذا (eap->line2 > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    eap->line2 = curbuf->b_ml.ml_سٽ_ڳڻپ;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    إذا (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	سيٽ_nextcmd(eap, cmd);
	إذا (eap->nextcmd == عدم)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    أرجع;
	}
    }

    إذا (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	أرجع;
    }

    إذا (!subflags.do_ڳڻپ && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	أرجع;
    }

    إذا (ڳولا_regcomp(pat.string, pat.ڊيگهائي, عدم, RE_SUBST, which_pat, ڳولا_HIS, &regmatch) == FAIL)
    {
	إذا (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	أرجع;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    إذا (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    وإلا إذا (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = عدم;
    sub_firstline.ڊيگهائي = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    إذا (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	إذا (p == عدم)
	    أرجع;
	sub = p;
    }
    وإلا
    {
	p = regtilde(sub, magic_isset());

	إذا (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    لكل (lnum = eap->line1; lnum <= line2 && !(got_quit
#إذا defined(FEAT_EVAL)
		|| aborting()
#جيڪڏهن_پورو
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, عدم);
	إذا (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_شروعات = {عدم, 0};
	    size_t	new_شروعات_سائيز = 0;
	    char_u	*new_پڇاڙي;
	    صحيح		did_sub = FALSE;
	    صحيح		lastone;
	    size_t	copy_len;
	    size_t	needed_سائيز;
	    ڊگهو	nmatch_tl = 0;	// nr of lines matched below lnum
	    صحيح		do_again;	// do it again after joining lines
	    صحيح		skip_ميلاپ = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_ورهايو = false;	// "\r" split the line
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    صحيح		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	ڪل_added =  0;
	    صحيح		متن_prop_ڳڻپ = 0;
#جيڪڏهن_پورو

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
	    إذا (!got_ميلاپ)
	    {
		setpcmark();
		got_ميلاپ = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    لكل (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		إذا (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		إذا (lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
		    اكسر;

		إذا (sub_firstline.string == عدم)
		{
		    sub_firstline.ڊيگهائي = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.ڊيگهائي);
		    إذا (sub_firstline.string == عدم)
		    {
			vim_free(new_شروعات.string);
			اذهب outofmem;
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
		إذا (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].کانو)
		{
		    إذا (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_ميلاپ = TRUE;
		    وإلا
		    {
			 // search for a match at next column
			إذا (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			وإلا
			    ++matchcol;
		    }
		    اذهب skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].کانو;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		إذا (subflags.do_ڳڻپ)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    إذا (nmatch > 1)
		    {
			matchcol = (colnr_T)sub_firstline.ڊيگهائي;
			nmatch = 1;
			skip_ميلاپ = TRUE;
		    }
		    sub_nsubs++;
		    did_sub = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		    // Skip the substitution, unless an expression is used,
		    // then it is evaluated in the sandbox.
		    إذا (!(sub[0] == '\\' && sub[1] == '='))
#جيڪڏهن_پورو
			اذهب skip;
		}

		إذا (subflags.do_ask)
		{
		    صحيح typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = گهڻائي_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.کانو = regmatch.startpos[0].کانو;
		    إذا (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    إذا (vim_strchr(p_cpo, CPO_UNDO) != عدم)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    طالما (subflags.do_ask)
		    {
			إذا (exmode_سرگرم)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_سٽ_no_اڳياڙي(lnum,
					 subflags.do_عدد, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, عدم, عدم, 0);
			    curwin->w_cursor.کانو = regmatch.endpos[0].کانو - 1;
			    إذا (curwin->w_cursor.کانو < 0)
				curwin->w_cursor.کانو = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   عدم, عدم, &ec, 0);
			    curwin->w_cursor.کانو = regmatch.startpos[0].کانو;
			    إذا (subflags.do_عدد || curwin->w_p_nu)
			    {
				صحيح numw = عدد_ويڪر(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    نياپو_شروعات();
			    لكل (i = 0; i < (ڊگهو)sc; ++i)
				نياپو_putchar(' ');
			    لكل ( ; i <= (ڊگهو)ec; ++i)
				نياپو_putchar('^');

			    resp = getexmodeline('?', عدم, 0, TRUE);
			    إذا (resp != عدم)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				إذا (ex_عام_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			وإلا
			{
			    string_T orig_سٽ = {عدم, 0};
			    صحيح    len_change = 0;
			    صحيح	   save_p_lz = p_lz;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
			    صحيح save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#جيڪڏهن_پورو
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    صحيح save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    إذا (new_شروعات.string != عدم)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_سٽ.ڊيگهائي = ml_get_len(lnum);
				orig_سٽ.string = vim_strnsave(ml_get(lnum), orig_سٽ.ڊيگهائي);
				إذا (orig_سٽ.string != عدم)
				{
				    string_T new_سٽ;

				    new_سٽ.string = concat_str(new_شروعات.string,
						     sub_firstline.string + copycol);
				    إذا (new_سٽ.string == عدم)
					VIM_CLEAR_STRING(orig_سٽ);
				    وإلا
				    {
					new_سٽ.ڊيگهائي =
					    new_شروعات.ڊيگهائي + (sub_firstline.ڊيگهائي - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (صحيح)new_سٽ.ڊيگهائي - (صحيح)orig_سٽ.ڊيگهائي;
					curwin->w_cursor.کانو += len_change;
					ml_replace(lnum, new_سٽ.string, FALSE);
				    }
				}
			    }

			    ڳولا_ميلاپ_سٽون = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    ڳولا_ميلاپ_endcol = regmatch.endpos[0].کانو
								 + len_change;
			    إذا (ڳولا_ميلاپ_سٽون == 0
						   && ڳولا_ميلاپ_endcol == 0)
				// highlight at least one character for /^/
				ڳولا_ميلاپ_endcol = 1;
			    highlight_ميلاپ = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_ميلاپ = FALSE;
			    redraw_later(UPD_SOME_VALID);

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#جيڪڏهن_پورو
			    إذا (نياپو_صف == صفون - 1)
				نياپو_didout = FALSE;	// avoid a scroll-up
			    نياپو_starthere();
			    i = نياپو_scroll;
			    نياپو_scroll = 0;		// truncate msg when
							// needed
			    نياپو_no_وڌيڪ = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    نياپو_no_وڌيڪ = FALSE;
			    نياپو_scroll = i;
			    showruler(TRUE);
			    windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
			    RedrawingDisabled = save_RedrawingDisabled;

#جيڪڏهن_وصف_ٿيل USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#جيڪڏهن_پورو
			    ++no_mapping;	// don't map this key
			    ++allow_ڪنجيون;	// allow special keys
			    typed = plain_vgetc();
			    --allow_ڪنجيون;
			    --no_mapping;

			    // clear the question
			    نياپو_didout = FALSE;	// don't scroll up
			    نياپو_کانو = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    إذا (orig_سٽ.string != عدم)
				ml_replace(lnum, orig_سٽ.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			إذا (typed == 'q' || typed == ESC || typed == Ctrl_C
#جيڪڏهن_وصف_ٿيل UNIX
				|| typed == intr_char
#جيڪڏهن_پورو
				)
			{
			    got_quit = TRUE;
			    اكسر;
			}
			إذا (typed == 'n')
			    اكسر;
			إذا (typed == 'y')
			    اكسر;
			إذا (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    اكسر;
			}
			إذا (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    اكسر;
			}
			إذا (typed == Ctrl_E)
			    scrollup_clamp();
			وإلا إذا (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    إذا (vim_strchr(p_cpo, CPO_UNDO) != عدم)
			--no_u_sync;

		    إذا (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			إذا (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.ڊيگهائي;
			    skip_ميلاپ = TRUE;
			}
			اذهب skip;
		    }
		    إذا (got_quit)
			اذهب skip;
		}

		// Move the cursor to the start of the match, so that we can
		// use "\=col(".").
		curwin->w_cursor.کانو = regmatch.startpos[0].کانو;

		/*
		 * 3. substitute the string.
		 */
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		save_ma = curbuf->b_p_ma;
		save_sandbox = sandbox;
		إذا (subflags.do_ڳڻپ)
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
#جيڪڏهن_پورو
		// Get length of substitution part, including the NUL.
		// When it fails sublen is zero.
		sublen = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, sub_firstline.string, 0,
		    REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		--textlock;

		// If getting the substitute string caused an error, don't do
		// the replacement.
		// Don't keep flags set by a recursive call.
		subflags = subflags_save;
		إذا (sublen == 0 || aborting() || subflags.do_ڳڻپ)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    اذهب skip;
		}
#جيڪڏهن_پورو

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		إذا (nmatch > curbuf->b_ml.ml_سٽ_ڳڻپ - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_سٽ_ڳڻپ - sub_firstlnum + 1;
		    skip_ميلاپ = TRUE;
		    // safety check
		    إذا (nmatch < 0)
			اذهب skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		إذا (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.ڊيگهائي = sub_firstline.ڊيگهائي;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		    إذا (curbuf->b_has_textprop)
		    {
			صحيح بائيٽون_added = (صحيح)sublen - 1 - (regmatch.endpos[0].کانو
						   - regmatch.startpos[0].کانو);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			إذا (adjust_prop_columns(lnum,
					ڪل_added + regmatch.startpos[0].کانو,
						       بائيٽون_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			ڪل_added += (colnr_T)بائيٽون_added;
		    }
#جيڪڏهن_پورو
		}
		وإلا
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		    إذا (curbuf->b_has_textprop)
		    {
			char_u	*prop_شروعات;

			// Props in the first line may be shortened or deleted
			إذا (adjust_prop_columns(lnum,
					ڪل_added + regmatch.startpos[0].کانو,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			ڪل_added -=
			    (colnr_T)(sub_firstline.ڊيگهائي - regmatch.startpos[0].کانو);

			// Props in the last line may be moved or deleted
			إذا (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].کانو, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			متن_prop_ڳڻپ = get_متن_props(curbuf, lastlnum,
							   &prop_شروعات, FALSE);
			إذا (متن_prop_ڳڻپ > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(متن_props);
			    متن_props = ALLOC_MULT(textprop_T,
							      متن_prop_ڳڻپ);
			    إذا (متن_props != عدم)
			    {
				mch_memmove(متن_props, prop_شروعات,
					 متن_prop_ڳڻپ * حجم(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				صحيح wi = 0;
				لكل (صحيح pi = 0; pi < متن_prop_ڳڻپ; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    إذا (متن_props[pi].tp_id < 0
					    || (متن_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					استمر;
				    متن_props[wi] = متن_props[pi];
				    متن_props[wi].tp_کانو +=
					regmatch.startpos[0].کانو + (colnr_T)sublen - 1;
				    متن_props[wi].u.tp_متن = عدم;
				    ++wi;
				}
				متن_prop_ڳڻپ = wi;
			    }
			}
		    }
#جيڪڏهن_پورو
		    tmp.string = ml_get(lastlnum);
		    tmp.ڊيگهائي = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		    إذا (curbuf->b_has_textprop)
			ڪل_added +=
			    (colnr_T)(tmp.ڊيگهائي - regmatch.endpos[0].کانو);
#جيڪڏهن_پورو
		}

		copy_len = (size_t)(regmatch.startpos[0].کانو - copycol);
		needed_سائيز = copy_len
		    + (tmp.ڊيگهائي - (size_t)regmatch.endpos[0].کانو) + sublen + 1;

		إذا (new_شروعات.string == عدم)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_شروعات_سائيز = needed_سائيز + 50;
		    إذا ((new_شروعات.string = alloc_clear(new_شروعات_سائيز)) == عدم)
			اذهب outofmem;
		    *new_شروعات.string = NUL;
		    new_شروعات.ڊيگهائي = 0;
		}
		وإلا
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_سائيز += new_شروعات.ڊيگهائي;
		    إذا (needed_سائيز > new_شروعات_سائيز)
		    {
			new_شروعات_سائيز = needed_سائيز + 50;
			إذا ((p1 = alloc_clear(new_شروعات_سائيز)) == عدم)
			{
			    vim_free(new_شروعات.string);
			    اذهب outofmem;
			}
			mch_memmove(p1, new_شروعات.string, new_شروعات.ڊيگهائي + 1);
			vim_free(new_شروعات.string);
			new_شروعات.string = p1;
		    }
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_شروعات.string + new_شروعات.ڊيگهائي,
		    sub_firstline.string + copycol, copy_len);
		new_شروعات.ڊيگهائي += copy_len;

		// new text added here
		new_پڇاڙي = new_شروعات.string + new_شروعات.ڊيگهائي;

		إذا (new_شروعات_سائيز - copy_len < sublen)
		    sublen = new_شروعات_سائيز - copy_len - 1;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		++textlock;
#جيڪڏهن_پورو
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_پڇاڙي, (صحيح)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		إذا (n > 0)
		    new_شروعات.ڊيگهائي += n - 1;	    // remove 1 for the NUL

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		--textlock;
#جيڪڏهن_پورو
		sub_nsubs++;
		did_sub = TRUE;

		// Move the cursor to the start of the line, to avoid that it
		// is beyond the end of the line after the substitution.
		curwin->w_cursor.کانو = 0;

		// For a multi-line match, make a copy of the last matched
		// line and continue in that one.
		إذا (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.ڊيگهائي = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.ڊيگهائي);
		    إذا (sub_firstline.string == عدم)
		    {
			vim_free(new_شروعات.string);
			اذهب outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    إذا (sub_firstlnum <= line2)
			do_again = TRUE;
		    وإلا
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].کانو;

		إذا (skip_ميلاپ)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    إذا (sub_firstline.string == عدم)
		    {
			vim_free(new_شروعات.string);
			اذهب outofmem;
		    }
		    sub_firstline.ڊيگهائي = 0;
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
		لكل (p1 = new_پڇاڙي; *p1; ++p1)
		{
		    إذا (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_شروعات.ڊيگهائي - (p1 - new_شروعات.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_شروعات.ڊيگهائي;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
			إذا (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    إذا (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_شروعات.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#جيڪڏهن_پورو
		    }
		    وإلا إذا (*p1 == CAR)
		    {
			إذا (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_شروعات.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_شروعات.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    إذا (subflags.do_ask)
				appended_سٽون(lnum - 1, 1L);
			    وإلا
			    {
				إذا (پهريون_سٽ == 0)
				    پهريون_سٽ = lnum;
				آخري_سٽ = lnum + 1;
			    }
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
			    adjust_props_for_ورهايو(lnum + 1, lnum,
							       plen, 1, FALSE);
#جيڪڏهن_پورو
			    // all line numbers increase
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    did_ورهايو = true;
			    // move the cursor to the new line, like Vi
			    ++curwin->w_cursor.lnum;
			    // copy the rest
			    n = new_شروعات.ڊيگهائي - (size_t)plen;
			    mch_memmove(new_شروعات.string, p1 + 1, n + 1);
			    new_شروعات.ڊيگهائي = n;
			    p1 = new_شروعات.string - 1;
			}
		    }
		    وإلا إذا (has_mbyte)
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
		lastone = (skip_ميلاپ
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
		إذا (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_ورهايو)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, عدم)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    إذا (new_شروعات.string != عدم)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * "matchcol" has to be adjusted, we use the end of
			 * the line as reference, because the substitute may
			 * have changed the number of characters.  Same for
			 * "prev_matchcol".
			 */
			STRCPY(new_شروعات.string + new_شروعات.ڊيگهائي,
			    sub_firstline.string + copycol);
			new_شروعات.ڊيگهائي += sub_firstline.ڊيگهائي - (size_t)copycol;
			matchcol = (colnr_T)(sub_firstline.ڊيگهائي - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.ڊيگهائي
							      - prev_matchcol);

			إذا (u_savesub(lnum) != OK)
			    اكسر;
			ml_replace(lnum, new_شروعات.string, TRUE);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
			إذا (متن_props != عدم)
			    جوڙيو_متن_props(lnum, متن_props, متن_prop_ڳڻپ);
#جيڪڏهن_پورو
			إذا (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    إذا (u_savedel(lnum, nmatch_tl) != OK)
				اكسر;
			    لكل (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    إذا (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (ڊگهو)MAXLNUM, -nmatch_tl);
			    وإلا
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (ڊگهو)MAXLNUM, -nmatch_tl);
			    إذا (subflags.do_ask)
				deleted_سٽون(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			إذا (subflags.do_ask)
			    بدليو_بائيٽون(lnum, 0);
			وإلا
			{
			    إذا (پهريون_سٽ == 0)
				پهريون_سٽ = lnum;
			    آخري_سٽ = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_شروعات.string;
			sub_firstline.ڊيگهائي = new_شروعات.ڊيگهائي;
			new_شروعات.string = عدم;
			new_شروعات.ڊيگهائي = 0;
			matchcol = (colnr_T)(sub_firstline.ڊيگهائي - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.ڊيگهائي
							      - prev_matchcol);
			copycol = 0;
			did_ورهايو = false;
		    }
		    إذا (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, عدم);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    إذا (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			إذا (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			اكسر;
		    }
		}

		سٽ_breakcheck();
	    }

	    إذا (did_sub)
		++sub_nlines;
	    vim_free(new_شروعات.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	سٽ_breakcheck();
    }

    إذا (پهريون_سٽ != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_سٽ_ڳڻپ - old_سٽ_ڳڻپ;
	بدليو_سٽون(پهريون_سٽ, 0, آخري_سٽ - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    vim_free(متن_props);
#جيڪڏهن_پورو

    // ":s/pat//n" doesn't move the cursor
    إذا (subflags.do_ڳڻپ)
	curwin->w_cursor = old_cursor;

    إذا (sub_nsubs > شروعات_nsubs)
    {
	إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_شروعات.lnum = eap->line1;
	    curbuf->b_op_پڇاڙي.lnum = line2;
	    curbuf->b_op_شروعات.کانو = curbuf->b_op_پڇاڙي.کانو = 0;
	}

	إذا (!global_busy)
	{
	    // when interactive leave cursor on the match
	    إذا (!subflags.do_ask)
	    {
		إذا (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		وإلا
		    beginline(BL_WHITE | BL_FIX);
	    }
	    إذا (!do_sub_نياپو(subflags.do_ڳڻپ) && subflags.do_ask)
		نياپو("");
	}
	وإلا
	    global_need_beginline = TRUE;
	إذا (subflags.do_print)
	    print_سٽ(curwin->w_cursor.lnum,
					 subflags.do_عدد, subflags.do_list);
    }
    وإلا إذا (!global_busy)
    {
	إذا (got_int)		// interrupted
	    emsg(_(e_interrupted));
	وإلا إذا (got_ميلاپ)	// did find something but nothing substituted
	    نياپو("");
	وإلا إذا (subflags.do_error)	// nothing found
	    semsg(_(e_نمونو_not_مليو_str), get_ڳولا_pat());
    }

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (subflags.do_ask && hasAnyFolding(curwin))
	// Cursor position may require updating
	بدليو_دري_setting();
#جيڪڏهن_پورو

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
    صحيح
do_sub_نياپو(
    صحيح	    ڳڻپ_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    إذا (((sub_nsubs > p_report && (ڪنجي_Typed || sub_nlines > 1 || p_report < 1))
		|| ڳڻپ_only)
	    && messaging())
    {
	حرف	*نياپو_single;
	حرف	*نياپو_plural;

	إذا (got_int)
	    STRCPY(نياپو_buf, _("(Interrupted) "));
	وإلا
	    *نياپو_buf = NUL;

	نياپو_single = ڳڻپ_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	نياپو_plural = ڳڻپ_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_جوڙيو(نياپو_buf, حجم(نياپو_buf),
				 NGETTEXT(نياپو_single, نياپو_plural, sub_nlines),
				 sub_nsubs, (ڊگهو)sub_nlines);

	إذا (نياپو(نياپو_buf))
	    // save message to display it after redraw
	    سيٽ_keep_نياپو((char_u *)نياپو_buf, 0);
	أرجع TRUE;
    }
    إذا (got_int)
    {
	emsg(_(e_interrupted));
	أرجع TRUE;
    }
    أرجع FALSE;
}

    ساكن فراغ
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.کانو = 0;
    إذا (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", عدم, عدم, DOCMD_NOWAIT);
    وإلا
	do_cmdline(cmd, عدم, عدم, DOCMD_NOWAIT);
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
    فراغ
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    صحيح		ndone = 0;
    صحيح		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*استعمال_ٿيل_pat;
    regmmatch_T	regmatch;
    صحيح		ميلاپ;
    صحيح		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    إذا (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_سٽ_ڳڻپ))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	أرجع;
    }

    إذا (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    وإلا
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_آخري;	    // default: use last used regexp

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (اندريون_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	أرجع;
#جيڪڏهن_پورو

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    إذا (*cmd == '\\')
    {
	++cmd;
	إذا (vim_strchr((char_u *)"/?&", *cmd) == عدم)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    أرجع;
	}
	إذا (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	وإلا
	    which_pat = RE_ڳولا;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    وإلا إذا (*cmd == NUL)
    {
	emsg(_(e_regular_اظهار_missing_from_global));
	أرجع;
    }
    وإلا إذا (check_regexp_delim(*cmd) == FAIL)
    {
	أرجع;
    }
    وإلا
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, عدم, عدم);
	إذا (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    إذا (ڳولا_regcomp(pat, patlen, &استعمال_ٿيل_pat, RE_BOTH, which_pat, ڳولا_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	أرجع;
    }

    إذا (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	ميلاپ = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, عدم);
	إذا ((type == 'g' && ميلاپ) || (type == 'v' && !ميلاپ))
	    global_exe_one(cmd, lnum);
    }
    وإلا
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	لكل (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    ميلاپ = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, عدم);
	    إذا (regmatch.regprog == عدم)
		اكسر;  // re-compiling regprog failed
	    إذا ((type == 'g' && ميلاپ) || (type == 'v' && !ميلاپ))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    سٽ_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	إذا (got_int)
	    نياپو(_(e_interrupted));
	وإلا إذا (ndone == 0)
	{
	    إذا (type == 'v')
	    {
		إذا (اندريون_vim9script())
		    semsg(_(e_نمونو_مليو_اندريون_every_سٽ_str), استعمال_ٿيل_pat);
		وإلا
		    smsg(_("Pattern found in every line: %s"), استعمال_ٿيل_pat);
	    }
	    وإلا
	    {
		إذا (اندريون_vim9script())
		    semsg(_(e_نمونو_not_مليو_str), استعمال_ٿيل_pat);
		وإلا
		    smsg(_("Pattern not found: %s"), استعمال_ٿيل_pat);
	    }
	}
	وإلا
	{
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	    inc_clip_provider();
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	    شروعات_global_changes();
#جيڪڏهن_پورو
	    global_exe(cmd);
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	    پڇاڙي_global_changes();
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	    dec_clip_provider();
#جيڪڏهن_پورو
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Execute "cmd" on lines marked with ml_setmarked().
 */
    فراغ
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
    نياپو_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_سٽ_ڳڻپ;
    طالما (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    إذا (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    وإلا
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    بدليو_سٽ_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    إذا (نياپو_کانو == 0 && نياپو_scrolled == 0)
	نياپو_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    إذا (!do_sub_نياپو(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_سٽ_ڳڻپ - old_lcount);
}

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(فراغ)
{
    أرجع old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    فراغ
سيٽ_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#جيڪڏهن_پورو // FEAT_VIMINFO

#إذا defined(EXITFREE)
    فراغ
free_old_sub(فراغ)
{
    vim_free(old_sub);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    صحيح
prepare_tagpreview(
    صحيح		undo_sync,	    // sync undo when leaving the window
    صحيح		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    إذا (curwin->w_p_pvw)
	أرجع FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    إذا (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_ڳوليو_preview_دري();
	إذا (wp != عدم)
	    popup_سيٽ_wantpos_cursor(wp, wp->w_minwidth, عدم);
    }
    وإلا إذا (use_popup != USEPOPUP_NONE)
    {
	wp = popup_ڳوليو_info_دري();
	إذا (wp != عدم)
	{
	    إذا (use_popup == USEPOPUP_عام)
		popup_show(wp);
	    وإلا
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    وإلا
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_p_pvw)
		اكسر;
    }
    إذا (wp != عدم)
    {
	win_enter(wp, undo_sync);
	أرجع FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    إذا ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	أرجع popup_create_preview_دري(use_popup != USEPOPUP_NONE);
# endif
    إذا (win_ورهايو(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	أرجع FALSE;
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
    أرجع TRUE;
}

#جيڪڏهن_پورو

/*
 * Make the user happy.
 */
    فراغ
ex_smile(exarg_T *eap اڻ_استعمال_ٿيل)
{
    ساكن حرف *ڪوڊ[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    حرف *p;
    صحيح n;
    صحيح i;

    نياپو_شروعات();
    نياپو_putchar('\n');
    لكل (i = 0; i < 2; ++i)
	لكل (p = ڪوڊ[i]; *p != NUL; ++p)
	    إذا (*p == 'x')
		نياپو_putchar('\n');
	    وإلا
		لكل (n = *p++; n > 0; --n)
		    إذا (*p == 'o' || *p == '$')
			نياپو_putchar_attr(*p, HL_ATTR(HLF_L));
		    وإلا
			نياپو_putchar(*p);
    نياپو_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    فراغ
ex_drop(exarg_T *eap)
{
    صحيح		ورهايو = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    إذا (ERROR_IF_POPUP_دري || ERROR_IF_رڪن_POPUP_دري)
	أرجع;

    /*
     * Check if the first argument is already being edited in a window.  If
     * so, jump to that window.
     * We would actually need to check all arguments, but that's complicated
     * and mostly only one file is dropped.
     * This also ignores wildcards, since it is very unlikely the user is
     * editing a file name with a wildcard character.
     */
    سيٽ_arglist(eap->arg);

    /*
     * Expanding wildcards may result in an empty argument list.  E.g. when
     * editing "foo.pyc" and ".pyc" is in 'wildignore'.  Assume that we
     * already did an error message for this.
     */
    إذا (ARGCOUNT == 0)
	أرجع;

    إذا (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	أرجع;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (wp->w_بفر == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    إذا (!buf_Is_بدليو(curbuf))
	    {
		صحيح save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    إذا (curbuf->b_ml.ml_flags & ML_خالي_جايون)
		ex_rewind(eap);
	    أرجع;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    إذا (!buf_hide(curbuf))
    {
	++emsg_off;
	ورهايو = check_بدليو(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    إذا (ورهايو)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    وإلا
	eap->cmdidx = CMD_پهريون;
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
skip_vimgrep_pat(char_u *p, char_u **s, صحيح *flags)
{
    أرجع skip_vimgrep_pat_ext(p, s, flags, عدم, عدم);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, صحيح *flags, char_u **nulp, صحيح *cp)
{
    صحيح		c;

    إذا (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	إذا (s != عدم)
	    *s = p;
	p = skiptowhite(p);
	إذا (s != عدم && *p != NUL)
	{
	    إذا (nulp != عدم)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    وإلا
    {
	// ":vimgrep /pattern/[g][j] fname"
	إذا (s != عدم)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	إذا (*p != c)
	    أرجع عدم;

	// Truncate the pattern.
	إذا (s != عدم)
	{
	    إذا (nulp != عدم)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	طالما (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    إذا (flags != عدم)
	    {
		إذا (*p == 'g')
		    *flags |= VGR_GLOBAL;
		وإلا إذا (*p == 'j')
		    *flags |= VGR_NOJUMP;
		وإلا
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    أرجع p;
}

#إذا defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    فراغ
ex_oldfiles(exarg_T *eap اڻ_استعمال_ٿيل)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    صحيح		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    صحيح		matches = -1;

    إذا (l == عدم)
    {
	نياپو(_("No old files"));
	أرجع;
    }

    نياپو_شروعات();
    نياپو_scroll = TRUE;
    لكل (li = l->lv_پهريون; li != عدم && !got_int; li = li->li_ايندڙ)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	إذا (!پيغام_filtered(fname))
	{
	    إذا (matches < 0)
		matches = nr;
	    وإلا
		matches = 0;
	    نياپو_outnum((ڊگهو)nr);
	    نياپو_puts(": ");
	    نياپو_outtrans(fname);
	    نياپو_clr_eos();
	    نياپو_putchar('\n');
	    ٻاهريون_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_وڌيڪ = FALSE;
	// we only need to prompt if there is more than 1 match
	إذا (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	وإلا
	    nr = prompt_for_عدد(FALSE);
	نياپو_starthere();
	إذا (nr > 0)
	{
	    char_u *p = list_ڳوليو_str(get_vim_var_list(VV_OLDFILES),
		    (ڊگهو)nr);

	    إذا (p != عدم)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, عدم);
		vim_free(p);
	    }
	}
    }
# endif
}
#جيڪڏهن_پورو
