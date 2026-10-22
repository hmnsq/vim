/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#𐤀𐤌 defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    صحيح
find_word_under_cursor(
	صحيح	    mouserow,
	صحيح	    mousecol,
	صحيح	    getword,
	صحيح	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	صحيح	    *colp,	// column where mouse hovers, can be NULL
	صحيح	    *startcolp) // column where text starts, can be NULL
{
    صحيح		row = mouserow;
    صحيح		col = mousecol;
    صحيح		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = عدم;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    𐤀𐤌 (wp == عدم || row < 0 || row >= wp->w_height || col >= wp->w_width)
	أرجع FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    𐤀𐤌 (mouse_comp_pos(wp, &row, &col, &lnum, عدم))
	أرجع FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    𐤀𐤌 (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	أرجع FAIL;		// past end of line

    // Not past end of line.
    𐤀𐤌 (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	صحيح		len;
	pos_T	*spos = عدم, *epos = عدم;

	𐤀𐤌 (VIsual_active)
	{
	    𐤀𐤌 (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    وإلا
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, عدم);
	scol = col;

	𐤀𐤌 (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (صحيح)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (صحيح)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    𐤀𐤌 (spos->lnum != epos->lnum || spos->col == epos->col)
		أرجع FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    𐤀𐤌 (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	وإلا
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    𐤀𐤌 (len == 0)
		أرجع FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    وإلا
	scol = col;

    𐤀𐤌 (winp != عدم)
	*winp = wp;
    𐤀𐤌 (lnump != عدم)
	*lnump = lnum;
    *textp = lbuf;
    𐤀𐤌 (colp != عدم)
	*colp = col;
    𐤀𐤌 (startcolp != عدم)
	*startcolp = scol;

    أرجع OK;
}
#endif

#𐤀𐤌 defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    صحيح
get_beval_info(
	BalloonEval	*beval,
	صحيح		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	صحيح		*colp)
{
    صحيح		row = mouse_row;
    صحيح		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    𐤀𐤌 (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    𐤀𐤌 (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, عدم) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	𐤀𐤌 ((*winp)->w_buffer->b_p_vts_array != عدم && beval->vts == عدم)
	{
	    𐤀𐤌 (getword)
		vim_free(*textp);
	    أرجع FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	أرجع OK;
    }

    أرجع FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    فراغ
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    𐤀𐤌 (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    𐤀𐤌 (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    صحيح
can_use_beval(فراغ)
{
    أرجع (0
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
	     ) && msg_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ساكن فراغ
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	صحيح		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    صحيح		use_sandbox;
    ساكن char_u  *result = عدم;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    لكل (cw = firstwin; cw != wp; cw = cw->w_next)
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
    𐤀𐤌 (use_sandbox)
	++sandbox;
    ++textlock;

    𐤀𐤌 (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	𐤀𐤌 (sp != عدم)
	    current_sctx = *sp;
    }
    وإلا
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    𐤀𐤌 (result != عدم)
    {
	len = STRLEN(result);
	𐤀𐤌 (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    𐤀𐤌 (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, عدم, -1);
    𐤀𐤌 (result != عدم && result[0] != NUL)
	post_balloon(beval, result, عدم);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    𐤀𐤌 (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    فراغ
general_beval_cb(BalloonEval *beval, صحيح state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    صحيح		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    ساكن صحيح	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    𐤀𐤌 (!can_use_beval() || beval == عدم)
	أرجع;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    𐤀𐤌 (recursive)
	أرجع;
    recursive = TRUE;

# ifdef FEAT_EVAL
    𐤀𐤌 (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	𐤀𐤌 (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    أرجع;
	}
	وإلا
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    𐤀𐤌 (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
