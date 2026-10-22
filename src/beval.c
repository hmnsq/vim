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

#إذا defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    صحيح
لبھو_لفظ_under_cursor(
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
    صحيح		صف = mouserow;
    صحيح		خانہ = mousecol;
    صحيح		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = عدم;
    wp = mouse_لبھو_win(&صف, &خانہ, FAIL_POPUP);
    إذا (wp == عدم || صف < 0 || صف >= wp->w_اچائی || خانہ >= wp->w_چوڑائی)
	أرجع FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    إذا (mouse_comp_pos(wp, &صف, &خانہ, &lnum, عدم))
	أرجع FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_بفر, lnum, FALSE);
    إذا (خانہ > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	أرجع FAIL;		// past end of line

    // Not past end of line.
    إذا (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	صحيح		len;
	pos_T	*spos = عدم, *epos = عدم;

	إذا (VIsual_سرگرم)
	{
	    إذا (LT_POS(VIsual, curwin->w_cursor))
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

	خانہ = vcol2col(wp, lnum, خانہ, عدم);
	scol = خانہ;

	إذا (VIsual_سرگرم
		&& wp->w_بفر == curwin->w_بفر
		&& (lnum == spos->lnum
		    ? خانہ >= (صحيح)spos->خانہ
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? خانہ <= (صحيح)epos->خانہ
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    إذا (spos->lnum != epos->lnum || spos->خانہ == epos->خانہ)
		أرجع FAIL;

	    lbuf = ml_get_buf(curwin->w_بفر, VIsual.lnum, FALSE);
	    len = epos->خانہ - spos->خانہ;
	    إذا (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->خانہ);
	    lbuf = vim_strnsave(lbuf + spos->خانہ, len);
	    lnum = spos->lnum;
	    خانہ = spos->خانہ;
	    scol = خانہ;
	}
	وإلا
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = لبھو_ident_at_pos(wp, lnum, (colnr_T)خانہ,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    إذا (len == 0)
		أرجع FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    وإلا
	scol = خانہ;

    إذا (winp != عدم)
	*winp = wp;
    إذا (lnump != عدم)
	*lnump = lnum;
    *textp = lbuf;
    إذا (colp != عدم)
	*colp = خانہ;
    إذا (startcolp != عدم)
	*startcolp = scol;

    أرجع OK;
}
#جے_انت

#إذا defined(FEAT_BEVAL)

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
    صحيح		صف = mouse_صف;
    صحيح		خانہ = mouse_خانہ;

# ifdef FEAT_BEVAL_GUI
    إذا (gui.وچ_use)
    {
	صف = Y_2_صف(beval->y);
	خانہ = X_2_خانہ(beval->x);
    }
# endif
    إذا (لبھو_لفظ_under_cursor(صف, خانہ, getword,
		لبھو_IDENT + لبھو_STRING + لبھو_EVAL,
		winp, lnump, textp, colp, عدم) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_بفر->b_p_vts_array);
	إذا ((*winp)->w_بفر->b_p_vts_array != عدم && beval->vts == عدم)
	{
	    إذا (getword)
		vim_free(*textp);
	    أرجع FAIL;
	}
# endif
	beval->ts = (*winp)->w_بفر->b_p_ts;
	أرجع OK;
    }

    أرجع FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    فراغ
post_balloon(BalloonEval *beval بن_ورتیا, char_u *mesg, list_T *list بن_ورتیا)
{
# ifdef FEAT_BEVAL_رکن
#  ifdef FEAT_GUI
    إذا (!gui.وچ_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    إذا (gui.وچ_use)
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
		|| (gui.وچ_use && p_beval)
# endif
# ifdef FEAT_BEVAL_رکن
		|| (
#  ifdef FEAT_GUI
		    !gui.وچ_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && پیغام_scrolled == 0;
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
	صحيح		خانہ,
	char_u		*متن)
{
    win_T	*cw;
    لما	winnr = 0;
    صحيح		use_sandbox;
    ساكن char_u  *نتیجہ = عدم;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    لكل (cw = firstwin; cw != wp; cw = cw->w_اگلا)
	++winnr;

    سیٹ_vim_var_nr(VV_BEVAL_BUFNR, (لما)wp->w_بفر->b_fnum);
    سیٹ_vim_var_nr(VV_BEVAL_WINNR, winnr);
    سیٹ_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    سیٹ_vim_var_nr(VV_BEVAL_LNUM, (لما)lnum);
    سیٹ_vim_var_nr(VV_BEVAL_خانہ, (لما)(خانہ + 1));
    سیٹ_vim_var_string(VV_BEVAL_متن, متن, -1);
    vim_free(متن);

    use_sandbox = was_سیٹ_insecurely(wp, (char_u *)"balloonexpr",
			    *wp->w_بفر->b_p_bexpr == NUL ? 0 : OPT_LOCAL);
    إذا (use_sandbox)
	++sandbox;
    ++textlock;

    إذا (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	إذا (sp != عدم)
	    current_sctx = *sp;
    }
    وإلا
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(نتیجہ);
    نتیجہ = eval_منزل_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    إذا (نتیجہ != عدم)
    {
	len = STRLEN(نتیجہ);
	إذا (len > 0 && نتیجہ[len - 1] == NL)
	    نتیجہ[len - 1] = NUL;
    }

    إذا (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    سیٹ_vim_var_string(VV_BEVAL_متن, عدم, -1);
    إذا (نتیجہ != عدم && نتیجہ[0] != NUL)
	post_balloon(beval, نتیجہ, عدم);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    إذا (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    فراغ
general_beval_cb(BalloonEval *beval, صحيح state بن_ورتیا)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    صحيح		خانہ;
    linenr_T	lnum;
    char_u	*متن;
    char_u	*bexpr;
# endif
    ساكن صحيح	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    إذا (!can_use_beval() || beval == عدم)
	أرجع;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    إذا (recursive)
	أرجع;
    recursive = TRUE;

# ifdef FEAT_EVAL
    إذا (get_beval_info(beval, TRUE, &wp, &lnum, &متن, &خانہ) == OK)
    {
	bexpr = (*wp->w_بفر->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_بفر->b_p_bexpr;
	إذا (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, خانہ, متن);
	    recursive = FALSE;
	    أرجع;
	}
	وإلا
	    vim_free(متن);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    إذا (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#جے_انت
