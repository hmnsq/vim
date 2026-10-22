/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    முழு
find_word_under_cursor(
	முழு	    mouserow,
	முழு	    mousecol,
	முழு	    getword,
	முழு	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	முழு	    *colp,	// column where mouse hovers, can be NULL
	முழு	    *startcolp) // column where text starts, can be NULL
{
    முழு		row = mouserow;
    முழு		col = mousecol;
    முழு		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = வெற்று;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    எனில் (wp == வெற்று || row < 0 || row >= wp->w_height || col >= wp->w_width)
	திரும்பு FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    எனில் (mouse_comp_pos(wp, &row, &col, &lnum, வெற்று))
	திரும்பு FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    எனில் (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	திரும்பு FAIL;		// past end of line

    // Not past end of line.
    எனில் (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	முழு		len;
	pos_T	*spos = வெற்று, *epos = வெற்று;

	எனில் (VIsual_active)
	{
	    எனில் (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    இல்லையெனில்
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, வெற்று);
	scol = col;

	எனில் (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (முழு)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (முழு)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    எனில் (spos->lnum != epos->lnum || spos->col == epos->col)
		திரும்பு FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    எனில் (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	இல்லையெனில்
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    எனில் (len == 0)
		திரும்பு FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    இல்லையெனில்
	scol = col;

    எனில் (winp != வெற்று)
	*winp = wp;
    எனில் (lnump != வெற்று)
	*lnump = lnum;
    *textp = lbuf;
    எனில் (colp != வெற்று)
	*colp = col;
    எனில் (startcolp != வெற்று)
	*startcolp = scol;

    திரும்பு OK;
}
#endif

#எனில் defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    முழு
get_beval_info(
	BalloonEval	*beval,
	முழு		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	முழு		*colp)
{
    முழு		row = mouse_row;
    முழு		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    எனில் (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    எனில் (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, வெற்று) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	எனில் ((*winp)->w_buffer->b_p_vts_array != வெற்று && beval->vts == வெற்று)
	{
	    எனில் (getword)
		vim_free(*textp);
	    திரும்பு FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	திரும்பு OK;
    }

    திரும்பு FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    வெறுமை
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    எனில் (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    எனில் (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    முழு
can_use_beval(வெறுமை)
{
    திரும்பு (0
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
	     ) && செய்தி_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    நிலையான வெறுமை
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	முழு		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    முழு		use_sandbox;
    நிலையான char_u  *result = வெற்று;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    க்கு (cw = firstwin; cw != wp; cw = cw->w_next)
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
    எனில் (use_sandbox)
	++sandbox;
    ++textlock;

    எனில் (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	எனில் (sp != வெற்று)
	    current_sctx = *sp;
    }
    இல்லையெனில்
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    எனில் (result != வெற்று)
    {
	len = STRLEN(result);
	எனில் (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    எனில் (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, வெற்று, -1);
    எனில் (result != வெற்று && result[0] != NUL)
	post_balloon(beval, result, வெற்று);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    எனில் (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    வெறுமை
general_beval_cb(BalloonEval *beval, முழு state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    முழு		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    நிலையான முழு	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    எனில் (!can_use_beval() || beval == வெற்று)
	திரும்பு;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    எனில் (recursive)
	திரும்பு;
    recursive = TRUE;

# ifdef FEAT_EVAL
    எனில் (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	எனில் (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    திரும்பு;
	}
	இல்லையெனில்
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    எனில் (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
