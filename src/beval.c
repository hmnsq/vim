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

#ប្រសិនបើ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    ចំនួនគត់
find_word_under_cursor(
	ចំនួនគត់	    mouserow,
	ចំនួនគត់	    mousecol,
	ចំនួនគត់	    getword,
	ចំនួនគត់	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	ចំនួនគត់	    *colp,	// column where mouse hovers, can be NULL
	ចំនួនគត់	    *startcolp) // column where text starts, can be NULL
{
    ចំនួនគត់		row = mouserow;
    ចំនួនគត់		col = mousecol;
    ចំនួនគត់		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = NULL;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ប្រសិនបើ (wp == NULL || row < 0 || row >= wp->w_height || col >= wp->w_width)
	ត្រឡប់ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ប្រសិនបើ (mouse_comp_pos(wp, &row, &col, &lnum, NULL))
	ត្រឡប់ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ប្រសិនបើ (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ត្រឡប់ FAIL;		// past end of line

    // Not past end of line.
    ប្រសិនបើ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	ចំនួនគត់		len;
	pos_T	*spos = NULL, *epos = NULL;

	ប្រសិនបើ (VIsual_active)
	{
	    ប្រសិនបើ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    បើមិនដូច្នេះ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, NULL);
	scol = col;

	ប្រសិនបើ (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (ចំនួនគត់)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (ចំនួនគត់)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ប្រសិនបើ (spos->lnum != epos->lnum || spos->col == epos->col)
		ត្រឡប់ FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    ប្រសិនបើ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	បើមិនដូច្នេះ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ប្រសិនបើ (len == 0)
		ត្រឡប់ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    បើមិនដូច្នេះ
	scol = col;

    ប្រសិនបើ (winp != NULL)
	*winp = wp;
    ប្រសិនបើ (lnump != NULL)
	*lnump = lnum;
    *textp = lbuf;
    ប្រសិនបើ (colp != NULL)
	*colp = col;
    ប្រសិនបើ (startcolp != NULL)
	*startcolp = scol;

    ត្រឡប់ OK;
}
#endif

#ប្រសិនបើ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    ចំនួនគត់
get_beval_info(
	BalloonEval	*beval,
	ចំនួនគត់		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	ចំនួនគត់		*colp)
{
    ចំនួនគត់		row = mouse_row;
    ចំនួនគត់		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    ប្រសិនបើ (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    ប្រសិនបើ (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, NULL) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	ប្រសិនបើ ((*winp)->w_buffer->b_p_vts_array != NULL && beval->vts == NULL)
	{
	    ប្រសិនបើ (getword)
		vim_free(*textp);
	    ត្រឡប់ FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	ត្រឡប់ OK;
    }

    ត្រឡប់ FAIL;
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
    ប្រសិនបើ (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ប្រសិនបើ (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    ចំនួនគត់
can_use_beval(void)
{
    ត្រឡប់ (0
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
	ចំនួនគត់		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    ចំនួនគត់		use_sandbox;
    static char_u  *result = NULL;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    សម្រាប់ (cw = firstwin; cw != wp; cw = cw->w_next)
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
    ប្រសិនបើ (use_sandbox)
	++sandbox;
    ++textlock;

    ប្រសិនបើ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ប្រសិនបើ (sp != NULL)
	    current_sctx = *sp;
    }
    បើមិនដូច្នេះ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ប្រសិនបើ (result != NULL)
    {
	len = STRLEN(result);
	ប្រសិនបើ (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    ប្រសិនបើ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, NULL, -1);
    ប្រសិនបើ (result != NULL && result[0] != NUL)
	post_balloon(beval, result, NULL);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ប្រសិនបើ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    void
general_beval_cb(BalloonEval *beval, ចំនួនគត់ state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    ចំនួនគត់		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    static ចំនួនគត់	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ប្រសិនបើ (!can_use_beval() || beval == NULL)
	ត្រឡប់;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ប្រសិនបើ (recursive)
	ត្រឡប់;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ប្រសិនបើ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	ប្រសិនបើ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    ត្រឡប់;
	}
	បើមិនដូច្នេះ
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ប្រសិនបើ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
