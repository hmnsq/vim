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

#ᤀᤡ᤺ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    puur_naa_nka
find_word_under_cursor(
	puur_naa_nka	    mouserow,
	puur_naa_nka	    mousecol,
	puur_naa_nka	    getword,
	puur_naa_nka	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	puur_naa_nka	    *colp,	// column where mouse hovers, can be NULL
	puur_naa_nka	    *startcolp) // column where text starts, can be NULL
{
    puur_naa_nka		row = mouserow;
    puur_naa_nka		col = mousecol;
    puur_naa_nka		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = NULL;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ᤀᤡ᤺ (wp == NULL || row < 0 || row >= wp->w_height || col >= wp->w_width)
	ᤀᤡᤴ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ᤀᤡ᤺ (mouse_comp_pos(wp, &row, &col, &lnum, NULL))
	ᤀᤡᤴ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ᤀᤡ᤺ (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ᤀᤡᤴ FAIL;		// past end of line

    // Not past end of line.
    ᤀᤡ᤺ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	puur_naa_nka		len;
	pos_T	*spos = NULL, *epos = NULL;

	ᤀᤡ᤺ (VIsual_active)
	{
	    ᤀᤡ᤺ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, NULL);
	scol = col;

	ᤀᤡ᤺ (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (puur_naa_nka)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (puur_naa_nka)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ᤀᤡ᤺ (spos->lnum != epos->lnum || spos->col == epos->col)
		ᤀᤡᤴ FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    ᤀᤡ᤺ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ᤀᤡ᤺ (len == 0)
		ᤀᤡᤴ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    ᤀᤡ᤺ᤆᤡ
	scol = col;

    ᤀᤡ᤺ (winp != NULL)
	*winp = wp;
    ᤀᤡ᤺ (lnump != NULL)
	*lnump = lnum;
    *textp = lbuf;
    ᤀᤡ᤺ (colp != NULL)
	*colp = col;
    ᤀᤡ᤺ (startcolp != NULL)
	*startcolp = scol;

    ᤀᤡᤴ OK;
}
#endif

#ᤀᤡ᤺ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    puur_naa_nka
get_beval_info(
	BalloonEval	*beval,
	puur_naa_nka		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	puur_naa_nka		*colp)
{
    puur_naa_nka		row = mouse_row;
    puur_naa_nka		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    ᤀᤡ᤺ (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    ᤀᤡ᤺ (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, NULL) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	ᤀᤡ᤺ ((*winp)->w_buffer->b_p_vts_array != NULL && beval->vts == NULL)
	{
	    ᤀᤡ᤺ (getword)
		vim_free(*textp);
	    ᤀᤡᤴ FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	ᤀᤡᤴ OK;
    }

    ᤀᤡᤴ FAIL;
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
    ᤀᤡ᤺ (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ᤀᤡ᤺ (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    puur_naa_nka
can_use_beval(void)
{
    ᤀᤡᤴ (0
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
	puur_naa_nka		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    puur_naa_nka		use_sandbox;
    static char_u  *result = NULL;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    for (cw = firstwin; cw != wp; cw = cw->w_next)
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
    ᤀᤡ᤺ (use_sandbox)
	++sandbox;
    ++textlock;

    ᤀᤡ᤺ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ᤀᤡ᤺ (sp != NULL)
	    current_sctx = *sp;
    }
    ᤀᤡ᤺ᤆᤡ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ᤀᤡ᤺ (result != NULL)
    {
	len = STRLEN(result);
	ᤀᤡ᤺ (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    ᤀᤡ᤺ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, NULL, -1);
    ᤀᤡ᤺ (result != NULL && result[0] != NUL)
	post_balloon(beval, result, NULL);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ᤀᤡ᤺ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    void
general_beval_cb(BalloonEval *beval, puur_naa_nka state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    puur_naa_nka		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    static puur_naa_nka	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ᤀᤡ᤺ (!can_use_beval() || beval == NULL)
	ᤀᤡᤴ;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ᤀᤡ᤺ (recursive)
	ᤀᤡᤴ;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ᤀᤡ᤺ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	ᤀᤡ᤺ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    ᤀᤡᤴ;
	}
	ᤀᤡ᤺ᤆᤡ
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ᤀᤡ᤺ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
