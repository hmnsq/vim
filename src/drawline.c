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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * Get the 'listchars' "extends" characters to use for "wp", or NUL if it
 * shouldn't be used.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lcs_ext(win_T *wp)
{
    ଯଦି (wp->w_p_wrap)
	// Line never continues beyond the right of the screen with 'wrap'.
	ଫେରନ୍ତୁ NUL;
    ଯଦି (wp->w_p_wrap_flags & P_INSECURE)
	// If 'nowrap' was set from a modeline, forcibly use '>'.
	ଫେରନ୍ତୁ '>';
    ଫେରନ୍ତୁ wp->w_p_list ? wp->w_lcs_chars.ext : NUL;
}

#ifdef FEAT_SYN_HL
/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
advance_color_col(ପୂର୍ଣ୍ଣସଂଖ୍ୟା vcol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା **color_cols)
{
    ଯେତେବେଳେ (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    ଫେରନ୍ତୁ (**color_cols >= 0);
}
#endif

#ifdef FEAT_SYN_HL
/*
 * Used when 'cursorlineopt' contains "screenline": compute the margins between
 * which the highlighting is used.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
margin_columns_win(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ବାମ_col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଡାହାଣ_col)
{
    // cache previous calculations depending on w_virtcol
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_w_virtcol;
    ସ୍ଥିର win_T *prev_wp;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_width1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_width2;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_ବାମ_col;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_ଡାହାଣ_col;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_col_off = win_col_off(wp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width2;

    width1 = wp->w_width - cur_col_off;
    width2 = width1 + win_col_off2(wp);

    ଯଦି (saved_w_virtcol == wp->w_virtcol && prev_wp == wp
	    && prev_width1 == width1 && prev_width2 == width2)
    {
	*ଡାହାଣ_col = prev_ଡାହାଣ_col;
	*ବାମ_col = prev_ବାମ_col;
	ଫେରନ୍ତୁ;
    }

    *ବାମ_col = 0;
    *ଡାହାଣ_col = width1;

    ଯଦି (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ଡାହାଣ_col = width1 + ((wp->w_virtcol - width1) / width2 + 1) * width2;
    ଯଦି (wp->w_virtcol >= (colnr_T)width1 && width2 > 0)
	*ବାମ_col = (wp->w_virtcol - width1) / width2 * width2 + width1;

    // cache values
    prev_ବାମ_col = *ବାମ_col;
    prev_ଡାହାଣ_col = *ଡାହାଣ_col;
    prev_wp = wp;
    prev_width1 = width1;
    prev_width2 = width2;
    saved_w_virtcol = wp->w_virtcol;
}
#endif

#ଯଦି defined(FEAT_SIGNS) || defined(FEAT_QUICKFIX) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
// using an attribute for the whole line
# define ଧାଡ଼ି_ATTR
#endif

// structure with variables passed between win_line() and other functions
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		draw_state;	// what to draw next

    linenr_T	lnum;		// line number to be drawn

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startrow;	// first row in the window to be drawn
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;		// row in the window, excl w_winrow
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_row;	// row on the screen, incl w_winrow

    long	vcol;		// virtual column, before wrapping
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;		// visual column on screen, after wrapping
#ifdef FEAT_CONCEAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		boguscols;	// nonexistent columns added to "col" to force
				// wrapping
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol_off_co;	// offset for concealed characters
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol_off_tp;	// offset for virtual text
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		draw_color_col;	// highlight colorcolumn
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*color_cols;	// pointer to according columns array
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eol_hl_off;	// 1 if highlighted char after EOL

    unsigned	off;		// offset in ScreenLines/ScreenAttrs

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		win_attr;	// background for the whole window, except
				// margins and "~" lines.
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cul_attr;	// set when 'cursorline' active
#endif
#ifdef ଧାଡ଼ି_ATTR
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_attr;	// for the whole line, includes 'cursorline'
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_ଧାଡ଼ି_flags;  // flags for screen_line()

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fromcol;	// start of inverting
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tocol;		// end of inverting

#ifdef FEAT_LINEBREAK
    long	vcol_sbr;	    // virtual column after showbreak
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_showbreak;	    // overlong line, skipping first x chars
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dont_use_showbreak; // do not use 'showbreak'
#endif
#ifdef FEAT_PROP_POPUP
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_above_ଗଣନା;
#endif

    // TRUE when 'cursorlineopt' has "screenline" and cursor is in this line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cul_screenline;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		char_attr;	// attributes for the next character

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_extra;	// number of extra bytes
    char_u	*p_extra;	// string of extra chars, plus NUL, only used
				// when c_extra and c_final are NUL
    char_u	*p_extra_free;  // p_extra buffer that needs to be freed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_attr;	// attributes for p_extra, should be combined
				// with win_attr if needed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_attr_skip;    // chars to skip before using extra_attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c_extra;	// extra chars, all the same
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c_final;	// final char, mandatory if set
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_for_textprop; // n_extra set for textprop
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_extra_for_textprop; // extra_for_textprop was just set

    // saved "extra" items for when draw_state becomes WL_LINE (again)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_n_extra;
    char_u	*saved_p_extra;
    char_u	*saved_p_extra_free;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_extra_attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_n_attr_skip;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_extra_for_textprop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_c_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_c_final;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_char_attr;

    char_u	extra[NUMBUFLEN + MB_MAXBYTES];
				// "%ld " and 'fdc' must fit in here, as well
				// any text sign

#ifdef FEAT_DIFF
    hlf_T	diff_hlf;	// type of diff highlighting
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filler_ଧାଡ଼ିଗୁଡ଼ିକ;	// nr of filler lines to be drawn
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filler_todo;	// nr of filler lines still to do + 1
#ifdef FEAT_SIGNS
    sign_attrs_T sattr;
#endif
#ifdef FEAT_LINEBREAK
     // do consider wrapping in linebreak mode only after encountering
     // a non whitespace char
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_lbr;
#endif
} winlinevars_T;

// draw_state values for items that are drawn in sequence:
#define WL_START	0		// nothing done yet, must be zero
#define WL_CMDLINE	(WL_START + 1)	// cmdline window column
#ifdef FEAT_FOLDING
# define WL_FOLD	(WL_CMDLINE + 1)	// 'foldcolumn'
#ଅନ୍ୟଥା
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	(WL_FOLD + 1)	// column for signs
#ଅନ୍ୟଥା
# define WL_SIGN	WL_FOLD		// column for signs
#endif
#define WL_NR		(WL_SIGN + 1)	// line number
#ifdef FEAT_LINEBREAK
# define WL_BRI		(WL_NR + 1)	// 'breakindent'
#ଅନ୍ୟଥା
# define WL_BRI		WL_NR
#endif
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		(WL_BRI + 1)	// 'showbreak' or 'diff'
#ଅନ୍ୟଥା
# define WL_SBR		WL_BRI
#endif
#define WL_ଧାଡ଼ି		(WL_SBR + 1)	// text in the line

#ଯଦି defined(FEAT_SIGNS) || defined(FEAT_FOLDING)
/*
 * Return TRUE if CursorLineSign / CursorLineFold highlight is to be used.
 * This depends only on 'cursorline', not on 'cursorlineopt'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_cursor_ଧାଡ଼ି_highlight(win_T *wp, linenr_T lnum)
{
    ଫେରନ୍ତୁ wp->w_p_cul
	    && lnum == wp->w_cursor.lnum;
}
#endif


#ifdef FEAT_FOLDING
/*
 * Setup for drawing the 'foldcolumn', if there is one.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_foldcolumn(win_T *wp, winlinevars_T *wlv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdc = compute_foldcolumn(wp, 0);

    ଯଦି (fdc <= 0)
	ଫେରନ୍ତୁ;

    // Allocate a buffer, "wlv->extra[]" may already be in use.
    vim_free(wlv->p_extra_free);
    wlv->p_extra_free = alloc(MAX_MCO * fdc + 1);
    ଯଦି (wlv->p_extra_free == ଖାଲି)
	ଫେରନ୍ତୁ;

    wlv->n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fill_foldcolumn(wlv->p_extra_free,
							 wp, FALSE, wlv->lnum);
    wlv->p_extra_free[wlv->n_extra] = NUL;
    wlv->p_extra = wlv->p_extra_free;
    wlv->c_extra = NUL;
    wlv->c_final = NUL;
    ଯଦି (use_cursor_ଧାଡ଼ି_highlight(wp, wlv->lnum))
	wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLF));
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
get_sign_display_info(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nrcol,
	win_T		*wp,
	winlinevars_T	*wlv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	text_sign;
# ifdef FEAT_SIGN_ICONS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	icon_sign;
# endif

    // Draw two cells with the sign value or blank.
    wlv->c_extra = ' ';
    wlv->c_final = NUL;
    ଯଦି (nrcol)
	wlv->n_extra = ସଂଖ୍ୟା_width(wp) + 1;
    ଅନ୍ୟଥା
    {
	ଯଦି (use_cursor_ଧାଡ଼ି_highlight(wp, wlv->lnum))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLS));
	ଅନ୍ୟଥା
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_SC));
	wlv->n_extra = 2;
    }

    ଯଦି (wlv->row == wlv->startrow
# ifdef FEAT_DIFF
	    + wlv->filler_ଧାଡ଼ିଗୁଡ଼ିକ && wlv->filler_todo <= 0
# endif
       )
    {
	text_sign = (wlv->sattr.sat_text != ଖାଲି) ? wlv->sattr.sat_typenr : 0;
# ifdef FEAT_SIGN_ICONS
	icon_sign = (wlv->sattr.sat_icon != ଖାଲି) ? wlv->sattr.sat_typenr : 0;
	ଯଦି (gui.in_use && icon_sign != 0)
	{
	    // Use the image in this position.
	    ଯଦି (nrcol)
	    {
		wlv->c_extra = NUL;
		wlv->n_extra = vim_snprintf((ଅକ୍ଷର *)wlv->extra, ଆକାର(wlv->extra),
						"%-*c ", ସଂଖ୍ୟା_width(wp), SIGN_BYTE);
		wlv->p_extra = wlv->extra;
	    }
	    ଅନ୍ୟଥା
		wlv->c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
	    ଯଦି (netbeans_active() && (buf_signcount(wp->w_buffer, wlv->lnum)
									  > 1))
	    {
		ଯଦି (nrcol)
		{
		    wlv->c_extra = NUL;
		    wlv->n_extra = vim_snprintf((ଅକ୍ଷର *)wlv->extra, ଆକାର(wlv->extra),
						"%-*c ", ସଂଖ୍ୟା_width(wp), MULTISIGN_BYTE);
		    wlv->p_extra = wlv->extra;
		}
		ଅନ୍ୟଥା
		    wlv->c_extra = MULTISIGN_BYTE;
	    }
#  endif
	    wlv->c_final = NUL;
	    wlv->char_attr = icon_sign;
	}
	ଅନ୍ୟଥା
# endif
	    ଯଦି (text_sign != 0)
	    {
		wlv->p_extra = wlv->sattr.sat_text;
		ଯଦି (wlv->p_extra != ଖାଲି)
		{
		    ଯଦି (nrcol)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = ସଂଖ୍ୟା_width(wp) - 2;

			vim_memset(wlv->extra, ' ', width);
			wlv->n_extra = width;
			wlv->n_extra += vim_snprintf((ଅକ୍ଷର *)wlv->extra + width,
				  ଆକାର(wlv->extra) - width, "%s ", wlv->p_extra);
			wlv->p_extra = wlv->extra;
		    }
		    ଅନ୍ୟଥା
			wlv->n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(wlv->p_extra);

		    wlv->c_extra = NUL;
		    wlv->c_final = NUL;
		}

		ଯଦି (use_cursor_ଧାଡ଼ି_highlight(wp, wlv->lnum)
						  && wlv->sattr.sat_culhl > 0)
		    wlv->char_attr = wlv->sattr.sat_culhl;
		ଅନ୍ୟଥା
		    wlv->char_attr = wlv->sattr.sat_texthl;
	    }
    }
}
#endif

/*
 * Display the absolute or relative line number.  After the first row fill with
 * blanks when the 'n' flag isn't in 'cpo'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_lnum_col(
	win_T		*wp,
	winlinevars_T	*wlv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sign_present UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_attr UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_cpo_n = vim_strchr(p_cpo, CPO_NUMCOL) != ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum_row = wlv->startrow + wlv->filler_ଧାଡ଼ିଗୁଡ଼ିକ
#ifdef FEAT_PROP_POPUP
		      + wlv->text_prop_above_ଗଣନା
#endif
		      ;

    ଯଦି ((wp->w_p_nu || wp->w_p_rnu)
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
	ଯଦି ((*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u') && sign_present
		&& wlv->sattr.sat_text != ଖାଲି)
	    get_sign_display_info(TRUE, wp, wlv);
	ଅନ୍ୟଥା
#endif
	{
	  // Draw the line number (empty space after wrapping).
	  // When there are text properties above the line put the line number
	  // below them.
	  ଯଦି (wlv->row == lnum_row
		    && (wp->w_skipcol == 0 || wlv->row > 0
					       || (wp->w_p_nu && wp->w_p_rnu)))
	  {
	      long ଅଙ୍କ;
	      ଅକ୍ଷର *fmt = "%*ld ";

	      ଯଦି (wp->w_p_nu && !wp->w_p_rnu)
		  // 'number' + 'norelativenumber'
		  ଅଙ୍କ = (long)wlv->lnum;
	      ଅନ୍ୟଥା
	      {
		  // 'relativenumber', don't use negative numbers
		  ଅଙ୍କ = labs((long)get_cursor_rel_lnum(wp, wlv->lnum));
		  ଯଦି (ଅଙ୍କ == 0 && wp->w_p_nu && wp->w_p_rnu)
		  {
		      // 'number' + 'relativenumber'
		      ଅଙ୍କ = wlv->lnum;
		      fmt = "%-*ld ";
		  }
	      }

	      vim_snprintf((ଅକ୍ଷର *)wlv->extra, ଆକାର(wlv->extra), fmt, ସଂଖ୍ୟା_width(wp), ଅଙ୍କ);
	      ଯଦି (wp->w_skipcol > 0 && wlv->startrow == 0)
		  ପାଇଁ (wlv->p_extra = wlv->extra; *wlv->p_extra == ' ';
			  ++wlv->p_extra)
		      *wlv->p_extra = '-';
#ifdef FEAT_RIGHTLEFT
	      ଯଦି (wp->w_p_rl)		    // reverse line numbers
	      {
		  char_u    *p1, *p2;
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    t;

		  // like rl_mirror(), but keep the space at the end
		  p2 = skipwhite(wlv->extra);
		  p2 = skiptowhite(p2) - 1;
		  ପାଇଁ (p1 = skipwhite(wlv->extra); p1 < p2; ++p1, --p2)
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
	  ଅନ୍ୟଥା
	  {
	      wlv->c_extra = ' ';
	      wlv->c_final = NUL;
	  }
	  wlv->n_extra = ସଂଖ୍ୟା_width(wp) + 1;
	  wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_N));
#ifdef FEAT_SYN_HL
	  // When 'cursorline' is set highlight the line number of
	  // the current line differently.
	  // When 'cursorlineopt' does not have "line" only
	  // highlight the line number itself.
	  // TODO: Can we use CursorLine instead of CursorLineNr
	  // when CursorLineNr isn't set?
	  ଯଦି (wp->w_p_cul
		  && wlv->lnum == wp->w_cursor.lnum
		  && (wp->w_p_culopt_flags & CULOPT_NBR)
		  && (wlv->row == lnum_row
		      || (wlv->row > lnum_row
			 && (wp->w_p_culopt_flags & CULOPT_ଧାଡ଼ି))))
	    wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_CLN));
#endif
	  ଯଦି (wp->w_p_rnu && wlv->lnum < wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNA) != 0)
	      // Use LineNrAbove
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNA));
	  ଯଦି (wp->w_p_rnu && wlv->lnum > wp->w_cursor.lnum
						      && HL_ATTR(HLF_LNB) != 0)
	      // Use LineNrBelow
	      wlv->char_attr = hl_combine_attr(get_win_attr(wp), HL_ATTR(HLF_LNB));
	}
#ifdef FEAT_SIGNS
	ଯଦି (ଅଙ୍କ_attr)
	    wlv->char_attr = ଅଙ୍କ_attr;
#endif
    }
}

#ifdef FEAT_LINEBREAK
    ସ୍ଥିର ଶୂନ୍ୟ
handle_breakindent(win_T *wp, winlinevars_T *wlv)
{
    ଯଦି (wp->w_briopt_sbr && wlv->draw_state == WL_BRI - 1
					    && *get_showbreak_ମୂଲ୍ୟ(wp) != NUL)
	// draw indent after showbreak value
	wlv->draw_state = WL_BRI;
    ଅନ୍ୟଥା ଯଦି (wp->w_briopt_sbr && wlv->draw_state == WL_SBR)
	// After the showbreak, draw the breakindent
	wlv->draw_state = WL_BRI - 1;

    // draw 'breakindent': indent wrapped text accordingly
    ଯଦି (wlv->draw_state == WL_BRI - 1)
    {
	wlv->draw_state = WL_BRI;
	// if wlv->need_showbreak is set, breakindent also applies
	ଯଦି (wp->w_p_bri && (wlv->row > wlv->startrow
# ifdef FEAT_DIFF
		    + wlv->filler_ଧାଡ଼ିଗୁଡ଼ିକ
# endif
		    || wlv->need_showbreak)
# ifdef FEAT_PROP_POPUP
		&& !wlv->dont_use_showbreak
# endif
	   )
	{
	    wlv->char_attr = 0;
# ifdef FEAT_DIFF
	    ଯଦି (wlv->diff_hlf != (hlf_T)0)
		wlv->char_attr = HL_ATTR(wlv->diff_hlf);
# endif
	    wlv->p_extra = ଖାଲି;
	    wlv->c_extra = ' ';
	    wlv->c_final = NUL;
	    wlv->n_extra = get_breakindent_win(wp,
				   ml_get_buf(wp->w_buffer, wlv->lnum, FALSE));
	    ଯଦି (wlv->row == wlv->startrow)
	    {
		wlv->n_extra -= win_col_off2(wp);
		ଯଦି (wlv->n_extra < 0)
		    wlv->n_extra = 0;
	    }

	    // Correct start of highlighted area for 'breakindent',
	    ଯଦି (wlv->fromcol >= wlv->vcol
				    && wlv->fromcol < wlv->vcol + wlv->n_extra)
		wlv->fromcol = wlv->vcol + wlv->n_extra;

	    // Correct end of highlighted area for 'breakindent',
	    // required when 'linebreak' is also set.
	    ଯଦି (wlv->tocol == wlv->vcol)
		wlv->tocol += wlv->n_extra;
	}

	ଯଦି (wp->w_skipcol > 0 && wlv->startrow == 0 && wp->w_p_wrap
							   && wp->w_briopt_sbr)
	    wlv->need_showbreak = FALSE;
    }
}
#endif

#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
    ସ୍ଥିର ଶୂନ୍ୟ
handle_showbreak_and_filler(win_T *wp, winlinevars_T *wlv)
{
# ifdef FEAT_DIFF
    ଯଦି (wlv->filler_todo > 0)
    {
	// Draw "deleted" diff line(s).
	ଯଦି (char2cells(wp->w_fill_chars.diff) > 1)
	{
	    wlv->c_extra = '-';
	    wlv->c_final = NUL;
	}
	ଅନ୍ୟଥା
	{
	    wlv->c_extra = wp->w_fill_chars.diff;
	    wlv->c_final = NUL;
	}
#  ifdef FEAT_RIGHTLEFT
	ଯଦି (wp->w_p_rl)
	    wlv->n_extra = wlv->col + 1;
	ଅନ୍ୟଥା
#  endif
	    wlv->n_extra = wp->w_width - wlv->col;
	wlv->char_attr = HL_ATTR(HLF_DED);
    }
# endif

# ifdef FEAT_LINEBREAK
    char_u *sbr = get_showbreak_ମୂଲ୍ୟ(wp);
    ଯଦି (*sbr != NUL && wlv->need_showbreak)
    {
	// Draw 'showbreak' at the start of each broken line.
	wlv->p_extra = sbr;
	wlv->c_extra = NUL;
	wlv->c_final = NUL;
	wlv->n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(sbr);
	wlv->vcol_sbr = wlv->vcol + MB_CHARLEN(sbr);

	// Correct start of highlighted area for 'showbreak'.
	ଯଦି (wlv->fromcol >= wlv->vcol && wlv->fromcol < wlv->vcol_sbr)
	    wlv->fromcol = wlv->vcol_sbr;

	// Correct end of highlighted area for 'showbreak',
	// required when 'linebreak' is also set.
	ଯଦି (wlv->tocol == wlv->vcol)
	    wlv->tocol = wlv->vcol_sbr;
	// combine 'showbreak' with HLF_WIN
	wlv->char_attr = hl_combine_attr(wlv->win_attr, HL_ATTR(HLF_AT));
#  ifdef FEAT_SYN_HL
	// combine 'showbreak' with 'cursorline'
	ଯଦି (wlv->cul_attr != 0)
	    wlv->char_attr = hl_combine_attr(wlv->char_attr, wlv->cul_attr);
#  endif
    }

    ଯଦି (wp->w_skipcol == 0 || wlv->startrow > 0 || !wp->w_p_wrap
							  || !wp->w_briopt_sbr)
	wlv->need_showbreak = FALSE;
# endif
}
#endif

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Return the cell size of virtual text after truncation.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
textprop_size_after_trunc(
	win_T	*wp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags,	    // TP_FLAG_ALIGN_*
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	added,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	padding,
	char_u	*text,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*n_used_ptr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	space = (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
				       ? wp->w_width - win_col_off(wp) : added;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା strsize = 0;
    char_u *p;

    // if the remaining size is too small and 'wrap' is set we wrap anyway and
    // use the next line
    ଯଦି (space < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)
	space += wp->w_width;
    ଯଦି (flags & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ABOVE))
	space -= padding;

    ପାଇଁ (p = text; *p != NUL; p += (*mb_ptr2len)(p))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen = ptr2cells(p);

	ଯଦି (strsize + clen > space)
	    ଭାଙ୍ଗ;
	strsize += clen;
    }
    *n_used_ptr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - text);

    ଫେରନ୍ତୁ strsize;
}

/*
 * Take care of padding, right-align and truncation of virtual text after a
 * line.
 * if "n_attr" is not NULL then "n_extra" and "p_extra" are adjusted for any
 * padding, right-align and truncation.  Otherwise only the size is computed.
 * When "n_attr" is NULL returns the number of screen cells used.
 * Otherwise returns TRUE when drawing continues on the next line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
text_prop_position(
	win_T	    *wp,
	textprop_T  *tp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vcol,	    // current text column
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scr_col,	    // current screen column
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *n_extra,	    // nr of bytes for virtual text
	char_u	    **p_extra,	    // virtual text
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *n_attr,	    // attribute cells, NULL if not used
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *n_attr_skip,   // cells to skip attr, NULL if not used
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    do_skip)	    // skip_cells is not zero
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଡାହାଣ = (tp->tp_flags & TP_FLAG_ALIGN_ଡାହାଣ);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    padding = tp->tp_col == MAXCOL ? tp->tp_padleft : 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col_with_padding = scr_col + (below ? 0 : padding);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    room = wp->w_width - col_with_padding;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    before = room;	// spaces before the text
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    after = 0;		// spaces after the text
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n_used = *n_extra;
    char_u  *l = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    strsize = vim_strsize(*p_extra);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cells = wrap ? strsize : textprop_size_after_trunc(wp,
			     tp->tp_flags, before, padding, *p_extra, &n_used);

    ଯଦି (wrap || ଡାହାଣ || above || below || padding > 0 || n_used < *n_extra)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col_off = win_col_off(wp) - win_col_off2(wp);

	ଯଦି (above)
	{
	    before = 0;
	    after = wp->w_width - cells - win_col_off(wp) - padding;
	    ଯଦି (after < 0)
	    {
		// text "above" has too much padding to fit
		padding += after;
		after = 0;
	    }
	}
	ଅନ୍ୟଥା
	{
	    // Right-align: fill with before
	    ଯଦି (ଡାହାଣ)
		before -= cells;

	    // Below-align: empty line add one character
	    ଯଦି (below && vcol == 0 && col_with_padding == col_off
					    && wp->w_width - col_off == before)
		col_with_padding += 1;

	    ଯଦି (before < 0
		    || !(ଡାହାଣ || below)
		    || (below ? (col_with_padding <= col_off || !wp->w_p_wrap)
			      : (n_used < *n_extra)))
	    {
		ଯଦି (ଡାହାଣ && (wrap
			      || (room < PROP_TEXT_MIN_CELLS && wp->w_p_wrap)))
		{
		    // right-align on next line instead of wrapping if possible
		    before = wp->w_width - col_off - strsize + room;
		    ଯଦି (before < 0)
			before = 0;
		    ଅନ୍ୟଥା
			n_used = *n_extra;
		}
		ଅନ୍ୟଥା ଯଦି (below && before > vcol && do_skip)
		    before -= vcol;
		ଅନ୍ୟଥା
		    before = 0;
	    }
	}

	// With 'nowrap' add one to show the "extends" character if needed (it
	// doesn't show if the text just fits).
	ଯଦି (n_used < *n_extra && get_lcs_ext(wp) != NUL)
	    ++n_used;

	// add 1 for NUL, 2 for when '…' is used
	ଯଦି (n_attr != ଖାଲି)
	    l = alloc(n_used + before + after + (padding > 0 ? padding : 0) + 3);
	ଯଦି (n_attr == ଖାଲି || l != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = 0;

	    ଯଦି (n_attr != ଖାଲି)
	    {
		vim_memset(l, ' ', before);
		off += before;
		ଯଦି (padding > 0)
		{
		    vim_memset(l + off, ' ', padding);
		    off += padding;
		}
		vim_strncpy(l + off, *p_extra, n_used);
		off += n_used;
	    }
	    ଅନ୍ୟଥା
	    {
		off = before + after + padding + n_used;
		cells += before + after + padding;
	    }
	    ଯଦି (n_attr != ଖାଲି)
	    {
		ଯଦି (n_used < *n_extra && wp->w_p_wrap)
		{
		    char_u *lp = l + off - 1;

		    ଯଦି (has_mbyte)
		    {
			char_u	buf[MB_MAXBYTES + 1];
			char_u	*cp = buf;

			// change the last character to '…', converted to the
			// current 'encoding'
			STRCPY(buf, "…");
			ଯଦି (!enc_utf8)
			{
			    vimconv_T	vc;

			    vc.vc_type = CONV_NONE;
			    convert_setup(&vc, (char_u *)"utf-8", p_enc);
			    ଯଦି (vc.vc_type != CONV_NONE)
			    {
				cp = string_convert(&vc, buf, ଖାଲି);
				ଯଦି (cp == ଖାଲି)
				{
				    // when conversion fails use '>'
				    cp = buf;
				    STRCPY(buf, ">");
				}
				convert_setup(&vc, ଖାଲି, ଖାଲି);
			    }
			}

			lp -= (*mb_ptr2cells)(cp) - 1;
			lp -= (*mb_head_off)(l, lp);
			STRCPY(lp, cp);
			n_used = lp - l + 3 - before - padding;
			ଯଦି (cp != buf)
			    vim_free(cp);
		    }
		    ଅନ୍ୟଥା
			// change last character to '>'
			*lp = '>';
		}
		ଅନ୍ୟଥା ଯଦି (after > 0)
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
		ଯଦି (above)
		    *n_attr -= after;
	    }
	}
    }

    ଯଦି (n_attr == ଖାଲି)
	ଫେରନ୍ତୁ cells;
    ଫେରନ୍ତୁ (below && col_with_padding > win_col_off(wp) && !wp->w_p_wrap);
}

# ଯଦି defined(FEAT_LINEBREAK)
/*
 * no 'showbreak' before "below" text property
 * or after "above" or "right" text property
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
text_prop_no_showbreak(textprop_T *tp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଡାହାଣ = (tp->tp_flags & TP_FLAG_ALIGN_ଡାହାଣ);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    above = (tp->tp_flags & TP_FLAG_ALIGN_ABOVE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    below = (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    wrap = tp->tp_col < MAXCOL || (tp->tp_flags & TP_FLAG_WRAP);

    ଫେରନ୍ତୁ (ଡାହାଣ || above || below || !wrap);
}
# endif
#endif

/*
 * Call screen_line() using values from "wlv".
 * Also takes care of putting "<<<" on the first line for 'smoothscroll'
 * when 'showbreak' is not set.
 * When "clear_end" is TRUE clear until the end of the screen line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wlv_screen_ଧାଡ଼ି(win_T *wp, winlinevars_T *wlv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା clear_end)
{
    ଯଦି (wlv->row == 0 && wp->w_skipcol > 0
#ଯଦି defined(FEAT_LINEBREAK)
	    // do not overwrite the 'showbreak' text with "<<<"
	    && *get_showbreak_ମୂଲ୍ୟ(wp) == NUL
#endif
	    // do not overwrite the 'listchars' "precedes" text with "<<<"
	    && !(wp->w_p_list && wp->w_lcs_chars.prec != 0))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(current_Screen_ଧାଡ଼ି - Screen_ଧାଡ଼ିଗୁଡ଼ିକ);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_off = off + screen_Columns;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip = 0;

	ଯଦି (wp->w_p_nu && wp->w_p_rnu)
	    // Do not overwrite the line number, change "123 text" to
	    // "123<<<xt".
	    ଯେତେବେଳେ (skip < wp->w_width && VIM_ISDIGIT(Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off]))
	    {
		++off;
		++skip;
	    }

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 3 && i + skip < wp->w_width; ++i)
	{
	    ଯଦି ((*mb_off2cells)(off, max_off) > 1)
		// When the first half of a double-width character is
		// overwritten, change the second half to a space.
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off + 1] = ' ';
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = '<';
	    ଯଦି (enc_utf8)
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = 0;
	    ScreenAttrs[off] = HL_ATTR(HLF_AT);
	    ++off;
	}
    }

    screen_ଧାଡ଼ି(wp, wlv->screen_row, wp->w_wincol, wlv->col,
		    clear_end ? wp->w_width : -wp->w_width,
		    wlv->vcol - 1, wlv->screen_ଧାଡ଼ି_flags);
}

/*
 * Called when finished with the line: draw the screen line and handle any
 * highlighting until the right of the window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
draw_screen_ଧାଡ଼ି(win_T *wp, winlinevars_T *wlv)
{
#ifdef FEAT_SYN_HL
    long	v;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wcol;

    // Highlight 'cursorcolumn' & 'colorcolumn' past end of the line.
    ଯଦି (wp->w_p_wrap)
	v = wlv->startrow == 0 ? wp->w_skipcol : 0;
    ଅନ୍ୟଥା
	v = wp->w_leftcol;

    wcol =
# ifdef FEAT_RIGHTLEFT
	wp->w_p_rl ? wp->w_width - wlv->col - 1 :
# endif
	wlv->col;
    // check if line ends before left margin
    ଯଦି (wlv->vcol < v + wcol - win_col_off(wp))
	wlv->vcol = v + wcol - win_col_off(wp);
# ifdef FEAT_CONCEAL
    // Get rid of the boguscols now, we want to draw until the right
    // edge for 'cursorcolumn'.
    wlv->col -= wlv->boguscols;
    wlv->boguscols = 0;
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_co - wlv->vcol_off_tp)
# ଅନ୍ୟଥା
#  define VCOL_HLC (wlv->vcol - wlv->vcol_off_tp)
# endif

    ଯଦି (wlv->draw_color_col)
	wlv->draw_color_col = advance_color_col(VCOL_HLC, &wlv->color_cols);

    ଯଦି (((wp->w_p_cuc
		    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_virtcol >= VCOL_HLC - wlv->eol_hl_off
		    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_virtcol <
			 (long)wp->w_width * (wlv->row - wlv->startrow + 1) + v
			 && wlv->lnum != wp->w_cursor.lnum)
		|| wlv->draw_color_col
# ifdef ଧାଡ଼ି_ATTR
		|| wlv->ଧାଡ଼ି_attr != 0
# endif
		|| wlv->win_attr != 0))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	rightmost_vcol = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	ଯଦି (wp->w_p_cuc)
	    rightmost_vcol = wp->w_virtcol;
	ଯଦି (wlv->draw_color_col)
	    // determine rightmost colorcolumn to possibly draw
	    ପାଇଁ (i = 0; wlv->color_cols[i] >= 0; ++i)
		ଯଦି (rightmost_vcol < wlv->color_cols[i])
		    rightmost_vcol = wlv->color_cols[i];

	ଯେତେବେଳେ (
# ifdef FEAT_RIGHTLEFT
		wp->w_p_rl ? (wlv->col >= 0) :
# endif
		(wlv->col < wp->w_width))
	{
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv->off] = ' ';
	    ଯଦି (enc_utf8)
		Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[wlv->off] = 0;

	    ଯଦି (wlv->draw_color_col)
		wlv->draw_color_col = advance_color_col(
						   VCOL_HLC, &wlv->color_cols);

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr = wlv->win_attr;
# ifdef ଧାଡ଼ି_ATTR
	    ଯଦି (wlv->ଧାଡ଼ି_attr != 0)
		attr = hl_combine_attr(attr, wlv->ଧାଡ଼ି_attr);
# endif
	    ଯଦି (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
		    && wlv->lnum != wp->w_cursor.lnum)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_CUC));
	    ଅନ୍ୟଥା ଯଦି (wlv->draw_color_col && VCOL_HLC == *wlv->color_cols)
		attr = hl_combine_attr(attr, HL_ATTR(HLF_MC));
	    ScreenAttrs[wlv->off] = attr;
	    ScreenCols[wlv->off] = wlv->vcol;
# ifdef FEAT_RIGHTLEFT
	    ଯଦି (wp->w_p_rl)
	    {
		--wlv->off;
		--wlv->col;
	    }
	    ଅନ୍ୟଥା
# endif
	    {
		++wlv->off;
		++wlv->col;
	    }
	    ++wlv->vcol;

	    ଯଦି (VCOL_HLC > rightmost_vcol
# ifdef ଧାଡ଼ି_ATTR
		    && wlv->ଧାଡ଼ି_attr == 0
# endif
		    && wlv->win_attr == 0)
		ଭାଙ୍ଗ;
	}
    }
#endif

    // Set increasing virtual columns in ScreenCols[] to set correct curswant
    // (or "coladd" for 'virtualedit') when clicking after end of line.
    wlv->screen_ଧାଡ଼ି_flags |= SLF_INC_VCOL;
    wlv_screen_ଧାଡ଼ି(wp, wlv, TRUE);
    wlv->screen_ଧାଡ଼ି_flags &= ~SLF_INC_VCOL;
    ++wlv->row;
    ++wlv->screen_row;
}
#undef VCOL_HLC

/*
 * Start a screen line at column zero.
 * When "save_extra" is TRUE save and reset n_extra, p_extra, etc.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_ଧାଡ଼ି_start(win_T *wp UNUSED, winlinevars_T *wlv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_extra)
{
    wlv->col = 0;
    wlv->off = (unsigned)(current_Screen_ଧାଡ଼ି - Screen_ଧାଡ଼ିଗୁଡ଼ିକ);
#ifdef FEAT_LINEBREAK
    wlv->need_lbr = FALSE;
#endif

#ifdef FEAT_RIGHTLEFT
    ଯଦି (wp->w_p_rl)
    {
	// Rightleft window: process the text in the normal direction, but put
	// it in current_ScreenLine[] from right to left.  Start at the
	// rightmost column of the window.
	wlv->col = wp->w_width - 1;
	wlv->off += wlv->col;
	wlv->screen_ଧାଡ଼ି_flags |= SLF_RIGHTLEFT;
    }
#endif
    ଯଦି (save_extra)
    {
	// reset the drawing state for the start of a wrapped line
	wlv->draw_state = WL_START;
	wlv->saved_n_extra = wlv->n_extra;
	wlv->saved_p_extra = wlv->p_extra;
	vim_free(wlv->saved_p_extra_free);
	wlv->saved_p_extra_free = wlv->p_extra_free;
	wlv->p_extra_free = ଖାଲି;
	wlv->saved_extra_attr = wlv->extra_attr;
	wlv->saved_n_attr_skip = wlv->n_attr_skip;
	wlv->saved_extra_for_textprop = wlv->extra_for_textprop;
	wlv->saved_c_extra = wlv->c_extra;
	wlv->saved_c_final = wlv->c_final;
#ifdef FEAT_LINEBREAK
	wlv->need_lbr = TRUE;
#endif
#ifdef FEAT_SYN_HL
	ଯଦି (!(wlv->cul_screenline
# ifdef FEAT_DIFF
		    && wlv->diff_hlf == (hlf_T)0
# endif
	     ))
	    wlv->saved_char_attr = wlv->char_attr;
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
win_ଧାଡ଼ି_continue(winlinevars_T *wlv)
{
    ଯଦି (wlv->saved_n_extra > 0)
    {
	// Continue item from end of wrapped line.
	wlv->n_extra = wlv->saved_n_extra;
	wlv->saved_n_extra = 0;
	wlv->c_extra = wlv->saved_c_extra;
	wlv->c_final = wlv->saved_c_final;
	wlv->p_extra = wlv->saved_p_extra;
	vim_free(wlv->p_extra_free);
	wlv->p_extra_free = wlv->saved_p_extra_free;
	wlv->saved_p_extra_free = ଖାଲି;
	wlv->extra_attr = wlv->saved_extra_attr;
	wlv->n_attr_skip = wlv->saved_n_attr_skip;
	wlv->extra_for_textprop = wlv->saved_extra_for_textprop;
	wlv->char_attr = wlv->saved_char_attr;
    }
    ଅନ୍ୟଥା
	wlv->char_attr = wlv->win_attr;
}

#ifdef FEAT_SYN_HL
    ସ୍ଥିର ଶୂନ୍ୟ
apply_cursorline_highlight(
	winlinevars_T *wlv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_present UNUSED)
{
    wlv->cul_attr = HL_ATTR(HLF_CUL);
# ifdef FEAT_SIGNS
    // Combine the 'cursorline' and sign highlighting, depending on
    // the sign priority.
    ଯଦି (sign_present && wlv->sattr.sat_linehl > 0)
    {
	ଯଦି (wlv->sattr.sat_priority >= 100)
	    wlv->ଧାଡ଼ି_attr = hl_combine_attr(wlv->cul_attr, wlv->ଧାଡ଼ି_attr);
	ଅନ୍ୟଥା
	    wlv->ଧାଡ଼ି_attr = hl_combine_attr(wlv->ଧାଡ଼ି_attr, wlv->cul_attr);
    }
    ଅନ୍ୟଥା
# endif
# ଯଦି defined(FEAT_QUICKFIX)
	// let the line attribute overrule 'cursorline', otherwise
	// it disappears when both have background set;
	// 'cursorline' can use underline or bold to make it show
	wlv->ଧାଡ଼ି_attr = hl_combine_attr(wlv->cul_attr, wlv->ଧାଡ଼ି_attr);
# ଅନ୍ୟଥା
	wlv->ଧାଡ଼ି_attr = wlv->cul_attr;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_ଧାଡ଼ି(
    win_T	*wp,
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startrow,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		endrow,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସଂଖ୍ୟା_only,
    spellvars_T	*spv UNUSED)
{
    winlinevars_T	wlv;		// variables passed between functions

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = 0;			// init for GCC
    char_u	*ଧାଡ଼ି;			// current line
    char_u	*ptr;			// current position in "line"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_curline = wp == curwin && lnum == curwin->w_cursor.lnum;

#ifdef FEAT_PROP_POPUP
    char_u	*p_extra_free2 = ଖାଲି;   // another p_extra to be freed
#endif
#ଯଦି defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_linebreak = FALSE;	// n_extra set for showing linebreak
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lcs_eol_one = wp->w_lcs_chars.eol; // eol until it's been used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lcs_prec_todo = wp->w_lcs_chars.prec;
					// prec until it's been used

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_attr = 0;	    // chars with special attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_attr2 = 0;    // char_attr saved for n_attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_attr3 = 0;	    // chars with overruling special attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_attr3 = 0;    // char_attr saved for n_attr3

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_cells = 0;		// nr of cells to skip for w_leftcol or
					// w_skipcol or concealing
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skipped_cells = 0;	// nr of skipped cells for virtual text
					// to be added to wlv.vcol later
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr_pri = FALSE;	// char_attr has priority
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_highlighting = FALSE; // Visual or incsearch highlighting
					   // in this line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vi_attr = 0;		// attributes for Visual and incsearch
					// highlighting
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_attr = 0;		// attributes desired by highlighting
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_attr = 0;	// attributes desired by 'hlsearch' or
					// ComplMatchIns
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol_save_attr = 0;	// saved attr for 'cursorcolumn'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syntax_attr = 0;	// attributes desired by syntax
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_syntax_col = -1;	// column of prev_syntax_attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_syntax_attr = 0;	// syntax_attr at prev_syntax_col
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_syntax = FALSE;	// this buffer has syntax highl.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg;
#endif
#ifdef FEAT_PROP_POPUP
# define WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN 32
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_ଧାଡ଼ି = FALSE;	// set to TRUE when line text done
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_textprop_text_idx = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_next = 0;	// next text property to use
    textprop_T	text_props_buf[WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_idxs_buf[WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN];
    char_u	text_prop_suffix_flags_buf[WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN + 1];
    textprop_T	*text_props = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*text_prop_idxs = ଖାଲି;
    char_u	*text_prop_suffix_flags = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_props_active = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_props_need_sort = FALSE;
    proptype_T  *text_prop_type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_attr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_attr_comb = 0;  // text_prop_attr combined with
					  // syntax_attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_id = 0;	// active property ID
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_above = FALSE;  // first doing virtual text above
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_prop_follows = FALSE;  // another text prop to display
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_ସନ୍ଧାନ_attr = 0;	// search_attr to be used when n_extra
					// goes to zero
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_area_attr = 0;	// idem for area_attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reset_extra_attr = FALSE;
#endif
#ifdef FEAT_SPELL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		can_spell = FALSE;
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];// text with start of the next line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextlinecol = 0;	// column where nextline[] starts
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextline_idx = 0;	// index in nextline[] where next line
					// starts
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		spell_attr = 0;		// attributes desired by spelling
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		word_end = 0;		// last byte with same spell_attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_checked_col = 0;	// checked column for current line
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_check = FALSE;	// has extra highlighting
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		multi_attr = 0;		// attributes desired by multibyte
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_l = 1;		// multi-byte byte length
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_c = 0;		// decoded multi-byte character
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_utf8 = FALSE;	// screen char is UTF-8 char
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u8cc[MAX_MCO];		// composing UTF-8 chars
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_start = MAXCOL;	// first col of changed area
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_end = -1;	// last col of changed area
    diffline_T	ଧାଡ଼ି_changes;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_index = -1;
#endif
    colnr_T	trailcol = MAXCOL;	// start of trailing spaces
    colnr_T	leadcol = 0;		// start of leading spaces
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_multispace = FALSE;	// in multiple consecutive spaces
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		multispace_pos = 0;	// position in lcs-multispace string
#ifdef ଧାଡ଼ି_ATTR
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_attr_save = 0;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sign_present = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_attr = 0;		// attribute for the number column
#ifdef FEAT_ARABIC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_c = 0;		// previous Arabic character
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_c1 = 0;		// first composing char for prev_c
#endif
#ଯଦି defined(ଧାଡ଼ି_ATTR)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_ଧାଡ଼ି_attr = 0;
#endif
#ifdef FEAT_TERMINAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		get_term_attr = FALSE;
#endif

#ଯଦି defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
    // margin columns for the screen line, needed for when 'cursorlineopt'
    // contains "screenline"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ବାମ_curline_col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଡାହାଣ_curline_col = 0;
#endif

#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		feedback_col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		feedback_old_attr = -1;
#endif

#ଯଦି defined(FEAT_CONCEAL) || defined(FEAT_ସନ୍ଧାନ_EXTRA)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match_conc	= 0;	// cchar for match functions
#endif
#ଯଦି defined(FEAT_CONCEAL) || defined(FEAT_ସନ୍ଧାନ_EXTRA) || defined(FEAT_LINEBREAK)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		on_last_col     = FALSE;
#endif
#ifdef FEAT_CONCEAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syntax_flags	= 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syntax_seqnr	= 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_syntax_id	= 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		conceal_attr	= HL_ATTR(HLF_CONCEAL);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_concealing	= FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_wcol	= FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_boguscols   = 0;
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
#ଅନ୍ୟଥା
# define VCOL_HLC (wlv.vcol - wlv.vcol_off_tp)
#endif

    ଯଦି (startrow > endrow)		// past the end already!
	ଫେରନ୍ତୁ startrow;

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

    ଯଦି (ସଂଖ୍ୟା_only == 0)
    {
	// To speed up the loop below, set extra_check when there is linebreak,
	// trailing white space and/or syntax processing to be done.
#ifdef FEAT_LINEBREAK
	extra_check = wp->w_p_lbr;
#endif
#ifdef FEAT_SYN_HL
	ଯଦି (syntax_present(wp) && !wp->w_s->b_syn_error
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
	    ଯଦି (did_emsg)
		wp->w_s->b_syn_error = TRUE;
	    ଅନ୍ୟଥା
	    {
		did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		ଯଦି (!wp->w_s->b_syn_slow)
# endif
		{
		    has_syntax = TRUE;
		    extra_check = TRUE;
		}
	    }
	}

	// Check for columns to display for 'colorcolumn'.
	wlv.color_cols = wp->w_p_cc_cols;
	ଯଦି (wlv.color_cols != ଖାଲି)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);
#endif

#ifdef FEAT_TERMINAL
	ଯଦି (term_show_buffer(wp->w_buffer))
	{
	    extra_check = TRUE;
	    get_term_attr = TRUE;
	    wlv.win_attr = term_get_attr(wp, lnum, -1);
	}
#endif

	// handle Visual active in this window
	ଯଦି (VIsual_active && wp->w_buffer == curwin->w_buffer)
	{
	    pos_T	*ଶୀର୍ଷ, *bot;

	    ଯଦି (LTOREQ_POS(curwin->w_cursor, VIsual))
	    {
		// Visual is after curwin->w_cursor
		ଶୀର୍ଷ = &curwin->w_cursor;
		bot = &VIsual;
	    }
	    ଅନ୍ୟଥା
	    {
		// Visual is before curwin->w_cursor
		ଶୀର୍ଷ = &VIsual;
		bot = &curwin->w_cursor;
	    }
	    lnum_in_visual_area = (lnum >= ଶୀର୍ଷ->lnum && lnum <= bot->lnum);
	    ଯଦି (VIsual_mode == Ctrl_V)
	    {
		// block mode
		ଯଦି (lnum_in_visual_area)
		{
		    wlv.fromcol = wp->w_old_cursor_fcol;
		    wlv.tocol = wp->w_old_cursor_lcol;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// non-block mode
		ଯଦି (lnum > ଶୀର୍ଷ->lnum && lnum <= bot->lnum)
		    wlv.fromcol = 0;
		ଅନ୍ୟଥା ଯଦି (lnum == ଶୀର୍ଷ->lnum)
		{
		    ଯଦି (VIsual_mode == 'V')	// linewise
			wlv.fromcol = 0;
		    ଅନ୍ୟଥା
		    {
			getvvcol(wp, ଶୀର୍ଷ, &wlv.fromcol, ଖାଲି, ଖାଲି, 0);
			ଯଦି (gchar_pos(ଶୀର୍ଷ) == NUL)
			    wlv.tocol = wlv.fromcol + 1;
		    }
		}
		ଯଦି (VIsual_mode != 'V' && lnum == bot->lnum)
		{
		    ଯଦି (*p_sel == 'e' && bot->col == 0 && bot->coladd == 0)
		    {
			wlv.fromcol = -10;
			wlv.tocol = MAXCOL;
		    }
		    ଅନ୍ୟଥା ଯଦି (bot->col == MAXCOL)
			wlv.tocol = MAXCOL;
		    ଅନ୍ୟଥା
		    {
			pos = *bot;
			ଯଦି (*p_sel == 'e')
			    getvvcol(wp, &pos, &wlv.tocol, ଖାଲି, ଖାଲି, 0);
			ଅନ୍ୟଥା
			{
			    getvvcol(wp, &pos, ଖାଲି, ଖାଲି, &wlv.tocol, 0);
			    ++wlv.tocol;
			}
		    }
		}
	    }

	    // if inverting in this line set area_highlighting
	    ଯଦି (wlv.fromcol >= 0)
	    {
		area_highlighting = TRUE;
		vi_attr = HL_ATTR(HLF_V);
#ଯଦି defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
		ଯଦି ((clip_star.available && !clip_star.owned
						    && clip_isautosel_star())
			|| (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		    vi_attr = HL_ATTR(HLF_VNC);
#endif
	    }
	}

	// handle 'incsearch' and ":s///c" highlighting
	ଅନ୍ୟଥା ଯଦି (highlight_match
		&& wp == curwin
		&& lnum >= curwin->w_cursor.lnum
		&& lnum <= curwin->w_cursor.lnum + ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ)
	{
	    ଯଦି (lnum == curwin->w_cursor.lnum)
		getvcol(curwin, &(curwin->w_cursor),
						&wlv.fromcol, ଖାଲି, ଖାଲି, 0);
	    ଅନ୍ୟଥା
		wlv.fromcol = 0;
	    ଯଦି (lnum == curwin->w_cursor.lnum + ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ)
	    {
		pos.lnum = lnum;
		pos.col = ସନ୍ଧାନ_match_endcol;
		getvcol(curwin, &pos, &wlv.tocol, ଖାଲି, ଖାଲି, 0);
	    }
	    ଅନ୍ୟଥା
		wlv.tocol = MAXCOL;
	    // do at least one character; happens when past end of line
	    ଯଦି (wlv.fromcol == wlv.tocol && ସନ୍ଧାନ_match_endcol)
		wlv.tocol = wlv.fromcol + 1;
	    area_highlighting = TRUE;
	    vi_attr = HL_ATTR(HLF_I);
	}
    }

#ifdef FEAT_DIFF

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା linestatus = 0;
    wlv.filler_ଧାଡ଼ିଗୁଡ଼ିକ = diff_check_with_linestatus(wp, lnum, &linestatus);

    CLEAR_FIELD(ଧାଡ଼ି_changes);

    ଯଦି (linestatus < 0)
    {
	ଯଦି (linestatus == -1)
	{
	    ଯଦି (diff_find_change(wp, lnum, &ଧାଡ଼ି_changes))
	    {
		wlv.diff_hlf = HLF_ADD;	// added line
	    }
	    ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି_changes.ଅଙ୍କ_changes > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା added = diff_change_parse(
			&ଧାଡ଼ି_changes, &ଧାଡ଼ି_changes.changes[0],
			&change_start, &change_end);
		ଯଦି (change_start == 0)
		{
		    ଯଦି (added)
			wlv.diff_hlf = HLF_TXA;	// added text on changed line
		    ଅନ୍ୟଥା
			wlv.diff_hlf = HLF_TXD;	// changed text on changed line
		}
		ଅନ୍ୟଥା
		    wlv.diff_hlf = HLF_CHD;	// unchanged text on changed line
		change_index = 0;
	    }
	    ଅନ୍ୟଥା
	    {
		wlv.diff_hlf = HLF_CHD;	// changed line
		change_index = 0;
	    }
	}
	ଅନ୍ୟଥା
	    wlv.diff_hlf = HLF_ADD;

	area_highlighting = TRUE;
    }

    ଯଦି (lnum == wp->w_topline)
	wlv.filler_ଧାଡ଼ିଗୁଡ଼ିକ = wp->w_topfill;

    wlv.filler_todo = wlv.filler_ଧାଡ଼ିଗୁଡ଼ିକ;
#endif

#ifdef FEAT_SIGNS
    sign_present = buf_get_signattrs(wp, lnum, &wlv.sattr);
    ଯଦି (sign_present)
	ଅଙ୍କ_attr = wlv.sattr.sat_numhl;
#endif

#ifdef ଧାଡ଼ି_ATTR
# ifdef FEAT_SIGNS
    // If this line has a sign with line highlighting set wlv.line_attr.
    ଯଦି (sign_present)
	wlv.ଧାଡ଼ି_attr = wlv.sattr.sat_linehl;
# endif
# ଯଦି defined(FEAT_QUICKFIX)
    // Highlight the current line in the quickfix window.
    ଯଦି (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	wlv.ଧାଡ଼ି_attr = HL_ATTR(HLF_QFL);
# endif
    ଯଦି (wlv.ଧାଡ଼ି_attr != 0)
	area_highlighting = TRUE;
#endif

#ifdef FEAT_SPELL
    ଯଦି (spv->spv_has_spell && ସଂଖ୍ୟା_only == 0)
    {
	// Prepare for spell checking.
	extra_check = TRUE;

	// When a word wrapped from the previous line the start of the
	// current line is valid.
	ଯଦି (lnum == spv->spv_checked_lnum)
	    cur_checked_col = spv->spv_checked_col;
	// Previous line was not spell checked, check for capital. This happens
	// for the first line in an updated region or after a closed fold.
	ଯଦି (spv->spv_capcol_lnum == 0 && check_need_cap(wp, lnum, 0))
	    spv->spv_cap_col = 0;
	ଅନ୍ୟଥା ଯଦି (lnum != spv->spv_capcol_lnum)
	    spv->spv_cap_col = -1;
	spv->spv_checked_lnum = 0;

	// Get the start of the next line, so that words that wrap to the
	// next line are found too: "et<line-break>al.".
	// Trick: skip a few chars for C/shell/Vim comments
	nextline[SPWORDLEN] = NUL;
	ଯଦି (lnum < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_ଧାଡ଼ି(nextline + SPWORDLEN, ଧାଡ଼ି, SPWORDLEN);
	}
	ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);

	// If current line is empty, check first word in next line for capital.
	ptr = skipwhite(ଧାଡ଼ି);
	ଯଦି (*ptr == NUL)
	{
	    spv->spv_cap_col = 0;
	    spv->spv_capcol_lnum = lnum + 1;
	}
	// For checking first word with a capital skip white space.
	ଅନ୍ୟଥା ଯଦି (spv->spv_cap_col == 0)
	    spv->spv_cap_col = ptr - ଧାଡ଼ି;

	// Copy the end of the current line into nextline[].
	ଯଦି (nextline[SPWORDLEN] == NUL)
	{
	    // No next line or it is empty.
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	ଅନ୍ୟଥା
	{
	    v = ml_get_buf_len(wp->w_buffer, lnum);
	    ଯଦି (v < SPWORDLEN)
	    {
		// Short line, use it completely and append the start of the
		// next line.
		nextlinecol = 0;
		mch_memmove(nextline, ଧାଡ଼ି, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    ଅନ୍ୟଥା
	    {
		// Long line, use only the last SPWORDLEN bytes.
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, ଧାଡ଼ି + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = ଧାଡ଼ି;

    ଯଦି (wp->w_p_list)
    {
	ଯଦି (wp->w_lcs_chars.space
		|| wp->w_lcs_chars.multispace != ଖାଲି
		|| wp->w_lcs_chars.leadmultispace != ଖାଲି
		|| wp->w_lcs_chars.trail
		|| wp->w_lcs_chars.lead
		|| wp->w_lcs_chars.nbsp)
	    extra_check = TRUE;

	// find start of trailing whitespace
	ଯଦି (wp->w_lcs_chars.trail)
	{
	    trailcol = ml_get_buf_len(wp->w_buffer, lnum);
	    ଯେତେବେଳେ (trailcol > (colnr_T)0 && VIM_ISWHITE(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T)(ptr - ଧାଡ଼ି);
	}
	// find end of leading whitespace
	ଯଦି (wp->w_lcs_chars.lead || wp->w_lcs_chars.leadmultispace != ଖାଲି ||
	    wp->w_lcs_chars.leadtab1 != NUL)
	{
	    leadcol = 0;
	    ଯେତେବେଳେ (VIM_ISWHITE(ptr[leadcol]))
		++leadcol;
	    ଯଦି (ptr[leadcol] == NUL)
		// in a line full of spaces all of them are treated as trailing
		leadcol = (colnr_T)0;
	    ଅନ୍ୟଥା
		// keep track of the first column not filled with spaces
		leadcol += (colnr_T)(ptr - ଧାଡ଼ି) + 1;
	}
    }

    ଯଦି (get_win_attr(wp) != 0)
    {
	wlv.win_attr = get_win_attr(wp);
	area_highlighting = TRUE;
    }

    // When w_skipcol is non-zero and there is virtual text above the actual
    // text, then this much of the virtual text is skipped.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା skipcol_in_text_prop_above = 0;

#ifdef FEAT_PROP_POPUP
    ଯଦି (WIN_IS_POPUP(wp))
	wlv.screen_ଧାଡ଼ି_flags |= SLF_POPUP;

    char_u *prop_start;
    text_prop_ଗଣନା = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
    ଯଦି (text_prop_ଗଣନା > 0)
    {
	// Make a copy of the properties, so that they are properly
	// aligned.
	ଯଦି (text_prop_ଗଣନା <= WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN)
	    text_props = text_props_buf;
	ଅନ୍ୟଥା
	    text_props = ALLOC_MULT(textprop_T, text_prop_ଗଣନା);
	ଯଦି (text_props != ଖାଲି)
	{
	    mch_memmove(text_props, prop_start,
				     text_prop_ଗଣନା * ଆକାର(textprop_T));

	    // Convert tp_text_offset to tp_text pointer for virtual
	    // text properties.  prop_start points into the memline
	    // after the prop_count field.
	    char_u *ଗଣନା_ptr = prop_start - PROP_ଗଣନା_SIZE;

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < text_prop_ଗଣନା; ++i)
	    {
		ଯଦି (text_props[i].tp_id < 0
				&& text_props[i].u.tp_text_offset > 0)
		{
		    text_props[i].u.tp_text =
			    ଗଣନା_ptr + text_props[i].u.tp_text_offset;
		    text_props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
		}
		ଅନ୍ୟଥା
		    text_props[i].u.tp_text = ଖାଲି;
	    }
	}

	// Allocate an array for the indexes.
	ଯଦି (text_prop_ଗଣନା <= WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN)
	    text_prop_idxs = text_prop_idxs_buf;
	ଅନ୍ୟଥା
	    text_prop_idxs = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, text_prop_ଗଣନା);
	ଯଦି (text_prop_idxs == ଖାଲି)
	{
	    ଯଦି (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    ଅନ୍ୟଥା
		text_props = ଖାଲି;
	}
	ଯଦି (text_prop_ଗଣନା <= WIN_ଧାଡ଼ି_TEXT_PROP_ଗଦା_LEN)
	    text_prop_suffix_flags = text_prop_suffix_flags_buf;
	ଅନ୍ୟଥା
	    text_prop_suffix_flags = ALLOC_MULT(char_u, text_prop_ଗଣନା + 1);
	ଯଦି (text_prop_suffix_flags == ଖାଲି)
	{
	    ଯଦି (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    text_prop_idxs = ଖାଲି;
	    ଯଦି (text_props != text_props_buf)
		VIM_CLEAR(text_props);
	    ଅନ୍ୟଥା
		text_props = ଖାଲି;
	}

	ଯଦି (text_props != ଖାଲି)
	{
	    area_highlighting = TRUE;
	    extra_check = TRUE;

	    /* Find the last text property that inserts text. */
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < text_prop_ଗଣନା; ++i)
		ଯଦି (text_props[i].tp_id < 0)
		    last_textprop_text_idx = i;

	    // Text props "above" move the line number down to where the text
	    // is.  Only count the ones that are visible, not those that are
	    // skipped because of w_skipcol.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା text_width = wp->w_width - win_col_off(wp);
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = text_prop_ଗଣନା - 1; i >= 0; --i)
		ଯଦି (text_props[i].tp_flags & TP_FLAG_ALIGN_ABOVE)
		{
		    ଯଦି (lnum == wp->w_topline
			    && wp->w_skipcol - skipcol_in_text_prop_above
								 >= text_width)
		    {
			// This virtual text above is skipped, remove it from
			// the array.
			skipcol_in_text_prop_above += text_width;
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = i + 1; j < text_prop_ଗଣନା; ++j)
			    text_props[j - 1] = text_props[j];
			++i;
			--text_prop_ଗଣନା;
		    }
		    ଅନ୍ୟଥା
			++wlv.text_prop_above_ଗଣନା;
		}

	    text_prop_suffix_flags[text_prop_ଗଣନା] = 0;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = text_prop_ଗଣନା - 1; i >= 0; --i)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = text_prop_suffix_flags[i + 1];

		ଯଦି (text_props[i].tp_col == MAXCOL)
		{
		    flags |= 1;
		    ଯଦି (text_props[i].tp_flags & TP_FLAG_ALIGN_BELOW)
			flags |= 2;
		}
		text_prop_suffix_flags[i] = flags;
	    }
	}
    }

    ଯଦି (ସଂଖ୍ୟା_only > 0)
    {
	// skip over rows only used for virtual text above
	wlv.row += wlv.text_prop_above_ଗଣନା;
	ଯଦି (wlv.row >= endrow)
	{
	    ଯଦି (text_props != text_props_buf)
		vim_free(text_props);
	    ଯଦି (text_prop_idxs != text_prop_idxs_buf)
		vim_free(text_prop_idxs);
	    ଯଦି (text_prop_suffix_flags != text_prop_suffix_flags_buf)
		vim_free(text_prop_suffix_flags);
	    ଫେରନ୍ତୁ wlv.row;
	}
	wlv.screen_row += wlv.text_prop_above_ଗଣନା;
    }
#endif

#ଯଦି defined(FEAT_LINEBREAK) && defined(FEAT_PROP_POPUP)
    colnr_T vcol_first_char = 0;
    ଯଦି (wp->w_p_lbr && ସଂଖ୍ୟା_only == 0)
    {
	chartabsize_T cts;
	init_chartabsize_arg(&cts, wp, lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
	(ଶୂନ୍ୟ)win_lbr_chartabsize(&cts, ଖାଲି, ଖାଲି);
	vcol_first_char = cts.cts_first_char;
	clear_chartabsize_arg(&cts);
    }
#endif

    // 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
    // first character to be displayed.
    ଯଦି (wp->w_p_wrap)
	v = startrow == 0 ? wp->w_skipcol - skipcol_in_text_prop_above : 0;
    ଅନ୍ୟଥା
	v = wp->w_leftcol;
    ଯଦି (v > 0 && ସଂଖ୍ୟା_only == 0)
    {
	char_u		*prev_ptr = ptr;
	chartabsize_T	cts;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charsize = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		head = 0;

	init_chartabsize_arg(&cts, wp, lnum, wlv.vcol, ଧାଡ଼ି, ptr);
	cts.cts_max_head_vcol = v;
	ଯେତେବେଳେ (cts.cts_vcol < v)
	{
	    head = 0;
	    charsize = win_lbr_chartabsize(&cts, &head, ଖାଲି);
	    cts.cts_vcol += charsize;
	    prev_ptr = cts.cts_ptr;
	    ଯଦି (*prev_ptr == NUL)
		ଭାଙ୍ଗ;
	    MB_PTR_ADV(cts.cts_ptr);
	    ଯଦି (wp->w_p_list)
	    {
		in_multispace = *prev_ptr == ' ' && (*cts.cts_ptr == ' '
				  || (prev_ptr > ଧାଡ଼ି && prev_ptr[-1] == ' '));
		ଯଦି (!in_multispace)
		    multispace_pos = 0;
		ଅନ୍ୟଥା ଯଦି (cts.cts_ptr >= ଧାଡ଼ି + leadcol
					 && wp->w_lcs_chars.multispace != ଖାଲି)
		{
		    ++multispace_pos;
		    ଯଦି (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		}
		ଅନ୍ୟଥା ଯଦି (cts.cts_ptr < ଧାଡ଼ି + leadcol
				     && wp->w_lcs_chars.leadmultispace != ଖାଲି)
		{
		    ++multispace_pos;
		    ଯଦି (wp->w_lcs_chars.leadmultispace[multispace_pos] == NUL)
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
	ଯଦି (wlv.vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || wlv.draw_color_col ||
#endif
	     virtual_active() ||
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	    wlv.vcol = v;

	// Handle a character that's not completely on the screen: Put ptr at
	// that character but skip the first few screen characters.
	ଯଦି (wlv.vcol > v)
	{
	    wlv.vcol -= charsize;
	    ptr = prev_ptr;
	}
	ଯଦି (v > wlv.vcol)
	    skip_cells = v - wlv.vcol - head;

	// Adjust for when the inverted text is before the screen,
	// and when the start of the inverted text is before the screen.
	ଯଦି (wlv.tocol <= wlv.vcol)
	    wlv.fromcol = 0;
	ଅନ୍ୟଥା ଯଦି (wlv.fromcol >= 0 && wlv.fromcol < wlv.vcol)
	    wlv.fromcol = wlv.vcol;

#ifdef FEAT_LINEBREAK
	// When w_skipcol is non-zero, first line needs 'showbreak'
	ଯଦି (wp->w_p_wrap)
	    wlv.need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	// When spell checking a word we need to figure out the start of the
	// word and if it's badly spelled or not.
	ଯଦି (spv->spv_has_spell)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	    colnr_T	linecol = (colnr_T)(ptr - ଧାଡ଼ି);
	    hlf_T	spell_hlf = HLF_ଗଣନା;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, SMT_ALL, TRUE, &spell_hlf);

	    // spell_move_to() may call ml_get() and make "line" invalid
	    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = ଧାଡ଼ି + linecol;

	    ଯଦି (len == 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_cursor.col > linecol)
	    {
		// no bad word found at line start, don't check until end of a
		// word
		spell_hlf = HLF_ଗଣନା;
		word_end = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(spell_to_word_end(ptr, wp) - ଧାଡ଼ି + 1);
	    }
	    ଅନ୍ୟଥା
	    {
		// bad word found, use attributes until end of word
		word_end = wp->w_cursor.col + len + 1;

		// Turn index into actual attributes.
		ଯଦି (spell_hlf != HLF_ଗଣନା)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    // Need to restart syntax highlighting for this line.
	    ଯଦି (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    ଯଦି (wlv.fromcol >= wlv.tocol)
	wlv.fromcol = -1;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ଯଦି (ସଂଖ୍ୟା_only == 0)
    {
	v = (long)(ptr - ଧାଡ଼ି);
	area_highlighting |= prepare_ସନ୍ଧାନ_hl_ଧାଡ଼ି(wp, lnum, (colnr_T)v,
					      &ଧାଡ଼ି, &screen_ସନ୍ଧାନ_hl,
					      &ସନ୍ଧାନ_attr);
	ptr = ଧାଡ଼ି + v; // "line" may have been updated
    }
#endif

    ଯଦି ((State & MODE_ଭର୍ତ୍ତି) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
	area_highlighting = TRUE;

#ifdef FEAT_SYN_HL
    // Cursor line highlighting for 'cursorline' in the current window.
    ଯଦି (wp->w_p_cul && lnum == wp->w_cursor.lnum)
    {
	// Do not show the cursor line in the text when Visual mode is active,
	// because it's not clear what is selected then.
	ଯଦି (!(wp == curwin && VIsual_active)
					 && wp->w_p_culopt_flags != CULOPT_NBR)
	{
	    wlv.cul_screenline = (wp->w_p_wrap
				   && (wp->w_p_culopt_flags & CULOPT_SCRLINE));

	    // Only apply CursorLine highlight here when "screenline" is not
	    // present in 'cursorlineopt'.  Otherwise it's done later.
	    ଯଦି (!wlv.cul_screenline)
		apply_cursorline_highlight(&wlv, sign_present);
	    ଅନ୍ୟଥା
	    {
		ଧାଡ଼ି_attr_save = wlv.ଧାଡ଼ି_attr;
		margin_columns_win(wp, &ବାମ_curline_col, &ଡାହାଣ_curline_col);
	    }
	    area_highlighting = TRUE;
	}
    }
#endif

    win_ଧାଡ଼ି_start(wp, &wlv, FALSE);

    // Repeat for the whole displayed line.
    ପାଇଁ (;;)
    {
#ଯଦି defined(FEAT_CONCEAL) || defined(FEAT_ସନ୍ଧାନ_EXTRA)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_match_conc = 0;	// match wants to conceal
#endif
#ifdef FEAT_CONCEAL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_decrement_ptr = FALSE;
#endif

	// Skip this quickly when working on the text.
	ଯଦି (wlv.draw_state != WL_ଧାଡ଼ି)
	{
#ifdef FEAT_SYN_HL
	    ଯଦି (wlv.cul_screenline)
	    {
		wlv.cul_attr = 0;
		wlv.ଧାଡ଼ି_attr = ଧାଡ଼ି_attr_save;
	    }
#endif
	    ଯଦି (wlv.draw_state == WL_CMDLINE - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_CMDLINE;
		ଯଦି (wp == cmdwin_win)
		{
		    wlv.n_extra = 1;
		    wlv.c_final = NUL;
		    ଯଦି (wlv.row == wlv.startrow)
		    {
			wlv.c_extra = cmdwin_type;
			wlv.char_attr = hl_combine_attr(
				    get_win_attr(wp), HL_ATTR(HLF_AT));
		    }
		    ଅନ୍ୟଥା
		    {
			wlv.c_extra = ' ';
			wlv.char_attr = get_win_attr(wp);
		    }
		}
	    }
#ifdef FEAT_FOLDING
	    ଯଦି (wlv.draw_state == WL_FOLD - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_FOLD;
		handle_foldcolumn(wp, &wlv);
	    }
#endif
#ifdef FEAT_SIGNS
	    ଯଦି (wlv.draw_state == WL_SIGN - 1 && wlv.n_extra == 0)
	    {
		// Show the sign column when desired or when using Netbeans.
		wlv.draw_state = WL_SIGN;
		ଯଦି (signcolumn_on(wp))
		    get_sign_display_info(FALSE, wp, &wlv);
	    }
#endif
	    ଯଦି (wlv.draw_state == WL_NR - 1 && wlv.n_extra == 0)
	    {
		// Show the line number, if desired.
		wlv.draw_state = WL_NR;
		handle_lnum_col(wp, &wlv, sign_present, ଅଙ୍କ_attr);
	    }

	    // When only displaying the (relative) line number and that's done,
	    // stop here.
	    ଯଦି (ସଂଖ୍ୟା_only > 0 && wlv.draw_state == WL_NR && wlv.n_extra == 0)
	    {
		wlv_screen_ଧାଡ଼ି(wp, &wlv, FALSE);
		// Need to update more screen lines if:
		// - LineNrAbove or LineNrBelow is used, or
		// - still drawing filler lines.
		ଯଦି ((wlv.row + 1 - wlv.startrow < ସଂଖ୍ୟା_only
			&& (HL_ATTR(HLF_LNA) != 0 || HL_ATTR(HLF_LNB) != 0))
#ifdef FEAT_DIFF
			|| wlv.filler_todo > 0
#endif
			)
		{
		    ++wlv.row;
		    ++wlv.screen_row;
		    ଯଦି (wlv.row == endrow)
			ଭାଙ୍ଗ;
#ifdef FEAT_DIFF
		    --wlv.filler_todo;
		    ଯଦି (wlv.filler_todo == 0 && wp->w_botfill)
			ଭାଙ୍ଗ;
#endif
		    win_ଧାଡ଼ି_start(wp, &wlv, TRUE);
		    ଜାରି;
		}
		ଅନ୍ୟଥା
		    ଭାଙ୍ଗ;
	    }

#ifdef FEAT_LINEBREAK
	    // Check if 'breakindent' applies and show it.
	    // May change wlv.draw_state to WL_BRI or WL_BRI - 1.
	    ଯଦି (wlv.n_extra == 0)
		handle_breakindent(wp, &wlv);
#endif
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    ଯଦି (wlv.draw_state == WL_SBR - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_SBR;
		handle_showbreak_and_filler(wp, &wlv);
	    }
#endif
	    ଯଦି (wlv.draw_state == WL_ଧାଡ଼ି - 1 && wlv.n_extra == 0)
	    {
		wlv.draw_state = WL_ଧାଡ଼ି;
		win_ଧାଡ଼ି_continue(&wlv);  // use wlv.saved_ values
	    }
	}

#ifdef FEAT_SYN_HL
	ଯଦି (wlv.cul_screenline && wlv.draw_state == WL_ଧାଡ଼ି
		&& wlv.vcol >= ବାମ_curline_col
		&& wlv.vcol < ଡାହାଣ_curline_col)
	    apply_cursorline_highlight(&wlv, sign_present);
#endif

	// When still displaying '$' of change command, stop at cursor.
	ଯଦି (dollar_vcol >= 0 && in_curline && wlv.vcol >= (long)wp->w_virtcol)
	{
	    wlv_screen_ଧାଡ଼ି(wp, &wlv, FALSE);
	    // Pretend we have finished updating the window.  Except when
	    // 'cursorcolumn' is set.
#ifdef FEAT_SYN_HL
	    ଯଦି (wp->w_p_cuc)
		wlv.row = wp->w_cline_row + wp->w_cline_height;
	    ଅନ୍ୟଥା
#endif
		wlv.row = wp->w_height;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (wlv.draw_state == WL_ଧାଡ଼ି && (area_highlighting || extra_check))
	{
#ifdef FEAT_PROP_POPUP
	    ଯଦି (text_props != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pi;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - ଧାଡ଼ି);

		ଯଦି (wlv.n_extra > 0
# ifdef FEAT_LINEBREAK
			&& !in_linebreak
# endif
			)
		    --bcol;  // still working on the previous char, e.g. Tab

		// Check if any active property ends.
		ପାଇଁ (pi = 0; pi < text_props_active; ++pi)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tpi = text_prop_idxs[pi];
		    textprop_T  *tp = &text_props[tpi];

		    // An inline property ends when after the start column plus
		    // length. An "above" property ends when used and n_extra
		    // is zero.
		    ଯଦି ((tp->tp_col != MAXCOL
				       && bcol >= tp->tp_col - 1 + tp->tp_len))
		    {
			ଯଦି (pi + 1 < text_props_active)
			    mch_memmove(text_prop_idxs + pi,
					text_prop_idxs + pi + 1,
					ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
					     * (text_props_active - (pi + 1)));
			--text_props_active;
			text_props_need_sort = TRUE;
			--pi;
# ifdef FEAT_LINEBREAK
			// not exactly right but should work in most cases
			ଯଦି (in_linebreak && syntax_attr == text_prop_attr_comb)
			    syntax_attr = 0;
# endif
		    }
		}

# ifdef FEAT_LINEBREAK
		ଯଦି (wlv.n_extra > 0 && in_linebreak)
		    // not on the next char yet, don't start another prop
		    --bcol;
# endif
		// Add any text property that starts in this column.
		ଯେତେବେଳେ (text_prop_next < text_prop_ଗଣନା)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା active;
		    textprop_T *tp = &text_props[text_prop_next];
		    ଯଦି (tp->tp_col == MAXCOL)
		    {
			ଯଦି (bcol == 0 && (tp->tp_flags & TP_FLAG_ALIGN_ABOVE))
			    active = TRUE;
			ଅନ୍ୟଥା ଯଦି (*ptr != NUL)
			    ଭାଙ୍ଗ;
			ଅନ୍ୟଥା
			{
			    // With 'nowrap' and not in the first screen line only "below"
			    // text prop can show.
			    active = wp->w_p_wrap
				  || wlv.row == startrow
				  || (tp->tp_flags & TP_FLAG_ALIGN_BELOW);
			}
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (bcol < tp->tp_col - 1)
			    ଭାଙ୍ଗ;
			active = bcol <= tp->tp_col - 1 + tp->tp_len;
		    }

		    ଯଦି (active)
		    {
			text_prop_idxs[text_props_active++] = text_prop_next;
			text_props_need_sort = TRUE;
		    }
		    ++text_prop_next;
		}

		ଯଦି (wlv.n_extra == 0 ||
			(!wlv.extra_for_textprop
			 && !(text_prop_type != ଖାଲି &&
			     text_prop_flags & PT_FLAG_OVERRIDE)
		    ))
		{
		    text_prop_attr = 0;
		    text_prop_attr_comb = 0;
		    text_prop_flags = 0;
		    text_prop_type = ଖାଲି;
		    text_prop_id = 0;
		    reset_extra_attr = FALSE;
		}
		ଯଦି (text_props_active > 0 && wlv.n_extra == 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା used_tpi = -1;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା used_attr = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା other_tpi = -1;

		    text_prop_above = FALSE;
		    text_prop_follows = FALSE;

		    ଯଦି (text_props_need_sort)
		    {
			// The active set only changes when a property starts
			// or ends, so avoid sorting again for every column.
			sort_text_props(wp->w_buffer, text_props,
					text_prop_idxs, text_props_active);
			text_props_need_sort = FALSE;
		    }

		    ପାଇଁ (pi = 0; pi < text_props_active; ++pi)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tpi = text_prop_idxs[pi];
			textprop_T  *tp = &text_props[tpi];
			proptype_T  *pt = text_prop_type_by_id(
						    wp->w_buffer, tp->tp_type);

			// Only use a text property that can be displayed.
			// Skip "after" properties when wrap is off and at the
			// end of the window.
			ଯଦି (pt != ଖାଲି
				&& (pt->pt_hl_id > 0 || tp->tp_id < 0)
				&& tp->tp_id != -MAXCOL
				&& !(tp->tp_id < 0
				    && !wp->w_p_wrap
				    && (tp->tp_flags & (TP_FLAG_ALIGN_ଡାହାଣ
						| TP_FLAG_ALIGN_ABOVE
						| TP_FLAG_ALIGN_BELOW)) == 0
				    && wlv.col >= wp->w_width))
			{
			    ଯଦି (tp->tp_col == MAXCOL
				     && *ptr == NUL
				     && ((wp->w_p_list && lcs_eol_one > 0
					     && (tp->tp_flags
						   & TP_FLAG_ALIGN_ABOVE) == 0)
					 || (ptr == ଧାଡ଼ି
						&& !did_ଧାଡ଼ି
						&& (tp->tp_flags
						      & TP_FLAG_ALIGN_BELOW))))
			    {
				// skip this prop, first display the '$' after
				// the line or display an empty line
				text_prop_follows = TRUE;
				ଜାରି;
			    }

			    ଯଦି (pt->pt_hl_id > 0)
				used_attr = syn_id2attr(pt->pt_hl_id);
			    text_prop_type = pt;
			    text_prop_attr =
				   hl_combine_attr(text_prop_attr, used_attr);
			    ଯଦି (used_tpi >= 0 && text_props[used_tpi].tp_id < 0)
				other_tpi = used_tpi;
			    text_prop_flags = pt->pt_flags;
			    text_prop_id = tp->tp_id;
			    used_tpi = tpi;
			}
		    }
		    ଯଦି (text_prop_id < 0 && used_tpi >= 0
				 && text_props[used_tpi].u.tp_text != ଖାଲି)
		    {
			textprop_T  *tp = &text_props[used_tpi];
			char_u	    *p = tp->u.tp_text;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    above = (tp->tp_flags
							& TP_FLAG_ALIGN_ABOVE);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    bail_out = FALSE;

			// reset the ID in the copy to avoid it being used
			// again
			tp->tp_id = -MAXCOL;

			ଯଦି (p != ଖାଲି)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଡାହାଣ = (tp->tp_flags
							& TP_FLAG_ALIGN_ଡାହାଣ);
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    below = (tp->tp_flags
							& TP_FLAG_ALIGN_BELOW);
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    wrap = tp->tp_col < MAXCOL
					      || (tp->tp_flags & TP_FLAG_WRAP);
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    padding = tp->tp_col == MAXCOL
							  ? tp->tp_padleft : 0;

			    // Insert virtual text before the current
			    // character, or add after the end of the line.
			    wlv.p_extra = p;
			    wlv.c_extra = NUL;
			    wlv.c_final = NUL;
			    wlv.n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
			    wlv.extra_for_textprop = TRUE;
			    wlv.start_extra_for_textprop = TRUE;
			    wlv.extra_attr = hl_combine_attr(wlv.win_attr,
								    used_attr);
			    n_attr = mb_charlen(p);
			    text_prop_attr = 0;
			    text_prop_attr_comb = 0;
			    ଯଦି (*ptr == NUL)
				// don't combine char attr after EOL
				text_prop_flags &= ~PT_FLAG_COMBINE;
# ifdef FEAT_LINEBREAK
			    ଯଦି (text_prop_no_showbreak(tp))
			    {
				wlv.need_showbreak = FALSE;
				wlv.dont_use_showbreak = TRUE;
			    }
# endif
			    ଯଦି ((ଡାହାଣ || above || below || !wrap
					    || padding > 0) && wp->w_width > 2)
			    {
				char_u	*prev_p_extra = wlv.p_extra;
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା	start_ଧାଡ଼ି;

				// Take care of padding, right-align and
				// truncation.
				// Shared with win_lbr_chartabsize(), must do
				// exactly the same.
				start_ଧାଡ଼ି = text_prop_position(wp, tp,
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
				ଯଦି (wlv.p_extra != prev_p_extra)
				{
				    // wlv.p_extra was allocated
				    vim_free(p_extra_free2);
				    p_extra_free2 = wlv.p_extra;
				}

				ଯଦି (above)
				    wlv.vcol_off_tp += vim_strsize(wlv.p_extra);

				ଯଦି (lcs_eol_one < 0
					&& wp->w_p_wrap
					&& wlv.col
					       + wlv.n_extra - 2 > wp->w_width)
				    // don't bail out at end of line
				    text_prop_follows = TRUE;

				// When 'wrap' is off then for "below" we need
				// to start a new line explicitly.
				ଯଦି (start_ଧାଡ଼ି)
				{
				    draw_screen_ଧାଡ଼ି(wp, &wlv);

				    // When line got too long for screen break
				    // here.
				    ଯଦି (wlv.row == endrow)
				    {
					++wlv.row;
					ଭାଙ୍ଗ;
				    }
				    win_ଧାଡ଼ି_start(wp, &wlv, TRUE);
				    bail_out = TRUE;
				}
			    }
			}

			// If the text didn't reach until the first window
			// column we need to skip cells.
			ଯଦି (skip_cells > 0)
			{
			    ଯଦି (wlv.n_extra > skip_cells)
			    {
				wlv.n_extra -= skip_cells;
				wlv.p_extra += skip_cells;
				wlv.n_attr_skip -= skip_cells;
				ଯଦି (wlv.n_attr_skip < 0)
				    wlv.n_attr_skip = 0;
				skipped_cells += skip_cells;
				skip_cells = 0;
			    }
			    ଅନ୍ୟଥା
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
			       & (TP_FLAG_ALIGN_BELOW | TP_FLAG_ALIGN_ଡାହାଣ)));

			ଯଦି (bail_out)
			    // starting a new line for "below"
			    ଜାରି;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (text_prop_next < text_prop_ଗଣନା
			   && ((*ptr != NUL && ptr[mb_ptr2len(ptr)] == NUL)
			       || (!wp->w_p_wrap && wlv.col == wp->w_width - 1)))
		{
		    // When at last-but-one character and a text property
		    // follows after it, we may need to flush the line after
		    // displaying that character.
		    // Or when not wrapping and at the rightmost column.

		    // Use the displayed width so a double-width or <Tab> last
		    // character filling the rightmost column is detected too.
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା only_below_follows = !wp->w_p_wrap
				 && wlv.col + win_chartabsize(wp, ptr, wlv.vcol)
								>= wp->w_width;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା suffix_flags = text_prop_suffix_flags[text_prop_next];

		    text_prop_follows = (suffix_flags
					& (only_below_follows ? 2 : 1)) != 0;
		}
	    }

	    ଯଦି (wlv.start_extra_for_textprop)
	    {
		wlv.start_extra_for_textprop = FALSE;
		// restore search_attr and area_attr when n_extra
		// is down to zero
		saved_ସନ୍ଧାନ_attr = ସନ୍ଧାନ_attr;
		saved_area_attr = area_attr;
		ସନ୍ଧାନ_attr = 0;
		area_attr = 0;
	    }
#endif

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *area_attr_p =
#ifdef FEAT_PROP_POPUP
		wlv.extra_for_textprop ? &saved_area_attr :
#endif
							    &area_attr;

	    // handle Visual or match highlighting in this line
	    ଯଦି (wlv.vcol == wlv.fromcol
		    || (has_mbyte && wlv.vcol + 1 == wlv.fromcol
			&& ((wlv.n_extra == 0 && (*mb_ptr2cells)(ptr) > 1)
			    || (wlv.n_extra > 0 && wlv.p_extra != ଖାଲି
				&& (*mb_ptr2cells)(wlv.p_extra) > 1))))
		*area_attr_p = vi_attr;		// start highlighting
	    ଅନ୍ୟଥା ଯଦି (*area_attr_p != 0 && wlv.vcol == wlv.tocol)
		*area_attr_p = 0;		// stop highlighting

	    ଯଦି (wlv.n_extra == 0)
	    {
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		// Check for start/end of 'hlsearch' and other matches.
		// After end, check for start/end of next match.
		// When another match, have to check for start again.
		v = (long)(ptr - ଧାଡ଼ି);
		ସନ୍ଧାନ_attr = update_ସନ୍ଧାନ_hl(wp, lnum, (colnr_T)v, &ଧାଡ଼ି,
				      &screen_ସନ୍ଧାନ_hl, &has_match_conc,
				      &match_conc, did_ଧାଡ଼ି_attr, lcs_eol_one,
				      &on_last_col);
		ptr = ଧାଡ଼ି + v;  // "line" may have been changed

		// Do not allow a conceal over EOL otherwise EOL will be missed
		// and bad things happen.
		ଯଦି (*ptr == NUL)
		    has_match_conc = 0;
#ଅନ୍ୟଥା
		ସନ୍ଧାନ_attr = 0;
#endif

		// Check if ComplMatchIns highlight is needed.
		ଯଦି ((State & MODE_ଭର୍ତ୍ତି) && ins_compl_win_active(wp)
			    && (in_curline || ins_compl_lnum_in_range(lnum)))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ins_match_attr =
			ins_compl_col_range_attr(lnum, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - ଧାଡ଼ି));
		    ଯଦି (ins_match_attr > 0)
			ସନ୍ଧାନ_attr =
			    hl_combine_attr(ସନ୍ଧାନ_attr, ins_match_attr);
		}
	    }

#ifdef FEAT_DIFF
	    ଯଦି (wlv.diff_hlf != (hlf_T)0)
	    {
		ଯଦି (ଧାଡ଼ି_changes.ଅଙ୍କ_changes > 0
			&& change_index >= 0
			&& change_index < ଧାଡ଼ି_changes.ଅଙ୍କ_changes - 1)
		{
		    ଯଦି (ptr - ଧାଡ଼ି >= ଧାଡ଼ି_changes.changes[change_index+1].dc_start[ଧାଡ଼ି_changes.bufidx])
			change_index += 1;
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା added = FALSE;
		ଯଦି (ଧାଡ଼ି_changes.ଅଙ୍କ_changes > 0 && change_index >= 0 && change_index < ଧାଡ଼ି_changes.ଅଙ୍କ_changes)
		{
		    added = diff_change_parse(&ଧାଡ଼ି_changes,
			    &ଧାଡ଼ି_changes.changes[change_index],
			    &change_start,
			    &change_end);
		}
		// When there is extra text (e.g. virtual text) it gets the
		// diff highlighting for the line, but not for changed text.
		ଯଦି (wlv.diff_hlf == HLF_CHD && ptr - ଧାଡ଼ି >= change_start
							   && wlv.n_extra == 0)
		    wlv.diff_hlf = added ? HLF_TXA : HLF_TXD;	// added/changed text
		ଯଦି ((wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
			&& ((ptr - ଧାଡ଼ି >= change_end && wlv.n_extra == 0)
			       || (wlv.n_extra > 0 && wlv.extra_for_textprop)))
		    wlv.diff_hlf = HLF_CHD;		// changed line
		wlv.ଧାଡ଼ି_attr = HL_ATTR(wlv.diff_hlf);
		ଯଦି (wp->w_p_cul && lnum == wp->w_cursor.lnum
			&& wp->w_p_culopt_flags != CULOPT_NBR
			&& (!wlv.cul_screenline || (wlv.vcol >= ବାମ_curline_col
					    && wlv.vcol <= ଡାହାଣ_curline_col)))
		    wlv.ଧାଡ଼ି_attr = hl_combine_attr(
					  wlv.ଧାଡ଼ି_attr, HL_ATTR(HLF_CUL));
	    }
#endif

#ifdef FEAT_SYN_HL
	    ଯଦି (extra_check && wlv.n_extra == 0)
	    {
		syntax_attr = 0;
# ifdef FEAT_TERMINAL
		ଯଦି (get_term_attr)
		    syntax_attr = term_get_attr(wp, lnum, wlv.vcol);
# endif
		// Get syntax attribute.
		ଯଦି (has_syntax)
		{
		    // Get the syntax attribute for the character.  If there
		    // is an error, disable syntax highlighting.
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    v = (long)(ptr - ଧାଡ଼ି);
		    ଯଦି (v == prev_syntax_col)
			// at same column again
			syntax_attr = prev_syntax_attr;
		    ଅନ୍ୟଥା
		    {
# ifdef FEAT_SPELL
			can_spell = TRUE;
# endif
			syntax_attr = get_syntax_attr((colnr_T)v,
# ifdef FEAT_SPELL
					    spv->spv_has_spell ? &can_spell :
# endif
					    ଖାଲି, FALSE);
			prev_syntax_col = v;
			prev_syntax_attr = syntax_attr;
		    }

		    ଯଦି (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
			syntax_attr = 0;
		    }
		    ଅନ୍ୟଥା
			did_emsg = save_did_emsg;
# ifdef SYN_TIME_LIMIT
		    ଯଦି (wp->w_s->b_syn_slow)
			has_syntax = FALSE;
# endif

		    // Need to get the line again, a multi-line regexp may
		    // have made it invalid.
		    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = ଧାଡ଼ି + v;
# ifdef FEAT_CONCEAL
		    // no concealing past the end of the line, it interferes
		    // with line highlighting
		    ଯଦି (*ptr == NUL)
			syntax_flags = 0;
		    ଅନ୍ୟଥା
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
	    }
# ifdef FEAT_PROP_POPUP
	    // Combine text property highlight into syntax highlight.
	    ଯଦି (text_prop_type != ଖାଲି)
	    {
		ଯଦି (text_prop_flags & PT_FLAG_COMBINE)
		    syntax_attr = hl_combine_attr(syntax_attr, text_prop_attr);
		ଅନ୍ୟଥା
		    syntax_attr = text_prop_attr;
		text_prop_attr_comb = syntax_attr;
	    }
# endif
#endif

	    // Decide which of the highlight attributes to use.
	    attr_pri = TRUE;
#ifdef ଧାଡ଼ି_ATTR
	    ଯଦି (area_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ଧାଡ଼ି_attr, area_attr);
		ଯଦି (!highlight_match)
		    // let search highlight show in Visual area if possible
		    wlv.char_attr = hl_combine_attr(ସନ୍ଧାନ_attr, wlv.char_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ଅନ୍ୟଥା ଯଦି (ସନ୍ଧାନ_attr != 0)
	    {
		wlv.char_attr = hl_combine_attr(wlv.ଧାଡ଼ି_attr, ସନ୍ଧାନ_attr);
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.char_attr);
# endif
	    }
	    ଅନ୍ୟଥା ଯଦି (wlv.ଧାଡ଼ି_attr != 0
		    && ((wlv.fromcol == -10 && wlv.tocol == MAXCOL)
			      || wlv.vcol < wlv.fromcol
			      || wlv.vcol >= wlv.tocol))
	    {
		// Use wlv.line_attr when not in the Visual or 'incsearch' area
# ifdef FEAT_SYN_HL
		wlv.char_attr = hl_combine_attr(syntax_attr, wlv.ଧାଡ଼ି_attr);
# ଅନ୍ୟଥା
		wlv.char_attr = wlv.ଧାଡ଼ି_attr;
# endif
		attr_pri = FALSE;
	    }
#ଅନ୍ୟଥା
	    ଯଦି (area_attr != 0)
		wlv.char_attr = area_attr;
	    ଅନ୍ୟଥା ଯଦି (ସନ୍ଧାନ_attr != 0)
		wlv.char_attr = ସନ୍ଧାନ_attr;
#endif
	    ଅନ୍ୟଥା
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		wlv.char_attr = syntax_attr;
#ଅନ୍ୟଥା
		wlv.char_attr = 0;
#endif
	    }
#ifdef FEAT_PROP_POPUP
	    // override with text property highlight when "override" is TRUE
	    ଯଦି (text_prop_type != ଖାଲି && (text_prop_flags & PT_FLAG_OVERRIDE))
		wlv.char_attr = hl_combine_attr(wlv.char_attr, text_prop_attr);
#endif
	}

	// combine attribute with HLF_WIN
	ଯଦି (wlv.win_attr != 0)
	{
	    ଯଦି (wlv.char_attr == 0)
		wlv.char_attr = wlv.win_attr;
	    ଅନ୍ୟଥା
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
	ଯଦି (wlv.n_extra > 0)
	{
	    ଯଦି (wlv.c_extra != NUL || (wlv.n_extra == 1 && wlv.c_final != NUL))
	    {
		c = (wlv.n_extra == 1 && wlv.c_final != NUL)
						   ? wlv.c_final : wlv.c_extra;
		mb_c = c;	// doesn't handle non-utf-8 multi-byte!
		ଯଦି (enc_utf8 && utf_char2len(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		ଅନ୍ୟଥା
		    mb_utf8 = FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		c = *wlv.p_extra;
		ଯଦି (has_mbyte)
		{
		    mb_c = c;
		    ଯଦି (enc_utf8)
		    {
			// If the UTF-8 character is more than one byte:
			// Decode it into "mb_c".
			mb_l = utfc_ptr2len(wlv.p_extra);
			mb_utf8 = FALSE;
			ଯଦି (mb_l > wlv.n_extra)
			    mb_l = 1;
			ଅନ୍ୟଥା ଯଦି (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(wlv.p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			// if this is a DBCS character, put it in "mb_c"
			mb_l = MB_BYTE2LEN(c);
			ଯଦି (mb_l >= wlv.n_extra)
			    mb_l = 1;
			ଅନ୍ୟଥା ଯଦି (mb_l > 1)
			    mb_c = (c << 8) + wlv.p_extra[1];
		    }
		    ଯଦି (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;

		    // If a double-width char doesn't fit display a '>' in the
		    // last column.
		    ଯଦି ((
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
			ଯଦି (wlv.cul_attr)
			    multi_attr = hl_combine_attr(
						     multi_attr, wlv.cul_attr);
#endif
			multi_attr = hl_combine_attr(wlv.win_attr, multi_attr);

			// put the pointer back to output the double-width
			// character at the start of the next line.
			++wlv.n_extra;
			--wlv.p_extra;
		    }
		    ଅନ୍ୟଥା
		    {
			wlv.n_extra -= mb_l - 1;
			wlv.p_extra += mb_l - 1;
		    }
		}
		++wlv.p_extra;
	    }
	    --wlv.n_extra;
#ଯଦି defined(FEAT_PROP_POPUP)
	    ଯଦି (wlv.n_extra <= 0)
	    {
		// Only restore search_attr and area_attr after "n_extra" in
		// the next screen line is also done.
		ଯଦି (wlv.saved_n_extra <= 0)
		{
		    ଯଦି (ସନ୍ଧାନ_attr == 0)
			ସନ୍ଧାନ_attr = saved_ସନ୍ଧାନ_attr;
		    ଯଦି (area_attr == 0 && *ptr != NUL)
			area_attr = saved_area_attr;

		    ଯଦି (wlv.extra_for_textprop)
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
	ଅନ୍ୟଥା
	{
#ifdef FEAT_LINEBREAK
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c0;
#endif
	    char_u	*prev_ptr = ptr;

	    // Get a character from the line itself.
	    c = *ptr;
#ifdef FEAT_LINEBREAK
	    c0 = *ptr;
#endif
	    ଯଦି (c == NUL)
	    {
#ifdef FEAT_PROP_POPUP
		// text is finished, may display a "below" virtual text
		did_ଧାଡ଼ି = TRUE;
#endif
		// no more cells to skip
		skip_cells = 0;
	    }

	    ଯଦି (has_mbyte)
	    {
		mb_c = c;
		ଯଦି (enc_utf8)
		{
		    // If the UTF-8 character is more than one byte: Decode it
		    // into "mb_c".
		    mb_l = utfc_ptr2len(ptr);
		    mb_utf8 = FALSE;
		    ଯଦି (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			// Overlong encoded ASCII or ASCII with composing char
			// is displayed normally, except a NUL.
			ଯଦି (mb_c < 0x80)
			{
			    c = mb_c;
#ifdef FEAT_LINEBREAK
			    c0 = mb_c;
#endif
			}
			mb_utf8 = TRUE;

			// At start of the line we can have a composing char.
			// Draw it as a space with a composing char.
			ଯଦି (utf_iscomposing(mb_c))
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

			    ପାଇଁ (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    ଯଦି ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c))))
		    {
			// Illegal UTF-8 byte: display as <xx>.
			// Non-BMP character : display as ? or fullwidth ?.
			transchar_hex(wlv.extra, mb_c);
#ifdef FEAT_RIGHTLEFT
			ଯଦି (wp->w_p_rl)		// reverse
			    rl_mirror(wlv.extra);
#endif
			wlv.p_extra = wlv.extra;
			c = *wlv.p_extra;
			mb_c = mb_ptr2char_adv(&wlv.p_extra);
			mb_utf8 = (c >= 0x80);
			wlv.n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(wlv.p_extra);
			wlv.c_extra = NUL;
			wlv.c_final = NUL;
			ଯଦି (area_attr == 0 && ସନ୍ଧାନ_attr == 0)
			{
			    n_attr = wlv.n_extra + 1;
			    wlv.extra_attr = hl_combine_attr(
						 wlv.win_attr, HL_ATTR(HLF_8));
			    saved_attr2 = wlv.char_attr; // save current attr
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
#ifdef FEAT_ARABIC
		    ଅନ୍ୟଥା ଯଦି (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			// Do Arabic shaping.
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pc, pc1, nc;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pcc[MAX_MCO];

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ଯଦି (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			ଅନ୍ୟଥା
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    ଅନ୍ୟଥା
			prev_c = mb_c;
#endif
		}
		ଅନ୍ୟଥା	// enc_dbcs
		{
		    mb_l = MB_BYTE2LEN(c);
		    ଯଦି (mb_l == 0)  // at the NUL at end-of-line
			mb_l = 1;
		    ଅନ୍ୟଥା ଯଦି (mb_l > 1)
		    {
			// We assume a second byte below 32 is illegal.
			// Hopefully this is OK for all double-byte encodings!
			ଯଦି (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			ଅନ୍ୟଥା
			{
			    ଯଦି (ptr[1] == NUL)
			    {
				// head byte at end of line
				mb_l = 1;
				transchar_nonprint(wp->w_buffer, wlv.extra, c);
				wlv.n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(wlv.extra) - 1;
			    }
			    ଅନ୍ୟଥା
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
			    ଯଦି (area_attr == 0 && ସନ୍ଧାନ_attr == 0)
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
		ଯଦି ((
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
		ଅନ୍ୟଥା ଯଦି (*ptr != NUL)
		    ptr += mb_l - 1;

		// If a double-width char doesn't fit at the left side display
		// a '<' in the first column.  Don't do this for unprintable
		// characters.
		ଯଦି (skip_cells > 0 && mb_l > 1 && wlv.n_extra == 0)
		{
		    wlv.n_extra = 1;
		    wlv.c_extra = MB_FILLER_CHAR;
		    wlv.c_final = NUL;
		    c = ' ';
		    ଯଦି (area_attr == 0 && ସନ୍ଧାନ_attr == 0)
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

	    ଯଦି (extra_check)
	    {
#ifdef FEAT_SPELL
		// Check spelling (unless at the end of the line).
		// Only do this when there is no syntax highlighting, the
		// @Spell cluster is not used or the current syntax item
		// contains the @Spell cluster.
		v = (long)(ptr - ଧାଡ଼ି);
		ଯଦି (spv->spv_has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
		    // do not calculate cap_col at the end of the line or when
		    // only white space is following
		    ଯଦି (c != 0 && (*skipwhite(prev_ptr) != NUL) && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*p;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
			hlf_T	spell_hlf = HLF_ଗଣନା;

			ଯଦି (has_mbyte)
			    v -= mb_l - 1;

			// Use nextline[] if possible, it has the start of the
			// next line concatenated.
			ଯଦି ((prev_ptr - ଧାଡ଼ି) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - ଧାଡ଼ି) - nextlinecol;
			ଅନ୍ୟଥା
			    p = prev_ptr;
			spv->spv_cap_col -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(prev_ptr - ଧାଡ଼ି);
			len = spell_check(wp, p, &spell_hlf, &spv->spv_cap_col,
							    spv->spv_unchanged);
			word_end = v + len;

			// In Insert mode only highlight a word that
			// doesn't touch the cursor.
			ଯଦି (spell_hlf != HLF_ଗଣନା
				&& (State & MODE_ଭର୍ତ୍ତି)
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - ଧାଡ଼ି)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_ଗଣନା;
			    spell_redraw_lnum = lnum;
			}

			ଯଦି (spell_hlf == HLF_ଗଣନା && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    // Remember that the good word continues at the
			    // start of the next line.
			    spv->spv_checked_lnum = lnum + 1;
			    spv->spv_checked_col = (p - nextline) + len
								- nextline_idx;
			}

			// Turn index into actual attributes.
			ଯଦି (spell_hlf != HLF_ଗଣନା)
			    spell_attr = highlight_attr[spell_hlf];

			ଯଦି (spv->spv_cap_col > 0)
			{
			    ଯଦି (p != prev_ptr
				    && (p - nextline) + spv->spv_cap_col
								>= nextline_idx)
			    {
				// Remember that the word in the next line
				// must start with a capital.
				spv->spv_capcol_lnum = lnum + 1;
				spv->spv_cap_col = ((p - nextline)
					    + spv->spv_cap_col - nextline_idx);
			    }
			    ଅନ୍ୟଥା
				// Compute the actual column.
				spv->spv_cap_col += (prev_ptr - ଧାଡ଼ି);
			}
		    }
		}
		ଯଦି (spell_attr != 0)
		{
		    ଯଦି (!attr_pri)
			wlv.char_attr = hl_combine_attr(wlv.char_attr,
								   spell_attr);
		    ଅନ୍ୟଥା
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
		ଯଦି ( wp->w_p_lbr && !wlv.need_lbr && c != NUL &&
			!VIM_ISBREAK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*ptr))
		    wlv.need_lbr = TRUE;
#endif
#ifdef FEAT_LINEBREAK
		// Found last space before word: check for line break.
		ଯଦି (wp->w_p_lbr && c0 == c && wlv.need_lbr
				  && VIM_ISBREAK(c) && !VIM_ISBREAK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*ptr))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mb_off = has_mbyte ? (*mb_head_off)(ଧାଡ଼ି, ptr - 1)
									   : 0;
		    char_u  *p = ptr - (mb_off + 1);
		    chartabsize_T cts;


		    colnr_T init_colnr = wlv.vcol;
# ifdef FEAT_PROP_POPUP
		    init_colnr -= vcol_first_char;
# endif
		    init_chartabsize_arg(&cts, wp, lnum, init_colnr, ଧାଡ଼ି, p);

# ifdef FEAT_PROP_POPUP
		    // do not want virtual text counted here
		    cts.cts_has_prop_with_text = FALSE;
# endif
		    // TODO: consider using "tailp" here
		    wlv.n_extra = win_lbr_chartabsize(&cts, ଖାଲି, ଖାଲି) - 1;
		    clear_chartabsize_arg(&cts);

		    ଯଦି (on_last_col && c != TAB)
			// Do not continue search/match highlighting over the
			// line break, but for TABs the highlighting should
			// include the complete width of the character
			ସନ୍ଧାନ_attr = 0;

		    ଯଦି (c == TAB && wlv.n_extra + wlv.col > wp->w_width)
# ifdef FEAT_VARTABS
			wlv.n_extra = tabstop_padding(wlv.vcol,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
# ଅନ୍ୟଥା
			wlv.n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts
				    - wlv.vcol % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts - 1;
# endif

		    wlv.c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
		    wlv.c_final = NUL;
# ifdef FEAT_PROP_POPUP
		    ଯଦି (wlv.n_extra > 0 && c != TAB)
			in_linebreak = TRUE;
# endif
		    ଯଦି (VIM_ISWHITE(c))
		    {
# ifdef FEAT_CONCEAL
			ଯଦି (c == TAB)
			    // See "Tab alignment" below.
			    FIX_FOR_BOGUSCOLS;
# endif
			ଯଦି (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif
		ଯଦି (wp->w_p_list)
		{
		    in_multispace = c == ' ' && (*ptr == ' '
				  || (prev_ptr > ଧାଡ଼ି && prev_ptr[-1] == ' '));
		    ଯଦି (!in_multispace)
			multispace_pos = 0;
		}

		// 'list': Change char 160 to 'nbsp' and space to 'space'
		// setting in 'listchars'.  But not when the character is
		// followed by a composing character (use mb_l to check that).
		ଯଦି (wp->w_p_list
			&& ((((c == 160 && mb_l == 1)
			      || (mb_utf8
				  && ((mb_c == 160 && mb_l == 2)
				      || (mb_c == 0x202f && mb_l == 3))))
			     && wp->w_lcs_chars.nbsp)
			    || (c == ' '
				&& mb_l == 1
				&& (wp->w_lcs_chars.space
				    || (in_multispace
					&& wp->w_lcs_chars.multispace != ଖାଲି))
				&& ptr - ଧାଡ଼ି >= leadcol
				&& ptr - ଧାଡ଼ି <= trailcol)))
		{
		    ଯଦି (in_multispace && wp->w_lcs_chars.multispace != ଖାଲି)
		    {
			c = wp->w_lcs_chars.multispace[multispace_pos++];
			ଯଦି (wp->w_lcs_chars.multispace[multispace_pos] == NUL)
			    multispace_pos = 0;
		    }
		    ଅନ୍ୟଥା
			c = (c == ' ') ? wp->w_lcs_chars.space
					: wp->w_lcs_chars.nbsp;
		    ଯଦି (area_attr == 0 && ସନ୍ଧାନ_attr == 0)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ଯଦି (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ଅନ୍ୟଥା
			mb_utf8 = FALSE;
		}

		ଯଦି (c == ' ' && ((trailcol != MAXCOL && ptr > ଧାଡ଼ି + trailcol)
				    || (leadcol != 0 && ptr < ଧାଡ଼ି + leadcol)))
		{
		    ଯଦି (leadcol != 0 && in_multispace && ptr < ଧାଡ଼ି + leadcol
			    && wp->w_lcs_chars.leadmultispace != ଖାଲି)
		    {
			c = wp->w_lcs_chars.leadmultispace[multispace_pos++];
			ଯଦି (wp->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			    multispace_pos = 0;
		    }

		    ଅନ୍ୟଥା ଯଦି (ptr > ଧାଡ଼ି + trailcol && wp->w_lcs_chars.trail)
			c = wp->w_lcs_chars.trail;

		    ଅନ୍ୟଥା ଯଦି (ptr < ଧାଡ଼ି + leadcol && wp->w_lcs_chars.lead)
			c = wp->w_lcs_chars.lead;

		    ଅନ୍ୟଥା ଯଦି (leadcol != 0 && wp->w_lcs_chars.space)
			c = wp->w_lcs_chars.space;


		    ଯଦି (!attr_pri)
		    {
			n_attr = 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_c = c;
		    ଯଦି (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ଅନ୍ୟଥା
			mb_utf8 = FALSE;
		}
	    }

	    // Handling of non-printable characters. 'isprint' does not apply
	    // to a UTF-8 leading byte, the character was checked above.
	    ଯଦି (!vim_isprintc(c) && !(enc_utf8 && c >= 0x80))
	    {
		// when getting a character from the file, we may have to
		// turn it into something else on the way to putting it
		// into "ScreenLines".
		ଯଦି (c == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tab_len = 0;
		    // Virtual text and 'showbreak' do not count for the size
		    // of a Tab.
		    long    vcol_adjusted = wlv.vcol - wlv.vcol_off_tp;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lcs_tab1 = wp->w_lcs_chars.tab1;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lcs_tab2 = wp->w_lcs_chars.tab2;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lcs_tab3 = wp->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ଯଦି (wp->w_p_list && wp->w_lcs_chars.leadtab1 != NUL &&
							  ptr < ଧାଡ଼ି + leadcol)
		    {
			lcs_tab1 = wp->w_lcs_chars.leadtab1;
			lcs_tab2 = wp->w_lcs_chars.leadtab2;
			lcs_tab3 = wp->w_lcs_chars.leadtab3;
		    }
#ifdef FEAT_LINEBREAK
		    char_u  *sbr = get_showbreak_ମୂଲ୍ୟ(wp);

		    // only adjust the tab_len, when at the first column
		    // after the showbreak value was drawn
		    ଯଦି (*sbr != NUL && wlv.vcol == wlv.vcol_sbr && wp->w_p_wrap)
			vcol_adjusted -= MB_CHARLEN(sbr);
#endif
		    // tab amount depends on current column
#ifdef FEAT_VARTABS
		    tab_len = tabstop_padding(vcol_adjusted,
					      wp->w_buffer->b_p_ts,
					      wp->w_buffer->b_p_vts_array) - 1;
#ଅନ୍ୟଥା
		    tab_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts
			       - vcol_adjusted % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts - 1;
#endif

#ifdef FEAT_LINEBREAK
		    ଯଦି (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    {
			// tab amount depends on current column
			wlv.n_extra = tab_len;
		    }
#ifdef FEAT_LINEBREAK
		    ଅନ୍ୟଥା
		    {
			char_u	*p;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	saved_nextra = wlv.n_extra;

# ifdef FEAT_CONCEAL
			ଯଦି (wlv.vcol_off_co > 0)
			    // there are characters to conceal
			    tab_len += wlv.vcol_off_co;

			// boguscols before FIX_FOR_BOGUSCOLS macro from above
			ଯଦି (wp->w_p_list && lcs_tab1 &&
			    old_boguscols > 0 &&
			    wlv.n_extra > tab_len)
			    tab_len += wlv.n_extra - tab_len;
# endif
			ଯଦି (tab_len > 0)
			{
			    // If wlv.n_extra > 0, it gives the number of chars
			    // to use for a tab, else we need to calculate the
			    // width for a tab.
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tab2_len = mb_char2len(lcs_tab2);
			    len = tab_len * tab2_len;
			    ଯଦି (lcs_tab3)
				len += mb_char2len(lcs_tab3) - tab2_len;
			    ଯଦି (wlv.n_extra > 0)
				len += wlv.n_extra - tab_len;
			    c = lcs_tab1;
			    p = alloc(len + 1);
			    ଯଦି (p == ଖାଲି)
				wlv.n_extra = 0;
			    ଅନ୍ୟଥା
			    {
				vim_memset(p, ' ', len);
				p[len] = NUL;
				vim_free(wlv.p_extra_free);
				wlv.p_extra_free = p;
				ପାଇଁ (i = 0; i < tab_len; i++)
				{
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcs = lcs_tab2;

				    ଯଦି (*p == NUL)
				    {
					tab_len = i;
					ଭାଙ୍ଗ;
				    }

				    // if tab3 is given, use it for the last
				    // char
				    ଯଦି (lcs_tab3 && i == tab_len - 1)
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
				ଯଦି (wlv.vcol_off_co > 0)
				    wlv.n_extra -= wlv.vcol_off_co;
# endif
			    }
			}
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା vc_saved = wlv.vcol_off_co;

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
			ଯଦି (wlv.n_extra == tab_len + vc_saved && wp->w_p_list
						&& wp->w_lcs_chars.tab1)
			    tab_len += vc_saved;
		    }
#endif
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		    ଯଦି (wp->w_p_list)
		    {
			c = (wlv.n_extra == 0 && lcs_tab3)  ? lcs_tab3
							    : lcs_tab1;
#ifdef FEAT_LINEBREAK
			ଯଦି (wp->w_p_lbr && wlv.p_extra != ଖାଲି
							&& *wlv.p_extra != NUL)
			    wlv.c_extra = NUL; // using p_extra from above
			ଅନ୍ୟଥା
#endif
			    wlv.c_extra = lcs_tab2;
			wlv.c_final = lcs_tab3;
			n_attr = tab_len + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
			saved_attr2 = wlv.char_attr; // save current attr
			mb_c = c;
			ଯଦି (enc_utf8 && utf_char2len(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			wlv.c_final = NUL;
			wlv.c_extra = ' ';
			c = ' ';
		    }
		}
		ଅନ୍ୟଥା ଯଦି (c == NUL
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
#ଯଦି defined(FEAT_DIFF) || defined(ଧାଡ଼ି_ATTR)
		    // For a diff line the highlighting continues after the
		    // "$".
		    ଯଦି (
# ifdef FEAT_DIFF
			    wlv.diff_hlf == (hlf_T)0
#  ifdef ଧାଡ଼ି_ATTR
			    &&
#  endif
# endif
# ifdef ଧାଡ଼ି_ATTR
			    wlv.ଧାଡ଼ି_attr == 0
# endif
		       )
#endif
		    {
			// In virtualedit, visual selections may extend
			// beyond end of line.
			ଯଦି (!(area_highlighting && virtual_active()
				       && wlv.tocol != MAXCOL
				       && wlv.vcol < wlv.tocol))
			    wlv.p_extra = (char_u *)"";
			wlv.n_extra = 0;
		    }
		    ଯଦି (wp->w_p_list && wp->w_lcs_chars.eol > 0)
			c = wp->w_lcs_chars.eol;
		    ଅନ୍ୟଥା
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    // put it back at the NUL
		    ଯଦି (!attr_pri)
		    {
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							      HL_ATTR(HLF_AT));
			n_attr = 1;
		    }
		    mb_c = c;
		    ଯଦି (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ଅନ୍ୟଥା
			mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ଅନ୍ୟଥା ଯଦି (c != NUL)
		{
		    wlv.p_extra = transchar_buf(wp->w_buffer, c);
		    ଯଦି (wlv.n_extra == 0)
			wlv.n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    ଯଦି ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(wlv.p_extra);	// reverse "<12>"
#endif
		    wlv.c_extra = NUL;
		    wlv.c_final = NUL;
#ifdef FEAT_LINEBREAK
		    ଯଦି (wp->w_p_lbr)
		    {
			char_u *p;

			c = *wlv.p_extra;
			p = alloc(wlv.n_extra + 1);
			ଯଦି (p == ଖାଲି)
			    wlv.n_extra = 0;
			ଅନ୍ୟଥା
			{
			    vim_memset(p, ' ', wlv.n_extra);
			    STRNCPY(p, wlv.p_extra + 1, STRLEN(wlv.p_extra) - 1);
			    p[wlv.n_extra] = NUL;
			    vim_free(wlv.p_extra_free);
			    wlv.p_extra_free = wlv.p_extra = p;
			}
		    }
		    ଅନ୍ୟଥା
#endif
		    {
			wlv.n_extra = byte2cells(c) - 1;
			c = *wlv.p_extra++;
		    }
		    ଯଦି (!attr_pri)
		    {
			n_attr = wlv.n_extra + 1;
			wlv.extra_attr = hl_combine_attr(wlv.win_attr,
							       HL_ATTR(HLF_8));
#ifdef FEAT_PROP_POPUP
			ଯଦି (text_prop_type != ଖାଲି &&
				 text_prop_flags & PT_FLAG_OVERRIDE)
			    wlv.extra_attr = hl_combine_attr(text_prop_attr,
							       wlv.extra_attr);
#endif

			saved_attr2 = wlv.char_attr; // save current attr
		    }
		    mb_utf8 = FALSE;	// don't draw as UTF-8
		}
		ଅନ୍ୟଥା ଯଦି (VIsual_active
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
#ଯଦି defined(ଧାଡ଼ି_ATTR)
		ଅନ୍ୟଥା ଯଦି ((
# ifdef FEAT_DIFF
			    wlv.diff_hlf != (hlf_T)0 ||
# endif
# ifdef FEAT_TERMINAL
			    wlv.win_attr != 0 ||
# endif
			    wlv.ଧାଡ଼ି_attr != 0
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
		    ++did_ଧାଡ଼ି_attr;

		    // don't do search HL for the rest of the line
		    ଯଦି (wlv.ଧାଡ଼ି_attr != 0 && wlv.char_attr == ସନ୍ଧାନ_attr
					&& (did_ଧାଡ଼ି_attr > 1
					    || (wp->w_p_list &&
						wp->w_lcs_chars.eol > 0)))
			wlv.char_attr = wlv.ଧାଡ଼ି_attr;
# ifdef FEAT_SIGNS
		    // At end of line: if Sign is present with line highlight, reset char_attr
		    // but not when cursorline is active
		    ଯଦି (sign_present && wlv.sattr.sat_linehl > 0 && wlv.draw_state == WL_ଧାଡ଼ି
			 && !(wp->w_p_cul && lnum == wp->w_cursor.lnum))
			wlv.char_attr = wlv.sattr.sat_linehl;
# endif
# ifdef FEAT_DIFF
		    ଯଦି (wlv.diff_hlf == HLF_TXD || wlv.diff_hlf == HLF_TXA)
		    {
			wlv.diff_hlf = HLF_CHD;
			ଯଦି (vi_attr == 0 || wlv.char_attr != vi_attr)
			{
			    wlv.char_attr = HL_ATTR(wlv.diff_hlf);
			    ଯଦି (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR
				    && (!wlv.cul_screenline
					|| (wlv.vcol >= ବାମ_curline_col
					    && wlv.vcol <= ଡାହାଣ_curline_col)))
				wlv.char_attr = hl_combine_attr(
					  wlv.char_attr, HL_ATTR(HLF_CUL));
			}
		    }
# endif
# ifdef FEAT_TERMINAL
		    ଯଦି (wlv.win_attr != 0)
		    {
			wlv.char_attr = wlv.win_attr;
			ଯଦି (wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && wp->w_p_culopt_flags != CULOPT_NBR)
			{
			    ଯଦି (!wlv.cul_screenline
				    || (wlv.vcol >= ବାମ_curline_col
					     && wlv.vcol <= ଡାହାଣ_curline_col))
				wlv.char_attr = hl_combine_attr(
					      wlv.char_attr, HL_ATTR(HLF_CUL));
			}
			ଅନ୍ୟଥା ଯଦି (wlv.ଧାଡ଼ି_attr)
			    wlv.char_attr = hl_combine_attr(
						 wlv.char_attr, wlv.ଧାଡ଼ି_attr);
			// Show a Visual or search highlight on the first cell
			// of an empty line, on top of the background color.
			ଯଦି (wlv.vcol == 0)
			{
			    ଯଦି (area_attr != 0)
				wlv.char_attr = hl_combine_attr(
						     wlv.char_attr, area_attr);
			    ଅନ୍ୟଥା ଯଦି (ସନ୍ଧାନ_attr != 0)
				wlv.char_attr = hl_combine_attr(
						   wlv.char_attr, ସନ୍ଧାନ_attr);
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    ଯଦି (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum
						    || conceal_cursor_ଧାଡ଼ି(wp))
		&& ((syntax_flags & HL_CONCEAL) != 0 || has_match_conc > 0)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == ଖାଲି))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା syntax_conceal = (syntax_flags & HL_CONCEAL) != 0;
		wlv.char_attr = conceal_attr;
		ଯଦି (((prev_syntax_id != syntax_seqnr && syntax_conceal)
			    || has_match_conc > 1)
			&& ((syntax_conceal && syn_get_sub_char() != NUL)
				|| (has_match_conc && match_conc)
				|| wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    // First time at this concealed item: display one
		    // character.
		    ଯଦି (has_match_conc && match_conc)
			c = match_conc;
		    ଅନ୍ୟଥା ଯଦି (syntax_conceal && syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    ଅନ୍ୟଥା ଯଦି (wp->w_lcs_chars.conceal != NUL)
			c = wp->w_lcs_chars.conceal;
		    ଅନ୍ୟଥା
			c = ' ';

		    ଯଦି (has_mbyte && (*mb_char2cells)(mb_c) > 1)
			// When the first char to be concealed is double-width,
			// need to advance one more virtual column.
			wlv.n_extra++;

		    mb_c = c;
		    ଯଦି (enc_utf8 && utf_char2len(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    ଅନ୍ୟଥା
			mb_utf8 = FALSE;	// don't draw as UTF-8

		    prev_syntax_id = syntax_seqnr;

		    ଯଦି (wlv.n_extra > 0)
			wlv.vcol_off_co += wlv.n_extra;
		    wlv.vcol += wlv.n_extra;
		    ଯଦି (wp->w_p_wrap && wlv.n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			ଯଦି (wp->w_p_rl)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
			ଅନ୍ୟଥା
# endif
			{
			    wlv.boguscols += wlv.n_extra;
			    wlv.col += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
		ଅନ୍ୟଥା ଯଦି (skip_cells == 0)
		{
		    is_concealing = TRUE;
		    skip_cells = 1;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }

	    ଯଦି (skip_cells > 0 && did_decrement_ptr)
		// not showing the '>', put pointer back to avoid getting stuck
		++ptr;

#endif // FEAT_CONCEAL
	}

#ifdef FEAT_CONCEAL
	// In the cursor line and we may be concealing characters: correct
	// the cursor column when we reach its position.
	// With 'virtualedit' we may never reach cursor position, but we still
	// need to correct the cursor column, so do that at end of line.
	ଯଦି (!did_wcol && wlv.draw_state == WL_ଧାଡ଼ି
		&& in_curline && conceal_cursor_ଧାଡ଼ି(wp)
		&& (wlv.vcol + skip_cells >= wp->w_virtcol || c == NUL))
	{
# ifdef FEAT_RIGHTLEFT
	    ଯଦି (wp->w_p_rl)
		wp->w_wcol = wp->w_width - wlv.col + wlv.boguscols - 1;
	    ଅନ୍ୟଥା
# endif
		wp->w_wcol = wlv.col - wlv.boguscols;
	    // Screen cells concealed before the cursor on this screen line, so
	    // pum_display() can line the menu up with the visible text;
	    // "skip_cells" is the concealed cell at the cursor not yet counted.
	    wp->w_wcol_conceal_off = wlv.vcol_off_co + skip_cells;
	    ଯଦି (wlv.vcol + skip_cells < wp->w_virtcol)
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
	ଯଦି (wlv.n_attr_skip == 0 && n_attr > 0
		&& wlv.draw_state == WL_ଧାଡ଼ି
		&& (!attr_pri
#ifdef FEAT_PROP_POPUP
		    || (text_prop_flags & PT_FLAG_OVERRIDE)
#endif
		   ))
	{
#ifdef ଧାଡ଼ି_ATTR
	    ଯଦି (wlv.ଧାଡ଼ି_attr)
		wlv.char_attr = hl_combine_attr(wlv.ଧାଡ଼ି_attr, wlv.extra_attr);
	    ଅନ୍ୟଥା
#endif
		wlv.char_attr = wlv.extra_attr;
#ifdef FEAT_PROP_POPUP
	    ଯଦି (reset_extra_attr)
	    {
		reset_extra_attr = FALSE;
		wlv.extra_attr = 0;
	    }
#endif
	}

#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// XIM don't send preedit_start and preedit_end, but they send
	// preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	// im_is_preediting() here.
	ଯଦି (p_imst == IM_ON_THE_SPOT
		&& xic != ଖାଲି
		&& lnum == wp->w_cursor.lnum
		&& (State & MODE_ଭର୍ତ୍ତି)
		&& !p_imdisable
		&& im_is_preediting()
		&& wlv.draw_state == WL_ଧାଡ଼ି)
	{
	    colnr_T tcol;

	    ଯଦି (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, ଖାଲି, ଖାଲି, 0);
	    ଅନ୍ୟଥା
		tcol = preedit_end_col;
	    ଯଦି ((long)preedit_start_col <= wlv.vcol && wlv.vcol < (long)tcol)
	    {
		ଯଦି (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = wlv.char_attr;
		}
		wlv.char_attr = im_get_feedback_attr(feedback_col);
		ଯଦି (wlv.char_attr < 0)
		    wlv.char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    ଅନ୍ୟଥା ଯଦି (feedback_old_attr >= 0)
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
	ଯଦି (lcs_prec_todo != NUL
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
	    ଯଦି (has_mbyte && (*mb_char2cells)(mb_c) > 1)
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
	    ଯଦି (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ଅନ୍ୟଥା
		mb_utf8 = FALSE;	// don't draw as UTF-8
	    ଯଦି (!attr_pri)
	    {
		saved_attr3 = wlv.char_attr; // save current attr
		wlv.char_attr = hl_combine_attr(wlv.win_attr, HL_ATTR(HLF_AT));
		n_attr3 = 1;
	    }
	}

	// At end of the text line or just after the last character.
	ଯଦି ((c == NUL
#ଯଦି defined(ଧାଡ଼ି_ATTR)
		|| did_ଧାଡ଼ି_attr == 1
#endif
		) && wlv.eol_hl_off == 0)
	{
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	    // flag to indicate whether prevcol equals startcol of search_hl or
	    // one of the matches
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prevcol_hl_flag = get_prevcol_hl_flag(wp, &screen_ସନ୍ଧାନ_hl,
					      (long)(ptr - ଧାଡ଼ି) - (c == NUL));
#endif
	    // Invert at least one char, used for Visual and empty line or
	    // highlight match at end of line. If it's beyond the last
	    // char on the screen, just overwrite that one (tricky!)  Not
	    // needed when a '$' was displayed for 'list'.
	    ଯଦି (wp->w_lcs_chars.eol == lcs_eol_one
		    && ((area_attr != 0 && wlv.vcol == wlv.fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
			// highlight 'hlsearch' match at end of line
			|| (prevcol_hl_flag
# ifdef FEAT_SYN_HL
			    && !(wp->w_p_cul && lnum == wp->w_cursor.lnum
				    && !(wp == curwin && VIsual_active))
# endif
# ifdef FEAT_DIFF
			    && wlv.diff_hlf == (hlf_T)0
# endif
# ଯଦି defined(ଧାଡ଼ି_ATTR)
			    && did_ଧାଡ଼ି_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

#ifdef FEAT_RIGHTLEFT
		ଯଦି (wp->w_p_rl)
		{
		    ଯଦି (wlv.col < 0)
			n = 1;
		}
		ଅନ୍ୟଥା
#endif
		{
		    ଯଦି (wlv.col >= wp->w_width)
			n = -1;
		}
		ଯଦି (n != 0)
		{
		    // At the window boundary, highlight the last character
		    // instead (better than nothing).
		    wlv.off += n;
		    wlv.col += n;
		}
		ଅନ୍ୟଥା
		{
		    // Add a blank character to highlight.
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = ' ';
		    ଯଦି (enc_utf8)
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[wlv.off] = 0;
		}
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		ଯଦି (area_attr == 0)
		{
		    // Use attributes from match with highest priority among
		    // 'search_hl' and the match list.
		    get_ସନ୍ଧାନ_match_hl(wp, &screen_ସନ୍ଧାନ_hl,
					   (long)(ptr - ଧାଡ଼ି), &wlv.char_attr);
		}
#endif
		ScreenAttrs[wlv.off] = wlv.char_attr;
		ScreenCols[wlv.off] = wlv.vcol;
#ifdef FEAT_RIGHTLEFT
		ଯଦି (wp->w_p_rl)
		{
		    --wlv.col;
		    --wlv.off;
		}
		ଅନ୍ୟଥା
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
	ଯଦି (c == NUL)
	{
#ifdef FEAT_PROP_POPUP
	    ଯଦି (text_prop_follows)
	    {
		// Put the pointer back to the NUL.
		--ptr;
		c = ' ';
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		draw_screen_ଧାଡ଼ି(wp, &wlv);

		// Update w_cline_height and w_cline_folded if the cursor line
		// was updated (saves a call to plines() later).
		ଯଦି (in_curline)
		{
		    curwin->w_cline_row = startrow;
		    curwin->w_cline_height = wlv.row - startrow;
#ifdef FEAT_FOLDING
		    curwin->w_cline_folded = false;
#endif
		    curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
		}
		ଭାଙ୍ଗ;
	    }
	}

	// Show "extends" character from 'listchars' if beyond the line end.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcs_ext = get_lcs_ext(wp);
	ଯଦି (lcs_ext != NUL
		&& wlv.draw_state == WL_ଧାଡ଼ି
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
	    ଯଦି (enc_utf8 && utf_char2len(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    ଅନ୍ୟଥା
		mb_utf8 = FALSE;
	}

#ifdef FEAT_SYN_HL
	// advance to the next 'colorcolumn'
	ଯଦି (wlv.draw_color_col)
	    wlv.draw_color_col = advance_color_col(VCOL_HLC, &wlv.color_cols);

	// Highlight the cursor column if 'cursorcolumn' is set.  But don't
	// highlight the cursor position itself.
	// Also highlight the 'colorcolumn' if it is different than
	// 'cursorcolumn'
	// Also highlight the 'colorcolumn' if 'breakindent' and/or 'showbreak'
	// options are set
	vcol_save_attr = -1;
	ଯଦି (((wlv.draw_state == WL_ଧାଡ଼ି
		    || wlv.draw_state == WL_BRI
		    || wlv.draw_state == WL_SBR)
		&& !lnum_in_visual_area
		&& ସନ୍ଧାନ_attr == 0
		&& area_attr == 0)
# ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
# endif
		)
	{
	    ଯଦି (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr,
							     HL_ATTR(HLF_CUC));
	    }
	    ଅନ୍ୟଥା ଯଦି (wlv.draw_color_col && VCOL_HLC == *wlv.color_cols)
	    {
		vcol_save_attr = wlv.char_attr;
		wlv.char_attr = hl_combine_attr(wlv.char_attr, HL_ATTR(HLF_MC));
	    }
	}
#endif

	// Store character to be displayed.
	// Skip characters that are left of the screen for 'nowrap'.
	ଯଦି (wlv.draw_state < WL_ଧାଡ଼ି || skip_cells <= 0)
	{
	    // Store the character.
#ଯଦି defined(FEAT_RIGHTLEFT)
	    ଯଦି (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		// A double-wide character is: put first half in left cell.
		--wlv.off;
		--wlv.col;
	    }
#endif
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = c;
	    ଯଦି (enc_dbcs == DBCS_JPNU)
	    {
		ଯଦି ((mb_c & 0xff00) == 0x8e00)
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = 0x8e;
		ScreenLines2[wlv.off] = mb_c & 0xff;
	    }
	    ଅନ୍ୟଥା ଯଦି (enc_utf8)
	    {
		ଯଦି (mb_utf8)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[wlv.off] = mb_c;
		    ଯଦି ((c & 0xff) == 0)
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = 0x80;   // avoid storing zero
		    ପାଇଁ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_C[i][wlv.off] = u8cc[i];
			ଯଦି (u8cc[i] == 0)
			    ଭାଙ୍ଗ;
		    }
		}
		ଅନ୍ୟଥା
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[wlv.off] = 0;
	    }
	    ଯଦି (multi_attr)
	    {
		ScreenAttrs[wlv.off] = multi_attr;
		multi_attr = 0;
	    }
	    ଅନ୍ୟଥା
		ScreenAttrs[wlv.off] = wlv.char_attr;

	    ଯଦି (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		    && wlv.filler_todo <= 0
#endif
		    )
		ScreenCols[wlv.off] = wlv.vcol;
	    ଅନ୍ୟଥା
		ScreenCols[wlv.off] = -1;

	    ଯଦି (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		// Need to fill two screen columns.
		++wlv.off;
		++wlv.col;
		ଯଦି (enc_utf8)
		    // UTF-8: Put a 0 in the second screen char.
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = 0;
		ଅନ୍ୟଥା
		    // DBCS: Put second byte in the second screen char.
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = mb_c & 0xff;

		ଯଦି (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
			)
		    ScreenCols[wlv.off] = ++wlv.vcol;
		ଅନ୍ୟଥା
		    ScreenCols[wlv.off] = -1;

		// When "wlv.tocol" is halfway a character, set it to the end
		// of the character, otherwise highlighting won't stop.
		ଯଦି (wlv.tocol == wlv.vcol)
		    ++wlv.tocol;

#ifdef FEAT_RIGHTLEFT
		ଯଦି (wp->w_p_rl)
		{
		    // now it's time to backup one cell
		    --wlv.off;
		    --wlv.col;
		}
#endif
	    }
#ifdef FEAT_RIGHTLEFT
	    ଯଦି (wp->w_p_rl)
	    {
		--wlv.off;
		--wlv.col;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		++wlv.off;
		++wlv.col;
	    }
	}
#ifdef FEAT_CONCEAL
	ଅନ୍ୟଥା ଯଦି (wp->w_p_cole > 0 && is_concealing)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା concealed_wide = has_mbyte && (*mb_char2cells)(mb_c) > 1;

	    --skip_cells;
	    ++wlv.vcol_off_co;
	    ଯଦି (concealed_wide)
	    {
		// When a double-width char is concealed,
		// need to advance one more virtual column.
		++wlv.vcol;
		++wlv.vcol_off_co;
	    }

	    ଯଦି (wlv.n_extra > 0)
		wlv.vcol_off_co += wlv.n_extra;

	    ଯଦି (wp->w_p_wrap)
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
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust_col = !WIN_IS_POPUP(wp);
# ଅନ୍ୟଥା
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust_col = TRUE;
# endif
		ଯଦି (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
# ifdef FEAT_RIGHTLEFT
		    ଯଦି (wp->w_p_rl)
		    {
			ଯଦି (adjust_col)
			{
			    wlv.col -= wlv.n_extra;
			    wlv.boguscols -= wlv.n_extra;
			}
		    }
		    ଅନ୍ୟଥା
# endif
		    {
			ଯଦି (adjust_col)
			{
			    wlv.col += wlv.n_extra;
			    wlv.boguscols += wlv.n_extra;
			}
		    }
		    wlv.n_extra = 0;
		    n_attr = 0;
		}

		ଯଦି (concealed_wide)
		{
		    // Need to fill two screen columns.
# ifdef FEAT_RIGHTLEFT
		    ଯଦି (wp->w_p_rl)
		    {
			ଯଦି (adjust_col)
			{
			    --wlv.boguscols;
			    --wlv.col;
			}
		    }
		    ଅନ୍ୟଥା
# endif
		    {
			ଯଦି (adjust_col)
			{
			    ++wlv.boguscols;
			    ++wlv.col;
			}
		    }
		}

# ifdef FEAT_RIGHTLEFT
		ଯଦି (wp->w_p_rl)
		{
		    ଯଦି (adjust_col)
		    {
			--wlv.boguscols;
			--wlv.col;
		    }
		}
		ଅନ୍ୟଥା
# endif
		{
		    ଯଦି (adjust_col)
		    {
			++wlv.boguscols;
			++wlv.col;
		    }
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (wlv.n_extra > 0)
		{
		    wlv.vcol += wlv.n_extra;
		    wlv.n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif // FEAT_CONCEAL
	ଅନ୍ୟଥା
	    --skip_cells;

	ଯଦି (wlv.draw_state > WL_NR && skipped_cells > 0)
	{
	    wlv.vcol += skipped_cells;
	    skipped_cells = 0;
	}

	// Only advance the "wlv.vcol" when after the 'number' or
	// 'relativenumber' column.
	ଯଦି (wlv.draw_state > WL_NR
#ifdef FEAT_DIFF
		&& wlv.filler_todo <= 0
#endif
		)
	    ++wlv.vcol;

#ifdef FEAT_SYN_HL
	ଯଦି (vcol_save_attr >= 0)
	    wlv.char_attr = vcol_save_attr;
#endif

	// restore attributes after "precedes" in 'listchars'
	ଯଦି (wlv.draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    wlv.char_attr = saved_attr3;

	// restore attributes after last 'listchars' or 'number' char
	ଯଦି (n_attr > 0 && wlv.draw_state == WL_ଧାଡ଼ି
				      && wlv.n_attr_skip == 0 && --n_attr == 0)
	    wlv.char_attr = saved_attr2;
	ଯଦି (wlv.n_attr_skip > 0)
	    --wlv.n_attr_skip;

	// At end of screen line and there is more to come: Display the line
	// so far.  If there is no more to display it is caught above.
	ଯଦି ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (wlv.col < 0) :
#endif
				    (wlv.col >= wp->w_width))
		&& (wlv.draw_state != WL_ଧାଡ଼ି
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
	    ଯଦି (wlv.boguscols != 0 && (
# ifdef ଧାଡ଼ି_ATTR
			wlv.ଧାଡ଼ି_attr != 0 ||
# endif
			wlv.win_attr != 0
			)
	       )
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr = wlv.win_attr;
# ifdef ଧାଡ଼ି_ATTR
		ଯଦି (wlv.ଧାଡ଼ି_attr != 0)
		    attr = hl_combine_attr(attr, wlv.ଧାଡ଼ି_attr);
# endif
		ଯେତେବେଳେ ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? wlv.col >= 0 :
# endif
			    wlv.col < wp->w_width))
		{
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[wlv.off] = ' ';
		    ଯଦି (enc_utf8)
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[wlv.off] = 0;
		    ScreenAttrs[wlv.off] = attr;
		    ScreenCols[wlv.off] = wlv.vcol - 1;
# ifdef FEAT_RIGHTLEFT
		    ଯଦି (wp->w_p_rl)
		    {
			wlv.off--;
			wlv.col--;
			wlv.boguscols++;
		    }
		    ଅନ୍ୟଥା
# endif
		    {
			wlv.off++;
			wlv.col++;
			wlv.boguscols--;
		    }
		}
	    }
	    wlv_screen_ଧାଡ଼ି(wp, &wlv, TRUE);
	    wlv.col += wlv.boguscols;
	    wlv.boguscols = 0;
	    wlv.vcol_off_co = 0;
#ଅନ୍ୟଥା
	    wlv_screen_ଧାଡ଼ି(wp, &wlv, TRUE);
#endif
	    ++wlv.row;
	    ++wlv.screen_row;

	    // When not wrapping and finished diff lines, break here.
	    ଯଦି (!wp->w_p_wrap
#ifdef FEAT_DIFF
			&& wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
			&& !text_prop_above
			&& !text_prop_follows
#endif
		       )
		ଭାଙ୍ଗ;
#ifdef FEAT_PROP_POPUP
	    ଯଦି (!wp->w_p_wrap && text_prop_follows && !text_prop_above)
	    {
		// do not output more of the line, only the "below" prop
		ptr = ଧାଡ଼ି + (size_t)ml_get_buf_len(wp->w_buffer, lnum);
# ifdef FEAT_LINEBREAK
		wlv.dont_use_showbreak = TRUE;
# endif
	    }
#endif

	    // When the window is too narrow draw all "@" lines.
	    ଯଦି (wlv.draw_state != WL_ଧାଡ଼ି
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
	    ଯଦି (wlv.row == endrow)
	    {
		++wlv.row;
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (screen_cur_row == wlv.screen_row - 1
#ifdef FEAT_DIFF
		     && wlv.filler_todo <= 0
#endif
#ifdef FEAT_PROP_POPUP
		     && !text_prop_above && !text_prop_follows
#endif
		     && wp->w_width == Columns)
	    {
		// Remember that the line wraps, used for modeless copy.
		ଧାଡ଼ି_Wraps[wlv.screen_row - 1] = TRUE;

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
		ଯଦି (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
			 && !(has_mbyte
			     && ((*mb_off2cells)(ଧାଡ଼ି_Offset[wlv.screen_row],
				   ଧାଡ଼ି_Offset[wlv.screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(
				     ଧାଡ଼ି_Offset[wlv.screen_row - 1]
							    + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)topframe->fr_width - 2,
				     ଧାଡ଼ି_Offset[wlv.screen_row]
						      + screen_Columns) == 2)))
		{
		    // First make sure we are at the end of the screen line,
		    // then output the same character again to let the
		    // terminal know about the wrap.  If the terminal doesn't
		    // auto-wrap, we overwrite the character.
		    ଯଦି (screen_cur_col != wp->w_width)
			screen_char(ଧାଡ଼ି_Offset[wlv.screen_row - 1]
						       + (unsigned)topframe->fr_width - 1,
				       wlv.screen_row - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(topframe->fr_width - 1));

		    // When there is a multi-byte character, just output a
		    // space to keep it simple.
		    ଯଦି (has_mbyte && MB_BYTE2LEN(Screen_ଧାଡ଼ିଗୁଡ଼ିକ[ଧାଡ଼ି_Offset[
				     wlv.screen_row - 1] + (topframe->fr_width - 1)]) > 1)
			out_char(' ');
		    ଅନ୍ୟଥା
			out_char(Screen_ଧାଡ଼ିଗୁଡ଼ିକ[ଧାଡ଼ି_Offset[wlv.screen_row - 1]
							    + (topframe->fr_width - 1)]);
		    // force a redraw of the first char on the next line
		    ScreenAttrs[ଧାଡ଼ି_Offset[wlv.screen_row]] = (sattr_T)-1;
		    screen_start();	// don't know where cursor is now
		}
	    }

	    win_ଧାଡ଼ି_start(wp, &wlv, TRUE);

	    lcs_prec_todo = wp->w_lcs_chars.prec;
#ifdef FEAT_LINEBREAK
	    ଯଦି (!wlv.dont_use_showbreak
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
	    ଯଦି (wlv.filler_todo == 0 && wp->w_botfill)
		ଭାଙ୍ଗ;
#endif
	}

    }	// for every character in the line
#ifdef FEAT_PROP_POPUP
    ଯଦି (text_props != text_props_buf)
	vim_free(text_props);
    ଯଦି (text_prop_idxs != text_prop_idxs_buf)
	vim_free(text_prop_idxs);
    ଯଦି (text_prop_suffix_flags != text_prop_suffix_flags_buf)
	vim_free(text_prop_suffix_flags);
    vim_free(p_extra_free2);
#endif

    vim_free(wlv.p_extra_free);
    vim_free(wlv.saved_p_extra_free);
    ଫେରନ୍ତୁ wlv.row;
}
