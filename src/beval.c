/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    పూర్ణాంకం
కనుగొను_మాట_under_cursor(
	పూర్ణాంకం	    mouserow,
	పూర్ణాంకం	    mousecol,
	పూర్ణాంకం	    getword,
	పూర్ణాంకం	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	పూర్ణాంకం	    *colp,	// column where mouse hovers, can be NULL
	పూర్ణాంకం	    *startcolp) // column where text starts, can be NULL
{
    పూర్ణాంకం		అడ్డువరుస = mouserow;
    పూర్ణాంకం		నిలువు = mousecol;
    పూర్ణాంకం		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = ఖాళీ;
    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, FAIL_POPUP);
    ఒకవేళ (wp == ఖాళీ || అడ్డువరుస < 0 || అడ్డువరుస >= wp->w_ఎత్తు || నిలువు >= wp->w_వెడల్పు)
	తిరిగి FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ఒకవేళ (mouse_comp_pos(wp, &అడ్డువరుస, &నిలువు, &lnum, ఖాళీ))
	తిరిగి FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    ఒకవేళ (నిలువు > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	తిరిగి FAIL;		// past end of line

    // Not past end of line.
    ఒకవేళ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	పూర్ణాంకం		len;
	pos_T	*spos = ఖాళీ, *epos = ఖాళీ;

	ఒకవేళ (VIsual_చురుకైన)
	{
	    ఒకవేళ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    లేకపోతే
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	నిలువు = vcol2col(wp, lnum, నిలువు, ఖాళీ);
	scol = నిలువు;

	ఒకవేళ (VIsual_చురుకైన
		&& wp->w_బఫరు == curwin->w_బఫరు
		&& (lnum == spos->lnum
		    ? నిలువు >= (పూర్ణాంకం)spos->నిలువు
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? నిలువు <= (పూర్ణాంకం)epos->నిలువు
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ఒకవేళ (spos->lnum != epos->lnum || spos->నిలువు == epos->నిలువు)
		తిరిగి FAIL;

	    lbuf = ml_get_buf(curwin->w_బఫరు, VIsual.lnum, FALSE);
	    len = epos->నిలువు - spos->నిలువు;
	    ఒకవేళ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->నిలువు);
	    lbuf = vim_strnsave(lbuf + spos->నిలువు, len);
	    lnum = spos->lnum;
	    నిలువు = spos->నిలువు;
	    scol = నిలువు;
	}
	లేకపోతే
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = కనుగొను_ident_at_pos(wp, lnum, (colnr_T)నిలువు,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ఒకవేళ (len == 0)
		తిరిగి FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    లేకపోతే
	scol = నిలువు;

    ఒకవేళ (winp != ఖాళీ)
	*winp = wp;
    ఒకవేళ (lnump != ఖాళీ)
	*lnump = lnum;
    *textp = lbuf;
    ఒకవేళ (colp != ఖాళీ)
	*colp = నిలువు;
    ఒకవేళ (startcolp != ఖాళీ)
	*startcolp = scol;

    తిరిగి OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
get_beval_info(
	BalloonEval	*beval,
	పూర్ణాంకం		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	పూర్ణాంకం		*colp)
{
    పూర్ణాంకం		అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం		నిలువు = mouse_నిలువు;

# ifdef FEAT_BEVAL_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	అడ్డువరుస = Y_2_అడ్డువరుస(beval->y);
	నిలువు = X_2_నిలువు(beval->x);
    }
# endif
    ఒకవేళ (కనుగొను_మాట_under_cursor(అడ్డువరుస, నిలువు, getword,
		కనుగొను_IDENT + కనుగొను_STRING + కనుగొను_EVAL,
		winp, lnump, textp, colp, ఖాళీ) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_బఫరు->b_p_vts_array);
	ఒకవేళ ((*winp)->w_బఫరు->b_p_vts_array != ఖాళీ && beval->vts == ఖాళీ)
	{
	    ఒకవేళ (getword)
		vim_free(*textp);
	    తిరిగి FAIL;
	}
# endif
	beval->ts = (*winp)->w_బఫరు->b_p_ts;
	తిరిగి OK;
    }

    తిరిగి FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    శూన్య
post_balloon(BalloonEval *beval వాడని, char_u *mesg, list_T *list వాడని)
{
# ifdef FEAT_BEVAL_పదం
#  ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ఒకవేళ (gui.లోపల_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    పూర్ణాంకం
can_use_beval(శూన్య)
{
    తిరిగి (0
# ifdef FEAT_BEVAL_GUI
		|| (gui.లోపల_use && p_beval)
# endif
# ifdef FEAT_BEVAL_పదం
		|| (
#  ifdef FEAT_GUI
		    !gui.లోపల_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && సందేశం_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    స్థిర శూన్య
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	పూర్ణాంకం		నిలువు,
	char_u		*పాఠ్యం)
{
    win_T	*cw;
    దీర్ఘ	winnr = 0;
    పూర్ణాంకం		use_sandbox;
    స్థిర char_u  *ఫలితం = ఖాళీ;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    కొరకు (cw = firstwin; cw != wp; cw = cw->w_తదుపరి)
	++winnr;

    సమితి_vim_var_nr(VV_BEVAL_BUFNR, (దీర్ఘ)wp->w_బఫరు->b_fnum);
    సమితి_vim_var_nr(VV_BEVAL_WINNR, winnr);
    సమితి_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    సమితి_vim_var_nr(VV_BEVAL_LNUM, (దీర్ఘ)lnum);
    సమితి_vim_var_nr(VV_BEVAL_నిలువు, (దీర్ఘ)(నిలువు + 1));
    సమితి_vim_var_string(VV_BEVAL_పాఠ్యం, పాఠ్యం, -1);
    vim_free(పాఠ్యం);

    use_sandbox = was_సమితి_insecurely(wp, (char_u *)"balloonexpr",
			    *wp->w_బఫరు->b_p_bexpr == NUL ? 0 : OPT_LOCAL);
    ఒకవేళ (use_sandbox)
	++sandbox;
    ++textlock;

    ఒకవేళ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ఒకవేళ (sp != ఖాళీ)
	    current_sctx = *sp;
    }
    లేకపోతే
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(ఫలితం);
    ఫలితం = eval_గమ్యం_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ఒకవేళ (ఫలితం != ఖాళీ)
    {
	len = STRLEN(ఫలితం);
	ఒకవేళ (len > 0 && ఫలితం[len - 1] == NL)
	    ఫలితం[len - 1] = NUL;
    }

    ఒకవేళ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    సమితి_vim_var_string(VV_BEVAL_పాఠ్యం, ఖాళీ, -1);
    ఒకవేళ (ఫలితం != ఖాళీ && ఫలితం[0] != NUL)
	post_balloon(beval, ఫలితం, ఖాళీ);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ఒకవేళ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    శూన్య
general_beval_cb(BalloonEval *beval, పూర్ణాంకం state వాడని)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    పూర్ణాంకం		నిలువు;
    linenr_T	lnum;
    char_u	*పాఠ్యం;
    char_u	*bexpr;
# endif
    స్థిర పూర్ణాంకం	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ఒకవేళ (!can_use_beval() || beval == ఖాళీ)
	తిరిగి;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ఒకవేళ (recursive)
	తిరిగి;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ఒకవేళ (get_beval_info(beval, TRUE, &wp, &lnum, &పాఠ్యం, &నిలువు) == OK)
    {
	bexpr = (*wp->w_బఫరు->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_బఫరు->b_p_bexpr;
	ఒకవేళ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, నిలువు, పాఠ్యం);
	    recursive = FALSE;
	    తిరిగి;
	}
	లేకపోతే
	    vim_free(పాఠ్యం);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ఒకవేళ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#అయితే_ముగింపు
