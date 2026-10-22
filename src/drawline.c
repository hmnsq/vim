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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lcs_ext(win_T *wp)
{
    ಯದಿ (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	ಪ್ರತ್ಯಾಗಮ NUL;
    ಯದಿ (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	ಪ್ರತ್ಯಾಗಮ '>';
    ಪ್ರತ್ಯಾಗಮ wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ifdef FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
advance_color_col(ಪೂರ್ಣಾಙ್ಕ vcol, ಪೂರ್ಣಾಙ್ಕ **color_cols)
{
    ಯಾವತ್ (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    ಪ್ರತ್ಯಾಗಮ (**color_cols >= 0);
}
#endif

#ifdef FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
margin_columns_win(win_T *wp, ಪೂರ್ಣಾಙ್ಕ *ಎಡ_col, ಪೂರ್ಣಾಙ್ಕ *ಬಲ_col)
{
    // cache previous calculations depending on w_virtcol
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ saved_w_virtcol;
    ಸ್ಥಿರ win_T *prev_wp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_width1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_width2;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_ಎಡ_col;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_ಬಲ_col;

    ಪೂರ್ಣಾಙ್ಕ cur_col_off = win_col_off(wp);
    ಪೂರ್ಣಾಙ್ಕ	width1;
    ಪೂರ್ಣಾಙ್ಕ	width2;

    width1 = wp->w_width - cur_col_off;
    width2 = width1 + win_col_off2(wp);

    ಯದಿ (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*ಬಲ_col = prev_ಬಲ_col;
	*ಎಡ_col = prev_ಎಡ_col;
	ಪ್ರತ್ಯಾಗಮ;
    }

    *ಎಡ_col = 0;
    *ಬಲ_col = width1;

    ಯದಿ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ಬಲ_col = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    ಯದಿ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ಎಡ_col = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ಎಡ_col = *ಎಡ_col;
    prev_ಬಲ_col = *ಬಲ_col;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#endif

#ಯದಿ defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define ಸಾಲು_ATTR
#endif

// structure with variables passed between win_line() and other functions
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    ಪೂರ್ಣಾಙ್ಕ		startrow;	// first row in the window to be drawn
    ಪೂರ್ಣಾಙ್ಕ		row;		// row in the window, excl w_winrow
    ಪೂರ್ಣಾಙ್ಕ		screen_row;	// row on the screen, incl w_winrow

    long	vcol;		// virtual column, before wrapping
    ಪೂರ್ಣಾಙ್ಕ		col;		// visual column on screen, after wrapping
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    ಪೂರ್ಣಾಙ್ಕ		vcol_off_co;	// offset for concealed characters
#endif
    ಪೂರ್ಣಾಙ್ಕ		vcol_off_tp;	// offset for virtual text
#ifdef FEAT_SYN_HL
    ಪೂರ್ಣಾಙ್ಕ		draw_color_col;	// highlight colorcolumn
    ಪೂರ್ಣಾಙ್ಕ		*color_cols;	// pointer to according columns array
#endif
    ಪೂರ್ಣಾಙ್ಕ		eol_hl_off;	// 1 if highlighted char after EOL

    unsigned	off;		// offset in ScreenLines/ScreenAttrs

    ಪೂರ್ಣಾಙ್ಕ		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ifdef FEAT_SYN_HL
    ಪೂರ್ಣಾಙ್ಕ		cul_attr;	// set when 'cursorline' active
#endif
#ifdef ಸಾಲು_ATTR
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_attr;	// for the whole line, includes 'cursorline'
#endif

    ಪೂರ್ಣಾಙ್ಕ		screen_ಸಾಲು_flags;  // flags for screen_line()

    ಪೂರ್ಣಾಙ್ಕ		fromcol;	// start of inverting
    ಪೂರ್ಣಾಙ್ಕ		tocol;		// end of inverting

#ifdef FEAT_LINEBREAK
    long	vcol_sbr;	    // virtual column after showbreak
    ಪೂರ್ಣಾಙ್ಕ		need_showbreak;	    // overlong line, skipping first x chars
    ಪೂರ್ಣಾಙ್ಕ		dont_use_showbreak; // do not use 'showbreak'
#endif
#ifdef FEAT_PROP_POPUP
    ಪೂರ್ಣಾಙ್ಕ		text_prop_above_ಎಣಿಕೆ;
#endif

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    ಪೂರ್ಣಾಙ್ಕ		cul_screenline;

    ಪೂರ್ಣಾಙ್ಕ		char_attr;	// attributes for the next character

    ಪೂರ್ಣಾಙ್ಕ		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    ಪೂರ್ಣಾಙ್ಕ		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    ಪೂರ್ಣಾಙ್ಕ		n_attr_skip;    // chars to skip before using extra_attr
    ಪೂರ್ಣಾಙ್ಕ		c_extra;	// extra chars, all the same
    ಪೂರ್ಣಾಙ್ಕ		c_final;	// final char, mandatory if set
    ಪೂರ್ಣಾಙ್ಕ		extra_for_textprop; // n_extra set for textprop
    ಪೂರ್ಣಾಙ್ಕ		start_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    ಪೂರ್ಣಾಙ್ಕ		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    ಪೂರ್ಣಾಙ್ಕ		saved_extra_attr;
    ಪೂರ್ಣಾಙ್ಕ		saved_n_attr_skip;
    ಪೂರ್ಣಾಙ್ಕ		saved_extra_for_textprop;
    ಪೂರ್ಣಾಙ್ಕ		saved_c_extra;
    ಪೂರ್ಣಾಙ್ಕ		saved_c_final;
    ಪೂರ್ಣಾಙ್ಕ		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ifdef FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#endif
    ಪೂರ್ಣಾಙ್ಕ		filler_ಸಾಲುಗಳು;	// nr of filler lines to be drawn
    ಪೂರ್ಣಾಙ್ಕ		filler_todo;	// nr of filler lines still to do + 1
#ifdef FEAT_SIGNS
    sign_attrs_T sattr;
#endif
#ifdef FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    ಪೂರ್ಣಾಙ್ಕ		need_lbr;
#endif
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#define WL_START	0		// nothing done yet, must be zero
#define WL_CMDLINE	(WL_START + 1)	// cmdline window column
#ifdef FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#ಅನ್ಯಥಾ
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#ಅನ್ಯಥಾ
# define WL_SIGN	WL_FOLD		// column for signs
#endif
#define WL_NR		(WL_SIGN + 1)	// line number
#ifdef FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#ಅನ್ಯಥಾ
# define WL_BRI		WL_NR
#endif
#ಯದಿ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#ಅನ್ಯಥಾ
# define WL_SBR		WL_BRI
#endif
#define WL_ಸಾಲು		(WL_SBR + 1)	// text in the line

#ಯದಿ defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
use_cursor_ಸಾಲು_highlight(win_T *wp, linenr_T lnum)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#endif


#ifdef FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    ಪೂರ್ಣಾಙ್ಕ fdc = compute_foldcolumn(wp, 0);

    ಯದಿ (fdc <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    ಯದಿ (wlv->p_extra_free == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    wlv->n_extra = (ಪೂರ್ಣಾಙ್ಕ)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    ಯದಿ (use_cursor_ಸಾಲು_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    ಅನ್ಯಥಾ
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_FC));
}
#endif

#ifdef FEAT_SIGNS
/*
 * Get information needed to display the sign in line "wlv->lnum" in window
 * "wp".
 * If "nrcol" is TRUE, the sign is going to be displayed in the number column.
 * Otherwise the sign is going to be displayed in the sign column.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_sign_display_info(
	ಪೂರ್ಣಾಙ್ಕ		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    ಪೂರ್ಣಾಙ್ಕ	text_sign;
# ifdef FEAT_SIGN_ICONS
    ಪೂರ್ಣಾಙ್ಕ	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    ಯದಿ (nrcol)
	wlv->n_extra = ಸಂಖ್ಯೆ_width(wp) + 1;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (use_cursor_ಸಾಲು_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	ಅನ್ಯಥಾ
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    ಯದಿ (wlv->row == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_ಸಾಲುಗಳು && wlv->filler_todo <= 0
# endif
       )
    {
	text_sign = (wlv->sattr.sat_text != ರಿಕ್ತ) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ರಿಕ್ತ) ? wlv->sattr.sat_typenr : 0;
	ಯದಿ (gui.in_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    ಯದಿ (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((ವರ್ಣ *)wlv->extra, ಪರಿಮಾಣ(wlv->extra),
						"%-*c ", ಸಂಖ್ಯೆ_width(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    ಅನ್ಯಥಾ
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    ಯದಿ (netbeans_active() && (buf_signcount(wp->w_buffer, wlv->lnum)
									  > 1))
	    {
		ಯದಿ (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((ವರ್ಣ *)wlv->extra, ಪರಿಮಾಣ(wlv->extra),
						"%-*c ", ಸಂಖ್ಯೆ_width(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		ಅನ್ಯಥಾ
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	ಅನ್ಯಥಾ
# endif
	    ಯದಿ (text_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_text;
		ಯದಿ (wlv->p_extra != ರಿಕ್ತ)
		{
		    ಯದಿ (nrcol)
		    {
			ಪೂರ್ಣಾಙ್ಕ width = ಸಂಖ್ಯೆ_width(wp) - 2;

			vim_memset(wlv->extra, ' ', width);
			wlv->n_extra = width;
			wlv->n_extra += vim_snprintf((ವರ್ಣ *)wlv->extra + width,
				  ಪರಿಮಾಣ(wlv->extra) - width, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    ಅನ್ಯಥಾ
			wlv->n_extra = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		ಯದಿ (use_cursor_ಸಾಲು_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		ಅನ್ಯಥಾ
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#endif

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_lnum_col(
	win_T		*wp,
	winlinevars_T	*wlv,
	ಪೂರ್ಣಾಙ್ಕ		sign_present UNUSED,
	ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_attr UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ lnum_row = wlv->startrow + wlv->filler_ಸಾಲುಗಳು
#ifdef FEAT_PROP_POPUP
		      + wlv->text_prop_above_ಎಣಿಕೆ
#endif
		      ;

    ಯದಿ ((wp->w_p_nu || wp->w_p_rnu)
	     && (wlv->row <= lnum_row || !has_cpo_n)
	     // there is no line number in a wrapped line when "n" is in
	     // 'cpoptions', but 'breakindent' assumes it anyway.
	     && !((has_cpo_n
#ifdef FEAT_LINEBREAK
		     && !wp->w_p_bri
#endif
		  ) && wp->w_skipcol > 0 && wlv->lnum == wp->w_topline))
    {
#ifdef FEAT_SIGNS
	// If 'signcolumn' is set to 'number' and a sign is present
	// in 'lnum', then display the sign instead of the line
	// number.
	ಯದಿ ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_text != ರಿಕ್ತ)
	    get_sign_display_info(TRUE, wp, wlv);
	ಅನ್ಯಥಾ
#endif
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  ಯದಿ (wlv->row == lnum_row
		    && (wp->w_skipcol == 0 || wlv->row > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      long ಅಂಕೆ;
	      ವರ್ಣ *fmt = "%*ld ";

	      ಯದಿ (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  ಅಂಕೆ = (long)wlv->lnum;
	      ಅನ್ಯಥಾ
	      {
		  // 'relativenumber', don't use negative numbers
		  ಅಂಕೆ = labs((long)get_cursor_rel_lnum(wp, wlv->lnum));
		  ಯದಿ (ಅಂಕೆ == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      ಅಂಕೆ = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((ವರ್ಣ *)wlv->extra, ಪರಿಮಾಣ(wlv->extra), fmt, ಸಂಖ್ಯೆ_width(wp), ಅಂಕೆ);
	      ಯದಿ (wp->w_skipcol > 0 && wlv->startrow == 0)
		  ಪ್ರತಿ (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ifdef FEAT_RIGHTLEFT
	      ಯದಿ (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  ಪೂರ್ಣಾಙ್ಕ	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  ಪ್ರತಿ (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
		  {
		      t = *p1;
		      *p1 = *p2;
		      *p2 = t;
		  }
	      }
#endif
	      wlv->p_extra = wlv->extra;
	      wlv->c_extra = NUL;
	      wlv->c_final = NUL;
	  }
	  ಅನ್ಯಥಾ
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = ಸಂಖ್ಯೆ_width(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ifdef FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  ಯದಿ (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->row == lnum_row
		      || (wlv->row > lnum_row
			 && (wp->w_p_culopt_flags & CULOPT_ಸಾಲು))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#endif
	  ಯದಿ (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  ಯದಿ (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ifdef FEAT_SIGNS
	ಯದಿ (ಅಂಕೆ_attr)
	    wlv->char_attr = ಅಂಕೆ_attr;
#endif
    }
}

#ifdef FEAT_LINEBREAK
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    ಯದಿ (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_ಮೌಲ್ಯ(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    ಅನ್ಯಥಾ ಯದಿ (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    ಯದಿ (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	ಯದಿ (wp->w_p_bri && (wlv->row > wlv->startrow
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
	    ಯದಿ (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ರಿಕ್ತ;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_buffer, wlv->lnum, FALSE));
	    ಯದಿ (wlv->row == wlv->startrow)
	    {
		wlv->n_extra -= win_col_off2(wp);
		ಯದಿ (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    ಯದಿ (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    ಯದಿ (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	ಯದಿ (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#endif

#ಯದಿ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    ಯದಿ (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	ಯದಿ (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	ಅನ್ಯಥಾ
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	ಯದಿ (wp->w_p_rl)
	    wlv->n_extra = wlv->col + 1;
	ಅನ್ಯಥಾ
#  endif
	    wlv->n_extra = wp->w_width - wlv->col;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_ಮೌಲ್ಯ(wp);
    ಯದಿ (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	ಯದಿ (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	ಯದಿ (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	ಯದಿ (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    ಯದಿ (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#endif

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
textprop_size_after_trunc(
	win_T	*wp,
	ಪೂರ್ಣಾಙ್ಕ	flags,	    // TP_FLAG_ALIGN_*
	ಪೂರ್ಣಾಙ್ಕ	added,
	ಪೂರ್ಣಾಙ್ಕ	padding,
	char_u	*text,
	ಪೂರ್ಣಾಙ್ಕ	*n_used_ptr)
{
    ಪೂರ್ಣಾಙ್ಕ	space = (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_width - win_col_off(wp) : added;
    ಪೂರ್ಣಾಙ್ಕ strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    ಯದಿ (space < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_width;
    ಯದಿ (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    ಪ್ರತಿ (p = text; *p != NUL; p += (*mb_ptr2len)(p))
    {
	ಪೂರ್ಣಾಙ್ಕ clen = ptr2cells(p);

	ಯದಿ (strsize + clen > space)
	    ಭಙ್ಗ;
	strsize += clen;
    }
    *n_used_ptr = (ಪೂರ್ಣಾಙ್ಕ)(p - text);

    ಪ್ರತ್ಯಾಗಮ strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    ಪೂರ್ಣಾಙ್ಕ
text_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	ಪೂರ್ಣಾಙ್ಕ	    vcol,	    // current text column
	ಪೂರ್ಣಾಙ್ಕ	    scr_col,	    // current screen column
	ಪೂರ್ಣಾಙ್ಕ	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	ಪೂರ್ಣಾಙ್ಕ	    *n_attr,	    // attribute cells, NULL if not used
	ಪೂರ್ಣಾಙ್ಕ	    *n_attr_skip,   // cells to skip attr, NULL if not used
	ಪೂರ್ಣಾಙ್ಕ	    do_skip)	    // skip_cells is not zero
{
    ಪೂರ್ಣಾಙ್ಕ	    ಬಲ = (tp->tp_flags & TP_FLAG_ALIGN_ಬಲ);
    ಪೂರ್ಣಾಙ್ಕ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ಪೂರ್ಣಾಙ್ಕ	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    ಪೂರ್ಣಾಙ್ಕ	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    ಪೂರ್ಣಾಙ್ಕ	    padding = tp->tp_col == MAXCOL ? tp->tp_padleft : 0;
    ಪೂರ್ಣಾಙ್ಕ	    col_with_padding = scr_col + (below ? 0 : padding);
    ಪೂರ್ಣಾಙ್ಕ	    room = wp->w_width - col_with_padding;
    ಪೂರ್ಣಾಙ್ಕ	    before = room;	// spaces before the text
    ಪೂರ್ಣಾಙ್ಕ	    after = 0;		// spaces after the text
    ಪೂರ್ಣಾಙ್ಕ	    n_used = *n_extra;
    char_u  *l = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ	    strsize = vim_strsize(*p_extra);
    ಪೂರ್ಣಾಙ್ಕ	    cells = wrap ? strsize : textprop_size_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_used);

    ಯದಿ (wrap || ಬಲ || above || below || padding > 0 || n_used < *n_extra)
    {
	ಪೂರ್ಣಾಙ್ಕ	    col_off = win_col_off(wp) - win_col_off2(wp);

	ಯದಿ (above)
	{
	    before = 0;
	    after = wp->w_width - cells - win_col_off(wp) - padding;
	    ಯದಿ (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // Right-align: fill with before
	    ಯದಿ (ಬಲ)
		before -= cells;

	    // Below-align: empty line add one character
	    ಯದಿ (below && vcol == 0 && col_with_padding == col_off
					    && wp->w_width - col_off == before)
		col_with_padding += 1;

	    ಯದಿ (before < 0
		    || !(ಬಲ || below)
		    || (below ? (col_with_padding <= col_off || !wp->w_p_wrap)
			      : (n_used < *n_extra)))
	    {
		ಯದಿ (ಬಲ && (wrap
			      || (room < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_width - col_off - strsize + room;
		    ಯದಿ (before < 0)
			before = 0;
		    ಅನ್ಯಥಾ
			n_used = *n_extra;
		}
		ಅನ್ಯಥಾ ಯದಿ (below && before > vcol && do_skip)
		    before -= vcol;
		ಅನ್ಯಥಾ
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	ಯದಿ (n_used < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_used;

	// add 1 for NUL, 2 for when '…' is used
	ಯದಿ (n_attr != ರಿಕ್ತ)
	    l = alloc(n_used + before + after + (padding > 0 ? padding : 0) + 3);
	ಯದಿ (n_attr == ರಿಕ್ತ || l != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ off = 0;

	    ಯದಿ (n_attr != ರಿಕ್ತ)
	    {
		vim_memset(l, ' ', before);
		off += before;
		ಯದಿ (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_used);
		off += n_used;
	    }
	    ಅನ್ಯಥಾ
	    {
		off = before + after + padding + n_used;
		cells += before + after + padding;
	    }
	    ಯದಿ (n_attr != ರಿಕ್ತ)
	    {
		ಯದಿ (n_used < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    ಯದಿ (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			ಯದಿ (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_setup(&vc, (char_u *)"utf-8", p_enc);
			    ಯದಿ (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ರಿಕ್ತ);
				ಯದಿ (cp == ರಿಕ್ತ)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_setup(&vc, ರಿಕ್ತ, ರಿಕ್ತ);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_head_off)(l, lp);
			STRCPY(lp, cp);
			n_used = lp - l + 3 - before - padding;
			ಯದಿ (cp != buf)
			    vim_free(cp);
		    }
		    ಅನ್ಯಥಾ
			// change last character to '>'
			*lp = '>';
		}
		ಅನ್ಯಥಾ ಯದಿ (after > 0)
		{
		    vim_memset(l + off, ' ', after);
		    l[off + after] = NUL;
		}

		*p_extra = l;
		*n_extra = n_used + before + after + padding;
		*n_attr = mb_charlen(*p_extra);
		// n_attr_skip will not be decremented before draw_state is
		// WL_LINE
		*n_attr_skip = before + (padding > 0 ? padding : 0);
		*n_attr -= *n_attr_skip;
		ಯದಿ (above)
		    *n_attr -= after;
	    }
	}
    }

    ಯದಿ (n_attr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ cells;
    ಪ್ರತ್ಯಾಗಮ (below && col_with_padding > win_col_off(wp) && !wp->w_p_wrap);
}

# ಯದಿ defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    ಪೂರ್ಣಾಙ್ಕ
text_prop_no_showbreak(textprop_T *tp)
{
    ಪೂರ್ಣಾಙ್ಕ	    ಬಲ = (tp->tp_flags & TP_FLAG_ALIGN_ಬಲ);
    ಪೂರ್ಣಾಙ್ಕ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ಪೂರ್ಣಾಙ್ಕ	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    ಪೂರ್ಣಾಙ್ಕ	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    ಪ್ರತ್ಯಾಗಮ (ಬಲ || above || below || !wrap);
}
# endif
#endif

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wlv_screen_ಸಾಲು(win_T *wp, winlinevars_T *wlv, ಪೂರ್ಣಾಙ್ಕ clear_end)
{
    ಯದಿ (wlv->row == 0 && wp->w_skipcol > 0
#ಯದಿ defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_ಮೌಲ್ಯ(wp) == NUL
#endif
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	ಪೂರ್ಣಾಙ್ಕ off = (ಪೂರ್ಣಾಙ್ಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
	ಪೂರ್ಣಾಙ್ಕ max_off = off + screen_Columns;
	ಪೂರ್ಣಾಙ್ಕ skip = 0;

	ಯದಿ (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    ಯಾವತ್ (skip < wp->w_width && VIM_ISDIGIT(Screen_ಸಾಲುಗಳು[off]))
	    {
		++off;
		++skip;
	    }

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3 && i + skip < wp->w_width; ++i)
	{
	    ಯದಿ ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_ಸಾಲುಗಳು[off + 1] = ' ';
	    Screen_ಸಾಲುಗಳು[off] = '<';
	    ಯದಿ (enc_utf8)
		Screen_ಸಾಲುಗಳು_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_ಸಾಲು(wp, wlv->screen_row, wp->w_wincol, wlv->col,
		    clear_end ? wp->w_width : -wp->w_width,
		    wlv->vcol - 1, wlv->screen_ಸಾಲು_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_screen_ಸಾಲು(win_T *wp, winlinevars_T *wlv)
{
#ifdef FEAT_SYN_HL
    long	v;
    ಪೂರ್ಣಾಙ್ಕ		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    ಯದಿ (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    ಅನ್ಯಥಾ
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_width - wlv->col - 1 :
# endif
	wlv->col;
    // check if line ends before left margin
    ಯದಿ (wlv->vcol < v + wcol - win_col_off(wp))
	wlv->vcol = v + wcol - win_col_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->col -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# ಅನ್ಯಥಾ
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    ಯದಿ (wlv->draw_color_col)
	wlv->draw_color_col = advance_color_col(VCOL_HLC, &wlv->color_cols);

    ಯದಿ (((wp->w_p_cuc
		    && (ಪೂರ್ಣಾಙ್ಕ)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (ಪೂರ್ಣಾಙ್ಕ)wp->w_virtcol <
			 (long)wp->w_width * (wlv->row - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_col
# ifdef ಸಾಲು_ATTR
		|| wlv->ಸಾಲು_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	ಪೂರ್ಣಾಙ್ಕ	rightmost_vcol = 0;
	ಪೂರ್ಣಾಙ್ಕ	i;

	ಯದಿ (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	ಯದಿ (wlv->draw_color_col)
	    // determine rightmost colorcolumn to possibly draw
	    ಪ್ರತಿ (i = 0; wlv->color_cols[i] >= 0; ++i)
		ಯದಿ (rightmost_vcol < wlv->color_cols[i])
		    rightmost_vcol = wlv->color_cols[i];

	ಯಾವತ್ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->col >= 0) :
# endif
		(wlv->col < wp->w_width))
	{
	    Screen_ಸಾಲುಗಳು[wlv->off] = ' ';
	    ಯದಿ (enc_utf8)
		Screen_ಸಾಲುಗಳು_UC[wlv->off] = 0;

	    ಯದಿ (wlv->draw_color_col)
		wlv->draw_color_col = advance_color_col(
						   VCOL_HLC, &wlv->color_cols);

	    ಪೂರ್ಣಾಙ್ಕ attr = wlv->win_attr;
# ifdef ಸಾಲು_ATTR
	    ಯದಿ (wlv->ಸಾಲು_attr != 0)
		attr = hl_combine_attr(attr, wlv->ಸಾಲು_attr);
# endif
	    ಯದಿ (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    ಅನ್ಯಥಾ ಯದಿ (wlv->draw_color_col && VCOL_HLC == *wlv->color_cols)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    ScreenCols[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->col;
	    }
	    ಅನ್ಯಥಾ
# endif
	    {
		++wlv->off;
		++wlv->col;
	    }
	    ++wlv->vcol;

	    ಯದಿ (VCOL_HLC > rightmost_vcol
# ifdef ಸಾಲು_ATTR
		    && wlv->ಸಾಲು_attr == 0
# endif
		    && wlv->win_attr == 0)
		ಭಙ್ಗ;
	}
    }
#endif

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_ಸಾಲು_flags |= SLF_INC_VCOL;
    wlv_screen_ಸಾಲು(wp, wlv, TRUE);
    wlv->screen_ಸಾಲು_flags &= ~SLF_INC_VCOL;
    ++wlv->row;
    ++wlv->screen_row;
}
#undef VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_ಸಾಲು_start(win_T *wp UNUSED, winlinevars_T *wlv, ಪೂರ್ಣಾಙ್ಕ save_extra)
{
    wlv->col = 0;
    wlv->off = (unsigned)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
#ifdef FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#endif

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->col = wp->w_width - 1;
	wlv->off += wlv->col;
	wlv->screen_ಸಾಲು_flags |= SLF_RIGHTLEFT;
    }
#endif
    ಯದಿ (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_START;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ರಿಕ್ತ;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ifdef FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#endif
#ifdef FEAT_SYN_HL
	ಯದಿ (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	ಅನ್ಯಥಾ
#endif
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
    ಯದಿ (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ರಿಕ್ತ;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    ಅನ್ಯಥಾ
	wlv->char_attr = wlv->win_attr;
}

#ifdef FEAT_SYN_HL
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_cursorline_highlight(
	winlinevars_T *wlv,
	ಪೂರ್ಣಾಙ್ಕ sign_present UNUSED)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    ಯದಿ (sign_present && wlv->sattr.sat_linehl > 0)
    {
	ಯದಿ (wlv->sattr.sat_priority >= 100)
	    wlv->ಸಾಲು_attr = hl_combine_attr(wlv->cul_attr, wlv->ಸಾಲು_attr);
	ಅನ್ಯಥಾ
	    wlv->ಸಾಲು_attr = hl_combine_attr(wlv->ಸಾಲು_attr, wlv->cul_attr);
    }
    ಅನ್ಯಥಾ
# endif
# ಯದಿ defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->ಸಾಲು_attr = hl_combine_attr(wlv->cul_attr, wlv->ಸಾಲು_attr);
# ಅನ್ಯಥಾ
	wlv->ಸಾಲು_attr = wlv->cul_attr;
# endif
}
#endif

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
    ಪೂರ್ಣಾಙ್ಕ
win_ಸಾಲು(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		startrow,
    ಪೂರ್ಣಾಙ್ಕ		endrow,
    ಪೂರ್ಣಾಙ್ಕ		ಸಂಖ್ಯೆ_only,
    spellvars_T	*spv UNUSED)
{
    winlinevars_T	wlv;		// variables passed between functions

    ಪೂರ್ಣಾಙ್ಕ		c = 0;			// init for GCC
    char_u	*ಸಾಲು;			// current line
    char_u	*ptr;			// current position in "line"
    ಪೂರ್ಣಾಙ್ಕ		in_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ifdef FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ರಿಕ್ತ;   // another p_extra to be freed
#endif
#ಯದಿ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    ಪೂರ್ಣಾಙ್ಕ		in_linebreak = FALSE;	// n_extra set for showing linebreak
#endif
    ಪೂರ್ಣಾಙ್ಕ		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    ಪೂರ್ಣಾಙ್ಕ		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    ಪೂರ್ಣಾಙ್ಕ		n_attr = 0;	    // chars with special attr
    ಪೂರ್ಣಾಙ್ಕ		saved_attr2 = 0;    // char_attr saved for n_attr
    ಪೂರ್ಣಾಙ್ಕ		n_attr3 = 0;	    // chars with overruling special attr
    ಪೂರ್ಣಾಙ್ಕ		saved_attr3 = 0;    // char_attr saved for n_attr3

    ಪೂರ್ಣಾಙ್ಕ		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    ಪೂರ್ಣಾಙ್ಕ		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    ಪೂರ್ಣಾಙ್ಕ		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    ಪೂರ್ಣಾಙ್ಕ		attr_pri = FALSE;	// char_attr has priority
    ಪೂರ್ಣಾಙ್ಕ		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    ಪೂರ್ಣಾಙ್ಕ		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    ಪೂರ್ಣಾಙ್ಕ		area_attr = 0;		// attributes desired by highlighting
    ಪೂರ್ಣಾಙ್ಕ		ಹುಡುಕು_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ifdef FEAT_SYN_HL
    ಪೂರ್ಣಾಙ್ಕ		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    ಪೂರ್ಣಾಙ್ಕ		syntax_attr = 0;	// attributes desired by syntax
    ಪೂರ್ಣಾಙ್ಕ		prev_syntax_col = -1;	// column of prev_syntax_attr
    ಪೂರ್ಣಾಙ್ಕ		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    ಪೂರ್ಣಾಙ್ಕ		has_syntax = FALSE;	// this buffer has syntax highl.
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg;
#endif
#ifdef FEAT_PROP_POPUP
# define WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN 32
    ಪೂರ್ಣಾಙ್ಕ		did_ಸಾಲು = FALSE;	// set to TRUE when line text done
    ಪೂರ್ಣಾಙ್ಕ		text_prop_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		last_textprop_text_idx = -1;
    ಪೂರ್ಣಾಙ್ಕ		text_prop_next = 0;	// next text property to use
    textprop_T	text_props_buf[WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN];
    ಪೂರ್ಣಾಙ್ಕ		text_prop_idxs_buf[WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN];
    char_u	text_prop_suffix_flags_buf[WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN + 1];
    textprop_T	*text_props = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		*text_prop_idxs = ರಿಕ್ತ;
    char_u	*text_prop_suffix_flags = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		text_props_active = 0;
    ಪೂರ್ಣಾಙ್ಕ		text_props_need_sort = FALSE;
    proptype_T  *text_prop_type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		text_prop_attr = 0;
    ಪೂರ್ಣಾಙ್ಕ		text_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    ಪೂರ್ಣಾಙ್ಕ		text_prop_id = 0;	// active property ID
    ಪೂರ್ಣಾಙ್ಕ		text_prop_flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		text_prop_above = FALSE;  // first doing virtual text above
    ಪೂರ್ಣಾಙ್ಕ		text_prop_follows = FALSE;  // another text prop to display
    ಪೂರ್ಣಾಙ್ಕ		saved_ಹುಡುಕು_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    ಪೂರ್ಣಾಙ್ಕ		saved_area_attr = 0;	// idem for area_attr
    ಪೂರ್ಣಾಙ್ಕ		reset_extra_attr = FALSE;
#endif
#ifdef FEAT_SPELL
    ಪೂರ್ಣಾಙ್ಕ		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    ಪೂರ್ಣಾಙ್ಕ		nextlinecol = 0;	// column where nextline[] starts
    ಪೂರ್ಣಾಙ್ಕ		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    ಪೂರ್ಣಾಙ್ಕ		spell_attr = 0;		// attributes desired by spelling
    ಪೂರ್ಣಾಙ್ಕ		word_end = 0;		// last byte with same spell_attr
    ಪೂರ್ಣಾಙ್ಕ		cur_checked_col = 0;	// checked column for current line
#endif
    ಪೂರ್ಣಾಙ್ಕ		extra_check = FALSE;	// has extra highlighting
    ಪೂರ್ಣಾಙ್ಕ		multi_attr = 0;		// attributes desired by multibyte
    ಪೂರ್ಣಾಙ್ಕ		mb_l = 1;		// multi-byte byte length
    ಪೂರ್ಣಾಙ್ಕ		mb_c = 0;		// decoded multi-byte character
    ಪೂರ್ಣಾಙ್ಕ		mb_utf8 = FALSE;	// screen char is UTF-8 char
    ಪೂರ್ಣಾಙ್ಕ		u8cc[MAX_MCO];		// composing UTF-8 chars
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		change_start = MAXCOL;	// first col of changed area
    ಪೂರ್ಣಾಙ್ಕ		change_end = -1;	// last col of changed area
    diffline_T	ಸಾಲು_changes;
    ಪೂರ್ಣಾಙ್ಕ		change_index = -1;
#endif
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    ಪೂರ್ಣಾಙ್ಕ		in_multispace = FALSE;	// in multiple consecutive spaces
    ಪೂರ್ಣಾಙ್ಕ		multispace_pos = 0;	// position in lcs-multispace string
#ifdef ಸಾಲು_ATTR
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_attr_save = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		sign_present = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_attr = 0;		// attribute for the number column
#ifdef FEAT_ARABIC
    ಪೂರ್ಣಾಙ್ಕ		prev_c = 0;		// previous Arabic character
    ಪೂರ್ಣಾಙ್ಕ		prev_c1 = 0;		// first composing char for prev_c
#endif
#ಯದಿ defined(ಸಾಲು_ATTR)
    ಪೂರ್ಣಾಙ್ಕ		did_ಸಾಲು_attr = 0;
#endif
#ifdef FEAT_TERMINAL
    ಪೂರ್ಣಾಙ್ಕ		get_term_attr = FALSE;
#endif

#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    ಪೂರ್ಣಾಙ್ಕ		ಎಡ_curline_col = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಬಲ_curline_col = 0;
#endif

#ಯದಿ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ಪೂರ್ಣಾಙ್ಕ		feedback_col = 0;
    ಪೂರ್ಣಾಙ್ಕ		feedback_old_attr = -1;
#endif

#ಯದಿ defined(FEAT_CONCEAL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    ಪೂರ್ಣಾಙ್ಕ		match_conc	= 0;	// cchar for match functions
#endif
#ಯದಿ defined(FEAT_CONCEAL) || defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_LINEBREAK)
    ಪೂರ್ಣಾಙ್ಕ		on_last_col     = FALSE;
#endif
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		syntax_flags	= 0;
    ಪೂರ್ಣಾಙ್ಕ		syntax_seqnr	= 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_syntax_id	= 0;
    ಪೂರ್ಣಾಙ್ಕ		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    ಪೂರ್ಣಾಙ್ಕ		is_concealing	= FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_wcol	= FALSE;
    ಪೂರ್ಣಾಙ್ಕ		old_boguscols   = 0;
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_co - wlv.vcol_off_tp)
# define FIX_FOR_BOGUSCOLS \
    { \
	wlv.n_extra += wlv.vcol_off_co; \
	wlv.vcol -= wlv.vcol_off_co; \
	wlv.vcol_off_co = 0; \
	wlv.col -= wlv.boguscols; \
	old_boguscols = wlv.boguscols; \
	wlv.boguscols = 0; \
    }
#ಅನ್ಯಥಾ
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#endif

    ಯದಿ (startrow > endrow)		// past the end already!
	ಪ್ರತ್ಯಾಗಮ startrow;

    CLEAR_FIELD(wlv);

    wlv.lnum = lnum;
    wlv.startrow = startrow;
    wlv.row = startrow;
    wlv.screen_row = wlv.row + W_WINROW(wp);
    wlv.fromcol = -10;
    wlv.tocol = MAXCOL;
#ifdef FEAT_LINEBREAK
    wlv.vcol_sbr = -1;
#endif

    ಯದಿ (ಸಂಖ್ಯೆ_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ifdef FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#endif
#ifdef FEAT_SYN_HL
	ಯದಿ (syntax_present(wp) && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
		&& !wp->w_s->b_syn_slow
# endif
	   )
	{
	    // Prepare for syntax highlighting in this line.  When there is an
	    // error, stop syntax highlighting.
	    save_did_emsg = did_emsg;
	    did_emsg = FALSE;
	    syntax_start(wp, lnum);
	    ಯದಿ (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    ಅನ್ಯಥಾ
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		ಯದಿ (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_cols = wp->w_p_cc_cols;
	ಯದಿ (wlv.color_cols != ರಿಕ್ತ)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);
#endif

#ifdef FEAT_TERMINAL
	ಯದಿ (term_show_buffer(wp->w_buffer))
	{
	    extra_check = TRUE;
	    get_term_attr = TRUE;
	    wlv.win_attr = term_get_attr(wp, lnum, -1);
	}
#endif

	// handle Visual active in this window
	ಯದಿ (VIsual_active && wp->w_buffer == curwin->w_buffer)
	{
	    pos_T	*ತುದಿ, *bot;

	    ಯದಿ (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		ತುದಿ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Visual is before curwin->w_cursor
		ತುದಿ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_in_visual_area = (lnum >= ತುದಿ->lnum && lnum <= bot->lnum);
	    ಯದಿ (VIsual_mode == Ctrl_V)
	    {
		// block mode
		ಯದಿ (lnum_in_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// non-block mode
		ಯದಿ (lnum > ತುದಿ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		ಅನ್ಯಥಾ ಯದಿ (lnum == ತುದಿ->lnum)
		{
		    ಯದಿ (VIsual_mode == 'V')	// linewise
			wlv.fromcol = 0;
		    ಅನ್ಯಥಾ
		    {
			getvvcol(wp, ತುದಿ, &wlv.fromcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
			ಯದಿ (gchar_pos(ತುದಿ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		ಯದಿ (VIsual_mode != 'V' && lnum == bot->lnum)
		{
		    ಯದಿ (*p_sel == 'e' && bot->col == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (bot->col == MAXCOL)
			wlv.tocol = MAXCOL;
		    ಅನ್ಯಥಾ
		    {
			pos = *bot;
			ಯದಿ (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
			ಅನ್ಯಥಾ
			{
			    getvvcol(wp, &pos, ರಿಕ್ತ, ರಿಕ್ತ, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    ಯದಿ (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#ಯದಿ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		ಯದಿ ((clip_star.available && !clip_star.owned
						    && clip_isautosel_star())
			|| (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#endif
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	ಅನ್ಯಥಾ ಯದಿ (highlight_match
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + ಹುಡುಕು_match_ಸಾಲುಗಳು)
	{
	    ಯದಿ (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	    ಅನ್ಯಥಾ
		wlv.fromcol = 0;
	    ಯದಿ (lnum == curwin->w_cursor.lnum + ಹುಡುಕು_match_ಸಾಲುಗಳು)
	    {
		pos.lnum = lnum;
		pos.col = ಹುಡುಕು_match_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	    }
	    ಅನ್ಯಥಾ
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    ಯದಿ (wlv.fromcol == wlv.tocol && ಹುಡುಕು_match_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ifdef FEAT_DIFF

    ಪೂರ್ಣಾಙ್ಕ linestatus = 0;
    wlv.filler_ಸಾಲುಗಳು = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_FIELD(ಸಾಲು_changes);

    ಯದಿ (linestatus < 0)
    {
	ಯದಿ (linestatus == -1)
	{
	    ಯದಿ (diff_find_change(wp, lnum, &ಸಾಲು_changes))
	    {
		wlv.diff_hlf = HLF_ADD;	// added line
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು_changes.ಅಂಕೆ_changes > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ added = diff_change_parse(
			&ಸಾಲು_changes, &ಸಾಲು_changes.changes[0],
			&change_start, &change_end);
		ಯದಿ (change_start == 0)
		{
		    ಯದಿ (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    ಅನ್ಯಥಾ
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		ಅನ್ಯಥಾ
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_index = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_index = 0;
	    }
	}
	ಅನ್ಯಥಾ
	    wlv.diff_hlf = HLF_ADD;

	area_highlighting = TRUE;
    }

    ಯದಿ (lnum == wp->w_topline)
	wlv.filler_ಸಾಲುಗಳು = wp->w_topfill;

    wlv.filler_todo = wlv.filler_ಸಾಲುಗಳು;
#endif

#ifdef FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    ಯದಿ (sign_present)
	ಅಂಕೆ_attr = wlv.sattr.sat_numhl;
#endif

#ifdef ಸಾಲು_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    ಯದಿ (sign_present)
	wlv.ಸಾಲು_attr = wlv.sattr.sat_linehl;
# endif
# ಯದಿ defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    ಯದಿ (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	wlv.ಸಾಲು_attr = HL_ATTR(HLF_QFL);
# endif
    ಯದಿ (wlv.ಸಾಲು_attr != 0)
	area_highlighting = TRUE;
#endif

#ifdef FEAT_SPELL
    ಯದಿ (spv->spv_has_spell && ಸಂಖ್ಯೆ_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	ಯದಿ (lnum == spv->spv_checked_lnum)
	    cur_checked_col = spv->spv_checked_col;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	ಯದಿ (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_col = 0;
	ಅನ್ಯಥಾ ಯದಿ (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_col = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	ಯದಿ (lnum < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_ಸಾಲು(nextline + SPWORDLEN, ಸಾಲು, SPWORDLEN);
	}
	ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(ಸಾಲು);
	ಯದಿ (*ptr == NUL)
	{
	    spv->spv_cap_col = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	ಅನ್ಯಥಾ ಯದಿ (spv->spv_cap_col == 0)
	    spv->spv_cap_col = ptr - ಸಾಲು;

	// Copy the end of the current line into nextline[].
	ಯದಿ (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	ಅನ್ಯಥಾ
	{
	    v = ml_get_buf_len(wp->w_buffer, lnum);
	    ಯದಿ (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, ಸಾಲು, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, ಸಾಲು + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = ಸಾಲು;

    ಯದಿ (wp->w_p_list)
    {
	ಯದಿ (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ರಿಕ್ತ
		|| wp->w_lcs_chars.leadmultispace != ರಿಕ್ತ
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	ಯದಿ (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_buffer, lnum);
	    ಯಾವತ್ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - ಸಾಲು);
	}
	// find end of leading whitespace
	ಯದಿ (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ರಿಕ್ತ ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    ಯಾವತ್ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    ಯದಿ (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    ಅನ್ಯಥಾ
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - ಸಾಲು) + 1;
	}
    }

    ಯದಿ (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    ಪೂರ್ಣಾಙ್ಕ skipcol_in_text_prop_above = 0;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (WIN_IS_POPUP(wp))
	wlv.screen_ಸಾಲು_flags |= SLF_POPUP;

    char_u *prop_start;
    text_prop_ಎಣಿಕೆ = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
    ಯದಿ (text_prop_ಎಣಿಕೆ > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	ಯದಿ (text_prop_ಎಣಿಕೆ <= WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN)
	    text_props = text_props_buf;
	ಅನ್ಯಥಾ
	    text_props = ALLOC_MULT(textprop_T, text_prop_ಎಣಿಕೆ);
	ಯದಿ (text_props != ರಿಕ್ತ)
	{
	    mch_memmove(text_props, prop_start,
				     text_prop_ಎಣಿಕೆ * ಪರಿಮಾಣ(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *ಎಣಿಕೆ_ptr = prop_start - PROP_ಎಣಿಕೆ_SIZE;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < text_prop_ಎಣಿಕೆ; ++i)
	    {
		ಯದಿ (text_props[i].tp_id < 0
				&& text_props[i].u.tp_text_offset > 0)
		{
		    text_props[i].u.tp_text =
			    ಎಣಿಕೆ_ptr + text_props[i].u.tp_text_offset;
		    text_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		ಅನ್ಯಥಾ
		    text_props[i].u.tp_text = ರಿಕ್ತ;
	    }
	}

	// Allocate an array for the indexes.
	ಯದಿ (text_prop_ಎಣಿಕೆ <= WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN)
	    text_prop_idxs = text_prop_idxs_buf;
	ಅನ್ಯಥಾ
	    text_prop_idxs = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, text_prop_ಎಣಿಕೆ);
	ಯದಿ (text_prop_idxs == ರಿಕ್ತ)
	{
	    ಯದಿ (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    ಅನ್ಯಥಾ
		text_props = ರಿಕ್ತ;
	}
	ಯದಿ (text_prop_ಎಣಿಕೆ <= WIN_ಸಾಲು_TEXT_PROP_ರಾಶಿ_LEN)
	    text_prop_suffix_flags = text_prop_suffix_flags_buf;
	ಅನ್ಯಥಾ
	    text_prop_suffix_flags = ALLOC_MULT(char_u, text_prop_ಎಣಿಕೆ + 1);
	ಯದಿ (text_prop_suffix_flags == ರಿಕ್ತ)
	{
	    ಯದಿ (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    text_prop_idxs = ರಿಕ್ತ;
	    ಯದಿ (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    ಅನ್ಯಥಾ
		text_props = ರಿಕ್ತ;
	}

	ಯದಿ (text_props != ರಿಕ್ತ)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < text_prop_ಎಣಿಕೆ; ++i)
		ಯದಿ (text_props[i].tp_id < 0)
		    last_textprop_text_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    ಪೂರ್ಣಾಙ್ಕ text_width = wp->w_width - win_col_off(wp);
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = text_prop_ಎಣಿಕೆ - 1; i >= 0; --i)
		ಯದಿ (text_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    ಯದಿ (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_in_text_prop_above
								 >= text_width)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_in_text_prop_above += text_width;
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = i + 1; j < text_prop_ಎಣಿಕೆ; ++j)
			    text_props[j - 1] = text_props[j];
			++i;
			--text_prop_ಎಣಿಕೆ;
		    }
		    ಅನ್ಯಥಾ
			++wlv.text_prop_above_ಎಣಿಕೆ;
		}

	    text_prop_suffix_flags[text_prop_ಎಣಿಕೆ] = 0;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = text_prop_ಎಣಿಕೆ - 1; i >= 0; --i)
	    {
		ಪೂರ್ಣಾಙ್ಕ flags = text_prop_suffix_flags[i + 1];

		ಯದಿ (text_props[i].tp_col == MAXCOL)
		{
		    flags |= 1;
		    ಯದಿ (text_props[i].tp_flags & TP_FLAG_ALIGN_BELOW)
			flags |= 2;
		}
		text_prop_suffix_flags[i] = flags;
	    }
	}
    }

    ಯದಿ (ಸಂಖ್ಯೆ_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.row += wlv.text_prop_above_ಎಣಿಕೆ;
	ಯದಿ (wlv.row >= endrow)
	{
	    ಯದಿ (text_props != text_props_buf)
		vim_free(text_props);
	    ಯದಿ (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    ಯದಿ (text_prop_suffix_flags != text_prop_suffix_flags_buf)
		vim_free(text_prop_suffix_flags);
	    ಪ್ರತ್ಯಾಗಮ wlv.row;
	}
	wlv.screen_row += wlv.text_prop_above_ಎಣಿಕೆ;
    }
#endif

#ಯದಿ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_first_char = 0;
    ಯದಿ (wp->w_p_lbr && ಸಂಖ್ಯೆ_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
	(ಶೂನ್ಯ)win_lbr_chartabsize(&cts, ರಿಕ್ತ, ರಿಕ್ತ);
	vcol_first_char = cts.cts_first_char;
	clear_chartabsize_arg(&cts);
    }
#endif

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    ಯದಿ (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_in_text_prop_above : 0;
    ಅನ್ಯಥಾ
	v = wp->w_leftcol;
    ಯದಿ (v > 0 && ಸಂಖ್ಯೆ_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	ಪೂರ್ಣಾಙ್ಕ		charsize = 0;
	ಪೂರ್ಣಾಙ್ಕ		head = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, ಸಾಲು, ptr);
	cts.cts_max_head_vcol = v;
	ಯಾವತ್ (cts.cts_vcol < v)
	{
	    head = 0;
	    charsize = win_lbr_chartabsize(&cts, &head, ರಿಕ್ತ);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    ಯದಿ (*prev_ptr == NUL)
		ಭಙ್ಗ;
	    MB_PTR_ADV(cts.cts_ptr);
	    ಯದಿ (wp->w_p_list)
	    {
		in_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > ಸಾಲು && prev_ptr[-1] == ' '));
		ಯದಿ (!in_multispace)
		    multispace_pos = 0;
		ಅನ್ಯಥಾ ಯದಿ (cts.cts_ptr >= ಸಾಲು + leadcol
					 && wp->w_lcs_chars.multispace != ರಿಕ್ತ)
		{
		    ++multispace_pos;
		    ಯದಿ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		ಅನ್ಯಥಾ ಯದಿ (cts.cts_ptr < ಸಾಲು + leadcol
				     && wp->w_lcs_chars.leadmultispace != ರಿಕ್ತ)
		{
		    ++multispace_pos;
		    ಯದಿ (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	ಯದಿ (wlv.vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_col ||
#endif
	     virtual_active() ||
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	ಯದಿ (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	ಯದಿ (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - head;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	ಯದಿ (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	ಅನ್ಯಥಾ ಯದಿ (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ifdef FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	ಯದಿ (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	ಯದಿ (spv->spv_has_spell)
	{
	    ಪೂರ್ಣಾಙ್ಕ		len;
	    colnr_T	linecol = (colnr_T)(ptr - ಸಾಲು);
	    hlf_T	spell_hlf = HLF_ಎಣಿಕೆ;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = ಸಾಲು + linecol;

	    ಯದಿ (len == 0 || (ಪೂರ್ಣಾಙ್ಕ)wp->w_cursor.col > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		// word
		spell_hlf = HLF_ಎಣಿಕೆ;
		word_end = (ಪೂರ್ಣಾಙ್ಕ)(spell_to_word_end(ptr, wp) - ಸಾಲು + 1);
	    }
	    ಅನ್ಯಥಾ
	    {
		// bad word found, use attributes until end of word
		word_end = wp->w_cursor.col + len + 1;

		// Turn index into actual attributes.
		ಯದಿ (spell_hlf != HLF_ಎಣಿಕೆ)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    ಯದಿ (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    ಯದಿ (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಯದಿ (ಸಂಖ್ಯೆ_only == 0)
    {
	v = (long)(ptr - ಸಾಲು);
	area_highlighting |= prepare_ಹುಡುಕು_hl_ಸಾಲು(wp, lnum, (colnr_T)v,
					      &ಸಾಲು, &screen_ಹುಡುಕು_hl,
					      &ಹುಡುಕು_attr);
	ptr = ಸಾಲು + v; // "line" may have been updated
    }
#endif

    ಯದಿ ((State & MODE_ಸೇರಿಸು) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
	area_highlighting = TRUE;

#ifdef FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    ಯದಿ (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	ಯದಿ (!(wp == curwin && VIsual_active)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    ಯದಿ (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    ಅನ್ಯಥಾ
	    {
		ಸಾಲು_attr_save = wlv.ಸಾಲು_attr;
		margin_columns_win(wp, &ಎಡ_curline_col, &ಬಲ_curline_col);
	    }
	    area_highlighting = TRUE;
	}
    }
#endif

    win_ಸಾಲು_start(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    ಪ್ರತಿ (;;)
    {
#ಯದಿ defined(FEAT_CONCEAL) || defined(FEAT_ಹುಡುಕು_EXTRA)
	ಪೂರ್ಣಾಙ್ಕ	has_match_conc = 0;	// match wants to conceal
#endif
#ifdef FEAT_CONCEAL
	ಪೂರ್ಣಾಙ್ಕ	did_decrement_ptr = FALSE;
#endif

	// Skip this quickly when working on the text.
	ಯದಿ (wlv.draw_state != WL_ಸಾಲು)
	{
#ifdef FEAT_SYN_HL
	    ಯದಿ (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.ಸಾಲು_attr = ಸಾಲು_attr_save;
	    }
#endif
	    ಯದಿ (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		ಯದಿ (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    ಯದಿ (wlv.row == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    ಅನ್ಯಥಾ
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ifdef FEAT_FOLDING
	    ಯದಿ (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#endif
#ifdef FEAT_SIGNS
	    ಯದಿ (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		ಯದಿ (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#endif
	    ಯದಿ (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_col(wp, &wlv, sign_present, ಅಂಕೆ_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    ಯದಿ (ಸಂಖ್ಯೆ_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_ಸಾಲು(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		ಯದಿ ((wlv.row + 1 - wlv.startrow < ಸಂಖ್ಯೆ_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ifdef FEAT_DIFF
			|| wlv.filler_todo > 0
#endif
			)
		{
		    ++wlv.row;
		    ++wlv.screen_row;
		    ಯದಿ (wlv.row == endrow)
			ಭಙ್ಗ;
#ifdef FEAT_DIFF
		    --wlv.filler_todo;
		    ಯದಿ (wlv.filler_todo == 0 && wp->w_botfill)
			ಭಙ್ಗ;
#endif
		    win_ಸಾಲು_start(wp, &wlv, TRUE);
		    ಅನುವರ್ತ;
		}
		ಅನ್ಯಥಾ
		    ಭಙ್ಗ;
	    }

#ifdef FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    ಯದಿ (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#endif
#ಯದಿ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    ಯದಿ (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#endif
	    ಯದಿ (wlv.draw_state == WL_ಸಾಲು - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_ಸಾಲು;
		win_ಸಾಲು_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ifdef FEAT_SYN_HL
	ಯದಿ (wlv.cul_screenline && wlv.draw_state == WL_ಸಾಲು
		&& wlv.vcol >= ಎಡ_curline_col
		&& wlv.vcol < ಬಲ_curline_col)
	    apply_cursorline_highlight(&wlv, sign_present);
#endif

	// When still displaying '$' of change command, stop at cursor.
	ಯದಿ (dollar_vcol >= 0 && in_curline && wlv.vcol >= (long)wp->w_virtcol)
	{
	    wlv_screen_ಸಾಲು(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ifdef FEAT_SYN_HL
	    ಯದಿ (wp->w_p_cuc)
		wlv.row = wp->w_cline_row + wp->w_cline_height;
	    ಅನ್ಯಥಾ
#endif
		wlv.row = wp->w_height;
	    ಭಙ್ಗ;
	}

	ಯದಿ (wlv.draw_state == WL_ಸಾಲು && (area_highlighting || extra_check))
	{
#ifdef FEAT_PROP_POPUP
	    ಯದಿ (text_props != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ pi;
		ಪೂರ್ಣಾಙ್ಕ bcol = (ಪೂರ್ಣಾಙ್ಕ)(ptr - ಸಾಲು);

		ಯದಿ (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !in_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		ಪ್ರತಿ (pi = 0; pi < text_props_active; ++pi)
		{
		    ಪೂರ್ಣಾಙ್ಕ		tpi = text_prop_idxs[pi];
		    textprop_T  *tp = &text_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    ಯದಿ ((tp->tp_col != MAXCOL
				       && bcol >= tp->tp_col - 1 + tp->tp_len))
		    {
			ಯದಿ (pi + 1 < text_props_active)
			    mch_memmove(text_prop_idxs + pi,
					text_prop_idxs + pi + 1,
					ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)
					     * (text_props_active - (pi + 1)));
			--text_props_active;
			text_props_need_sort = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			ಯದಿ (in_linebreak && syntax_attr == text_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		ಯದಿ (wlv.n_extra > 0 && in_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		ಯಾವತ್ (text_prop_next < text_prop_ಎಣಿಕೆ)
		{
		    ಪೂರ್ಣಾಙ್ಕ active;
		    textprop_T *tp = &text_props[text_prop_next];
		    ಯದಿ (tp->tp_col == MAXCOL)
		    {
			ಯದಿ (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    active = TRUE;
			ಅನ್ಯಥಾ ಯದಿ (*ptr != NUL)
			    ಭಙ್ಗ;
			ಅನ್ಯಥಾ
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    active = wp->w_p_wrap
				  || wlv.row == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (bcol < tp->tp_col - 1)
			    ಭಙ್ಗ;
			active = bcol <= tp->tp_col - 1 + tp->tp_len;
		    }

		    ಯದಿ (active)
		    {
			text_prop_idxs[text_props_active++] = text_prop_next;
			text_props_need_sort = TRUE;
		    }
		    ++text_prop_next;
		}

		ಯದಿ (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(text_prop_type != ರಿಕ್ತ &&
			     text_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    text_prop_attr = 0;
		    text_prop_attr_comb = 0;
		    text_prop_flags = 0;
		    text_prop_type = ರಿಕ್ತ;
		    text_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		ಯದಿ (text_props_active > 0 && wlv.n_extra == 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ used_tpi = -1;
		    ಪೂರ್ಣಾಙ್ಕ used_attr = 0;
		    ಪೂರ್ಣಾಙ್ಕ other_tpi = -1;

		    text_prop_above = FALSE;
		    text_prop_follows = FALSE;

		    ಯದಿ (text_props_need_sort)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			sort_text_props(wp->w_buffer, text_props,
					text_prop_idxs, text_props_active);
			text_props_need_sort = FALSE;
		    }

		    ಪ್ರತಿ (pi = 0; pi < text_props_active; ++pi)
		    {
			ಪೂರ್ಣಾಙ್ಕ	    tpi = text_prop_idxs[pi];
			textprop_T  *tp = &text_props[tpi];
			proptype_T  *pt = text_prop_type_by_id(
						    wp->w_buffer, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			ಯದಿ (pt != ರಿಕ್ತ
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_ಬಲ
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_BELOW)) == 0
				    && wlv.col >= wp->w_width))
			{
			    ಯದಿ (tp->tp_col == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == ಸಾಲು
						&& !did_ಸಾಲು
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_BELOW))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				text_prop_follows = TRUE;
				ಅನುವರ್ತ;
			    }

			    ಯದಿ (pt->pt_hl_id > 0)
				used_attr = syn_id2attr(pt->pt_hl_id);
			    text_prop_type = pt;
			    text_prop_attr =
				   hl_combine_attr(text_prop_attr, used_attr);
			    ಯದಿ (used_tpi >= 0 && text_props[used_tpi].tp_id < 0)
				other_tpi = used_tpi;
			    text_prop_flags = pt->pt_flags;
			    text_prop_id = tp->tp_id;
			    used_tpi = tpi;
			}
		    }
		    ಯದಿ (text_prop_id < 0 && used_tpi >= 0
				 && text_props[used_tpi].u.tp_text != ರಿಕ್ತ)
		    {
			textprop_T  *tp = &text_props[used_tpi];
			char_u	    *p = tp->u.tp_text;
			ಪೂರ್ಣಾಙ್ಕ	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			ಪೂರ್ಣಾಙ್ಕ	    bail_out = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			ಯದಿ (p != ರಿಕ್ತ)
			{
			    ಪೂರ್ಣಾಙ್ಕ	    ಬಲ = (tp->tp_flags
							& TP_FLAG_ALIGN_ಬಲ);
			    ಪೂರ್ಣಾಙ್ಕ	    below = (tp->tp_flags
							& TP_FLAG_ALIGN_BELOW);
			    ಪೂರ್ಣಾಙ್ಕ	    wrap = tp->tp_col < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    ಪೂರ್ಣಾಙ್ಕ	    padding = tp->tp_col == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.start_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    used_attr);
			    n_attr = mb_charlen(p);
			    text_prop_attr = 0;
			    text_prop_attr_comb = 0;
			    ಯದಿ (*ptr == NUL)
				// don't combine char attr after EOL
				text_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    ಯದಿ (text_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    ಯದಿ ((ಬಲ || above || below || !wrap
					    || padding > 0) && wp->w_width > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				ಪೂರ್ಣಾಙ್ಕ	start_ಸಾಲು;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				start_ಸಾಲು = text_prop_position(wp, tp,
						    wlv.vcol,
# ifdef FEAT_RIGHTLEFT
						    wp->w_p_rl
						    ? wp->w_width - wlv.col - 1
						    :
# endif
						    wlv.col,
						    &wlv.n_extra, &wlv.p_extra,
						    &n_attr, &wlv.n_attr_skip,
						    skip_cells > 0);
				ಯದಿ (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				ಯದಿ (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				ಯದಿ (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.col
					       + wlv.n_extra - 2 > wp->w_width)
				    // don't bail out at end of line
				    text_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				ಯದಿ (start_ಸಾಲು)
				{
				    draw_screen_ಸಾಲು(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    ಯದಿ (wlv.row == endrow)
				    {
					++wlv.row;
					ಭಙ್ಗ;
				    }
				    win_ಸಾಲು_start(wp, &wlv, TRUE);
				    bail_out = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			ಯದಿ (skip_cells > 0)
			{
			    ಯದಿ (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				ಯದಿ (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    ಅನ್ಯಥಾ
			    {
				// the whole text is left of the window, drop
				// it and advance to the next one
				skip_cells -= wlv.n_extra;
				skipped_cells += wlv.n_extra;
				wlv.n_extra = 0;
				wlv.n_attr_skip = 0;
				bail_out = TRUE;
			    }
			}

			// If another text prop follows the condition below at
			// the last window column must know.
			// If this is an "above" text prop and 'nowrap' then we
			// must wrap anyway.
			text_prop_above = above;
			text_prop_follows |= other_tpi != -1
					&& (wp->w_p_wrap
					     || (text_props[other_tpi].tp_flags
			       & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ಬಲ)));

			ಯದಿ (bail_out)
			    // starting a new line for "below"
			    ಅನುವರ್ತ;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (text_prop_next < text_prop_ಎಣಿಕೆ
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.col == wp->w_width - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    ಪೂರ್ಣಾಙ್ಕ only_below_follows = !wp->w_p_wrap
				 && wlv.col + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_width;
		    ಪೂರ್ಣಾಙ್ಕ suffix_flags = text_prop_suffix_flags[text_prop_next];

		    text_prop_follows = (suffix_flags
					& (only_below_follows ? 2 : 1)) != 0;
		}
	    }

	    ಯದಿ (wlv.start_extra_for_textprop)
	    {
		wlv.start_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_ಹುಡುಕು_attr = ಹುಡುಕು_attr;
		saved_area_attr = area_attr;
		ಹುಡುಕು_attr = 0;
		area_attr = 0;
	    }
#endif

	    ಪೂರ್ಣಾಙ್ಕ *area_attr_p =
#ifdef FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#endif
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    ಯದಿ (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ರಿಕ್ತ
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    ಅನ್ಯಥಾ ಯದಿ (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    ಯದಿ (wlv.n_extra == 0)
	    {
#ifdef FEAT_ಹುಡುಕು_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (long)(ptr - ಸಾಲು);
		ಹುಡುಕು_attr = update_ಹುಡುಕು_hl(wp, lnum, (colnr_T)v, &ಸಾಲು,
				      &screen_ಹುಡುಕು_hl, &has_match_conc,
				      &match_conc, did_ಸಾಲು_attr, lcs_eol_one,
				      &on_last_col);
		ptr = ಸಾಲು + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		ಯದಿ (*ptr == NUL)
		    has_match_conc = 0;
#ಅನ್ಯಥಾ
		ಹುಡುಕು_attr = 0;
#endif

		// Check if ComplMatchIns highlight is needed.
		ಯದಿ ((State & MODE_ಸೇರಿಸು) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
		{
		    ಪೂರ್ಣಾಙ್ಕ ins_match_attr =
			ins_compl_col_range_attr(lnum, (ಪೂರ್ಣಾಙ್ಕ)(ptr - ಸಾಲು));
		    ಯದಿ (ins_match_attr > 0)
			ಹುಡುಕು_attr =
			    hl_combine_attr(ಹುಡುಕು_attr, ins_match_attr);
		}
	    }

#ifdef FEAT_DIFF
	    ಯದಿ (wlv.diff_hlf != (hlf_T)0)
	    {
		ಯದಿ (ಸಾಲು_changes.ಅಂಕೆ_changes > 0
			&& change_index >= 0
			&& change_index < ಸಾಲು_changes.ಅಂಕೆ_changes - 1)
		{
		    ಯದಿ (ptr - ಸಾಲು >= ಸಾಲು_changes.changes[change_index+1].dc_start[ಸಾಲು_changes.bufidx])
			change_index += 1;
		}
		ಪೂರ್ಣಾಙ್ಕ added = FALSE;
		ಯದಿ (ಸಾಲು_changes.ಅಂಕೆ_changes > 0 && change_index >= 0 && change_index < ಸಾಲು_changes.ಅಂಕೆ_changes)
		{
		    added = diff_change_parse(&ಸಾಲು_changes,
			    &ಸಾಲು_changes.changes[change_index],
			    &change_start,
			    &change_end);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		ಯದಿ (wlv.diff_hlf == HLF_CHD && ptr - ಸಾಲು >= change_start
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		ಯದಿ ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - ಸಾಲು >= change_end && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.ಸಾಲು_attr = HL_ATTR(wlv.diff_hlf);
		ಯದಿ (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ಎಡ_curline_col
					    && wlv.vcol <= ಬಲ_curline_col)))
		    wlv.ಸಾಲು_attr = hl_combine_attr(
					  wlv.ಸಾಲು_attr, HL_ATTR(HLF_CUL));
	    }
#endif

#ifdef FEAT_SYN_HL
	    ಯದಿ (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		ಯದಿ (get_term_attr)
		    syntax_attr = term_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		ಯದಿ (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (long)(ptr - ಸಾಲು);
		    ಯದಿ (v == prev_syntax_col)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    ಅನ್ಯಥಾ
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ರಿಕ್ತ, FALSE);
			prev_syntax_col = v;
			prev_syntax_attr = syntax_attr;
		    }

		    ಯದಿ (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    ಅನ್ಯಥಾ
			did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		    ಯದಿ (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = ಸಾಲು + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    ಯದಿ (*ptr == NUL)
			syntax_flags = 0;
		    ಅನ್ಯಥಾ
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    ಯದಿ (text_prop_type != ರಿಕ್ತ)
	    {
		ಯದಿ (text_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, text_prop_attr);
		ಅನ್ಯಥಾ
		    syntax_attr = text_prop_attr;
		text_prop_attr_comb = syntax_attr;
	    }
# endif
#endif

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ifdef ಸಾಲು_ATTR
	    ಯದಿ (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ಸಾಲು_attr, area_attr);
		ಯದಿ (!highlight_match)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(ಹುಡುಕು_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ಹುಡುಕು_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ಸಾಲು_attr, ಹುಡುಕು_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (wlv.ಸಾಲು_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.ಸಾಲು_attr);
# ಅನ್ಯಥಾ
		wlv.char_attr = wlv.ಸಾಲು_attr;
# endif
		attr_pri = FALSE;
	    }
#ಅನ್ಯಥಾ
	    ಯದಿ (area_attr != 0)
		wlv.char_attr = area_attr;
	    ಅನ್ಯಥಾ ಯದಿ (ಹುಡುಕು_attr != 0)
		wlv.char_attr = ಹುಡುಕು_attr;
#endif
	    ಅನ್ಯಥಾ
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#ಅನ್ಯಥಾ
		wlv.char_attr = 0;
#endif
	    }
#ifdef FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    ಯದಿ (text_prop_type != ರಿಕ್ತ && (text_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, text_prop_attr);
#endif
	}

	// combine attribute with HLF_WIN
	ಯದಿ (wlv.win_attr != 0)
	{
	    ಯದಿ (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    ಅನ್ಯಥಾ
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
	ಯದಿ (wlv.n_extra > 0)
	{
	    ಯದಿ (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		ಅನ್ಯಥಾ
		    mb_utf8 = FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		c = *wlv.p_extra;
		ಯದಿ (has_mbyte)
		{
		    mb_c = c;
		    ಯದಿ (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			ಯದಿ (mb_l > wlv.n_extra)
			    mb_l = 1;
			ಅನ್ಯಥಾ ಯದಿ (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			ಯದಿ (mb_l >= wlv.n_extra)
			    mb_l = 1;
			ಅನ್ಯಥಾ ಯದಿ (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    ಯದಿ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    ಯದಿ ((
#ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.col <= 0) :
#endif
				    (wlv.col >= wp->w_width - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = HL_ATTR(HLF_AT);
#ifdef FEAT_SYN_HL
			ಯದಿ (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#endif
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    ಅನ್ಯಥಾ
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#ಯದಿ defined(FEAT_PROP_POPUP)
	    ಯದಿ (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		ಯದಿ (wlv.saved_n_extra <= 0)
		{
		    ಯದಿ (ಹುಡುಕು_attr == 0)
			ಹುಡುಕು_attr = saved_ಹುಡುಕು_attr;
		    ಯದಿ (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    ಯದಿ (wlv.extra_for_textprop)
			// wlv.extra_attr should be used at this position but
			// not any further.
			reset_extra_attr = TRUE;
		}

		wlv.extra_for_textprop = FALSE;
# ifdef FEAT_LINEBREAK
		in_linebreak = FALSE;
# endif
	    }
#endif
	}
	ಅನ್ಯಥಾ
	{
#ifdef FEAT_LINEBREAK
	    ಪೂರ್ಣಾಙ್ಕ		c0;
#endif
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ifdef FEAT_LINEBREAK
	    c0 = *ptr;
#endif
	    ಯದಿ (c == NUL)
	    {
#ifdef FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_ಸಾಲು = TRUE;
#endif
		// no more cells to skip
		skip_cells = 0;
	    }

	    ಯದಿ (has_mbyte)
	    {
		mb_c = c;
		ಯದಿ (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    ಯದಿ (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			ಯದಿ (mb_c < 0x80)
			{
			    c = mb_c;
#ifdef FEAT_LINEBREAK
			    c0 = mb_c;
#endif
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			ಯದಿ (utf_iscomposing(mb_c))
			{
			    ಪೂರ್ಣಾಙ್ಕ i;

			    ಪ್ರತಿ (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    ಯದಿ ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ifdef FEAT_RIGHTLEFT
			ಯದಿ (wp->w_p_rl)		// reverse
			    rl_mirror(wlv.extra);
#endif
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			ಯದಿ (area_attr == 0 && ಹುಡುಕು_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ifdef FEAT_ARABIC
		    ಅನ್ಯಥಾ ಯದಿ (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			ಪೂರ್ಣಾಙ್ಕ	pc, pc1, nc;
			ಪೂರ್ಣಾಙ್ಕ	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ಯದಿ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			ಅನ್ಯಥಾ
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    ಅನ್ಯಥಾ
			prev_c = mb_c;
#endif
		}
		ಅನ್ಯಥಾ	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    ಯದಿ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    ಅನ್ಯಥಾ ಯದಿ (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			ಯದಿ (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			ಅನ್ಯಥಾ
			{
			    ಯದಿ (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_buffer, wlv.extra, c);
				wlv.n_extra = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(wlv.extra) - 1;
			    }
			    ಅನ್ಯಥಾ
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
			    ಯದಿ (area_attr == 0 && ಹುಡುಕು_attr == 0)
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
		ಯದಿ ((
#ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.col <= 0) :
#endif
				(wlv.col >= wp->w_width - 1))
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
#ifdef FEAT_CONCEAL
		    did_decrement_ptr = TRUE;
#endif
		}
		ಅನ್ಯಥಾ ಯದಿ (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		ಯದಿ (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    ಯದಿ (area_attr == 0 && ಹುಡುಕು_attr == 0)
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

	    ಯದಿ (extra_check)
	    {
#ifdef FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (long)(ptr - ಸಾಲು);
		ಯದಿ (spv->spv_has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    ಯದಿ (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			ಪೂರ್ಣಾಙ್ಕ	len;
			hlf_T	spell_hlf = HLF_ಎಣಿಕೆ;

			ಯದಿ (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			ಯದಿ ((prev_ptr - ಸಾಲು) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - ಸಾಲು) - nextlinecol;
			ಅನ್ಯಥಾ
			    p = prev_ptr;
			spv->spv_cap_col -= (ಪೂರ್ಣಾಙ್ಕ)(prev_ptr - ಸಾಲು);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_col,
							    spv->spv_unchanged);
			word_end = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			ಯದಿ (spell_hlf != HLF_ಎಣಿಕೆ
				&& (State & MODE_ಸೇರಿಸು)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - ಸಾಲು)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_ಎಣಿಕೆ;
			    spell_redraw_lnum = lnum;
			}

			ಯದಿ (spell_hlf == HLF_ಎಣಿಕೆ && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_col = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			ಯದಿ (spell_hlf != HLF_ಎಣಿಕೆ)
			    spell_attr = highlight_attr[spell_hlf];

			ಯದಿ (spv->spv_cap_col > 0)
			{
			    ಯದಿ (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_col
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_col = ((p - nextline)
					    + spv->spv_cap_col - nextline_idx);
			    }
			    ಅನ್ಯಥಾ
				// Compute the actual column.
				spv->spv_cap_col += (prev_ptr - ಸಾಲು);
			}
		    }
		}
		ಯದಿ (spell_attr != 0)
		{
		    ಯದಿ (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    ಅನ್ಯಥಾ
			wlv.char_attr = hl_combine_attr(spell_attr,
								wlv.char_attr);
		}
#endif
#ifdef FEAT_LINEBREAK
		// we don't want linebreak to apply for lines that start with
		// leading spaces, followed by long letters (since it would add
		// a break at the beginning of a line and this might be unexpected)
		//
		// So only allow to linebreak, once we have found chars not in
		// 'breakat' in the line.
		ಯದಿ ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((ಪೂರ್ಣಾಙ್ಕ)*ptr))
		    wlv.need_lbr = TRUE;
#endif
#ifdef FEAT_LINEBREAK
		// Found last space before word: check for line break.
		ಯದಿ (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((ಪೂರ್ಣಾಙ್ಕ)*ptr))
		{
		    ಪೂರ್ಣಾಙ್ಕ	    mb_off = has_mbyte ? (*mb_head_off)(ಸಾಲು, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_first_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, ಸಾಲು, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_text = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ರಿಕ್ತ, ರಿಕ್ತ) - 1;
		    clear_chartabsize_arg(&cts);

		    ಯದಿ (on_last_col && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			ಹುಡುಕು_attr = 0;

		    ಯದಿ (c == TAB && wlv.n_extra + wlv.col > wp->w_width)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
# ಅನ್ಯಥಾ
			wlv.n_extra = (ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts
				    - wlv.vcol % (ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    ಯದಿ (wlv.n_extra > 0 && c != TAB)
			in_linebreak = TRUE;
# endif
		    ಯದಿ (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			ಯದಿ (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			ಯದಿ (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif
		ಯದಿ (wp->w_p_list)
		{
		    in_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > ಸಾಲು && prev_ptr[-1] == ' '));
		    ಯದಿ (!in_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		ಯದಿ (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (in_multispace
					&& wp->w_lcs_chars.multispace != ರಿಕ್ತ))
				&& ptr - ಸಾಲು >= leadcol
				&& ptr - ಸಾಲು <= trailcol)))
		{
		    ಯದಿ (in_multispace && wp->w_lcs_chars.multispace != ರಿಕ್ತ)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			ಯದಿ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    ಅನ್ಯಥಾ
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    ಯದಿ (area_attr == 0 && ಹುಡುಕು_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಅನ್ಯಥಾ
			mb_utf8 = FALSE;
		}

		ಯದಿ (c == ' ' && ((trailcol != MAXCOL && ptr > ಸಾಲು + trailcol)
				    || (leadcol != 0 && ptr < ಸಾಲು + leadcol)))
		{
		    ಯದಿ (leadcol != 0 && in_multispace && ptr < ಸಾಲು + leadcol
			    && wp->w_lcs_chars.leadmultispace != ರಿಕ್ತ)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			ಯದಿ (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    ಅನ್ಯಥಾ ಯದಿ (ptr > ಸಾಲು + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    ಅನ್ಯಥಾ ಯದಿ (ptr < ಸಾಲು + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    ಅನ್ಯಥಾ ಯದಿ (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    ಯದಿ (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಅನ್ಯಥಾ
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    ಯದಿ (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		ಯದಿ (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    ಪೂರ್ಣಾಙ್ಕ	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    long    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    ಪೂರ್ಣಾಙ್ಕ	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    ಪೂರ್ಣಾಙ್ಕ	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    ಪೂರ್ಣಾಙ್ಕ	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ಯದಿ (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < ಸಾಲು + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ifdef FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_ಮೌಲ್ಯ(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    ಯದಿ (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#endif
		    // tab amount depends on current column
#ifdef FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
#ಅನ್ಯಥಾ
		    tab_len = (ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts
			       - vcol_adjusted % (ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts - 1;
#endif

#ifdef FEAT_LINEBREAK
		    ಯದಿ (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ifdef FEAT_LINEBREAK
		    ಅನ್ಯಥಾ
		    {
			char_u	*p;
			ಪೂರ್ಣಾಙ್ಕ	len;
			ಪೂರ್ಣಾಙ್ಕ	i;
			ಪೂರ್ಣಾಙ್ಕ	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			ಯದಿ (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			ಯದಿ (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			ಯದಿ (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    ಪೂರ್ಣಾಙ್ಕ tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    ಯದಿ (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    ಯದಿ (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    ಯದಿ (p == ರಿಕ್ತ)
				wlv.n_extra = 0;
			    ಅನ್ಯಥಾ
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				ಪ್ರತಿ (i = 0; i < tab_len; i++)
				{
				    ಪೂರ್ಣಾಙ್ಕ lcs = lcs_tab2;

				    ಯದಿ (*p == NUL)
				    {
					tab_len = i;
					ಭಙ್ಗ;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    ಯದಿ (lcs_tab3 && i == tab_len - 1)
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
				ಯದಿ (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			ಪೂರ್ಣಾಙ್ಕ vc_saved = wlv.vcol_off_co;

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
			ಯದಿ (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#endif
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    ಯದಿ (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ifdef FEAT_LINEBREAK
			ಯದಿ (wp->w_p_lbr && wlv.p_extra != ರಿಕ್ತ
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			ಅನ್ಯಥಾ
#endif
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (c == NUL
			&& wlv.n_extra == 0
			&& (wp->w_p_list
			    || (wlv.fromcol >= 0
				&& wlv.tocol > wlv.vcol
				&& VIsual_mode != Ctrl_V
				&& (
#ifdef FEAT_RIGHTLEFT
				    wp->w_p_rl ? (wlv.col >= 0) :
#endif
				    (wlv.col < wp->w_width))))
			&& lcs_eol_one > 0)
		{
		    // Display a '$' after the line or highlight an extra
		    // character if the line break is included.
#ಯದಿ defined(FEAT_DIFF) || defined(ಸಾಲು_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    ಯದಿ (
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
#endif
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			ಯದಿ (!(area_highlighting && virtual_active()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    ಯದಿ (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    ಅನ್ಯಥಾ
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    ಯದಿ (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಅನ್ಯಥಾ
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ಅನ್ಯಥಾ ಯದಿ (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_buffer, c);
		    ಯದಿ (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    ಯದಿ ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#endif
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ifdef FEAT_LINEBREAK
		    ಯದಿ (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			ಯದಿ (p == ರಿಕ್ತ)
			    wlv.n_extra = 0;
			ಅನ್ಯಥಾ
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    ಅನ್ಯಥಾ
#endif
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    ಯದಿ (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ifdef FEAT_PROP_POPUP
			ಯದಿ (text_prop_type != ರಿಕ್ತ &&
				 text_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(text_prop_attr,
							       wlv.extra_attr);
#endif

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ಅನ್ಯಥಾ ಯದಿ (VIsual_active
			 && (VIsual_mode == Ctrl_V
			     || VIsual_mode == 'v')
			 && virtual_active()
			 && wlv.tocol != MAXCOL
			 && wlv.vcol < wlv.tocol
			 && (
#ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.col >= 0) :
#endif
			    (wlv.col < wp->w_width)))
		{
		    c = ' ';
		    --ptr;	    // put it back at the NUL
		}
#ಯದಿ defined(ಸಾಲು_ATTR)
		ಅನ್ಯಥಾ ಯದಿ ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.ಸಾಲು_attr != 0
			) && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.col >= 0) :
# endif
			    (wlv.col
# ifdef FEAT_CONCEAL
				- wlv.boguscols
# endif
					    < wp->w_width)))
		{
		    // Highlight until the right side of the window
		    c = ' ';
		    --ptr;	    // put it back at the NUL

		    // Remember we do the char for line highlighting.
		    ++did_ಸಾಲು_attr;

		    // don't do search HL for the rest of the line
		    ಯದಿ (wlv.ಸಾಲು_attr != 0 && wlv.char_attr == ಹುಡುಕು_attr
					&& (did_ಸಾಲು_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.ಸಾಲು_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    ಯದಿ (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_ಸಾಲು
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    ಯದಿ (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			ಯದಿ (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    ಯದಿ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ಎಡ_curline_col
					    && wlv.vcol <= ಬಲ_curline_col)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    ಯದಿ (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			ಯದಿ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    ಯದಿ (!wlv.cul_screenline
				    || (wlv.vcol >= ಎಡ_curline_col
					     && wlv.vcol <= ಬಲ_curline_col))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			ಅನ್ಯಥಾ ಯದಿ (wlv.ಸಾಲು_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.ಸಾಲು_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			ಯದಿ (wlv.vcol == 0)
			{
			    ಯದಿ (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    ಅನ್ಯಥಾ ಯದಿ (ಹುಡುಕು_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, ಹುಡುಕು_attr);
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    ಯದಿ (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_ಸಾಲು(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_match_conc > 0)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ರಿಕ್ತ))
	    {
		ಪೂರ್ಣಾಙ್ಕ syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		ಯದಿ (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_match_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_match_conc && match_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    ಯದಿ (has_match_conc && match_conc)
			c = match_conc;
		    ಅನ್ಯಥಾ ಯದಿ (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    ಅನ್ಯಥಾ ಯದಿ (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    ಅನ್ಯಥಾ
			c = ' ';

		    ಯದಿ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ಅನ್ಯಥಾ
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    ಯದಿ (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    ಯದಿ (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			ಯದಿ (wp->w_p_rl)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			ಅನ್ಯಥಾ
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.col += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		ಅನ್ಯಥಾ ಯದಿ (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    ಯದಿ (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#endif // FEAT_CONCEAL
	}

#ifdef FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	ಯದಿ (!did_wcol && wlv.draw_state == WL_ಸಾಲು
		&& in_curline && conceal_cursor_ಸಾಲು(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
		wp->w_wcol = wp->w_width - wlv.col + wlv.boguscols - 1;
	    ಅನ್ಯಥಾ
# endif
		wp->w_wcol = wlv.col - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    ಯದಿ (wlv.vcol + skip_cells < wp->w_virtcol)
		// Cursor beyond end of the line with 'virtualedit'.
		wp->w_wcol += wp->w_virtcol - wlv.vcol - skip_cells;
	    wp->w_wrow = wlv.row;
	    did_wcol = TRUE;
	    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
# ifdef FEAT_PROP_POPUP
	    curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
# endif
	}
#endif

	// Use "wlv.extra_attr", but don't override visual selection
	// highlighting, unless text property overrides.
	// Don't use "wlv.extra_attr" until wlv.n_attr_skip is zero.
	ಯದಿ (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_ಸಾಲು
		&& (!attr_pri
#ifdef FEAT_PROP_POPUP
		    || (text_prop_flags & PT_FLAG_OVERRIDE)
#endif
		   ))
	{
#ifdef ಸಾಲು_ATTR
	    ಯದಿ (wlv.ಸಾಲು_attr)
		wlv.char_attr = hl_combine_attr(wlv.ಸಾಲು_attr, wlv.extra_attr);
	    ಅನ್ಯಥಾ
#endif
		wlv.char_attr = wlv.extra_attr;
#ifdef FEAT_PROP_POPUP
	    ಯದಿ (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#endif
	}

#ಯದಿ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	ಯದಿ (p_imst == IM_ON_THE_SPOT
		&& xic != ರಿಕ್ತ
		&& lnum == wp->w_cursor.lnum
		&& (State & MODE_ಸೇರಿಸು)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_ಸಾಲು)
	{
	    colnr_T tcol;

	    ಯದಿ (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	    ಅನ್ಯಥಾ
		tcol = preedit_end_col;
	    ಯದಿ ((long)preedit_start_col <= wlv.vcol && wlv.vcol < (long)tcol)
	    {
		ಯದಿ (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_col);
		ಯದಿ (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (feedback_old_attr >= 0)
	    {
		wlv.char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_col = 0;
	    }
	}
#endif
	// Handle the case where we are in column 0 but not on the first
	// character of the line and the user wants us to show us a
	// special character (via 'listchars' option "precedes:<char>").
	ಯದಿ (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? (wp->w_skipcol > 0 && wlv.row == 0)
				 : wp->w_leftcol > 0)
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		&& wlv.draw_state > WL_NR
		&& skip_cells <= 0
		&& c != NUL)
	{
	    c = wp->w_lcs_chars.prec;
	    lcs_prec_todo = NUL;
	    ಯದಿ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ಅನ್ಯಥಾ
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    ಯದಿ (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	ಯದಿ ((c == NUL
#ಯದಿ defined(ಸಾಲು_ATTR)
		|| did_ಸಾಲು_attr == 1
#endif
		) && wlv.eol_hl_off == 0)
	{
#ifdef FEAT_ಹುಡುಕು_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    ಪೂರ್ಣಾಙ್ಕ prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_ಹುಡುಕು_hl,
					      (long)(ptr - ಸಾಲು) - (c == NUL));
#endif
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    ಯದಿ (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_ಹುಡುಕು_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_active))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# ಯದಿ defined(ಸಾಲು_ATTR)
			    && did_ಸಾಲು_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		ಪೂರ್ಣಾಙ್ಕ n = 0;

#ifdef FEAT_RIGHTLEFT
		ಯದಿ (wp->w_p_rl)
		{
		    ಯದಿ (wlv.col < 0)
			n = 1;
		}
		ಅನ್ಯಥಾ
#endif
		{
		    ಯದಿ (wlv.col >= wp->w_width)
			n = -1;
		}
		ಯದಿ (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.col += n;
		}
		ಅನ್ಯಥಾ
		{
		    // Add a blank character to highlight.
		    Screen_ಸಾಲುಗಳು[wlv.off] = ' ';
		    ಯದಿ (enc_utf8)
			Screen_ಸಾಲುಗಳು_UC[wlv.off] = 0;
		}
#ifdef FEAT_ಹುಡುಕು_EXTRA
		ಯದಿ (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_ಹುಡುಕು_match_hl(wp, &screen_ಹುಡುಕು_hl,
					   (long)(ptr - ಸಾಲು), &wlv.char_attr);
		}
#endif
		ScreenAttrs[wlv.off] = wlv.char_attr;
		ScreenCols[wlv.off] = wlv.vcol;
#ifdef FEAT_RIGHTLEFT
		ಯದಿ (wp->w_p_rl)
		{
		    --wlv.col;
		    --wlv.off;
		}
		ಅನ್ಯಥಾ
#endif
		{
		    ++wlv.col;
		    ++wlv.off;
		}
		++wlv.vcol;
		wlv.eol_hl_off = 1;
	    }
	}

	// At end of the text line.
	ಯದಿ (c == NUL)
	{
#ifdef FEAT_PROP_POPUP
	    ಯದಿ (text_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		draw_screen_ಸಾಲು(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		ಯದಿ (in_curline)
		{
		    curwin->w_cline_row = startrow;
		    curwin->w_cline_height = wlv.row - startrow;
#ifdef FEAT_FOLDING
		    curwin->w_cline_folded = false;
#endif
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		ಭಙ್ಗ;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	ಪೂರ್ಣಾಙ್ಕ lcs_ext = get_lcs_ext(wp);
	ಯದಿ (lcs_ext != NUL
		&& wlv.draw_state == WL_ಸಾಲು
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		&& (
#ifdef FEAT_RIGHTLEFT
		    wp->w_p_rl ? wlv.col == 0 :
#endif
		    wlv.col == wp->w_width - 1)
		&& (*ptr != NUL
		    || lcs_eol_one > 0
		    || (wlv.n_extra > 0 && (wlv.c_extra != NUL
						     || *wlv.p_extra != NUL))
#ifdef FEAT_PROP_POPUP
		    || text_prop_next <= last_textprop_text_idx
#endif
		   ))
	{
	    c = lcs_ext;
	    wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    mb_c = c;
	    ಯದಿ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ಅನ್ಯಥಾ
		mb_utf8 = FALSE;
	}

#ifdef FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	ಯದಿ (wlv.draw_color_col)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	ಯದಿ (((wlv.draw_state == WL_ಸಾಲು
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_in_visual_area
		&& ಹುಡುಕು_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    ಯದಿ (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    ಅನ್ಯಥಾ ಯದಿ (wlv.draw_color_col && VCOL_HLC == *wlv.color_cols)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#endif

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	ಯದಿ (wlv.draw_state < WL_ಸಾಲು || skip_cells <= 0)
	{
	    // Store the character.
#ಯದಿ defined(FEAT_RIGHTLEFT)
	    ಯದಿ (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.col;
	    }
#endif
	    Screen_ಸಾಲುಗಳು[wlv.off] = c;
	    ಯದಿ (enc_dbcs == DBCS_JPNU)
	    {
		ಯದಿ ((mb_c & 0xff00) == 0x8e00)
		    Screen_ಸಾಲುಗಳು[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
	    {
		ಯದಿ (mb_utf8)
		{
		    ಪೂರ್ಣಾಙ್ಕ i;

		    Screen_ಸಾಲುಗಳು_UC[wlv.off] = mb_c;
		    ಯದಿ ((c & 0xff) == 0)
			Screen_ಸಾಲುಗಳು[wlv.off] = 0x80;   // avoid storing zero
		    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ಸಾಲುಗಳು_C[i][wlv.off] = u8cc[i];
			ಯದಿ (u8cc[i] == 0)
			    ಭಙ್ಗ;
		    }
		}
		ಅನ್ಯಥಾ
		    Screen_ಸಾಲುಗಳು_UC[wlv.off] = 0;
	    }
	    ಯದಿ (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    ಅನ್ಯಥಾ
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    ಯದಿ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
		ScreenCols[wlv.off] = wlv.vcol;
	    ಅನ್ಯಥಾ
		ScreenCols[wlv.off] = -1;

	    ಯದಿ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.col;
		ಯದಿ (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_ಸಾಲುಗಳು[wlv.off] = 0;
		ಅನ್ಯಥಾ
		    // DBCS: Put second byte in the second screen char.
		    Screen_ಸಾಲುಗಳು[wlv.off] = mb_c & 0xff;

		ಯದಿ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
			)
		    ScreenCols[wlv.off] = ++wlv.vcol;
		ಅನ್ಯಥಾ
		    ScreenCols[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		ಯದಿ (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ifdef FEAT_RIGHTLEFT
		ಯದಿ (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.col;
		}
#endif
	    }
#ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.col;
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		++wlv.off;
		++wlv.col;
	    }
	}
#ifdef FEAT_CONCEAL
	ಅನ್ಯಥಾ ಯದಿ (wp->w_p_cole > 0 && is_concealing)
	{
	    ಪೂರ್ಣಾಙ್ಕ concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    ಯದಿ (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    ಯದಿ (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    ಯದಿ (wp->w_p_wrap)
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
		ಪೂರ್ಣಾಙ್ಕ adjust_col = !WIN_IS_POPUP(wp);
# ಅನ್ಯಥಾ
		ಪೂರ್ಣಾಙ್ಕ adjust_col = TRUE;
# endif
		ಯದಿ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    ಯದಿ (wp->w_p_rl)
		    {
			ಯದಿ (adjust_col)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    ಅನ್ಯಥಾ
# endif
		    {
			ಯದಿ (adjust_col)
			{
			    wlv.col += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		ಯದಿ (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    ಯದಿ (wp->w_p_rl)
		    {
			ಯದಿ (adjust_col)
			{
			    --wlv.boguscols;
			    --wlv.col;
			}
		    }
		    ಅನ್ಯಥಾ
# endif
		    {
			ಯದಿ (adjust_col)
			{
			    ++wlv.boguscols;
			    ++wlv.col;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		ಯದಿ (wp->w_p_rl)
		{
		    ಯದಿ (adjust_col)
		    {
			--wlv.boguscols;
			--wlv.col;
		    }
		}
		ಅನ್ಯಥಾ
# endif
		{
		    ಯದಿ (adjust_col)
		    {
			++wlv.boguscols;
			++wlv.col;
		    }
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif // FEAT_CONCEAL
	ಅನ್ಯಥಾ
	    --skip_cells;

	ಯದಿ (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	ಯದಿ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		)
	    ++wlv.vcol;

#ifdef FEAT_SYN_HL
	ಯದಿ (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#endif

	// restore attributes after "precedes" in 'listchars'
	ಯದಿ (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	ಯದಿ (n_attr > 0 && wlv.draw_state == WL_ಸಾಲು
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	ಯದಿ (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	ಯದಿ ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.col < 0) :
#endif
				    (wlv.col >= wp->w_width))
		&& (wlv.draw_state != WL_ಸಾಲು
		    || *ptr != NUL
#ifdef FEAT_DIFF
		    || wlv.filler_todo > 0
#endif
#ifdef FEAT_PROP_POPUP
		    || text_prop_above || text_prop_follows
		    || text_prop_next <= last_textprop_text_idx
#endif
		    || (wp->w_p_list && wp->w_lcs_chars.eol != NUL
						&& lcs_eol_one != -1)
		    || (wlv.n_extra != 0 && (wlv.c_extra != NUL
						      || *wlv.p_extra != NUL)))
		)
	{
#ifdef FEAT_CONCEAL
	    wlv.col -= wlv.boguscols;
	    // Apply 'cursorline' and HLF_WIN highlight.
	    ಯದಿ (wlv.boguscols != 0 && (
# ifdef ಸಾಲು_ATTR
			wlv.ಸಾಲು_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		ಪೂರ್ಣಾಙ್ಕ attr = wlv.win_attr;
# ifdef ಸಾಲು_ATTR
		ಯದಿ (wlv.ಸಾಲು_attr != 0)
		    attr = hl_combine_attr(attr, wlv.ಸಾಲು_attr);
# endif
		ಯಾವತ್ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.col >= 0 :
# endif
			    wlv.col < wp->w_width))
		{
		    Screen_ಸಾಲುಗಳು[wlv.off] = ' ';
		    ಯದಿ (enc_utf8)
			Screen_ಸಾಲುಗಳು_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    ScreenCols[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    ಯದಿ (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.col--;
			wlv.boguscols++;
		    }
		    ಅನ್ಯಥಾ
# endif
		    {
			wlv.off++;
			wlv.col++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_ಸಾಲು(wp, &wlv, TRUE);
	    wlv.col += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#ಅನ್ಯಥಾ
	    wlv_screen_ಸಾಲು(wp, &wlv, TRUE);
#endif
	    ++wlv.row;
	    ++wlv.screen_row;

	    // When not wrapping and finished diff lines, break here.
	    ಯದಿ (!wp->w_p_wrap
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
			&& !text_prop_above
			&& !text_prop_follows
#endif
		       )
		ಭಙ್ಗ;
#ifdef FEAT_PROP_POPUP
	    ಯದಿ (!wp->w_p_wrap && text_prop_follows && !text_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = ಸಾಲು + (size_t)ml_get_buf_len(wp->w_buffer, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#endif

	    // When the window is too narrow draw all "@" lines.
	    ಯದಿ (wlv.draw_state != WL_ಸಾಲು
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
	    {
		win_draw_end(wp, '@', ' ', TRUE, wlv.row, wp->w_height, HLF_AT);
		draw_vsep_win(wp, wlv.row);
		wlv.row = endrow;
	    }

	    // When line got too long for screen break here.
	    ಯದಿ (wlv.row == endrow)
	    {
		++wlv.row;
		ಭಙ್ಗ;
	    }

	    ಯದಿ (screen_cur_row == wlv.screen_row - 1
#ifdef FEAT_DIFF
		     && wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
		     && !text_prop_above && !text_prop_follows
#endif
		     && wp->w_width == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		ಸಾಲು_Wraps[wlv.screen_row - 1] = TRUE;

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
		ಯದಿ (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
			 && !(has_mbyte
			     && ((*mb_off2cells)(ಸಾಲು_Offset[wlv.screen_row],
				   ಸಾಲು_Offset[wlv.screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     ಸಾಲು_Offset[wlv.screen_row - 1]
							    + (ಪೂರ್ಣಾಙ್ಕ)topframe->fr_width - 2,
				     ಸಾಲು_Offset[wlv.screen_row]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    ಯದಿ (screen_cur_col != wp->w_width)
			screen_char(ಸಾಲು_Offset[wlv.screen_row - 1]
						       + (unsigned)topframe->fr_width - 1,
				       wlv.screen_row - 1, (ಪೂರ್ಣಾಙ್ಕ)(topframe->fr_width - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    ಯದಿ (has_mbyte && MB_BYTE2LEN(Screen_ಸಾಲುಗಳು[ಸಾಲು_Offset[
				     wlv.screen_row - 1] + (topframe->fr_width - 1)]) > 1)
			out_char(' ');
		    ಅನ್ಯಥಾ
			out_char(Screen_ಸಾಲುಗಳು[ಸಾಲು_Offset[wlv.screen_row - 1]
							    + (topframe->fr_width - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[ಸಾಲು_Offset[wlv.screen_row]] = (sattr_T)-1;
		    screen_start();	// don't know where cursor is now
		}
	    }

	    win_ಸಾಲು_start(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ifdef FEAT_LINEBREAK
	    ಯದಿ (!wlv.dont_use_showbreak
# ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
# endif
	       )
		wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_DIFF
	    --wlv.filler_todo;
	    // When the filler lines are actually below the last line of the
	    // file, don't draw the line itself, break here.
	    ಯದಿ (wlv.filler_todo == 0 && wp->w_botfill)
		ಭಙ್ಗ;
#endif
	}

    }	// for every character in the line
#ifdef FEAT_PROP_POPUP
    ಯದಿ (text_props != text_props_buf)
	vim_free(text_props);
    ಯದಿ (text_prop_idxs != text_prop_idxs_buf)
	vim_free(text_prop_idxs);
    ಯದಿ (text_prop_suffix_flags != text_prop_suffix_flags_buf)
	vim_free(text_prop_suffix_flags);
    vim_free(p_extra_free2);
#endif

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    ಪ್ರತ್ಯಾಗಮ wlv.row;
}
