/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

#nếu defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    số_nguyên
find_word_under_cursor(
	số_nguyên	    mouserow,
	số_nguyên	    mousecol,
	số_nguyên	    getword,
	số_nguyên	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	số_nguyên	    *colp,	// column where mouse hovers, can be NULL
	số_nguyên	    *startcolp) // column where text starts, can be NULL
{
    số_nguyên		row = mouserow;
    số_nguyên		col = mousecol;
    số_nguyên		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = NULL;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    nếu (wp == NULL || row < 0 || row >= wp->w_height || col >= wp->w_width)
	trả_về FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    nếu (mouse_comp_pos(wp, &row, &col, &lnum, NULL))
	trả_về FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    nếu (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	trả_về FAIL;		// past end of line

    // Not past end of line.
    nếu (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	số_nguyên		len;
	pos_T	*spos = NULL, *epos = NULL;

	nếu (VIsual_active)
	{
	    nếu (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    không_thì
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, NULL);
	scol = col;

	nếu (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (số_nguyên)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (số_nguyên)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    nếu (spos->lnum != epos->lnum || spos->col == epos->col)
		trả_về FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    nếu (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	không_thì
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    nếu (len == 0)
		trả_về FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    không_thì
	scol = col;

    nếu (winp != NULL)
	*winp = wp;
    nếu (lnump != NULL)
	*lnump = lnum;
    *textp = lbuf;
    nếu (colp != NULL)
	*colp = col;
    nếu (startcolp != NULL)
	*startcolp = scol;

    trả_về OK;
}
#endif

#nếu defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    số_nguyên
get_beval_info(
	BalloonEval	*beval,
	số_nguyên		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	số_nguyên		*colp)
{
    số_nguyên		row = mouse_row;
    số_nguyên		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    nếu (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    nếu (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, NULL) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	nếu ((*winp)->w_buffer->b_p_vts_array != NULL && beval->vts == NULL)
	{
	    nếu (getword)
		vim_free(*textp);
	    trả_về FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	trả_về OK;
    }

    trả_về FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    void
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    nếu (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    nếu (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    số_nguyên
can_use_beval(void)
{
    trả_về (0
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
    static void
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	số_nguyên		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    số_nguyên		use_sandbox;
    static char_u  *result = NULL;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    cho (cw = firstwin; cw != wp; cw = cw->w_next)
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
    nếu (use_sandbox)
	++sandbox;
    ++textlock;

    nếu (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	nếu (sp != NULL)
	    current_sctx = *sp;
    }
    không_thì
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    nếu (result != NULL)
    {
	len = STRLEN(result);
	nếu (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    nếu (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, NULL, -1);
    nếu (result != NULL && result[0] != NUL)
	post_balloon(beval, result, NULL);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    nếu (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    void
general_beval_cb(BalloonEval *beval, số_nguyên state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    số_nguyên		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    static số_nguyên	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    nếu (!can_use_beval() || beval == NULL)
	trả_về;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    nếu (recursive)
	trả_về;
    recursive = TRUE;

# ifdef FEAT_EVAL
    nếu (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	nếu (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    trả_về;
	}
	không_thì
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    nếu (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
