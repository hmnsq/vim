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

#অন্তর্ভাৱ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    স্থির পূর্ণাঙ্ক
get_lcs_ext(win_T *wp)
{
    যদি (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	ঘূৰাই NUL;
    যদি (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	ঘূৰাই '>';
    ঘূৰাই wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ifdef FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    স্থির পূর্ণাঙ্ক
advance_color_col(পূর্ণাঙ্ক vcol, পূর্ণাঙ্ক **color_cols)
{
    যেতিয়ালৈ (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    ঘূৰাই (**color_cols >= 0);
}
#endif

#ifdef FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    স্থির শূন্য
margin_columns_win(win_T *wp, পূর্ণাঙ্ক *বাম_col, পূর্ণাঙ্ক *ডান_col)
{
    // cache previous calculations depending on w_virtcol
    স্থির পূর্ণাঙ্ক saved_w_virtcol;
    স্থির win_T *prev_wp;
    স্থির পূর্ণাঙ্ক prev_width1;
    স্থির পূর্ণাঙ্ক prev_width2;
    স্থির পূর্ণাঙ্ক prev_বাম_col;
    স্থির পূর্ণাঙ্ক prev_ডান_col;

    পূর্ণাঙ্ক cur_col_off = win_col_off(wp);
    পূর্ণাঙ্ক	width1;
    পূর্ণাঙ্ক	width2;

    width1 = wp->w_width - cur_col_off;
    width2 = width1 + win_col_off2(wp);

    যদি (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*ডান_col = prev_ডান_col;
	*বাম_col = prev_বাম_col;
	ঘূৰাই;
    }

    *বাম_col = 0;
    *ডান_col = width1;

    যদি (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ডান_col = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    যদি (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*বাম_col = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_বাম_col = *বাম_col;
    prev_ডান_col = *ডান_col;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#endif

#যদি defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define সাৰি_ATTR
#endif

// structure with variables passed between win_line() and other functions
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    পূর্ণাঙ্ক		startrow;	// first row in the window to be drawn
    পূর্ণাঙ্ক		row;		// row in the window, excl w_winrow
    পূর্ণাঙ্ক		screen_row;	// row on the screen, incl w_winrow

    long	vcol;		// virtual column, before wrapping
    পূর্ণাঙ্ক		col;		// visual column on screen, after wrapping
#ifdef FEAT_CONCEAL
    পূর্ণাঙ্ক		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    পূর্ণাঙ্ক		vcol_off_co;	// offset for concealed characters
#endif
    পূর্ণাঙ্ক		vcol_off_tp;	// offset for virtual text
#ifdef FEAT_SYN_HL
    পূর্ণাঙ্ক		draw_color_col;	// highlight colorcolumn
    পূর্ণাঙ্ক		*color_cols;	// pointer to according columns array
#endif
    পূর্ণাঙ্ক		eol_hl_off;	// 1 if highlighted char after EOL

    unsigned	off;		// offset in ScreenLines/ScreenAttrs

    পূর্ণাঙ্ক		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ifdef FEAT_SYN_HL
    পূর্ণাঙ্ক		cul_attr;	// set when 'cursorline' active
#endif
#ifdef সাৰি_ATTR
    পূর্ণাঙ্ক		সাৰি_attr;	// for the whole line, includes 'cursorline'
#endif

    পূর্ণাঙ্ক		screen_সাৰি_flags;  // flags for screen_line()

    পূর্ণাঙ্ক		fromcol;	// start of inverting
    পূর্ণাঙ্ক		tocol;		// end of inverting

#ifdef FEAT_LINEBREAK
    long	vcol_sbr;	    // virtual column after showbreak
    পূর্ণাঙ্ক		need_showbreak;	    // overlong line, skipping first x chars
    পূর্ণাঙ্ক		dont_use_showbreak; // do not use 'showbreak'
#endif
#ifdef FEAT_PROP_POPUP
    পূর্ণাঙ্ক		text_prop_above_গুনতি;
#endif

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    পূর্ণাঙ্ক		cul_screenline;

    পূর্ণাঙ্ক		char_attr;	// attributes for the next character

    পূর্ণাঙ্ক		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    পূর্ণাঙ্ক		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    পূর্ণাঙ্ক		n_attr_skip;    // chars to skip before using extra_attr
    পূর্ণাঙ্ক		c_extra;	// extra chars, all the same
    পূর্ণাঙ্ক		c_final;	// final char, mandatory if set
    পূর্ণাঙ্ক		extra_for_textprop; // n_extra set for textprop
    পূর্ণাঙ্ক		start_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    পূর্ণাঙ্ক		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    পূর্ণাঙ্ক		saved_extra_attr;
    পূর্ণাঙ্ক		saved_n_attr_skip;
    পূর্ণাঙ্ক		saved_extra_for_textprop;
    পূর্ণাঙ্ক		saved_c_extra;
    পূর্ণাঙ্ক		saved_c_final;
    পূর্ণাঙ্ক		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ifdef FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#endif
    পূর্ণাঙ্ক		filler_সাৰিগুলি;	// nr of filler lines to be drawn
    পূর্ণাঙ্ক		filler_todo;	// nr of filler lines still to do + 1
#ifdef FEAT_SIGNS
    sign_attrs_T sattr;
#endif
#ifdef FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    পূর্ণাঙ্ক		need_lbr;
#endif
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#define WL_START	0		// nothing done yet, must be zero
#define WL_CMDLINE	(WL_START + 1)	// cmdline window column
#ifdef FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#অন্যথা
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#অন্যথা
# define WL_SIGN	WL_FOLD		// column for signs
#endif
#define WL_NR		(WL_SIGN + 1)	// line number
#ifdef FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#অন্যথা
# define WL_BRI		WL_NR
#endif
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#অন্যথা
# define WL_SBR		WL_BRI
#endif
#define WL_সাৰি		(WL_SBR + 1)	// text in the line

#যদি defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    স্থির পূর্ণাঙ্ক
use_cursor_সাৰি_highlight(win_T *wp, linenr_T lnum)
{
    ঘূৰাই wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#endif


#ifdef FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    স্থির শূন্য
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    পূর্ণাঙ্ক fdc = compute_foldcolumn(wp, 0);

    যদি (fdc <= 0)
	ঘূৰাই;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    যদি (wlv->p_extra_free == রিক্ত)
	ঘূৰাই;

    wlv->n_extra = (পূর্ণাঙ্ক)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    যদি (use_cursor_সাৰি_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    অন্যথা
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
    স্থির শূন্য
get_sign_display_info(
	পূর্ণাঙ্ক		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    পূর্ণাঙ্ক	text_sign;
# ifdef FEAT_SIGN_ICONS
    পূর্ণাঙ্ক	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    যদি (nrcol)
	wlv->n_extra = সংখ্যা_width(wp) + 1;
    অন্যথা
    {
	যদি (use_cursor_সাৰি_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	অন্যথা
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    যদি (wlv->row == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_সাৰিগুলি && wlv->filler_todo <= 0
# endif
       )
    {
	text_sign = (wlv->sattr.sat_text != রিক্ত) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != রিক্ত) ? wlv->sattr.sat_typenr : 0;
	যদি (gui.in_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    যদি (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((ৱর্ণ *)wlv->extra, পরিমাণ(wlv->extra),
						"%-*c ", সংখ্যা_width(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    অন্যথা
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    যদি (netbeans_active() && (buf_signcount(wp->w_buffer, wlv->lnum)
									  > 1))
	    {
		যদি (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((ৱর্ণ *)wlv->extra, পরিমাণ(wlv->extra),
						"%-*c ", সংখ্যা_width(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		অন্যথা
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	অন্যথা
# endif
	    যদি (text_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_text;
		যদি (wlv->p_extra != রিক্ত)
		{
		    যদি (nrcol)
		    {
			পূর্ণাঙ্ক width = সংখ্যা_width(wp) - 2;

			vim_memset(wlv->extra, ' ', width);
			wlv->n_extra = width;
			wlv->n_extra += vim_snprintf((ৱর্ণ *)wlv->extra + width,
				  পরিমাণ(wlv->extra) - width, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    অন্যথা
			wlv->n_extra = (পূর্ণাঙ্ক)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		যদি (use_cursor_সাৰি_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		অন্যথা
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#endif

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    স্থির শূন্য
handle_lnum_col(
	win_T		*wp,
	winlinevars_T	*wlv,
	পূর্ণাঙ্ক		sign_present UNUSED,
	পূর্ণাঙ্ক		অঙ্ক_attr UNUSED)
{
    পূর্ণাঙ্ক has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != রিক্ত;
    পূর্ণাঙ্ক lnum_row = wlv->startrow + wlv->filler_সাৰিগুলি
#ifdef FEAT_PROP_POPUP
		      + wlv->text_prop_above_গুনতি
#endif
		      ;

    যদি ((wp->w_p_nu || wp->w_p_rnu)
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
	যদি ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_text != রিক্ত)
	    get_sign_display_info(TRUE, wp, wlv);
	অন্যথা
#endif
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  যদি (wlv->row == lnum_row
		    && (wp->w_skipcol == 0 || wlv->row > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      long অঙ্ক;
	      ৱর্ণ *fmt = "%*ld ";

	      যদি (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  অঙ্ক = (long)wlv->lnum;
	      অন্যথা
	      {
		  // 'relativenumber', don't use negative numbers
		  অঙ্ক = labs((long)get_cursor_rel_lnum(wp, wlv->lnum));
		  যদি (অঙ্ক == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      অঙ্ক = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((ৱর্ণ *)wlv->extra, পরিমাণ(wlv->extra), fmt, সংখ্যা_width(wp), অঙ্ক);
	      যদি (wp->w_skipcol > 0 && wlv->startrow == 0)
		  বাবে (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ifdef FEAT_RIGHTLEFT
	      যদি (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  পূর্ণাঙ্ক	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  বাবে (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
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
	  অন্যথা
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = সংখ্যা_width(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ifdef FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  যদি (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->row == lnum_row
		      || (wlv->row > lnum_row
			 && (wp->w_p_culopt_flags & CULOPT_সাৰি))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#endif
	  যদি (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  যদি (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ifdef FEAT_SIGNS
	যদি (অঙ্ক_attr)
	    wlv->char_attr = অঙ্ক_attr;
#endif
    }
}

#ifdef FEAT_LINEBREAK
    স্থির শূন্য
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    যদি (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_মান(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    অন্যথা যদি (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    যদি (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	যদি (wp->w_p_bri && (wlv->row > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_সাৰিগুলি
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    যদি (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = রিক্ত;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_buffer, wlv->lnum, FALSE));
	    যদি (wlv->row == wlv->startrow)
	    {
		wlv->n_extra -= win_col_off2(wp);
		যদি (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    যদি (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    যদি (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	যদি (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#endif

#যদি defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    স্থির শূন্য
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    যদি (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	যদি (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	অন্যথা
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	যদি (wp->w_p_rl)
	    wlv->n_extra = wlv->col + 1;
	অন্যথা
#  endif
	    wlv->n_extra = wp->w_width - wlv->col;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_মান(wp);
    যদি (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (পূর্ণাঙ্ক)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	যদি (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	যদি (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	যদি (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    যদি (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#endif

#যদি defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    স্থির পূর্ণাঙ্ক
textprop_size_after_trunc(
	win_T	*wp,
	পূর্ণাঙ্ক	flags,	    // TP_FLAG_ALIGN_*
	পূর্ণাঙ্ক	added,
	পূর্ণাঙ্ক	padding,
	char_u	*text,
	পূর্ণাঙ্ক	*n_used_ptr)
{
    পূর্ণাঙ্ক	space = (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_width - win_col_off(wp) : added;
    পূর্ণাঙ্ক strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    যদি (space < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_width;
    যদি (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    বাবে (p = text; *p != NUL; p += (*mb_ptr2len)(p))
    {
	পূর্ণাঙ্ক clen = ptr2cells(p);

	যদি (strsize + clen > space)
	    ভঙ্গ;
	strsize += clen;
    }
    *n_used_ptr = (পূর্ণাঙ্ক)(p - text);

    ঘূৰাই strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    পূর্ণাঙ্ক
text_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	পূর্ণাঙ্ক	    vcol,	    // current text column
	পূর্ণাঙ্ক	    scr_col,	    // current screen column
	পূর্ণাঙ্ক	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	পূর্ণাঙ্ক	    *n_attr,	    // attribute cells, NULL if not used
	পূর্ণাঙ্ক	    *n_attr_skip,   // cells to skip attr, NULL if not used
	পূর্ণাঙ্ক	    do_skip)	    // skip_cells is not zero
{
    পূর্ণাঙ্ক	    ডান = (tp->tp_flags & TP_FLAG_ALIGN_ডান);
    পূর্ণাঙ্ক	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    পূর্ণাঙ্ক	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    পূর্ণাঙ্ক	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    পূর্ণাঙ্ক	    padding = tp->tp_col == MAXCOL ? tp->tp_padleft : 0;
    পূর্ণাঙ্ক	    col_with_padding = scr_col + (below ? 0 : padding);
    পূর্ণাঙ্ক	    room = wp->w_width - col_with_padding;
    পূর্ণাঙ্ক	    before = room;	// spaces before the text
    পূর্ণাঙ্ক	    after = 0;		// spaces after the text
    পূর্ণাঙ্ক	    n_used = *n_extra;
    char_u  *l = রিক্ত;
    পূর্ণাঙ্ক	    strsize = vim_strsize(*p_extra);
    পূর্ণাঙ্ক	    cells = wrap ? strsize : textprop_size_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_used);

    যদি (wrap || ডান || above || below || padding > 0 || n_used < *n_extra)
    {
	পূর্ণাঙ্ক	    col_off = win_col_off(wp) - win_col_off2(wp);

	যদি (above)
	{
	    before = 0;
	    after = wp->w_width - cells - win_col_off(wp) - padding;
	    যদি (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	অন্যথা
	{
	    // Right-align: fill with before
	    যদি (ডান)
		before -= cells;

	    // Below-align: empty line add one character
	    যদি (below && vcol == 0 && col_with_padding == col_off
					    && wp->w_width - col_off == before)
		col_with_padding += 1;

	    যদি (before < 0
		    || !(ডান || below)
		    || (below ? (col_with_padding <= col_off || !wp->w_p_wrap)
			      : (n_used < *n_extra)))
	    {
		যদি (ডান && (wrap
			      || (room < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_width - col_off - strsize + room;
		    যদি (before < 0)
			before = 0;
		    অন্যথা
			n_used = *n_extra;
		}
		অন্যথা যদি (below && before > vcol && do_skip)
		    before -= vcol;
		অন্যথা
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	যদি (n_used < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_used;

	// add 1 for NUL, 2 for when '…' is used
	যদি (n_attr != রিক্ত)
	    l = alloc(n_used + before + after + (padding > 0 ? padding : 0) + 3);
	যদি (n_attr == রিক্ত || l != রিক্ত)
	{
	    পূর্ণাঙ্ক off = 0;

	    যদি (n_attr != রিক্ত)
	    {
		vim_memset(l, ' ', before);
		off += before;
		যদি (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_used);
		off += n_used;
	    }
	    অন্যথা
	    {
		off = before + after + padding + n_used;
		cells += before + after + padding;
	    }
	    যদি (n_attr != রিক্ত)
	    {
		যদি (n_used < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    যদি (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			যদি (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_setup(&vc, (char_u *)"utf-8", p_enc);
			    যদি (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, রিক্ত);
				যদি (cp == রিক্ত)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_setup(&vc, রিক্ত, রিক্ত);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_head_off)(l, lp);
			STRCPY(lp, cp);
			n_used = lp - l + 3 - before - padding;
			যদি (cp != buf)
			    vim_free(cp);
		    }
		    অন্যথা
			// change last character to '>'
			*lp = '>';
		}
		অন্যথা যদি (after > 0)
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
		যদি (above)
		    *n_attr -= after;
	    }
	}
    }

    যদি (n_attr == রিক্ত)
	ঘূৰাই cells;
    ঘূৰাই (below && col_with_padding > win_col_off(wp) && !wp->w_p_wrap);
}

# যদি defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    পূর্ণাঙ্ক
text_prop_no_showbreak(textprop_T *tp)
{
    পূর্ণাঙ্ক	    ডান = (tp->tp_flags & TP_FLAG_ALIGN_ডান);
    পূর্ণাঙ্ক	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    পূর্ণাঙ্ক	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    পূর্ণাঙ্ক	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    ঘূৰাই (ডান || above || below || !wrap);
}
# endif
#endif

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    স্থির শূন্য
wlv_screen_সাৰি(win_T *wp, winlinevars_T *wlv, পূর্ণাঙ্ক clear_end)
{
    যদি (wlv->row == 0 && wp->w_skipcol > 0
#যদি defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_মান(wp) == NUL
#endif
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	পূর্ণাঙ্ক off = (পূর্ণাঙ্ক)(current_Screen_সাৰি - Screen_সাৰিগুলি);
	পূর্ণাঙ্ক max_off = off + screen_Columns;
	পূর্ণাঙ্ক skip = 0;

	যদি (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    যেতিয়ালৈ (skip < wp->w_width && VIM_ISDIGIT(Screen_সাৰিগুলি[off]))
	    {
		++off;
		++skip;
	    }

	বাবে (পূর্ণাঙ্ক i = 0; i < 3 && i + skip < wp->w_width; ++i)
	{
	    যদি ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_সাৰিগুলি[off + 1] = ' ';
	    Screen_সাৰিগুলি[off] = '<';
	    যদি (enc_utf8)
		Screen_সাৰিগুলি_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_সাৰি(wp, wlv->screen_row, wp->w_wincol, wlv->col,
		    clear_end ? wp->w_width : -wp->w_width,
		    wlv->vcol - 1, wlv->screen_সাৰি_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    স্থির শূন্য
draw_screen_সাৰি(win_T *wp, winlinevars_T *wlv)
{
#ifdef FEAT_SYN_HL
    long	v;
    পূর্ণাঙ্ক		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    যদি (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    অন্যথা
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_width - wlv->col - 1 :
# endif
	wlv->col;
    // check if line ends before left margin
    যদি (wlv->vcol < v + wcol - win_col_off(wp))
	wlv->vcol = v + wcol - win_col_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->col -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# অন্যথা
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    যদি (wlv->draw_color_col)
	wlv->draw_color_col = advance_color_col(VCOL_HLC, &wlv->color_cols);

    যদি (((wp->w_p_cuc
		    && (পূর্ণাঙ্ক)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (পূর্ণাঙ্ক)wp->w_virtcol <
			 (long)wp->w_width * (wlv->row - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_col
# ifdef সাৰি_ATTR
		|| wlv->সাৰি_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	পূর্ণাঙ্ক	rightmost_vcol = 0;
	পূর্ণাঙ্ক	i;

	যদি (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	যদি (wlv->draw_color_col)
	    // determine rightmost colorcolumn to possibly draw
	    বাবে (i = 0; wlv->color_cols[i] >= 0; ++i)
		যদি (rightmost_vcol < wlv->color_cols[i])
		    rightmost_vcol = wlv->color_cols[i];

	যেতিয়ালৈ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->col >= 0) :
# endif
		(wlv->col < wp->w_width))
	{
	    Screen_সাৰিগুলি[wlv->off] = ' ';
	    যদি (enc_utf8)
		Screen_সাৰিগুলি_UC[wlv->off] = 0;

	    যদি (wlv->draw_color_col)
		wlv->draw_color_col = advance_color_col(
						   VCOL_HLC, &wlv->color_cols);

	    পূর্ণাঙ্ক attr = wlv->win_attr;
# ifdef সাৰি_ATTR
	    যদি (wlv->সাৰি_attr != 0)
		attr = hl_combine_attr(attr, wlv->সাৰি_attr);
# endif
	    যদি (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    অন্যথা যদি (wlv->draw_color_col && VCOL_HLC == *wlv->color_cols)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    ScreenCols[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->col;
	    }
	    অন্যথা
# endif
	    {
		++wlv->off;
		++wlv->col;
	    }
	    ++wlv->vcol;

	    যদি (VCOL_HLC > rightmost_vcol
# ifdef সাৰি_ATTR
		    && wlv->সাৰি_attr == 0
# endif
		    && wlv->win_attr == 0)
		ভঙ্গ;
	}
    }
#endif

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_সাৰি_flags |= SLF_INC_VCOL;
    wlv_screen_সাৰি(wp, wlv, TRUE);
    wlv->screen_সাৰি_flags &= ~SLF_INC_VCOL;
    ++wlv->row;
    ++wlv->screen_row;
}
#undef VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    স্থির শূন্য
win_সাৰি_start(win_T *wp UNUSED, winlinevars_T *wlv, পূর্ণাঙ্ক save_extra)
{
    wlv->col = 0;
    wlv->off = (unsigned)(current_Screen_সাৰি - Screen_সাৰিগুলি);
#ifdef FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#endif

#ifdef FEAT_RIGHTLEFT
    যদি (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->col = wp->w_width - 1;
	wlv->off += wlv->col;
	wlv->screen_সাৰি_flags |= SLF_RIGHTLEFT;
    }
#endif
    যদি (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_START;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = রিক্ত;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ifdef FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#endif
#ifdef FEAT_SYN_HL
	যদি (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	অন্যথা
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
    স্থির শূন্য
win_সাৰি_continue(winlinevars_T *wlv)
{
    যদি (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = রিক্ত;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    অন্যথা
	wlv->char_attr = wlv->win_attr;
}

#ifdef FEAT_SYN_HL
    স্থির শূন্য
apply_cursorline_highlight(
	winlinevars_T *wlv,
	পূর্ণাঙ্ক sign_present UNUSED)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    যদি (sign_present && wlv->sattr.sat_linehl > 0)
    {
	যদি (wlv->sattr.sat_priority >= 100)
	    wlv->সাৰি_attr = hl_combine_attr(wlv->cul_attr, wlv->সাৰি_attr);
	অন্যথা
	    wlv->সাৰি_attr = hl_combine_attr(wlv->সাৰি_attr, wlv->cul_attr);
    }
    অন্যথা
# endif
# যদি defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->সাৰি_attr = hl_combine_attr(wlv->cul_attr, wlv->সাৰি_attr);
# অন্যথা
	wlv->সাৰি_attr = wlv->cul_attr;
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
    পূর্ণাঙ্ক
win_সাৰি(
    win_T	*wp,
    linenr_T	lnum,
    পূর্ণাঙ্ক		startrow,
    পূর্ণাঙ্ক		endrow,
    পূর্ণাঙ্ক		সংখ্যা_only,
    spellvars_T	*spv UNUSED)
{
    winlinevars_T	wlv;		// variables passed between functions

    পূর্ণাঙ্ক		c = 0;			// init for GCC
    char_u	*সাৰি;			// current line
    char_u	*ptr;			// current position in "line"
    পূর্ণাঙ্ক		in_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ifdef FEAT_PROP_POPUP
    char_u	*p_extra_free2 = রিক্ত;   // another p_extra to be freed
#endif
#যদি defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    পূর্ণাঙ্ক		in_linebreak = FALSE;	// n_extra set for showing linebreak
#endif
    পূর্ণাঙ্ক		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    পূর্ণাঙ্ক		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    পূর্ণাঙ্ক		n_attr = 0;	    // chars with special attr
    পূর্ণাঙ্ক		saved_attr2 = 0;    // char_attr saved for n_attr
    পূর্ণাঙ্ক		n_attr3 = 0;	    // chars with overruling special attr
    পূর্ণাঙ্ক		saved_attr3 = 0;    // char_attr saved for n_attr3

    পূর্ণাঙ্ক		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    পূর্ণাঙ্ক		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    পূর্ণাঙ্ক		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    পূর্ণাঙ্ক		attr_pri = FALSE;	// char_attr has priority
    পূর্ণাঙ্ক		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    পূর্ণাঙ্ক		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    পূর্ণাঙ্ক		area_attr = 0;		// attributes desired by highlighting
    পূর্ণাঙ্ক		অনুসন্ধান_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ifdef FEAT_SYN_HL
    পূর্ণাঙ্ক		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    পূর্ণাঙ্ক		syntax_attr = 0;	// attributes desired by syntax
    পূর্ণাঙ্ক		prev_syntax_col = -1;	// column of prev_syntax_attr
    পূর্ণাঙ্ক		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    পূর্ণাঙ্ক		has_syntax = FALSE;	// this buffer has syntax highl.
    পূর্ণাঙ্ক		save_did_emsg;
#endif
#ifdef FEAT_PROP_POPUP
# define WIN_সাৰি_TEXT_PROP_স্তূপ_LEN 32
    পূর্ণাঙ্ক		did_সাৰি = FALSE;	// set to TRUE when line text done
    পূর্ণাঙ্ক		text_prop_গুনতি;
    পূর্ণাঙ্ক		last_textprop_text_idx = -1;
    পূর্ণাঙ্ক		text_prop_next = 0;	// next text property to use
    textprop_T	text_props_buf[WIN_সাৰি_TEXT_PROP_স্তূপ_LEN];
    পূর্ণাঙ্ক		text_prop_idxs_buf[WIN_সাৰি_TEXT_PROP_স্তূপ_LEN];
    char_u	text_prop_suffix_flags_buf[WIN_সাৰি_TEXT_PROP_স্তূপ_LEN + 1];
    textprop_T	*text_props = রিক্ত;
    পূর্ণাঙ্ক		*text_prop_idxs = রিক্ত;
    char_u	*text_prop_suffix_flags = রিক্ত;
    পূর্ণাঙ্ক		text_props_active = 0;
    পূর্ণাঙ্ক		text_props_need_sort = FALSE;
    proptype_T  *text_prop_type = রিক্ত;
    পূর্ণাঙ্ক		text_prop_attr = 0;
    পূর্ণাঙ্ক		text_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    পূর্ণাঙ্ক		text_prop_id = 0;	// active property ID
    পূর্ণাঙ্ক		text_prop_flags = 0;
    পূর্ণাঙ্ক		text_prop_above = FALSE;  // first doing virtual text above
    পূর্ণাঙ্ক		text_prop_follows = FALSE;  // another text prop to display
    পূর্ণাঙ্ক		saved_অনুসন্ধান_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    পূর্ণাঙ্ক		saved_area_attr = 0;	// idem for area_attr
    পূর্ণাঙ্ক		reset_extra_attr = FALSE;
#endif
#ifdef FEAT_SPELL
    পূর্ণাঙ্ক		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    পূর্ণাঙ্ক		nextlinecol = 0;	// column where nextline[] starts
    পূর্ণাঙ্ক		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    পূর্ণাঙ্ক		spell_attr = 0;		// attributes desired by spelling
    পূর্ণাঙ্ক		word_end = 0;		// last byte with same spell_attr
    পূর্ণাঙ্ক		cur_checked_col = 0;	// checked column for current line
#endif
    পূর্ণাঙ্ক		extra_check = FALSE;	// has extra highlighting
    পূর্ণাঙ্ক		multi_attr = 0;		// attributes desired by multibyte
    পূর্ণাঙ্ক		mb_l = 1;		// multi-byte byte length
    পূর্ণাঙ্ক		mb_c = 0;		// decoded multi-byte character
    পূর্ণাঙ্ক		mb_utf8 = FALSE;	// screen char is UTF-8 char
    পূর্ণাঙ্ক		u8cc[MAX_MCO];		// composing UTF-8 chars
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		change_start = MAXCOL;	// first col of changed area
    পূর্ণাঙ্ক		change_end = -1;	// last col of changed area
    diffline_T	সাৰি_changes;
    পূর্ণাঙ্ক		change_index = -1;
#endif
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    পূর্ণাঙ্ক		in_multispace = FALSE;	// in multiple consecutive spaces
    পূর্ণাঙ্ক		multispace_pos = 0;	// position in lcs-multispace string
#ifdef সাৰি_ATTR
    পূর্ণাঙ্ক		সাৰি_attr_save = 0;
#endif
    পূর্ণাঙ্ক		sign_present = FALSE;
    পূর্ণাঙ্ক		অঙ্ক_attr = 0;		// attribute for the number column
#ifdef FEAT_ARABIC
    পূর্ণাঙ্ক		prev_c = 0;		// previous Arabic character
    পূর্ণাঙ্ক		prev_c1 = 0;		// first composing char for prev_c
#endif
#যদি defined(সাৰি_ATTR)
    পূর্ণাঙ্ক		did_সাৰি_attr = 0;
#endif
#ifdef FEAT_TERMINAL
    পূর্ণাঙ্ক		get_term_attr = FALSE;
#endif

#যদি defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    পূর্ণাঙ্ক		বাম_curline_col = 0;
    পূর্ণাঙ্ক		ডান_curline_col = 0;
#endif

#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    পূর্ণাঙ্ক		feedback_col = 0;
    পূর্ণাঙ্ক		feedback_old_attr = -1;
#endif

#যদি defined(FEAT_CONCEAL) || defined(FEAT_অনুসন্ধান_EXTRA)
    পূর্ণাঙ্ক		match_conc	= 0;	// cchar for match functions
#endif
#যদি defined(FEAT_CONCEAL) || defined(FEAT_অনুসন্ধান_EXTRA) || defined(FEAT_LINEBREAK)
    পূর্ণাঙ্ক		on_last_col     = FALSE;
#endif
#ifdef FEAT_CONCEAL
    পূর্ণাঙ্ক		syntax_flags	= 0;
    পূর্ণাঙ্ক		syntax_seqnr	= 0;
    পূর্ণাঙ্ক		prev_syntax_id	= 0;
    পূর্ণাঙ্ক		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    পূর্ণাঙ্ক		is_concealing	= FALSE;
    পূর্ণাঙ্ক		did_wcol	= FALSE;
    পূর্ণাঙ্ক		old_boguscols   = 0;
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
#অন্যথা
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#endif

    যদি (startrow > endrow)		// past the end already!
	ঘূৰাই startrow;

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

    যদি (সংখ্যা_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ifdef FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#endif
#ifdef FEAT_SYN_HL
	যদি (syntax_present(wp) && !wp->w_s->b_syn_error
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
	    যদি (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    অন্যথা
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		যদি (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_cols = wp->w_p_cc_cols;
	যদি (wlv.color_cols != রিক্ত)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);
#endif

#ifdef FEAT_TERMINAL
	যদি (term_show_buffer(wp->w_buffer))
	{
	    extra_check = TRUE;
	    get_term_attr = TRUE;
	    wlv.win_attr = term_get_attr(wp, lnum, -1);
	}
#endif

	// handle Visual active in this window
	যদি (VIsual_active && wp->w_buffer == curwin->w_buffer)
	{
	    pos_T	*শীৰ্ষ, *bot;

	    যদি (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		শীৰ্ষ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    অন্যথা
	    {
		// Visual is before curwin->w_cursor
		শীৰ্ষ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_in_visual_area = (lnum >= শীৰ্ষ->lnum && lnum <= bot->lnum);
	    যদি (VIsual_mode == Ctrl_V)
	    {
		// block mode
		যদি (lnum_in_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    অন্যথা
	    {
		// non-block mode
		যদি (lnum > শীৰ্ষ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		অন্যথা যদি (lnum == শীৰ্ষ->lnum)
		{
		    যদি (VIsual_mode == 'V')	// linewise
			wlv.fromcol = 0;
		    অন্যথা
		    {
			getvvcol(wp, শীৰ্ষ, &wlv.fromcol, রিক্ত, রিক্ত, 0);
			যদি (gchar_pos(শীৰ্ষ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		যদি (VIsual_mode != 'V' && lnum == bot->lnum)
		{
		    যদি (*p_sel == 'e' && bot->col == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    অন্যথা যদি (bot->col == MAXCOL)
			wlv.tocol = MAXCOL;
		    অন্যথা
		    {
			pos = *bot;
			যদি (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, রিক্ত, রিক্ত, 0);
			অন্যথা
			{
			    getvvcol(wp, &pos, রিক্ত, রিক্ত, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    যদি (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#যদি defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		যদি ((clip_star.available && !clip_star.owned
						    && clip_isautosel_star())
			|| (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#endif
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	অন্যথা যদি (highlight_match
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + অনুসন্ধান_match_সাৰিগুলি)
	{
	    যদি (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, রিক্ত, রিক্ত, 0);
	    অন্যথা
		wlv.fromcol = 0;
	    যদি (lnum == curwin->w_cursor.lnum + অনুসন্ধান_match_সাৰিগুলি)
	    {
		pos.lnum = lnum;
		pos.col = অনুসন্ধান_match_endcol;
		getvcol(curwin, &pos, &wlv.tocol, রিক্ত, রিক্ত, 0);
	    }
	    অন্যথা
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    যদি (wlv.fromcol == wlv.tocol && অনুসন্ধান_match_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ifdef FEAT_DIFF

    পূর্ণাঙ্ক linestatus = 0;
    wlv.filler_সাৰিগুলি = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_FIELD(সাৰি_changes);

    যদি (linestatus < 0)
    {
	যদি (linestatus == -1)
	{
	    যদি (diff_find_change(wp, lnum, &সাৰি_changes))
	    {
		wlv.diff_hlf = HLF_ADD;	// added line
	    }
	    অন্যথা যদি (সাৰি_changes.অঙ্ক_changes > 0)
	    {
		পূর্ণাঙ্ক added = diff_change_parse(
			&সাৰি_changes, &সাৰি_changes.changes[0],
			&change_start, &change_end);
		যদি (change_start == 0)
		{
		    যদি (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    অন্যথা
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		অন্যথা
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_index = 0;
	    }
	    অন্যথা
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_index = 0;
	    }
	}
	অন্যথা
	    wlv.diff_hlf = HLF_ADD;

	area_highlighting = TRUE;
    }

    যদি (lnum == wp->w_topline)
	wlv.filler_সাৰিগুলি = wp->w_topfill;

    wlv.filler_todo = wlv.filler_সাৰিগুলি;
#endif

#ifdef FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    যদি (sign_present)
	অঙ্ক_attr = wlv.sattr.sat_numhl;
#endif

#ifdef সাৰি_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    যদি (sign_present)
	wlv.সাৰি_attr = wlv.sattr.sat_linehl;
# endif
# যদি defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    যদি (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	wlv.সাৰি_attr = HL_ATTR(HLF_QFL);
# endif
    যদি (wlv.সাৰি_attr != 0)
	area_highlighting = TRUE;
#endif

#ifdef FEAT_SPELL
    যদি (spv->spv_has_spell && সংখ্যা_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	যদি (lnum == spv->spv_checked_lnum)
	    cur_checked_col = spv->spv_checked_col;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	যদি (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_col = 0;
	অন্যথা যদি (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_col = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	যদি (lnum < wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	{
	    সাৰি = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_সাৰি(nextline + SPWORDLEN, সাৰি, SPWORDLEN);
	}
	সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(সাৰি);
	যদি (*ptr == NUL)
	{
	    spv->spv_cap_col = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	অন্যথা যদি (spv->spv_cap_col == 0)
	    spv->spv_cap_col = ptr - সাৰি;

	// Copy the end of the current line into nextline[].
	যদি (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	অন্যথা
	{
	    v = ml_get_buf_len(wp->w_buffer, lnum);
	    যদি (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, সাৰি, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    অন্যথা
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, সাৰি + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = সাৰি;

    যদি (wp->w_p_list)
    {
	যদি (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != রিক্ত
		|| wp->w_lcs_chars.leadmultispace != রিক্ত
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	যদি (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_buffer, lnum);
	    যেতিয়ালৈ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - সাৰি);
	}
	// find end of leading whitespace
	যদি (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != রিক্ত ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    যেতিয়ালৈ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    যদি (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    অন্যথা
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - সাৰি) + 1;
	}
    }

    যদি (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    পূর্ণাঙ্ক skipcol_in_text_prop_above = 0;

#ifdef FEAT_PROP_POPUP
    যদি (WIN_IS_POPUP(wp))
	wlv.screen_সাৰি_flags |= SLF_POPUP;

    char_u *prop_start;
    text_prop_গুনতি = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
    যদি (text_prop_গুনতি > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	যদি (text_prop_গুনতি <= WIN_সাৰি_TEXT_PROP_স্তূপ_LEN)
	    text_props = text_props_buf;
	অন্যথা
	    text_props = ALLOC_MULT(textprop_T, text_prop_গুনতি);
	যদি (text_props != রিক্ত)
	{
	    mch_memmove(text_props, prop_start,
				     text_prop_গুনতি * পরিমাণ(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *গুনতি_ptr = prop_start - PROP_গুনতি_SIZE;

	    বাবে (পূর্ণাঙ্ক i = 0; i < text_prop_গুনতি; ++i)
	    {
		যদি (text_props[i].tp_id < 0
				&& text_props[i].u.tp_text_offset > 0)
		{
		    text_props[i].u.tp_text =
			    গুনতি_ptr + text_props[i].u.tp_text_offset;
		    text_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		অন্যথা
		    text_props[i].u.tp_text = রিক্ত;
	    }
	}

	// Allocate an array for the indexes.
	যদি (text_prop_গুনতি <= WIN_সাৰি_TEXT_PROP_স্তূপ_LEN)
	    text_prop_idxs = text_prop_idxs_buf;
	অন্যথা
	    text_prop_idxs = ALLOC_MULT(পূর্ণাঙ্ক, text_prop_গুনতি);
	যদি (text_prop_idxs == রিক্ত)
	{
	    যদি (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    অন্যথা
		text_props = রিক্ত;
	}
	যদি (text_prop_গুনতি <= WIN_সাৰি_TEXT_PROP_স্তূপ_LEN)
	    text_prop_suffix_flags = text_prop_suffix_flags_buf;
	অন্যথা
	    text_prop_suffix_flags = ALLOC_MULT(char_u, text_prop_গুনতি + 1);
	যদি (text_prop_suffix_flags == রিক্ত)
	{
	    যদি (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    text_prop_idxs = রিক্ত;
	    যদি (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    অন্যথা
		text_props = রিক্ত;
	}

	যদি (text_props != রিক্ত)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    বাবে (পূর্ণাঙ্ক i = 0; i < text_prop_গুনতি; ++i)
		যদি (text_props[i].tp_id < 0)
		    last_textprop_text_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    পূর্ণাঙ্ক text_width = wp->w_width - win_col_off(wp);
	    বাবে (পূর্ণাঙ্ক i = text_prop_গুনতি - 1; i >= 0; --i)
		যদি (text_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    যদি (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_in_text_prop_above
								 >= text_width)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_in_text_prop_above += text_width;
			বাবে (পূর্ণাঙ্ক j = i + 1; j < text_prop_গুনতি; ++j)
			    text_props[j - 1] = text_props[j];
			++i;
			--text_prop_গুনতি;
		    }
		    অন্যথা
			++wlv.text_prop_above_গুনতি;
		}

	    text_prop_suffix_flags[text_prop_গুনতি] = 0;
	    বাবে (পূর্ণাঙ্ক i = text_prop_গুনতি - 1; i >= 0; --i)
	    {
		পূর্ণাঙ্ক flags = text_prop_suffix_flags[i + 1];

		যদি (text_props[i].tp_col == MAXCOL)
		{
		    flags |= 1;
		    যদি (text_props[i].tp_flags & TP_FLAG_ALIGN_BELOW)
			flags |= 2;
		}
		text_prop_suffix_flags[i] = flags;
	    }
	}
    }

    যদি (সংখ্যা_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.row += wlv.text_prop_above_গুনতি;
	যদি (wlv.row >= endrow)
	{
	    যদি (text_props != text_props_buf)
		vim_free(text_props);
	    যদি (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    যদি (text_prop_suffix_flags != text_prop_suffix_flags_buf)
		vim_free(text_prop_suffix_flags);
	    ঘূৰাই wlv.row;
	}
	wlv.screen_row += wlv.text_prop_above_গুনতি;
    }
#endif

#যদি defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_first_char = 0;
    যদি (wp->w_p_lbr && সংখ্যা_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, সাৰি, সাৰি);
	(শূন্য)win_lbr_chartabsize(&cts, রিক্ত, রিক্ত);
	vcol_first_char = cts.cts_first_char;
	clear_chartabsize_arg(&cts);
    }
#endif

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    যদি (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_in_text_prop_above : 0;
    অন্যথা
	v = wp->w_leftcol;
    যদি (v > 0 && সংখ্যা_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	পূর্ণাঙ্ক		charsize = 0;
	পূর্ণাঙ্ক		head = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, সাৰি, ptr);
	cts.cts_max_head_vcol = v;
	যেতিয়ালৈ (cts.cts_vcol < v)
	{
	    head = 0;
	    charsize = win_lbr_chartabsize(&cts, &head, রিক্ত);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    যদি (*prev_ptr == NUL)
		ভঙ্গ;
	    MB_PTR_ADV(cts.cts_ptr);
	    যদি (wp->w_p_list)
	    {
		in_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > সাৰি && prev_ptr[-1] == ' '));
		যদি (!in_multispace)
		    multispace_pos = 0;
		অন্যথা যদি (cts.cts_ptr >= সাৰি + leadcol
					 && wp->w_lcs_chars.multispace != রিক্ত)
		{
		    ++multispace_pos;
		    যদি (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		অন্যথা যদি (cts.cts_ptr < সাৰি + leadcol
				     && wp->w_lcs_chars.leadmultispace != রিক্ত)
		{
		    ++multispace_pos;
		    যদি (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	যদি (wlv.vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_col ||
#endif
	     virtual_active() ||
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	যদি (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	যদি (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - head;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	যদি (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	অন্যথা যদি (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ifdef FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	যদি (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	যদি (spv->spv_has_spell)
	{
	    পূর্ণাঙ্ক		len;
	    colnr_T	linecol = (colnr_T)(ptr - সাৰি);
	    hlf_T	spell_hlf = HLF_গুনতি;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = সাৰি + linecol;

	    যদি (len == 0 || (পূর্ণাঙ্ক)wp->w_cursor.col > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		// word
		spell_hlf = HLF_গুনতি;
		word_end = (পূর্ণাঙ্ক)(spell_to_word_end(ptr, wp) - সাৰি + 1);
	    }
	    অন্যথা
	    {
		// bad word found, use attributes until end of word
		word_end = wp->w_cursor.col + len + 1;

		// Turn index into actual attributes.
		যদি (spell_hlf != HLF_গুনতি)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    যদি (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    যদি (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ifdef FEAT_অনুসন্ধান_EXTRA
    যদি (সংখ্যা_only == 0)
    {
	v = (long)(ptr - সাৰি);
	area_highlighting |= prepare_অনুসন্ধান_hl_সাৰি(wp, lnum, (colnr_T)v,
					      &সাৰি, &screen_অনুসন্ধান_hl,
					      &অনুসন্ধান_attr);
	ptr = সাৰি + v; // "line" may have been updated
    }
#endif

    যদি ((State & MODE_ঢোকাও) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
	area_highlighting = TRUE;

#ifdef FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    যদি (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	যদি (!(wp == curwin && VIsual_active)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    যদি (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    অন্যথা
	    {
		সাৰি_attr_save = wlv.সাৰি_attr;
		margin_columns_win(wp, &বাম_curline_col, &ডান_curline_col);
	    }
	    area_highlighting = TRUE;
	}
    }
#endif

    win_সাৰি_start(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    বাবে (;;)
    {
#যদি defined(FEAT_CONCEAL) || defined(FEAT_অনুসন্ধান_EXTRA)
	পূর্ণাঙ্ক	has_match_conc = 0;	// match wants to conceal
#endif
#ifdef FEAT_CONCEAL
	পূর্ণাঙ্ক	did_decrement_ptr = FALSE;
#endif

	// Skip this quickly when working on the text.
	যদি (wlv.draw_state != WL_সাৰি)
	{
#ifdef FEAT_SYN_HL
	    যদি (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.সাৰি_attr = সাৰি_attr_save;
	    }
#endif
	    যদি (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		যদি (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    যদি (wlv.row == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    অন্যথা
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ifdef FEAT_FOLDING
	    যদি (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#endif
#ifdef FEAT_SIGNS
	    যদি (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		যদি (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#endif
	    যদি (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_col(wp, &wlv, sign_present, অঙ্ক_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    যদি (সংখ্যা_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_সাৰি(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		যদি ((wlv.row + 1 - wlv.startrow < সংখ্যা_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ifdef FEAT_DIFF
			|| wlv.filler_todo > 0
#endif
			)
		{
		    ++wlv.row;
		    ++wlv.screen_row;
		    যদি (wlv.row == endrow)
			ভঙ্গ;
#ifdef FEAT_DIFF
		    --wlv.filler_todo;
		    যদি (wlv.filler_todo == 0 && wp->w_botfill)
			ভঙ্গ;
#endif
		    win_সাৰি_start(wp, &wlv, TRUE);
		    অনুৱর্ত;
		}
		অন্যথা
		    ভঙ্গ;
	    }

#ifdef FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    যদি (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#endif
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    যদি (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#endif
	    যদি (wlv.draw_state == WL_সাৰি - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_সাৰি;
		win_সাৰি_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ifdef FEAT_SYN_HL
	যদি (wlv.cul_screenline && wlv.draw_state == WL_সাৰি
		&& wlv.vcol >= বাম_curline_col
		&& wlv.vcol < ডান_curline_col)
	    apply_cursorline_highlight(&wlv, sign_present);
#endif

	// When still displaying '$' of change command, stop at cursor.
	যদি (dollar_vcol >= 0 && in_curline && wlv.vcol >= (long)wp->w_virtcol)
	{
	    wlv_screen_সাৰি(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ifdef FEAT_SYN_HL
	    যদি (wp->w_p_cuc)
		wlv.row = wp->w_cline_row + wp->w_cline_height;
	    অন্যথা
#endif
		wlv.row = wp->w_height;
	    ভঙ্গ;
	}

	যদি (wlv.draw_state == WL_সাৰি && (area_highlighting || extra_check))
	{
#ifdef FEAT_PROP_POPUP
	    যদি (text_props != রিক্ত)
	    {
		পূর্ণাঙ্ক pi;
		পূর্ণাঙ্ক bcol = (পূর্ণাঙ্ক)(ptr - সাৰি);

		যদি (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !in_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		বাবে (pi = 0; pi < text_props_active; ++pi)
		{
		    পূর্ণাঙ্ক		tpi = text_prop_idxs[pi];
		    textprop_T  *tp = &text_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    যদি ((tp->tp_col != MAXCOL
				       && bcol >= tp->tp_col - 1 + tp->tp_len))
		    {
			যদি (pi + 1 < text_props_active)
			    mch_memmove(text_prop_idxs + pi,
					text_prop_idxs + pi + 1,
					পরিমাণ(পূর্ণাঙ্ক)
					     * (text_props_active - (pi + 1)));
			--text_props_active;
			text_props_need_sort = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			যদি (in_linebreak && syntax_attr == text_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		যদি (wlv.n_extra > 0 && in_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		যেতিয়ালৈ (text_prop_next < text_prop_গুনতি)
		{
		    পূর্ণাঙ্ক active;
		    textprop_T *tp = &text_props[text_prop_next];
		    যদি (tp->tp_col == MAXCOL)
		    {
			যদি (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    active = TRUE;
			অন্যথা যদি (*ptr != NUL)
			    ভঙ্গ;
			অন্যথা
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    active = wp->w_p_wrap
				  || wlv.row == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
			}
		    }
		    অন্যথা
		    {
			যদি (bcol < tp->tp_col - 1)
			    ভঙ্গ;
			active = bcol <= tp->tp_col - 1 + tp->tp_len;
		    }

		    যদি (active)
		    {
			text_prop_idxs[text_props_active++] = text_prop_next;
			text_props_need_sort = TRUE;
		    }
		    ++text_prop_next;
		}

		যদি (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(text_prop_type != রিক্ত &&
			     text_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    text_prop_attr = 0;
		    text_prop_attr_comb = 0;
		    text_prop_flags = 0;
		    text_prop_type = রিক্ত;
		    text_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		যদি (text_props_active > 0 && wlv.n_extra == 0)
		{
		    পূর্ণাঙ্ক used_tpi = -1;
		    পূর্ণাঙ্ক used_attr = 0;
		    পূর্ণাঙ্ক other_tpi = -1;

		    text_prop_above = FALSE;
		    text_prop_follows = FALSE;

		    যদি (text_props_need_sort)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			sort_text_props(wp->w_buffer, text_props,
					text_prop_idxs, text_props_active);
			text_props_need_sort = FALSE;
		    }

		    বাবে (pi = 0; pi < text_props_active; ++pi)
		    {
			পূর্ণাঙ্ক	    tpi = text_prop_idxs[pi];
			textprop_T  *tp = &text_props[tpi];
			proptype_T  *pt = text_prop_type_by_id(
						    wp->w_buffer, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			যদি (pt != রিক্ত
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_ডান
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_BELOW)) == 0
				    && wlv.col >= wp->w_width))
			{
			    যদি (tp->tp_col == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == সাৰি
						&& !did_সাৰি
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_BELOW))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				text_prop_follows = TRUE;
				অনুৱর্ত;
			    }

			    যদি (pt->pt_hl_id > 0)
				used_attr = syn_id2attr(pt->pt_hl_id);
			    text_prop_type = pt;
			    text_prop_attr =
				   hl_combine_attr(text_prop_attr, used_attr);
			    যদি (used_tpi >= 0 && text_props[used_tpi].tp_id < 0)
				other_tpi = used_tpi;
			    text_prop_flags = pt->pt_flags;
			    text_prop_id = tp->tp_id;
			    used_tpi = tpi;
			}
		    }
		    যদি (text_prop_id < 0 && used_tpi >= 0
				 && text_props[used_tpi].u.tp_text != রিক্ত)
		    {
			textprop_T  *tp = &text_props[used_tpi];
			char_u	    *p = tp->u.tp_text;
			পূর্ণাঙ্ক	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			পূর্ণাঙ্ক	    bail_out = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			যদি (p != রিক্ত)
			{
			    পূর্ণাঙ্ক	    ডান = (tp->tp_flags
							& TP_FLAG_ALIGN_ডান);
			    পূর্ণাঙ্ক	    below = (tp->tp_flags
							& TP_FLAG_ALIGN_BELOW);
			    পূর্ণাঙ্ক	    wrap = tp->tp_col < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    পূর্ণাঙ্ক	    padding = tp->tp_col == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (পূর্ণাঙ্ক)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.start_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    used_attr);
			    n_attr = mb_charlen(p);
			    text_prop_attr = 0;
			    text_prop_attr_comb = 0;
			    যদি (*ptr == NUL)
				// don't combine char attr after EOL
				text_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    যদি (text_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    যদি ((ডান || above || below || !wrap
					    || padding > 0) && wp->w_width > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				পূর্ণাঙ্ক	start_সাৰি;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				start_সাৰি = text_prop_position(wp, tp,
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
				যদি (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				যদি (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				যদি (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.col
					       + wlv.n_extra - 2 > wp->w_width)
				    // don't bail out at end of line
				    text_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				যদি (start_সাৰি)
				{
				    draw_screen_সাৰি(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    যদি (wlv.row == endrow)
				    {
					++wlv.row;
					ভঙ্গ;
				    }
				    win_সাৰি_start(wp, &wlv, TRUE);
				    bail_out = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			যদি (skip_cells > 0)
			{
			    যদি (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				যদি (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    অন্যথা
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
			       & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ডান)));

			যদি (bail_out)
			    // starting a new line for "below"
			    অনুৱর্ত;
		    }
		}
		অন্যথা যদি (text_prop_next < text_prop_গুনতি
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.col == wp->w_width - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    পূর্ণাঙ্ক only_below_follows = !wp->w_p_wrap
				 && wlv.col + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_width;
		    পূর্ণাঙ্ক suffix_flags = text_prop_suffix_flags[text_prop_next];

		    text_prop_follows = (suffix_flags
					& (only_below_follows ? 2 : 1)) != 0;
		}
	    }

	    যদি (wlv.start_extra_for_textprop)
	    {
		wlv.start_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_অনুসন্ধান_attr = অনুসন্ধান_attr;
		saved_area_attr = area_attr;
		অনুসন্ধান_attr = 0;
		area_attr = 0;
	    }
#endif

	    পূর্ণাঙ্ক *area_attr_p =
#ifdef FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#endif
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    যদি (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != রিক্ত
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    অন্যথা যদি (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    যদি (wlv.n_extra == 0)
	    {
#ifdef FEAT_অনুসন্ধান_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (long)(ptr - সাৰি);
		অনুসন্ধান_attr = update_অনুসন্ধান_hl(wp, lnum, (colnr_T)v, &সাৰি,
				      &screen_অনুসন্ধান_hl, &has_match_conc,
				      &match_conc, did_সাৰি_attr, lcs_eol_one,
				      &on_last_col);
		ptr = সাৰি + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		যদি (*ptr == NUL)
		    has_match_conc = 0;
#অন্যথা
		অনুসন্ধান_attr = 0;
#endif

		// Check if ComplMatchIns highlight is needed.
		যদি ((State & MODE_ঢোকাও) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
		{
		    পূর্ণাঙ্ক ins_match_attr =
			ins_compl_col_range_attr(lnum, (পূর্ণাঙ্ক)(ptr - সাৰি));
		    যদি (ins_match_attr > 0)
			অনুসন্ধান_attr =
			    hl_combine_attr(অনুসন্ধান_attr, ins_match_attr);
		}
	    }

#ifdef FEAT_DIFF
	    যদি (wlv.diff_hlf != (hlf_T)0)
	    {
		যদি (সাৰি_changes.অঙ্ক_changes > 0
			&& change_index >= 0
			&& change_index < সাৰি_changes.অঙ্ক_changes - 1)
		{
		    যদি (ptr - সাৰি >= সাৰি_changes.changes[change_index+1].dc_start[সাৰি_changes.bufidx])
			change_index += 1;
		}
		পূর্ণাঙ্ক added = FALSE;
		যদি (সাৰি_changes.অঙ্ক_changes > 0 && change_index >= 0 && change_index < সাৰি_changes.অঙ্ক_changes)
		{
		    added = diff_change_parse(&সাৰি_changes,
			    &সাৰি_changes.changes[change_index],
			    &change_start,
			    &change_end);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		যদি (wlv.diff_hlf == HLF_CHD && ptr - সাৰি >= change_start
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		যদি ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - সাৰি >= change_end && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.সাৰি_attr = HL_ATTR(wlv.diff_hlf);
		যদি (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= বাম_curline_col
					    && wlv.vcol <= ডান_curline_col)))
		    wlv.সাৰি_attr = hl_combine_attr(
					  wlv.সাৰি_attr, HL_ATTR(HLF_CUL));
	    }
#endif

#ifdef FEAT_SYN_HL
	    যদি (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		যদি (get_term_attr)
		    syntax_attr = term_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		যদি (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (long)(ptr - সাৰি);
		    যদি (v == prev_syntax_col)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    অন্যথা
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    রিক্ত, FALSE);
			prev_syntax_col = v;
			prev_syntax_attr = syntax_attr;
		    }

		    যদি (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    অন্যথা
			did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		    যদি (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = সাৰি + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    যদি (*ptr == NUL)
			syntax_flags = 0;
		    অন্যথা
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    যদি (text_prop_type != রিক্ত)
	    {
		যদি (text_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, text_prop_attr);
		অন্যথা
		    syntax_attr = text_prop_attr;
		text_prop_attr_comb = syntax_attr;
	    }
# endif
#endif

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ifdef সাৰি_ATTR
	    যদি (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.সাৰি_attr, area_attr);
		যদি (!highlight_match)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(অনুসন্ধান_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    অন্যথা যদি (অনুসন্ধান_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.সাৰি_attr, অনুসন্ধান_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    অন্যথা যদি (wlv.সাৰি_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.সাৰি_attr);
# অন্যথা
		wlv.char_attr = wlv.সাৰি_attr;
# endif
		attr_pri = FALSE;
	    }
#অন্যথা
	    যদি (area_attr != 0)
		wlv.char_attr = area_attr;
	    অন্যথা যদি (অনুসন্ধান_attr != 0)
		wlv.char_attr = অনুসন্ধান_attr;
#endif
	    অন্যথা
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#অন্যথা
		wlv.char_attr = 0;
#endif
	    }
#ifdef FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    যদি (text_prop_type != রিক্ত && (text_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, text_prop_attr);
#endif
	}

	// combine attribute with HLF_WIN
	যদি (wlv.win_attr != 0)
	{
	    যদি (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    অন্যথা
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
	যদি (wlv.n_extra > 0)
	{
	    যদি (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		যদি (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		অন্যথা
		    mb_utf8 = FALSE;
	    }
	    অন্যথা
	    {
		c = *wlv.p_extra;
		যদি (has_mbyte)
		{
		    mb_c = c;
		    যদি (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			যদি (mb_l > wlv.n_extra)
			    mb_l = 1;
			অন্যথা যদি (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    অন্যথা
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			যদি (mb_l >= wlv.n_extra)
			    mb_l = 1;
			অন্যথা যদি (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    যদি (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    যদি ((
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
			যদি (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#endif
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    অন্যথা
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#যদি defined(FEAT_PROP_POPUP)
	    যদি (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		যদি (wlv.saved_n_extra <= 0)
		{
		    যদি (অনুসন্ধান_attr == 0)
			অনুসন্ধান_attr = saved_অনুসন্ধান_attr;
		    যদি (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    যদি (wlv.extra_for_textprop)
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
	অন্যথা
	{
#ifdef FEAT_LINEBREAK
	    পূর্ণাঙ্ক		c0;
#endif
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ifdef FEAT_LINEBREAK
	    c0 = *ptr;
#endif
	    যদি (c == NUL)
	    {
#ifdef FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_সাৰি = TRUE;
#endif
		// no more cells to skip
		skip_cells = 0;
	    }

	    যদি (has_mbyte)
	    {
		mb_c = c;
		যদি (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    যদি (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			যদি (mb_c < 0x80)
			{
			    c = mb_c;
#ifdef FEAT_LINEBREAK
			    c0 = mb_c;
#endif
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			যদি (utf_iscomposing(mb_c))
			{
			    পূর্ণাঙ্ক i;

			    বাবে (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    যদি ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ifdef FEAT_RIGHTLEFT
			যদি (wp->w_p_rl)		// reverse
			    rl_mirror(wlv.extra);
#endif
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (পূর্ণাঙ্ক)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			যদি (area_attr == 0 && অনুসন্ধান_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    অন্যথা যদি (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ifdef FEAT_ARABIC
		    অন্যথা যদি (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			পূর্ণাঙ্ক	pc, pc1, nc;
			পূর্ণাঙ্ক	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			যদি (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			অন্যথা
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    অন্যথা
			prev_c = mb_c;
#endif
		}
		অন্যথা	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    যদি (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    অন্যথা যদি (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			যদি (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			অন্যথা
			{
			    যদি (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_buffer, wlv.extra, c);
				wlv.n_extra = (পূর্ণাঙ্ক)STRLEN(wlv.extra) - 1;
			    }
			    অন্যথা
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
			    যদি (area_attr == 0 && অনুসন্ধান_attr == 0)
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
		যদি ((
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
		অন্যথা যদি (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		যদি (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    যদি (area_attr == 0 && অনুসন্ধান_attr == 0)
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

	    যদি (extra_check)
	    {
#ifdef FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (long)(ptr - সাৰি);
		যদি (spv->spv_has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    যদি (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			পূর্ণাঙ্ক	len;
			hlf_T	spell_hlf = HLF_গুনতি;

			যদি (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			যদি ((prev_ptr - সাৰি) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - সাৰি) - nextlinecol;
			অন্যথা
			    p = prev_ptr;
			spv->spv_cap_col -= (পূর্ণাঙ্ক)(prev_ptr - সাৰি);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_col,
							    spv->spv_unchanged);
			word_end = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			যদি (spell_hlf != HLF_গুনতি
				&& (State & MODE_ঢোকাও)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - সাৰি)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_গুনতি;
			    spell_redraw_lnum = lnum;
			}

			যদি (spell_hlf == HLF_গুনতি && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_col = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			যদি (spell_hlf != HLF_গুনতি)
			    spell_attr = highlight_attr[spell_hlf];

			যদি (spv->spv_cap_col > 0)
			{
			    যদি (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_col
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_col = ((p - nextline)
					    + spv->spv_cap_col - nextline_idx);
			    }
			    অন্যথা
				// Compute the actual column.
				spv->spv_cap_col += (prev_ptr - সাৰি);
			}
		    }
		}
		যদি (spell_attr != 0)
		{
		    যদি (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    অন্যথা
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
		যদি ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((পূর্ণাঙ্ক)*ptr))
		    wlv.need_lbr = TRUE;
#endif
#ifdef FEAT_LINEBREAK
		// Found last space before word: check for line break.
		যদি (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((পূর্ণাঙ্ক)*ptr))
		{
		    পূর্ণাঙ্ক	    mb_off = has_mbyte ? (*mb_head_off)(সাৰি, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_first_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, সাৰি, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_text = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, রিক্ত, রিক্ত) - 1;
		    clear_chartabsize_arg(&cts);

		    যদি (on_last_col && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			অনুসন্ধান_attr = 0;

		    যদি (c == TAB && wlv.n_extra + wlv.col > wp->w_width)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
# অন্যথা
			wlv.n_extra = (পূর্ণাঙ্ক)wp->w_buffer->b_p_ts
				    - wlv.vcol % (পূর্ণাঙ্ক)wp->w_buffer->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    যদি (wlv.n_extra > 0 && c != TAB)
			in_linebreak = TRUE;
# endif
		    যদি (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			যদি (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			যদি (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif
		যদি (wp->w_p_list)
		{
		    in_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > সাৰি && prev_ptr[-1] == ' '));
		    যদি (!in_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		যদি (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (in_multispace
					&& wp->w_lcs_chars.multispace != রিক্ত))
				&& ptr - সাৰি >= leadcol
				&& ptr - সাৰি <= trailcol)))
		{
		    যদি (in_multispace && wp->w_lcs_chars.multispace != রিক্ত)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			যদি (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    অন্যথা
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    যদি (area_attr == 0 && অনুসন্ধান_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    যদি (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    অন্যথা
			mb_utf8 = FALSE;
		}

		যদি (c == ' ' && ((trailcol != MAXCOL && ptr > সাৰি + trailcol)
				    || (leadcol != 0 && ptr < সাৰি + leadcol)))
		{
		    যদি (leadcol != 0 && in_multispace && ptr < সাৰি + leadcol
			    && wp->w_lcs_chars.leadmultispace != রিক্ত)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			যদি (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    অন্যথা যদি (ptr > সাৰি + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    অন্যথা যদি (ptr < সাৰি + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    অন্যথা যদি (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    যদি (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    যদি (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    অন্যথা
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    যদি (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		যদি (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    পূর্ণাঙ্ক	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    long    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    পূর্ণাঙ্ক	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    পূর্ণাঙ্ক	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    পূর্ণাঙ্ক	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    যদি (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < সাৰি + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ifdef FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_মান(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    যদি (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#endif
		    // tab amount depends on current column
#ifdef FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
#অন্যথা
		    tab_len = (পূর্ণাঙ্ক)wp->w_buffer->b_p_ts
			       - vcol_adjusted % (পূর্ণাঙ্ক)wp->w_buffer->b_p_ts - 1;
#endif

#ifdef FEAT_LINEBREAK
		    যদি (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ifdef FEAT_LINEBREAK
		    অন্যথা
		    {
			char_u	*p;
			পূর্ণাঙ্ক	len;
			পূর্ণাঙ্ক	i;
			পূর্ণাঙ্ক	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			যদি (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			যদি (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			যদি (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    পূর্ণাঙ্ক tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    যদি (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    যদি (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    যদি (p == রিক্ত)
				wlv.n_extra = 0;
			    অন্যথা
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				বাবে (i = 0; i < tab_len; i++)
				{
				    পূর্ণাঙ্ক lcs = lcs_tab2;

				    যদি (*p == NUL)
				    {
					tab_len = i;
					ভঙ্গ;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    যদি (lcs_tab3 && i == tab_len - 1)
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
				যদি (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			পূর্ণাঙ্ক vc_saved = wlv.vcol_off_co;

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
			যদি (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#endif
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    যদি (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ifdef FEAT_LINEBREAK
			যদি (wp->w_p_lbr && wlv.p_extra != রিক্ত
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			অন্যথা
#endif
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			যদি (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    অন্যথা
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		অন্যথা যদি (c == NUL
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
#যদি defined(FEAT_DIFF) || defined(সাৰি_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    যদি (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef সাৰি_ATTR
			    &&
#  endif
# endif
# ifdef সাৰি_ATTR
			    wlv.সাৰি_attr == 0
# endif
		       )
#endif
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			যদি (!(area_highlighting && virtual_active()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    যদি (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    অন্যথা
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    যদি (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    যদি (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    অন্যথা
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		অন্যথা যদি (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_buffer, c);
		    যদি (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    যদি ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#endif
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ifdef FEAT_LINEBREAK
		    যদি (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			যদি (p == রিক্ত)
			    wlv.n_extra = 0;
			অন্যথা
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    অন্যথা
#endif
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    যদি (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ifdef FEAT_PROP_POPUP
			যদি (text_prop_type != রিক্ত &&
				 text_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(text_prop_attr,
							       wlv.extra_attr);
#endif

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		অন্যথা যদি (VIsual_active
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
#যদি defined(সাৰি_ATTR)
		অন্যথা যদি ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.সাৰি_attr != 0
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
		    ++did_সাৰি_attr;

		    // don't do search HL for the rest of the line
		    যদি (wlv.সাৰি_attr != 0 && wlv.char_attr == অনুসন্ধান_attr
					&& (did_সাৰি_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.সাৰি_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    যদি (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_সাৰি
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    যদি (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			যদি (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    যদি (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= বাম_curline_col
					    && wlv.vcol <= ডান_curline_col)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    যদি (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			যদি (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    যদি (!wlv.cul_screenline
				    || (wlv.vcol >= বাম_curline_col
					     && wlv.vcol <= ডান_curline_col))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			অন্যথা যদি (wlv.সাৰি_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.সাৰি_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			যদি (wlv.vcol == 0)
			{
			    যদি (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    অন্যথা যদি (অনুসন্ধান_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, অনুসন্ধান_attr);
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    যদি (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_সাৰি(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_match_conc > 0)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == রিক্ত))
	    {
		পূর্ণাঙ্ক syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		যদি (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_match_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_match_conc && match_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    যদি (has_match_conc && match_conc)
			c = match_conc;
		    অন্যথা যদি (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    অন্যথা যদি (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    অন্যথা
			c = ' ';

		    যদি (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    যদি (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    অন্যথা
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    যদি (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    যদি (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			যদি (wp->w_p_rl)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			অন্যথা
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.col += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		অন্যথা যদি (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    অন্যথা
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    যদি (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#endif // FEAT_CONCEAL
	}

#ifdef FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	যদি (!did_wcol && wlv.draw_state == WL_সাৰি
		&& in_curline && conceal_cursor_সাৰি(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
		wp->w_wcol = wp->w_width - wlv.col + wlv.boguscols - 1;
	    অন্যথা
# endif
		wp->w_wcol = wlv.col - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    যদি (wlv.vcol + skip_cells < wp->w_virtcol)
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
	যদি (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_সাৰি
		&& (!attr_pri
#ifdef FEAT_PROP_POPUP
		    || (text_prop_flags & PT_FLAG_OVERRIDE)
#endif
		   ))
	{
#ifdef সাৰি_ATTR
	    যদি (wlv.সাৰি_attr)
		wlv.char_attr = hl_combine_attr(wlv.সাৰি_attr, wlv.extra_attr);
	    অন্যথা
#endif
		wlv.char_attr = wlv.extra_attr;
#ifdef FEAT_PROP_POPUP
	    যদি (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#endif
	}

#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	যদি (p_imst == IM_ON_THE_SPOT
		&& xic != রিক্ত
		&& lnum == wp->w_cursor.lnum
		&& (State & MODE_ঢোকাও)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_সাৰি)
	{
	    colnr_T tcol;

	    যদি (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, রিক্ত, রিক্ত, 0);
	    অন্যথা
		tcol = preedit_end_col;
	    যদি ((long)preedit_start_col <= wlv.vcol && wlv.vcol < (long)tcol)
	    {
		যদি (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_col);
		যদি (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    অন্যথা যদি (feedback_old_attr >= 0)
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
	যদি (lcs_prec_todo != NUL
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
	    যদি (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    যদি (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    অন্যথা
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    যদি (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	যদি ((c == NUL
#যদি defined(সাৰি_ATTR)
		|| did_সাৰি_attr == 1
#endif
		) && wlv.eol_hl_off == 0)
	{
#ifdef FEAT_অনুসন্ধান_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    পূর্ণাঙ্ক prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_অনুসন্ধান_hl,
					      (long)(ptr - সাৰি) - (c == NUL));
#endif
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    যদি (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_অনুসন্ধান_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_active))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# যদি defined(সাৰি_ATTR)
			    && did_সাৰি_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		পূর্ণাঙ্ক n = 0;

#ifdef FEAT_RIGHTLEFT
		যদি (wp->w_p_rl)
		{
		    যদি (wlv.col < 0)
			n = 1;
		}
		অন্যথা
#endif
		{
		    যদি (wlv.col >= wp->w_width)
			n = -1;
		}
		যদি (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.col += n;
		}
		অন্যথা
		{
		    // Add a blank character to highlight.
		    Screen_সাৰিগুলি[wlv.off] = ' ';
		    যদি (enc_utf8)
			Screen_সাৰিগুলি_UC[wlv.off] = 0;
		}
#ifdef FEAT_অনুসন্ধান_EXTRA
		যদি (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_অনুসন্ধান_match_hl(wp, &screen_অনুসন্ধান_hl,
					   (long)(ptr - সাৰি), &wlv.char_attr);
		}
#endif
		ScreenAttrs[wlv.off] = wlv.char_attr;
		ScreenCols[wlv.off] = wlv.vcol;
#ifdef FEAT_RIGHTLEFT
		যদি (wp->w_p_rl)
		{
		    --wlv.col;
		    --wlv.off;
		}
		অন্যথা
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
	যদি (c == NUL)
	{
#ifdef FEAT_PROP_POPUP
	    যদি (text_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    অন্যথা
#endif
	    {
		draw_screen_সাৰি(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		যদি (in_curline)
		{
		    curwin->w_cline_row = startrow;
		    curwin->w_cline_height = wlv.row - startrow;
#ifdef FEAT_FOLDING
		    curwin->w_cline_folded = false;
#endif
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		ভঙ্গ;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	পূর্ণাঙ্ক lcs_ext = get_lcs_ext(wp);
	যদি (lcs_ext != NUL
		&& wlv.draw_state == WL_সাৰি
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
	    যদি (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    অন্যথা
		mb_utf8 = FALSE;
	}

#ifdef FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	যদি (wlv.draw_color_col)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	যদি (((wlv.draw_state == WL_সাৰি
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_in_visual_area
		&& অনুসন্ধান_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    যদি (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    অন্যথা যদি (wlv.draw_color_col && VCOL_HLC == *wlv.color_cols)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#endif

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	যদি (wlv.draw_state < WL_সাৰি || skip_cells <= 0)
	{
	    // Store the character.
#যদি defined(FEAT_RIGHTLEFT)
	    যদি (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.col;
	    }
#endif
	    Screen_সাৰিগুলি[wlv.off] = c;
	    যদি (enc_dbcs == DBCS_JPNU)
	    {
		যদি ((mb_c & 0xff00) == 0x8e00)
		    Screen_সাৰিগুলি[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    অন্যথা যদি (enc_utf8)
	    {
		যদি (mb_utf8)
		{
		    পূর্ণাঙ্ক i;

		    Screen_সাৰিগুলি_UC[wlv.off] = mb_c;
		    যদি ((c & 0xff) == 0)
			Screen_সাৰিগুলি[wlv.off] = 0x80;   // avoid storing zero
		    বাবে (i = 0; i < Screen_mco; ++i)
		    {
			Screen_সাৰিগুলি_C[i][wlv.off] = u8cc[i];
			যদি (u8cc[i] == 0)
			    ভঙ্গ;
		    }
		}
		অন্যথা
		    Screen_সাৰিগুলি_UC[wlv.off] = 0;
	    }
	    যদি (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    অন্যথা
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    যদি (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
		ScreenCols[wlv.off] = wlv.vcol;
	    অন্যথা
		ScreenCols[wlv.off] = -1;

	    যদি (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.col;
		যদি (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_সাৰিগুলি[wlv.off] = 0;
		অন্যথা
		    // DBCS: Put second byte in the second screen char.
		    Screen_সাৰিগুলি[wlv.off] = mb_c & 0xff;

		যদি (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
			)
		    ScreenCols[wlv.off] = ++wlv.vcol;
		অন্যথা
		    ScreenCols[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		যদি (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ifdef FEAT_RIGHTLEFT
		যদি (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.col;
		}
#endif
	    }
#ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.col;
	    }
	    অন্যথা
#endif
	    {
		++wlv.off;
		++wlv.col;
	    }
	}
#ifdef FEAT_CONCEAL
	অন্যথা যদি (wp->w_p_cole > 0 && is_concealing)
	{
	    পূর্ণাঙ্ক concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    যদি (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    যদি (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    যদি (wp->w_p_wrap)
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
		পূর্ণাঙ্ক adjust_col = !WIN_IS_POPUP(wp);
# অন্যথা
		পূর্ণাঙ্ক adjust_col = TRUE;
# endif
		যদি (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    যদি (wp->w_p_rl)
		    {
			যদি (adjust_col)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    অন্যথা
# endif
		    {
			যদি (adjust_col)
			{
			    wlv.col += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		যদি (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    যদি (wp->w_p_rl)
		    {
			যদি (adjust_col)
			{
			    --wlv.boguscols;
			    --wlv.col;
			}
		    }
		    অন্যথা
# endif
		    {
			যদি (adjust_col)
			{
			    ++wlv.boguscols;
			    ++wlv.col;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		যদি (wp->w_p_rl)
		{
		    যদি (adjust_col)
		    {
			--wlv.boguscols;
			--wlv.col;
		    }
		}
		অন্যথা
# endif
		{
		    যদি (adjust_col)
		    {
			++wlv.boguscols;
			++wlv.col;
		    }
		}
	    }
	    অন্যথা
	    {
		যদি (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif // FEAT_CONCEAL
	অন্যথা
	    --skip_cells;

	যদি (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	যদি (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		)
	    ++wlv.vcol;

#ifdef FEAT_SYN_HL
	যদি (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#endif

	// restore attributes after "precedes" in 'listchars'
	যদি (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	যদি (n_attr > 0 && wlv.draw_state == WL_সাৰি
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	যদি (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	যদি ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.col < 0) :
#endif
				    (wlv.col >= wp->w_width))
		&& (wlv.draw_state != WL_সাৰি
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
	    যদি (wlv.boguscols != 0 && (
# ifdef সাৰি_ATTR
			wlv.সাৰি_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		পূর্ণাঙ্ক attr = wlv.win_attr;
# ifdef সাৰি_ATTR
		যদি (wlv.সাৰি_attr != 0)
		    attr = hl_combine_attr(attr, wlv.সাৰি_attr);
# endif
		যেতিয়ালৈ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.col >= 0 :
# endif
			    wlv.col < wp->w_width))
		{
		    Screen_সাৰিগুলি[wlv.off] = ' ';
		    যদি (enc_utf8)
			Screen_সাৰিগুলি_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    ScreenCols[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    যদি (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.col--;
			wlv.boguscols++;
		    }
		    অন্যথা
# endif
		    {
			wlv.off++;
			wlv.col++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_সাৰি(wp, &wlv, TRUE);
	    wlv.col += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#অন্যথা
	    wlv_screen_সাৰি(wp, &wlv, TRUE);
#endif
	    ++wlv.row;
	    ++wlv.screen_row;

	    // When not wrapping and finished diff lines, break here.
	    যদি (!wp->w_p_wrap
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
			&& !text_prop_above
			&& !text_prop_follows
#endif
		       )
		ভঙ্গ;
#ifdef FEAT_PROP_POPUP
	    যদি (!wp->w_p_wrap && text_prop_follows && !text_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = সাৰি + (size_t)ml_get_buf_len(wp->w_buffer, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#endif

	    // When the window is too narrow draw all "@" lines.
	    যদি (wlv.draw_state != WL_সাৰি
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
	    যদি (wlv.row == endrow)
	    {
		++wlv.row;
		ভঙ্গ;
	    }

	    যদি (screen_cur_row == wlv.screen_row - 1
#ifdef FEAT_DIFF
		     && wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
		     && !text_prop_above && !text_prop_follows
#endif
		     && wp->w_width == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		সাৰি_Wraps[wlv.screen_row - 1] = TRUE;

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
		যদি (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
			 && !(has_mbyte
			     && ((*mb_off2cells)(সাৰি_Offset[wlv.screen_row],
				   সাৰি_Offset[wlv.screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     সাৰি_Offset[wlv.screen_row - 1]
							    + (পূর্ণাঙ্ক)topframe->fr_width - 2,
				     সাৰি_Offset[wlv.screen_row]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    যদি (screen_cur_col != wp->w_width)
			screen_char(সাৰি_Offset[wlv.screen_row - 1]
						       + (unsigned)topframe->fr_width - 1,
				       wlv.screen_row - 1, (পূর্ণাঙ্ক)(topframe->fr_width - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    যদি (has_mbyte && MB_BYTE2LEN(Screen_সাৰিগুলি[সাৰি_Offset[
				     wlv.screen_row - 1] + (topframe->fr_width - 1)]) > 1)
			out_char(' ');
		    অন্যথা
			out_char(Screen_সাৰিগুলি[সাৰি_Offset[wlv.screen_row - 1]
							    + (topframe->fr_width - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[সাৰি_Offset[wlv.screen_row]] = (sattr_T)-1;
		    screen_start();	// don't know where cursor is now
		}
	    }

	    win_সাৰি_start(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ifdef FEAT_LINEBREAK
	    যদি (!wlv.dont_use_showbreak
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
	    যদি (wlv.filler_todo == 0 && wp->w_botfill)
		ভঙ্গ;
#endif
	}

    }	// for every character in the line
#ifdef FEAT_PROP_POPUP
    যদি (text_props != text_props_buf)
	vim_free(text_props);
    যদি (text_prop_idxs != text_prop_idxs_buf)
	vim_free(text_prop_idxs);
    যদি (text_prop_suffix_flags != text_prop_suffix_flags_buf)
	vim_free(text_prop_suffix_flags);
    vim_free(p_extra_free2);
#endif

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    ঘূৰাই wlv.row;
}
