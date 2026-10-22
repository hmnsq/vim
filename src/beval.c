/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_word_under_cursor(
	ಪೂರ್ಣಾಙ್ಕ	    mouserow,
	ಪೂರ್ಣಾಙ್ಕ	    mousecol,
	ಪೂರ್ಣಾಙ್ಕ	    getword,
	ಪೂರ್ಣಾಙ್ಕ	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	ಪೂರ್ಣಾಙ್ಕ	    *colp,	// column where mouse hovers, can be NULL
	ಪೂರ್ಣಾಙ್ಕ	    *startcolp) // column where text starts, can be NULL
{
    ಪೂರ್ಣಾಙ್ಕ		row = mouserow;
    ಪೂರ್ಣಾಙ್ಕ		col = mousecol;
    ಪೂರ್ಣಾಙ್ಕ		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = ರಿಕ್ತ;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ಯದಿ (wp == ರಿಕ್ತ || row < 0 || row >= wp->w_height || col >= wp->w_width)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ಯದಿ (mouse_comp_pos(wp, &row, &col, &lnum, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ಯದಿ (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ಪ್ರತ್ಯಾಗಮ FAIL;		// past end of line

    // Not past end of line.
    ಯದಿ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	ಪೂರ್ಣಾಙ್ಕ		len;
	pos_T	*spos = ರಿಕ್ತ, *epos = ರಿಕ್ತ;

	ಯದಿ (VIsual_active)
	{
	    ಯದಿ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    ಅನ್ಯಥಾ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, ರಿಕ್ತ);
	scol = col;

	ಯದಿ (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (ಪೂರ್ಣಾಙ್ಕ)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (ಪೂರ್ಣಾಙ್ಕ)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ಯದಿ (spos->lnum != epos->lnum || spos->col == epos->col)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    ಯದಿ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	ಅನ್ಯಥಾ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ಯದಿ (len == 0)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    ಅನ್ಯಥಾ
	scol = col;

    ಯದಿ (winp != ರಿಕ್ತ)
	*winp = wp;
    ಯದಿ (lnump != ರಿಕ್ತ)
	*lnump = lnum;
    *textp = lbuf;
    ಯದಿ (colp != ರಿಕ್ತ)
	*colp = col;
    ಯದಿ (startcolp != ರಿಕ್ತ)
	*startcolp = scol;

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ಯದಿ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_beval_info(
	BalloonEval	*beval,
	ಪೂರ್ಣಾಙ್ಕ		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	ಪೂರ್ಣಾಙ್ಕ		*colp)
{
    ಪೂರ್ಣಾಙ್ಕ		row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    ಯದಿ (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    ಯದಿ (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, ರಿಕ್ತ) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	ಯದಿ ((*winp)->w_buffer->b_p_vts_array != ರಿಕ್ತ && beval->vts == ರಿಕ್ತ)
	{
	    ಯದಿ (getword)
		vim_free(*textp);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    ಶೂನ್ಯ
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ಯದಿ (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    ಪೂರ್ಣಾಙ್ಕ
can_use_beval(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (0
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
	     ) && ಸಂದೇಶ_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	ಪೂರ್ಣಾಙ್ಕ		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox;
    ಸ್ಥಿರ char_u  *result = ರಿಕ್ತ;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    ಪ್ರತಿ (cw = firstwin; cw != wp; cw = cw->w_next)
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
    ಯದಿ (use_sandbox)
	++sandbox;
    ++textlock;

    ಯದಿ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ಯದಿ (sp != ರಿಕ್ತ)
	    current_sctx = *sp;
    }
    ಅನ್ಯಥಾ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ಯದಿ (result != ರಿಕ್ತ)
    {
	len = STRLEN(result);
	ಯದಿ (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    ಯದಿ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, ರಿಕ್ತ, -1);
    ಯದಿ (result != ರಿಕ್ತ && result[0] != NUL)
	post_balloon(beval, result, ರಿಕ್ತ);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ಯದಿ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    ಶೂನ್ಯ
general_beval_cb(BalloonEval *beval, ಪೂರ್ಣಾಙ್ಕ state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ಯದಿ (!can_use_beval() || beval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ಯದಿ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	ಯದಿ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ಯದಿ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
