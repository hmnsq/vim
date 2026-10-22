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

#include "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    static puur_naa_nka
get_lcs_ext(win_T *wp)
{
    ᮚᮦᮔ᮪ (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	ᮝᮜᮤ NUL;
    ᮚᮦᮔ᮪ (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	ᮝᮜᮤ '>';
    ᮝᮜᮤ wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ifdef FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    static puur_naa_nka
advance_color_col(puur_naa_nka vcol, puur_naa_nka **color_cols)
{
    ᮞᮨᮓᮨᮜ (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    ᮝᮜᮤ (**color_cols >= 0);
}
#endif

#ifdef FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    static void
margin_columns_win(win_T *wp, puur_naa_nka *left_col, puur_naa_nka *right_col)
{
    // cache previous calculations depending on w_virtcol
    static puur_naa_nka saved_w_virtcol;
    static win_T *prev_wp;
    static puur_naa_nka prev_width1;
    static puur_naa_nka prev_width2;
    static puur_naa_nka prev_left_col;
    static puur_naa_nka prev_right_col;

    puur_naa_nka cur_col_off = win_col_off(wp);
    puur_naa_nka	width1;
    puur_naa_nka	width2;

    width1 = wp->w_width - cur_col_off;
    width2 = width1 + win_col_off2(wp);

    ᮚᮦᮔ᮪ (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*right_col = prev_right_col;
	*left_col = prev_left_col;
	ᮝᮜᮤ;
    }

    *left_col = 0;
    *right_col = width1;

    ᮚᮦᮔ᮪ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*right_col = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    ᮚᮦᮔ᮪ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*left_col = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_left_col = *left_col;
    prev_right_col = *right_col;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define LINE_ATTR
#endif

// structure with variables passed between win_line() and other functions
typedef sa_nracanaa {
    puur_naa_nka		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    puur_naa_nka		startrow;	// first row in the window to be drawn
    puur_naa_nka		row;		// row in the window, excl w_winrow
    puur_naa_nka		screen_row;	// row on the screen, incl w_winrow

    long	vcol;		// virtual column, before wrapping
    puur_naa_nka		col;		// visual column on screen, after wrapping
#ifdef FEAT_CONCEAL
    puur_naa_nka		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    puur_naa_nka		vcol_off_co;	// offset for concealed characters
#endif
    puur_naa_nka		vcol_off_tp;	// offset for virtual text
#ifdef FEAT_SYN_HL
    puur_naa_nka		draw_color_col;	// highlight colorcolumn
    puur_naa_nka		*color_cols;	// pointer to according columns array
#endif
    puur_naa_nka		eol_hl_off;	// 1 if highlighted char after EOL

    unsigned	off;		// offset in ScreenLines/ScreenAttrs

    puur_naa_nka		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ifdef FEAT_SYN_HL
    puur_naa_nka		cul_attr;	// set when 'cursorline' active
#endif
#ifdef LINE_ATTR
    puur_naa_nka		line_attr;	// for the whole line, includes 'cursorline'
#endif

    puur_naa_nka		screen_line_flags;  // flags for screen_line()

    puur_naa_nka		fromcol;	// start of inverting
    puur_naa_nka		tocol;		// end of inverting

#ifdef FEAT_LINEBREAK
    long	vcol_sbr;	    // virtual column after showbreak
    puur_naa_nka		need_showbreak;	    // overlong line, skipping first x chars
    puur_naa_nka		dont_use_showbreak; // do not use 'showbreak'
#endif
#ifdef FEAT_PROP_POPUP
    puur_naa_nka		text_prop_above_count;
#endif

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    puur_naa_nka		cul_screenline;

    puur_naa_nka		char_attr;	// attributes for the next character

    puur_naa_nka		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    puur_naa_nka		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    puur_naa_nka		n_attr_skip;    // chars to skip before using extra_attr
    puur_naa_nka		c_extra;	// extra chars, all the same
    puur_naa_nka		c_final;	// final char, mandatory if set
    puur_naa_nka		extra_for_textprop; // n_extra set for textprop
    puur_naa_nka		start_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    puur_naa_nka		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    puur_naa_nka		saved_extra_attr;
    puur_naa_nka		saved_n_attr_skip;
    puur_naa_nka		saved_extra_for_textprop;
    puur_naa_nka		saved_c_extra;
    puur_naa_nka		saved_c_final;
    puur_naa_nka		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ifdef FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#endif
    puur_naa_nka		filler_lines;	// nr of filler lines to be drawn
    puur_naa_nka		filler_todo;	// nr of filler lines still to do + 1
#ifdef FEAT_SIGNS
    sign_attrs_T sattr;
#endif
#ifdef FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    puur_naa_nka		need_lbr;
#endif
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#define WL_START	0		// nothing done yet, must be zero
#define WL_CMDLINE	(WL_START + 1)	// cmdline window column
#ifdef FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#ᮜᮤᮚ
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#ᮜᮤᮚ
# define WL_SIGN	WL_FOLD		// column for signs
#endif
#define WL_NR		(WL_SIGN + 1)	// line number
#ifdef FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#ᮜᮤᮚ
# define WL_BRI		WL_NR
#endif
#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#ᮜᮤᮚ
# define WL_SBR		WL_BRI
#endif
#define WL_LINE		(WL_SBR + 1)	// text in the line

#ᮚᮦᮔ᮪ defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    static puur_naa_nka
use_cursor_line_highlight(win_T *wp, linenr_T lnum)
{
    ᮝᮜᮤ wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#endif


#ifdef FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    static void
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    puur_naa_nka fdc = compute_foldcolumn(wp, 0);

    ᮚᮦᮔ᮪ (fdc <= 0)
	ᮝᮜᮤ;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    ᮚᮦᮔ᮪ (wlv->p_extra_free == NULL)
	ᮝᮜᮤ;

    wlv->n_extra = (puur_naa_nka)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    ᮚᮦᮔ᮪ (use_cursor_line_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    ᮜᮤᮚ
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
    static void
get_sign_display_info(
	puur_naa_nka		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    puur_naa_nka	text_sign;
# ifdef FEAT_SIGN_ICONS
    puur_naa_nka	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    ᮚᮦᮔ᮪ (nrcol)
	wlv->n_extra = number_width(wp) + 1;
    ᮜᮤᮚ
    {
	ᮚᮦᮔ᮪ (use_cursor_line_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	ᮜᮤᮚ
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    ᮚᮦᮔ᮪ (wlv->row == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_lines && wlv->filler_todo <= 0
# endif
       )
    {
	text_sign = (wlv->sattr.sat_text != NULL) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != NULL) ? wlv->sattr.sat_typenr : 0;
	ᮚᮦᮔ᮪ (gui.in_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    ᮚᮦᮔ᮪ (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((var_na *)wlv->extra, parimaa_na(wlv->extra),
						"%-*c ", number_width(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    ᮜᮤᮚ
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    ᮚᮦᮔ᮪ (netbeans_active() && (buf_signcount(wp->w_buffer, wlv->lnum)
									  > 1))
	    {
		ᮚᮦᮔ᮪ (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((var_na *)wlv->extra, parimaa_na(wlv->extra),
						"%-*c ", number_width(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		ᮜᮤᮚ
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	ᮜᮤᮚ
# endif
	    ᮚᮦᮔ᮪ (text_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_text;
		ᮚᮦᮔ᮪ (wlv->p_extra != NULL)
		{
		    ᮚᮦᮔ᮪ (nrcol)
		    {
			puur_naa_nka width = number_width(wp) - 2;

			vim_memset(wlv->extra, ' ', width);
			wlv->n_extra = width;
			wlv->n_extra += vim_snprintf((var_na *)wlv->extra + width,
				  parimaa_na(wlv->extra) - width, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    ᮜᮤᮚ
			wlv->n_extra = (puur_naa_nka)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		ᮚᮦᮔ᮪ (use_cursor_line_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		ᮜᮤᮚ
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#endif

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    static void
handle_lnum_col(
	win_T		*wp,
	winlinevars_T	*wlv,
	puur_naa_nka		sign_present UNUSED,
	puur_naa_nka		num_attr UNUSED)
{
    puur_naa_nka has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != NULL;
    puur_naa_nka lnum_row = wlv->startrow + wlv->filler_lines
#ifdef FEAT_PROP_POPUP
		      + wlv->text_prop_above_count
#endif
		      ;

    ᮚᮦᮔ᮪ ((wp->w_p_nu || wp->w_p_rnu)
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
	ᮚᮦᮔ᮪ ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_text != NULL)
	    get_sign_display_info(TRUE, wp, wlv);
	ᮜᮤᮚ
#endif
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  ᮚᮦᮔ᮪ (wlv->row == lnum_row
		    && (wp->w_skipcol == 0 || wlv->row > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      long num;
	      var_na *fmt = "%*ld ";

	      ᮚᮦᮔ᮪ (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  num = (long)wlv->lnum;
	      ᮜᮤᮚ
	      {
		  // 'relativenumber', don't use negative numbers
		  num = labs((long)get_cursor_rel_lnum(wp, wlv->lnum));
		  ᮚᮦᮔ᮪ (num == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      num = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((var_na *)wlv->extra, parimaa_na(wlv->extra), fmt, number_width(wp), num);
	      ᮚᮦᮔ᮪ (wp->w_skipcol > 0 && wlv->startrow == 0)
		  for (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ifdef FEAT_RIGHTLEFT
	      ᮚᮦᮔ᮪ (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  puur_naa_nka	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  for (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
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
	  ᮜᮤᮚ
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = number_width(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ifdef FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  ᮚᮦᮔ᮪ (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->row == lnum_row
		      || (wlv->row > lnum_row
			 && (wp->w_p_culopt_flags & CULOPT_LINE))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#endif
	  ᮚᮦᮔ᮪ (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  ᮚᮦᮔ᮪ (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ifdef FEAT_SIGNS
	ᮚᮦᮔ᮪ (num_attr)
	    wlv->char_attr = num_attr;
#endif
    }
}

#ifdef FEAT_LINEBREAK
    static void
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    ᮚᮦᮔ᮪ (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_value(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    ᮚᮦᮔ᮪ (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	ᮚᮦᮔ᮪ (wp->w_p_bri && (wlv->row > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_lines
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    ᮚᮦᮔ᮪ (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = NULL;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_buffer, wlv->lnum, FALSE));
	    ᮚᮦᮔ᮪ (wlv->row == wlv->startrow)
	    {
		wlv->n_extra -= win_col_off2(wp);
		ᮚᮦᮔ᮪ (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    ᮚᮦᮔ᮪ (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    ᮚᮦᮔ᮪ (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	ᮚᮦᮔ᮪ (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    static void
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    ᮚᮦᮔ᮪ (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	ᮚᮦᮔ᮪ (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	ᮜᮤᮚ
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	ᮚᮦᮔ᮪ (wp->w_p_rl)
	    wlv->n_extra = wlv->col + 1;
	ᮜᮤᮚ
#  endif
	    wlv->n_extra = wp->w_width - wlv->col;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_value(wp);
    ᮚᮦᮔ᮪ (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (puur_naa_nka)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	ᮚᮦᮔ᮪ (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	ᮚᮦᮔ᮪ (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	ᮚᮦᮔ᮪ (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    ᮚᮦᮔ᮪ (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    static puur_naa_nka
textprop_size_after_trunc(
	win_T	*wp,
	puur_naa_nka	flags,	    // TP_FLAG_ALIGN_*
	puur_naa_nka	added,
	puur_naa_nka	padding,
	char_u	*text,
	puur_naa_nka	*n_used_ptr)
{
    puur_naa_nka	space = (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_width - win_col_off(wp) : added;
    puur_naa_nka strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    ᮚᮦᮔ᮪ (space < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_width;
    ᮚᮦᮔ᮪ (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    for (p = text; *p != NUL; p += (*mb_ptr2len)(p))
    {
	puur_naa_nka clen = ptr2cells(p);

	ᮚᮦᮔ᮪ (strsize + clen > space)
	    bha_nga;
	strsize += clen;
    }
    *n_used_ptr = (puur_naa_nka)(p - text);

    ᮝᮜᮤ strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    puur_naa_nka
text_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	puur_naa_nka	    vcol,	    // current text column
	puur_naa_nka	    scr_col,	    // current screen column
	puur_naa_nka	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	puur_naa_nka	    *n_attr,	    // attribute cells, NULL if not used
	puur_naa_nka	    *n_attr_skip,   // cells to skip attr, NULL if not used
	puur_naa_nka	    do_skip)	    // skip_cells is not zero
{
    puur_naa_nka	    right = (tp->tp_flags & TP_FLAG_ALIGN_RIGHT);
    puur_naa_nka	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    puur_naa_nka	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    puur_naa_nka	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    puur_naa_nka	    padding = tp->tp_col == MAXCOL ? tp->tp_padleft : 0;
    puur_naa_nka	    col_with_padding = scr_col + (below ? 0 : padding);
    puur_naa_nka	    room = wp->w_width - col_with_padding;
    puur_naa_nka	    before = room;	// spaces before the text
    puur_naa_nka	    after = 0;		// spaces after the text
    puur_naa_nka	    n_used = *n_extra;
    char_u  *l = NULL;
    puur_naa_nka	    strsize = vim_strsize(*p_extra);
    puur_naa_nka	    cells = wrap ? strsize : textprop_size_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_used);

    ᮚᮦᮔ᮪ (wrap || right || above || below || padding > 0 || n_used < *n_extra)
    {
	puur_naa_nka	    col_off = win_col_off(wp) - win_col_off2(wp);

	ᮚᮦᮔ᮪ (above)
	{
	    before = 0;
	    after = wp->w_width - cells - win_col_off(wp) - padding;
	    ᮚᮦᮔ᮪ (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	ᮜᮤᮚ
	{
	    // Right-align: fill with before
	    ᮚᮦᮔ᮪ (right)
		before -= cells;

	    // Below-align: empty line add one character
	    ᮚᮦᮔ᮪ (below && vcol == 0 && col_with_padding == col_off
					    && wp->w_width - col_off == before)
		col_with_padding += 1;

	    ᮚᮦᮔ᮪ (before < 0
		    || !(right || below)
		    || (below ? (col_with_padding <= col_off || !wp->w_p_wrap)
			      : (n_used < *n_extra)))
	    {
		ᮚᮦᮔ᮪ (right && (wrap
			      || (room < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_width - col_off - strsize + room;
		    ᮚᮦᮔ᮪ (before < 0)
			before = 0;
		    ᮜᮤᮚ
			n_used = *n_extra;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (below && before > vcol && do_skip)
		    before -= vcol;
		ᮜᮤᮚ
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	ᮚᮦᮔ᮪ (n_used < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_used;

	// add 1 for NUL, 2 for when '…' is used
	ᮚᮦᮔ᮪ (n_attr != NULL)
	    l = alloc(n_used + before + after + (padding > 0 ? padding : 0) + 3);
	ᮚᮦᮔ᮪ (n_attr == NULL || l != NULL)
	{
	    puur_naa_nka off = 0;

	    ᮚᮦᮔ᮪ (n_attr != NULL)
	    {
		vim_memset(l, ' ', before);
		off += before;
		ᮚᮦᮔ᮪ (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_used);
		off += n_used;
	    }
	    ᮜᮤᮚ
	    {
		off = before + after + padding + n_used;
		cells += before + after + padding;
	    }
	    ᮚᮦᮔ᮪ (n_attr != NULL)
	    {
		ᮚᮦᮔ᮪ (n_used < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    ᮚᮦᮔ᮪ (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			ᮚᮦᮔ᮪ (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_setup(&vc, (char_u *)"utf-8", p_enc);
			    ᮚᮦᮔ᮪ (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, NULL);
				ᮚᮦᮔ᮪ (cp == NULL)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_setup(&vc, NULL, NULL);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_head_off)(l, lp);
			STRCPY(lp, cp);
			n_used = lp - l + 3 - before - padding;
			ᮚᮦᮔ᮪ (cp != buf)
			    vim_free(cp);
		    }
		    ᮜᮤᮚ
			// change last character to '>'
			*lp = '>';
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (after > 0)
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
		ᮚᮦᮔ᮪ (above)
		    *n_attr -= after;
	    }
	}
    }

    ᮚᮦᮔ᮪ (n_attr == NULL)
	ᮝᮜᮤ cells;
    ᮝᮜᮤ (below && col_with_padding > win_col_off(wp) && !wp->w_p_wrap);
}

# ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    puur_naa_nka
text_prop_no_showbreak(textprop_T *tp)
{
    puur_naa_nka	    right = (tp->tp_flags & TP_FLAG_ALIGN_RIGHT);
    puur_naa_nka	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    puur_naa_nka	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    puur_naa_nka	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    ᮝᮜᮤ (right || above || below || !wrap);
}
# endif
#endif

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    static void
wlv_screen_line(win_T *wp, winlinevars_T *wlv, puur_naa_nka clear_end)
{
    ᮚᮦᮔ᮪ (wlv->row == 0 && wp->w_skipcol > 0
#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_value(wp) == NUL
#endif
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	puur_naa_nka off = (puur_naa_nka)(current_ScreenLine - ScreenLines);
	puur_naa_nka max_off = off + screen_Columns;
	puur_naa_nka skip = 0;

	ᮚᮦᮔ᮪ (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    ᮞᮨᮓᮨᮜ (skip < wp->w_width && VIM_ISDIGIT(ScreenLines[off]))
	    {
		++off;
		++skip;
	    }

	for (puur_naa_nka i = 0; i < 3 && i + skip < wp->w_width; ++i)
	{
	    ᮚᮦᮔ᮪ ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		ScreenLines[off + 1] = ' ';
	    ScreenLines[off] = '<';
	    ᮚᮦᮔ᮪ (enc_utf8)
		ScreenLinesUC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_line(wp, wlv->screen_row, wp->w_wincol, wlv->col,
		    clear_end ? wp->w_width : -wp->w_width,
		    wlv->vcol - 1, wlv->screen_line_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    static void
draw_screen_line(win_T *wp, winlinevars_T *wlv)
{
#ifdef FEAT_SYN_HL
    long	v;
    puur_naa_nka		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    ᮚᮦᮔ᮪ (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    ᮜᮤᮚ
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_width - wlv->col - 1 :
# endif
	wlv->col;
    // check if line ends before left margin
    ᮚᮦᮔ᮪ (wlv->vcol < v + wcol - win_col_off(wp))
	wlv->vcol = v + wcol - win_col_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->col -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# ᮜᮤᮚ
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    ᮚᮦᮔ᮪ (wlv->draw_color_col)
	wlv->draw_color_col = advance_color_col(VCOL_HLC, &wlv->color_cols);

    ᮚᮦᮔ᮪ (((wp->w_p_cuc
		    && (puur_naa_nka)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (puur_naa_nka)wp->w_virtcol <
			 (long)wp->w_width * (wlv->row - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_col
# ifdef LINE_ATTR
		|| wlv->line_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	puur_naa_nka	rightmost_vcol = 0;
	puur_naa_nka	i;

	ᮚᮦᮔ᮪ (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	ᮚᮦᮔ᮪ (wlv->draw_color_col)
	    // determine rightmost colorcolumn to possibly draw
	    for (i = 0; wlv->color_cols[i] >= 0; ++i)
		ᮚᮦᮔ᮪ (rightmost_vcol < wlv->color_cols[i])
		    rightmost_vcol = wlv->color_cols[i];

	ᮞᮨᮓᮨᮜ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->col >= 0) :
# endif
		(wlv->col < wp->w_width))
	{
	    ScreenLines[wlv->off] = ' ';
	    ᮚᮦᮔ᮪ (enc_utf8)
		ScreenLinesUC[wlv->off] = 0;

	    ᮚᮦᮔ᮪ (wlv->draw_color_col)
		wlv->draw_color_col = advance_color_col(
						   VCOL_HLC, &wlv->color_cols);

	    puur_naa_nka attr = wlv->win_attr;
# ifdef LINE_ATTR
	    ᮚᮦᮔ᮪ (wlv->line_attr != 0)
		attr = hl_combine_attr(attr, wlv->line_attr);
# endif
	    ᮚᮦᮔ᮪ (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wlv->draw_color_col && VCOL_HLC == *wlv->color_cols)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    ScreenCols[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    ᮚᮦᮔ᮪ (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->col;
	    }
	    ᮜᮤᮚ
# endif
	    {
		++wlv->off;
		++wlv->col;
	    }
	    ++wlv->vcol;

	    ᮚᮦᮔ᮪ (VCOL_HLC > rightmost_vcol
# ifdef LINE_ATTR
		    && wlv->line_attr == 0
# endif
		    && wlv->win_attr == 0)
		bha_nga;
	}
    }
#endif

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_line_flags |= SLF_INC_VCOL;
    wlv_screen_line(wp, wlv, TRUE);
    wlv->screen_line_flags &= ~SLF_INC_VCOL;
    ++wlv->row;
    ++wlv->screen_row;
}
#undef VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    static void
win_line_start(win_T *wp UNUSED, winlinevars_T *wlv, puur_naa_nka save_extra)
{
    wlv->col = 0;
    wlv->off = (unsigned)(current_ScreenLine - ScreenLines);
#ifdef FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#endif

#ifdef FEAT_RIGHTLEFT
    ᮚᮦᮔ᮪ (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->col = wp->w_width - 1;
	wlv->off += wlv->col;
	wlv->screen_line_flags |= SLF_RIGHTLEFT;
    }
#endif
    ᮚᮦᮔ᮪ (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_START;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = NULL;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ifdef FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#endif
#ifdef FEAT_SYN_HL
	ᮚᮦᮔ᮪ (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	ᮜᮤᮚ
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
    static void
win_line_continue(winlinevars_T *wlv)
{
    ᮚᮦᮔ᮪ (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = NULL;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    ᮜᮤᮚ
	wlv->char_attr = wlv->win_attr;
}

#ifdef FEAT_SYN_HL
    static void
apply_cursorline_highlight(
	winlinevars_T *wlv,
	puur_naa_nka sign_present UNUSED)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    ᮚᮦᮔ᮪ (sign_present && wlv->sattr.sat_linehl > 0)
    {
	ᮚᮦᮔ᮪ (wlv->sattr.sat_priority >= 100)
	    wlv->line_attr = hl_combine_attr(wlv->cul_attr, wlv->line_attr);
	ᮜᮤᮚ
	    wlv->line_attr = hl_combine_attr(wlv->line_attr, wlv->cul_attr);
    }
    ᮜᮤᮚ
# endif
# ᮚᮦᮔ᮪ defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->line_attr = hl_combine_attr(wlv->cul_attr, wlv->line_attr);
# ᮜᮤᮚ
	wlv->line_attr = wlv->cul_attr;
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
    puur_naa_nka
win_line(
    win_T	*wp,
    linenr_T	lnum,
    puur_naa_nka		startrow,
    puur_naa_nka		endrow,
    puur_naa_nka		number_only,
    spellvars_T	*spv UNUSED)
{
    winlinevars_T	wlv;		// variables passed between functions

    puur_naa_nka		c = 0;			// init for GCC
    char_u	*line;			// current line
    char_u	*ptr;			// current position in "line"
    puur_naa_nka		in_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ifdef FEAT_PROP_POPUP
    char_u	*p_extra_free2 = NULL;   // another p_extra to be freed
#endif
#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    puur_naa_nka		in_linebreak = FALSE;	// n_extra set for showing linebreak
#endif
    puur_naa_nka		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    puur_naa_nka		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    puur_naa_nka		n_attr = 0;	    // chars with special attr
    puur_naa_nka		saved_attr2 = 0;    // char_attr saved for n_attr
    puur_naa_nka		n_attr3 = 0;	    // chars with overruling special attr
    puur_naa_nka		saved_attr3 = 0;    // char_attr saved for n_attr3

    puur_naa_nka		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    puur_naa_nka		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    puur_naa_nka		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    puur_naa_nka		attr_pri = FALSE;	// char_attr has priority
    puur_naa_nka		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    puur_naa_nka		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    puur_naa_nka		area_attr = 0;		// attributes desired by highlighting
    puur_naa_nka		search_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ifdef FEAT_SYN_HL
    puur_naa_nka		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    puur_naa_nka		syntax_attr = 0;	// attributes desired by syntax
    puur_naa_nka		prev_syntax_col = -1;	// column of prev_syntax_attr
    puur_naa_nka		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    puur_naa_nka		has_syntax = FALSE;	// this buffer has syntax highl.
    puur_naa_nka		save_did_emsg;
#endif
#ifdef FEAT_PROP_POPUP
# define WIN_LINE_TEXT_PROP_STACK_LEN 32
    puur_naa_nka		did_line = FALSE;	// set to TRUE when line text done
    puur_naa_nka		text_prop_count;
    puur_naa_nka		last_textprop_text_idx = -1;
    puur_naa_nka		text_prop_next = 0;	// next text property to use
    textprop_T	text_props_buf[WIN_LINE_TEXT_PROP_STACK_LEN];
    puur_naa_nka		text_prop_idxs_buf[WIN_LINE_TEXT_PROP_STACK_LEN];
    char_u	text_prop_suffix_flags_buf[WIN_LINE_TEXT_PROP_STACK_LEN + 1];
    textprop_T	*text_props = NULL;
    puur_naa_nka		*text_prop_idxs = NULL;
    char_u	*text_prop_suffix_flags = NULL;
    puur_naa_nka		text_props_active = 0;
    puur_naa_nka		text_props_need_sort = FALSE;
    proptype_T  *text_prop_type = NULL;
    puur_naa_nka		text_prop_attr = 0;
    puur_naa_nka		text_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    puur_naa_nka		text_prop_id = 0;	// active property ID
    puur_naa_nka		text_prop_flags = 0;
    puur_naa_nka		text_prop_above = FALSE;  // first doing virtual text above
    puur_naa_nka		text_prop_follows = FALSE;  // another text prop to display
    puur_naa_nka		saved_search_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    puur_naa_nka		saved_area_attr = 0;	// idem for area_attr
    puur_naa_nka		reset_extra_attr = FALSE;
#endif
#ifdef FEAT_SPELL
    puur_naa_nka		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    puur_naa_nka		nextlinecol = 0;	// column where nextline[] starts
    puur_naa_nka		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    puur_naa_nka		spell_attr = 0;		// attributes desired by spelling
    puur_naa_nka		word_end = 0;		// last byte with same spell_attr
    puur_naa_nka		cur_checked_col = 0;	// checked column for current line
#endif
    puur_naa_nka		extra_check = FALSE;	// has extra highlighting
    puur_naa_nka		multi_attr = 0;		// attributes desired by multibyte
    puur_naa_nka		mb_l = 1;		// multi-byte byte length
    puur_naa_nka		mb_c = 0;		// decoded multi-byte character
    puur_naa_nka		mb_utf8 = FALSE;	// screen char is UTF-8 char
    puur_naa_nka		u8cc[MAX_MCO];		// composing UTF-8 chars
#ifdef FEAT_DIFF
    puur_naa_nka		change_start = MAXCOL;	// first col of changed area
    puur_naa_nka		change_end = -1;	// last col of changed area
    diffline_T	line_changes;
    puur_naa_nka		change_index = -1;
#endif
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    puur_naa_nka		in_multispace = FALSE;	// in multiple consecutive spaces
    puur_naa_nka		multispace_pos = 0;	// position in lcs-multispace string
#ifdef LINE_ATTR
    puur_naa_nka		line_attr_save = 0;
#endif
    puur_naa_nka		sign_present = FALSE;
    puur_naa_nka		num_attr = 0;		// attribute for the number column
#ifdef FEAT_ARABIC
    puur_naa_nka		prev_c = 0;		// previous Arabic character
    puur_naa_nka		prev_c1 = 0;		// first composing char for prev_c
#endif
#ᮚᮦᮔ᮪ defined(LINE_ATTR)
    puur_naa_nka		did_line_attr = 0;
#endif
#ifdef FEAT_TERMINAL
    puur_naa_nka		get_term_attr = FALSE;
#endif

#ᮚᮦᮔ᮪ defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    puur_naa_nka		left_curline_col = 0;
    puur_naa_nka		right_curline_col = 0;
#endif

#ᮚᮦᮔ᮪ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    puur_naa_nka		feedback_col = 0;
    puur_naa_nka		feedback_old_attr = -1;
#endif

#ᮚᮦᮔ᮪ defined(FEAT_CONCEAL) || defined(FEAT_SEARCH_EXTRA)
    puur_naa_nka		match_conc	= 0;	// cchar for match functions
#endif
#ᮚᮦᮔ᮪ defined(FEAT_CONCEAL) || defined(FEAT_SEARCH_EXTRA) || defined(FEAT_LINEBREAK)
    puur_naa_nka		on_last_col     = FALSE;
#endif
#ifdef FEAT_CONCEAL
    puur_naa_nka		syntax_flags	= 0;
    puur_naa_nka		syntax_seqnr	= 0;
    puur_naa_nka		prev_syntax_id	= 0;
    puur_naa_nka		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    puur_naa_nka		is_concealing	= FALSE;
    puur_naa_nka		did_wcol	= FALSE;
    puur_naa_nka		old_boguscols   = 0;
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
#ᮜᮤᮚ
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#endif

    ᮚᮦᮔ᮪ (startrow > endrow)		// past the end already!
	ᮝᮜᮤ startrow;

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

    ᮚᮦᮔ᮪ (number_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ifdef FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#endif
#ifdef FEAT_SYN_HL
	ᮚᮦᮔ᮪ (syntax_present(wp) && !wp->w_s->b_syn_error
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
	    ᮚᮦᮔ᮪ (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    ᮜᮤᮚ
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		ᮚᮦᮔ᮪ (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_cols = wp->w_p_cc_cols;
	ᮚᮦᮔ᮪ (wlv.color_cols != NULL)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);
#endif

#ifdef FEAT_TERMINAL
	ᮚᮦᮔ᮪ (term_show_buffer(wp->w_buffer))
	{
	    extra_check = TRUE;
	    get_term_attr = TRUE;
	    wlv.win_attr = term_get_attr(wp, lnum, -1);
	}
#endif

	// handle Visual active in this window
	ᮚᮦᮔ᮪ (VIsual_active && wp->w_buffer == curwin->w_buffer)
	{
	    pos_T	*top, *bot;

	    ᮚᮦᮔ᮪ (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		top = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    ᮜᮤᮚ
	    {
		// Visual is before curwin->w_cursor
		top = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_in_visual_area = (lnum >= top->lnum && lnum <= bot->lnum);
	    ᮚᮦᮔ᮪ (VIsual_mode == Ctrl_V)
	    {
		// block mode
		ᮚᮦᮔ᮪ (lnum_in_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    ᮜᮤᮚ
	    {
		// non-block mode
		ᮚᮦᮔ᮪ (lnum > top->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (lnum == top->lnum)
		{
		    ᮚᮦᮔ᮪ (VIsual_mode == 'V')	// linewise
			wlv.fromcol = 0;
		    ᮜᮤᮚ
		    {
			getvvcol(wp, top, &wlv.fromcol, NULL, NULL, 0);
			ᮚᮦᮔ᮪ (gchar_pos(top) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		ᮚᮦᮔ᮪ (VIsual_mode != 'V' && lnum == bot->lnum)
		{
		    ᮚᮦᮔ᮪ (*p_sel == 'e' && bot->col == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (bot->col == MAXCOL)
			wlv.tocol = MAXCOL;
		    ᮜᮤᮚ
		    {
			pos = *bot;
			ᮚᮦᮔ᮪ (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, NULL, NULL, 0);
			ᮜᮤᮚ
			{
			    getvvcol(wp, &pos, NULL, NULL, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    ᮚᮦᮔ᮪ (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#ᮚᮦᮔ᮪ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		ᮚᮦᮔ᮪ ((clip_star.available && !clip_star.owned
						    && clip_isautosel_star())
			|| (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#endif
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	ᮜᮤᮚ ᮚᮦᮔ᮪ (highlight_match
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + search_match_lines)
	{
	    ᮚᮦᮔ᮪ (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, NULL, NULL, 0);
	    ᮜᮤᮚ
		wlv.fromcol = 0;
	    ᮚᮦᮔ᮪ (lnum == curwin->w_cursor.lnum + search_match_lines)
	    {
		pos.lnum = lnum;
		pos.col = search_match_endcol;
		getvcol(curwin, &pos, &wlv.tocol, NULL, NULL, 0);
	    }
	    ᮜᮤᮚ
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    ᮚᮦᮔ᮪ (wlv.fromcol == wlv.tocol && search_match_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ifdef FEAT_DIFF

    puur_naa_nka linestatus = 0;
    wlv.filler_lines = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_FIELD(line_changes);

    ᮚᮦᮔ᮪ (linestatus < 0)
    {
	ᮚᮦᮔ᮪ (linestatus == -1)
	{
	    ᮚᮦᮔ᮪ (diff_find_change(wp, lnum, &line_changes))
	    {
		wlv.diff_hlf = HLF_ADD;	// added line
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (line_changes.num_changes > 0)
	    {
		puur_naa_nka added = diff_change_parse(
			&line_changes, &line_changes.changes[0],
			&change_start, &change_end);
		ᮚᮦᮔ᮪ (change_start == 0)
		{
		    ᮚᮦᮔ᮪ (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    ᮜᮤᮚ
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		ᮜᮤᮚ
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_index = 0;
	    }
	    ᮜᮤᮚ
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_index = 0;
	    }
	}
	ᮜᮤᮚ
	    wlv.diff_hlf = HLF_ADD;

	area_highlighting = TRUE;
    }

    ᮚᮦᮔ᮪ (lnum == wp->w_topline)
	wlv.filler_lines = wp->w_topfill;

    wlv.filler_todo = wlv.filler_lines;
#endif

#ifdef FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    ᮚᮦᮔ᮪ (sign_present)
	num_attr = wlv.sattr.sat_numhl;
#endif

#ifdef LINE_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    ᮚᮦᮔ᮪ (sign_present)
	wlv.line_attr = wlv.sattr.sat_linehl;
# endif
# ᮚᮦᮔ᮪ defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    ᮚᮦᮔ᮪ (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	wlv.line_attr = HL_ATTR(HLF_QFL);
# endif
    ᮚᮦᮔ᮪ (wlv.line_attr != 0)
	area_highlighting = TRUE;
#endif

#ifdef FEAT_SPELL
    ᮚᮦᮔ᮪ (spv->spv_has_spell && number_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	ᮚᮦᮔ᮪ (lnum == spv->spv_checked_lnum)
	    cur_checked_col = spv->spv_checked_col;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	ᮚᮦᮔ᮪ (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_col = 0;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_col = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	ᮚᮦᮔ᮪ (lnum < wp->w_buffer->b_ml.ml_line_count)
	{
	    line = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_line(nextline + SPWORDLEN, line, SPWORDLEN);
	}
	line = ml_get_buf(wp->w_buffer, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(line);
	ᮚᮦᮔ᮪ (*ptr == NUL)
	{
	    spv->spv_cap_col = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	ᮜᮤᮚ ᮚᮦᮔ᮪ (spv->spv_cap_col == 0)
	    spv->spv_cap_col = ptr - line;

	// Copy the end of the current line into nextline[].
	ᮚᮦᮔ᮪ (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	ᮜᮤᮚ
	{
	    v = ml_get_buf_len(wp->w_buffer, lnum);
	    ᮚᮦᮔ᮪ (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, line, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    ᮜᮤᮚ
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, line + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = line;

    ᮚᮦᮔ᮪ (wp->w_p_list)
    {
	ᮚᮦᮔ᮪ (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != NULL
		|| wp->w_lcs_chars.leadmultispace != NULL
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	ᮚᮦᮔ᮪ (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_buffer, lnum);
	    ᮞᮨᮓᮨᮜ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - line);
	}
	// find end of leading whitespace
	ᮚᮦᮔ᮪ (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != NULL ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    ᮞᮨᮓᮨᮜ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    ᮚᮦᮔ᮪ (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    ᮜᮤᮚ
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - line) + 1;
	}
    }

    ᮚᮦᮔ᮪ (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    puur_naa_nka skipcol_in_text_prop_above = 0;

#ifdef FEAT_PROP_POPUP
    ᮚᮦᮔ᮪ (WIN_IS_POPUP(wp))
	wlv.screen_line_flags |= SLF_POPUP;

    char_u *prop_start;
    text_prop_count = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
    ᮚᮦᮔ᮪ (text_prop_count > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	ᮚᮦᮔ᮪ (text_prop_count <= WIN_LINE_TEXT_PROP_STACK_LEN)
	    text_props = text_props_buf;
	ᮜᮤᮚ
	    text_props = ALLOC_MULT(textprop_T, text_prop_count);
	ᮚᮦᮔ᮪ (text_props != NULL)
	{
	    mch_memmove(text_props, prop_start,
				     text_prop_count * parimaa_na(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *count_ptr = prop_start - PROP_COUNT_SIZE;

	    for (puur_naa_nka i = 0; i < text_prop_count; ++i)
	    {
		ᮚᮦᮔ᮪ (text_props[i].tp_id < 0
				&& text_props[i].u.tp_text_offset > 0)
		{
		    text_props[i].u.tp_text =
			    count_ptr + text_props[i].u.tp_text_offset;
		    text_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		ᮜᮤᮚ
		    text_props[i].u.tp_text = NULL;
	    }
	}

	// Allocate an array for the indexes.
	ᮚᮦᮔ᮪ (text_prop_count <= WIN_LINE_TEXT_PROP_STACK_LEN)
	    text_prop_idxs = text_prop_idxs_buf;
	ᮜᮤᮚ
	    text_prop_idxs = ALLOC_MULT(puur_naa_nka, text_prop_count);
	ᮚᮦᮔ᮪ (text_prop_idxs == NULL)
	{
	    ᮚᮦᮔ᮪ (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    ᮜᮤᮚ
		text_props = NULL;
	}
	ᮚᮦᮔ᮪ (text_prop_count <= WIN_LINE_TEXT_PROP_STACK_LEN)
	    text_prop_suffix_flags = text_prop_suffix_flags_buf;
	ᮜᮤᮚ
	    text_prop_suffix_flags = ALLOC_MULT(char_u, text_prop_count + 1);
	ᮚᮦᮔ᮪ (text_prop_suffix_flags == NULL)
	{
	    ᮚᮦᮔ᮪ (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    text_prop_idxs = NULL;
	    ᮚᮦᮔ᮪ (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    ᮜᮤᮚ
		text_props = NULL;
	}

	ᮚᮦᮔ᮪ (text_props != NULL)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    for (puur_naa_nka i = 0; i < text_prop_count; ++i)
		ᮚᮦᮔ᮪ (text_props[i].tp_id < 0)
		    last_textprop_text_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    puur_naa_nka text_width = wp->w_width - win_col_off(wp);
	    for (puur_naa_nka i = text_prop_count - 1; i >= 0; --i)
		ᮚᮦᮔ᮪ (text_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    ᮚᮦᮔ᮪ (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_in_text_prop_above
								 >= text_width)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_in_text_prop_above += text_width;
			for (puur_naa_nka j = i + 1; j < text_prop_count; ++j)
			    text_props[j - 1] = text_props[j];
			++i;
			--text_prop_count;
		    }
		    ᮜᮤᮚ
			++wlv.text_prop_above_count;
		}

	    text_prop_suffix_flags[text_prop_count] = 0;
	    for (puur_naa_nka i = text_prop_count - 1; i >= 0; --i)
	    {
		puur_naa_nka flags = text_prop_suffix_flags[i + 1];

		ᮚᮦᮔ᮪ (text_props[i].tp_col == MAXCOL)
		{
		    flags |= 1;
		    ᮚᮦᮔ᮪ (text_props[i].tp_flags & TP_FLAG_ALIGN_BELOW)
			flags |= 2;
		}
		text_prop_suffix_flags[i] = flags;
	    }
	}
    }

    ᮚᮦᮔ᮪ (number_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.row += wlv.text_prop_above_count;
	ᮚᮦᮔ᮪ (wlv.row >= endrow)
	{
	    ᮚᮦᮔ᮪ (text_props != text_props_buf)
		vim_free(text_props);
	    ᮚᮦᮔ᮪ (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    ᮚᮦᮔ᮪ (text_prop_suffix_flags != text_prop_suffix_flags_buf)
		vim_free(text_prop_suffix_flags);
	    ᮝᮜᮤ wlv.row;
	}
	wlv.screen_row += wlv.text_prop_above_count;
    }
#endif

#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_first_char = 0;
    ᮚᮦᮔ᮪ (wp->w_p_lbr && number_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
	(void)win_lbr_chartabsize(&cts, NULL, NULL);
	vcol_first_char = cts.cts_first_char;
	clear_chartabsize_arg(&cts);
    }
#endif

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    ᮚᮦᮔ᮪ (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_in_text_prop_above : 0;
    ᮜᮤᮚ
	v = wp->w_leftcol;
    ᮚᮦᮔ᮪ (v > 0 && number_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	puur_naa_nka		charsize = 0;
	puur_naa_nka		head = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, line, ptr);
	cts.cts_max_head_vcol = v;
	ᮞᮨᮓᮨᮜ (cts.cts_vcol < v)
	{
	    head = 0;
	    charsize = win_lbr_chartabsize(&cts, &head, NULL);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    ᮚᮦᮔ᮪ (*prev_ptr == NUL)
		bha_nga;
	    MB_PTR_ADV(cts.cts_ptr);
	    ᮚᮦᮔ᮪ (wp->w_p_list)
	    {
		in_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > line && prev_ptr[-1] == ' '));
		ᮚᮦᮔ᮪ (!in_multispace)
		    multispace_pos = 0;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (cts.cts_ptr >= line + leadcol
					 && wp->w_lcs_chars.multispace != NULL)
		{
		    ++multispace_pos;
		    ᮚᮦᮔ᮪ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (cts.cts_ptr < line + leadcol
				     && wp->w_lcs_chars.leadmultispace != NULL)
		{
		    ++multispace_pos;
		    ᮚᮦᮔ᮪ (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	ᮚᮦᮔ᮪ (wlv.vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_col ||
#endif
	     virtual_active() ||
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	ᮚᮦᮔ᮪ (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	ᮚᮦᮔ᮪ (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - head;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	ᮚᮦᮔ᮪ (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ifdef FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	ᮚᮦᮔ᮪ (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	ᮚᮦᮔ᮪ (spv->spv_has_spell)
	{
	    puur_naa_nka		len;
	    colnr_T	linecol = (colnr_T)(ptr - line);
	    hlf_T	spell_hlf = HLF_COUNT;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = line + linecol;

	    ᮚᮦᮔ᮪ (len == 0 || (puur_naa_nka)wp->w_cursor.col > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		// word
		spell_hlf = HLF_COUNT;
		word_end = (puur_naa_nka)(spell_to_word_end(ptr, wp) - line + 1);
	    }
	    ᮜᮤᮚ
	    {
		// bad word found, use attributes until end of word
		word_end = wp->w_cursor.col + len + 1;

		// Turn index into actual attributes.
		ᮚᮦᮔ᮪ (spell_hlf != HLF_COUNT)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    ᮚᮦᮔ᮪ (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    ᮚᮦᮔ᮪ (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ifdef FEAT_SEARCH_EXTRA
    ᮚᮦᮔ᮪ (number_only == 0)
    {
	v = (long)(ptr - line);
	area_highlighting |= prepare_search_hl_line(wp, lnum, (colnr_T)v,
					      &line, &screen_search_hl,
					      &search_attr);
	ptr = line + v; // "line" may have been updated
    }
#endif

    ᮚᮦᮔ᮪ ((State & MODE_INSERT) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
	area_highlighting = TRUE;

#ifdef FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    ᮚᮦᮔ᮪ (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	ᮚᮦᮔ᮪ (!(wp == curwin && VIsual_active)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    ᮚᮦᮔ᮪ (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    ᮜᮤᮚ
	    {
		line_attr_save = wlv.line_attr;
		margin_columns_win(wp, &left_curline_col, &right_curline_col);
	    }
	    area_highlighting = TRUE;
	}
    }
#endif

    win_line_start(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    for (;;)
    {
#ᮚᮦᮔ᮪ defined(FEAT_CONCEAL) || defined(FEAT_SEARCH_EXTRA)
	puur_naa_nka	has_match_conc = 0;	// match wants to conceal
#endif
#ifdef FEAT_CONCEAL
	puur_naa_nka	did_decrement_ptr = FALSE;
#endif

	// Skip this quickly when working on the text.
	ᮚᮦᮔ᮪ (wlv.draw_state != WL_LINE)
	{
#ifdef FEAT_SYN_HL
	    ᮚᮦᮔ᮪ (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.line_attr = line_attr_save;
	    }
#endif
	    ᮚᮦᮔ᮪ (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		ᮚᮦᮔ᮪ (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    ᮚᮦᮔ᮪ (wlv.row == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    ᮜᮤᮚ
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ifdef FEAT_FOLDING
	    ᮚᮦᮔ᮪ (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#endif
#ifdef FEAT_SIGNS
	    ᮚᮦᮔ᮪ (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		ᮚᮦᮔ᮪ (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#endif
	    ᮚᮦᮔ᮪ (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_col(wp, &wlv, sign_present, num_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    ᮚᮦᮔ᮪ (number_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_line(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		ᮚᮦᮔ᮪ ((wlv.row + 1 - wlv.startrow < number_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ifdef FEAT_DIFF
			|| wlv.filler_todo > 0
#endif
			)
		{
		    ++wlv.row;
		    ++wlv.screen_row;
		    ᮚᮦᮔ᮪ (wlv.row == endrow)
			bha_nga;
#ifdef FEAT_DIFF
		    --wlv.filler_todo;
		    ᮚᮦᮔ᮪ (wlv.filler_todo == 0 && wp->w_botfill)
			bha_nga;
#endif
		    win_line_start(wp, &wlv, TRUE);
		    continue;
		}
		ᮜᮤᮚ
		    bha_nga;
	    }

#ifdef FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    ᮚᮦᮔ᮪ (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#endif
#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    ᮚᮦᮔ᮪ (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#endif
	    ᮚᮦᮔ᮪ (wlv.draw_state == WL_LINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_LINE;
		win_line_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ifdef FEAT_SYN_HL
	ᮚᮦᮔ᮪ (wlv.cul_screenline && wlv.draw_state == WL_LINE
		&& wlv.vcol >= left_curline_col
		&& wlv.vcol < right_curline_col)
	    apply_cursorline_highlight(&wlv, sign_present);
#endif

	// When still displaying '$' of change command, stop at cursor.
	ᮚᮦᮔ᮪ (dollar_vcol >= 0 && in_curline && wlv.vcol >= (long)wp->w_virtcol)
	{
	    wlv_screen_line(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ifdef FEAT_SYN_HL
	    ᮚᮦᮔ᮪ (wp->w_p_cuc)
		wlv.row = wp->w_cline_row + wp->w_cline_height;
	    ᮜᮤᮚ
#endif
		wlv.row = wp->w_height;
	    bha_nga;
	}

	ᮚᮦᮔ᮪ (wlv.draw_state == WL_LINE && (area_highlighting || extra_check))
	{
#ifdef FEAT_PROP_POPUP
	    ᮚᮦᮔ᮪ (text_props != NULL)
	    {
		puur_naa_nka pi;
		puur_naa_nka bcol = (puur_naa_nka)(ptr - line);

		ᮚᮦᮔ᮪ (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !in_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		for (pi = 0; pi < text_props_active; ++pi)
		{
		    puur_naa_nka		tpi = text_prop_idxs[pi];
		    textprop_T  *tp = &text_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    ᮚᮦᮔ᮪ ((tp->tp_col != MAXCOL
				       && bcol >= tp->tp_col - 1 + tp->tp_len))
		    {
			ᮚᮦᮔ᮪ (pi + 1 < text_props_active)
			    mch_memmove(text_prop_idxs + pi,
					text_prop_idxs + pi + 1,
					parimaa_na(puur_naa_nka)
					     * (text_props_active - (pi + 1)));
			--text_props_active;
			text_props_need_sort = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			ᮚᮦᮔ᮪ (in_linebreak && syntax_attr == text_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		ᮚᮦᮔ᮪ (wlv.n_extra > 0 && in_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		ᮞᮨᮓᮨᮜ (text_prop_next < text_prop_count)
		{
		    puur_naa_nka active;
		    textprop_T *tp = &text_props[text_prop_next];
		    ᮚᮦᮔ᮪ (tp->tp_col == MAXCOL)
		    {
			ᮚᮦᮔ᮪ (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    active = TRUE;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (*ptr != NUL)
			    bha_nga;
			ᮜᮤᮚ
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    active = wp->w_p_wrap
				  || wlv.row == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
			}
		    }
		    ᮜᮤᮚ
		    {
			ᮚᮦᮔ᮪ (bcol < tp->tp_col - 1)
			    bha_nga;
			active = bcol <= tp->tp_col - 1 + tp->tp_len;
		    }

		    ᮚᮦᮔ᮪ (active)
		    {
			text_prop_idxs[text_props_active++] = text_prop_next;
			text_props_need_sort = TRUE;
		    }
		    ++text_prop_next;
		}

		ᮚᮦᮔ᮪ (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(text_prop_type != NULL &&
			     text_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    text_prop_attr = 0;
		    text_prop_attr_comb = 0;
		    text_prop_flags = 0;
		    text_prop_type = NULL;
		    text_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		ᮚᮦᮔ᮪ (text_props_active > 0 && wlv.n_extra == 0)
		{
		    puur_naa_nka used_tpi = -1;
		    puur_naa_nka used_attr = 0;
		    puur_naa_nka other_tpi = -1;

		    text_prop_above = FALSE;
		    text_prop_follows = FALSE;

		    ᮚᮦᮔ᮪ (text_props_need_sort)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			sort_text_props(wp->w_buffer, text_props,
					text_prop_idxs, text_props_active);
			text_props_need_sort = FALSE;
		    }

		    for (pi = 0; pi < text_props_active; ++pi)
		    {
			puur_naa_nka	    tpi = text_prop_idxs[pi];
			textprop_T  *tp = &text_props[tpi];
			proptype_T  *pt = text_prop_type_by_id(
						    wp->w_buffer, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			ᮚᮦᮔ᮪ (pt != NULL
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_RIGHT
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_BELOW)) == 0
				    && wlv.col >= wp->w_width))
			{
			    ᮚᮦᮔ᮪ (tp->tp_col == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == line
						&& !did_line
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_BELOW))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				text_prop_follows = TRUE;
				continue;
			    }

			    ᮚᮦᮔ᮪ (pt->pt_hl_id > 0)
				used_attr = syn_id2attr(pt->pt_hl_id);
			    text_prop_type = pt;
			    text_prop_attr =
				   hl_combine_attr(text_prop_attr, used_attr);
			    ᮚᮦᮔ᮪ (used_tpi >= 0 && text_props[used_tpi].tp_id < 0)
				other_tpi = used_tpi;
			    text_prop_flags = pt->pt_flags;
			    text_prop_id = tp->tp_id;
			    used_tpi = tpi;
			}
		    }
		    ᮚᮦᮔ᮪ (text_prop_id < 0 && used_tpi >= 0
				 && text_props[used_tpi].u.tp_text != NULL)
		    {
			textprop_T  *tp = &text_props[used_tpi];
			char_u	    *p = tp->u.tp_text;
			puur_naa_nka	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			puur_naa_nka	    bail_out = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			ᮚᮦᮔ᮪ (p != NULL)
			{
			    puur_naa_nka	    right = (tp->tp_flags
							& TP_FLAG_ALIGN_RIGHT);
			    puur_naa_nka	    below = (tp->tp_flags
							& TP_FLAG_ALIGN_BELOW);
			    puur_naa_nka	    wrap = tp->tp_col < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    puur_naa_nka	    padding = tp->tp_col == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (puur_naa_nka)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.start_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    used_attr);
			    n_attr = mb_charlen(p);
			    text_prop_attr = 0;
			    text_prop_attr_comb = 0;
			    ᮚᮦᮔ᮪ (*ptr == NUL)
				// don't combine char attr after EOL
				text_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    ᮚᮦᮔ᮪ (text_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    ᮚᮦᮔ᮪ ((right || above || below || !wrap
					    || padding > 0) && wp->w_width > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				puur_naa_nka	start_line;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				start_line = text_prop_position(wp, tp,
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
				ᮚᮦᮔ᮪ (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				ᮚᮦᮔ᮪ (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				ᮚᮦᮔ᮪ (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.col
					       + wlv.n_extra - 2 > wp->w_width)
				    // don't bail out at end of line
				    text_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				ᮚᮦᮔ᮪ (start_line)
				{
				    draw_screen_line(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    ᮚᮦᮔ᮪ (wlv.row == endrow)
				    {
					++wlv.row;
					bha_nga;
				    }
				    win_line_start(wp, &wlv, TRUE);
				    bail_out = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			ᮚᮦᮔ᮪ (skip_cells > 0)
			{
			    ᮚᮦᮔ᮪ (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				ᮚᮦᮔ᮪ (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    ᮜᮤᮚ
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
			       & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_RIGHT)));

			ᮚᮦᮔ᮪ (bail_out)
			    // starting a new line for "below"
			    continue;
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (text_prop_next < text_prop_count
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.col == wp->w_width - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    puur_naa_nka only_below_follows = !wp->w_p_wrap
				 && wlv.col + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_width;
		    puur_naa_nka suffix_flags = text_prop_suffix_flags[text_prop_next];

		    text_prop_follows = (suffix_flags
					& (only_below_follows ? 2 : 1)) != 0;
		}
	    }

	    ᮚᮦᮔ᮪ (wlv.start_extra_for_textprop)
	    {
		wlv.start_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_search_attr = search_attr;
		saved_area_attr = area_attr;
		search_attr = 0;
		area_attr = 0;
	    }
#endif

	    puur_naa_nka *area_attr_p =
#ifdef FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#endif
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    ᮚᮦᮔ᮪ (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != NULL
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    ᮚᮦᮔ᮪ (wlv.n_extra == 0)
	    {
#ifdef FEAT_SEARCH_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (long)(ptr - line);
		search_attr = update_search_hl(wp, lnum, (colnr_T)v, &line,
				      &screen_search_hl, &has_match_conc,
				      &match_conc, did_line_attr, lcs_eol_one,
				      &on_last_col);
		ptr = line + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		ᮚᮦᮔ᮪ (*ptr == NUL)
		    has_match_conc = 0;
#ᮜᮤᮚ
		search_attr = 0;
#endif

		// Check if ComplMatchIns highlight is needed.
		ᮚᮦᮔ᮪ ((State & MODE_INSERT) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
		{
		    puur_naa_nka ins_match_attr =
			ins_compl_col_range_attr(lnum, (puur_naa_nka)(ptr - line));
		    ᮚᮦᮔ᮪ (ins_match_attr > 0)
			search_attr =
			    hl_combine_attr(search_attr, ins_match_attr);
		}
	    }

#ifdef FEAT_DIFF
	    ᮚᮦᮔ᮪ (wlv.diff_hlf != (hlf_T)0)
	    {
		ᮚᮦᮔ᮪ (line_changes.num_changes > 0
			&& change_index >= 0
			&& change_index < line_changes.num_changes - 1)
		{
		    ᮚᮦᮔ᮪ (ptr - line >= line_changes.changes[change_index+1].dc_start[line_changes.bufidx])
			change_index += 1;
		}
		puur_naa_nka added = FALSE;
		ᮚᮦᮔ᮪ (line_changes.num_changes > 0 && change_index >= 0 && change_index < line_changes.num_changes)
		{
		    added = diff_change_parse(&line_changes,
			    &line_changes.changes[change_index],
			    &change_start,
			    &change_end);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		ᮚᮦᮔ᮪ (wlv.diff_hlf == HLF_CHD && ptr - line >= change_start
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		ᮚᮦᮔ᮪ ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - line >= change_end && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.line_attr = HL_ATTR(wlv.diff_hlf);
		ᮚᮦᮔ᮪ (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= left_curline_col
					    && wlv.vcol <= right_curline_col)))
		    wlv.line_attr = hl_combine_attr(
					  wlv.line_attr, HL_ATTR(HLF_CUL));
	    }
#endif

#ifdef FEAT_SYN_HL
	    ᮚᮦᮔ᮪ (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		ᮚᮦᮔ᮪ (get_term_attr)
		    syntax_attr = term_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		ᮚᮦᮔ᮪ (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (long)(ptr - line);
		    ᮚᮦᮔ᮪ (v == prev_syntax_col)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    ᮜᮤᮚ
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    NULL, FALSE);
			prev_syntax_col = v;
			prev_syntax_attr = syntax_attr;
		    }

		    ᮚᮦᮔ᮪ (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    ᮜᮤᮚ
			did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		    ᮚᮦᮔ᮪ (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = line + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    ᮚᮦᮔ᮪ (*ptr == NUL)
			syntax_flags = 0;
		    ᮜᮤᮚ
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    ᮚᮦᮔ᮪ (text_prop_type != NULL)
	    {
		ᮚᮦᮔ᮪ (text_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, text_prop_attr);
		ᮜᮤᮚ
		    syntax_attr = text_prop_attr;
		text_prop_attr_comb = syntax_attr;
	    }
# endif
#endif

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ifdef LINE_ATTR
	    ᮚᮦᮔ᮪ (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.line_attr, area_attr);
		ᮚᮦᮔ᮪ (!highlight_match)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(search_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (search_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.line_attr, search_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wlv.line_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.line_attr);
# ᮜᮤᮚ
		wlv.char_attr = wlv.line_attr;
# endif
		attr_pri = FALSE;
	    }
#ᮜᮤᮚ
	    ᮚᮦᮔ᮪ (area_attr != 0)
		wlv.char_attr = area_attr;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (search_attr != 0)
		wlv.char_attr = search_attr;
#endif
	    ᮜᮤᮚ
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#ᮜᮤᮚ
		wlv.char_attr = 0;
#endif
	    }
#ifdef FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    ᮚᮦᮔ᮪ (text_prop_type != NULL && (text_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, text_prop_attr);
#endif
	}

	// combine attribute with HLF_WIN
	ᮚᮦᮔ᮪ (wlv.win_attr != 0)
	{
	    ᮚᮦᮔ᮪ (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    ᮜᮤᮚ
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
	ᮚᮦᮔ᮪ (wlv.n_extra > 0)
	{
	    ᮚᮦᮔ᮪ (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		ᮜᮤᮚ
		    mb_utf8 = FALSE;
	    }
	    ᮜᮤᮚ
	    {
		c = *wlv.p_extra;
		ᮚᮦᮔ᮪ (has_mbyte)
		{
		    mb_c = c;
		    ᮚᮦᮔ᮪ (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			ᮚᮦᮔ᮪ (mb_l > wlv.n_extra)
			    mb_l = 1;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    ᮜᮤᮚ
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			ᮚᮦᮔ᮪ (mb_l >= wlv.n_extra)
			    mb_l = 1;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    ᮚᮦᮔ᮪ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    ᮚᮦᮔ᮪ ((
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
			ᮚᮦᮔ᮪ (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#endif
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    ᮜᮤᮚ
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#ᮚᮦᮔ᮪ defined(FEAT_PROP_POPUP)
	    ᮚᮦᮔ᮪ (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		ᮚᮦᮔ᮪ (wlv.saved_n_extra <= 0)
		{
		    ᮚᮦᮔ᮪ (search_attr == 0)
			search_attr = saved_search_attr;
		    ᮚᮦᮔ᮪ (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    ᮚᮦᮔ᮪ (wlv.extra_for_textprop)
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
	ᮜᮤᮚ
	{
#ifdef FEAT_LINEBREAK
	    puur_naa_nka		c0;
#endif
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ifdef FEAT_LINEBREAK
	    c0 = *ptr;
#endif
	    ᮚᮦᮔ᮪ (c == NUL)
	    {
#ifdef FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_line = TRUE;
#endif
		// no more cells to skip
		skip_cells = 0;
	    }

	    ᮚᮦᮔ᮪ (has_mbyte)
	    {
		mb_c = c;
		ᮚᮦᮔ᮪ (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    ᮚᮦᮔ᮪ (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			ᮚᮦᮔ᮪ (mb_c < 0x80)
			{
			    c = mb_c;
#ifdef FEAT_LINEBREAK
			    c0 = mb_c;
#endif
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			ᮚᮦᮔ᮪ (utf_iscomposing(mb_c))
			{
			    puur_naa_nka i;

			    for (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    ᮚᮦᮔ᮪ ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ifdef FEAT_RIGHTLEFT
			ᮚᮦᮔ᮪ (wp->w_p_rl)		// reverse
			    rl_mirror(wlv.extra);
#endif
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (puur_naa_nka)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			ᮚᮦᮔ᮪ (area_attr == 0 && search_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ifdef FEAT_ARABIC
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			puur_naa_nka	pc, pc1, nc;
			puur_naa_nka	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ᮚᮦᮔ᮪ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			ᮜᮤᮚ
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    ᮜᮤᮚ
			prev_c = mb_c;
#endif
		}
		ᮜᮤᮚ	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    ᮚᮦᮔ᮪ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			ᮚᮦᮔ᮪ (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			ᮜᮤᮚ
			{
			    ᮚᮦᮔ᮪ (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_buffer, wlv.extra, c);
				wlv.n_extra = (puur_naa_nka)STRLEN(wlv.extra) - 1;
			    }
			    ᮜᮤᮚ
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
			    ᮚᮦᮔ᮪ (area_attr == 0 && search_attr == 0)
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
		ᮚᮦᮔ᮪ ((
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
		ᮜᮤᮚ ᮚᮦᮔ᮪ (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		ᮚᮦᮔ᮪ (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    ᮚᮦᮔ᮪ (area_attr == 0 && search_attr == 0)
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

	    ᮚᮦᮔ᮪ (extra_check)
	    {
#ifdef FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (long)(ptr - line);
		ᮚᮦᮔ᮪ (spv->spv_has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    ᮚᮦᮔ᮪ (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			puur_naa_nka	len;
			hlf_T	spell_hlf = HLF_COUNT;

			ᮚᮦᮔ᮪ (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			ᮚᮦᮔ᮪ ((prev_ptr - line) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - line) - nextlinecol;
			ᮜᮤᮚ
			    p = prev_ptr;
			spv->spv_cap_col -= (puur_naa_nka)(prev_ptr - line);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_col,
							    spv->spv_unchanged);
			word_end = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			ᮚᮦᮔ᮪ (spell_hlf != HLF_COUNT
				&& (State & MODE_INSERT)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - line)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_COUNT;
			    spell_redraw_lnum = lnum;
			}

			ᮚᮦᮔ᮪ (spell_hlf == HLF_COUNT && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_col = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			ᮚᮦᮔ᮪ (spell_hlf != HLF_COUNT)
			    spell_attr = highlight_attr[spell_hlf];

			ᮚᮦᮔ᮪ (spv->spv_cap_col > 0)
			{
			    ᮚᮦᮔ᮪ (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_col
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_col = ((p - nextline)
					    + spv->spv_cap_col - nextline_idx);
			    }
			    ᮜᮤᮚ
				// Compute the actual column.
				spv->spv_cap_col += (prev_ptr - line);
			}
		    }
		}
		ᮚᮦᮔ᮪ (spell_attr != 0)
		{
		    ᮚᮦᮔ᮪ (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    ᮜᮤᮚ
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
		ᮚᮦᮔ᮪ ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((puur_naa_nka)*ptr))
		    wlv.need_lbr = TRUE;
#endif
#ifdef FEAT_LINEBREAK
		// Found last space before word: check for line break.
		ᮚᮦᮔ᮪ (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((puur_naa_nka)*ptr))
		{
		    puur_naa_nka	    mb_off = has_mbyte ? (*mb_head_off)(line, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_first_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, line, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_text = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, NULL, NULL) - 1;
		    clear_chartabsize_arg(&cts);

		    ᮚᮦᮔ᮪ (on_last_col && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			search_attr = 0;

		    ᮚᮦᮔ᮪ (c == TAB && wlv.n_extra + wlv.col > wp->w_width)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
# ᮜᮤᮚ
			wlv.n_extra = (puur_naa_nka)wp->w_buffer->b_p_ts
				    - wlv.vcol % (puur_naa_nka)wp->w_buffer->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    ᮚᮦᮔ᮪ (wlv.n_extra > 0 && c != TAB)
			in_linebreak = TRUE;
# endif
		    ᮚᮦᮔ᮪ (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			ᮚᮦᮔ᮪ (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			ᮚᮦᮔ᮪ (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif
		ᮚᮦᮔ᮪ (wp->w_p_list)
		{
		    in_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > line && prev_ptr[-1] == ' '));
		    ᮚᮦᮔ᮪ (!in_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		ᮚᮦᮔ᮪ (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (in_multispace
					&& wp->w_lcs_chars.multispace != NULL))
				&& ptr - line >= leadcol
				&& ptr - line <= trailcol)))
		{
		    ᮚᮦᮔ᮪ (in_multispace && wp->w_lcs_chars.multispace != NULL)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			ᮚᮦᮔ᮪ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    ᮜᮤᮚ
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    ᮚᮦᮔ᮪ (area_attr == 0 && search_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ᮜᮤᮚ
			mb_utf8 = FALSE;
		}

		ᮚᮦᮔ᮪ (c == ' ' && ((trailcol != MAXCOL && ptr > line + trailcol)
				    || (leadcol != 0 && ptr < line + leadcol)))
		{
		    ᮚᮦᮔ᮪ (leadcol != 0 && in_multispace && ptr < line + leadcol
			    && wp->w_lcs_chars.leadmultispace != NULL)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			ᮚᮦᮔ᮪ (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    ᮜᮤᮚ ᮚᮦᮔ᮪ (ptr > line + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    ᮜᮤᮚ ᮚᮦᮔ᮪ (ptr < line + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    ᮜᮤᮚ ᮚᮦᮔ᮪ (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    ᮚᮦᮔ᮪ (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ᮜᮤᮚ
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    ᮚᮦᮔ᮪ (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		ᮚᮦᮔ᮪ (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    puur_naa_nka	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    long    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    puur_naa_nka	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    puur_naa_nka	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    puur_naa_nka	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ᮚᮦᮔ᮪ (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < line + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ifdef FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_value(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    ᮚᮦᮔ᮪ (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#endif
		    // tab amount depends on current column
#ifdef FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
#ᮜᮤᮚ
		    tab_len = (puur_naa_nka)wp->w_buffer->b_p_ts
			       - vcol_adjusted % (puur_naa_nka)wp->w_buffer->b_p_ts - 1;
#endif

#ifdef FEAT_LINEBREAK
		    ᮚᮦᮔ᮪ (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ifdef FEAT_LINEBREAK
		    ᮜᮤᮚ
		    {
			char_u	*p;
			puur_naa_nka	len;
			puur_naa_nka	i;
			puur_naa_nka	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			ᮚᮦᮔ᮪ (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			ᮚᮦᮔ᮪ (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			ᮚᮦᮔ᮪ (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    puur_naa_nka tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    ᮚᮦᮔ᮪ (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    ᮚᮦᮔ᮪ (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    ᮚᮦᮔ᮪ (p == NULL)
				wlv.n_extra = 0;
			    ᮜᮤᮚ
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				for (i = 0; i < tab_len; i++)
				{
				    puur_naa_nka lcs = lcs_tab2;

				    ᮚᮦᮔ᮪ (*p == NUL)
				    {
					tab_len = i;
					bha_nga;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    ᮚᮦᮔ᮪ (lcs_tab3 && i == tab_len - 1)
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
				ᮚᮦᮔ᮪ (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			puur_naa_nka vc_saved = wlv.vcol_off_co;

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
			ᮚᮦᮔ᮪ (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#endif
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    ᮚᮦᮔ᮪ (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ifdef FEAT_LINEBREAK
			ᮚᮦᮔ᮪ (wp->w_p_lbr && wlv.p_extra != NULL
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			ᮜᮤᮚ
#endif
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    ᮜᮤᮚ
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (c == NUL
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
#ᮚᮦᮔ᮪ defined(FEAT_DIFF) || defined(LINE_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    ᮚᮦᮔ᮪ (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef LINE_ATTR
			    &&
#  endif
# endif
# ifdef LINE_ATTR
			    wlv.line_attr == 0
# endif
		       )
#endif
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			ᮚᮦᮔ᮪ (!(area_highlighting && virtual_active()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    ᮚᮦᮔ᮪ (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    ᮜᮤᮚ
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    ᮚᮦᮔ᮪ (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ᮜᮤᮚ
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_buffer, c);
		    ᮚᮦᮔ᮪ (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    ᮚᮦᮔ᮪ ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#endif
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ifdef FEAT_LINEBREAK
		    ᮚᮦᮔ᮪ (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			ᮚᮦᮔ᮪ (p == NULL)
			    wlv.n_extra = 0;
			ᮜᮤᮚ
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    ᮜᮤᮚ
#endif
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    ᮚᮦᮔ᮪ (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ifdef FEAT_PROP_POPUP
			ᮚᮦᮔ᮪ (text_prop_type != NULL &&
				 text_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(text_prop_attr,
							       wlv.extra_attr);
#endif

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (VIsual_active
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
#ᮚᮦᮔ᮪ defined(LINE_ATTR)
		ᮜᮤᮚ ᮚᮦᮔ᮪ ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.line_attr != 0
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
		    ++did_line_attr;

		    // don't do search HL for the rest of the line
		    ᮚᮦᮔ᮪ (wlv.line_attr != 0 && wlv.char_attr == search_attr
					&& (did_line_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.line_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    ᮚᮦᮔ᮪ (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_LINE
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    ᮚᮦᮔ᮪ (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			ᮚᮦᮔ᮪ (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    ᮚᮦᮔ᮪ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= left_curline_col
					    && wlv.vcol <= right_curline_col)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    ᮚᮦᮔ᮪ (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			ᮚᮦᮔ᮪ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    ᮚᮦᮔ᮪ (!wlv.cul_screenline
				    || (wlv.vcol >= left_curline_col
					     && wlv.vcol <= right_curline_col))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			ᮜᮤᮚ ᮚᮦᮔ᮪ (wlv.line_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.line_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			ᮚᮦᮔ᮪ (wlv.vcol == 0)
			{
			    ᮚᮦᮔ᮪ (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    ᮜᮤᮚ ᮚᮦᮔ᮪ (search_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, search_attr);
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    ᮚᮦᮔ᮪ (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_line(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_match_conc > 0)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == NULL))
	    {
		puur_naa_nka syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		ᮚᮦᮔ᮪ (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_match_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_match_conc && match_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    ᮚᮦᮔ᮪ (has_match_conc && match_conc)
			c = match_conc;
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    ᮜᮤᮚ
			c = ' ';

		    ᮚᮦᮔ᮪ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ᮜᮤᮚ
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    ᮚᮦᮔ᮪ (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    ᮚᮦᮔ᮪ (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			ᮚᮦᮔ᮪ (wp->w_p_rl)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			ᮜᮤᮚ
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.col += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    ᮜᮤᮚ
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    ᮚᮦᮔ᮪ (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#endif // FEAT_CONCEAL
	}

#ifdef FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	ᮚᮦᮔ᮪ (!did_wcol && wlv.draw_state == WL_LINE
		&& in_curline && conceal_cursor_line(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    ᮚᮦᮔ᮪ (wp->w_p_rl)
		wp->w_wcol = wp->w_width - wlv.col + wlv.boguscols - 1;
	    ᮜᮤᮚ
# endif
		wp->w_wcol = wlv.col - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    ᮚᮦᮔ᮪ (wlv.vcol + skip_cells < wp->w_virtcol)
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
	ᮚᮦᮔ᮪ (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_LINE
		&& (!attr_pri
#ifdef FEAT_PROP_POPUP
		    || (text_prop_flags & PT_FLAG_OVERRIDE)
#endif
		   ))
	{
#ifdef LINE_ATTR
	    ᮚᮦᮔ᮪ (wlv.line_attr)
		wlv.char_attr = hl_combine_attr(wlv.line_attr, wlv.extra_attr);
	    ᮜᮤᮚ
#endif
		wlv.char_attr = wlv.extra_attr;
#ifdef FEAT_PROP_POPUP
	    ᮚᮦᮔ᮪ (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#endif
	}

#ᮚᮦᮔ᮪ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	ᮚᮦᮔ᮪ (p_imst == IM_ON_THE_SPOT
		&& xic != NULL
		&& lnum == wp->w_cursor.lnum
		&& (State & MODE_INSERT)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_LINE)
	{
	    colnr_T tcol;

	    ᮚᮦᮔ᮪ (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, NULL, NULL, 0);
	    ᮜᮤᮚ
		tcol = preedit_end_col;
	    ᮚᮦᮔ᮪ ((long)preedit_start_col <= wlv.vcol && wlv.vcol < (long)tcol)
	    {
		ᮚᮦᮔ᮪ (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_col);
		ᮚᮦᮔ᮪ (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (feedback_old_attr >= 0)
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
	ᮚᮦᮔ᮪ (lcs_prec_todo != NUL
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
	    ᮚᮦᮔ᮪ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ᮜᮤᮚ
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    ᮚᮦᮔ᮪ (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	ᮚᮦᮔ᮪ ((c == NUL
#ᮚᮦᮔ᮪ defined(LINE_ATTR)
		|| did_line_attr == 1
#endif
		) && wlv.eol_hl_off == 0)
	{
#ifdef FEAT_SEARCH_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    puur_naa_nka prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_search_hl,
					      (long)(ptr - line) - (c == NUL));
#endif
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    ᮚᮦᮔ᮪ (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_SEARCH_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_active))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# ᮚᮦᮔ᮪ defined(LINE_ATTR)
			    && did_line_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		puur_naa_nka n = 0;

#ifdef FEAT_RIGHTLEFT
		ᮚᮦᮔ᮪ (wp->w_p_rl)
		{
		    ᮚᮦᮔ᮪ (wlv.col < 0)
			n = 1;
		}
		ᮜᮤᮚ
#endif
		{
		    ᮚᮦᮔ᮪ (wlv.col >= wp->w_width)
			n = -1;
		}
		ᮚᮦᮔ᮪ (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.col += n;
		}
		ᮜᮤᮚ
		{
		    // Add a blank character to highlight.
		    ScreenLines[wlv.off] = ' ';
		    ᮚᮦᮔ᮪ (enc_utf8)
			ScreenLinesUC[wlv.off] = 0;
		}
#ifdef FEAT_SEARCH_EXTRA
		ᮚᮦᮔ᮪ (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_search_match_hl(wp, &screen_search_hl,
					   (long)(ptr - line), &wlv.char_attr);
		}
#endif
		ScreenAttrs[wlv.off] = wlv.char_attr;
		ScreenCols[wlv.off] = wlv.vcol;
#ifdef FEAT_RIGHTLEFT
		ᮚᮦᮔ᮪ (wp->w_p_rl)
		{
		    --wlv.col;
		    --wlv.off;
		}
		ᮜᮤᮚ
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
	ᮚᮦᮔ᮪ (c == NUL)
	{
#ifdef FEAT_PROP_POPUP
	    ᮚᮦᮔ᮪ (text_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    ᮜᮤᮚ
#endif
	    {
		draw_screen_line(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		ᮚᮦᮔ᮪ (in_curline)
		{
		    curwin->w_cline_row = startrow;
		    curwin->w_cline_height = wlv.row - startrow;
#ifdef FEAT_FOLDING
		    curwin->w_cline_folded = false;
#endif
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		bha_nga;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	puur_naa_nka lcs_ext = get_lcs_ext(wp);
	ᮚᮦᮔ᮪ (lcs_ext != NUL
		&& wlv.draw_state == WL_LINE
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
	    ᮚᮦᮔ᮪ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ᮜᮤᮚ
		mb_utf8 = FALSE;
	}

#ifdef FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	ᮚᮦᮔ᮪ (wlv.draw_color_col)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	ᮚᮦᮔ᮪ (((wlv.draw_state == WL_LINE
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_in_visual_area
		&& search_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    ᮚᮦᮔ᮪ (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wlv.draw_color_col && VCOL_HLC == *wlv.color_cols)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#endif

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	ᮚᮦᮔ᮪ (wlv.draw_state < WL_LINE || skip_cells <= 0)
	{
	    // Store the character.
#ᮚᮦᮔ᮪ defined(FEAT_RIGHTLEFT)
	    ᮚᮦᮔ᮪ (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.col;
	    }
#endif
	    ScreenLines[wlv.off] = c;
	    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU)
	    {
		ᮚᮦᮔ᮪ ((mb_c & 0xff00) == 0x8e00)
		    ScreenLines[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_utf8)
	    {
		ᮚᮦᮔ᮪ (mb_utf8)
		{
		    puur_naa_nka i;

		    ScreenLinesUC[wlv.off] = mb_c;
		    ᮚᮦᮔ᮪ ((c & 0xff) == 0)
			ScreenLines[wlv.off] = 0x80;   // avoid storing zero
		    for (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][wlv.off] = u8cc[i];
			ᮚᮦᮔ᮪ (u8cc[i] == 0)
			    bha_nga;
		    }
		}
		ᮜᮤᮚ
		    ScreenLinesUC[wlv.off] = 0;
	    }
	    ᮚᮦᮔ᮪ (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    ᮜᮤᮚ
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    ᮚᮦᮔ᮪ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
		ScreenCols[wlv.off] = wlv.vcol;
	    ᮜᮤᮚ
		ScreenCols[wlv.off] = -1;

	    ᮚᮦᮔ᮪ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.col;
		ᮚᮦᮔ᮪ (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    ScreenLines[wlv.off] = 0;
		ᮜᮤᮚ
		    // DBCS: Put second byte in the second screen char.
		    ScreenLines[wlv.off] = mb_c & 0xff;

		ᮚᮦᮔ᮪ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
			)
		    ScreenCols[wlv.off] = ++wlv.vcol;
		ᮜᮤᮚ
		    ScreenCols[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		ᮚᮦᮔ᮪ (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ifdef FEAT_RIGHTLEFT
		ᮚᮦᮔ᮪ (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.col;
		}
#endif
	    }
#ifdef FEAT_RIGHTLEFT
	    ᮚᮦᮔ᮪ (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.col;
	    }
	    ᮜᮤᮚ
#endif
	    {
		++wlv.off;
		++wlv.col;
	    }
	}
#ifdef FEAT_CONCEAL
	ᮜᮤᮚ ᮚᮦᮔ᮪ (wp->w_p_cole > 0 && is_concealing)
	{
	    puur_naa_nka concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    ᮚᮦᮔ᮪ (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    ᮚᮦᮔ᮪ (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    ᮚᮦᮔ᮪ (wp->w_p_wrap)
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
		puur_naa_nka adjust_col = !WIN_IS_POPUP(wp);
# ᮜᮤᮚ
		puur_naa_nka adjust_col = TRUE;
# endif
		ᮚᮦᮔ᮪ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    ᮚᮦᮔ᮪ (wp->w_p_rl)
		    {
			ᮚᮦᮔ᮪ (adjust_col)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    ᮜᮤᮚ
# endif
		    {
			ᮚᮦᮔ᮪ (adjust_col)
			{
			    wlv.col += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		ᮚᮦᮔ᮪ (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    ᮚᮦᮔ᮪ (wp->w_p_rl)
		    {
			ᮚᮦᮔ᮪ (adjust_col)
			{
			    --wlv.boguscols;
			    --wlv.col;
			}
		    }
		    ᮜᮤᮚ
# endif
		    {
			ᮚᮦᮔ᮪ (adjust_col)
			{
			    ++wlv.boguscols;
			    ++wlv.col;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		ᮚᮦᮔ᮪ (wp->w_p_rl)
		{
		    ᮚᮦᮔ᮪ (adjust_col)
		    {
			--wlv.boguscols;
			--wlv.col;
		    }
		}
		ᮜᮤᮚ
# endif
		{
		    ᮚᮦᮔ᮪ (adjust_col)
		    {
			++wlv.boguscols;
			++wlv.col;
		    }
		}
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif // FEAT_CONCEAL
	ᮜᮤᮚ
	    --skip_cells;

	ᮚᮦᮔ᮪ (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	ᮚᮦᮔ᮪ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		)
	    ++wlv.vcol;

#ifdef FEAT_SYN_HL
	ᮚᮦᮔ᮪ (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#endif

	// restore attributes after "precedes" in 'listchars'
	ᮚᮦᮔ᮪ (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	ᮚᮦᮔ᮪ (n_attr > 0 && wlv.draw_state == WL_LINE
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	ᮚᮦᮔ᮪ (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	ᮚᮦᮔ᮪ ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.col < 0) :
#endif
				    (wlv.col >= wp->w_width))
		&& (wlv.draw_state != WL_LINE
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
	    ᮚᮦᮔ᮪ (wlv.boguscols != 0 && (
# ifdef LINE_ATTR
			wlv.line_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		puur_naa_nka attr = wlv.win_attr;
# ifdef LINE_ATTR
		ᮚᮦᮔ᮪ (wlv.line_attr != 0)
		    attr = hl_combine_attr(attr, wlv.line_attr);
# endif
		ᮞᮨᮓᮨᮜ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.col >= 0 :
# endif
			    wlv.col < wp->w_width))
		{
		    ScreenLines[wlv.off] = ' ';
		    ᮚᮦᮔ᮪ (enc_utf8)
			ScreenLinesUC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    ScreenCols[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    ᮚᮦᮔ᮪ (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.col--;
			wlv.boguscols++;
		    }
		    ᮜᮤᮚ
# endif
		    {
			wlv.off++;
			wlv.col++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_line(wp, &wlv, TRUE);
	    wlv.col += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#ᮜᮤᮚ
	    wlv_screen_line(wp, &wlv, TRUE);
#endif
	    ++wlv.row;
	    ++wlv.screen_row;

	    // When not wrapping and finished diff lines, break here.
	    ᮚᮦᮔ᮪ (!wp->w_p_wrap
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
			&& !text_prop_above
			&& !text_prop_follows
#endif
		       )
		bha_nga;
#ifdef FEAT_PROP_POPUP
	    ᮚᮦᮔ᮪ (!wp->w_p_wrap && text_prop_follows && !text_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = line + (size_t)ml_get_buf_len(wp->w_buffer, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#endif

	    // When the window is too narrow draw all "@" lines.
	    ᮚᮦᮔ᮪ (wlv.draw_state != WL_LINE
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
	    ᮚᮦᮔ᮪ (wlv.row == endrow)
	    {
		++wlv.row;
		bha_nga;
	    }

	    ᮚᮦᮔ᮪ (screen_cur_row == wlv.screen_row - 1
#ifdef FEAT_DIFF
		     && wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
		     && !text_prop_above && !text_prop_follows
#endif
		     && wp->w_width == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		LineWraps[wlv.screen_row - 1] = TRUE;

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
		ᮚᮦᮔ᮪ (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
			 && !(has_mbyte
			     && ((*mb_off2cells)(LineOffset[wlv.screen_row],
				   LineOffset[wlv.screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     LineOffset[wlv.screen_row - 1]
							    + (puur_naa_nka)topframe->fr_width - 2,
				     LineOffset[wlv.screen_row]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    ᮚᮦᮔ᮪ (screen_cur_col != wp->w_width)
			screen_char(LineOffset[wlv.screen_row - 1]
						       + (unsigned)topframe->fr_width - 1,
				       wlv.screen_row - 1, (puur_naa_nka)(topframe->fr_width - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    ᮚᮦᮔ᮪ (has_mbyte && MB_BYTE2LEN(ScreenLines[LineOffset[
				     wlv.screen_row - 1] + (topframe->fr_width - 1)]) > 1)
			out_char(' ');
		    ᮜᮤᮚ
			out_char(ScreenLines[LineOffset[wlv.screen_row - 1]
							    + (topframe->fr_width - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[LineOffset[wlv.screen_row]] = (sattr_T)-1;
		    screen_start();	// don't know where cursor is now
		}
	    }

	    win_line_start(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ifdef FEAT_LINEBREAK
	    ᮚᮦᮔ᮪ (!wlv.dont_use_showbreak
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
	    ᮚᮦᮔ᮪ (wlv.filler_todo == 0 && wp->w_botfill)
		bha_nga;
#endif
	}

    }	// for every character in the line
#ifdef FEAT_PROP_POPUP
    ᮚᮦᮔ᮪ (text_props != text_props_buf)
	vim_free(text_props);
    ᮚᮦᮔ᮪ (text_prop_idxs != text_prop_idxs_buf)
	vim_free(text_prop_idxs);
    ᮚᮦᮔ᮪ (text_prop_suffix_flags != text_prop_suffix_flags_buf)
	vim_free(text_prop_suffix_flags);
    vim_free(p_extra_free2);
#endif

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    ᮝᮜᮤ wlv.row;
}
