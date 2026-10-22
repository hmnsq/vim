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

#གལ་ཏེ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
/*
 * Find text under the mouse position "row" / "col".
 * If "getword" is TRUE the returned text in "*textp" is not the whole line but
 * the relevant word in allocated memory.
 * Return OK if found.
 * Return FAIL if not found, no text at the mouse position.
 */
    ཧྲིལ
find_word_under_cursor(
	ཧྲིལ	    mouserow,
	ཧྲིལ	    mousecol,
	ཧྲིལ	    getword,
	ཧྲིལ	    flags,	// flags for find_ident_at_pos()
	win_T	    **winp,	// can be NULL
	linenr_T    *lnump,	// can be NULL
	char_u	    **textp,
	ཧྲིལ	    *colp,	// column where mouse hovers, can be NULL
	ཧྲིལ	    *startcolp) // column where text starts, can be NULL
{
    ཧྲིལ		row = mouserow;
    ཧྲིལ		col = mousecol;
    ཧྲིལ		scol;
    win_T	*wp;
    char_u	*lbuf;
    linenr_T	lnum;

    *textp = NULL;
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    གལ་ཏེ (wp == NULL || row < 0 || row >= wp->w_height || col >= wp->w_width)
	ཕྱིར FAIL;

    // Found a window and the cursor is in the text.  Now find the line
    // number.
    གལ་ཏེ (mouse_comp_pos(wp, &row, &col, &lnum, NULL))
	ཕྱིར FAIL;		// position is below the last line

    // Not past end of the file.
    lbuf = ml_get_buf(wp->w_buffer, lnum, FALSE);
    གལ་ཏེ (col > win_linetabsize(wp, lnum, lbuf, (colnr_T)MAXCOL))
	ཕྱིར FAIL;		// past end of line

    // Not past end of line.
    གལ་ཏེ (getword)
    {
	// For Netbeans we get the relevant part of the line
	// instead of the whole line.
	ཧྲིལ		len;
	pos_T	*spos = NULL, *epos = NULL;

	གལ་ཏེ (VIsual_active)
	{
	    གལ་ཏེ (LT_POS(VIsual, curwin->w_cursor))
	    {
		spos = &VIsual;
		epos = &curwin->w_cursor;
	    }
	    དེ་མིན
	    {
		spos = &curwin->w_cursor;
		epos = &VIsual;
	    }
	}

	col = vcol2col(wp, lnum, col, NULL);
	scol = col;

	གལ་ཏེ (VIsual_active
		&& wp->w_buffer == curwin->w_buffer
		&& (lnum == spos->lnum
		    ? col >= (ཧྲིལ)spos->col
		    : lnum > spos->lnum)
		&& (lnum == epos->lnum
		    ? col <= (ཧྲིལ)epos->col
		    : lnum < epos->lnum))
	{
	    // Visual mode and pointing to the line with the
	    // Visual selection: return selected text, with a
	    // maximum of one line.
	    གལ་ཏེ (spos->lnum != epos->lnum || spos->col == epos->col)
		ཕྱིར FAIL;

	    lbuf = ml_get_buf(curwin->w_buffer, VIsual.lnum, FALSE);
	    len = epos->col - spos->col;
	    གལ་ཏེ (*p_sel != 'e')
		len += mb_ptr2len(lbuf + epos->col);
	    lbuf = vim_strnsave(lbuf + spos->col, len);
	    lnum = spos->lnum;
	    col = spos->col;
	    scol = col;
	}
	དེ་མིན
	{
	    // Find the word under the cursor.
	    ++emsg_off;
	    len = find_ident_at_pos(wp, lnum, (colnr_T)col,
		    &lbuf, &scol, flags);
	    --emsg_off;
	    གལ་ཏེ (len == 0)
		ཕྱིར FAIL;
	    lbuf = vim_strnsave(lbuf, len);
	}
    }
    དེ་མིན
	scol = col;

    གལ་ཏེ (winp != NULL)
	*winp = wp;
    གལ་ཏེ (lnump != NULL)
	*lnump = lnum;
    *textp = lbuf;
    གལ་ཏེ (colp != NULL)
	*colp = col;
    གལ་ཏེ (startcolp != NULL)
	*startcolp = scol;

    ཕྱིར OK;
}
#endif

#གལ་ཏེ defined(FEAT_BEVAL)

/*
 * Get the text and position to be evaluated for "beval".
 * If "getword" is TRUE the returned text is not the whole line but the
 * relevant word in allocated memory.
 * Returns OK or FAIL.
 */
    ཧྲིལ
get_beval_info(
	BalloonEval	*beval,
	ཧྲིལ		getword,
	win_T		**winp,
	linenr_T	*lnump,
	char_u		**textp,
	ཧྲིལ		*colp)
{
    ཧྲིལ		row = mouse_row;
    ཧྲིལ		col = mouse_col;

# ifdef FEAT_BEVAL_GUI
    གལ་ཏེ (gui.in_use)
    {
	row = Y_2_ROW(beval->y);
	col = X_2_COL(beval->x);
    }
# endif
    གལ་ཏེ (find_word_under_cursor(row, col, getword,
		FIND_IDENT + FIND_STRING + FIND_EVAL,
		winp, lnump, textp, colp, NULL) == OK)
    {
# ifdef FEAT_VARTABS
	vim_free(beval->vts);
	beval->vts = tabstop_copy((*winp)->w_buffer->b_p_vts_array);
	གལ་ཏེ ((*winp)->w_buffer->b_p_vts_array != NULL && beval->vts == NULL)
	{
	    གལ་ཏེ (getword)
		vim_free(*textp);
	    ཕྱིར FAIL;
	}
# endif
	beval->ts = (*winp)->w_buffer->b_p_ts;
	ཕྱིར OK;
    }

    ཕྱིར FAIL;
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
    གལ་ཏེ (!gui.in_use)
#  endif
	ui_post_balloon(mesg, list);
# endif
# ifdef FEAT_BEVAL_GUI
    གལ་ཏེ (gui.in_use)
	// GUI can't handle a list
	gui_mch_post_balloon(beval, mesg);
# endif
}

/*
 * Returns TRUE if the balloon eval has been enabled:
 * 'ballooneval' for the GUI and 'balloonevalterm' for the terminal.
 * Also checks if the screen isn't scrolled up.
 */
    ཧྲིལ
can_use_beval(void)
{
    ཕྱིར (0
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
	ཧྲིལ		col,
	char_u		*text)
{
    win_T	*cw;
    long	winnr = 0;
    ཧྲིལ		use_sandbox;
    static char_u  *result = NULL;
    size_t	len;

    sctx_T	save_sctx = current_sctx;

    // Convert window pointer to number.
    ཆེད་དུ (cw = firstwin; cw != wp; cw = cw->w_next)
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
    གལ་ཏེ (use_sandbox)
	++sandbox;
    ++textlock;

    གལ་ཏེ (bexpr == p_bexpr)
    {
	sctx_T *sp = get_option_sctx("balloonexpr");

	གལ་ཏེ (sp != NULL)
	    current_sctx = *sp;
    }
    དེ་མིན
	current_sctx = curbuf->b_p_script_ctx[BV_BEXPR];

    vim_free(result);
    result = eval_to_string(bexpr, TRUE, TRUE);

    // Remove one trailing newline, it is added when the result was a
    // list and it's hardly ever useful.  If the user really wants a
    // trailing newline he can add two and one remains.
    གལ་ཏེ (result != NULL)
    {
	len = STRLEN(result);
	གལ་ཏེ (len > 0 && result[len - 1] == NL)
	    result[len - 1] = NUL;
    }

    གལ་ཏེ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    set_vim_var_string(VV_BEVAL_TEXT, NULL, -1);
    གལ་ཏེ (result != NULL && result[0] != NUL)
	post_balloon(beval, result, NULL);

    // The 'balloonexpr' evaluation may show something on the screen
    // that requires a screen update.
    གལ་ཏེ (must_redraw)
	redraw_after_callback(FALSE, FALSE);
}
# endif

/*
 * Common code, invoked when the mouse is resting for a moment.
 */
    void
general_beval_cb(BalloonEval *beval, ཧྲིལ state UNUSED)
{
# ifdef FEAT_EVAL
    win_T	*wp;
    ཧྲིལ		col;
    linenr_T	lnum;
    char_u	*text;
    char_u	*bexpr;
# endif
    static ཧྲིལ	recursive = FALSE;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no beval area.
    གལ་ཏེ (!can_use_beval() || beval == NULL)
	ཕྱིར;

    // Don't do this recursively.  Happens when the expression evaluation
    // takes a long time and invokes something that checks for CTRL-C typed.
    གལ་ཏེ (recursive)
	ཕྱིར;
    recursive = TRUE;

# ifdef FEAT_EVAL
    གལ་ཏེ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) == OK)
    {
	bexpr = (*wp->w_buffer->b_p_bexpr == NUL) ? p_bexpr
						    : wp->w_buffer->b_p_bexpr;
	གལ་ཏེ (*bexpr != NUL)
	{
	    bexpr_eval(beval, bexpr, wp, lnum, col, text);
	    recursive = FALSE;
	    ཕྱིར;
	}
	དེ་མིན
	    vim_free(text);
    }
# endif
# ifdef FEAT_NETBEANS_INTG
    གལ་ཏེ (bevalServers & BEVAL_NETBEANS)
	netbeans_beval_cb(beval, state);
# endif

    recursive = FALSE;
}

#endif
