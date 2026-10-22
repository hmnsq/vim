/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_word_under_cursor(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mouserow,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mousecol,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    getword,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *colp,	// column where mouse hovers, can be NULL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *startcolp) // column where text starts, can be NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = mouserow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mousecol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = ଖାଲି;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ଯଦି (wp == ଖାଲି || row < 0 || row >= wp->w_height || col >= wp->w_width)
	ଫେରନ୍ତୁ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ଯଦି (mouse_comp_pos(wp, &row, &col, &lnum, ଖାଲି))
	ଫେରନ୍ତୁ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ଯଦି (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ଫେରନ୍ତୁ FAIL;		// past end of line

    // Not past end of line.
    ଯଦି (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	pos_T	*spos = ଖାଲି, *epos = ଖାଲି;

	ଯଦି (VIsual_active)
	{
	    ଯଦି (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    ଅନ୍ୟଥା
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, ଖାଲି);
	scol = col;

	ଯଦି (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ଯଦି (spos->lnum != epos->lnum || spos->col == epos->col)
		ଫେରନ୍ତୁ FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    ଯଦି (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	ଅନ୍ୟଥା
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ଯଦି (len == 0)
		ଫେରନ୍ତୁ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    ଅନ୍ୟଥା
	scol = col;

    ଯଦି (winp != ଖାଲି)
	*winp = wp;
    ଯଦି (lnump != ଖାଲି)
	*lnump = lnum;
    *textp = lbuf;
    ଯଦି (colp != ଖାଲି)
	*colp = col;
    ଯଦି (startcolp != ଖାଲି)
	*startcolp = scol;

    ଫେରନ୍ତୁ OK;
}
#endif

#ଯଦି defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_beval_info(
	BalloonEval	*beval,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*colp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    ଯଦି (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    ଯଦି (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, ଖାଲି) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	ଯଦି ((*winp)->w_buffer->b_p_vts_array != ଖାଲି && beval->vts == ଖାଲି)
	{
	    ଯଦି (getword)
		vim_free(*textp);
	    ଫେରନ୍ତୁ FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    ଶୂନ୍ୟ
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    ଯଦି (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ଯଦି (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_use_beval(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (0
# ifdef FEAT_BEVAL_GUI
		|| (gui.in_use && p_beval)
# endif
# ifdef FEAT_BEVAL_TERM
		|| (
#  ifdef FEAT_GUI
		    !gui.in_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && ସନ୍ଦେଶ_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox;
    ସ୍ଥିର char_u  *result = ଖାଲି;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    ପାଇଁ (cw = firstwin; cw != wp; cw = cw->w_next)
	++winnr;

    set_vim_var_nr(VV_BEVAL_BUFNR, (long)wp->w_buffer->b_fnum);
    set_vim_var_nr(VV_BEVAL_WINNR, winnr);
    set_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    set_vim_var_nr(VV_BEVAL_LNUM, (long)lnum);
    set_vim_var_nr(VV_BEVAL_COL, (long)(col + 1));
    set_vim_var_string(VV_BEVAL_TEXT, text, -1);
    vim_free(text);

    use_sandbox = was_set_insecurely(wp, (char_u *)"balloonexpr",
			    *wp->w_buffer->b_p_bexpr == NUL ? 0 : OPT_LOCAL);
    ଯଦି (use_sandbox)
	++sandbox;
    ++textlock;

    ଯଦି (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ଯଦି (sp != ଖାଲି)
	    current_sctx = *sp;
    }
    ଅନ୍ୟଥା
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ଯଦି (result != ଖାଲି)
    {
	len = STRLEN(result);
	ଯଦି (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    ଯଦି (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, ଖାଲି, -1);
    ଯଦି (result != ଖାଲି && result[0] != NUL)
	post_balloon(beval, result, ଖାଲି);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ଯଦି (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    ଶୂନ୍ୟ
general_beval_cb(BalloonEval *beval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ଯଦି (!can_use_beval() || beval == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ଯଦି (recursive)
	ଫେରନ୍ତୁ;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ଯଦି (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	ଯଦି (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ଯଦି (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
