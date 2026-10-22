/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#اگر defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    صحیح
ڈھونڈو_لفظ_under_cursor(
	صحیح	    mouserow,
	صحیح	    mousecol,
	صحیح	    getword,
	صحیح	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	صحیح	    *colp,	// column where mouse hovers, can be NULL
	صحیح	    *startcolp) // column where text starts, can be NULL
{
    صحیح		صف = mouserow;
    صحیح		ستون = mousecol;
    صحیح		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = تہی;
    wp = mouse_ڈھونڈو_win(&صف, &ستون, FAIL_POPUP);
    اگر (wp == تہی || صف < 0 || صف >= wp->w_اونچائی || ستون >= wp->w_چوڑائی)
	واپس FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    اگر (mouse_comp_pos(wp, &صف, &ستون, &lnum, تہی))
	واپس FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_بفر, lnum, FALSE);
    اگر (ستون > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	واپس FAIL;		// past end of line

    // Not past end of line.
    اگر (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	صحیح		len;
	pos_T	*spos = تہی, *epos = تہی;

	اگر (VIsual_فعال)
	{
	    اگر (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    ورنہ
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	ستون = vcol2col(wp, lnum, ستون, تہی);
	scol = ستون;

	اگر (VIsual_فعال
		&& wp->w_بفر == curwin->w_بفر
		&& (lnum == spos->lnum
		    ? ستون >= (صحیح)spos->ستون
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? ستون <= (صحیح)epos->ستون
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    اگر (spos->lnum != epos->lnum || spos->ستون == epos->ستون)
		واپس FAIL;

	    lbuf = ml_get_buf(curwin->w_بفر, VIsual.lnum, FALSE);
	    len = epos->ستون - spos->ستون;
	    اگر (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->ستون);
	    lbuf = vim_strnsave(lbuf + spos->ستون, len);
	    lnum = spos->lnum;
	    ستون = spos->ستون;
	    scol = ستون;
	}
	ورنہ
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = ڈھونڈو_ident_at_pos(wp, lnum, (colnr_T)ستون,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    اگر (len == 0)
		واپس FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    ورنہ
	scol = ستون;

    اگر (winp != تہی)
	*winp = wp;
    اگر (lnump != تہی)
	*lnump = lnum;
    *textp = lbuf;
    اگر (colp != تہی)
	*colp = ستون;
    اگر (startcolp != تہی)
	*startcolp = scol;

    واپس OK;
}
#اگر_ختم

#اگر defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    صحیح
get_beval_info(
	BalloonEval	*beval,
	صحیح		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	صحیح		*colp)
{
    صحیح		صف = mouse_صف;
    صحیح		ستون = mouse_ستون;

# ifdef FEAT_BEVAL_GUI
    اگر (gui.اندر_use)
    {
	صف = Y_2_صف(beval->y);
	ستون = X_2_ستون(beval->x);
    }
# endif
    اگر (ڈھونڈو_لفظ_under_cursor(صف, ستون, getword,
		ڈھونڈو_IDENT + ڈھونڈو_STRING + ڈھونڈو_EVAL,
		winp, lnump, textp, colp, تہی) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_بفر->b_p_vts_array);
	اگر ((*winp)->w_بفر->b_p_vts_array != تہی && beval->vts == تہی)
	{
	    اگر (getword)
		vim_free(*textp);
	    واپس FAIL;
	}
# endif
	beval->ts = (*winp)->w_بفر->b_p_ts;
	واپس OK;
    }

    واپس FAIL;
}

/*
 * Show a balloon with "mesg" or "list".
 * Hide the balloon when both are NULL.
 */
    خالی
post_balloon(BalloonEval *beval غیر_مستعمل, char_u *mesg, list_T *list غیر_مستعمل)
{
# ifdef FEAT_BEVAL_رکن
#  ifdef FEAT_GUI
    اگر (!gui.اندر_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    اگر (gui.اندر_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    صحیح
can_use_beval(خالی)
{
    واپس (0
# ifdef FEAT_BEVAL_GUI
		|| (gui.اندر_use && p_beval)
# endif
# ifdef FEAT_BEVAL_رکن
		|| (
#  ifdef FEAT_GUI
		    !gui.اندر_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && پیغام_scrolled == 0;
}

# ifdef FEAT_EVAL
/*
 * Evaluate the expression 'bexpr' and set the text in the balloon 'beval'.
 */
    ساکن خالی
bexpr_eval(
	BalloonEval	*beval,
	char_u		*bexpr,
	win_T		*wp,
	linenr_T	lnum,
	صحیح		ستون,
	char_u		*متن)
{
    win_T	*cw;
    طویل	winnr = 0;
    صحیح		use_sandbox;
    ساکن char_u  *نتیجہ = تہی;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    برائے (cw = firstwin; cw != wp; cw = cw->w_اگلا)
	++winnr;

    سیٹ_vim_var_nr(VV_BEVAL_BUFNR, (طویل)wp->w_بفر->b_fnum);
    سیٹ_vim_var_nr(VV_BEVAL_WINNR, winnr);
    سیٹ_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    سیٹ_vim_var_nr(VV_BEVAL_LNUM, (طویل)lnum);
    سیٹ_vim_var_nr(VV_BEVAL_ستون, (طویل)(ستون + 1));
    سیٹ_vim_var_string(VV_BEVAL_متن, متن, -1);
    vim_free(متن);

    use_sandbox = was_سیٹ_insecurely(wp, (char_u *)"balloonexpr",
			    *wp->w_بفر->b_p_bexpr == NUL ? 0 : OPT_LOCAL);
    اگر (use_sandbox)
	++sandbox;
    ++textlock;

    اگر (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	اگر (sp != تہی)
	    current_sctx = *sp;
    }
    ورنہ
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(نتیجہ);
    نتیجہ = eval_مقصد_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    اگر (نتیجہ != تہی)
    {
	len = STRLEN(نتیجہ);
	اگر (len > 0 && نتیجہ[len - 1] == NL)
	    نتیجہ[len - 1] = NUL;
    }

    اگر (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    سیٹ_vim_var_string(VV_BEVAL_متن, تہی, -1);
    اگر (نتیجہ != تہی && نتیجہ[0] != NUL)
	post_balloon(beval, نتیجہ, تہی);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    اگر (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    خالی
general_beval_cb(BalloonEval *beval, صحیح state غیر_مستعمل)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    صحیح		ستون;
    linenr_T	lnum;
    char_u	*متن;
    char_u	*bexpr;
# endif
    ساکن صحیح	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    اگر (!can_use_beval() || beval == تہی)
	واپس;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    اگر (recursive)
	واپس;
    recursive = TRUE;

# ifdef FEAT_EVAL
    اگر (get_beval_info(beval, TRUE, &wp, &lnum, &متن, &ستون) == OK)
    {
	bexpr = (*wp->w_بفر->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_بفر->b_p_bexpr;
	اگر (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, ستون, متن);
	    recursive = FALSE;
	    واپس;
	}
	ورنہ
	    vim_free(متن);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    اگر (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#اگر_ختم
