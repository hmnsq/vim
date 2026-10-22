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

#અન્તર્ભાવ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    સ્થિર પૂર્ણાંક
get_lcs_ext(win_T *wp)
{
    જો (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	પાછા NUL;
    જો (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	પાછા '>';
    પાછા wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ifdef FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    સ્થિર પૂર્ણાંક
advance_color_col(પૂર્ણાંક vcol, પૂર્ણાંક **color_cols)
{
    જ્યારે (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    પાછા (**color_cols >= 0);
}
#endif

#ifdef FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    સ્થિર શૂન્ય
margin_columns_win(win_T *wp, પૂર્ણાંક *ડાબું_col, પૂર્ણાંક *જમણું_col)
{
    // cache previous calculations depending on w_virtcol
    સ્થિર પૂર્ણાંક saved_w_virtcol;
    સ્થિર win_T *prev_wp;
    સ્થિર પૂર્ણાંક prev_width1;
    સ્થિર પૂર્ણાંક prev_width2;
    સ્થિર પૂર્ણાંક prev_ડાબું_col;
    સ્થિર પૂર્ણાંક prev_જમણું_col;

    પૂર્ણાંક cur_col_off = win_col_off(wp);
    પૂર્ણાંક	width1;
    પૂર્ણાંક	width2;

    width1 = wp->w_width - cur_col_off;
    width2 = width1 + win_col_off2(wp);

    જો (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*જમણું_col = prev_જમણું_col;
	*ડાબું_col = prev_ડાબું_col;
	પાછા;
    }

    *ડાબું_col = 0;
    *જમણું_col = width1;

    જો (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*જમણું_col = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    જો (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ડાબું_col = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ડાબું_col = *ડાબું_col;
    prev_જમણું_col = *જમણું_col;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#endif

#જો defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define લીટી_ATTR
#endif

// structure with variables passed between win_line() and other functions
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    પૂર્ણાંક		startrow;	// first row in the window to be drawn
    પૂર્ણાંક		row;		// row in the window, excl w_winrow
    પૂર્ણાંક		screen_row;	// row on the screen, incl w_winrow

    long	vcol;		// virtual column, before wrapping
    પૂર્ણાંક		col;		// visual column on screen, after wrapping
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    પૂર્ણાંક		vcol_off_co;	// offset for concealed characters
#endif
    પૂર્ણાંક		vcol_off_tp;	// offset for virtual text
#ifdef FEAT_SYN_HL
    પૂર્ણાંક		draw_color_col;	// highlight colorcolumn
    પૂર્ણાંક		*color_cols;	// pointer to according columns array
#endif
    પૂર્ણાંક		eol_hl_off;	// 1 if highlighted char after EOL

    unsigned	off;		// offset in ScreenLines/ScreenAttrs

    પૂર્ણાંક		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ifdef FEAT_SYN_HL
    પૂર્ણાંક		cul_attr;	// set when 'cursorline' active
#endif
#ifdef લીટી_ATTR
    પૂર્ણાંક		લીટી_attr;	// for the whole line, includes 'cursorline'
#endif

    પૂર્ણાંક		screen_લીટી_flags;  // flags for screen_line()

    પૂર્ણાંક		fromcol;	// start of inverting
    પૂર્ણાંક		tocol;		// end of inverting

#ifdef FEAT_LINEBREAK
    long	vcol_sbr;	    // virtual column after showbreak
    પૂર્ણાંક		need_showbreak;	    // overlong line, skipping first x chars
    પૂર્ણાંક		dont_use_showbreak; // do not use 'showbreak'
#endif
#ifdef FEAT_PROP_POPUP
    પૂર્ણાંક		text_prop_above_ગણતરી;
#endif

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    પૂર્ણાંક		cul_screenline;

    પૂર્ણાંક		char_attr;	// attributes for the next character

    પૂર્ણાંક		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    પૂર્ણાંક		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    પૂર્ણાંક		n_attr_skip;    // chars to skip before using extra_attr
    પૂર્ણાંક		c_extra;	// extra chars, all the same
    પૂર્ણાંક		c_final;	// final char, mandatory if set
    પૂર્ણાંક		extra_for_textprop; // n_extra set for textprop
    પૂર્ણાંક		start_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    પૂર્ણાંક		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    પૂર્ણાંક		saved_extra_attr;
    પૂર્ણાંક		saved_n_attr_skip;
    પૂર્ણાંક		saved_extra_for_textprop;
    પૂર્ણાંક		saved_c_extra;
    પૂર્ણાંક		saved_c_final;
    પૂર્ણાંક		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ifdef FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#endif
    પૂર્ણાંક		filler_લીટીઓ;	// nr of filler lines to be drawn
    પૂર્ણાંક		filler_todo;	// nr of filler lines still to do + 1
#ifdef FEAT_SIGNS
    sign_attrs_T sattr;
#endif
#ifdef FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    પૂર્ણાંક		need_lbr;
#endif
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#define WL_START	0		// nothing done yet, must be zero
#define WL_CMDLINE	(WL_START + 1)	// cmdline window column
#ifdef FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#નહીંતર
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#નહીંતર
# define WL_SIGN	WL_FOLD		// column for signs
#endif
#define WL_NR		(WL_SIGN + 1)	// line number
#ifdef FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#નહીંતર
# define WL_BRI		WL_NR
#endif
#જો defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#નહીંતર
# define WL_SBR		WL_BRI
#endif
#define WL_લીટી		(WL_SBR + 1)	// text in the line

#જો defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    સ્થિર પૂર્ણાંક
use_cursor_લીટી_highlight(win_T *wp, linenr_T lnum)
{
    પાછા wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#endif


#ifdef FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    સ્થિર શૂન્ય
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    પૂર્ણાંક fdc = compute_foldcolumn(wp, 0);

    જો (fdc <= 0)
	પાછા;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    જો (wlv->p_extra_free == ખાલી)
	પાછા;

    wlv->n_extra = (પૂર્ણાંક)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    જો (use_cursor_લીટી_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    નહીંતર
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
    સ્થિર શૂન્ય
get_sign_display_info(
	પૂર્ણાંક		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    પૂર્ણાંક	text_sign;
# ifdef FEAT_SIGN_ICONS
    પૂર્ણાંક	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    જો (nrcol)
	wlv->n_extra = સંખ્યા_width(wp) + 1;
    નહીંતર
    {
	જો (use_cursor_લીટી_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	નહીંતર
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    જો (wlv->row == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_લીટીઓ && wlv->filler_todo <= 0
# endif
       )
    {
	text_sign = (wlv->sattr.sat_text != ખાલી) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ખાલી) ? wlv->sattr.sat_typenr : 0;
	જો (gui.in_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    જો (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((અક્ષર *)wlv->extra, કદ(wlv->extra),
						"%-*c ", સંખ્યા_width(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    નહીંતર
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    જો (netbeans_active() && (buf_signcount(wp->w_buffer, wlv->lnum)
									  > 1))
	    {
		જો (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((અક્ષર *)wlv->extra, કદ(wlv->extra),
						"%-*c ", સંખ્યા_width(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		નહીંતર
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	નહીંતર
# endif
	    જો (text_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_text;
		જો (wlv->p_extra != ખાલી)
		{
		    જો (nrcol)
		    {
			પૂર્ણાંક width = સંખ્યા_width(wp) - 2;

			vim_memset(wlv->extra, ' ', width);
			wlv->n_extra = width;
			wlv->n_extra += vim_snprintf((અક્ષર *)wlv->extra + width,
				  કદ(wlv->extra) - width, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    નહીંતર
			wlv->n_extra = (પૂર્ણાંક)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		જો (use_cursor_લીટી_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		નહીંતર
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#endif

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    સ્થિર શૂન્ય
handle_lnum_col(
	win_T		*wp,
	winlinevars_T	*wlv,
	પૂર્ણાંક		sign_present UNUSED,
	પૂર્ણાંક		અંક_attr UNUSED)
{
    પૂર્ણાંક has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ખાલી;
    પૂર્ણાંક lnum_row = wlv->startrow + wlv->filler_લીટીઓ
#ifdef FEAT_PROP_POPUP
		      + wlv->text_prop_above_ગણતરી
#endif
		      ;

    જો ((wp->w_p_nu || wp->w_p_rnu)
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
	જો ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_text != ખાલી)
	    get_sign_display_info(TRUE, wp, wlv);
	નહીંતર
#endif
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  જો (wlv->row == lnum_row
		    && (wp->w_skipcol == 0 || wlv->row > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      long અંક;
	      અક્ષર *fmt = "%*ld ";

	      જો (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  અંક = (long)wlv->lnum;
	      નહીંતર
	      {
		  // 'relativenumber', don't use negative numbers
		  અંક = labs((long)get_cursor_rel_lnum(wp, wlv->lnum));
		  જો (અંક == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      અંક = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((અક્ષર *)wlv->extra, કદ(wlv->extra), fmt, સંખ્યા_width(wp), અંક);
	      જો (wp->w_skipcol > 0 && wlv->startrow == 0)
		  માટે (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ifdef FEAT_RIGHTLEFT
	      જો (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  પૂર્ણાંક	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  માટે (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
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
	  નહીંતર
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = સંખ્યા_width(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ifdef FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  જો (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->row == lnum_row
		      || (wlv->row > lnum_row
			 && (wp->w_p_culopt_flags & CULOPT_લીટી))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#endif
	  જો (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  જો (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ifdef FEAT_SIGNS
	જો (અંક_attr)
	    wlv->char_attr = અંક_attr;
#endif
    }
}

#ifdef FEAT_LINEBREAK
    સ્થિર શૂન્ય
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    જો (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_કિંમત(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    નહીંતર જો (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    જો (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	જો (wp->w_p_bri && (wlv->row > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_લીટીઓ
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    જો (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ખાલી;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_buffer, wlv->lnum, FALSE));
	    જો (wlv->row == wlv->startrow)
	    {
		wlv->n_extra -= win_col_off2(wp);
		જો (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    જો (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    જો (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	જો (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#endif

#જો defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    સ્થિર શૂન્ય
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    જો (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	જો (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	નહીંતર
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	જો (wp->w_p_rl)
	    wlv->n_extra = wlv->col + 1;
	નહીંતર
#  endif
	    wlv->n_extra = wp->w_width - wlv->col;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_કિંમત(wp);
    જો (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (પૂર્ણાંક)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	જો (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	જો (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	જો (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    જો (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#endif

#જો defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    સ્થિર પૂર્ણાંક
textprop_size_after_trunc(
	win_T	*wp,
	પૂર્ણાંક	flags,	    // TP_FLAG_ALIGN_*
	પૂર્ણાંક	added,
	પૂર્ણાંક	padding,
	char_u	*text,
	પૂર્ણાંક	*n_used_ptr)
{
    પૂર્ણાંક	space = (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_width - win_col_off(wp) : added;
    પૂર્ણાંક strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    જો (space < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_width;
    જો (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    માટે (p = text; *p != NUL; p += (*mb_ptr2len)(p))
    {
	પૂર્ણાંક clen = ptr2cells(p);

	જો (strsize + clen > space)
	    તોડો;
	strsize += clen;
    }
    *n_used_ptr = (પૂર્ણાંક)(p - text);

    પાછા strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    પૂર્ણાંક
text_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	પૂર્ણાંક	    vcol,	    // current text column
	પૂર્ણાંક	    scr_col,	    // current screen column
	પૂર્ણાંક	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	પૂર્ણાંક	    *n_attr,	    // attribute cells, NULL if not used
	પૂર્ણાંક	    *n_attr_skip,   // cells to skip attr, NULL if not used
	પૂર્ણાંક	    do_skip)	    // skip_cells is not zero
{
    પૂર્ણાંક	    જમણું = (tp->tp_flags & TP_FLAG_ALIGN_જમણું);
    પૂર્ણાંક	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    પૂર્ણાંક	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    પૂર્ણાંક	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    પૂર્ણાંક	    padding = tp->tp_col == MAXCOL ? tp->tp_padleft : 0;
    પૂર્ણાંક	    col_with_padding = scr_col + (below ? 0 : padding);
    પૂર્ણાંક	    room = wp->w_width - col_with_padding;
    પૂર્ણાંક	    before = room;	// spaces before the text
    પૂર્ણાંક	    after = 0;		// spaces after the text
    પૂર્ણાંક	    n_used = *n_extra;
    char_u  *l = ખાલી;
    પૂર્ણાંક	    strsize = vim_strsize(*p_extra);
    પૂર્ણાંક	    cells = wrap ? strsize : textprop_size_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_used);

    જો (wrap || જમણું || above || below || padding > 0 || n_used < *n_extra)
    {
	પૂર્ણાંક	    col_off = win_col_off(wp) - win_col_off2(wp);

	જો (above)
	{
	    before = 0;
	    after = wp->w_width - cells - win_col_off(wp) - padding;
	    જો (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	નહીંતર
	{
	    // Right-align: fill with before
	    જો (જમણું)
		before -= cells;

	    // Below-align: empty line add one character
	    જો (below && vcol == 0 && col_with_padding == col_off
					    && wp->w_width - col_off == before)
		col_with_padding += 1;

	    જો (before < 0
		    || !(જમણું || below)
		    || (below ? (col_with_padding <= col_off || !wp->w_p_wrap)
			      : (n_used < *n_extra)))
	    {
		જો (જમણું && (wrap
			      || (room < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_width - col_off - strsize + room;
		    જો (before < 0)
			before = 0;
		    નહીંતર
			n_used = *n_extra;
		}
		નહીંતર જો (below && before > vcol && do_skip)
		    before -= vcol;
		નહીંતર
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	જો (n_used < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_used;

	// add 1 for NUL, 2 for when '…' is used
	જો (n_attr != ખાલી)
	    l = alloc(n_used + before + after + (padding > 0 ? padding : 0) + 3);
	જો (n_attr == ખાલી || l != ખાલી)
	{
	    પૂર્ણાંક off = 0;

	    જો (n_attr != ખાલી)
	    {
		vim_memset(l, ' ', before);
		off += before;
		જો (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_used);
		off += n_used;
	    }
	    નહીંતર
	    {
		off = before + after + padding + n_used;
		cells += before + after + padding;
	    }
	    જો (n_attr != ખાલી)
	    {
		જો (n_used < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    જો (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			જો (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_setup(&vc, (char_u *)"utf-8", p_enc);
			    જો (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ખાલી);
				જો (cp == ખાલી)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_setup(&vc, ખાલી, ખાલી);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_head_off)(l, lp);
			STRCPY(lp, cp);
			n_used = lp - l + 3 - before - padding;
			જો (cp != buf)
			    vim_free(cp);
		    }
		    નહીંતર
			// change last character to '>'
			*lp = '>';
		}
		નહીંતર જો (after > 0)
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
		જો (above)
		    *n_attr -= after;
	    }
	}
    }

    જો (n_attr == ખાલી)
	પાછા cells;
    પાછા (below && col_with_padding > win_col_off(wp) && !wp->w_p_wrap);
}

# જો defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    પૂર્ણાંક
text_prop_no_showbreak(textprop_T *tp)
{
    પૂર્ણાંક	    જમણું = (tp->tp_flags & TP_FLAG_ALIGN_જમણું);
    પૂર્ણાંક	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    પૂર્ણાંક	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    પૂર્ણાંક	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    પાછા (જમણું || above || below || !wrap);
}
# endif
#endif

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    સ્થિર શૂન્ય
wlv_screen_લીટી(win_T *wp, winlinevars_T *wlv, પૂર્ણાંક clear_end)
{
    જો (wlv->row == 0 && wp->w_skipcol > 0
#જો defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_કિંમત(wp) == NUL
#endif
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	પૂર્ણાંક off = (પૂર્ણાંક)(current_Screen_લીટી - Screen_લીટીઓ);
	પૂર્ણાંક max_off = off + screen_Columns;
	પૂર્ણાંક skip = 0;

	જો (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    જ્યારે (skip < wp->w_width && VIM_ISDIGIT(Screen_લીટીઓ[off]))
	    {
		++off;
		++skip;
	    }

	માટે (પૂર્ણાંક i = 0; i < 3 && i + skip < wp->w_width; ++i)
	{
	    જો ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_લીટીઓ[off + 1] = ' ';
	    Screen_લીટીઓ[off] = '<';
	    જો (enc_utf8)
		Screen_લીટીઓ_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_લીટી(wp, wlv->screen_row, wp->w_wincol, wlv->col,
		    clear_end ? wp->w_width : -wp->w_width,
		    wlv->vcol - 1, wlv->screen_લીટી_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    સ્થિર શૂન્ય
draw_screen_લીટી(win_T *wp, winlinevars_T *wlv)
{
#ifdef FEAT_SYN_HL
    long	v;
    પૂર્ણાંક		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    જો (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    નહીંતર
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_width - wlv->col - 1 :
# endif
	wlv->col;
    // check if line ends before left margin
    જો (wlv->vcol < v + wcol - win_col_off(wp))
	wlv->vcol = v + wcol - win_col_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->col -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# નહીંતર
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    જો (wlv->draw_color_col)
	wlv->draw_color_col = advance_color_col(VCOL_HLC, &wlv->color_cols);

    જો (((wp->w_p_cuc
		    && (પૂર્ણાંક)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (પૂર્ણાંક)wp->w_virtcol <
			 (long)wp->w_width * (wlv->row - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_col
# ifdef લીટી_ATTR
		|| wlv->લીટી_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	પૂર્ણાંક	rightmost_vcol = 0;
	પૂર્ણાંક	i;

	જો (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	જો (wlv->draw_color_col)
	    // determine rightmost colorcolumn to possibly draw
	    માટે (i = 0; wlv->color_cols[i] >= 0; ++i)
		જો (rightmost_vcol < wlv->color_cols[i])
		    rightmost_vcol = wlv->color_cols[i];

	જ્યારે (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->col >= 0) :
# endif
		(wlv->col < wp->w_width))
	{
	    Screen_લીટીઓ[wlv->off] = ' ';
	    જો (enc_utf8)
		Screen_લીટીઓ_UC[wlv->off] = 0;

	    જો (wlv->draw_color_col)
		wlv->draw_color_col = advance_color_col(
						   VCOL_HLC, &wlv->color_cols);

	    પૂર્ણાંક attr = wlv->win_attr;
# ifdef લીટી_ATTR
	    જો (wlv->લીટી_attr != 0)
		attr = hl_combine_attr(attr, wlv->લીટી_attr);
# endif
	    જો (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    નહીંતર જો (wlv->draw_color_col && VCOL_HLC == *wlv->color_cols)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    ScreenCols[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->col;
	    }
	    નહીંતર
# endif
	    {
		++wlv->off;
		++wlv->col;
	    }
	    ++wlv->vcol;

	    જો (VCOL_HLC > rightmost_vcol
# ifdef લીટી_ATTR
		    && wlv->લીટી_attr == 0
# endif
		    && wlv->win_attr == 0)
		તોડો;
	}
    }
#endif

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_લીટી_flags |= SLF_INC_VCOL;
    wlv_screen_લીટી(wp, wlv, TRUE);
    wlv->screen_લીટી_flags &= ~SLF_INC_VCOL;
    ++wlv->row;
    ++wlv->screen_row;
}
#undef VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    સ્થિર શૂન્ય
win_લીટી_start(win_T *wp UNUSED, winlinevars_T *wlv, પૂર્ણાંક save_extra)
{
    wlv->col = 0;
    wlv->off = (unsigned)(current_Screen_લીટી - Screen_લીટીઓ);
#ifdef FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#endif

#ifdef FEAT_RIGHTLEFT
    જો (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->col = wp->w_width - 1;
	wlv->off += wlv->col;
	wlv->screen_લીટી_flags |= SLF_RIGHTLEFT;
    }
#endif
    જો (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_START;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ખાલી;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ifdef FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#endif
#ifdef FEAT_SYN_HL
	જો (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	નહીંતર
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
    સ્થિર શૂન્ય
win_લીટી_continue(winlinevars_T *wlv)
{
    જો (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ખાલી;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    નહીંતર
	wlv->char_attr = wlv->win_attr;
}

#ifdef FEAT_SYN_HL
    સ્થિર શૂન્ય
apply_cursorline_highlight(
	winlinevars_T *wlv,
	પૂર્ણાંક sign_present UNUSED)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    જો (sign_present && wlv->sattr.sat_linehl > 0)
    {
	જો (wlv->sattr.sat_priority >= 100)
	    wlv->લીટી_attr = hl_combine_attr(wlv->cul_attr, wlv->લીટી_attr);
	નહીંતર
	    wlv->લીટી_attr = hl_combine_attr(wlv->લીટી_attr, wlv->cul_attr);
    }
    નહીંતર
# endif
# જો defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->લીટી_attr = hl_combine_attr(wlv->cul_attr, wlv->લીટી_attr);
# નહીંતર
	wlv->લીટી_attr = wlv->cul_attr;
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
    પૂર્ણાંક
win_લીટી(
    win_T	*wp,
    linenr_T	lnum,
    પૂર્ણાંક		startrow,
    પૂર્ણાંક		endrow,
    પૂર્ણાંક		સંખ્યા_only,
    spellvars_T	*spv UNUSED)
{
    winlinevars_T	wlv;		// variables passed between functions

    પૂર્ણાંક		c = 0;			// init for GCC
    char_u	*લીટી;			// current line
    char_u	*ptr;			// current position in "line"
    પૂર્ણાંક		in_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ifdef FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ખાલી;   // another p_extra to be freed
#endif
#જો defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    પૂર્ણાંક		in_linebreak = FALSE;	// n_extra set for showing linebreak
#endif
    પૂર્ણાંક		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    પૂર્ણાંક		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    પૂર્ણાંક		n_attr = 0;	    // chars with special attr
    પૂર્ણાંક		saved_attr2 = 0;    // char_attr saved for n_attr
    પૂર્ણાંક		n_attr3 = 0;	    // chars with overruling special attr
    પૂર્ણાંક		saved_attr3 = 0;    // char_attr saved for n_attr3

    પૂર્ણાંક		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    પૂર્ણાંક		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    પૂર્ણાંક		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    પૂર્ણાંક		attr_pri = FALSE;	// char_attr has priority
    પૂર્ણાંક		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    પૂર્ણાંક		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    પૂર્ણાંક		area_attr = 0;		// attributes desired by highlighting
    પૂર્ણાંક		શોધ_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ifdef FEAT_SYN_HL
    પૂર્ણાંક		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    પૂર્ણાંક		syntax_attr = 0;	// attributes desired by syntax
    પૂર્ણાંક		prev_syntax_col = -1;	// column of prev_syntax_attr
    પૂર્ણાંક		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    પૂર્ણાંક		has_syntax = FALSE;	// this buffer has syntax highl.
    પૂર્ણાંક		save_did_emsg;
#endif
#ifdef FEAT_PROP_POPUP
# define WIN_લીટી_TEXT_PROP_ઢગલો_LEN 32
    પૂર્ણાંક		did_લીટી = FALSE;	// set to TRUE when line text done
    પૂર્ણાંક		text_prop_ગણતરી;
    પૂર્ણાંક		last_textprop_text_idx = -1;
    પૂર્ણાંક		text_prop_next = 0;	// next text property to use
    textprop_T	text_props_buf[WIN_લીટી_TEXT_PROP_ઢગલો_LEN];
    પૂર્ણાંક		text_prop_idxs_buf[WIN_લીટી_TEXT_PROP_ઢગલો_LEN];
    char_u	text_prop_suffix_flags_buf[WIN_લીટી_TEXT_PROP_ઢગલો_LEN + 1];
    textprop_T	*text_props = ખાલી;
    પૂર્ણાંક		*text_prop_idxs = ખાલી;
    char_u	*text_prop_suffix_flags = ખાલી;
    પૂર્ણાંક		text_props_active = 0;
    પૂર્ણાંક		text_props_need_sort = FALSE;
    proptype_T  *text_prop_type = ખાલી;
    પૂર્ણાંક		text_prop_attr = 0;
    પૂર્ણાંક		text_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    પૂર્ણાંક		text_prop_id = 0;	// active property ID
    પૂર્ણાંક		text_prop_flags = 0;
    પૂર્ણાંક		text_prop_above = FALSE;  // first doing virtual text above
    પૂર્ણાંક		text_prop_follows = FALSE;  // another text prop to display
    પૂર્ણાંક		saved_શોધ_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    પૂર્ણાંક		saved_area_attr = 0;	// idem for area_attr
    પૂર્ણાંક		reset_extra_attr = FALSE;
#endif
#ifdef FEAT_SPELL
    પૂર્ણાંક		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    પૂર્ણાંક		nextlinecol = 0;	// column where nextline[] starts
    પૂર્ણાંક		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    પૂર્ણાંક		spell_attr = 0;		// attributes desired by spelling
    પૂર્ણાંક		word_end = 0;		// last byte with same spell_attr
    પૂર્ણાંક		cur_checked_col = 0;	// checked column for current line
#endif
    પૂર્ણાંક		extra_check = FALSE;	// has extra highlighting
    પૂર્ણાંક		multi_attr = 0;		// attributes desired by multibyte
    પૂર્ણાંક		mb_l = 1;		// multi-byte byte length
    પૂર્ણાંક		mb_c = 0;		// decoded multi-byte character
    પૂર્ણાંક		mb_utf8 = FALSE;	// screen char is UTF-8 char
    પૂર્ણાંક		u8cc[MAX_MCO];		// composing UTF-8 chars
#ifdef FEAT_DIFF
    પૂર્ણાંક		change_start = MAXCOL;	// first col of changed area
    પૂર્ણાંક		change_end = -1;	// last col of changed area
    diffline_T	લીટી_changes;
    પૂર્ણાંક		change_index = -1;
#endif
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    પૂર્ણાંક		in_multispace = FALSE;	// in multiple consecutive spaces
    પૂર્ણાંક		multispace_pos = 0;	// position in lcs-multispace string
#ifdef લીટી_ATTR
    પૂર્ણાંક		લીટી_attr_save = 0;
#endif
    પૂર્ણાંક		sign_present = FALSE;
    પૂર્ણાંક		અંક_attr = 0;		// attribute for the number column
#ifdef FEAT_ARABIC
    પૂર્ણાંક		prev_c = 0;		// previous Arabic character
    પૂર્ણાંક		prev_c1 = 0;		// first composing char for prev_c
#endif
#જો defined(લીટી_ATTR)
    પૂર્ણાંક		did_લીટી_attr = 0;
#endif
#ifdef FEAT_TERMINAL
    પૂર્ણાંક		get_term_attr = FALSE;
#endif

#જો defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    પૂર્ણાંક		ડાબું_curline_col = 0;
    પૂર્ણાંક		જમણું_curline_col = 0;
#endif

#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    પૂર્ણાંક		feedback_col = 0;
    પૂર્ણાંક		feedback_old_attr = -1;
#endif

#જો defined(FEAT_CONCEAL) || defined(FEAT_શોધ_EXTRA)
    પૂર્ણાંક		match_conc	= 0;	// cchar for match functions
#endif
#જો defined(FEAT_CONCEAL) || defined(FEAT_શોધ_EXTRA) || defined(FEAT_LINEBREAK)
    પૂર્ણાંક		on_last_col     = FALSE;
#endif
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		syntax_flags	= 0;
    પૂર્ણાંક		syntax_seqnr	= 0;
    પૂર્ણાંક		prev_syntax_id	= 0;
    પૂર્ણાંક		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    પૂર્ણાંક		is_concealing	= FALSE;
    પૂર્ણાંક		did_wcol	= FALSE;
    પૂર્ણાંક		old_boguscols   = 0;
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
#નહીંતર
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#endif

    જો (startrow > endrow)		// past the end already!
	પાછા startrow;

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

    જો (સંખ્યા_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ifdef FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#endif
#ifdef FEAT_SYN_HL
	જો (syntax_present(wp) && !wp->w_s->b_syn_error
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
	    જો (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    નહીંતર
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		જો (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_cols = wp->w_p_cc_cols;
	જો (wlv.color_cols != ખાલી)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);
#endif

#ifdef FEAT_TERMINAL
	જો (term_show_buffer(wp->w_buffer))
	{
	    extra_check = TRUE;
	    get_term_attr = TRUE;
	    wlv.win_attr = term_get_attr(wp, lnum, -1);
	}
#endif

	// handle Visual active in this window
	જો (VIsual_active && wp->w_buffer == curwin->w_buffer)
	{
	    pos_T	*ટોચ, *bot;

	    જો (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		ટોચ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    નહીંતર
	    {
		// Visual is before curwin->w_cursor
		ટોચ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_in_visual_area = (lnum >= ટોચ->lnum && lnum <= bot->lnum);
	    જો (VIsual_mode == Ctrl_V)
	    {
		// block mode
		જો (lnum_in_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    નહીંતર
	    {
		// non-block mode
		જો (lnum > ટોચ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		નહીંતર જો (lnum == ટોચ->lnum)
		{
		    જો (VIsual_mode == 'V')	// linewise
			wlv.fromcol = 0;
		    નહીંતર
		    {
			getvvcol(wp, ટોચ, &wlv.fromcol, ખાલી, ખાલી, 0);
			જો (gchar_pos(ટોચ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		જો (VIsual_mode != 'V' && lnum == bot->lnum)
		{
		    જો (*p_sel == 'e' && bot->col == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    નહીંતર જો (bot->col == MAXCOL)
			wlv.tocol = MAXCOL;
		    નહીંતર
		    {
			pos = *bot;
			જો (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ખાલી, ખાલી, 0);
			નહીંતર
			{
			    getvvcol(wp, &pos, ખાલી, ખાલી, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    જો (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#જો defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		જો ((clip_star.available && !clip_star.owned
						    && clip_isautosel_star())
			|| (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#endif
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	નહીંતર જો (highlight_match
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + શોધ_match_લીટીઓ)
	{
	    જો (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ખાલી, ખાલી, 0);
	    નહીંતર
		wlv.fromcol = 0;
	    જો (lnum == curwin->w_cursor.lnum + શોધ_match_લીટીઓ)
	    {
		pos.lnum = lnum;
		pos.col = શોધ_match_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ખાલી, ખાલી, 0);
	    }
	    નહીંતર
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    જો (wlv.fromcol == wlv.tocol && શોધ_match_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ifdef FEAT_DIFF

    પૂર્ણાંક linestatus = 0;
    wlv.filler_લીટીઓ = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_FIELD(લીટી_changes);

    જો (linestatus < 0)
    {
	જો (linestatus == -1)
	{
	    જો (diff_find_change(wp, lnum, &લીટી_changes))
	    {
		wlv.diff_hlf = HLF_ADD;	// added line
	    }
	    નહીંતર જો (લીટી_changes.અંક_changes > 0)
	    {
		પૂર્ણાંક added = diff_change_parse(
			&લીટી_changes, &લીટી_changes.changes[0],
			&change_start, &change_end);
		જો (change_start == 0)
		{
		    જો (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    નહીંતર
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		નહીંતર
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_index = 0;
	    }
	    નહીંતર
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_index = 0;
	    }
	}
	નહીંતર
	    wlv.diff_hlf = HLF_ADD;

	area_highlighting = TRUE;
    }

    જો (lnum == wp->w_topline)
	wlv.filler_લીટીઓ = wp->w_topfill;

    wlv.filler_todo = wlv.filler_લીટીઓ;
#endif

#ifdef FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    જો (sign_present)
	અંક_attr = wlv.sattr.sat_numhl;
#endif

#ifdef લીટી_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    જો (sign_present)
	wlv.લીટી_attr = wlv.sattr.sat_linehl;
# endif
# જો defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    જો (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	wlv.લીટી_attr = HL_ATTR(HLF_QFL);
# endif
    જો (wlv.લીટી_attr != 0)
	area_highlighting = TRUE;
#endif

#ifdef FEAT_SPELL
    જો (spv->spv_has_spell && સંખ્યા_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	જો (lnum == spv->spv_checked_lnum)
	    cur_checked_col = spv->spv_checked_col;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	જો (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_col = 0;
	નહીંતર જો (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_col = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	જો (lnum < wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	{
	    લીટી = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_લીટી(nextline + SPWORDLEN, લીટી, SPWORDLEN);
	}
	લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(લીટી);
	જો (*ptr == NUL)
	{
	    spv->spv_cap_col = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	નહીંતર જો (spv->spv_cap_col == 0)
	    spv->spv_cap_col = ptr - લીટી;

	// Copy the end of the current line into nextline[].
	જો (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	નહીંતર
	{
	    v = ml_get_buf_len(wp->w_buffer, lnum);
	    જો (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, લીટી, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    નહીંતર
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, લીટી + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = લીટી;

    જો (wp->w_p_list)
    {
	જો (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ખાલી
		|| wp->w_lcs_chars.leadmultispace != ખાલી
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	જો (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_buffer, lnum);
	    જ્યારે (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - લીટી);
	}
	// find end of leading whitespace
	જો (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ખાલી ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    જ્યારે (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    જો (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    નહીંતર
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - લીટી) + 1;
	}
    }

    જો (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    પૂર્ણાંક skipcol_in_text_prop_above = 0;

#ifdef FEAT_PROP_POPUP
    જો (WIN_IS_POPUP(wp))
	wlv.screen_લીટી_flags |= SLF_POPUP;

    char_u *prop_start;
    text_prop_ગણતરી = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
    જો (text_prop_ગણતરી > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	જો (text_prop_ગણતરી <= WIN_લીટી_TEXT_PROP_ઢગલો_LEN)
	    text_props = text_props_buf;
	નહીંતર
	    text_props = ALLOC_MULT(textprop_T, text_prop_ગણતરી);
	જો (text_props != ખાલી)
	{
	    mch_memmove(text_props, prop_start,
				     text_prop_ગણતરી * કદ(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *ગણતરી_ptr = prop_start - PROP_ગણતરી_SIZE;

	    માટે (પૂર્ણાંક i = 0; i < text_prop_ગણતરી; ++i)
	    {
		જો (text_props[i].tp_id < 0
				&& text_props[i].u.tp_text_offset > 0)
		{
		    text_props[i].u.tp_text =
			    ગણતરી_ptr + text_props[i].u.tp_text_offset;
		    text_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		નહીંતર
		    text_props[i].u.tp_text = ખાલી;
	    }
	}

	// Allocate an array for the indexes.
	જો (text_prop_ગણતરી <= WIN_લીટી_TEXT_PROP_ઢગલો_LEN)
	    text_prop_idxs = text_prop_idxs_buf;
	નહીંતર
	    text_prop_idxs = ALLOC_MULT(પૂર્ણાંક, text_prop_ગણતરી);
	જો (text_prop_idxs == ખાલી)
	{
	    જો (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    નહીંતર
		text_props = ખાલી;
	}
	જો (text_prop_ગણતરી <= WIN_લીટી_TEXT_PROP_ઢગલો_LEN)
	    text_prop_suffix_flags = text_prop_suffix_flags_buf;
	નહીંતર
	    text_prop_suffix_flags = ALLOC_MULT(char_u, text_prop_ગણતરી + 1);
	જો (text_prop_suffix_flags == ખાલી)
	{
	    જો (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    text_prop_idxs = ખાલી;
	    જો (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    નહીંતર
		text_props = ખાલી;
	}

	જો (text_props != ખાલી)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    માટે (પૂર્ણાંક i = 0; i < text_prop_ગણતરી; ++i)
		જો (text_props[i].tp_id < 0)
		    last_textprop_text_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    પૂર્ણાંક text_width = wp->w_width - win_col_off(wp);
	    માટે (પૂર્ણાંક i = text_prop_ગણતરી - 1; i >= 0; --i)
		જો (text_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    જો (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_in_text_prop_above
								 >= text_width)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_in_text_prop_above += text_width;
			માટે (પૂર્ણાંક j = i + 1; j < text_prop_ગણતરી; ++j)
			    text_props[j - 1] = text_props[j];
			++i;
			--text_prop_ગણતરી;
		    }
		    નહીંતર
			++wlv.text_prop_above_ગણતરી;
		}

	    text_prop_suffix_flags[text_prop_ગણતરી] = 0;
	    માટે (પૂર્ણાંક i = text_prop_ગણતરી - 1; i >= 0; --i)
	    {
		પૂર્ણાંક flags = text_prop_suffix_flags[i + 1];

		જો (text_props[i].tp_col == MAXCOL)
		{
		    flags |= 1;
		    જો (text_props[i].tp_flags & TP_FLAG_ALIGN_BELOW)
			flags |= 2;
		}
		text_prop_suffix_flags[i] = flags;
	    }
	}
    }

    જો (સંખ્યા_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.row += wlv.text_prop_above_ગણતરી;
	જો (wlv.row >= endrow)
	{
	    જો (text_props != text_props_buf)
		vim_free(text_props);
	    જો (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    જો (text_prop_suffix_flags != text_prop_suffix_flags_buf)
		vim_free(text_prop_suffix_flags);
	    પાછા wlv.row;
	}
	wlv.screen_row += wlv.text_prop_above_ગણતરી;
    }
#endif

#જો defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_first_char = 0;
    જો (wp->w_p_lbr && સંખ્યા_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, લીટી, લીટી);
	(શૂન્ય)win_lbr_chartabsize(&cts, ખાલી, ખાલી);
	vcol_first_char = cts.cts_first_char;
	clear_chartabsize_arg(&cts);
    }
#endif

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    જો (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_in_text_prop_above : 0;
    નહીંતર
	v = wp->w_leftcol;
    જો (v > 0 && સંખ્યા_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	પૂર્ણાંક		charsize = 0;
	પૂર્ણાંક		head = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, લીટી, ptr);
	cts.cts_max_head_vcol = v;
	જ્યારે (cts.cts_vcol < v)
	{
	    head = 0;
	    charsize = win_lbr_chartabsize(&cts, &head, ખાલી);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    જો (*prev_ptr == NUL)
		તોડો;
	    MB_PTR_ADV(cts.cts_ptr);
	    જો (wp->w_p_list)
	    {
		in_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > લીટી && prev_ptr[-1] == ' '));
		જો (!in_multispace)
		    multispace_pos = 0;
		નહીંતર જો (cts.cts_ptr >= લીટી + leadcol
					 && wp->w_lcs_chars.multispace != ખાલી)
		{
		    ++multispace_pos;
		    જો (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		નહીંતર જો (cts.cts_ptr < લીટી + leadcol
				     && wp->w_lcs_chars.leadmultispace != ખાલી)
		{
		    ++multispace_pos;
		    જો (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	જો (wlv.vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_col ||
#endif
	     virtual_active() ||
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	જો (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	જો (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - head;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	જો (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	નહીંતર જો (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ifdef FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	જો (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	જો (spv->spv_has_spell)
	{
	    પૂર્ણાંક		len;
	    colnr_T	linecol = (colnr_T)(ptr - લીટી);
	    hlf_T	spell_hlf = HLF_ગણતરી;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = લીટી + linecol;

	    જો (len == 0 || (પૂર્ણાંક)wp->w_cursor.col > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		// word
		spell_hlf = HLF_ગણતરી;
		word_end = (પૂર્ણાંક)(spell_to_word_end(ptr, wp) - લીટી + 1);
	    }
	    નહીંતર
	    {
		// bad word found, use attributes until end of word
		word_end = wp->w_cursor.col + len + 1;

		// Turn index into actual attributes.
		જો (spell_hlf != HLF_ગણતરી)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    જો (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    જો (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ifdef FEAT_શોધ_EXTRA
    જો (સંખ્યા_only == 0)
    {
	v = (long)(ptr - લીટી);
	area_highlighting |= prepare_શોધ_hl_લીટી(wp, lnum, (colnr_T)v,
					      &લીટી, &screen_શોધ_hl,
					      &શોધ_attr);
	ptr = લીટી + v; // "line" may have been updated
    }
#endif

    જો ((State & MODE_ઉમેરો) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
	area_highlighting = TRUE;

#ifdef FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    જો (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	જો (!(wp == curwin && VIsual_active)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    જો (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    નહીંતર
	    {
		લીટી_attr_save = wlv.લીટી_attr;
		margin_columns_win(wp, &ડાબું_curline_col, &જમણું_curline_col);
	    }
	    area_highlighting = TRUE;
	}
    }
#endif

    win_લીટી_start(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    માટે (;;)
    {
#જો defined(FEAT_CONCEAL) || defined(FEAT_શોધ_EXTRA)
	પૂર્ણાંક	has_match_conc = 0;	// match wants to conceal
#endif
#ifdef FEAT_CONCEAL
	પૂર્ણાંક	did_decrement_ptr = FALSE;
#endif

	// Skip this quickly when working on the text.
	જો (wlv.draw_state != WL_લીટી)
	{
#ifdef FEAT_SYN_HL
	    જો (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.લીટી_attr = લીટી_attr_save;
	    }
#endif
	    જો (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		જો (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    જો (wlv.row == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    નહીંતર
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ifdef FEAT_FOLDING
	    જો (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#endif
#ifdef FEAT_SIGNS
	    જો (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		જો (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#endif
	    જો (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_col(wp, &wlv, sign_present, અંક_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    જો (સંખ્યા_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_લીટી(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		જો ((wlv.row + 1 - wlv.startrow < સંખ્યા_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ifdef FEAT_DIFF
			|| wlv.filler_todo > 0
#endif
			)
		{
		    ++wlv.row;
		    ++wlv.screen_row;
		    જો (wlv.row == endrow)
			તોડો;
#ifdef FEAT_DIFF
		    --wlv.filler_todo;
		    જો (wlv.filler_todo == 0 && wp->w_botfill)
			તોડો;
#endif
		    win_લીટી_start(wp, &wlv, TRUE);
		    ચાલુ;
		}
		નહીંતર
		    તોડો;
	    }

#ifdef FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    જો (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#endif
#જો defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    જો (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#endif
	    જો (wlv.draw_state == WL_લીટી - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_લીટી;
		win_લીટી_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ifdef FEAT_SYN_HL
	જો (wlv.cul_screenline && wlv.draw_state == WL_લીટી
		&& wlv.vcol >= ડાબું_curline_col
		&& wlv.vcol < જમણું_curline_col)
	    apply_cursorline_highlight(&wlv, sign_present);
#endif

	// When still displaying '$' of change command, stop at cursor.
	જો (dollar_vcol >= 0 && in_curline && wlv.vcol >= (long)wp->w_virtcol)
	{
	    wlv_screen_લીટી(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ifdef FEAT_SYN_HL
	    જો (wp->w_p_cuc)
		wlv.row = wp->w_cline_row + wp->w_cline_height;
	    નહીંતર
#endif
		wlv.row = wp->w_height;
	    તોડો;
	}

	જો (wlv.draw_state == WL_લીટી && (area_highlighting || extra_check))
	{
#ifdef FEAT_PROP_POPUP
	    જો (text_props != ખાલી)
	    {
		પૂર્ણાંક pi;
		પૂર્ણાંક bcol = (પૂર્ણાંક)(ptr - લીટી);

		જો (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !in_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		માટે (pi = 0; pi < text_props_active; ++pi)
		{
		    પૂર્ણાંક		tpi = text_prop_idxs[pi];
		    textprop_T  *tp = &text_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    જો ((tp->tp_col != MAXCOL
				       && bcol >= tp->tp_col - 1 + tp->tp_len))
		    {
			જો (pi + 1 < text_props_active)
			    mch_memmove(text_prop_idxs + pi,
					text_prop_idxs + pi + 1,
					કદ(પૂર્ણાંક)
					     * (text_props_active - (pi + 1)));
			--text_props_active;
			text_props_need_sort = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			જો (in_linebreak && syntax_attr == text_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		જો (wlv.n_extra > 0 && in_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		જ્યારે (text_prop_next < text_prop_ગણતરી)
		{
		    પૂર્ણાંક active;
		    textprop_T *tp = &text_props[text_prop_next];
		    જો (tp->tp_col == MAXCOL)
		    {
			જો (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    active = TRUE;
			નહીંતર જો (*ptr != NUL)
			    તોડો;
			નહીંતર
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    active = wp->w_p_wrap
				  || wlv.row == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
			}
		    }
		    નહીંતર
		    {
			જો (bcol < tp->tp_col - 1)
			    તોડો;
			active = bcol <= tp->tp_col - 1 + tp->tp_len;
		    }

		    જો (active)
		    {
			text_prop_idxs[text_props_active++] = text_prop_next;
			text_props_need_sort = TRUE;
		    }
		    ++text_prop_next;
		}

		જો (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(text_prop_type != ખાલી &&
			     text_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    text_prop_attr = 0;
		    text_prop_attr_comb = 0;
		    text_prop_flags = 0;
		    text_prop_type = ખાલી;
		    text_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		જો (text_props_active > 0 && wlv.n_extra == 0)
		{
		    પૂર્ણાંક used_tpi = -1;
		    પૂર્ણાંક used_attr = 0;
		    પૂર્ણાંક other_tpi = -1;

		    text_prop_above = FALSE;
		    text_prop_follows = FALSE;

		    જો (text_props_need_sort)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			sort_text_props(wp->w_buffer, text_props,
					text_prop_idxs, text_props_active);
			text_props_need_sort = FALSE;
		    }

		    માટે (pi = 0; pi < text_props_active; ++pi)
		    {
			પૂર્ણાંક	    tpi = text_prop_idxs[pi];
			textprop_T  *tp = &text_props[tpi];
			proptype_T  *pt = text_prop_type_by_id(
						    wp->w_buffer, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			જો (pt != ખાલી
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_જમણું
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_BELOW)) == 0
				    && wlv.col >= wp->w_width))
			{
			    જો (tp->tp_col == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == લીટી
						&& !did_લીટી
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_BELOW))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				text_prop_follows = TRUE;
				ચાલુ;
			    }

			    જો (pt->pt_hl_id > 0)
				used_attr = syn_id2attr(pt->pt_hl_id);
			    text_prop_type = pt;
			    text_prop_attr =
				   hl_combine_attr(text_prop_attr, used_attr);
			    જો (used_tpi >= 0 && text_props[used_tpi].tp_id < 0)
				other_tpi = used_tpi;
			    text_prop_flags = pt->pt_flags;
			    text_prop_id = tp->tp_id;
			    used_tpi = tpi;
			}
		    }
		    જો (text_prop_id < 0 && used_tpi >= 0
				 && text_props[used_tpi].u.tp_text != ખાલી)
		    {
			textprop_T  *tp = &text_props[used_tpi];
			char_u	    *p = tp->u.tp_text;
			પૂર્ણાંક	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			પૂર્ણાંક	    bail_out = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			જો (p != ખાલી)
			{
			    પૂર્ણાંક	    જમણું = (tp->tp_flags
							& TP_FLAG_ALIGN_જમણું);
			    પૂર્ણાંક	    below = (tp->tp_flags
							& TP_FLAG_ALIGN_BELOW);
			    પૂર્ણાંક	    wrap = tp->tp_col < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    પૂર્ણાંક	    padding = tp->tp_col == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (પૂર્ણાંક)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.start_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    used_attr);
			    n_attr = mb_charlen(p);
			    text_prop_attr = 0;
			    text_prop_attr_comb = 0;
			    જો (*ptr == NUL)
				// don't combine char attr after EOL
				text_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    જો (text_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    જો ((જમણું || above || below || !wrap
					    || padding > 0) && wp->w_width > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				પૂર્ણાંક	start_લીટી;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				start_લીટી = text_prop_position(wp, tp,
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
				જો (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				જો (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				જો (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.col
					       + wlv.n_extra - 2 > wp->w_width)
				    // don't bail out at end of line
				    text_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				જો (start_લીટી)
				{
				    draw_screen_લીટી(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    જો (wlv.row == endrow)
				    {
					++wlv.row;
					તોડો;
				    }
				    win_લીટી_start(wp, &wlv, TRUE);
				    bail_out = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			જો (skip_cells > 0)
			{
			    જો (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				જો (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    નહીંતર
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
			       & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_જમણું)));

			જો (bail_out)
			    // starting a new line for "below"
			    ચાલુ;
		    }
		}
		નહીંતર જો (text_prop_next < text_prop_ગણતરી
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.col == wp->w_width - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    પૂર્ણાંક only_below_follows = !wp->w_p_wrap
				 && wlv.col + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_width;
		    પૂર્ણાંક suffix_flags = text_prop_suffix_flags[text_prop_next];

		    text_prop_follows = (suffix_flags
					& (only_below_follows ? 2 : 1)) != 0;
		}
	    }

	    જો (wlv.start_extra_for_textprop)
	    {
		wlv.start_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_શોધ_attr = શોધ_attr;
		saved_area_attr = area_attr;
		શોધ_attr = 0;
		area_attr = 0;
	    }
#endif

	    પૂર્ણાંક *area_attr_p =
#ifdef FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#endif
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    જો (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ખાલી
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    નહીંતર જો (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    જો (wlv.n_extra == 0)
	    {
#ifdef FEAT_શોધ_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (long)(ptr - લીટી);
		શોધ_attr = update_શોધ_hl(wp, lnum, (colnr_T)v, &લીટી,
				      &screen_શોધ_hl, &has_match_conc,
				      &match_conc, did_લીટી_attr, lcs_eol_one,
				      &on_last_col);
		ptr = લીટી + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		જો (*ptr == NUL)
		    has_match_conc = 0;
#નહીંતર
		શોધ_attr = 0;
#endif

		// Check if ComplMatchIns highlight is needed.
		જો ((State & MODE_ઉમેરો) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
		{
		    પૂર્ણાંક ins_match_attr =
			ins_compl_col_range_attr(lnum, (પૂર્ણાંક)(ptr - લીટી));
		    જો (ins_match_attr > 0)
			શોધ_attr =
			    hl_combine_attr(શોધ_attr, ins_match_attr);
		}
	    }

#ifdef FEAT_DIFF
	    જો (wlv.diff_hlf != (hlf_T)0)
	    {
		જો (લીટી_changes.અંક_changes > 0
			&& change_index >= 0
			&& change_index < લીટી_changes.અંક_changes - 1)
		{
		    જો (ptr - લીટી >= લીટી_changes.changes[change_index+1].dc_start[લીટી_changes.bufidx])
			change_index += 1;
		}
		પૂર્ણાંક added = FALSE;
		જો (લીટી_changes.અંક_changes > 0 && change_index >= 0 && change_index < લીટી_changes.અંક_changes)
		{
		    added = diff_change_parse(&લીટી_changes,
			    &લીટી_changes.changes[change_index],
			    &change_start,
			    &change_end);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		જો (wlv.diff_hlf == HLF_CHD && ptr - લીટી >= change_start
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		જો ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - લીટી >= change_end && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.લીટી_attr = HL_ATTR(wlv.diff_hlf);
		જો (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ડાબું_curline_col
					    && wlv.vcol <= જમણું_curline_col)))
		    wlv.લીટી_attr = hl_combine_attr(
					  wlv.લીટી_attr, HL_ATTR(HLF_CUL));
	    }
#endif

#ifdef FEAT_SYN_HL
	    જો (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		જો (get_term_attr)
		    syntax_attr = term_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		જો (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (long)(ptr - લીટી);
		    જો (v == prev_syntax_col)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    નહીંતર
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ખાલી, FALSE);
			prev_syntax_col = v;
			prev_syntax_attr = syntax_attr;
		    }

		    જો (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    નહીંતર
			did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		    જો (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = લીટી + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    જો (*ptr == NUL)
			syntax_flags = 0;
		    નહીંતર
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    જો (text_prop_type != ખાલી)
	    {
		જો (text_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, text_prop_attr);
		નહીંતર
		    syntax_attr = text_prop_attr;
		text_prop_attr_comb = syntax_attr;
	    }
# endif
#endif

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ifdef લીટી_ATTR
	    જો (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.લીટી_attr, area_attr);
		જો (!highlight_match)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(શોધ_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    નહીંતર જો (શોધ_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.લીટી_attr, શોધ_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    નહીંતર જો (wlv.લીટી_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.લીટી_attr);
# નહીંતર
		wlv.char_attr = wlv.લીટી_attr;
# endif
		attr_pri = FALSE;
	    }
#નહીંતર
	    જો (area_attr != 0)
		wlv.char_attr = area_attr;
	    નહીંતર જો (શોધ_attr != 0)
		wlv.char_attr = શોધ_attr;
#endif
	    નહીંતર
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#નહીંતર
		wlv.char_attr = 0;
#endif
	    }
#ifdef FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    જો (text_prop_type != ખાલી && (text_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, text_prop_attr);
#endif
	}

	// combine attribute with HLF_WIN
	જો (wlv.win_attr != 0)
	{
	    જો (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    નહીંતર
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
	જો (wlv.n_extra > 0)
	{
	    જો (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		જો (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		નહીંતર
		    mb_utf8 = FALSE;
	    }
	    નહીંતર
	    {
		c = *wlv.p_extra;
		જો (has_mbyte)
		{
		    mb_c = c;
		    જો (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			જો (mb_l > wlv.n_extra)
			    mb_l = 1;
			નહીંતર જો (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    નહીંતર
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			જો (mb_l >= wlv.n_extra)
			    mb_l = 1;
			નહીંતર જો (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    જો (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    જો ((
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
			જો (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#endif
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    નહીંતર
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#જો defined(FEAT_PROP_POPUP)
	    જો (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		જો (wlv.saved_n_extra <= 0)
		{
		    જો (શોધ_attr == 0)
			શોધ_attr = saved_શોધ_attr;
		    જો (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    જો (wlv.extra_for_textprop)
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
	નહીંતર
	{
#ifdef FEAT_LINEBREAK
	    પૂર્ણાંક		c0;
#endif
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ifdef FEAT_LINEBREAK
	    c0 = *ptr;
#endif
	    જો (c == NUL)
	    {
#ifdef FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_લીટી = TRUE;
#endif
		// no more cells to skip
		skip_cells = 0;
	    }

	    જો (has_mbyte)
	    {
		mb_c = c;
		જો (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    જો (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			જો (mb_c < 0x80)
			{
			    c = mb_c;
#ifdef FEAT_LINEBREAK
			    c0 = mb_c;
#endif
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			જો (utf_iscomposing(mb_c))
			{
			    પૂર્ણાંક i;

			    માટે (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    જો ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ifdef FEAT_RIGHTLEFT
			જો (wp->w_p_rl)		// reverse
			    rl_mirror(wlv.extra);
#endif
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (પૂર્ણાંક)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			જો (area_attr == 0 && શોધ_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    નહીંતર જો (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ifdef FEAT_ARABIC
		    નહીંતર જો (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			પૂર્ણાંક	pc, pc1, nc;
			પૂર્ણાંક	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			જો (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			નહીંતર
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    નહીંતર
			prev_c = mb_c;
#endif
		}
		નહીંતર	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    જો (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    નહીંતર જો (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			જો (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			નહીંતર
			{
			    જો (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_buffer, wlv.extra, c);
				wlv.n_extra = (પૂર્ણાંક)STRLEN(wlv.extra) - 1;
			    }
			    નહીંતર
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
			    જો (area_attr == 0 && શોધ_attr == 0)
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
		જો ((
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
		નહીંતર જો (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		જો (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    જો (area_attr == 0 && શોધ_attr == 0)
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

	    જો (extra_check)
	    {
#ifdef FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (long)(ptr - લીટી);
		જો (spv->spv_has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    જો (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			પૂર્ણાંક	len;
			hlf_T	spell_hlf = HLF_ગણતરી;

			જો (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			જો ((prev_ptr - લીટી) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - લીટી) - nextlinecol;
			નહીંતર
			    p = prev_ptr;
			spv->spv_cap_col -= (પૂર્ણાંક)(prev_ptr - લીટી);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_col,
							    spv->spv_unchanged);
			word_end = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			જો (spell_hlf != HLF_ગણતરી
				&& (State & MODE_ઉમેરો)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - લીટી)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_ગણતરી;
			    spell_redraw_lnum = lnum;
			}

			જો (spell_hlf == HLF_ગણતરી && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_col = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			જો (spell_hlf != HLF_ગણતરી)
			    spell_attr = highlight_attr[spell_hlf];

			જો (spv->spv_cap_col > 0)
			{
			    જો (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_col
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_col = ((p - nextline)
					    + spv->spv_cap_col - nextline_idx);
			    }
			    નહીંતર
				// Compute the actual column.
				spv->spv_cap_col += (prev_ptr - લીટી);
			}
		    }
		}
		જો (spell_attr != 0)
		{
		    જો (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    નહીંતર
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
		જો ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((પૂર્ણાંક)*ptr))
		    wlv.need_lbr = TRUE;
#endif
#ifdef FEAT_LINEBREAK
		// Found last space before word: check for line break.
		જો (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((પૂર્ણાંક)*ptr))
		{
		    પૂર્ણાંક	    mb_off = has_mbyte ? (*mb_head_off)(લીટી, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_first_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, લીટી, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_text = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ખાલી, ખાલી) - 1;
		    clear_chartabsize_arg(&cts);

		    જો (on_last_col && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			શોધ_attr = 0;

		    જો (c == TAB && wlv.n_extra + wlv.col > wp->w_width)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
# નહીંતર
			wlv.n_extra = (પૂર્ણાંક)wp->w_buffer->b_p_ts
				    - wlv.vcol % (પૂર્ણાંક)wp->w_buffer->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    જો (wlv.n_extra > 0 && c != TAB)
			in_linebreak = TRUE;
# endif
		    જો (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			જો (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			જો (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif
		જો (wp->w_p_list)
		{
		    in_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > લીટી && prev_ptr[-1] == ' '));
		    જો (!in_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		જો (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (in_multispace
					&& wp->w_lcs_chars.multispace != ખાલી))
				&& ptr - લીટી >= leadcol
				&& ptr - લીટી <= trailcol)))
		{
		    જો (in_multispace && wp->w_lcs_chars.multispace != ખાલી)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			જો (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    નહીંતર
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    જો (area_attr == 0 && શોધ_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    જો (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    નહીંતર
			mb_utf8 = FALSE;
		}

		જો (c == ' ' && ((trailcol != MAXCOL && ptr > લીટી + trailcol)
				    || (leadcol != 0 && ptr < લીટી + leadcol)))
		{
		    જો (leadcol != 0 && in_multispace && ptr < લીટી + leadcol
			    && wp->w_lcs_chars.leadmultispace != ખાલી)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			જો (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    નહીંતર જો (ptr > લીટી + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    નહીંતર જો (ptr < લીટી + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    નહીંતર જો (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    જો (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    જો (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    નહીંતર
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    જો (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		જો (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    પૂર્ણાંક	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    long    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    પૂર્ણાંક	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    પૂર્ણાંક	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    પૂર્ણાંક	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    જો (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < લીટી + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ifdef FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_કિંમત(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    જો (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#endif
		    // tab amount depends on current column
#ifdef FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
#નહીંતર
		    tab_len = (પૂર્ણાંક)wp->w_buffer->b_p_ts
			       - vcol_adjusted % (પૂર્ણાંક)wp->w_buffer->b_p_ts - 1;
#endif

#ifdef FEAT_LINEBREAK
		    જો (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ifdef FEAT_LINEBREAK
		    નહીંતર
		    {
			char_u	*p;
			પૂર્ણાંક	len;
			પૂર્ણાંક	i;
			પૂર્ણાંક	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			જો (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			જો (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			જો (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    પૂર્ણાંક tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    જો (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    જો (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    જો (p == ખાલી)
				wlv.n_extra = 0;
			    નહીંતર
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				માટે (i = 0; i < tab_len; i++)
				{
				    પૂર્ણાંક lcs = lcs_tab2;

				    જો (*p == NUL)
				    {
					tab_len = i;
					તોડો;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    જો (lcs_tab3 && i == tab_len - 1)
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
				જો (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			પૂર્ણાંક vc_saved = wlv.vcol_off_co;

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
			જો (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#endif
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    જો (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ifdef FEAT_LINEBREAK
			જો (wp->w_p_lbr && wlv.p_extra != ખાલી
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			નહીંતર
#endif
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			જો (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    નહીંતર
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		નહીંતર જો (c == NUL
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
#જો defined(FEAT_DIFF) || defined(લીટી_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    જો (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef લીટી_ATTR
			    &&
#  endif
# endif
# ifdef લીટી_ATTR
			    wlv.લીટી_attr == 0
# endif
		       )
#endif
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			જો (!(area_highlighting && virtual_active()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    જો (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    નહીંતર
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    જો (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    જો (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    નહીંતર
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		નહીંતર જો (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_buffer, c);
		    જો (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    જો ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#endif
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ifdef FEAT_LINEBREAK
		    જો (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			જો (p == ખાલી)
			    wlv.n_extra = 0;
			નહીંતર
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    નહીંતર
#endif
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    જો (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ifdef FEAT_PROP_POPUP
			જો (text_prop_type != ખાલી &&
				 text_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(text_prop_attr,
							       wlv.extra_attr);
#endif

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		નહીંતર જો (VIsual_active
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
#જો defined(લીટી_ATTR)
		નહીંતર જો ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.લીટી_attr != 0
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
		    ++did_લીટી_attr;

		    // don't do search HL for the rest of the line
		    જો (wlv.લીટી_attr != 0 && wlv.char_attr == શોધ_attr
					&& (did_લીટી_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.લીટી_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    જો (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_લીટી
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    જો (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			જો (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    જો (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ડાબું_curline_col
					    && wlv.vcol <= જમણું_curline_col)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    જો (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			જો (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    જો (!wlv.cul_screenline
				    || (wlv.vcol >= ડાબું_curline_col
					     && wlv.vcol <= જમણું_curline_col))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			નહીંતર જો (wlv.લીટી_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.લીટી_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			જો (wlv.vcol == 0)
			{
			    જો (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    નહીંતર જો (શોધ_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, શોધ_attr);
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    જો (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_લીટી(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_match_conc > 0)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ખાલી))
	    {
		પૂર્ણાંક syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		જો (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_match_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_match_conc && match_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    જો (has_match_conc && match_conc)
			c = match_conc;
		    નહીંતર જો (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    નહીંતર જો (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    નહીંતર
			c = ' ';

		    જો (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    જો (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    નહીંતર
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    જો (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    જો (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			જો (wp->w_p_rl)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			નહીંતર
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.col += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		નહીંતર જો (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    નહીંતર
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    જો (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#endif // FEAT_CONCEAL
	}

#ifdef FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	જો (!did_wcol && wlv.draw_state == WL_લીટી
		&& in_curline && conceal_cursor_લીટી(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
		wp->w_wcol = wp->w_width - wlv.col + wlv.boguscols - 1;
	    નહીંતર
# endif
		wp->w_wcol = wlv.col - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    જો (wlv.vcol + skip_cells < wp->w_virtcol)
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
	જો (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_લીટી
		&& (!attr_pri
#ifdef FEAT_PROP_POPUP
		    || (text_prop_flags & PT_FLAG_OVERRIDE)
#endif
		   ))
	{
#ifdef લીટી_ATTR
	    જો (wlv.લીટી_attr)
		wlv.char_attr = hl_combine_attr(wlv.લીટી_attr, wlv.extra_attr);
	    નહીંતર
#endif
		wlv.char_attr = wlv.extra_attr;
#ifdef FEAT_PROP_POPUP
	    જો (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#endif
	}

#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	જો (p_imst == IM_ON_THE_SPOT
		&& xic != ખાલી
		&& lnum == wp->w_cursor.lnum
		&& (State & MODE_ઉમેરો)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_લીટી)
	{
	    colnr_T tcol;

	    જો (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ખાલી, ખાલી, 0);
	    નહીંતર
		tcol = preedit_end_col;
	    જો ((long)preedit_start_col <= wlv.vcol && wlv.vcol < (long)tcol)
	    {
		જો (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_col);
		જો (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    નહીંતર જો (feedback_old_attr >= 0)
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
	જો (lcs_prec_todo != NUL
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
	    જો (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    જો (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    નહીંતર
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    જો (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	જો ((c == NUL
#જો defined(લીટી_ATTR)
		|| did_લીટી_attr == 1
#endif
		) && wlv.eol_hl_off == 0)
	{
#ifdef FEAT_શોધ_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    પૂર્ણાંક prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_શોધ_hl,
					      (long)(ptr - લીટી) - (c == NUL));
#endif
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    જો (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_શોધ_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_active))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# જો defined(લીટી_ATTR)
			    && did_લીટી_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		પૂર્ણાંક n = 0;

#ifdef FEAT_RIGHTLEFT
		જો (wp->w_p_rl)
		{
		    જો (wlv.col < 0)
			n = 1;
		}
		નહીંતર
#endif
		{
		    જો (wlv.col >= wp->w_width)
			n = -1;
		}
		જો (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.col += n;
		}
		નહીંતર
		{
		    // Add a blank character to highlight.
		    Screen_લીટીઓ[wlv.off] = ' ';
		    જો (enc_utf8)
			Screen_લીટીઓ_UC[wlv.off] = 0;
		}
#ifdef FEAT_શોધ_EXTRA
		જો (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_શોધ_match_hl(wp, &screen_શોધ_hl,
					   (long)(ptr - લીટી), &wlv.char_attr);
		}
#endif
		ScreenAttrs[wlv.off] = wlv.char_attr;
		ScreenCols[wlv.off] = wlv.vcol;
#ifdef FEAT_RIGHTLEFT
		જો (wp->w_p_rl)
		{
		    --wlv.col;
		    --wlv.off;
		}
		નહીંતર
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
	જો (c == NUL)
	{
#ifdef FEAT_PROP_POPUP
	    જો (text_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    નહીંતર
#endif
	    {
		draw_screen_લીટી(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		જો (in_curline)
		{
		    curwin->w_cline_row = startrow;
		    curwin->w_cline_height = wlv.row - startrow;
#ifdef FEAT_FOLDING
		    curwin->w_cline_folded = false;
#endif
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		તોડો;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	પૂર્ણાંક lcs_ext = get_lcs_ext(wp);
	જો (lcs_ext != NUL
		&& wlv.draw_state == WL_લીટી
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
	    જો (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    નહીંતર
		mb_utf8 = FALSE;
	}

#ifdef FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	જો (wlv.draw_color_col)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	જો (((wlv.draw_state == WL_લીટી
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_in_visual_area
		&& શોધ_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    જો (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    નહીંતર જો (wlv.draw_color_col && VCOL_HLC == *wlv.color_cols)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#endif

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	જો (wlv.draw_state < WL_લીટી || skip_cells <= 0)
	{
	    // Store the character.
#જો defined(FEAT_RIGHTLEFT)
	    જો (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.col;
	    }
#endif
	    Screen_લીટીઓ[wlv.off] = c;
	    જો (enc_dbcs == DBCS_JPNU)
	    {
		જો ((mb_c & 0xff00) == 0x8e00)
		    Screen_લીટીઓ[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    નહીંતર જો (enc_utf8)
	    {
		જો (mb_utf8)
		{
		    પૂર્ણાંક i;

		    Screen_લીટીઓ_UC[wlv.off] = mb_c;
		    જો ((c & 0xff) == 0)
			Screen_લીટીઓ[wlv.off] = 0x80;   // avoid storing zero
		    માટે (i = 0; i < Screen_mco; ++i)
		    {
			Screen_લીટીઓ_C[i][wlv.off] = u8cc[i];
			જો (u8cc[i] == 0)
			    તોડો;
		    }
		}
		નહીંતર
		    Screen_લીટીઓ_UC[wlv.off] = 0;
	    }
	    જો (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    નહીંતર
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    જો (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
		ScreenCols[wlv.off] = wlv.vcol;
	    નહીંતર
		ScreenCols[wlv.off] = -1;

	    જો (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.col;
		જો (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_લીટીઓ[wlv.off] = 0;
		નહીંતર
		    // DBCS: Put second byte in the second screen char.
		    Screen_લીટીઓ[wlv.off] = mb_c & 0xff;

		જો (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
			)
		    ScreenCols[wlv.off] = ++wlv.vcol;
		નહીંતર
		    ScreenCols[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		જો (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ifdef FEAT_RIGHTLEFT
		જો (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.col;
		}
#endif
	    }
#ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.col;
	    }
	    નહીંતર
#endif
	    {
		++wlv.off;
		++wlv.col;
	    }
	}
#ifdef FEAT_CONCEAL
	નહીંતર જો (wp->w_p_cole > 0 && is_concealing)
	{
	    પૂર્ણાંક concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    જો (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    જો (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    જો (wp->w_p_wrap)
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
		પૂર્ણાંક adjust_col = !WIN_IS_POPUP(wp);
# નહીંતર
		પૂર્ણાંક adjust_col = TRUE;
# endif
		જો (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    જો (wp->w_p_rl)
		    {
			જો (adjust_col)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    નહીંતર
# endif
		    {
			જો (adjust_col)
			{
			    wlv.col += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		જો (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    જો (wp->w_p_rl)
		    {
			જો (adjust_col)
			{
			    --wlv.boguscols;
			    --wlv.col;
			}
		    }
		    નહીંતર
# endif
		    {
			જો (adjust_col)
			{
			    ++wlv.boguscols;
			    ++wlv.col;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		જો (wp->w_p_rl)
		{
		    જો (adjust_col)
		    {
			--wlv.boguscols;
			--wlv.col;
		    }
		}
		નહીંતર
# endif
		{
		    જો (adjust_col)
		    {
			++wlv.boguscols;
			++wlv.col;
		    }
		}
	    }
	    નહીંતર
	    {
		જો (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif // FEAT_CONCEAL
	નહીંતર
	    --skip_cells;

	જો (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	જો (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		)
	    ++wlv.vcol;

#ifdef FEAT_SYN_HL
	જો (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#endif

	// restore attributes after "precedes" in 'listchars'
	જો (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	જો (n_attr > 0 && wlv.draw_state == WL_લીટી
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	જો (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	જો ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.col < 0) :
#endif
				    (wlv.col >= wp->w_width))
		&& (wlv.draw_state != WL_લીટી
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
	    જો (wlv.boguscols != 0 && (
# ifdef લીટી_ATTR
			wlv.લીટી_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		પૂર્ણાંક attr = wlv.win_attr;
# ifdef લીટી_ATTR
		જો (wlv.લીટી_attr != 0)
		    attr = hl_combine_attr(attr, wlv.લીટી_attr);
# endif
		જ્યારે ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.col >= 0 :
# endif
			    wlv.col < wp->w_width))
		{
		    Screen_લીટીઓ[wlv.off] = ' ';
		    જો (enc_utf8)
			Screen_લીટીઓ_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    ScreenCols[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    જો (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.col--;
			wlv.boguscols++;
		    }
		    નહીંતર
# endif
		    {
			wlv.off++;
			wlv.col++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_લીટી(wp, &wlv, TRUE);
	    wlv.col += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#નહીંતર
	    wlv_screen_લીટી(wp, &wlv, TRUE);
#endif
	    ++wlv.row;
	    ++wlv.screen_row;

	    // When not wrapping and finished diff lines, break here.
	    જો (!wp->w_p_wrap
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
			&& !text_prop_above
			&& !text_prop_follows
#endif
		       )
		તોડો;
#ifdef FEAT_PROP_POPUP
	    જો (!wp->w_p_wrap && text_prop_follows && !text_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = લીટી + (size_t)ml_get_buf_len(wp->w_buffer, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#endif

	    // When the window is too narrow draw all "@" lines.
	    જો (wlv.draw_state != WL_લીટી
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
	    જો (wlv.row == endrow)
	    {
		++wlv.row;
		તોડો;
	    }

	    જો (screen_cur_row == wlv.screen_row - 1
#ifdef FEAT_DIFF
		     && wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
		     && !text_prop_above && !text_prop_follows
#endif
		     && wp->w_width == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		લીટી_Wraps[wlv.screen_row - 1] = TRUE;

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
		જો (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
			 && !(has_mbyte
			     && ((*mb_off2cells)(લીટી_Offset[wlv.screen_row],
				   લીટી_Offset[wlv.screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     લીટી_Offset[wlv.screen_row - 1]
							    + (પૂર્ણાંક)topframe->fr_width - 2,
				     લીટી_Offset[wlv.screen_row]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    જો (screen_cur_col != wp->w_width)
			screen_char(લીટી_Offset[wlv.screen_row - 1]
						       + (unsigned)topframe->fr_width - 1,
				       wlv.screen_row - 1, (પૂર્ણાંક)(topframe->fr_width - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    જો (has_mbyte && MB_BYTE2LEN(Screen_લીટીઓ[લીટી_Offset[
				     wlv.screen_row - 1] + (topframe->fr_width - 1)]) > 1)
			out_char(' ');
		    નહીંતર
			out_char(Screen_લીટીઓ[લીટી_Offset[wlv.screen_row - 1]
							    + (topframe->fr_width - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[લીટી_Offset[wlv.screen_row]] = (sattr_T)-1;
		    screen_start();	// don't know where cursor is now
		}
	    }

	    win_લીટી_start(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ifdef FEAT_LINEBREAK
	    જો (!wlv.dont_use_showbreak
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
	    જો (wlv.filler_todo == 0 && wp->w_botfill)
		તોડો;
#endif
	}

    }	// for every character in the line
#ifdef FEAT_PROP_POPUP
    જો (text_props != text_props_buf)
	vim_free(text_props);
    જો (text_prop_idxs != text_prop_idxs_buf)
	vim_free(text_prop_idxs);
    જો (text_prop_suffix_flags != text_prop_suffix_flags_buf)
	vim_free(text_prop_suffix_flags);
    vim_free(p_extra_free2);
#endif

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    પાછા wlv.row;
}
