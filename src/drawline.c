/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * drawline.c: Functions for drawing window lines on the screen.
 * This is the middle level, drawscreen.c is the higher level and screen.c the
 * lower level.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    ਸਥਿਰ ਪੂਰਨ
get_lcs_ext(win_T *wp)
{
    ਜੇ (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	ਵਾਪਸ NUL;
    ਜੇ (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	ਵਾਪਸ '>';
    ਵਾਪਸ wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    ਸਥਿਰ ਪੂਰਨ
advance_color_ਖਾਨਾ(ਪੂਰਨ vcol, ਪੂਰਨ **color_ਖਾਨੇ_ਗਿਣਤੀ)
{
    ਜਦੋਂ (**color_ਖਾਨੇ_ਗਿਣਤੀ >= 0 && vcol > **color_ਖਾਨੇ_ਗਿਣਤੀ)
	++*color_ਖਾਨੇ_ਗਿਣਤੀ;
    ਵਾਪਸ (**color_ਖਾਨੇ_ਗਿਣਤੀ >= 0);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    ਸਥਿਰ ਖਾਲੀ
margin_columns_win(win_T *wp, ਪੂਰਨ *ਖੱਬਾ_ਖਾਨਾ, ਪੂਰਨ *ਸੱਜਾ_ਖਾਨਾ)
{
    // cache previous calculations depending on w_virtcol
    ਸਥਿਰ ਪੂਰਨ saved_w_virtcol;
    ਸਥਿਰ win_T *prev_wp;
    ਸਥਿਰ ਪੂਰਨ prev_width1;
    ਸਥਿਰ ਪੂਰਨ prev_width2;
    ਸਥਿਰ ਪੂਰਨ prev_ਖੱਬਾ_ਖਾਨਾ;
    ਸਥਿਰ ਪੂਰਨ prev_ਸੱਜਾ_ਖਾਨਾ;

    ਪੂਰਨ cur_ਖਾਨਾ_off = win_ਖਾਨਾ_off(wp);
    ਪੂਰਨ	width1;
    ਪੂਰਨ	width2;

    width1 = wp->w_ਚੌੜਾਈ - cur_ਖਾਨਾ_off;
    width2 = width1 + win_ਖਾਨਾ_off2(wp);

    ਜੇ (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*ਸੱਜਾ_ਖਾਨਾ = prev_ਸੱਜਾ_ਖਾਨਾ;
	*ਖੱਬਾ_ਖਾਨਾ = prev_ਖੱਬਾ_ਖਾਨਾ;
	ਵਾਪਸ;
    }

    *ਖੱਬਾ_ਖਾਨਾ = 0;
    *ਸੱਜਾ_ਖਾਨਾ = width1;

    ਜੇ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ਸੱਜਾ_ਖਾਨਾ = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    ਜੇ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ਖੱਬਾ_ਖਾਨਾ = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ਖੱਬਾ_ਖਾਨਾ = *ਖੱਬਾ_ਖਾਨਾ;
    prev_ਸੱਜਾ_ਖਾਨਾ = *ਸੱਜਾ_ਖਾਨਾ;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define ਸਤਰ_ATTR
#ਜੇ_ਅੰਤ

// structure with variables passed between win_line() and other functions
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    ਪੂਰਨ		startrow;	// first row in the window to be drawn
    ਪੂਰਨ		ਪੰਕਤੀ;		// row in the window, excl w_winrow
    ਪੂਰਨ		screen_ਪੰਕਤੀ;	// row on the screen, incl w_winrow

    ਲੰਮਾ	vcol;		// virtual column, before wrapping
    ਪੂਰਨ		ਖਾਨਾ;		// visual column on screen, after wrapping
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    ਪੂਰਨ		vcol_off_co;	// offset for concealed characters
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		vcol_off_tp;	// offset for virtual text
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		draw_color_ਖਾਨਾ;	// highlight colorcolumn
    ਪੂਰਨ		*color_ਖਾਨੇ_ਗਿਣਤੀ;	// pointer to according columns array
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		eol_hl_off;	// 1 if highlighted char after EOL

    ਬਿਨਾਨਿਸ਼ਾਨੀ	off;		// offset in ScreenLines/ScreenAttrs

    ਪੂਰਨ		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		cul_attr;	// set when 'cursorline' active
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸਤਰ_ATTR
    ਪੂਰਨ		ਸਤਰ_attr;	// for the whole line, includes 'cursorline'
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		screen_ਸਤਰ_flags;  // flags for screen_line()

    ਪੂਰਨ		fromcol;	// start of inverting
    ਪੂਰਨ		tocol;		// end of inverting

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਲੰਮਾ	vcol_sbr;	    // virtual column after showbreak
    ਪੂਰਨ		need_showbreak;	    // overlong line, skipping first x chars
    ਪੂਰਨ		dont_use_showbreak; // do not use 'showbreak'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਪੂਰਨ		ਪਾਠ_prop_above_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    ਪੂਰਨ		cul_screenline;

    ਪੂਰਨ		char_attr;	// attributes for the next character

    ਪੂਰਨ		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    ਪੂਰਨ		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    ਪੂਰਨ		n_attr_skip;    // chars to skip before using extra_attr
    ਪੂਰਨ		c_extra;	// extra chars, all the same
    ਪੂਰਨ		c_final;	// final char, mandatory if set
    ਪੂਰਨ		extra_for_textprop; // n_extra set for textprop
    ਪੂਰਨ		ਸ਼ੁਰੂ_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    ਪੂਰਨ		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    ਪੂਰਨ		saved_extra_attr;
    ਪੂਰਨ		saved_n_attr_skip;
    ਪੂਰਨ		saved_extra_for_textprop;
    ਪੂਰਨ		saved_c_extra;
    ਪੂਰਨ		saved_c_final;
    ਪੂਰਨ		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		filler_ਸਤਰਾਂ;	// nr of filler lines to be drawn
    ਪੂਰਨ		filler_todo;	// nr of filler lines still to do + 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    sign_attrs_T sattr;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    ਪੂਰਨ		need_lbr;
#ਜੇ_ਅੰਤ
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WL_ਸ਼ੁਰੂ	0		// nothing done yet, must be zero
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WL_CMDLINE	(WL_ਸ਼ੁਰੂ + 1)	// cmdline window column
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#ਨਹੀਂਤਾਂ
# define WL_FOLD	WL_CMDLINE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#ਨਹੀਂਤਾਂ
# define WL_SIGN	WL_FOLD		// column for signs
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WL_NR		(WL_SIGN + 1)	// line number
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#ਨਹੀਂਤਾਂ
# define WL_BRI		WL_NR
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#ਨਹੀਂਤਾਂ
# define WL_SBR		WL_BRI
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WL_ਸਤਰ		(WL_SBR + 1)	// text in the line

#ਜੇ defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    ਸਥਿਰ ਪੂਰਨ
use_cursor_ਸਤਰ_highlight(win_T *wp, linenr_T lnum)
{
    ਵਾਪਸ wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    ਪੂਰਨ fdc = compute_foldcolumn(wp, 0);

    ਜੇ (fdc <= 0)
	ਵਾਪਸ;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    ਜੇ (wlv->p_extra_free == ਨਿਰਾ)
	ਵਾਪਸ;

    wlv->n_extra = (ਪੂਰਨ)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    ਜੇ (use_cursor_ਸਤਰ_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    ਨਹੀਂਤਾਂ
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_FC));
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
/*
 * Get information needed to display the sign in line "wlv->lnum" in window
 * "wp".
 * If "nrcol" is TRUE, the sign is going to be displayed in the number column.
 * Otherwise the sign is going to be displayed in the sign column.
 */
    ਸਥਿਰ ਖਾਲੀ
get_sign_display_info(
	ਪੂਰਨ		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    ਪੂਰਨ	ਪਾਠ_sign;
# ifdef FEAT_SIGN_ICONS
    ਪੂਰਨ	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    ਜੇ (nrcol)
	wlv->n_extra = ਸੰਖਿਆ_ਚੌੜਾਈ(wp) + 1;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (use_cursor_ਸਤਰ_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	ਨਹੀਂਤਾਂ
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    ਜੇ (wlv->ਪੰਕਤੀ == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_ਸਤਰਾਂ && wlv->filler_todo <= 0
# endif
       )
    {
	ਪਾਠ_sign = (wlv->sattr.sat_ਪਾਠ != ਨਿਰਾ) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ਨਿਰਾ) ? wlv->sattr.sat_typenr : 0;
	ਜੇ (gui.ਵਿੱਚ_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    ਜੇ (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((ਅੱਖਰ *)wlv->extra, ਆਕਾਰ(wlv->extra),
						"%-*c ", ਸੰਖਿਆ_ਚੌੜਾਈ(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    ਨਹੀਂਤਾਂ
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    ਜੇ (netbeans_ਸਰਗਰਮ() && (buf_signcount(wp->w_ਬਫਰ, wlv->lnum)
									  > 1))
	    {
		ਜੇ (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((ਅੱਖਰ *)wlv->extra, ਆਕਾਰ(wlv->extra),
						"%-*c ", ਸੰਖਿਆ_ਚੌੜਾਈ(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		ਨਹੀਂਤਾਂ
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	ਨਹੀਂਤਾਂ
# endif
	    ਜੇ (ਪਾਠ_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_ਪਾਠ;
		ਜੇ (wlv->p_extra != ਨਿਰਾ)
		{
		    ਜੇ (nrcol)
		    {
			ਪੂਰਨ ਚੌੜਾਈ = ਸੰਖਿਆ_ਚੌੜਾਈ(wp) - 2;

			vim_memset(wlv->extra, ' ', ਚੌੜਾਈ);
			wlv->n_extra = ਚੌੜਾਈ;
			wlv->n_extra += vim_snprintf((ਅੱਖਰ *)wlv->extra + ਚੌੜਾਈ,
				  ਆਕਾਰ(wlv->extra) - ਚੌੜਾਈ, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    ਨਹੀਂਤਾਂ
			wlv->n_extra = (ਪੂਰਨ)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		ਜੇ (use_cursor_ਸਤਰ_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		ਨਹੀਂਤਾਂ
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#ਜੇ_ਅੰਤ

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_lnum_ਖਾਨਾ(
	win_T		*wp,
	winlinevars_T	*wlv,
	ਪੂਰਨ		sign_present ਅਣਵਰਤਿਆ,
	ਪੂਰਨ		ਅੰਕ_attr ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ਨਿਰਾ;
    ਪੂਰਨ lnum_ਪੰਕਤੀ = wlv->startrow + wlv->filler_ਸਤਰਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		      + wlv->ਪਾਠ_prop_above_ਗਿਣਤੀ
#ਜੇ_ਅੰਤ
		      ;

    ਜੇ ((wp->w_p_nu || wp->w_p_rnu)
	     && (wlv->ਪੰਕਤੀ <= lnum_ਪੰਕਤੀ || !has_cpo_n)
	     // there is no line number in a wrapped line when "n" is in
	     // 'cpoptions', but 'breakindent' assumes it anyway.
	     && !((has_cpo_n
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		     && !wp->w_p_bri
#ਜੇ_ਅੰਤ
		  ) && wp->w_skipcol > 0 && wlv->lnum == wp->w_topline))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	// If 'signcolumn' is set to 'number' and a sign is present
	// in 'lnum', then display the sign instead of the line
	// number.
	ਜੇ ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_ਪਾਠ != ਨਿਰਾ)
	    get_sign_display_info(TRUE, wp, wlv);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  ਜੇ (wlv->ਪੰਕਤੀ == lnum_ਪੰਕਤੀ
		    && (wp->w_skipcol == 0 || wlv->ਪੰਕਤੀ > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      ਲੰਮਾ ਅੰਕ;
	      ਅੱਖਰ *fmt = "%*ld ";

	      ਜੇ (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  ਅੰਕ = (ਲੰਮਾ)wlv->lnum;
	      ਨਹੀਂਤਾਂ
	      {
		  // 'relativenumber', don't use negative numbers
		  ਅੰਕ = labs((ਲੰਮਾ)get_cursor_rel_lnum(wp, wlv->lnum));
		  ਜੇ (ਅੰਕ == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      ਅੰਕ = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((ਅੱਖਰ *)wlv->extra, ਆਕਾਰ(wlv->extra), fmt, ਸੰਖਿਆ_ਚੌੜਾਈ(wp), ਅੰਕ);
	      ਜੇ (wp->w_skipcol > 0 && wlv->startrow == 0)
		  ਲਈ (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	      ਜੇ (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  ਪੂਰਨ	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  ਲਈ (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
		  {
		      t = *p1;
		      *p1 = *p2;
		      *p2 = t;
		  }
	      }
#ਜੇ_ਅੰਤ
	      wlv->p_extra = wlv->extra;
	      wlv->c_extra = NUL;
	      wlv->c_final = NUL;
	  }
	  ਨਹੀਂਤਾਂ
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = ਸੰਖਿਆ_ਚੌੜਾਈ(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  ਜੇ (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->ਪੰਕਤੀ == lnum_ਪੰਕਤੀ
		      || (wlv->ਪੰਕਤੀ > lnum_ਪੰਕਤੀ
			 && (wp->w_p_culopt_flags & CULOPT_ਸਤਰ))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#ਜੇ_ਅੰਤ
	  ਜੇ (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  ਜੇ (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	ਜੇ (ਅੰਕ_attr)
	    wlv->char_attr = ਅੰਕ_attr;
#ਜੇ_ਅੰਤ
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਸਥਿਰ ਖਾਲੀ
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    ਜੇ (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_ਮੁੱਲ(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    ਜੇ (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	ਜੇ (wp->w_p_bri && (wlv->ਪੰਕਤੀ > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_ਸਤਰਾਂ
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    ਜੇ (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ਨਿਰਾ;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_ਬਫਰ, wlv->lnum, FALSE));
	    ਜੇ (wlv->ਪੰਕਤੀ == wlv->startrow)
	    {
		wlv->n_extra -= win_ਖਾਨਾ_off2(wp);
		ਜੇ (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    ਜੇ (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    ਜੇ (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	ਜੇ (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    ਸਥਿਰ ਖਾਲੀ
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    ਜੇ (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	ਜੇ (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	ਨਹੀਂਤਾਂ
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	ਜੇ (wp->w_p_rl)
	    wlv->n_extra = wlv->ਖਾਨਾ + 1;
	ਨਹੀਂਤਾਂ
#  endif
	    wlv->n_extra = wp->w_ਚੌੜਾਈ - wlv->ਖਾਨਾ;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_ਮੁੱਲ(wp);
    ਜੇ (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (ਪੂਰਨ)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	ਜੇ (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	ਜੇ (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	ਜੇ (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    ਜੇ (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    ਸਥਿਰ ਪੂਰਨ
textprop_ਮਾਪ_after_trunc(
	win_T	*wp,
	ਪੂਰਨ	flags,	    // TP_FLAG_ALIGN_*
	ਪੂਰਨ	added,
	ਪੂਰਨ	padding,
	char_u	*ਪਾਠ,
	ਪੂਰਨ	*n_ਵਰਤਿਆ_ptr)
{
    ਪੂਰਨ	space = (flags & (TP_FLAG_ALIGN_ਹੇਠਾਂ | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp) : added;
    ਪੂਰਨ strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    ਜੇ (space < PROP_ਪਾਠ_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_ਚੌੜਾਈ;
    ਜੇ (flags & (TP_FLAG_ALIGN_ਹੇਠਾਂ | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    ਲਈ (p = ਪਾਠ; *p != NUL; p += (*mb_ptr2len)(p))
    {
	ਪੂਰਨ clen = ptr2cells(p);

	ਜੇ (strsize + clen > space)
	    ਰੋਕੋ;
	strsize += clen;
    }
    *n_ਵਰਤਿਆ_ptr = (ਪੂਰਨ)(p - ਪਾਠ);

    ਵਾਪਸ strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    ਪੂਰਨ
ਪਾਠ_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	ਪੂਰਨ	    vcol,	    // current text column
	ਪੂਰਨ	    scr_ਖਾਨਾ,	    // current screen column
	ਪੂਰਨ	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	ਪੂਰਨ	    *n_attr,	    // attribute cells, NULL if not used
	ਪੂਰਨ	    *n_attr_skip,   // cells to skip attr, NULL if not used
	ਪੂਰਨ	    do_skip)	    // skip_cells is not zero
{
    ਪੂਰਨ	    ਸੱਜਾ = (tp->tp_flags & TP_FLAG_ALIGN_ਸੱਜਾ);
    ਪੂਰਨ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ਪੂਰਨ	    ਹੇਠਾਂ = (tp->tp_flags & TP_FLAG_ALIGN_ਹੇਠਾਂ);
    ਪੂਰਨ	    wrap = tp->tp_ਖਾਨਾ < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    ਪੂਰਨ	    padding = tp->tp_ਖਾਨਾ == MAXCOL ? tp->tp_padleft : 0;
    ਪੂਰਨ	    ਖਾਨਾ_with_padding = scr_ਖਾਨਾ + (ਹੇਠਾਂ ? 0 : padding);
    ਪੂਰਨ	    ਗੁੰਜਾਇਸ਼ = wp->w_ਚੌੜਾਈ - ਖਾਨਾ_with_padding;
    ਪੂਰਨ	    before = ਗੁੰਜਾਇਸ਼;	// spaces before the text
    ਪੂਰਨ	    after = 0;		// spaces after the text
    ਪੂਰਨ	    n_ਵਰਤਿਆ = *n_extra;
    char_u  *l = ਨਿਰਾ;
    ਪੂਰਨ	    strsize = vim_strsize(*p_extra);
    ਪੂਰਨ	    cells = wrap ? strsize : textprop_ਮਾਪ_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_ਵਰਤਿਆ);

    ਜੇ (wrap || ਸੱਜਾ || above || ਹੇਠਾਂ || padding > 0 || n_ਵਰਤਿਆ < *n_extra)
    {
	ਪੂਰਨ	    ਖਾਨਾ_off = win_ਖਾਨਾ_off(wp) - win_ਖਾਨਾ_off2(wp);

	ਜੇ (above)
	{
	    before = 0;
	    after = wp->w_ਚੌੜਾਈ - cells - win_ਖਾਨਾ_off(wp) - padding;
	    ਜੇ (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Right-align: fill with before
	    ਜੇ (ਸੱਜਾ)
		before -= cells;

	    // Below-align: empty line add one character
	    ਜੇ (ਹੇਠਾਂ && vcol == 0 && ਖਾਨਾ_with_padding == ਖਾਨਾ_off
					    && wp->w_ਚੌੜਾਈ - ਖਾਨਾ_off == before)
		ਖਾਨਾ_with_padding += 1;

	    ਜੇ (before < 0
		    || !(ਸੱਜਾ || ਹੇਠਾਂ)
		    || (ਹੇਠਾਂ ? (ਖਾਨਾ_with_padding <= ਖਾਨਾ_off || !wp->w_p_wrap)
			      : (n_ਵਰਤਿਆ < *n_extra)))
	    {
		ਜੇ (ਸੱਜਾ && (wrap
			      || (ਗੁੰਜਾਇਸ਼ < PROP_ਪਾਠ_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_ਚੌੜਾਈ - ਖਾਨਾ_off - strsize + ਗੁੰਜਾਇਸ਼;
		    ਜੇ (before < 0)
			before = 0;
		    ਨਹੀਂਤਾਂ
			n_ਵਰਤਿਆ = *n_extra;
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਹੇਠਾਂ && before > vcol && do_skip)
		    before -= vcol;
		ਨਹੀਂਤਾਂ
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	ਜੇ (n_ਵਰਤਿਆ < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_ਵਰਤਿਆ;

	// add 1 for NUL, 2 for when '…' is used
	ਜੇ (n_attr != ਨਿਰਾ)
	    l = alloc(n_ਵਰਤਿਆ + before + after + (padding > 0 ? padding : 0) + 3);
	ਜੇ (n_attr == ਨਿਰਾ || l != ਨਿਰਾ)
	{
	    ਪੂਰਨ off = 0;

	    ਜੇ (n_attr != ਨਿਰਾ)
	    {
		vim_memset(l, ' ', before);
		off += before;
		ਜੇ (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_ਵਰਤਿਆ);
		off += n_ਵਰਤਿਆ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		off = before + after + padding + n_ਵਰਤਿਆ;
		cells += before + after + padding;
	    }
	    ਜੇ (n_attr != ਨਿਰਾ)
	    {
		ਜੇ (n_ਵਰਤਿਆ < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    ਜੇ (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			ਜੇ (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_ਤਿਆਰੀ(&vc, (char_u *)"utf-8", p_enc);
			    ਜੇ (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ਨਿਰਾ);
				ਜੇ (cp == ਨਿਰਾ)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_ਤਿਆਰੀ(&vc, ਨਿਰਾ, ਨਿਰਾ);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_ਸਿਰ_off)(l, lp);
			STRCPY(lp, cp);
			n_ਵਰਤਿਆ = lp - l + 3 - before - padding;
			ਜੇ (cp != buf)
			    vim_free(cp);
		    }
		    ਨਹੀਂਤਾਂ
			// change last character to '>'
			*lp = '>';
		}
		ਨਹੀਂਤਾਂ ਜੇ (after > 0)
		{
		    vim_memset(l + off, ' ', after);
		    l[off + after] = NUL;
		}

		*p_extra = l;
		*n_extra = n_ਵਰਤਿਆ + before + after + padding;
		*n_attr = mb_charlen(*p_extra);
		// n_attr_skip will not be decremented before draw_state is
		// WL_LINE
		*n_attr_skip = before + (padding > 0 ? padding : 0);
		*n_attr -= *n_attr_skip;
		ਜੇ (above)
		    *n_attr -= after;
	    }
	}
    }

    ਜੇ (n_attr == ਨਿਰਾ)
	ਵਾਪਸ cells;
    ਵਾਪਸ (ਹੇਠਾਂ && ਖਾਨਾ_with_padding > win_ਖਾਨਾ_off(wp) && !wp->w_p_wrap);
}

# ਜੇ defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    ਪੂਰਨ
ਪਾਠ_prop_no_showbreak(textprop_T *tp)
{
    ਪੂਰਨ	    ਸੱਜਾ = (tp->tp_flags & TP_FLAG_ALIGN_ਸੱਜਾ);
    ਪੂਰਨ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ਪੂਰਨ	    ਹੇਠਾਂ = (tp->tp_flags & TP_FLAG_ALIGN_ਹੇਠਾਂ);
    ਪੂਰਨ	    wrap = tp->tp_ਖਾਨਾ < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    ਵਾਪਸ (ਸੱਜਾ || above || ਹੇਠਾਂ || !wrap);
}
# endif
#ਜੇ_ਅੰਤ

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    ਸਥਿਰ ਖਾਲੀ
wlv_screen_ਸਤਰ(win_T *wp, winlinevars_T *wlv, ਪੂਰਨ clear_ਸਿਰਾ)
{
    ਜੇ (wlv->ਪੰਕਤੀ == 0 && wp->w_skipcol > 0
#ਜੇ defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_ਮੁੱਲ(wp) == NUL
#ਜੇ_ਅੰਤ
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	ਪੂਰਨ off = (ਪੂਰਨ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);
	ਪੂਰਨ max_off = off + screen_Columns;
	ਪੂਰਨ skip = 0;

	ਜੇ (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    ਜਦੋਂ (skip < wp->w_ਚੌੜਾਈ && VIM_ISDIGIT(Screen_ਸਤਰਾਂ[off]))
	    {
		++off;
		++skip;
	    }

	ਲਈ (ਪੂਰਨ i = 0; i < 3 && i + skip < wp->w_ਚੌੜਾਈ; ++i)
	{
	    ਜੇ ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_ਸਤਰਾਂ[off + 1] = ' ';
	    Screen_ਸਤਰਾਂ[off] = '<';
	    ਜੇ (enc_utf8)
		Screen_ਸਤਰਾਂ_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_ਸਤਰ(wp, wlv->screen_ਪੰਕਤੀ, wp->w_wincol, wlv->ਖਾਨਾ,
		    clear_ਸਿਰਾ ? wp->w_ਚੌੜਾਈ : -wp->w_ਚੌੜਾਈ,
		    wlv->vcol - 1, wlv->screen_ਸਤਰ_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_screen_ਸਤਰ(win_T *wp, winlinevars_T *wlv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਲੰਮਾ	v;
    ਪੂਰਨ		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    ਜੇ (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    ਨਹੀਂਤਾਂ
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_ਚੌੜਾਈ - wlv->ਖਾਨਾ - 1 :
# endif
	wlv->ਖਾਨਾ;
    // check if line ends before left margin
    ਜੇ (wlv->vcol < v + wcol - win_ਖਾਨਾ_off(wp))
	wlv->vcol = v + wcol - win_ਖਾਨਾ_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->ਖਾਨਾ -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# ਨਹੀਂਤਾਂ
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    ਜੇ (wlv->draw_color_ਖਾਨਾ)
	wlv->draw_color_ਖਾਨਾ = advance_color_ਖਾਨਾ(VCOL_HLC, &wlv->color_ਖਾਨੇ_ਗਿਣਤੀ);

    ਜੇ (((wp->w_p_cuc
		    && (ਪੂਰਨ)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (ਪੂਰਨ)wp->w_virtcol <
			 (ਲੰਮਾ)wp->w_ਚੌੜਾਈ * (wlv->ਪੰਕਤੀ - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_ਖਾਨਾ
# ifdef ਸਤਰ_ATTR
		|| wlv->ਸਤਰ_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	ਪੂਰਨ	rightmost_vcol = 0;
	ਪੂਰਨ	i;

	ਜੇ (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	ਜੇ (wlv->draw_color_ਖਾਨਾ)
	    // determine rightmost colorcolumn to possibly draw
	    ਲਈ (i = 0; wlv->color_ਖਾਨੇ_ਗਿਣਤੀ[i] >= 0; ++i)
		ਜੇ (rightmost_vcol < wlv->color_ਖਾਨੇ_ਗਿਣਤੀ[i])
		    rightmost_vcol = wlv->color_ਖਾਨੇ_ਗਿਣਤੀ[i];

	ਜਦੋਂ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->ਖਾਨਾ >= 0) :
# endif
		(wlv->ਖਾਨਾ < wp->w_ਚੌੜਾਈ))
	{
	    Screen_ਸਤਰਾਂ[wlv->off] = ' ';
	    ਜੇ (enc_utf8)
		Screen_ਸਤਰਾਂ_UC[wlv->off] = 0;

	    ਜੇ (wlv->draw_color_ਖਾਨਾ)
		wlv->draw_color_ਖਾਨਾ = advance_color_ਖਾਨਾ(
						   VCOL_HLC, &wlv->color_ਖਾਨੇ_ਗਿਣਤੀ);

	    ਪੂਰਨ attr = wlv->win_attr;
# ifdef ਸਤਰ_ATTR
	    ਜੇ (wlv->ਸਤਰ_attr != 0)
		attr = hl_combine_attr(attr, wlv->ਸਤਰ_attr);
# endif
	    ਜੇ (wp->w_p_cuc && VCOL_HLC == (ਲੰਮਾ)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    ਨਹੀਂਤਾਂ ਜੇ (wlv->draw_color_ਖਾਨਾ && VCOL_HLC == *wlv->color_ਖਾਨੇ_ਗਿਣਤੀ)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ
# endif
	    {
		++wlv->off;
		++wlv->ਖਾਨਾ;
	    }
	    ++wlv->vcol;

	    ਜੇ (VCOL_HLC > rightmost_vcol
# ifdef ਸਤਰ_ATTR
		    && wlv->ਸਤਰ_attr == 0
# endif
		    && wlv->win_attr == 0)
		ਰੋਕੋ;
	}
    }
#ਜੇ_ਅੰਤ

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_ਸਤਰ_flags |= SLF_INC_VCOL;
    wlv_screen_ਸਤਰ(wp, wlv, TRUE);
    wlv->screen_ਸਤਰ_flags &= ~SLF_INC_VCOL;
    ++wlv->ਪੰਕਤੀ;
    ++wlv->screen_ਪੰਕਤੀ;
}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    ਸਥਿਰ ਖਾਲੀ
win_ਸਤਰ_ਸ਼ੁਰੂ(win_T *wp ਅਣਵਰਤਿਆ, winlinevars_T *wlv, ਪੂਰਨ save_extra)
{
    wlv->ਖਾਨਾ = 0;
    wlv->off = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->ਖਾਨਾ = wp->w_ਚੌੜਾਈ - 1;
	wlv->off += wlv->ਖਾਨਾ;
	wlv->screen_ਸਤਰ_flags |= SLF_RIGHTLEFT;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_ਸ਼ੁਰੂ;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ਨਿਰਾ;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    wlv->saved_char_attr = 0;

	// these are not used until restored in win_line_continue()
	wlv->n_extra = 0;
	wlv->n_attr_skip = 0;
    }
}

/*
 * Called when wlv->draw_state is set to WL_LINE.
 */
    ਸਥਿਰ ਖਾਲੀ
win_ਸਤਰ_continue(winlinevars_T *wlv)
{
    ਜੇ (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ਨਿਰਾ;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    ਨਹੀਂਤਾਂ
	wlv->char_attr = wlv->win_attr;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਸਥਿਰ ਖਾਲੀ
apply_cursorline_highlight(
	winlinevars_T *wlv,
	ਪੂਰਨ sign_present ਅਣਵਰਤਿਆ)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    ਜੇ (sign_present && wlv->sattr.sat_linehl > 0)
    {
	ਜੇ (wlv->sattr.sat_ਤਰਜੀਹ >= 100)
	    wlv->ਸਤਰ_attr = hl_combine_attr(wlv->cul_attr, wlv->ਸਤਰ_attr);
	ਨਹੀਂਤਾਂ
	    wlv->ਸਤਰ_attr = hl_combine_attr(wlv->ਸਤਰ_attr, wlv->cul_attr);
    }
    ਨਹੀਂਤਾਂ
# endif
# ਜੇ defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->ਸਤਰ_attr = hl_combine_attr(wlv->cul_attr, wlv->ਸਤਰ_attr);
# ਨਹੀਂਤਾਂ
	wlv->ਸਤਰ_attr = wlv->cul_attr;
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Display line "lnum" of window "wp" on the screen.
 * Start at row "startrow", stop when "endrow" is reached.
 * When only updating the number column, "number_only" is set to the height of
 * the line, otherwise it is set to 0.
 * "spv" is used to store information for spell checking, kept between
 * sequential calls for the same window.
 * wp->w_virtcol needs to be valid.
 *
 * Return the number of last row the line occupies.
 */
    ਪੂਰਨ
win_ਸਤਰ(
    win_T	*wp,
    linenr_T	lnum,
    ਪੂਰਨ		startrow,
    ਪੂਰਨ		endrow,
    ਪੂਰਨ		ਸੰਖਿਆ_only,
    spellvars_T	*spv ਅਣਵਰਤਿਆ)
{
    winlinevars_T	wlv;		// variables passed between functions

    ਪੂਰਨ		c = 0;			// init for GCC
    char_u	*ਸਤਰ;			// current line
    char_u	*ptr;			// current position in "line"
    ਪੂਰਨ		ਵਿੱਚ_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ਨਿਰਾ;   // another p_extra to be freed
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    ਪੂਰਨ		ਵਿੱਚ_linebreak = FALSE;	// n_extra set for showing linebreak
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    ਪੂਰਨ		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    ਪੂਰਨ		n_attr = 0;	    // chars with special attr
    ਪੂਰਨ		saved_attr2 = 0;    // char_attr saved for n_attr
    ਪੂਰਨ		n_attr3 = 0;	    // chars with overruling special attr
    ਪੂਰਨ		saved_attr3 = 0;    // char_attr saved for n_attr3

    ਪੂਰਨ		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    ਪੂਰਨ		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    ਪੂਰਨ		lnum_ਵਿੱਚ_visual_area = FALSE;
    pos_T	pos;
    ਲੰਮਾ	v;

    ਪੂਰਨ		attr_pri = FALSE;	// char_attr has priority
    ਪੂਰਨ		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    ਪੂਰਨ		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    ਪੂਰਨ		area_attr = 0;		// attributes desired by highlighting
    ਪੂਰਨ		ਖੋਜ_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    ਪੂਰਨ		syntax_attr = 0;	// attributes desired by syntax
    ਪੂਰਨ		prev_syntax_ਖਾਨਾ = -1;	// column of prev_syntax_attr
    ਪੂਰਨ		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    ਪੂਰਨ		has_syntax = FALSE;	// this buffer has syntax highl.
    ਪੂਰਨ		save_did_emsg;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
# define WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN 32
    ਪੂਰਨ		did_ਸਤਰ = FALSE;	// set to TRUE when line text done
    ਪੂਰਨ		ਪਾਠ_prop_ਗਿਣਤੀ;
    ਪੂਰਨ		ਆਖਰੀ_textprop_ਪਾਠ_idx = -1;
    ਪੂਰਨ		ਪਾਠ_prop_ਅਗਲਾ = 0;	// next text property to use
    textprop_T	ਪਾਠ_props_buf[WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN];
    ਪੂਰਨ		ਪਾਠ_prop_idxs_buf[WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN];
    char_u	ਪਾਠ_prop_suffix_flags_buf[WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN + 1];
    textprop_T	*ਪਾਠ_props = ਨਿਰਾ;
    ਪੂਰਨ		*ਪਾਠ_prop_idxs = ਨਿਰਾ;
    char_u	*ਪਾਠ_prop_suffix_flags = ਨਿਰਾ;
    ਪੂਰਨ		ਪਾਠ_props_ਸਰਗਰਮ = 0;
    ਪੂਰਨ		ਪਾਠ_props_need_ਛਾਂਟੋ = FALSE;
    proptype_T  *ਪਾਠ_prop_type = ਨਿਰਾ;
    ਪੂਰਨ		ਪਾਠ_prop_attr = 0;
    ਪੂਰਨ		ਪਾਠ_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    ਪੂਰਨ		ਪਾਠ_prop_id = 0;	// active property ID
    ਪੂਰਨ		ਪਾਠ_prop_flags = 0;
    ਪੂਰਨ		ਪਾਠ_prop_above = FALSE;  // first doing virtual text above
    ਪੂਰਨ		ਪਾਠ_prop_follows = FALSE;  // another text prop to display
    ਪੂਰਨ		saved_ਖੋਜ_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    ਪੂਰਨ		saved_area_attr = 0;	// idem for area_attr
    ਪੂਰਨ		reset_extra_attr = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਪੂਰਨ		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    ਪੂਰਨ		nextlinecol = 0;	// column where nextline[] starts
    ਪੂਰਨ		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    ਪੂਰਨ		spell_attr = 0;		// attributes desired by spelling
    ਪੂਰਨ		ਸ਼ਬਦ_ਸਿਰਾ = 0;		// last byte with same spell_attr
    ਪੂਰਨ		cur_checked_ਖਾਨਾ = 0;	// checked column for current line
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		extra_check = FALSE;	// has extra highlighting
    ਪੂਰਨ		multi_attr = 0;		// attributes desired by multibyte
    ਪੂਰਨ		mb_l = 1;		// multi-byte byte length
    ਪੂਰਨ		mb_c = 0;		// decoded multi-byte character
    ਪੂਰਨ		mb_utf8 = FALSE;	// screen char is UTF-8 char
    ਪੂਰਨ		u8cc[MAX_MCO];		// composing UTF-8 chars
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		change_ਸ਼ੁਰੂ = MAXCOL;	// first col of changed area
    ਪੂਰਨ		change_ਸਿਰਾ = -1;	// last col of changed area
    diffline_T	ਸਤਰ_changes;
    ਪੂਰਨ		change_ਸੂਚਕਾਂਕ = -1;
#ਜੇ_ਅੰਤ
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    ਪੂਰਨ		ਵਿੱਚ_multispace = FALSE;	// in multiple consecutive spaces
    ਪੂਰਨ		multispace_pos = 0;	// position in lcs-multispace string
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸਤਰ_ATTR
    ਪੂਰਨ		ਸਤਰ_attr_save = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		sign_present = FALSE;
    ਪੂਰਨ		ਅੰਕ_attr = 0;		// attribute for the number column
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
    ਪੂਰਨ		prev_c = 0;		// previous Arabic character
    ਪੂਰਨ		prev_c1 = 0;		// first composing char for prev_c
#ਜੇ_ਅੰਤ
#ਜੇ defined(ਸਤਰ_ATTR)
    ਪੂਰਨ		did_ਸਤਰ_attr = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪੂਰਨ		get_ਪਦ_attr = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    ਪੂਰਨ		ਖੱਬਾ_curline_ਖਾਨਾ = 0;
    ਪੂਰਨ		ਸੱਜਾ_curline_ਖਾਨਾ = 0;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ਪੂਰਨ		feedback_ਖਾਨਾ = 0;
    ਪੂਰਨ		feedback_old_attr = -1;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CONCEAL) || defined(FEAT_ਖੋਜ_EXTRA)
    ਪੂਰਨ		ਮੇਲ_conc	= 0;	// cchar for match functions
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CONCEAL) || defined(FEAT_ਖੋਜ_EXTRA) || defined(FEAT_LINEBREAK)
    ਪੂਰਨ		on_ਆਖਰੀ_ਖਾਨਾ     = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		syntax_flags	= 0;
    ਪੂਰਨ		syntax_seqnr	= 0;
    ਪੂਰਨ		prev_syntax_id	= 0;
    ਪੂਰਨ		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    ਪੂਰਨ		is_concealing	= FALSE;
    ਪੂਰਨ		did_wcol	= FALSE;
    ਪੂਰਨ		old_boguscols   = 0;
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_co - wlv.vcol_off_tp)
# define FIX_FOR_BOGUSCOLS \
    { \
	wlv.n_extra += wlv.vcol_off_co; \
	wlv.vcol -= wlv.vcol_off_co; \
	wlv.vcol_off_co = 0; \
	wlv.ਖਾਨਾ -= wlv.boguscols; \
	old_boguscols = wlv.boguscols; \
	wlv.boguscols = 0; \
    }
#ਨਹੀਂਤਾਂ
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#ਜੇ_ਅੰਤ

    ਜੇ (startrow > endrow)		// past the end already!
	ਵਾਪਸ startrow;

    CLEAR_ਖੇਤਰ(wlv);

    wlv.lnum = lnum;
    wlv.startrow = startrow;
    wlv.ਪੰਕਤੀ = startrow;
    wlv.screen_ਪੰਕਤੀ = wlv.ਪੰਕਤੀ + W_WINROW(wp);
    wlv.fromcol = -10;
    wlv.tocol = MAXCOL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    wlv.vcol_sbr = -1;
#ਜੇ_ਅੰਤ

    ਜੇ (ਸੰਖਿਆ_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (syntax_present(wp) && !wp->w_s->b_syn_error
# ifdef SYN_ਸਮਾਂ_ਹੱਦਬੰਦੀ
		&& !wp->w_s->b_syn_slow
# endif
	   )
	{
	    // Prepare for syntax highlighting in this line.  When there is an
	    // error, stop syntax highlighting.
	    save_did_emsg = did_emsg;
	    did_emsg = FALSE;
	    syntax_ਸ਼ੁਰੂ(wp, lnum);
	    ਜੇ (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_ਸਮਾਂ_ਹੱਦਬੰਦੀ
		ਜੇ (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_ਖਾਨੇ_ਗਿਣਤੀ = wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ;
	ਜੇ (wlv.color_ਖਾਨੇ_ਗਿਣਤੀ != ਨਿਰਾ)
	    wlv.draw_color_ਖਾਨਾ = advance_color_ਖਾਨਾ(VCOL_HLC, &wlv.color_ਖਾਨੇ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਜੇ (ਪਦ_show_ਬਫਰ(wp->w_ਬਫਰ))
	{
	    extra_check = TRUE;
	    get_ਪਦ_attr = TRUE;
	    wlv.win_attr = ਪਦ_get_attr(wp, lnum, -1);
	}
#ਜੇ_ਅੰਤ

	// handle Visual active in this window
	ਜੇ (VIsual_ਸਰਗਰਮ && wp->w_ਬਫਰ == curwin->w_ਬਫਰ)
	{
	    pos_T	*ਸਿਖਰ, *bot;

	    ਜੇ (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		ਸਿਖਰ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Visual is before curwin->w_cursor
		ਸਿਖਰ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_ਵਿੱਚ_visual_area = (lnum >= ਸਿਖਰ->lnum && lnum <= bot->lnum);
	    ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V)
	    {
		// block mode
		ਜੇ (lnum_ਵਿੱਚ_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// non-block mode
		ਜੇ (lnum > ਸਿਖਰ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		ਨਹੀਂਤਾਂ ਜੇ (lnum == ਸਿਖਰ->lnum)
		{
		    ਜੇ (VIsual_ਬਹੁਲਕ == 'V')	// linewise
			wlv.fromcol = 0;
		    ਨਹੀਂਤਾਂ
		    {
			getvvcol(wp, ਸਿਖਰ, &wlv.fromcol, ਨਿਰਾ, ਨਿਰਾ, 0);
			ਜੇ (gchar_pos(ਸਿਖਰ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		ਜੇ (VIsual_ਬਹੁਲਕ != 'V' && lnum == bot->lnum)
		{
		    ਜੇ (*p_sel == 'e' && bot->ਖਾਨਾ == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (bot->ਖਾਨਾ == MAXCOL)
			wlv.tocol = MAXCOL;
		    ਨਹੀਂਤਾਂ
		    {
			pos = *bot;
			ਜੇ (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ਨਿਰਾ, ਨਿਰਾ, 0);
			ਨਹੀਂਤਾਂ
			{
			    getvvcol(wp, &pos, ਨਿਰਾ, ਨਿਰਾ, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    ਜੇ (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#ਜੇ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		ਜੇ ((clip_ਤਾਰਾ.ਉਪਲਬਧ && !clip_ਤਾਰਾ.owned
						    && clip_isautosel_ਤਾਰਾ())
			|| (clip_plus.ਉਪਲਬਧ && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#ਜੇ_ਅੰਤ
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	ਨਹੀਂਤਾਂ ਜੇ (highlight_ਮੇਲ
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ)
	{
	    ਜੇ (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ਨਿਰਾ, ਨਿਰਾ, 0);
	    ਨਹੀਂਤਾਂ
		wlv.fromcol = 0;
	    ਜੇ (lnum == curwin->w_cursor.lnum + ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ)
	    {
		pos.lnum = lnum;
		pos.ਖਾਨਾ = ਖੋਜ_ਮੇਲ_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ਨਿਰਾ, ਨਿਰਾ, 0);
	    }
	    ਨਹੀਂਤਾਂ
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    ਜੇ (wlv.fromcol == wlv.tocol && ਖੋਜ_ਮੇਲ_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF

    ਪੂਰਨ linestatus = 0;
    wlv.filler_ਸਤਰਾਂ = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_ਖੇਤਰ(ਸਤਰ_changes);

    ਜੇ (linestatus < 0)
    {
	ਜੇ (linestatus == -1)
	{
	    ਜੇ (diff_ਲੱਭੋ_change(wp, lnum, &ਸਤਰ_changes))
	    {
		wlv.diff_hlf = HLF_ਜੋੜੋ;	// added line
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ_changes.ਅੰਕ_changes > 0)
	    {
		ਪੂਰਨ added = diff_change_parse(
			&ਸਤਰ_changes, &ਸਤਰ_changes.changes[0],
			&change_ਸ਼ੁਰੂ, &change_ਸਿਰਾ);
		ਜੇ (change_ਸ਼ੁਰੂ == 0)
		{
		    ਜੇ (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    ਨਹੀਂਤਾਂ
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		ਨਹੀਂਤਾਂ
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_ਸੂਚਕਾਂਕ = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_ਸੂਚਕਾਂਕ = 0;
	    }
	}
	ਨਹੀਂਤਾਂ
	    wlv.diff_hlf = HLF_ਜੋੜੋ;

	area_highlighting = TRUE;
    }

    ਜੇ (lnum == wp->w_topline)
	wlv.filler_ਸਤਰਾਂ = wp->w_topfill;

    wlv.filler_todo = wlv.filler_ਸਤਰਾਂ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    ਜੇ (sign_present)
	ਅੰਕ_attr = wlv.sattr.sat_numhl;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸਤਰ_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    ਜੇ (sign_present)
	wlv.ਸਤਰ_attr = wlv.sattr.sat_linehl;
# endif
# ਜੇ defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    ਜੇ (bt_quickfix(wp->w_ਬਫਰ) && qf_current_ਇੰਦਰਾਜ(wp) == lnum)
	wlv.ਸਤਰ_attr = HL_ATTR(HLF_QFL);
# endif
    ਜੇ (wlv.ਸਤਰ_attr != 0)
	area_highlighting = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਜੇ (spv->spv_has_spell && ਸੰਖਿਆ_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	ਜੇ (lnum == spv->spv_checked_lnum)
	    cur_checked_ਖਾਨਾ = spv->spv_checked_ਖਾਨਾ;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	ਜੇ (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_ਖਾਨਾ = 0;
	ਨਹੀਂਤਾਂ ਜੇ (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_ਖਾਨਾ = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	ਜੇ (lnum < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum + 1, FALSE);
	    spell_cat_ਸਤਰ(nextline + SPWORDLEN, ਸਤਰ, SPWORDLEN);
	}
	ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(ਸਤਰ);
	ਜੇ (*ptr == NUL)
	{
	    spv->spv_cap_ਖਾਨਾ = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	ਨਹੀਂਤਾਂ ਜੇ (spv->spv_cap_ਖਾਨਾ == 0)
	    spv->spv_cap_ਖਾਨਾ = ptr - ਸਤਰ;

	// Copy the end of the current line into nextline[].
	ਜੇ (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    v = ml_get_buf_len(wp->w_ਬਫਰ, lnum);
	    ਜੇ (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, ਸਤਰ, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, ਸਤਰ + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#ਜੇ_ਅੰਤ

    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    ptr = ਸਤਰ;

    ਜੇ (wp->w_p_list)
    {
	ਜੇ (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ਨਿਰਾ
		|| wp->w_lcs_chars.leadmultispace != ਨਿਰਾ
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	ਜੇ (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_ਬਫਰ, lnum);
	    ਜਦੋਂ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - ਸਤਰ);
	}
	// find end of leading whitespace
	ਜੇ (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ਨਿਰਾ ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    ਜਦੋਂ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    ਜੇ (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    ਨਹੀਂਤਾਂ
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - ਸਤਰ) + 1;
	}
    }

    ਜੇ (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    ਪੂਰਨ skipcol_ਵਿੱਚ_ਪਾਠ_prop_above = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (WIN_IS_POPUP(wp))
	wlv.screen_ਸਤਰ_flags |= SLF_POPUP;

    char_u *prop_ਸ਼ੁਰੂ;
    ਪਾਠ_prop_ਗਿਣਤੀ = get_ਪਾਠ_props(wp->w_ਬਫਰ, lnum, &prop_ਸ਼ੁਰੂ, FALSE);
    ਜੇ (ਪਾਠ_prop_ਗਿਣਤੀ > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	ਜੇ (ਪਾਠ_prop_ਗਿਣਤੀ <= WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN)
	    ਪਾਠ_props = ਪਾਠ_props_buf;
	ਨਹੀਂਤਾਂ
	    ਪਾਠ_props = ALLOC_MULT(textprop_T, ਪਾਠ_prop_ਗਿਣਤੀ);
	ਜੇ (ਪਾਠ_props != ਨਿਰਾ)
	{
	    mch_memmove(ਪਾਠ_props, prop_ਸ਼ੁਰੂ,
				     ਪਾਠ_prop_ਗਿਣਤੀ * ਆਕਾਰ(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *ਗਿਣਤੀ_ptr = prop_ਸ਼ੁਰੂ - PROP_ਗਿਣਤੀ_ਮਾਪ;

	    ਲਈ (ਪੂਰਨ i = 0; i < ਪਾਠ_prop_ਗਿਣਤੀ; ++i)
	    {
		ਜੇ (ਪਾਠ_props[i].tp_id < 0
				&& ਪਾਠ_props[i].u.tp_ਪਾਠ_ਵਿੱਥ > 0)
		{
		    ਪਾਠ_props[i].u.tp_ਪਾਠ =
			    ਗਿਣਤੀ_ptr + ਪਾਠ_props[i].u.tp_ਪਾਠ_ਵਿੱਥ;
		    ਪਾਠ_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		ਨਹੀਂਤਾਂ
		    ਪਾਠ_props[i].u.tp_ਪਾਠ = ਨਿਰਾ;
	    }
	}

	// Allocate an array for the indexes.
	ਜੇ (ਪਾਠ_prop_ਗਿਣਤੀ <= WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN)
	    ਪਾਠ_prop_idxs = ਪਾਠ_prop_idxs_buf;
	ਨਹੀਂਤਾਂ
	    ਪਾਠ_prop_idxs = ALLOC_MULT(ਪੂਰਨ, ਪਾਠ_prop_ਗਿਣਤੀ);
	ਜੇ (ਪਾਠ_prop_idxs == ਨਿਰਾ)
	{
	    ਜੇ (ਪਾਠ_props != ਪਾਠ_props_buf)
		VIM_CLEAR(ਪਾਠ_props);
	    ਨਹੀਂਤਾਂ
		ਪਾਠ_props = ਨਿਰਾ;
	}
	ਜੇ (ਪਾਠ_prop_ਗਿਣਤੀ <= WIN_ਸਤਰ_ਪਾਠ_PROP_ਢੇਰ_LEN)
	    ਪਾਠ_prop_suffix_flags = ਪਾਠ_prop_suffix_flags_buf;
	ਨਹੀਂਤਾਂ
	    ਪਾਠ_prop_suffix_flags = ALLOC_MULT(char_u, ਪਾਠ_prop_ਗਿਣਤੀ + 1);
	ਜੇ (ਪਾਠ_prop_suffix_flags == ਨਿਰਾ)
	{
	    ਜੇ (ਪਾਠ_prop_idxs != ਪਾਠ_prop_idxs_buf)
		vim_free(ਪਾਠ_prop_idxs);
	    ਪਾਠ_prop_idxs = ਨਿਰਾ;
	    ਜੇ (ਪਾਠ_props != ਪਾਠ_props_buf)
		VIM_CLEAR(ਪਾਠ_props);
	    ਨਹੀਂਤਾਂ
		ਪਾਠ_props = ਨਿਰਾ;
	}

	ਜੇ (ਪਾਠ_props != ਨਿਰਾ)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    ਲਈ (ਪੂਰਨ i = 0; i < ਪਾਠ_prop_ਗਿਣਤੀ; ++i)
		ਜੇ (ਪਾਠ_props[i].tp_id < 0)
		    ਆਖਰੀ_textprop_ਪਾਠ_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    ਪੂਰਨ ਪਾਠ_ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);
	    ਲਈ (ਪੂਰਨ i = ਪਾਠ_prop_ਗਿਣਤੀ - 1; i >= 0; --i)
		ਜੇ (ਪਾਠ_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    ਜੇ (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_ਵਿੱਚ_ਪਾਠ_prop_above
								 >= ਪਾਠ_ਚੌੜਾਈ)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_ਵਿੱਚ_ਪਾਠ_prop_above += ਪਾਠ_ਚੌੜਾਈ;
			ਲਈ (ਪੂਰਨ j = i + 1; j < ਪਾਠ_prop_ਗਿਣਤੀ; ++j)
			    ਪਾਠ_props[j - 1] = ਪਾਠ_props[j];
			++i;
			--ਪਾਠ_prop_ਗਿਣਤੀ;
		    }
		    ਨਹੀਂਤਾਂ
			++wlv.ਪਾਠ_prop_above_ਗਿਣਤੀ;
		}

	    ਪਾਠ_prop_suffix_flags[ਪਾਠ_prop_ਗਿਣਤੀ] = 0;
	    ਲਈ (ਪੂਰਨ i = ਪਾਠ_prop_ਗਿਣਤੀ - 1; i >= 0; --i)
	    {
		ਪੂਰਨ flags = ਪਾਠ_prop_suffix_flags[i + 1];

		ਜੇ (ਪਾਠ_props[i].tp_ਖਾਨਾ == MAXCOL)
		{
		    flags |= 1;
		    ਜੇ (ਪਾਠ_props[i].tp_flags & TP_FLAG_ALIGN_ਹੇਠਾਂ)
			flags |= 2;
		}
		ਪਾਠ_prop_suffix_flags[i] = flags;
	    }
	}
    }

    ਜੇ (ਸੰਖਿਆ_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.ਪੰਕਤੀ += wlv.ਪਾਠ_prop_above_ਗਿਣਤੀ;
	ਜੇ (wlv.ਪੰਕਤੀ >= endrow)
	{
	    ਜੇ (ਪਾਠ_props != ਪਾਠ_props_buf)
		vim_free(ਪਾਠ_props);
	    ਜੇ (ਪਾਠ_prop_idxs != ਪਾਠ_prop_idxs_buf)
		vim_free(ਪਾਠ_prop_idxs);
	    ਜੇ (ਪਾਠ_prop_suffix_flags != ਪਾਠ_prop_suffix_flags_buf)
		vim_free(ਪਾਠ_prop_suffix_flags);
	    ਵਾਪਸ wlv.ਪੰਕਤੀ;
	}
	wlv.screen_ਪੰਕਤੀ += wlv.ਪਾਠ_prop_above_ਗਿਣਤੀ;
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_ਪਹਿਲਾ_char = 0;
    ਜੇ (wp->w_p_lbr && ਸੰਖਿਆ_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, ਸਤਰ, ਸਤਰ);
	(ਖਾਲੀ)win_lbr_chartabsize(&cts, ਨਿਰਾ, ਨਿਰਾ);
	vcol_ਪਹਿਲਾ_char = cts.cts_ਪਹਿਲਾ_char;
	clear_chartabsize_arg(&cts);
    }
#ਜੇ_ਅੰਤ

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    ਜੇ (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_ਵਿੱਚ_ਪਾਠ_prop_above : 0;
    ਨਹੀਂਤਾਂ
	v = wp->w_leftcol;
    ਜੇ (v > 0 && ਸੰਖਿਆ_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	ਪੂਰਨ		charsize = 0;
	ਪੂਰਨ		ਸਿਰ = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, ਸਤਰ, ptr);
	cts.cts_max_ਸਿਰ_vcol = v;
	ਜਦੋਂ (cts.cts_vcol < v)
	{
	    ਸਿਰ = 0;
	    charsize = win_lbr_chartabsize(&cts, &ਸਿਰ, ਨਿਰਾ);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    ਜੇ (*prev_ptr == NUL)
		ਰੋਕੋ;
	    MB_PTR_ADV(cts.cts_ptr);
	    ਜੇ (wp->w_p_list)
	    {
		ਵਿੱਚ_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > ਸਤਰ && prev_ptr[-1] == ' '));
		ਜੇ (!ਵਿੱਚ_multispace)
		    multispace_pos = 0;
		ਨਹੀਂਤਾਂ ਜੇ (cts.cts_ptr >= ਸਤਰ + leadcol
					 && wp->w_lcs_chars.multispace != ਨਿਰਾ)
		{
		    ++multispace_pos;
		    ਜੇ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		ਨਹੀਂਤਾਂ ਜੇ (cts.cts_ptr < ਸਤਰ + leadcol
				     && wp->w_lcs_chars.leadmultispace != ਨਿਰਾ)
		{
		    ++multispace_pos;
		    ਜੇ (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
	    }
	}
	wlv.vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// When:
	// - 'cuc' is set, or
	// - 'colorcolumn' is set, or
	// - 'virtualedit' is set, or
	// - the visual mode is active,
	// the end of the line may be before the start of the displayed part.
	ਜੇ (wlv.vcol < v && (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_ਖਾਨਾ ||
#ਜੇ_ਅੰਤ
	     virtual_ਸਰਗਰਮ() ||
	     (VIsual_ਸਰਗਰਮ && wp->w_ਬਫਰ == curwin->w_ਬਫਰ)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	ਜੇ (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	ਜੇ (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - ਸਿਰ;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	ਜੇ (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	ਨਹੀਂਤਾਂ ਜੇ (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	ਜੇ (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	ਜੇ (spv->spv_has_spell)
	{
	    ਪੂਰਨ		len;
	    colnr_T	linecol = (colnr_T)(ptr - ਸਤਰ);
	    hlf_T	spell_hlf = HLF_ਗਿਣਤੀ;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.ਖਾਨਾ = linecol;
	    len = spell_move_ਮੰਜ਼ਿਲ(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
	    ptr = ਸਤਰ + linecol;

	    ਜੇ (len == 0 || (ਪੂਰਨ)wp->w_cursor.ਖਾਨਾ > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		//ਸ਼ਬਦ
		spell_hlf = HLF_ਗਿਣਤੀ;
		ਸ਼ਬਦ_ਸਿਰਾ = (ਪੂਰਨ)(spell_ਮੰਜ਼ਿਲ_ਸ਼ਬਦ_ਸਿਰਾ(ptr, wp) - ਸਤਰ + 1);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// bad word found, use attributes until end of word
		ਸ਼ਬਦ_ਸਿਰਾ = wp->w_cursor.ਖਾਨਾ + len + 1;

		// Turn index into actual attributes.
		ਜੇ (spell_hlf != HLF_ਗਿਣਤੀ)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    ਜੇ (has_syntax)
		syntax_ਸ਼ੁਰੂ(wp, lnum);
# endif
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਜੇ (ਸੰਖਿਆ_only == 0)
    {
	v = (ਲੰਮਾ)(ptr - ਸਤਰ);
	area_highlighting |= prepare_ਖੋਜ_hl_ਸਤਰ(wp, lnum, (colnr_T)v,
					      &ਸਤਰ, &screen_ਖੋਜ_hl,
					      &ਖੋਜ_attr);
	ptr = ਸਤਰ + v; // "line" may have been updated
    }
#ਜੇ_ਅੰਤ

    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && ins_compl_win_ਸਰਗਰਮ(wp)
			    && (ਵਿੱਚ_curline || ins_compl_lnum_ਵਿੱਚ_range(lnum)))
	area_highlighting = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    ਜੇ (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	ਜੇ (!(wp == curwin && VIsual_ਸਰਗਰਮ)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    ਜੇ (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    ਨਹੀਂਤਾਂ
	    {
		ਸਤਰ_attr_save = wlv.ਸਤਰ_attr;
		margin_columns_win(wp, &ਖੱਬਾ_curline_ਖਾਨਾ, &ਸੱਜਾ_curline_ਖਾਨਾ);
	    }
	    area_highlighting = TRUE;
	}
    }
#ਜੇ_ਅੰਤ

    win_ਸਤਰ_ਸ਼ੁਰੂ(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    ਲਈ (;;)
    {
#ਜੇ defined(FEAT_CONCEAL) || defined(FEAT_ਖੋਜ_EXTRA)
	ਪੂਰਨ	has_ਮੇਲ_conc = 0;	// match wants to conceal
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	ਪੂਰਨ	did_decrement_ptr = FALSE;
#ਜੇ_ਅੰਤ

	// Skip this quickly when working on the text.
	ਜੇ (wlv.draw_state != WL_ਸਤਰ)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.ਸਤਰ_attr = ਸਤਰ_attr_save;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		ਜੇ (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    ਜੇ (wlv.ਪੰਕਤੀ == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    ਨਹੀਂਤਾਂ
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    ਜੇ (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		ਜੇ (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_ਖਾਨਾ(wp, &wlv, sign_present, ਅੰਕ_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    ਜੇ (ਸੰਖਿਆ_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_ਸਤਰ(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		ਜੇ ((wlv.ਪੰਕਤੀ + 1 - wlv.startrow < ਸੰਖਿਆ_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			|| wlv.filler_todo > 0
#ਜੇ_ਅੰਤ
			)
		{
		    ++wlv.ਪੰਕਤੀ;
		    ++wlv.screen_ਪੰਕਤੀ;
		    ਜੇ (wlv.ਪੰਕਤੀ == endrow)
			ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    --wlv.filler_todo;
		    ਜੇ (wlv.filler_todo == 0 && wp->w_botfill)
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		    win_ਸਤਰ_ਸ਼ੁਰੂ(wp, &wlv, TRUE);
		    ਜਾਰੀ;
		}
		ਨਹੀਂਤਾਂ
		    ਰੋਕੋ;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    ਜੇ (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    ਜੇ (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (wlv.draw_state == WL_ਸਤਰ - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_ਸਤਰ;
		win_ਸਤਰ_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (wlv.cul_screenline && wlv.draw_state == WL_ਸਤਰ
		&& wlv.vcol >= ਖੱਬਾ_curline_ਖਾਨਾ
		&& wlv.vcol < ਸੱਜਾ_curline_ਖਾਨਾ)
	    apply_cursorline_highlight(&wlv, sign_present);
#ਜੇ_ਅੰਤ

	// When still displaying '$' of change command, stop at cursor.
	ਜੇ (dollar_vcol >= 0 && ਵਿੱਚ_curline && wlv.vcol >= (ਲੰਮਾ)wp->w_virtcol)
	{
	    wlv_screen_ਸਤਰ(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (wp->w_p_cuc)
		wlv.ਪੰਕਤੀ = wp->w_cline_ਪੰਕਤੀ + wp->w_cline_ਉਚਾਈ;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		wlv.ਪੰਕਤੀ = wp->w_ਉਚਾਈ;
	    ਰੋਕੋ;
	}

	ਜੇ (wlv.draw_state == WL_ਸਤਰ && (area_highlighting || extra_check))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (ਪਾਠ_props != ਨਿਰਾ)
	    {
		ਪੂਰਨ pi;
		ਪੂਰਨ bcol = (ਪੂਰਨ)(ptr - ਸਤਰ);

		ਜੇ (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !ਵਿੱਚ_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		ਲਈ (pi = 0; pi < ਪਾਠ_props_ਸਰਗਰਮ; ++pi)
		{
		    ਪੂਰਨ		tpi = ਪਾਠ_prop_idxs[pi];
		    textprop_T  *tp = &ਪਾਠ_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    ਜੇ ((tp->tp_ਖਾਨਾ != MAXCOL
				       && bcol >= tp->tp_ਖਾਨਾ - 1 + tp->tp_len))
		    {
			ਜੇ (pi + 1 < ਪਾਠ_props_ਸਰਗਰਮ)
			    mch_memmove(ਪਾਠ_prop_idxs + pi,
					ਪਾਠ_prop_idxs + pi + 1,
					ਆਕਾਰ(ਪੂਰਨ)
					     * (ਪਾਠ_props_ਸਰਗਰਮ - (pi + 1)));
			--ਪਾਠ_props_ਸਰਗਰਮ;
			ਪਾਠ_props_need_ਛਾਂਟੋ = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			ਜੇ (ਵਿੱਚ_linebreak && syntax_attr == ਪਾਠ_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		ਜੇ (wlv.n_extra > 0 && ਵਿੱਚ_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		ਜਦੋਂ (ਪਾਠ_prop_ਅਗਲਾ < ਪਾਠ_prop_ਗਿਣਤੀ)
		{
		    ਪੂਰਨ ਸਰਗਰਮ;
		    textprop_T *tp = &ਪਾਠ_props[ਪਾਠ_prop_ਅਗਲਾ];
		    ਜੇ (tp->tp_ਖਾਨਾ == MAXCOL)
		    {
			ਜੇ (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    ਸਰਗਰਮ = TRUE;
			ਨਹੀਂਤਾਂ ਜੇ (*ptr != NUL)
			    ਰੋਕੋ;
			ਨਹੀਂਤਾਂ
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    ਸਰਗਰਮ = wp->w_p_wrap
				  || wlv.ਪੰਕਤੀ == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_ਹੇਠਾਂ);
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (bcol < tp->tp_ਖਾਨਾ - 1)
			    ਰੋਕੋ;
			ਸਰਗਰਮ = bcol <= tp->tp_ਖਾਨਾ - 1 + tp->tp_len;
		    }

		    ਜੇ (ਸਰਗਰਮ)
		    {
			ਪਾਠ_prop_idxs[ਪਾਠ_props_ਸਰਗਰਮ++] = ਪਾਠ_prop_ਅਗਲਾ;
			ਪਾਠ_props_need_ਛਾਂਟੋ = TRUE;
		    }
		    ++ਪਾਠ_prop_ਅਗਲਾ;
		}

		ਜੇ (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(ਪਾਠ_prop_type != ਨਿਰਾ &&
			     ਪਾਠ_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    ਪਾਠ_prop_attr = 0;
		    ਪਾਠ_prop_attr_comb = 0;
		    ਪਾਠ_prop_flags = 0;
		    ਪਾਠ_prop_type = ਨਿਰਾ;
		    ਪਾਠ_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		ਜੇ (ਪਾਠ_props_ਸਰਗਰਮ > 0 && wlv.n_extra == 0)
		{
		    ਪੂਰਨ ਵਰਤਿਆ_tpi = -1;
		    ਪੂਰਨ ਵਰਤਿਆ_attr = 0;
		    ਪੂਰਨ other_tpi = -1;

		    ਪਾਠ_prop_above = FALSE;
		    ਪਾਠ_prop_follows = FALSE;

		    ਜੇ (ਪਾਠ_props_need_ਛਾਂਟੋ)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			ਛਾਂਟੋ_ਪਾਠ_props(wp->w_ਬਫਰ, ਪਾਠ_props,
					ਪਾਠ_prop_idxs, ਪਾਠ_props_ਸਰਗਰਮ);
			ਪਾਠ_props_need_ਛਾਂਟੋ = FALSE;
		    }

		    ਲਈ (pi = 0; pi < ਪਾਠ_props_ਸਰਗਰਮ; ++pi)
		    {
			ਪੂਰਨ	    tpi = ਪਾਠ_prop_idxs[pi];
			textprop_T  *tp = &ਪਾਠ_props[tpi];
			proptype_T  *pt = ਪਾਠ_prop_type_by_id(
						    wp->w_ਬਫਰ, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			ਜੇ (pt != ਨਿਰਾ
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_ਸੱਜਾ
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_ਹੇਠਾਂ)) == 0
				    && wlv.ਖਾਨਾ >= wp->w_ਚੌੜਾਈ))
			{
			    ਜੇ (tp->tp_ਖਾਨਾ == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == ਸਤਰ
						&& !did_ਸਤਰ
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_ਹੇਠਾਂ))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				ਪਾਠ_prop_follows = TRUE;
				ਜਾਰੀ;
			    }

			    ਜੇ (pt->pt_hl_id > 0)
				ਵਰਤਿਆ_attr = syn_id2attr(pt->pt_hl_id);
			    ਪਾਠ_prop_type = pt;
			    ਪਾਠ_prop_attr =
				   hl_combine_attr(ਪਾਠ_prop_attr, ਵਰਤਿਆ_attr);
			    ਜੇ (ਵਰਤਿਆ_tpi >= 0 && ਪਾਠ_props[ਵਰਤਿਆ_tpi].tp_id < 0)
				other_tpi = ਵਰਤਿਆ_tpi;
			    ਪਾਠ_prop_flags = pt->pt_flags;
			    ਪਾਠ_prop_id = tp->tp_id;
			    ਵਰਤਿਆ_tpi = tpi;
			}
		    }
		    ਜੇ (ਪਾਠ_prop_id < 0 && ਵਰਤਿਆ_tpi >= 0
				 && ਪਾਠ_props[ਵਰਤਿਆ_tpi].u.tp_ਪਾਠ != ਨਿਰਾ)
		    {
			textprop_T  *tp = &ਪਾਠ_props[ਵਰਤਿਆ_tpi];
			char_u	    *p = tp->u.tp_ਪਾਠ;
			ਪੂਰਨ	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			ਪੂਰਨ	    bail_ਬਾਹਰਲਾ = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			ਜੇ (p != ਨਿਰਾ)
			{
			    ਪੂਰਨ	    ਸੱਜਾ = (tp->tp_flags
							& TP_FLAG_ALIGN_ਸੱਜਾ);
			    ਪੂਰਨ	    ਹੇਠਾਂ = (tp->tp_flags
							& TP_FLAG_ALIGN_ਹੇਠਾਂ);
			    ਪੂਰਨ	    wrap = tp->tp_ਖਾਨਾ < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    ਪੂਰਨ	    padding = tp->tp_ਖਾਨਾ == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (ਪੂਰਨ)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.ਸ਼ੁਰੂ_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    ਵਰਤਿਆ_attr);
			    n_attr = mb_charlen(p);
			    ਪਾਠ_prop_attr = 0;
			    ਪਾਠ_prop_attr_comb = 0;
			    ਜੇ (*ptr == NUL)
				// don't combine char attr after EOL
				ਪਾਠ_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    ਜੇ (ਪਾਠ_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    ਜੇ ((ਸੱਜਾ || above || ਹੇਠਾਂ || !wrap
					    || padding > 0) && wp->w_ਚੌੜਾਈ > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				ਪੂਰਨ	ਸ਼ੁਰੂ_ਸਤਰ;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				ਸ਼ੁਰੂ_ਸਤਰ = ਪਾਠ_prop_position(wp, tp,
						    wlv.vcol,
# ifdef FEAT_RIGHTLEFT
						    wp->w_p_rl
						    ? wp->w_ਚੌੜਾਈ - wlv.ਖਾਨਾ - 1
						    :
# endif
						    wlv.ਖਾਨਾ,
						    &wlv.n_extra, &wlv.p_extra,
						    &n_attr, &wlv.n_attr_skip,
						    skip_cells > 0);
				ਜੇ (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				ਜੇ (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				ਜੇ (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.ਖਾਨਾ
					       + wlv.n_extra - 2 > wp->w_ਚੌੜਾਈ)
				    // don't bail out at end of line
				    ਪਾਠ_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				ਜੇ (ਸ਼ੁਰੂ_ਸਤਰ)
				{
				    draw_screen_ਸਤਰ(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    ਜੇ (wlv.ਪੰਕਤੀ == endrow)
				    {
					++wlv.ਪੰਕਤੀ;
					ਰੋਕੋ;
				    }
				    win_ਸਤਰ_ਸ਼ੁਰੂ(wp, &wlv, TRUE);
				    bail_ਬਾਹਰਲਾ = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			ਜੇ (skip_cells > 0)
			{
			    ਜੇ (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				ਜੇ (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				// the whole text is left of the window, drop
				// it and advance to the next one
				skip_cells -= wlv.n_extra;
				skipped_cells += wlv.n_extra;
				wlv.n_extra = 0;
				wlv.n_attr_skip = 0;
				bail_ਬਾਹਰਲਾ = TRUE;
			    }
			}

			// If another text prop follows the condition below at
			// the last window column must know.
			// If this is an "above" text prop and 'nowrap' then we
			// must wrap anyway.
			ਪਾਠ_prop_above = above;
			ਪਾਠ_prop_follows |= other_tpi != -1
					&& (wp->w_p_wrap
					     || (ਪਾਠ_props[other_tpi].tp_flags
			       & (TP_FLAG_ALIGN_ਹੇਠਾਂ | TP_FLAG_ALIGN_ਸੱਜਾ)));

			ਜੇ (bail_ਬਾਹਰਲਾ)
			    // starting a new line for "below"
			    ਜਾਰੀ;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਪਾਠ_prop_ਅਗਲਾ < ਪਾਠ_prop_ਗਿਣਤੀ
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.ਖਾਨਾ == wp->w_ਚੌੜਾਈ - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    ਪੂਰਨ only_ਹੇਠਾਂ_follows = !wp->w_p_wrap
				 && wlv.ਖਾਨਾ + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_ਚੌੜਾਈ;
		    ਪੂਰਨ suffix_flags = ਪਾਠ_prop_suffix_flags[ਪਾਠ_prop_ਅਗਲਾ];

		    ਪਾਠ_prop_follows = (suffix_flags
					& (only_ਹੇਠਾਂ_follows ? 2 : 1)) != 0;
		}
	    }

	    ਜੇ (wlv.ਸ਼ੁਰੂ_extra_for_textprop)
	    {
		wlv.ਸ਼ੁਰੂ_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_ਖੋਜ_attr = ਖੋਜ_attr;
		saved_area_attr = area_attr;
		ਖੋਜ_attr = 0;
		area_attr = 0;
	    }
#ਜੇ_ਅੰਤ

	    ਪੂਰਨ *area_attr_p =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#ਜੇ_ਅੰਤ
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    ਜੇ (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ਨਿਰਾ
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    ਨਹੀਂਤਾਂ ਜੇ (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    ਜੇ (wlv.n_extra == 0)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (ਲੰਮਾ)(ptr - ਸਤਰ);
		ਖੋਜ_attr = update_ਖੋਜ_hl(wp, lnum, (colnr_T)v, &ਸਤਰ,
				      &screen_ਖੋਜ_hl, &has_ਮੇਲ_conc,
				      &ਮੇਲ_conc, did_ਸਤਰ_attr, lcs_eol_one,
				      &on_ਆਖਰੀ_ਖਾਨਾ);
		ptr = ਸਤਰ + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		ਜੇ (*ptr == NUL)
		    has_ਮੇਲ_conc = 0;
#ਨਹੀਂਤਾਂ
		ਖੋਜ_attr = 0;
#ਜੇ_ਅੰਤ

		// Check if ComplMatchIns highlight is needed.
		ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && ins_compl_win_ਸਰਗਰਮ(wp)
			    && (ਵਿੱਚ_curline || ins_compl_lnum_ਵਿੱਚ_range(lnum)))
		{
		    ਪੂਰਨ ins_ਮੇਲ_attr =
			ins_compl_ਖਾਨਾ_range_attr(lnum, (ਪੂਰਨ)(ptr - ਸਤਰ));
		    ਜੇ (ins_ਮੇਲ_attr > 0)
			ਖੋਜ_attr =
			    hl_combine_attr(ਖੋਜ_attr, ins_ਮੇਲ_attr);
		}
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਜੇ (wlv.diff_hlf != (hlf_T)0)
	    {
		ਜੇ (ਸਤਰ_changes.ਅੰਕ_changes > 0
			&& change_ਸੂਚਕਾਂਕ >= 0
			&& change_ਸੂਚਕਾਂਕ < ਸਤਰ_changes.ਅੰਕ_changes - 1)
		{
		    ਜੇ (ptr - ਸਤਰ >= ਸਤਰ_changes.changes[change_ਸੂਚਕਾਂਕ+1].dc_ਸ਼ੁਰੂ[ਸਤਰ_changes.bufidx])
			change_ਸੂਚਕਾਂਕ += 1;
		}
		ਪੂਰਨ added = FALSE;
		ਜੇ (ਸਤਰ_changes.ਅੰਕ_changes > 0 && change_ਸੂਚਕਾਂਕ >= 0 && change_ਸੂਚਕਾਂਕ < ਸਤਰ_changes.ਅੰਕ_changes)
		{
		    added = diff_change_parse(&ਸਤਰ_changes,
			    &ਸਤਰ_changes.changes[change_ਸੂਚਕਾਂਕ],
			    &change_ਸ਼ੁਰੂ,
			    &change_ਸਿਰਾ);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		ਜੇ (wlv.diff_hlf == HLF_CHD && ptr - ਸਤਰ >= change_ਸ਼ੁਰੂ
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		ਜੇ ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - ਸਤਰ >= change_ਸਿਰਾ && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.ਸਤਰ_attr = HL_ATTR(wlv.diff_hlf);
		ਜੇ (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ਖੱਬਾ_curline_ਖਾਨਾ
					    && wlv.vcol <= ਸੱਜਾ_curline_ਖਾਨਾ)))
		    wlv.ਸਤਰ_attr = hl_combine_attr(
					  wlv.ਸਤਰ_attr, HL_ATTR(HLF_CUL));
	    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		ਜੇ (get_ਪਦ_attr)
		    syntax_attr = ਪਦ_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		ਜੇ (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (ਲੰਮਾ)(ptr - ਸਤਰ);
		    ਜੇ (v == prev_syntax_ਖਾਨਾ)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    ਨਹੀਂਤਾਂ
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ਨਿਰਾ, FALSE);
			prev_syntax_ਖਾਨਾ = v;
			prev_syntax_attr = syntax_attr;
		    }

		    ਜੇ (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    ਨਹੀਂਤਾਂ
			did_emsg = save_did_emsg;
# ifdef SYN_ਸਮਾਂ_ਹੱਦਬੰਦੀ
		    ਜੇ (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
		    ptr = ਸਤਰ + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    ਜੇ (*ptr == NUL)
			syntax_flags = 0;
		    ਨਹੀਂਤਾਂ
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    ਜੇ (ਪਾਠ_prop_type != ਨਿਰਾ)
	    {
		ਜੇ (ਪਾਠ_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, ਪਾਠ_prop_attr);
		ਨਹੀਂਤਾਂ
		    syntax_attr = ਪਾਠ_prop_attr;
		ਪਾਠ_prop_attr_comb = syntax_attr;
	    }
# endif
#ਜੇ_ਅੰਤ

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸਤਰ_ATTR
	    ਜੇ (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ਸਤਰ_attr, area_attr);
		ਜੇ (!highlight_ਮੇਲ)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(ਖੋਜ_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਖੋਜ_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ਸਤਰ_attr, ਖੋਜ_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (wlv.ਸਤਰ_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.ਸਤਰ_attr);
# ਨਹੀਂਤਾਂ
		wlv.char_attr = wlv.ਸਤਰ_attr;
# endif
		attr_pri = FALSE;
	    }
#ਨਹੀਂਤਾਂ
	    ਜੇ (area_attr != 0)
		wlv.char_attr = area_attr;
	    ਨਹੀਂਤਾਂ ਜੇ (ਖੋਜ_attr != 0)
		wlv.char_attr = ਖੋਜ_attr;
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
	    {
		attr_pri = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#ਨਹੀਂਤਾਂ
		wlv.char_attr = 0;
#ਜੇ_ਅੰਤ
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    ਜੇ (ਪਾਠ_prop_type != ਨਿਰਾ && (ਪਾਠ_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, ਪਾਠ_prop_attr);
#ਜੇ_ਅੰਤ
	}

	// combine attribute with HLF_WIN
	ਜੇ (wlv.win_attr != 0)
	{
	    ਜੇ (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    ਨਹੀਂਤਾਂ
		wlv.char_attr = hl_combine_attr(wlv.win_attr, wlv.char_attr);
	}

	// Get the next character to put on the screen.

	// The "p_extra" points to the extra stuff that is inserted to
	// represent special characters (non-printable stuff) and other
	// things.  When all characters are the same, c_extra is used.
	// If wlv.c_final is set, it will compulsorily be used at the end.
	// "p_extra" must end in a NUL to avoid mb_ptr2len() reads past
	// "p_extra[n_extra]".
	// For the '$' of the 'list' option, n_extra == 1, p_extra == "".
	ਜੇ (wlv.n_extra > 0)
	{
	    ਜੇ (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		ਜੇ (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		ਨਹੀਂਤਾਂ
		    mb_utf8 = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		c = *wlv.p_extra;
		ਜੇ (has_mbyte)
		{
		    mb_c = c;
		    ਜੇ (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			ਜੇ (mb_l > wlv.n_extra)
			    mb_l = 1;
			ਨਹੀਂਤਾਂ ਜੇ (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			ਜੇ (mb_l >= wlv.n_extra)
			    mb_l = 1;
			ਨਹੀਂਤਾਂ ਜੇ (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    ਜੇ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    ਜੇ ((
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ਖਾਨਾ <= 0) :
#ਜੇ_ਅੰਤ
				    (wlv.ਖਾਨਾ >= wp->w_ਚੌੜਾਈ - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = HL_ATTR(HLF_AT);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			ਜੇ (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#ਜੇ_ਅੰਤ
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#ਜੇ defined(FEAT_PROP_POPUP)
	    ਜੇ (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		ਜੇ (wlv.saved_n_extra <= 0)
		{
		    ਜੇ (ਖੋਜ_attr == 0)
			ਖੋਜ_attr = saved_ਖੋਜ_attr;
		    ਜੇ (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    ਜੇ (wlv.extra_for_textprop)
			// wlv.extra_attr should be used at this position but
			// not any further.
			reset_extra_attr = TRUE;
		}

		wlv.extra_for_textprop = FALSE;
# ifdef FEAT_LINEBREAK
		ਵਿੱਚ_linebreak = FALSE;
# endif
	    }
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    ਪੂਰਨ		c0;
#ਜੇ_ਅੰਤ
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    c0 = *ptr;
#ਜੇ_ਅੰਤ
	    ਜੇ (c == NUL)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_ਸਤਰ = TRUE;
#ਜੇ_ਅੰਤ
		// no more cells to skip
		skip_cells = 0;
	    }

	    ਜੇ (has_mbyte)
	    {
		mb_c = c;
		ਜੇ (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    ਜੇ (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			ਜੇ (mb_c < 0x80)
			{
			    c = mb_c;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    c0 = mb_c;
#ਜੇ_ਅੰਤ
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			ਜੇ (utf_iscomposing(mb_c))
			{
			    ਪੂਰਨ i;

			    ਲਈ (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    ਜੇ ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			ਜੇ (wp->w_p_rl)		//ਉਲਟ
			    rl_mirror(wlv.extra);
#ਜੇ_ਅੰਤ
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (ਪੂਰਨ)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			ਜੇ (area_attr == 0 && ਖੋਜ_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
		    ਨਹੀਂਤਾਂ ਜੇ (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			ਪੂਰਨ	pc, pc1, nc;
			ਪੂਰਨ	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ਜੇ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			ਨਹੀਂਤਾਂ
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    ਨਹੀਂਤਾਂ
			prev_c = mb_c;
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    ਜੇ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    ਨਹੀਂਤਾਂ ਜੇ (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			ਜੇ (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_ਬਫਰ, wlv.extra, c);
				wlv.n_extra = (ਪੂਰਨ)STRLEN(wlv.extra) - 1;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				// illegal tail byte
				mb_l = 2;
				STRCPY(wlv.extra, "XX");
				wlv.n_extra = 1;
			    }
			    wlv.p_extra = wlv.extra;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    c = *wlv.p_extra++;
			    ਜੇ (area_attr == 0 && ਖੋਜ_attr == 0)
			    {
				n_attr = wlv.n_extra + 1;
				wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
				// save current attr
				saved_attr2 = wlv.char_attr;
			    }
			    mb_c = c;
			}
		    }
		}
		// If a double-width char doesn't fit display a '>' in the
		// last column; the character is displayed at the start of the
		// next line.
		ਜੇ ((
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ਖਾਨਾ <= 0) :
#ਜੇ_ਅੰਤ
				(wlv.ਖਾਨਾ >= wp->w_ਚੌੜਾਈ - 1))
			&& (*mb_char2cells)(mb_c) == 2)
		{
		    c = '>';
		    mb_c = c;
		    mb_utf8 = FALSE;
		    mb_l = 1;
		    multi_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		    // Put pointer back so that the character will be
		    // displayed at the start of the next line.
		    --ptr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		    did_decrement_ptr = TRUE;
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ ਜੇ (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		ਜੇ (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    ਜੇ (area_attr == 0 && ਖੋਜ_attr == 0)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(
						wlv.win_attr, HL_ATTR(HLF_AT));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    mb_utf8 = FALSE;
		    mb_l = 1;
		}

	    }
	    ++ptr;

	    ਜੇ (extra_check)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (ਲੰਮਾ)(ptr - ਸਤਰ);
		ਜੇ (spv->spv_has_spell && v >= ਸ਼ਬਦ_ਸਿਰਾ && v > cur_checked_ਖਾਨਾ)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    ਜੇ (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			ਪੂਰਨ	len;
			hlf_T	spell_hlf = HLF_ਗਿਣਤੀ;

			ਜੇ (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			ਜੇ ((prev_ptr - ਸਤਰ) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - ਸਤਰ) - nextlinecol;
			ਨਹੀਂਤਾਂ
			    p = prev_ptr;
			spv->spv_cap_ਖਾਨਾ -= (ਪੂਰਨ)(prev_ptr - ਸਤਰ);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_ਖਾਨਾ,
							    spv->spv_unchanged);
			ਸ਼ਬਦ_ਸਿਰਾ = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			ਜੇ (spell_hlf != HLF_ਗਿਣਤੀ
				&& (State & ਬਹੁਲਕ_ਪਾਓ)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ਖਾਨਾ >=
						    (colnr_T)(prev_ptr - ਸਤਰ)
				&& wp->w_cursor.ਖਾਨਾ < (colnr_T)ਸ਼ਬਦ_ਸਿਰਾ)
			{
			    spell_hlf = HLF_ਗਿਣਤੀ;
			    spell_redraw_lnum = lnum;
			}

			ਜੇ (spell_hlf == HLF_ਗਿਣਤੀ && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_ਖਾਨਾ = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			ਜੇ (spell_hlf != HLF_ਗਿਣਤੀ)
			    spell_attr = highlight_attr[spell_hlf];

			ਜੇ (spv->spv_cap_ਖਾਨਾ > 0)
			{
			    ਜੇ (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_ਖਾਨਾ
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_ਖਾਨਾ = ((p - nextline)
					    + spv->spv_cap_ਖਾਨਾ - nextline_idx);
			    }
			    ਨਹੀਂਤਾਂ
				// Compute the actual column.
				spv->spv_cap_ਖਾਨਾ += (prev_ptr - ਸਤਰ);
			}
		    }
		}
		ਜੇ (spell_attr != 0)
		{
		    ਜੇ (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    ਨਹੀਂਤਾਂ
			wlv.char_attr = hl_combine_attr(spell_attr,
								wlv.char_attr);
		}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// we don't want linebreak to apply for lines that start with
		// leading spaces, followed by long letters (since it would add
		// a break at the beginning of a line and this might be unexpected)
		//
		// So only allow to linebreak, once we have found chars not in
		// 'breakat' in the line.
		ਜੇ ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((ਪੂਰਨ)*ptr))
		    wlv.need_lbr = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Found last space before word: check for line break.
		ਜੇ (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((ਪੂਰਨ)*ptr))
		{
		    ਪੂਰਨ	    mb_off = has_mbyte ? (*mb_ਸਿਰ_off)(ਸਤਰ, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_ਪਹਿਲਾ_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, ਸਤਰ, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_ਪਾਠ = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ਨਿਰਾ, ਨਿਰਾ) - 1;
		    clear_chartabsize_arg(&cts);

		    ਜੇ (on_ਆਖਰੀ_ਖਾਨਾ && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			ਖੋਜ_attr = 0;

		    ਜੇ (c == TAB && wlv.n_extra + wlv.ਖਾਨਾ > wp->w_ਚੌੜਾਈ)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_ਬਫਰ->b_p_ts,
					      wp->w_ਬਫਰ->b_p_vts_array) - 1;
# ਨਹੀਂਤਾਂ
			wlv.n_extra = (ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts
				    - wlv.vcol % (ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    ਜੇ (wlv.n_extra > 0 && c != TAB)
			ਵਿੱਚ_linebreak = TRUE;
# endif
		    ਜੇ (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			ਜੇ (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			ਜੇ (!wp->w_p_list)
			    c = ' ';
		    }
		}
#ਜੇ_ਅੰਤ
		ਜੇ (wp->w_p_list)
		{
		    ਵਿੱਚ_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > ਸਤਰ && prev_ptr[-1] == ' '));
		    ਜੇ (!ਵਿੱਚ_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		ਜੇ (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (ਵਿੱਚ_multispace
					&& wp->w_lcs_chars.multispace != ਨਿਰਾ))
				&& ptr - ਸਤਰ >= leadcol
				&& ptr - ਸਤਰ <= trailcol)))
		{
		    ਜੇ (ਵਿੱਚ_multispace && wp->w_lcs_chars.multispace != ਨਿਰਾ)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			ਜੇ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    ਨਹੀਂਤਾਂ
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    ਜੇ (area_attr == 0 && ਖੋਜ_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ਜੇ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ਨਹੀਂਤਾਂ
			mb_utf8 = FALSE;
		}

		ਜੇ (c == ' ' && ((trailcol != MAXCOL && ptr > ਸਤਰ + trailcol)
				    || (leadcol != 0 && ptr < ਸਤਰ + leadcol)))
		{
		    ਜੇ (leadcol != 0 && ਵਿੱਚ_multispace && ptr < ਸਤਰ + leadcol
			    && wp->w_lcs_chars.leadmultispace != ਨਿਰਾ)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			ਜੇ (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    ਨਹੀਂਤਾਂ ਜੇ (ptr > ਸਤਰ + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    ਨਹੀਂਤਾਂ ਜੇ (ptr < ਸਤਰ + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    ਨਹੀਂਤਾਂ ਜੇ (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    ਜੇ (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ਜੇ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ਨਹੀਂਤਾਂ
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    ਜੇ (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		ਜੇ (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    ਪੂਰਨ	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    ਲੰਮਾ    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    ਪੂਰਨ	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    ਪੂਰਨ	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    ਪੂਰਨ	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ਜੇ (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < ਸਤਰ + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_ਮੁੱਲ(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    ਜੇ (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#ਜੇ_ਅੰਤ
		    // tab amount depends on current column
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_ਬਫਰ->b_p_ts,
					      wp->w_ਬਫਰ->b_p_vts_array) - 1;
#ਨਹੀਂਤਾਂ
		    tab_len = (ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts
			       - vcol_adjusted % (ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts - 1;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		    ਜੇ (!wp->w_p_lbr || !wp->w_p_list)
#ਜੇ_ਅੰਤ
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		    ਨਹੀਂਤਾਂ
		    {
			char_u	*p;
			ਪੂਰਨ	len;
			ਪੂਰਨ	i;
			ਪੂਰਨ	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			ਜੇ (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			ਜੇ (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			ਜੇ (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    ਪੂਰਨ tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    ਜੇ (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    ਜੇ (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    ਜੇ (p == ਨਿਰਾ)
				wlv.n_extra = 0;
			    ਨਹੀਂਤਾਂ
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				ਲਈ (i = 0; i < tab_len; i++)
				{
				    ਪੂਰਨ lcs = lcs_tab2;

				    ਜੇ (*p == NUL)
				    {
					tab_len = i;
					ਰੋਕੋ;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    ਜੇ (lcs_tab3 && i == tab_len - 1)
					lcs = lcs_tab3;
				    p += mb_char2bytes(lcs, p);
				    wlv.n_extra += mb_char2len(lcs)
						  - (saved_nextra > 0 ? 1 : 0);
				}
				wlv.p_extra = wlv.p_extra_free;
# ifdef FEAT_CONCEAL
				// n_extra will be increased by
				// FIX_FOX_BOGUSCOLS macro below, so need to
				// adjust for that here
				ਜੇ (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		    {
			ਪੂਰਨ vc_saved = wlv.vcol_off_co;

			// Tab alignment should be identical regardless of
			// 'conceallevel' value. So tab compensates of all
			// previous concealed characters, and thus resets
			// vcol_off_co and boguscols accumulated so far in the
			// line. Note that the tab can be longer than
			// 'tabstop' when there are concealed characters.
			FIX_FOR_BOGUSCOLS;

			// Make sure, the highlighting for the tab char will be
			// correctly set further below (effectively reverts the
			// FIX_FOR_BOGUSCOLS macro).
			ਜੇ (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#ਜੇ_ਅੰਤ
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    ਜੇ (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			ਜੇ (wp->w_p_lbr && wlv.p_extra != ਨਿਰਾ
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			ਜੇ (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (c == NUL
			&& wlv.n_extra == 0
			&& (wp->w_p_list
			    || (wlv.fromcol >= 0
				&& wlv.tocol > wlv.vcol
				&& VIsual_ਬਹੁਲਕ != Ctrl_V
				&& (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
				    wp->w_p_rl ? (wlv.ਖਾਨਾ >= 0) :
#ਜੇ_ਅੰਤ
				    (wlv.ਖਾਨਾ < wp->w_ਚੌੜਾਈ))))
			&& lcs_eol_one > 0)
		{
		    // Display a '$' after the line or highlight an extra
		    // character if the line break is included.
#ਜੇ defined(FEAT_DIFF) || defined(ਸਤਰ_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    ਜੇ (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef ਸਤਰ_ATTR
			    &&
#  endif
# endif
# ifdef ਸਤਰ_ATTR
			    wlv.ਸਤਰ_attr == 0
# endif
		       )
#ਜੇ_ਅੰਤ
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			ਜੇ (!(area_highlighting && virtual_ਸਰਗਰਮ()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    ਜੇ (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    ਨਹੀਂਤਾਂ
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    ਜੇ (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    ਜੇ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ਨਹੀਂਤਾਂ
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ਨਹੀਂਤਾਂ ਜੇ (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_ਬਫਰ, c);
		    ਜੇ (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    ਜੇ ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#ਜੇ_ਅੰਤ
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		    ਜੇ (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			ਜੇ (p == ਨਿਰਾ)
			    wlv.n_extra = 0;
			ਨਹੀਂਤਾਂ
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    ਜੇ (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			ਜੇ (ਪਾਠ_prop_type != ਨਿਰਾ &&
				 ਪਾਠ_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(ਪਾਠ_prop_attr,
							       wlv.extra_attr);
#ਜੇ_ਅੰਤ

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ
			 && (VIsual_ਬਹੁਲਕ == Ctrl_V
			     || VIsual_ਬਹੁਲਕ == 'v')
			 && virtual_ਸਰਗਰਮ()
			 && wlv.tocol != MAXCOL
			 && wlv.vcol < wlv.tocol
			 && (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ਖਾਨਾ >= 0) :
#ਜੇ_ਅੰਤ
			    (wlv.ਖਾਨਾ < wp->w_ਚੌੜਾਈ)))
		{
		    c = ' ';
		    --ptr;	    // put it back at the NUL
		}
#ਜੇ defined(ਸਤਰ_ATTR)
		ਨਹੀਂਤਾਂ ਜੇ ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.ਸਤਰ_attr != 0
			) && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ਖਾਨਾ >= 0) :
# endif
			    (wlv.ਖਾਨਾ
# ifdef FEAT_CONCEAL
				- wlv.boguscols
# endif
					    < wp->w_ਚੌੜਾਈ)))
		{
		    // Highlight until the right side of the window
		    c = ' ';
		    --ptr;	    // put it back at the NUL

		    // Remember we do the char for line highlighting.
		    ++did_ਸਤਰ_attr;

		    // don't do search HL for the rest of the line
		    ਜੇ (wlv.ਸਤਰ_attr != 0 && wlv.char_attr == ਖੋਜ_attr
					&& (did_ਸਤਰ_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.ਸਤਰ_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    ਜੇ (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_ਸਤਰ
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    ਜੇ (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			ਜੇ (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    ਜੇ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ਖੱਬਾ_curline_ਖਾਨਾ
					    && wlv.vcol <= ਸੱਜਾ_curline_ਖਾਨਾ)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    ਜੇ (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			ਜੇ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    ਜੇ (!wlv.cul_screenline
				    || (wlv.vcol >= ਖੱਬਾ_curline_ਖਾਨਾ
					     && wlv.vcol <= ਸੱਜਾ_curline_ਖਾਨਾ))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			ਨਹੀਂਤਾਂ ਜੇ (wlv.ਸਤਰ_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.ਸਤਰ_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			ਜੇ (wlv.vcol == 0)
			{
			    ਜੇ (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    ਨਹੀਂਤਾਂ ਜੇ (ਖੋਜ_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, ਖੋਜ_attr);
			}
		    }
# endif
		}
#ਜੇ_ਅੰਤ
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    ਜੇ (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_ਸਤਰ(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_ਮੇਲ_conc > 0)
		&& !(lnum_ਵਿੱਚ_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ਨਿਰਾ))
	    {
		ਪੂਰਨ syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		ਜੇ (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_ਮੇਲ_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_ਮੇਲ_conc && ਮੇਲ_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    ਜੇ (has_ਮੇਲ_conc && ਮੇਲ_conc)
			c = ਮੇਲ_conc;
		    ਨਹੀਂਤਾਂ ਜੇ (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    ਨਹੀਂਤਾਂ ਜੇ (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    ਨਹੀਂਤਾਂ
			c = ' ';

		    ਜੇ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    ਜੇ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ਨਹੀਂਤਾਂ
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    ਜੇ (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    ਜੇ (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			ਜੇ (wp->w_p_rl)
			{
			    wlv.ਖਾਨਾ -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			ਨਹੀਂਤਾਂ
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.ਖਾਨਾ += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		ਨਹੀਂਤਾਂ ਜੇ (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    ਜੇ (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#ਜੇ_ਅੰਤ // FEAT_CONCEAL
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	ਜੇ (!did_wcol && wlv.draw_state == WL_ਸਤਰ
		&& ਵਿੱਚ_curline && conceal_cursor_ਸਤਰ(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
		wp->w_wcol = wp->w_ਚੌੜਾਈ - wlv.ਖਾਨਾ + wlv.boguscols - 1;
	    ਨਹੀਂਤਾਂ
# endif
		wp->w_wcol = wlv.ਖਾਨਾ - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    ਜੇ (wlv.vcol + skip_cells < wp->w_virtcol)
		// Cursor beyond end of the line with 'virtualedit'.
		wp->w_wcol += wp->w_virtcol - wlv.vcol - skip_cells;
	    wp->w_wrow = wlv.ਪੰਕਤੀ;
	    did_wcol = TRUE;
	    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
# ifdef FEAT_PROP_POPUP
	    curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
# endif
	}
#ਜੇ_ਅੰਤ

	// Use "wlv.extra_attr", but don't override visual selection
	// highlighting, unless text property overrides.
	// Don't use "wlv.extra_attr" until wlv.n_attr_skip is zero.
	ਜੇ (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_ਸਤਰ
		&& (!attr_pri
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    || (ਪਾਠ_prop_flags & PT_FLAG_OVERRIDE)
#ਜੇ_ਅੰਤ
		   ))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸਤਰ_ATTR
	    ਜੇ (wlv.ਸਤਰ_attr)
		wlv.char_attr = hl_combine_attr(wlv.ਸਤਰ_attr, wlv.extra_attr);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		wlv.char_attr = wlv.extra_attr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#ਜੇ_ਅੰਤ
	}

#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	ਜੇ (p_imst == IM_ON_THE_SPOT
		&& xic != ਨਿਰਾ
		&& lnum == wp->w_cursor.lnum
		&& (State & ਬਹੁਲਕ_ਪਾਓ)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_ਸਤਰ)
	{
	    colnr_T tcol;

	    ਜੇ (preedit_ਸਿਰਾ_ਖਾਨਾ == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ਨਿਰਾ, ਨਿਰਾ, 0);
	    ਨਹੀਂਤਾਂ
		tcol = preedit_ਸਿਰਾ_ਖਾਨਾ;
	    ਜੇ ((ਲੰਮਾ)preedit_ਸ਼ੁਰੂ_ਖਾਨਾ <= wlv.vcol && wlv.vcol < (ਲੰਮਾ)tcol)
	    {
		ਜੇ (feedback_old_attr < 0)
		{
		    feedback_ਖਾਨਾ = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_ਖਾਨਾ);
		ਜੇ (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_ਖਾਨਾ++;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (feedback_old_attr >= 0)
	    {
		wlv.char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_ਖਾਨਾ = 0;
	    }
	}
#ਜੇ_ਅੰਤ
	// Handle the case where we are in column 0 but not on the first
	// character of the line and the user wants us to show us a
	// special character (via 'listchars' option "precedes:<char>").
	ਜੇ (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? (wp->w_skipcol > 0 && wlv.ਪੰਕਤੀ == 0)
				 : wp->w_leftcol > 0)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
		&& wlv.draw_state > WL_NR
		&& skip_cells <= 0
		&& c != NUL)
	{
	    c = wp->w_lcs_chars.prec;
	    lcs_prec_todo = NUL;
	    ਜੇ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Double-width character being overwritten by the "precedes"
		// character, need to fill up half the character.
		wlv.c_extra = MB_FILLER_CHAR;
		wlv.c_final = NUL;
		wlv.n_extra = 1;
		n_attr = 2;
		wlv.extra_attr =
				hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    }
	    mb_c = c;
	    ਜੇ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ਨਹੀਂਤਾਂ
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    ਜੇ (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	ਜੇ ((c == NUL
#ਜੇ defined(ਸਤਰ_ATTR)
		|| did_ਸਤਰ_attr == 1
#ਜੇ_ਅੰਤ
		) && wlv.eol_hl_off == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    ਪੂਰਨ prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_ਖੋਜ_hl,
					      (ਲੰਮਾ)(ptr - ਸਤਰ) - (c == NUL));
#ਜੇ_ਅੰਤ
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    ਜੇ (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_ਬਹੁਲਕ != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_ਸਰਗਰਮ))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# ਜੇ defined(ਸਤਰ_ATTR)
			    && did_ਸਤਰ_attr <= 1
# endif
			   )
#ਜੇ_ਅੰਤ
		       ))
	    {
		ਪੂਰਨ n = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		ਜੇ (wp->w_p_rl)
		{
		    ਜੇ (wlv.ਖਾਨਾ < 0)
			n = 1;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    ਜੇ (wlv.ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
			n = -1;
		}
		ਜੇ (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.ਖਾਨਾ += n;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Add a blank character to highlight.
		    Screen_ਸਤਰਾਂ[wlv.off] = ' ';
		    ਜੇ (enc_utf8)
			Screen_ਸਤਰਾਂ_UC[wlv.off] = 0;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		ਜੇ (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_ਖੋਜ_ਮੇਲ_hl(wp, &screen_ਖੋਜ_hl,
					   (ਲੰਮਾ)(ptr - ਸਤਰ), &wlv.char_attr);
		}
#ਜੇ_ਅੰਤ
		ScreenAttrs[wlv.off] = wlv.char_attr;
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv.off] = wlv.vcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		ਜੇ (wp->w_p_rl)
		{
		    --wlv.ਖਾਨਾ;
		    --wlv.off;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    ++wlv.ਖਾਨਾ;
		    ++wlv.off;
		}
		++wlv.vcol;
		wlv.eol_hl_off = 1;
	    }
	}

	// At end of the text line.
	ਜੇ (c == NUL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (ਪਾਠ_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		draw_screen_ਸਤਰ(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		ਜੇ (ਵਿੱਚ_curline)
		{
		    curwin->w_cline_ਪੰਕਤੀ = startrow;
		    curwin->w_cline_ਉਚਾਈ = wlv.ਪੰਕਤੀ - startrow;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    curwin->w_cline_folded = false;
#ਜੇ_ਅੰਤ
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		ਰੋਕੋ;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	ਪੂਰਨ lcs_ext = get_lcs_ext(wp);
	ਜੇ (lcs_ext != NUL
		&& wlv.draw_state == WL_ਸਤਰ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
		&& (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    wp->w_p_rl ? wlv.ਖਾਨਾ == 0 :
#ਜੇ_ਅੰਤ
		    wlv.ਖਾਨਾ == wp->w_ਚੌੜਾਈ - 1)
		&& (*ptr != NUL
		    || lcs_eol_one > 0
		    || (wlv.n_extra > 0 && (wlv.c_extra != NUL
						     || *wlv.p_extra != NUL))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    || ਪਾਠ_prop_ਅਗਲਾ <= ਆਖਰੀ_textprop_ਪਾਠ_idx
#ਜੇ_ਅੰਤ
		   ))
	{
	    c = lcs_ext;
	    wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    mb_c = c;
	    ਜੇ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ਨਹੀਂਤਾਂ
		mb_utf8 = FALSE;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	ਜੇ (wlv.draw_color_ਖਾਨਾ)
	    wlv.draw_color_ਖਾਨਾ = advance_color_ਖਾਨਾ(VCOL_HLC, &wlv.color_ਖਾਨੇ_ਗਿਣਤੀ);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	ਜੇ (((wlv.draw_state == WL_ਸਤਰ
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_ਵਿੱਚ_visual_area
		&& ਖੋਜ_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    ਜੇ (wp->w_p_cuc && VCOL_HLC == (ਲੰਮਾ)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (wlv.draw_color_ਖਾਨਾ && VCOL_HLC == *wlv.color_ਖਾਨੇ_ਗਿਣਤੀ)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#ਜੇ_ਅੰਤ

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	ਜੇ (wlv.draw_state < WL_ਸਤਰ || skip_cells <= 0)
	{
	    // Store the character.
#ਜੇ defined(FEAT_RIGHTLEFT)
	    ਜੇ (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.ਖਾਨਾ;
	    }
#ਜੇ_ਅੰਤ
	    Screen_ਸਤਰਾਂ[wlv.off] = c;
	    ਜੇ (enc_dbcs == DBCS_JPNU)
	    {
		ਜੇ ((mb_c & 0xff00) == 0x8e00)
		    Screen_ਸਤਰਾਂ[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
	    {
		ਜੇ (mb_utf8)
		{
		    ਪੂਰਨ i;

		    Screen_ਸਤਰਾਂ_UC[wlv.off] = mb_c;
		    ਜੇ ((c & 0xff) == 0)
			Screen_ਸਤਰਾਂ[wlv.off] = 0x80;   // avoid storing zero
		    ਲਈ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ਸਤਰਾਂ_C[i][wlv.off] = u8cc[i];
			ਜੇ (u8cc[i] == 0)
			    ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ
		    Screen_ਸਤਰਾਂ_UC[wlv.off] = 0;
	    }
	    ਜੇ (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    ਨਹੀਂਤਾਂ
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    ਜੇ (wlv.draw_state > WL_NR
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    && wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
		    )
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv.off] = wlv.vcol;
	    ਨਹੀਂਤਾਂ
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv.off] = -1;

	    ਜੇ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.ਖਾਨਾ;
		ਜੇ (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_ਸਤਰਾਂ[wlv.off] = 0;
		ਨਹੀਂਤਾਂ
		    // DBCS: Put second byte in the second screen char.
		    Screen_ਸਤਰਾਂ[wlv.off] = mb_c & 0xff;

		ਜੇ (wlv.draw_state > WL_NR
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			&& wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
			)
		    Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv.off] = ++wlv.vcol;
		ਨਹੀਂਤਾਂ
		    Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		ਜੇ (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		ਜੇ (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.ਖਾਨਾ;
		}
#ਜੇ_ਅੰਤ
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		++wlv.off;
		++wlv.ਖਾਨਾ;
	    }
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_p_cole > 0 && is_concealing)
	{
	    ਪੂਰਨ concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    ਜੇ (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    ਜੇ (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    ਜੇ (wp->w_p_wrap)
	    {
		// Special voodoo required if 'wrap' is on.
		//
		// Advance the column indicator to force the line
		// drawing to wrap early. This will make the line
		// take up the same screen space when parts are concealed,
		// so that cursor line computations aren't messed up.
		//
		// To avoid the fictitious advance of 'wlv.col' causing
		// trailing junk to be written out of the screen line
		// we are building, 'boguscols' keeps track of the number
		// of bad columns we have advanced.
		//
		// For popup windows with fixed width, don't advance col
		// to allow full text to be displayed.
# ifdef FEAT_PROP_POPUP
		ਪੂਰਨ adjust_ਖਾਨਾ = !WIN_IS_POPUP(wp);
# ਨਹੀਂਤਾਂ
		ਪੂਰਨ adjust_ਖਾਨਾ = TRUE;
# endif
		ਜੇ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    ਜੇ (wp->w_p_rl)
		    {
			ਜੇ (adjust_ਖਾਨਾ)
			{
			    wlv.ਖਾਨਾ -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    ਨਹੀਂਤਾਂ
# endif
		    {
			ਜੇ (adjust_ਖਾਨਾ)
			{
			    wlv.ਖਾਨਾ += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		ਜੇ (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    ਜੇ (wp->w_p_rl)
		    {
			ਜੇ (adjust_ਖਾਨਾ)
			{
			    --wlv.boguscols;
			    --wlv.ਖਾਨਾ;
			}
		    }
		    ਨਹੀਂਤਾਂ
# endif
		    {
			ਜੇ (adjust_ਖਾਨਾ)
			{
			    ++wlv.boguscols;
			    ++wlv.ਖਾਨਾ;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		ਜੇ (wp->w_p_rl)
		{
		    ਜੇ (adjust_ਖਾਨਾ)
		    {
			--wlv.boguscols;
			--wlv.ਖਾਨਾ;
		    }
		}
		ਨਹੀਂਤਾਂ
# endif
		{
		    ਜੇ (adjust_ਖਾਨਾ)
		    {
			++wlv.boguscols;
			++wlv.ਖਾਨਾ;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#ਜੇ_ਅੰਤ // FEAT_CONCEAL
	ਨਹੀਂਤਾਂ
	    --skip_cells;

	ਜੇ (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	ਜੇ (wlv.draw_state > WL_NR
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
		)
	    ++wlv.vcol;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#ਜੇ_ਅੰਤ

	// restore attributes after "precedes" in 'listchars'
	ਜੇ (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	ਜੇ (n_attr > 0 && wlv.draw_state == WL_ਸਤਰ
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	ਜੇ (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	ਜੇ ((
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.ਖਾਨਾ < 0) :
#ਜੇ_ਅੰਤ
				    (wlv.ਖਾਨਾ >= wp->w_ਚੌੜਾਈ))
		&& (wlv.draw_state != WL_ਸਤਰ
		    || *ptr != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    || wlv.filler_todo > 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    || ਪਾਠ_prop_above || ਪਾਠ_prop_follows
		    || ਪਾਠ_prop_ਅਗਲਾ <= ਆਖਰੀ_textprop_ਪਾਠ_idx
#ਜੇ_ਅੰਤ
		    || (wp->w_p_list && wp->w_lcs_chars.eol != NUL
						&& lcs_eol_one != -1)
		    || (wlv.n_extra != 0 && (wlv.c_extra != NUL
						      || *wlv.p_extra != NUL)))
		)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    wlv.ਖਾਨਾ -= wlv.boguscols;
	    // Apply 'cursorline' and HLF_WIN highlight.
	    ਜੇ (wlv.boguscols != 0 && (
# ifdef ਸਤਰ_ATTR
			wlv.ਸਤਰ_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		ਪੂਰਨ attr = wlv.win_attr;
# ifdef ਸਤਰ_ATTR
		ਜੇ (wlv.ਸਤਰ_attr != 0)
		    attr = hl_combine_attr(attr, wlv.ਸਤਰ_attr);
# endif
		ਜਦੋਂ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.ਖਾਨਾ >= 0 :
# endif
			    wlv.ਖਾਨਾ < wp->w_ਚੌੜਾਈ))
		{
		    Screen_ਸਤਰਾਂ[wlv.off] = ' ';
		    ਜੇ (enc_utf8)
			Screen_ਸਤਰਾਂ_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    Screen_ਖਾਨੇ_ਗਿਣਤੀ[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    ਜੇ (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.ਖਾਨਾ--;
			wlv.boguscols++;
		    }
		    ਨਹੀਂਤਾਂ
# endif
		    {
			wlv.off++;
			wlv.ਖਾਨਾ++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_ਸਤਰ(wp, &wlv, TRUE);
	    wlv.ਖਾਨਾ += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#ਨਹੀਂਤਾਂ
	    wlv_screen_ਸਤਰ(wp, &wlv, TRUE);
#ਜੇ_ਅੰਤ
	    ++wlv.ਪੰਕਤੀ;
	    ++wlv.screen_ਪੰਕਤੀ;

	    // When not wrapping and finished diff lines, break here.
	    ਜੇ (!wp->w_p_wrap
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			&& wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			&& !ਪਾਠ_prop_above
			&& !ਪਾਠ_prop_follows
#ਜੇ_ਅੰਤ
		       )
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (!wp->w_p_wrap && ਪਾਠ_prop_follows && !ਪਾਠ_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = ਸਤਰ + (size_t)ml_get_buf_len(wp->w_ਬਫਰ, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#ਜੇ_ਅੰਤ

	    // When the window is too narrow draw all "@" lines.
	    ਜੇ (wlv.draw_state != WL_ਸਤਰ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    && wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
		    )
	    {
		win_draw_ਸਿਰਾ(wp, '@', ' ', TRUE, wlv.ਪੰਕਤੀ, wp->w_ਉਚਾਈ, HLF_AT);
		draw_vsep_win(wp, wlv.ਪੰਕਤੀ);
		wlv.ਪੰਕਤੀ = endrow;
	    }

	    // When line got too long for screen break here.
	    ਜੇ (wlv.ਪੰਕਤੀ == endrow)
	    {
		++wlv.ਪੰਕਤੀ;
		ਰੋਕੋ;
	    }

	    ਜੇ (screen_cur_ਪੰਕਤੀ == wlv.screen_ਪੰਕਤੀ - 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		     && wlv.filler_todo <= 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		     && !ਪਾਠ_prop_above && !ਪਾਠ_prop_follows
#ਜੇ_ਅੰਤ
		     && wp->w_ਚੌੜਾਈ == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		ਸਤਰ_Wraps[wlv.screen_ਪੰਕਤੀ - 1] = TRUE;

		// Special trick to make copy/paste of wrapped lines work with
		// xterm/screen: write an extra character beyond the end of
		// the line. This will work with all terminal types
		// (regardless of the xn,am settings).
		// Only do this on a fast tty.
		// Only do this if the cursor is on the current line
		// (something has been written in it).
		// Don't do this for the GUI.
		// Don't do this for double-width characters.
		// Don't do this for a window not at the right screen border.
		ਜੇ (p_tf
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			 && !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
			 && !(has_mbyte
			     && ((*mb_off2cells)(ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ],
				   ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ - 1]
							    + (ਪੂਰਨ)topframe->fr_ਚੌੜਾਈ - 2,
				     ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    ਜੇ (screen_cur_ਖਾਨਾ != wp->w_ਚੌੜਾਈ)
			screen_char(ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ - 1]
						       + (ਬਿਨਾਨਿਸ਼ਾਨੀ)topframe->fr_ਚੌੜਾਈ - 1,
				       wlv.screen_ਪੰਕਤੀ - 1, (ਪੂਰਨ)(topframe->fr_ਚੌੜਾਈ - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    ਜੇ (has_mbyte && MB_BYTE2LEN(Screen_ਸਤਰਾਂ[ਸਤਰ_ਵਿੱਥ[
				     wlv.screen_ਪੰਕਤੀ - 1] + (topframe->fr_ਚੌੜਾਈ - 1)]) > 1)
			ਬਾਹਰਲਾ_char(' ');
		    ਨਹੀਂਤਾਂ
			ਬਾਹਰਲਾ_char(Screen_ਸਤਰਾਂ[ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ - 1]
							    + (topframe->fr_ਚੌੜਾਈ - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[ਸਤਰ_ਵਿੱਥ[wlv.screen_ਪੰਕਤੀ]] = (sattr_T)-1;
		    screen_ਸ਼ੁਰੂ();	// don't know where cursor is now
		}
	    }

	    win_ਸਤਰ_ਸ਼ੁਰੂ(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    ਜੇ (!wlv.dont_use_showbreak
# ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
# endif
	       )
		wlv.need_showbreak = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    --wlv.filler_todo;
	    // When the filler lines are actually below the last line of the
	    // file, don't draw the line itself, break here.
	    ਜੇ (wlv.filler_todo == 0 && wp->w_botfill)
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	}

    }	// for every character in the line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (ਪਾਠ_props != ਪਾਠ_props_buf)
	vim_free(ਪਾਠ_props);
    ਜੇ (ਪਾਠ_prop_idxs != ਪਾਠ_prop_idxs_buf)
	vim_free(ਪਾਠ_prop_idxs);
    ਜੇ (ਪਾਠ_prop_suffix_flags != ਪਾਠ_prop_suffix_flags_buf)
	vim_free(ਪਾਠ_prop_suffix_flags);
    vim_free(p_extra_free2);
#ਜੇ_ਅੰਤ

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    ਵਾਪਸ wlv.ਪੰਕਤੀ;
}
