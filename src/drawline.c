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

#ضمن "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    ساكن صحيح
get_lcs_ext(win_T *wp)
{
    إذا (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	أرجع NUL;
    إذا (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	أرجع '>';
    أرجع wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#اگر_تعریف_شده FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    ساكن صحيح
advance_color_ستون(صحيح vcol, صحيح **color_تعداد_ستون)
{
    طالما (**color_تعداد_ستون >= 0 && vcol > **color_تعداد_ستون)
	++*color_تعداد_ستون;
    أرجع (**color_تعداد_ستون >= 0);
}
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    ساكن فراغ
margin_columns_win(win_T *wp, صحيح *چپ_ستون, صحيح *راست_ستون)
{
    // cache previous calculations depending on w_virtcol
    ساكن صحيح saved_w_virtcol;
    ساكن win_T *prev_wp;
    ساكن صحيح prev_width1;
    ساكن صحيح prev_width2;
    ساكن صحيح prev_چپ_ستون;
    ساكن صحيح prev_راست_ستون;

    صحيح cur_ستون_off = win_ستون_off(wp);
    صحيح	width1;
    صحيح	width2;

    width1 = wp->w_عرض - cur_ستون_off;
    width2 = width1 + win_ستون_off2(wp);

    إذا (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*راست_ستون = prev_راست_ستون;
	*چپ_ستون = prev_چپ_ستون;
	أرجع;
    }

    *چپ_ستون = 0;
    *راست_ستون = width1;

    إذا (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*راست_ستون = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    إذا (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*چپ_ستون = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_چپ_ستون = *چپ_ستون;
    prev_راست_ستون = *راست_ستون;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#پایان_اگر

#إذا defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define سطر_ATTR
#پایان_اگر

// structure with variables passed between win_line() and other functions
تعريفنوع بنية {
    صحيح		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    صحيح		startrow;	// first row in the window to be drawn
    صحيح		ردیف;		// row in the window, excl w_winrow
    صحيح		screen_ردیف;	// row on the screen, incl w_winrow

    دراز	vcol;		// virtual column, before wrapping
    صحيح		ستون;		// visual column on screen, after wrapping
#اگر_تعریف_شده FEAT_CONCEAL
    صحيح		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    صحيح		vcol_off_co;	// offset for concealed characters
#پایان_اگر
    صحيح		vcol_off_tp;	// offset for virtual text
#اگر_تعریف_شده FEAT_SYN_HL
    صحيح		draw_color_ستون;	// highlight colorcolumn
    صحيح		*color_تعداد_ستون;	// pointer to according columns array
#پایان_اگر
    صحيح		eol_hl_off;	// 1 if highlighted char after EOL

    بیعلامت	off;		// offset in ScreenLines/ScreenAttrs

    صحيح		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#اگر_تعریف_شده FEAT_SYN_HL
    صحيح		cul_attr;	// set when 'cursorline' active
#پایان_اگر
#اگر_تعریف_شده سطر_ATTR
    صحيح		سطر_attr;	// for the whole line, includes 'cursorline'
#پایان_اگر

    صحيح		screen_سطر_flags;  // flags for screen_line()

    صحيح		fromcol;	// start of inverting
    صحيح		tocol;		// end of inverting

#اگر_تعریف_شده FEAT_LINEBREAK
    دراز	vcol_sbr;	    // virtual column after showbreak
    صحيح		need_showbreak;	    // overlong line, skipping first x chars
    صحيح		dont_use_showbreak; // do not use 'showbreak'
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
    صحيح		متن_prop_above_شمار;
#پایان_اگر

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    صحيح		cul_screenline;

    صحيح		char_attr;	// attributes for the next character

    صحيح		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    صحيح		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    صحيح		n_attr_skip;    // chars to skip before using extra_attr
    صحيح		c_extra;	// extra chars, all the same
    صحيح		c_final;	// final char, mandatory if set
    صحيح		extra_for_textprop; // n_extra set for textprop
    صحيح		شروع_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    صحيح		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    صحيح		saved_extra_attr;
    صحيح		saved_n_attr_skip;
    صحيح		saved_extra_for_textprop;
    صحيح		saved_c_extra;
    صحيح		saved_c_final;
    صحيح		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#اگر_تعریف_شده FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#پایان_اگر
    صحيح		filler_سطرها;	// nr of filler lines to be drawn
    صحيح		filler_todo;	// nr of filler lines still to do + 1
#اگر_تعریف_شده FEAT_SIGNS
    sign_attrs_T sattr;
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    صحيح		need_lbr;
#پایان_اگر
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#تعریف_کن WL_شروع	0		// nothing done yet, must be zero
#تعریف_کن WL_CMDLINE	(WL_شروع + 1)	// cmdline window column
#اگر_تعریف_شده FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#وإلا
# define WL_FOLD	WL_CMDLINE
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#وإلا
# define WL_SIGN	WL_FOLD		// column for signs
#پایان_اگر
#تعریف_کن WL_NR		(WL_SIGN + 1)	// line number
#اگر_تعریف_شده FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#وإلا
# define WL_BRI		WL_NR
#پایان_اگر
#إذا defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#وإلا
# define WL_SBR		WL_BRI
#پایان_اگر
#تعریف_کن WL_سطر		(WL_SBR + 1)	// text in the line

#إذا defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    ساكن صحيح
use_cursor_سطر_highlight(win_T *wp, linenr_T lnum)
{
    أرجع wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#پایان_اگر


#اگر_تعریف_شده FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    ساكن فراغ
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    صحيح fdc = compute_foldcolumn(wp, 0);

    إذا (fdc <= 0)
	أرجع;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    إذا (wlv->p_extra_free == عدم)
	أرجع;

    wlv->n_extra = (صحيح)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    إذا (use_cursor_سطر_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    وإلا
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_FC));
}
#پایان_اگر

#اگر_تعریف_شده FEAT_SIGNS
/*
 * Get information needed to display the sign in line "wlv->lnum" in window
 * "wp".
 * If "nrcol" is TRUE, the sign is going to be displayed in the number column.
 * Otherwise the sign is going to be displayed in the sign column.
 */
    ساكن فراغ
get_sign_display_info(
	صحيح		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    صحيح	متن_sign;
# ifdef FEAT_SIGN_ICONS
    صحيح	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    إذا (nrcol)
	wlv->n_extra = عدد_عرض(wp) + 1;
    وإلا
    {
	إذا (use_cursor_سطر_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	وإلا
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    إذا (wlv->ردیف == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_سطرها && wlv->filler_todo <= 0
# endif
       )
    {
	متن_sign = (wlv->sattr.sat_متن != عدم) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != عدم) ? wlv->sattr.sat_typenr : 0;
	إذا (gui.درون_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    إذا (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((حرف *)wlv->extra, حجم(wlv->extra),
						"%-*c ", عدد_عرض(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    وإلا
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    إذا (netbeans_فعال() && (buf_signcount(wp->w_میانگیر, wlv->lnum)
									  > 1))
	    {
		إذا (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((حرف *)wlv->extra, حجم(wlv->extra),
						"%-*c ", عدد_عرض(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		وإلا
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	وإلا
# endif
	    إذا (متن_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_متن;
		إذا (wlv->p_extra != عدم)
		{
		    إذا (nrcol)
		    {
			صحيح عرض = عدد_عرض(wp) - 2;

			vim_memset(wlv->extra, ' ', عرض);
			wlv->n_extra = عرض;
			wlv->n_extra += vim_snprintf((حرف *)wlv->extra + عرض,
				  حجم(wlv->extra) - عرض, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    وإلا
			wlv->n_extra = (صحيح)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		إذا (use_cursor_سطر_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		وإلا
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#پایان_اگر

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    ساكن فراغ
handle_lnum_ستون(
	win_T		*wp,
	winlinevars_T	*wlv,
	صحيح		sign_present استفادهنشده,
	صحيح		رقم_attr استفادهنشده)
{
    صحيح has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != عدم;
    صحيح lnum_ردیف = wlv->startrow + wlv->filler_سطرها
#اگر_تعریف_شده FEAT_PROP_POPUP
		      + wlv->متن_prop_above_شمار
#پایان_اگر
		      ;

    إذا ((wp->w_p_nu || wp->w_p_rnu)
	     && (wlv->ردیف <= lnum_ردیف || !has_cpo_n)
	     // there is no line number in a wrapped line when "n" is in
	     // 'cpoptions', but 'breakindent' assumes it anyway.
	     && !((has_cpo_n
#اگر_تعریف_شده FEAT_LINEBREAK
		     && !wp->w_p_bri
#پایان_اگر
		  ) && wp->w_skipcol > 0 && wlv->lnum == wp->w_topline))
    {
#اگر_تعریف_شده FEAT_SIGNS
	// If 'signcolumn' is set to 'number' and a sign is present
	// in 'lnum', then display the sign instead of the line
	// number.
	إذا ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_متن != عدم)
	    get_sign_display_info(TRUE, wp, wlv);
	وإلا
#پایان_اگر
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  إذا (wlv->ردیف == lnum_ردیف
		    && (wp->w_skipcol == 0 || wlv->ردیف > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      دراز رقم;
	      حرف *fmt = "%*ld ";

	      إذا (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  رقم = (دراز)wlv->lnum;
	      وإلا
	      {
		  // 'relativenumber', don't use negative numbers
		  رقم = labs((دراز)get_cursor_rel_lnum(wp, wlv->lnum));
		  إذا (رقم == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      رقم = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((حرف *)wlv->extra, حجم(wlv->extra), fmt, عدد_عرض(wp), رقم);
	      إذا (wp->w_skipcol > 0 && wlv->startrow == 0)
		  لكل (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#اگر_تعریف_شده FEAT_RIGHTLEFT
	      إذا (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  صحيح	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  لكل (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
		  {
		      t = *p1;
		      *p1 = *p2;
		      *p2 = t;
		  }
	      }
#پایان_اگر
	      wlv->p_extra = wlv->extra;
	      wlv->c_extra = NUL;
	      wlv->c_final = NUL;
	  }
	  وإلا
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = عدد_عرض(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#اگر_تعریف_شده FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  إذا (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->ردیف == lnum_ردیف
		      || (wlv->ردیف > lnum_ردیف
			 && (wp->w_p_culopt_flags & CULOPT_سطر))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#پایان_اگر
	  إذا (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  إذا (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#اگر_تعریف_شده FEAT_SIGNS
	إذا (رقم_attr)
	    wlv->char_attr = رقم_attr;
#پایان_اگر
    }
}

#اگر_تعریف_شده FEAT_LINEBREAK
    ساكن فراغ
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    إذا (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_مقدار(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    وإلا إذا (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    إذا (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	إذا (wp->w_p_bri && (wlv->ردیف > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_سطرها
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    إذا (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = عدم;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_میانگیر, wlv->lnum, FALSE));
	    إذا (wlv->ردیف == wlv->startrow)
	    {
		wlv->n_extra -= win_ستون_off2(wp);
		إذا (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    إذا (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    إذا (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	إذا (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#پایان_اگر

#إذا defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    ساكن فراغ
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    إذا (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	إذا (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	وإلا
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	إذا (wp->w_p_rl)
	    wlv->n_extra = wlv->ستون + 1;
	وإلا
#  endif
	    wlv->n_extra = wp->w_عرض - wlv->ستون;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_مقدار(wp);
    إذا (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (صحيح)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	إذا (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	إذا (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	إذا (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    إذا (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#پایان_اگر

#إذا defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    ساكن صحيح
textprop_اندازه_after_trunc(
	win_T	*wp,
	صحيح	flags,	    // TP_FLAG_ALIGN_*
	صحيح	added,
	صحيح	padding,
	char_u	*متن,
	صحيح	*n_استفاده_شده_ptr)
{
    صحيح	space = (flags & (TP_FLAG_ALIGN_زیر | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_عرض - win_ستون_off(wp) : added;
    صحيح strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    إذا (space < PROP_متن_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_عرض;
    إذا (flags & (TP_FLAG_ALIGN_زیر | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    لكل (p = متن; *p != NUL; p += (*mb_ptr2len)(p))
    {
	صحيح clen = ptr2cells(p);

	إذا (strsize + clen > space)
	    اكسر;
	strsize += clen;
    }
    *n_استفاده_شده_ptr = (صحيح)(p - متن);

    أرجع strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    صحيح
متن_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	صحيح	    vcol,	    // current text column
	صحيح	    scr_ستون,	    // current screen column
	صحيح	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	صحيح	    *n_attr,	    // attribute cells, NULL if not used
	صحيح	    *n_attr_skip,   // cells to skip attr, NULL if not used
	صحيح	    do_skip)	    // skip_cells is not zero
{
    صحيح	    راست_2 = (tp->tp_flags & TP_FLAG_ALIGN_راست);
    صحيح	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    صحيح	    زیر = (tp->tp_flags & TP_FLAG_ALIGN_زیر);
    صحيح	    wrap = tp->tp_ستون < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    صحيح	    padding = tp->tp_ستون == MAXCOL ? tp->tp_padleft : 0;
    صحيح	    ستون_with_padding = scr_ستون + (زیر ? 0 : padding);
    صحيح	    جا = wp->w_عرض - ستون_with_padding;
    صحيح	    before = جا;	// spaces before the text
    صحيح	    after = 0;		// spaces after the text
    صحيح	    n_استفاده_شده = *n_extra;
    char_u  *l = عدم;
    صحيح	    strsize = vim_strsize(*p_extra);
    صحيح	    cells = wrap ? strsize : textprop_اندازه_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_استفاده_شده);

    إذا (wrap || راست_2 || above || زیر || padding > 0 || n_استفاده_شده < *n_extra)
    {
	صحيح	    ستون_off = win_ستون_off(wp) - win_ستون_off2(wp);

	إذا (above)
	{
	    before = 0;
	    after = wp->w_عرض - cells - win_ستون_off(wp) - padding;
	    إذا (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	وإلا
	{
	    // Right-align: fill with before
	    إذا (راست_2)
		before -= cells;

	    // Below-align: empty line add one character
	    إذا (زیر && vcol == 0 && ستون_with_padding == ستون_off
					    && wp->w_عرض - ستون_off == before)
		ستون_with_padding += 1;

	    إذا (before < 0
		    || !(راست_2 || زیر)
		    || (زیر ? (ستون_with_padding <= ستون_off || !wp->w_p_wrap)
			      : (n_استفاده_شده < *n_extra)))
	    {
		إذا (راست_2 && (wrap
			      || (جا < PROP_متن_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_عرض - ستون_off - strsize + جا;
		    إذا (before < 0)
			before = 0;
		    وإلا
			n_استفاده_شده = *n_extra;
		}
		وإلا إذا (زیر && before > vcol && do_skip)
		    before -= vcol;
		وإلا
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	إذا (n_استفاده_شده < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_استفاده_شده;

	// add 1 for NUL, 2 for when '…' is used
	إذا (n_attr != عدم)
	    l = alloc(n_استفاده_شده + before + after + (padding > 0 ? padding : 0) + 3);
	إذا (n_attr == عدم || l != عدم)
	{
	    صحيح off = 0;

	    إذا (n_attr != عدم)
	    {
		vim_memset(l, ' ', before);
		off += before;
		إذا (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_استفاده_شده);
		off += n_استفاده_شده;
	    }
	    وإلا
	    {
		off = before + after + padding + n_استفاده_شده;
		cells += before + after + padding;
	    }
	    إذا (n_attr != عدم)
	    {
		إذا (n_استفاده_شده < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    إذا (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			إذا (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_راهاندازی(&vc, (char_u *)"utf-8", p_enc);
			    إذا (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, عدم);
				إذا (cp == عدم)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_راهاندازی(&vc, عدم, عدم);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_سر_off)(l, lp);
			STRCPY(lp, cp);
			n_استفاده_شده = lp - l + 3 - before - padding;
			إذا (cp != buf)
			    vim_free(cp);
		    }
		    وإلا
			// change last character to '>'
			*lp = '>';
		}
		وإلا إذا (after > 0)
		{
		    vim_memset(l + off, ' ', after);
		    l[off + after] = NUL;
		}

		*p_extra = l;
		*n_extra = n_استفاده_شده + before + after + padding;
		*n_attr = mb_charlen(*p_extra);
		// n_attr_skip will not be decremented before draw_state is
		// WL_LINE
		*n_attr_skip = before + (padding > 0 ? padding : 0);
		*n_attr -= *n_attr_skip;
		إذا (above)
		    *n_attr -= after;
	    }
	}
    }

    إذا (n_attr == عدم)
	أرجع cells;
    أرجع (زیر && ستون_with_padding > win_ستون_off(wp) && !wp->w_p_wrap);
}

# إذا defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    صحيح
متن_prop_no_showbreak(textprop_T *tp)
{
    صحيح	    راست_2 = (tp->tp_flags & TP_FLAG_ALIGN_راست);
    صحيح	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    صحيح	    زیر = (tp->tp_flags & TP_FLAG_ALIGN_زیر);
    صحيح	    wrap = tp->tp_ستون < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    أرجع (راست_2 || above || زیر || !wrap);
}
# endif
#پایان_اگر

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    ساكن فراغ
wlv_screen_سطر(win_T *wp, winlinevars_T *wlv, صحيح clear_انتها)
{
    إذا (wlv->ردیف == 0 && wp->w_skipcol > 0
#إذا defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_مقدار(wp) == NUL
#پایان_اگر
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	صحيح off = (صحيح)(current_Screen_سطر - Screen_سطرها);
	صحيح max_off = off + screen_Columns;
	صحيح skip = 0;

	إذا (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    طالما (skip < wp->w_عرض && VIM_ISDIGIT(Screen_سطرها[off]))
	    {
		++off;
		++skip;
	    }

	لكل (صحيح i = 0; i < 3 && i + skip < wp->w_عرض; ++i)
	{
	    إذا ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_سطرها[off + 1] = ' ';
	    Screen_سطرها[off] = '<';
	    إذا (enc_utf8)
		Screen_سطرها_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_سطر(wp, wlv->screen_ردیف, wp->w_wincol, wlv->ستون,
		    clear_انتها ? wp->w_عرض : -wp->w_عرض,
		    wlv->vcol - 1, wlv->screen_سطر_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    ساكن فراغ
draw_screen_سطر(win_T *wp, winlinevars_T *wlv)
{
#اگر_تعریف_شده FEAT_SYN_HL
    دراز	v;
    صحيح		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    إذا (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    وإلا
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_عرض - wlv->ستون - 1 :
# endif
	wlv->ستون;
    // check if line ends before left margin
    إذا (wlv->vcol < v + wcol - win_ستون_off(wp))
	wlv->vcol = v + wcol - win_ستون_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->ستون -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# وإلا
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    إذا (wlv->draw_color_ستون)
	wlv->draw_color_ستون = advance_color_ستون(VCOL_HLC, &wlv->color_تعداد_ستون);

    إذا (((wp->w_p_cuc
		    && (صحيح)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (صحيح)wp->w_virtcol <
			 (دراز)wp->w_عرض * (wlv->ردیف - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_ستون
# ifdef سطر_ATTR
		|| wlv->سطر_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	صحيح	rightmost_vcol = 0;
	صحيح	i;

	إذا (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	إذا (wlv->draw_color_ستون)
	    // determine rightmost colorcolumn to possibly draw
	    لكل (i = 0; wlv->color_تعداد_ستون[i] >= 0; ++i)
		إذا (rightmost_vcol < wlv->color_تعداد_ستون[i])
		    rightmost_vcol = wlv->color_تعداد_ستون[i];

	طالما (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->ستون >= 0) :
# endif
		(wlv->ستون < wp->w_عرض))
	{
	    Screen_سطرها[wlv->off] = ' ';
	    إذا (enc_utf8)
		Screen_سطرها_UC[wlv->off] = 0;

	    إذا (wlv->draw_color_ستون)
		wlv->draw_color_ستون = advance_color_ستون(
						   VCOL_HLC, &wlv->color_تعداد_ستون);

	    صحيح attr = wlv->win_attr;
# ifdef سطر_ATTR
	    إذا (wlv->سطر_attr != 0)
		attr = hl_combine_attr(attr, wlv->سطر_attr);
# endif
	    إذا (wp->w_p_cuc && VCOL_HLC == (دراز)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    وإلا إذا (wlv->draw_color_ستون && VCOL_HLC == *wlv->color_تعداد_ستون)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    Screen_تعداد_ستون[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->ستون;
	    }
	    وإلا
# endif
	    {
		++wlv->off;
		++wlv->ستون;
	    }
	    ++wlv->vcol;

	    إذا (VCOL_HLC > rightmost_vcol
# ifdef سطر_ATTR
		    && wlv->سطر_attr == 0
# endif
		    && wlv->win_attr == 0)
		اكسر;
	}
    }
#پایان_اگر

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_سطر_flags |= SLF_INC_VCOL;
    wlv_screen_سطر(wp, wlv, TRUE);
    wlv->screen_سطر_flags &= ~SLF_INC_VCOL;
    ++wlv->ردیف;
    ++wlv->screen_ردیف;
}
#لغو_تعریف VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    ساكن فراغ
win_سطر_شروع(win_T *wp استفادهنشده, winlinevars_T *wlv, صحيح save_extra)
{
    wlv->ستون = 0;
    wlv->off = (بیعلامت)(current_Screen_سطر - Screen_سطرها);
#اگر_تعریف_شده FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#پایان_اگر

#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->ستون = wp->w_عرض - 1;
	wlv->off += wlv->ستون;
	wlv->screen_سطر_flags |= SLF_RIGHTLEFT;
    }
#پایان_اگر
    إذا (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_شروع;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = عدم;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#اگر_تعریف_شده FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
	إذا (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	وإلا
#پایان_اگر
	    wlv->saved_char_attr = 0;

	// these are not used until restored in win_line_continue()
	wlv->n_extra = 0;
	wlv->n_attr_skip = 0;
    }
}

/*
 * Called when wlv->draw_state is set to WL_LINE.
 */
    ساكن فراغ
win_سطر_continue(winlinevars_T *wlv)
{
    إذا (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = عدم;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    وإلا
	wlv->char_attr = wlv->win_attr;
}

#اگر_تعریف_شده FEAT_SYN_HL
    ساكن فراغ
apply_cursorline_highlight(
	winlinevars_T *wlv,
	صحيح sign_present استفادهنشده)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    إذا (sign_present && wlv->sattr.sat_linehl > 0)
    {
	إذا (wlv->sattr.sat_اولویت >= 100)
	    wlv->سطر_attr = hl_combine_attr(wlv->cul_attr, wlv->سطر_attr);
	وإلا
	    wlv->سطر_attr = hl_combine_attr(wlv->سطر_attr, wlv->cul_attr);
    }
    وإلا
# endif
# إذا defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->سطر_attr = hl_combine_attr(wlv->cul_attr, wlv->سطر_attr);
# وإلا
	wlv->سطر_attr = wlv->cul_attr;
# endif
}
#پایان_اگر

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
    صحيح
win_سطر(
    win_T	*wp,
    linenr_T	lnum,
    صحيح		startrow,
    صحيح		endrow,
    صحيح		عدد_only,
    spellvars_T	*spv استفادهنشده)
{
    winlinevars_T	wlv;		// variables passed between functions

    صحيح		c = 0;			// init for GCC
    char_u	*سطر;			// current line
    char_u	*ptr;			// current position in "line"
    صحيح		درون_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#اگر_تعریف_شده FEAT_PROP_POPUP
    char_u	*p_extra_free2 = عدم;   // another p_extra to be freed
#پایان_اگر
#إذا defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    صحيح		درون_linebreak = FALSE;	// n_extra set for showing linebreak
#پایان_اگر
    صحيح		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    صحيح		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    صحيح		n_attr = 0;	    // chars with special attr
    صحيح		saved_attr2 = 0;    // char_attr saved for n_attr
    صحيح		n_attr3 = 0;	    // chars with overruling special attr
    صحيح		saved_attr3 = 0;    // char_attr saved for n_attr3

    صحيح		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    صحيح		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    صحيح		lnum_درون_visual_area = FALSE;
    pos_T	pos;
    دراز	v;

    صحيح		attr_pri = FALSE;	// char_attr has priority
    صحيح		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    صحيح		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    صحيح		area_attr = 0;		// attributes desired by highlighting
    صحيح		جستجو_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#اگر_تعریف_شده FEAT_SYN_HL
    صحيح		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    صحيح		syntax_attr = 0;	// attributes desired by syntax
    صحيح		prev_syntax_ستون = -1;	// column of prev_syntax_attr
    صحيح		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    صحيح		has_syntax = FALSE;	// this buffer has syntax highl.
    صحيح		save_did_emsg;
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
# define WIN_سطر_متن_PROP_پشته_LEN 32
    صحيح		did_سطر = FALSE;	// set to TRUE when line text done
    صحيح		متن_prop_شمار;
    صحيح		آخرین_textprop_متن_idx = -1;
    صحيح		متن_prop_بعدی = 0;	// next text property to use
    textprop_T	متن_props_buf[WIN_سطر_متن_PROP_پشته_LEN];
    صحيح		متن_prop_idxs_buf[WIN_سطر_متن_PROP_پشته_LEN];
    char_u	متن_prop_suffix_flags_buf[WIN_سطر_متن_PROP_پشته_LEN + 1];
    textprop_T	*متن_props = عدم;
    صحيح		*متن_prop_idxs = عدم;
    char_u	*متن_prop_suffix_flags = عدم;
    صحيح		متن_props_فعال = 0;
    صحيح		متن_props_need_مرتب_سازی = FALSE;
    proptype_T  *متن_prop_type = عدم;
    صحيح		متن_prop_attr = 0;
    صحيح		متن_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    صحيح		متن_prop_id = 0;	// active property ID
    صحيح		متن_prop_flags = 0;
    صحيح		متن_prop_above = FALSE;  // first doing virtual text above
    صحيح		متن_prop_follows = FALSE;  // another text prop to display
    صحيح		saved_جستجو_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    صحيح		saved_area_attr = 0;	// idem for area_attr
    صحيح		reset_extra_attr = FALSE;
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
    صحيح		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    صحيح		nextlinecol = 0;	// column where nextline[] starts
    صحيح		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    صحيح		spell_attr = 0;		// attributes desired by spelling
    صحيح		کلمه_انتها = 0;		// last byte with same spell_attr
    صحيح		cur_checked_ستون = 0;	// checked column for current line
#پایان_اگر
    صحيح		extra_check = FALSE;	// has extra highlighting
    صحيح		multi_attr = 0;		// attributes desired by multibyte
    صحيح		mb_l = 1;		// multi-byte byte length
    صحيح		mb_c = 0;		// decoded multi-byte character
    صحيح		mb_utf8 = FALSE;	// screen char is UTF-8 char
    صحيح		u8cc[MAX_MCO];		// composing UTF-8 chars
#اگر_تعریف_شده FEAT_DIFF
    صحيح		change_شروع = MAXCOL;	// first col of changed area
    صحيح		change_انتها = -1;	// last col of changed area
    diffline_T	سطر_changes;
    صحيح		change_شاخص = -1;
#پایان_اگر
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    صحيح		درون_multispace = FALSE;	// in multiple consecutive spaces
    صحيح		multispace_pos = 0;	// position in lcs-multispace string
#اگر_تعریف_شده سطر_ATTR
    صحيح		سطر_attr_save = 0;
#پایان_اگر
    صحيح		sign_present = FALSE;
    صحيح		رقم_attr = 0;		// attribute for the number column
#اگر_تعریف_شده FEAT_ARABIC
    صحيح		prev_c = 0;		// previous Arabic character
    صحيح		prev_c1 = 0;		// first composing char for prev_c
#پایان_اگر
#إذا defined(سطر_ATTR)
    صحيح		did_سطر_attr = 0;
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    صحيح		get_جمله_attr = FALSE;
#پایان_اگر

#إذا defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    صحيح		چپ_curline_ستون = 0;
    صحيح		راست_curline_ستون = 0;
#پایان_اگر

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    صحيح		feedback_ستون = 0;
    صحيح		feedback_old_attr = -1;
#پایان_اگر

#إذا defined(FEAT_CONCEAL) || defined(FEAT_جستجو_EXTRA)
    صحيح		تطابق_conc	= 0;	// cchar for match functions
#پایان_اگر
#إذا defined(FEAT_CONCEAL) || defined(FEAT_جستجو_EXTRA) || defined(FEAT_LINEBREAK)
    صحيح		on_آخرین_ستون     = FALSE;
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
    صحيح		syntax_flags	= 0;
    صحيح		syntax_seqnr	= 0;
    صحيح		prev_syntax_id	= 0;
    صحيح		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    صحيح		is_concealing	= FALSE;
    صحيح		did_wcol	= FALSE;
    صحيح		old_boguscols   = 0;
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
#وإلا
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#پایان_اگر

    إذا (startrow > endrow)		// past the end already!
	أرجع startrow;

    CLEAR_فیلد(wlv);

    wlv.lnum = lnum;
    wlv.startrow = startrow;
    wlv.ردیف = startrow;
    wlv.screen_ردیف = wlv.ردیف + W_WINROW(wp);
    wlv.fromcol = -10;
    wlv.tocol = MAXCOL;
#اگر_تعریف_شده FEAT_LINEBREAK
    wlv.vcol_sbr = -1;
#پایان_اگر

    إذا (عدد_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#اگر_تعریف_شده FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
	إذا (syntax_present(wp) && !wp->w_s->b_syn_error
# ifdef SYN_زمان_حد
		&& !wp->w_s->b_syn_slow
# endif
	   )
	{
	    // Prepare for syntax highlighting in this line.  When there is an
	    // error, stop syntax highlighting.
	    save_did_emsg = did_emsg;
	    did_emsg = FALSE;
	    syntax_شروع(wp, lnum);
	    إذا (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    وإلا
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_زمان_حد
		إذا (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_تعداد_ستون = wp->w_p_cc_تعداد_ستون;
	إذا (wlv.color_تعداد_ستون != عدم)
	    wlv.draw_color_ستون = advance_color_ستون(VCOL_HLC, &wlv.color_تعداد_ستون);
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMINAL
	إذا (جمله_show_میانگیر(wp->w_میانگیر))
	{
	    extra_check = TRUE;
	    get_جمله_attr = TRUE;
	    wlv.win_attr = جمله_get_attr(wp, lnum, -1);
	}
#پایان_اگر

	// handle Visual active in this window
	إذا (VIsual_فعال && wp->w_میانگیر == curwin->w_میانگیر)
	{
	    pos_T	*بالاترین, *bot;

	    إذا (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		بالاترین = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    وإلا
	    {
		// Visual is before curwin->w_cursor
		بالاترین = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_درون_visual_area = (lnum >= بالاترین->lnum && lnum <= bot->lnum);
	    إذا (VIsual_نما == Ctrl_V)
	    {
		// block mode
		إذا (lnum_درون_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    وإلا
	    {
		// non-block mode
		إذا (lnum > بالاترین->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		وإلا إذا (lnum == بالاترین->lnum)
		{
		    إذا (VIsual_نما == 'V')	// linewise
			wlv.fromcol = 0;
		    وإلا
		    {
			getvvcol(wp, بالاترین, &wlv.fromcol, عدم, عدم, 0);
			إذا (gchar_pos(بالاترین) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		إذا (VIsual_نما != 'V' && lnum == bot->lnum)
		{
		    إذا (*p_sel == 'e' && bot->ستون == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    وإلا إذا (bot->ستون == MAXCOL)
			wlv.tocol = MAXCOL;
		    وإلا
		    {
			pos = *bot;
			إذا (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, عدم, عدم, 0);
			وإلا
			{
			    getvvcol(wp, &pos, عدم, عدم, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    إذا (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#إذا defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		إذا ((clip_ستاره.موجود && !clip_ستاره.owned
						    && clip_isautosel_ستاره())
			|| (clip_plus.موجود && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#پایان_اگر
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	وإلا إذا (highlight_تطابق
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + جستجو_تطابق_سطرها)
	{
	    إذا (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, عدم, عدم, 0);
	    وإلا
		wlv.fromcol = 0;
	    إذا (lnum == curwin->w_cursor.lnum + جستجو_تطابق_سطرها)
	    {
		pos.lnum = lnum;
		pos.ستون = جستجو_تطابق_endcol;
		getvcol(curwin, &pos, &wlv.tocol, عدم, عدم, 0);
	    }
	    وإلا
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    إذا (wlv.fromcol == wlv.tocol && جستجو_تطابق_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#اگر_تعریف_شده FEAT_DIFF

    صحيح linestatus = 0;
    wlv.filler_سطرها = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_فیلد(سطر_changes);

    إذا (linestatus < 0)
    {
	إذا (linestatus == -1)
	{
	    إذا (diff_پیدا_کن_change(wp, lnum, &سطر_changes))
	    {
		wlv.diff_hlf = HLF_جمع_کن;	// added line
	    }
	    وإلا إذا (سطر_changes.رقم_changes > 0)
	    {
		صحيح added = diff_change_parse(
			&سطر_changes, &سطر_changes.changes[0],
			&change_شروع, &change_انتها);
		إذا (change_شروع == 0)
		{
		    إذا (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    وإلا
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		وإلا
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_شاخص = 0;
	    }
	    وإلا
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_شاخص = 0;
	    }
	}
	وإلا
	    wlv.diff_hlf = HLF_جمع_کن;

	area_highlighting = TRUE;
    }

    إذا (lnum == wp->w_topline)
	wlv.filler_سطرها = wp->w_topfill;

    wlv.filler_todo = wlv.filler_سطرها;
#پایان_اگر

#اگر_تعریف_شده FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    إذا (sign_present)
	رقم_attr = wlv.sattr.sat_numhl;
#پایان_اگر

#اگر_تعریف_شده سطر_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    إذا (sign_present)
	wlv.سطر_attr = wlv.sattr.sat_linehl;
# endif
# إذا defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    إذا (bt_quickfix(wp->w_میانگیر) && qf_current_مدخل(wp) == lnum)
	wlv.سطر_attr = HL_ATTR(HLF_QFL);
# endif
    إذا (wlv.سطر_attr != 0)
	area_highlighting = TRUE;
#پایان_اگر

#اگر_تعریف_شده FEAT_SPELL
    إذا (spv->spv_has_spell && عدد_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	إذا (lnum == spv->spv_checked_lnum)
	    cur_checked_ستون = spv->spv_checked_ستون;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	إذا (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_ستون = 0;
	وإلا إذا (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_ستون = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	إذا (lnum < wp->w_میانگیر->b_ml.ml_سطر_شمار)
	{
	    سطر = ml_get_buf(wp->w_میانگیر, lnum + 1, FALSE);
	    spell_cat_سطر(nextline + SPWORDLEN, سطر, SPWORDLEN);
	}
	سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(سطر);
	إذا (*ptr == NUL)
	{
	    spv->spv_cap_ستون = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	وإلا إذا (spv->spv_cap_ستون == 0)
	    spv->spv_cap_ستون = ptr - سطر;

	// Copy the end of the current line into nextline[].
	إذا (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	وإلا
	{
	    v = ml_get_buf_len(wp->w_میانگیر, lnum);
	    إذا (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, سطر, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    وإلا
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, سطر + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#پایان_اگر

    سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);
    ptr = سطر;

    إذا (wp->w_p_list)
    {
	إذا (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != عدم
		|| wp->w_lcs_chars.leadmultispace != عدم
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	إذا (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_میانگیر, lnum);
	    طالما (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - سطر);
	}
	// find end of leading whitespace
	إذا (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != عدم ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    طالما (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    إذا (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    وإلا
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - سطر) + 1;
	}
    }

    إذا (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    صحيح skipcol_درون_متن_prop_above = 0;

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (WIN_IS_POPUP(wp))
	wlv.screen_سطر_flags |= SLF_POPUP;

    char_u *prop_شروع;
    متن_prop_شمار = get_متن_props(wp->w_میانگیر, lnum, &prop_شروع, FALSE);
    إذا (متن_prop_شمار > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	إذا (متن_prop_شمار <= WIN_سطر_متن_PROP_پشته_LEN)
	    متن_props = متن_props_buf;
	وإلا
	    متن_props = ALLOC_MULT(textprop_T, متن_prop_شمار);
	إذا (متن_props != عدم)
	{
	    mch_memmove(متن_props, prop_شروع,
				     متن_prop_شمار * حجم(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *شمار_ptr = prop_شروع - PROP_شمار_اندازه;

	    لكل (صحيح i = 0; i < متن_prop_شمار; ++i)
	    {
		إذا (متن_props[i].tp_id < 0
				&& متن_props[i].u.tp_متن_جابه_جایی > 0)
		{
		    متن_props[i].u.tp_متن =
			    شمار_ptr + متن_props[i].u.tp_متن_جابه_جایی;
		    متن_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		وإلا
		    متن_props[i].u.tp_متن = عدم;
	    }
	}

	// Allocate an array for the indexes.
	إذا (متن_prop_شمار <= WIN_سطر_متن_PROP_پشته_LEN)
	    متن_prop_idxs = متن_prop_idxs_buf;
	وإلا
	    متن_prop_idxs = ALLOC_MULT(صحيح, متن_prop_شمار);
	إذا (متن_prop_idxs == عدم)
	{
	    إذا (متن_props != متن_props_buf)
		VIM_CLEAR(متن_props);
	    وإلا
		متن_props = عدم;
	}
	إذا (متن_prop_شمار <= WIN_سطر_متن_PROP_پشته_LEN)
	    متن_prop_suffix_flags = متن_prop_suffix_flags_buf;
	وإلا
	    متن_prop_suffix_flags = ALLOC_MULT(char_u, متن_prop_شمار + 1);
	إذا (متن_prop_suffix_flags == عدم)
	{
	    إذا (متن_prop_idxs != متن_prop_idxs_buf)
		vim_free(متن_prop_idxs);
	    متن_prop_idxs = عدم;
	    إذا (متن_props != متن_props_buf)
		VIM_CLEAR(متن_props);
	    وإلا
		متن_props = عدم;
	}

	إذا (متن_props != عدم)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    لكل (صحيح i = 0; i < متن_prop_شمار; ++i)
		إذا (متن_props[i].tp_id < 0)
		    آخرین_textprop_متن_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    صحيح متن_عرض = wp->w_عرض - win_ستون_off(wp);
	    لكل (صحيح i = متن_prop_شمار - 1; i >= 0; --i)
		إذا (متن_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    إذا (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_درون_متن_prop_above
								 >= متن_عرض)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_درون_متن_prop_above += متن_عرض;
			لكل (صحيح j = i + 1; j < متن_prop_شمار; ++j)
			    متن_props[j - 1] = متن_props[j];
			++i;
			--متن_prop_شمار;
		    }
		    وإلا
			++wlv.متن_prop_above_شمار;
		}

	    متن_prop_suffix_flags[متن_prop_شمار] = 0;
	    لكل (صحيح i = متن_prop_شمار - 1; i >= 0; --i)
	    {
		صحيح flags = متن_prop_suffix_flags[i + 1];

		إذا (متن_props[i].tp_ستون == MAXCOL)
		{
		    flags |= 1;
		    إذا (متن_props[i].tp_flags & TP_FLAG_ALIGN_زیر)
			flags |= 2;
		}
		متن_prop_suffix_flags[i] = flags;
	    }
	}
    }

    إذا (عدد_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.ردیف += wlv.متن_prop_above_شمار;
	إذا (wlv.ردیف >= endrow)
	{
	    إذا (متن_props != متن_props_buf)
		vim_free(متن_props);
	    إذا (متن_prop_idxs != متن_prop_idxs_buf)
		vim_free(متن_prop_idxs);
	    إذا (متن_prop_suffix_flags != متن_prop_suffix_flags_buf)
		vim_free(متن_prop_suffix_flags);
	    أرجع wlv.ردیف;
	}
	wlv.screen_ردیف += wlv.متن_prop_above_شمار;
    }
#پایان_اگر

#إذا defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_اولی_char = 0;
    إذا (wp->w_p_lbr && عدد_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
	(فراغ)win_lbr_chartabsize(&cts, عدم, عدم);
	vcol_اولی_char = cts.cts_اولی_char;
	clear_chartabsize_arg(&cts);
    }
#پایان_اگر

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    إذا (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_درون_متن_prop_above : 0;
    وإلا
	v = wp->w_leftcol;
    إذا (v > 0 && عدد_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	صحيح		charsize = 0;
	صحيح		سر = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, سطر, ptr);
	cts.cts_max_سر_vcol = v;
	طالما (cts.cts_vcol < v)
	{
	    سر = 0;
	    charsize = win_lbr_chartabsize(&cts, &سر, عدم);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    إذا (*prev_ptr == NUL)
		اكسر;
	    MB_PTR_ADV(cts.cts_ptr);
	    إذا (wp->w_p_list)
	    {
		درون_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > سطر && prev_ptr[-1] == ' '));
		إذا (!درون_multispace)
		    multispace_pos = 0;
		وإلا إذا (cts.cts_ptr >= سطر + leadcol
					 && wp->w_lcs_chars.multispace != عدم)
		{
		    ++multispace_pos;
		    إذا (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		وإلا إذا (cts.cts_ptr < سطر + leadcol
				     && wp->w_lcs_chars.leadmultispace != عدم)
		{
		    ++multispace_pos;
		    إذا (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	إذا (wlv.vcol < v && (
#اگر_تعریف_شده FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_ستون ||
#پایان_اگر
	     virtual_فعال() ||
	     (VIsual_فعال && wp->w_میانگیر == curwin->w_میانگیر)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	إذا (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	إذا (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - سر;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	إذا (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	وإلا إذا (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#اگر_تعریف_شده FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	إذا (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	إذا (spv->spv_has_spell)
	{
	    صحيح		len;
	    colnr_T	linecol = (colnr_T)(ptr - سطر);
	    hlf_T	spell_hlf = HLF_شمار;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.ستون = linecol;
	    len = spell_move_مقصد(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);
	    ptr = سطر + linecol;

	    إذا (len == 0 || (صحيح)wp->w_cursor.ستون > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		//کلمه
		spell_hlf = HLF_شمار;
		کلمه_انتها = (صحيح)(spell_مقصد_کلمه_انتها(ptr, wp) - سطر + 1);
	    }
	    وإلا
	    {
		// bad word found, use attributes until end of word
		کلمه_انتها = wp->w_cursor.ستون + len + 1;

		// Turn index into actual attributes.
		إذا (spell_hlf != HLF_شمار)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    إذا (has_syntax)
		syntax_شروع(wp, lnum);
# endif
	}
#پایان_اگر
    }

    إذا (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#اگر_تعریف_شده FEAT_جستجو_EXTRA
    إذا (عدد_only == 0)
    {
	v = (دراز)(ptr - سطر);
	area_highlighting |= prepare_جستجو_hl_سطر(wp, lnum, (colnr_T)v,
					      &سطر, &screen_جستجو_hl,
					      &جستجو_attr);
	ptr = سطر + v; // "line" may have been updated
    }
#پایان_اگر

    إذا ((State & نما_درج) && ins_compl_win_فعال(wp)
			    && (درون_curline || ins_compl_lnum_درون_range(lnum)))
	area_highlighting = TRUE;

#اگر_تعریف_شده FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    إذا (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	إذا (!(wp == curwin && VIsual_فعال)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    إذا (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    وإلا
	    {
		سطر_attr_save = wlv.سطر_attr;
		margin_columns_win(wp, &چپ_curline_ستون, &راست_curline_ستون);
	    }
	    area_highlighting = TRUE;
	}
    }
#پایان_اگر

    win_سطر_شروع(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    لكل (;;)
    {
#إذا defined(FEAT_CONCEAL) || defined(FEAT_جستجو_EXTRA)
	صحيح	has_تطابق_conc = 0;	// match wants to conceal
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
	صحيح	did_decrement_ptr = FALSE;
#پایان_اگر

	// Skip this quickly when working on the text.
	إذا (wlv.draw_state != WL_سطر)
	{
#اگر_تعریف_شده FEAT_SYN_HL
	    إذا (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.سطر_attr = سطر_attr_save;
	    }
#پایان_اگر
	    إذا (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		إذا (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    إذا (wlv.ردیف == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    وإلا
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#اگر_تعریف_شده FEAT_FOLDING
	    إذا (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
	    إذا (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		إذا (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#پایان_اگر
	    إذا (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_ستون(wp, &wlv, sign_present, رقم_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    إذا (عدد_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_سطر(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		إذا ((wlv.ردیف + 1 - wlv.startrow < عدد_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#اگر_تعریف_شده FEAT_DIFF
			|| wlv.filler_todo > 0
#پایان_اگر
			)
		{
		    ++wlv.ردیف;
		    ++wlv.screen_ردیف;
		    إذا (wlv.ردیف == endrow)
			اكسر;
#اگر_تعریف_شده FEAT_DIFF
		    --wlv.filler_todo;
		    إذا (wlv.filler_todo == 0 && wp->w_botfill)
			اكسر;
#پایان_اگر
		    win_سطر_شروع(wp, &wlv, TRUE);
		    استمر;
		}
		وإلا
		    اكسر;
	    }

#اگر_تعریف_شده FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    إذا (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#پایان_اگر
#إذا defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    إذا (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#پایان_اگر
	    إذا (wlv.draw_state == WL_سطر - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_سطر;
		win_سطر_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#اگر_تعریف_شده FEAT_SYN_HL
	إذا (wlv.cul_screenline && wlv.draw_state == WL_سطر
		&& wlv.vcol >= چپ_curline_ستون
		&& wlv.vcol < راست_curline_ستون)
	    apply_cursorline_highlight(&wlv, sign_present);
#پایان_اگر

	// When still displaying '$' of change command, stop at cursor.
	إذا (dollar_vcol >= 0 && درون_curline && wlv.vcol >= (دراز)wp->w_virtcol)
	{
	    wlv_screen_سطر(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#اگر_تعریف_شده FEAT_SYN_HL
	    إذا (wp->w_p_cuc)
		wlv.ردیف = wp->w_cline_ردیف + wp->w_cline_ارتفاع;
	    وإلا
#پایان_اگر
		wlv.ردیف = wp->w_ارتفاع;
	    اكسر;
	}

	إذا (wlv.draw_state == WL_سطر && (area_highlighting || extra_check))
	{
#اگر_تعریف_شده FEAT_PROP_POPUP
	    إذا (متن_props != عدم)
	    {
		صحيح pi;
		صحيح bcol = (صحيح)(ptr - سطر);

		إذا (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !درون_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		لكل (pi = 0; pi < متن_props_فعال; ++pi)
		{
		    صحيح		tpi = متن_prop_idxs[pi];
		    textprop_T  *tp = &متن_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    إذا ((tp->tp_ستون != MAXCOL
				       && bcol >= tp->tp_ستون - 1 + tp->tp_len))
		    {
			إذا (pi + 1 < متن_props_فعال)
			    mch_memmove(متن_prop_idxs + pi,
					متن_prop_idxs + pi + 1,
					حجم(صحيح)
					     * (متن_props_فعال - (pi + 1)));
			--متن_props_فعال;
			متن_props_need_مرتب_سازی = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			إذا (درون_linebreak && syntax_attr == متن_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		إذا (wlv.n_extra > 0 && درون_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		طالما (متن_prop_بعدی < متن_prop_شمار)
		{
		    صحيح فعال;
		    textprop_T *tp = &متن_props[متن_prop_بعدی];
		    إذا (tp->tp_ستون == MAXCOL)
		    {
			إذا (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    فعال = TRUE;
			وإلا إذا (*ptr != NUL)
			    اكسر;
			وإلا
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    فعال = wp->w_p_wrap
				  || wlv.ردیف == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_زیر);
			}
		    }
		    وإلا
		    {
			إذا (bcol < tp->tp_ستون - 1)
			    اكسر;
			فعال = bcol <= tp->tp_ستون - 1 + tp->tp_len;
		    }

		    إذا (فعال)
		    {
			متن_prop_idxs[متن_props_فعال++] = متن_prop_بعدی;
			متن_props_need_مرتب_سازی = TRUE;
		    }
		    ++متن_prop_بعدی;
		}

		إذا (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(متن_prop_type != عدم &&
			     متن_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    متن_prop_attr = 0;
		    متن_prop_attr_comb = 0;
		    متن_prop_flags = 0;
		    متن_prop_type = عدم;
		    متن_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		إذا (متن_props_فعال > 0 && wlv.n_extra == 0)
		{
		    صحيح استفاده_شده_tpi = -1;
		    صحيح استفاده_شده_attr = 0;
		    صحيح other_tpi = -1;

		    متن_prop_above = FALSE;
		    متن_prop_follows = FALSE;

		    إذا (متن_props_need_مرتب_سازی)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			مرتب_سازی_متن_props(wp->w_میانگیر, متن_props,
					متن_prop_idxs, متن_props_فعال);
			متن_props_need_مرتب_سازی = FALSE;
		    }

		    لكل (pi = 0; pi < متن_props_فعال; ++pi)
		    {
			صحيح	    tpi = متن_prop_idxs[pi];
			textprop_T  *tp = &متن_props[tpi];
			proptype_T  *pt = متن_prop_type_by_id(
						    wp->w_میانگیر, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			إذا (pt != عدم
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_راست
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_زیر)) == 0
				    && wlv.ستون >= wp->w_عرض))
			{
			    إذا (tp->tp_ستون == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == سطر
						&& !did_سطر
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_زیر))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				متن_prop_follows = TRUE;
				استمر;
			    }

			    إذا (pt->pt_hl_id > 0)
				استفاده_شده_attr = syn_id2attr(pt->pt_hl_id);
			    متن_prop_type = pt;
			    متن_prop_attr =
				   hl_combine_attr(متن_prop_attr, استفاده_شده_attr);
			    إذا (استفاده_شده_tpi >= 0 && متن_props[استفاده_شده_tpi].tp_id < 0)
				other_tpi = استفاده_شده_tpi;
			    متن_prop_flags = pt->pt_flags;
			    متن_prop_id = tp->tp_id;
			    استفاده_شده_tpi = tpi;
			}
		    }
		    إذا (متن_prop_id < 0 && استفاده_شده_tpi >= 0
				 && متن_props[استفاده_شده_tpi].u.tp_متن != عدم)
		    {
			textprop_T  *tp = &متن_props[استفاده_شده_tpi];
			char_u	    *p = tp->u.tp_متن;
			صحيح	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			صحيح	    bail_بیرون = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			إذا (p != عدم)
			{
			    صحيح	    راست_2 = (tp->tp_flags
							& TP_FLAG_ALIGN_راست);
			    صحيح	    زیر = (tp->tp_flags
							& TP_FLAG_ALIGN_زیر);
			    صحيح	    wrap = tp->tp_ستون < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    صحيح	    padding = tp->tp_ستون == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (صحيح)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.شروع_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    استفاده_شده_attr);
			    n_attr = mb_charlen(p);
			    متن_prop_attr = 0;
			    متن_prop_attr_comb = 0;
			    إذا (*ptr == NUL)
				// don't combine char attr after EOL
				متن_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    إذا (متن_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    إذا ((راست_2 || above || زیر || !wrap
					    || padding > 0) && wp->w_عرض > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				صحيح	شروع_سطر;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				شروع_سطر = متن_prop_position(wp, tp,
						    wlv.vcol,
# ifdef FEAT_RIGHTLEFT
						    wp->w_p_rl
						    ? wp->w_عرض - wlv.ستون - 1
						    :
# endif
						    wlv.ستون,
						    &wlv.n_extra, &wlv.p_extra,
						    &n_attr, &wlv.n_attr_skip,
						    skip_cells > 0);
				إذا (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				إذا (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				إذا (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.ستون
					       + wlv.n_extra - 2 > wp->w_عرض)
				    // don't bail out at end of line
				    متن_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				إذا (شروع_سطر)
				{
				    draw_screen_سطر(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    إذا (wlv.ردیف == endrow)
				    {
					++wlv.ردیف;
					اكسر;
				    }
				    win_سطر_شروع(wp, &wlv, TRUE);
				    bail_بیرون = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			إذا (skip_cells > 0)
			{
			    إذا (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				إذا (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    وإلا
			    {
				// the whole text is left of the window, drop
				// it and advance to the next one
				skip_cells -= wlv.n_extra;
				skipped_cells += wlv.n_extra;
				wlv.n_extra = 0;
				wlv.n_attr_skip = 0;
				bail_بیرون = TRUE;
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
			       & (TP_FLAG_ALIGN_زیر | TP_FLAG_ALIGN_راست)));

			إذا (bail_بیرون)
			    // starting a new line for "below"
			    استمر;
		    }
		}
		وإلا إذا (متن_prop_بعدی < متن_prop_شمار
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.ستون == wp->w_عرض - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    صحيح only_زیر_follows = !wp->w_p_wrap
				 && wlv.ستون + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_عرض;
		    صحيح suffix_flags = متن_prop_suffix_flags[متن_prop_بعدی];

		    متن_prop_follows = (suffix_flags
					& (only_زیر_follows ? 2 : 1)) != 0;
		}
	    }

	    إذا (wlv.شروع_extra_for_textprop)
	    {
		wlv.شروع_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_جستجو_attr = جستجو_attr;
		saved_area_attr = area_attr;
		جستجو_attr = 0;
		area_attr = 0;
	    }
#پایان_اگر

	    صحيح *area_attr_p =
#اگر_تعریف_شده FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#پایان_اگر
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    إذا (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != عدم
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    وإلا إذا (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    إذا (wlv.n_extra == 0)
	    {
#اگر_تعریف_شده FEAT_جستجو_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (دراز)(ptr - سطر);
		جستجو_attr = update_جستجو_hl(wp, lnum, (colnr_T)v, &سطر,
				      &screen_جستجو_hl, &has_تطابق_conc,
				      &تطابق_conc, did_سطر_attr, lcs_eol_one,
				      &on_آخرین_ستون);
		ptr = سطر + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		إذا (*ptr == NUL)
		    has_تطابق_conc = 0;
#وإلا
		جستجو_attr = 0;
#پایان_اگر

		// Check if ComplMatchIns highlight is needed.
		إذا ((State & نما_درج) && ins_compl_win_فعال(wp)
			    && (درون_curline || ins_compl_lnum_درون_range(lnum)))
		{
		    صحيح ins_تطابق_attr =
			ins_compl_ستون_range_attr(lnum, (صحيح)(ptr - سطر));
		    إذا (ins_تطابق_attr > 0)
			جستجو_attr =
			    hl_combine_attr(جستجو_attr, ins_تطابق_attr);
		}
	    }

#اگر_تعریف_شده FEAT_DIFF
	    إذا (wlv.diff_hlf != (hlf_T)0)
	    {
		إذا (سطر_changes.رقم_changes > 0
			&& change_شاخص >= 0
			&& change_شاخص < سطر_changes.رقم_changes - 1)
		{
		    إذا (ptr - سطر >= سطر_changes.changes[change_شاخص+1].dc_شروع[سطر_changes.bufidx])
			change_شاخص += 1;
		}
		صحيح added = FALSE;
		إذا (سطر_changes.رقم_changes > 0 && change_شاخص >= 0 && change_شاخص < سطر_changes.رقم_changes)
		{
		    added = diff_change_parse(&سطر_changes,
			    &سطر_changes.changes[change_شاخص],
			    &change_شروع,
			    &change_انتها);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		إذا (wlv.diff_hlf == HLF_CHD && ptr - سطر >= change_شروع
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		إذا ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - سطر >= change_انتها && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.سطر_attr = HL_ATTR(wlv.diff_hlf);
		إذا (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= چپ_curline_ستون
					    && wlv.vcol <= راست_curline_ستون)))
		    wlv.سطر_attr = hl_combine_attr(
					  wlv.سطر_attr, HL_ATTR(HLF_CUL));
	    }
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
	    إذا (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		إذا (get_جمله_attr)
		    syntax_attr = جمله_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		إذا (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (دراز)(ptr - سطر);
		    إذا (v == prev_syntax_ستون)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    وإلا
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    عدم, FALSE);
			prev_syntax_ستون = v;
			prev_syntax_attr = syntax_attr;
		    }

		    إذا (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    وإلا
			did_emsg = save_did_emsg;
# ifdef SYN_زمان_حد
		    إذا (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);
		    ptr = سطر + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    إذا (*ptr == NUL)
			syntax_flags = 0;
		    وإلا
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    إذا (متن_prop_type != عدم)
	    {
		إذا (متن_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, متن_prop_attr);
		وإلا
		    syntax_attr = متن_prop_attr;
		متن_prop_attr_comb = syntax_attr;
	    }
# endif
#پایان_اگر

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#اگر_تعریف_شده سطر_ATTR
	    إذا (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.سطر_attr, area_attr);
		إذا (!highlight_تطابق)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(جستجو_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    وإلا إذا (جستجو_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.سطر_attr, جستجو_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    وإلا إذا (wlv.سطر_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.سطر_attr);
# وإلا
		wlv.char_attr = wlv.سطر_attr;
# endif
		attr_pri = FALSE;
	    }
#وإلا
	    إذا (area_attr != 0)
		wlv.char_attr = area_attr;
	    وإلا إذا (جستجو_attr != 0)
		wlv.char_attr = جستجو_attr;
#پایان_اگر
	    وإلا
	    {
		attr_pri = FALSE;
#اگر_تعریف_شده FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#وإلا
		wlv.char_attr = 0;
#پایان_اگر
	    }
#اگر_تعریف_شده FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    إذا (متن_prop_type != عدم && (متن_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, متن_prop_attr);
#پایان_اگر
	}

	// combine attribute with HLF_WIN
	إذا (wlv.win_attr != 0)
	{
	    إذا (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    وإلا
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
	إذا (wlv.n_extra > 0)
	{
	    إذا (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		إذا (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		وإلا
		    mb_utf8 = FALSE;
	    }
	    وإلا
	    {
		c = *wlv.p_extra;
		إذا (has_mbyte)
		{
		    mb_c = c;
		    إذا (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			إذا (mb_l > wlv.n_extra)
			    mb_l = 1;
			وإلا إذا (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    وإلا
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			إذا (mb_l >= wlv.n_extra)
			    mb_l = 1;
			وإلا إذا (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    إذا (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    إذا ((
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون <= 0) :
#پایان_اگر
				    (wlv.ستون >= wp->w_عرض - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = HL_ATTR(HLF_AT);
#اگر_تعریف_شده FEAT_SYN_HL
			إذا (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#پایان_اگر
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    وإلا
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#إذا defined(FEAT_PROP_POPUP)
	    إذا (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		إذا (wlv.saved_n_extra <= 0)
		{
		    إذا (جستجو_attr == 0)
			جستجو_attr = saved_جستجو_attr;
		    إذا (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    إذا (wlv.extra_for_textprop)
			// wlv.extra_attr should be used at this position but
			// not any further.
			reset_extra_attr = TRUE;
		}

		wlv.extra_for_textprop = FALSE;
# ifdef FEAT_LINEBREAK
		درون_linebreak = FALSE;
# endif
	    }
#پایان_اگر
	}
	وإلا
	{
#اگر_تعریف_شده FEAT_LINEBREAK
	    صحيح		c0;
#پایان_اگر
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#اگر_تعریف_شده FEAT_LINEBREAK
	    c0 = *ptr;
#پایان_اگر
	    إذا (c == NUL)
	    {
#اگر_تعریف_شده FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_سطر = TRUE;
#پایان_اگر
		// no more cells to skip
		skip_cells = 0;
	    }

	    إذا (has_mbyte)
	    {
		mb_c = c;
		إذا (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    إذا (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			إذا (mb_c < 0x80)
			{
			    c = mb_c;
#اگر_تعریف_شده FEAT_LINEBREAK
			    c0 = mb_c;
#پایان_اگر
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			إذا (utf_iscomposing(mb_c))
			{
			    صحيح i;

			    لكل (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    إذا ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#اگر_تعریف_شده FEAT_RIGHTLEFT
			إذا (wp->w_p_rl)		//معکوس
			    rl_mirror(wlv.extra);
#پایان_اگر
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (صحيح)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			إذا (area_attr == 0 && جستجو_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    وإلا إذا (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#اگر_تعریف_شده FEAT_ARABIC
		    وإلا إذا (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			صحيح	pc, pc1, nc;
			صحيح	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			إذا (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			وإلا
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    وإلا
			prev_c = mb_c;
#پایان_اگر
		}
		وإلا	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    إذا (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    وإلا إذا (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			إذا (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			وإلا
			{
			    إذا (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_میانگیر, wlv.extra, c);
				wlv.n_extra = (صحيح)STRLEN(wlv.extra) - 1;
			    }
			    وإلا
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
			    إذا (area_attr == 0 && جستجو_attr == 0)
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
		إذا ((
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون <= 0) :
#پایان_اگر
				(wlv.ستون >= wp->w_عرض - 1))
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
#اگر_تعریف_شده FEAT_CONCEAL
		    did_decrement_ptr = TRUE;
#پایان_اگر
		}
		وإلا إذا (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		إذا (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    إذا (area_attr == 0 && جستجو_attr == 0)
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

	    إذا (extra_check)
	    {
#اگر_تعریف_شده FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (دراز)(ptr - سطر);
		إذا (spv->spv_has_spell && v >= کلمه_انتها && v > cur_checked_ستون)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    إذا (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			صحيح	len;
			hlf_T	spell_hlf = HLF_شمار;

			إذا (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			إذا ((prev_ptr - سطر) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - سطر) - nextlinecol;
			وإلا
			    p = prev_ptr;
			spv->spv_cap_ستون -= (صحيح)(prev_ptr - سطر);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_ستون,
							    spv->spv_unchanged);
			کلمه_انتها = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			إذا (spell_hlf != HLF_شمار
				&& (State & نما_درج)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ستون >=
						    (colnr_T)(prev_ptr - سطر)
				&& wp->w_cursor.ستون < (colnr_T)کلمه_انتها)
			{
			    spell_hlf = HLF_شمار;
			    spell_redraw_lnum = lnum;
			}

			إذا (spell_hlf == HLF_شمار && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_ستون = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			إذا (spell_hlf != HLF_شمار)
			    spell_attr = highlight_attr[spell_hlf];

			إذا (spv->spv_cap_ستون > 0)
			{
			    إذا (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_ستون
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_ستون = ((p - nextline)
					    + spv->spv_cap_ستون - nextline_idx);
			    }
			    وإلا
				// Compute the actual column.
				spv->spv_cap_ستون += (prev_ptr - سطر);
			}
		    }
		}
		إذا (spell_attr != 0)
		{
		    إذا (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    وإلا
			wlv.char_attr = hl_combine_attr(spell_attr,
								wlv.char_attr);
		}
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
		// we don't want linebreak to apply for lines that start with
		// leading spaces, followed by long letters (since it would add
		// a break at the beginning of a line and this might be unexpected)
		//
		// So only allow to linebreak, once we have found chars not in
		// 'breakat' in the line.
		إذا ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((صحيح)*ptr))
		    wlv.need_lbr = TRUE;
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
		// Found last space before word: check for line break.
		إذا (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((صحيح)*ptr))
		{
		    صحيح	    mb_off = has_mbyte ? (*mb_سر_off)(سطر, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_اولی_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, سطر, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_متن = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, عدم, عدم) - 1;
		    clear_chartabsize_arg(&cts);

		    إذا (on_آخرین_ستون && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			جستجو_attr = 0;

		    إذا (c == TAB && wlv.n_extra + wlv.ستون > wp->w_عرض)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_میانگیر->b_p_ts,
					      wp->w_میانگیر->b_p_vts_array) - 1;
# وإلا
			wlv.n_extra = (صحيح)wp->w_میانگیر->b_p_ts
				    - wlv.vcol % (صحيح)wp->w_میانگیر->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    إذا (wlv.n_extra > 0 && c != TAB)
			درون_linebreak = TRUE;
# endif
		    إذا (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			إذا (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			إذا (!wp->w_p_list)
			    c = ' ';
		    }
		}
#پایان_اگر
		إذا (wp->w_p_list)
		{
		    درون_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > سطر && prev_ptr[-1] == ' '));
		    إذا (!درون_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		إذا (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (درون_multispace
					&& wp->w_lcs_chars.multispace != عدم))
				&& ptr - سطر >= leadcol
				&& ptr - سطر <= trailcol)))
		{
		    إذا (درون_multispace && wp->w_lcs_chars.multispace != عدم)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			إذا (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    وإلا
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    إذا (area_attr == 0 && جستجو_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    إذا (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    وإلا
			mb_utf8 = FALSE;
		}

		إذا (c == ' ' && ((trailcol != MAXCOL && ptr > سطر + trailcol)
				    || (leadcol != 0 && ptr < سطر + leadcol)))
		{
		    إذا (leadcol != 0 && درون_multispace && ptr < سطر + leadcol
			    && wp->w_lcs_chars.leadmultispace != عدم)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			إذا (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    وإلا إذا (ptr > سطر + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    وإلا إذا (ptr < سطر + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    وإلا إذا (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    إذا (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    إذا (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    وإلا
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    إذا (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		إذا (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    صحيح	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    دراز    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    صحيح	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    صحيح	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    صحيح	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    إذا (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < سطر + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#اگر_تعریف_شده FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_مقدار(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    إذا (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#پایان_اگر
		    // tab amount depends on current column
#اگر_تعریف_شده FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_میانگیر->b_p_ts,
					      wp->w_میانگیر->b_p_vts_array) - 1;
#وإلا
		    tab_len = (صحيح)wp->w_میانگیر->b_p_ts
			       - vcol_adjusted % (صحيح)wp->w_میانگیر->b_p_ts - 1;
#پایان_اگر

#اگر_تعریف_شده FEAT_LINEBREAK
		    إذا (!wp->w_p_lbr || !wp->w_p_list)
#پایان_اگر
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#اگر_تعریف_شده FEAT_LINEBREAK
		    وإلا
		    {
			char_u	*p;
			صحيح	len;
			صحيح	i;
			صحيح	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			إذا (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			إذا (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			إذا (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    صحيح tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    إذا (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    إذا (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    إذا (p == عدم)
				wlv.n_extra = 0;
			    وإلا
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				لكل (i = 0; i < tab_len; i++)
				{
				    صحيح lcs = lcs_tab2;

				    إذا (*p == NUL)
				    {
					tab_len = i;
					اكسر;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    إذا (lcs_tab3 && i == tab_len - 1)
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
				إذا (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
		    {
			صحيح vc_saved = wlv.vcol_off_co;

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
			إذا (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#پایان_اگر
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    إذا (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#اگر_تعریف_شده FEAT_LINEBREAK
			إذا (wp->w_p_lbr && wlv.p_extra != عدم
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			وإلا
#پایان_اگر
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			إذا (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    وإلا
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		وإلا إذا (c == NUL
			&& wlv.n_extra == 0
			&& (wp->w_p_list
			    || (wlv.fromcol >= 0
				&& wlv.tocol > wlv.vcol
				&& VIsual_نما != Ctrl_V
				&& (
#اگر_تعریف_شده FEAT_RIGHTLEFT
				    wp->w_p_rl ? (wlv.ستون >= 0) :
#پایان_اگر
				    (wlv.ستون < wp->w_عرض))))
			&& lcs_eol_one > 0)
		{
		    // Display a '$' after the line or highlight an extra
		    // character if the line break is included.
#إذا defined(FEAT_DIFF) || defined(سطر_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    إذا (
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
#پایان_اگر
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			إذا (!(area_highlighting && virtual_فعال()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    إذا (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    وإلا
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    إذا (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    إذا (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    وإلا
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		وإلا إذا (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_میانگیر, c);
		    إذا (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#اگر_تعریف_شده FEAT_RIGHTLEFT
		    إذا ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#پایان_اگر
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#اگر_تعریف_شده FEAT_LINEBREAK
		    إذا (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			إذا (p == عدم)
			    wlv.n_extra = 0;
			وإلا
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    وإلا
#پایان_اگر
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    إذا (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#اگر_تعریف_شده FEAT_PROP_POPUP
			إذا (متن_prop_type != عدم &&
				 متن_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(متن_prop_attr,
							       wlv.extra_attr);
#پایان_اگر

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		وإلا إذا (VIsual_فعال
			 && (VIsual_نما == Ctrl_V
			     || VIsual_نما == 'v')
			 && virtual_فعال()
			 && wlv.tocol != MAXCOL
			 && wlv.vcol < wlv.tocol
			 && (
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.ستون >= 0) :
#پایان_اگر
			    (wlv.ستون < wp->w_عرض)))
		{
		    c = ' ';
		    --ptr;	    // put it back at the NUL
		}
#إذا defined(سطر_ATTR)
		وإلا إذا ((
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
					    < wp->w_عرض)))
		{
		    // Highlight until the right side of the window
		    c = ' ';
		    --ptr;	    // put it back at the NUL

		    // Remember we do the char for line highlighting.
		    ++did_سطر_attr;

		    // don't do search HL for the rest of the line
		    إذا (wlv.سطر_attr != 0 && wlv.char_attr == جستجو_attr
					&& (did_سطر_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.سطر_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    إذا (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_سطر
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    إذا (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			إذا (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    إذا (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= چپ_curline_ستون
					    && wlv.vcol <= راست_curline_ستون)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    إذا (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			إذا (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    إذا (!wlv.cul_screenline
				    || (wlv.vcol >= چپ_curline_ستون
					     && wlv.vcol <= راست_curline_ستون))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			وإلا إذا (wlv.سطر_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.سطر_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			إذا (wlv.vcol == 0)
			{
			    إذا (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    وإلا إذا (جستجو_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, جستجو_attr);
			}
		    }
# endif
		}
#پایان_اگر
	    }

#اگر_تعریف_شده FEAT_CONCEAL
	    إذا (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_سطر(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_تطابق_conc > 0)
		&& !(lnum_درون_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == عدم))
	    {
		صحيح syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		إذا (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_تطابق_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_تطابق_conc && تطابق_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    إذا (has_تطابق_conc && تطابق_conc)
			c = تطابق_conc;
		    وإلا إذا (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    وإلا إذا (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    وإلا
			c = ' ';

		    إذا (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    إذا (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    وإلا
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    إذا (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    إذا (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			إذا (wp->w_p_rl)
			{
			    wlv.ستون -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			وإلا
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.ستون += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		وإلا إذا (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    وإلا
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    إذا (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#پایان_اگر // FEAT_CONCEAL
	}

#اگر_تعریف_شده FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	إذا (!did_wcol && wlv.draw_state == WL_سطر
		&& درون_curline && conceal_cursor_سطر(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		wp->w_wcol = wp->w_عرض - wlv.ستون + wlv.boguscols - 1;
	    وإلا
# endif
		wp->w_wcol = wlv.ستون - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    إذا (wlv.vcol + skip_cells < wp->w_virtcol)
		// Cursor beyond end of the line with 'virtualedit'.
		wp->w_wcol += wp->w_virtcol - wlv.vcol - skip_cells;
	    wp->w_wrow = wlv.ردیف;
	    did_wcol = TRUE;
	    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
# ifdef FEAT_PROP_POPUP
	    curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
# endif
	}
#پایان_اگر

	// Use "wlv.extra_attr", but don't override visual selection
	// highlighting, unless text property overrides.
	// Don't use "wlv.extra_attr" until wlv.n_attr_skip is zero.
	إذا (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_سطر
		&& (!attr_pri
#اگر_تعریف_شده FEAT_PROP_POPUP
		    || (متن_prop_flags & PT_FLAG_OVERRIDE)
#پایان_اگر
		   ))
	{
#اگر_تعریف_شده سطر_ATTR
	    إذا (wlv.سطر_attr)
		wlv.char_attr = hl_combine_attr(wlv.سطر_attr, wlv.extra_attr);
	    وإلا
#پایان_اگر
		wlv.char_attr = wlv.extra_attr;
#اگر_تعریف_شده FEAT_PROP_POPUP
	    إذا (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#پایان_اگر
	}

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	إذا (p_imst == IM_ON_THE_SPOT
		&& xic != عدم
		&& lnum == wp->w_cursor.lnum
		&& (State & نما_درج)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_سطر)
	{
	    colnr_T tcol;

	    إذا (preedit_انتها_ستون == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, عدم, عدم, 0);
	    وإلا
		tcol = preedit_انتها_ستون;
	    إذا ((دراز)preedit_شروع_ستون <= wlv.vcol && wlv.vcol < (دراز)tcol)
	    {
		إذا (feedback_old_attr < 0)
		{
		    feedback_ستون = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_ستون);
		إذا (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_ستون++;
	    }
	    وإلا إذا (feedback_old_attr >= 0)
	    {
		wlv.char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_ستون = 0;
	    }
	}
#پایان_اگر
	// Handle the case where we are in column 0 but not on the first
	// character of the line and the user wants us to show us a
	// special character (via 'listchars' option "precedes:<char>").
	إذا (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? (wp->w_skipcol > 0 && wlv.ردیف == 0)
				 : wp->w_leftcol > 0)
#اگر_تعریف_شده FEAT_DIFF
		&& wlv.filler_todo <= 0
#پایان_اگر
		&& wlv.draw_state > WL_NR
		&& skip_cells <= 0
		&& c != NUL)
	{
	    c = wp->w_lcs_chars.prec;
	    lcs_prec_todo = NUL;
	    إذا (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    إذا (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    وإلا
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    إذا (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	إذا ((c == NUL
#إذا defined(سطر_ATTR)
		|| did_سطر_attr == 1
#پایان_اگر
		) && wlv.eol_hl_off == 0)
	{
#اگر_تعریف_شده FEAT_جستجو_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    صحيح prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_جستجو_hl,
					      (دراز)(ptr - سطر) - (c == NUL));
#پایان_اگر
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    إذا (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_نما != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#اگر_تعریف_شده FEAT_جستجو_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_فعال))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# إذا defined(سطر_ATTR)
			    && did_سطر_attr <= 1
# endif
			   )
#پایان_اگر
		       ))
	    {
		صحيح n = 0;

#اگر_تعریف_شده FEAT_RIGHTLEFT
		إذا (wp->w_p_rl)
		{
		    إذا (wlv.ستون < 0)
			n = 1;
		}
		وإلا
#پایان_اگر
		{
		    إذا (wlv.ستون >= wp->w_عرض)
			n = -1;
		}
		إذا (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.ستون += n;
		}
		وإلا
		{
		    // Add a blank character to highlight.
		    Screen_سطرها[wlv.off] = ' ';
		    إذا (enc_utf8)
			Screen_سطرها_UC[wlv.off] = 0;
		}
#اگر_تعریف_شده FEAT_جستجو_EXTRA
		إذا (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_جستجو_تطابق_hl(wp, &screen_جستجو_hl,
					   (دراز)(ptr - سطر), &wlv.char_attr);
		}
#پایان_اگر
		ScreenAttrs[wlv.off] = wlv.char_attr;
		Screen_تعداد_ستون[wlv.off] = wlv.vcol;
#اگر_تعریف_شده FEAT_RIGHTLEFT
		إذا (wp->w_p_rl)
		{
		    --wlv.ستون;
		    --wlv.off;
		}
		وإلا
#پایان_اگر
		{
		    ++wlv.ستون;
		    ++wlv.off;
		}
		++wlv.vcol;
		wlv.eol_hl_off = 1;
	    }
	}

	// At end of the text line.
	إذا (c == NUL)
	{
#اگر_تعریف_شده FEAT_PROP_POPUP
	    إذا (متن_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    وإلا
#پایان_اگر
	    {
		draw_screen_سطر(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		إذا (درون_curline)
		{
		    curwin->w_cline_ردیف = startrow;
		    curwin->w_cline_ارتفاع = wlv.ردیف - startrow;
#اگر_تعریف_شده FEAT_FOLDING
		    curwin->w_cline_folded = false;
#پایان_اگر
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		اكسر;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	صحيح lcs_ext = get_lcs_ext(wp);
	إذا (lcs_ext != NUL
		&& wlv.draw_state == WL_سطر
#اگر_تعریف_شده FEAT_DIFF
		&& wlv.filler_todo <= 0
#پایان_اگر
		&& (
#اگر_تعریف_شده FEAT_RIGHTLEFT
		    wp->w_p_rl ? wlv.ستون == 0 :
#پایان_اگر
		    wlv.ستون == wp->w_عرض - 1)
		&& (*ptr != NUL
		    || lcs_eol_one > 0
		    || (wlv.n_extra > 0 && (wlv.c_extra != NUL
						     || *wlv.p_extra != NUL))
#اگر_تعریف_شده FEAT_PROP_POPUP
		    || متن_prop_بعدی <= آخرین_textprop_متن_idx
#پایان_اگر
		   ))
	{
	    c = lcs_ext;
	    wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    mb_c = c;
	    إذا (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    وإلا
		mb_utf8 = FALSE;
	}

#اگر_تعریف_شده FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	إذا (wlv.draw_color_ستون)
	    wlv.draw_color_ستون = advance_color_ستون(VCOL_HLC, &wlv.color_تعداد_ستون);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	إذا (((wlv.draw_state == WL_سطر
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_درون_visual_area
		&& جستجو_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    إذا (wp->w_p_cuc && VCOL_HLC == (دراز)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    وإلا إذا (wlv.draw_color_ستون && VCOL_HLC == *wlv.color_تعداد_ستون)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#پایان_اگر

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	إذا (wlv.draw_state < WL_سطر || skip_cells <= 0)
	{
	    // Store the character.
#إذا defined(FEAT_RIGHTLEFT)
	    إذا (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.ستون;
	    }
#پایان_اگر
	    Screen_سطرها[wlv.off] = c;
	    إذا (enc_dbcs == DBCS_JPNU)
	    {
		إذا ((mb_c & 0xff00) == 0x8e00)
		    Screen_سطرها[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    وإلا إذا (enc_utf8)
	    {
		إذا (mb_utf8)
		{
		    صحيح i;

		    Screen_سطرها_UC[wlv.off] = mb_c;
		    إذا ((c & 0xff) == 0)
			Screen_سطرها[wlv.off] = 0x80;   // avoid storing zero
		    لكل (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطرها_C[i][wlv.off] = u8cc[i];
			إذا (u8cc[i] == 0)
			    اكسر;
		    }
		}
		وإلا
		    Screen_سطرها_UC[wlv.off] = 0;
	    }
	    إذا (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    وإلا
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    إذا (wlv.draw_state > WL_NR
#اگر_تعریف_شده FEAT_DIFF
		    && wlv.filler_todo <= 0
#پایان_اگر
		    )
		Screen_تعداد_ستون[wlv.off] = wlv.vcol;
	    وإلا
		Screen_تعداد_ستون[wlv.off] = -1;

	    إذا (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.ستون;
		إذا (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_سطرها[wlv.off] = 0;
		وإلا
		    // DBCS: Put second byte in the second screen char.
		    Screen_سطرها[wlv.off] = mb_c & 0xff;

		إذا (wlv.draw_state > WL_NR
#اگر_تعریف_شده FEAT_DIFF
			&& wlv.filler_todo <= 0
#پایان_اگر
			)
		    Screen_تعداد_ستون[wlv.off] = ++wlv.vcol;
		وإلا
		    Screen_تعداد_ستون[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		إذا (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#اگر_تعریف_شده FEAT_RIGHTLEFT
		إذا (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.ستون;
		}
#پایان_اگر
	    }
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.ستون;
	    }
	    وإلا
#پایان_اگر
	    {
		++wlv.off;
		++wlv.ستون;
	    }
	}
#اگر_تعریف_شده FEAT_CONCEAL
	وإلا إذا (wp->w_p_cole > 0 && is_concealing)
	{
	    صحيح concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    إذا (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    إذا (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    إذا (wp->w_p_wrap)
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
		صحيح adjust_ستون = !WIN_IS_POPUP(wp);
# وإلا
		صحيح adjust_ستون = TRUE;
# endif
		إذا (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    إذا (wp->w_p_rl)
		    {
			إذا (adjust_ستون)
			{
			    wlv.ستون -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    وإلا
# endif
		    {
			إذا (adjust_ستون)
			{
			    wlv.ستون += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		إذا (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    إذا (wp->w_p_rl)
		    {
			إذا (adjust_ستون)
			{
			    --wlv.boguscols;
			    --wlv.ستون;
			}
		    }
		    وإلا
# endif
		    {
			إذا (adjust_ستون)
			{
			    ++wlv.boguscols;
			    ++wlv.ستون;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		إذا (wp->w_p_rl)
		{
		    إذا (adjust_ستون)
		    {
			--wlv.boguscols;
			--wlv.ستون;
		    }
		}
		وإلا
# endif
		{
		    إذا (adjust_ستون)
		    {
			++wlv.boguscols;
			++wlv.ستون;
		    }
		}
	    }
	    وإلا
	    {
		إذا (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#پایان_اگر // FEAT_CONCEAL
	وإلا
	    --skip_cells;

	إذا (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	إذا (wlv.draw_state > WL_NR
#اگر_تعریف_شده FEAT_DIFF
		&& wlv.filler_todo <= 0
#پایان_اگر
		)
	    ++wlv.vcol;

#اگر_تعریف_شده FEAT_SYN_HL
	إذا (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#پایان_اگر

	// restore attributes after "precedes" in 'listchars'
	إذا (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	إذا (n_attr > 0 && wlv.draw_state == WL_سطر
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	إذا (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	إذا ((
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.ستون < 0) :
#پایان_اگر
				    (wlv.ستون >= wp->w_عرض))
		&& (wlv.draw_state != WL_سطر
		    || *ptr != NUL
#اگر_تعریف_شده FEAT_DIFF
		    || wlv.filler_todo > 0
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
		    || متن_prop_above || متن_prop_follows
		    || متن_prop_بعدی <= آخرین_textprop_متن_idx
#پایان_اگر
		    || (wp->w_p_list && wp->w_lcs_chars.eol != NUL
						&& lcs_eol_one != -1)
		    || (wlv.n_extra != 0 && (wlv.c_extra != NUL
						      || *wlv.p_extra != NUL)))
		)
	{
#اگر_تعریف_شده FEAT_CONCEAL
	    wlv.ستون -= wlv.boguscols;
	    // Apply 'cursorline' and HLF_WIN highlight.
	    إذا (wlv.boguscols != 0 && (
# ifdef سطر_ATTR
			wlv.سطر_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		صحيح attr = wlv.win_attr;
# ifdef سطر_ATTR
		إذا (wlv.سطر_attr != 0)
		    attr = hl_combine_attr(attr, wlv.سطر_attr);
# endif
		طالما ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.ستون >= 0 :
# endif
			    wlv.ستون < wp->w_عرض))
		{
		    Screen_سطرها[wlv.off] = ' ';
		    إذا (enc_utf8)
			Screen_سطرها_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    Screen_تعداد_ستون[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    إذا (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.ستون--;
			wlv.boguscols++;
		    }
		    وإلا
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
#وإلا
	    wlv_screen_سطر(wp, &wlv, TRUE);
#پایان_اگر
	    ++wlv.ردیف;
	    ++wlv.screen_ردیف;

	    // When not wrapping and finished diff lines, break here.
	    إذا (!wp->w_p_wrap
#اگر_تعریف_شده FEAT_DIFF
			&& wlv.filler_todo <= 0
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
			&& !متن_prop_above
			&& !متن_prop_follows
#پایان_اگر
		       )
		اكسر;
#اگر_تعریف_شده FEAT_PROP_POPUP
	    إذا (!wp->w_p_wrap && متن_prop_follows && !متن_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = سطر + (size_t)ml_get_buf_len(wp->w_میانگیر, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#پایان_اگر

	    // When the window is too narrow draw all "@" lines.
	    إذا (wlv.draw_state != WL_سطر
#اگر_تعریف_شده FEAT_DIFF
		    && wlv.filler_todo <= 0
#پایان_اگر
		    )
	    {
		win_draw_انتها(wp, '@', ' ', TRUE, wlv.ردیف, wp->w_ارتفاع, HLF_AT);
		draw_vsep_win(wp, wlv.ردیف);
		wlv.ردیف = endrow;
	    }

	    // When line got too long for screen break here.
	    إذا (wlv.ردیف == endrow)
	    {
		++wlv.ردیف;
		اكسر;
	    }

	    إذا (screen_cur_ردیف == wlv.screen_ردیف - 1
#اگر_تعریف_شده FEAT_DIFF
		     && wlv.filler_todo <= 0
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
		     && !متن_prop_above && !متن_prop_follows
#پایان_اگر
		     && wp->w_عرض == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		سطر_Wraps[wlv.screen_ردیف - 1] = TRUE;

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
		إذا (p_tf
#اگر_تعریف_شده FEAT_GUI
			 && !gui.درون_use
#پایان_اگر
			 && !(has_mbyte
			     && ((*mb_off2cells)(سطر_جابه_جایی[wlv.screen_ردیف],
				   سطر_جابه_جایی[wlv.screen_ردیف] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     سطر_جابه_جایی[wlv.screen_ردیف - 1]
							    + (صحيح)topframe->fr_عرض - 2,
				     سطر_جابه_جایی[wlv.screen_ردیف]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    إذا (screen_cur_ستون != wp->w_عرض)
			screen_char(سطر_جابه_جایی[wlv.screen_ردیف - 1]
						       + (بیعلامت)topframe->fr_عرض - 1,
				       wlv.screen_ردیف - 1, (صحيح)(topframe->fr_عرض - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    إذا (has_mbyte && MB_BYTE2LEN(Screen_سطرها[سطر_جابه_جایی[
				     wlv.screen_ردیف - 1] + (topframe->fr_عرض - 1)]) > 1)
			بیرون_char(' ');
		    وإلا
			بیرون_char(Screen_سطرها[سطر_جابه_جایی[wlv.screen_ردیف - 1]
							    + (topframe->fr_عرض - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[سطر_جابه_جایی[wlv.screen_ردیف]] = (sattr_T)-1;
		    screen_شروع();	// don't know where cursor is now
		}
	    }

	    win_سطر_شروع(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#اگر_تعریف_شده FEAT_LINEBREAK
	    إذا (!wlv.dont_use_showbreak
# ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
# endif
	       )
		wlv.need_showbreak = TRUE;
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
	    --wlv.filler_todo;
	    // When the filler lines are actually below the last line of the
	    // file, don't draw the line itself, break here.
	    إذا (wlv.filler_todo == 0 && wp->w_botfill)
		اكسر;
#پایان_اگر
	}

    }	// for every character in the line
#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (متن_props != متن_props_buf)
	vim_free(متن_props);
    إذا (متن_prop_idxs != متن_prop_idxs_buf)
	vim_free(متن_prop_idxs);
    إذا (متن_prop_suffix_flags != متن_prop_suffix_flags_buf)
	vim_free(متن_prop_suffix_flags);
    vim_free(p_extra_free2);
#پایان_اگر

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    أرجع wlv.ردیف;
}
