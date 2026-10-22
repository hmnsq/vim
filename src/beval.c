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
ڳوليو_لفظ_under_cursor(
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
    صحيح		کانو = mousecol;
    صحيح		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = عدم;
    wp = mouse_ڳوليو_win(&صف, &کانو, FAIL_POPUP);
    إذا (wp == عدم || صف < 0 || صف >= wp->w_اوچائي || کانو >= wp->w_ويڪر)
	أرجع FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    إذا (mouse_comp_pos(wp, &صف, &کانو, &lnum, عدم))
	أرجع FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_بفر, lnum, FALSE);
    إذا (کانو > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
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

	کانو = vcol2col(wp, lnum, کانو, عدم);
	scol = کانو;

	إذا (VIsual_سرگرم
		&& wp->w_بفر == curwin->w_بفر
		&& (lnum == spos->lnum
		    ? کانو >= (صحيح)spos->کانو
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? کانو <= (صحيح)epos->کانو
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    إذا (spos->lnum != epos->lnum || spos->کانو == epos->کانو)
		أرجع FAIL;

	    lbuf = ml_get_buf(curwin->w_بفر, VIsual.lnum, FALSE);
	    len = epos->کانو - spos->کانو;
	    إذا (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->کانو);
	    lbuf = vim_strnsave(lbuf + spos->کانو, len);
	    lnum = spos->lnum;
	    کانو = spos->کانو;
	    scol = کانو;
	}
	وإلا
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = ڳوليو_ident_at_pos(wp, lnum, (colnr_T)کانو,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    إذا (len == 0)
		أرجع FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    وإلا
	scol = کانو;

    إذا (winp != عدم)
	*winp = wp;
    إذا (lnump != عدم)
	*lnump = lnum;
    *textp = lbuf;
    إذا (colp != عدم)
	*colp = کانو;
    إذا (startcolp != عدم)
	*startcolp = scol;

    أرجع OK;
}
#جيڪڏهن_پورو

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
    صحيح		کانو = mouse_کانو;

# ifdef FEAT_BEVAL_GUI
    إذا (gui.اندريون_use)
    {
	صف = Y_2_صف(beval->y);
	کانو = X_2_کانو(beval->x);
    }
# endif
    إذا (ڳوليو_لفظ_under_cursor(صف, کانو, getword,
		ڳوليو_IDENT + ڳوليو_STRING + ڳوليو_EVAL,
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
post_balloon(BalloonEval *beval اڻ_استعمال_ٿيل, char_u *mesg, list_T *list اڻ_استعمال_ٿيل)
{
# ifdef FEAT_BEVAL_رڪن
#  ifdef FEAT_GUI
    إذا (!gui.اندريون_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    إذا (gui.اندريون_use)
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
		|| (gui.اندريون_use && p_beval)
# endif
# ifdef FEAT_BEVAL_رڪن
		|| (
#  ifdef FEAT_GUI
		    !gui.اندريون_use &&
#  endif
		    p_bevalterm)
# endif
	     ) && نياپو_scrolled == 0;
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
	صحيح		کانو,
	char_u		*متن)
{
    win_T	*cw;
    ڊگهو	winnr = 0;
    صحيح		use_sandbox;
    ساكن char_u  *نتيجو = عدم;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    لكل (cw = firstwin; cw != wp; cw = cw->w_ايندڙ)
	++winnr;

    سيٽ_vim_var_nr(VV_BEVAL_BUFNR, (ڊگهو)wp->w_بفر->b_fnum);
    سيٽ_vim_var_nr(VV_BEVAL_WINNR, winnr);
    سيٽ_vim_var_nr(VV_BEVAL_WINID, wp->w_id);
    سيٽ_vim_var_nr(VV_BEVAL_LNUM, (ڊگهو)lnum);
    سيٽ_vim_var_nr(VV_BEVAL_کانو, (ڊگهو)(کانو + 1));
    سيٽ_vim_var_string(VV_BEVAL_متن, متن, -1);
    vim_free(متن);

    use_sandbox = was_سيٽ_insecurely(wp, (char_u *)"balloonexpr",
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

    vim_free(نتيجو);
    نتيجو = eval_منزل_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    إذا (نتيجو != عدم)
    {
	len = STRLEN(نتيجو);
	إذا (len > 0 && نتيجو[len - 1] == NL)
	    نتيجو[len - 1] = NUL;
    }

    إذا (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    سيٽ_vim_var_string(VV_BEVAL_متن, عدم, -1);
    إذا (نتيجو != عدم && نتيجو[0] != NUL)
	post_balloon(beval, نتيجو, عدم);

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
general_beval_cb(BalloonEval *beval, صحيح state اڻ_استعمال_ٿيل)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    صحيح		کانو;
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
    إذا (get_beval_info(beval, TRUE, &wp, &lnum, &متن, &کانو) == OK)
    {
	bexpr = (*wp->w_بفر->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_بفر->b_p_bexpr;
	إذا (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, کانو, متن);
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

#جيڪڏهن_پورو
