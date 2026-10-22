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

#شامل "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    ساکن صحیح
get_lcs_ext(win_T *wp)
{
    اگر (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	واپس NUL;
    اگر (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	واپس '>';
    واپس wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#اگر_تعریف_شدہ FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    ساکن صحیح
advance_color_ستون(صحیح vcol, صحیح **color_ستون_تعداد)
{
    جب (**color_ستون_تعداد >= 0 && vcol > **color_ستون_تعداد)
	++*color_ستون_تعداد;
    واپس (**color_ستون_تعداد >= 0);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    ساکن خالی
margin_columns_win(win_T *wp, صحیح *بایاں_ستون, صحیح *دایاں_ستون)
{
    // cache previous calculations depending on w_virtcol
    ساکن صحیح saved_w_virtcol;
    ساکن win_T *پچھلا_wp;
    ساکن صحیح پچھلا_width1;
    ساکن صحیح پچھلا_width2;
    ساکن صحیح پچھلا_بایاں_ستون;
    ساکن صحیح پچھلا_دایاں_ستون;

    صحیح cur_ستون_off = win_ستون_off(wp);
    صحیح	width1;
    صحیح	width2;

    width1 = wp->w_چوڑائی - cur_ستون_off;
    width2 = width1 + win_ستون_off2(wp);

    اگر (saved_w_virtcol == wp->w_virtcol && پچھلا_wp == wp
	    && پچھلا_width1 == width1 && پچھلا_width2 == width2)
    {
	*دایاں_ستون = پچھلا_دایاں_ستون;
	*بایاں_ستون = پچھلا_بایاں_ستون;
	واپس;
    }

    *بایاں_ستون = 0;
    *دایاں_ستون = width1;

    اگر (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*دایاں_ستون = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    اگر (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*بایاں_ستون = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    پچھلا_بایاں_ستون = *بایاں_ستون;
    پچھلا_دایاں_ستون = *دایاں_ستون;
    پچھلا_wp = wp;
    پچھلا_width1 = width1;
    پچھلا_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#اگر_ختم

#اگر defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define سطر_ATTR
#اگر_ختم

// structure with variables passed between win_line() and other functions
قسمتعریف ساخت {
    صحیح		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    صحیح		startrow;	// first row in the window to be drawn
    صحیح		صف;		// row in the window, excl w_winrow
    صحیح		screen_صف;	// row on the screen, incl w_winrow

    طویل	vcol;		// virtual column, before wrapping
    صحیح		ستون;		// visual column on screen, after wrapping
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    صحیح		vcol_off_co;	// offset for concealed characters
#اگر_ختم
    صحیح		vcol_off_tp;	// offset for virtual text
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح		draw_color_ستون;	// highlight colorcolumn
    صحیح		*color_ستون_تعداد;	// pointer to according columns array
#اگر_ختم
    صحیح		eol_hl_off;	// 1 if highlighted char after EOL

    غیرعلامتی	off;		// offset in ScreenLines/ScreenAttrs

    صحیح		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح		cul_attr;	// set when 'cursorline' active
#اگر_ختم
#اگر_تعریف_شدہ سطر_ATTR
    صحیح		سطر_attr;	// for the whole line, includes 'cursorline'
#اگر_ختم

    صحیح		screen_سطر_flags;  // flags for screen_line()

    صحیح		fromcol;	// start of inverting
    صحیح		tocol;		// end of inverting

#اگر_تعریف_شدہ FEAT_LINEBREAK
    طویل	vcol_sbr;	    // virtual column after showbreak
    صحیح		need_showbreak;	    // overlong line, skipping first x chars
    صحیح		dont_use_showbreak; // do not use 'showbreak'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    صحیح		متن_prop_above_گنتی;
#اگر_ختم

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    صحیح		cul_screenline;

    صحیح		char_attr;	// attributes for the next character

    صحیح		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    صحیح		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    صحیح		n_attr_skip;    // chars to skip before using extra_attr
    صحیح		c_extra;	// extra chars, all the same
    صحیح		c_final;	// final char, mandatory if set
    صحیح		extra_for_textprop; // n_extra set for textprop
    صحیح		آغاز_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    صحیح		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    صحیح		saved_extra_attr;
    صحیح		saved_n_attr_skip;
    صحیح		saved_extra_for_textprop;
    صحیح		saved_c_extra;
    صحیح		saved_c_final;
    صحیح		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#اگر_تعریف_شدہ FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#اگر_ختم
    صحیح		filler_سطریں;	// nr of filler lines to be drawn
    صحیح		filler_todo;	// nr of filler lines still to do + 1
#اگر_تعریف_شدہ FEAT_SIGNS
    sign_attrs_T sattr;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    صحیح		need_lbr;
#اگر_ختم
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#تعریف_کرو WL_آغاز	0		// nothing done yet, must be zero
#تعریف_کرو WL_CMDLINE	(WL_آغاز + 1)	// cmdline window column
#اگر_تعریف_شدہ FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#ورنہ
# define WL_FOLD	WL_CMDLINE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#ورنہ
# define WL_SIGN	WL_FOLD		// column for signs
#اگر_ختم
#تعریف_کرو WL_NR		(WL_SIGN + 1)	// line number
#اگر_تعریف_شدہ FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#ورنہ
# define WL_BRI		WL_NR
#اگر_ختم
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#ورنہ
# define WL_SBR		WL_BRI
#اگر_ختم
#تعریف_کرو WL_سطر		(WL_SBR + 1)	// text in the line

#اگر defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    ساکن صحیح
use_cursor_سطر_highlight(win_T *wp, linenr_T lnum)
{
    واپس wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#اگر_ختم


#اگر_تعریف_شدہ FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    ساکن خالی
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    صحیح fdc = compute_foldcolumn(wp, 0);

    اگر (fdc <= 0)
	واپس;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    اگر (wlv->p_extra_free == تہی)
	واپس;

    wlv->n_extra = (صحیح)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    اگر (use_cursor_سطر_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    ورنہ
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_FC));
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SIGNS
/*
 * Get information needed to display the sign in line "wlv->lnum" in window
 * "wp".
 * If "nrcol" is TRUE, the sign is going to be displayed in the number column.
 * Otherwise the sign is going to be displayed in the sign column.
 */
    ساکن خالی
get_sign_display_info(
	صحیح		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    صحیح	متن_sign;
# ifdef FEAT_SIGN_ICONS
    صحیح	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    اگر (nrcol)
	wlv->n_extra = نمبر_چوڑائی(wp) + 1;
    ورنہ
    {
	اگر (use_cursor_سطر_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	ورنہ
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    اگر (wlv->صف == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_سطریں && wlv->filler_todo <= 0
# endif
       )
    {
	متن_sign = (wlv->sattr.sat_متن != تہی) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != تہی) ? wlv->sattr.sat_typenr : 0;
	اگر (gui.اندر_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    اگر (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((حرف *)wlv->extra, حجم(wlv->extra),
						"%-*c ", نمبر_چوڑائی(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    ورنہ
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    اگر (netbeans_فعال() && (buf_signcount(wp->w_بفر, wlv->lnum)
									  > 1))
	    {
		اگر (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((حرف *)wlv->extra, حجم(wlv->extra),
						"%-*c ", نمبر_چوڑائی(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		ورنہ
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	ورنہ
# endif
	    اگر (متن_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_متن;
		اگر (wlv->p_extra != تہی)
		{
		    اگر (nrcol)
		    {
			صحیح چوڑائی = نمبر_چوڑائی(wp) - 2;

			vim_memset(wlv->extra, ' ', چوڑائی);
			wlv->n_extra = چوڑائی;
			wlv->n_extra += vim_snprintf((حرف *)wlv->extra + چوڑائی,
				  حجم(wlv->extra) - چوڑائی, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    ورنہ
			wlv->n_extra = (صحیح)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		اگر (use_cursor_سطر_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		ورنہ
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#اگر_ختم

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    ساکن خالی
handle_lnum_ستون(
	win_T		*wp,
	winlinevars_T	*wlv,
	صحیح		sign_present غیر_مستعمل,
	صحیح		عدد_attr غیر_مستعمل)
{
    صحیح has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != تہی;
    صحیح lnum_صف = wlv->startrow + wlv->filler_سطریں
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		      + wlv->متن_prop_above_گنتی
#اگر_ختم
		      ;

    اگر ((wp->w_p_nu || wp->w_p_rnu)
	     && (wlv->صف <= lnum_صف || !has_cpo_n)
	     // there is no line number in a wrapped line when "n" is in
	     // 'cpoptions', but 'breakindent' assumes it anyway.
	     && !((has_cpo_n
#اگر_تعریف_شدہ FEAT_LINEBREAK
		     && !wp->w_p_bri
#اگر_ختم
		  ) && wp->w_skipcol > 0 && wlv->lnum == wp->w_topline))
    {
#اگر_تعریف_شدہ FEAT_SIGNS
	// If 'signcolumn' is set to 'number' and a sign is present
	// in 'lnum', then display the sign instead of the line
	// number.
	اگر ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_متن != تہی)
	    get_sign_display_info(TRUE, wp, wlv);
	ورنہ
#اگر_ختم
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  اگر (wlv->صف == lnum_صف
		    && (wp->w_skipcol == 0 || wlv->صف > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      طویل عدد;
	      حرف *fmt = "%*ld ";

	      اگر (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  عدد = (طویل)wlv->lnum;
	      ورنہ
	      {
		  // 'relativenumber', don't use negative numbers
		  عدد = labs((طویل)get_cursor_rel_lnum(wp, wlv->lnum));
		  اگر (عدد == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      عدد = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((حرف *)wlv->extra, حجم(wlv->extra), fmt, نمبر_چوڑائی(wp), عدد);
	      اگر (wp->w_skipcol > 0 && wlv->startrow == 0)
		  برائے (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	      اگر (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  صحیح	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  برائے (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
		  {
		      t = *p1;
		      *p1 = *p2;
		      *p2 = t;
		  }
	      }
#اگر_ختم
	      wlv->p_extra = wlv->extra;
	      wlv->c_extra = NUL;
	      wlv->c_final = NUL;
	  }
	  ورنہ
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = نمبر_چوڑائی(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#اگر_تعریف_شدہ FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  اگر (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->صف == lnum_صف
		      || (wlv->صف > lnum_صف
			 && (wp->w_p_culopt_flags & CULOPT_سطر))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#اگر_ختم
	  اگر (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  اگر (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#اگر_تعریف_شدہ FEAT_SIGNS
	اگر (عدد_attr)
	    wlv->char_attr = عدد_attr;
#اگر_ختم
    }
}

#اگر_تعریف_شدہ FEAT_LINEBREAK
    ساکن خالی
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    اگر (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_قدر(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    ورنہ اگر (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    اگر (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	اگر (wp->w_p_bri && (wlv->صف > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_سطریں
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    اگر (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = تہی;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_بفر, wlv->lnum, FALSE));
	    اگر (wlv->صف == wlv->startrow)
	    {
		wlv->n_extra -= win_ستون_off2(wp);
		اگر (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    اگر (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    اگر (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	اگر (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#اگر_ختم

#اگر defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    ساکن خالی
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    اگر (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	اگر (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	ورنہ
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	اگر (wp->w_p_rl)
	    wlv->n_extra = wlv->ستون + 1;
	ورنہ
#  endif
	    wlv->n_extra = wp->w_چوڑائی - wlv->ستون;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_قدر(wp);
    اگر (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (صحیح)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	اگر (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	اگر (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	اگر (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    اگر (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#اگر_ختم

#اگر defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    ساکن صحیح
textprop_سائز_after_trunc(
	win_T	*wp,
	صحیح	flags,	    // TP_FLAG_ALIGN_*
	صحیح	added,
	صحیح	padding,
	char_u	*متن,
	صحیح	*n_مستعمل_ptr)
{
    صحیح	space = (flags & (TP_FLAG_ALIGN_نیچے | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_چوڑائی - win_ستون_off(wp) : added;
    صحیح strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    اگر (space < PROP_متن_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_چوڑائی;
    اگر (flags & (TP_FLAG_ALIGN_نیچے | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    برائے (p = متن; *p != NUL; p += (*mb_ptr2len)(p))
    {
	صحیح clen = ptr2cells(p);

	اگر (strsize + clen > space)
	    توڑو;
	strsize += clen;
    }
    *n_مستعمل_ptr = (صحیح)(p - متن);

    واپس strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    صحیح
متن_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	صحیح	    vcol,	    // current text column
	صحیح	    scr_ستون,	    // current screen column
	صحیح	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	صحیح	    *n_attr,	    // attribute cells, NULL if not used
	صحیح	    *n_attr_skip,   // cells to skip attr, NULL if not used
	صحیح	    do_skip)	    // skip_cells is not zero
{
    صحیح	    دایاں = (tp->tp_flags & TP_FLAG_ALIGN_دایاں);
    صحیح	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    صحیح	    نیچے = (tp->tp_flags & TP_FLAG_ALIGN_نیچے);
    صحیح	    wrap = tp->tp_ستون < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    صحیح	    padding = tp->tp_ستون == MAXCOL ? tp->tp_padleft : 0;
    صحیح	    ستون_with_padding = scr_ستون + (نیچے ? 0 : padding);
    صحیح	    گنجائش = wp->w_چوڑائی - ستون_with_padding;
    صحیح	    before = گنجائش;	// spaces before the text
    صحیح	    after = 0;		// spaces after the text
    صحیح	    n_مستعمل = *n_extra;
    char_u  *l = تہی;
    صحیح	    strsize = vim_strsize(*p_extra);
    صحیح	    cells = wrap ? strsize : textprop_سائز_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_مستعمل);

    اگر (wrap || دایاں || above || نیچے || padding > 0 || n_مستعمل < *n_extra)
    {
	صحیح	    ستون_off = win_ستون_off(wp) - win_ستون_off2(wp);

	اگر (above)
	{
	    before = 0;
	    after = wp->w_چوڑائی - cells - win_ستون_off(wp) - padding;
	    اگر (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	ورنہ
	{
	    // Right-align: fill with before
	    اگر (دایاں)
		before -= cells;

	    // Below-align: empty line add one character
	    اگر (نیچے && vcol == 0 && ستون_with_padding == ستون_off
					    && wp->w_چوڑائی - ستون_off == before)
		ستون_with_padding += 1;

	    اگر (before < 0
		    || !(دایاں || نیچے)
		    || (نیچے ? (ستون_with_padding <= ستون_off || !wp->w_p_wrap)
			      : (n_مستعمل < *n_extra)))
	    {
		اگر (دایاں && (wrap
			      || (گنجائش < PROP_متن_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_چوڑائی - ستون_off - strsize + گنجائش;
		    اگر (before < 0)
			before = 0;
		    ورنہ
			n_مستعمل = *n_extra;
		}
		ورنہ اگر (نیچے && before > vcol && do_skip)
		    before -= vcol;
		ورنہ
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	اگر (n_مستعمل < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_مستعمل;

	// add 1 for NUL, 2 for when '…' is used
	اگر (n_attr != تہی)
	    l = alloc(n_مستعمل + before + after + (padding > 0 ? padding : 0) + 3);
	اگر (n_attr == تہی || l != تہی)
	{
	    صحیح off = 0;

	    اگر (n_attr != تہی)
	    {
		vim_memset(l, ' ', before);
		off += before;
		اگر (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_مستعمل);
		off += n_مستعمل;
	    }
	    ورنہ
	    {
		off = before + after + padding + n_مستعمل;
		cells += before + after + padding;
	    }
	    اگر (n_attr != تہی)
	    {
		اگر (n_مستعمل < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    اگر (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			اگر (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_تیاری(&vc, (char_u *)"utf-8", p_enc);
			    اگر (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, تہی);
				اگر (cp == تہی)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_تیاری(&vc, تہی, تہی);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_سر_off)(l, lp);
			STRCPY(lp, cp);
			n_مستعمل = lp - l + 3 - before - padding;
			اگر (cp != buf)
			    vim_free(cp);
		    }
		    ورنہ
			// change last character to '>'
			*lp = '>';
		}
		ورنہ اگر (after > 0)
		{
		    vim_memset(l + off, ' ', after);
		    l[off + after] = NUL;
		}

		*p_extra = l;
		*n_extra = n_مستعمل + before + after + padding;
		*n_attr = mb_charlen(*p_extra);
		// n_attr_skip will not be decremented before draw_state is
		// WL_LINE
		*n_attr_skip = before + (padding > 0 ? padding : 0);
		*n_attr -= *n_attr_skip;
		اگر (above)
		    *n_attr -= after;
	    }
	}
    }

    اگر (n_attr == تہی)
	واپس cells;
    واپس (نیچے && ستون_with_padding > win_ستون_off(wp) && !wp->w_p_wrap);
}

# اگر defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    صحیح
متن_prop_no_showbreak(textprop_T *tp)
{
    صحیح	    دایاں = (tp->tp_flags & TP_FLAG_ALIGN_دایاں);
    صحیح	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    صحیح	    نیچے = (tp->tp_flags & TP_FLAG_ALIGN_نیچے);
    صحیح	    wrap = tp->tp_ستون < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    واپس (دایاں || above || نیچے || !wrap);
}
# endif
#اگر_ختم

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    ساکن خالی
wlv_screen_سطر(win_T *wp, winlinevars_T *wlv, صحیح clear_انتہا)
{
    اگر (wlv->صف == 0 && wp->w_skipcol > 0
#اگر defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_قدر(wp) == NUL
#اگر_ختم
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	صحیح off = (صحیح)(current_Screen_سطر - Screen_سطریں);
	صحیح max_off = off + screen_Columns;
	صحیح skip = 0;

	اگر (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    جب (skip < wp->w_چوڑائی && VIM_ISDIGIT(Screen_سطریں[off]))
	    {
		++off;
		++skip;
	    }

	برائے (صحیح i = 0; i < 3 && i + skip < wp->w_چوڑائی; ++i)
	{
	    اگر ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_سطریں[off + 1] = ' ';
	    Screen_سطریں[off] = '<';
	    اگر (enc_utf8)
		Screen_سطریں_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_سطر(wp, wlv->screen_صف, wp->w_wincol, wlv->ستون,
		    clear_انتہا ? wp->w_چوڑائی : -wp->w_چوڑائی,
		    wlv->vcol - 1, wlv->screen_سطر_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    ساکن خالی
draw_screen_سطر(win_T *wp, winlinevars_T *wlv)
{
#اگر_تعریف_شدہ FEAT_SYN_HL
    طویل	v;
    صحیح		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    اگر (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    ورنہ
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_چوڑائی - wlv->ستون - 1 :
# endif
	wlv->ستون;
    // check if line ends before left margin
    اگر (wlv->vcol < v + wcol - win_ستون_off(wp))
	wlv->vcol = v + wcol - win_ستون_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->ستون -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# ورنہ
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    اگر (wlv->draw_color_ستون)
	wlv->draw_color_ستون = advance_color_ستون(VCOL_HLC, &wlv->color_ستون_تعداد);

    اگر (((wp->w_p_cuc
		    && (صحیح)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (صحیح)wp->w_virtcol <
			 (طویل)wp->w_چوڑائی * (wlv->صف - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_ستون
# ifdef سطر_ATTR
		|| wlv->سطر_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	صحیح	rightmost_vcol = 0;
	صحیح	i;

	اگر (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	اگر (wlv->draw_color_ستون)
	    // determine rightmost colorcolumn to possibly draw
	    برائے (i = 0; wlv->color_ستون_تعداد[i] >= 0; ++i)
		اگر (rightmost_vcol < wlv->color_ستون_تعداد[i])
		    rightmost_vcol = wlv->color_ستون_تعداد[i];

	جب (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->ستون >= 0) :
# endif
		(wlv->ستون < wp->w_چوڑائی))
	{
	    Screen_سطریں[wlv->off] = ' ';
	    اگر (enc_utf8)
		Screen_سطریں_UC[wlv->off] = 0;

	    اگر (wlv->draw_color_ستون)
		wlv->draw_color_ستون = advance_color_ستون(
						   VCOL_HLC, &wlv->color_ستون_تعداد);

	    صحیح attr = wlv->win_attr;
# ifdef سطر_ATTR
	    اگر (wlv->سطر_attr != 0)
		attr = hl_combine_attr(attr, wlv->سطر_attr);
# endif
	    اگر (wp->w_p_cuc && VCOL_HLC == (طویل)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    ورنہ اگر (wlv->draw_color_ستون && VCOL_HLC == *wlv->color_ستون_تعداد)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    Screen_ستون_تعداد[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->ستون;
	    }
	    ورنہ
# endif
	    {
		++wlv->off;
		++wlv->ستون;
	    }
	    ++wlv->vcol;

	    اگر (VCOL_HLC > rightmost_vcol
# ifdef سطر_ATTR
		    && wlv->سطر_attr == 0
# endif
		    && wlv->win_attr == 0)
		توڑو;
	}
    }
#اگر_ختم

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_سطر_flags |= SLF_INC_VCOL;
    wlv_screen_سطر(wp, wlv, TRUE);
    wlv->screen_سطر_flags &= ~SLF_INC_VCOL;
    ++wlv->صف;
    ++wlv->screen_صف;
}
#تعریف_منسوخ VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    ساکن خالی
win_سطر_آغاز(win_T *wp غیر_مستعمل, winlinevars_T *wlv, صحیح save_extra)
{
    wlv->ستون = 0;
    wlv->off = (غیرعلامتی)(current_Screen_سطر - Screen_سطریں);
#اگر_تعریف_شدہ FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->ستون = wp->w_چوڑائی - 1;
	wlv->off += wlv->ستون;
	wlv->screen_سطر_flags |= SLF_RIGHTLEFT;
    }
#اگر_ختم
    اگر (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_آغاز;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = تہی;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#اگر_تعریف_شدہ FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	ورنہ
#اگر_ختم
	    wlv->saved_char_attr = 0;

	// these are not used until restored in win_line_continue()
	wlv->n_extra = 0;
	wlv->n_attr_skip = 0;
    }
}

/*
 * Called when wlv->draw_state is set to WL_LINE.
 */
    ساکن خالی
win_سطر_continue(winlinevars_T *wlv)
{
    اگر (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = تہی;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    ورنہ
	wlv->char_attr = wlv->win_attr;
}

#اگر_تعریف_شدہ FEAT_SYN_HL
    ساکن خالی
apply_cursorline_highlight(
	winlinevars_T *wlv,
	صحیح sign_present غیر_مستعمل)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    اگر (sign_present && wlv->sattr.sat_linehl > 0)
    {
	اگر (wlv->sattr.sat_ترجیح >= 100)
	    wlv->سطر_attr = hl_combine_attr(wlv->cul_attr, wlv->سطر_attr);
	ورنہ
	    wlv->سطر_attr = hl_combine_attr(wlv->سطر_attr, wlv->cul_attr);
    }
    ورنہ
# endif
# اگر defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->سطر_attr = hl_combine_attr(wlv->cul_attr, wlv->سطر_attr);
# ورنہ
	wlv->سطر_attr = wlv->cul_attr;
# endif
}
#اگر_ختم

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
    صحیح
win_سطر(
    win_T	*wp,
    linenr_T	lnum,
    صحیح		startrow,
    صحیح		endrow,
    صحیح		نمبر_only,
    spellvars_T	*spv غیر_مستعمل)
{
    winlinevars_T	wlv;		// variables passed between functions

    صحیح		c = 0;			// init for GCC
    char_u	*سطر;			// current line
    char_u	*ptr;			// current position in "line"
    صحیح		اندر_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    char_u	*p_extra_free2 = تہی;   // another p_extra to be freed
#اگر_ختم
#اگر defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    صحیح		اندر_linebreak = FALSE;	// n_extra set for showing linebreak
#اگر_ختم
    صحیح		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    صحیح		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    صحیح		n_attr = 0;	    // chars with special attr
    صحیح		saved_attr2 = 0;    // char_attr saved for n_attr
    صحیح		n_attr3 = 0;	    // chars with overruling special attr
    صحیح		saved_attr3 = 0;    // char_attr saved for n_attr3

    صحیح		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    صحیح		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    صحیح		lnum_اندر_visual_area = FALSE;
    pos_T	pos;
    طویل	v;

    صحیح		attr_pri = FALSE;	// char_attr has priority
    صحیح		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    صحیح		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    صحیح		area_attr = 0;		// attributes desired by highlighting
    صحیح		تلاش_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    صحیح		syntax_attr = 0;	// attributes desired by syntax
    صحیح		پچھلا_syntax_ستون = -1;	// column of prev_syntax_attr
    صحیح		پچھلا_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    صحیح		has_syntax = FALSE;	// this buffer has syntax highl.
    صحیح		save_did_emsg;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
# define WIN_سطر_متن_PROP_ڈھیر_LEN 32
    صحیح		did_سطر = FALSE;	// set to TRUE when line text done
    صحیح		متن_prop_گنتی;
    صحیح		آخری_textprop_متن_idx = -1;
    صحیح		متن_prop_اگلا = 0;	// next text property to use
    textprop_T	متن_props_buf[WIN_سطر_متن_PROP_ڈھیر_LEN];
    صحیح		متن_prop_idxs_buf[WIN_سطر_متن_PROP_ڈھیر_LEN];
    char_u	متن_prop_suffix_flags_buf[WIN_سطر_متن_PROP_ڈھیر_LEN + 1];
    textprop_T	*متن_props = تہی;
    صحیح		*متن_prop_idxs = تہی;
    char_u	*متن_prop_suffix_flags = تہی;
    صحیح		متن_props_فعال = 0;
    صحیح		متن_props_need_ترتیب_دو = FALSE;
    proptype_T  *متن_prop_type = تہی;
    صحیح		متن_prop_attr = 0;
    صحیح		متن_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    صحیح		متن_prop_id = 0;	// active property ID
    صحیح		متن_prop_flags = 0;
    صحیح		متن_prop_above = FALSE;  // first doing virtual text above
    صحیح		متن_prop_follows = FALSE;  // another text prop to display
    صحیح		saved_تلاش_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    صحیح		saved_area_attr = 0;	// idem for area_attr
    صحیح		reset_extra_attr = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
    صحیح		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    صحیح		nextlinecol = 0;	// column where nextline[] starts
    صحیح		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    صحیح		spell_attr = 0;		// attributes desired by spelling
    صحیح		لفظ_انتہا = 0;		// last byte with same spell_attr
    صحیح		cur_checked_ستون = 0;	// checked column for current line
#اگر_ختم
    صحیح		extra_check = FALSE;	// has extra highlighting
    صحیح		multi_attr = 0;		// attributes desired by multibyte
    صحیح		mb_l = 1;		// multi-byte byte length
    صحیح		mb_c = 0;		// decoded multi-byte character
    صحیح		mb_utf8 = FALSE;	// screen char is UTF-8 char
    صحیح		u8cc[MAX_MCO];		// composing UTF-8 chars
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		change_آغاز = MAXCOL;	// first col of changed area
    صحیح		change_انتہا = -1;	// last col of changed area
    diffline_T	سطر_changes;
    صحیح		change_اشاریہ = -1;
#اگر_ختم
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    صحیح		اندر_multispace = FALSE;	// in multiple consecutive spaces
    صحیح		multispace_pos = 0;	// position in lcs-multispace string
#اگر_تعریف_شدہ سطر_ATTR
    صحیح		سطر_attr_save = 0;
#اگر_ختم
    صحیح		sign_present = FALSE;
    صحیح		عدد_attr = 0;		// attribute for the number column
#اگر_تعریف_شدہ FEAT_ARABIC
    صحیح		پچھلا_c = 0;		// previous Arabic character
    صحیح		پچھلا_c1 = 0;		// first composing char for prev_c
#اگر_ختم
#اگر defined(سطر_ATTR)
    صحیح		did_سطر_attr = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    صحیح		get_رکن_attr = FALSE;
#اگر_ختم

#اگر defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    صحیح		بایاں_curline_ستون = 0;
    صحیح		دایاں_curline_ستون = 0;
#اگر_ختم

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    صحیح		feedback_ستون = 0;
    صحیح		feedback_old_attr = -1;
#اگر_ختم

#اگر defined(FEAT_CONCEAL) || defined(FEAT_تلاش_EXTRA)
    صحیح		مطابقت_conc	= 0;	// cchar for match functions
#اگر_ختم
#اگر defined(FEAT_CONCEAL) || defined(FEAT_تلاش_EXTRA) || defined(FEAT_LINEBREAK)
    صحیح		on_آخری_ستون     = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		syntax_flags	= 0;
    صحیح		syntax_seqnr	= 0;
    صحیح		پچھلا_syntax_id	= 0;
    صحیح		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    صحیح		is_concealing	= FALSE;
    صحیح		did_wcol	= FALSE;
    صحیح		old_boguscols   = 0;
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_co - wlv.vcol_off_tp)
# define FIX_FOR_BOGUSCOLS \
    { \
	wlv.n_extra += wlv.vcol_off_co; \
	wlv.vcol -= wlv.vcol_off_co; \
	wlv.vcol_off_co = 0; \
	wlv.ستون -= wlv.boguscols; \
	old_boguscols = wlv.boguscols; \
	wlv.boguscols = 0; \
    }
#ورنہ
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#اگر_ختم

    اگر (startrow > endrow)		// past the end already!
	واپس startrow;

    CLEAR_میدان(wlv);

    wlv.lnum = lnum;
    wlv.startrow = startrow;
    wlv.صف = startrow;
    wlv.screen_صف = wlv.صف + W_WINROW(wp);
    wlv.fromcol = -10;
    wlv.tocol = MAXCOL;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    wlv.vcol_sbr = -1;
#اگر_ختم

    اگر (نمبر_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#اگر_تعریف_شدہ FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (syntax_present(wp) && !wp->w_s->b_syn_error
# ifdef SYN_وقت_حد_بندی
		&& !wp->w_s->b_syn_slow
# endif
	   )
	{
	    // Prepare for syntax highlighting in this line.  When there is an
	    // error, stop syntax highlighting.
	    save_did_emsg = did_emsg;
	    did_emsg = FALSE;
	    syntax_آغاز(wp, lnum);
	    اگر (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    ورنہ
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_وقت_حد_بندی
		اگر (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_ستون_تعداد = wp->w_p_cc_ستون_تعداد;
	اگر (wlv.color_ستون_تعداد != تہی)
	    wlv.draw_color_ستون = advance_color_ستون(VCOL_HLC, &wlv.color_ستون_تعداد);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMINAL
	اگر (رکن_show_بفر(wp->w_بفر))
	{
	    extra_check = TRUE;
	    get_رکن_attr = TRUE;
	    wlv.win_attr = رکن_get_attr(wp, lnum, -1);
	}
#اگر_ختم

	// handle Visual active in this window
	اگر (VIsual_فعال && wp->w_بفر == curwin->w_بفر)
	{
	    pos_T	*چوٹی, *bot;

	    اگر (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		چوٹی = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    ورنہ
	    {
		// Visual is before curwin->w_cursor
		چوٹی = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_اندر_visual_area = (lnum >= چوٹی->lnum && lnum <= bot->lnum);
	    اگر (VIsual_منوال == Ctrl_V)
	    {
		// block mode
		اگر (lnum_اندر_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    ورنہ
	    {
		// non-block mode
		اگر (lnum > چوٹی->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		ورنہ اگر (lnum == چوٹی->lnum)
		{
		    اگر (VIsual_منوال == 'V')	// linewise
			wlv.fromcol = 0;
		    ورنہ
		    {
			getvvcol(wp, چوٹی, &wlv.fromcol, تہی, تہی, 0);
			اگر (gchar_pos(چوٹی) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		اگر (VIsual_منوال != 'V' && lnum == bot->lnum)
		{
		    اگر (*p_sel == 'e' && bot->ستون == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    ورنہ اگر (bot->ستون == MAXCOL)
			wlv.tocol = MAXCOL;
		    ورنہ
		    {
			pos = *bot;
			اگر (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, تہی, تہی, 0);
			ورنہ
			{
			    getvvcol(wp, &pos, تہی, تہی, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    اگر (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#اگر defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		اگر ((clip_ستارہ.دستیاب && !clip_ستارہ.owned
						    && clip_isautosel_ستارہ())
			|| (clip_plus.دستیاب && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#اگر_ختم
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	ورنہ اگر (highlight_مطابقت
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + تلاش_مطابقت_سطریں)
	{
	    اگر (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, تہی, تہی, 0);
	    ورنہ
		wlv.fromcol = 0;
	    اگر (lnum == curwin->w_cursor.lnum + تلاش_مطابقت_سطریں)
	    {
		pos.lnum = lnum;
		pos.ستون = تلاش_مطابقت_endcol;
		getvcol(curwin, &pos, &wlv.tocol, تہی, تہی, 0);
	    }
	    ورنہ
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    اگر (wlv.fromcol == wlv.tocol && تلاش_مطابقت_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#اگر_تعریف_شدہ FEAT_DIFF

    صحیح linestatus = 0;
    wlv.filler_سطریں = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_میدان(سطر_changes);

    اگر (linestatus < 0)
    {
	اگر (linestatus == -1)
	{
	    اگر (diff_ڈھونڈو_change(wp, lnum, &سطر_changes))
	    {
		wlv.diff_hlf = HLF_جوڑو;	// added line
	    }
	    ورنہ اگر (سطر_changes.عدد_changes > 0)
	    {
		صحیح added = diff_change_parse(
			&سطر_changes, &سطر_changes.changes[0],
			&change_آغاز, &change_انتہا);
		اگر (change_آغاز == 0)
		{
		    اگر (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    ورنہ
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		ورنہ
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_اشاریہ = 0;
	    }
	    ورنہ
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_اشاریہ = 0;
	    }
	}
	ورنہ
	    wlv.diff_hlf = HLF_جوڑو;

	area_highlighting = TRUE;
    }

    اگر (lnum == wp->w_topline)
	wlv.filler_سطریں = wp->w_topfill;

    wlv.filler_todo = wlv.filler_سطریں;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    اگر (sign_present)
	عدد_attr = wlv.sattr.sat_numhl;
#اگر_ختم

#اگر_تعریف_شدہ سطر_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    اگر (sign_present)
	wlv.سطر_attr = wlv.sattr.sat_linehl;
# endif
# اگر defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    اگر (bt_quickfix(wp->w_بفر) && qf_current_اندراج(wp) == lnum)
	wlv.سطر_attr = HL_ATTR(HLF_QFL);
# endif
    اگر (wlv.سطر_attr != 0)
	area_highlighting = TRUE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SPELL
    اگر (spv->spv_has_spell && نمبر_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	اگر (lnum == spv->spv_checked_lnum)
	    cur_checked_ستون = spv->spv_checked_ستون;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	اگر (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_ستون = 0;
	ورنہ اگر (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_ستون = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	اگر (lnum < wp->w_بفر->b_ml.ml_سطر_گنتی)
	{
	    سطر = ml_get_buf(wp->w_بفر, lnum + 1, FALSE);
	    spell_cat_سطر(nextline + SPWORDLEN, سطر, SPWORDLEN);
	}
	سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(سطر);
	اگر (*ptr == NUL)
	{
	    spv->spv_cap_ستون = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	ورنہ اگر (spv->spv_cap_ستون == 0)
	    spv->spv_cap_ستون = ptr - سطر;

	// Copy the end of the current line into nextline[].
	اگر (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	ورنہ
	{
	    v = ml_get_buf_len(wp->w_بفر, lnum);
	    اگر (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, سطر, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    ورنہ
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, سطر + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#اگر_ختم

    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
    ptr = سطر;

    اگر (wp->w_p_list)
    {
	اگر (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != تہی
		|| wp->w_lcs_chars.leadmultispace != تہی
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	اگر (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_بفر, lnum);
	    جب (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - سطر);
	}
	// find end of leading whitespace
	اگر (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != تہی ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    جب (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    اگر (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    ورنہ
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - سطر) + 1;
	}
    }

    اگر (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    صحیح skipcol_اندر_متن_prop_above = 0;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (WIN_IS_POPUP(wp))
	wlv.screen_سطر_flags |= SLF_POPUP;

    char_u *prop_آغاز;
    متن_prop_گنتی = get_متن_props(wp->w_بفر, lnum, &prop_آغاز, FALSE);
    اگر (متن_prop_گنتی > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	اگر (متن_prop_گنتی <= WIN_سطر_متن_PROP_ڈھیر_LEN)
	    متن_props = متن_props_buf;
	ورنہ
	    متن_props = ALLOC_MULT(textprop_T, متن_prop_گنتی);
	اگر (متن_props != تہی)
	{
	    mch_memmove(متن_props, prop_آغاز,
				     متن_prop_گنتی * حجم(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *گنتی_ptr = prop_آغاز - PROP_گنتی_سائز;

	    برائے (صحیح i = 0; i < متن_prop_گنتی; ++i)
	    {
		اگر (متن_props[i].tp_id < 0
				&& متن_props[i].u.tp_متن_انحراف > 0)
		{
		    متن_props[i].u.tp_متن =
			    گنتی_ptr + متن_props[i].u.tp_متن_انحراف;
		    متن_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		ورنہ
		    متن_props[i].u.tp_متن = تہی;
	    }
	}

	// Allocate an array for the indexes.
	اگر (متن_prop_گنتی <= WIN_سطر_متن_PROP_ڈھیر_LEN)
	    متن_prop_idxs = متن_prop_idxs_buf;
	ورنہ
	    متن_prop_idxs = ALLOC_MULT(صحیح, متن_prop_گنتی);
	اگر (متن_prop_idxs == تہی)
	{
	    اگر (متن_props != متن_props_buf)
		VIM_CLEAR(متن_props);
	    ورنہ
		متن_props = تہی;
	}
	اگر (متن_prop_گنتی <= WIN_سطر_متن_PROP_ڈھیر_LEN)
	    متن_prop_suffix_flags = متن_prop_suffix_flags_buf;
	ورنہ
	    متن_prop_suffix_flags = ALLOC_MULT(char_u, متن_prop_گنتی + 1);
	اگر (متن_prop_suffix_flags == تہی)
	{
	    اگر (متن_prop_idxs != متن_prop_idxs_buf)
		vim_free(متن_prop_idxs);
	    متن_prop_idxs = تہی;
	    اگر (متن_props != متن_props_buf)
		VIM_CLEAR(متن_props);
	    ورنہ
		متن_props = تہی;
	}

	اگر (متن_props != تہی)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    برائے (صحیح i = 0; i < متن_prop_گنتی; ++i)
		اگر (متن_props[i].tp_id < 0)
		    آخری_textprop_متن_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    صحیح متن_چوڑائی = wp->w_چوڑائی - win_ستون_off(wp);
	    برائے (صحیح i = متن_prop_گنتی - 1; i >= 0; --i)
		اگر (متن_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    اگر (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_اندر_متن_prop_above
								 >= متن_چوڑائی)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_اندر_متن_prop_above += متن_چوڑائی;
			برائے (صحیح j = i + 1; j < متن_prop_گنتی; ++j)
			    متن_props[j - 1] = متن_props[j];
			++i;
			--متن_prop_گنتی;
		    }
		    ورنہ
			++wlv.متن_prop_above_گنتی;
		}

	    متن_prop_suffix_flags[متن_prop_گنتی] = 0;
	    برائے (صحیح i = متن_prop_گنتی - 1; i >= 0; --i)
	    {
		صحیح flags = متن_prop_suffix_flags[i + 1];

		اگر (متن_props[i].tp_ستون == MAXCOL)
		{
		    flags |= 1;
		    اگر (متن_props[i].tp_flags & TP_FLAG_ALIGN_نیچے)
			flags |= 2;
		}
		متن_prop_suffix_flags[i] = flags;
	    }
	}
    }

    اگر (نمبر_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.صف += wlv.متن_prop_above_گنتی;
	اگر (wlv.صف >= endrow)
	{
	    اگر (متن_props != متن_props_buf)
		vim_free(متن_props);
	    اگر (متن_prop_idxs != متن_prop_idxs_buf)
		vim_free(متن_prop_idxs);
	    اگر (متن_prop_suffix_flags != متن_prop_suffix_flags_buf)
		vim_free(متن_prop_suffix_flags);
	    واپس wlv.صف;
	}
	wlv.screen_صف += wlv.متن_prop_above_گنتی;
    }
#اگر_ختم

#اگر defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_پہلا_char = 0;
    اگر (wp->w_p_lbr && نمبر_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
	(خالی)win_lbr_chartabsize(&cts, تہی, تہی);
	vcol_پہلا_char = cts.cts_پہلا_char;
	clear_chartabsize_arg(&cts);
    }
#اگر_ختم

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    اگر (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_اندر_متن_prop_above : 0;
    ورنہ
	v = wp->w_leftcol;
    اگر (v > 0 && نمبر_only == 0)
    {
	char_u		*پچھلا_ptr = ptr;
	chartabsize_T	cts;
	صحیح		charsize = 0;
	صحیح		سر = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, سطر, ptr);
	cts.cts_max_سر_vcol = v;
	جب (cts.cts_vcol < v)
	{
	    سر = 0;
	    charsize = win_lbr_chartabsize(&cts, &سر, تہی);
	    cts.cts_vcol += charsize;
	    پچھلا_ptr = cts.cts_ptr;
	    اگر (*پچھلا_ptr == NUL)
		توڑو;
	    MB_PTR_ADV(cts.cts_ptr);
	    اگر (wp->w_p_list)
	    {
		اندر_multispace = *پچھلا_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (پچھلا_ptr > سطر && پچھلا_ptr[-1] == ' '));
		اگر (!اندر_multispace)
		    multispace_pos = 0;
		ورنہ اگر (cts.cts_ptr >= سطر + leadcol
					 && wp->w_lcs_chars.multispace != تہی)
		{
		    ++multispace_pos;
		    اگر (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		ورنہ اگر (cts.cts_ptr < سطر + leadcol
				     && wp->w_lcs_chars.leadmultispace != تہی)
		{
		    ++multispace_pos;
		    اگر (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	اگر (wlv.vcol < v && (
#اگر_تعریف_شدہ FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_ستون ||
#اگر_ختم
	     virtual_فعال() ||
	     (VIsual_فعال && wp->w_بفر == curwin->w_بفر)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	اگر (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = پچھلا_ptr;
	}
	اگر (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - سر;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	اگر (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	ورنہ اگر (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#اگر_تعریف_شدہ FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	اگر (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	اگر (spv->spv_has_spell)
	{
	    صحیح		len;
	    colnr_T	linecol = (colnr_T)(ptr - سطر);
	    hlf_T	spell_hlf = HLF_گنتی;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.ستون = linecol;
	    len = spell_move_مقصد(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
	    ptr = سطر + linecol;

	    اگر (len == 0 || (صحیح)wp->w_cursor.ستون > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		//لفظ
		spell_hlf = HLF_گنتی;
		لفظ_انتہا = (صحیح)(spell_مقصد_لفظ_انتہا(ptr, wp) - سطر + 1);
	    }
	    ورنہ
	    {
		// bad word found, use attributes until end of word
		لفظ_انتہا = wp->w_cursor.ستون + len + 1;

		// Turn index into actual attributes.
		اگر (spell_hlf != HLF_گنتی)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    اگر (has_syntax)
		syntax_آغاز(wp, lnum);
# endif
	}
#اگر_ختم
    }

    اگر (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    اگر (نمبر_only == 0)
    {
	v = (طویل)(ptr - سطر);
	area_highlighting |= prepare_تلاش_hl_سطر(wp, lnum, (colnr_T)v,
					      &سطر, &screen_تلاش_hl,
					      &تلاش_attr);
	ptr = سطر + v; // "line" may have been updated
    }
#اگر_ختم

    اگر ((State & منوال_داخل_کرو) && ins_compl_win_فعال(wp)
			    && (اندر_curline || ins_compl_lnum_اندر_range(lnum)))
	area_highlighting = TRUE;

#اگر_تعریف_شدہ FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    اگر (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	اگر (!(wp == curwin && VIsual_فعال)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    اگر (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    ورنہ
	    {
		سطر_attr_save = wlv.سطر_attr;
		margin_columns_win(wp, &بایاں_curline_ستون, &دایاں_curline_ستون);
	    }
	    area_highlighting = TRUE;
	}
    }
#اگر_ختم

    win_سطر_آغاز(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    برائے (;;)
    {
#اگر defined(FEAT_CONCEAL) || defined(FEAT_تلاش_EXTRA)
	صحیح	has_مطابقت_conc = 0;	// match wants to conceal
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
	صحیح	did_decrement_ptr = FALSE;
#اگر_ختم

	// Skip this quickly when working on the text.
	اگر (wlv.draw_state != WL_سطر)
	{
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.سطر_attr = سطر_attr_save;
	    }
#اگر_ختم
	    اگر (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		اگر (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    اگر (wlv.صف == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    ورنہ
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	    اگر (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		اگر (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#اگر_ختم
	    اگر (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_ستون(wp, &wlv, sign_present, عدد_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    اگر (نمبر_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_سطر(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		اگر ((wlv.صف + 1 - wlv.startrow < نمبر_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#اگر_تعریف_شدہ FEAT_DIFF
			|| wlv.filler_todo > 0
#اگر_ختم
			)
		{
		    ++wlv.صف;
		    ++wlv.screen_صف;
		    اگر (wlv.صف == endrow)
			توڑو;
#اگر_تعریف_شدہ FEAT_DIFF
		    --wlv.filler_todo;
		    اگر (wlv.filler_todo == 0 && wp->w_botfill)
			توڑو;
#اگر_ختم
		    win_سطر_آغاز(wp, &wlv, TRUE);
		    جاری;
		}
		ورنہ
		    توڑو;
	    }

#اگر_تعریف_شدہ FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    اگر (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#اگر_ختم
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    اگر (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#اگر_ختم
	    اگر (wlv.draw_state == WL_سطر - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_سطر;
		win_سطر_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (wlv.cul_screenline && wlv.draw_state == WL_سطر
		&& wlv.vcol >= بایاں_curline_ستون
		&& wlv.vcol < دایاں_curline_ستون)
	    apply_cursorline_highlight(&wlv, sign_present);
#اگر_ختم

	// When still displaying '$' of change command, stop at cursor.
	اگر (dollar_vcol >= 0 && اندر_curline && wlv.vcol >= (طویل)wp->w_virtcol)
	{
	    wlv_screen_سطر(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (wp->w_p_cuc)
		wlv.صف = wp->w_cline_صف + wp->w_cline_اونچائی;
	    ورنہ
#اگر_ختم
		wlv.صف = wp->w_اونچائی;
	    توڑو;
	}

	اگر (wlv.draw_state == WL_سطر && (area_highlighting || extra_check))
	{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (متن_props != تہی)
	    {
		صحیح pi;
		صحیح bcol = (صحیح)(ptr - سطر);

		اگر (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !اندر_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		برائے (pi = 0; pi < متن_props_فعال; ++pi)
		{
		    صحیح		tpi = متن_prop_idxs[pi];
		    textprop_T  *tp = &متن_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    اگر ((tp->tp_ستون != MAXCOL
				       && bcol >= tp->tp_ستون - 1 + tp->tp_len))
		    {
			اگر (pi + 1 < متن_props_فعال)
			    mch_memmove(متن_prop_idxs + pi,
					متن_prop_idxs + pi + 1,
					حجم(صحیح)
					     * (متن_props_فعال - (pi + 1)));
			--متن_props_فعال;
			متن_props_need_ترتیب_دو = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			اگر (اندر_linebreak && syntax_attr == متن_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		اگر (wlv.n_extra > 0 && اندر_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		جب (متن_prop_اگلا < متن_prop_گنتی)
		{
		    صحیح فعال;
		    textprop_T *tp = &متن_props[متن_prop_اگلا];
		    اگر (tp->tp_ستون == MAXCOL)
		    {
			اگر (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    فعال = TRUE;
			ورنہ اگر (*ptr != NUL)
			    توڑو;
			ورنہ
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    فعال = wp->w_p_wrap
				  || wlv.صف == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_نیچے);
			}
		    }
		    ورنہ
		    {
			اگر (bcol < tp->tp_ستون - 1)
			    توڑو;
			فعال = bcol <= tp->tp_ستون - 1 + tp->tp_len;
		    }

		    اگر (فعال)
		    {
			متن_prop_idxs[متن_props_فعال++] = متن_prop_اگلا;
			متن_props_need_ترتیب_دو = TRUE;
		    }
		    ++متن_prop_اگلا;
		}

		اگر (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(متن_prop_type != تہی &&
			     متن_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    متن_prop_attr = 0;
		    متن_prop_attr_comb = 0;
		    متن_prop_flags = 0;
		    متن_prop_type = تہی;
		    متن_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		اگر (متن_props_فعال > 0 && wlv.n_extra == 0)
		{
		    صحیح مستعمل_tpi = -1;
		    صحیح مستعمل_attr = 0;
		    صحیح other_tpi = -1;

		    متن_prop_above = FALSE;
		    متن_prop_follows = FALSE;

		    اگر (متن_props_need_ترتیب_دو)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			ترتیب_دو_متن_props(wp->w_بفر, متن_props,
					متن_prop_idxs, متن_props_فعال);
			متن_props_need_ترتیب_دو = FALSE;
		    }

		    برائے (pi = 0; pi < متن_props_فعال; ++pi)
		    {
			صحیح	    tpi = متن_prop_idxs[pi];
			textprop_T  *tp = &متن_props[tpi];
			proptype_T  *pt = متن_prop_type_by_id(
						    wp->w_بفر, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			اگر (pt != تہی
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_دایاں
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_نیچے)) == 0
				    && wlv.ستون >= wp->w_چوڑائی))
			{
			    اگر (tp->tp_ستون == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == سطر
						&& !did_سطر
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_نیچے))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				متن_prop_follows = TRUE;
				جاری;
			    }

			    اگر (pt->pt_hl_id > 0)
				مستعمل_attr = syn_id2attr(pt->pt_hl_id);
			    متن_prop_type = pt;
			    متن_prop_attr =
				   hl_combine_attr(متن_prop_attr, مستعمل_attr);
			    اگر (مستعمل_tpi >= 0 && متن_props[مستعمل_tpi].tp_id < 0)
				other_tpi = مستعمل_tpi;
			    متن_prop_flags = pt->pt_flags;
			    متن_prop_id = tp->tp_id;
			    مستعمل_tpi = tpi;
			}
		    }
		    اگر (متن_prop_id < 0 && مستعمل_tpi >= 0
				 && متن_props[مستعمل_tpi].u.tp_متن != تہی)
		    {
			textprop_T  *tp = &متن_props[مستعمل_tpi];
			char_u	    *p = tp->u.tp_متن;
			صحیح	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			صحیح	    bail_برآمد = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			اگر (p != تہی)
			{
			    صحیح	    دایاں = (tp->tp_flags
							& TP_FLAG_ALIGN_دایاں);
			    صحیح	    نیچے = (tp->tp_flags
							& TP_FLAG_ALIGN_نیچے);
			    صحیح	    wrap = tp->tp_ستون < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    صحیح	    padding = tp->tp_ستون == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (صحیح)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.آغاز_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    مستعمل_attr);
			    n_attr = mb_charlen(p);
			    متن_prop_attr = 0;
			    متن_prop_attr_comb = 0;
			    اگر (*ptr == NUL)
				// don't combine char attr after EOL
				متن_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    اگر (متن_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    اگر ((دایاں || above || نیچے || !wrap
					    || padding > 0) && wp->w_چوڑائی > 2)
			    {
				char_u	*پچھلا_p_extra = wlv.p_extra;
				صحیح	آغاز_سطر;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				آغاز_سطر = متن_prop_position(wp, tp,
						    wlv.vcol,
# ifdef FEAT_RIGHTLEFT
						    wp->w_p_rl
						    ? wp->w_چوڑائی - wlv.ستون - 1
						    :
# endif
						    wlv.ستون,
						    &wlv.n_extra, &wlv.p_extra,
						    &n_attr, &wlv.n_attr_skip,
						    skip_cells > 0);
				اگر (wlv.p_extra != پچھلا_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				اگر (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				اگر (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.ستون
					       + wlv.n_extra - 2 > wp->w_چوڑائی)
				    // don't bail out at end of line
				    متن_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				اگر (آغاز_سطر)
				{
				    draw_screen_سطر(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    اگر (wlv.صف == endrow)
				    {
					++wlv.صف;
					توڑو;
				    }
				    win_سطر_آغاز(wp, &wlv, TRUE);
				    bail_برآمد = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			اگر (skip_cells > 0)
			{
			    اگر (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				اگر (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    ورنہ
			    {
				// the whole text is left of the window, drop
				// it and advance to the next one
				skip_cells -= wlv.n_extra;
				skipped_cells += wlv.n_extra;
				wlv.n_extra = 0;
				wlv.n_attr_skip = 0;
				bail_برآمد = TRUE;
			    }
			}

			// If another text prop follows the condition below at
			// the last window column must know.
			// If this is an "above" text prop and 'nowrap' then we
			// must wrap anyway.
			متن_prop_above = above;
			متن_prop_follows |= other_tpi != -1
					&& (wp->w_p_wrap
					     || (متن_props[other_tpi].tp_flags
			       & (TP_FLAG_ALIGN_نیچے | TP_FLAG_ALIGN_دایاں)));

			اگر (bail_برآمد)
			    // starting a new line for "below"
			    جاری;
		    }
		}
		ورنہ اگر (متن_prop_اگلا < متن_prop_گنتی
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.ستون == wp->w_چوڑائی - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    صحیح only_نیچے_follows = !wp->w_p_wrap
				 && wlv.ستون + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_چوڑائی;
		    صحیح suffix_flags = متن_prop_suffix_flags[متن_prop_اگلا];

		    متن_prop_follows = (suffix_flags
					& (only_نیچے_follows ? 2 : 1)) != 0;
		}
	    }

	    اگر (wlv.آغاز_extra_for_textprop)
	    {
		wlv.آغاز_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_تلاش_attr = تلاش_attr;
		saved_area_attr = area_attr;
		تلاش_attr = 0;
		area_attr = 0;
	    }
#اگر_ختم

	    صحیح *area_attr_p =
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#اگر_ختم
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    اگر (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != تہی
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    ورنہ اگر (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    اگر (wlv.n_extra == 0)
	    {
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (طویل)(ptr - سطر);
		تلاش_attr = update_تلاش_hl(wp, lnum, (colnr_T)v, &سطر,
				      &screen_تلاش_hl, &has_مطابقت_conc,
				      &مطابقت_conc, did_سطر_attr, lcs_eol_one,
				      &on_آخری_ستون);
		ptr = سطر + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		اگر (*ptr == NUL)
		    has_مطابقت_conc = 0;
#ورنہ
		تلاش_attr = 0;
#اگر_ختم

		// Check if ComplMatchIns highlight is needed.
		اگر ((State & منوال_داخل_کرو) && ins_compl_win_فعال(wp)
			    && (اندر_curline || ins_compl_lnum_اندر_range(lnum)))
		{
		    صحیح ins_مطابقت_attr =
			ins_compl_ستون_range_attr(lnum, (صحیح)(ptr - سطر));
		    اگر (ins_مطابقت_attr > 0)
			تلاش_attr =
			    hl_combine_attr(تلاش_attr, ins_مطابقت_attr);
		}
	    }

#اگر_تعریف_شدہ FEAT_DIFF
	    اگر (wlv.diff_hlf != (hlf_T)0)
	    {
		اگر (سطر_changes.عدد_changes > 0
			&& change_اشاریہ >= 0
			&& change_اشاریہ < سطر_changes.عدد_changes - 1)
		{
		    اگر (ptr - سطر >= سطر_changes.changes[change_اشاریہ+1].dc_آغاز[سطر_changes.bufidx])
			change_اشاریہ += 1;
		}
		صحیح added = FALSE;
		اگر (سطر_changes.عدد_changes > 0 && change_اشاریہ >= 0 && change_اشاریہ < سطر_changes.عدد_changes)
		{
		    added = diff_change_parse(&سطر_changes,
			    &سطر_changes.changes[change_اشاریہ],
			    &change_آغاز,
			    &change_انتہا);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		اگر (wlv.diff_hlf == HLF_CHD && ptr - سطر >= change_آغاز
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		اگر ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - سطر >= change_انتہا && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.سطر_attr = HL_ATTR(wlv.diff_hlf);
		اگر (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= بایاں_curline_ستون
					    && wlv.vcol <= دایاں_curline_ستون)))
		    wlv.سطر_attr = hl_combine_attr(
					  wlv.سطر_attr, HL_ATTR(HLF_CUL));
	    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		اگر (get_رکن_attr)
		    syntax_attr = رکن_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		اگر (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (طویل)(ptr - سطر);
		    اگر (v == پچھلا_syntax_ستون)
			// at same column again
			syntax_attr = پچھلا_syntax_attr;
		    ورنہ
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    تہی, FALSE);
			پچھلا_syntax_ستون = v;
			پچھلا_syntax_attr = syntax_attr;
		    }

		    اگر (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    ورنہ
			did_emsg = save_did_emsg;
# ifdef SYN_وقت_حد_بندی
		    اگر (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
		    ptr = سطر + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    اگر (*ptr == NUL)
			syntax_flags = 0;
		    ورنہ
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    اگر (متن_prop_type != تہی)
	    {
		اگر (متن_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, متن_prop_attr);
		ورنہ
		    syntax_attr = متن_prop_attr;
		متن_prop_attr_comb = syntax_attr;
	    }
# endif
#اگر_ختم

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#اگر_تعریف_شدہ سطر_ATTR
	    اگر (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.سطر_attr, area_attr);
		اگر (!highlight_مطابقت)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(تلاش_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ورنہ اگر (تلاش_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.سطر_attr, تلاش_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ورنہ اگر (wlv.سطر_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.سطر_attr);
# ورنہ
		wlv.char_attr = wlv.سطر_attr;
# endif
		attr_pri = FALSE;
	    }
#ورنہ
	    اگر (area_attr != 0)
		wlv.char_attr = area_attr;
	    ورنہ اگر (تلاش_attr != 0)
		wlv.char_attr = تلاش_attr;
#اگر_ختم
	    ورنہ
	    {
		attr_pri = FALSE;
#اگر_تعریف_شدہ FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#ورنہ
		wlv.char_attr = 0;
#اگر_ختم
	    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    اگر (متن_prop_type != تہی && (متن_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, متن_prop_attr);
#اگر_ختم
	}

	// combine attribute with HLF_WIN
	اگر (wlv.win_attr != 0)
	{
	    اگر (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    ورنہ
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
	اگر (wlv.n_extra > 0)
	{
	    اگر (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		اگر (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		ورنہ
		    mb_utf8 = FALSE;
	    }
	    ورنہ
	    {
		c = *wlv.p_extra;
		اگر (has_mbyte)
		{
		    mb_c = c;
		    اگر (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			اگر (mb_l > wlv.n_extra)
			    mb_l = 1;
			ورنہ اگر (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    ورنہ
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			اگر (mb_l >= wlv.n_extra)
			    mb_l = 1;
			ورنہ اگر (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    اگر (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    اگر ((
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون <= 0) :
#اگر_ختم
				    (wlv.ستون >= wp->w_چوڑائی - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = HL_ATTR(HLF_AT);
#اگر_تعریف_شدہ FEAT_SYN_HL
			اگر (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#اگر_ختم
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    ورنہ
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#اگر defined(FEAT_PROP_POPUP)
	    اگر (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		اگر (wlv.saved_n_extra <= 0)
		{
		    اگر (تلاش_attr == 0)
			تلاش_attr = saved_تلاش_attr;
		    اگر (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    اگر (wlv.extra_for_textprop)
			// wlv.extra_attr should be used at this position but
			// not any further.
			reset_extra_attr = TRUE;
		}

		wlv.extra_for_textprop = FALSE;
# ifdef FEAT_LINEBREAK
		اندر_linebreak = FALSE;
# endif
	    }
#اگر_ختم
	}
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    صحیح		c0;
#اگر_ختم
	    char_u	*پچھلا_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    c0 = *ptr;
#اگر_ختم
	    اگر (c == NUL)
	    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_سطر = TRUE;
#اگر_ختم
		// no more cells to skip
		skip_cells = 0;
	    }

	    اگر (has_mbyte)
	    {
		mb_c = c;
		اگر (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    اگر (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			اگر (mb_c < 0x80)
			{
			    c = mb_c;
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    c0 = mb_c;
#اگر_ختم
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			اگر (utf_iscomposing(mb_c))
			{
			    صحیح i;

			    برائے (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    اگر ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			اگر (wp->w_p_rl)		//الٹ
			    rl_mirror(wlv.extra);
#اگر_ختم
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (صحیح)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			اگر (area_attr == 0 && تلاش_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    ورنہ اگر (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#اگر_تعریف_شدہ FEAT_ARABIC
		    ورنہ اگر (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			صحیح	pc, pc1, nc;
			صحیح	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			اگر (wp->w_p_rl)
			{
			    pc = پچھلا_c;
			    pc1 = پچھلا_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    پچھلا_c1 = u8cc[0];
			}
			ورنہ
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = پچھلا_c;
			    pc1 = pcc[0];
			}
			پچھلا_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    ورنہ
			پچھلا_c = mb_c;
#اگر_ختم
		}
		ورنہ	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    اگر (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    ورنہ اگر (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			اگر (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			ورنہ
			{
			    اگر (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_بفر, wlv.extra, c);
				wlv.n_extra = (صحیح)STRLEN(wlv.extra) - 1;
			    }
			    ورنہ
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
			    اگر (area_attr == 0 && تلاش_attr == 0)
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
		اگر ((
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون <= 0) :
#اگر_ختم
				(wlv.ستون >= wp->w_چوڑائی - 1))
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
#اگر_تعریف_شدہ FEAT_CONCEAL
		    did_decrement_ptr = TRUE;
#اگر_ختم
		}
		ورنہ اگر (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		اگر (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    اگر (area_attr == 0 && تلاش_attr == 0)
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

	    اگر (extra_check)
	    {
#اگر_تعریف_شدہ FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (طویل)(ptr - سطر);
		اگر (spv->spv_has_spell && v >= لفظ_انتہا && v > cur_checked_ستون)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    اگر (c != 0 && (*skipwhite(پچھلا_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			صحیح	len;
			hlf_T	spell_hlf = HLF_گنتی;

			اگر (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			اگر ((پچھلا_ptr - سطر) - nextlinecol >= 0)
			    p = nextline + (پچھلا_ptr - سطر) - nextlinecol;
			ورنہ
			    p = پچھلا_ptr;
			spv->spv_cap_ستون -= (صحیح)(پچھلا_ptr - سطر);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_ستون,
							    spv->spv_unchanged);
			لفظ_انتہا = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			اگر (spell_hlf != HLF_گنتی
				&& (State & منوال_داخل_کرو)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ستون >=
						    (colnr_T)(پچھلا_ptr - سطر)
				&& wp->w_cursor.ستون < (colnr_T)لفظ_انتہا)
			{
			    spell_hlf = HLF_گنتی;
			    spell_redraw_lnum = lnum;
			}

			اگر (spell_hlf == HLF_گنتی && p != پچھلا_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_ستون = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			اگر (spell_hlf != HLF_گنتی)
			    spell_attr = highlight_attr[spell_hlf];

			اگر (spv->spv_cap_ستون > 0)
			{
			    اگر (p != پچھلا_ptr
				    && (p - nextline) + spv->spv_cap_ستون
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_ستون = ((p - nextline)
					    + spv->spv_cap_ستون - nextline_idx);
			    }
			    ورنہ
				// Compute the actual column.
				spv->spv_cap_ستون += (پچھلا_ptr - سطر);
			}
		    }
		}
		اگر (spell_attr != 0)
		{
		    اگر (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    ورنہ
			wlv.char_attr = hl_combine_attr(spell_attr,
								wlv.char_attr);
		}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// we don't want linebreak to apply for lines that start with
		// leading spaces, followed by long letters (since it would add
		// a break at the beginning of a line and this might be unexpected)
		//
		// So only allow to linebreak, once we have found chars not in
		// 'breakat' in the line.
		اگر ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((صحیح)*ptr))
		    wlv.need_lbr = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Found last space before word: check for line break.
		اگر (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((صحیح)*ptr))
		{
		    صحیح	    mb_off = has_mbyte ? (*mb_سر_off)(سطر, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_پہلا_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, سطر, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_متن = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, تہی, تہی) - 1;
		    clear_chartabsize_arg(&cts);

		    اگر (on_آخری_ستون && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			تلاش_attr = 0;

		    اگر (c == TAB && wlv.n_extra + wlv.ستون > wp->w_چوڑائی)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_بفر->b_p_ts,
					      wp->w_بفر->b_p_vts_array) - 1;
# ورنہ
			wlv.n_extra = (صحیح)wp->w_بفر->b_p_ts
				    - wlv.vcol % (صحیح)wp->w_بفر->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    اگر (wlv.n_extra > 0 && c != TAB)
			اندر_linebreak = TRUE;
# endif
		    اگر (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			اگر (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			اگر (!wp->w_p_list)
			    c = ' ';
		    }
		}
#اگر_ختم
		اگر (wp->w_p_list)
		{
		    اندر_multispace = c == ' ' && (*ptr == ' '
				  || (پچھلا_ptr > سطر && پچھلا_ptr[-1] == ' '));
		    اگر (!اندر_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		اگر (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (اندر_multispace
					&& wp->w_lcs_chars.multispace != تہی))
				&& ptr - سطر >= leadcol
				&& ptr - سطر <= trailcol)))
		{
		    اگر (اندر_multispace && wp->w_lcs_chars.multispace != تہی)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			اگر (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    ورنہ
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    اگر (area_attr == 0 && تلاش_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    اگر (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ورنہ
			mb_utf8 = FALSE;
		}

		اگر (c == ' ' && ((trailcol != MAXCOL && ptr > سطر + trailcol)
				    || (leadcol != 0 && ptr < سطر + leadcol)))
		{
		    اگر (leadcol != 0 && اندر_multispace && ptr < سطر + leadcol
			    && wp->w_lcs_chars.leadmultispace != تہی)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			اگر (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    ورنہ اگر (ptr > سطر + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    ورنہ اگر (ptr < سطر + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    ورنہ اگر (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    اگر (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    اگر (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ورنہ
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    اگر (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		اگر (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    صحیح	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    طویل    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    صحیح	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    صحیح	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    صحیح	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    اگر (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < سطر + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#اگر_تعریف_شدہ FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_قدر(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    اگر (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#اگر_ختم
		    // tab amount depends on current column
#اگر_تعریف_شدہ FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_بفر->b_p_ts,
					      wp->w_بفر->b_p_vts_array) - 1;
#ورنہ
		    tab_len = (صحیح)wp->w_بفر->b_p_ts
			       - vcol_adjusted % (صحیح)wp->w_بفر->b_p_ts - 1;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_LINEBREAK
		    اگر (!wp->w_p_lbr || !wp->w_p_list)
#اگر_ختم
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#اگر_تعریف_شدہ FEAT_LINEBREAK
		    ورنہ
		    {
			char_u	*p;
			صحیح	len;
			صحیح	i;
			صحیح	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			اگر (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			اگر (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			اگر (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    صحیح tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    اگر (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    اگر (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    اگر (p == تہی)
				wlv.n_extra = 0;
			    ورنہ
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				برائے (i = 0; i < tab_len; i++)
				{
				    صحیح lcs = lcs_tab2;

				    اگر (*p == NUL)
				    {
					tab_len = i;
					توڑو;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    اگر (lcs_tab3 && i == tab_len - 1)
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
				اگر (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
		    {
			صحیح vc_saved = wlv.vcol_off_co;

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
			اگر (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#اگر_ختم
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    اگر (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#اگر_تعریف_شدہ FEAT_LINEBREAK
			اگر (wp->w_p_lbr && wlv.p_extra != تہی
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			ورنہ
#اگر_ختم
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			اگر (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    ورنہ
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		ورنہ اگر (c == NUL
			&& wlv.n_extra == 0
			&& (wp->w_p_list
			    || (wlv.fromcol >= 0
				&& wlv.tocol > wlv.vcol
				&& VIsual_منوال != Ctrl_V
				&& (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
				    wp->w_p_rl ? (wlv.ستون >= 0) :
#اگر_ختم
				    (wlv.ستون < wp->w_چوڑائی))))
			&& lcs_eol_one > 0)
		{
		    // Display a '$' after the line or highlight an extra
		    // character if the line break is included.
#اگر defined(FEAT_DIFF) || defined(سطر_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    اگر (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef سطر_ATTR
			    &&
#  endif
# endif
# ifdef سطر_ATTR
			    wlv.سطر_attr == 0
# endif
		       )
#اگر_ختم
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			اگر (!(area_highlighting && virtual_فعال()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    اگر (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    ورنہ
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    اگر (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    اگر (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ورنہ
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ورنہ اگر (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_بفر, c);
		    اگر (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    اگر ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#اگر_ختم
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#اگر_تعریف_شدہ FEAT_LINEBREAK
		    اگر (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			اگر (p == تہی)
			    wlv.n_extra = 0;
			ورنہ
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    ورنہ
#اگر_ختم
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    اگر (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			اگر (متن_prop_type != تہی &&
				 متن_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(متن_prop_attr,
							       wlv.extra_attr);
#اگر_ختم

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ورنہ اگر (VIsual_فعال
			 && (VIsual_منوال == Ctrl_V
			     || VIsual_منوال == 'v')
			 && virtual_فعال()
			 && wlv.tocol != MAXCOL
			 && wlv.vcol < wlv.tocol
			 && (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون >= 0) :
#اگر_ختم
			    (wlv.ستون < wp->w_چوڑائی)))
		{
		    c = ' ';
		    --ptr;	    // put it back at the NUL
		}
#اگر defined(سطر_ATTR)
		ورنہ اگر ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.سطر_attr != 0
			) && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون >= 0) :
# endif
			    (wlv.ستون
# ifdef FEAT_CONCEAL
				- wlv.boguscols
# endif
					    < wp->w_چوڑائی)))
		{
		    // Highlight until the right side of the window
		    c = ' ';
		    --ptr;	    // put it back at the NUL

		    // Remember we do the char for line highlighting.
		    ++did_سطر_attr;

		    // don't do search HL for the rest of the line
		    اگر (wlv.سطر_attr != 0 && wlv.char_attr == تلاش_attr
					&& (did_سطر_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.سطر_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    اگر (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_سطر
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    اگر (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			اگر (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    اگر (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= بایاں_curline_ستون
					    && wlv.vcol <= دایاں_curline_ستون)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    اگر (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			اگر (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    اگر (!wlv.cul_screenline
				    || (wlv.vcol >= بایاں_curline_ستون
					     && wlv.vcol <= دایاں_curline_ستون))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			ورنہ اگر (wlv.سطر_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.سطر_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			اگر (wlv.vcol == 0)
			{
			    اگر (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    ورنہ اگر (تلاش_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, تلاش_attr);
			}
		    }
# endif
		}
#اگر_ختم
	    }

#اگر_تعریف_شدہ FEAT_CONCEAL
	    اگر (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_سطر(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_مطابقت_conc > 0)
		&& !(lnum_اندر_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == تہی))
	    {
		صحیح syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		اگر (((پچھلا_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_مطابقت_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_مطابقت_conc && مطابقت_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    اگر (has_مطابقت_conc && مطابقت_conc)
			c = مطابقت_conc;
		    ورنہ اگر (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    ورنہ اگر (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    ورنہ
			c = ' ';

		    اگر (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    اگر (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ورنہ
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    پچھلا_syntax_id = syntax_seqnr;

		    اگر (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    اگر (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			اگر (wp->w_p_rl)
			{
			    wlv.ستون -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			ورنہ
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.ستون += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		ورنہ اگر (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    ورنہ
	    {
		پچھلا_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    اگر (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#اگر_ختم // FEAT_CONCEAL
	}

#اگر_تعریف_شدہ FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	اگر (!did_wcol && wlv.draw_state == WL_سطر
		&& اندر_curline && conceal_cursor_سطر(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		wp->w_wcol = wp->w_چوڑائی - wlv.ستون + wlv.boguscols - 1;
	    ورنہ
# endif
		wp->w_wcol = wlv.ستون - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    اگر (wlv.vcol + skip_cells < wp->w_virtcol)
		// Cursor beyond end of the line with 'virtualedit'.
		wp->w_wcol += wp->w_virtcol - wlv.vcol - skip_cells;
	    wp->w_wrow = wlv.صف;
	    did_wcol = TRUE;
	    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
# ifdef FEAT_PROP_POPUP
	    curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
# endif
	}
#اگر_ختم

	// Use "wlv.extra_attr", but don't override visual selection
	// highlighting, unless text property overrides.
	// Don't use "wlv.extra_attr" until wlv.n_attr_skip is zero.
	اگر (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_سطر
		&& (!attr_pri
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    || (متن_prop_flags & PT_FLAG_OVERRIDE)
#اگر_ختم
		   ))
	{
#اگر_تعریف_شدہ سطر_ATTR
	    اگر (wlv.سطر_attr)
		wlv.char_attr = hl_combine_attr(wlv.سطر_attr, wlv.extra_attr);
	    ورنہ
#اگر_ختم
		wlv.char_attr = wlv.extra_attr;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#اگر_ختم
	}

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	اگر (p_imst == IM_ON_THE_SPOT
		&& xic != تہی
		&& lnum == wp->w_cursor.lnum
		&& (State & منوال_داخل_کرو)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_سطر)
	{
	    colnr_T tcol;

	    اگر (preedit_انتہا_ستون == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, تہی, تہی, 0);
	    ورنہ
		tcol = preedit_انتہا_ستون;
	    اگر ((طویل)preedit_آغاز_ستون <= wlv.vcol && wlv.vcol < (طویل)tcol)
	    {
		اگر (feedback_old_attr < 0)
		{
		    feedback_ستون = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_ستون);
		اگر (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_ستون++;
	    }
	    ورنہ اگر (feedback_old_attr >= 0)
	    {
		wlv.char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_ستون = 0;
	    }
	}
#اگر_ختم
	// Handle the case where we are in column 0 but not on the first
	// character of the line and the user wants us to show us a
	// special character (via 'listchars' option "precedes:<char>").
	اگر (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? (wp->w_skipcol > 0 && wlv.صف == 0)
				 : wp->w_leftcol > 0)
#اگر_تعریف_شدہ FEAT_DIFF
		&& wlv.filler_todo <= 0
#اگر_ختم
		&& wlv.draw_state > WL_NR
		&& skip_cells <= 0
		&& c != NUL)
	{
	    c = wp->w_lcs_chars.prec;
	    lcs_prec_todo = NUL;
	    اگر (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    اگر (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ورنہ
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    اگر (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	اگر ((c == NUL
#اگر defined(سطر_ATTR)
		|| did_سطر_attr == 1
#اگر_ختم
		) && wlv.eol_hl_off == 0)
	{
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    صحیح prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_تلاش_hl,
					      (طویل)(ptr - سطر) - (c == NUL));
#اگر_ختم
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    اگر (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_منوال != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_فعال))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# اگر defined(سطر_ATTR)
			    && did_سطر_attr <= 1
# endif
			   )
#اگر_ختم
		       ))
	    {
		صحیح n = 0;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		اگر (wp->w_p_rl)
		{
		    اگر (wlv.ستون < 0)
			n = 1;
		}
		ورنہ
#اگر_ختم
		{
		    اگر (wlv.ستون >= wp->w_چوڑائی)
			n = -1;
		}
		اگر (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.ستون += n;
		}
		ورنہ
		{
		    // Add a blank character to highlight.
		    Screen_سطریں[wlv.off] = ' ';
		    اگر (enc_utf8)
			Screen_سطریں_UC[wlv.off] = 0;
		}
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		اگر (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_تلاش_مطابقت_hl(wp, &screen_تلاش_hl,
					   (طویل)(ptr - سطر), &wlv.char_attr);
		}
#اگر_ختم
		ScreenAttrs[wlv.off] = wlv.char_attr;
		Screen_ستون_تعداد[wlv.off] = wlv.vcol;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		اگر (wp->w_p_rl)
		{
		    --wlv.ستون;
		    --wlv.off;
		}
		ورنہ
#اگر_ختم
		{
		    ++wlv.ستون;
		    ++wlv.off;
		}
		++wlv.vcol;
		wlv.eol_hl_off = 1;
	    }
	}

	// At end of the text line.
	اگر (c == NUL)
	{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (متن_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    ورنہ
#اگر_ختم
	    {
		draw_screen_سطر(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		اگر (اندر_curline)
		{
		    curwin->w_cline_صف = startrow;
		    curwin->w_cline_اونچائی = wlv.صف - startrow;
#اگر_تعریف_شدہ FEAT_FOLDING
		    curwin->w_cline_folded = false;
#اگر_ختم
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		توڑو;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	صحیح lcs_ext = get_lcs_ext(wp);
	اگر (lcs_ext != NUL
		&& wlv.draw_state == WL_سطر
#اگر_تعریف_شدہ FEAT_DIFF
		&& wlv.filler_todo <= 0
#اگر_ختم
		&& (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    wp->w_p_rl ? wlv.ستون == 0 :
#اگر_ختم
		    wlv.ستون == wp->w_چوڑائی - 1)
		&& (*ptr != NUL
		    || lcs_eol_one > 0
		    || (wlv.n_extra > 0 && (wlv.c_extra != NUL
						     || *wlv.p_extra != NUL))
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    || متن_prop_اگلا <= آخری_textprop_متن_idx
#اگر_ختم
		   ))
	{
	    c = lcs_ext;
	    wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    mb_c = c;
	    اگر (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ورنہ
		mb_utf8 = FALSE;
	}

#اگر_تعریف_شدہ FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	اگر (wlv.draw_color_ستون)
	    wlv.draw_color_ستون = advance_color_ستون(VCOL_HLC, &wlv.color_ستون_تعداد);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	اگر (((wlv.draw_state == WL_سطر
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_اندر_visual_area
		&& تلاش_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    اگر (wp->w_p_cuc && VCOL_HLC == (طویل)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    ورنہ اگر (wlv.draw_color_ستون && VCOL_HLC == *wlv.color_ستون_تعداد)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#اگر_ختم

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	اگر (wlv.draw_state < WL_سطر || skip_cells <= 0)
	{
	    // Store the character.
#اگر defined(FEAT_RIGHTLEFT)
	    اگر (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.ستون;
	    }
#اگر_ختم
	    Screen_سطریں[wlv.off] = c;
	    اگر (enc_dbcs == DBCS_JPNU)
	    {
		اگر ((mb_c & 0xff00) == 0x8e00)
		    Screen_سطریں[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    ورنہ اگر (enc_utf8)
	    {
		اگر (mb_utf8)
		{
		    صحیح i;

		    Screen_سطریں_UC[wlv.off] = mb_c;
		    اگر ((c & 0xff) == 0)
			Screen_سطریں[wlv.off] = 0x80;   // avoid storing zero
		    برائے (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطریں_C[i][wlv.off] = u8cc[i];
			اگر (u8cc[i] == 0)
			    توڑو;
		    }
		}
		ورنہ
		    Screen_سطریں_UC[wlv.off] = 0;
	    }
	    اگر (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    ورنہ
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    اگر (wlv.draw_state > WL_NR
#اگر_تعریف_شدہ FEAT_DIFF
		    && wlv.filler_todo <= 0
#اگر_ختم
		    )
		Screen_ستون_تعداد[wlv.off] = wlv.vcol;
	    ورنہ
		Screen_ستون_تعداد[wlv.off] = -1;

	    اگر (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.ستون;
		اگر (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_سطریں[wlv.off] = 0;
		ورنہ
		    // DBCS: Put second byte in the second screen char.
		    Screen_سطریں[wlv.off] = mb_c & 0xff;

		اگر (wlv.draw_state > WL_NR
#اگر_تعریف_شدہ FEAT_DIFF
			&& wlv.filler_todo <= 0
#اگر_ختم
			)
		    Screen_ستون_تعداد[wlv.off] = ++wlv.vcol;
		ورنہ
		    Screen_ستون_تعداد[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		اگر (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		اگر (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.ستون;
		}
#اگر_ختم
	    }
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.ستون;
	    }
	    ورنہ
#اگر_ختم
	    {
		++wlv.off;
		++wlv.ستون;
	    }
	}
#اگر_تعریف_شدہ FEAT_CONCEAL
	ورنہ اگر (wp->w_p_cole > 0 && is_concealing)
	{
	    صحیح concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    اگر (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    اگر (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    اگر (wp->w_p_wrap)
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
		صحیح adjust_ستون = !WIN_IS_POPUP(wp);
# ورنہ
		صحیح adjust_ستون = TRUE;
# endif
		اگر (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    اگر (wp->w_p_rl)
		    {
			اگر (adjust_ستون)
			{
			    wlv.ستون -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    ورنہ
# endif
		    {
			اگر (adjust_ستون)
			{
			    wlv.ستون += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		اگر (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    اگر (wp->w_p_rl)
		    {
			اگر (adjust_ستون)
			{
			    --wlv.boguscols;
			    --wlv.ستون;
			}
		    }
		    ورنہ
# endif
		    {
			اگر (adjust_ستون)
			{
			    ++wlv.boguscols;
			    ++wlv.ستون;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		اگر (wp->w_p_rl)
		{
		    اگر (adjust_ستون)
		    {
			--wlv.boguscols;
			--wlv.ستون;
		    }
		}
		ورنہ
# endif
		{
		    اگر (adjust_ستون)
		    {
			++wlv.boguscols;
			++wlv.ستون;
		    }
		}
	    }
	    ورنہ
	    {
		اگر (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#اگر_ختم // FEAT_CONCEAL
	ورنہ
	    --skip_cells;

	اگر (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	اگر (wlv.draw_state > WL_NR
#اگر_تعریف_شدہ FEAT_DIFF
		&& wlv.filler_todo <= 0
#اگر_ختم
		)
	    ++wlv.vcol;

#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#اگر_ختم

	// restore attributes after "precedes" in 'listchars'
	اگر (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	اگر (n_attr > 0 && wlv.draw_state == WL_سطر
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	اگر (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	اگر ((
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.ستون < 0) :
#اگر_ختم
				    (wlv.ستون >= wp->w_چوڑائی))
		&& (wlv.draw_state != WL_سطر
		    || *ptr != NUL
#اگر_تعریف_شدہ FEAT_DIFF
		    || wlv.filler_todo > 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    || متن_prop_above || متن_prop_follows
		    || متن_prop_اگلا <= آخری_textprop_متن_idx
#اگر_ختم
		    || (wp->w_p_list && wp->w_lcs_chars.eol != NUL
						&& lcs_eol_one != -1)
		    || (wlv.n_extra != 0 && (wlv.c_extra != NUL
						      || *wlv.p_extra != NUL)))
		)
	{
#اگر_تعریف_شدہ FEAT_CONCEAL
	    wlv.ستون -= wlv.boguscols;
	    // Apply 'cursorline' and HLF_WIN highlight.
	    اگر (wlv.boguscols != 0 && (
# ifdef سطر_ATTR
			wlv.سطر_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		صحیح attr = wlv.win_attr;
# ifdef سطر_ATTR
		اگر (wlv.سطر_attr != 0)
		    attr = hl_combine_attr(attr, wlv.سطر_attr);
# endif
		جب ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.ستون >= 0 :
# endif
			    wlv.ستون < wp->w_چوڑائی))
		{
		    Screen_سطریں[wlv.off] = ' ';
		    اگر (enc_utf8)
			Screen_سطریں_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    Screen_ستون_تعداد[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    اگر (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.ستون--;
			wlv.boguscols++;
		    }
		    ورنہ
# endif
		    {
			wlv.off++;
			wlv.ستون++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_سطر(wp, &wlv, TRUE);
	    wlv.ستون += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#ورنہ
	    wlv_screen_سطر(wp, &wlv, TRUE);
#اگر_ختم
	    ++wlv.صف;
	    ++wlv.screen_صف;

	    // When not wrapping and finished diff lines, break here.
	    اگر (!wp->w_p_wrap
#اگر_تعریف_شدہ FEAT_DIFF
			&& wlv.filler_todo <= 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			&& !متن_prop_above
			&& !متن_prop_follows
#اگر_ختم
		       )
		توڑو;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (!wp->w_p_wrap && متن_prop_follows && !متن_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = سطر + (size_t)ml_get_buf_len(wp->w_بفر, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#اگر_ختم

	    // When the window is too narrow draw all "@" lines.
	    اگر (wlv.draw_state != WL_سطر
#اگر_تعریف_شدہ FEAT_DIFF
		    && wlv.filler_todo <= 0
#اگر_ختم
		    )
	    {
		win_draw_انتہا(wp, '@', ' ', TRUE, wlv.صف, wp->w_اونچائی, HLF_AT);
		draw_vsep_win(wp, wlv.صف);
		wlv.صف = endrow;
	    }

	    // When line got too long for screen break here.
	    اگر (wlv.صف == endrow)
	    {
		++wlv.صف;
		توڑو;
	    }

	    اگر (screen_cur_صف == wlv.screen_صف - 1
#اگر_تعریف_شدہ FEAT_DIFF
		     && wlv.filler_todo <= 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		     && !متن_prop_above && !متن_prop_follows
#اگر_ختم
		     && wp->w_چوڑائی == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		سطر_Wraps[wlv.screen_صف - 1] = TRUE;

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
		اگر (p_tf
#اگر_تعریف_شدہ FEAT_GUI
			 && !gui.اندر_use
#اگر_ختم
			 && !(has_mbyte
			     && ((*mb_off2cells)(سطر_انحراف[wlv.screen_صف],
				   سطر_انحراف[wlv.screen_صف] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     سطر_انحراف[wlv.screen_صف - 1]
							    + (صحیح)topframe->fr_چوڑائی - 2,
				     سطر_انحراف[wlv.screen_صف]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    اگر (screen_cur_ستون != wp->w_چوڑائی)
			screen_char(سطر_انحراف[wlv.screen_صف - 1]
						       + (غیرعلامتی)topframe->fr_چوڑائی - 1,
				       wlv.screen_صف - 1, (صحیح)(topframe->fr_چوڑائی - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    اگر (has_mbyte && MB_BYTE2LEN(Screen_سطریں[سطر_انحراف[
				     wlv.screen_صف - 1] + (topframe->fr_چوڑائی - 1)]) > 1)
			برآمد_char(' ');
		    ورنہ
			برآمد_char(Screen_سطریں[سطر_انحراف[wlv.screen_صف - 1]
							    + (topframe->fr_چوڑائی - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[سطر_انحراف[wlv.screen_صف]] = (sattr_T)-1;
		    screen_آغاز();	// don't know where cursor is now
		}
	    }

	    win_سطر_آغاز(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    اگر (!wlv.dont_use_showbreak
# ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
# endif
	       )
		wlv.need_showbreak = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
	    --wlv.filler_todo;
	    // When the filler lines are actually below the last line of the
	    // file, don't draw the line itself, break here.
	    اگر (wlv.filler_todo == 0 && wp->w_botfill)
		توڑو;
#اگر_ختم
	}

    }	// for every character in the line
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (متن_props != متن_props_buf)
	vim_free(متن_props);
    اگر (متن_prop_idxs != متن_prop_idxs_buf)
	vim_free(متن_prop_idxs);
    اگر (متن_prop_suffix_flags != متن_prop_suffix_flags_buf)
	vim_free(متن_prop_suffix_flags);
    vim_free(p_extra_free2);
#اگر_ختم

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    واپس wlv.صف;
}
