/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#اگر defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    صحیح
find_word_under_cursor(
	صحیح	    mouserow,
	صحیح	    mousecol,
	صحیح	    getword,
	صحیح	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	صحیح	    *colp,	// column where mouse hovers, can be NULL
	صحیح	    *startcolp) // column where text starts, can be NULL
{
    صحیح		row = mouserow;
    صحیح		col = mousecol;
    صحیح		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = پوچ;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    اگر (wp == پوچ || row < 0 || row >= wp->w_height || col >= wp->w_width)
	بازگشت FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    اگر (mouse_comp_pos(wp, &row, &col, &lnum, پوچ))
	بازگشت FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    اگر (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	بازگشت FAIL;		// past end of line

    // Not past end of line.
    اگر (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	صحیح		len;
	pos_T	*spos = پوچ, *epos = پوچ;

	اگر (VIsual_active)
	{
	    اگر (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    وگرنه
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, پوچ);
	scol = col;

	اگر (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (صحیح)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (صحیح)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    اگر (spos->lnum != epos->lnum || spos->col == epos->col)
		بازگشت FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    اگر (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	وگرنه
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    اگر (len == 0)
		بازگشت FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    وگرنه
	scol = col;

    اگر (winp != پوچ)
	*winp = wp;
    اگر (lnump != پوچ)
	*lnump = lnum;
    *textp = lbuf;
    اگر (colp != پوچ)
	*colp = col;
    اگر (startcolp != پوچ)
	*startcolp = scol;

    بازگشت OK;
}
#endif

#اگر defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    صحیح
get_beval_info(
	BalloonEval	*beval,
	صحیح		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	صحیح		*colp)
{
    صحیح		row = mouse_row;
    صحیح		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    اگر (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    اگر (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, پوچ) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	اگر ((*winp)->w_buffer->b_p_vts_array != پوچ && beval->vts == پوچ)
	{
	    اگر (getword)
		vim_free(*textp);
	    بازگشت FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	بازگشت OK;
    }

    بازگشت FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    تهی
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    اگر (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    اگر (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    صحیح
can_use_beval(تهی)
{
    بازگشت (0
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
	     ) && پیام_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ایستا تهی
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	صحیح		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    صحیح		use_sandbox;
    ایستا char_u  *result = پوچ;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    برای (cw = firstwin; cw != wp; cw = cw->w_next)
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
    اگر (use_sandbox)
	++sandbox;
    ++textlock;

    اگر (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	اگر (sp != پوچ)
	    current_sctx = *sp;
    }
    وگرنه
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    اگر (result != پوچ)
    {
	len = STRLEN(result);
	اگر (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    اگر (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, پوچ, -1);
    اگر (result != پوچ && result[0] != NUL)
	post_balloon(beval, result, پوچ);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    اگر (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    تهی
general_beval_cb(BalloonEval *beval, صحیح state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    صحیح		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    ایستا صحیح	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    اگر (!can_use_beval() || beval == پوچ)
	بازگشت;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    اگر (recursive)
	بازگشت;
    recursive = TRUE;

# ifdef FEAT_EVAL
    اگر (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	اگر (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    بازگشت;
	}
	وگرنه
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    اگر (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
