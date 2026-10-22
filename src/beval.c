/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    ਪੂਰਨ
ਲੱਭੋ_ਸ਼ਬਦ_under_cursor(
	ਪੂਰਨ	    mouserow,
	ਪੂਰਨ	    mousecol,
	ਪੂਰਨ	    getword,
	ਪੂਰਨ	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	ਪੂਰਨ	    *colp,	// column where mouse hovers, can be NULL
	ਪੂਰਨ	    *startcolp) // column where text starts, can be NULL
{
    ਪੂਰਨ		ਪੰਕਤੀ = mouserow;
    ਪੂਰਨ		ਖਾਨਾ = mousecol;
    ਪੂਰਨ		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = ਨਿਰਾ;
    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, FAIL_POPUP);
    ਜੇ (wp == ਨਿਰਾ || ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= wp->w_ਉਚਾਈ || ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
	ਵਾਪਸ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ਜੇ (mouse_comp_pos(wp, &ਪੰਕਤੀ, &ਖਾਨਾ, &lnum, ਨਿਰਾ))
	ਵਾਪਸ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    ਜੇ (ਖਾਨਾ > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ਵਾਪਸ FAIL;		// past end of line

    // Not past end of line.
    ਜੇ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	ਪੂਰਨ		len;
	pos_T	*spos = ਨਿਰਾ, *epos = ਨਿਰਾ;

	ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    ਜੇ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	ਖਾਨਾ = vcol2col(wp, lnum, ਖਾਨਾ, ਨਿਰਾ);
	scol = ਖਾਨਾ;

	ਜੇ (VIsual_ਸਰਗਰਮ
		&& wp->w_ਬਫਰ == curwin->w_ਬਫਰ
		&& (lnum == spos->lnum
		    ? ਖਾਨਾ >= (ਪੂਰਨ)spos->ਖਾਨਾ
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? ਖਾਨਾ <= (ਪੂਰਨ)epos->ਖਾਨਾ
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ਜੇ (spos->lnum != epos->lnum || spos->ਖਾਨਾ == epos->ਖਾਨਾ)
		ਵਾਪਸ FAIL;

	    lbuf = ml_get_buf(curwin->w_ਬਫਰ, VIsual.lnum, FALSE);
	    len = epos->ਖਾਨਾ - spos->ਖਾਨਾ;
	    ਜੇ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->ਖਾਨਾ);
	    lbuf = vim_strnsave(lbuf + spos->ਖਾਨਾ, len);
	    lnum = spos->lnum;
	    ਖਾਨਾ = spos->ਖਾਨਾ;
	    scol = ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = ਲੱਭੋ_ident_at_pos(wp, lnum, (colnr_T)ਖਾਨਾ,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ਜੇ (len == 0)
		ਵਾਪਸ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    ਨਹੀਂਤਾਂ
	scol = ਖਾਨਾ;

    ਜੇ (winp != ਨਿਰਾ)
	*winp = wp;
    ਜੇ (lnump != ਨਿਰਾ)
	*lnump = lnum;
    *textp = lbuf;
    ਜੇ (colp != ਨਿਰਾ)
	*colp = ਖਾਨਾ;
    ਜੇ (startcolp != ਨਿਰਾ)
	*startcolp = scol;

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
get_beval_info(
	BalloonEval	*beval,
	ਪੂਰਨ		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	ਪੂਰਨ		*colp)
{
    ਪੂਰਨ		ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ = mouse_ਖਾਨਾ;

# ifdef FEAT_BEVAL_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਪੰਕਤੀ = Y_2_ਪੰਕਤੀ(beval->y);
	ਖਾਨਾ = X_2_ਖਾਨਾ(beval->x);
    }
# endif
    ਜੇ (ਲੱਭੋ_ਸ਼ਬਦ_under_cursor(ਪੰਕਤੀ, ਖਾਨਾ, getword,
		ਲੱਭੋ_IDENT + ਲੱਭੋ_STRING + ਲੱਭੋ_EVAL,
		winp, lnump, textp, colp, ਨਿਰਾ) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_ਬਫਰ->b_p_vts_array);
	ਜੇ ((*winp)->w_ਬਫਰ->b_p_vts_array != ਨਿਰਾ && beval->vts == ਨਿਰਾ)
	{
	    ਜੇ (getword)
		vim_free(*textp);
	    ਵਾਪਸ FAIL;
	}
# endif
	beval->ts = (*winp)->w_ਬਫਰ->b_p_ts;
	ਵਾਪਸ OK;
    }

    ਵਾਪਸ FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    ਖਾਲੀ
post_balloon(BalloonEval *beval ਅਣਵਰਤਿਆ, char_u *mesg, list_T *list ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_BEVAL_ਪਦ
#  ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    ਪੂਰਨ
can_use_beval(ਖਾਲੀ)
{
    ਵਾਪਸ (0
# ifdef FEAT_BEVAL_GUI
		|| (gui.ਵਿੱਚ_use && p_beval)
# endif
# ifdef FEAT_BEVAL_ਪਦ
		|| (
#  ifdef FEAT_GUI
		    !gui.ਵਿੱਚ_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && ਸੁਨੇਹਾ_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ਸਥਿਰ ਖਾਲੀ
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	ਪੂਰਨ		ਖਾਨਾ,
	char_u		*ਪਾਠ)
{
    win_T	*cw;
    ਲੰਮਾ	winnr = 0;
    ਪੂਰਨ		use_sandbox;
    ਸਥਿਰ char_u  *ਨਤੀਜਾ = ਨਿਰਾ;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    ਲਈ (cw = firstwin; cw != wp; cw = cw->w_ਅਗਲਾ)
	++winnr;

    ਸੈੱਟ_vim_var_nr(VV_BEVAL_BUFNR, (ਲੰਮਾ)wp->w_ਬਫਰ->b_fnum);
    ਸੈੱਟ_vim_var_nr(VV_BEVAL_WINNR, winnr);
    ਸੈੱਟ_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    ਸੈੱਟ_vim_var_nr(VV_BEVAL_LNUM, (ਲੰਮਾ)lnum);
    ਸੈੱਟ_vim_var_nr(VV_BEVAL_ਖਾਨਾ, (ਲੰਮਾ)(ਖਾਨਾ + 1));
    ਸੈੱਟ_vim_var_string(VV_BEVAL_ਪਾਠ, ਪਾਠ, -1);
    vim_free(ਪਾਠ);

    use_sandbox = was_ਸੈੱਟ_insecurely(wp, (char_u *)"balloonexpr",
			    *wp->w_ਬਫਰ->b_p_bexpr == NUL ? 0 : OPT_LOCAL);
    ਜੇ (use_sandbox)
	++sandbox;
    ++textlock;

    ਜੇ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ਜੇ (sp != ਨਿਰਾ)
	    current_sctx = *sp;
    }
    ਨਹੀਂਤਾਂ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(ਨਤੀਜਾ);
    ਨਤੀਜਾ = eval_ਮੰਜ਼ਿਲ_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
    {
	len = STRLEN(ਨਤੀਜਾ);
	ਜੇ (len > 0 && ਨਤੀਜਾ[len - 1] == NL)
	    ਨਤੀਜਾ[len - 1] = NUL;
    }

    ਜੇ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    ਸੈੱਟ_vim_var_string(VV_BEVAL_ਪਾਠ, ਨਿਰਾ, -1);
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ && ਨਤੀਜਾ[0] != NUL)
	post_balloon(beval, ਨਤੀਜਾ, ਨਿਰਾ);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ਜੇ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    ਖਾਲੀ
general_beval_cb(BalloonEval *beval, ਪੂਰਨ state ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    ਪੂਰਨ		ਖਾਨਾ;
    linenr_T	lnum;
    char_u	*ਪਾਠ;
    char_u	*bexpr;
# endif
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ਜੇ (!can_use_beval() || beval == ਨਿਰਾ)
	ਵਾਪਸ;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ਜੇ (recursive)
	ਵਾਪਸ;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ਜੇ (get_beval_info(beval, TRUE, &wp, &lnum, &ਪਾਠ, &ਖਾਨਾ) == OK)
    {
	bexpr = (*wp->w_ਬਫਰ->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_ਬਫਰ->b_p_bexpr;
	ਜੇ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, ਖਾਨਾ, ਪਾਠ);
	    recursive = FALSE;
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ
	    vim_free(ਪਾਠ);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ਜੇ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#ਜੇ_ਅੰਤ
