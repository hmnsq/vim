/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    পূর্ণাঙ্ক
find_word_under_cursor(
	পূর্ণাঙ্ক	    mouserow,
	পূর্ণাঙ্ক	    mousecol,
	পূর্ণাঙ্ক	    getword,
	পূর্ণাঙ্ক	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	পূর্ণাঙ্ক	    *colp,	// column where mouse hovers, can be NULL
	পূর্ণাঙ্ক	    *startcolp) // column where text starts, can be NULL
{
    পূর্ণাঙ্ক		row = mouserow;
    পূর্ণাঙ্ক		col = mousecol;
    পূর্ণাঙ্ক		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = রিক্ত;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    যদি (wp == রিক্ত || row < 0 || row >= wp->w_height || col >= wp->w_width)
	ঘূৰাই FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    যদি (mouse_comp_pos(wp, &row, &col, &lnum, রিক্ত))
	ঘূৰাই FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    যদি (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ঘূৰাই FAIL;		// past end of line

    // Not past end of line.
    যদি (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	পূর্ণাঙ্ক		len;
	pos_T	*spos = রিক্ত, *epos = রিক্ত;

	যদি (VIsual_active)
	{
	    যদি (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    অন্যথা
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, রিক্ত);
	scol = col;

	যদি (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (পূর্ণাঙ্ক)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (পূর্ণাঙ্ক)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    যদি (spos->lnum != epos->lnum || spos->col == epos->col)
		ঘূৰাই FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    যদি (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	অন্যথা
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    যদি (len == 0)
		ঘূৰাই FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    অন্যথা
	scol = col;

    যদি (winp != রিক্ত)
	*winp = wp;
    যদি (lnump != রিক্ত)
	*lnump = lnum;
    *textp = lbuf;
    যদি (colp != রিক্ত)
	*colp = col;
    যদি (startcolp != রিক্ত)
	*startcolp = scol;

    ঘূৰাই OK;
}
#endif

#যদি defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
get_beval_info(
	BalloonEval	*beval,
	পূর্ণাঙ্ক		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	পূর্ণাঙ্ক		*colp)
{
    পূর্ণাঙ্ক		row = mouse_row;
    পূর্ণাঙ্ক		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    যদি (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    যদি (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, রিক্ত) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	যদি ((*winp)->w_buffer->b_p_vts_array != রিক্ত && beval->vts == রিক্ত)
	{
	    যদি (getword)
		vim_free(*textp);
	    ঘূৰাই FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	ঘূৰাই OK;
    }

    ঘূৰাই FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    শূন্য
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    যদি (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    যদি (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    পূর্ণাঙ্ক
can_use_beval(শূন্য)
{
    ঘূৰাই (0
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
	     ) && বাৰ্তা_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    স্থির শূন্য
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	পূর্ণাঙ্ক		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    পূর্ণাঙ্ক		use_sandbox;
    স্থির char_u  *result = রিক্ত;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    বাবে (cw = firstwin; cw != wp; cw = cw->w_next)
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
    যদি (use_sandbox)
	++sandbox;
    ++textlock;

    যদি (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	যদি (sp != রিক্ত)
	    current_sctx = *sp;
    }
    অন্যথা
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    যদি (result != রিক্ত)
    {
	len = STRLEN(result);
	যদি (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    যদি (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, রিক্ত, -1);
    যদি (result != রিক্ত && result[0] != NUL)
	post_balloon(beval, result, রিক্ত);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    যদি (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    শূন্য
general_beval_cb(BalloonEval *beval, পূর্ণাঙ্ক state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    পূর্ণাঙ্ক		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    স্থির পূর্ণাঙ্ক	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    যদি (!can_use_beval() || beval == রিক্ত)
	ঘূৰাই;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    যদি (recursive)
	ঘূৰাই;
    recursive = TRUE;

# ifdef FEAT_EVAL
    যদি (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	যদি (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    ঘূৰাই;
	}
	অন্যথা
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    যদি (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
