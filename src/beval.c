/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    പൂർണ്ണസംഖ്യ
find_word_under_cursor(
	പൂർണ്ണസംഖ്യ	    mouserow,
	പൂർണ്ണസംഖ്യ	    mousecol,
	പൂർണ്ണസംഖ്യ	    getword,
	പൂർണ്ണസംഖ്യ	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	പൂർണ്ണസംഖ്യ	    *colp,	// column where mouse hovers, can be NULL
	പൂർണ്ണസംഖ്യ	    *startcolp) // column where text starts, can be NULL
{
    പൂർണ്ണസംഖ്യ		row = mouserow;
    പൂർണ്ണസംഖ്യ		col = mousecol;
    പൂർണ്ണസംഖ്യ		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = ശൂന്യ;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    എങ്കിൽ (wp == ശൂന്യ || row < 0 || row >= wp->w_height || col >= wp->w_width)
	തിരികെ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    എങ്കിൽ (mouse_comp_pos(wp, &row, &col, &lnum, ശൂന്യ))
	തിരികെ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    എങ്കിൽ (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	തിരികെ FAIL;		// past end of line

    // Not past end of line.
    എങ്കിൽ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	പൂർണ്ണസംഖ്യ		len;
	pos_T	*spos = ശൂന്യ, *epos = ശൂന്യ;

	എങ്കിൽ (VIsual_active)
	{
	    എങ്കിൽ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    അല്ലെങ്കിൽ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, ശൂന്യ);
	scol = col;

	എങ്കിൽ (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (പൂർണ്ണസംഖ്യ)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (പൂർണ്ണസംഖ്യ)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    എങ്കിൽ (spos->lnum != epos->lnum || spos->col == epos->col)
		തിരികെ FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    എങ്കിൽ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	അല്ലെങ്കിൽ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    എങ്കിൽ (len == 0)
		തിരികെ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    അല്ലെങ്കിൽ
	scol = col;

    എങ്കിൽ (winp != ശൂന്യ)
	*winp = wp;
    എങ്കിൽ (lnump != ശൂന്യ)
	*lnump = lnum;
    *textp = lbuf;
    എങ്കിൽ (colp != ശൂന്യ)
	*colp = col;
    എങ്കിൽ (startcolp != ശൂന്യ)
	*startcolp = scol;

    തിരികെ OK;
}
#endif

#എങ്കിൽ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
get_beval_info(
	BalloonEval	*beval,
	പൂർണ്ണസംഖ്യ		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	പൂർണ്ണസംഖ്യ		*colp)
{
    പൂർണ്ണസംഖ്യ		row = mouse_row;
    പൂർണ്ണസംഖ്യ		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    എങ്കിൽ (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    എങ്കിൽ (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, ശൂന്യ) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	എങ്കിൽ ((*winp)->w_buffer->b_p_vts_array != ശൂന്യ && beval->vts == ശൂന്യ)
	{
	    എങ്കിൽ (getword)
		vim_free(*textp);
	    തിരികെ FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	തിരികെ OK;
    }

    തിരികെ FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    ശൂന്യം
post_balloon(BalloonEval *beval UNUSED, char_u *mesg, list_T *list UNUSED)
{
# ifdef FEAT_BEVAL_TERM
#  ifdef FEAT_GUI
    എങ്കിൽ (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    എങ്കിൽ (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    പൂർണ്ണസംഖ്യ
can_use_beval(ശൂന്യം)
{
    തിരികെ (0
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
	     ) && സന്ദേശം_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    സ്ഥിതം ശൂന്യം
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	പൂർണ്ണസംഖ്യ		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    പൂർണ്ണസംഖ്യ		use_sandbox;
    സ്ഥിതം char_u  *result = ശൂന്യ;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    വേണ്ടി (cw = firstwin; cw != wp; cw = cw->w_next)
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
    എങ്കിൽ (use_sandbox)
	++sandbox;
    ++textlock;

    എങ്കിൽ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	എങ്കിൽ (sp != ശൂന്യ)
	    current_sctx = *sp;
    }
    അല്ലെങ്കിൽ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    എങ്കിൽ (result != ശൂന്യ)
    {
	len = STRLEN(result);
	എങ്കിൽ (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    എങ്കിൽ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, ശൂന്യ, -1);
    എങ്കിൽ (result != ശൂന്യ && result[0] != NUL)
	post_balloon(beval, result, ശൂന്യ);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    എങ്കിൽ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    ശൂന്യം
general_beval_cb(BalloonEval *beval, പൂർണ്ണസംഖ്യ state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    എങ്കിൽ (!can_use_beval() || beval == ശൂന്യ)
	തിരികെ;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    എങ്കിൽ (recursive)
	തിരികെ;
    recursive = TRUE;

# ifdef FEAT_EVAL
    എങ്കിൽ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	എങ്കിൽ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    തിരികെ;
	}
	അല്ലെങ്കിൽ
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    എങ്കിൽ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
