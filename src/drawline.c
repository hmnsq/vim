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

#അന്തര്ഭാവ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lcs_ext(win_T *wp)
{
    എങ്കിൽ (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	തിരികെ NUL;
    എങ്കിൽ (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	തിരികെ '>';
    തിരികെ wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ifdef FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
advance_color_col(പൂർണ്ണസംഖ്യ vcol, പൂർണ്ണസംഖ്യ **color_cols)
{
    ആകുമ്പോൾ (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    തിരികെ (**color_cols >= 0);
}
#endif

#ifdef FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    സ്ഥിതം ശൂന്യം
margin_columns_win(win_T *wp, പൂർണ്ണസംഖ്യ *ഇടത്_col, പൂർണ്ണസംഖ്യ *വലത്_col)
{
    // cache previous calculations depending on w_virtcol
    സ്ഥിതം പൂർണ്ണസംഖ്യ saved_w_virtcol;
    സ്ഥിതം win_T *prev_wp;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_width1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_width2;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_ഇടത്_col;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_വലത്_col;

    പൂർണ്ണസംഖ്യ cur_col_off = win_col_off(wp);
    പൂർണ്ണസംഖ്യ	width1;
    പൂർണ്ണസംഖ്യ	width2;

    width1 = wp->w_width - cur_col_off;
    width2 = width1 + win_col_off2(wp);

    എങ്കിൽ (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*വലത്_col = prev_വലത്_col;
	*ഇടത്_col = prev_ഇടത്_col;
	തിരികെ;
    }

    *ഇടത്_col = 0;
    *വലത്_col = width1;

    എങ്കിൽ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*വലത്_col = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    എങ്കിൽ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ഇടത്_col = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ഇടത്_col = *ഇടത്_col;
    prev_വലത്_col = *വലത്_col;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#endif

#എങ്കിൽ defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define വരി_ATTR
#endif

// structure with variables passed between win_line() and other functions
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    പൂർണ്ണസംഖ്യ		startrow;	// first row in the window to be drawn
    പൂർണ്ണസംഖ്യ		row;		// row in the window, excl w_winrow
    പൂർണ്ണസംഖ്യ		screen_row;	// row on the screen, incl w_winrow

    long	vcol;		// virtual column, before wrapping
    പൂർണ്ണസംഖ്യ		col;		// visual column on screen, after wrapping
#ifdef FEAT_CONCEAL
    പൂർണ്ണസംഖ്യ		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    പൂർണ്ണസംഖ്യ		vcol_off_co;	// offset for concealed characters
#endif
    പൂർണ്ണസംഖ്യ		vcol_off_tp;	// offset for virtual text
#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ		draw_color_col;	// highlight colorcolumn
    പൂർണ്ണസംഖ്യ		*color_cols;	// pointer to according columns array
#endif
    പൂർണ്ണസംഖ്യ		eol_hl_off;	// 1 if highlighted char after EOL

    unsigned	off;		// offset in ScreenLines/ScreenAttrs

    പൂർണ്ണസംഖ്യ		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ		cul_attr;	// set when 'cursorline' active
#endif
#ifdef വരി_ATTR
    പൂർണ്ണസംഖ്യ		വരി_attr;	// for the whole line, includes 'cursorline'
#endif

    പൂർണ്ണസംഖ്യ		screen_വരി_flags;  // flags for screen_line()

    പൂർണ്ണസംഖ്യ		fromcol;	// start of inverting
    പൂർണ്ണസംഖ്യ		tocol;		// end of inverting

#ifdef FEAT_LINEBREAK
    long	vcol_sbr;	    // virtual column after showbreak
    പൂർണ്ണസംഖ്യ		need_showbreak;	    // overlong line, skipping first x chars
    പൂർണ്ണസംഖ്യ		dont_use_showbreak; // do not use 'showbreak'
#endif
#ifdef FEAT_PROP_POPUP
    പൂർണ്ണസംഖ്യ		text_prop_above_എണ്ണം;
#endif

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    പൂർണ്ണസംഖ്യ		cul_screenline;

    പൂർണ്ണസംഖ്യ		char_attr;	// attributes for the next character

    പൂർണ്ണസംഖ്യ		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    പൂർണ്ണസംഖ്യ		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    പൂർണ്ണസംഖ്യ		n_attr_skip;    // chars to skip before using extra_attr
    പൂർണ്ണസംഖ്യ		c_extra;	// extra chars, all the same
    പൂർണ്ണസംഖ്യ		c_final;	// final char, mandatory if set
    പൂർണ്ണസംഖ്യ		extra_for_textprop; // n_extra set for textprop
    പൂർണ്ണസംഖ്യ		start_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    പൂർണ്ണസംഖ്യ		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    പൂർണ്ണസംഖ്യ		saved_extra_attr;
    പൂർണ്ണസംഖ്യ		saved_n_attr_skip;
    പൂർണ്ണസംഖ്യ		saved_extra_for_textprop;
    പൂർണ്ണസംഖ്യ		saved_c_extra;
    പൂർണ്ണസംഖ്യ		saved_c_final;
    പൂർണ്ണസംഖ്യ		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ifdef FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#endif
    പൂർണ്ണസംഖ്യ		filler_വരികൾ;	// nr of filler lines to be drawn
    പൂർണ്ണസംഖ്യ		filler_todo;	// nr of filler lines still to do + 1
#ifdef FEAT_SIGNS
    sign_attrs_T sattr;
#endif
#ifdef FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    പൂർണ്ണസംഖ്യ		need_lbr;
#endif
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#define WL_START	0		// nothing done yet, must be zero
#define WL_CMDLINE	(WL_START + 1)	// cmdline window column
#ifdef FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#അല്ലെങ്കിൽ
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#അല്ലെങ്കിൽ
# define WL_SIGN	WL_FOLD		// column for signs
#endif
#define WL_NR		(WL_SIGN + 1)	// line number
#ifdef FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#അല്ലെങ്കിൽ
# define WL_BRI		WL_NR
#endif
#എങ്കിൽ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#അല്ലെങ്കിൽ
# define WL_SBR		WL_BRI
#endif
#define WL_വരി		(WL_SBR + 1)	// text in the line

#എങ്കിൽ defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
use_cursor_വരി_highlight(win_T *wp, linenr_T lnum)
{
    തിരികെ wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#endif


#ifdef FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    സ്ഥിതം ശൂന്യം
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    പൂർണ്ണസംഖ്യ fdc = compute_foldcolumn(wp, 0);

    എങ്കിൽ (fdc <= 0)
	തിരികെ;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    എങ്കിൽ (wlv->p_extra_free == ശൂന്യ)
	തിരികെ;

    wlv->n_extra = (പൂർണ്ണസംഖ്യ)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    എങ്കിൽ (use_cursor_വരി_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
get_sign_display_info(
	പൂർണ്ണസംഖ്യ		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    പൂർണ്ണസംഖ്യ	text_sign;
# ifdef FEAT_SIGN_ICONS
    പൂർണ്ണസംഖ്യ	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    എങ്കിൽ (nrcol)
	wlv->n_extra = സംഖ്യ_width(wp) + 1;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (use_cursor_വരി_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	അല്ലെങ്കിൽ
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    എങ്കിൽ (wlv->row == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_വരികൾ && wlv->filler_todo <= 0
# endif
       )
    {
	text_sign = (wlv->sattr.sat_text != ശൂന്യ) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ശൂന്യ) ? wlv->sattr.sat_typenr : 0;
	എങ്കിൽ (gui.in_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    എങ്കിൽ (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((അക്ഷരം *)wlv->extra, വലുപ്പം(wlv->extra),
						"%-*c ", സംഖ്യ_width(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    അല്ലെങ്കിൽ
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    എങ്കിൽ (netbeans_active() && (buf_signcount(wp->w_buffer, wlv->lnum)
									  > 1))
	    {
		എങ്കിൽ (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((അക്ഷരം *)wlv->extra, വലുപ്പം(wlv->extra),
						"%-*c ", സംഖ്യ_width(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		അല്ലെങ്കിൽ
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	അല്ലെങ്കിൽ
# endif
	    എങ്കിൽ (text_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_text;
		എങ്കിൽ (wlv->p_extra != ശൂന്യ)
		{
		    എങ്കിൽ (nrcol)
		    {
			പൂർണ്ണസംഖ്യ width = സംഖ്യ_width(wp) - 2;

			vim_memset(wlv->extra, ' ', width);
			wlv->n_extra = width;
			wlv->n_extra += vim_snprintf((അക്ഷരം *)wlv->extra + width,
				  വലുപ്പം(wlv->extra) - width, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    അല്ലെങ്കിൽ
			wlv->n_extra = (പൂർണ്ണസംഖ്യ)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		എങ്കിൽ (use_cursor_വരി_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		അല്ലെങ്കിൽ
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#endif

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    സ്ഥിതം ശൂന്യം
handle_lnum_col(
	win_T		*wp,
	winlinevars_T	*wlv,
	പൂർണ്ണസംഖ്യ		sign_present UNUSED,
	പൂർണ്ണസംഖ്യ		അക്കം_attr UNUSED)
{
    പൂർണ്ണസംഖ്യ has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ശൂന്യ;
    പൂർണ്ണസംഖ്യ lnum_row = wlv->startrow + wlv->filler_വരികൾ
#ifdef FEAT_PROP_POPUP
		      + wlv->text_prop_above_എണ്ണം
#endif
		      ;

    എങ്കിൽ ((wp->w_p_nu || wp->w_p_rnu)
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
	എങ്കിൽ ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_text != ശൂന്യ)
	    get_sign_display_info(TRUE, wp, wlv);
	അല്ലെങ്കിൽ
#endif
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  എങ്കിൽ (wlv->row == lnum_row
		    && (wp->w_skipcol == 0 || wlv->row > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      long അക്കം;
	      അക്ഷരം *fmt = "%*ld ";

	      എങ്കിൽ (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  അക്കം = (long)wlv->lnum;
	      അല്ലെങ്കിൽ
	      {
		  // 'relativenumber', don't use negative numbers
		  അക്കം = labs((long)get_cursor_rel_lnum(wp, wlv->lnum));
		  എങ്കിൽ (അക്കം == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      അക്കം = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((അക്ഷരം *)wlv->extra, വലുപ്പം(wlv->extra), fmt, സംഖ്യ_width(wp), അക്കം);
	      എങ്കിൽ (wp->w_skipcol > 0 && wlv->startrow == 0)
		  വേണ്ടി (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ifdef FEAT_RIGHTLEFT
	      എങ്കിൽ (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  പൂർണ്ണസംഖ്യ	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  വേണ്ടി (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
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
	  അല്ലെങ്കിൽ
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = സംഖ്യ_width(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ifdef FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  എങ്കിൽ (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->row == lnum_row
		      || (wlv->row > lnum_row
			 && (wp->w_p_culopt_flags & CULOPT_വരി))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#endif
	  എങ്കിൽ (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  എങ്കിൽ (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ifdef FEAT_SIGNS
	എങ്കിൽ (അക്കം_attr)
	    wlv->char_attr = അക്കം_attr;
#endif
    }
}

#ifdef FEAT_LINEBREAK
    സ്ഥിതം ശൂന്യം
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    എങ്കിൽ (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_മൂല്യം(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    എങ്കിൽ (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	എങ്കിൽ (wp->w_p_bri && (wlv->row > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_വരികൾ
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    എങ്കിൽ (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ശൂന്യ;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_buffer, wlv->lnum, FALSE));
	    എങ്കിൽ (wlv->row == wlv->startrow)
	    {
		wlv->n_extra -= win_col_off2(wp);
		എങ്കിൽ (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    എങ്കിൽ (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    എങ്കിൽ (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	എങ്കിൽ (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#endif

#എങ്കിൽ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    സ്ഥിതം ശൂന്യം
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    എങ്കിൽ (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	എങ്കിൽ (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	അല്ലെങ്കിൽ
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (wp->w_p_rl)
	    wlv->n_extra = wlv->col + 1;
	അല്ലെങ്കിൽ
#  endif
	    wlv->n_extra = wp->w_width - wlv->col;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_മൂല്യം(wp);
    എങ്കിൽ (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (പൂർണ്ണസംഖ്യ)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	എങ്കിൽ (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	എങ്കിൽ (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	എങ്കിൽ (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    എങ്കിൽ (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#endif

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
textprop_size_after_trunc(
	win_T	*wp,
	പൂർണ്ണസംഖ്യ	flags,	    // TP_FLAG_ALIGN_*
	പൂർണ്ണസംഖ്യ	added,
	പൂർണ്ണസംഖ്യ	padding,
	char_u	*text,
	പൂർണ്ണസംഖ്യ	*n_used_ptr)
{
    പൂർണ്ണസംഖ്യ	space = (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_width - win_col_off(wp) : added;
    പൂർണ്ണസംഖ്യ strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    എങ്കിൽ (space < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_width;
    എങ്കിൽ (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    വേണ്ടി (p = text; *p != NUL; p += (*mb_ptr2len)(p))
    {
	പൂർണ്ണസംഖ്യ clen = ptr2cells(p);

	എങ്കിൽ (strsize + clen > space)
	    നിർത്തുക;
	strsize += clen;
    }
    *n_used_ptr = (പൂർണ്ണസംഖ്യ)(p - text);

    തിരികെ strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    പൂർണ്ണസംഖ്യ
text_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	പൂർണ്ണസംഖ്യ	    vcol,	    // current text column
	പൂർണ്ണസംഖ്യ	    scr_col,	    // current screen column
	പൂർണ്ണസംഖ്യ	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	പൂർണ്ണസംഖ്യ	    *n_attr,	    // attribute cells, NULL if not used
	പൂർണ്ണസംഖ്യ	    *n_attr_skip,   // cells to skip attr, NULL if not used
	പൂർണ്ണസംഖ്യ	    do_skip)	    // skip_cells is not zero
{
    പൂർണ്ണസംഖ്യ	    വലത് = (tp->tp_flags & TP_FLAG_ALIGN_വലത്);
    പൂർണ്ണസംഖ്യ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    പൂർണ്ണസംഖ്യ	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    പൂർണ്ണസംഖ്യ	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    പൂർണ്ണസംഖ്യ	    padding = tp->tp_col == MAXCOL ? tp->tp_padleft : 0;
    പൂർണ്ണസംഖ്യ	    col_with_padding = scr_col + (below ? 0 : padding);
    പൂർണ്ണസംഖ്യ	    room = wp->w_width - col_with_padding;
    പൂർണ്ണസംഖ്യ	    before = room;	// spaces before the text
    പൂർണ്ണസംഖ്യ	    after = 0;		// spaces after the text
    പൂർണ്ണസംഖ്യ	    n_used = *n_extra;
    char_u  *l = ശൂന്യ;
    പൂർണ്ണസംഖ്യ	    strsize = vim_strsize(*p_extra);
    പൂർണ്ണസംഖ്യ	    cells = wrap ? strsize : textprop_size_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_used);

    എങ്കിൽ (wrap || വലത് || above || below || padding > 0 || n_used < *n_extra)
    {
	പൂർണ്ണസംഖ്യ	    col_off = win_col_off(wp) - win_col_off2(wp);

	എങ്കിൽ (above)
	{
	    before = 0;
	    after = wp->w_width - cells - win_col_off(wp) - padding;
	    എങ്കിൽ (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // Right-align: fill with before
	    എങ്കിൽ (വലത്)
		before -= cells;

	    // Below-align: empty line add one character
	    എങ്കിൽ (below && vcol == 0 && col_with_padding == col_off
					    && wp->w_width - col_off == before)
		col_with_padding += 1;

	    എങ്കിൽ (before < 0
		    || !(വലത് || below)
		    || (below ? (col_with_padding <= col_off || !wp->w_p_wrap)
			      : (n_used < *n_extra)))
	    {
		എങ്കിൽ (വലത് && (wrap
			      || (room < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_width - col_off - strsize + room;
		    എങ്കിൽ (before < 0)
			before = 0;
		    അല്ലെങ്കിൽ
			n_used = *n_extra;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (below && before > vcol && do_skip)
		    before -= vcol;
		അല്ലെങ്കിൽ
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	എങ്കിൽ (n_used < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_used;

	// add 1 for NUL, 2 for when '…' is used
	എങ്കിൽ (n_attr != ശൂന്യ)
	    l = alloc(n_used + before + after + (padding > 0 ? padding : 0) + 3);
	എങ്കിൽ (n_attr == ശൂന്യ || l != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ off = 0;

	    എങ്കിൽ (n_attr != ശൂന്യ)
	    {
		vim_memset(l, ' ', before);
		off += before;
		എങ്കിൽ (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_used);
		off += n_used;
	    }
	    അല്ലെങ്കിൽ
	    {
		off = before + after + padding + n_used;
		cells += before + after + padding;
	    }
	    എങ്കിൽ (n_attr != ശൂന്യ)
	    {
		എങ്കിൽ (n_used < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    എങ്കിൽ (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			എങ്കിൽ (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_setup(&vc, (char_u *)"utf-8", p_enc);
			    എങ്കിൽ (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ശൂന്യ);
				എങ്കിൽ (cp == ശൂന്യ)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_setup(&vc, ശൂന്യ, ശൂന്യ);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_head_off)(l, lp);
			STRCPY(lp, cp);
			n_used = lp - l + 3 - before - padding;
			എങ്കിൽ (cp != buf)
			    vim_free(cp);
		    }
		    അല്ലെങ്കിൽ
			// change last character to '>'
			*lp = '>';
		}
		അല്ലെങ്കിൽ എങ്കിൽ (after > 0)
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
		എങ്കിൽ (above)
		    *n_attr -= after;
	    }
	}
    }

    എങ്കിൽ (n_attr == ശൂന്യ)
	തിരികെ cells;
    തിരികെ (below && col_with_padding > win_col_off(wp) && !wp->w_p_wrap);
}

# എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    പൂർണ്ണസംഖ്യ
text_prop_no_showbreak(textprop_T *tp)
{
    പൂർണ്ണസംഖ്യ	    വലത് = (tp->tp_flags & TP_FLAG_ALIGN_വലത്);
    പൂർണ്ണസംഖ്യ	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    പൂർണ്ണസംഖ്യ	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    പൂർണ്ണസംഖ്യ	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    തിരികെ (വലത് || above || below || !wrap);
}
# endif
#endif

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    സ്ഥിതം ശൂന്യം
wlv_screen_വരി(win_T *wp, winlinevars_T *wlv, പൂർണ്ണസംഖ്യ clear_end)
{
    എങ്കിൽ (wlv->row == 0 && wp->w_skipcol > 0
#എങ്കിൽ defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_മൂല്യം(wp) == NUL
#endif
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	പൂർണ്ണസംഖ്യ off = (പൂർണ്ണസംഖ്യ)(current_Screen_വരി - Screen_വരികൾ);
	പൂർണ്ണസംഖ്യ max_off = off + screen_Columns;
	പൂർണ്ണസംഖ്യ skip = 0;

	എങ്കിൽ (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    ആകുമ്പോൾ (skip < wp->w_width && VIM_ISDIGIT(Screen_വരികൾ[off]))
	    {
		++off;
		++skip;
	    }

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 3 && i + skip < wp->w_width; ++i)
	{
	    എങ്കിൽ ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_വരികൾ[off + 1] = ' ';
	    Screen_വരികൾ[off] = '<';
	    എങ്കിൽ (enc_utf8)
		Screen_വരികൾ_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_വരി(wp, wlv->screen_row, wp->w_wincol, wlv->col,
		    clear_end ? wp->w_width : -wp->w_width,
		    wlv->vcol - 1, wlv->screen_വരി_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    സ്ഥിതം ശൂന്യം
draw_screen_വരി(win_T *wp, winlinevars_T *wlv)
{
#ifdef FEAT_SYN_HL
    long	v;
    പൂർണ്ണസംഖ്യ		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    എങ്കിൽ (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    അല്ലെങ്കിൽ
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_width - wlv->col - 1 :
# endif
	wlv->col;
    // check if line ends before left margin
    എങ്കിൽ (wlv->vcol < v + wcol - win_col_off(wp))
	wlv->vcol = v + wcol - win_col_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->col -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# അല്ലെങ്കിൽ
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    എങ്കിൽ (wlv->draw_color_col)
	wlv->draw_color_col = advance_color_col(VCOL_HLC, &wlv->color_cols);

    എങ്കിൽ (((wp->w_p_cuc
		    && (പൂർണ്ണസംഖ്യ)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (പൂർണ്ണസംഖ്യ)wp->w_virtcol <
			 (long)wp->w_width * (wlv->row - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_col
# ifdef വരി_ATTR
		|| wlv->വരി_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	പൂർണ്ണസംഖ്യ	rightmost_vcol = 0;
	പൂർണ്ണസംഖ്യ	i;

	എങ്കിൽ (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	എങ്കിൽ (wlv->draw_color_col)
	    // determine rightmost colorcolumn to possibly draw
	    വേണ്ടി (i = 0; wlv->color_cols[i] >= 0; ++i)
		എങ്കിൽ (rightmost_vcol < wlv->color_cols[i])
		    rightmost_vcol = wlv->color_cols[i];

	ആകുമ്പോൾ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->col >= 0) :
# endif
		(wlv->col < wp->w_width))
	{
	    Screen_വരികൾ[wlv->off] = ' ';
	    എങ്കിൽ (enc_utf8)
		Screen_വരികൾ_UC[wlv->off] = 0;

	    എങ്കിൽ (wlv->draw_color_col)
		wlv->draw_color_col = advance_color_col(
						   VCOL_HLC, &wlv->color_cols);

	    പൂർണ്ണസംഖ്യ attr = wlv->win_attr;
# ifdef വരി_ATTR
	    എങ്കിൽ (wlv->വരി_attr != 0)
		attr = hl_combine_attr(attr, wlv->വരി_attr);
# endif
	    എങ്കിൽ (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    അല്ലെങ്കിൽ എങ്കിൽ (wlv->draw_color_col && VCOL_HLC == *wlv->color_cols)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    ScreenCols[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->col;
	    }
	    അല്ലെങ്കിൽ
# endif
	    {
		++wlv->off;
		++wlv->col;
	    }
	    ++wlv->vcol;

	    എങ്കിൽ (VCOL_HLC > rightmost_vcol
# ifdef വരി_ATTR
		    && wlv->വരി_attr == 0
# endif
		    && wlv->win_attr == 0)
		നിർത്തുക;
	}
    }
#endif

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_വരി_flags |= SLF_INC_VCOL;
    wlv_screen_വരി(wp, wlv, TRUE);
    wlv->screen_വരി_flags &= ~SLF_INC_VCOL;
    ++wlv->row;
    ++wlv->screen_row;
}
#undef VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    സ്ഥിതം ശൂന്യം
win_വരി_start(win_T *wp UNUSED, winlinevars_T *wlv, പൂർണ്ണസംഖ്യ save_extra)
{
    wlv->col = 0;
    wlv->off = (unsigned)(current_Screen_വരി - Screen_വരികൾ);
#ifdef FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#endif

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->col = wp->w_width - 1;
	wlv->off += wlv->col;
	wlv->screen_വരി_flags |= SLF_RIGHTLEFT;
    }
#endif
    എങ്കിൽ (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_START;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ശൂന്യ;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ifdef FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#endif
#ifdef FEAT_SYN_HL
	എങ്കിൽ (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
win_വരി_continue(winlinevars_T *wlv)
{
    എങ്കിൽ (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ശൂന്യ;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    അല്ലെങ്കിൽ
	wlv->char_attr = wlv->win_attr;
}

#ifdef FEAT_SYN_HL
    സ്ഥിതം ശൂന്യം
apply_cursorline_highlight(
	winlinevars_T *wlv,
	പൂർണ്ണസംഖ്യ sign_present UNUSED)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    എങ്കിൽ (sign_present && wlv->sattr.sat_linehl > 0)
    {
	എങ്കിൽ (wlv->sattr.sat_priority >= 100)
	    wlv->വരി_attr = hl_combine_attr(wlv->cul_attr, wlv->വരി_attr);
	അല്ലെങ്കിൽ
	    wlv->വരി_attr = hl_combine_attr(wlv->വരി_attr, wlv->cul_attr);
    }
    അല്ലെങ്കിൽ
# endif
# എങ്കിൽ defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->വരി_attr = hl_combine_attr(wlv->cul_attr, wlv->വരി_attr);
# അല്ലെങ്കിൽ
	wlv->വരി_attr = wlv->cul_attr;
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
    പൂർണ്ണസംഖ്യ
win_വരി(
    win_T	*wp,
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		startrow,
    പൂർണ്ണസംഖ്യ		endrow,
    പൂർണ്ണസംഖ്യ		സംഖ്യ_only,
    spellvars_T	*spv UNUSED)
{
    winlinevars_T	wlv;		// variables passed between functions

    പൂർണ്ണസംഖ്യ		c = 0;			// init for GCC
    char_u	*വരി;			// current line
    char_u	*ptr;			// current position in "line"
    പൂർണ്ണസംഖ്യ		in_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ifdef FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ശൂന്യ;   // another p_extra to be freed
#endif
#എങ്കിൽ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    പൂർണ്ണസംഖ്യ		in_linebreak = FALSE;	// n_extra set for showing linebreak
#endif
    പൂർണ്ണസംഖ്യ		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    പൂർണ്ണസംഖ്യ		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    പൂർണ്ണസംഖ്യ		n_attr = 0;	    // chars with special attr
    പൂർണ്ണസംഖ്യ		saved_attr2 = 0;    // char_attr saved for n_attr
    പൂർണ്ണസംഖ്യ		n_attr3 = 0;	    // chars with overruling special attr
    പൂർണ്ണസംഖ്യ		saved_attr3 = 0;    // char_attr saved for n_attr3

    പൂർണ്ണസംഖ്യ		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    പൂർണ്ണസംഖ്യ		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    പൂർണ്ണസംഖ്യ		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    പൂർണ്ണസംഖ്യ		attr_pri = FALSE;	// char_attr has priority
    പൂർണ്ണസംഖ്യ		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    പൂർണ്ണസംഖ്യ		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    പൂർണ്ണസംഖ്യ		area_attr = 0;		// attributes desired by highlighting
    പൂർണ്ണസംഖ്യ		തിരയൽ_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    പൂർണ്ണസംഖ്യ		syntax_attr = 0;	// attributes desired by syntax
    പൂർണ്ണസംഖ്യ		prev_syntax_col = -1;	// column of prev_syntax_attr
    പൂർണ്ണസംഖ്യ		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    പൂർണ്ണസംഖ്യ		has_syntax = FALSE;	// this buffer has syntax highl.
    പൂർണ്ണസംഖ്യ		save_did_emsg;
#endif
#ifdef FEAT_PROP_POPUP
# define WIN_വരി_TEXT_PROP_അട്ടി_LEN 32
    പൂർണ്ണസംഖ്യ		did_വരി = FALSE;	// set to TRUE when line text done
    പൂർണ്ണസംഖ്യ		text_prop_എണ്ണം;
    പൂർണ്ണസംഖ്യ		last_textprop_text_idx = -1;
    പൂർണ്ണസംഖ്യ		text_prop_next = 0;	// next text property to use
    textprop_T	text_props_buf[WIN_വരി_TEXT_PROP_അട്ടി_LEN];
    പൂർണ്ണസംഖ്യ		text_prop_idxs_buf[WIN_വരി_TEXT_PROP_അട്ടി_LEN];
    char_u	text_prop_suffix_flags_buf[WIN_വരി_TEXT_PROP_അട്ടി_LEN + 1];
    textprop_T	*text_props = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		*text_prop_idxs = ശൂന്യ;
    char_u	*text_prop_suffix_flags = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		text_props_active = 0;
    പൂർണ്ണസംഖ്യ		text_props_need_sort = FALSE;
    proptype_T  *text_prop_type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		text_prop_attr = 0;
    പൂർണ്ണസംഖ്യ		text_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    പൂർണ്ണസംഖ്യ		text_prop_id = 0;	// active property ID
    പൂർണ്ണസംഖ്യ		text_prop_flags = 0;
    പൂർണ്ണസംഖ്യ		text_prop_above = FALSE;  // first doing virtual text above
    പൂർണ്ണസംഖ്യ		text_prop_follows = FALSE;  // another text prop to display
    പൂർണ്ണസംഖ്യ		saved_തിരയൽ_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    പൂർണ്ണസംഖ്യ		saved_area_attr = 0;	// idem for area_attr
    പൂർണ്ണസംഖ്യ		reset_extra_attr = FALSE;
#endif
#ifdef FEAT_SPELL
    പൂർണ്ണസംഖ്യ		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    പൂർണ്ണസംഖ്യ		nextlinecol = 0;	// column where nextline[] starts
    പൂർണ്ണസംഖ്യ		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    പൂർണ്ണസംഖ്യ		spell_attr = 0;		// attributes desired by spelling
    പൂർണ്ണസംഖ്യ		word_end = 0;		// last byte with same spell_attr
    പൂർണ്ണസംഖ്യ		cur_checked_col = 0;	// checked column for current line
#endif
    പൂർണ്ണസംഖ്യ		extra_check = FALSE;	// has extra highlighting
    പൂർണ്ണസംഖ്യ		multi_attr = 0;		// attributes desired by multibyte
    പൂർണ്ണസംഖ്യ		mb_l = 1;		// multi-byte byte length
    പൂർണ്ണസംഖ്യ		mb_c = 0;		// decoded multi-byte character
    പൂർണ്ണസംഖ്യ		mb_utf8 = FALSE;	// screen char is UTF-8 char
    പൂർണ്ണസംഖ്യ		u8cc[MAX_MCO];		// composing UTF-8 chars
#ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		change_start = MAXCOL;	// first col of changed area
    പൂർണ്ണസംഖ്യ		change_end = -1;	// last col of changed area
    diffline_T	വരി_changes;
    പൂർണ്ണസംഖ്യ		change_index = -1;
#endif
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    പൂർണ്ണസംഖ്യ		in_multispace = FALSE;	// in multiple consecutive spaces
    പൂർണ്ണസംഖ്യ		multispace_pos = 0;	// position in lcs-multispace string
#ifdef വരി_ATTR
    പൂർണ്ണസംഖ്യ		വരി_attr_save = 0;
#endif
    പൂർണ്ണസംഖ്യ		sign_present = FALSE;
    പൂർണ്ണസംഖ്യ		അക്കം_attr = 0;		// attribute for the number column
#ifdef FEAT_ARABIC
    പൂർണ്ണസംഖ്യ		prev_c = 0;		// previous Arabic character
    പൂർണ്ണസംഖ്യ		prev_c1 = 0;		// first composing char for prev_c
#endif
#എങ്കിൽ defined(വരി_ATTR)
    പൂർണ്ണസംഖ്യ		did_വരി_attr = 0;
#endif
#ifdef FEAT_TERMINAL
    പൂർണ്ണസംഖ്യ		get_term_attr = FALSE;
#endif

#എങ്കിൽ defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    പൂർണ്ണസംഖ്യ		ഇടത്_curline_col = 0;
    പൂർണ്ണസംഖ്യ		വലത്_curline_col = 0;
#endif

#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    പൂർണ്ണസംഖ്യ		feedback_col = 0;
    പൂർണ്ണസംഖ്യ		feedback_old_attr = -1;
#endif

#എങ്കിൽ defined(FEAT_CONCEAL) || defined(FEAT_തിരയൽ_EXTRA)
    പൂർണ്ണസംഖ്യ		match_conc	= 0;	// cchar for match functions
#endif
#എങ്കിൽ defined(FEAT_CONCEAL) || defined(FEAT_തിരയൽ_EXTRA) || defined(FEAT_LINEBREAK)
    പൂർണ്ണസംഖ്യ		on_last_col     = FALSE;
#endif
#ifdef FEAT_CONCEAL
    പൂർണ്ണസംഖ്യ		syntax_flags	= 0;
    പൂർണ്ണസംഖ്യ		syntax_seqnr	= 0;
    പൂർണ്ണസംഖ്യ		prev_syntax_id	= 0;
    പൂർണ്ണസംഖ്യ		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    പൂർണ്ണസംഖ്യ		is_concealing	= FALSE;
    പൂർണ്ണസംഖ്യ		did_wcol	= FALSE;
    പൂർണ്ണസംഖ്യ		old_boguscols   = 0;
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
#അല്ലെങ്കിൽ
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#endif

    എങ്കിൽ (startrow > endrow)		// past the end already!
	തിരികെ startrow;

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

    എങ്കിൽ (സംഖ്യ_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ifdef FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#endif
#ifdef FEAT_SYN_HL
	എങ്കിൽ (syntax_present(wp) && !wp->w_s->b_syn_error
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
	    എങ്കിൽ (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    അല്ലെങ്കിൽ
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		എങ്കിൽ (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_cols = wp->w_p_cc_cols;
	എങ്കിൽ (wlv.color_cols != ശൂന്യ)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);
#endif

#ifdef FEAT_TERMINAL
	എങ്കിൽ (term_show_buffer(wp->w_buffer))
	{
	    extra_check = TRUE;
	    get_term_attr = TRUE;
	    wlv.win_attr = term_get_attr(wp, lnum, -1);
	}
#endif

	// handle Visual active in this window
	എങ്കിൽ (VIsual_active && wp->w_buffer == curwin->w_buffer)
	{
	    pos_T	*മുകൾ, *bot;

	    എങ്കിൽ (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		മുകൾ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Visual is before curwin->w_cursor
		മുകൾ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_in_visual_area = (lnum >= മുകൾ->lnum && lnum <= bot->lnum);
	    എങ്കിൽ (VIsual_mode == Ctrl_V)
	    {
		// block mode
		എങ്കിൽ (lnum_in_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// non-block mode
		എങ്കിൽ (lnum > മുകൾ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		അല്ലെങ്കിൽ എങ്കിൽ (lnum == മുകൾ->lnum)
		{
		    എങ്കിൽ (VIsual_mode == 'V')	// linewise
			wlv.fromcol = 0;
		    അല്ലെങ്കിൽ
		    {
			getvvcol(wp, മുകൾ, &wlv.fromcol, ശൂന്യ, ശൂന്യ, 0);
			എങ്കിൽ (gchar_pos(മുകൾ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		എങ്കിൽ (VIsual_mode != 'V' && lnum == bot->lnum)
		{
		    എങ്കിൽ (*p_sel == 'e' && bot->col == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (bot->col == MAXCOL)
			wlv.tocol = MAXCOL;
		    അല്ലെങ്കിൽ
		    {
			pos = *bot;
			എങ്കിൽ (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ശൂന്യ, ശൂന്യ, 0);
			അല്ലെങ്കിൽ
			{
			    getvvcol(wp, &pos, ശൂന്യ, ശൂന്യ, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    എങ്കിൽ (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#എങ്കിൽ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		എങ്കിൽ ((clip_star.available && !clip_star.owned
						    && clip_isautosel_star())
			|| (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#endif
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	അല്ലെങ്കിൽ എങ്കിൽ (highlight_match
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + തിരയൽ_match_വരികൾ)
	{
	    എങ്കിൽ (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ശൂന്യ, ശൂന്യ, 0);
	    അല്ലെങ്കിൽ
		wlv.fromcol = 0;
	    എങ്കിൽ (lnum == curwin->w_cursor.lnum + തിരയൽ_match_വരികൾ)
	    {
		pos.lnum = lnum;
		pos.col = തിരയൽ_match_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ശൂന്യ, ശൂന്യ, 0);
	    }
	    അല്ലെങ്കിൽ
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    എങ്കിൽ (wlv.fromcol == wlv.tocol && തിരയൽ_match_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ifdef FEAT_DIFF

    പൂർണ്ണസംഖ്യ linestatus = 0;
    wlv.filler_വരികൾ = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_FIELD(വരി_changes);

    എങ്കിൽ (linestatus < 0)
    {
	എങ്കിൽ (linestatus == -1)
	{
	    എങ്കിൽ (diff_find_change(wp, lnum, &വരി_changes))
	    {
		wlv.diff_hlf = HLF_ADD;	// added line
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (വരി_changes.അക്കം_changes > 0)
	    {
		പൂർണ്ണസംഖ്യ added = diff_change_parse(
			&വരി_changes, &വരി_changes.changes[0],
			&change_start, &change_end);
		എങ്കിൽ (change_start == 0)
		{
		    എങ്കിൽ (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    അല്ലെങ്കിൽ
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		അല്ലെങ്കിൽ
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_index = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_index = 0;
	    }
	}
	അല്ലെങ്കിൽ
	    wlv.diff_hlf = HLF_ADD;

	area_highlighting = TRUE;
    }

    എങ്കിൽ (lnum == wp->w_topline)
	wlv.filler_വരികൾ = wp->w_topfill;

    wlv.filler_todo = wlv.filler_വരികൾ;
#endif

#ifdef FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    എങ്കിൽ (sign_present)
	അക്കം_attr = wlv.sattr.sat_numhl;
#endif

#ifdef വരി_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    എങ്കിൽ (sign_present)
	wlv.വരി_attr = wlv.sattr.sat_linehl;
# endif
# എങ്കിൽ defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    എങ്കിൽ (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	wlv.വരി_attr = HL_ATTR(HLF_QFL);
# endif
    എങ്കിൽ (wlv.വരി_attr != 0)
	area_highlighting = TRUE;
#endif

#ifdef FEAT_SPELL
    എങ്കിൽ (spv->spv_has_spell && സംഖ്യ_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	എങ്കിൽ (lnum == spv->spv_checked_lnum)
	    cur_checked_col = spv->spv_checked_col;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	എങ്കിൽ (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_col = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_col = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	എങ്കിൽ (lnum < wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	{
	    വരി = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_വരി(nextline + SPWORDLEN, വരി, SPWORDLEN);
	}
	വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(വരി);
	എങ്കിൽ (*ptr == NUL)
	{
	    spv->spv_cap_col = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	അല്ലെങ്കിൽ എങ്കിൽ (spv->spv_cap_col == 0)
	    spv->spv_cap_col = ptr - വരി;

	// Copy the end of the current line into nextline[].
	എങ്കിൽ (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	അല്ലെങ്കിൽ
	{
	    v = ml_get_buf_len(wp->w_buffer, lnum);
	    എങ്കിൽ (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, വരി, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, വരി + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = വരി;

    എങ്കിൽ (wp->w_p_list)
    {
	എങ്കിൽ (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ശൂന്യ
		|| wp->w_lcs_chars.leadmultispace != ശൂന്യ
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	എങ്കിൽ (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_buffer, lnum);
	    ആകുമ്പോൾ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - വരി);
	}
	// find end of leading whitespace
	എങ്കിൽ (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ശൂന്യ ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    ആകുമ്പോൾ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    എങ്കിൽ (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    അല്ലെങ്കിൽ
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - വരി) + 1;
	}
    }

    എങ്കിൽ (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    പൂർണ്ണസംഖ്യ skipcol_in_text_prop_above = 0;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (WIN_IS_POPUP(wp))
	wlv.screen_വരി_flags |= SLF_POPUP;

    char_u *prop_start;
    text_prop_എണ്ണം = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
    എങ്കിൽ (text_prop_എണ്ണം > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	എങ്കിൽ (text_prop_എണ്ണം <= WIN_വരി_TEXT_PROP_അട്ടി_LEN)
	    text_props = text_props_buf;
	അല്ലെങ്കിൽ
	    text_props = ALLOC_MULT(textprop_T, text_prop_എണ്ണം);
	എങ്കിൽ (text_props != ശൂന്യ)
	{
	    mch_memmove(text_props, prop_start,
				     text_prop_എണ്ണം * വലുപ്പം(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *എണ്ണം_ptr = prop_start - PROP_എണ്ണം_SIZE;

	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < text_prop_എണ്ണം; ++i)
	    {
		എങ്കിൽ (text_props[i].tp_id < 0
				&& text_props[i].u.tp_text_offset > 0)
		{
		    text_props[i].u.tp_text =
			    എണ്ണം_ptr + text_props[i].u.tp_text_offset;
		    text_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		അല്ലെങ്കിൽ
		    text_props[i].u.tp_text = ശൂന്യ;
	    }
	}

	// Allocate an array for the indexes.
	എങ്കിൽ (text_prop_എണ്ണം <= WIN_വരി_TEXT_PROP_അട്ടി_LEN)
	    text_prop_idxs = text_prop_idxs_buf;
	അല്ലെങ്കിൽ
	    text_prop_idxs = ALLOC_MULT(പൂർണ്ണസംഖ്യ, text_prop_എണ്ണം);
	എങ്കിൽ (text_prop_idxs == ശൂന്യ)
	{
	    എങ്കിൽ (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    അല്ലെങ്കിൽ
		text_props = ശൂന്യ;
	}
	എങ്കിൽ (text_prop_എണ്ണം <= WIN_വരി_TEXT_PROP_അട്ടി_LEN)
	    text_prop_suffix_flags = text_prop_suffix_flags_buf;
	അല്ലെങ്കിൽ
	    text_prop_suffix_flags = ALLOC_MULT(char_u, text_prop_എണ്ണം + 1);
	എങ്കിൽ (text_prop_suffix_flags == ശൂന്യ)
	{
	    എങ്കിൽ (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    text_prop_idxs = ശൂന്യ;
	    എങ്കിൽ (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    അല്ലെങ്കിൽ
		text_props = ശൂന്യ;
	}

	എങ്കിൽ (text_props != ശൂന്യ)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < text_prop_എണ്ണം; ++i)
		എങ്കിൽ (text_props[i].tp_id < 0)
		    last_textprop_text_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    പൂർണ്ണസംഖ്യ text_width = wp->w_width - win_col_off(wp);
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = text_prop_എണ്ണം - 1; i >= 0; --i)
		എങ്കിൽ (text_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    എങ്കിൽ (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_in_text_prop_above
								 >= text_width)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_in_text_prop_above += text_width;
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = i + 1; j < text_prop_എണ്ണം; ++j)
			    text_props[j - 1] = text_props[j];
			++i;
			--text_prop_എണ്ണം;
		    }
		    അല്ലെങ്കിൽ
			++wlv.text_prop_above_എണ്ണം;
		}

	    text_prop_suffix_flags[text_prop_എണ്ണം] = 0;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = text_prop_എണ്ണം - 1; i >= 0; --i)
	    {
		പൂർണ്ണസംഖ്യ flags = text_prop_suffix_flags[i + 1];

		എങ്കിൽ (text_props[i].tp_col == MAXCOL)
		{
		    flags |= 1;
		    എങ്കിൽ (text_props[i].tp_flags & TP_FLAG_ALIGN_BELOW)
			flags |= 2;
		}
		text_prop_suffix_flags[i] = flags;
	    }
	}
    }

    എങ്കിൽ (സംഖ്യ_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.row += wlv.text_prop_above_എണ്ണം;
	എങ്കിൽ (wlv.row >= endrow)
	{
	    എങ്കിൽ (text_props != text_props_buf)
		vim_free(text_props);
	    എങ്കിൽ (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    എങ്കിൽ (text_prop_suffix_flags != text_prop_suffix_flags_buf)
		vim_free(text_prop_suffix_flags);
	    തിരികെ wlv.row;
	}
	wlv.screen_row += wlv.text_prop_above_എണ്ണം;
    }
#endif

#എങ്കിൽ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_first_char = 0;
    എങ്കിൽ (wp->w_p_lbr && സംഖ്യ_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, വരി, വരി);
	(ശൂന്യം)win_lbr_chartabsize(&cts, ശൂന്യ, ശൂന്യ);
	vcol_first_char = cts.cts_first_char;
	clear_chartabsize_arg(&cts);
    }
#endif

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    എങ്കിൽ (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_in_text_prop_above : 0;
    അല്ലെങ്കിൽ
	v = wp->w_leftcol;
    എങ്കിൽ (v > 0 && സംഖ്യ_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	പൂർണ്ണസംഖ്യ		charsize = 0;
	പൂർണ്ണസംഖ്യ		head = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, വരി, ptr);
	cts.cts_max_head_vcol = v;
	ആകുമ്പോൾ (cts.cts_vcol < v)
	{
	    head = 0;
	    charsize = win_lbr_chartabsize(&cts, &head, ശൂന്യ);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    എങ്കിൽ (*prev_ptr == NUL)
		നിർത്തുക;
	    MB_PTR_ADV(cts.cts_ptr);
	    എങ്കിൽ (wp->w_p_list)
	    {
		in_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > വരി && prev_ptr[-1] == ' '));
		എങ്കിൽ (!in_multispace)
		    multispace_pos = 0;
		അല്ലെങ്കിൽ എങ്കിൽ (cts.cts_ptr >= വരി + leadcol
					 && wp->w_lcs_chars.multispace != ശൂന്യ)
		{
		    ++multispace_pos;
		    എങ്കിൽ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cts.cts_ptr < വരി + leadcol
				     && wp->w_lcs_chars.leadmultispace != ശൂന്യ)
		{
		    ++multispace_pos;
		    എങ്കിൽ (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	എങ്കിൽ (wlv.vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_col ||
#endif
	     virtual_active() ||
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	എങ്കിൽ (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	എങ്കിൽ (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - head;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	എങ്കിൽ (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ifdef FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	എങ്കിൽ (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	എങ്കിൽ (spv->spv_has_spell)
	{
	    പൂർണ്ണസംഖ്യ		len;
	    colnr_T	linecol = (colnr_T)(ptr - വരി);
	    hlf_T	spell_hlf = HLF_എണ്ണം;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = വരി + linecol;

	    എങ്കിൽ (len == 0 || (പൂർണ്ണസംഖ്യ)wp->w_cursor.col > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		// word
		spell_hlf = HLF_എണ്ണം;
		word_end = (പൂർണ്ണസംഖ്യ)(spell_to_word_end(ptr, wp) - വരി + 1);
	    }
	    അല്ലെങ്കിൽ
	    {
		// bad word found, use attributes until end of word
		word_end = wp->w_cursor.col + len + 1;

		// Turn index into actual attributes.
		എങ്കിൽ (spell_hlf != HLF_എണ്ണം)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    എങ്കിൽ (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    എങ്കിൽ (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ifdef FEAT_തിരയൽ_EXTRA
    എങ്കിൽ (സംഖ്യ_only == 0)
    {
	v = (long)(ptr - വരി);
	area_highlighting |= prepare_തിരയൽ_hl_വരി(wp, lnum, (colnr_T)v,
					      &വരി, &screen_തിരയൽ_hl,
					      &തിരയൽ_attr);
	ptr = വരി + v; // "line" may have been updated
    }
#endif

    എങ്കിൽ ((State & MODE_തിരുകുക) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
	area_highlighting = TRUE;

#ifdef FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    എങ്കിൽ (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	എങ്കിൽ (!(wp == curwin && VIsual_active)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    എങ്കിൽ (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    അല്ലെങ്കിൽ
	    {
		വരി_attr_save = wlv.വരി_attr;
		margin_columns_win(wp, &ഇടത്_curline_col, &വലത്_curline_col);
	    }
	    area_highlighting = TRUE;
	}
    }
#endif

    win_വരി_start(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    വേണ്ടി (;;)
    {
#എങ്കിൽ defined(FEAT_CONCEAL) || defined(FEAT_തിരയൽ_EXTRA)
	പൂർണ്ണസംഖ്യ	has_match_conc = 0;	// match wants to conceal
#endif
#ifdef FEAT_CONCEAL
	പൂർണ്ണസംഖ്യ	did_decrement_ptr = FALSE;
#endif

	// Skip this quickly when working on the text.
	എങ്കിൽ (wlv.draw_state != WL_വരി)
	{
#ifdef FEAT_SYN_HL
	    എങ്കിൽ (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.വരി_attr = വരി_attr_save;
	    }
#endif
	    എങ്കിൽ (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		എങ്കിൽ (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    എങ്കിൽ (wlv.row == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    അല്ലെങ്കിൽ
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ifdef FEAT_FOLDING
	    എങ്കിൽ (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#endif
#ifdef FEAT_SIGNS
	    എങ്കിൽ (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		എങ്കിൽ (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#endif
	    എങ്കിൽ (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_col(wp, &wlv, sign_present, അക്കം_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    എങ്കിൽ (സംഖ്യ_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_വരി(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		എങ്കിൽ ((wlv.row + 1 - wlv.startrow < സംഖ്യ_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ifdef FEAT_DIFF
			|| wlv.filler_todo > 0
#endif
			)
		{
		    ++wlv.row;
		    ++wlv.screen_row;
		    എങ്കിൽ (wlv.row == endrow)
			നിർത്തുക;
#ifdef FEAT_DIFF
		    --wlv.filler_todo;
		    എങ്കിൽ (wlv.filler_todo == 0 && wp->w_botfill)
			നിർത്തുക;
#endif
		    win_വരി_start(wp, &wlv, TRUE);
		    തുടരുക;
		}
		അല്ലെങ്കിൽ
		    നിർത്തുക;
	    }

#ifdef FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    എങ്കിൽ (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#endif
#എങ്കിൽ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    എങ്കിൽ (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#endif
	    എങ്കിൽ (wlv.draw_state == WL_വരി - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_വരി;
		win_വരി_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ifdef FEAT_SYN_HL
	എങ്കിൽ (wlv.cul_screenline && wlv.draw_state == WL_വരി
		&& wlv.vcol >= ഇടത്_curline_col
		&& wlv.vcol < വലത്_curline_col)
	    apply_cursorline_highlight(&wlv, sign_present);
#endif

	// When still displaying '$' of change command, stop at cursor.
	എങ്കിൽ (dollar_vcol >= 0 && in_curline && wlv.vcol >= (long)wp->w_virtcol)
	{
	    wlv_screen_വരി(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ifdef FEAT_SYN_HL
	    എങ്കിൽ (wp->w_p_cuc)
		wlv.row = wp->w_cline_row + wp->w_cline_height;
	    അല്ലെങ്കിൽ
#endif
		wlv.row = wp->w_height;
	    നിർത്തുക;
	}

	എങ്കിൽ (wlv.draw_state == WL_വരി && (area_highlighting || extra_check))
	{
#ifdef FEAT_PROP_POPUP
	    എങ്കിൽ (text_props != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ pi;
		പൂർണ്ണസംഖ്യ bcol = (പൂർണ്ണസംഖ്യ)(ptr - വരി);

		എങ്കിൽ (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !in_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		വേണ്ടി (pi = 0; pi < text_props_active; ++pi)
		{
		    പൂർണ്ണസംഖ്യ		tpi = text_prop_idxs[pi];
		    textprop_T  *tp = &text_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    എങ്കിൽ ((tp->tp_col != MAXCOL
				       && bcol >= tp->tp_col - 1 + tp->tp_len))
		    {
			എങ്കിൽ (pi + 1 < text_props_active)
			    mch_memmove(text_prop_idxs + pi,
					text_prop_idxs + pi + 1,
					വലുപ്പം(പൂർണ്ണസംഖ്യ)
					     * (text_props_active - (pi + 1)));
			--text_props_active;
			text_props_need_sort = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			എങ്കിൽ (in_linebreak && syntax_attr == text_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		എങ്കിൽ (wlv.n_extra > 0 && in_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		ആകുമ്പോൾ (text_prop_next < text_prop_എണ്ണം)
		{
		    പൂർണ്ണസംഖ്യ active;
		    textprop_T *tp = &text_props[text_prop_next];
		    എങ്കിൽ (tp->tp_col == MAXCOL)
		    {
			എങ്കിൽ (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    active = TRUE;
			അല്ലെങ്കിൽ എങ്കിൽ (*ptr != NUL)
			    നിർത്തുക;
			അല്ലെങ്കിൽ
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    active = wp->w_p_wrap
				  || wlv.row == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (bcol < tp->tp_col - 1)
			    നിർത്തുക;
			active = bcol <= tp->tp_col - 1 + tp->tp_len;
		    }

		    എങ്കിൽ (active)
		    {
			text_prop_idxs[text_props_active++] = text_prop_next;
			text_props_need_sort = TRUE;
		    }
		    ++text_prop_next;
		}

		എങ്കിൽ (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(text_prop_type != ശൂന്യ &&
			     text_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    text_prop_attr = 0;
		    text_prop_attr_comb = 0;
		    text_prop_flags = 0;
		    text_prop_type = ശൂന്യ;
		    text_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		എങ്കിൽ (text_props_active > 0 && wlv.n_extra == 0)
		{
		    പൂർണ്ണസംഖ്യ used_tpi = -1;
		    പൂർണ്ണസംഖ്യ used_attr = 0;
		    പൂർണ്ണസംഖ്യ other_tpi = -1;

		    text_prop_above = FALSE;
		    text_prop_follows = FALSE;

		    എങ്കിൽ (text_props_need_sort)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			sort_text_props(wp->w_buffer, text_props,
					text_prop_idxs, text_props_active);
			text_props_need_sort = FALSE;
		    }

		    വേണ്ടി (pi = 0; pi < text_props_active; ++pi)
		    {
			പൂർണ്ണസംഖ്യ	    tpi = text_prop_idxs[pi];
			textprop_T  *tp = &text_props[tpi];
			proptype_T  *pt = text_prop_type_by_id(
						    wp->w_buffer, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			എങ്കിൽ (pt != ശൂന്യ
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_വലത്
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_BELOW)) == 0
				    && wlv.col >= wp->w_width))
			{
			    എങ്കിൽ (tp->tp_col == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == വരി
						&& !did_വരി
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_BELOW))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				text_prop_follows = TRUE;
				തുടരുക;
			    }

			    എങ്കിൽ (pt->pt_hl_id > 0)
				used_attr = syn_id2attr(pt->pt_hl_id);
			    text_prop_type = pt;
			    text_prop_attr =
				   hl_combine_attr(text_prop_attr, used_attr);
			    എങ്കിൽ (used_tpi >= 0 && text_props[used_tpi].tp_id < 0)
				other_tpi = used_tpi;
			    text_prop_flags = pt->pt_flags;
			    text_prop_id = tp->tp_id;
			    used_tpi = tpi;
			}
		    }
		    എങ്കിൽ (text_prop_id < 0 && used_tpi >= 0
				 && text_props[used_tpi].u.tp_text != ശൂന്യ)
		    {
			textprop_T  *tp = &text_props[used_tpi];
			char_u	    *p = tp->u.tp_text;
			പൂർണ്ണസംഖ്യ	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			പൂർണ്ണസംഖ്യ	    bail_out = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			എങ്കിൽ (p != ശൂന്യ)
			{
			    പൂർണ്ണസംഖ്യ	    വലത് = (tp->tp_flags
							& TP_FLAG_ALIGN_വലത്);
			    പൂർണ്ണസംഖ്യ	    below = (tp->tp_flags
							& TP_FLAG_ALIGN_BELOW);
			    പൂർണ്ണസംഖ്യ	    wrap = tp->tp_col < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    പൂർണ്ണസംഖ്യ	    padding = tp->tp_col == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (പൂർണ്ണസംഖ്യ)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.start_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    used_attr);
			    n_attr = mb_charlen(p);
			    text_prop_attr = 0;
			    text_prop_attr_comb = 0;
			    എങ്കിൽ (*ptr == NUL)
				// don't combine char attr after EOL
				text_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    എങ്കിൽ (text_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    എങ്കിൽ ((വലത് || above || below || !wrap
					    || padding > 0) && wp->w_width > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				പൂർണ്ണസംഖ്യ	start_വരി;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				start_വരി = text_prop_position(wp, tp,
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
				എങ്കിൽ (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				എങ്കിൽ (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				എങ്കിൽ (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.col
					       + wlv.n_extra - 2 > wp->w_width)
				    // don't bail out at end of line
				    text_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				എങ്കിൽ (start_വരി)
				{
				    draw_screen_വരി(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    എങ്കിൽ (wlv.row == endrow)
				    {
					++wlv.row;
					നിർത്തുക;
				    }
				    win_വരി_start(wp, &wlv, TRUE);
				    bail_out = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			എങ്കിൽ (skip_cells > 0)
			{
			    എങ്കിൽ (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				എങ്കിൽ (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    അല്ലെങ്കിൽ
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
			       & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_വലത്)));

			എങ്കിൽ (bail_out)
			    // starting a new line for "below"
			    തുടരുക;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (text_prop_next < text_prop_എണ്ണം
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.col == wp->w_width - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    പൂർണ്ണസംഖ്യ only_below_follows = !wp->w_p_wrap
				 && wlv.col + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_width;
		    പൂർണ്ണസംഖ്യ suffix_flags = text_prop_suffix_flags[text_prop_next];

		    text_prop_follows = (suffix_flags
					& (only_below_follows ? 2 : 1)) != 0;
		}
	    }

	    എങ്കിൽ (wlv.start_extra_for_textprop)
	    {
		wlv.start_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_തിരയൽ_attr = തിരയൽ_attr;
		saved_area_attr = area_attr;
		തിരയൽ_attr = 0;
		area_attr = 0;
	    }
#endif

	    പൂർണ്ണസംഖ്യ *area_attr_p =
#ifdef FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#endif
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    എങ്കിൽ (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ശൂന്യ
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    അല്ലെങ്കിൽ എങ്കിൽ (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    എങ്കിൽ (wlv.n_extra == 0)
	    {
#ifdef FEAT_തിരയൽ_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (long)(ptr - വരി);
		തിരയൽ_attr = update_തിരയൽ_hl(wp, lnum, (colnr_T)v, &വരി,
				      &screen_തിരയൽ_hl, &has_match_conc,
				      &match_conc, did_വരി_attr, lcs_eol_one,
				      &on_last_col);
		ptr = വരി + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		എങ്കിൽ (*ptr == NUL)
		    has_match_conc = 0;
#അല്ലെങ്കിൽ
		തിരയൽ_attr = 0;
#endif

		// Check if ComplMatchIns highlight is needed.
		എങ്കിൽ ((State & MODE_തിരുകുക) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
		{
		    പൂർണ്ണസംഖ്യ ins_match_attr =
			ins_compl_col_range_attr(lnum, (പൂർണ്ണസംഖ്യ)(ptr - വരി));
		    എങ്കിൽ (ins_match_attr > 0)
			തിരയൽ_attr =
			    hl_combine_attr(തിരയൽ_attr, ins_match_attr);
		}
	    }

#ifdef FEAT_DIFF
	    എങ്കിൽ (wlv.diff_hlf != (hlf_T)0)
	    {
		എങ്കിൽ (വരി_changes.അക്കം_changes > 0
			&& change_index >= 0
			&& change_index < വരി_changes.അക്കം_changes - 1)
		{
		    എങ്കിൽ (ptr - വരി >= വരി_changes.changes[change_index+1].dc_start[വരി_changes.bufidx])
			change_index += 1;
		}
		പൂർണ്ണസംഖ്യ added = FALSE;
		എങ്കിൽ (വരി_changes.അക്കം_changes > 0 && change_index >= 0 && change_index < വരി_changes.അക്കം_changes)
		{
		    added = diff_change_parse(&വരി_changes,
			    &വരി_changes.changes[change_index],
			    &change_start,
			    &change_end);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		എങ്കിൽ (wlv.diff_hlf == HLF_CHD && ptr - വരി >= change_start
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		എങ്കിൽ ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - വരി >= change_end && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.വരി_attr = HL_ATTR(wlv.diff_hlf);
		എങ്കിൽ (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ഇടത്_curline_col
					    && wlv.vcol <= വലത്_curline_col)))
		    wlv.വരി_attr = hl_combine_attr(
					  wlv.വരി_attr, HL_ATTR(HLF_CUL));
	    }
#endif

#ifdef FEAT_SYN_HL
	    എങ്കിൽ (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		എങ്കിൽ (get_term_attr)
		    syntax_attr = term_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		എങ്കിൽ (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (long)(ptr - വരി);
		    എങ്കിൽ (v == prev_syntax_col)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    അല്ലെങ്കിൽ
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ശൂന്യ, FALSE);
			prev_syntax_col = v;
			prev_syntax_attr = syntax_attr;
		    }

		    എങ്കിൽ (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    അല്ലെങ്കിൽ
			did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		    എങ്കിൽ (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = വരി + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    എങ്കിൽ (*ptr == NUL)
			syntax_flags = 0;
		    അല്ലെങ്കിൽ
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    എങ്കിൽ (text_prop_type != ശൂന്യ)
	    {
		എങ്കിൽ (text_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, text_prop_attr);
		അല്ലെങ്കിൽ
		    syntax_attr = text_prop_attr;
		text_prop_attr_comb = syntax_attr;
	    }
# endif
#endif

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ifdef വരി_ATTR
	    എങ്കിൽ (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.വരി_attr, area_attr);
		എങ്കിൽ (!highlight_match)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(തിരയൽ_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (തിരയൽ_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.വരി_attr, തിരയൽ_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (wlv.വരി_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.വരി_attr);
# അല്ലെങ്കിൽ
		wlv.char_attr = wlv.വരി_attr;
# endif
		attr_pri = FALSE;
	    }
#അല്ലെങ്കിൽ
	    എങ്കിൽ (area_attr != 0)
		wlv.char_attr = area_attr;
	    അല്ലെങ്കിൽ എങ്കിൽ (തിരയൽ_attr != 0)
		wlv.char_attr = തിരയൽ_attr;
#endif
	    അല്ലെങ്കിൽ
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#അല്ലെങ്കിൽ
		wlv.char_attr = 0;
#endif
	    }
#ifdef FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    എങ്കിൽ (text_prop_type != ശൂന്യ && (text_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, text_prop_attr);
#endif
	}

	// combine attribute with HLF_WIN
	എങ്കിൽ (wlv.win_attr != 0)
	{
	    എങ്കിൽ (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    അല്ലെങ്കിൽ
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
	എങ്കിൽ (wlv.n_extra > 0)
	{
	    എങ്കിൽ (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		അല്ലെങ്കിൽ
		    mb_utf8 = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		c = *wlv.p_extra;
		എങ്കിൽ (has_mbyte)
		{
		    mb_c = c;
		    എങ്കിൽ (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			എങ്കിൽ (mb_l > wlv.n_extra)
			    mb_l = 1;
			അല്ലെങ്കിൽ എങ്കിൽ (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			എങ്കിൽ (mb_l >= wlv.n_extra)
			    mb_l = 1;
			അല്ലെങ്കിൽ എങ്കിൽ (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    എങ്കിൽ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    എങ്കിൽ ((
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
			എങ്കിൽ (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#endif
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    അല്ലെങ്കിൽ
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#എങ്കിൽ defined(FEAT_PROP_POPUP)
	    എങ്കിൽ (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		എങ്കിൽ (wlv.saved_n_extra <= 0)
		{
		    എങ്കിൽ (തിരയൽ_attr == 0)
			തിരയൽ_attr = saved_തിരയൽ_attr;
		    എങ്കിൽ (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    എങ്കിൽ (wlv.extra_for_textprop)
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
	അല്ലെങ്കിൽ
	{
#ifdef FEAT_LINEBREAK
	    പൂർണ്ണസംഖ്യ		c0;
#endif
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ifdef FEAT_LINEBREAK
	    c0 = *ptr;
#endif
	    എങ്കിൽ (c == NUL)
	    {
#ifdef FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_വരി = TRUE;
#endif
		// no more cells to skip
		skip_cells = 0;
	    }

	    എങ്കിൽ (has_mbyte)
	    {
		mb_c = c;
		എങ്കിൽ (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    എങ്കിൽ (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			എങ്കിൽ (mb_c < 0x80)
			{
			    c = mb_c;
#ifdef FEAT_LINEBREAK
			    c0 = mb_c;
#endif
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			എങ്കിൽ (utf_iscomposing(mb_c))
			{
			    പൂർണ്ണസംഖ്യ i;

			    വേണ്ടി (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    എങ്കിൽ ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ifdef FEAT_RIGHTLEFT
			എങ്കിൽ (wp->w_p_rl)		// reverse
			    rl_mirror(wlv.extra);
#endif
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (പൂർണ്ണസംഖ്യ)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			എങ്കിൽ (area_attr == 0 && തിരയൽ_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ifdef FEAT_ARABIC
		    അല്ലെങ്കിൽ എങ്കിൽ (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			പൂർണ്ണസംഖ്യ	pc, pc1, nc;
			പൂർണ്ണസംഖ്യ	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			എങ്കിൽ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			അല്ലെങ്കിൽ
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    അല്ലെങ്കിൽ
			prev_c = mb_c;
#endif
		}
		അല്ലെങ്കിൽ	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    എങ്കിൽ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    അല്ലെങ്കിൽ എങ്കിൽ (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			എങ്കിൽ (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_buffer, wlv.extra, c);
				wlv.n_extra = (പൂർണ്ണസംഖ്യ)STRLEN(wlv.extra) - 1;
			    }
			    അല്ലെങ്കിൽ
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
			    എങ്കിൽ (area_attr == 0 && തിരയൽ_attr == 0)
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
		എങ്കിൽ ((
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
		അല്ലെങ്കിൽ എങ്കിൽ (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		എങ്കിൽ (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    എങ്കിൽ (area_attr == 0 && തിരയൽ_attr == 0)
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

	    എങ്കിൽ (extra_check)
	    {
#ifdef FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (long)(ptr - വരി);
		എങ്കിൽ (spv->spv_has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    എങ്കിൽ (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			പൂർണ്ണസംഖ്യ	len;
			hlf_T	spell_hlf = HLF_എണ്ണം;

			എങ്കിൽ (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			എങ്കിൽ ((prev_ptr - വരി) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - വരി) - nextlinecol;
			അല്ലെങ്കിൽ
			    p = prev_ptr;
			spv->spv_cap_col -= (പൂർണ്ണസംഖ്യ)(prev_ptr - വരി);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_col,
							    spv->spv_unchanged);
			word_end = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			എങ്കിൽ (spell_hlf != HLF_എണ്ണം
				&& (State & MODE_തിരുകുക)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - വരി)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_എണ്ണം;
			    spell_redraw_lnum = lnum;
			}

			എങ്കിൽ (spell_hlf == HLF_എണ്ണം && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_col = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			എങ്കിൽ (spell_hlf != HLF_എണ്ണം)
			    spell_attr = highlight_attr[spell_hlf];

			എങ്കിൽ (spv->spv_cap_col > 0)
			{
			    എങ്കിൽ (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_col
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_col = ((p - nextline)
					    + spv->spv_cap_col - nextline_idx);
			    }
			    അല്ലെങ്കിൽ
				// Compute the actual column.
				spv->spv_cap_col += (prev_ptr - വരി);
			}
		    }
		}
		എങ്കിൽ (spell_attr != 0)
		{
		    എങ്കിൽ (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    അല്ലെങ്കിൽ
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
		എങ്കിൽ ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((പൂർണ്ണസംഖ്യ)*ptr))
		    wlv.need_lbr = TRUE;
#endif
#ifdef FEAT_LINEBREAK
		// Found last space before word: check for line break.
		എങ്കിൽ (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((പൂർണ്ണസംഖ്യ)*ptr))
		{
		    പൂർണ്ണസംഖ്യ	    mb_off = has_mbyte ? (*mb_head_off)(വരി, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_first_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, വരി, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_text = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ശൂന്യ, ശൂന്യ) - 1;
		    clear_chartabsize_arg(&cts);

		    എങ്കിൽ (on_last_col && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			തിരയൽ_attr = 0;

		    എങ്കിൽ (c == TAB && wlv.n_extra + wlv.col > wp->w_width)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
# അല്ലെങ്കിൽ
			wlv.n_extra = (പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts
				    - wlv.vcol % (പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    എങ്കിൽ (wlv.n_extra > 0 && c != TAB)
			in_linebreak = TRUE;
# endif
		    എങ്കിൽ (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			എങ്കിൽ (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			എങ്കിൽ (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif
		എങ്കിൽ (wp->w_p_list)
		{
		    in_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > വരി && prev_ptr[-1] == ' '));
		    എങ്കിൽ (!in_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		എങ്കിൽ (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (in_multispace
					&& wp->w_lcs_chars.multispace != ശൂന്യ))
				&& ptr - വരി >= leadcol
				&& ptr - വരി <= trailcol)))
		{
		    എങ്കിൽ (in_multispace && wp->w_lcs_chars.multispace != ശൂന്യ)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			എങ്കിൽ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    അല്ലെങ്കിൽ
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    എങ്കിൽ (area_attr == 0 && തിരയൽ_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    അല്ലെങ്കിൽ
			mb_utf8 = FALSE;
		}

		എങ്കിൽ (c == ' ' && ((trailcol != MAXCOL && ptr > വരി + trailcol)
				    || (leadcol != 0 && ptr < വരി + leadcol)))
		{
		    എങ്കിൽ (leadcol != 0 && in_multispace && ptr < വരി + leadcol
			    && wp->w_lcs_chars.leadmultispace != ശൂന്യ)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			എങ്കിൽ (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    അല്ലെങ്കിൽ എങ്കിൽ (ptr > വരി + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    അല്ലെങ്കിൽ എങ്കിൽ (ptr < വരി + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    അല്ലെങ്കിൽ എങ്കിൽ (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    എങ്കിൽ (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    അല്ലെങ്കിൽ
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    എങ്കിൽ (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		എങ്കിൽ (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    പൂർണ്ണസംഖ്യ	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    long    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    പൂർണ്ണസംഖ്യ	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    പൂർണ്ണസംഖ്യ	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    പൂർണ്ണസംഖ്യ	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    എങ്കിൽ (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < വരി + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ifdef FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_മൂല്യം(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    എങ്കിൽ (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#endif
		    // tab amount depends on current column
#ifdef FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
#അല്ലെങ്കിൽ
		    tab_len = (പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts
			       - vcol_adjusted % (പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts - 1;
#endif

#ifdef FEAT_LINEBREAK
		    എങ്കിൽ (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ifdef FEAT_LINEBREAK
		    അല്ലെങ്കിൽ
		    {
			char_u	*p;
			പൂർണ്ണസംഖ്യ	len;
			പൂർണ്ണസംഖ്യ	i;
			പൂർണ്ണസംഖ്യ	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			എങ്കിൽ (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			എങ്കിൽ (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			എങ്കിൽ (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    പൂർണ്ണസംഖ്യ tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    എങ്കിൽ (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    എങ്കിൽ (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    എങ്കിൽ (p == ശൂന്യ)
				wlv.n_extra = 0;
			    അല്ലെങ്കിൽ
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				വേണ്ടി (i = 0; i < tab_len; i++)
				{
				    പൂർണ്ണസംഖ്യ lcs = lcs_tab2;

				    എങ്കിൽ (*p == NUL)
				    {
					tab_len = i;
					നിർത്തുക;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    എങ്കിൽ (lcs_tab3 && i == tab_len - 1)
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
				എങ്കിൽ (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			പൂർണ്ണസംഖ്യ vc_saved = wlv.vcol_off_co;

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
			എങ്കിൽ (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#endif
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    എങ്കിൽ (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ifdef FEAT_LINEBREAK
			എങ്കിൽ (wp->w_p_lbr && wlv.p_extra != ശൂന്യ
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			അല്ലെങ്കിൽ
#endif
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (c == NUL
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
#എങ്കിൽ defined(FEAT_DIFF) || defined(വരി_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    എങ്കിൽ (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef വരി_ATTR
			    &&
#  endif
# endif
# ifdef വരി_ATTR
			    wlv.വരി_attr == 0
# endif
		       )
#endif
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			എങ്കിൽ (!(area_highlighting && virtual_active()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    എങ്കിൽ (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    അല്ലെങ്കിൽ
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    എങ്കിൽ (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    അല്ലെങ്കിൽ
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		അല്ലെങ്കിൽ എങ്കിൽ (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_buffer, c);
		    എങ്കിൽ (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    എങ്കിൽ ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#endif
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ifdef FEAT_LINEBREAK
		    എങ്കിൽ (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			എങ്കിൽ (p == ശൂന്യ)
			    wlv.n_extra = 0;
			അല്ലെങ്കിൽ
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    അല്ലെങ്കിൽ
#endif
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    എങ്കിൽ (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ifdef FEAT_PROP_POPUP
			എങ്കിൽ (text_prop_type != ശൂന്യ &&
				 text_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(text_prop_attr,
							       wlv.extra_attr);
#endif

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active
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
#എങ്കിൽ defined(വരി_ATTR)
		അല്ലെങ്കിൽ എങ്കിൽ ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.വരി_attr != 0
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
		    ++did_വരി_attr;

		    // don't do search HL for the rest of the line
		    എങ്കിൽ (wlv.വരി_attr != 0 && wlv.char_attr == തിരയൽ_attr
					&& (did_വരി_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.വരി_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    എങ്കിൽ (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_വരി
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    എങ്കിൽ (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			എങ്കിൽ (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    എങ്കിൽ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ഇടത്_curline_col
					    && wlv.vcol <= വലത്_curline_col)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    എങ്കിൽ (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			എങ്കിൽ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    എങ്കിൽ (!wlv.cul_screenline
				    || (wlv.vcol >= ഇടത്_curline_col
					     && wlv.vcol <= വലത്_curline_col))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (wlv.വരി_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.വരി_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			എങ്കിൽ (wlv.vcol == 0)
			{
			    എങ്കിൽ (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    അല്ലെങ്കിൽ എങ്കിൽ (തിരയൽ_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, തിരയൽ_attr);
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    എങ്കിൽ (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_വരി(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_match_conc > 0)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ശൂന്യ))
	    {
		പൂർണ്ണസംഖ്യ syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		എങ്കിൽ (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_match_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_match_conc && match_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    എങ്കിൽ (has_match_conc && match_conc)
			c = match_conc;
		    അല്ലെങ്കിൽ എങ്കിൽ (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    അല്ലെങ്കിൽ
			c = ' ';

		    എങ്കിൽ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    അല്ലെങ്കിൽ
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    എങ്കിൽ (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    എങ്കിൽ (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			എങ്കിൽ (wp->w_p_rl)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			അല്ലെങ്കിൽ
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.col += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    എങ്കിൽ (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#endif // FEAT_CONCEAL
	}

#ifdef FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	എങ്കിൽ (!did_wcol && wlv.draw_state == WL_വരി
		&& in_curline && conceal_cursor_വരി(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (wp->w_p_rl)
		wp->w_wcol = wp->w_width - wlv.col + wlv.boguscols - 1;
	    അല്ലെങ്കിൽ
# endif
		wp->w_wcol = wlv.col - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    എങ്കിൽ (wlv.vcol + skip_cells < wp->w_virtcol)
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
	എങ്കിൽ (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_വരി
		&& (!attr_pri
#ifdef FEAT_PROP_POPUP
		    || (text_prop_flags & PT_FLAG_OVERRIDE)
#endif
		   ))
	{
#ifdef വരി_ATTR
	    എങ്കിൽ (wlv.വരി_attr)
		wlv.char_attr = hl_combine_attr(wlv.വരി_attr, wlv.extra_attr);
	    അല്ലെങ്കിൽ
#endif
		wlv.char_attr = wlv.extra_attr;
#ifdef FEAT_PROP_POPUP
	    എങ്കിൽ (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#endif
	}

#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	എങ്കിൽ (p_imst == IM_ON_THE_SPOT
		&& xic != ശൂന്യ
		&& lnum == wp->w_cursor.lnum
		&& (State & MODE_തിരുകുക)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_വരി)
	{
	    colnr_T tcol;

	    എങ്കിൽ (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ശൂന്യ, ശൂന്യ, 0);
	    അല്ലെങ്കിൽ
		tcol = preedit_end_col;
	    എങ്കിൽ ((long)preedit_start_col <= wlv.vcol && wlv.vcol < (long)tcol)
	    {
		എങ്കിൽ (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_col);
		എങ്കിൽ (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (feedback_old_attr >= 0)
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
	എങ്കിൽ (lcs_prec_todo != NUL
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
	    എങ്കിൽ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    അല്ലെങ്കിൽ
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    എങ്കിൽ (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	എങ്കിൽ ((c == NUL
#എങ്കിൽ defined(വരി_ATTR)
		|| did_വരി_attr == 1
#endif
		) && wlv.eol_hl_off == 0)
	{
#ifdef FEAT_തിരയൽ_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    പൂർണ്ണസംഖ്യ prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_തിരയൽ_hl,
					      (long)(ptr - വരി) - (c == NUL));
#endif
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    എങ്കിൽ (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_തിരയൽ_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_active))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# എങ്കിൽ defined(വരി_ATTR)
			    && did_വരി_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		പൂർണ്ണസംഖ്യ n = 0;

#ifdef FEAT_RIGHTLEFT
		എങ്കിൽ (wp->w_p_rl)
		{
		    എങ്കിൽ (wlv.col < 0)
			n = 1;
		}
		അല്ലെങ്കിൽ
#endif
		{
		    എങ്കിൽ (wlv.col >= wp->w_width)
			n = -1;
		}
		എങ്കിൽ (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.col += n;
		}
		അല്ലെങ്കിൽ
		{
		    // Add a blank character to highlight.
		    Screen_വരികൾ[wlv.off] = ' ';
		    എങ്കിൽ (enc_utf8)
			Screen_വരികൾ_UC[wlv.off] = 0;
		}
#ifdef FEAT_തിരയൽ_EXTRA
		എങ്കിൽ (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_തിരയൽ_match_hl(wp, &screen_തിരയൽ_hl,
					   (long)(ptr - വരി), &wlv.char_attr);
		}
#endif
		ScreenAttrs[wlv.off] = wlv.char_attr;
		ScreenCols[wlv.off] = wlv.vcol;
#ifdef FEAT_RIGHTLEFT
		എങ്കിൽ (wp->w_p_rl)
		{
		    --wlv.col;
		    --wlv.off;
		}
		അല്ലെങ്കിൽ
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
	എങ്കിൽ (c == NUL)
	{
#ifdef FEAT_PROP_POPUP
	    എങ്കിൽ (text_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		draw_screen_വരി(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		എങ്കിൽ (in_curline)
		{
		    curwin->w_cline_row = startrow;
		    curwin->w_cline_height = wlv.row - startrow;
#ifdef FEAT_FOLDING
		    curwin->w_cline_folded = false;
#endif
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		നിർത്തുക;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	പൂർണ്ണസംഖ്യ lcs_ext = get_lcs_ext(wp);
	എങ്കിൽ (lcs_ext != NUL
		&& wlv.draw_state == WL_വരി
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
	    എങ്കിൽ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    അല്ലെങ്കിൽ
		mb_utf8 = FALSE;
	}

#ifdef FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	എങ്കിൽ (wlv.draw_color_col)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	എങ്കിൽ (((wlv.draw_state == WL_വരി
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_in_visual_area
		&& തിരയൽ_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    എങ്കിൽ (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (wlv.draw_color_col && VCOL_HLC == *wlv.color_cols)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#endif

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	എങ്കിൽ (wlv.draw_state < WL_വരി || skip_cells <= 0)
	{
	    // Store the character.
#എങ്കിൽ defined(FEAT_RIGHTLEFT)
	    എങ്കിൽ (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.col;
	    }
#endif
	    Screen_വരികൾ[wlv.off] = c;
	    എങ്കിൽ (enc_dbcs == DBCS_JPNU)
	    {
		എങ്കിൽ ((mb_c & 0xff00) == 0x8e00)
		    Screen_വരികൾ[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
	    {
		എങ്കിൽ (mb_utf8)
		{
		    പൂർണ്ണസംഖ്യ i;

		    Screen_വരികൾ_UC[wlv.off] = mb_c;
		    എങ്കിൽ ((c & 0xff) == 0)
			Screen_വരികൾ[wlv.off] = 0x80;   // avoid storing zero
		    വേണ്ടി (i = 0; i < Screen_mco; ++i)
		    {
			Screen_വരികൾ_C[i][wlv.off] = u8cc[i];
			എങ്കിൽ (u8cc[i] == 0)
			    നിർത്തുക;
		    }
		}
		അല്ലെങ്കിൽ
		    Screen_വരികൾ_UC[wlv.off] = 0;
	    }
	    എങ്കിൽ (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    അല്ലെങ്കിൽ
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    എങ്കിൽ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
		ScreenCols[wlv.off] = wlv.vcol;
	    അല്ലെങ്കിൽ
		ScreenCols[wlv.off] = -1;

	    എങ്കിൽ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.col;
		എങ്കിൽ (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_വരികൾ[wlv.off] = 0;
		അല്ലെങ്കിൽ
		    // DBCS: Put second byte in the second screen char.
		    Screen_വരികൾ[wlv.off] = mb_c & 0xff;

		എങ്കിൽ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
			)
		    ScreenCols[wlv.off] = ++wlv.vcol;
		അല്ലെങ്കിൽ
		    ScreenCols[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		എങ്കിൽ (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ifdef FEAT_RIGHTLEFT
		എങ്കിൽ (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.col;
		}
#endif
	    }
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.col;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		++wlv.off;
		++wlv.col;
	    }
	}
#ifdef FEAT_CONCEAL
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_p_cole > 0 && is_concealing)
	{
	    പൂർണ്ണസംഖ്യ concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    എങ്കിൽ (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    എങ്കിൽ (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    എങ്കിൽ (wp->w_p_wrap)
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
		പൂർണ്ണസംഖ്യ adjust_col = !WIN_IS_POPUP(wp);
# അല്ലെങ്കിൽ
		പൂർണ്ണസംഖ്യ adjust_col = TRUE;
# endif
		എങ്കിൽ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    എങ്കിൽ (wp->w_p_rl)
		    {
			എങ്കിൽ (adjust_col)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    അല്ലെങ്കിൽ
# endif
		    {
			എങ്കിൽ (adjust_col)
			{
			    wlv.col += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		എങ്കിൽ (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    എങ്കിൽ (wp->w_p_rl)
		    {
			എങ്കിൽ (adjust_col)
			{
			    --wlv.boguscols;
			    --wlv.col;
			}
		    }
		    അല്ലെങ്കിൽ
# endif
		    {
			എങ്കിൽ (adjust_col)
			{
			    ++wlv.boguscols;
			    ++wlv.col;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		എങ്കിൽ (wp->w_p_rl)
		{
		    എങ്കിൽ (adjust_col)
		    {
			--wlv.boguscols;
			--wlv.col;
		    }
		}
		അല്ലെങ്കിൽ
# endif
		{
		    എങ്കിൽ (adjust_col)
		    {
			++wlv.boguscols;
			++wlv.col;
		    }
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif // FEAT_CONCEAL
	അല്ലെങ്കിൽ
	    --skip_cells;

	എങ്കിൽ (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	എങ്കിൽ (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		)
	    ++wlv.vcol;

#ifdef FEAT_SYN_HL
	എങ്കിൽ (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#endif

	// restore attributes after "precedes" in 'listchars'
	എങ്കിൽ (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	എങ്കിൽ (n_attr > 0 && wlv.draw_state == WL_വരി
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	എങ്കിൽ (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	എങ്കിൽ ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.col < 0) :
#endif
				    (wlv.col >= wp->w_width))
		&& (wlv.draw_state != WL_വരി
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
	    എങ്കിൽ (wlv.boguscols != 0 && (
# ifdef വരി_ATTR
			wlv.വരി_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		പൂർണ്ണസംഖ്യ attr = wlv.win_attr;
# ifdef വരി_ATTR
		എങ്കിൽ (wlv.വരി_attr != 0)
		    attr = hl_combine_attr(attr, wlv.വരി_attr);
# endif
		ആകുമ്പോൾ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.col >= 0 :
# endif
			    wlv.col < wp->w_width))
		{
		    Screen_വരികൾ[wlv.off] = ' ';
		    എങ്കിൽ (enc_utf8)
			Screen_വരികൾ_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    ScreenCols[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    എങ്കിൽ (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.col--;
			wlv.boguscols++;
		    }
		    അല്ലെങ്കിൽ
# endif
		    {
			wlv.off++;
			wlv.col++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_വരി(wp, &wlv, TRUE);
	    wlv.col += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#അല്ലെങ്കിൽ
	    wlv_screen_വരി(wp, &wlv, TRUE);
#endif
	    ++wlv.row;
	    ++wlv.screen_row;

	    // When not wrapping and finished diff lines, break here.
	    എങ്കിൽ (!wp->w_p_wrap
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
			&& !text_prop_above
			&& !text_prop_follows
#endif
		       )
		നിർത്തുക;
#ifdef FEAT_PROP_POPUP
	    എങ്കിൽ (!wp->w_p_wrap && text_prop_follows && !text_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = വരി + (size_t)ml_get_buf_len(wp->w_buffer, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#endif

	    // When the window is too narrow draw all "@" lines.
	    എങ്കിൽ (wlv.draw_state != WL_വരി
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
	    എങ്കിൽ (wlv.row == endrow)
	    {
		++wlv.row;
		നിർത്തുക;
	    }

	    എങ്കിൽ (screen_cur_row == wlv.screen_row - 1
#ifdef FEAT_DIFF
		     && wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
		     && !text_prop_above && !text_prop_follows
#endif
		     && wp->w_width == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		വരി_Wraps[wlv.screen_row - 1] = TRUE;

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
		എങ്കിൽ (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
			 && !(has_mbyte
			     && ((*mb_off2cells)(വരി_Offset[wlv.screen_row],
				   വരി_Offset[wlv.screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     വരി_Offset[wlv.screen_row - 1]
							    + (പൂർണ്ണസംഖ്യ)topframe->fr_width - 2,
				     വരി_Offset[wlv.screen_row]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    എങ്കിൽ (screen_cur_col != wp->w_width)
			screen_char(വരി_Offset[wlv.screen_row - 1]
						       + (unsigned)topframe->fr_width - 1,
				       wlv.screen_row - 1, (പൂർണ്ണസംഖ്യ)(topframe->fr_width - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    എങ്കിൽ (has_mbyte && MB_BYTE2LEN(Screen_വരികൾ[വരി_Offset[
				     wlv.screen_row - 1] + (topframe->fr_width - 1)]) > 1)
			out_char(' ');
		    അല്ലെങ്കിൽ
			out_char(Screen_വരികൾ[വരി_Offset[wlv.screen_row - 1]
							    + (topframe->fr_width - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[വരി_Offset[wlv.screen_row]] = (sattr_T)-1;
		    screen_start();	// don't know where cursor is now
		}
	    }

	    win_വരി_start(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ifdef FEAT_LINEBREAK
	    എങ്കിൽ (!wlv.dont_use_showbreak
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
	    എങ്കിൽ (wlv.filler_todo == 0 && wp->w_botfill)
		നിർത്തുക;
#endif
	}

    }	// for every character in the line
#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (text_props != text_props_buf)
	vim_free(text_props);
    എങ്കിൽ (text_prop_idxs != text_prop_idxs_buf)
	vim_free(text_prop_idxs);
    എങ്കിൽ (text_prop_suffix_flags != text_prop_suffix_flags_buf)
	vim_free(text_prop_suffix_flags);
    vim_free(p_extra_free2);
#endif

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    തിരികെ wlv.row;
}
