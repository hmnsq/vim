/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    पूर्णाङ्क
find_word_under_cursor(
	पूर्णाङ्क	    mouserow,
	पूर्णाङ्क	    mousecol,
	पूर्णाङ्क	    getword,
	पूर्णाङ्क	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	पूर्णाङ्क	    *colp,	// column where mouse hovers, can be NULL
	पूर्णाङ्क	    *startcolp) // column where text starts, can be NULL
{
    पूर्णाङ्क		row = mouserow;
    पूर्णाङ्क		col = mousecol;
    पूर्णाङ्क		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = रिक्त;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    सचे (wp == रिक्त || row < 0 || row >= wp->w_height || col >= wp->w_width)
	पटिनिवत्त FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    सचे (mouse_comp_pos(wp, &row, &col, &lnum, रिक्त))
	पटिनिवत्त FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    सचे (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	पटिनिवत्त FAIL;		// past end of line

    // Not past end of line.
    सचे (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	पूर्णाङ्क		len;
	pos_T	*spos = रिक्त, *epos = रिक्त;

	सचे (VIsual_active)
	{
	    सचे (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    अञ्ञथा
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, रिक्त);
	scol = col;

	सचे (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (पूर्णाङ्क)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (पूर्णाङ्क)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    सचे (spos->lnum != epos->lnum || spos->col == epos->col)
		पटिनिवत्त FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    सचे (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	अञ्ञथा
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    सचे (len == 0)
		पटिनिवत्त FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    अञ्ञथा
	scol = col;

    सचे (winp != रिक्त)
	*winp = wp;
    सचे (lnump != रिक्त)
	*lnump = lnum;
    *textp = lbuf;
    सचे (colp != रिक्त)
	*colp = col;
    सचे (startcolp != रिक्त)
	*startcolp = scol;

    पटिनिवत्त OK;
}
#endif

#सचे defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
get_beval_info(
	BalloonEval	*beval,
	पूर्णाङ्क		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	पूर्णाङ्क		*colp)
{
    पूर्णाङ्क		row = mouse_row;
    पूर्णाङ्क		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    सचे (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    सचे (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, रिक्त) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	सचे ((*winp)->w_buffer->b_p_vts_array != रिक्त && beval->vts == रिक्त)
	{
	    सचे (getword)
		vim_free(*textp);
	    पटिनिवत्त FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	पटिनिवत्त OK;
    }

    पटिनिवत्त FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    शून्य
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    सचे (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    सचे (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    पूर्णाङ्क
can_use_beval(शून्य)
{
    पटिनिवत्त (0
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
	     ) && सन्देशः_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    स्थिर शून्य
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	पूर्णाङ्क		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    पूर्णाङ्क		use_sandbox;
    स्थिर char_u  *result = रिक्त;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    अत्थाय (cw = firstwin; cw != wp; cw = cw->w_next)
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
    सचे (use_sandbox)
	++sandbox;
    ++textlock;

    सचे (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	सचे (sp != रिक्त)
	    current_sctx = *sp;
    }
    अञ्ञथा
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    सचे (result != रिक्त)
    {
	len = STRLEN(result);
	सचे (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    सचे (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, रिक्त, -1);
    सचे (result != रिक्त && result[0] != NUL)
	post_balloon(beval, result, रिक्त);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    सचे (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    शून्य
general_beval_cb(BalloonEval *beval, पूर्णाङ्क state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    पूर्णाङ्क		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    स्थिर पूर्णाङ्क	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    सचे (!can_use_beval() || beval == रिक्त)
	पटिनिवत्त;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    सचे (recursive)
	पटिनिवत्त;
    recursive = TRUE;

# ifdef FEAT_EVAL
    सचे (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	सचे (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    पटिनिवत्त;
	}
	अञ्ञथा
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    सचे (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
