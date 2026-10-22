/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಶಬ್ದ_under_cursor(
	ಪೂರ್ಣಾಂಕ	    mouserow,
	ಪೂರ್ಣಾಂಕ	    mousecol,
	ಪೂರ್ಣಾಂಕ	    getword,
	ಪೂರ್ಣಾಂಕ	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	ಪೂರ್ಣಾಂಕ	    *colp,	// column where mouse hovers, can be NULL
	ಪೂರ್ಣಾಂಕ	    *startcolp) // column where text starts, can be NULL
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = mouserow;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mousecol;
    ಪೂರ್ಣಾಂಕ		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = ಖಾಲಿ;
    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, FAIL_POPUP);
    ಆದರೆ (wp == ಖಾಲಿ || ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= wp->w_ಎತ್ತರ || ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    ಆದರೆ (mouse_comp_pos(wp, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &lnum, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    ಆದರೆ (ಲಂಬಸಾಲು > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ಹಿಂತಿರುಗಿ FAIL;		// past end of line

    // Not past end of line.
    ಆದರೆ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	ಪೂರ್ಣಾಂಕ		len;
	pos_T	*spos = ಖಾಲಿ, *epos = ಖಾಲಿ;

	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	ಲಂಬಸಾಲು = vcol2col(wp, lnum, ಲಂಬಸಾಲು, ಖಾಲಿ);
	scol = ಲಂಬಸಾಲು;

	ಆದರೆ (VIsual_ಸಕ್ರಿಯ
		&& wp->w_ಬಫರ್ == curwin->w_ಬಫರ್
		&& (lnum == spos->lnum
		    ? ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)spos->ಲಂಬಸಾಲು
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? ಲಂಬಸಾಲು <= (ಪೂರ್ಣಾಂಕ)epos->ಲಂಬಸಾಲು
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    ಆದರೆ (spos->lnum != epos->lnum || spos->ಲಂಬಸಾಲು == epos->ಲಂಬಸಾಲು)
		ಹಿಂತಿರುಗಿ FAIL;

	    lbuf = ml_get_buf(curwin->w_ಬಫರ್, VIsual.lnum, FALSE);
	    len = epos->ಲಂಬಸಾಲು - spos->ಲಂಬಸಾಲು;
	    ಆದರೆ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->ಲಂಬಸಾಲು);
	    lbuf = vim_strnsave(lbuf + spos->ಲಂಬಸಾಲು, len);
	    lnum = spos->lnum;
	    ಲಂಬಸಾಲು = spos->ಲಂಬಸಾಲು;
	    scol = ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = ಕಂಡುಹಿಡಿ_ident_at_pos(wp, lnum, (colnr_T)ಲಂಬಸಾಲು,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    ಆದರೆ (len == 0)
		ಹಿಂತಿರುಗಿ FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	scol = ಲಂಬಸಾಲು;

    ಆದರೆ (winp != ಖಾಲಿ)
	*winp = wp;
    ಆದರೆ (lnump != ಖಾಲಿ)
	*lnump = lnum;
    *textp = lbuf;
    ಆದರೆ (colp != ಖಾಲಿ)
	*colp = ಲಂಬಸಾಲು;
    ಆದರೆ (startcolp != ಖಾಲಿ)
	*startcolp = scol;

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
get_beval_info(
	BalloonEval	*beval,
	ಪೂರ್ಣಾಂಕ		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	ಪೂರ್ಣಾಂಕ		*colp)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

# ifdef FEAT_BEVAL_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಅಡ್ಡಸಾಲು = Y_2_ಅಡ್ಡಸಾಲು(beval->y);
	ಲಂಬಸಾಲು = X_2_ಲಂಬಸಾಲು(beval->x);
    }
# endif
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಶಬ್ದ_under_cursor(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, getword,
		ಕಂಡುಹಿಡಿ_IDENT + ಕಂಡುಹಿಡಿ_STRING + ಕಂಡುಹಿಡಿ_EVAL,
		winp, lnump, textp, colp, ಖಾಲಿ) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_ಬಫರ್->b_p_vts_array);
	ಆದರೆ ((*winp)->w_ಬಫರ್->b_p_vts_array != ಖಾಲಿ && beval->vts == ಖಾಲಿ)
	{
	    ಆದರೆ (getword)
		vim_free(*textp);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
# endif
	beval->ts = (*winp)->w_ಬಫರ್->b_p_ts;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    ಶೂನ್ಯ
post_balloon(BalloonEval *beval ಬಳಸದ, char_u *mesg, list_T *list ಬಳಸದ)
{
# ifdef FEAT_BEVAL_ಪದ
#  ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    ಆದರೆ (gui.ಒಳ_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    ಪೂರ್ಣಾಂಕ
can_use_beval(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (0
# ifdef FEAT_BEVAL_GUI
		|| (gui.ಒಳ_use && p_beval)
# endif
# ifdef FEAT_BEVAL_ಪದ
		|| (
#  ifdef FEAT_GUI
		    !gui.ಒಳ_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && ಸಂದೇಶ_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
	char_u		*ಪಠ್ಯ)
{
    win_T	*cw;
    ದೀರ್ಘ	winnr = 0;
    ಪೂರ್ಣಾಂಕ		use_sandbox;
    ಸ್ಥಿರ char_u  *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    ಗಾಗಿ (cw = firstwin; cw != wp; cw = cw->w_ಮುಂದಿನ)
	++winnr;

    ಗಣ_vim_var_nr(VV_BEVAL_BUFNR, (ದೀರ್ಘ)wp->w_ಬಫರ್->b_fnum);
    ಗಣ_vim_var_nr(VV_BEVAL_WINNR, winnr);
    ಗಣ_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    ಗಣ_vim_var_nr(VV_BEVAL_LNUM, (ದೀರ್ಘ)lnum);
    ಗಣ_vim_var_nr(VV_BEVAL_ಲಂಬಸಾಲು, (ದೀರ್ಘ)(ಲಂಬಸಾಲು + 1));
    ಗಣ_vim_var_string(VV_BEVAL_ಪಠ್ಯ, ಪಠ್ಯ, -1);
    vim_free(ಪಠ್ಯ);

    use_sandbox = was_ಗಣ_insecurely(wp, (char_u *)"balloonexpr",
			    *wp->w_ಬಫರ್->b_p_bexpr == NUL ? 0 : OPT_LOCAL);
    ಆದರೆ (use_sandbox)
	++sandbox;
    ++textlock;

    ಆದರೆ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	ಆದರೆ (sp != ಖಾಲಿ)
	    current_sctx = *sp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ = eval_ಗಮ್ಯ_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
    {
	len = STRLEN(ಫಲಿತಾಂಶ);
	ಆದರೆ (len > 0 && ಫಲಿತಾಂಶ[len - 1] == NL)
	    ಫಲಿತಾಂಶ[len - 1] = NUL;
    }

    ಆದರೆ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    ಗಣ_vim_var_string(VV_BEVAL_ಪಠ್ಯ, ಖಾಲಿ, -1);
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ && ಫಲಿತಾಂಶ[0] != NUL)
	post_balloon(beval, ಫಲಿತಾಂಶ, ಖಾಲಿ);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    ಆದರೆ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    ಶೂನ್ಯ
general_beval_cb(BalloonEval *beval, ಪೂರ್ಣಾಂಕ state ಬಳಸದ)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    linenr_T	lnum;
    char_u	*ಪಠ್ಯ;
    char_u	*bexpr;
# endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    ಆದರೆ (!can_use_beval() || beval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;
    recursive = TRUE;

# ifdef FEAT_EVAL
    ಆದರೆ (get_beval_info(beval, TRUE, &wp, &lnum, &ಪಠ್ಯ, &ಲಂಬಸಾಲು) == OK)
    {
	bexpr = (*wp->w_ಬಫರ್->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_ಬಫರ್->b_p_bexpr;
	ಆದರೆ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, ಲಂಬಸಾಲು, ಪಠ್ಯ);
	    recursive = FALSE;
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(ಪಠ್ಯ);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    ಆದರೆ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ
