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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lcs_ext(win_T *wp)
{
    ಆದರೆ (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	ಹಿಂತಿರುಗಿ NUL;
    ಆದರೆ (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	ಹಿಂತಿರುಗಿ '>';
    ಹಿಂತಿರುಗಿ wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
advance_color_ಲಂಬಸಾಲು(ಪೂರ್ಣಾಂಕ vcol, ಪೂರ್ಣಾಂಕ **color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    ಆಗ (**color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ >= 0 && vcol > **color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	++*color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ (**color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ >= 0);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
margin_columns_win(win_T *wp, ಪೂರ್ಣಾಂಕ *ಎಡ_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ *ಬಲ_ಲಂಬಸಾಲು)
{
    // cache previous calculations depending on w_virtcol
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ saved_w_virtcol;
    ಸ್ಥಿರ win_T *prev_wp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_width1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_width2;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_ಎಡ_ಲಂಬಸಾಲು;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_ಬಲ_ಲಂಬಸಾಲು;

    ಪೂರ್ಣಾಂಕ cur_ಲಂಬಸಾಲು_off = win_ಲಂಬಸಾಲು_off(wp);
    ಪೂರ್ಣಾಂಕ	width1;
    ಪೂರ್ಣಾಂಕ	width2;

    width1 = wp->w_ಅಗಲ - cur_ಲಂಬಸಾಲು_off;
    width2 = width1 + win_ಲಂಬಸಾಲು_off2(wp);

    ಆದರೆ (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*ಬಲ_ಲಂಬಸಾಲು = prev_ಬಲ_ಲಂಬಸಾಲು;
	*ಎಡ_ಲಂಬಸಾಲು = prev_ಎಡ_ಲಂಬಸಾಲು;
	ಹಿಂತಿರುಗಿ;
    }

    *ಎಡ_ಲಂಬಸಾಲು = 0;
    *ಬಲ_ಲಂಬಸಾಲು = width1;

    ಆದರೆ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ಬಲ_ಲಂಬಸಾಲು = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    ಆದರೆ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ಎಡ_ಲಂಬಸಾಲು = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ಎಡ_ಲಂಬಸಾಲು = *ಎಡ_ಲಂಬಸಾಲು;
    prev_ಬಲ_ಲಂಬಸಾಲು = *ಬಲ_ಲಂಬಸಾಲು;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define ಸಾಲು_ATTR
#ಆದರೆ_ಅಂತ್ಯ

// structure with variables passed between win_line() and other functions
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    ಪೂರ್ಣಾಂಕ		startrow;	// first row in the window to be drawn
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;		// row in the window, excl w_winrow
    ಪೂರ್ಣಾಂಕ		screen_ಅಡ್ಡಸಾಲು;	// row on the screen, incl w_winrow

    ದೀರ್ಘ	vcol;		// virtual column, before wrapping
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;		// visual column on screen, after wrapping
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    ಪೂರ್ಣಾಂಕ		vcol_off_co;	// offset for concealed characters
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		vcol_off_tp;	// offset for virtual text
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		draw_color_ಲಂಬಸಾಲು;	// highlight colorcolumn
    ಪೂರ್ಣಾಂಕ		*color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;	// pointer to according columns array
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		eol_hl_off;	// 1 if highlighted char after EOL

    ಚಿಹ್ನೆರಹಿತ	off;		// offset in ScreenLines/ScreenAttrs

    ಪೂರ್ಣಾಂಕ		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		cul_attr;	// set when 'cursorline' active
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಾಲು_ATTR
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_attr;	// for the whole line, includes 'cursorline'
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		screen_ಸಾಲು_flags;  // flags for screen_line()

    ಪೂರ್ಣಾಂಕ		fromcol;	// start of inverting
    ಪೂರ್ಣಾಂಕ		tocol;		// end of inverting

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ದೀರ್ಘ	vcol_sbr;	    // virtual column after showbreak
    ಪೂರ್ಣಾಂಕ		need_showbreak;	    // overlong line, skipping first x chars
    ಪೂರ್ಣಾಂಕ		dont_use_showbreak; // do not use 'showbreak'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_above_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    ಪೂರ್ಣಾಂಕ		cul_screenline;

    ಪೂರ್ಣಾಂಕ		char_attr;	// attributes for the next character

    ಪೂರ್ಣಾಂಕ		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    ಪೂರ್ಣಾಂಕ		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    ಪೂರ್ಣಾಂಕ		n_attr_skip;    // chars to skip before using extra_attr
    ಪೂರ್ಣಾಂಕ		c_extra;	// extra chars, all the same
    ಪೂರ್ಣಾಂಕ		c_final;	// final char, mandatory if set
    ಪೂರ್ಣಾಂಕ		extra_for_textprop; // n_extra set for textprop
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    ಪೂರ್ಣಾಂಕ		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    ಪೂರ್ಣಾಂಕ		saved_extra_attr;
    ಪೂರ್ಣಾಂಕ		saved_n_attr_skip;
    ಪೂರ್ಣಾಂಕ		saved_extra_for_textprop;
    ಪೂರ್ಣಾಂಕ		saved_c_extra;
    ಪೂರ್ಣಾಂಕ		saved_c_final;
    ಪೂರ್ಣಾಂಕ		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		filler_ಸಾಲುಗಳು;	// nr of filler lines to be drawn
    ಪೂರ್ಣಾಂಕ		filler_todo;	// nr of filler lines still to do + 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    sign_attrs_T sattr;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    ಪೂರ್ಣಾಂಕ		need_lbr;
#ಆದರೆ_ಅಂತ್ಯ
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#ವ್ಯಾಖ್ಯಾನಿಸು WL_ಆರಂಭ	0		// nothing done yet, must be zero
#ವ್ಯಾಖ್ಯಾನಿಸು WL_CMDLINE	(WL_ಆರಂಭ + 1)	// cmdline window column
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#ಇಲ್ಲದಿದ್ದರೆ
# define WL_FOLD	WL_CMDLINE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#ಇಲ್ಲದಿದ್ದರೆ
# define WL_SIGN	WL_FOLD		// column for signs
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು WL_NR		(WL_SIGN + 1)	// line number
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#ಇಲ್ಲದಿದ್ದರೆ
# define WL_BRI		WL_NR
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#ಇಲ್ಲದಿದ್ದರೆ
# define WL_SBR		WL_BRI
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು WL_ಸಾಲು		(WL_SBR + 1)	// text in the line

#ಆದರೆ defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
use_cursor_ಸಾಲು_highlight(win_T *wp, linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    ಪೂರ್ಣಾಂಕ fdc = compute_foldcolumn(wp, 0);

    ಆದರೆ (fdc <= 0)
	ಹಿಂತಿರುಗಿ;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    ಆದರೆ (wlv->p_extra_free == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    wlv->n_extra = (ಪೂರ್ಣಾಂಕ)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    ಆದರೆ (use_cursor_ಸಾಲು_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    ಇಲ್ಲದಿದ್ದರೆ
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_FC));
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
/*
 * Get information needed to display the sign in line "wlv->lnum" in window
 * "wp".
 * If "nrcol" is TRUE, the sign is going to be displayed in the number column.
 * Otherwise the sign is going to be displayed in the sign column.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_sign_display_info(
	ಪೂರ್ಣಾಂಕ		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    ಪೂರ್ಣಾಂಕ	ಪಠ್ಯ_sign;
# ifdef FEAT_SIGN_ICONS
    ಪೂರ್ಣಾಂಕ	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    ಆದರೆ (nrcol)
	wlv->n_extra = ಸಂಖ್ಯೆ_ಅಗಲ(wp) + 1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (use_cursor_ಸಾಲು_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	ಇಲ್ಲದಿದ್ದರೆ
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    ಆದರೆ (wlv->ಅಡ್ಡಸಾಲು == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_ಸಾಲುಗಳು && wlv->filler_todo <= 0
# endif
       )
    {
	ಪಠ್ಯ_sign = (wlv->sattr.sat_ಪಠ್ಯ != ಖಾಲಿ) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ಖಾಲಿ) ? wlv->sattr.sat_typenr : 0;
	ಆದರೆ (gui.ಒಳ_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    ಆದರೆ (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((ಅಕ್ಷರ *)wlv->extra, ಗಾತ್ರ(wlv->extra),
						"%-*c ", ಸಂಖ್ಯೆ_ಅಗಲ(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    ಆದರೆ (netbeans_ಸಕ್ರಿಯ() && (buf_signcount(wp->w_ಬಫರ್, wlv->lnum)
									  > 1))
	    {
		ಆದರೆ (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((ಅಕ್ಷರ *)wlv->extra, ಗಾತ್ರ(wlv->extra),
						"%-*c ", ಸಂಖ್ಯೆ_ಅಗಲ(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    ಆದರೆ (ಪಠ್ಯ_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_ಪಠ್ಯ;
		ಆದರೆ (wlv->p_extra != ಖಾಲಿ)
		{
		    ಆದರೆ (nrcol)
		    {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = ಸಂಖ್ಯೆ_ಅಗಲ(wp) - 2;

			vim_memset(wlv->extra, ' ', ಅಗಲ);
			wlv->n_extra = ಅಗಲ;
			wlv->n_extra += vim_snprintf((ಅಕ್ಷರ *)wlv->extra + ಅಗಲ,
				  ಗಾತ್ರ(wlv->extra) - ಅಗಲ, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			wlv->n_extra = (ಪೂರ್ಣಾಂಕ)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		ಆದರೆ (use_cursor_ಸಾಲು_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		ಇಲ್ಲದಿದ್ದರೆ
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_lnum_ಲಂಬಸಾಲು(
	win_T		*wp,
	winlinevars_T	*wlv,
	ಪೂರ್ಣಾಂಕ		sign_present ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_attr ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ lnum_ಅಡ್ಡಸಾಲು = wlv->startrow + wlv->filler_ಸಾಲುಗಳು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		      + wlv->ಪಠ್ಯ_prop_above_ಎಣಿಕೆ
#ಆದರೆ_ಅಂತ್ಯ
		      ;

    ಆದರೆ ((wp->w_p_nu || wp->w_p_rnu)
	     && (wlv->ಅಡ್ಡಸಾಲು <= lnum_ಅಡ್ಡಸಾಲು || !has_cpo_n)
	     // there is no line number in a wrapped line when "n" is in
	     // 'cpoptions', but 'breakindent' assumes it anyway.
	     && !((has_cpo_n
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		     && !wp->w_p_bri
#ಆದರೆ_ಅಂತ್ಯ
		  ) && wp->w_skipcol > 0 && wlv->lnum == wp->w_topline))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	// If 'signcolumn' is set to 'number' and a sign is present
	// in 'lnum', then display the sign instead of the line
	// number.
	ಆದರೆ ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_ಪಠ್ಯ != ಖಾಲಿ)
	    get_sign_display_info(TRUE, wp, wlv);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  ಆದರೆ (wlv->ಅಡ್ಡಸಾಲು == lnum_ಅಡ್ಡಸಾಲು
		    && (wp->w_skipcol == 0 || wlv->ಅಡ್ಡಸಾಲು > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      ದೀರ್ಘ ಅಂಕೆ;
	      ಅಕ್ಷರ *fmt = "%*ld ";

	      ಆದರೆ (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  ಅಂಕೆ = (ದೀರ್ಘ)wlv->lnum;
	      ಇಲ್ಲದಿದ್ದರೆ
	      {
		  // 'relativenumber', don't use negative numbers
		  ಅಂಕೆ = labs((ದೀರ್ಘ)get_cursor_rel_lnum(wp, wlv->lnum));
		  ಆದರೆ (ಅಂಕೆ == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      ಅಂಕೆ = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((ಅಕ್ಷರ *)wlv->extra, ಗಾತ್ರ(wlv->extra), fmt, ಸಂಖ್ಯೆ_ಅಗಲ(wp), ಅಂಕೆ);
	      ಆದರೆ (wp->w_skipcol > 0 && wlv->startrow == 0)
		  ಗಾಗಿ (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	      ಆದರೆ (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  ಪೂರ್ಣಾಂಕ	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  ಗಾಗಿ (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
		  {
		      t = *p1;
		      *p1 = *p2;
		      *p2 = t;
		  }
	      }
#ಆದರೆ_ಅಂತ್ಯ
	      wlv->p_extra = wlv->extra;
	      wlv->c_extra = NUL;
	      wlv->c_final = NUL;
	  }
	  ಇಲ್ಲದಿದ್ದರೆ
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = ಸಂಖ್ಯೆ_ಅಗಲ(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  ಆದರೆ (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->ಅಡ್ಡಸಾಲು == lnum_ಅಡ್ಡಸಾಲು
		      || (wlv->ಅಡ್ಡಸಾಲು > lnum_ಅಡ್ಡಸಾಲು
			 && (wp->w_p_culopt_flags & CULOPT_ಸಾಲು))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#ಆದರೆ_ಅಂತ್ಯ
	  ಆದರೆ (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  ಆದರೆ (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	ಆದರೆ (ಅಂಕೆ_attr)
	    wlv->char_attr = ಅಂಕೆ_attr;
#ಆದರೆ_ಅಂತ್ಯ
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    ಆದರೆ (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_ಮೌಲ್ಯ(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    ಆದರೆ (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	ಆದರೆ (wp->w_p_bri && (wlv->ಅಡ್ಡಸಾಲು > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_ಸಾಲುಗಳು
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    ಆದರೆ (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ಖಾಲಿ;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_ಬಫರ್, wlv->lnum, FALSE));
	    ಆದರೆ (wlv->ಅಡ್ಡಸಾಲು == wlv->startrow)
	    {
		wlv->n_extra -= win_ಲಂಬಸಾಲು_off2(wp);
		ಆದರೆ (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    ಆದರೆ (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    ಆದರೆ (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	ಆದರೆ (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    ಆದರೆ (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	ಆದರೆ (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	ಆದರೆ (wp->w_p_rl)
	    wlv->n_extra = wlv->ಲಂಬಸಾಲು + 1;
	ಇಲ್ಲದಿದ್ದರೆ
#  endif
	    wlv->n_extra = wp->w_ಅಗಲ - wlv->ಲಂಬಸಾಲು;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_ಮೌಲ್ಯ(wp);
    ಆದರೆ (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (ಪೂರ್ಣಾಂಕ)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	ಆದರೆ (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	ಆದರೆ (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	ಆದರೆ (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    ಆದರೆ (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
textprop_ಗಾತ್ರ_after_trunc(
	win_T	*wp,
	ಪೂರ್ಣಾಂಕ	flags,	    // TP_FLAG_ALIGN_*
	ಪೂರ್ಣಾಂಕ	added,
	ಪೂರ್ಣಾಂಕ	padding,
	char_u	*ಪಠ್ಯ,
	ಪೂರ್ಣಾಂಕ	*n_ಬಳಸಿದ_ptr)
{
    ಪೂರ್ಣಾಂಕ	space = (flags & (TP_FLAG_ALIGN_ಕೆಳಗೆ | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp) : added;
    ಪೂರ್ಣಾಂಕ strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    ಆದರೆ (space < PROP_ಪಠ್ಯ_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_ಅಗಲ;
    ಆದರೆ (flags & (TP_FLAG_ALIGN_ಕೆಳಗೆ | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    ಗಾಗಿ (p = ಪಠ್ಯ; *p != NUL; p += (*mb_ptr2len)(p))
    {
	ಪೂರ್ಣಾಂಕ clen = ptr2cells(p);

	ಆದರೆ (strsize + clen > space)
	    ನಿಲ್ಲಿಸು;
	strsize += clen;
    }
    *n_ಬಳಸಿದ_ptr = (ಪೂರ್ಣಾಂಕ)(p - ಪಠ್ಯ);

    ಹಿಂತಿರುಗಿ strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	ಪೂರ್ಣಾಂಕ	    vcol,	    // current text column
	ಪೂರ್ಣಾಂಕ	    scr_ಲಂಬಸಾಲು,	    // current screen column
	ಪೂರ್ಣಾಂಕ	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	ಪೂರ್ಣಾಂಕ	    *n_attr,	    // attribute cells, NULL if not used
	ಪೂರ್ಣಾಂಕ	    *n_attr_skip,   // cells to skip attr, NULL if not used
	ಪೂರ್ಣಾಂಕ	    do_skip)	    // skip_cells is not zero
{
    ಪೂರ್ಣಾಂಕ	    ಬಲ_2 = (tp->tp_flags & TP_FLAG_ALIGN_ಬಲ);
    ಪೂರ್ಣಾಂಕ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ಪೂರ್ಣಾಂಕ	    ಕೆಳಗೆ = (tp->tp_flags & TP_FLAG_ALIGN_ಕೆಳಗೆ);
    ಪೂರ್ಣಾಂಕ	    wrap = tp->tp_ಲಂಬಸಾಲು < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    ಪೂರ್ಣಾಂಕ	    padding = tp->tp_ಲಂಬಸಾಲು == MAXCOL ? tp->tp_padleft : 0;
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_with_padding = scr_ಲಂಬಸಾಲು + (ಕೆಳಗೆ ? 0 : padding);
    ಪೂರ್ಣಾಂಕ	    ಸ್ಥಳ = wp->w_ಅಗಲ - ಲಂಬಸಾಲು_with_padding;
    ಪೂರ್ಣಾಂಕ	    before = ಸ್ಥಳ;	// spaces before the text
    ಪೂರ್ಣಾಂಕ	    after = 0;		// spaces after the text
    ಪೂರ್ಣಾಂಕ	    n_ಬಳಸಿದ = *n_extra;
    char_u  *l = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    strsize = vim_strsize(*p_extra);
    ಪೂರ್ಣಾಂಕ	    cells = wrap ? strsize : textprop_ಗಾತ್ರ_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_ಬಳಸಿದ);

    ಆದರೆ (wrap || ಬಲ_2 || above || ಕೆಳಗೆ || padding > 0 || n_ಬಳಸಿದ < *n_extra)
    {
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_off = win_ಲಂಬಸಾಲು_off(wp) - win_ಲಂಬಸಾಲು_off2(wp);

	ಆದರೆ (above)
	{
	    before = 0;
	    after = wp->w_ಅಗಲ - cells - win_ಲಂಬಸಾಲು_off(wp) - padding;
	    ಆದರೆ (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Right-align: fill with before
	    ಆದರೆ (ಬಲ_2)
		before -= cells;

	    // Below-align: empty line add one character
	    ಆದರೆ (ಕೆಳಗೆ && vcol == 0 && ಲಂಬಸಾಲು_with_padding == ಲಂಬಸಾಲು_off
					    && wp->w_ಅಗಲ - ಲಂಬಸಾಲು_off == before)
		ಲಂಬಸಾಲು_with_padding += 1;

	    ಆದರೆ (before < 0
		    || !(ಬಲ_2 || ಕೆಳಗೆ)
		    || (ಕೆಳಗೆ ? (ಲಂಬಸಾಲು_with_padding <= ಲಂಬಸಾಲು_off || !wp->w_p_wrap)
			      : (n_ಬಳಸಿದ < *n_extra)))
	    {
		ಆದರೆ (ಬಲ_2 && (wrap
			      || (ಸ್ಥಳ < PROP_ಪಠ್ಯ_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_ಅಗಲ - ಲಂಬಸಾಲು_off - strsize + ಸ್ಥಳ;
		    ಆದರೆ (before < 0)
			before = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
			n_ಬಳಸಿದ = *n_extra;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೆಳಗೆ && before > vcol && do_skip)
		    before -= vcol;
		ಇಲ್ಲದಿದ್ದರೆ
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	ಆದರೆ (n_ಬಳಸಿದ < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_ಬಳಸಿದ;

	// add 1 for NUL, 2 for when '…' is used
	ಆದರೆ (n_attr != ಖಾಲಿ)
	    l = alloc(n_ಬಳಸಿದ + before + after + (padding > 0 ? padding : 0) + 3);
	ಆದರೆ (n_attr == ಖಾಲಿ || l != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ off = 0;

	    ಆದರೆ (n_attr != ಖಾಲಿ)
	    {
		vim_memset(l, ' ', before);
		off += before;
		ಆದರೆ (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_ಬಳಸಿದ);
		off += n_ಬಳಸಿದ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		off = before + after + padding + n_ಬಳಸಿದ;
		cells += before + after + padding;
	    }
	    ಆದರೆ (n_attr != ಖಾಲಿ)
	    {
		ಆದರೆ (n_ಬಳಸಿದ < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    ಆದರೆ (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			ಆದರೆ (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_ಸಿದ್ಧತೆ(&vc, (char_u *)"utf-8", p_enc);
			    ಆದರೆ (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ಖಾಲಿ);
				ಆದರೆ (cp == ಖಾಲಿ)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_ಸಿದ್ಧತೆ(&vc, ಖಾಲಿ, ಖಾಲಿ);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_ತಲೆ_off)(l, lp);
			STRCPY(lp, cp);
			n_ಬಳಸಿದ = lp - l + 3 - before - padding;
			ಆದರೆ (cp != buf)
			    vim_free(cp);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// change last character to '>'
			*lp = '>';
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (after > 0)
		{
		    vim_memset(l + off, ' ', after);
		    l[off + after] = NUL;
		}

		*p_extra = l;
		*n_extra = n_ಬಳಸಿದ + before + after + padding;
		*n_attr = mb_charlen(*p_extra);
		// n_attr_skip will not be decremented before draw_state is
		// WL_LINE
		*n_attr_skip = before + (padding > 0 ? padding : 0);
		*n_attr -= *n_attr_skip;
		ಆದರೆ (above)
		    *n_attr -= after;
	    }
	}
    }

    ಆದರೆ (n_attr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ cells;
    ಹಿಂತಿರುಗಿ (ಕೆಳಗೆ && ಲಂಬಸಾಲು_with_padding > win_ಲಂಬಸಾಲು_off(wp) && !wp->w_p_wrap);
}

# ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_prop_no_showbreak(textprop_T *tp)
{
    ಪೂರ್ಣಾಂಕ	    ಬಲ_2 = (tp->tp_flags & TP_FLAG_ALIGN_ಬಲ);
    ಪೂರ್ಣಾಂಕ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ಪೂರ್ಣಾಂಕ	    ಕೆಳಗೆ = (tp->tp_flags & TP_FLAG_ALIGN_ಕೆಳಗೆ);
    ಪೂರ್ಣಾಂಕ	    wrap = tp->tp_ಲಂಬಸಾಲು < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    ಹಿಂತಿರುಗಿ (ಬಲ_2 || above || ಕೆಳಗೆ || !wrap);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wlv_screen_ಸಾಲು(win_T *wp, winlinevars_T *wlv, ಪೂರ್ಣಾಂಕ clear_ಕೊನೆ)
{
    ಆದರೆ (wlv->ಅಡ್ಡಸಾಲು == 0 && wp->w_skipcol > 0
#ಆದರೆ defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_ಮೌಲ್ಯ(wp) == NUL
#ಆದರೆ_ಅಂತ್ಯ
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	ಪೂರ್ಣಾಂಕ off = (ಪೂರ್ಣಾಂಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
	ಪೂರ್ಣಾಂಕ max_off = off + screen_Columns;
	ಪೂರ್ಣಾಂಕ skip = 0;

	ಆದರೆ (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    ಆಗ (skip < wp->w_ಅಗಲ && VIM_ISDIGIT(Screen_ಸಾಲುಗಳು[off]))
	    {
		++off;
		++skip;
	    }

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3 && i + skip < wp->w_ಅಗಲ; ++i)
	{
	    ಆದರೆ ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_ಸಾಲುಗಳು[off + 1] = ' ';
	    Screen_ಸಾಲುಗಳು[off] = '<';
	    ಆದರೆ (enc_utf8)
		Screen_ಸಾಲುಗಳು_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_ಸಾಲು(wp, wlv->screen_ಅಡ್ಡಸಾಲು, wp->w_wincol, wlv->ಲಂಬಸಾಲು,
		    clear_ಕೊನೆ ? wp->w_ಅಗಲ : -wp->w_ಅಗಲ,
		    wlv->vcol - 1, wlv->screen_ಸಾಲು_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_screen_ಸಾಲು(win_T *wp, winlinevars_T *wlv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ದೀರ್ಘ	v;
    ಪೂರ್ಣಾಂಕ		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    ಆದರೆ (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    ಇಲ್ಲದಿದ್ದರೆ
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_ಅಗಲ - wlv->ಲಂಬಸಾಲು - 1 :
# endif
	wlv->ಲಂಬಸಾಲು;
    // check if line ends before left margin
    ಆದರೆ (wlv->vcol < v + wcol - win_ಲಂಬಸಾಲು_off(wp))
	wlv->vcol = v + wcol - win_ಲಂಬಸಾಲು_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->ಲಂಬಸಾಲು -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# ಇಲ್ಲದಿದ್ದರೆ
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    ಆದರೆ (wlv->draw_color_ಲಂಬಸಾಲು)
	wlv->draw_color_ಲಂಬಸಾಲು = advance_color_ಲಂಬಸಾಲು(VCOL_HLC, &wlv->color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    ಆದರೆ (((wp->w_p_cuc
		    && (ಪೂರ್ಣಾಂಕ)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (ಪೂರ್ಣಾಂಕ)wp->w_virtcol <
			 (ದೀರ್ಘ)wp->w_ಅಗಲ * (wlv->ಅಡ್ಡಸಾಲು - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_ಲಂಬಸಾಲು
# ifdef ಸಾಲು_ATTR
		|| wlv->ಸಾಲು_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	ಪೂರ್ಣಾಂಕ	rightmost_vcol = 0;
	ಪೂರ್ಣಾಂಕ	i;

	ಆದರೆ (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	ಆದರೆ (wlv->draw_color_ಲಂಬಸಾಲು)
	    // determine rightmost colorcolumn to possibly draw
	    ಗಾಗಿ (i = 0; wlv->color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[i] >= 0; ++i)
		ಆದರೆ (rightmost_vcol < wlv->color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[i])
		    rightmost_vcol = wlv->color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[i];

	ಆಗ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->ಲಂಬಸಾಲು >= 0) :
# endif
		(wlv->ಲಂಬಸಾಲು < wp->w_ಅಗಲ))
	{
	    Screen_ಸಾಲುಗಳು[wlv->off] = ' ';
	    ಆದರೆ (enc_utf8)
		Screen_ಸಾಲುಗಳು_UC[wlv->off] = 0;

	    ಆದರೆ (wlv->draw_color_ಲಂಬಸಾಲು)
		wlv->draw_color_ಲಂಬಸಾಲು = advance_color_ಲಂಬಸಾಲು(
						   VCOL_HLC, &wlv->color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

	    ಪೂರ್ಣಾಂಕ attr = wlv->win_attr;
# ifdef ಸಾಲು_ATTR
	    ಆದರೆ (wlv->ಸಾಲು_attr != 0)
		attr = hl_combine_attr(attr, wlv->ಸಾಲು_attr);
# endif
	    ಆದರೆ (wp->w_p_cuc && VCOL_HLC == (ದೀರ್ಘ)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wlv->draw_color_ಲಂಬಸಾಲು && VCOL_HLC == *wlv->color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
	    {
		++wlv->off;
		++wlv->ಲಂಬಸಾಲು;
	    }
	    ++wlv->vcol;

	    ಆದರೆ (VCOL_HLC > rightmost_vcol
# ifdef ಸಾಲು_ATTR
		    && wlv->ಸಾಲು_attr == 0
# endif
		    && wlv->win_attr == 0)
		ನಿಲ್ಲಿಸು;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_ಸಾಲು_flags |= SLF_INC_VCOL;
    wlv_screen_ಸಾಲು(wp, wlv, TRUE);
    wlv->screen_ಸಾಲು_flags &= ~SLF_INC_VCOL;
    ++wlv->ಅಡ್ಡಸಾಲು;
    ++wlv->screen_ಅಡ್ಡಸಾಲು;
}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_ಸಾಲು_ಆರಂಭ(win_T *wp ಬಳಸದ, winlinevars_T *wlv, ಪೂರ್ಣಾಂಕ save_extra)
{
    wlv->ಲಂಬಸಾಲು = 0;
    wlv->off = (ಚಿಹ್ನೆರಹಿತ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->ಲಂಬಸಾಲು = wp->w_ಅಗಲ - 1;
	wlv->off += wlv->ಲಂಬಸಾಲು;
	wlv->screen_ಸಾಲು_flags |= SLF_RIGHTLEFT;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_ಆರಂಭ;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ಖಾಲಿ;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    wlv->saved_char_attr = 0;

	// these are not used until restored in win_line_continue()
	wlv->n_extra = 0;
	wlv->n_attr_skip = 0;
    }
}

/*
 * Called when wlv->draw_state is set to WL_LINE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_ಸಾಲು_continue(winlinevars_T *wlv)
{
    ಆದರೆ (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ಖಾಲಿ;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	wlv->char_attr = wlv->win_attr;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_cursorline_highlight(
	winlinevars_T *wlv,
	ಪೂರ್ಣಾಂಕ sign_present ಬಳಸದ)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    ಆದರೆ (sign_present && wlv->sattr.sat_linehl > 0)
    {
	ಆದರೆ (wlv->sattr.sat_ಆದ್ಯತೆ >= 100)
	    wlv->ಸಾಲು_attr = hl_combine_attr(wlv->cul_attr, wlv->ಸಾಲು_attr);
	ಇಲ್ಲದಿದ್ದರೆ
	    wlv->ಸಾಲು_attr = hl_combine_attr(wlv->ಸಾಲು_attr, wlv->cul_attr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ಆದರೆ defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->ಸಾಲು_attr = hl_combine_attr(wlv->cul_attr, wlv->ಸಾಲು_attr);
# ಇಲ್ಲದಿದ್ದರೆ
	wlv->ಸಾಲು_attr = wlv->cul_attr;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
win_ಸಾಲು(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		startrow,
    ಪೂರ್ಣಾಂಕ		endrow,
    ಪೂರ್ಣಾಂಕ		ಸಂಖ್ಯೆ_only,
    spellvars_T	*spv ಬಳಸದ)
{
    winlinevars_T	wlv;		// variables passed between functions

    ಪೂರ್ಣಾಂಕ		c = 0;			// init for GCC
    char_u	*ಸಾಲು;			// current line
    char_u	*ptr;			// current position in "line"
    ಪೂರ್ಣಾಂಕ		ಒಳ_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ಖಾಲಿ;   // another p_extra to be freed
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    ಪೂರ್ಣಾಂಕ		ಒಳ_linebreak = FALSE;	// n_extra set for showing linebreak
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    ಪೂರ್ಣಾಂಕ		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    ಪೂರ್ಣಾಂಕ		n_attr = 0;	    // chars with special attr
    ಪೂರ್ಣಾಂಕ		saved_attr2 = 0;    // char_attr saved for n_attr
    ಪೂರ್ಣಾಂಕ		n_attr3 = 0;	    // chars with overruling special attr
    ಪೂರ್ಣಾಂಕ		saved_attr3 = 0;    // char_attr saved for n_attr3

    ಪೂರ್ಣಾಂಕ		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    ಪೂರ್ಣಾಂಕ		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    ಪೂರ್ಣಾಂಕ		lnum_ಒಳ_visual_area = FALSE;
    pos_T	pos;
    ದೀರ್ಘ	v;

    ಪೂರ್ಣಾಂಕ		attr_pri = FALSE;	// char_attr has priority
    ಪೂರ್ಣಾಂಕ		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    ಪೂರ್ಣಾಂಕ		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    ಪೂರ್ಣಾಂಕ		area_attr = 0;		// attributes desired by highlighting
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    ಪೂರ್ಣಾಂಕ		syntax_attr = 0;	// attributes desired by syntax
    ಪೂರ್ಣಾಂಕ		prev_syntax_ಲಂಬಸಾಲು = -1;	// column of prev_syntax_attr
    ಪೂರ್ಣಾಂಕ		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    ಪೂರ್ಣಾಂಕ		has_syntax = FALSE;	// this buffer has syntax highl.
    ಪೂರ್ಣಾಂಕ		save_did_emsg;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
# define WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN 32
    ಪೂರ್ಣಾಂಕ		did_ಸಾಲು = FALSE;	// set to TRUE when line text done
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_textprop_ಪಠ್ಯ_idx = -1;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_ಮುಂದಿನ = 0;	// next text property to use
    textprop_T	ಪಠ್ಯ_props_buf[WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN];
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_idxs_buf[WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN];
    char_u	ಪಠ್ಯ_prop_suffix_flags_buf[WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN + 1];
    textprop_T	*ಪಠ್ಯ_props = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		*ಪಠ್ಯ_prop_idxs = ಖಾಲಿ;
    char_u	*ಪಠ್ಯ_prop_suffix_flags = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_props_ಸಕ್ರಿಯ = 0;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_props_need_ವಿಂಗಡಿಸು = FALSE;
    proptype_T  *ಪಠ್ಯ_prop_type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_attr = 0;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_id = 0;	// active property ID
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_flags = 0;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_above = FALSE;  // first doing virtual text above
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_prop_follows = FALSE;  // another text prop to display
    ಪೂರ್ಣಾಂಕ		saved_ಹುಡುಕು_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    ಪೂರ್ಣಾಂಕ		saved_area_attr = 0;	// idem for area_attr
    ಪೂರ್ಣಾಂಕ		reset_extra_attr = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಪೂರ್ಣಾಂಕ		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    ಪೂರ್ಣಾಂಕ		nextlinecol = 0;	// column where nextline[] starts
    ಪೂರ್ಣಾಂಕ		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    ಪೂರ್ಣಾಂಕ		spell_attr = 0;		// attributes desired by spelling
    ಪೂರ್ಣಾಂಕ		ಶಬ್ದ_ಕೊನೆ = 0;		// last byte with same spell_attr
    ಪೂರ್ಣಾಂಕ		cur_checked_ಲಂಬಸಾಲು = 0;	// checked column for current line
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		extra_check = FALSE;	// has extra highlighting
    ಪೂರ್ಣಾಂಕ		multi_attr = 0;		// attributes desired by multibyte
    ಪೂರ್ಣಾಂಕ		mb_l = 1;		// multi-byte byte length
    ಪೂರ್ಣಾಂಕ		mb_c = 0;		// decoded multi-byte character
    ಪೂರ್ಣಾಂಕ		mb_utf8 = FALSE;	// screen char is UTF-8 char
    ಪೂರ್ಣಾಂಕ		u8cc[MAX_MCO];		// composing UTF-8 chars
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		change_ಆರಂಭ = MAXCOL;	// first col of changed area
    ಪೂರ್ಣಾಂಕ		change_ಕೊನೆ = -1;	// last col of changed area
    diffline_T	ಸಾಲು_changes;
    ಪೂರ್ಣಾಂಕ		change_ಸೂಚ್ಯಂಕ = -1;
#ಆದರೆ_ಅಂತ್ಯ
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    ಪೂರ್ಣಾಂಕ		ಒಳ_multispace = FALSE;	// in multiple consecutive spaces
    ಪೂರ್ಣಾಂಕ		multispace_pos = 0;	// position in lcs-multispace string
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಾಲು_ATTR
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_attr_save = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		sign_present = FALSE;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_attr = 0;		// attribute for the number column
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
    ಪೂರ್ಣಾಂಕ		prev_c = 0;		// previous Arabic character
    ಪೂರ್ಣಾಂಕ		prev_c1 = 0;		// first composing char for prev_c
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(ಸಾಲು_ATTR)
    ಪೂರ್ಣಾಂಕ		did_ಸಾಲು_attr = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪೂರ್ಣಾಂಕ		get_ಪದ_attr = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    ಪೂರ್ಣಾಂಕ		ಎಡ_curline_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		ಬಲ_curline_ಲಂಬಸಾಲು = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ಪೂರ್ಣಾಂಕ		feedback_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		feedback_old_attr = -1;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CONCEAL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ_conc	= 0;	// cchar for match functions
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CONCEAL) || defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_LINEBREAK)
    ಪೂರ್ಣಾಂಕ		on_ಕೊನೆಯ_ಲಂಬಸಾಲು     = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		syntax_flags	= 0;
    ಪೂರ್ಣಾಂಕ		syntax_seqnr	= 0;
    ಪೂರ್ಣಾಂಕ		prev_syntax_id	= 0;
    ಪೂರ್ಣಾಂಕ		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    ಪೂರ್ಣಾಂಕ		is_concealing	= FALSE;
    ಪೂರ್ಣಾಂಕ		did_wcol	= FALSE;
    ಪೂರ್ಣಾಂಕ		old_boguscols   = 0;
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_co - wlv.vcol_off_tp)
# define FIX_FOR_BOGUSCOLS \
    { \
	wlv.n_extra += wlv.vcol_off_co; \
	wlv.vcol -= wlv.vcol_off_co; \
	wlv.vcol_off_co = 0; \
	wlv.ಲಂಬಸಾಲು -= wlv.boguscols; \
	old_boguscols = wlv.boguscols; \
	wlv.boguscols = 0; \
    }
#ಇಲ್ಲದಿದ್ದರೆ
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (startrow > endrow)		// past the end already!
	ಹಿಂತಿರುಗಿ startrow;

    CLEAR_ಕ್ಷೇತ್ರ(wlv);

    wlv.lnum = lnum;
    wlv.startrow = startrow;
    wlv.ಅಡ್ಡಸಾಲು = startrow;
    wlv.screen_ಅಡ್ಡಸಾಲು = wlv.ಅಡ್ಡಸಾಲು + W_WINROW(wp);
    wlv.fromcol = -10;
    wlv.tocol = MAXCOL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    wlv.vcol_sbr = -1;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಸಂಖ್ಯೆ_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (syntax_present(wp) && !wp->w_s->b_syn_error
# ifdef SYN_ಸಮಯ_ಎಲ್ಲೆ
		&& !wp->w_s->b_syn_slow
# endif
	   )
	{
	    // Prepare for syntax highlighting in this line.  When there is an
	    // error, stop syntax highlighting.
	    save_did_emsg = did_emsg;
	    did_emsg = FALSE;
	    syntax_ಆರಂಭ(wp, lnum);
	    ಆದರೆ (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_ಸಮಯ_ಎಲ್ಲೆ
		ಆದರೆ (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	ಆದರೆ (wlv.color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != ಖಾಲಿ)
	    wlv.draw_color_ಲಂಬಸಾಲು = advance_color_ಲಂಬಸಾಲು(VCOL_HLC, &wlv.color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಆದರೆ (ಪದ_show_ಬಫರ್(wp->w_ಬಫರ್))
	{
	    extra_check = TRUE;
	    get_ಪದ_attr = TRUE;
	    wlv.win_attr = ಪದ_get_attr(wp, lnum, -1);
	}
#ಆದರೆ_ಅಂತ್ಯ

	// handle Visual active in this window
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ && wp->w_ಬಫರ್ == curwin->w_ಬಫರ್)
	{
	    pos_T	*ತುದಿ, *bot;

	    ಆದರೆ (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		ತುದಿ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Visual is before curwin->w_cursor
		ತುದಿ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_ಒಳ_visual_area = (lnum >= ತುದಿ->lnum && lnum <= bot->lnum);
	    ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V)
	    {
		// block mode
		ಆದರೆ (lnum_ಒಳ_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// non-block mode
		ಆದರೆ (lnum > ತುದಿ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == ತುದಿ->lnum)
		{
		    ಆದರೆ (VIsual_ಬಹುಲಕ == 'V')	// linewise
			wlv.fromcol = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			getvvcol(wp, ತುದಿ, &wlv.fromcol, ಖಾಲಿ, ಖಾಲಿ, 0);
			ಆದರೆ (gchar_pos(ತುದಿ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		ಆದರೆ (VIsual_ಬಹುಲಕ != 'V' && lnum == bot->lnum)
		{
		    ಆದರೆ (*p_sel == 'e' && bot->ಲಂಬಸಾಲು == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bot->ಲಂಬಸಾಲು == MAXCOL)
			wlv.tocol = MAXCOL;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			pos = *bot;
			ಆದರೆ (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ಖಾಲಿ, ಖಾಲಿ, 0);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    getvvcol(wp, &pos, ಖಾಲಿ, ಖಾಲಿ, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    ಆದರೆ (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#ಆದರೆ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		ಆದರೆ ((clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && !clip_ನಕ್ಷತ್ರ.owned
						    && clip_isautosel_ನಕ್ಷತ್ರ())
			|| (clip_plus.ಲಭ್ಯ && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (highlight_ಹೊಂದಿಕೆ
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು)
	{
	    ಆದರೆ (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ಖಾಲಿ, ಖಾಲಿ, 0);
	    ಇಲ್ಲದಿದ್ದರೆ
		wlv.fromcol = 0;
	    ಆದರೆ (lnum == curwin->w_cursor.lnum + ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು)
	    {
		pos.lnum = lnum;
		pos.ಲಂಬಸಾಲು = ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ಖಾಲಿ, ಖಾಲಿ, 0);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    ಆದರೆ (wlv.fromcol == wlv.tocol && ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF

    ಪೂರ್ಣಾಂಕ linestatus = 0;
    wlv.filler_ಸಾಲುಗಳು = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_ಕ್ಷೇತ್ರ(ಸಾಲು_changes);

    ಆದರೆ (linestatus < 0)
    {
	ಆದರೆ (linestatus == -1)
	{
	    ಆದರೆ (diff_ಕಂಡುಹಿಡಿ_change(wp, lnum, &ಸಾಲು_changes))
	    {
		wlv.diff_hlf = HLF_ಕೂಡಿಸು;	// added line
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು_changes.ಅಂಕೆ_changes > 0)
	    {
		ಪೂರ್ಣಾಂಕ added = diff_change_parse(
			&ಸಾಲು_changes, &ಸಾಲು_changes.changes[0],
			&change_ಆರಂಭ, &change_ಕೊನೆ);
		ಆದರೆ (change_ಆರಂಭ == 0)
		{
		    ಆದರೆ (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    ಇಲ್ಲದಿದ್ದರೆ
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_ಸೂಚ್ಯಂಕ = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_ಸೂಚ್ಯಂಕ = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    wlv.diff_hlf = HLF_ಕೂಡಿಸು;

	area_highlighting = TRUE;
    }

    ಆದರೆ (lnum == wp->w_topline)
	wlv.filler_ಸಾಲುಗಳು = wp->w_topfill;

    wlv.filler_todo = wlv.filler_ಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    ಆದರೆ (sign_present)
	ಅಂಕೆ_attr = wlv.sattr.sat_numhl;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಾಲು_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    ಆದರೆ (sign_present)
	wlv.ಸಾಲು_attr = wlv.sattr.sat_linehl;
# endif
# ಆದರೆ defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    ಆದರೆ (bt_quickfix(wp->w_ಬಫರ್) && qf_current_ನಮೂದು(wp) == lnum)
	wlv.ಸಾಲು_attr = HL_ATTR(HLF_QFL);
# endif
    ಆದರೆ (wlv.ಸಾಲು_attr != 0)
	area_highlighting = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಆದರೆ (spv->spv_has_spell && ಸಂಖ್ಯೆ_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	ಆದರೆ (lnum == spv->spv_checked_lnum)
	    cur_checked_ಲಂಬಸಾಲು = spv->spv_checked_ಲಂಬಸಾಲು;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	ಆದರೆ (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_ಲಂಬಸಾಲು = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_ಲಂಬಸಾಲು = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	ಆದರೆ (lnum < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum + 1, FALSE);
	    spell_cat_ಸಾಲು(nextline + SPWORDLEN, ಸಾಲು, SPWORDLEN);
	}
	ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(ಸಾಲು);
	ಆದರೆ (*ptr == NUL)
	{
	    spv->spv_cap_ಲಂಬಸಾಲು = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (spv->spv_cap_ಲಂಬಸಾಲು == 0)
	    spv->spv_cap_ಲಂಬಸಾಲು = ptr - ಸಾಲು;

	// Copy the end of the current line into nextline[].
	ಆದರೆ (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    v = ml_get_buf_len(wp->w_ಬಫರ್, lnum);
	    ಆದರೆ (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, ಸಾಲು, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, ಸಾಲು + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    ptr = ಸಾಲು;

    ಆದರೆ (wp->w_p_list)
    {
	ಆದರೆ (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ಖಾಲಿ
		|| wp->w_lcs_chars.leadmultispace != ಖಾಲಿ
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	ಆದರೆ (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_ಬಫರ್, lnum);
	    ಆಗ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - ಸಾಲು);
	}
	// find end of leading whitespace
	ಆದರೆ (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ಖಾಲಿ ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    ಆಗ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    ಆದರೆ (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    ಇಲ್ಲದಿದ್ದರೆ
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - ಸಾಲು) + 1;
	}
    }

    ಆದರೆ (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    ಪೂರ್ಣಾಂಕ skipcol_ಒಳ_ಪಠ್ಯ_prop_above = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (WIN_IS_POPUP(wp))
	wlv.screen_ಸಾಲು_flags |= SLF_POPUP;

    char_u *prop_ಆರಂಭ;
    ಪಠ್ಯ_prop_ಎಣಿಕೆ = get_ಪಠ್ಯ_props(wp->w_ಬಫರ್, lnum, &prop_ಆರಂಭ, FALSE);
    ಆದರೆ (ಪಠ್ಯ_prop_ಎಣಿಕೆ > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	ಆದರೆ (ಪಠ್ಯ_prop_ಎಣಿಕೆ <= WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN)
	    ಪಠ್ಯ_props = ಪಠ್ಯ_props_buf;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ_props = ALLOC_MULT(textprop_T, ಪಠ್ಯ_prop_ಎಣಿಕೆ);
	ಆದರೆ (ಪಠ್ಯ_props != ಖಾಲಿ)
	{
	    mch_memmove(ಪಠ್ಯ_props, prop_ಆರಂಭ,
				     ಪಠ್ಯ_prop_ಎಣಿಕೆ * ಗಾತ್ರ(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *ಎಣಿಕೆ_ptr = prop_ಆರಂಭ - PROP_ಎಣಿಕೆ_ಗಾತ್ರ;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪಠ್ಯ_prop_ಎಣಿಕೆ; ++i)
	    {
		ಆದರೆ (ಪಠ್ಯ_props[i].tp_id < 0
				&& ಪಠ್ಯ_props[i].u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ > 0)
		{
		    ಪಠ್ಯ_props[i].u.tp_ಪಠ್ಯ =
			    ಎಣಿಕೆ_ptr + ಪಠ್ಯ_props[i].u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ;
		    ಪಠ್ಯ_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಪಠ್ಯ_props[i].u.tp_ಪಠ್ಯ = ಖಾಲಿ;
	    }
	}

	// Allocate an array for the indexes.
	ಆದರೆ (ಪಠ್ಯ_prop_ಎಣಿಕೆ <= WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN)
	    ಪಠ್ಯ_prop_idxs = ಪಠ್ಯ_prop_idxs_buf;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ_prop_idxs = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಪಠ್ಯ_prop_ಎಣಿಕೆ);
	ಆದರೆ (ಪಠ್ಯ_prop_idxs == ಖಾಲಿ)
	{
	    ಆದರೆ (ಪಠ್ಯ_props != ಪಠ್ಯ_props_buf)
		VIM_CLEAR(ಪಠ್ಯ_props);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಪಠ್ಯ_props = ಖಾಲಿ;
	}
	ಆದರೆ (ಪಠ್ಯ_prop_ಎಣಿಕೆ <= WIN_ಸಾಲು_ಪಠ್ಯ_PROP_ರಾಶಿ_LEN)
	    ಪಠ್ಯ_prop_suffix_flags = ಪಠ್ಯ_prop_suffix_flags_buf;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ_prop_suffix_flags = ALLOC_MULT(char_u, ಪಠ್ಯ_prop_ಎಣಿಕೆ + 1);
	ಆದರೆ (ಪಠ್ಯ_prop_suffix_flags == ಖಾಲಿ)
	{
	    ಆದರೆ (ಪಠ್ಯ_prop_idxs != ಪಠ್ಯ_prop_idxs_buf)
		vim_free(ಪಠ್ಯ_prop_idxs);
	    ಪಠ್ಯ_prop_idxs = ಖಾಲಿ;
	    ಆದರೆ (ಪಠ್ಯ_props != ಪಠ್ಯ_props_buf)
		VIM_CLEAR(ಪಠ್ಯ_props);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಪಠ್ಯ_props = ಖಾಲಿ;
	}

	ಆದರೆ (ಪಠ್ಯ_props != ಖಾಲಿ)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪಠ್ಯ_prop_ಎಣಿಕೆ; ++i)
		ಆದರೆ (ಪಠ್ಯ_props[i].tp_id < 0)
		    ಕೊನೆಯ_textprop_ಪಠ್ಯ_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_ಅಗಲ = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಪಠ್ಯ_prop_ಎಣಿಕೆ - 1; i >= 0; --i)
		ಆದರೆ (ಪಠ್ಯ_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    ಆದರೆ (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_ಒಳ_ಪಠ್ಯ_prop_above
								 >= ಪಠ್ಯ_ಅಗಲ)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_ಒಳ_ಪಠ್ಯ_prop_above += ಪಠ್ಯ_ಅಗಲ;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = i + 1; j < ಪಠ್ಯ_prop_ಎಣಿಕೆ; ++j)
			    ಪಠ್ಯ_props[j - 1] = ಪಠ್ಯ_props[j];
			++i;
			--ಪಠ್ಯ_prop_ಎಣಿಕೆ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			++wlv.ಪಠ್ಯ_prop_above_ಎಣಿಕೆ;
		}

	    ಪಠ್ಯ_prop_suffix_flags[ಪಠ್ಯ_prop_ಎಣಿಕೆ] = 0;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಪಠ್ಯ_prop_ಎಣಿಕೆ - 1; i >= 0; --i)
	    {
		ಪೂರ್ಣಾಂಕ flags = ಪಠ್ಯ_prop_suffix_flags[i + 1];

		ಆದರೆ (ಪಠ್ಯ_props[i].tp_ಲಂಬಸಾಲು == MAXCOL)
		{
		    flags |= 1;
		    ಆದರೆ (ಪಠ್ಯ_props[i].tp_flags & TP_FLAG_ALIGN_ಕೆಳಗೆ)
			flags |= 2;
		}
		ಪಠ್ಯ_prop_suffix_flags[i] = flags;
	    }
	}
    }

    ಆದರೆ (ಸಂಖ್ಯೆ_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.ಅಡ್ಡಸಾಲು += wlv.ಪಠ್ಯ_prop_above_ಎಣಿಕೆ;
	ಆದರೆ (wlv.ಅಡ್ಡಸಾಲು >= endrow)
	{
	    ಆದರೆ (ಪಠ್ಯ_props != ಪಠ್ಯ_props_buf)
		vim_free(ಪಠ್ಯ_props);
	    ಆದರೆ (ಪಠ್ಯ_prop_idxs != ಪಠ್ಯ_prop_idxs_buf)
		vim_free(ಪಠ್ಯ_prop_idxs);
	    ಆದರೆ (ಪಠ್ಯ_prop_suffix_flags != ಪಠ್ಯ_prop_suffix_flags_buf)
		vim_free(ಪಠ್ಯ_prop_suffix_flags);
	    ಹಿಂತಿರುಗಿ wlv.ಅಡ್ಡಸಾಲು;
	}
	wlv.screen_ಅಡ್ಡಸಾಲು += wlv.ಪಠ್ಯ_prop_above_ಎಣಿಕೆ;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_ಮೊದಲ_char = 0;
    ಆದರೆ (wp->w_p_lbr && ಸಂಖ್ಯೆ_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
	(ಶೂನ್ಯ)win_lbr_chartabsize(&cts, ಖಾಲಿ, ಖಾಲಿ);
	vcol_ಮೊದಲ_char = cts.cts_ಮೊದಲ_char;
	clear_chartabsize_arg(&cts);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    ಆದರೆ (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_ಒಳ_ಪಠ್ಯ_prop_above : 0;
    ಇಲ್ಲದಿದ್ದರೆ
	v = wp->w_leftcol;
    ಆದರೆ (v > 0 && ಸಂಖ್ಯೆ_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	ಪೂರ್ಣಾಂಕ		charsize = 0;
	ಪೂರ್ಣಾಂಕ		ತಲೆ = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, ಸಾಲು, ptr);
	cts.cts_max_ತಲೆ_vcol = v;
	ಆಗ (cts.cts_vcol < v)
	{
	    ತಲೆ = 0;
	    charsize = win_lbr_chartabsize(&cts, &ತಲೆ, ಖಾಲಿ);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    ಆದರೆ (*prev_ptr == NUL)
		ನಿಲ್ಲಿಸು;
	    MB_PTR_ADV(cts.cts_ptr);
	    ಆದರೆ (wp->w_p_list)
	    {
		ಒಳ_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > ಸಾಲು && prev_ptr[-1] == ' '));
		ಆದರೆ (!ಒಳ_multispace)
		    multispace_pos = 0;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cts.cts_ptr >= ಸಾಲು + leadcol
					 && wp->w_lcs_chars.multispace != ಖಾಲಿ)
		{
		    ++multispace_pos;
		    ಆದರೆ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cts.cts_ptr < ಸಾಲು + leadcol
				     && wp->w_lcs_chars.leadmultispace != ಖಾಲಿ)
		{
		    ++multispace_pos;
		    ಆದರೆ (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	ಆದರೆ (wlv.vcol < v && (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_ಲಂಬಸಾಲು ||
#ಆದರೆ_ಅಂತ್ಯ
	     virtual_ಸಕ್ರಿಯ() ||
	     (VIsual_ಸಕ್ರಿಯ && wp->w_ಬಫರ್ == curwin->w_ಬಫರ್)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	ಆದರೆ (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	ಆದರೆ (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - ತಲೆ;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	ಆದರೆ (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	ಆದರೆ (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	ಆದರೆ (spv->spv_has_spell)
	{
	    ಪೂರ್ಣಾಂಕ		len;
	    colnr_T	linecol = (colnr_T)(ptr - ಸಾಲು);
	    hlf_T	spell_hlf = HLF_ಎಣಿಕೆ;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.ಲಂಬಸಾಲು = linecol;
	    len = spell_move_ಗಮ್ಯ(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
	    ptr = ಸಾಲು + linecol;

	    ಆದರೆ (len == 0 || (ಪೂರ್ಣಾಂಕ)wp->w_cursor.ಲಂಬಸಾಲು > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		//ಶಬ್ದ
		spell_hlf = HLF_ಎಣಿಕೆ;
		ಶಬ್ದ_ಕೊನೆ = (ಪೂರ್ಣಾಂಕ)(spell_ಗಮ್ಯ_ಶಬ್ದ_ಕೊನೆ(ptr, wp) - ಸಾಲು + 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// bad word found, use attributes until end of word
		ಶಬ್ದ_ಕೊನೆ = wp->w_cursor.ಲಂಬಸಾಲು + len + 1;

		// Turn index into actual attributes.
		ಆದರೆ (spell_hlf != HLF_ಎಣಿಕೆ)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    ಆದರೆ (has_syntax)
		syntax_ಆರಂಭ(wp, lnum);
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಆದರೆ (ಸಂಖ್ಯೆ_only == 0)
    {
	v = (ದೀರ್ಘ)(ptr - ಸಾಲು);
	area_highlighting |= prepare_ಹುಡುಕು_hl_ಸಾಲು(wp, lnum, (colnr_T)v,
					      &ಸಾಲು, &screen_ಹುಡುಕು_hl,
					      &ಹುಡುಕು_attr);
	ptr = ಸಾಲು + v; // "line" may have been updated
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && ins_compl_win_ಸಕ್ರಿಯ(wp)
			    && (ಒಳ_curline || ins_compl_lnum_ಒಳ_range(lnum)))
	area_highlighting = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    ಆದರೆ (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	ಆದರೆ (!(wp == curwin && VIsual_ಸಕ್ರಿಯ)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    ಆದರೆ (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸಾಲು_attr_save = wlv.ಸಾಲು_attr;
		margin_columns_win(wp, &ಎಡ_curline_ಲಂಬಸಾಲು, &ಬಲ_curline_ಲಂಬಸಾಲು);
	    }
	    area_highlighting = TRUE;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    win_ಸಾಲು_ಆರಂಭ(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    ಗಾಗಿ (;;)
    {
#ಆದರೆ defined(FEAT_CONCEAL) || defined(FEAT_ಹುಡುಕು_EXTRA)
	ಪೂರ್ಣಾಂಕ	has_ಹೊಂದಿಕೆ_conc = 0;	// match wants to conceal
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	ಪೂರ್ಣಾಂಕ	did_decrement_ptr = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

	// Skip this quickly when working on the text.
	ಆದರೆ (wlv.draw_state != WL_ಸಾಲು)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.ಸಾಲು_attr = ಸಾಲು_attr_save;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		ಆದರೆ (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    ಆದರೆ (wlv.ಅಡ್ಡಸಾಲು == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    ಆದರೆ (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		ಆದರೆ (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_ಲಂಬಸಾಲು(wp, &wlv, sign_present, ಅಂಕೆ_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    ಆದರೆ (ಸಂಖ್ಯೆ_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_ಸಾಲು(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		ಆದರೆ ((wlv.ಅಡ್ಡಸಾಲು + 1 - wlv.startrow < ಸಂಖ್ಯೆ_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			|| wlv.filler_todo > 0
#ಆದರೆ_ಅಂತ್ಯ
			)
		{
		    ++wlv.ಅಡ್ಡಸಾಲು;
		    ++wlv.screen_ಅಡ್ಡಸಾಲು;
		    ಆದರೆ (wlv.ಅಡ್ಡಸಾಲು == endrow)
			ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    --wlv.filler_todo;
		    ಆದರೆ (wlv.filler_todo == 0 && wp->w_botfill)
			ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		    win_ಸಾಲು_ಆರಂಭ(wp, &wlv, TRUE);
		    ಮುಂದುವರಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ನಿಲ್ಲಿಸು;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    ಆದರೆ (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    ಆದರೆ (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (wlv.draw_state == WL_ಸಾಲು - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_ಸಾಲು;
		win_ಸಾಲು_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (wlv.cul_screenline && wlv.draw_state == WL_ಸಾಲು
		&& wlv.vcol >= ಎಡ_curline_ಲಂಬಸಾಲು
		&& wlv.vcol < ಬಲ_curline_ಲಂಬಸಾಲು)
	    apply_cursorline_highlight(&wlv, sign_present);
#ಆದರೆ_ಅಂತ್ಯ

	// When still displaying '$' of change command, stop at cursor.
	ಆದರೆ (dollar_vcol >= 0 && ಒಳ_curline && wlv.vcol >= (ದೀರ್ಘ)wp->w_virtcol)
	{
	    wlv_screen_ಸಾಲು(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (wp->w_p_cuc)
		wlv.ಅಡ್ಡಸಾಲು = wp->w_cline_ಅಡ್ಡಸಾಲು + wp->w_cline_ಎತ್ತರ;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		wlv.ಅಡ್ಡಸಾಲು = wp->w_ಎತ್ತರ;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (wlv.draw_state == WL_ಸಾಲು && (area_highlighting || extra_check))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (ಪಠ್ಯ_props != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ pi;
		ಪೂರ್ಣಾಂಕ bcol = (ಪೂರ್ಣಾಂಕ)(ptr - ಸಾಲು);

		ಆದರೆ (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !ಒಳ_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		ಗಾಗಿ (pi = 0; pi < ಪಠ್ಯ_props_ಸಕ್ರಿಯ; ++pi)
		{
		    ಪೂರ್ಣಾಂಕ		tpi = ಪಠ್ಯ_prop_idxs[pi];
		    textprop_T  *tp = &ಪಠ್ಯ_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    ಆದರೆ ((tp->tp_ಲಂಬಸಾಲು != MAXCOL
				       && bcol >= tp->tp_ಲಂಬಸಾಲು - 1 + tp->tp_len))
		    {
			ಆದರೆ (pi + 1 < ಪಠ್ಯ_props_ಸಕ್ರಿಯ)
			    mch_memmove(ಪಠ್ಯ_prop_idxs + pi,
					ಪಠ್ಯ_prop_idxs + pi + 1,
					ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)
					     * (ಪಠ್ಯ_props_ಸಕ್ರಿಯ - (pi + 1)));
			--ಪಠ್ಯ_props_ಸಕ್ರಿಯ;
			ಪಠ್ಯ_props_need_ವಿಂಗಡಿಸು = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			ಆದರೆ (ಒಳ_linebreak && syntax_attr == ಪಠ್ಯ_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		ಆದರೆ (wlv.n_extra > 0 && ಒಳ_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		ಆಗ (ಪಠ್ಯ_prop_ಮುಂದಿನ < ಪಠ್ಯ_prop_ಎಣಿಕೆ)
		{
		    ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ;
		    textprop_T *tp = &ಪಠ್ಯ_props[ಪಠ್ಯ_prop_ಮುಂದಿನ];
		    ಆದರೆ (tp->tp_ಲಂಬಸಾಲು == MAXCOL)
		    {
			ಆದರೆ (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    ಸಕ್ರಿಯ = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ptr != NUL)
			    ನಿಲ್ಲಿಸು;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    ಸಕ್ರಿಯ = wp->w_p_wrap
				  || wlv.ಅಡ್ಡಸಾಲು == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_ಕೆಳಗೆ);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (bcol < tp->tp_ಲಂಬಸಾಲು - 1)
			    ನಿಲ್ಲಿಸು;
			ಸಕ್ರಿಯ = bcol <= tp->tp_ಲಂಬಸಾಲು - 1 + tp->tp_len;
		    }

		    ಆದರೆ (ಸಕ್ರಿಯ)
		    {
			ಪಠ್ಯ_prop_idxs[ಪಠ್ಯ_props_ಸಕ್ರಿಯ++] = ಪಠ್ಯ_prop_ಮುಂದಿನ;
			ಪಠ್ಯ_props_need_ವಿಂಗಡಿಸು = TRUE;
		    }
		    ++ಪಠ್ಯ_prop_ಮುಂದಿನ;
		}

		ಆದರೆ (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(ಪಠ್ಯ_prop_type != ಖಾಲಿ &&
			     ಪಠ್ಯ_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    ಪಠ್ಯ_prop_attr = 0;
		    ಪಠ್ಯ_prop_attr_comb = 0;
		    ಪಠ್ಯ_prop_flags = 0;
		    ಪಠ್ಯ_prop_type = ಖಾಲಿ;
		    ಪಠ್ಯ_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		ಆದರೆ (ಪಠ್ಯ_props_ಸಕ್ರಿಯ > 0 && wlv.n_extra == 0)
		{
		    ಪೂರ್ಣಾಂಕ ಬಳಸಿದ_tpi = -1;
		    ಪೂರ್ಣಾಂಕ ಬಳಸಿದ_attr = 0;
		    ಪೂರ್ಣಾಂಕ other_tpi = -1;

		    ಪಠ್ಯ_prop_above = FALSE;
		    ಪಠ್ಯ_prop_follows = FALSE;

		    ಆದರೆ (ಪಠ್ಯ_props_need_ವಿಂಗಡಿಸು)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			ವಿಂಗಡಿಸು_ಪಠ್ಯ_props(wp->w_ಬಫರ್, ಪಠ್ಯ_props,
					ಪಠ್ಯ_prop_idxs, ಪಠ್ಯ_props_ಸಕ್ರಿಯ);
			ಪಠ್ಯ_props_need_ವಿಂಗಡಿಸು = FALSE;
		    }

		    ಗಾಗಿ (pi = 0; pi < ಪಠ್ಯ_props_ಸಕ್ರಿಯ; ++pi)
		    {
			ಪೂರ್ಣಾಂಕ	    tpi = ಪಠ್ಯ_prop_idxs[pi];
			textprop_T  *tp = &ಪಠ್ಯ_props[tpi];
			proptype_T  *pt = ಪಠ್ಯ_prop_type_by_id(
						    wp->w_ಬಫರ್, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			ಆದರೆ (pt != ಖಾಲಿ
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_ಬಲ
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_ಕೆಳಗೆ)) == 0
				    && wlv.ಲಂಬಸಾಲು >= wp->w_ಅಗಲ))
			{
			    ಆದರೆ (tp->tp_ಲಂಬಸಾಲು == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == ಸಾಲು
						&& !did_ಸಾಲು
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_ಕೆಳಗೆ))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				ಪಠ್ಯ_prop_follows = TRUE;
				ಮುಂದುವರಿ;
			    }

			    ಆದರೆ (pt->pt_hl_id > 0)
				ಬಳಸಿದ_attr = syn_id2attr(pt->pt_hl_id);
			    ಪಠ್ಯ_prop_type = pt;
			    ಪಠ್ಯ_prop_attr =
				   hl_combine_attr(ಪಠ್ಯ_prop_attr, ಬಳಸಿದ_attr);
			    ಆದರೆ (ಬಳಸಿದ_tpi >= 0 && ಪಠ್ಯ_props[ಬಳಸಿದ_tpi].tp_id < 0)
				other_tpi = ಬಳಸಿದ_tpi;
			    ಪಠ್ಯ_prop_flags = pt->pt_flags;
			    ಪಠ್ಯ_prop_id = tp->tp_id;
			    ಬಳಸಿದ_tpi = tpi;
			}
		    }
		    ಆದರೆ (ಪಠ್ಯ_prop_id < 0 && ಬಳಸಿದ_tpi >= 0
				 && ಪಠ್ಯ_props[ಬಳಸಿದ_tpi].u.tp_ಪಠ್ಯ != ಖಾಲಿ)
		    {
			textprop_T  *tp = &ಪಠ್ಯ_props[ಬಳಸಿದ_tpi];
			char_u	    *p = tp->u.tp_ಪಠ್ಯ;
			ಪೂರ್ಣಾಂಕ	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			ಪೂರ್ಣಾಂಕ	    bail_ಹೊರಹೋಗು = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			ಆದರೆ (p != ಖಾಲಿ)
			{
			    ಪೂರ್ಣಾಂಕ	    ಬಲ_2 = (tp->tp_flags
							& TP_FLAG_ALIGN_ಬಲ);
			    ಪೂರ್ಣಾಂಕ	    ಕೆಳಗೆ = (tp->tp_flags
							& TP_FLAG_ALIGN_ಕೆಳಗೆ);
			    ಪೂರ್ಣಾಂಕ	    wrap = tp->tp_ಲಂಬಸಾಲು < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    ಪೂರ್ಣಾಂಕ	    padding = tp->tp_ಲಂಬಸಾಲು == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.ಆರಂಭ_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    ಬಳಸಿದ_attr);
			    n_attr = mb_charlen(p);
			    ಪಠ್ಯ_prop_attr = 0;
			    ಪಠ್ಯ_prop_attr_comb = 0;
			    ಆದರೆ (*ptr == NUL)
				// don't combine char attr after EOL
				ಪಠ್ಯ_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    ಆದರೆ (ಪಠ್ಯ_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    ಆದರೆ ((ಬಲ_2 || above || ಕೆಳಗೆ || !wrap
					    || padding > 0) && wp->w_ಅಗಲ > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				ಪೂರ್ಣಾಂಕ	ಆರಂಭ_ಸಾಲು;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				ಆರಂಭ_ಸಾಲು = ಪಠ್ಯ_prop_position(wp, tp,
						    wlv.vcol,
# ifdef FEAT_RIGHTLEFT
						    wp->w_p_rl
						    ? wp->w_ಅಗಲ - wlv.ಲಂಬಸಾಲು - 1
						    :
# endif
						    wlv.ಲಂಬಸಾಲು,
						    &wlv.n_extra, &wlv.p_extra,
						    &n_attr, &wlv.n_attr_skip,
						    skip_cells > 0);
				ಆದರೆ (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				ಆದರೆ (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				ಆದರೆ (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.ಲಂಬಸಾಲು
					       + wlv.n_extra - 2 > wp->w_ಅಗಲ)
				    // don't bail out at end of line
				    ಪಠ್ಯ_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				ಆದರೆ (ಆರಂಭ_ಸಾಲು)
				{
				    draw_screen_ಸಾಲು(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    ಆದರೆ (wlv.ಅಡ್ಡಸಾಲು == endrow)
				    {
					++wlv.ಅಡ್ಡಸಾಲು;
					ನಿಲ್ಲಿಸು;
				    }
				    win_ಸಾಲು_ಆರಂಭ(wp, &wlv, TRUE);
				    bail_ಹೊರಹೋಗು = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			ಆದರೆ (skip_cells > 0)
			{
			    ಆದರೆ (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				ಆದರೆ (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				// the whole text is left of the window, drop
				// it and advance to the next one
				skip_cells -= wlv.n_extra;
				skipped_cells += wlv.n_extra;
				wlv.n_extra = 0;
				wlv.n_attr_skip = 0;
				bail_ಹೊರಹೋಗು = TRUE;
			    }
			}

			// If another text prop follows the condition below at
			// the last window column must know.
			// If this is an "above" text prop and 'nowrap' then we
			// must wrap anyway.
			ಪಠ್ಯ_prop_above = above;
			ಪಠ್ಯ_prop_follows |= other_tpi != -1
					&& (wp->w_p_wrap
					     || (ಪಠ್ಯ_props[other_tpi].tp_flags
			       & (TP_FLAG_ALIGN_ಕೆಳಗೆ | TP_FLAG_ALIGN_ಬಲ)));

			ಆದರೆ (bail_ಹೊರಹೋಗು)
			    // starting a new line for "below"
			    ಮುಂದುವರಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪಠ್ಯ_prop_ಮುಂದಿನ < ಪಠ್ಯ_prop_ಎಣಿಕೆ
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.ಲಂಬಸಾಲು == wp->w_ಅಗಲ - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    ಪೂರ್ಣಾಂಕ only_ಕೆಳಗೆ_follows = !wp->w_p_wrap
				 && wlv.ಲಂಬಸಾಲು + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_ಅಗಲ;
		    ಪೂರ್ಣಾಂಕ suffix_flags = ಪಠ್ಯ_prop_suffix_flags[ಪಠ್ಯ_prop_ಮುಂದಿನ];

		    ಪಠ್ಯ_prop_follows = (suffix_flags
					& (only_ಕೆಳಗೆ_follows ? 2 : 1)) != 0;
		}
	    }

	    ಆದರೆ (wlv.ಆರಂಭ_extra_for_textprop)
	    {
		wlv.ಆರಂಭ_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_ಹುಡುಕು_attr = ಹುಡುಕು_attr;
		saved_area_attr = area_attr;
		ಹುಡುಕು_attr = 0;
		area_attr = 0;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    ಪೂರ್ಣಾಂಕ *area_attr_p =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#ಆದರೆ_ಅಂತ್ಯ
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    ಆದರೆ (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ಖಾಲಿ
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    ಆದರೆ (wlv.n_extra == 0)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (ದೀರ್ಘ)(ptr - ಸಾಲು);
		ಹುಡುಕು_attr = update_ಹುಡುಕು_hl(wp, lnum, (colnr_T)v, &ಸಾಲು,
				      &screen_ಹುಡುಕು_hl, &has_ಹೊಂದಿಕೆ_conc,
				      &ಹೊಂದಿಕೆ_conc, did_ಸಾಲು_attr, lcs_eol_one,
				      &on_ಕೊನೆಯ_ಲಂಬಸಾಲು);
		ptr = ಸಾಲು + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		ಆದರೆ (*ptr == NUL)
		    has_ಹೊಂದಿಕೆ_conc = 0;
#ಇಲ್ಲದಿದ್ದರೆ
		ಹುಡುಕು_attr = 0;
#ಆದರೆ_ಅಂತ್ಯ

		// Check if ComplMatchIns highlight is needed.
		ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && ins_compl_win_ಸಕ್ರಿಯ(wp)
			    && (ಒಳ_curline || ins_compl_lnum_ಒಳ_range(lnum)))
		{
		    ಪೂರ್ಣಾಂಕ ins_ಹೊಂದಿಕೆ_attr =
			ins_compl_ಲಂಬಸಾಲು_range_attr(lnum, (ಪೂರ್ಣಾಂಕ)(ptr - ಸಾಲು));
		    ಆದರೆ (ins_ಹೊಂದಿಕೆ_attr > 0)
			ಹುಡುಕು_attr =
			    hl_combine_attr(ಹುಡುಕು_attr, ins_ಹೊಂದಿಕೆ_attr);
		}
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಆದರೆ (wlv.diff_hlf != (hlf_T)0)
	    {
		ಆದರೆ (ಸಾಲು_changes.ಅಂಕೆ_changes > 0
			&& change_ಸೂಚ್ಯಂಕ >= 0
			&& change_ಸೂಚ್ಯಂಕ < ಸಾಲು_changes.ಅಂಕೆ_changes - 1)
		{
		    ಆದರೆ (ptr - ಸಾಲು >= ಸಾಲು_changes.changes[change_ಸೂಚ್ಯಂಕ+1].dc_ಆರಂಭ[ಸಾಲು_changes.bufidx])
			change_ಸೂಚ್ಯಂಕ += 1;
		}
		ಪೂರ್ಣಾಂಕ added = FALSE;
		ಆದರೆ (ಸಾಲು_changes.ಅಂಕೆ_changes > 0 && change_ಸೂಚ್ಯಂಕ >= 0 && change_ಸೂಚ್ಯಂಕ < ಸಾಲು_changes.ಅಂಕೆ_changes)
		{
		    added = diff_change_parse(&ಸಾಲು_changes,
			    &ಸಾಲು_changes.changes[change_ಸೂಚ್ಯಂಕ],
			    &change_ಆರಂಭ,
			    &change_ಕೊನೆ);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		ಆದರೆ (wlv.diff_hlf == HLF_CHD && ptr - ಸಾಲು >= change_ಆರಂಭ
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		ಆದರೆ ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - ಸಾಲು >= change_ಕೊನೆ && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.ಸಾಲು_attr = HL_ATTR(wlv.diff_hlf);
		ಆದರೆ (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ಎಡ_curline_ಲಂಬಸಾಲು
					    && wlv.vcol <= ಬಲ_curline_ಲಂಬಸಾಲು)))
		    wlv.ಸಾಲು_attr = hl_combine_attr(
					  wlv.ಸಾಲು_attr, HL_ATTR(HLF_CUL));
	    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		ಆದರೆ (get_ಪದ_attr)
		    syntax_attr = ಪದ_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		ಆದರೆ (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (ದೀರ್ಘ)(ptr - ಸಾಲು);
		    ಆದರೆ (v == prev_syntax_ಲಂಬಸಾಲು)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ಖಾಲಿ, FALSE);
			prev_syntax_ಲಂಬಸಾಲು = v;
			prev_syntax_attr = syntax_attr;
		    }

		    ಆದರೆ (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			did_emsg = save_did_emsg;
# ifdef SYN_ಸಮಯ_ಎಲ್ಲೆ
		    ಆದರೆ (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
		    ptr = ಸಾಲು + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    ಆದರೆ (*ptr == NUL)
			syntax_flags = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    ಆದರೆ (ಪಠ್ಯ_prop_type != ಖಾಲಿ)
	    {
		ಆದರೆ (ಪಠ್ಯ_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, ಪಠ್ಯ_prop_attr);
		ಇಲ್ಲದಿದ್ದರೆ
		    syntax_attr = ಪಠ್ಯ_prop_attr;
		ಪಠ್ಯ_prop_attr_comb = syntax_attr;
	    }
# endif
#ಆದರೆ_ಅಂತ್ಯ

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಾಲು_ATTR
	    ಆದರೆ (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ಸಾಲು_attr, area_attr);
		ಆದರೆ (!highlight_ಹೊಂದಿಕೆ)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(ಹುಡುಕು_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹುಡುಕು_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ಸಾಲು_attr, ಹುಡುಕು_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wlv.ಸಾಲು_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.ಸಾಲು_attr);
# ಇಲ್ಲದಿದ್ದರೆ
		wlv.char_attr = wlv.ಸಾಲು_attr;
# endif
		attr_pri = FALSE;
	    }
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (area_attr != 0)
		wlv.char_attr = area_attr;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹುಡುಕು_attr != 0)
		wlv.char_attr = ಹುಡುಕು_attr;
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		attr_pri = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#ಇಲ್ಲದಿದ್ದರೆ
		wlv.char_attr = 0;
#ಆದರೆ_ಅಂತ್ಯ
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    ಆದರೆ (ಪಠ್ಯ_prop_type != ಖಾಲಿ && (ಪಠ್ಯ_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, ಪಠ್ಯ_prop_attr);
#ಆದರೆ_ಅಂತ್ಯ
	}

	// combine attribute with HLF_WIN
	ಆದರೆ (wlv.win_attr != 0)
	{
	    ಆದರೆ (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (wlv.n_extra > 0)
	{
	    ಆದರೆ (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    mb_utf8 = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		c = *wlv.p_extra;
		ಆದರೆ (has_mbyte)
		{
		    mb_c = c;
		    ಆದರೆ (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			ಆದರೆ (mb_l > wlv.n_extra)
			    mb_l = 1;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			ಆದರೆ (mb_l >= wlv.n_extra)
			    mb_l = 1;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    ಆದರೆ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    ಆದರೆ ((
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ಲಂಬಸಾಲು <= 0) :
#ಆದರೆ_ಅಂತ್ಯ
				    (wlv.ಲಂಬಸಾಲು >= wp->w_ಅಗಲ - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = HL_ATTR(HLF_AT);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			ಆದರೆ (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#ಆದರೆ_ಅಂತ್ಯ
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#ಆದರೆ defined(FEAT_PROP_POPUP)
	    ಆದರೆ (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		ಆದರೆ (wlv.saved_n_extra <= 0)
		{
		    ಆದರೆ (ಹುಡುಕು_attr == 0)
			ಹುಡುಕು_attr = saved_ಹುಡುಕು_attr;
		    ಆದರೆ (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    ಆದರೆ (wlv.extra_for_textprop)
			// wlv.extra_attr should be used at this position but
			// not any further.
			reset_extra_attr = TRUE;
		}

		wlv.extra_for_textprop = FALSE;
# ifdef FEAT_LINEBREAK
		ಒಳ_linebreak = FALSE;
# endif
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    ಪೂರ್ಣಾಂಕ		c0;
#ಆದರೆ_ಅಂತ್ಯ
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    c0 = *ptr;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (c == NUL)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_ಸಾಲು = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		// no more cells to skip
		skip_cells = 0;
	    }

	    ಆದರೆ (has_mbyte)
	    {
		mb_c = c;
		ಆದರೆ (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    ಆದರೆ (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			ಆದರೆ (mb_c < 0x80)
			{
			    c = mb_c;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    c0 = mb_c;
#ಆದರೆ_ಅಂತ್ಯ
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			ಆದರೆ (utf_iscomposing(mb_c))
			{
			    ಪೂರ್ಣಾಂಕ i;

			    ಗಾಗಿ (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    ಆದರೆ ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			ಆದರೆ (wp->w_p_rl)		//ಹಿಮ್ಮುಖ
			    rl_mirror(wlv.extra);
#ಆದರೆ_ಅಂತ್ಯ
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (ಪೂರ್ಣಾಂಕ)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			ಆದರೆ (area_attr == 0 && ಹುಡುಕು_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			ಪೂರ್ಣಾಂಕ	pc, pc1, nc;
			ಪೂರ್ಣಾಂಕ	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ಆದರೆ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			prev_c = mb_c;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    ಆದರೆ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			ಆದರೆ (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_ಬಫರ್, wlv.extra, c);
				wlv.n_extra = (ಪೂರ್ಣಾಂಕ)STRLEN(wlv.extra) - 1;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
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
			    ಆದರೆ (area_attr == 0 && ಹುಡುಕು_attr == 0)
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
		ಆದರೆ ((
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ಲಂಬಸಾಲು <= 0) :
#ಆದರೆ_ಅಂತ್ಯ
				(wlv.ಲಂಬಸಾಲು >= wp->w_ಅಗಲ - 1))
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		    did_decrement_ptr = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		ಆದರೆ (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    ಆದರೆ (area_attr == 0 && ಹುಡುಕು_attr == 0)
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

	    ಆದರೆ (extra_check)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (ದೀರ್ಘ)(ptr - ಸಾಲು);
		ಆದರೆ (spv->spv_has_spell && v >= ಶಬ್ದ_ಕೊನೆ && v > cur_checked_ಲಂಬಸಾಲು)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    ಆದರೆ (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			ಪೂರ್ಣಾಂಕ	len;
			hlf_T	spell_hlf = HLF_ಎಣಿಕೆ;

			ಆದರೆ (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			ಆದರೆ ((prev_ptr - ಸಾಲು) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - ಸಾಲು) - nextlinecol;
			ಇಲ್ಲದಿದ್ದರೆ
			    p = prev_ptr;
			spv->spv_cap_ಲಂಬಸಾಲು -= (ಪೂರ್ಣಾಂಕ)(prev_ptr - ಸಾಲು);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_ಲಂಬಸಾಲು,
							    spv->spv_unchanged);
			ಶಬ್ದ_ಕೊನೆ = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			ಆದರೆ (spell_hlf != HLF_ಎಣಿಕೆ
				&& (State & ಬಹುಲಕ_ಸೇರಿಸು)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ಲಂಬಸಾಲು >=
						    (colnr_T)(prev_ptr - ಸಾಲು)
				&& wp->w_cursor.ಲಂಬಸಾಲು < (colnr_T)ಶಬ್ದ_ಕೊನೆ)
			{
			    spell_hlf = HLF_ಎಣಿಕೆ;
			    spell_redraw_lnum = lnum;
			}

			ಆದರೆ (spell_hlf == HLF_ಎಣಿಕೆ && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_ಲಂಬಸಾಲು = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			ಆದರೆ (spell_hlf != HLF_ಎಣಿಕೆ)
			    spell_attr = highlight_attr[spell_hlf];

			ಆದರೆ (spv->spv_cap_ಲಂಬಸಾಲು > 0)
			{
			    ಆದರೆ (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_ಲಂಬಸಾಲು
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_ಲಂಬಸಾಲು = ((p - nextline)
					    + spv->spv_cap_ಲಂಬಸಾಲು - nextline_idx);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				// Compute the actual column.
				spv->spv_cap_ಲಂಬಸಾಲು += (prev_ptr - ಸಾಲು);
			}
		    }
		}
		ಆದರೆ (spell_attr != 0)
		{
		    ಆದರೆ (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    ಇಲ್ಲದಿದ್ದರೆ
			wlv.char_attr = hl_combine_attr(spell_attr,
								wlv.char_attr);
		}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// we don't want linebreak to apply for lines that start with
		// leading spaces, followed by long letters (since it would add
		// a break at the beginning of a line and this might be unexpected)
		//
		// So only allow to linebreak, once we have found chars not in
		// 'breakat' in the line.
		ಆದರೆ ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((ಪೂರ್ಣಾಂಕ)*ptr))
		    wlv.need_lbr = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Found last space before word: check for line break.
		ಆದರೆ (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((ಪೂರ್ಣಾಂಕ)*ptr))
		{
		    ಪೂರ್ಣಾಂಕ	    mb_off = has_mbyte ? (*mb_ತಲೆ_off)(ಸಾಲು, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_ಮೊದಲ_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, ಸಾಲು, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_ಪಠ್ಯ = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ಖಾಲಿ, ಖಾಲಿ) - 1;
		    clear_chartabsize_arg(&cts);

		    ಆದರೆ (on_ಕೊನೆಯ_ಲಂಬಸಾಲು && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			ಹುಡುಕು_attr = 0;

		    ಆದರೆ (c == TAB && wlv.n_extra + wlv.ಲಂಬಸಾಲು > wp->w_ಅಗಲ)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_ಬಫರ್->b_p_ts,
					      wp->w_ಬಫರ್->b_p_vts_array) - 1;
# ಇಲ್ಲದಿದ್ದರೆ
			wlv.n_extra = (ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts
				    - wlv.vcol % (ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    ಆದರೆ (wlv.n_extra > 0 && c != TAB)
			ಒಳ_linebreak = TRUE;
# endif
		    ಆದರೆ (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			ಆದರೆ (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			ಆದರೆ (!wp->w_p_list)
			    c = ' ';
		    }
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (wp->w_p_list)
		{
		    ಒಳ_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > ಸಾಲು && prev_ptr[-1] == ' '));
		    ಆದರೆ (!ಒಳ_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		ಆದರೆ (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (ಒಳ_multispace
					&& wp->w_lcs_chars.multispace != ಖಾಲಿ))
				&& ptr - ಸಾಲು >= leadcol
				&& ptr - ಸಾಲು <= trailcol)))
		{
		    ಆದರೆ (ಒಳ_multispace && wp->w_lcs_chars.multispace != ಖಾಲಿ)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			ಆದರೆ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    ಆದರೆ (area_attr == 0 && ಹುಡುಕು_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			mb_utf8 = FALSE;
		}

		ಆದರೆ (c == ' ' && ((trailcol != MAXCOL && ptr > ಸಾಲು + trailcol)
				    || (leadcol != 0 && ptr < ಸಾಲು + leadcol)))
		{
		    ಆದರೆ (leadcol != 0 && ಒಳ_multispace && ptr < ಸಾಲು + leadcol
			    && wp->w_lcs_chars.leadmultispace != ಖಾಲಿ)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			ಆದರೆ (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ptr > ಸಾಲು + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ptr < ಸಾಲು + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    ಆದರೆ (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    ಆದರೆ (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		ಆದರೆ (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    ಪೂರ್ಣಾಂಕ	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    ದೀರ್ಘ    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    ಪೂರ್ಣಾಂಕ	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    ಪೂರ್ಣಾಂಕ	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    ಪೂರ್ಣಾಂಕ	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ಆದರೆ (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < ಸಾಲು + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_ಮೌಲ್ಯ(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    ಆದರೆ (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#ಆದರೆ_ಅಂತ್ಯ
		    // tab amount depends on current column
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_ಬಫರ್->b_p_ts,
					      wp->w_ಬಫರ್->b_p_vts_array) - 1;
#ಇಲ್ಲದಿದ್ದರೆ
		    tab_len = (ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts
			       - vcol_adjusted % (ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts - 1;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		    ಆದರೆ (!wp->w_p_lbr || !wp->w_p_list)
#ಆದರೆ_ಅಂತ್ಯ
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			char_u	*p;
			ಪೂರ್ಣಾಂಕ	len;
			ಪೂರ್ಣಾಂಕ	i;
			ಪೂರ್ಣಾಂಕ	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			ಆದರೆ (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			ಆದರೆ (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			ಆದರೆ (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    ಪೂರ್ಣಾಂಕ tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    ಆದರೆ (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    ಆದರೆ (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    ಆದರೆ (p == ಖಾಲಿ)
				wlv.n_extra = 0;
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				ಗಾಗಿ (i = 0; i < tab_len; i++)
				{
				    ಪೂರ್ಣಾಂಕ lcs = lcs_tab2;

				    ಆದರೆ (*p == NUL)
				    {
					tab_len = i;
					ನಿಲ್ಲಿಸು;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    ಆದರೆ (lcs_tab3 && i == tab_len - 1)
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
				ಆದರೆ (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		    {
			ಪೂರ್ಣಾಂಕ vc_saved = wlv.vcol_off_co;

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
			ಆದರೆ (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    ಆದರೆ (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			ಆದರೆ (wp->w_p_lbr && wlv.p_extra != ಖಾಲಿ
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == NUL
			&& wlv.n_extra == 0
			&& (wp->w_p_list
			    || (wlv.fromcol >= 0
				&& wlv.tocol > wlv.vcol
				&& VIsual_ಬಹುಲಕ != Ctrl_V
				&& (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
				    wp->w_p_rl ? (wlv.ಲಂಬಸಾಲು >= 0) :
#ಆದರೆ_ಅಂತ್ಯ
				    (wlv.ಲಂಬಸಾಲು < wp->w_ಅಗಲ))))
			&& lcs_eol_one > 0)
		{
		    // Display a '$' after the line or highlight an extra
		    // character if the line break is included.
#ಆದರೆ defined(FEAT_DIFF) || defined(ಸಾಲು_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    ಆದರೆ (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef ಸಾಲು_ATTR
			    &&
#  endif
# endif
# ifdef ಸಾಲು_ATTR
			    wlv.ಸಾಲು_attr == 0
# endif
		       )
#ಆದರೆ_ಅಂತ್ಯ
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			ಆದರೆ (!(area_highlighting && virtual_ಸಕ್ರಿಯ()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    ಆದರೆ (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    ಇಲ್ಲದಿದ್ದರೆ
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    ಆದರೆ (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_ಬಫರ್, c);
		    ಆದರೆ (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    ಆದರೆ ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#ಆದರೆ_ಅಂತ್ಯ
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		    ಆದರೆ (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			ಆದರೆ (p == ಖಾಲಿ)
			    wlv.n_extra = 0;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    ಆದರೆ (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			ಆದರೆ (ಪಠ್ಯ_prop_type != ಖಾಲಿ &&
				 ಪಠ್ಯ_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(ಪಠ್ಯ_prop_attr,
							       wlv.extra_attr);
#ಆದರೆ_ಅಂತ್ಯ

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ
			 && (VIsual_ಬಹುಲಕ == Ctrl_V
			     || VIsual_ಬಹುಲಕ == 'v')
			 && virtual_ಸಕ್ರಿಯ()
			 && wlv.tocol != MAXCOL
			 && wlv.vcol < wlv.tocol
			 && (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ಲಂಬಸಾಲು >= 0) :
#ಆದರೆ_ಅಂತ್ಯ
			    (wlv.ಲಂಬಸಾಲು < wp->w_ಅಗಲ)))
		{
		    c = ' ';
		    --ptr;	    // put it back at the NUL
		}
#ಆದರೆ defined(ಸಾಲು_ATTR)
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.ಸಾಲು_attr != 0
			) && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ಲಂಬಸಾಲು >= 0) :
# endif
			    (wlv.ಲಂಬಸಾಲು
# ifdef FEAT_CONCEAL
				- wlv.boguscols
# endif
					    < wp->w_ಅಗಲ)))
		{
		    // Highlight until the right side of the window
		    c = ' ';
		    --ptr;	    // put it back at the NUL

		    // Remember we do the char for line highlighting.
		    ++did_ಸಾಲು_attr;

		    // don't do search HL for the rest of the line
		    ಆದರೆ (wlv.ಸಾಲು_attr != 0 && wlv.char_attr == ಹುಡುಕು_attr
					&& (did_ಸಾಲು_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.ಸಾಲು_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    ಆದರೆ (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_ಸಾಲು
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    ಆದರೆ (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			ಆದರೆ (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    ಆದರೆ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ಎಡ_curline_ಲಂಬಸಾಲು
					    && wlv.vcol <= ಬಲ_curline_ಲಂಬಸಾಲು)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    ಆದರೆ (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			ಆದರೆ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    ಆದರೆ (!wlv.cul_screenline
				    || (wlv.vcol >= ಎಡ_curline_ಲಂಬಸಾಲು
					     && wlv.vcol <= ಬಲ_curline_ಲಂಬಸಾಲು))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wlv.ಸಾಲು_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.ಸಾಲು_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			ಆದರೆ (wlv.vcol == 0)
			{
			    ಆದರೆ (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹುಡುಕು_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, ಹುಡುಕು_attr);
			}
		    }
# endif
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    ಆದರೆ (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_ಸಾಲು(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_ಹೊಂದಿಕೆ_conc > 0)
		&& !(lnum_ಒಳ_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ಖಾಲಿ))
	    {
		ಪೂರ್ಣಾಂಕ syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		ಆದರೆ (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_ಹೊಂದಿಕೆ_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_ಹೊಂದಿಕೆ_conc && ಹೊಂದಿಕೆ_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    ಆದರೆ (has_ಹೊಂದಿಕೆ_conc && ಹೊಂದಿಕೆ_conc)
			c = ಹೊಂದಿಕೆ_conc;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    ಇಲ್ಲದಿದ್ದರೆ
			c = ' ';

		    ಆದರೆ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    ಆದರೆ (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    ಆದರೆ (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			ಆದರೆ (wp->w_p_rl)
			{
			    wlv.ಲಂಬಸಾಲು -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			ಇಲ್ಲದಿದ್ದರೆ
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.ಲಂಬಸಾಲು += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    ಆದರೆ (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CONCEAL
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	ಆದರೆ (!did_wcol && wlv.draw_state == WL_ಸಾಲು
		&& ಒಳ_curline && conceal_cursor_ಸಾಲು(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
		wp->w_wcol = wp->w_ಅಗಲ - wlv.ಲಂಬಸಾಲು + wlv.boguscols - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		wp->w_wcol = wlv.ಲಂಬಸಾಲು - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    ಆದರೆ (wlv.vcol + skip_cells < wp->w_virtcol)
		// Cursor beyond end of the line with 'virtualedit'.
		wp->w_wcol += wp->w_virtcol - wlv.vcol - skip_cells;
	    wp->w_wrow = wlv.ಅಡ್ಡಸಾಲು;
	    did_wcol = TRUE;
	    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
# ifdef FEAT_PROP_POPUP
	    curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Use "wlv.extra_attr", but don't override visual selection
	// highlighting, unless text property overrides.
	// Don't use "wlv.extra_attr" until wlv.n_attr_skip is zero.
	ಆದರೆ (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_ಸಾಲು
		&& (!attr_pri
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    || (ಪಠ್ಯ_prop_flags & PT_FLAG_OVERRIDE)
#ಆದರೆ_ಅಂತ್ಯ
		   ))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಾಲು_ATTR
	    ಆದರೆ (wlv.ಸಾಲು_attr)
		wlv.char_attr = hl_combine_attr(wlv.ಸಾಲು_attr, wlv.extra_attr);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		wlv.char_attr = wlv.extra_attr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	ಆದರೆ (p_imst == IM_ON_THE_SPOT
		&& xic != ಖಾಲಿ
		&& lnum == wp->w_cursor.lnum
		&& (State & ಬಹುಲಕ_ಸೇರಿಸು)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_ಸಾಲು)
	{
	    colnr_T tcol;

	    ಆದರೆ (preedit_ಕೊನೆ_ಲಂಬಸಾಲು == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ಖಾಲಿ, ಖಾಲಿ, 0);
	    ಇಲ್ಲದಿದ್ದರೆ
		tcol = preedit_ಕೊನೆ_ಲಂಬಸಾಲು;
	    ಆದರೆ ((ದೀರ್ಘ)preedit_ಆರಂಭ_ಲಂಬಸಾಲು <= wlv.vcol && wlv.vcol < (ದೀರ್ಘ)tcol)
	    {
		ಆದರೆ (feedback_old_attr < 0)
		{
		    feedback_ಲಂಬಸಾಲು = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_ಲಂಬಸಾಲು);
		ಆದರೆ (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_ಲಂಬಸಾಲು++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (feedback_old_attr >= 0)
	    {
		wlv.char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_ಲಂಬಸಾಲು = 0;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	// Handle the case where we are in column 0 but not on the first
	// character of the line and the user wants us to show us a
	// special character (via 'listchars' option "precedes:<char>").
	ಆದರೆ (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? (wp->w_skipcol > 0 && wlv.ಅಡ್ಡಸಾಲು == 0)
				 : wp->w_leftcol > 0)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
		&& wlv.draw_state > WL_NR
		&& skip_cells <= 0
		&& c != NUL)
	{
	    c = wp->w_lcs_chars.prec;
	    lcs_prec_todo = NUL;
	    ಆದರೆ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    ಆದರೆ (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	ಆದರೆ ((c == NUL
#ಆದರೆ defined(ಸಾಲು_ATTR)
		|| did_ಸಾಲು_attr == 1
#ಆದರೆ_ಅಂತ್ಯ
		) && wlv.eol_hl_off == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    ಪೂರ್ಣಾಂಕ prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_ಹುಡುಕು_hl,
					      (ದೀರ್ಘ)(ptr - ಸಾಲು) - (c == NUL));
#ಆದರೆ_ಅಂತ್ಯ
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    ಆದರೆ (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_ಬಹುಲಕ != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_ಸಕ್ರಿಯ))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# ಆದರೆ defined(ಸಾಲು_ATTR)
			    && did_ಸಾಲು_attr <= 1
# endif
			   )
#ಆದರೆ_ಅಂತ್ಯ
		       ))
	    {
		ಪೂರ್ಣಾಂಕ n = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		ಆದರೆ (wp->w_p_rl)
		{
		    ಆದರೆ (wlv.ಲಂಬಸಾಲು < 0)
			n = 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    ಆದರೆ (wlv.ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
			n = -1;
		}
		ಆದರೆ (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.ಲಂಬಸಾಲು += n;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Add a blank character to highlight.
		    Screen_ಸಾಲುಗಳು[wlv.off] = ' ';
		    ಆದರೆ (enc_utf8)
			Screen_ಸಾಲುಗಳು_UC[wlv.off] = 0;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		ಆದರೆ (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_ಹುಡುಕು_ಹೊಂದಿಕೆ_hl(wp, &screen_ಹುಡುಕು_hl,
					   (ದೀರ್ಘ)(ptr - ಸಾಲು), &wlv.char_attr);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ScreenAttrs[wlv.off] = wlv.char_attr;
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv.off] = wlv.vcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		ಆದರೆ (wp->w_p_rl)
		{
		    --wlv.ಲಂಬಸಾಲು;
		    --wlv.off;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    ++wlv.ಲಂಬಸಾಲು;
		    ++wlv.off;
		}
		++wlv.vcol;
		wlv.eol_hl_off = 1;
	    }
	}

	// At end of the text line.
	ಆದರೆ (c == NUL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (ಪಠ್ಯ_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		draw_screen_ಸಾಲು(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		ಆದರೆ (ಒಳ_curline)
		{
		    curwin->w_cline_ಅಡ್ಡಸಾಲು = startrow;
		    curwin->w_cline_ಎತ್ತರ = wlv.ಅಡ್ಡಸಾಲು - startrow;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    curwin->w_cline_folded = false;
#ಆದರೆ_ಅಂತ್ಯ
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	ಪೂರ್ಣಾಂಕ lcs_ext = get_lcs_ext(wp);
	ಆದರೆ (lcs_ext != NUL
		&& wlv.draw_state == WL_ಸಾಲು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
		&& (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    wp->w_p_rl ? wlv.ಲಂಬಸಾಲು == 0 :
#ಆದರೆ_ಅಂತ್ಯ
		    wlv.ಲಂಬಸಾಲು == wp->w_ಅಗಲ - 1)
		&& (*ptr != NUL
		    || lcs_eol_one > 0
		    || (wlv.n_extra > 0 && (wlv.c_extra != NUL
						     || *wlv.p_extra != NUL))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    || ಪಠ್ಯ_prop_ಮುಂದಿನ <= ಕೊನೆಯ_textprop_ಪಠ್ಯ_idx
#ಆದರೆ_ಅಂತ್ಯ
		   ))
	{
	    c = lcs_ext;
	    wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    mb_c = c;
	    ಆದರೆ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mb_utf8 = FALSE;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	ಆದರೆ (wlv.draw_color_ಲಂಬಸಾಲು)
	    wlv.draw_color_ಲಂಬಸಾಲು = advance_color_ಲಂಬಸಾಲು(VCOL_HLC, &wlv.color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	ಆದರೆ (((wlv.draw_state == WL_ಸಾಲು
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_ಒಳ_visual_area
		&& ಹುಡುಕು_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    ಆದರೆ (wp->w_p_cuc && VCOL_HLC == (ದೀರ್ಘ)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wlv.draw_color_ಲಂಬಸಾಲು && VCOL_HLC == *wlv.color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	ಆದರೆ (wlv.draw_state < WL_ಸಾಲು || skip_cells <= 0)
	{
	    // Store the character.
#ಆದರೆ defined(FEAT_RIGHTLEFT)
	    ಆದರೆ (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.ಲಂಬಸಾಲು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    Screen_ಸಾಲುಗಳು[wlv.off] = c;
	    ಆದರೆ (enc_dbcs == DBCS_JPNU)
	    {
		ಆದರೆ ((mb_c & 0xff00) == 0x8e00)
		    Screen_ಸಾಲುಗಳು[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
	    {
		ಆದರೆ (mb_utf8)
		{
		    ಪೂರ್ಣಾಂಕ i;

		    Screen_ಸಾಲುಗಳು_UC[wlv.off] = mb_c;
		    ಆದರೆ ((c & 0xff) == 0)
			Screen_ಸಾಲುಗಳು[wlv.off] = 0x80;   // avoid storing zero
		    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ಸಾಲುಗಳು_C[i][wlv.off] = u8cc[i];
			ಆದರೆ (u8cc[i] == 0)
			    ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    Screen_ಸಾಲುಗಳು_UC[wlv.off] = 0;
	    }
	    ಆದರೆ (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    ಆದರೆ (wlv.draw_state > WL_NR
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    && wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
		    )
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv.off] = wlv.vcol;
	    ಇಲ್ಲದಿದ್ದರೆ
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv.off] = -1;

	    ಆದರೆ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.ಲಂಬಸಾಲು;
		ಆದರೆ (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_ಸಾಲುಗಳು[wlv.off] = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    // DBCS: Put second byte in the second screen char.
		    Screen_ಸಾಲುಗಳು[wlv.off] = mb_c & 0xff;

		ಆದರೆ (wlv.draw_state > WL_NR
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			&& wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
			)
		    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv.off] = ++wlv.vcol;
		ಇಲ್ಲದಿದ್ದರೆ
		    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		ಆದರೆ (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		ಆದರೆ (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.ಲಂಬಸಾಲು;
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		++wlv.off;
		++wlv.ಲಂಬಸಾಲು;
	    }
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_p_cole > 0 && is_concealing)
	{
	    ಪೂರ್ಣಾಂಕ concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    ಆದರೆ (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    ಆದರೆ (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    ಆದರೆ (wp->w_p_wrap)
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
		ಪೂರ್ಣಾಂಕ adjust_ಲಂಬಸಾಲು = !WIN_IS_POPUP(wp);
# ಇಲ್ಲದಿದ್ದರೆ
		ಪೂರ್ಣಾಂಕ adjust_ಲಂಬಸಾಲು = TRUE;
# endif
		ಆದರೆ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    ಆದರೆ (wp->w_p_rl)
		    {
			ಆದರೆ (adjust_ಲಂಬಸಾಲು)
			{
			    wlv.ಲಂಬಸಾಲು -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
		    {
			ಆದರೆ (adjust_ಲಂಬಸಾಲು)
			{
			    wlv.ಲಂಬಸಾಲು += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		ಆದರೆ (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    ಆದರೆ (wp->w_p_rl)
		    {
			ಆದರೆ (adjust_ಲಂಬಸಾಲು)
			{
			    --wlv.boguscols;
			    --wlv.ಲಂಬಸಾಲು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
		    {
			ಆದರೆ (adjust_ಲಂಬಸಾಲು)
			{
			    ++wlv.boguscols;
			    ++wlv.ಲಂಬಸಾಲು;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		ಆದರೆ (wp->w_p_rl)
		{
		    ಆದರೆ (adjust_ಲಂಬಸಾಲು)
		    {
			--wlv.boguscols;
			--wlv.ಲಂಬಸಾಲು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
# endif
		{
		    ಆದರೆ (adjust_ಲಂಬಸಾಲು)
		    {
			++wlv.boguscols;
			++wlv.ಲಂಬಸಾಲು;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_CONCEAL
	ಇಲ್ಲದಿದ್ದರೆ
	    --skip_cells;

	ಆದರೆ (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	ಆದರೆ (wlv.draw_state > WL_NR
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
		)
	    ++wlv.vcol;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#ಆದರೆ_ಅಂತ್ಯ

	// restore attributes after "precedes" in 'listchars'
	ಆದರೆ (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	ಆದರೆ (n_attr > 0 && wlv.draw_state == WL_ಸಾಲು
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	ಆದರೆ (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	ಆದರೆ ((
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.ಲಂಬಸಾಲು < 0) :
#ಆದರೆ_ಅಂತ್ಯ
				    (wlv.ಲಂಬಸಾಲು >= wp->w_ಅಗಲ))
		&& (wlv.draw_state != WL_ಸಾಲು
		    || *ptr != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    || wlv.filler_todo > 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    || ಪಠ್ಯ_prop_above || ಪಠ್ಯ_prop_follows
		    || ಪಠ್ಯ_prop_ಮುಂದಿನ <= ಕೊನೆಯ_textprop_ಪಠ್ಯ_idx
#ಆದರೆ_ಅಂತ್ಯ
		    || (wp->w_p_list && wp->w_lcs_chars.eol != NUL
						&& lcs_eol_one != -1)
		    || (wlv.n_extra != 0 && (wlv.c_extra != NUL
						      || *wlv.p_extra != NUL)))
		)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    wlv.ಲಂಬಸಾಲು -= wlv.boguscols;
	    // Apply 'cursorline' and HLF_WIN highlight.
	    ಆದರೆ (wlv.boguscols != 0 && (
# ifdef ಸಾಲು_ATTR
			wlv.ಸಾಲು_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		ಪೂರ್ಣಾಂಕ attr = wlv.win_attr;
# ifdef ಸಾಲು_ATTR
		ಆದರೆ (wlv.ಸಾಲು_attr != 0)
		    attr = hl_combine_attr(attr, wlv.ಸಾಲು_attr);
# endif
		ಆಗ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.ಲಂಬಸಾಲು >= 0 :
# endif
			    wlv.ಲಂಬಸಾಲು < wp->w_ಅಗಲ))
		{
		    Screen_ಸಾಲುಗಳು[wlv.off] = ' ';
		    ಆದರೆ (enc_utf8)
			Screen_ಸಾಲುಗಳು_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    ಆದರೆ (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.ಲಂಬಸಾಲು--;
			wlv.boguscols++;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
		    {
			wlv.off++;
			wlv.ಲಂಬಸಾಲು++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_ಸಾಲು(wp, &wlv, TRUE);
	    wlv.ಲಂಬಸಾಲು += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#ಇಲ್ಲದಿದ್ದರೆ
	    wlv_screen_ಸಾಲು(wp, &wlv, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
	    ++wlv.ಅಡ್ಡಸಾಲು;
	    ++wlv.screen_ಅಡ್ಡಸಾಲು;

	    // When not wrapping and finished diff lines, break here.
	    ಆದರೆ (!wp->w_p_wrap
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			&& wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			&& !ಪಠ್ಯ_prop_above
			&& !ಪಠ್ಯ_prop_follows
#ಆದರೆ_ಅಂತ್ಯ
		       )
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (!wp->w_p_wrap && ಪಠ್ಯ_prop_follows && !ಪಠ್ಯ_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = ಸಾಲು + (size_t)ml_get_buf_len(wp->w_ಬಫರ್, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // When the window is too narrow draw all "@" lines.
	    ಆದರೆ (wlv.draw_state != WL_ಸಾಲು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    && wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		win_draw_ಕೊನೆ(wp, '@', ' ', TRUE, wlv.ಅಡ್ಡಸಾಲು, wp->w_ಎತ್ತರ, HLF_AT);
		draw_vsep_win(wp, wlv.ಅಡ್ಡಸಾಲು);
		wlv.ಅಡ್ಡಸಾಲು = endrow;
	    }

	    // When line got too long for screen break here.
	    ಆದರೆ (wlv.ಅಡ್ಡಸಾಲು == endrow)
	    {
		++wlv.ಅಡ್ಡಸಾಲು;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (screen_cur_ಅಡ್ಡಸಾಲು == wlv.screen_ಅಡ್ಡಸಾಲು - 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		     && wlv.filler_todo <= 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		     && !ಪಠ್ಯ_prop_above && !ಪಠ್ಯ_prop_follows
#ಆದರೆ_ಅಂತ್ಯ
		     && wp->w_ಅಗಲ == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		ಸಾಲು_Wraps[wlv.screen_ಅಡ್ಡಸಾಲು - 1] = TRUE;

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
		ಆದರೆ (p_tf
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			 && !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
			 && !(has_mbyte
			     && ((*mb_off2cells)(ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು],
				   ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು - 1]
							    + (ಪೂರ್ಣಾಂಕ)topframe->fr_ಅಗಲ - 2,
				     ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    ಆದರೆ (screen_cur_ಲಂಬಸಾಲು != wp->w_ಅಗಲ)
			screen_char(ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು - 1]
						       + (ಚಿಹ್ನೆರಹಿತ)topframe->fr_ಅಗಲ - 1,
				       wlv.screen_ಅಡ್ಡಸಾಲು - 1, (ಪೂರ್ಣಾಂಕ)(topframe->fr_ಅಗಲ - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    ಆದರೆ (has_mbyte && MB_BYTE2LEN(Screen_ಸಾಲುಗಳು[ಸಾಲು_ಸ್ಥಾನಾಂತರ[
				     wlv.screen_ಅಡ್ಡಸಾಲು - 1] + (topframe->fr_ಅಗಲ - 1)]) > 1)
			ಹೊರಹೋಗು_char(' ');
		    ಇಲ್ಲದಿದ್ದರೆ
			ಹೊರಹೋಗು_char(Screen_ಸಾಲುಗಳು[ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು - 1]
							    + (topframe->fr_ಅಗಲ - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[wlv.screen_ಅಡ್ಡಸಾಲು]] = (sattr_T)-1;
		    screen_ಆರಂಭ();	// don't know where cursor is now
		}
	    }

	    win_ಸಾಲು_ಆರಂಭ(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    ಆದರೆ (!wlv.dont_use_showbreak
# ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
# endif
	       )
		wlv.need_showbreak = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    --wlv.filler_todo;
	    // When the filler lines are actually below the last line of the
	    // file, don't draw the line itself, break here.
	    ಆದರೆ (wlv.filler_todo == 0 && wp->w_botfill)
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	}

    }	// for every character in the line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (ಪಠ್ಯ_props != ಪಠ್ಯ_props_buf)
	vim_free(ಪಠ್ಯ_props);
    ಆದರೆ (ಪಠ್ಯ_prop_idxs != ಪಠ್ಯ_prop_idxs_buf)
	vim_free(ಪಠ್ಯ_prop_idxs);
    ಆದರೆ (ಪಠ್ಯ_prop_suffix_flags != ಪಠ್ಯ_prop_suffix_flags_buf)
	vim_free(ಪಠ್ಯ_prop_suffix_flags);
    vim_free(p_extra_free2);
#ಆದರೆ_ಅಂತ್ಯ

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    ಹಿಂತಿರುಗಿ wlv.ಅಡ್ಡಸಾಲು;
}
