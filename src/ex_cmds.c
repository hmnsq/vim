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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ಅನ್ತರ್ಭಾವ <float.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ linelen(ಪೂರ್ಣಾಂಕ *has_tab);
ಸ್ಥಿರ ಶೂನ್ಯ do_filter(linenr_T line1, linenr_T line2, exarg_T *eap, char_u *cmd, ಪೂರ್ಣಾಂಕ do_ಒಳ, ಪೂರ್ಣಾಂಕ do_ಹೊರಹೋಗು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ not_writing(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_readonly(ಪೂರ್ಣಾಂಕ *forceit, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ delbuf_ಸಂದೇಶ(char_u *name);

/*
 * ":ascii" and "ga".
 */
    ಶೂನ್ಯ
do_ascii(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		cval;
    ಅಕ್ಷರ	buf1[20];
    ಅಕ್ಷರ	buf2[20];
    char_u	buf3[7];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
    char_u      *dig;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		cc[MAX_MCO];
    ಪೂರ್ಣಾಂಕ		ci = 0;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    ಇಲ್ಲದಿದ್ದರೆ
	c = gchar_cursor();
    ಆದರೆ (c == NUL)
    {
	ಸಂದೇಶ("NUL");
	ಹಿಂತಿರುಗಿ;
    }

    IObuff[0] = NUL;
    ಆದರೆ (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	ಆದರೆ (c == NL)	    // NUL is stored as NL
	    c = NUL;
	ಆದರೆ (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	ಇಲ್ಲದಿದ್ದರೆ
	    cval = c;
	ಆದರೆ (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, ಗಾತ್ರ(buf1), "  <%s>", (ಅಕ್ಷರ *)buf3);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf1[0] = NUL;
	ಆದರೆ (c >= 0x80)
	    vim_snprintf(buf2, ಗಾತ್ರ(buf2), "  <M-%s>",
						 (ಅಕ್ಷರ *)transchar(c & 0x7f));
	ಇಲ್ಲದಿದ್ದರೆ
	    buf2[0] = NUL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	ಆದರೆ (dig != ಖಾಲಿ)
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	ಆದರೆ (enc_utf8)
	    c = cc[ci++];
	ಇಲ್ಲದಿದ್ದರೆ
	    c = 0;
    }

    // Repeat for combining characters.
    ಆಗ (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	ಆದರೆ (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	ಆದರೆ (enc_utf8 && utf_iscomposing(c)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GUI
		&& !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	ಆದರೆ (dig != ಖಾಲಿ)
	    vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	ಆದರೆ (ci == MAX_MCO)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (enc_utf8)
	    c = cc[ci++];
	ಇಲ್ಲದಿದ್ದರೆ
	    c = 0;
    }

    ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
}

/*
 * ":left", ":center" and ":right": align text.
 */
    ಶೂನ್ಯ
ex_align(exarg_T *eap)
{
    pos_T	save_curpos;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		indent = 0;
    ಪೂರ್ಣಾಂಕ		new_indent;
    ಪೂರ್ಣಾಂಕ		has_tab;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl)
    {
	// switch left and right aligning
	ಆದರೆ (eap->cmdidx == CMD_ಬಲ)
	    eap->cmdidx = CMD_ಎಡ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_ಎಡ)
	    eap->cmdidx = CMD_ಬಲ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಅಗಲ = atoi((ಅಕ್ಷರ *)eap->arg);
    save_curpos = curwin->w_cursor;
    ಆದರೆ (eap->cmdidx == CMD_ಎಡ)    // width is used for new indent
    {
	ಆದರೆ (ಅಗಲ >= 0)
	    indent = ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	ಆದರೆ (ಅಗಲ <= 0)
	    ಅಗಲ = curbuf->b_p_tw;
	ಆದರೆ (ಅಗಲ == 0 && curbuf->b_p_wm > 0)
	    ಅಗಲ = curwin->w_ಅಗಲ - curbuf->b_p_wm;
	ಆದರೆ (ಅಗಲ <= 0)
	    ಅಗಲ = 80;
    }

    ಆದರೆ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	ಆದರೆ (eap->cmdidx == CMD_ಎಡ)		// left align
	    new_indent = indent;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    has_tab = FALSE;	// avoid uninit warnings
	    len = linelen(eap->cmdidx == CMD_ಬಲ ? &has_tab
						   : ಖಾಲಿ) - get_indent();

	    ಆದರೆ (len <= 0)			// skip blank lines
		ಮುಂದುವರಿ;

	    ಆದರೆ (eap->cmdidx == CMD_center)
		new_indent = (ಅಗಲ - len) / 2;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		new_indent = ಅಗಲ - len;	// right align

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		ಆದರೆ (has_tab)
		    ಆಗ (new_indent > 0)
		    {
			(ಶೂನ್ಯ)ಗಣ_indent(new_indent, 0);
			ಆದರೆ (linelen(ಖಾಲಿ) <= ಅಗಲ)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    ಮಾಡು
				(ಶೂನ್ಯ)ಗಣ_indent(++new_indent, 0);
			    ಆಗ (linelen(ಖಾಲಿ) <= ಅಗಲ);
			    --new_indent;
			    ನಿಲ್ಲಿಸು;
			}
			--new_indent;
		    }
	    }
	}
	ಆದರೆ (new_indent < 0)
	    new_indent = 0;
	(ಶೂನ್ಯ)ಗಣ_indent(new_indent, 0);		// set indent
    }
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
linelen(ಪೂರ್ಣಾಂಕ *has_tab)
{
    char_u  *ಸಾಲು;
    char_u  *ಮೊದಲ;
    char_u  *ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ	    save;
    ಪೂರ್ಣಾಂಕ	    len;

    // Get the line.  If it's empty bail out early (could be the empty string
    // for an unloaded buffer).
    ಸಾಲು = ml_get_curline();
    ಆದರೆ (*ಸಾಲು == NUL)
	ಹಿಂತಿರುಗಿ 0;

    // find the first non-blank character
    ಮೊದಲ = skipwhite(ಸಾಲು);

    // find the character after the last non-blank character
    ಗಾಗಿ (ಕೊನೆಯ = ಮೊದಲ + STRLEN(ಮೊದಲ);
				ಕೊನೆಯ > ಮೊದಲ && VIM_ISWHITE(ಕೊನೆಯ[-1]); --ಕೊನೆಯ)
	;
    save = *ಕೊನೆಯ;
    *ಕೊನೆಯ = NUL;
    len = linetabsize_str(ಸಾಲು);	// get line length on screen
    ಆದರೆ (has_tab != ಖಾಲಿ)		// check for embedded TAB
	*has_tab = (vim_strchr(ಮೊದಲ, TAB) != ಖಾಲಿ);
    *ಕೊನೆಯ = save;

    ಹಿಂತಿರುಗಿ len;
}

// Buffer for two lines used during sorting.  They are allocated to
// contain the longest line being sorted.
ಸ್ಥಿರ char_u	*sortbuf1;
ಸ್ಥಿರ char_u	*sortbuf2;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_lc;	// sort using locale
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_ic;	// ignore case
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_nr;	// sort on number
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_rx;	// sort on regex instead of skipping it
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_flt;	// sort on floating number

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_abort;	// flag to indicate if sorting has been interrupted

// Struct to store info to be sorted.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	lnum;			// line number
    ಒಕ್ಕೂಟ {
	ರಚನೆ
	{
	    varnumber_T	ಆರಂಭ_ಲಂಬಸಾಲು_nr;	// starting column number
	    varnumber_T	ಕೊನೆ_ಲಂಬಸಾಲು_nr;	// ending column number
	} ಸಾಲು;
	ರಚನೆ
	{
	    varnumber_T	ಮೌಲ್ಯ;		// value if sorting by integer
	    ಪೂರ್ಣಾಂಕ is_ಸಂಖ್ಯೆ;		// TRUE when line contains a number
	} ಅಂಕೆ;
	float_T ಮೌಲ್ಯ_flt;		// value if sorting by float
    } st_u;
} sorti_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
string_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಆದರೆ (ವಿಂಗಡಿಸು_lc)
	ಹಿಂತಿರುಗಿ strcoll((ಅಕ್ಷರ *)s1, (ಅಕ್ಷರ *)s2);
    ಹಿಂತಿರುಗಿ ವಿಂಗಡಿಸು_ic ? STRICMP(s1, s2) : STRCMP(s1, s2);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ವಿಂಗಡಿಸು_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    sorti_T	l1 = *(sorti_T *)s1;
    sorti_T	l2 = *(sorti_T *)s2;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = 0;

    // If the user interrupts, there's no way to stop qsort() immediately, but
    // if we return 0 every time, qsort will assume it's done sorting and
    // exit.
    ಆದರೆ (ವಿಂಗಡಿಸು_abort)
	ಹಿಂತಿರುಗಿ 0;
    fast_breakcheck();
    ಆದರೆ (got_int)
	ವಿಂಗಡಿಸು_abort = TRUE;

    ಆದರೆ (ವಿಂಗಡಿಸು_nr)
    {
	ಆದರೆ (l1.st_u.ಅಂಕೆ.is_ಸಂಖ್ಯೆ != l2.st_u.ಅಂಕೆ.is_ಸಂಖ್ಯೆ)
	    ಫಲಿತಾಂಶ = l1.st_u.ಅಂಕೆ.is_ಸಂಖ್ಯೆ > l2.st_u.ಅಂಕೆ.is_ಸಂಖ್ಯೆ ? 1 : -1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = l1.st_u.ಅಂಕೆ.ಮೌಲ್ಯ == l2.st_u.ಅಂಕೆ.ಮೌಲ್ಯ ? 0
			     : l1.st_u.ಅಂಕೆ.ಮೌಲ್ಯ > l2.st_u.ಅಂಕೆ.ಮೌಲ್ಯ ? 1 : -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಿಂಗಡಿಸು_flt)
	ಫಲಿತಾಂಶ = l1.st_u.ಮೌಲ್ಯ_flt == l2.st_u.ಮೌಲ್ಯ_flt ? 0
			     : l1.st_u.ಮೌಲ್ಯ_flt > l2.st_u.ಮೌಲ್ಯ_flt ? 1 : -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// We need to copy one line into "sortbuf1", because there is no
	// guarantee that the first pointer becomes invalid when obtaining the
	// second one.
	STRNCPY(sortbuf1, ml_get(l1.lnum) + l1.st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr,
		     l1.st_u.ಸಾಲು.ಕೊನೆ_ಲಂಬಸಾಲು_nr - l1.st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr + 1);
	sortbuf1[l1.st_u.ಸಾಲು.ಕೊನೆ_ಲಂಬಸಾಲು_nr - l1.st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr] = 0;
	STRNCPY(sortbuf2, ml_get(l2.lnum) + l2.st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr,
		     l2.st_u.ಸಾಲು.ಕೊನೆ_ಲಂಬಸಾಲು_nr - l2.st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr + 1);
	sortbuf2[l2.st_u.ಸಾಲು.ಕೊನೆ_ಲಂಬಸಾಲು_nr - l2.st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr] = 0;

	ಫಲಿತಾಂಶ = string_compare(sortbuf1, sortbuf2);
    }

    // If two lines have the same value, preserve the original line order.
    ಆದರೆ (ಫಲಿತಾಂಶ == 0)
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(l1.lnum - l2.lnum);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * ":sort".
 */
    ಶೂನ್ಯ
ex_ವಿಂಗಡಿಸು(exarg_T *eap)
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		len;
    linenr_T	lnum;
    ದೀರ್ಘ	maxlen = 0;
    sorti_T	*nrs;
    size_t	ಎಣಿಕೆ_2 = (size_t)(eap->line2 - eap->line1 + 1);
    size_t	i;
    char_u	*p;
    char_u	*s;
    char_u	*s2;
    char_u	c;			// temporary character storage
    ಪೂರ್ಣಾಂಕ		unique = FALSE;
    ದೀರ್ಘ	deleted;
    colnr_T	ಆರಂಭ_ಲಂಬಸಾಲು;
    colnr_T	ಕೊನೆ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು_what = 0;
    ಪೂರ್ಣಾಂಕ		format_ಸಿಕ್ಕಿತು = 0;
    ಪೂರ್ಣಾಂಕ		change_occurred = FALSE; // Buffer contents changed.

    // Sorting one line is really quick!
    ಆದರೆ (ಎಣಿಕೆ_2 <= 1)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;
    sortbuf1 = ಖಾಲಿ;
    sortbuf2 = ಖಾಲಿ;
    regmatch.regprog = ಖಾಲಿ;
    nrs = ALLOC_MULT(sorti_T, ಎಣಿಕೆ_2);
    ಆದರೆ (nrs == ಖಾಲಿ)
	ಹೋಗು sortend;

    ವಿಂಗಡಿಸು_abort = ವಿಂಗಡಿಸು_ic = ವಿಂಗಡಿಸು_lc = ವಿಂಗಡಿಸು_rx = ವಿಂಗಡಿಸು_nr = 0;
    ವಿಂಗಡಿಸು_flt = 0;

    ಗಾಗಿ (p = eap->arg; *p != NUL; ++p)
    {
	ಆದರೆ (VIM_ISWHITE(*p))
	    ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'i')
	    ವಿಂಗಡಿಸು_ic = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'l')
	    ವಿಂಗಡಿಸು_lc = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'r')
	    ವಿಂಗಡಿಸು_rx = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'n')
	{
	    ವಿಂಗಡಿಸು_nr = 1;
	    ++format_ಸಿಕ್ಕಿತು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'f')
	{
	    ವಿಂಗಡಿಸು_flt = 1;
	    ++format_ಸಿಕ್ಕಿತು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'b')
	{
	    ವಿಂಗಡಿಸು_what = STR2NR_BIN + STR2NR_FORCE;
	    ++format_ಸಿಕ್ಕಿತು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'o')
	{
	    ವಿಂಗಡಿಸು_what = STR2NR_OCT + STR2NR_FORCE;
	    ++format_ಸಿಕ್ಕಿತು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'x')
	{
	    ವಿಂಗಡಿಸು_what = STR2NR_HEX + STR2NR_FORCE;
	    ++format_ಸಿಕ್ಕಿತು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'u')
	    unique = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"')	// comment start
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->nextcmd == ಖಾಲಿ && check_nextcmd(p) != ಖಾಲಿ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ASCII_ISALPHA(*p) && regmatch.regprog == ಖಾಲಿ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು sortend;
	    *s = NUL;
	    // Use last search pattern if sort pattern is empty.
	    ಆದರೆ (s == p + 1)
	    {
		ಆದರೆ (ಕೊನೆಯ_ಹುಡುಕು_pat() == ಖಾಲಿ)
		{
		    emsg(_(e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ));
		    ಹೋಗು sortend;
		}
		regmatch.regprog = vim_regcomp(ಕೊನೆಯ_ಹುಡುಕು_pat(), RE_MAGIC);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		ಹೋಗು sortend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ವಾದ_str), p);
	    ಹೋಗು sortend;
	}
    }

    // Can only have one of 'n', 'b', 'o' and 'x'.
    ಆದರೆ (format_ಸಿಕ್ಕಿತು > 1)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹೋಗು sortend;
    }

    // From here on "sort_nr" is used as a flag for any integer number
    // sorting.
    ವಿಂಗಡಿಸು_nr += ವಿಂಗಡಿಸು_what;

    /*
     * Make an array with all line numbers.  This avoids having to copy all
     * the lines into allocated memory.
     * When sorting on strings "start_col_nr" is the offset in the line, for
     * numbers sorting it's the number to sort on.  This means the pattern
     * matching and number conversion only has to be done once per line.
     * Also get the longest line length for allocating "sortbuf".
     */
    ಗಾಗಿ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	s = ml_get(lnum);
	len = ml_get_len(lnum);
	ಆದರೆ (maxlen < len)
	    maxlen = len;

	ಆರಂಭ_ಲಂಬಸಾಲು = 0;
	ಕೊನೆ_ಲಂಬಸಾಲು = len;
	ಆದರೆ (regmatch.regprog != ಖಾಲಿ && vim_regexec(&regmatch, s, 0))
	{
	    ಆದರೆ (ವಿಂಗಡಿಸು_rx)
	    {
		ಆರಂಭ_ಲಂಬಸಾಲು = (colnr_T)(regmatch.startp[0] - s);
		ಕೊನೆ_ಲಂಬಸಾಲು = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆರಂಭ_ಲಂಬಸಾಲು = (colnr_T)(regmatch.endp[0] - s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
		ಕೊನೆ_ಲಂಬಸಾಲು = 0;

	ಆದರೆ (ವಿಂಗಡಿಸು_nr || ವಿಂಗಡಿಸು_flt)
	{
	    // Make sure vim_str2nr() doesn't read any digits past the end
	    // of the match, by temporarily terminating the string there
	    s2 = s + ಕೊನೆ_ಲಂಬಸಾಲು;
	    c = *s2;
	    *s2 = NUL;
	    // Sorting on number: Store the number itself.
	    p = s + ಆರಂಭ_ಲಂಬಸಾಲು;
	    ಆದರೆ (ವಿಂಗಡಿಸು_nr)
	    {
		ಆದರೆ (ವಿಂಗಡಿಸು_what & STR2NR_HEX)
		    s = skiptohex(p);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಿಂಗಡಿಸು_what & STR2NR_BIN)
		    s = skiptobin(p);
		ಇಲ್ಲದಿದ್ದರೆ
		    s = skiptodigit(p);
		ಆದರೆ (s > p && s[-1] == '-')
		    --s;  // include preceding negative sign
		ಆದರೆ (*s == NUL)
		{
		    // line without number should sort before any number
		    nrs[lnum - eap->line1].st_u.ಅಂಕೆ.is_ಸಂಖ್ಯೆ = FALSE;
		    nrs[lnum - eap->line1].st_u.ಅಂಕೆ.ಮೌಲ್ಯ = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    nrs[lnum - eap->line1].st_u.ಅಂಕೆ.is_ಸಂಖ್ಯೆ = TRUE;
		    vim_str2nr(s, ಖಾಲಿ, ಖಾಲಿ, ವಿಂಗಡಿಸು_what,
			&nrs[lnum - eap->line1].st_u.ಅಂಕೆ.ಮೌಲ್ಯ,
			ಖಾಲಿ, 0, FALSE, ಖಾಲಿ);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		s = skipwhite(p);
		ಆದರೆ (*s == '+')
		    s = skipwhite(s + 1);

		ಆದರೆ (*s == NUL)
		    // empty line should sort before any number
		    nrs[lnum - eap->line1].st_u.ಮೌಲ್ಯ_flt = -DBL_MAX;
		ಇಲ್ಲದಿದ್ದರೆ
		    nrs[lnum - eap->line1].st_u.ಮೌಲ್ಯ_flt =
						      strtod((ಅಕ್ಷರ *)s, ಖಾಲಿ);
	    }
	    *s2 = c;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Store the column to sort at.
	    nrs[lnum - eap->line1].st_u.ಸಾಲು.ಆರಂಭ_ಲಂಬಸಾಲು_nr = ಆರಂಭ_ಲಂಬಸಾಲು;
	    nrs[lnum - eap->line1].st_u.ಸಾಲು.ಕೊನೆ_ಲಂಬಸಾಲು_nr = ಕೊನೆ_ಲಂಬಸಾಲು;
	}

	nrs[lnum - eap->line1].lnum = lnum;

	ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
	    fast_breakcheck();
	ಆದರೆ (got_int)
	    ಹೋಗು sortend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ಆದರೆ (sortbuf1 == ಖಾಲಿ)
	ಹೋಗು sortend;
    sortbuf2 = alloc(maxlen + 1);
    ಆದರೆ (sortbuf2 == ಖಾಲಿ)
	ಹೋಗು sortend;

    // Sort the array of line numbers.  Note: can't be interrupted!
    qsort((ಶೂನ್ಯ *)nrs, ಎಣಿಕೆ_2, ಗಾತ್ರ(sorti_T), ವಿಂಗಡಿಸು_compare);

    ಆದರೆ (ವಿಂಗಡಿಸು_abort)
	ಹೋಗು sortend;

    // Insert the lines in the sorted order below the last one.
    lnum = eap->line2;
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	linenr_T get_lnum = nrs[eap->forceit ? ಎಣಿಕೆ_2 - i - 1 : i].lnum;

	// If the original line number of the line being placed is not the same
	// as "lnum" (accounting for offset), we know that the buffer changed.
	ಆದರೆ (get_lnum + ((linenr_T)ಎಣಿಕೆ_2 - 1) != lnum)
	    change_occurred = TRUE;

	s = ml_get(get_lnum);
	ಆದರೆ (!unique || i == 0 || string_compare(s, sortbuf1) != 0)
	{
	    // Copy the line into a buffer, it may become invalid in
	    // ml_append(). And it's needed for "unique".
	    STRCPY(sortbuf1, s);
	    ಆದರೆ (ml_append(lnum++, sortbuf1, (colnr_T)0, FALSE) == FAIL)
		ನಿಲ್ಲಿಸು;
	}
	fast_breakcheck();
	ಆದರೆ (got_int)
	    ಹೋಗು sortend;
    }

    // delete the original lines if appending worked
    ಆದರೆ (i == ಎಣಿಕೆ_2)
	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	    ml_delete(eap->line1);
    ಇಲ್ಲದಿದ್ದರೆ
	ಎಣಿಕೆ_2 = 0;

    // Adjust marks for deleted (or added) lines and prepare for displaying.
    deleted = (ದೀರ್ಘ)(ಎಣಿಕೆ_2 - (lnum - eap->line2));
    ಆದರೆ (deleted > 0)
    {
	mark_adjust(eap->line2 - deleted, eap->line2, (ದೀರ್ಘ)MAXLNUM, -deleted);
	msgmore(-deleted);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (deleted < 0)
	mark_adjust(eap->line2, MAXLNUM, -deleted, 0L);

    ಆದರೆ (change_occurred || deleted != 0)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

sortend:
    vim_free(nrs);
    vim_free(sortbuf1);
    vim_free(sortbuf2);
    vim_regfree(regmatch.regprog);
    ಆದರೆ (got_int)
	emsg(_(e_interrupted));
}

/*
 * ":uniq".
 */
    ಶೂನ್ಯ
ex_uniq(exarg_T *eap)
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		len;
    linenr_T	lnum;
    ದೀರ್ಘ	maxlen = 0;
    linenr_T	ಎಣಿಕೆ_2 = eap->line2 - eap->line1 + 1;
    char_u	*p;
    char_u	*s;
    char_u	save_c = 0;		// temporary character storage
    ಪೂರ್ಣಾಂಕ		keep_only_unique = FALSE;
    ಪೂರ್ಣಾಂಕ		keep_only_not_unique = eap->forceit ? TRUE : FALSE;
    ದೀರ್ಘ	deleted = 0;
    colnr_T	ಆರಂಭ_ಲಂಬಸಾಲು;
    colnr_T	ಕೊನೆ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		change_occurred = FALSE; // Buffer contents changed.

    // Uniq one line is really quick!
    ಆದರೆ (ಎಣಿಕೆ_2 <= 1)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (u_save((linenr_T)(eap->line1 - 1), (linenr_T)(eap->line2 + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;
    sortbuf1 = ಖಾಲಿ;
    regmatch.regprog = ಖಾಲಿ;

    ವಿಂಗಡಿಸು_abort = ವಿಂಗಡಿಸು_ic = ವಿಂಗಡಿಸು_lc = ವಿಂಗಡಿಸು_rx = ವಿಂಗಡಿಸು_nr = 0;
    ವಿಂಗಡಿಸು_flt = 0;

    ಗಾಗಿ (p = eap->arg; *p != NUL; ++p)
    {
	ಆದರೆ (VIM_ISWHITE(*p))
	    ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'i')
	    ವಿಂಗಡಿಸು_ic = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'l')
	    ವಿಂಗಡಿಸು_lc = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'r')
	    ವಿಂಗಡಿಸು_rx = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'u')
	{
	    // 'u' is only valid when '!' is not given.
	    ಆದರೆ (!keep_only_not_unique)
		keep_only_unique = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"')	// comment start
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->nextcmd == ಖಾಲಿ && check_nextcmd(p) != ಖಾಲಿ)
	{
	    eap->nextcmd = check_nextcmd(p);
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ASCII_ISALPHA(*p) && regmatch.regprog == ಖಾಲಿ)
	{
	    s = skip_regexp_err(p + 1, *p, TRUE);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು uniqend;
	    *s = NUL;
	    // Use last search pattern if uniq pattern is empty.
	    ಆದರೆ (s == p + 1)
	    {
		ಆದರೆ (ಕೊನೆಯ_ಹುಡುಕು_pat() == ಖಾಲಿ)
		{
		    emsg(_(e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ));
		    ಹೋಗು uniqend;
		}
		regmatch.regprog = vim_regcomp(ಕೊನೆಯ_ಹುಡುಕು_pat(), RE_MAGIC);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		regmatch.regprog = vim_regcomp(p + 1, RE_MAGIC);
	    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		ಹೋಗು uniqend;
	    p = s;		// continue after the regexp
	    regmatch.rm_ic = p_ic;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ವಾದ_str), p);
	    ಹೋಗು uniqend;
	}
    }

    // Find the length of the longest line.
    ಗಾಗಿ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
    {
	len = ml_get_len(lnum);
	ಆದರೆ (maxlen < len)
	    maxlen = len;

	ಆದರೆ (got_int)
	    ಹೋಗು uniqend;
    }

    // Allocate a buffer that can hold the longest line.
    sortbuf1 = alloc(maxlen + 1);
    ಆದರೆ (sortbuf1 == ಖಾಲಿ)
	ಹೋಗು uniqend;

    // Delete lines according to options.
    ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ_continue = FALSE;
    ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_is_unmatch = FALSE;
    ಪೂರ್ಣಾಂಕ is_ಹೊಂದಿಕೆ;
    linenr_T ಮುಗಿದ_lnum = eap->line1 - 1;
    linenr_T delete_lnum = 0;
    ಗಾಗಿ (linenr_T i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	linenr_T get_lnum = eap->line1 + i;

	s = ml_get(get_lnum);
	len = ml_get_len(get_lnum);

	ಆರಂಭ_ಲಂಬಸಾಲು = 0;
	ಕೊನೆ_ಲಂಬಸಾಲು = len;
	ಆದರೆ (regmatch.regprog != ಖಾಲಿ && vim_regexec(&regmatch, s, 0))
	{
	    ಆದರೆ (ವಿಂಗಡಿಸು_rx)
	    {
		ಆರಂಭ_ಲಂಬಸಾಲು = (colnr_T)(regmatch.startp[0] - s);
		ಕೊನೆ_ಲಂಬಸಾಲು = (colnr_T)(regmatch.endp[0] - s);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆರಂಭ_ಲಂಬಸಾಲು = (colnr_T)(regmatch.endp[0] - s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
		ಕೊನೆ_ಲಂಬಸಾಲು = 0;
	ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು > 0)
	{
	    save_c = s[ಕೊನೆ_ಲಂಬಸಾಲು];
	    s[ಕೊನೆ_ಲಂಬಸಾಲು] = NUL;
	}

	is_ಹೊಂದಿಕೆ = i > 0 ? !string_compare(&s[ಆರಂಭ_ಲಂಬಸಾಲು], sortbuf1) : FALSE;
	delete_lnum = 0;
	ಆದರೆ (ಮುಂದಿನ_is_unmatch)
	{
	    is_ಹೊಂದಿಕೆ = FALSE;
	    ಮುಂದಿನ_is_unmatch = FALSE;
	}

	ಆದರೆ (!keep_only_unique && !keep_only_not_unique)
	{
	    ಆದರೆ (is_ಹೊಂದಿಕೆ)
		delete_lnum = get_lnum;
	    ಇಲ್ಲದಿದ್ದರೆ
		STRCPY(sortbuf1, &s[ಆರಂಭ_ಲಂಬಸಾಲು]);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (keep_only_not_unique)
	{
	    ಆದರೆ (is_ಹೊಂದಿಕೆ)
	    {
		ಮುಗಿದ_lnum = get_lnum - 1;
		delete_lnum = get_lnum;
		ಹೊಂದಿಕೆ_continue = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (i > 0 && !ಹೊಂದಿಕೆ_continue && get_lnum - 1 > ಮುಗಿದ_lnum)
		{
		    delete_lnum = get_lnum - 1;
		    ಮುಂದಿನ_is_unmatch = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= ಎಣಿಕೆ_2 - 1)
		    delete_lnum = get_lnum;
		ಹೊಂದಿಕೆ_continue = FALSE;
		STRCPY(sortbuf1, &s[ಆರಂಭ_ಲಂಬಸಾಲು]);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ // keep_only_unique
	{
	    ಆದರೆ (is_ಹೊಂದಿಕೆ)
	    {
		ಆದರೆ (!ಹೊಂದಿಕೆ_continue)
		    delete_lnum = get_lnum - 1;
		ಇಲ್ಲದಿದ್ದರೆ
		    delete_lnum = get_lnum;
		ಹೊಂದಿಕೆ_continue = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (i == 0 && ಹೊಂದಿಕೆ_continue)
		    delete_lnum = get_lnum;
		ಹೊಂದಿಕೆ_continue = FALSE;
		STRCPY(sortbuf1, &s[ಆರಂಭ_ಲಂಬಸಾಲು]);
	    }
	}

	ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು > 0)
	    s[ಕೊನೆ_ಲಂಬಸಾಲು] = save_c;

	ಆದರೆ (delete_lnum > 0)
	{
	    ml_delete(delete_lnum);
	    i -= get_lnum - delete_lnum + 1;
	    ಎಣಿಕೆ_2--;
	    deleted++;
	    change_occurred = TRUE;
	}

	fast_breakcheck();
	ಆದರೆ (got_int)
	    ಹೋಗು uniqend;
    }

    // Adjust marks for deleted lines and prepare for displaying.
    mark_adjust(eap->line2 - deleted, eap->line2, (ದೀರ್ಘ)MAXLNUM, -deleted);
    msgmore(-deleted);

    ಆದರೆ (change_occurred)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(eap->line1, 0, eap->line2 + 1, -deleted);

    curwin->w_cursor.lnum = eap->line1;
    beginline(BL_WHITE | BL_FIX);

uniqend:
    vim_free(sortbuf1);
    vim_regfree(regmatch.regprog);
    ಆದರೆ (got_int)
	emsg(_(e_interrupted));
}

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
do_move(linenr_T line1, linenr_T line2, linenr_T dest)
{
    char_u	*str;
    linenr_T	l;
    linenr_T	extra;	    // Num lines added before line1
    linenr_T	ಅಂಕೆ_ಸಾಲುಗಳು;  // Num lines moved
    linenr_T	ಕೊನೆಯ_ಸಾಲು;  // Last line in file after adding new text
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    win_T	*win;
    tabpage_T	*tp;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (dest >= line1 && dest < line2)
    {
	emsg(_(e_cannot_move_range_ದ_ಸಾಲುಗಳು_into_itself));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Do nothing if we are not actually moving any lines.  This will prevent
    // the 'modified' flag from being set without cause.
    ಆದರೆ (dest == line1 - 1 || dest == line2)
    {
	// Move the cursor as if lines were moved (see below) to be backwards
	// compatible.
	ಆದರೆ (dest >= line1)
	    curwin->w_cursor.lnum = dest;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

	ಹಿಂತಿರುಗಿ OK;
    }

    ಅಂಕೆ_ಸಾಲುಗಳು = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    ಆದರೆ (u_save(dest, dest + 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಾಗಿ (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strnsave(ml_get(l + extra), ml_get_len(l + extra));
	ಆದರೆ (str != ಖಾಲಿ)
	{
	    ml_append(dest + l - line1, str, (colnr_T)0, FALSE);
	    vim_free(str);
	    ಆದರೆ (dest < line1)
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
    ಕೊನೆಯ_ಸಾಲು = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    mark_adjust_nofold(line1, line2, ಕೊನೆಯ_ಸಾಲು - line2, 0L);
    ಆದರೆ (dest >= line2)
    {
	mark_adjust_nofold(line2 + 1, dest, -ಅಂಕೆ_ಸಾಲುಗಳು, 0L);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ಆದರೆ (win->w_ಬಫರ್ == curbuf)
		foldMoveRange(&win->w_folds, line1, line2, dest);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_ಆರಂಭ.lnum = dest - ಅಂಕೆ_ಸಾಲುಗಳು + 1;
	    curbuf->b_op_ಕೊನೆ.lnum = dest;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mark_adjust_nofold(dest + 1, line1 - 1, ಅಂಕೆ_ಸಾಲುಗಳು, 0L);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	FOR_ALL_TAB_WINDOWS(tp, win)
	{
	    ಆದರೆ (win->w_ಬಫರ್ == curbuf)
		foldMoveRange(&win->w_folds, dest + 1, line1 - 1, line2);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    curbuf->b_op_ಆರಂಭ.lnum = dest + 1;
	    curbuf->b_op_ಕೊನೆ.lnum = dest + ಅಂಕೆ_ಸಾಲುಗಳು;
	}
    }
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
    mark_adjust_nofold(ಕೊನೆಯ_ಸಾಲು - ಅಂಕೆ_ಸಾಲುಗಳು + 1, ಕೊನೆಯ_ಸಾಲು,
					     -(ಕೊನೆಯ_ಸಾಲು - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    ಆದರೆ (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (l = line1; l <= line2; l++)
	ml_delete_flags(line1 + extra, ML_DEL_ಸಂದೇಶ);

    ಆದರೆ (!global_busy && ಅಂಕೆ_ಸಾಲುಗಳು > p_report)
	smsg(NGETTEXT("%ld line moved", "%ld lines moved", ಅಂಕೆ_ಸಾಲುಗಳು),
			(ದೀರ್ಘ)ಅಂಕೆ_ಸಾಲುಗಳು);

    /*
     * Leave the cursor on the last of the moved lines.
     */
    ಆದರೆ (dest >= line1)
	curwin->w_cursor.lnum = dest;
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    ಆದರೆ (line1 < dest)
    {
	dest += ಅಂಕೆ_ಸಾಲುಗಳು + 1;
	ಕೊನೆಯ_ಸಾಲು = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಆದರೆ (dest > ಕೊನೆಯ_ಸಾಲು + 1)
	    dest = ಕೊನೆಯ_ಸಾಲು + 1;
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(line1, 0, dest, 0L);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(dest + 1, 0, line1 + ಅಂಕೆ_ಸಾಲುಗಳು, 0L);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * ":copy"
 */
    ಶೂನ್ಯ
ex_copy(linenr_T line1, linenr_T line2, linenr_T n)
{
    linenr_T	ಎಣಿಕೆ_2;
    char_u	*p;

    ಎಣಿಕೆ_2 = line2 - line1 + 1;
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_ಆರಂಭ.lnum = n + 1;
	curbuf->b_op_ಕೊನೆ.lnum = n + ಎಣಿಕೆ_2;
	curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
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
    ಆದರೆ (u_save(n, n + 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    curwin->w_cursor.lnum = n;
    ಆಗ (line1 <= line2)
    {
	// need to make a copy because the line will be unlocked within
	// ml_append()
	p = vim_strnsave(ml_get(line1), ml_get_len(line1));
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_T)0, FALSE);
	    vim_free(p);
	}
	// situation 2: skip already copied lines
	ಆದರೆ (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	ಆದರೆ (curwin->w_cursor.lnum < line1)
	    ++line1;
	ಆದರೆ (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }

    appended_ಸಾಲುಗಳು_mark(n, ಎಣಿಕೆ_2);
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	check_pos(curbuf, &VIsual);

    msgmore((ದೀರ್ಘ)ಎಣಿಕೆ_2);
}

ಸ್ಥಿರ char_u	*prevcmd = ಖಾಲಿ;	// the previous command

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_prev_shellcmd(ಶೂನ್ಯ)
{
    vim_free(prevcmd);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check that "prevcmd" is not NULL.  If it is NULL then give an error message
 * and return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prevcmd_is_ಗಣ(ಶೂನ್ಯ)
{
    ಆದರೆ (prevcmd == ಖಾಲಿ)
    {
	emsg(_(e_no_ಹಿಂದಿನ_command));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    ಶೂನ್ಯ
do_bang(
    ಪೂರ್ಣಾಂಕ		addr_ಎಣಿಕೆ,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		forceit,
    ಪೂರ್ಣಾಂಕ		do_ಒಳ,
    ಪೂರ್ಣಾಂಕ		do_ಹೊರಹೋಗು)
{
    char_u		*arg = eap->arg;	// command
    linenr_T		line1 = eap->line1;	// start of range
    linenr_T		line2 = eap->line2;	// end of range
    char_u		*newcmd = ಖಾಲಿ;		// the new command
    ಪೂರ್ಣಾಂಕ			free_newcmd = FALSE;    // need to free() newcmd
    ಪೂರ್ಣಾಂಕ			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    ಪೂರ್ಣಾಂಕ			len;
    ಪೂರ್ಣಾಂಕ			scroll_save = ಸಂದೇಶ_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (addr_ಎಣಿಕೆ == 0)		// :!
    {
	ಸಂದೇಶ_scroll = FALSE;	    // don't scroll here
	autowrite_all();
	ಸಂದೇಶ_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in ":!<cmd> ! [args]"
     * ":!!" is indicated by the 'forceit' variable.
     */
    ins_prevcmd = forceit;

    // Skip leading white space to avoid a strange error with some shells.
    trailarg = skipwhite(arg);
    ಮಾಡು
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(trailarg) + 1;
	ಆದರೆ (newcmd != ಖಾಲಿ)
	    len += (ಪೂರ್ಣಾಂಕ)STRLEN(newcmd);
	ಆದರೆ (ins_prevcmd)
	{
	    ಆದರೆ (!prevcmd_is_ಗಣ())
	    {
		vim_free(newcmd);
		ಹಿಂತಿರುಗಿ;
	    }
	    len += (ಪೂರ್ಣಾಂಕ)STRLEN(prevcmd);
	}
	ಆದರೆ ((t = alloc(len)) == ಖಾಲಿ)
	{
	    vim_free(newcmd);
	    ಹಿಂತಿರುಗಿ;
	}
	*t = NUL;
	ಆದರೆ (newcmd != ಖಾಲಿ)
	    STRCAT(t, newcmd);
	ಆದರೆ (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = ಖಾಲಿ;
	ಆಗ (*p)
	{
	    ಆದರೆ (*p == '!')
	    {
		ಆದರೆ (p > newcmd && p[-1] == '\\')
		    STRMOVE(p - 1, p);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ++p;
	}
    } ಆಗ (trailarg != ಖಾಲಿ);

    // Only set "prevcmd" if there is a command to run, otherwise keep te one
    // we have.
    ಆದರೆ (STRLEN(newcmd) > 0)
    {
	vim_free(prevcmd);
	prevcmd = newcmd;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	free_newcmd = TRUE;

    ಆದರೆ (bangredo)	    // put cmd in redo buffer for ! command
    {
	ಆದರೆ (!prevcmd_is_ಗಣ())
	    ಹೋಗು theend;

	// If % or # appears in the command, it must have been escaped.
	// Reescape them, so that redoing them does not substitute them by the
	// buffername.
	char_u *cmd = vim_strsave_escaped(prevcmd, (char_u *)"%#");

	ಆದರೆ (cmd != ಖಾಲಿ)
	{
	    Append_ಗಮ್ಯ_Redobuff_Lit(cmd, -1);
	    vim_free(cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    Append_ಗಮ್ಯ_Redobuff_Lit(prevcmd, -1);
	Append_ಗಮ್ಯ_Redobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    ಆದರೆ (*p_shq != NUL)
    {
	ಆದರೆ (free_newcmd)
	    vim_free(newcmd);
	newcmd = alloc(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1);
	ಆದರೆ (newcmd == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    ಆದರೆ (addr_ಎಣಿಕೆ == 0)		// :!
    {
	// echo the command
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_putchar(':');
	ಸಂದೇಶ_putchar('!');
	ಸಂದೇಶ_outtrans(newcmd);
	ಸಂದೇಶ_clr_eos();
	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);

	do_shell(newcmd, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ				// :range!
    {
	// Careful: This may recursively call do_bang() again! (because of
	// autocommands)
	do_filter(line1, line2, eap, newcmd, do_ಒಳ, do_ಹೊರಹೋಗು);
	apply_autocmds(EVENT_SHELLFILTERPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    }

theend:
    ಆದರೆ (free_newcmd)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
do_filter(
    linenr_T	line1,
    linenr_T	line2,
    exarg_T	*eap,		// for forced 'ff' and 'fenc'
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		do_ಒಳ,
    ಪೂರ್ಣಾಂಕ		do_ಹೊರಹೋಗು)
{
    char_u	*itmp = ಖಾಲಿ;
    char_u	*otmp = ಖಾಲಿ;
    linenr_T	linecount;
    linenr_T	read_linecount;
    pos_T	cursor_save;
    char_u	*cmd_buf;
    buf_T	*old_curbuf = curbuf;
    ಪೂರ್ಣಾಂಕ		shell_flags = 0;
    pos_T	orig_ಆರಂಭ = curbuf->b_op_ಆರಂಭ;
    pos_T	orig_ಕೊನೆ = curbuf->b_op_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		save_cmod_flags = cmdmod.cmod_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FILTERPIPE
    ಪೂರ್ಣಾಂಕ		stmp = p_stmp;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (*cmd == NUL)	    // no filter command
	ಹಿಂತಿರುಗಿ;

    // Temporarily disable lockmarks since that's needed to propagate changed
    // regions of the buffer for foldUpdate(), linecount, etc.
    cmdmod.cmod_flags &= ~CMOD_LOCKMARKS;

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
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

    ಆದರೆ (do_ಹೊರಹೋಗು)
	shell_flags |= SHELL_DOOUT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FILTERPIPE
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
	stmp = 1;   // Console mode doesn't support filterpipe.
# endif

    ಆದರೆ (!do_ಒಳ && do_ಹೊರಹೋಗು && !stmp)
    {
	// Use a pipe to fetch stdout of the command, do not use a temp file.
	shell_flags |= SHELL_READ;
	curwin->w_cursor.lnum = line2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_ಒಳ && !do_ಹೊರಹೋಗು && !stmp)
    {
	// Use a pipe to write stdin of the command, do not use a temp file.
	shell_flags |= SHELL_WRITE;
	curbuf->b_op_ಆರಂಭ.lnum = line1;
	curbuf->b_op_ಕೊನೆ.lnum = line2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_ಒಳ && do_ಹೊರಹೋಗು && !stmp)
    {
	// Use a pipe to write stdin and fetch stdout of the command, do not
	// use a temp file.
	shell_flags |= SHELL_READ|SHELL_WRITE;
	curbuf->b_op_ಆರಂಭ.lnum = line1;
	curbuf->b_op_ಕೊನೆ.lnum = line2;
	curwin->w_cursor.lnum = line2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ ((do_ಒಳ && (itmp = vim_tempname('i', FALSE)) == ಖಾಲಿ)
		|| (do_ಹೊರಹೋಗು && (otmp = vim_tempname('o', FALSE)) == ಖಾಲಿ))
	{
	    emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
	    ಹೋಗು filterend;
	}

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		// don't call wait_return() while busy
    ಆದರೆ (itmp != ಖಾಲಿ && buf_write(curbuf, itmp, ಖಾಲಿ, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	ಸಂದೇಶ_putchar('\n');		// keep message from buf_write()
	--no_wait_return;
#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (!aborting())
#ಆದರೆ_ಅಂತ್ಯ
	    // will call wait_return()
	    (ಶೂನ್ಯ)semsg(_(e_cant_create_file_str), itmp);
	ಹೋಗು filterend;
    }
    ಆದರೆ (curbuf != old_curbuf)
	ಹೋಗು filterend;

    ಆದರೆ (!do_ಹೊರಹೋಗು)
	ಸಂದೇಶ_putchar('\n');

    // Create the shell command in allocated memory.
    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    ಆದರೆ (cmd_buf == ಖಾಲಿ)
	ಹೋಗು filterend;

    windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    ಆದರೆ (!do_ಹೊರಹೋಗು || STRCMP(p_srr, ">") == 0 || !do_ಒಳ)
	redraw_later_clear();

    ಆದರೆ (do_ಹೊರಹೋಗು)
    {
	ಆದರೆ (u_save(line2, (linenr_T)(line2 + 1)) == FAIL)
	{
	    vim_free(cmd_buf);
	    ಹೋಗು error;
	}
	redraw_curbuf_later(UPD_VALID);
    }
    read_linecount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    ಆದರೆ (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED | shell_flags))
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

    ಆದರೆ (do_ಹೊರಹೋಗು)
    {
	ಆದರೆ (otmp != ಖಾಲಿ)
	{
	    ಆದರೆ (readfile(otmp, ಖಾಲಿ, line2, (linenr_T)0, (linenr_T)MAXLNUM,
						    eap, READ_FILTER) != OK)
	    {
#ಆದರೆ defined(FEAT_EVAL)
		ಆದರೆ (!aborting())
#ಆದರೆ_ಅಂತ್ಯ
		{
		    ಸಂದೇಶ_putchar('\n');
		    semsg(_(e_cant_read_file_str), otmp);
		}
		ಹೋಗು error;
	    }
	    ಆದರೆ (curbuf != old_curbuf)
		ಹೋಗು filterend;
	}

	read_linecount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - read_linecount;

	ಆದರೆ (shell_flags & SHELL_READ)
	{
	    curbuf->b_op_ಆರಂಭ.lnum = line2 + 1;
	    curbuf->b_op_ಕೊನೆ.lnum = curwin->w_cursor.lnum;
	    appended_ಸಾಲುಗಳು_mark(line2, read_linecount);
	}

	ಆದರೆ (do_ಒಳ)
	{
	    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPMARKS)
				     || vim_strchr(p_cpo, CPO_REMMARK) == ಖಾಲಿ)
	    {
		ಆದರೆ (read_linecount >= linecount)
		    // move all marks from old lines to new lines
		    mark_adjust(line1, line2, linecount, 0L);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (save_cmod_flags & CMOD_LOCKMARKS)
		{
		    // Move marks from the lines below the new lines down by
		    // the number of lines lost.
		    // Move marks from the lines that will be deleted to the
		    // new lines and below.
		    mark_adjust(line2 + 1, (linenr_T)MAXLNUM,
					       linecount - read_linecount, 0L);
		    mark_adjust(line1, line2, linecount, 0L);
		}
		ಇಲ್ಲದಿದ್ದರೆ
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
	    del_ಸಾಲುಗಳು(linecount, TRUE);
	    ಆದರೆ (read_linecount == 0)
	    {
		// no filter output: clamp '[ and '] to a valid line
		curbuf->b_op_ಆರಂಭ.lnum = curbuf->b_op_ಕೊನೆ.lnum =
					MIN(line1, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
		curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		curbuf->b_op_ಆರಂಭ.lnum -= linecount;	// adjust '[
		curbuf->b_op_ಕೊನೆ.lnum -= linecount;	// adjust ']
	    }
	    write_lnum_adjust(-linecount);		// adjust last line
							// for next write
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    foldUpdate(curwin, curbuf->b_op_ಆರಂಭ.lnum, curbuf->b_op_ಕೊನೆ.lnum);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    linecount = curbuf->b_op_ಕೊನೆ.lnum - curbuf->b_op_ಆರಂಭ.lnum + 1;
	    curwin->w_cursor.lnum = curbuf->b_op_ಕೊನೆ.lnum;
	}

	beginline(BL_WHITE | BL_FIX);	    // cursor on first non-blank
	--no_wait_return;

	ಆದರೆ (linecount > p_report)
	{
	    ಆದರೆ (do_ಒಳ)
	    {
		vim_snprintf(ಸಂದೇಶ_buf, ಗಾತ್ರ(ಸಂದೇಶ_buf),
				    _("%ld lines filtered"), (ದೀರ್ಘ)linecount);
		ಆದರೆ (ಸಂದೇಶ(ಸಂದೇಶ_buf) && !ಸಂದೇಶ_scroll)
		    // save message to display it after redraw
		    ಗಣ_keep_ಸಂದೇಶ((char_u *)ಸಂದೇಶ_buf, 0);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		msgmore((ದೀರ್ಘ)linecount);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
error:
	// put cursor back in same position for ":w !cmd"
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

    cmdmod.cmod_flags = save_cmod_flags;
    ಆದರೆ (curbuf != old_curbuf)
    {
	--no_wait_return;
	emsg(_(e_filter_autocommands_must_not_change_current_ಬಫರ್));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	curbuf->b_op_ಆರಂಭ = orig_ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = orig_ಕೊನೆ;
    }

    ಆದರೆ (itmp != ಖಾಲಿ)
	mch_remove(itmp);
    ಆದರೆ (otmp != ಖಾಲಿ)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * Call a shell to execute a command.
 * When "cmd" is NULL start an interactive shell.
 */
    ಶೂನ್ಯ
do_shell(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		flags)	// may be SHELL_DOOUT when output is redirected
{
    buf_T	*buf;
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಪೂರ್ಣಾಂಕ		save_nwr;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಪೂರ್ಣಾಂಕ		winstart = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		keep_termcap = !termcap_ಸಕ್ರಿಯ;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    ಆದರೆ (check_restricted() || check_secure())
    {
	ಸಂದೇಶ_ಕೊನೆ();
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    /*
     * Check if ":!start" is used.  This implies not stopping termcap mode.
     */
    ಆದರೆ (cmd != ಖಾಲಿ)
	keep_termcap = winstart = (STRNICMP(cmd, "start ", 6) == 0);

# ಆದರೆ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // Don't stop termcap mode when using a terminal window for the shell.
    ಆದರೆ (gui.ಒಳ_use && vim_strchr(p_go, GO_TERMINAL) != ಖಾಲಿ)
	keep_termcap = TRUE;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    ಸಂದೇಶ_putchar('\r');			// put cursor at start of line
    ಆದರೆ (!autocmd_busy)
    {
	ಆದರೆ (!keep_termcap)
	    stoptermcap();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (!winstart)
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_putchar('\n');		// may shift screen one line up

    // warning message before calling the shell
    ಆದರೆ (p_warn && !autocmd_busy && ಸಂದೇಶ_silent == 0)
	FOR_ALL_BUFFERS(buf)
	    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು_Not_ಪದ(buf))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
		ಆದರೆ (!keep_termcap)
		    starttermcap();	// don't want a message box here
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_puts(_("[No write since last change]\n"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
		ಆದರೆ (!keep_termcap)
		    stoptermcap();
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    }

    // This windgoto is required for when the '\n' resulted in a "delete line
    // 1" command to the terminal.
    ಆದರೆ (!swapping_screen())
	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
    cursor_on();
    (ಶೂನ್ಯ)call_shell(cmd, SHELL_COOKED | flags);
    did_check_timestamps = FALSE;
    need_check_timestamps = TRUE;

    /*
     * put the message cursor at the end of the screen, avoids wait_return()
     * to overwrite the text that the external command showed
     */
    ಆದರೆ (!swapping_screen())
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    }

    ಆದರೆ (autocmd_busy)
    {
	ಆದರೆ (ಸಂದೇಶ_silent == 0)
	    redraw_later_clear();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
	ಆದರೆ (!gui.ಒಳ_use)
# endif
	{
	    ಆದರೆ (cmd == ಖಾಲಿ
# ifdef MSWIN
		    || (keep_termcap && !need_wait_return)
# endif
	       )
	    {
		ಆದರೆ (ಸಂದೇಶ_silent == 0)
		    redraw_later_clear();
		need_wait_return = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		/*
		 * If we switch screens when starttermcap() is called, we
		 * really want to wait for "hit return to continue".
		 */
		save_nwr = no_wait_return;
		ಆದರೆ (swapping_screen())
		    no_wait_return = FALSE;
		// ":silent" leaves the output of the command on the screen,
		// except where it cannot be seen there anyway.
		ಪೂರ್ಣಾಂಕ redraw = ಸಂದೇಶ_silent == 0 || swapping_screen();
# ifdef FEAT_GUI
		ಆದರೆ (gui.ಒಳ_use)
		    redraw = TRUE;
# endif
# ifdef AMIGA
		wait_return(ಪದ_console ? -1 : redraw); // see below
# ಇಲ್ಲದಿದ್ದರೆ
		wait_return(redraw);
# endif
		no_wait_return = save_nwr;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_MSWIN

	ಆದರೆ (!keep_termcap)	// if keep_termcap is TRUE didn't stop termcap
	    starttermcap();	// start termcap if not done by wait_return()

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
	ಆದರೆ (skip_redraw)		// ':' hit in wait_return()
	{
	    ಆದರೆ (ಸಂದೇಶ_silent == 0)
		redraw_later_clear();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_console)
	{
	    ಹೊರಹೋಗು_STR("\033[0 q");	// get window size
	    ಆದರೆ (got_int && ಸಂದೇಶ_silent == 0)
		redraw_later_clear();	// if got_int is TRUE, redraw needed
	    ಇಲ್ಲದಿದ್ದರೆ
		must_redraw = 0;	// no extra redraw needed
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // display any error messages now
    display_errors();

    apply_autocmds(EVENT_SHELLCMDPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
}

#ಆದರೆ !defined(UNIX)
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_pipe(char_u *cmd)
{
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    inquote = FALSE;

    ಗಾಗಿ (p = cmd; *p != NUL; ++p)
    {
	ಆದರೆ (!inquote && *p == '|')
	    ಹಿಂತಿರುಗಿ p;
	ಆದರೆ (*p == '"')
	    inquote = !inquote;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rem_backslash(p))
	    ++p;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ		is_powershell = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಪೂರ್ಣಾಂಕ		is_fish_shell;
#ಆದರೆ_ಅಂತ್ಯ

    char_u *shell_name = get_isolated_shell_name();
    ಆದರೆ (shell_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ಆದರೆ defined(UNIX)
    // Account for fish's different syntax for subshells
    is_fish_shell = fnamecmp(shell_name, "fish") == 0;
    ಆದರೆ (is_fish_shell)
	len = (long_u)STRLEN(cmd) + 13;		// "begin; " + "; end" + NUL
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	is_powershell = (shell_name[0] == 'p')
			&& (fnamecmp(shell_name, "powershell") == 0
				|| fnamecmp(shell_name, "powershell.exe") == 0
				|| fnamecmp(shell_name, "pwsh") == 0
				|| fnamecmp(shell_name, "pwsh.exe") == 0);
	ಆದರೆ (is_powershell)
	    len = (long_u)STRLEN(cmd) + 7; // "& { " + " }" + NUL
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (long_u)STRLEN(cmd) + 3; // "()" + NUL
    }

    ಆದರೆ (itmp != ಖಾಲಿ)
    {
	ಆದರೆ (is_powershell)
	    // "Get-Content " + " | & "
	    len += (long_u)STRLEN(itmp) + 17;
	ಇಲ್ಲದಿದ್ದರೆ
	    len += (long_u)STRLEN(itmp) + 9;	// " { < " + " } "
    }
    ಆದರೆ (otmp != ಖಾಲಿ)
	len += (long_u)STRLEN(otmp) + (long_u)STRLEN(p_srr) + 2; // "  "

    vim_free(shell_name);

    buf = alloc(len);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (is_powershell)
    {
	ಆದರೆ (itmp != ಖಾಲಿ)
	    vim_snprintf((ಅಕ್ಷರ *)buf, len, "& { Get-Content %s | & %s }",
								itmp, cmd);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_snprintf((ಅಕ್ಷರ *)buf, len, "& { %s }", cmd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ defined(UNIX)
	// Put braces around the command (for concatenated commands) when
	// redirecting input and/or output.
	ಆದರೆ (itmp != ಖಾಲಿ || otmp != ಖಾಲಿ)
	{
	    ಆದರೆ (is_fish_shell)
		vim_snprintf((ಅಕ್ಷರ *)buf, len, "begin; %s; end", (ಅಕ್ಷರ *)cmd);
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_snprintf((ಅಕ್ಷರ *)buf, len, "(%s)", (ಅಕ್ಷರ *)cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    STRCPY(buf, cmd);
	ಆದರೆ (itmp != ಖಾಲಿ)
	{
	    STRCAT(buf, " < ");
	    STRCAT(buf, itmp);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	// For shells that don't understand braces around commands, at least
	// allow the use of commands in a pipe.
	ಆದರೆ (*p_sxe != NUL && *p_sxq == '(')
	{
	    ಆದರೆ (itmp != ಖಾಲಿ || otmp != ಖಾಲಿ)
		vim_snprintf((ಅಕ್ಷರ *)buf, len, "(%s)", (ಅಕ್ಷರ *)cmd);
	    ಇಲ್ಲದಿದ್ದರೆ
		STRCPY(buf, cmd);
	    ಆದರೆ (itmp != ಖಾಲಿ)
	    {
		STRCAT(buf, " < ");
		STRCAT(buf, itmp);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    STRCPY(buf, cmd);
	    ಆದರೆ (itmp != ಖಾಲಿ)
	    {
		char_u	*p;

		// If there is a pipe, we have to put the '<' in front of it.
		// Don't do this when 'shellquote' is not empty, otherwise the
		// redirection would be inside the quotes.
		ಆದರೆ (*p_shq == NUL)
		{
		    p = ಕಂಡುಹಿಡಿ_pipe(buf);
		    ಆದರೆ (p != ಖಾಲಿ)
			*p = NUL;
		}
		STRCAT(buf, " <");	// " < " causes problems on Amiga
		STRCAT(buf, itmp);
		ಆದರೆ (*p_shq == NUL)
		{
		    p = ಕಂಡುಹಿಡಿ_pipe(cmd);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			// insert a space before the '|' for DOS
			STRCAT(buf, " ");
			STRCAT(buf, p);
		    }
		}
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (otmp != ಖಾಲಿ)
	append_redir(buf, (ಪೂರ್ಣಾಂಕ)len, p_srr, otmp);

    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Append output redirection for file "fname" to the end of string buffer
 * "buf[buflen]"
 * Works with the 'shellredir' and 'shellpipe' options.
 * The caller should make sure that there is enough room:
 *	STRLEN(opt) + STRLEN(fname) + 3
 */
    ಶೂನ್ಯ
append_redir(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		buflen,
    char_u	*opt,
    char_u	*fname)
{
    char_u	*p;
    char_u	*ಕೊನೆ;

    ಕೊನೆ = buf + STRLEN(buf);
    // find "%s"
    ಗಾಗಿ (p = opt; (p = vim_strchr(p, '%')) != ಖಾಲಿ; ++p)
    {
	ಆದರೆ (p[1] == 's') // found %s
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (p[1] == '%') // skip %%
	    ++p;
    }
    ಆದರೆ (p != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	*ಕೊನೆ++ = ' '; // not really needed? Not with sh, ksh or bash
#ಆದರೆ_ಅಂತ್ಯ
	vim_snprintf((ಅಕ್ಷರ *)ಕೊನೆ, (size_t)(buflen - (ಕೊನೆ - buf)),
						  (ಅಕ್ಷರ *)opt, (ಅಕ್ಷರ *)fname);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf((ಅಕ್ಷರ *)ಕೊನೆ, (size_t)(buflen - (ಕೊನೆ - buf)), " %s %s",
		(ಅಕ್ಷರ *)opt, (ಅಕ್ಷರ *)fname);
}

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    ಶೂನ್ಯ
do_fixdel(exarg_T *eap ಬಳಸದ)
{
    char_u  *p;

    p = ಕಂಡುಹಿಡಿ_termcode((char_u *)"kb");
    ಕೂಡಿಸು_termcode((char_u *)"kD", p != ಖಾಲಿ
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
}

    ಶೂನ್ಯ
print_ಸಾಲು_no_ಪೂರ್ವಪ್ರತ್ಯಯ(
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		use_ಸಂಖ್ಯೆ,
    ಪೂರ್ಣಾಂಕ		list)
{
    ಅಕ್ಷರ	numbuf[30];

    ಆದರೆ (curwin->w_p_nu || use_ಸಂಖ್ಯೆ)
    {
	vim_snprintf(numbuf, ಗಾತ್ರ(numbuf),
				   "%*ld ", ಸಂಖ್ಯೆ_ಅಗಲ(curwin), (ದೀರ್ಘ)lnum);
	ಸಂದೇಶ_puts_attr(numbuf, HL_ATTR(HLF_N));	// Highlight line nrs
    }
    ಸಂದೇಶ_prt_ಸಾಲು(ml_get(lnum), list);
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    ಶೂನ್ಯ
print_ಸಾಲು(linenr_T lnum, ಪೂರ್ಣಾಂಕ use_ಸಂಖ್ಯೆ, ಪೂರ್ಣಾಂಕ list)
{
    ಪೂರ್ಣಾಂಕ		save_silent = silent_ಬಹುಲಕ;

    // apply :filter /pat/
    ಆದರೆ (ಸಂದೇಶ_filtered(ml_get(lnum)))
	ಹಿಂತಿರುಗಿ;

    ಸಂದೇಶ_ಆರಂಭ();
    silent_ಬಹುಲಕ = FALSE;
    info_ಸಂದೇಶ = TRUE;	// use mch_msg(), not mch_errmsg()
    print_ಸಾಲು_no_ಪೂರ್ವಪ್ರತ್ಯಯ(lnum, use_ಸಂಖ್ಯೆ, list);
    ಆದರೆ (save_silent)
    {
	ಸಂದೇಶ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	ಹೊರಹೋಗು_flush();
	silent_ಬಹುಲಕ = save_silent;
    }
    info_ಸಂದೇಶ = FALSE;
}

    ಪೂರ್ಣಾಂಕ
rename_ಬಫರ್(char_u *new_fname)
{
    char_u	*fname, *sfname, *xfname;
    buf_T	*buf;

    buf = curbuf;
    apply_autocmds(EVENT_BUFFILEPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    // buffer changed, don't change name now
    ಆದರೆ (buf != curbuf)
	ಹಿಂತಿರುಗಿ FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (aborting())	    // autocmds may abort script processing
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
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
    curbuf->b_ffname = ಖಾಲಿ;
    curbuf->b_sfname = ಖಾಲಿ;
    ಆದರೆ (setfname(curbuf, new_fname, ಖಾಲಿ, TRUE) == FAIL)
    {
	curbuf->b_ffname = fname;
	curbuf->b_sfname = sfname;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    curbuf->b_flags |= BF_NOTEDITED;
    ಆದರೆ (xfname != ಖಾಲಿ && *xfname != NUL)
    {
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, 0);
	ಆದರೆ (buf != ಖಾಲಿ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	    curwin->w_alt_fnum = buf->b_fnum;
    }
    vim_free(fname);
    vim_free(sfname);
    apply_autocmds(EVENT_BUFFILEPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * ":file[!] [fname]".
 */
    ಶೂನ್ಯ
ex_file(exarg_T *eap)
{
    // ":0file" removes the file name.  Check for illegal uses ":3file",
    // "0file name", etc.
    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0
	    && (*eap->arg != NUL
		|| eap->line2 > 0
		|| eap->addr_ಎಣಿಕೆ > 1))
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*eap->arg != NUL || eap->addr_ಎಣಿಕೆ == 1)
    {
	ಆದರೆ (rename_ಬಫರ್(eap->arg) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
	redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // print file name if no argument or 'F' is not in 'shortmess'
    ಆದರೆ (*eap->arg == NUL || !shortmess(SHM_FILEINFO))
	fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    ಶೂನ್ಯ
ex_update(exarg_T *eap)
{
    ಆದರೆ (curbuf_Is_ಬದಲಾಯಿತು())
	(ಶೂನ್ಯ)do_write(eap);
}

/*
 * ":write" and ":saveas".
 */
    ಶೂನ್ಯ
ex_write(exarg_T *eap)
{
    ಆದರೆ (eap->cmdidx == CMD_saveas)
    {
	// :saveas does not take a range, uses all lines.
	eap->line1 = 1;
	eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }

    ಆದರೆ (eap->usefilter)		// input lines to shell command
	do_bang(1, eap, FALSE, TRUE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)do_write(eap);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_writable(char_u *fname)
{
    ಆದರೆ (mch_nodetype(fname) == ಗಂಟು_OTHER)
    {
	semsg(_(e_str_is_not_file_or_writable_device), fname);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Write the current buffer to file "eap->arg".
 * If "eap->append" is TRUE, append to the file.
 *
 * If "*eap->arg == NUL" write to current file.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
do_write(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		other;
    char_u	*fname = ಖಾಲಿ;		// init to shut up gcc
    char_u	*ffname;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    char_u	*free_fname = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    char_u	*browse_file = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    buf_T	*alt_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		name_was_missing;

    ಆದರೆ (not_writing())		// check 'write' option
	ಹಿಂತಿರುಗಿ FAIL;

    ffname = eap->arg;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
    {
	browse_file = do_browse(BROWSE_SAVE, (char_u *)_("Save As"), ffname,
						    ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, curbuf);
	ಆದರೆ (browse_file == ಖಾಲಿ)
	    ಹೋಗು theend;
	ffname = browse_file;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (*ffname == NUL)
    {
	ಆದರೆ (eap->cmdidx == CMD_saveas)
	{
	    emsg(_(e_ವಾದ_required));
	    ಹೋಗು theend;
	}
	other = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	ಆದರೆ (free_fname != ಖಾಲಿ)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    ಆದರೆ (other)
    {
	ಆದರೆ (vim_strchr(p_cpo, CPO_ALTWRITE) != ಖಾಲಿ
						 || eap->cmdidx == CMD_saveas)
	    alt_buf = setaltfname(ffname, fname, (linenr_T)1);
	ಇಲ್ಲದಿದ್ದರೆ
	    alt_buf = buflist_findname(ffname);
	ಆದರೆ (alt_buf != ಖಾಲಿ && alt_buf->b_ml.ml_mfp != ಖಾಲಿ)
	{
	    // Overwriting a file that is loaded in another buffer is not a
	    // good idea.
	    emsg(_(e_file_is_loaded_ಒಳ_another_ಬಫರ್));
	    ಹೋಗು theend;
	}
    }

    /*
     * Writing to the current file is not allowed in readonly mode
     * and a file name is required.
     * "nofile" and "nowrite" buffers cannot be written implicitly either.
     */
    ಆದರೆ (!other && (bt_dontwrite_ಸಂದೇಶ(curbuf) || check_fname() == FAIL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		|| check_writable(curbuf->b_ffname) == FAIL
#ಆದರೆ_ಅಂತ್ಯ
		|| check_readonly(&eap->forceit, curbuf)))
	ಹೋಗು theend;

    ಆದರೆ (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	ಆದರೆ (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಆದರೆ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		ಆದರೆ (vim_dialog_yesno(VIM_QUESTION, ಖಾಲಿ,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    ಹೋಗು theend;
		eap->forceit = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		emsg(_(e_use_bang_ಗಮ್ಯ_write_partial_ಬಫರ್));
		ಹೋಗು theend;
	    }
	}
    }

    ಆದರೆ (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
    {
	ಆದರೆ (eap->cmdidx == CMD_saveas && alt_buf != ಖಾಲಿ)
	{
	    buf_T	*was_curbuf = curbuf;

	    apply_autocmds(EVENT_BUFFILEPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, alt_buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (curbuf != was_curbuf || aborting())
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (curbuf != was_curbuf)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		// buffer changed, don't change name now
		retval = FAIL;
		ಹೋಗು theend;
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
	    buf_name_ಬದಲಾಯಿತು(curbuf);

	    apply_autocmds(EVENT_BUFFILEPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    apply_autocmds(EVENT_BUFFILEPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, alt_buf);
	    ಆದರೆ (!alt_buf->b_p_bl)
	    {
		alt_buf->b_p_bl = TRUE;
		apply_autocmds(EVENT_BUFADD, ಖಾಲಿ, ಖಾಲಿ, FALSE, alt_buf);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (curbuf != was_curbuf || aborting())
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (curbuf != was_curbuf)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		// buffer changed, don't write the file
		retval = FAIL;
		ಹೋಗು theend;
	    }

	    // If 'filetype' was empty try detecting it now.
	    ಆದರೆ (*curbuf->b_p_ft == NUL)
	    {
		ಆದರೆ (au_has_ಗುಂಪು((char_u *)"filetypedetect"))
		    (ಶೂನ್ಯ)do_doautocmd((char_u *)"filetypedetect BufRead",
								  TRUE, ಖಾಲಿ);
		do_modelines(0);
	    }

	    // Autocommands may have changed buffer names, esp. when
	    // 'autochdir' is set.
	    fname = curbuf->b_sfname;
	}

	name_was_missing = curbuf->b_ffname == ಖಾಲಿ;

	retval = buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				 eap, eap->append, eap->forceit, TRUE, FALSE);

	// After ":saveas fname" reset 'readonly'.
	ಆದರೆ (eap->cmdidx == CMD_saveas)
	{
	    ಆದರೆ (retval == OK)
	    {
		curbuf->b_p_ro = FALSE;
		redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}

	// Change directories when the 'acd' option is set and the file name
	// got changed or set.
	ಆದರೆ (eap->cmdidx == CMD_saveas || name_was_missing)
	    DO_AUTOCHDIR;
    }

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    vim_free(browse_file);
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(free_fname);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
    ಪೂರ್ಣಾಂಕ
check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    ಪೂರ್ಣಾಂಕ		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     * If "other" is FALSE and bt_nofilename(buf) is TRUE, this must be
     * writing an "acwrite" buffer to the same file as its b_ffname, and
     * buf_write() will only allow writing with BufWriteCmd autocommands,
     * so there is no need for an overwrite check.
     */
    ಆದರೆ (       (other
		|| (!bt_nofilename(buf)
		    && ((buf->b_flags & BF_NOTEDITED)
			|| ((buf->b_flags & BF_NEW)
			    && vim_strchr(p_cpo, CPO_OVERNEW) == ಖಾಲಿ)
			|| (buf->b_flags & BF_READERR))))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	ಆದರೆ (!eap->forceit && !eap->append)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    // with UNIX it is possible to open a directory
	    ಆದರೆ (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಆದರೆ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_ಸಂದೇಶ_ಗಾತ್ರ];

		dialog_ಸಂದೇಶ(buff, _("Overwrite existing file \"%s\"?"), fname);
		ಆದರೆ (vim_dialog_yesno(VIM_QUESTION, ಖಾಲಿ, buff, 2) != VIM_YES)
		    ಹಿಂತಿರುಗಿ FAIL;
		eap->forceit = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		emsg(_(e_file_exists));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	ಆದರೆ (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    ಪೂರ್ಣಾಂಕ		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    ಆದರೆ (*p_dir == NUL)
	    {
		dir = alloc(5);
		ಆದರೆ (dir == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		STRCPY(dir, ".");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		dir = alloc(MAXPATHL);
		ಆದರೆ (dir == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    ಆದರೆ (r)
	    {
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		ಆದರೆ (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_ಸಂದೇಶ_ಗಾತ್ರ];

		    dialog_ಸಂದೇಶ(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    ಆದರೆ (vim_dialog_yesno(VIM_QUESTION, ಖಾಲಿ, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    eap->forceit = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    semsg(_(e_ಅದಲುಬದಲು_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    ಶೂನ್ಯ
ex_wnext(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (ಪೂರ್ಣಾಂಕ)eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
	i = curwin->w_arg_idx - (ಪೂರ್ಣಾಂಕ)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    ಶೂನ್ಯ
do_wqall(exarg_T *eap)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		error = 0;
    ಪೂರ್ಣಾಂಕ		save_forceit = eap->forceit;
    ಪೂರ್ಣಾಂಕ		save_exiting = exiting;

    ಆದರೆ (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
    {
	ಆದರೆ (before_quit_all(eap) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	exiting = TRUE;
    }

    FOR_ALL_BUFFERS(buf)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಆದರೆ (exiting && !eap->forceit && ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	{
	    bufref_T	bufref;

	    ಗಣ_bufref(&bufref, buf);
	    ಆದರೆ (ಪದ_try_stop_ಕೆಲಸ(buf) == FAIL)
	    {
		no_write_ಸಂದೇಶ_buf(buf);
		++error;
	    }
	    // Stopping the job may have freed the terminal buffer.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf) && !bt_dontwrite(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    ಆದರೆ (not_writing())
	    {
		++error;
		ನಿಲ್ಲಿಸು;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
	    // ":browse wall": ask for file name if there isn't one
	    ಆದರೆ (buf->b_ffname == ಖಾಲಿ && (cmdmod.cmod_flags & CMOD_BROWSE))
		browse_save_fname(buf);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (buf->b_ffname == ಖಾಲಿ)
	    {
		semsg(_(e_no_file_name_for_ಬಫರ್_nr), (ದೀರ್ಘ)buf->b_fnum);
		++error;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf);
		ಆದರೆ (buf_write_all(buf, eap->forceit) == FAIL)
		    ++error;
		// an autocommand may have deleted the buffer
		ಆದರೆ (!bufref_valid(&bufref))
		    buf = firstbuf;
	    }
	    eap->forceit = save_forceit;    // check_overwrite() may set it
	}
    }
    ಆದರೆ (exiting)
    {
	ಆದರೆ (!error)
	    getout(0);		// exit Vim
	not_exiting(save_exiting);
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
not_writing(ಶೂನ್ಯ)
{
    ಆದರೆ (p_write)
	ಹಿಂತಿರುಗಿ FALSE;
    emsg(_(e_file_not_written_writing_is_disabled_by_write_option));
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check if a buffer is read-only (either 'readonly' option is set or file is
 * read-only). Ask for overruling in a dialog. Return TRUE and give an error
 * message when the buffer is readonly.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_readonly(ಪೂರ್ಣಾಂಕ *forceit, buf_T *buf)
{
    stat_T	st;

    // Handle a file being readonly when the 'readonly' option is set or when
    // the file exists and permissions are read-only.
    // We will send 0777 to check_file_readonly(), as the "perm" variable is
    // important for device checks but not here.
    ಆದರೆ (!*forceit && (buf->b_p_ro
		|| (mch_stat((ಅಕ್ಷರ *)buf->b_ffname, &st) >= 0
		    && check_file_readonly(buf->b_ffname, 0777))))
    {
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಆದರೆ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						       && buf->b_fname != ಖಾಲಿ)
	{
	    char_u	buff[DIALOG_ಸಂದೇಶ_ಗಾತ್ರ];

	    ಆದರೆ (buf->b_p_ro)
		dialog_ಸಂದೇಶ(buff, _("'readonly' option is set for \"%s\".\nDo you wish to write anyway?"),
		    buf->b_fname);
	    ಇಲ್ಲದಿದ್ದರೆ
		dialog_ಸಂದೇಶ(buff, _("File permissions of \"%s\" are read-only.\nIt may still be possible to write it.\nDo you wish to try?"),
		    buf->b_fname);

	    ಆದರೆ (vim_dialog_yesno(VIM_QUESTION, ಖಾಲಿ, buff, 2) == VIM_YES)
	    {
		// Set forceit, to force the writing of a readonly file
		*forceit = TRUE;
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (buf->b_p_ro)
	    emsg(_(e_readonly_option_is_ಗಣ_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_str_is_read_only_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override), buf->b_fname);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ
getfile(
    ಪೂರ್ಣಾಂಕ		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ಪೂರ್ಣಾಂಕ		setpm,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    ಪೂರ್ಣಾಂಕ		other;
    ಪೂರ್ಣಾಂಕ		retval;
    char_u	*free_me = ಖಾಲಿ;

    ಆದರೆ (!check_can_ಗಣ_curbuf_forceit(forceit))
	ಹಿಂತಿರುಗಿ GETFILE_ERROR;

    ಆದರೆ (ಪಠ್ಯ_locked())
	ಹಿಂತಿರುಗಿ GETFILE_ERROR;
    ಆದರೆ (curbuf_locked())
	ಹಿಂತಿರುಗಿ GETFILE_ERROR;

    ಆದರೆ (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    ಇಲ್ಲದಿದ್ದರೆ
	other = (fnum != curbuf->b_fnum);

    ಆದರೆ (other)
	++no_wait_return;	    // don't wait for autowrite message
    ಆದರೆ (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbuf_Is_ಬದಲಾಯಿತು() && autowrite(curbuf, forceit) == FAIL)
    {
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಆದರೆ (p_confirm && p_write)
	    dialog_ಬದಲಾಯಿತು(curbuf, FALSE);
	ಆದರೆ (curbuf_Is_ಬದಲಾಯಿತು())
#ಆದರೆ_ಅಂತ್ಯ
	{
	    --no_wait_return;
	    no_write_ಸಂದೇಶ();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    ಹೋಗು theend;
	}
    }
    ಆದರೆ (other)
	--no_wait_return;
    ಆದರೆ (setpm)
	setpcmark();
    ಆದರೆ (!other)
    {
	ಆದರೆ (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_ecmd(fnum, ffname, sfname, ಖಾಲಿ, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    ಇಲ್ಲದಿದ್ದರೆ
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    ಹಿಂತಿರುಗಿ retval;
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
    ಪೂರ್ಣಾಂಕ
do_ecmd(
    ಪೂರ್ಣಾಂಕ		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    ಪೂರ್ಣಾಂಕ		flags,
    win_T	*oldwin)
{
    ಪೂರ್ಣಾಂಕ		other_file;		// TRUE if editing another file
    ಪೂರ್ಣಾಂಕ		oldbuf;			// TRUE if using existing buffer
    ಪೂರ್ಣಾಂಕ		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = ಖಾಲಿ;
#ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ		did_ಗಣ_swapcommand = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    char_u	dot_ಮಾರ್ಗ[] = ".";
    char_u	*browse_file = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    ದೀರ್ಘ	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    ಪೂರ್ಣಾಂಕ		newcol = -1;
    ಪೂರ್ಣಾಂಕ		solcol = -1;
    pos_T	*pos;
    char_u	*command = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಪೂರ್ಣಾಂಕ		did_get_winopts = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		readfile_flags = 0;
    ಪೂರ್ಣಾಂಕ		did_inc_redrawing_disabled = FALSE;
    ದೀರ್ಘ	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (eap != ಖಾಲಿ)
	command = eap->do_ecmd_cmd;
    ಗಣ_bufref(&old_curbuf, curbuf);

    ಆದರೆ (fnum != 0)
    {
	ಆದರೆ (fnum == curbuf->b_fnum)	// file is already being edited
	    ಹಿಂತಿರುಗಿ OK;			// nothing to do
	other_file = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
	ಆದರೆ ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    ಆದರೆ (
# ifdef FEAT_GUI
		!gui.ಒಳ_use &&
# endif
		    au_has_ಗುಂಪು((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		ಆದರೆ (ffname == ಖಾಲಿ || !mch_isdir(ffname))
		    ffname = dot_ಮಾರ್ಗ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, curbuf);
		ಆದರೆ (browse_file == ಖಾಲಿ)
		    ಹೋಗು theend;
		ffname = browse_file;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	// if no short name given, use ffname for short name
	ಆದರೆ (sfname == ಖಾಲಿ)
	    sfname = ffname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FNAME_CASE
	ಆದರೆ (sfname != ಖಾಲಿ)
	    fname_case(sfname, 0);   // set correct case for sfname
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == ಖಾಲಿ || *ffname == NUL))
	    ಹೋಗು theend;

	ಆದರೆ (ffname == ಖಾಲಿ)
	    other_file = TRUE;
					    // there is no file name
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ffname == NUL && curbuf->b_ffname == ಖಾಲಿ)
	    other_file = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    ಆದರೆ (free_fname != ಖಾಲಿ)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    ಆದರೆ (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_ಬದಲಾಯಿತು(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == ಖಾಲಿ ? 0 : CCGD_EXCMD)))
    {
	ಆದರೆ (fnum == 0 && other_file && ffname != ಖಾಲಿ)
	    (ಶೂನ್ಯ)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	ಹೋಗು theend;
    }


    // End Visual mode before switching to another buffer, so the text can be
    // copied into the GUI selection buffer.
    // Careful: may trigger ModeChanged() autocommand

    // Should we block autocommands here?
    reset_VIsual();

    // autocommands freed window :(
    ಆದರೆ (oldwin != ಖಾಲಿ && !win_valid(oldwin))
	oldwin = ಖಾಲಿ;

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ ((command != ಖಾಲಿ || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	string_T    val;
	size_t	    valsize;

	// Set v:swapcommand for the SwapExists autocommands.
	ಆದರೆ (command != ಖಾಲಿ)
	    valsize = (ಪೂರ್ಣಾಂಕ)STRLEN(command) + 3;
	ಇಲ್ಲದಿದ್ದರೆ
	    valsize = 30;
	val.string = alloc(valsize);
	ಆದರೆ (val.string != ಖಾಲಿ)
	{
	    ಆದರೆ (command != ಖಾಲಿ)
		val.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)val.string,
		    valsize, ":%s\r", command);
	    ಇಲ್ಲದಿದ್ದರೆ
		val.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)val.string,
		    valsize, "%ldG", (ದೀರ್ಘ)newlnum);
	    ಗಣ_vim_var_string(VV_SWAPCOMMAND, val.string, (ಪೂರ್ಣಾಂಕ)val.ಉದ್ದಳತೆ);
	    did_ಗಣ_swapcommand = TRUE;
	    vim_free(val.string);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    ಆದರೆ (other_file)
    {
	ಪೂರ್ಣಾಂಕ prev_alt_fnum = curwin->w_alt_fnum;

	ಆದರೆ (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    ಆದರೆ (oldwin != ಖಾಲಿ)
		buflist_altfpos(oldwin);
	}

	ಆದರೆ (fnum)
	    buf = buflist_findnr(fnum);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		ಆದರೆ (command != ಖಾಲಿ)
		{
		    tlnum = atol((ಅಕ್ಷರ *)command);
		    ಆದರೆ (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		ಆದರೆ (newbuf != ಖಾಲಿ)
		{
		    ಆದರೆ (flags & ECMD_ALTBUF)
			curwin->w_alt_fnum = newbuf->b_fnum;
		    ಆದರೆ (tlnum > 0)
			newbuf->b_ಕೊನೆಯ_cursor.lnum = tlnum;
		}
		ಹೋಗು theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_ಗಣ_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    ಆದರೆ (oldwin != ಖಾಲಿ)
		oldwin = curwin;
	    ಗಣ_bufref(&old_curbuf, curbuf);
	}
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹೋಗು theend;
	// autocommands try to edit a closing buffer, which like splitting, can
	// result in more windows displaying it; abort
	ಆದರೆ (buf->b_locked_ವಿಭಜಿಸು)
	{
	    emsg(_(e_cannot_switch_ಗಮ್ಯ_a_closing_ಬಫರ್));
	    ಹೋಗು theend;
	}
	ಆದರೆ (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ					// existing memfile
	{
	    oldbuf = TRUE;
	    ಗಣ_bufref(&bufref, buf);
	    (ಶೂನ್ಯ)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    ಆದರೆ (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		ಹೋಗು theend;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (aborting())	    // autocmds may abort script processing
		ಹೋಗು theend;
#ಆದರೆ_ಅಂತ್ಯ
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	ಆದರೆ ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_ಕೊನೆಯ)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->ಲಂಬಸಾಲು;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	ಆದರೆ (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
	    ಪೂರ್ಣಾಂಕ		save_cmdwin_type = cmdwin_type;
	    win_T	*save_cmdwin_win = cmdwin_win;

	    // Should only be possible to get here if the cmdwin is closed, or
	    // if it's opening and its buffer hasn't been set yet (the new
	    // buffer is for it).
	    ಪ್ರತಿಜ್ಞಾ(cmdwin_buf == ಖಾಲಿ);

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
	    cmdwin_win = ಖಾಲಿ;

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
	    ಆದರೆ (buf->b_fname != ಖಾಲಿ)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    ಗಣ_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

	    cmdwin_type = save_cmdwin_type;
	    cmdwin_win = save_cmdwin_win;

	    ಆದರೆ (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_ಸಂದೇಶ(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		ಹೋಗು theend;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		ಹೋಗು theend;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		win_T	    *the_curwin = curwin;

		// Set the w_locked flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		++the_curwin->w_locked;
		++buf->b_locked;

		ಆದರೆ (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This may set
		// curwin->w_buffer to NULL.
		u_sync(FALSE);
		close_ಬಫರ್(curwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE,
			 oldwin != ಖಾಲಿ);

		// Autocommands may have closed the window.
		ಆದರೆ (win_valid(the_curwin))
		    --the_curwin->w_locked;
		--buf->b_locked;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		// autocmds may abort script processing
		ಆದರೆ (aborting() && curwin->w_ಬಫರ್ != ಖಾಲಿ)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    ಹೋಗು theend;
		}
#ಆದರೆ_ಅಂತ್ಯ
		// Be careful again, like above.
		ಆದರೆ (!bufref_valid(&au_new_curbuf))
		{
		    // New buffer was deleted.  If curwin->w_buffer is NULL, we
		    // must enter some buffer.  Hopefully the last one is OK.
		    ಆದರೆ (curwin->w_ಬಫರ್ == ಖಾಲಿ)
			buf = lastbuf;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			delbuf_ಸಂದೇಶ(new_name);	// frees new_name
			au_new_curbuf = save_au_new_curbuf;
			ಹೋಗು theend;
		    }
		}
		ಆದರೆ (buf == curbuf)		// already in new buffer
		    auto_buf = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    ಆದರೆ (curwin->w_ಬಫರ್ == ಖಾಲಿ
			    || curwin->w_s == &(curwin->w_ಬಫರ್->b_s))
			curwin->w_s = &(buf->b_s);
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (curwin->w_ಬಫರ್ != ಖಾಲಿ)
			--curwin->w_ಬಫರ್->b_nwindows;

		    curwin->w_ಬಫರ್ = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    ಆದರೆ (!oldbuf && eap != ಖಾಲಿ)
		    {
			ಗಣ_file_options(TRUE, eap);
			ಗಣ_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
		did_get_winopts = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.ಲಂಬಸಾಲು = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ // !other_file
    {
	ಆದರೆ ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    ಹೋಗು theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    ಆದರೆ ((flags & ECMD_ಗಣ_HELP) || keep_help_flag)
    {
	prepare_help_ಬಫರ್();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	ಆದರೆ (!curbuf->b_help)
	    ಗಣ_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    ಆದರೆ (buf != curbuf)
	ಹೋಗು theend;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (aborting())	    // autocmds may abort script processing
	ಹೋಗು theend;
#ಆದರೆ_ಅಂತ್ಯ

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
    ಆದರೆ (!other_file && !oldbuf)		// re-use the buffer
    {
	ಗಣ_ಕೊನೆಯ_cursor(curwin);	// may set b_last_cursor
	ಆದರೆ (newlnum == ECMD_ಕೊನೆಯ || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.ಲಂಬಸಾಲು;
	}
	buf = curbuf;
	ಆದರೆ (buf->b_fname != ಖಾಲಿ)
	    new_name = vim_strsave(buf->b_fname);
	ಇಲ್ಲದಿದ್ದರೆ
	    new_name = ಖಾಲಿ;
	ಗಣ_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	ಆದರೆ (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    ಆದರೆ (u_savecommon(0, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		ಹೋಗು theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	ಆದರೆ (!bufref_valid(&bufref))
	{
	    delbuf_ಸಂದೇಶ(new_name);	// frees new_name
	    ಹೋಗು theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	ಆದರೆ (buf != curbuf)
	    ಹೋಗು theend;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (aborting())	    // autocmds may abort script processing
	    ಹೋಗು theend;
#ಆದರೆ_ಅಂತ್ಯ
	buf_clear_file(curbuf);
	curbuf->b_op_ಆರಂಭ.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_ಕೊನೆ.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    // If the file name was changed, reset the not-edit flag so that ":write"
    // works.
    ಆದರೆ (!other_file)
	curbuf->b_flags &= ~BF_NOTEDITED;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    ಆದರೆ (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		ಆದರೆ (win->w_ಬಫರ್ == curbuf)
		    foldUpdateAll(win);
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	ಆದರೆ (!oldbuf)			    // need to read the file
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    ಆದರೆ (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#ಆದರೆ_ಅಂತ್ಯ
	    ಅದಲುಬದಲು_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    ಆದರೆ (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#ಆದರೆ defined(FEAT_EVAL)
	    ಆದರೆ (should_abort(open_ಬಫರ್(FALSE, eap, readfile_flags)))
		retval = FAIL;
#ಇಲ್ಲದಿದ್ದರೆ
	    (ಶೂನ್ಯ)open_ಬಫರ್(FALSE, eap, readfile_flags);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_ಅದಲುಬದಲು_exists(&old_curbuf);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE,
							      curbuf, &retval);
	    ಆದರೆ ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	ಆದರೆ (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *ಪಠ್ಯ = ml_get_curline();

	    ಆದರೆ (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.ಲಂಬಸಾಲು != (ಪೂರ್ಣಾಂಕ)(skipwhite(ಪಠ್ಯ) - ಪಠ್ಯ))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.ಲಂಬಸಾಲು;
	    }
	}
	ಆದರೆ (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();

	maketitle();
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	ಆದರೆ (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_ಗಣ_title(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    ಆದರೆ (curwin->w_p_diff)
    {
	diff_buf_ಕೂಡಿಸು(curbuf);
	diff_invalidate(curbuf);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    ಆದರೆ (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ಶೂನ್ಯ)parse_spelllang(curwin);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (command == ಖಾಲಿ)
    {
	ಆದರೆ (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.ಲಂಬಸಾಲು = newcol;
	    check_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    ಆದರೆ (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.ಲಂಬಸಾಲು = solcol;
		check_cursor_ಲಂಬಸಾಲು();
		curwin->w_cursor.coladd = 0;
		curwin->w_ಗಣ_curswant = true;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		beginline(BL_SOL | BL_FIX);
	}
	ಇಲ್ಲದಿದ್ದರೆ			// no line number, go to last line in Ex mode
	{
	    ಆದರೆ (exmode_ಸಕ್ರಿಯ)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    ಆದರೆ (oldbuf && !auto_buf)
    {
	ಪೂರ್ಣಾಂಕ	ಸಂದೇಶ_scroll_save = ಸಂದೇಶ_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	ಆದರೆ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    ಸಂದೇಶ_scroll = FALSE;
	ಆದರೆ (!ಸಂದೇಶ_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll_save;
	ಸಂದೇಶ_scrolled_ign = TRUE;

	ಆದರೆ (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	ಸಂದೇಶ_scrolled_ign = FALSE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    curbuf->b_ಕೊನೆಯ_ಬಳಸಿದ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (command != ಖಾಲಿ)
	do_cmdline(command, ಖಾಲಿ, ಖಾಲಿ, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    ಆದರೆ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ಶೂನ್ಯ)keymap_init();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    ಆದರೆ (!skip_redraw)
    {
	n = *so_ptr;
	ಆದರೆ (topline == 0 && command == ಖಾಲಿ)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(UPD_NOT_VALID);	// redraw this buffer later
    }

    ಆದರೆ (p_im && (State & ಬಹುಲಕ_ಸೇರಿಸು) == 0)
	need_ಆರಂಭ_insertmode = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    ಆದರೆ (p_acd && curbuf->b_ffname != ಖಾಲಿ)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	ಆದರೆ (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_NETBEANS_INTG)
    ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ)
    {
# ifdef FEAT_NETBEANS_INTG
	ಆದರೆ ((flags & ECMD_ಗಣ_HELP) != ECMD_ಗಣ_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

theend:
    ಆದರೆ (did_inc_redrawing_disabled && RedrawingDisabled > 0)
	--RedrawingDisabled;
#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (did_ಗಣ_swapcommand)
	ಗಣ_vim_var_string(VV_SWAPCOMMAND, ಖಾಲಿ, -1);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    vim_free(browse_file);
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(free_fname);
    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
delbuf_ಸಂದೇಶ(char_u *name)
{
    semsg(_(e_autocommands_unexpectedly_deleted_new_ಬಫರ್_str),
					   name == ಖಾಲಿ ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf.br_buf = ಖಾಲಿ;
    au_new_curbuf.br_buf_free_ಎಣಿಕೆ = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ append_indent = 0;	    // autoindent for first line

/*
 * Get the next line of text for ":append", ":insert" and ":change": the text
 * after the bar, the next line of the command, or a line from the script or
 * the user.  Returns NULL when there is no more.
 */
    ಸ್ಥಿರ char_u *
get_append_ಸಾಲು(exarg_T *eap, ಪೂರ್ಣಾಂಕ indent)
{
    char_u	*theline;
    char_u	*p;

    ಆದರೆ (*eap->arg == '|')
    {
	// Get the text after the trailing bar.
	theline = vim_strsave(eap->arg + 1);
	*eap->arg = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->ea_getline == ಖಾಲಿ)
    {
	// No getline() function, use the lines that follow. This ends
	// when there is no more.
	ಆದರೆ (eap->nextcmd == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	p = vim_strchr(eap->nextcmd, NL);
	ಆದರೆ (p == ಖಾಲಿ)
	    p = eap->nextcmd + STRLEN(eap->nextcmd);
	theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	ಆದರೆ (*p != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ
	    p = ಖಾಲಿ;
	eap->nextcmd = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ save_State = State;

	// Set State to avoid the cursor shape to be set to MODE_INSERT
	// state when getline() returns.
	State = ಬಹುಲಕ_CMDLINE;
	theline = eap->ea_getline(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		eap->cstack->cs_looplevel > 0 ? -1 :
#ಆದರೆ_ಅಂತ್ಯ
		NUL, eap->cookie, indent, TRUE);
	State = save_State;
    }
    ಹಿಂತಿರುಗಿ theline;
}

/*
 * ":insert" and ":append", also used by ":change"
 */
    ಶೂನ್ಯ
ex_append(exarg_T *eap)
{
    char_u	*theline;
    ಪೂರ್ಣಾಂಕ		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    ಪೂರ್ಣಾಂಕ		indent = 0;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		vcol;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2 = (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (not_ಒಳ_vim9(eap, eap->skip) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (eap->skip)
    {
	// Not executing the command, only read the lines up to the ".".
	ಆಗ ((theline = get_append_ಸಾಲು(eap, 0)) != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ ಕೊನೆ = theline[0] == '.' && theline[1] == NUL;

	    vim_free(theline);
	    ಆದರೆ (ಕೊನೆ)
		ನಿಲ್ಲಿಸು;
	}
	ಹಿಂತಿರುಗಿ;
    }

    // the ! flag toggles autoindent
    ಆದರೆ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    ಆದರೆ (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    ಆದರೆ (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    ಆದರೆ (ಖಾಲಿಗಳು_2 && lnum == 1)
	lnum = 0;

    State = ಬಹುಲಕ_ಸೇರಿಸು;		    // behave like in Insert mode
    ಆದರೆ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= ಬಹುಲಕ_LANGMAP;

    ಗಾಗಿ (;;)
    {
	ಸಂದೇಶ_scroll = TRUE;
	need_wait_return = FALSE;
	ಆದರೆ (curbuf->b_p_ai)
	{
	    ಆದರೆ (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	theline = get_append_ಸಾಲು(eap, indent);
	ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಆದರೆ (theline == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	ಆದರೆ (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	ಗಾಗಿ (p = theline; indent > vcol; ++p)
	{
	    ಆದರೆ (*p == ' ')
		++vcol;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == TAB)
		vcol += 8 - vcol % 8;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (ಖಾಲಿಗಳು_2 ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    ನಿಲ್ಲಿಸು;
	}

	// don't use autoindent if nothing was typed.
	ಆದರೆ (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	ಆದರೆ (ಖಾಲಿಗಳು_2)
	    // there are no marks below the inserted lines
	    appended_ಸಾಲುಗಳು(lnum, 1L);
	ಇಲ್ಲದಿದ್ದರೆ
	    appended_ಸಾಲುಗಳು_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	ಆದರೆ (ಖಾಲಿಗಳು_2)
	{
	    ml_delete(2L);
	    ಖಾಲಿಗಳು_2 = FALSE;
	}
    }
    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;

    ಆದರೆ (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_ಆರಂಭ.lnum = (eap->line2 < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಆದರೆ (eap->cmdidx != CMD_append)
	    --curbuf->b_op_ಆರಂಭ.lnum;
	curbuf->b_op_ಕೊನೆ.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_ಆರಂಭ.lnum;
	curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
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
    ಶೂನ್ಯ
ex_change(exarg_T *eap)
{
    linenr_T	lnum;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (not_ಒಳ_vim9(eap, eap->skip) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (eap->skip)
    {
	ex_append(eap);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // the ! flag toggles autoindent
    ಆದರೆ (eap->forceit ? !curbuf->b_p_ai : curbuf->b_p_ai)
	append_indent = get_indent_lnum(eap->line1);

    ಗಾಗಿ (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)	    // nothing to delete
	    ನಿಲ್ಲಿಸು;
	ml_delete(eap->line1);
    }

    // make sure the cursor is not beyond the end of the file now
    check_cursor_lnum();
    deleted_ಸಾಲುಗಳು_mark(eap->line1, (ದೀರ್ಘ)(eap->line2 - lnum));

    // ":append" on the line above the deleted lines.
    eap->line2 = eap->line1;
    ex_append(eap);
}

    ಶೂನ್ಯ
ex_z(exarg_T *eap)
{
    char_u	*x;
    ದೀರ್ಘ	bigness;
    char_u	*ಬಗೆ;
    ಪೂರ್ಣಾಂಕ		minus = 0;
    linenr_T	ಆರಂಭ, ಕೊನೆ, curs, i;
    ಪೂರ್ಣಾಂಕ		j;
    linenr_T	lnum = eap->line2;

    // Vi compatible: ":z!" uses display height, without a count uses
    // 'scroll'
    ಆದರೆ (eap->forceit)
	bigness = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ONE_ಕಿಟಕಿ)
	bigness = curwin->w_ಎತ್ತರ - 3;
    ಇಲ್ಲದಿದ್ದರೆ
	bigness = curwin->w_p_scr * 2;
    ಆದರೆ (bigness < 1)
	bigness = 1;

    x = eap->arg;
    ಬಗೆ = x;
    ಆದರೆ (*ಬಗೆ == '-' || *ಬಗೆ == '+' || *ಬಗೆ == '='
					      || *ಬಗೆ == '^' || *ಬಗೆ == '.')
	++x;
    ಆಗ (*x == '-' || *x == '+')
	++x;

    ಆದರೆ (*x != 0)
    {
	ಆದರೆ (!VIM_ISDIGIT(*x))
	{
	    emsg(_(e_non_numeric_ವಾದ_ಗಮ್ಯ_z));
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    bigness = atol((ಅಕ್ಷರ *)x);

	    // bigness could be < 0 if atol(x) overflows.
	    ಆದರೆ (bigness > 2 * curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || bigness < 0)
		bigness = 2 * curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

	    p_ಕಿಟಕಿ = bigness;
	    ಆದರೆ (*ಬಗೆ == '=')
		bigness += 2;
	}
    }

    // the number of '-' and '+' multiplies the distance
    ಆದರೆ (*ಬಗೆ == '-' || *ಬಗೆ == '+')
	ಗಾಗಿ (x = ಬಗೆ + 1; *x == *ಬಗೆ; ++x)
	    ;

    ಆಯ್ಕೆ (*ಬಗೆ)
    {
	ಸ್ಥಿತಿ '-':
	    ಆರಂಭ = lnum - bigness * (linenr_T)(x - ಬಗೆ) + 1;
	    ಕೊನೆ = ಆರಂಭ + bigness - 1;
	    curs = ಕೊನೆ;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '=':
	    ಆರಂಭ = lnum - (bigness + 1) / 2 + 1;
	    ಕೊನೆ = lnum + (bigness + 1) / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '^':
	    ಆರಂಭ = lnum - bigness * 2;
	    ಕೊನೆ = lnum - bigness;
	    curs = lnum - bigness;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '.':
	    ಆರಂಭ = lnum - (bigness + 1) / 2 + 1;
	    ಕೊನೆ = lnum + (bigness + 1) / 2 - 1;
	    curs = ಕೊನೆ;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:  // '+'
	    ಆರಂಭ = lnum;
	    ಆದರೆ (*ಬಗೆ == '+')
		ಆರಂಭ += bigness * (linenr_T)(x - ಬಗೆ - 1) + 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
		++ಆರಂಭ;
	    ಕೊನೆ = ಆರಂಭ + bigness - 1;
	    curs = ಕೊನೆ;
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (ಆರಂಭ < 1)
	ಆರಂಭ = 1;

    ಆದರೆ (ಕೊನೆ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (curs > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	curs = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curs < 1)
	curs = 1;

    ಗಾಗಿ (i = ಆರಂಭ; i <= ಕೊನೆ; i++)
    {
	ಆದರೆ (minus && i == lnum)
	{
	    ಸಂದೇಶ_putchar('\n');

	    ಗಾಗಿ (j = 1; j < Columns; j++)
		ಸಂದೇಶ_putchar('-');
	}

	print_ಸಾಲು(i, eap->flags & EXFLAG_NR, eap->flags & EXFLAG_LIST);

	ಆದರೆ (minus && i == lnum)
	{
	    ಸಂದೇಶ_putchar('\n');

	    ಗಾಗಿ (j = 1; j < Columns; j++)
		ಸಂದೇಶ_putchar('-');
	}
    }

    ಆದರೆ (curwin->w_cursor.lnum != curs)
    {
	curwin->w_cursor.lnum = curs;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    }
    ex_no_reprint = TRUE;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
check_restricted(ಶೂನ್ಯ)
{
    ಆದರೆ (restricted)
    {
	emsg(_(e_shell_commands_and_some_functionality_not_allowed_ಒಳ_rvim));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
check_secure(ಶೂನ್ಯ)
{
    ಆದರೆ (secure)
    {
	secure = 2;
	emsg(_(e_command_not_allowed_from_vimrc_ಒಳ_current_dir_or_ಟ್ಯಾಗ್_ಹುಡುಕು));
	ಹಿಂತಿರುಗಿ TRUE;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    ಆದರೆ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ಒಳ_sandbox));
	ಹಿಂತಿರುಗಿ TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ char_u	*old_sub = ಖಾಲಿ;	// previous substitute pattern
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	global_need_beginline;	// call beginline() after ":g"

/*
 * Flags that are kept between calls to :substitute.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	do_all;		// do multiple substitutions per line
    ಪೂರ್ಣಾಂಕ	do_ask;		// ask for confirmation
    ಪೂರ್ಣಾಂಕ	do_ಎಣಿಕೆ;	// count only
    ಪೂರ್ಣಾಂಕ	do_error;	// if false, ignore errors
    ಪೂರ್ಣಾಂಕ	do_print;	// print last line with subs.
    ಪೂರ್ಣಾಂಕ	do_list;	// list last line with subs.
    ಪೂರ್ಣಾಂಕ	do_ಸಂಖ್ಯೆ;	// list last line with line nr
    ಪೂರ್ಣಾಂಕ	do_ic;		// ignore case flag
} subflags_T;

/*
 * Skip over the "sub" part in :s/pat/sub/ where "delimiter" is the separating
 * character.
 */
    char_u *
skip_substitute(char_u *ಆರಂಭ, ಪೂರ್ಣಾಂಕ delimiter)
{
    char_u *p = ಆರಂಭ;

    ಆಗ (p[0])
    {
	ಆದರೆ (p[0] == delimiter)		// end delimiter found
	{
	    *p++ = NUL;			// replace it with a NUL
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (p[0] == '\\' && p[1] != 0)	// skip escaped characters
	    ++p;
	MB_PTR_ADV(p);
    }
    ಹಿಂತಿರುಗಿ p;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_regexp_delim(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (ಸುರಕ್ಷಿತ_isalpha(c))
    {
	emsg(_(e_regular_expressions_cant_be_delimited_by_letters));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    ಶೂನ್ಯ
ex_substitute(exarg_T *eap)
{
    linenr_T	lnum;
    ದೀರ್ಘ	i = 0;
    regmmatch_T regmatch;
    ಸ್ಥಿರ subflags_T subflags = {FALSE, FALSE, FALSE, TRUE, FALSE,
							      FALSE, FALSE, 0};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    subflags_T	subflags_save;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		save_do_all;		// remember user specified 'g' flag
    ಪೂರ್ಣಾಂಕ		save_do_ask;		// remember user specified 'c' flag
    char_u	*sub = ಖಾಲಿ;		// init for GCC
    string_T	pat = {ಖಾಲಿ, 0};
    ಪೂರ್ಣಾಂಕ		delimiter;
    size_t	sublen;
    ಪೂರ್ಣಾಂಕ		got_quit = FALSE;
    ಪೂರ್ಣಾಂಕ		got_ಹೊಂದಿಕೆ = FALSE;
    ಪೂರ್ಣಾಂಕ		which_pat;
    char_u	*cmd;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		save_State;
    linenr_T	ಮೊದಲ_ಸಾಲು = 0;		// first changed line
    linenr_T	ಕೊನೆಯ_ಸಾಲು = 0;		// below last changed line AFTER the
					// change
    linenr_T	old_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    linenr_T	line2;
    ದೀರ್ಘ	nmatch;			// number of lines in match
    string_T	sub_firstline;		// allocated copy of first sub line
    ಪೂರ್ಣಾಂಕ		endcolumn = FALSE;	// cursor in last column when done
    pos_T	old_cursor = curwin->w_cursor;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_nsubs;
    ಪೂರ್ಣಾಂಕ		keeppatterns = cmdmod.cmod_flags & CMOD_KEEPPATTERNS;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		save_ma = 0;
    ಪೂರ್ಣಾಂಕ		save_sandbox = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    textprop_T	*ಪಠ್ಯ_props = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    cmd = eap->arg;
    ಆದರೆ (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }
    ಆರಂಭ_nsubs = sub_nsubs;

    ಆದರೆ (eap->cmdidx == CMD_tilde)
	which_pat = RE_ಕೊನೆಯ;	// use last used regexp
    ಇಲ್ಲದಿದ್ದರೆ
	which_pat = RE_SUBST;	// use last substitute regexp

				// new pattern and substitution
    ಆದರೆ (eap->cmd[0] == 's' && *cmd != NUL && !VIM_ISWHITE(*cmd)
		&& vim_strchr((char_u *)"0123456789cegriIp|\"", *cmd) == ಖಾಲಿ)
    {
	// don't accept alphanumeric for separator
	ಆದರೆ (check_regexp_delim(*cmd) == FAIL)
	    ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (ಒಳ_vim9script() && check_global_and_subst(eap->cmd, eap->arg)
								      == FAIL)
	    ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	ಆದರೆ (*cmd == '\\')
	{
	    ಆದರೆ (ಒಳ_vim9script())
	    {
		emsg(_(e_cannot_use_s_backslash_ಒಳ_vim9_script));
		ಹಿಂತಿರುಗಿ;
	    }
	    ++cmd;
	    ಆದರೆ (vim_strchr((char_u *)"/?&", *cmd) == ಖಾಲಿ)
	    {
		emsg(_(e_backslash_should_be_followed_by));
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (*cmd != '&')
		which_pat = RE_ಹುಡುಕು;	    // use last '/' pattern
	    STR_LITERAL_ಗಣ(pat, "");	    // empty search pattern
	    delimiter = *cmd++;		    // remember delimiter character
	}
	ಇಲ್ಲದಿದ್ದರೆ		// find the end of the regexp
	{
	    which_pat = RE_ಕೊನೆಯ;	    // use last used regexp
	    delimiter = *cmd++;		    // remember delimiter character
	    pat.string = cmd;		    // remember start of search pat
	    cmd = skip_regexp_ex(cmd, delimiter, magic_isset(),
							&eap->arg, ಖಾಲಿ, ಖಾಲಿ);
	    pat.ಉದ್ದಳತೆ = (size_t)(cmd - pat.string);
	    ಆದರೆ (cmd[0] == delimiter)	    // end delimiter found
		*cmd++ = NUL;		    // replace it with a NUL
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	p = cmd;	    // remember the start of the substitution
	cmd = skip_substitute(cmd, delimiter);
	sub = vim_strsave(p);
	ಆದರೆ (sub == ಖಾಲಿ)
	    // out of memory
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!eap->skip)
	{
	    // In POSIX vi ":s/pat/%/" uses the previous subst. string.
	    ಆದರೆ (STRCMP(sub, "%") == 0
				 && vim_strchr(p_cpo, CPO_SUBPERCENT) != ಖಾಲಿ)
	    {
		ಆದರೆ (old_sub == ಖಾಲಿ)	// there is no previous command
		{
		    emsg(_(e_no_ಹಿಂದಿನ_substitute_regular_ಅಭಿವ್ಯಕ್ತಿ));
		    vim_free(sub);
		    ಹಿಂತಿರುಗಿ;
		}
		vim_free(sub);
		sub = vim_strsave(old_sub);
		ಆದರೆ (sub == ಖಾಲಿ)
		    // out of memory
		    ಹಿಂತಿರುಗಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!keeppatterns)
	    {
		vim_free(old_sub);
		old_sub = vim_strsave(sub);
		ಆದರೆ (old_sub == ಖಾಲಿ)
		{
		    // out of memory
		    vim_free(sub);
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!eap->skip)	// use previous pattern and substitution
    {
	ಆದರೆ (old_sub == ಖಾಲಿ)	// there is no previous command
	{
	    emsg(_(e_no_ಹಿಂದಿನ_substitute_regular_ಅಭಿವ್ಯಕ್ತಿ));
	    ಹಿಂತಿರುಗಿ;
	}
	pat.string = ಖಾಲಿ;		// search_regcomp() will use previous pattern
	pat.ಉದ್ದಳತೆ = 0;
	sub = vim_strsave(old_sub);
	ಆದರೆ (sub == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	// Vi compatibility quirk: repeating with ":s" keeps the cursor in the
	// last column after using "$".
	endcolumn = (curwin->w_curswant == MAXCOL);
    }

    // Recognize ":%s/\n//" and turn it into a join command, which is much
    // more efficient.
    // TODO: find a generic solution to make line-joining operations more
    // efficient, avoid allocating a string that grows in size.
    ಆದರೆ (pat.string != ಖಾಲಿ && STRCMP(pat.string, "\\n") == 0
	    && *sub == NUL
	    && (*cmd == NUL || (cmd[1] == NUL && (*cmd == 'g' || *cmd == 'l'
					     || *cmd == 'p' || *cmd == '#'))))
    {
	linenr_T    joined_ಸಾಲುಗಳು_ಎಣಿಕೆ;

	ಆದರೆ (eap->skip)
	{
	    vim_free(sub);
	    ಹಿಂತಿರುಗಿ;
	}
	curwin->w_cursor.lnum = eap->line1;
	ಆದರೆ (*cmd == 'l')
	    eap->flags = EXFLAG_LIST;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == '#')
	    eap->flags = EXFLAG_NR;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'p')
	    eap->flags = EXFLAG_PRINT;

	// The number of lines joined is the number of lines in the range plus
	// one.  One less when the last line is included.
	joined_ಸಾಲುಗಳು_ಎಣಿಕೆ = eap->line2 - eap->line1 + 1;
	ಆದರೆ (eap->line2 < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ++joined_ಸಾಲುಗಳು_ಎಣಿಕೆ;
	ಆದರೆ (joined_ಸಾಲುಗಳು_ಎಣಿಕೆ > 1)
	{
	    (ಶೂನ್ಯ)do_join(joined_ಸಾಲುಗಳು_ಎಣಿಕೆ, FALSE, TRUE, FALSE, TRUE);
	    sub_nsubs = joined_ಸಾಲುಗಳು_ಎಣಿಕೆ - 1;
	    sub_nlines = 1;
	    (ಶೂನ್ಯ)do_sub_ಸಂದೇಶ(FALSE);
	    ex_may_print(eap);
	}

	ಆದರೆ (!keeppatterns)
	    save_re_pat(RE_SUBST, pat.string, pat.ಉದ್ದಳತೆ, magic_isset());
	// put pattern in history
	ಕೂಡಿಸು_ಗಮ್ಯ_history(HIST_ಹುಡುಕು, pat.string, pat.ಉದ್ದಳತೆ, TRUE, NUL);
	vim_free(sub);

	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    ಆದರೆ (*cmd == '&')
	++cmd;
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (ಒಳ_vim9script())
	{
	    // ignore 'gdefault' and 'edcompatible'
	    subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!p_ed)
	{
	    ಆದರೆ (p_gd)		// default is global on
		subflags.do_all = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		subflags.do_all = FALSE;
	    subflags.do_ask = FALSE;
	}
	subflags.do_error = TRUE;
	subflags.do_print = FALSE;
	subflags.do_list = FALSE;
	subflags.do_ಎಣಿಕೆ = FALSE;
	subflags.do_ಸಂಖ್ಯೆ = FALSE;
	subflags.do_ic = 0;
    }
    ಆಗ (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
	 * 'r' is never inverted.
	 */
	ಆದರೆ (*cmd == 'g')
	    subflags.do_all = !subflags.do_all;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'c')
	    subflags.do_ask = !subflags.do_ask;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'n')
	    subflags.do_ಎಣಿಕೆ = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'e')
	    subflags.do_error = !subflags.do_error;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'r')	    // use last used regexp
	    which_pat = RE_ಕೊನೆಯ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'p')
	    subflags.do_print = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == '#')
	{
	    subflags.do_print = TRUE;
	    subflags.do_ಸಂಖ್ಯೆ = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'l')
	{
	    subflags.do_print = TRUE;
	    subflags.do_list = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'i')	    // ignore case
	    subflags.do_ic = 'i';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 'I')	    // don't ignore case
	    subflags.do_ic = 'I';
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
	++cmd;
    }
    ಆದರೆ (subflags.do_ಎಣಿಕೆ)
	subflags.do_ask = FALSE;

    save_do_all = subflags.do_all;
    save_do_ask = subflags.do_ask;

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    ಆದರೆ (VIM_ISDIGIT(*cmd))
    {
	i = getdigits(&cmd);
	ಆದರೆ (i <= 0 && !eap->skip && subflags.do_error)
	{
	    emsg(_(e_ಧನಾತ್ಮಕ_ಎಣಿಕೆ_required));
	    vim_free(sub);
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	{
	    ಅಕ್ಷರ	buf[20];
	    vim_snprintf(buf, ಗಾತ್ರ(buf), "%ld", i);
	    semsg(_(e_val_too_large), buf);
	    vim_free(sub);
	    ಹಿಂತಿರುಗಿ;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
	ಆದರೆ (eap->line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    ಆದರೆ (*cmd && *cmd != '"')	    // if not end-of-line or comment
    {
	ಗಣ_nextcmd(eap, cmd);
	ಆದರೆ (eap->nextcmd == ಖಾಲಿ)
	{
	    semsg(_(e_trailing_characters_str), cmd);
	    vim_free(sub);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (eap->skip)	    // not executing commands, only parsing
    {
	vim_free(sub);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!subflags.do_ಎಣಿಕೆ && !curbuf->b_p_ma)
    {
	// Substitution is not allowed in non-'modifiable' buffer
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	vim_free(sub);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಹುಡುಕು_regcomp(pat.string, pat.ಉದ್ದಳತೆ, ಖಾಲಿ, RE_SUBST, which_pat, ಹುಡುಕು_HIS, &regmatch) == FAIL)
    {
	ಆದರೆ (subflags.do_error)
	    emsg(_(e_invalid_command));
	vim_free(sub);
	ಹಿಂತಿರುಗಿ;
    }

    // the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase'
    ಆದರೆ (subflags.do_ic == 'i')
	regmatch.rmm_ic = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (subflags.do_ic == 'I')
	regmatch.rmm_ic = FALSE;

    sub_firstline.string = ಖಾಲಿ;
    sub_firstline.ಉದ್ದಳತೆ = 0;

    /*
     * If the substitute pattern starts with "\=" then it's an expression.
     * Make a copy, a recursive function may free it.
     * Otherwise, '~' in the substitute pattern is replaced with the old
     * pattern.  We do it here once to avoid it to be replaced over and over
     * again.
     */
    ಆದರೆ (sub[0] == '\\' && sub[1] == '=')
    {
	p = vim_strsave(sub);
	vim_free(sub);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	sub = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = regtilde(sub, magic_isset());

	ಆದರೆ (p != sub)
	{
	    vim_free(sub);
	    sub = p;
	}
    }

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    ಗಾಗಿ (lnum = eap->line1; lnum <= line2 && !(got_quit
#ಆದರೆ defined(FEAT_EVAL)
		|| aborting()
#ಆದರೆ_ಅಂತ್ಯ
		); ++lnum)
    {
	nmatch = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ಖಾಲಿ);
	ಆದರೆ (nmatch)
	{
	    colnr_T	copycol;
	    colnr_T	matchcol;
	    colnr_T	prev_matchcol = MAXCOL;
	    string_T	new_ಆರಂಭ = {ಖಾಲಿ, 0};
	    size_t	new_ಆರಂಭ_ಗಾತ್ರ = 0;
	    char_u	*new_ಕೊನೆ;
	    ಪೂರ್ಣಾಂಕ		did_sub = FALSE;
	    ಪೂರ್ಣಾಂಕ		lastone;
	    size_t	copy_len;
	    size_t	needed_ಗಾತ್ರ;
	    ದೀರ್ಘ	nmatch_tl = 0;	// nr of lines matched below lnum
	    ಪೂರ್ಣಾಂಕ		do_again;	// do it again after joining lines
	    ಪೂರ್ಣಾಂಕ		skip_ಹೊಂದಿಕೆ = FALSE;
	    linenr_T	sub_firstlnum;	// nr of first sub line
	    bool	did_ವಿಭಜಿಸು = false;	// "\r" split the line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಪೂರ್ಣಾಂಕ		apc_flags = APC_SAVE_FOR_UNDO | APC_SUBSTITUTE;
	    colnr_T	ಒಟ್ಟು_added =  0;
	    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

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
	    ಆದರೆ (!got_ಹೊಂದಿಕೆ)
	    {
		setpcmark();
		got_ಹೊಂದಿಕೆ = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    ಗಾಗಿ (;;)
	    {
		string_T    tmp;
		char_u	    *p1;
		size_t	    n;

		// Advance "lnum" to the line where the match starts.  The
		// match does not start in the first line when there is a line
		// break before \zs.
		ಆದರೆ (regmatch.startpos[0].lnum > 0)
		{
		    lnum += regmatch.startpos[0].lnum;
		    sub_firstlnum += regmatch.startpos[0].lnum;
		    nmatch -= regmatch.startpos[0].lnum;
		    VIM_CLEAR_STRING(sub_firstline);
		}

		// Match might be after the last line for "\n\zs" matching at
		// the end of the last line.
		ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ನಿಲ್ಲಿಸು;

		ಆದರೆ (sub_firstline.string == ಖಾಲಿ)
		{
		    sub_firstline.ಉದ್ದಳತೆ = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.ಉದ್ದಳತೆ);
		    ಆದರೆ (sub_firstline.string == ಖಾಲಿ)
		    {
			vim_free(new_ಆರಂಭ.string);
			ಹೋಗು outofmem;
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
		ಆದರೆ (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].ಲಂಬಸಾಲು)
		{
		    ಆದರೆ (sub_firstline.string[matchcol] == NUL)
			// We already were at the end of the line.  Don't look
			// for a match in this line again.
			skip_ಹೊಂದಿಕೆ = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			 // search for a match at next column
			ಆದರೆ (has_mbyte)
			    matchcol += mb_ptr2len(sub_firstline.string + matchcol);
			ಇಲ್ಲದಿದ್ದರೆ
			    ++matchcol;
		    }
		    ಹೋಗು skip;
		}

		// Normally we continue searching for a match just after the
		// previous match.
		matchcol = regmatch.endpos[0].ಲಂಬಸಾಲು;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_count is set only increase the counter.
		 *    If do_ask is set, ask for confirmation.
		 */
		ಆದರೆ (subflags.do_ಎಣಿಕೆ)
		{
		    // For a multi-line match, put matchcol at the NUL at
		    // the end of the line and set nmatch to one, so that
		    // we continue looking for a match on the next line.
		    // Avoids that ":s/\nB\@=//gc" get stuck.
		    ಆದರೆ (nmatch > 1)
		    {
			matchcol = (colnr_T)sub_firstline.ಉದ್ದಳತೆ;
			nmatch = 1;
			skip_ಹೊಂದಿಕೆ = TRUE;
		    }
		    sub_nsubs++;
		    did_sub = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    // Skip the substitution, unless an expression is used,
		    // then it is evaluated in the sandbox.
		    ಆದರೆ (!(sub[0] == '\\' && sub[1] == '='))
#ಆದರೆ_ಅಂತ್ಯ
			ಹೋಗು skip;
		}

		ಆದರೆ (subflags.do_ask)
		{
		    ಪೂರ್ಣಾಂಕ typed = 0;

		    // change State to MODE_CONFIRM, so that the mouse works
		    // properly
		    save_State = State;
		    State = ಬಹುಲಕ_CONFIRM;
		    setmouse();		// disable mouse in xterm
		    curwin->w_cursor.ಲಂಬಸಾಲು = regmatch.startpos[0].ಲಂಬಸಾಲು;
		    ಆದರೆ (curwin->w_p_crb)
			do_check_cursorbind();

		    // When 'cpoptions' contains "u" don't sync undo when
		    // asking for confirmation.
		    ಆದರೆ (vim_strchr(p_cpo, CPO_UNDO) != ಖಾಲಿ)
			++no_u_sync;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    ಆಗ (subflags.do_ask)
		    {
			ಆದರೆ (exmode_ಸಕ್ರಿಯ)
			{
			    char_u	*resp;
			    colnr_T	sc, ec;

			    print_ಸಾಲು_no_ಪೂರ್ವಪ್ರತ್ಯಯ(lnum,
					 subflags.do_ಸಂಖ್ಯೆ, subflags.do_list);

			    getvcol(curwin, &curwin->w_cursor,
							   &sc, ಖಾಲಿ, ಖಾಲಿ, 0);
			    curwin->w_cursor.ಲಂಬಸಾಲು = regmatch.endpos[0].ಲಂಬಸಾಲು - 1;
			    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < 0)
				curwin->w_cursor.ಲಂಬಸಾಲು = 0;
			    getvcol(curwin, &curwin->w_cursor,
							   ಖಾಲಿ, ಖಾಲಿ, &ec, 0);
			    curwin->w_cursor.ಲಂಬಸಾಲು = regmatch.startpos[0].ಲಂಬಸಾಲು;
			    ಆದರೆ (subflags.do_ಸಂಖ್ಯೆ || curwin->w_p_nu)
			    {
				ಪೂರ್ಣಾಂಕ numw = ಸಂಖ್ಯೆ_ಅಗಲ(curwin) + 1;
				sc += numw;
				ec += numw;
			    }
			    ಸಂದೇಶ_ಆರಂಭ();
			    ಗಾಗಿ (i = 0; i < (ದೀರ್ಘ)sc; ++i)
				ಸಂದೇಶ_putchar(' ');
			    ಗಾಗಿ ( ; i <= (ದೀರ್ಘ)ec; ++i)
				ಸಂದೇಶ_putchar('^');

			    resp = getexmodeline('?', ಖಾಲಿ, 0, TRUE);
			    ಆದರೆ (resp != ಖಾಲಿ)
			    {
				typed = *resp;
				vim_free(resp);
				// When ":normal" runs out of characters we get
				// an empty line.  Use "q" to get out of the
				// loop.
				ಆದರೆ (ex_ಸಾಮಾನ್ಯ_busy && typed == NUL)
				    typed = 'q';
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    string_T orig_ಸಾಲು = {ಖಾಲಿ, 0};
			    ಪೂರ್ಣಾಂಕ    len_change = 0;
			    ಪೂರ್ಣಾಂಕ	   save_p_lz = p_lz;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    ಪೂರ್ಣಾಂಕ save_p_fen = curwin->w_p_fen;

			    curwin->w_p_fen = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
			    // Invert the matched string.
			    // Remove the inversion afterwards.
			    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
			    RedrawingDisabled = 0;

			    // avoid calling update_screen() in vgetorpeek()
			    p_lz = FALSE;

			    ಆದರೆ (new_ಆರಂಭ.string != ಖಾಲಿ)
			    {
				// There already was a substitution, we would
				// like to show this to the user.  We cannot
				// really update the line, it would change
				// what matches.  Temporarily replace the line
				// and change it back afterwards.
				orig_ಸಾಲು.ಉದ್ದಳತೆ = ml_get_len(lnum);
				orig_ಸಾಲು.string = vim_strnsave(ml_get(lnum), orig_ಸಾಲು.ಉದ್ದಳತೆ);
				ಆದರೆ (orig_ಸಾಲು.string != ಖಾಲಿ)
				{
				    string_T new_ಸಾಲು;

				    new_ಸಾಲು.string = concat_str(new_ಆರಂಭ.string,
						     sub_firstline.string + copycol);
				    ಆದರೆ (new_ಸಾಲು.string == ಖಾಲಿ)
					VIM_CLEAR_STRING(orig_ಸಾಲು);
				    ಇಲ್ಲದಿದ್ದರೆ
				    {
					new_ಸಾಲು.ಉದ್ದಳತೆ =
					    new_ಆರಂಭ.ಉದ್ದಳತೆ + (sub_firstline.ಉದ್ದಳತೆ - copycol);
					// Position the cursor relative to the
					// end of the line, the previous
					// substitute may have inserted or
					// deleted characters before the
					// cursor.
					len_change =
					    (ಪೂರ್ಣಾಂಕ)new_ಸಾಲು.ಉದ್ದಳತೆ - (ಪೂರ್ಣಾಂಕ)orig_ಸಾಲು.ಉದ್ದಳತೆ;
					curwin->w_cursor.ಲಂಬಸಾಲು += len_change;
					ml_replace(lnum, new_ಸಾಲು.string, FALSE);
				    }
				}
			    }

			    ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು = regmatch.endpos[0].lnum
						  - regmatch.startpos[0].lnum;
			    ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol = regmatch.endpos[0].ಲಂಬಸಾಲು
								 + len_change;
			    ಆದರೆ (ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು == 0
						   && ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol == 0)
				// highlight at least one character for /^/
				ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol = 1;
			    highlight_ಹೊಂದಿಕೆ = TRUE;

			    update_topline();
			    validate_cursor();
			    update_screen(UPD_SOME_VALID);
			    highlight_ಹೊಂದಿಕೆ = FALSE;
			    redraw_later(UPD_SOME_VALID);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    curwin->w_p_fen = save_p_fen;
#ಆದರೆ_ಅಂತ್ಯ
			    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1)
				ಸಂದೇಶ_didout = FALSE;	// avoid a scroll-up
			    ಸಂದೇಶ_starthere();
			    i = ಸಂದೇಶ_scroll;
			    ಸಂದೇಶ_scroll = 0;		// truncate msg when
							// needed
			    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = TRUE;
			    // write message same highlighting as for
			    // wait_return()
			    smsg_attr(HL_ATTR(HLF_R),
				_("replace with %s (y/n/a/q/l/^E/^Y)?"), sub);
			    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = FALSE;
			    ಸಂದೇಶ_scroll = i;
			    showruler(TRUE);
			    windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
			    RedrawingDisabled = save_RedrawingDisabled;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
			    dont_scroll = FALSE; // allow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
			    ++no_mapping;	// don't map this key
			    ++allow_ಕೀಲಿಗಳು;	// allow special keys
			    typed = plain_vgetc();
			    --allow_ಕೀಲಿಗಳು;
			    --no_mapping;

			    // clear the question
			    ಸಂದೇಶ_didout = FALSE;	// don't scroll up
			    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
			    gotocmdline(TRUE);
			    p_lz = save_p_lz;

			    // restore the line
			    ಆದರೆ (orig_ಸಾಲು.string != ಖಾಲಿ)
				ml_replace(lnum, orig_ಸಾಲು.string, FALSE);
			}

			need_wait_return = FALSE; // no hit-return prompt
			ಆದರೆ (typed == 'q' || typed == ESC || typed == Ctrl_C
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
				|| typed == intr_char
#ಆದರೆ_ಅಂತ್ಯ
				)
			{
			    got_quit = TRUE;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (typed == 'n')
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (typed == 'y')
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (typed == 'l')
			{
			    // last: replace and then stop
			    subflags.do_all = FALSE;
			    line2 = lnum;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (typed == 'a')
			{
			    subflags.do_ask = FALSE;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (typed == Ctrl_E)
			    scrollup_clamp();
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (typed == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    State = save_State;
		    setmouse();
		    ಆದರೆ (vim_strchr(p_cpo, CPO_UNDO) != ಖಾಲಿ)
			--no_u_sync;

		    ಆದರೆ (typed == 'n')
		    {
			// For a multi-line match, put matchcol at the NUL at
			// the end of the line and set nmatch to one, so that
			// we continue looking for a match on the next line.
			// Avoids that ":%s/\nB\@=//gc" and ":%s/\n/,\r/gc"
			// get stuck when pressing 'n'.
			ಆದರೆ (nmatch > 1)
			{
			    matchcol = (colnr_T)sub_firstline.ಉದ್ದಳತೆ;
			    skip_ಹೊಂದಿಕೆ = TRUE;
			}
			ಹೋಗು skip;
		    }
		    ಆದರೆ (got_quit)
			ಹೋಗು skip;
		}

		// Move the cursor to the start of the match, so that we can
		// use "\=col(".").
		curwin->w_cursor.ಲಂಬಸಾಲು = regmatch.startpos[0].ಲಂಬಸಾಲು;

		/*
		 * 3. substitute the string.
		 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		save_ma = curbuf->b_p_ma;
		save_sandbox = sandbox;
		ಆದರೆ (subflags.do_ಎಣಿಕೆ)
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
#ಆದರೆ_ಅಂತ್ಯ
		// Get length of substitution part, including the NUL.
		// When it fails sublen is zero.
		sublen = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, sub_firstline.string, 0,
		    REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		--textlock;

		// If getting the substitute string caused an error, don't do
		// the replacement.
		// Don't keep flags set by a recursive call.
		subflags = subflags_save;
		ಆದರೆ (sublen == 0 || aborting() || subflags.do_ಎಣಿಕೆ)
		{
		    curbuf->b_p_ma = save_ma;
		    sandbox = save_sandbox;
		    ಹೋಗು skip;
		}
#ಆದರೆ_ಅಂತ್ಯ

		// When the match included the "$" of the last line it may
		// go beyond the last line of the buffer.
		ಆದರೆ (nmatch > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - sub_firstlnum + 1)
		{
		    nmatch = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - sub_firstlnum + 1;
		    skip_ಹೊಂದಿಕೆ = TRUE;
		    // safety check
		    ಆದರೆ (nmatch < 0)
			ಹೋಗು skip;
		}

		// Need room for:
		// - result so far in new_start (not for first sub in line)
		// - original text up to match
		// - length of substituted part
		// - original text after match
		// Adjust text properties here, since we have all information
		// needed.
		ಆದರೆ (nmatch == 1)
		{
		    tmp.string = sub_firstline.string;
		    tmp.ಉದ್ದಳತೆ = sub_firstline.ಉದ್ದಳತೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    ಆದರೆ (curbuf->b_has_textprop)
		    {
			ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು_added = (ಪೂರ್ಣಾಂಕ)sublen - 1 - (regmatch.endpos[0].ಲಂಬಸಾಲು
						   - regmatch.startpos[0].ಲಂಬಸಾಲು);

			// When text properties are changed, need to save for
			// undo first, unless done already.
			ಆದರೆ (adjust_prop_columns(lnum,
					ಒಟ್ಟು_added + regmatch.startpos[0].ಲಂಬಸಾಲು,
						       ಬೈಟುಗಳು_added, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			// Offset for column byte number of the text property
			// in the resulting buffer afterwards.
			ಒಟ್ಟು_added += (colnr_T)ಬೈಟುಗಳು_added;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    linenr_T	lastlnum = sub_firstlnum + nmatch - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    ಆದರೆ (curbuf->b_has_textprop)
		    {
			char_u	*prop_ಆರಂಭ;

			// Props in the first line may be shortened or deleted
			ಆದರೆ (adjust_prop_columns(lnum,
					ಒಟ್ಟು_added + regmatch.startpos[0].ಲಂಬಸಾಲು,
						       -MAXCOL, apc_flags))
			    apc_flags &= ~APC_SAVE_FOR_UNDO;
			ಒಟ್ಟು_added -=
			    (colnr_T)(sub_firstline.ಉದ್ದಳತೆ - regmatch.startpos[0].ಲಂಬಸಾಲು);

			// Props in the last line may be moved or deleted
			ಆದರೆ (adjust_prop_columns(lastlnum,
					0, -regmatch.endpos[0].ಲಂಬಸಾಲು, apc_flags))
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    apc_flags &= ~APC_SAVE_FOR_UNDO;

			// Copy the text props of the last line, they will be
			// later appended to the changed line.
			ಪಠ್ಯ_prop_ಎಣಿಕೆ = get_ಪಠ್ಯ_props(curbuf, lastlnum,
							   &prop_ಆರಂಭ, FALSE);
			ಆದರೆ (ಪಠ್ಯ_prop_ಎಣಿಕೆ > 0)
			{
			    // TODO: what when we already did this?
			    vim_free(ಪಠ್ಯ_props);
			    ಪಠ್ಯ_props = ALLOC_MULT(textprop_T,
							      ಪಠ್ಯ_prop_ಎಣಿಕೆ);
			    ಆದರೆ (ಪಠ್ಯ_props != ಖಾಲಿ)
			    {
				mch_memmove(ಪಠ್ಯ_props, prop_ಆರಂಭ,
					 ಪಠ್ಯ_prop_ಎಣಿಕೆ * ಗಾತ್ರ(textprop_T));
				// Filter out virtual text and continuation
				// properties from deleted lines, convert
				// offsets to pointers, and adjust columns.
				ಪೂರ್ಣಾಂಕ wi = 0;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ pi = 0; pi < ಪಠ್ಯ_prop_ಎಣಿಕೆ; ++pi)
				{
				    // Skip virtual text and continuation
				    // properties from the deleted line.
				    ಆದರೆ (ಪಠ್ಯ_props[pi].tp_id < 0
					    || (ಪಠ್ಯ_props[pi].tp_flags
							& TP_FLAG_CONT_PREV))
					ಮುಂದುವರಿ;
				    ಪಠ್ಯ_props[wi] = ಪಠ್ಯ_props[pi];
				    ಪಠ್ಯ_props[wi].tp_ಲಂಬಸಾಲು +=
					regmatch.startpos[0].ಲಂಬಸಾಲು + (colnr_T)sublen - 1;
				    ಪಠ್ಯ_props[wi].u.tp_ಪಠ್ಯ = ಖಾಲಿ;
				    ++wi;
				}
				ಪಠ್ಯ_prop_ಎಣಿಕೆ = wi;
			    }
			}
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    tmp.string = ml_get(lastlnum);
		    tmp.ಉದ್ದಳತೆ = ml_get_len(lastlnum);
		    nmatch_tl += nmatch - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    ಆದರೆ (curbuf->b_has_textprop)
			ಒಟ್ಟು_added +=
			    (colnr_T)(tmp.ಉದ್ದಳತೆ - regmatch.endpos[0].ಲಂಬಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ
		}

		copy_len = (size_t)(regmatch.startpos[0].ಲಂಬಸಾಲು - copycol);
		needed_ಗಾತ್ರ = copy_len
		    + (tmp.ಉದ್ದಳತೆ - (size_t)regmatch.endpos[0].ಲಂಬಸಾಲು) + sublen + 1;

		ಆದರೆ (new_ಆರಂಭ.string == ಖಾಲಿ)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_ಆರಂಭ_ಗಾತ್ರ = needed_ಗಾತ್ರ + 50;
		    ಆದರೆ ((new_ಆರಂಭ.string = alloc_clear(new_ಆರಂಭ_ಗಾತ್ರ)) == ಖಾಲಿ)
			ಹೋಗು outofmem;
		    *new_ಆರಂಭ.string = NUL;
		    new_ಆರಂಭ.ಉದ್ದಳತೆ = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    needed_ಗಾತ್ರ += new_ಆರಂಭ.ಉದ್ದಳತೆ;
		    ಆದರೆ (needed_ಗಾತ್ರ > new_ಆರಂಭ_ಗಾತ್ರ)
		    {
			new_ಆರಂಭ_ಗಾತ್ರ = needed_ಗಾತ್ರ + 50;
			ಆದರೆ ((p1 = alloc_clear(new_ಆರಂಭ_ಗಾತ್ರ)) == ಖಾಲಿ)
			{
			    vim_free(new_ಆರಂಭ.string);
			    ಹೋಗು outofmem;
			}
			mch_memmove(p1, new_ಆರಂಭ.string, new_ಆರಂಭ.ಉದ್ದಳತೆ + 1);
			vim_free(new_ಆರಂಭ.string);
			new_ಆರಂಭ.string = p1;
		    }
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_ಆರಂಭ.string + new_ಆರಂಭ.ಉದ್ದಳತೆ,
		    sub_firstline.string + copycol, copy_len);
		new_ಆರಂಭ.ಉದ್ದಳತೆ += copy_len;

		// new text added here
		new_ಕೊನೆ = new_ಆರಂಭ.string + new_ಆರಂಭ.ಉದ್ದಳತೆ;

		ಆದರೆ (new_ಆರಂಭ_ಗಾತ್ರ - copy_len < sublen)
		    sublen = new_ಆರಂಭ_ಗಾತ್ರ - copy_len - 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		++textlock;
#ಆದರೆ_ಅಂತ್ಯ
		n = (size_t)vim_regsub_multi(&regmatch,
		    sub_firstlnum - regmatch.startpos[0].lnum,
		    sub, new_ಕೊನೆ, (ಪೂರ್ಣಾಂಕ)sublen,
		    REGSUB_COPY | REGSUB_BACKSLASH | (magic_isset() ? REGSUB_MAGIC : 0));

		ಆದರೆ (n > 0)
		    new_ಆರಂಭ.ಉದ್ದಳತೆ += n - 1;	    // remove 1 for the NUL

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		--textlock;
#ಆದರೆ_ಅಂತ್ಯ
		sub_nsubs++;
		did_sub = TRUE;

		// Move the cursor to the start of the line, to avoid that it
		// is beyond the end of the line after the substitution.
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;

		// For a multi-line match, make a copy of the last matched
		// line and continue in that one.
		ಆದರೆ (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline.string);
		    sub_firstline.ಉದ್ದಳತೆ = ml_get_len(sub_firstlnum);
		    sub_firstline.string =
			vim_strnsave(ml_get(sub_firstlnum), sub_firstline.ಉದ್ದಳತೆ);
		    ಆದರೆ (sub_firstline.string == ಖಾಲಿ)
		    {
			vim_free(new_ಆರಂಭ.string);
			ಹೋಗು outofmem;
		    }
		    // When going beyond the last line, stop substituting.
		    ಆದರೆ (sub_firstlnum <= line2)
			do_again = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
			subflags.do_all = FALSE;
		}

		// Remember next character to be copied.
		copycol = regmatch.endpos[0].ಲಂಬಸಾಲು;

		ಆದರೆ (skip_ಹೊಂದಿಕೆ)
		{
		    // Already hit end of the buffer, sub_firstlnum is one
		    // less than what it ought to be.
		    vim_free(sub_firstline.string);
		    sub_firstline.string = vim_strnsave((char_u *)"", 0);
		    ಆದರೆ (sub_firstline.string == ಖಾಲಿ)
		    {
			vim_free(new_ಆರಂಭ.string);
			ಹೋಗು outofmem;
		    }
		    sub_firstline.ಉದ್ದಳತೆ = 0;
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
		ಗಾಗಿ (p1 = new_ಕೊನೆ; *p1; ++p1)
		{
		    ಆದರೆ (p1[0] == '\\' && p1[1] != NUL)  // remove backslash
		    {
			n = (size_t)(new_ಆರಂಭ.ಉದ್ದಳತೆ - (p1 - new_ಆರಂಭ.string));
			mch_memmove(p1, p1 + 1, n + 1);
			--new_ಆರಂಭ.ಉದ್ದಳತೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			ಆದರೆ (curbuf->b_has_textprop)
			{
			    // When text properties are changed, need to save
			    // for undo first, unless done already.
			    ಆದರೆ (adjust_prop_columns(lnum,
					(colnr_T)(p1 - new_ಆರಂಭ.string), -1,
					apc_flags))
				apc_flags &= ~APC_SAVE_FOR_UNDO;
			}
#ಆದರೆ_ಅಂತ್ಯ
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p1 == CAR)
		    {
			ಆದರೆ (u_inssub(lnum) == OK)   // prepare for undo
			{
			    colnr_T	plen = (colnr_T)(p1 - new_ಆರಂಭ.string + 1);

			    *p1 = NUL;		    // truncate up to the CR
			    ml_append(lnum - 1, new_ಆರಂಭ.string, plen, FALSE);
			    mark_adjust(lnum + 1, (linenr_T)MAXLNUM, 1L, 0L);
			    ಆದರೆ (subflags.do_ask)
				appended_ಸಾಲುಗಳು(lnum - 1, 1L);
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಆದರೆ (ಮೊದಲ_ಸಾಲು == 0)
				    ಮೊದಲ_ಸಾಲು = lnum;
				ಕೊನೆಯ_ಸಾಲು = lnum + 1;
			    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			    adjust_props_for_ವಿಭಜಿಸು(lnum + 1, lnum,
							       plen, 1, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
			    // all line numbers increase
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    did_ವಿಭಜಿಸು = true;
			    // move the cursor to the new line, like Vi
			    ++curwin->w_cursor.lnum;
			    // copy the rest
			    n = new_ಆರಂಭ.ಉದ್ದಳತೆ - (size_t)plen;
			    mch_memmove(new_ಆರಂಭ.string, p1 + 1, n + 1);
			    new_ಆರಂಭ.ಉದ್ದಳತೆ = n;
			    p1 = new_ಆರಂಭ.string - 1;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
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
		lastone = (skip_ಹೊಂದಿಕೆ
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
		ಆದರೆ (lastone
			|| nmatch_tl > 0
			|| (subflags.do_ask && did_ವಿಭಜಿಸು)
			|| (nmatch = vim_regexec_multi(&regmatch, curwin,
							curbuf, sub_firstlnum,
						    matchcol, ಖಾಲಿ)) == 0
			|| regmatch.startpos[0].lnum > 0)
		{
		    ಆದರೆ (new_ಆರಂಭ.string != ಖಾಲಿ)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * "matchcol" has to be adjusted, we use the end of
			 * the line as reference, because the substitute may
			 * have changed the number of characters.  Same for
			 * "prev_matchcol".
			 */
			STRCPY(new_ಆರಂಭ.string + new_ಆರಂಭ.ಉದ್ದಳತೆ,
			    sub_firstline.string + copycol);
			new_ಆರಂಭ.ಉದ್ದಳತೆ += sub_firstline.ಉದ್ದಳತೆ - (size_t)copycol;
			matchcol = (colnr_T)(sub_firstline.ಉದ್ದಳತೆ - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.ಉದ್ದಳತೆ
							      - prev_matchcol);

			ಆದರೆ (u_savesub(lnum) != OK)
			    ನಿಲ್ಲಿಸು;
			ml_replace(lnum, new_ಆರಂಭ.string, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			ಆದರೆ (ಪಠ್ಯ_props != ಖಾಲಿ)
			    ಕೂಡಿಸು_ಪಠ್ಯ_props(lnum, ಪಠ್ಯ_props, ಪಠ್ಯ_prop_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
			ಆದರೆ (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    ಆದರೆ (u_savedel(lnum, nmatch_tl) != OK)
				ನಿಲ್ಲಿಸು;
			    ಗಾಗಿ (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum);
			    ಆದರೆ (copycol > 0)
				mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (ದೀರ್ಘ)MAXLNUM, -nmatch_tl);
			    ಇಲ್ಲದಿದ್ದರೆ
				// The entire last matched line was consumed,
				// so the first line was effectively replaced
				// by lines below.
				mark_adjust(lnum - 1, lnum - 1,
						   (ದೀರ್ಘ)MAXLNUM, -nmatch_tl);
			    ಆದರೆ (subflags.do_ask)
				deleted_ಸಾಲುಗಳು(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; // nr of lines decreases
			    nmatch_tl = 0;
			}

			// When asking, undo is saved each time, must also set
			// changed flag each time.
			ಆದರೆ (subflags.do_ask)
			    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(lnum, 0);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (ಮೊದಲ_ಸಾಲು == 0)
				ಮೊದಲ_ಸಾಲು = lnum;
			    ಕೊನೆಯ_ಸಾಲು = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline.string);    // free the temp buffer
			sub_firstline.string = new_ಆರಂಭ.string;
			sub_firstline.ಉದ್ದಳತೆ = new_ಆರಂಭ.ಉದ್ದಳತೆ;
			new_ಆರಂಭ.string = ಖಾಲಿ;
			new_ಆರಂಭ.ಉದ್ದಳತೆ = 0;
			matchcol = (colnr_T)(sub_firstline.ಉದ್ದಳತೆ - matchcol);
			prev_matchcol = (colnr_T)(sub_firstline.ಉದ್ದಳತೆ
							      - prev_matchcol);
			copycol = 0;
			did_ವಿಭಜಿಸು = false;
		    }
		    ಆದರೆ (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, curwin, curbuf,
					  sub_firstlnum, matchcol, ಖಾಲಿ);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    ಆದರೆ (nmatch <= 0)
		    {
			// If the match found didn't start where we were
			// searching, do the next search in the line where we
			// found the match.
			ಆದರೆ (nmatch == -1)
			    lnum -= regmatch.startpos[0].lnum;
			ನಿಲ್ಲಿಸು;
		    }
		}

		ಸಾಲು_breakcheck();
	    }

	    ಆದರೆ (did_sub)
		++sub_nlines;
	    vim_free(new_ಆರಂಭ.string);	// for when substitute was cancelled
	    VIM_CLEAR_STRING(sub_firstline);	// free the copy of the original line
	}

	ಸಾಲು_breakcheck();
    }

    ಆದರೆ (ಮೊದಲ_ಸಾಲು != 0)
    {
	// Need to subtract the number of added lines from "last_line" to get
	// the line number before the change (same as adding the number of
	// deleted lines).
	i = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_ಸಾಲು_ಎಣಿಕೆ;
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ಮೊದಲ_ಸಾಲು, 0, ಕೊನೆಯ_ಸಾಲು - i, i);
    }

outofmem:
    vim_free(sub_firstline.string); // may have to free allocated copy of the line

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    vim_free(ಪಠ್ಯ_props);
#ಆದರೆ_ಅಂತ್ಯ

    // ":s/pat//n" doesn't move the cursor
    ಆದರೆ (subflags.do_ಎಣಿಕೆ)
	curwin->w_cursor = old_cursor;

    ಆದರೆ (sub_nsubs > ಆರಂಭ_nsubs)
    {
	ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ and '] marks.
	    curbuf->b_op_ಆರಂಭ.lnum = eap->line1;
	    curbuf->b_op_ಕೊನೆ.lnum = line2;
	    curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;
	}

	ಆದರೆ (!global_busy)
	{
	    // when interactive leave cursor on the match
	    ಆದರೆ (!subflags.do_ask)
	    {
		ಆದರೆ (endcolumn)
		    coladvance((colnr_T)MAXCOL);
		ಇಲ್ಲದಿದ್ದರೆ
		    beginline(BL_WHITE | BL_FIX);
	    }
	    ಆದರೆ (!do_sub_ಸಂದೇಶ(subflags.do_ಎಣಿಕೆ) && subflags.do_ask)
		ಸಂದೇಶ("");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    global_need_beginline = TRUE;
	ಆದರೆ (subflags.do_print)
	    print_ಸಾಲು(curwin->w_cursor.lnum,
					 subflags.do_ಸಂಖ್ಯೆ, subflags.do_list);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!global_busy)
    {
	ಆದರೆ (got_int)		// interrupted
	    emsg(_(e_interrupted));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (got_ಹೊಂದಿಕೆ)	// did find something but nothing substituted
	    ಸಂದೇಶ("");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (subflags.do_error)	// nothing found
	    semsg(_(e_ನಮೂನೆ_not_ಸಿಕ್ಕಿತು_str), get_ಹುಡುಕು_pat());
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (subflags.do_ask && hasAnyFolding(curwin))
	// Cursor position may require updating
	ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
do_sub_ಸಂದೇಶ(
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_only)		// used 'n' flag for ":s"
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    ಆದರೆ (((sub_nsubs > p_report && (ಕೀಲಿ_Typed || sub_nlines > 1 || p_report < 1))
		|| ಎಣಿಕೆ_only)
	    && messaging())
    {
	ಅಕ್ಷರ	*ಸಂದೇಶ_single;
	ಅಕ್ಷರ	*ಸಂದೇಶ_plural;

	ಆದರೆ (got_int)
	    STRCPY(ಸಂದೇಶ_buf, _("(Interrupted) "));
	ಇಲ್ಲದಿದ್ದರೆ
	    *ಸಂದೇಶ_buf = NUL;

	ಸಂದೇಶ_single = ಎಣಿಕೆ_only
		    ? NGETTEXT("%ld match on %ld line",
					  "%ld matches on %ld line", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld line",
				   "%ld substitutions on %ld line", sub_nsubs);
	ಸಂದೇಶ_plural = ಎಣಿಕೆ_only
		    ? NGETTEXT("%ld match on %ld lines",
					 "%ld matches on %ld lines", sub_nsubs)
		    : NGETTEXT("%ld substitution on %ld lines",
				  "%ld substitutions on %ld lines", sub_nsubs);

	vim_snprintf_ಕೂಡಿಸು(ಸಂದೇಶ_buf, ಗಾತ್ರ(ಸಂದೇಶ_buf),
				 NGETTEXT(ಸಂದೇಶ_single, ಸಂದೇಶ_plural, sub_nlines),
				 sub_nsubs, (ದೀರ್ಘ)sub_nlines);

	ಆದರೆ (ಸಂದೇಶ(ಸಂದೇಶ_buf))
	    // save message to display it after redraw
	    ಗಣ_keep_ಸಂದೇಶ((char_u *)ಸಂದೇಶ_buf, 0);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಆದರೆ (got_int)
    {
	emsg(_(e_interrupted));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
global_exe_one(char_u *cmd, linenr_T lnum)
{
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    ಆದರೆ (*cmd == NUL || *cmd == '\n')
	do_cmdline((char_u *)"p", ಖಾಲಿ, ಖಾಲಿ, DOCMD_NOWAIT);
    ಇಲ್ಲದಿದ್ದರೆ
	do_cmdline(cmd, ಖಾಲಿ, ಖಾಲಿ, DOCMD_NOWAIT);
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
    ಶೂನ್ಯ
ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    ಪೂರ್ಣಾಂಕ		ndone = 0;
    ಪೂರ್ಣಾಂಕ		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    size_t	patlen;
    char_u	*ಬಳಸಿದ_pat;
    regmmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    ಆದರೆ (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    ಇಲ್ಲದಿದ್ದರೆ
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_ಕೊನೆಯ;	    // default: use last used regexp

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ಒಳ_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    ಆದರೆ (*cmd == '\\')
    {
	++cmd;
	ಆದರೆ (vim_strchr((char_u *)"/?&", *cmd) == ಖಾಲಿ)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	ಇಲ್ಲದಿದ್ದರೆ
	    which_pat = RE_ಹುಡುಕು;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
	patlen = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == NUL)
    {
	emsg(_(e_regular_ಅಭಿವ್ಯಕ್ತಿ_missing_from_global));
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_regexp_delim(*cmd) == FAIL)
    {
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
	patlen = STRLEN(pat);
    }

    ಆದರೆ (ಹುಡುಕು_regcomp(pat, patlen, &ಬಳಸಿದ_pat, RE_BOTH, which_pat, ಹುಡುಕು_HIS,
							    &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	ಹೊಂದಿಕೆ = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ಖಾಲಿ);
	ಆದರೆ ((type == 'g' && ಹೊಂದಿಕೆ) || (type == 'v' && !ಹೊಂದಿಕೆ))
	    global_exe_one(cmd, lnum);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	ಗಾಗಿ (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    ಹೊಂದಿಕೆ = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, ಖಾಲಿ);
	    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;  // re-compiling regprog failed
	    ಆದರೆ ((type == 'g' && ಹೊಂದಿಕೆ) || (type == 'v' && !ಹೊಂದಿಕೆ))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    ಸಾಲು_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	ಆದರೆ (got_int)
	    ಸಂದೇಶ(_(e_interrupted));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ndone == 0)
	{
	    ಆದರೆ (type == 'v')
	    {
		ಆದರೆ (ಒಳ_vim9script())
		    semsg(_(e_ನಮೂನೆ_ಸಿಕ್ಕಿತು_ಒಳ_every_ಸಾಲು_str), ಬಳಸಿದ_pat);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg(_("Pattern found in every line: %s"), ಬಳಸಿದ_pat);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಒಳ_vim9script())
		    semsg(_(e_ನಮೂನೆ_not_ಸಿಕ್ಕಿತು_str), ಬಳಸಿದ_pat);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg(_("Pattern not found: %s"), ಬಳಸಿದ_pat);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	    inc_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	    ಆರಂಭ_global_changes();
#ಆದರೆ_ಅಂತ್ಯ
	    global_exe(cmd);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	    ಕೊನೆ_global_changes();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	    dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Execute "cmd" on lines marked with ml_setmarked().
 */
    ಶೂನ್ಯ
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
    ಸಂದೇಶ_didout = TRUE;

    sub_nsubs = 0;
    sub_nlines = 0;
    global_need_beginline = FALSE;
    global_busy = 1;
    old_lcount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆಗ (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
    {
	global_exe_one(cmd, lnum);
	ui_breakcheck();
    }

    global_busy = 0;
    ಆದರೆ (global_need_beginline)
	beginline(BL_WHITE | BL_FIX);
    ಇಲ್ಲದಿದ್ದರೆ
	check_cursor();	// cursor may be beyond the end of the line

    // the cursor may not have moved in the text but a change in a previous
    // line may move it on the screen
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();

    // If it looks like no message was written, allow overwriting the
    // command with the report for number of changes.
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು == 0 && ಸಂದೇಶ_scrolled == 0)
	ಸಂದೇಶ_didout = FALSE;

    // If substitutes done, report number of substitutes, otherwise report
    // number of extra or deleted lines.
    // Don't report extra or deleted lines in the edge case where the buffer
    // we are in after execution is different from the buffer we started in.
    ಆದರೆ (!do_sub_ಸಂದೇಶ(FALSE) && curbuf == old_buf)
	msgmore(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_lcount);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
/*
 * Get the previous substitute pattern.
 */
    char_u *
get_old_sub(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ old_sub;
}

/*
 * Set the previous substitute pattern.  "val" must be allocated.
 */
    ಶೂನ್ಯ
ಗಣ_old_sub(char_u *val)
{
    vim_free(old_sub);
    old_sub = val;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_VIMINFO

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_old_sub(ಶೂನ್ಯ)
{
    vim_free(old_sub);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * Set up for a tagpreview.
 * Makes the preview window the current window.
 * Return TRUE when it was created.
 */
    ಪೂರ್ಣಾಂಕ
prepare_tagpreview(
    ಪೂರ್ಣಾಂಕ		undo_sync,	    // sync undo when leaving the window
    ಪೂರ್ಣಾಂಕ		use_previewpopup,   // use popup if 'previewpopup' set
    use_popup_T	use_popup)	    // use other popup window
{
    win_T	*wp;

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    ಆದರೆ (curwin->w_p_pvw)
	ಹಿಂತಿರುಗಿ FALSE;

    /*
     * If there is already a preview window open, use that one.
     */
# ifdef FEAT_PROP_POPUP
    ಆದರೆ (use_previewpopup && *p_pvp != NUL)
    {
	wp = popup_ಕಂಡುಹಿಡಿ_preview_ಕಿಟಕಿ();
	ಆದರೆ (wp != ಖಾಲಿ)
	    popup_ಗಣ_wantpos_cursor(wp, wp->w_minwidth, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_popup != USEPOPUP_NONE)
    {
	wp = popup_ಕಂಡುಹಿಡಿ_info_ಕಿಟಕಿ();
	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    ಆದರೆ (use_popup == USEPOPUP_ಸಾಮಾನ್ಯ)
		popup_show(wp);
	    ಇಲ್ಲದಿದ್ದರೆ
		popup_hide(wp);
	    // When the popup moves or resizes it may reveal part of
	    // another window.  TODO: can this be done more efficiently?
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_p_pvw)
		ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	win_enter(wp, undo_sync);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    /*
     * There is no preview window open yet.  Create one.
     */
# ifdef FEAT_PROP_POPUP
    ಆದರೆ ((use_previewpopup && *p_pvp != NUL)
	    || use_popup != USEPOPUP_NONE)
	ಹಿಂತಿರುಗಿ popup_create_preview_ಕಿಟಕಿ(use_popup != USEPOPUP_NONE);
# endif
    ಆದರೆ (win_ವಿಭಜಿಸು(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;
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
    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Make the user happy.
 */
    ಶೂನ್ಯ
ex_smile(exarg_T *eap ಬಳಸದ)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *ಸಂಕೇತ_2[] = {
	"\34 \4o\14$\4ox\30 \2o\30$\1ox\25 \2o\36$\1o\11 \1o\1$\3 \2$\1 \1o\1$x\5 \1o\1 \1$\1 \2o\10 \1o\44$\1o\7 \2$\1 \2$\1 \2$\1o\1$x\2 \2o\1 \1$\1 \1$\1 \1\"\1$\6 \1o\11$\4 \15$\4 \11$\1o\7 \3$\1o\2$\1o\1$x\2 \1\"\6$\1o\1$\5 \1o\11$\6 \13$\6 \12$\1o\4 \10$x\4 \7$\4 \13$\6 \13$\6 \27$x\4 \27$\4 \15$\4 \16$\2 \3\"\3$x\5 \1\"\3$\4\"\61$\5 \1\"\3$x\6 \3$\3 \1o\62$\5 \1\"\3$\1ox\5 \1o\2$\1\"\3 \63$\7 \3$\1ox\5 \3$\4 \55$\1\"\1 \1\"\6$",
	"\5o\4$\1ox\4 \1o\3$\4o\5$\2 \45$\3 \1o\21$x\4 \10$\1\"\4$\3 \42$\5 \4$\10\"x\3 \4\"\7 \4$\4 \1\"\34$\1\"\6 \1o\3$x\16 \1\"\3$\1o\5 \3\"\22$\1\"\2$\1\"\11 \3$x\20 \3$\1o\12 \1\"\2$\2\"\6$\4\"\13 \1o\3$x\21 \4$\1o\40 \1o\3$\1\"x\22 \1\"\4$\1o\6 \1o\6$\1o\1\"\4$\1o\10 \1o\4$x\24 \1\"\5$\2o\5 \2\"\4$\1o\5$\1o\3 \1o\4$\2\"x\27 \2\"\5$\4o\2 \1\"\3$\1o\11$\3\"x\32 \2\"\7$\2o\1 \12$x\42 \4\"\13$x\46 \14$x\47 \12$\1\"x\50 \1\"\3$\4\"x"
    };
    ಅಕ್ಷರ *p;
    ಪೂರ್ಣಾಂಕ n;
    ಪೂರ್ಣಾಂಕ i;

    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_putchar('\n');
    ಗಾಗಿ (i = 0; i < 2; ++i)
	ಗಾಗಿ (p = ಸಂಕೇತ_2[i]; *p != NUL; ++p)
	    ಆದರೆ (*p == 'x')
		ಸಂದೇಶ_putchar('\n');
	    ಇಲ್ಲದಿದ್ದರೆ
		ಗಾಗಿ (n = *p++; n > 0; --n)
		    ಆದರೆ (*p == 'o' || *p == '$')
			ಸಂದೇಶ_putchar_attr(*p, HL_ATTR(HLF_L));
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸಂದೇಶ_putchar(*p);
    ಸಂದೇಶ_clr_eos();
}

/*
 * ":drop"
 * Opens the first argument in a window, and the argument list is redefined.
 */
    ಶೂನ್ಯ
ex_drop(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು = FALSE;
    win_T	*wp;
    buf_T	*buf;
    tabpage_T	*tp;

    ಆದರೆ (ERROR_IF_POPUP_ಕಿಟಕಿ || ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    /*
     * Check if the first argument is already being edited in a window.  If
     * so, jump to that window.
     * We would actually need to check all arguments, but that's complicated
     * and mostly only one file is dropped.
     * This also ignores wildcards, since it is very unlikely the user is
     * editing a file name with a wildcard character.
     */
    ಗಣ_arglist(eap->arg);

    /*
     * Expanding wildcards may result in an empty argument list.  E.g. when
     * editing "foo.pyc" and ".pyc" is in 'wildignore'.  Assume that we
     * already did an error message for this.
     */
    ಆದರೆ (ARGCOUNT == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (cmdmod.cmod_tab)
    {
	// ":tab drop file ...": open a tab for each argument that isn't
	// edited in a window yet.  It's like ":tab all" but without closing
	// windows or tabs.
	ex_all(eap);
	cmdmod.cmod_tab = 0;
	ex_rewind(eap);
	ಹಿಂತಿರುಗಿ;
    }

    // ":drop file ...": Edit the first argument.  Jump to an existing
    // window if possible, edit in current window if the current buffer
    // can be abandoned, otherwise open a new window.
    buf = buflist_findnr(ARGLIST[0].ae_fnum);

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	{
	    goto_tabpage_win(tp, wp);
	    curwin->w_arg_idx = 0;
	    ಆದರೆ (!buf_Is_ಬದಲಾಯಿತು(curbuf))
	    {
		ಪೂರ್ಣಾಂಕ save_ar = curbuf->b_p_ar;

		// reload the file if it is newer
		curbuf->b_p_ar = TRUE;
		buf_check_timestamp(curbuf, FALSE);
		curbuf->b_p_ar = save_ar;
	    }
	    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
		ex_rewind(eap);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    /*
     * Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * Skip the check if the 'hidden' option is set, as in this case the
     * buffer won't be lost.
     */
    ಆದರೆ (!buf_hide(curbuf))
    {
	++emsg_off;
	ವಿಭಜಿಸು = check_ಬದಲಾಯಿತು(curbuf, CCGD_AW | CCGD_EXCMD);
	--emsg_off;
    }

    // Fake a ":sfirst" or ":first" command edit the first argument.
    ಆದರೆ (ವಿಭಜಿಸು)
    {
	eap->cmdidx = CMD_sfirst;
	eap->cmd[0] = 's';
    }
    ಇಲ್ಲದಿದ್ದರೆ
	eap->cmdidx = CMD_ಮೊದಲ;
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
skip_vimgrep_pat(char_u *p, char_u **s, ಪೂರ್ಣಾಂಕ *flags)
{
    ಹಿಂತಿರುಗಿ skip_vimgrep_pat_ext(p, s, flags, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * As skip_vimgrep_pat() and store the character overwritten by NUL in "cp"
 * and the pointer to it in "nulp".
 */
    char_u *
skip_vimgrep_pat_ext(char_u *p, char_u **s, ಪೂರ್ಣಾಂಕ *flags, char_u **nulp, ಪೂರ್ಣಾಂಕ *cp)
{
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (vim_isIDc(*p))
    {
	// ":vimgrep pattern fname"
	ಆದರೆ (s != ಖಾಲಿ)
	    *s = p;
	p = skiptowhite(p);
	ಆದರೆ (s != ಖಾಲಿ && *p != NUL)
	{
	    ಆದರೆ (nulp != ಖಾಲಿ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p++ = NUL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ":vimgrep /pattern/[g][j] fname"
	ಆದರೆ (s != ಖಾಲಿ)
	    *s = p + 1;
	c = *p;
	p = skip_regexp(p + 1, c, TRUE);
	ಆದರೆ (*p != c)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// Truncate the pattern.
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    ಆದರೆ (nulp != ಖಾಲಿ)
	    {
		*nulp = p;
		*cp = *p;
	    }
	    *p = NUL;
	}
	++p;

	// Find the flags
	ಆಗ (*p == 'g' || *p == 'j' || *p == 'f')
	{
	    ಆದರೆ (flags != ಖಾಲಿ)
	    {
		ಆದರೆ (*p == 'g')
		    *flags |= VGR_GLOBAL;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'j')
		    *flags |= VGR_NOJUMP;
		ಇಲ್ಲದಿದ್ದರೆ
		    *flags |= VGR_FUZZY;
	    }
	    ++p;
	}
    }
    ಹಿಂತಿರುಗಿ p;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * List v:oldfiles in a nice way.
 */
    ಶೂನ್ಯ
ex_oldfiles(exarg_T *eap ಬಳಸದ)
{
    list_T	*l = get_vim_var_list(VV_OLDFILES);
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		nr = 0;
    char_u	*fname;
    // for a single filtered match, remember the number
    // so we can jump directly to it without prompting
    ಪೂರ್ಣಾಂಕ		matches = -1;

    ಆದರೆ (l == ಖಾಲಿ)
    {
	ಸಂದೇಶ(_("No old files"));
	ಹಿಂತಿರುಗಿ;
    }

    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_scroll = TRUE;
    ಗಾಗಿ (li = l->lv_ಮೊದಲ; li != ಖಾಲಿ && !got_int; li = li->li_ಮುಂದಿನ)
    {
	++nr;
	fname = tv_get_string(&li->li_tv);
	ಆದರೆ (!ಸಂದೇಶ_filtered(fname))
	{
	    ಆದರೆ (matches < 0)
		matches = nr;
	    ಇಲ್ಲದಿದ್ದರೆ
		matches = 0;
	    ಸಂದೇಶ_outnum((ದೀರ್ಘ)nr);
	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_outtrans(fname);
	    ಸಂದೇಶ_clr_eos();
	    ಸಂದೇಶ_putchar('\n');
	    ಹೊರಹೋಗು_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }

    // Assume "got_int" was set to truncate the listing.
    got_int = FALSE;

# ifdef FEAT_BROWSE_CMD
    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	quit_ಇನ್ನಷ್ಟು = FALSE;
	// we only need to prompt if there is more than 1 match
	ಆದರೆ (matches > 0)
	{
	    nr = matches;
	    // msg_putchar above sets needs_wait_return
	    need_wait_return = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    nr = prompt_for_ಸಂಖ್ಯೆ(FALSE);
	ಸಂದೇಶ_starthere();
	ಆದರೆ (nr > 0)
	{
	    char_u *p = list_ಕಂಡುಹಿಡಿ_str(get_vim_var_list(VV_OLDFILES),
		    (ದೀರ್ಘ)nr);

	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		p = expand_env_save(p);
		eap->arg = p;
		eap->cmdidx = CMD_edit;
		cmdmod.cmod_flags &= ~CMOD_BROWSE;
		do_exedit(eap, ಖಾಲಿ);
		vim_free(p);
	    }
	}
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ
