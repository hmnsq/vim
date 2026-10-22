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

#అన్తర్భావ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    స్థిర పూర్ణాంకం
get_lcs_ext(win_T *wp)
{
    ఒకవేళ (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	తిరిగి NUL;
    ఒకవేళ (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	తిరిగి '>';
    తిరిగి wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#నిర్వచితమైతే FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    స్థిర పూర్ణాంకం
advance_color_నిలువు(పూర్ణాంకం vcol, పూర్ణాంకం **color_నిలువుల_సంఖ్య)
{
    అయితే (**color_నిలువుల_సంఖ్య >= 0 && vcol > **color_నిలువుల_సంఖ్య)
	++*color_నిలువుల_సంఖ్య;
    తిరిగి (**color_నిలువుల_సంఖ్య >= 0);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    స్థిర శూన్య
margin_columns_win(win_T *wp, పూర్ణాంకం *ఎడమ_నిలువు, పూర్ణాంకం *కుడి_నిలువు)
{
    // cache previous calculations depending on w_virtcol
    స్థిర పూర్ణాంకం saved_w_virtcol;
    స్థిర win_T *prev_wp;
    స్థిర పూర్ణాంకం prev_width1;
    స్థిర పూర్ణాంకం prev_width2;
    స్థిర పూర్ణాంకం prev_ఎడమ_నిలువు;
    స్థిర పూర్ణాంకం prev_కుడి_నిలువు;

    పూర్ణాంకం cur_నిలువు_off = win_నిలువు_off(wp);
    పూర్ణాంకం	width1;
    పూర్ణాంకం	width2;

    width1 = wp->w_వెడల్పు - cur_నిలువు_off;
    width2 = width1 + win_నిలువు_off2(wp);

    ఒకవేళ (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*కుడి_నిలువు = prev_కుడి_నిలువు;
	*ఎడమ_నిలువు = prev_ఎడమ_నిలువు;
	తిరిగి;
    }

    *ఎడమ_నిలువు = 0;
    *కుడి_నిలువు = width1;

    ఒకవేళ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*కుడి_నిలువు = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    ఒకవేళ (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ఎడమ_నిలువు = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ఎడమ_నిలువు = *ఎడమ_నిలువు;
    prev_కుడి_నిలువు = *కుడి_నిలువు;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define పంక్తి_ATTR
#అయితే_ముగింపు

// structure with variables passed between win_line() and other functions
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    పూర్ణాంకం		startrow;	// first row in the window to be drawn
    పూర్ణాంకం		అడ్డువరుస;		// row in the window, excl w_winrow
    పూర్ణాంకం		screen_అడ్డువరుస;	// row on the screen, incl w_winrow

    దీర్ఘ	vcol;		// virtual column, before wrapping
    పూర్ణాంకం		నిలువు;		// visual column on screen, after wrapping
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    పూర్ణాంకం		vcol_off_co;	// offset for concealed characters
#అయితే_ముగింపు
    పూర్ణాంకం		vcol_off_tp;	// offset for virtual text
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		draw_color_నిలువు;	// highlight colorcolumn
    పూర్ణాంకం		*color_నిలువుల_సంఖ్య;	// pointer to according columns array
#అయితే_ముగింపు
    పూర్ణాంకం		eol_hl_off;	// 1 if highlighted char after EOL

    గుర్తులేని	off;		// offset in ScreenLines/ScreenAttrs

    పూర్ణాంకం		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		cul_attr;	// set when 'cursorline' active
#అయితే_ముగింపు
#నిర్వచితమైతే పంక్తి_ATTR
    పూర్ణాంకం		పంక్తి_attr;	// for the whole line, includes 'cursorline'
#అయితే_ముగింపు

    పూర్ణాంకం		screen_పంక్తి_flags;  // flags for screen_line()

    పూర్ణాంకం		fromcol;	// start of inverting
    పూర్ణాంకం		tocol;		// end of inverting

#నిర్వచితమైతే FEAT_LINEBREAK
    దీర్ఘ	vcol_sbr;	    // virtual column after showbreak
    పూర్ణాంకం		need_showbreak;	    // overlong line, skipping first x chars
    పూర్ణాంకం		dont_use_showbreak; // do not use 'showbreak'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    పూర్ణాంకం		పాఠ్యం_prop_above_లెక్క;
#అయితే_ముగింపు

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    పూర్ణాంకం		cul_screenline;

    పూర్ణాంకం		char_attr;	// attributes for the next character

    పూర్ణాంకం		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    పూర్ణాంకం		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    పూర్ణాంకం		n_attr_skip;    // chars to skip before using extra_attr
    పూర్ణాంకం		c_extra;	// extra chars, all the same
    పూర్ణాంకం		c_final;	// final char, mandatory if set
    పూర్ణాంకం		extra_for_textprop; // n_extra set for textprop
    పూర్ణాంకం		ప్రారంభం_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    పూర్ణాంకం		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    పూర్ణాంకం		saved_extra_attr;
    పూర్ణాంకం		saved_n_attr_skip;
    పూర్ణాంకం		saved_extra_for_textprop;
    పూర్ణాంకం		saved_c_extra;
    పూర్ణాంకం		saved_c_final;
    పూర్ణాంకం		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#నిర్వచితమైతే FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#అయితే_ముగింపు
    పూర్ణాంకం		filler_పంక్తులు;	// nr of filler lines to be drawn
    పూర్ణాంకం		filler_todo;	// nr of filler lines still to do + 1
#నిర్వచితమైతే FEAT_SIGNS
    sign_attrs_T sattr;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    పూర్ణాంకం		need_lbr;
#అయితే_ముగింపు
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#నిర్వచించు WL_ప్రారంభం	0		// nothing done yet, must be zero
#నిర్వచించు WL_CMDLINE	(WL_ప్రారంభం + 1)	// cmdline window column
#నిర్వచితమైతే FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#లేకపోతే
# define WL_FOLD	WL_CMDLINE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#లేకపోతే
# define WL_SIGN	WL_FOLD		// column for signs
#అయితే_ముగింపు
#నిర్వచించు WL_NR		(WL_SIGN + 1)	// line number
#నిర్వచితమైతే FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#లేకపోతే
# define WL_BRI		WL_NR
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#లేకపోతే
# define WL_SBR		WL_BRI
#అయితే_ముగింపు
#నిర్వచించు WL_పంక్తి		(WL_SBR + 1)	// text in the line

#ఒకవేళ defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    స్థిర పూర్ణాంకం
use_cursor_పంక్తి_highlight(win_T *wp, linenr_T lnum)
{
    తిరిగి wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#అయితే_ముగింపు


#నిర్వచితమైతే FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    స్థిర శూన్య
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    పూర్ణాంకం fdc = compute_foldcolumn(wp, 0);

    ఒకవేళ (fdc <= 0)
	తిరిగి;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    ఒకవేళ (wlv->p_extra_free == ఖాళీ)
	తిరిగి;

    wlv->n_extra = (పూర్ణాంకం)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    ఒకవేళ (use_cursor_పంక్తి_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    లేకపోతే
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_FC));
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SIGNS
/*
 * Get information needed to display the sign in line "wlv->lnum" in window
 * "wp".
 * If "nrcol" is TRUE, the sign is going to be displayed in the number column.
 * Otherwise the sign is going to be displayed in the sign column.
 */
    స్థిర శూన్య
get_sign_display_info(
	పూర్ణాంకం		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    పూర్ణాంకం	పాఠ్యం_sign;
# ifdef FEAT_SIGN_ICONS
    పూర్ణాంకం	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    ఒకవేళ (nrcol)
	wlv->n_extra = సంఖ్య_వెడల్పు(wp) + 1;
    లేకపోతే
    {
	ఒకవేళ (use_cursor_పంక్తి_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	లేకపోతే
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    ఒకవేళ (wlv->అడ్డువరుస == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_పంక్తులు && wlv->filler_todo <= 0
# endif
       )
    {
	పాఠ్యం_sign = (wlv->sattr.sat_పాఠ్యం != ఖాళీ) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ఖాళీ) ? wlv->sattr.sat_typenr : 0;
	ఒకవేళ (gui.లోపల_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    ఒకవేళ (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((అక్షరం *)wlv->extra, పరిమాణం(wlv->extra),
						"%-*c ", సంఖ్య_వెడల్పు(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    లేకపోతే
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    ఒకవేళ (netbeans_చురుకైన() && (buf_signcount(wp->w_బఫరు, wlv->lnum)
									  > 1))
	    {
		ఒకవేళ (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((అక్షరం *)wlv->extra, పరిమాణం(wlv->extra),
						"%-*c ", సంఖ్య_వెడల్పు(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		లేకపోతే
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	లేకపోతే
# endif
	    ఒకవేళ (పాఠ్యం_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_పాఠ్యం;
		ఒకవేళ (wlv->p_extra != ఖాళీ)
		{
		    ఒకవేళ (nrcol)
		    {
			పూర్ణాంకం వెడల్పు = సంఖ్య_వెడల్పు(wp) - 2;

			vim_memset(wlv->extra, ' ', వెడల్పు);
			wlv->n_extra = వెడల్పు;
			wlv->n_extra += vim_snprintf((అక్షరం *)wlv->extra + వెడల్పు,
				  పరిమాణం(wlv->extra) - వెడల్పు, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    లేకపోతే
			wlv->n_extra = (పూర్ణాంకం)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		ఒకవేళ (use_cursor_పంక్తి_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		లేకపోతే
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#అయితే_ముగింపు

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    స్థిర శూన్య
handle_lnum_నిలువు(
	win_T		*wp,
	winlinevars_T	*wlv,
	పూర్ణాంకం		sign_present వాడని,
	పూర్ణాంకం		అంకె_attr వాడని)
{
    పూర్ణాంకం has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ఖాళీ;
    పూర్ణాంకం lnum_అడ్డువరుస = wlv->startrow + wlv->filler_పంక్తులు
#నిర్వచితమైతే FEAT_PROP_POPUP
		      + wlv->పాఠ్యం_prop_above_లెక్క
#అయితే_ముగింపు
		      ;

    ఒకవేళ ((wp->w_p_nu || wp->w_p_rnu)
	     && (wlv->అడ్డువరుస <= lnum_అడ్డువరుస || !has_cpo_n)
	     // there is no line number in a wrapped line when "n" is in
	     // 'cpoptions', but 'breakindent' assumes it anyway.
	     && !((has_cpo_n
#నిర్వచితమైతే FEAT_LINEBREAK
		     && !wp->w_p_bri
#అయితే_ముగింపు
		  ) && wp->w_skipcol > 0 && wlv->lnum == wp->w_topline))
    {
#నిర్వచితమైతే FEAT_SIGNS
	// If 'signcolumn' is set to 'number' and a sign is present
	// in 'lnum', then display the sign instead of the line
	// number.
	ఒకవేళ ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_పాఠ్యం != ఖాళీ)
	    get_sign_display_info(TRUE, wp, wlv);
	లేకపోతే
#అయితే_ముగింపు
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  ఒకవేళ (wlv->అడ్డువరుస == lnum_అడ్డువరుస
		    && (wp->w_skipcol == 0 || wlv->అడ్డువరుస > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      దీర్ఘ అంకె;
	      అక్షరం *fmt = "%*ld ";

	      ఒకవేళ (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  అంకె = (దీర్ఘ)wlv->lnum;
	      లేకపోతే
	      {
		  // 'relativenumber', don't use negative numbers
		  అంకె = labs((దీర్ఘ)get_cursor_rel_lnum(wp, wlv->lnum));
		  ఒకవేళ (అంకె == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      అంకె = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((అక్షరం *)wlv->extra, పరిమాణం(wlv->extra), fmt, సంఖ్య_వెడల్పు(wp), అంకె);
	      ఒకవేళ (wp->w_skipcol > 0 && wlv->startrow == 0)
		  కొరకు (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#నిర్వచితమైతే FEAT_RIGHTLEFT
	      ఒకవేళ (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  పూర్ణాంకం	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  కొరకు (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
		  {
		      t = *p1;
		      *p1 = *p2;
		      *p2 = t;
		  }
	      }
#అయితే_ముగింపు
	      wlv->p_extra = wlv->extra;
	      wlv->c_extra = NUL;
	      wlv->c_final = NUL;
	  }
	  లేకపోతే
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = సంఖ్య_వెడల్పు(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#నిర్వచితమైతే FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  ఒకవేళ (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->అడ్డువరుస == lnum_అడ్డువరుస
		      || (wlv->అడ్డువరుస > lnum_అడ్డువరుస
			 && (wp->w_p_culopt_flags & CULOPT_పంక్తి))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#అయితే_ముగింపు
	  ఒకవేళ (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  ఒకవేళ (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#నిర్వచితమైతే FEAT_SIGNS
	ఒకవేళ (అంకె_attr)
	    wlv->char_attr = అంకె_attr;
#అయితే_ముగింపు
    }
}

#నిర్వచితమైతే FEAT_LINEBREAK
    స్థిర శూన్య
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    ఒకవేళ (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_విలువ(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    లేకపోతే ఒకవేళ (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    ఒకవేళ (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	ఒకవేళ (wp->w_p_bri && (wlv->అడ్డువరుస > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_పంక్తులు
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    ఒకవేళ (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ఖాళీ;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_బఫరు, wlv->lnum, FALSE));
	    ఒకవేళ (wlv->అడ్డువరుస == wlv->startrow)
	    {
		wlv->n_extra -= win_నిలువు_off2(wp);
		ఒకవేళ (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    ఒకవేళ (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    ఒకవేళ (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	ఒకవేళ (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    స్థిర శూన్య
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    ఒకవేళ (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	ఒకవేళ (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	లేకపోతే
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	ఒకవేళ (wp->w_p_rl)
	    wlv->n_extra = wlv->నిలువు + 1;
	లేకపోతే
#  endif
	    wlv->n_extra = wp->w_వెడల్పు - wlv->నిలువు;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_విలువ(wp);
    ఒకవేళ (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (పూర్ణాంకం)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	ఒకవేళ (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	ఒకవేళ (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	ఒకవేళ (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    ఒకవేళ (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    స్థిర పూర్ణాంకం
textprop_కొలత_after_trunc(
	win_T	*wp,
	పూర్ణాంకం	flags,	    // TP_FLAG_ALIGN_*
	పూర్ణాంకం	added,
	పూర్ణాంకం	padding,
	char_u	*పాఠ్యం,
	పూర్ణాంకం	*n_వాడిన_ptr)
{
    పూర్ణాంకం	space = (flags & (TP_FLAG_ALIGN_క్రింద | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_వెడల్పు - win_నిలువు_off(wp) : added;
    పూర్ణాంకం strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    ఒకవేళ (space < PROP_పాఠ్యం_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_వెడల్పు;
    ఒకవేళ (flags & (TP_FLAG_ALIGN_క్రింద | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    కొరకు (p = పాఠ్యం; *p != NUL; p += (*mb_ptr2len)(p))
    {
	పూర్ణాంకం clen = ptr2cells(p);

	ఒకవేళ (strsize + clen > space)
	    ఆపు;
	strsize += clen;
    }
    *n_వాడిన_ptr = (పూర్ణాంకం)(p - పాఠ్యం);

    తిరిగి strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    పూర్ణాంకం
పాఠ్యం_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	పూర్ణాంకం	    vcol,	    // current text column
	పూర్ణాంకం	    scr_నిలువు,	    // current screen column
	పూర్ణాంకం	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	పూర్ణాంకం	    *n_attr,	    // attribute cells, NULL if not used
	పూర్ణాంకం	    *n_attr_skip,   // cells to skip attr, NULL if not used
	పూర్ణాంకం	    do_skip)	    // skip_cells is not zero
{
    పూర్ణాంకం	    కుడి_2 = (tp->tp_flags & TP_FLAG_ALIGN_కుడి);
    పూర్ణాంకం	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    పూర్ణాంకం	    క్రింద = (tp->tp_flags & TP_FLAG_ALIGN_క్రింద);
    పూర్ణాంకం	    wrap = tp->tp_నిలువు < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    పూర్ణాంకం	    padding = tp->tp_నిలువు == MAXCOL ? tp->tp_padleft : 0;
    పూర్ణాంకం	    నిలువు_with_padding = scr_నిలువు + (క్రింద ? 0 : padding);
    పూర్ణాంకం	    స్థలం = wp->w_వెడల్పు - నిలువు_with_padding;
    పూర్ణాంకం	    before = స్థలం;	// spaces before the text
    పూర్ణాంకం	    after = 0;		// spaces after the text
    పూర్ణాంకం	    n_వాడిన = *n_extra;
    char_u  *l = ఖాళీ;
    పూర్ణాంకం	    strsize = vim_strsize(*p_extra);
    పూర్ణాంకం	    cells = wrap ? strsize : textprop_కొలత_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_వాడిన);

    ఒకవేళ (wrap || కుడి_2 || above || క్రింద || padding > 0 || n_వాడిన < *n_extra)
    {
	పూర్ణాంకం	    నిలువు_off = win_నిలువు_off(wp) - win_నిలువు_off2(wp);

	ఒకవేళ (above)
	{
	    before = 0;
	    after = wp->w_వెడల్పు - cells - win_నిలువు_off(wp) - padding;
	    ఒకవేళ (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	లేకపోతే
	{
	    // Right-align: fill with before
	    ఒకవేళ (కుడి_2)
		before -= cells;

	    // Below-align: empty line add one character
	    ఒకవేళ (క్రింద && vcol == 0 && నిలువు_with_padding == నిలువు_off
					    && wp->w_వెడల్పు - నిలువు_off == before)
		నిలువు_with_padding += 1;

	    ఒకవేళ (before < 0
		    || !(కుడి_2 || క్రింద)
		    || (క్రింద ? (నిలువు_with_padding <= నిలువు_off || !wp->w_p_wrap)
			      : (n_వాడిన < *n_extra)))
	    {
		ఒకవేళ (కుడి_2 && (wrap
			      || (స్థలం < PROP_పాఠ్యం_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_వెడల్పు - నిలువు_off - strsize + స్థలం;
		    ఒకవేళ (before < 0)
			before = 0;
		    లేకపోతే
			n_వాడిన = *n_extra;
		}
		లేకపోతే ఒకవేళ (క్రింద && before > vcol && do_skip)
		    before -= vcol;
		లేకపోతే
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	ఒకవేళ (n_వాడిన < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_వాడిన;

	// add 1 for NUL, 2 for when '…' is used
	ఒకవేళ (n_attr != ఖాళీ)
	    l = alloc(n_వాడిన + before + after + (padding > 0 ? padding : 0) + 3);
	ఒకవేళ (n_attr == ఖాళీ || l != ఖాళీ)
	{
	    పూర్ణాంకం off = 0;

	    ఒకవేళ (n_attr != ఖాళీ)
	    {
		vim_memset(l, ' ', before);
		off += before;
		ఒకవేళ (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_వాడిన);
		off += n_వాడిన;
	    }
	    లేకపోతే
	    {
		off = before + after + padding + n_వాడిన;
		cells += before + after + padding;
	    }
	    ఒకవేళ (n_attr != ఖాళీ)
	    {
		ఒకవేళ (n_వాడిన < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    ఒకవేళ (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			ఒకవేళ (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_అమరిక(&vc, (char_u *)"utf-8", p_enc);
			    ఒకవేళ (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ఖాళీ);
				ఒకవేళ (cp == ఖాళీ)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_అమరిక(&vc, ఖాళీ, ఖాళీ);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_తల_off)(l, lp);
			STRCPY(lp, cp);
			n_వాడిన = lp - l + 3 - before - padding;
			ఒకవేళ (cp != buf)
			    vim_free(cp);
		    }
		    లేకపోతే
			// change last character to '>'
			*lp = '>';
		}
		లేకపోతే ఒకవేళ (after > 0)
		{
		    vim_memset(l + off, ' ', after);
		    l[off + after] = NUL;
		}

		*p_extra = l;
		*n_extra = n_వాడిన + before + after + padding;
		*n_attr = mb_charlen(*p_extra);
		// n_attr_skip will not be decremented before draw_state is
		// WL_LINE
		*n_attr_skip = before + (padding > 0 ? padding : 0);
		*n_attr -= *n_attr_skip;
		ఒకవేళ (above)
		    *n_attr -= after;
	    }
	}
    }

    ఒకవేళ (n_attr == ఖాళీ)
	తిరిగి cells;
    తిరిగి (క్రింద && నిలువు_with_padding > win_నిలువు_off(wp) && !wp->w_p_wrap);
}

# ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    పూర్ణాంకం
పాఠ్యం_prop_no_showbreak(textprop_T *tp)
{
    పూర్ణాంకం	    కుడి_2 = (tp->tp_flags & TP_FLAG_ALIGN_కుడి);
    పూర్ణాంకం	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    పూర్ణాంకం	    క్రింద = (tp->tp_flags & TP_FLAG_ALIGN_క్రింద);
    పూర్ణాంకం	    wrap = tp->tp_నిలువు < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    తిరిగి (కుడి_2 || above || క్రింద || !wrap);
}
# endif
#అయితే_ముగింపు

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    స్థిర శూన్య
wlv_screen_పంక్తి(win_T *wp, winlinevars_T *wlv, పూర్ణాంకం clear_చివర)
{
    ఒకవేళ (wlv->అడ్డువరుస == 0 && wp->w_skipcol > 0
#ఒకవేళ defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_విలువ(wp) == NUL
#అయితే_ముగింపు
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	పూర్ణాంకం off = (పూర్ణాంకం)(current_Screen_పంక్తి - Screen_పంక్తులు);
	పూర్ణాంకం max_off = off + screen_Columns;
	పూర్ణాంకం skip = 0;

	ఒకవేళ (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    అయితే (skip < wp->w_వెడల్పు && VIM_ISDIGIT(Screen_పంక్తులు[off]))
	    {
		++off;
		++skip;
	    }

	కొరకు (పూర్ణాంకం i = 0; i < 3 && i + skip < wp->w_వెడల్పు; ++i)
	{
	    ఒకవేళ ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_పంక్తులు[off + 1] = ' ';
	    Screen_పంక్తులు[off] = '<';
	    ఒకవేళ (enc_utf8)
		Screen_పంక్తులు_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_పంక్తి(wp, wlv->screen_అడ్డువరుస, wp->w_wincol, wlv->నిలువు,
		    clear_చివర ? wp->w_వెడల్పు : -wp->w_వెడల్పు,
		    wlv->vcol - 1, wlv->screen_పంక్తి_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    స్థిర శూన్య
draw_screen_పంక్తి(win_T *wp, winlinevars_T *wlv)
{
#నిర్వచితమైతే FEAT_SYN_HL
    దీర్ఘ	v;
    పూర్ణాంకం		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    ఒకవేళ (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    లేకపోతే
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_వెడల్పు - wlv->నిలువు - 1 :
# endif
	wlv->నిలువు;
    // check if line ends before left margin
    ఒకవేళ (wlv->vcol < v + wcol - win_నిలువు_off(wp))
	wlv->vcol = v + wcol - win_నిలువు_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->నిలువు -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# లేకపోతే
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    ఒకవేళ (wlv->draw_color_నిలువు)
	wlv->draw_color_నిలువు = advance_color_నిలువు(VCOL_HLC, &wlv->color_నిలువుల_సంఖ్య);

    ఒకవేళ (((wp->w_p_cuc
		    && (పూర్ణాంకం)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (పూర్ణాంకం)wp->w_virtcol <
			 (దీర్ఘ)wp->w_వెడల్పు * (wlv->అడ్డువరుస - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_నిలువు
# ifdef పంక్తి_ATTR
		|| wlv->పంక్తి_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	పూర్ణాంకం	rightmost_vcol = 0;
	పూర్ణాంకం	i;

	ఒకవేళ (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	ఒకవేళ (wlv->draw_color_నిలువు)
	    // determine rightmost colorcolumn to possibly draw
	    కొరకు (i = 0; wlv->color_నిలువుల_సంఖ్య[i] >= 0; ++i)
		ఒకవేళ (rightmost_vcol < wlv->color_నిలువుల_సంఖ్య[i])
		    rightmost_vcol = wlv->color_నిలువుల_సంఖ్య[i];

	అయితే (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->నిలువు >= 0) :
# endif
		(wlv->నిలువు < wp->w_వెడల్పు))
	{
	    Screen_పంక్తులు[wlv->off] = ' ';
	    ఒకవేళ (enc_utf8)
		Screen_పంక్తులు_UC[wlv->off] = 0;

	    ఒకవేళ (wlv->draw_color_నిలువు)
		wlv->draw_color_నిలువు = advance_color_నిలువు(
						   VCOL_HLC, &wlv->color_నిలువుల_సంఖ్య);

	    పూర్ణాంకం attr = wlv->win_attr;
# ifdef పంక్తి_ATTR
	    ఒకవేళ (wlv->పంక్తి_attr != 0)
		attr = hl_combine_attr(attr, wlv->పంక్తి_attr);
# endif
	    ఒకవేళ (wp->w_p_cuc && VCOL_HLC == (దీర్ఘ)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    లేకపోతే ఒకవేళ (wlv->draw_color_నిలువు && VCOL_HLC == *wlv->color_నిలువుల_సంఖ్య)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    Screen_నిలువుల_సంఖ్య[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->నిలువు;
	    }
	    లేకపోతే
# endif
	    {
		++wlv->off;
		++wlv->నిలువు;
	    }
	    ++wlv->vcol;

	    ఒకవేళ (VCOL_HLC > rightmost_vcol
# ifdef పంక్తి_ATTR
		    && wlv->పంక్తి_attr == 0
# endif
		    && wlv->win_attr == 0)
		ఆపు;
	}
    }
#అయితే_ముగింపు

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_పంక్తి_flags |= SLF_INC_VCOL;
    wlv_screen_పంక్తి(wp, wlv, TRUE);
    wlv->screen_పంక్తి_flags &= ~SLF_INC_VCOL;
    ++wlv->అడ్డువరుస;
    ++wlv->screen_అడ్డువరుస;
}
#నిర్వచనం_రద్దు VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    స్థిర శూన్య
win_పంక్తి_ప్రారంభం(win_T *wp వాడని, winlinevars_T *wlv, పూర్ణాంకం save_extra)
{
    wlv->నిలువు = 0;
    wlv->off = (గుర్తులేని)(current_Screen_పంక్తి - Screen_పంక్తులు);
#నిర్వచితమైతే FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->నిలువు = wp->w_వెడల్పు - 1;
	wlv->off += wlv->నిలువు;
	wlv->screen_పంక్తి_flags |= SLF_RIGHTLEFT;
    }
#అయితే_ముగింపు
    ఒకవేళ (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_ప్రారంభం;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ఖాళీ;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#నిర్వచితమైతే FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	లేకపోతే
#అయితే_ముగింపు
	    wlv->saved_char_attr = 0;

	// these are not used until restored in win_line_continue()
	wlv->n_extra = 0;
	wlv->n_attr_skip = 0;
    }
}

/*
 * Called when wlv->draw_state is set to WL_LINE.
 */
    స్థిర శూన్య
win_పంక్తి_continue(winlinevars_T *wlv)
{
    ఒకవేళ (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ఖాళీ;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    లేకపోతే
	wlv->char_attr = wlv->win_attr;
}

#నిర్వచితమైతే FEAT_SYN_HL
    స్థిర శూన్య
apply_cursorline_highlight(
	winlinevars_T *wlv,
	పూర్ణాంకం sign_present వాడని)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    ఒకవేళ (sign_present && wlv->sattr.sat_linehl > 0)
    {
	ఒకవేళ (wlv->sattr.sat_ప్రాధాన్యత >= 100)
	    wlv->పంక్తి_attr = hl_combine_attr(wlv->cul_attr, wlv->పంక్తి_attr);
	లేకపోతే
	    wlv->పంక్తి_attr = hl_combine_attr(wlv->పంక్తి_attr, wlv->cul_attr);
    }
    లేకపోతే
# endif
# ఒకవేళ defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->పంక్తి_attr = hl_combine_attr(wlv->cul_attr, wlv->పంక్తి_attr);
# లేకపోతే
	wlv->పంక్తి_attr = wlv->cul_attr;
# endif
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
win_పంక్తి(
    win_T	*wp,
    linenr_T	lnum,
    పూర్ణాంకం		startrow,
    పూర్ణాంకం		endrow,
    పూర్ణాంకం		సంఖ్య_only,
    spellvars_T	*spv వాడని)
{
    winlinevars_T	wlv;		// variables passed between functions

    పూర్ణాంకం		c = 0;			// init for GCC
    char_u	*పంక్తి;			// current line
    char_u	*ptr;			// current position in "line"
    పూర్ణాంకం		లోపల_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#నిర్వచితమైతే FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ఖాళీ;   // another p_extra to be freed
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    పూర్ణాంకం		లోపల_linebreak = FALSE;	// n_extra set for showing linebreak
#అయితే_ముగింపు
    పూర్ణాంకం		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    పూర్ణాంకం		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    పూర్ణాంకం		n_attr = 0;	    // chars with special attr
    పూర్ణాంకం		saved_attr2 = 0;    // char_attr saved for n_attr
    పూర్ణాంకం		n_attr3 = 0;	    // chars with overruling special attr
    పూర్ణాంకం		saved_attr3 = 0;    // char_attr saved for n_attr3

    పూర్ణాంకం		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    పూర్ణాంకం		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    పూర్ణాంకం		lnum_లోపల_visual_area = FALSE;
    pos_T	pos;
    దీర్ఘ	v;

    పూర్ణాంకం		attr_pri = FALSE;	// char_attr has priority
    పూర్ణాంకం		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    పూర్ణాంకం		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    పూర్ణాంకం		area_attr = 0;		// attributes desired by highlighting
    పూర్ణాంకం		శోధన_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    పూర్ణాంకం		syntax_attr = 0;	// attributes desired by syntax
    పూర్ణాంకం		prev_syntax_నిలువు = -1;	// column of prev_syntax_attr
    పూర్ణాంకం		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    పూర్ణాంకం		has_syntax = FALSE;	// this buffer has syntax highl.
    పూర్ణాంకం		save_did_emsg;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
# define WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN 32
    పూర్ణాంకం		did_పంక్తి = FALSE;	// set to TRUE when line text done
    పూర్ణాంకం		పాఠ్యం_prop_లెక్క;
    పూర్ణాంకం		చివరి_textprop_పాఠ్యం_idx = -1;
    పూర్ణాంకం		పాఠ్యం_prop_తదుపరి = 0;	// next text property to use
    textprop_T	పాఠ్యం_props_buf[WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN];
    పూర్ణాంకం		పాఠ్యం_prop_idxs_buf[WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN];
    char_u	పాఠ్యం_prop_suffix_flags_buf[WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN + 1];
    textprop_T	*పాఠ్యం_props = ఖాళీ;
    పూర్ణాంకం		*పాఠ్యం_prop_idxs = ఖాళీ;
    char_u	*పాఠ్యం_prop_suffix_flags = ఖాళీ;
    పూర్ణాంకం		పాఠ్యం_props_చురుకైన = 0;
    పూర్ణాంకం		పాఠ్యం_props_need_క్రమీకరించు = FALSE;
    proptype_T  *పాఠ్యం_prop_type = ఖాళీ;
    పూర్ణాంకం		పాఠ్యం_prop_attr = 0;
    పూర్ణాంకం		పాఠ్యం_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    పూర్ణాంకం		పాఠ్యం_prop_id = 0;	// active property ID
    పూర్ణాంకం		పాఠ్యం_prop_flags = 0;
    పూర్ణాంకం		పాఠ్యం_prop_above = FALSE;  // first doing virtual text above
    పూర్ణాంకం		పాఠ్యం_prop_follows = FALSE;  // another text prop to display
    పూర్ణాంకం		saved_శోధన_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    పూర్ణాంకం		saved_area_attr = 0;	// idem for area_attr
    పూర్ణాంకం		reset_extra_attr = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
    పూర్ణాంకం		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    పూర్ణాంకం		nextlinecol = 0;	// column where nextline[] starts
    పూర్ణాంకం		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    పూర్ణాంకం		spell_attr = 0;		// attributes desired by spelling
    పూర్ణాంకం		మాట_చివర = 0;		// last byte with same spell_attr
    పూర్ణాంకం		cur_checked_నిలువు = 0;	// checked column for current line
#అయితే_ముగింపు
    పూర్ణాంకం		extra_check = FALSE;	// has extra highlighting
    పూర్ణాంకం		multi_attr = 0;		// attributes desired by multibyte
    పూర్ణాంకం		mb_l = 1;		// multi-byte byte length
    పూర్ణాంకం		mb_c = 0;		// decoded multi-byte character
    పూర్ణాంకం		mb_utf8 = FALSE;	// screen char is UTF-8 char
    పూర్ణాంకం		u8cc[MAX_MCO];		// composing UTF-8 chars
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		change_ప్రారంభం = MAXCOL;	// first col of changed area
    పూర్ణాంకం		change_చివర = -1;	// last col of changed area
    diffline_T	పంక్తి_changes;
    పూర్ణాంకం		change_సూచీ = -1;
#అయితే_ముగింపు
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    పూర్ణాంకం		లోపల_multispace = FALSE;	// in multiple consecutive spaces
    పూర్ణాంకం		multispace_pos = 0;	// position in lcs-multispace string
#నిర్వచితమైతే పంక్తి_ATTR
    పూర్ణాంకం		పంక్తి_attr_save = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		sign_present = FALSE;
    పూర్ణాంకం		అంకె_attr = 0;		// attribute for the number column
#నిర్వచితమైతే FEAT_ARABIC
    పూర్ణాంకం		prev_c = 0;		// previous Arabic character
    పూర్ణాంకం		prev_c1 = 0;		// first composing char for prev_c
#అయితే_ముగింపు
#ఒకవేళ defined(పంక్తి_ATTR)
    పూర్ణాంకం		did_పంక్తి_attr = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    పూర్ణాంకం		get_పదం_attr = FALSE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    పూర్ణాంకం		ఎడమ_curline_నిలువు = 0;
    పూర్ణాంకం		కుడి_curline_నిలువు = 0;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    పూర్ణాంకం		feedback_నిలువు = 0;
    పూర్ణాంకం		feedback_old_attr = -1;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CONCEAL) || defined(FEAT_శోధన_EXTRA)
    పూర్ణాంకం		సరిపోలిక_conc	= 0;	// cchar for match functions
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CONCEAL) || defined(FEAT_శోధన_EXTRA) || defined(FEAT_LINEBREAK)
    పూర్ణాంకం		on_చివరి_నిలువు     = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		syntax_flags	= 0;
    పూర్ణాంకం		syntax_seqnr	= 0;
    పూర్ణాంకం		prev_syntax_id	= 0;
    పూర్ణాంకం		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    పూర్ణాంకం		is_concealing	= FALSE;
    పూర్ణాంకం		did_wcol	= FALSE;
    పూర్ణాంకం		old_boguscols   = 0;
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_co - wlv.vcol_off_tp)
# define FIX_FOR_BOGUSCOLS \
    { \
	wlv.n_extra += wlv.vcol_off_co; \
	wlv.vcol -= wlv.vcol_off_co; \
	wlv.vcol_off_co = 0; \
	wlv.నిలువు -= wlv.boguscols; \
	old_boguscols = wlv.boguscols; \
	wlv.boguscols = 0; \
    }
#లేకపోతే
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#అయితే_ముగింపు

    ఒకవేళ (startrow > endrow)		// past the end already!
	తిరిగి startrow;

    CLEAR_క్షేత్రం(wlv);

    wlv.lnum = lnum;
    wlv.startrow = startrow;
    wlv.అడ్డువరుస = startrow;
    wlv.screen_అడ్డువరుస = wlv.అడ్డువరుస + W_WINROW(wp);
    wlv.fromcol = -10;
    wlv.tocol = MAXCOL;
#నిర్వచితమైతే FEAT_LINEBREAK
    wlv.vcol_sbr = -1;
#అయితే_ముగింపు

    ఒకవేళ (సంఖ్య_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#నిర్వచితమైతే FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (syntax_present(wp) && !wp->w_s->b_syn_error
# ifdef SYN_సమయం_హద్దు
		&& !wp->w_s->b_syn_slow
# endif
	   )
	{
	    // Prepare for syntax highlighting in this line.  When there is an
	    // error, stop syntax highlighting.
	    save_did_emsg = did_emsg;
	    did_emsg = FALSE;
	    syntax_ప్రారంభం(wp, lnum);
	    ఒకవేళ (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    లేకపోతే
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_సమయం_హద్దు
		ఒకవేళ (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_నిలువుల_సంఖ్య = wp->w_p_cc_నిలువుల_సంఖ్య;
	ఒకవేళ (wlv.color_నిలువుల_సంఖ్య != ఖాళీ)
	    wlv.draw_color_నిలువు = advance_color_నిలువు(VCOL_HLC, &wlv.color_నిలువుల_సంఖ్య);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMINAL
	ఒకవేళ (పదం_show_బఫరు(wp->w_బఫరు))
	{
	    extra_check = TRUE;
	    get_పదం_attr = TRUE;
	    wlv.win_attr = పదం_get_attr(wp, lnum, -1);
	}
#అయితే_ముగింపు

	// handle Visual active in this window
	ఒకవేళ (VIsual_చురుకైన && wp->w_బఫరు == curwin->w_బఫరు)
	{
	    pos_T	*శిఖరం, *bot;

	    ఒకవేళ (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		శిఖరం = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    లేకపోతే
	    {
		// Visual is before curwin->w_cursor
		శిఖరం = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_లోపల_visual_area = (lnum >= శిఖరం->lnum && lnum <= bot->lnum);
	    ఒకవేళ (VIsual_బహుళకం == Ctrl_V)
	    {
		// block mode
		ఒకవేళ (lnum_లోపల_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    లేకపోతే
	    {
		// non-block mode
		ఒకవేళ (lnum > శిఖరం->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		లేకపోతే ఒకవేళ (lnum == శిఖరం->lnum)
		{
		    ఒకవేళ (VIsual_బహుళకం == 'V')	// linewise
			wlv.fromcol = 0;
		    లేకపోతే
		    {
			getvvcol(wp, శిఖరం, &wlv.fromcol, ఖాళీ, ఖాళీ, 0);
			ఒకవేళ (gchar_pos(శిఖరం) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		ఒకవేళ (VIsual_బహుళకం != 'V' && lnum == bot->lnum)
		{
		    ఒకవేళ (*p_sel == 'e' && bot->నిలువు == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    లేకపోతే ఒకవేళ (bot->నిలువు == MAXCOL)
			wlv.tocol = MAXCOL;
		    లేకపోతే
		    {
			pos = *bot;
			ఒకవేళ (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ఖాళీ, ఖాళీ, 0);
			లేకపోతే
			{
			    getvvcol(wp, &pos, ఖాళీ, ఖాళీ, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    ఒకవేళ (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#ఒకవేళ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		ఒకవేళ ((clip_నక్షత్రం.అందుబాటు && !clip_నక్షత్రం.owned
						    && clip_isautosel_నక్షత్రం())
			|| (clip_plus.అందుబాటు && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#అయితే_ముగింపు
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	లేకపోతే ఒకవేళ (highlight_సరిపోలిక
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + శోధన_సరిపోలిక_పంక్తులు)
	{
	    ఒకవేళ (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ఖాళీ, ఖాళీ, 0);
	    లేకపోతే
		wlv.fromcol = 0;
	    ఒకవేళ (lnum == curwin->w_cursor.lnum + శోధన_సరిపోలిక_పంక్తులు)
	    {
		pos.lnum = lnum;
		pos.నిలువు = శోధన_సరిపోలిక_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ఖాళీ, ఖాళీ, 0);
	    }
	    లేకపోతే
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    ఒకవేళ (wlv.fromcol == wlv.tocol && శోధన_సరిపోలిక_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#నిర్వచితమైతే FEAT_DIFF

    పూర్ణాంకం linestatus = 0;
    wlv.filler_పంక్తులు = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_క్షేత్రం(పంక్తి_changes);

    ఒకవేళ (linestatus < 0)
    {
	ఒకవేళ (linestatus == -1)
	{
	    ఒకవేళ (diff_కనుగొను_change(wp, lnum, &పంక్తి_changes))
	    {
		wlv.diff_hlf = HLF_కూడు;	// added line
	    }
	    లేకపోతే ఒకవేళ (పంక్తి_changes.అంకె_changes > 0)
	    {
		పూర్ణాంకం added = diff_change_parse(
			&పంక్తి_changes, &పంక్తి_changes.changes[0],
			&change_ప్రారంభం, &change_చివర);
		ఒకవేళ (change_ప్రారంభం == 0)
		{
		    ఒకవేళ (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    లేకపోతే
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		లేకపోతే
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_సూచీ = 0;
	    }
	    లేకపోతే
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_సూచీ = 0;
	    }
	}
	లేకపోతే
	    wlv.diff_hlf = HLF_కూడు;

	area_highlighting = TRUE;
    }

    ఒకవేళ (lnum == wp->w_topline)
	wlv.filler_పంక్తులు = wp->w_topfill;

    wlv.filler_todo = wlv.filler_పంక్తులు;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    ఒకవేళ (sign_present)
	అంకె_attr = wlv.sattr.sat_numhl;
#అయితే_ముగింపు

#నిర్వచితమైతే పంక్తి_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    ఒకవేళ (sign_present)
	wlv.పంక్తి_attr = wlv.sattr.sat_linehl;
# endif
# ఒకవేళ defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    ఒకవేళ (bt_quickfix(wp->w_బఫరు) && qf_current_నమోదు(wp) == lnum)
	wlv.పంక్తి_attr = HL_ATTR(HLF_QFL);
# endif
    ఒకవేళ (wlv.పంక్తి_attr != 0)
	area_highlighting = TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SPELL
    ఒకవేళ (spv->spv_has_spell && సంఖ్య_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	ఒకవేళ (lnum == spv->spv_checked_lnum)
	    cur_checked_నిలువు = spv->spv_checked_నిలువు;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	ఒకవేళ (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_నిలువు = 0;
	లేకపోతే ఒకవేళ (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_నిలువు = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	ఒకవేళ (lnum < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	{
	    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum + 1, FALSE);
	    spell_cat_పంక్తి(nextline + SPWORDLEN, పంక్తి, SPWORDLEN);
	}
	పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(పంక్తి);
	ఒకవేళ (*ptr == NUL)
	{
	    spv->spv_cap_నిలువు = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	లేకపోతే ఒకవేళ (spv->spv_cap_నిలువు == 0)
	    spv->spv_cap_నిలువు = ptr - పంక్తి;

	// Copy the end of the current line into nextline[].
	ఒకవేళ (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	లేకపోతే
	{
	    v = ml_get_buf_len(wp->w_బఫరు, lnum);
	    ఒకవేళ (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, పంక్తి, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    లేకపోతే
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, పంక్తి + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#అయితే_ముగింపు

    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    ptr = పంక్తి;

    ఒకవేళ (wp->w_p_list)
    {
	ఒకవేళ (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ఖాళీ
		|| wp->w_lcs_chars.leadmultispace != ఖాళీ
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	ఒకవేళ (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_బఫరు, lnum);
	    అయితే (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - పంక్తి);
	}
	// find end of leading whitespace
	ఒకవేళ (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ఖాళీ ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    అయితే (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    ఒకవేళ (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    లేకపోతే
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - పంక్తి) + 1;
	}
    }

    ఒకవేళ (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    పూర్ణాంకం skipcol_లోపల_పాఠ్యం_prop_above = 0;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (WIN_IS_POPUP(wp))
	wlv.screen_పంక్తి_flags |= SLF_POPUP;

    char_u *prop_ప్రారంభం;
    పాఠ్యం_prop_లెక్క = get_పాఠ్యం_props(wp->w_బఫరు, lnum, &prop_ప్రారంభం, FALSE);
    ఒకవేళ (పాఠ్యం_prop_లెక్క > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	ఒకవేళ (పాఠ్యం_prop_లెక్క <= WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN)
	    పాఠ్యం_props = పాఠ్యం_props_buf;
	లేకపోతే
	    పాఠ్యం_props = ALLOC_MULT(textprop_T, పాఠ్యం_prop_లెక్క);
	ఒకవేళ (పాఠ్యం_props != ఖాళీ)
	{
	    mch_memmove(పాఠ్యం_props, prop_ప్రారంభం,
				     పాఠ్యం_prop_లెక్క * పరిమాణం(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *లెక్క_ptr = prop_ప్రారంభం - PROP_లెక్క_కొలత;

	    కొరకు (పూర్ణాంకం i = 0; i < పాఠ్యం_prop_లెక్క; ++i)
	    {
		ఒకవేళ (పాఠ్యం_props[i].tp_id < 0
				&& పాఠ్యం_props[i].u.tp_పాఠ్యం_స్థానభ్రంశం > 0)
		{
		    పాఠ్యం_props[i].u.tp_పాఠ్యం =
			    లెక్క_ptr + పాఠ్యం_props[i].u.tp_పాఠ్యం_స్థానభ్రంశం;
		    పాఠ్యం_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		లేకపోతే
		    పాఠ్యం_props[i].u.tp_పాఠ్యం = ఖాళీ;
	    }
	}

	// Allocate an array for the indexes.
	ఒకవేళ (పాఠ్యం_prop_లెక్క <= WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN)
	    పాఠ్యం_prop_idxs = పాఠ్యం_prop_idxs_buf;
	లేకపోతే
	    పాఠ్యం_prop_idxs = ALLOC_MULT(పూర్ణాంకం, పాఠ్యం_prop_లెక్క);
	ఒకవేళ (పాఠ్యం_prop_idxs == ఖాళీ)
	{
	    ఒకవేళ (పాఠ్యం_props != పాఠ్యం_props_buf)
		VIM_CLEAR(పాఠ్యం_props);
	    లేకపోతే
		పాఠ్యం_props = ఖాళీ;
	}
	ఒకవేళ (పాఠ్యం_prop_లెక్క <= WIN_పంక్తి_పాఠ్యం_PROP_దొంతర_LEN)
	    పాఠ్యం_prop_suffix_flags = పాఠ్యం_prop_suffix_flags_buf;
	లేకపోతే
	    పాఠ్యం_prop_suffix_flags = ALLOC_MULT(char_u, పాఠ్యం_prop_లెక్క + 1);
	ఒకవేళ (పాఠ్యం_prop_suffix_flags == ఖాళీ)
	{
	    ఒకవేళ (పాఠ్యం_prop_idxs != పాఠ్యం_prop_idxs_buf)
		vim_free(పాఠ్యం_prop_idxs);
	    పాఠ్యం_prop_idxs = ఖాళీ;
	    ఒకవేళ (పాఠ్యం_props != పాఠ్యం_props_buf)
		VIM_CLEAR(పాఠ్యం_props);
	    లేకపోతే
		పాఠ్యం_props = ఖాళీ;
	}

	ఒకవేళ (పాఠ్యం_props != ఖాళీ)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    కొరకు (పూర్ణాంకం i = 0; i < పాఠ్యం_prop_లెక్క; ++i)
		ఒకవేళ (పాఠ్యం_props[i].tp_id < 0)
		    చివరి_textprop_పాఠ్యం_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    పూర్ణాంకం పాఠ్యం_వెడల్పు = wp->w_వెడల్పు - win_నిలువు_off(wp);
	    కొరకు (పూర్ణాంకం i = పాఠ్యం_prop_లెక్క - 1; i >= 0; --i)
		ఒకవేళ (పాఠ్యం_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    ఒకవేళ (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_లోపల_పాఠ్యం_prop_above
								 >= పాఠ్యం_వెడల్పు)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_లోపల_పాఠ్యం_prop_above += పాఠ్యం_వెడల్పు;
			కొరకు (పూర్ణాంకం j = i + 1; j < పాఠ్యం_prop_లెక్క; ++j)
			    పాఠ్యం_props[j - 1] = పాఠ్యం_props[j];
			++i;
			--పాఠ్యం_prop_లెక్క;
		    }
		    లేకపోతే
			++wlv.పాఠ్యం_prop_above_లెక్క;
		}

	    పాఠ్యం_prop_suffix_flags[పాఠ్యం_prop_లెక్క] = 0;
	    కొరకు (పూర్ణాంకం i = పాఠ్యం_prop_లెక్క - 1; i >= 0; --i)
	    {
		పూర్ణాంకం flags = పాఠ్యం_prop_suffix_flags[i + 1];

		ఒకవేళ (పాఠ్యం_props[i].tp_నిలువు == MAXCOL)
		{
		    flags |= 1;
		    ఒకవేళ (పాఠ్యం_props[i].tp_flags & TP_FLAG_ALIGN_క్రింద)
			flags |= 2;
		}
		పాఠ్యం_prop_suffix_flags[i] = flags;
	    }
	}
    }

    ఒకవేళ (సంఖ్య_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.అడ్డువరుస += wlv.పాఠ్యం_prop_above_లెక్క;
	ఒకవేళ (wlv.అడ్డువరుస >= endrow)
	{
	    ఒకవేళ (పాఠ్యం_props != పాఠ్యం_props_buf)
		vim_free(పాఠ్యం_props);
	    ఒకవేళ (పాఠ్యం_prop_idxs != పాఠ్యం_prop_idxs_buf)
		vim_free(పాఠ్యం_prop_idxs);
	    ఒకవేళ (పాఠ్యం_prop_suffix_flags != పాఠ్యం_prop_suffix_flags_buf)
		vim_free(పాఠ్యం_prop_suffix_flags);
	    తిరిగి wlv.అడ్డువరుస;
	}
	wlv.screen_అడ్డువరుస += wlv.పాఠ్యం_prop_above_లెక్క;
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_మొదటి_char = 0;
    ఒకవేళ (wp->w_p_lbr && సంఖ్య_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, పంక్తి, పంక్తి);
	(శూన్య)win_lbr_chartabsize(&cts, ఖాళీ, ఖాళీ);
	vcol_మొదటి_char = cts.cts_మొదటి_char;
	clear_chartabsize_arg(&cts);
    }
#అయితే_ముగింపు

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    ఒకవేళ (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_లోపల_పాఠ్యం_prop_above : 0;
    లేకపోతే
	v = wp->w_leftcol;
    ఒకవేళ (v > 0 && సంఖ్య_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	పూర్ణాంకం		charsize = 0;
	పూర్ణాంకం		తల = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, పంక్తి, ptr);
	cts.cts_max_తల_vcol = v;
	అయితే (cts.cts_vcol < v)
	{
	    తల = 0;
	    charsize = win_lbr_chartabsize(&cts, &తల, ఖాళీ);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    ఒకవేళ (*prev_ptr == NUL)
		ఆపు;
	    MB_PTR_ADV(cts.cts_ptr);
	    ఒకవేళ (wp->w_p_list)
	    {
		లోపల_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > పంక్తి && prev_ptr[-1] == ' '));
		ఒకవేళ (!లోపల_multispace)
		    multispace_pos = 0;
		లేకపోతే ఒకవేళ (cts.cts_ptr >= పంక్తి + leadcol
					 && wp->w_lcs_chars.multispace != ఖాళీ)
		{
		    ++multispace_pos;
		    ఒకవేళ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		లేకపోతే ఒకవేళ (cts.cts_ptr < పంక్తి + leadcol
				     && wp->w_lcs_chars.leadmultispace != ఖాళీ)
		{
		    ++multispace_pos;
		    ఒకవేళ (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	ఒకవేళ (wlv.vcol < v && (
#నిర్వచితమైతే FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_నిలువు ||
#అయితే_ముగింపు
	     virtual_చురుకైన() ||
	     (VIsual_చురుకైన && wp->w_బఫరు == curwin->w_బఫరు)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	ఒకవేళ (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	ఒకవేళ (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - తల;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	ఒకవేళ (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	లేకపోతే ఒకవేళ (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#నిర్వచితమైతే FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	ఒకవేళ (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	ఒకవేళ (spv->spv_has_spell)
	{
	    పూర్ణాంకం		len;
	    colnr_T	linecol = (colnr_T)(ptr - పంక్తి);
	    hlf_T	spell_hlf = HLF_లెక్క;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.నిలువు = linecol;
	    len = spell_move_గమ్యం(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
	    ptr = పంక్తి + linecol;

	    ఒకవేళ (len == 0 || (పూర్ణాంకం)wp->w_cursor.నిలువు > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		//మాట
		spell_hlf = HLF_లెక్క;
		మాట_చివర = (పూర్ణాంకం)(spell_గమ్యం_మాట_చివర(ptr, wp) - పంక్తి + 1);
	    }
	    లేకపోతే
	    {
		// bad word found, use attributes until end of word
		మాట_చివర = wp->w_cursor.నిలువు + len + 1;

		// Turn index into actual attributes.
		ఒకవేళ (spell_hlf != HLF_లెక్క)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    ఒకవేళ (has_syntax)
		syntax_ప్రారంభం(wp, lnum);
# endif
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    ఒకవేళ (సంఖ్య_only == 0)
    {
	v = (దీర్ఘ)(ptr - పంక్తి);
	area_highlighting |= prepare_శోధన_hl_పంక్తి(wp, lnum, (colnr_T)v,
					      &పంక్తి, &screen_శోధన_hl,
					      &శోధన_attr);
	ptr = పంక్తి + v; // "line" may have been updated
    }
#అయితే_ముగింపు

    ఒకవేళ ((State & బహుళకం_చొప్పించు) && ins_compl_win_చురుకైన(wp)
			    && (లోపల_curline || ins_compl_lnum_లోపల_range(lnum)))
	area_highlighting = TRUE;

#నిర్వచితమైతే FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    ఒకవేళ (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	ఒకవేళ (!(wp == curwin && VIsual_చురుకైన)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    ఒకవేళ (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    లేకపోతే
	    {
		పంక్తి_attr_save = wlv.పంక్తి_attr;
		margin_columns_win(wp, &ఎడమ_curline_నిలువు, &కుడి_curline_నిలువు);
	    }
	    area_highlighting = TRUE;
	}
    }
#అయితే_ముగింపు

    win_పంక్తి_ప్రారంభం(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    కొరకు (;;)
    {
#ఒకవేళ defined(FEAT_CONCEAL) || defined(FEAT_శోధన_EXTRA)
	పూర్ణాంకం	has_సరిపోలిక_conc = 0;	// match wants to conceal
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
	పూర్ణాంకం	did_decrement_ptr = FALSE;
#అయితే_ముగింపు

	// Skip this quickly when working on the text.
	ఒకవేళ (wlv.draw_state != WL_పంక్తి)
	{
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.పంక్తి_attr = పంక్తి_attr_save;
	    }
#అయితే_ముగింపు
	    ఒకవేళ (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		ఒకవేళ (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    ఒకవేళ (wlv.అడ్డువరుస == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    లేకపోతే
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	    ఒకవేళ (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		ఒకవేళ (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#అయితే_ముగింపు
	    ఒకవేళ (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_నిలువు(wp, &wlv, sign_present, అంకె_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    ఒకవేళ (సంఖ్య_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_పంక్తి(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		ఒకవేళ ((wlv.అడ్డువరుస + 1 - wlv.startrow < సంఖ్య_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#నిర్వచితమైతే FEAT_DIFF
			|| wlv.filler_todo > 0
#అయితే_ముగింపు
			)
		{
		    ++wlv.అడ్డువరుస;
		    ++wlv.screen_అడ్డువరుస;
		    ఒకవేళ (wlv.అడ్డువరుస == endrow)
			ఆపు;
#నిర్వచితమైతే FEAT_DIFF
		    --wlv.filler_todo;
		    ఒకవేళ (wlv.filler_todo == 0 && wp->w_botfill)
			ఆపు;
#అయితే_ముగింపు
		    win_పంక్తి_ప్రారంభం(wp, &wlv, TRUE);
		    కొనసాగు;
		}
		లేకపోతే
		    ఆపు;
	    }

#నిర్వచితమైతే FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    ఒకవేళ (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    ఒకవేళ (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#అయితే_ముగింపు
	    ఒకవేళ (wlv.draw_state == WL_పంక్తి - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_పంక్తి;
		win_పంక్తి_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (wlv.cul_screenline && wlv.draw_state == WL_పంక్తి
		&& wlv.vcol >= ఎడమ_curline_నిలువు
		&& wlv.vcol < కుడి_curline_నిలువు)
	    apply_cursorline_highlight(&wlv, sign_present);
#అయితే_ముగింపు

	// When still displaying '$' of change command, stop at cursor.
	ఒకవేళ (dollar_vcol >= 0 && లోపల_curline && wlv.vcol >= (దీర్ఘ)wp->w_virtcol)
	{
	    wlv_screen_పంక్తి(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (wp->w_p_cuc)
		wlv.అడ్డువరుస = wp->w_cline_అడ్డువరుస + wp->w_cline_ఎత్తు;
	    లేకపోతే
#అయితే_ముగింపు
		wlv.అడ్డువరుస = wp->w_ఎత్తు;
	    ఆపు;
	}

	ఒకవేళ (wlv.draw_state == WL_పంక్తి && (area_highlighting || extra_check))
	{
#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (పాఠ్యం_props != ఖాళీ)
	    {
		పూర్ణాంకం pi;
		పూర్ణాంకం bcol = (పూర్ణాంకం)(ptr - పంక్తి);

		ఒకవేళ (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !లోపల_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		కొరకు (pi = 0; pi < పాఠ్యం_props_చురుకైన; ++pi)
		{
		    పూర్ణాంకం		tpi = పాఠ్యం_prop_idxs[pi];
		    textprop_T  *tp = &పాఠ్యం_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    ఒకవేళ ((tp->tp_నిలువు != MAXCOL
				       && bcol >= tp->tp_నిలువు - 1 + tp->tp_len))
		    {
			ఒకవేళ (pi + 1 < పాఠ్యం_props_చురుకైన)
			    mch_memmove(పాఠ్యం_prop_idxs + pi,
					పాఠ్యం_prop_idxs + pi + 1,
					పరిమాణం(పూర్ణాంకం)
					     * (పాఠ్యం_props_చురుకైన - (pi + 1)));
			--పాఠ్యం_props_చురుకైన;
			పాఠ్యం_props_need_క్రమీకరించు = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			ఒకవేళ (లోపల_linebreak && syntax_attr == పాఠ్యం_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		ఒకవేళ (wlv.n_extra > 0 && లోపల_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		అయితే (పాఠ్యం_prop_తదుపరి < పాఠ్యం_prop_లెక్క)
		{
		    పూర్ణాంకం చురుకైన;
		    textprop_T *tp = &పాఠ్యం_props[పాఠ్యం_prop_తదుపరి];
		    ఒకవేళ (tp->tp_నిలువు == MAXCOL)
		    {
			ఒకవేళ (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    చురుకైన = TRUE;
			లేకపోతే ఒకవేళ (*ptr != NUL)
			    ఆపు;
			లేకపోతే
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    చురుకైన = wp->w_p_wrap
				  || wlv.అడ్డువరుస == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_క్రింద);
			}
		    }
		    లేకపోతే
		    {
			ఒకవేళ (bcol < tp->tp_నిలువు - 1)
			    ఆపు;
			చురుకైన = bcol <= tp->tp_నిలువు - 1 + tp->tp_len;
		    }

		    ఒకవేళ (చురుకైన)
		    {
			పాఠ్యం_prop_idxs[పాఠ్యం_props_చురుకైన++] = పాఠ్యం_prop_తదుపరి;
			పాఠ్యం_props_need_క్రమీకరించు = TRUE;
		    }
		    ++పాఠ్యం_prop_తదుపరి;
		}

		ఒకవేళ (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(పాఠ్యం_prop_type != ఖాళీ &&
			     పాఠ్యం_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    పాఠ్యం_prop_attr = 0;
		    పాఠ్యం_prop_attr_comb = 0;
		    పాఠ్యం_prop_flags = 0;
		    పాఠ్యం_prop_type = ఖాళీ;
		    పాఠ్యం_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		ఒకవేళ (పాఠ్యం_props_చురుకైన > 0 && wlv.n_extra == 0)
		{
		    పూర్ణాంకం వాడిన_tpi = -1;
		    పూర్ణాంకం వాడిన_attr = 0;
		    పూర్ణాంకం other_tpi = -1;

		    పాఠ్యం_prop_above = FALSE;
		    పాఠ్యం_prop_follows = FALSE;

		    ఒకవేళ (పాఠ్యం_props_need_క్రమీకరించు)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			క్రమీకరించు_పాఠ్యం_props(wp->w_బఫరు, పాఠ్యం_props,
					పాఠ్యం_prop_idxs, పాఠ్యం_props_చురుకైన);
			పాఠ్యం_props_need_క్రమీకరించు = FALSE;
		    }

		    కొరకు (pi = 0; pi < పాఠ్యం_props_చురుకైన; ++pi)
		    {
			పూర్ణాంకం	    tpi = పాఠ్యం_prop_idxs[pi];
			textprop_T  *tp = &పాఠ్యం_props[tpi];
			proptype_T  *pt = పాఠ్యం_prop_type_by_id(
						    wp->w_బఫరు, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			ఒకవేళ (pt != ఖాళీ
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_కుడి
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_క్రింద)) == 0
				    && wlv.నిలువు >= wp->w_వెడల్పు))
			{
			    ఒకవేళ (tp->tp_నిలువు == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == పంక్తి
						&& !did_పంక్తి
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_క్రింద))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				పాఠ్యం_prop_follows = TRUE;
				కొనసాగు;
			    }

			    ఒకవేళ (pt->pt_hl_id > 0)
				వాడిన_attr = syn_id2attr(pt->pt_hl_id);
			    పాఠ్యం_prop_type = pt;
			    పాఠ్యం_prop_attr =
				   hl_combine_attr(పాఠ్యం_prop_attr, వాడిన_attr);
			    ఒకవేళ (వాడిన_tpi >= 0 && పాఠ్యం_props[వాడిన_tpi].tp_id < 0)
				other_tpi = వాడిన_tpi;
			    పాఠ్యం_prop_flags = pt->pt_flags;
			    పాఠ్యం_prop_id = tp->tp_id;
			    వాడిన_tpi = tpi;
			}
		    }
		    ఒకవేళ (పాఠ్యం_prop_id < 0 && వాడిన_tpi >= 0
				 && పాఠ్యం_props[వాడిన_tpi].u.tp_పాఠ్యం != ఖాళీ)
		    {
			textprop_T  *tp = &పాఠ్యం_props[వాడిన_tpi];
			char_u	    *p = tp->u.tp_పాఠ్యం;
			పూర్ణాంకం	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			పూర్ణాంకం	    bail_బహిర్గతం = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			ఒకవేళ (p != ఖాళీ)
			{
			    పూర్ణాంకం	    కుడి_2 = (tp->tp_flags
							& TP_FLAG_ALIGN_కుడి);
			    పూర్ణాంకం	    క్రింద = (tp->tp_flags
							& TP_FLAG_ALIGN_క్రింద);
			    పూర్ణాంకం	    wrap = tp->tp_నిలువు < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    పూర్ణాంకం	    padding = tp->tp_నిలువు == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (పూర్ణాంకం)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.ప్రారంభం_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    వాడిన_attr);
			    n_attr = mb_charlen(p);
			    పాఠ్యం_prop_attr = 0;
			    పాఠ్యం_prop_attr_comb = 0;
			    ఒకవేళ (*ptr == NUL)
				// don't combine char attr after EOL
				పాఠ్యం_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    ఒకవేళ (పాఠ్యం_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    ఒకవేళ ((కుడి_2 || above || క్రింద || !wrap
					    || padding > 0) && wp->w_వెడల్పు > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				పూర్ణాంకం	ప్రారంభం_పంక్తి;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				ప్రారంభం_పంక్తి = పాఠ్యం_prop_position(wp, tp,
						    wlv.vcol,
# ifdef FEAT_RIGHTLEFT
						    wp->w_p_rl
						    ? wp->w_వెడల్పు - wlv.నిలువు - 1
						    :
# endif
						    wlv.నిలువు,
						    &wlv.n_extra, &wlv.p_extra,
						    &n_attr, &wlv.n_attr_skip,
						    skip_cells > 0);
				ఒకవేళ (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				ఒకవేళ (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				ఒకవేళ (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.నిలువు
					       + wlv.n_extra - 2 > wp->w_వెడల్పు)
				    // don't bail out at end of line
				    పాఠ్యం_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				ఒకవేళ (ప్రారంభం_పంక్తి)
				{
				    draw_screen_పంక్తి(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    ఒకవేళ (wlv.అడ్డువరుస == endrow)
				    {
					++wlv.అడ్డువరుస;
					ఆపు;
				    }
				    win_పంక్తి_ప్రారంభం(wp, &wlv, TRUE);
				    bail_బహిర్గతం = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			ఒకవేళ (skip_cells > 0)
			{
			    ఒకవేళ (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				ఒకవేళ (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    లేకపోతే
			    {
				// the whole text is left of the window, drop
				// it and advance to the next one
				skip_cells -= wlv.n_extra;
				skipped_cells += wlv.n_extra;
				wlv.n_extra = 0;
				wlv.n_attr_skip = 0;
				bail_బహిర్గతం = TRUE;
			    }
			}

			// If another text prop follows the condition below at
			// the last window column must know.
			// If this is an "above" text prop and 'nowrap' then we
			// must wrap anyway.
			పాఠ్యం_prop_above = above;
			పాఠ్యం_prop_follows |= other_tpi != -1
					&& (wp->w_p_wrap
					     || (పాఠ్యం_props[other_tpi].tp_flags
			       & (TP_FLAG_ALIGN_క్రింద | TP_FLAG_ALIGN_కుడి)));

			ఒకవేళ (bail_బహిర్గతం)
			    // starting a new line for "below"
			    కొనసాగు;
		    }
		}
		లేకపోతే ఒకవేళ (పాఠ్యం_prop_తదుపరి < పాఠ్యం_prop_లెక్క
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.నిలువు == wp->w_వెడల్పు - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    పూర్ణాంకం only_క్రింద_follows = !wp->w_p_wrap
				 && wlv.నిలువు + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_వెడల్పు;
		    పూర్ణాంకం suffix_flags = పాఠ్యం_prop_suffix_flags[పాఠ్యం_prop_తదుపరి];

		    పాఠ్యం_prop_follows = (suffix_flags
					& (only_క్రింద_follows ? 2 : 1)) != 0;
		}
	    }

	    ఒకవేళ (wlv.ప్రారంభం_extra_for_textprop)
	    {
		wlv.ప్రారంభం_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_శోధన_attr = శోధన_attr;
		saved_area_attr = area_attr;
		శోధన_attr = 0;
		area_attr = 0;
	    }
#అయితే_ముగింపు

	    పూర్ణాంకం *area_attr_p =
#నిర్వచితమైతే FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#అయితే_ముగింపు
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    ఒకవేళ (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ఖాళీ
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    లేకపోతే ఒకవేళ (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    ఒకవేళ (wlv.n_extra == 0)
	    {
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (దీర్ఘ)(ptr - పంక్తి);
		శోధన_attr = update_శోధన_hl(wp, lnum, (colnr_T)v, &పంక్తి,
				      &screen_శోధన_hl, &has_సరిపోలిక_conc,
				      &సరిపోలిక_conc, did_పంక్తి_attr, lcs_eol_one,
				      &on_చివరి_నిలువు);
		ptr = పంక్తి + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		ఒకవేళ (*ptr == NUL)
		    has_సరిపోలిక_conc = 0;
#లేకపోతే
		శోధన_attr = 0;
#అయితే_ముగింపు

		// Check if ComplMatchIns highlight is needed.
		ఒకవేళ ((State & బహుళకం_చొప్పించు) && ins_compl_win_చురుకైన(wp)
			    && (లోపల_curline || ins_compl_lnum_లోపల_range(lnum)))
		{
		    పూర్ణాంకం ins_సరిపోలిక_attr =
			ins_compl_నిలువు_range_attr(lnum, (పూర్ణాంకం)(ptr - పంక్తి));
		    ఒకవేళ (ins_సరిపోలిక_attr > 0)
			శోధన_attr =
			    hl_combine_attr(శోధన_attr, ins_సరిపోలిక_attr);
		}
	    }

#నిర్వచితమైతే FEAT_DIFF
	    ఒకవేళ (wlv.diff_hlf != (hlf_T)0)
	    {
		ఒకవేళ (పంక్తి_changes.అంకె_changes > 0
			&& change_సూచీ >= 0
			&& change_సూచీ < పంక్తి_changes.అంకె_changes - 1)
		{
		    ఒకవేళ (ptr - పంక్తి >= పంక్తి_changes.changes[change_సూచీ+1].dc_ప్రారంభం[పంక్తి_changes.bufidx])
			change_సూచీ += 1;
		}
		పూర్ణాంకం added = FALSE;
		ఒకవేళ (పంక్తి_changes.అంకె_changes > 0 && change_సూచీ >= 0 && change_సూచీ < పంక్తి_changes.అంకె_changes)
		{
		    added = diff_change_parse(&పంక్తి_changes,
			    &పంక్తి_changes.changes[change_సూచీ],
			    &change_ప్రారంభం,
			    &change_చివర);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		ఒకవేళ (wlv.diff_hlf == HLF_CHD && ptr - పంక్తి >= change_ప్రారంభం
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		ఒకవేళ ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - పంక్తి >= change_చివర && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.పంక్తి_attr = HL_ATTR(wlv.diff_hlf);
		ఒకవేళ (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ఎడమ_curline_నిలువు
					    && wlv.vcol <= కుడి_curline_నిలువు)))
		    wlv.పంక్తి_attr = hl_combine_attr(
					  wlv.పంక్తి_attr, HL_ATTR(HLF_CUL));
	    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		ఒకవేళ (get_పదం_attr)
		    syntax_attr = పదం_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		ఒకవేళ (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (దీర్ఘ)(ptr - పంక్తి);
		    ఒకవేళ (v == prev_syntax_నిలువు)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    లేకపోతే
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ఖాళీ, FALSE);
			prev_syntax_నిలువు = v;
			prev_syntax_attr = syntax_attr;
		    }

		    ఒకవేళ (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    లేకపోతే
			did_emsg = save_did_emsg;
# ifdef SYN_సమయం_హద్దు
		    ఒకవేళ (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
		    ptr = పంక్తి + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    ఒకవేళ (*ptr == NUL)
			syntax_flags = 0;
		    లేకపోతే
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    ఒకవేళ (పాఠ్యం_prop_type != ఖాళీ)
	    {
		ఒకవేళ (పాఠ్యం_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, పాఠ్యం_prop_attr);
		లేకపోతే
		    syntax_attr = పాఠ్యం_prop_attr;
		పాఠ్యం_prop_attr_comb = syntax_attr;
	    }
# endif
#అయితే_ముగింపు

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#నిర్వచితమైతే పంక్తి_ATTR
	    ఒకవేళ (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.పంక్తి_attr, area_attr);
		ఒకవేళ (!highlight_సరిపోలిక)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(శోధన_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    లేకపోతే ఒకవేళ (శోధన_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.పంక్తి_attr, శోధన_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    లేకపోతే ఒకవేళ (wlv.పంక్తి_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.పంక్తి_attr);
# లేకపోతే
		wlv.char_attr = wlv.పంక్తి_attr;
# endif
		attr_pri = FALSE;
	    }
#లేకపోతే
	    ఒకవేళ (area_attr != 0)
		wlv.char_attr = area_attr;
	    లేకపోతే ఒకవేళ (శోధన_attr != 0)
		wlv.char_attr = శోధన_attr;
#అయితే_ముగింపు
	    లేకపోతే
	    {
		attr_pri = FALSE;
#నిర్వచితమైతే FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#లేకపోతే
		wlv.char_attr = 0;
#అయితే_ముగింపు
	    }
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    ఒకవేళ (పాఠ్యం_prop_type != ఖాళీ && (పాఠ్యం_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, పాఠ్యం_prop_attr);
#అయితే_ముగింపు
	}

	// combine attribute with HLF_WIN
	ఒకవేళ (wlv.win_attr != 0)
	{
	    ఒకవేళ (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    లేకపోతే
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
	ఒకవేళ (wlv.n_extra > 0)
	{
	    ఒకవేళ (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		లేకపోతే
		    mb_utf8 = FALSE;
	    }
	    లేకపోతే
	    {
		c = *wlv.p_extra;
		ఒకవేళ (has_mbyte)
		{
		    mb_c = c;
		    ఒకవేళ (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			ఒకవేళ (mb_l > wlv.n_extra)
			    mb_l = 1;
			లేకపోతే ఒకవేళ (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    లేకపోతే
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			ఒకవేళ (mb_l >= wlv.n_extra)
			    mb_l = 1;
			లేకపోతే ఒకవేళ (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    ఒకవేళ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    ఒకవేళ ((
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.నిలువు <= 0) :
#అయితే_ముగింపు
				    (wlv.నిలువు >= wp->w_వెడల్పు - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = HL_ATTR(HLF_AT);
#నిర్వచితమైతే FEAT_SYN_HL
			ఒకవేళ (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#అయితే_ముగింపు
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    లేకపోతే
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#ఒకవేళ defined(FEAT_PROP_POPUP)
	    ఒకవేళ (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		ఒకవేళ (wlv.saved_n_extra <= 0)
		{
		    ఒకవేళ (శోధన_attr == 0)
			శోధన_attr = saved_శోధన_attr;
		    ఒకవేళ (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    ఒకవేళ (wlv.extra_for_textprop)
			// wlv.extra_attr should be used at this position but
			// not any further.
			reset_extra_attr = TRUE;
		}

		wlv.extra_for_textprop = FALSE;
# ifdef FEAT_LINEBREAK
		లోపల_linebreak = FALSE;
# endif
	    }
#అయితే_ముగింపు
	}
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_LINEBREAK
	    పూర్ణాంకం		c0;
#అయితే_ముగింపు
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#నిర్వచితమైతే FEAT_LINEBREAK
	    c0 = *ptr;
#అయితే_ముగింపు
	    ఒకవేళ (c == NUL)
	    {
#నిర్వచితమైతే FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_పంక్తి = TRUE;
#అయితే_ముగింపు
		// no more cells to skip
		skip_cells = 0;
	    }

	    ఒకవేళ (has_mbyte)
	    {
		mb_c = c;
		ఒకవేళ (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    ఒకవేళ (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			ఒకవేళ (mb_c < 0x80)
			{
			    c = mb_c;
#నిర్వచితమైతే FEAT_LINEBREAK
			    c0 = mb_c;
#అయితే_ముగింపు
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			ఒకవేళ (utf_iscomposing(mb_c))
			{
			    పూర్ణాంకం i;

			    కొరకు (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    ఒకవేళ ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#నిర్వచితమైతే FEAT_RIGHTLEFT
			ఒకవేళ (wp->w_p_rl)		//తిరగేయి
			    rl_mirror(wlv.extra);
#అయితే_ముగింపు
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (పూర్ణాంకం)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			ఒకవేళ (area_attr == 0 && శోధన_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    లేకపోతే ఒకవేళ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#నిర్వచితమైతే FEAT_ARABIC
		    లేకపోతే ఒకవేళ (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			పూర్ణాంకం	pc, pc1, nc;
			పూర్ణాంకం	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ఒకవేళ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			లేకపోతే
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    లేకపోతే
			prev_c = mb_c;
#అయితే_ముగింపు
		}
		లేకపోతే	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    ఒకవేళ (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    లేకపోతే ఒకవేళ (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			ఒకవేళ (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			లేకపోతే
			{
			    ఒకవేళ (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_బఫరు, wlv.extra, c);
				wlv.n_extra = (పూర్ణాంకం)STRLEN(wlv.extra) - 1;
			    }
			    లేకపోతే
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
			    ఒకవేళ (area_attr == 0 && శోధన_attr == 0)
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
		ఒకవేళ ((
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.నిలువు <= 0) :
#అయితే_ముగింపు
				(wlv.నిలువు >= wp->w_వెడల్పు - 1))
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
#నిర్వచితమైతే FEAT_CONCEAL
		    did_decrement_ptr = TRUE;
#అయితే_ముగింపు
		}
		లేకపోతే ఒకవేళ (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		ఒకవేళ (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    ఒకవేళ (area_attr == 0 && శోధన_attr == 0)
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

	    ఒకవేళ (extra_check)
	    {
#నిర్వచితమైతే FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (దీర్ఘ)(ptr - పంక్తి);
		ఒకవేళ (spv->spv_has_spell && v >= మాట_చివర && v > cur_checked_నిలువు)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    ఒకవేళ (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			పూర్ణాంకం	len;
			hlf_T	spell_hlf = HLF_లెక్క;

			ఒకవేళ (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			ఒకవేళ ((prev_ptr - పంక్తి) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - పంక్తి) - nextlinecol;
			లేకపోతే
			    p = prev_ptr;
			spv->spv_cap_నిలువు -= (పూర్ణాంకం)(prev_ptr - పంక్తి);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_నిలువు,
							    spv->spv_unchanged);
			మాట_చివర = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			ఒకవేళ (spell_hlf != HLF_లెక్క
				&& (State & బహుళకం_చొప్పించు)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.నిలువు >=
						    (colnr_T)(prev_ptr - పంక్తి)
				&& wp->w_cursor.నిలువు < (colnr_T)మాట_చివర)
			{
			    spell_hlf = HLF_లెక్క;
			    spell_redraw_lnum = lnum;
			}

			ఒకవేళ (spell_hlf == HLF_లెక్క && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_నిలువు = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			ఒకవేళ (spell_hlf != HLF_లెక్క)
			    spell_attr = highlight_attr[spell_hlf];

			ఒకవేళ (spv->spv_cap_నిలువు > 0)
			{
			    ఒకవేళ (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_నిలువు
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_నిలువు = ((p - nextline)
					    + spv->spv_cap_నిలువు - nextline_idx);
			    }
			    లేకపోతే
				// Compute the actual column.
				spv->spv_cap_నిలువు += (prev_ptr - పంక్తి);
			}
		    }
		}
		ఒకవేళ (spell_attr != 0)
		{
		    ఒకవేళ (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    లేకపోతే
			wlv.char_attr = hl_combine_attr(spell_attr,
								wlv.char_attr);
		}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
		// we don't want linebreak to apply for lines that start with
		// leading spaces, followed by long letters (since it would add
		// a break at the beginning of a line and this might be unexpected)
		//
		// So only allow to linebreak, once we have found chars not in
		// 'breakat' in the line.
		ఒకవేళ ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((పూర్ణాంకం)*ptr))
		    wlv.need_lbr = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
		// Found last space before word: check for line break.
		ఒకవేళ (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((పూర్ణాంకం)*ptr))
		{
		    పూర్ణాంకం	    mb_off = has_mbyte ? (*mb_తల_off)(పంక్తి, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_మొదటి_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, పంక్తి, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_పాఠ్యం = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ఖాళీ, ఖాళీ) - 1;
		    clear_chartabsize_arg(&cts);

		    ఒకవేళ (on_చివరి_నిలువు && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			శోధన_attr = 0;

		    ఒకవేళ (c == TAB && wlv.n_extra + wlv.నిలువు > wp->w_వెడల్పు)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_బఫరు->b_p_ts,
					      wp->w_బఫరు->b_p_vts_array) - 1;
# లేకపోతే
			wlv.n_extra = (పూర్ణాంకం)wp->w_బఫరు->b_p_ts
				    - wlv.vcol % (పూర్ణాంకం)wp->w_బఫరు->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    ఒకవేళ (wlv.n_extra > 0 && c != TAB)
			లోపల_linebreak = TRUE;
# endif
		    ఒకవేళ (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			ఒకవేళ (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			ఒకవేళ (!wp->w_p_list)
			    c = ' ';
		    }
		}
#అయితే_ముగింపు
		ఒకవేళ (wp->w_p_list)
		{
		    లోపల_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > పంక్తి && prev_ptr[-1] == ' '));
		    ఒకవేళ (!లోపల_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		ఒకవేళ (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (లోపల_multispace
					&& wp->w_lcs_chars.multispace != ఖాళీ))
				&& ptr - పంక్తి >= leadcol
				&& ptr - పంక్తి <= trailcol)))
		{
		    ఒకవేళ (లోపల_multispace && wp->w_lcs_chars.multispace != ఖాళీ)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			ఒకవేళ (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    లేకపోతే
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    ఒకవేళ (area_attr == 0 && శోధన_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    లేకపోతే
			mb_utf8 = FALSE;
		}

		ఒకవేళ (c == ' ' && ((trailcol != MAXCOL && ptr > పంక్తి + trailcol)
				    || (leadcol != 0 && ptr < పంక్తి + leadcol)))
		{
		    ఒకవేళ (leadcol != 0 && లోపల_multispace && ptr < పంక్తి + leadcol
			    && wp->w_lcs_chars.leadmultispace != ఖాళీ)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			ఒకవేళ (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    లేకపోతే ఒకవేళ (ptr > పంక్తి + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    లేకపోతే ఒకవేళ (ptr < పంక్తి + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    లేకపోతే ఒకవేళ (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    ఒకవేళ (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    లేకపోతే
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    ఒకవేళ (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		ఒకవేళ (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    పూర్ణాంకం	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    దీర్ఘ    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    పూర్ణాంకం	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    పూర్ణాంకం	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    పూర్ణాంకం	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ఒకవేళ (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < పంక్తి + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#నిర్వచితమైతే FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_విలువ(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    ఒకవేళ (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#అయితే_ముగింపు
		    // tab amount depends on current column
#నిర్వచితమైతే FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_బఫరు->b_p_ts,
					      wp->w_బఫరు->b_p_vts_array) - 1;
#లేకపోతే
		    tab_len = (పూర్ణాంకం)wp->w_బఫరు->b_p_ts
			       - vcol_adjusted % (పూర్ణాంకం)wp->w_బఫరు->b_p_ts - 1;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_LINEBREAK
		    ఒకవేళ (!wp->w_p_lbr || !wp->w_p_list)
#అయితే_ముగింపు
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#నిర్వచితమైతే FEAT_LINEBREAK
		    లేకపోతే
		    {
			char_u	*p;
			పూర్ణాంకం	len;
			పూర్ణాంకం	i;
			పూర్ణాంకం	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			ఒకవేళ (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			ఒకవేళ (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			ఒకవేళ (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    పూర్ణాంకం tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    ఒకవేళ (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    ఒకవేళ (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    ఒకవేళ (p == ఖాళీ)
				wlv.n_extra = 0;
			    లేకపోతే
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				కొరకు (i = 0; i < tab_len; i++)
				{
				    పూర్ణాంకం lcs = lcs_tab2;

				    ఒకవేళ (*p == NUL)
				    {
					tab_len = i;
					ఆపు;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    ఒకవేళ (lcs_tab3 && i == tab_len - 1)
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
				ఒకవేళ (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
		    {
			పూర్ణాంకం vc_saved = wlv.vcol_off_co;

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
			ఒకవేళ (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#అయితే_ముగింపు
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    ఒకవేళ (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#నిర్వచితమైతే FEAT_LINEBREAK
			ఒకవేళ (wp->w_p_lbr && wlv.p_extra != ఖాళీ
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			లేకపోతే
#అయితే_ముగింపు
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    లేకపోతే
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		లేకపోతే ఒకవేళ (c == NUL
			&& wlv.n_extra == 0
			&& (wp->w_p_list
			    || (wlv.fromcol >= 0
				&& wlv.tocol > wlv.vcol
				&& VIsual_బహుళకం != Ctrl_V
				&& (
#నిర్వచితమైతే FEAT_RIGHTLEFT
				    wp->w_p_rl ? (wlv.నిలువు >= 0) :
#అయితే_ముగింపు
				    (wlv.నిలువు < wp->w_వెడల్పు))))
			&& lcs_eol_one > 0)
		{
		    // Display a '$' after the line or highlight an extra
		    // character if the line break is included.
#ఒకవేళ defined(FEAT_DIFF) || defined(పంక్తి_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    ఒకవేళ (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef పంక్తి_ATTR
			    &&
#  endif
# endif
# ifdef పంక్తి_ATTR
			    wlv.పంక్తి_attr == 0
# endif
		       )
#అయితే_ముగింపు
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			ఒకవేళ (!(area_highlighting && virtual_చురుకైన()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    ఒకవేళ (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    లేకపోతే
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    ఒకవేళ (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    లేకపోతే
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		లేకపోతే ఒకవేళ (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_బఫరు, c);
		    ఒకవేళ (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    ఒకవేళ ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#అయితే_ముగింపు
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#నిర్వచితమైతే FEAT_LINEBREAK
		    ఒకవేళ (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			ఒకవేళ (p == ఖాళీ)
			    wlv.n_extra = 0;
			లేకపోతే
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    లేకపోతే
#అయితే_ముగింపు
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    ఒకవేళ (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#నిర్వచితమైతే FEAT_PROP_POPUP
			ఒకవేళ (పాఠ్యం_prop_type != ఖాళీ &&
				 పాఠ్యం_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(పాఠ్యం_prop_attr,
							       wlv.extra_attr);
#అయితే_ముగింపు

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		లేకపోతే ఒకవేళ (VIsual_చురుకైన
			 && (VIsual_బహుళకం == Ctrl_V
			     || VIsual_బహుళకం == 'v')
			 && virtual_చురుకైన()
			 && wlv.tocol != MAXCOL
			 && wlv.vcol < wlv.tocol
			 && (
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.నిలువు >= 0) :
#అయితే_ముగింపు
			    (wlv.నిలువు < wp->w_వెడల్పు)))
		{
		    c = ' ';
		    --ptr;	    // put it back at the NUL
		}
#ఒకవేళ defined(పంక్తి_ATTR)
		లేకపోతే ఒకవేళ ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.పంక్తి_attr != 0
			) && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (wlv.నిలువు >= 0) :
# endif
			    (wlv.నిలువు
# ifdef FEAT_CONCEAL
				- wlv.boguscols
# endif
					    < wp->w_వెడల్పు)))
		{
		    // Highlight until the right side of the window
		    c = ' ';
		    --ptr;	    // put it back at the NUL

		    // Remember we do the char for line highlighting.
		    ++did_పంక్తి_attr;

		    // don't do search HL for the rest of the line
		    ఒకవేళ (wlv.పంక్తి_attr != 0 && wlv.char_attr == శోధన_attr
					&& (did_పంక్తి_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.పంక్తి_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    ఒకవేళ (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_పంక్తి
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    ఒకవేళ (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			ఒకవేళ (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    ఒకవేళ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ఎడమ_curline_నిలువు
					    && wlv.vcol <= కుడి_curline_నిలువు)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    ఒకవేళ (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			ఒకవేళ (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    ఒకవేళ (!wlv.cul_screenline
				    || (wlv.vcol >= ఎడమ_curline_నిలువు
					     && wlv.vcol <= కుడి_curline_నిలువు))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			లేకపోతే ఒకవేళ (wlv.పంక్తి_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.పంక్తి_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			ఒకవేళ (wlv.vcol == 0)
			{
			    ఒకవేళ (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    లేకపోతే ఒకవేళ (శోధన_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, శోధన_attr);
			}
		    }
# endif
		}
#అయితే_ముగింపు
	    }

#నిర్వచితమైతే FEAT_CONCEAL
	    ఒకవేళ (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_పంక్తి(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_సరిపోలిక_conc > 0)
		&& !(lnum_లోపల_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ఖాళీ))
	    {
		పూర్ణాంకం syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		ఒకవేళ (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_సరిపోలిక_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_సరిపోలిక_conc && సరిపోలిక_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    ఒకవేళ (has_సరిపోలిక_conc && సరిపోలిక_conc)
			c = సరిపోలిక_conc;
		    లేకపోతే ఒకవేళ (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    లేకపోతే ఒకవేళ (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    లేకపోతే
			c = ' ';

		    ఒకవేళ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    లేకపోతే
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    ఒకవేళ (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    ఒకవేళ (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			ఒకవేళ (wp->w_p_rl)
			{
			    wlv.నిలువు -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			లేకపోతే
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.నిలువు += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		లేకపోతే ఒకవేళ (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    లేకపోతే
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    ఒకవేళ (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#అయితే_ముగింపు // FEAT_CONCEAL
	}

#నిర్వచితమైతే FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	ఒకవేళ (!did_wcol && wlv.draw_state == WL_పంక్తి
		&& లోపల_curline && conceal_cursor_పంక్తి(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
		wp->w_wcol = wp->w_వెడల్పు - wlv.నిలువు + wlv.boguscols - 1;
	    లేకపోతే
# endif
		wp->w_wcol = wlv.నిలువు - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    ఒకవేళ (wlv.vcol + skip_cells < wp->w_virtcol)
		// Cursor beyond end of the line with 'virtualedit'.
		wp->w_wcol += wp->w_virtcol - wlv.vcol - skip_cells;
	    wp->w_wrow = wlv.అడ్డువరుస;
	    did_wcol = TRUE;
	    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
# ifdef FEAT_PROP_POPUP
	    curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
# endif
	}
#అయితే_ముగింపు

	// Use "wlv.extra_attr", but don't override visual selection
	// highlighting, unless text property overrides.
	// Don't use "wlv.extra_attr" until wlv.n_attr_skip is zero.
	ఒకవేళ (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_పంక్తి
		&& (!attr_pri
#నిర్వచితమైతే FEAT_PROP_POPUP
		    || (పాఠ్యం_prop_flags & PT_FLAG_OVERRIDE)
#అయితే_ముగింపు
		   ))
	{
#నిర్వచితమైతే పంక్తి_ATTR
	    ఒకవేళ (wlv.పంక్తి_attr)
		wlv.char_attr = hl_combine_attr(wlv.పంక్తి_attr, wlv.extra_attr);
	    లేకపోతే
#అయితే_ముగింపు
		wlv.char_attr = wlv.extra_attr;
#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#అయితే_ముగింపు
	}

#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	ఒకవేళ (p_imst == IM_ON_THE_SPOT
		&& xic != ఖాళీ
		&& lnum == wp->w_cursor.lnum
		&& (State & బహుళకం_చొప్పించు)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_పంక్తి)
	{
	    colnr_T tcol;

	    ఒకవేళ (preedit_చివర_నిలువు == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ఖాళీ, ఖాళీ, 0);
	    లేకపోతే
		tcol = preedit_చివర_నిలువు;
	    ఒకవేళ ((దీర్ఘ)preedit_ప్రారంభం_నిలువు <= wlv.vcol && wlv.vcol < (దీర్ఘ)tcol)
	    {
		ఒకవేళ (feedback_old_attr < 0)
		{
		    feedback_నిలువు = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_నిలువు);
		ఒకవేళ (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_నిలువు++;
	    }
	    లేకపోతే ఒకవేళ (feedback_old_attr >= 0)
	    {
		wlv.char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_నిలువు = 0;
	    }
	}
#అయితే_ముగింపు
	// Handle the case where we are in column 0 but not on the first
	// character of the line and the user wants us to show us a
	// special character (via 'listchars' option "precedes:<char>").
	ఒకవేళ (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? (wp->w_skipcol > 0 && wlv.అడ్డువరుస == 0)
				 : wp->w_leftcol > 0)
#నిర్వచితమైతే FEAT_DIFF
		&& wlv.filler_todo <= 0
#అయితే_ముగింపు
		&& wlv.draw_state > WL_NR
		&& skip_cells <= 0
		&& c != NUL)
	{
	    c = wp->w_lcs_chars.prec;
	    lcs_prec_todo = NUL;
	    ఒకవేళ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    లేకపోతే
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    ఒకవేళ (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	ఒకవేళ ((c == NUL
#ఒకవేళ defined(పంక్తి_ATTR)
		|| did_పంక్తి_attr == 1
#అయితే_ముగింపు
		) && wlv.eol_hl_off == 0)
	{
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    పూర్ణాంకం prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_శోధన_hl,
					      (దీర్ఘ)(ptr - పంక్తి) - (c == NUL));
#అయితే_ముగింపు
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    ఒకవేళ (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_బహుళకం != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#నిర్వచితమైతే FEAT_శోధన_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_చురుకైన))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# ఒకవేళ defined(పంక్తి_ATTR)
			    && did_పంక్తి_attr <= 1
# endif
			   )
#అయితే_ముగింపు
		       ))
	    {
		పూర్ణాంకం n = 0;

#నిర్వచితమైతే FEAT_RIGHTLEFT
		ఒకవేళ (wp->w_p_rl)
		{
		    ఒకవేళ (wlv.నిలువు < 0)
			n = 1;
		}
		లేకపోతే
#అయితే_ముగింపు
		{
		    ఒకవేళ (wlv.నిలువు >= wp->w_వెడల్పు)
			n = -1;
		}
		ఒకవేళ (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.నిలువు += n;
		}
		లేకపోతే
		{
		    // Add a blank character to highlight.
		    Screen_పంక్తులు[wlv.off] = ' ';
		    ఒకవేళ (enc_utf8)
			Screen_పంక్తులు_UC[wlv.off] = 0;
		}
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		ఒకవేళ (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_శోధన_సరిపోలిక_hl(wp, &screen_శోధన_hl,
					   (దీర్ఘ)(ptr - పంక్తి), &wlv.char_attr);
		}
#అయితే_ముగింపు
		ScreenAttrs[wlv.off] = wlv.char_attr;
		Screen_నిలువుల_సంఖ్య[wlv.off] = wlv.vcol;
#నిర్వచితమైతే FEAT_RIGHTLEFT
		ఒకవేళ (wp->w_p_rl)
		{
		    --wlv.నిలువు;
		    --wlv.off;
		}
		లేకపోతే
#అయితే_ముగింపు
		{
		    ++wlv.నిలువు;
		    ++wlv.off;
		}
		++wlv.vcol;
		wlv.eol_hl_off = 1;
	    }
	}

	// At end of the text line.
	ఒకవేళ (c == NUL)
	{
#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (పాఠ్యం_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		draw_screen_పంక్తి(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		ఒకవేళ (లోపల_curline)
		{
		    curwin->w_cline_అడ్డువరుస = startrow;
		    curwin->w_cline_ఎత్తు = wlv.అడ్డువరుస - startrow;
#నిర్వచితమైతే FEAT_FOLDING
		    curwin->w_cline_folded = false;
#అయితే_ముగింపు
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		ఆపు;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	పూర్ణాంకం lcs_ext = get_lcs_ext(wp);
	ఒకవేళ (lcs_ext != NUL
		&& wlv.draw_state == WL_పంక్తి
#నిర్వచితమైతే FEAT_DIFF
		&& wlv.filler_todo <= 0
#అయితే_ముగింపు
		&& (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    wp->w_p_rl ? wlv.నిలువు == 0 :
#అయితే_ముగింపు
		    wlv.నిలువు == wp->w_వెడల్పు - 1)
		&& (*ptr != NUL
		    || lcs_eol_one > 0
		    || (wlv.n_extra > 0 && (wlv.c_extra != NUL
						     || *wlv.p_extra != NUL))
#నిర్వచితమైతే FEAT_PROP_POPUP
		    || పాఠ్యం_prop_తదుపరి <= చివరి_textprop_పాఠ్యం_idx
#అయితే_ముగింపు
		   ))
	{
	    c = lcs_ext;
	    wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
	    mb_c = c;
	    ఒకవేళ (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    లేకపోతే
		mb_utf8 = FALSE;
	}

#నిర్వచితమైతే FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	ఒకవేళ (wlv.draw_color_నిలువు)
	    wlv.draw_color_నిలువు = advance_color_నిలువు(VCOL_HLC, &wlv.color_నిలువుల_సంఖ్య);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	ఒకవేళ (((wlv.draw_state == WL_పంక్తి
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_లోపల_visual_area
		&& శోధన_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    ఒకవేళ (wp->w_p_cuc && VCOL_HLC == (దీర్ఘ)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    లేకపోతే ఒకవేళ (wlv.draw_color_నిలువు && VCOL_HLC == *wlv.color_నిలువుల_సంఖ్య)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#అయితే_ముగింపు

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	ఒకవేళ (wlv.draw_state < WL_పంక్తి || skip_cells <= 0)
	{
	    // Store the character.
#ఒకవేళ defined(FEAT_RIGHTLEFT)
	    ఒకవేళ (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.నిలువు;
	    }
#అయితే_ముగింపు
	    Screen_పంక్తులు[wlv.off] = c;
	    ఒకవేళ (enc_dbcs == DBCS_JPNU)
	    {
		ఒకవేళ ((mb_c & 0xff00) == 0x8e00)
		    Screen_పంక్తులు[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    లేకపోతే ఒకవేళ (enc_utf8)
	    {
		ఒకవేళ (mb_utf8)
		{
		    పూర్ణాంకం i;

		    Screen_పంక్తులు_UC[wlv.off] = mb_c;
		    ఒకవేళ ((c & 0xff) == 0)
			Screen_పంక్తులు[wlv.off] = 0x80;   // avoid storing zero
		    కొరకు (i = 0; i < Screen_mco; ++i)
		    {
			Screen_పంక్తులు_C[i][wlv.off] = u8cc[i];
			ఒకవేళ (u8cc[i] == 0)
			    ఆపు;
		    }
		}
		లేకపోతే
		    Screen_పంక్తులు_UC[wlv.off] = 0;
	    }
	    ఒకవేళ (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    లేకపోతే
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    ఒకవేళ (wlv.draw_state > WL_NR
#నిర్వచితమైతే FEAT_DIFF
		    && wlv.filler_todo <= 0
#అయితే_ముగింపు
		    )
		Screen_నిలువుల_సంఖ్య[wlv.off] = wlv.vcol;
	    లేకపోతే
		Screen_నిలువుల_సంఖ్య[wlv.off] = -1;

	    ఒకవేళ (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.నిలువు;
		ఒకవేళ (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_పంక్తులు[wlv.off] = 0;
		లేకపోతే
		    // DBCS: Put second byte in the second screen char.
		    Screen_పంక్తులు[wlv.off] = mb_c & 0xff;

		ఒకవేళ (wlv.draw_state > WL_NR
#నిర్వచితమైతే FEAT_DIFF
			&& wlv.filler_todo <= 0
#అయితే_ముగింపు
			)
		    Screen_నిలువుల_సంఖ్య[wlv.off] = ++wlv.vcol;
		లేకపోతే
		    Screen_నిలువుల_సంఖ్య[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		ఒకవేళ (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#నిర్వచితమైతే FEAT_RIGHTLEFT
		ఒకవేళ (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.నిలువు;
		}
#అయితే_ముగింపు
	    }
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.నిలువు;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		++wlv.off;
		++wlv.నిలువు;
	    }
	}
#నిర్వచితమైతే FEAT_CONCEAL
	లేకపోతే ఒకవేళ (wp->w_p_cole > 0 && is_concealing)
	{
	    పూర్ణాంకం concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    ఒకవేళ (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    ఒకవేళ (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    ఒకవేళ (wp->w_p_wrap)
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
		పూర్ణాంకం adjust_నిలువు = !WIN_IS_POPUP(wp);
# లేకపోతే
		పూర్ణాంకం adjust_నిలువు = TRUE;
# endif
		ఒకవేళ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    ఒకవేళ (wp->w_p_rl)
		    {
			ఒకవేళ (adjust_నిలువు)
			{
			    wlv.నిలువు -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    లేకపోతే
# endif
		    {
			ఒకవేళ (adjust_నిలువు)
			{
			    wlv.నిలువు += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		ఒకవేళ (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    ఒకవేళ (wp->w_p_rl)
		    {
			ఒకవేళ (adjust_నిలువు)
			{
			    --wlv.boguscols;
			    --wlv.నిలువు;
			}
		    }
		    లేకపోతే
# endif
		    {
			ఒకవేళ (adjust_నిలువు)
			{
			    ++wlv.boguscols;
			    ++wlv.నిలువు;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		ఒకవేళ (wp->w_p_rl)
		{
		    ఒకవేళ (adjust_నిలువు)
		    {
			--wlv.boguscols;
			--wlv.నిలువు;
		    }
		}
		లేకపోతే
# endif
		{
		    ఒకవేళ (adjust_నిలువు)
		    {
			++wlv.boguscols;
			++wlv.నిలువు;
		    }
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#అయితే_ముగింపు // FEAT_CONCEAL
	లేకపోతే
	    --skip_cells;

	ఒకవేళ (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	ఒకవేళ (wlv.draw_state > WL_NR
#నిర్వచితమైతే FEAT_DIFF
		&& wlv.filler_todo <= 0
#అయితే_ముగింపు
		)
	    ++wlv.vcol;

#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#అయితే_ముగింపు

	// restore attributes after "precedes" in 'listchars'
	ఒకవేళ (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	ఒకవేళ (n_attr > 0 && wlv.draw_state == WL_పంక్తి
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	ఒకవేళ (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	ఒకవేళ ((
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.నిలువు < 0) :
#అయితే_ముగింపు
				    (wlv.నిలువు >= wp->w_వెడల్పు))
		&& (wlv.draw_state != WL_పంక్తి
		    || *ptr != NUL
#నిర్వచితమైతే FEAT_DIFF
		    || wlv.filler_todo > 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
		    || పాఠ్యం_prop_above || పాఠ్యం_prop_follows
		    || పాఠ్యం_prop_తదుపరి <= చివరి_textprop_పాఠ్యం_idx
#అయితే_ముగింపు
		    || (wp->w_p_list && wp->w_lcs_chars.eol != NUL
						&& lcs_eol_one != -1)
		    || (wlv.n_extra != 0 && (wlv.c_extra != NUL
						      || *wlv.p_extra != NUL)))
		)
	{
#నిర్వచితమైతే FEAT_CONCEAL
	    wlv.నిలువు -= wlv.boguscols;
	    // Apply 'cursorline' and HLF_WIN highlight.
	    ఒకవేళ (wlv.boguscols != 0 && (
# ifdef పంక్తి_ATTR
			wlv.పంక్తి_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		పూర్ణాంకం attr = wlv.win_attr;
# ifdef పంక్తి_ATTR
		ఒకవేళ (wlv.పంక్తి_attr != 0)
		    attr = hl_combine_attr(attr, wlv.పంక్తి_attr);
# endif
		అయితే ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.నిలువు >= 0 :
# endif
			    wlv.నిలువు < wp->w_వెడల్పు))
		{
		    Screen_పంక్తులు[wlv.off] = ' ';
		    ఒకవేళ (enc_utf8)
			Screen_పంక్తులు_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    Screen_నిలువుల_సంఖ్య[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    ఒకవేళ (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.నిలువు--;
			wlv.boguscols++;
		    }
		    లేకపోతే
# endif
		    {
			wlv.off++;
			wlv.నిలువు++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_పంక్తి(wp, &wlv, TRUE);
	    wlv.నిలువు += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#లేకపోతే
	    wlv_screen_పంక్తి(wp, &wlv, TRUE);
#అయితే_ముగింపు
	    ++wlv.అడ్డువరుస;
	    ++wlv.screen_అడ్డువరుస;

	    // When not wrapping and finished diff lines, break here.
	    ఒకవేళ (!wp->w_p_wrap
#నిర్వచితమైతే FEAT_DIFF
			&& wlv.filler_todo <= 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
			&& !పాఠ్యం_prop_above
			&& !పాఠ్యం_prop_follows
#అయితే_ముగింపు
		       )
		ఆపు;
#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (!wp->w_p_wrap && పాఠ్యం_prop_follows && !పాఠ్యం_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = పంక్తి + (size_t)ml_get_buf_len(wp->w_బఫరు, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#అయితే_ముగింపు

	    // When the window is too narrow draw all "@" lines.
	    ఒకవేళ (wlv.draw_state != WL_పంక్తి
#నిర్వచితమైతే FEAT_DIFF
		    && wlv.filler_todo <= 0
#అయితే_ముగింపు
		    )
	    {
		win_draw_చివర(wp, '@', ' ', TRUE, wlv.అడ్డువరుస, wp->w_ఎత్తు, HLF_AT);
		draw_vsep_win(wp, wlv.అడ్డువరుస);
		wlv.అడ్డువరుస = endrow;
	    }

	    // When line got too long for screen break here.
	    ఒకవేళ (wlv.అడ్డువరుస == endrow)
	    {
		++wlv.అడ్డువరుస;
		ఆపు;
	    }

	    ఒకవేళ (screen_cur_అడ్డువరుస == wlv.screen_అడ్డువరుస - 1
#నిర్వచితమైతే FEAT_DIFF
		     && wlv.filler_todo <= 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
		     && !పాఠ్యం_prop_above && !పాఠ్యం_prop_follows
#అయితే_ముగింపు
		     && wp->w_వెడల్పు == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		పంక్తి_Wraps[wlv.screen_అడ్డువరుస - 1] = TRUE;

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
		ఒకవేళ (p_tf
#నిర్వచితమైతే FEAT_GUI
			 && !gui.లోపల_use
#అయితే_ముగింపు
			 && !(has_mbyte
			     && ((*mb_off2cells)(పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస],
				   పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస - 1]
							    + (పూర్ణాంకం)topframe->fr_వెడల్పు - 2,
				     పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    ఒకవేళ (screen_cur_నిలువు != wp->w_వెడల్పు)
			screen_char(పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస - 1]
						       + (గుర్తులేని)topframe->fr_వెడల్పు - 1,
				       wlv.screen_అడ్డువరుస - 1, (పూర్ణాంకం)(topframe->fr_వెడల్పు - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    ఒకవేళ (has_mbyte && MB_BYTE2LEN(Screen_పంక్తులు[పంక్తి_స్థానభ్రంశం[
				     wlv.screen_అడ్డువరుస - 1] + (topframe->fr_వెడల్పు - 1)]) > 1)
			బహిర్గతం_char(' ');
		    లేకపోతే
			బహిర్గతం_char(Screen_పంక్తులు[పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస - 1]
							    + (topframe->fr_వెడల్పు - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[పంక్తి_స్థానభ్రంశం[wlv.screen_అడ్డువరుస]] = (sattr_T)-1;
		    screen_ప్రారంభం();	// don't know where cursor is now
		}
	    }

	    win_పంక్తి_ప్రారంభం(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#నిర్వచితమైతే FEAT_LINEBREAK
	    ఒకవేళ (!wlv.dont_use_showbreak
# ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
# endif
	       )
		wlv.need_showbreak = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
	    --wlv.filler_todo;
	    // When the filler lines are actually below the last line of the
	    // file, don't draw the line itself, break here.
	    ఒకవేళ (wlv.filler_todo == 0 && wp->w_botfill)
		ఆపు;
#అయితే_ముగింపు
	}

    }	// for every character in the line
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (పాఠ్యం_props != పాఠ్యం_props_buf)
	vim_free(పాఠ్యం_props);
    ఒకవేళ (పాఠ్యం_prop_idxs != పాఠ్యం_prop_idxs_buf)
	vim_free(పాఠ్యం_prop_idxs);
    ఒకవేళ (పాఠ్యం_prop_suffix_flags != పాఠ్యం_prop_suffix_flags_buf)
	vim_free(పాఠ్యం_prop_suffix_flags);
    vim_free(p_extra_free2);
#అయితే_ముగింపు

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    తిరిగి wlv.అడ్డువరుస;
}
