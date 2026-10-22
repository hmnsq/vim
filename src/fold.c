/* vi:set ts=8 sts=4 sw=4 noet:
 * vim600:fdm=marker fdl=1 fdc=3:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * fold.c: code for folding
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
రకనిర్వచనం నిర్మాణం
{
    linenr_T	fd_శిఖరం;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    అక్షరం	fd_flags;	// see below
    అక్షరం	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
స్థిర శూన్య newFoldLevelWin(win_T *wp);
స్థిర పూర్ణాంకం checkCloseRec(garray_T *gap, linenr_T lnum, పూర్ణాంకం level);
స్థిర పూర్ణాంకం fold_కనుగొను(garray_T *gap, linenr_T lnum, fold_T **fpp);
స్థిర పూర్ణాంకం foldLevelWin(win_T *wp, linenr_T lnum);
స్థిర శూన్య checkupdate(win_T *wp);
స్థిర శూన్య సమితి_Fold_Repeat(linenr_T lnum, దీర్ఘ లెక్క, పూర్ణాంకం do_open);
స్థిర linenr_T సమితి_Manual_Fold(linenr_T lnum, పూర్ణాంకం opening, పూర్ణాంకం recurse, పూర్ణాంకం *donep);
స్థిర linenr_T సమితి_Manual_Fold_Win(win_T *wp, linenr_T lnum, పూర్ణాంకం opening, పూర్ణాంకం recurse, పూర్ణాంకం *donep);
స్థిర శూన్య foldOpenNested(fold_T *fpr);
స్థిర శూన్య delete_Fold_నమోదు(garray_T *gap, పూర్ణాంకం idx, పూర్ణాంకం recursive);
స్థిర శూన్య foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, దీర్ఘ amount, దీర్ఘ amount_after);
స్థిర పూర్ణాంకం get_అత్యంతలోతు_Nesting_Recurse(garray_T *gap);
స్థిర పూర్ణాంకం check_closed(win_T *win, fold_T *fp, పూర్ణాంకం *use_levelp, పూర్ణాంకం level, పూర్ణాంకం *maybe_smallp, linenr_T lnum_off);
స్థిర శూన్య checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
స్థిర శూన్య సమితి_Small_Maybe(garray_T *gap);
స్థిర శూన్య foldCreateMarkers(linenr_T ప్రారంభం, linenr_T చివర);
స్థిర శూన్య fold_కూడు_Marker(linenr_T lnum, char_u *marker, పూర్ణాంకం markerlen);
స్థిర శూన్య deleteFoldMarkers(fold_T *fp, పూర్ణాంకం recursive, linenr_T lnum_off);
స్థిర శూన్య foldDelMarker(linenr_T lnum, char_u *marker, పూర్ణాంకం markerlen);
స్థిర శూన్య foldUpdateIEMS(win_T *wp, linenr_T శిఖరం, linenr_T bot);
స్థిర శూన్య parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
స్థిర linenr_T invalid_శిఖరం = (linenr_T)0;
స్థిర linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
స్థిర linenr_T prev_lnum = 0;
స్థిర పూర్ణాంకం prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define పూర్తి_NOTHING	0
# define పూర్తి_ACTION	1	// did close or open a fold
# define పూర్తి_FOLD	2	// did find a fold

స్థిర పూర్ణాంకం foldstartmarkerlen;
స్థిర char_u *foldendmarker;
స్థిర పూర్ణాంకం foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    శూన్య
copyFoldingState(win_T *wp_from, win_T *wp_గమ్యం)
{
    wp_గమ్యం->w_fold_manual = wp_from->w_fold_manual;
    wp_గమ్యం->w_foldinvalid = wp_from->w_foldinvalid;
    cloneFoldGrowArray(&wp_from->w_folds, &wp_గమ్యం->w_folds);
}

// hasAnyFolding() {{{2
/*
 * Return TRUE if there may be folded lines in window "win".
 */
    పూర్ణాంకం
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    తిరిగి (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    పూర్ణాంకం
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    తిరిగి hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ఖాళీ);
}

// hasFoldingWin() {{{2
    పూర్ణాంకం
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    పూర్ణాంకం		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    పూర్ణాంకం		had_folded = FALSE;
    linenr_T	మొదటి = 0;
    linenr_T	చివరి = 0;
    linenr_T	lnum_rel = lnum;
    పూర్ణాంకం		x;
    fold_T	*fp;
    పూర్ణాంకం		level = 0;
    పూర్ణాంకం		use_level = FALSE;
    పూర్ణాంకం		maybe_small = FALSE;
    garray_T	*gap;
    పూర్ణాంకం		దిగువ_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    ఒకవేళ (!hasAnyFolding(win))
    {
	ఒకవేళ (infop != ఖాళీ)
	    infop->fi_level = 0;
	తిరిగి FALSE;
    }

    ఒకవేళ (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = కనుగొను_wl_నమోదు(win, lnum);
	ఒకవేళ (x >= 0)
	{
	    మొదటి = win->w_పంక్తులు[x].wl_lnum;
	    చివరి = win->w_పంక్తులు[x].wl_lastlnum;
	    had_folded = win->w_పంక్తులు[x].wl_folded;
	}
    }

    ఒకవేళ (మొదటి == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	కొరకు (;;)
	{
	    ఒకవేళ (!fold_కనుగొను(gap, lnum_rel, &fp))
		ఆపు;

	    // Remember lowest level of fold that starts in "lnum".
	    ఒకవేళ (lnum_rel == fp->fd_శిఖరం && దిగువ_level == 0)
		దిగువ_level = level + 1;

	    మొదటి += fp->fd_శిఖరం;
	    చివరి += fp->fd_శిఖరం;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    ఒకవేళ (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		చివరి += fp->fd_len - 1;
		ఆపు;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_శిఖరం;
	    ++level;
	}
    }

    ఒకవేళ (!had_folded)
    {
	ఒకవేళ (infop != ఖాళీ)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_దిగువ_level = దిగువ_level == 0 ? level : దిగువ_level;
	}
	తిరిగి FALSE;
    }

    ఒకవేళ (చివరి > win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	చివరి = win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (lastp != ఖాళీ)
	*lastp = చివరి;
    ఒకవేళ (firstp != ఖాళీ)
	*firstp = మొదటి;
    ఒకవేళ (infop != ఖాళీ)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = మొదటి;
	infop->fi_దిగువ_level = దిగువ_level == 0 ? level + 1 : దిగువ_level;
    }
    తిరిగి TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    పూర్ణాంకం
పంక్తి_Folded(win_T *win, linenr_T lnum)
{
    తిరిగి folded_లెక్క(win, lnum, ఖాళీ) != 0;
}

// foldedCount() {{{2
/*
 * Count the number of lines that are folded at line number "lnum".
 * Normally "lnum" is the first line of a possible fold, and the returned
 * number is the number of lines in the fold.
 * Doesn't use caching from the displayed window.
 * Returns number of folded lines from "lnum", or 0 if line is not folded.
 * When "infop" is not NULL, fills *infop with the fold level info.
 */
    దీర్ఘ
folded_లెక్క(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	చివరి;

    ఒకవేళ (hasFoldingWin(win, lnum, ఖాళీ, &చివరి, FALSE, infop))
	తిరిగి (దీర్ఘ)(చివరి - lnum + 1);
    తిరిగి 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    పూర్ణాంకం
foldmethodIsManual(win_T *wp)
{
    తిరిగి (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    పూర్ణాంకం
foldmethodIsIndent(win_T *wp)
{
    తిరిగి (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    పూర్ణాంకం
foldmethod_Is_సమాసం(win_T *wp)
{
    తిరిగి (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    పూర్ణాంకం
foldmethodIsMarker(win_T *wp)
{
    తిరిగి (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    పూర్ణాంకం
foldmethodIsSyntax(win_T *wp)
{
    తిరిగి (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    పూర్ణాంకం
foldmethodIsDiff(win_T *wp)
{
    తిరిగి (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    శూన్య
closeFold(linenr_T lnum, దీర్ఘ లెక్క)
{
    సమితి_Fold_Repeat(lnum, లెక్క, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    శూన్య
closeFoldRecurse(linenr_T lnum)
{
    (శూన్య)సమితి_Manual_Fold(lnum, FALSE, TRUE, ఖాళీ);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    శూన్య
opFoldRange(
    linenr_T	మొదటి,
    linenr_T	చివరి,
    పూర్ణాంకం		opening,	// TRUE to open, FALSE to close
    పూర్ణాంకం		recurse,	// TRUE to do it recursively
    పూర్ణాంకం		had_visual)	// TRUE when Visual selection used
{
    పూర్ణాంకం		పూర్తి = పూర్తి_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_తదుపరి;

    కొరకు (lnum = మొదటి; lnum <= చివరి; lnum = lnum_తదుపరి + 1)
    {
	lnum_తదుపరి = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	ఒకవేళ (opening && !recurse)
	    (శూన్య)hasFolding(lnum, ఖాళీ, &lnum_తదుపరి);
	(శూన్య)సమితి_Manual_Fold(lnum, opening, recurse, &పూర్తి);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	ఒకవేళ (!opening && !recurse)
	    (శూన్య)hasFolding(lnum, ఖాళీ, &lnum_తదుపరి);
    }
    ఒకవేళ (పూర్తి == పూర్తి_NOTHING)
	emsg(_(e_no_fold_దొరికింది));
    // Force a redraw to remove the Visual highlighting.
    ఒకవేళ (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    శూన్య
openFold(linenr_T lnum, దీర్ఘ లెక్క)
{
    సమితి_Fold_Repeat(lnum, లెక్క, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    శూన్య
openFoldRecurse(linenr_T lnum)
{
    (శూన్య)సమితి_Manual_Fold(lnum, TRUE, TRUE, ఖాళీ);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    శూన్య
foldOpenCursor(శూన్య)
{
    పూర్ణాంకం		పూర్తి;

    checkupdate(curwin);
    ఒకవేళ (hasAnyFolding(curwin))
	కొరకు (;;)
	{
	    పూర్తి = పూర్తి_NOTHING;
	    (శూన్య)సమితి_Manual_Fold(curwin->w_cursor.lnum, TRUE, FALSE, &పూర్తి);
	    ఒకవేళ (!(పూర్తి & పూర్తి_ACTION))
		ఆపు;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    శూన్య
newFoldLevel(శూన్య)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    ఒకవేళ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    స్థిర శూన్య
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    పూర్ణాంకం		i;

    checkupdate(wp);
    ఒకవేళ (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_దత్తాంశం;
	కొరకు (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    మారింది_కిటికీ_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    శూన్య
foldCheckClose(శూన్య)
{
    ఒకవేళ (*p_fcl == NUL)
	తిరిగి;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    ఒకవేళ (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(పూర్ణాంకం)curwin->w_p_fdl))
	మారింది_కిటికీ_setting();
}

// checkCloseRec() {{{2
    స్థిర పూర్ణాంకం
checkCloseRec(garray_T *gap, linenr_T lnum, పూర్ణాంకం level)
{
    fold_T	*fp;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		i;

    fp = (fold_T *)gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	ఒకవేళ (fp[i].fd_flags == FD_OPEN)
	{
	    ఒకవేళ (level <= 0 && (lnum < fp[i].fd_శిఖరం
				      || lnum >= fp[i].fd_శిఖరం + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    లేకపోతే
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_శిఖరం,
								   level - 1);
	}
    }
    తిరిగి retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    పూర్ణాంకం
foldManualAllowed(పూర్ణాంకం create)
{
    ఒకవేళ (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	తిరిగి TRUE;
    ఒకవేళ (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    లేకపోతే
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    తిరిగి FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    శూన్య
foldCreate(linenr_T ప్రారంభం, linenr_T చివర)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		cont;
    పూర్ణాంకం		use_level = FALSE;
    పూర్ణాంకం		closed = FALSE;
    పూర్ణాంకం		level = 0;
    linenr_T	ప్రారంభం_rel = ప్రారంభం;
    linenr_T	చివర_rel = చివర;

    ఒకవేళ (ప్రారంభం > చివర)
    {
	// reverse the range
	చివర = ప్రారంభం_rel;
	ప్రారంభం = చివర_rel;
	ప్రారంభం_rel = ప్రారంభం;
	చివర_rel = చివర;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    ఒకవేళ (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(ప్రారంభం, చివర);
	తిరిగి;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    ఒకవేళ (gap->ga_len == 0)
	i = 0;
    లేకపోతే
    {
	కొరకు (;;)
	{
	    ఒకవేళ (!fold_కనుగొను(gap, ప్రారంభం_rel, &fp))
		ఆపు;
	    ఒకవేళ (fp->fd_శిఖరం + fp->fd_len > చివర_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		ప్రారంభం_rel -= fp->fd_శిఖరం;
		చివర_rel -= fp->fd_శిఖరం;
		ఒకవేళ (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    ఒకవేళ (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		లేకపోతే ఒకవేళ (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    లేకపోతే
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		ఆపు;
	    }
	}
	ఒకవేళ (gap->ga_len == 0)
	    i = 0;
	లేకపోతే
	    i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం);
    }

    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి;

    fp = (fold_T *)gap->ga_దత్తాంశం + i;
    ga_init2(&fold_ga, పరిమాణం(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    కొరకు (cont = 0; i + cont < gap->ga_len; ++cont)
	ఒకవేళ (fp[cont].fd_శిఖరం > చివర_rel)
	    ఆపు;
    ఒకవేళ (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	ఒకవేళ (ప్రారంభం_rel > fp->fd_శిఖరం)
	    ప్రారంభం_rel = fp->fd_శిఖరం;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	ఒకవేళ (చివర_rel < fp[cont - 1].fd_శిఖరం + fp[cont - 1].fd_len - 1)
	    చివర_rel = fp[cont - 1].fd_శిఖరం + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_దత్తాంశం, fp, పరిమాణం(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	కొరకు (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_దత్తాంశం)[j].fd_శిఖరం -= ప్రారంభం_rel;
    }
    // Move remaining entries to after the new fold.
    ఒకవేళ (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_దత్తాంశం + i,
		పరిమాణం(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_శిఖరం = ప్రారంభం_rel;
    fp->fd_len = చివర_rel - ప్రారంభం_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    ఒకవేళ (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(ప్రారంభం, 1L);
    ఒకవేళ (!use_level)
	curwin->w_fold_manual = true;
    fp->fd_flags = FD_CLOSED;
    fp->fd_small = MAYBE;

    // redraw
    మారింది_కిటికీ_setting();
}

// deleteFold() {{{2
/*
 * Delete a fold at line "start" in the current window.
 * When "end" is not 0, delete all folds from "start" to "end".
 * When "recursive" is TRUE delete recursively.
 */
    శూన్య
deleteFold(
    linenr_T	ప్రారంభం,
    linenr_T	చివర,
    పూర్ణాంకం		recursive,
    పూర్ణాంకం		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*దొరికింది_ga;
    fold_T	*దొరికింది_fp = ఖాళీ;
    linenr_T	దొరికింది_off = 0;
    పూర్ణాంకం		use_level;
    పూర్ణాంకం		maybe_small = FALSE;
    పూర్ణాంకం		level = 0;
    linenr_T	lnum = ప్రారంభం;
    linenr_T	lnum_off;
    పూర్ణాంకం		did_one = FALSE;
    linenr_T	మొదటి_lnum = MAXLNUM;
    linenr_T	చివరి_lnum = 0;

    checkupdate(curwin);

    అయితే (lnum <= చివర)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	దొరికింది_ga = ఖాళీ;
	lnum_off = 0;
	use_level = FALSE;
	కొరకు (;;)
	{
	    ఒకవేళ (!fold_కనుగొను(gap, lnum - lnum_off, &fp))
		ఆపు;
	    // lnum is inside this fold, remember info
	    దొరికింది_ga = gap;
	    దొరికింది_fp = fp;
	    దొరికింది_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    ఒకవేళ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		ఆపు;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_శిఖరం;
	    ++level;
	}
	ఒకవేళ (దొరికింది_ga == ఖాళీ)
	{
	    ++lnum;
	}
	లేకపోతే
	{
	    lnum = దొరికింది_fp->fd_శిఖరం + దొరికింది_fp->fd_len + దొరికింది_off;

	    ఒకవేళ (foldmethodIsManual(curwin))
		delete_Fold_నమోదు(దొరికింది_ga,
		    (పూర్ణాంకం)(దొరికింది_fp - (fold_T *)దొరికింది_ga->ga_దత్తాంశం), recursive);
	    లేకపోతే
	    {
		ఒకవేళ (మొదటి_lnum > దొరికింది_fp->fd_శిఖరం + దొరికింది_off)
		    మొదటి_lnum = దొరికింది_fp->fd_శిఖరం + దొరికింది_off;
		ఒకవేళ (చివరి_lnum < lnum)
		    చివరి_lnum = lnum;
		ఒకవేళ (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(దొరికింది_fp, recursive, దొరికింది_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    మారింది_కిటికీ_setting();
	}
    }
    ఒకవేళ (!did_one)
    {
	emsg(_(e_no_fold_దొరికింది));
	// Force a redraw to remove the Visual highlighting.
	ఒకవేళ (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    లేకపోతే
	// Deleting markers may make cursor column invalid.
	check_cursor_నిలువు();

    ఒకవేళ (చివరి_lnum > 0)
	మారింది_పంక్తులు(మొదటి_lnum, (colnr_T)0, చివరి_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    శూన్య
clearFolding(win_T *win)
{
    deleteFoldRecurse(&win->w_folds);
    win->w_foldinvalid = false;
}

// foldUpdate() {{{2
/*
 * Update folds for changes in the buffer of a window.
 * Note that inserted/deleted lines must have already been taken care of by
 * calling foldMarkAdjust().
 * The changes in lines from top to bot (inclusive).
 */
    శూన్య
foldUpdate(win_T *wp, linenr_T శిఖరం, linenr_T bot)
{
    fold_T	*fp;

    ఒకవేళ (disable_fold_update > 0)
	తిరిగి;
# ifdef FEAT_DIFF
    ఒకవేళ (need_diff_redraw)
	// will update later
	తిరిగి;
# endif

    ఒకవేళ (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_ప్రారంభం = శిఖరం;
	linenr_T	maybe_small_చివర = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	ఒకవేళ (శిఖరం > bot)
	{
	    maybe_small_ప్రారంభం = bot;
	    maybe_small_చివర = శిఖరం;
	}
	(శూన్య)fold_కనుగొను(&wp->w_folds, maybe_small_ప్రారంభం, &fp);
	అయితే (fp < (fold_T *)wp->w_folds.ga_దత్తాంశం + wp->w_folds.ga_len
		&& fp->fd_శిఖరం <= maybe_small_చివర)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    ఒకవేళ (foldmethodIsIndent(wp)
	    || foldmethod_Is_సమాసం(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	పూర్ణాంకం save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, శిఖరం, bot);
	got_int |= save_got_int;
    }
}

// foldUpdateAll() {{{2
/*
 * Update all lines in a window for folding.
 * Used when a fold setting changes or after reloading the buffer.
 * The actual updating is postponed until fold info is used, to avoid doing
 * every time a setting is changed or a syntax item is added.
 */
    శూన్య
foldUpdateAll(win_T *win)
{
    win->w_foldinvalid = true;
    redraw_win_later(win, UPD_NOT_VALID);
}

// foldMoveTo() {{{2
/*
 * If "updown" is FALSE: Move to the start or end of the fold.
 * If "updown" is TRUE: move to fold at the same level.
 * If not moved return FAIL.
 */
    పూర్ణాంకం
fold_Move_గమ్యం(
    పూర్ణాంకం		updown,
    పూర్ణాంకం		dir,	    // FORWARD or BACKWARD
    దీర్ఘ	లెక్క)
{
    దీర్ఘ	n;
    పూర్ణాంకం		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_దొరికింది;
    linenr_T	lnum;
    పూర్ణాంకం		use_level;
    పూర్ణాంకం		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    పూర్ణాంకం		level;
    పూర్ణాంకం		చివరి;

    checkupdate(curwin);

    // Repeat "count" times.
    కొరకు (n = 0; n < లెక్క; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	ఒకవేళ (gap->ga_len == 0)
	    ఆపు;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_దొరికింది = curwin->w_cursor.lnum;
	level = 0;
	చివరి = FALSE;
	కొరకు (;;)
	{
	    ఒకవేళ (!fold_కనుగొను(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		ఒకవేళ (!updown || gap->ga_len == 0)
		    ఆపు;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		ఒకవేళ (dir == FORWARD)
		{
		    ఒకవేళ (fp - (fold_T *)gap->ga_దత్తాంశం >= gap->ga_len)
			ఆపు;
		    --fp;
		}
		లేకపోతే
		{
		    ఒకవేళ (fp == (fold_T *)gap->ga_దత్తాంశం)
			ఆపు;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		చివరి = TRUE;
	    }

	    ఒకవేళ (!చివరి)
	    {
		// Check if this fold is closed.
		ఒకవేళ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    చివరి = TRUE;

		// "[z" and "]z" stop at closed fold
		ఒకవేళ (చివరి && !updown)
		    ఆపు;
	    }

	    ఒకవేళ (updown)
	    {
		ఒకవేళ (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    ఒకవేళ (fp + 1 - (fold_T *)gap->ga_దత్తాంశం < gap->ga_len)
		    {
			lnum = fp[1].fd_శిఖరం + lnum_off;
			ఒకవేళ (lnum > curwin->w_cursor.lnum)
			    lnum_దొరికింది = lnum;
		    }
		}
		లేకపోతే
		{
		    // to end of previous fold if there is one
		    ఒకవేళ (fp > (fold_T *)gap->ga_దత్తాంశం)
		    {
			lnum = fp[-1].fd_శిఖరం + lnum_off + fp[-1].fd_len - 1;
			ఒకవేళ (lnum < curwin->w_cursor.lnum)
			    lnum_దొరికింది = lnum;
		    }
		}
	    }
	    లేకపోతే
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		ఒకవేళ (dir == FORWARD)
		{
		    lnum = fp->fd_శిఖరం + lnum_off + fp->fd_len - 1;
		    ఒకవేళ (lnum > curwin->w_cursor.lnum)
			lnum_దొరికింది = lnum;
		}
		లేకపోతే
		{
		    lnum = fp->fd_శిఖరం + lnum_off;
		    ఒకవేళ (lnum < curwin->w_cursor.lnum)
			lnum_దొరికింది = lnum;
		}
	    }

	    ఒకవేళ (చివరి)
		ఆపు;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_శిఖరం;
	    ++level;
	}
	ఒకవేళ (lnum_దొరికింది != curwin->w_cursor.lnum)
	{
	    ఒకవేళ (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_దొరికింది;
	    curwin->w_cursor.నిలువు = 0;
	    retval = OK;
	}
	లేకపోతే
	    ఆపు;
    }

    తిరిగి retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    శూన్య
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, పరిమాణం(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    పూర్ణాంకం
కనుగొను_wl_నమోదు(win_T *win, linenr_T lnum)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < win->w_పంక్తులు_valid; ++i)
	ఒకవేళ (win->w_పంక్తులు[i].wl_valid)
	{
	    ఒకవేళ (lnum < win->w_పంక్తులు[i].wl_lnum)
		తిరిగి -1;
	    ఒకవేళ (lnum <= win->w_పంక్తులు[i].wl_lastlnum)
		తిరిగి i;
	}
    తిరిగి -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    శూన్య
foldAdjustVisual(శూన్య)
{
    pos_T	*ప్రారంభం, *చివర;

    ఒకవేళ (!VIsual_చురుకైన || !hasAnyFolding(curwin))
	తిరిగి;

    ఒకవేళ (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	ప్రారంభం = &VIsual;
	చివర = &curwin->w_cursor;
    }
    లేకపోతే
    {
	ప్రారంభం = &curwin->w_cursor;
	చివర = &VIsual;
    }
    ఒకవేళ (hasFolding(ప్రారంభం->lnum, &ప్రారంభం->lnum, ఖాళీ))
	ప్రారంభం->నిలువు = 0;

    ఒకవేళ (!hasFolding(చివర->lnum, ఖాళీ, &చివర->lnum))
	తిరిగి;

    చివర->నిలువు = ml_get_len(చివర->lnum);
    ఒకవేళ (చివర->నిలువు > 0 && *p_sel == 'o')
	--చివర->నిలువు;
    // prevent cursor from moving on the trail byte
    ఒకవేళ (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    శూన్య
foldAdjustCursor(శూన్య)
{
    (శూన్య)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ఖాళీ);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    శూన్య
cloneFoldGrowArray(garray_T *from, garray_T *గమ్యం)
{
    పూర్ణాంకం		i;
    fold_T	*from_p;
    fold_T	*గమ్యం_p;

    ga_init2(గమ్యం, from->ga_itemsize, from->ga_growsize);
    ఒకవేళ (from->ga_len == 0 || ga_grow(గమ్యం, from->ga_len) == FAIL)
	తిరిగి;

    from_p = (fold_T *)from->ga_దత్తాంశం;
    గమ్యం_p = (fold_T *)గమ్యం->ga_దత్తాంశం;

    కొరకు (i = 0; i < from->ga_len; i++)
    {
	గమ్యం_p->fd_శిఖరం = from_p->fd_శిఖరం;
	గమ్యం_p->fd_len = from_p->fd_len;
	గమ్యం_p->fd_flags = from_p->fd_flags;
	గమ్యం_p->fd_small = from_p->fd_small;
	cloneFoldGrowArray(&from_p->fd_nested, &గమ్యం_p->fd_nested);
	++గమ్యం->ga_len;
	++from_p;
	++గమ్యం_p;
    }
}

// foldFind() {{{2
/*
 * Search for line "lnum" in folds of growarray "gap".
 * Set "*fpp" to the fold struct for the fold that contains "lnum" or
 * the first fold below it (careful: it can be beyond the end of the array!).
 * Returns FALSE when there is no fold that contains "lnum".
 */
    స్థిర పూర్ణాంకం
fold_కనుగొను(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	దిగువ, ఎగువ;
    fold_T	*fp;
    పూర్ణాంకం		i;

    ఒకవేళ (gap->ga_len == 0)
    {
	*fpp = ఖాళీ;
	తిరిగి FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_దత్తాంశం;
    దిగువ = 0;
    ఎగువ = gap->ga_len - 1;
    అయితే (దిగువ <= ఎగువ)
    {
	i = (దిగువ + ఎగువ) / 2;
	ఒకవేళ (fp[i].fd_శిఖరం > lnum)
	    // fold below lnum, adjust high
	    ఎగువ = i - 1;
	లేకపోతే ఒకవేళ (fp[i].fd_శిఖరం + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    దిగువ = i + 1;
	లేకపోతే
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    తిరిగి TRUE;
	}
    }
    *fpp = fp + దిగువ;
    తిరిగి FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    స్థిర పూర్ణాంకం
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    పూర్ణాంకం		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    కొరకు (;;)
    {
	ఒకవేళ (!fold_కనుగొను(gap, lnum_rel, &fp))
	    ఆపు;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_శిఖరం;
	++level;
    }

    తిరిగి level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    స్థిర శూన్య
checkupdate(win_T *wp)
{
    ఒకవేళ (!wp->w_foldinvalid)
	తిరిగి;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    స్థిర శూన్య
సమితి_Fold_Repeat(linenr_T lnum, దీర్ఘ లెక్క, పూర్ణాంకం do_open)
{
    పూర్ణాంకం		పూర్తి;
    దీర్ఘ	n;

    కొరకు (n = 0; n < లెక్క; ++n)
    {
	పూర్తి = పూర్తి_NOTHING;
	(శూన్య)సమితి_Manual_Fold(lnum, do_open, FALSE, &పూర్తి);
	ఒకవేళ (!(పూర్తి & పూర్తి_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    ఒకవేళ (n == 0 && !(పూర్తి & పూర్తి_FOLD))
		emsg(_(e_no_fold_దొరికింది));
	    ఆపు;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    స్థిర linenr_T
సమితి_Manual_Fold(
    linenr_T	lnum,
    పూర్ణాంకం		opening,    // TRUE when opening, FALSE when closing
    పూర్ణాంకం		recurse,    // TRUE when closing/opening recursive
    పూర్ణాంకం		*donep)
{
# ifdef FEAT_DIFF
    ఒకవేళ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		ఒకవేళ (dlnum != 0)
		    (శూన్య)సమితి_Manual_Fold_Win(wp, dlnum, opening, recurse, ఖాళీ);
	    }
	}
    }
# endif

    తిరిగి సమితి_Manual_Fold_Win(curwin, lnum, opening, recurse, donep);
}

// setManualFoldWin() {{{2
/*
 * Open or close the fold in window "wp" which contains "lnum".
 * "donep", when not NULL, points to flag that is set to DONE_FOLD when some
 * fold was found and to DONE_ACTION when some fold was opened or closed.
 * When "donep" is NULL give an error message when no fold was found for
 * "lnum", but only if "wp" is "curwin".
 * Return the line number of the next line that could be closed.
 * It's only valid when "opening" is TRUE!
 */
    స్థిర linenr_T
సమితి_Manual_Fold_Win(
    win_T	*wp,
    linenr_T	lnum,
    పూర్ణాంకం		opening,    // TRUE when opening, FALSE when closing
    పూర్ణాంకం		recurse,    // TRUE when closing/opening recursive
    పూర్ణాంకం		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*దొరికింది = ఖాళీ;
    పూర్ణాంకం		j;
    పూర్ణాంకం		level = 0;
    పూర్ణాంకం		use_level = FALSE;
    పూర్ణాంకం		దొరికింది_fold = FALSE;
    garray_T	*gap;
    linenr_T	తదుపరి = MAXLNUM;
    linenr_T	off = 0;
    పూర్ణాంకం		పూర్తి = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    కొరకు (;;)
    {
	ఒకవేళ (!fold_కనుగొను(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    ఒకవేళ (fp != ఖాళీ && fp < (fold_T *)gap->ga_దత్తాంశం + gap->ga_len)
		తదుపరి = fp->fd_శిఖరం + off;
	    ఆపు;
	}

	// lnum is inside this fold
	దొరికింది_fold = TRUE;

	// If there is a following fold, continue there next time.
	ఒకవేళ (fp + 1 < (fold_T *)gap->ga_దత్తాంశం + gap->ga_len)
	    తదుపరి = fp[1].fd_శిఖరం + off;

	// Change from level-dependent folding to manual.
	ఒకవేళ (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    ఒకవేళ (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    లేకపోతే
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_దత్తాంశం;
	    కొరకు (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	ఒకవేళ (!opening && recurse)
	{
	    ఒకవేళ (fp->fd_flags != FD_CLOSED)
	    {
		పూర్తి |= పూర్తి_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	లేకపోతే ఒకవేళ (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    ఒకవేళ (opening)
	    {
		fp->fd_flags = FD_OPEN;
		పూర్తి |= పూర్తి_ACTION;
		ఒకవేళ (recurse)
		    foldOpenNested(fp);
	    }
	    ఆపు;
	}

	// fold is open, check nested folds
	దొరికింది = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_శిఖరం;
	off += fp->fd_శిఖరం;
	++level;
    }
    ఒకవేళ (దొరికింది_fold)
    {
	// When closing and not recurse, close deepest open fold.
	ఒకవేళ (!opening && దొరికింది != ఖాళీ)
	{
	    దొరికింది->fd_flags = FD_CLOSED;
	    పూర్తి |= పూర్తి_ACTION;
	}
	wp->w_fold_manual = true;
	ఒకవేళ (పూర్తి & పూర్తి_ACTION)
	    మారింది_కిటికీ_setting_win(wp);
	పూర్తి |= పూర్తి_FOLD;
    }
    లేకపోతే ఒకవేళ (donep == ఖాళీ && wp == curwin)
	emsg(_(e_no_fold_దొరికింది));

    ఒకవేళ (donep != ఖాళీ)
	*donep |= పూర్తి;

    తిరిగి తదుపరి;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    స్థిర శూన్య
foldOpenNested(fold_T *fpr)
{
    పూర్ణాంకం		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_దత్తాంశం;
    కొరకు (i = 0; i < fpr->fd_nested.ga_len; ++i)
    {
	foldOpenNested(&fp[i]);
	fp[i].fd_flags = FD_OPEN;
    }
}

// deleteFoldEntry() {{{2
/*
 * Delete fold "idx" from growarray "gap".
 * When "recursive" is TRUE also delete all the folds contained in it.
 * When "recursive" is FALSE contained folds are moved one level up.
 */
    స్థిర శూన్య
delete_Fold_నమోదు(garray_T *gap, పూర్ణాంకం idx, పూర్ణాంకం recursive)
{
    fold_T	*fp;
    పూర్ణాంకం		i;
    దీర్ఘ	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_దత్తాంశం + idx;
    ఒకవేళ (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	ఒకవేళ (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, పరిమాణం(fold_T) * (gap->ga_len - idx));
    }
    లేకపోతే
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	ఒకవేళ (ga_grow(gap, (పూర్ణాంకం)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_దత్తాంశం + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_దత్తాంశం;
	    కొరకు (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_శిఖరం += fp->fd_శిఖరం;
		ఒకవేళ (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		ఒకవేళ (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    ఒకవేళ (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  పరిమాణం(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(పరిమాణం(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    శూన్య
deleteFoldRecurse(garray_T *gap)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_T *)(gap->ga_దత్తాంశం))[i].fd_nested));
    ga_clear(gap);
}

// foldMarkAdjust() {{{2
/*
 * Update line numbers of folds for inserted/deleted lines.
 *
 * We are adjusting the folds in the range from line1 til line2,
 * make sure that line2 does not get smaller than line1
 */
    శూన్య
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    ఒకవేళ (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    ఒకవేళ (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    ఒకవేళ ((State & బహుళకం_చొప్పించు) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    స్థిర శూన్య
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after)
{
    fold_T	*fp;
    పూర్ణాంకం		i;
    linenr_T	చివరి;
    linenr_T	శిఖరం;

    ఒకవేళ (gap->ga_len == 0)
	తిరిగి;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    ఒకవేళ ((State & బహుళకం_చొప్పించు) && amount == (linenr_T)1 && line2 == MAXLNUM)
	శిఖరం = line1 + 1;
    లేకపోతే
	శిఖరం = line1;

    // Find the fold containing or just below "line1".
    (శూన్య)fold_కనుగొను(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    కొరకు (i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం); i < gap->ga_len; ++i, ++fp)
    {
	/*
	 * Check for these situations:
	 *	  1  2	3
	 *	  1  2	3
	 * line1     2	3  4  5
	 *	     2	3  4  5
	 *	     2	3  4  5
	 * line2     2	3  4  5
	 *		3     5  6
	 *		3     5  6
	 */

	చివరి = fp->fd_శిఖరం + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	ఒకవేళ (చివరి < line1)
	    కొనసాగు;

	// 6. fold below line2: only adjust for amount_after
	ఒకవేళ (fp->fd_శిఖరం > line2)
	{
	    ఒకవేళ (amount_after == 0)
		ఆపు;
	    fp->fd_శిఖరం += amount_after;
	}
	లేకపోతే
	{
	    ఒకవేళ (fp->fd_శిఖరం >= శిఖరం && చివరి <= line2)
	    {
		// 4. fold completely contained in range
		ఒకవేళ (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    delete_Fold_నమోదు(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		లేకపోతే
		    fp->fd_శిఖరం += amount;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (fp->fd_శిఖరం < శిఖరం)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_శిఖరం,
				  line2 - fp->fd_శిఖరం, amount, amount_after);
		    ఒకవేళ (చివరి <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			ఒకవేళ (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_శిఖరం;
			లేకపోతే
			    fp->fd_len += amount;
		    }
		    లేకపోతే
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		లేకపోతే
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    ఒకవేళ (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_శిఖరం,
				  amount,
				  amount_after + (fp->fd_శిఖరం - శిఖరం));
			fp->fd_len -= line2 - fp->fd_శిఖరం + 1;
			fp->fd_శిఖరం = line1;
		    }
		    లేకపోతే
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_శిఖరం,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_శిఖరం += amount;
		    }
		}
	    }
	}
    }
}

// getDeepestNesting() {{{2
/*
 * Get the lowest 'foldlevel' value that makes the deepest nested fold in the
 * current window open.
 */
    పూర్ణాంకం
get_అత్యంతలోతు_Nesting(శూన్య)
{
    checkupdate(curwin);
    తిరిగి get_అత్యంతలోతు_Nesting_Recurse(&curwin->w_folds);
}

    స్థిర పూర్ణాంకం
get_అత్యంతలోతు_Nesting_Recurse(garray_T *gap)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		level;
    పూర్ణాంకం		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	level = get_అత్యంతలోతు_Nesting_Recurse(&fp[i].fd_nested) + 1;
	ఒకవేళ (level > maxlevel)
	    maxlevel = level;
    }

    తిరిగి maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    స్థిర పూర్ణాంకం
check_closed(
    win_T	*win,
    fold_T	*fp,
    పూర్ణాంకం		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    పూర్ణాంకం		level,		    // folding depth
    పూర్ణాంకం		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    పూర్ణాంకం		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    ఒకవేళ (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	ఒకవేళ (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    లేకపోతే ఒకవేళ (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    ఒకవేళ (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    ఒకవేళ (closed)
    {
	ఒకవేళ (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	ఒకవేళ (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    తిరిగి closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    స్థిర శూన్య
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		n;

    ఒకవేళ (fp->fd_small != MAYBE)
	తిరిగి;

    // Mark any nested folds to maybe-small
    సమితి_Small_Maybe(&fp->fd_nested);

    ఒకవేళ (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    లేకపోతే
    {
	లెక్క = 0;
	కొరకు (n = 0; n < fp->fd_len; ++n)
	{
	    లెక్క += plines_win_nofold(wp, fp->fd_శిఖరం + lnum_off + n);
	    ఒకవేళ (లెక్క > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		తిరిగి;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    స్థిర శూన్య
సమితి_Small_Maybe(garray_T *gap)
{
    పూర్ణాంకం		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    స్థిర శూన్య
foldCreateMarkers(linenr_T ప్రారంభం, linenr_T చివర)
{
    ఒకవేళ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	తిరిగి;
    }
    parseMarker(curwin);

    fold_కూడు_Marker(ప్రారంభం, curwin->w_p_fmr, foldstartmarkerlen);
    fold_కూడు_Marker(చివర, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    మారింది_పంక్తులు(ప్రారంభం, (colnr_T)0, చివర, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    స్థిర శూన్య
fold_కూడు_Marker(linenr_T lnum, char_u *marker, పూర్ణాంకం markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*పంక్తి;
    పూర్ణాంకం		పంక్తి_len;
    char_u	*newline;
    char_u	*p = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)curbuf->b_p_cms, "%s");
    పూర్ణాంకం		పంక్తి_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    పంక్తి = ml_get(lnum);
    పంక్తి_len = ml_get_len(lnum);

    ఒకవేళ (u_save(lnum - 1, lnum + 1) != OK)
	తిరిగి;

    // Check if the line ends with an unclosed comment
    (శూన్య)skip_comment(పంక్తి, FALSE, FALSE, &పంక్తి_is_comment);
    newline = alloc(పంక్తి_len + markerlen + STRLEN(cms) + 1);
    ఒకవేళ (newline == ఖాళీ)
	తిరిగి;
    STRCPY(newline, పంక్తి);
    // Append the marker to the end of the line
    ఒకవేళ (p == ఖాళీ || పంక్తి_is_comment)
	vim_strncpy(newline + పంక్తి_len, marker, markerlen);
    లేకపోతే
    {
	STRCPY(newline + పంక్తి_len, cms);
	STRNCPY(newline + పంక్తి_len + (p - cms), marker, markerlen);
	STRCPY(newline + పంక్తి_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    స్థిర శూన్య
deleteFoldMarkers(
    fold_T	*fp,
    పూర్ణాంకం		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    పూర్ణాంకం		i;

    ఒకవేళ (recursive)
	కొరకు (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_దత్తాంశం + i, TRUE,
						       lnum_off + fp->fd_శిఖరం);
    foldDelMarker(fp->fd_శిఖరం + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_శిఖరం + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    స్థిర శూన్య
foldDelMarker(linenr_T lnum, char_u *marker, పూర్ణాంకం markerlen)
{
    char_u	*పంక్తి;
    char_u	*newline;
    char_u	*p;
    పూర్ణాంకం		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;
    పంక్తి = ml_get(lnum);
    కొరకు (p = పంక్తి; *p != NUL; ++p)
	ఒకవేళ (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    ఒకవేళ (VIM_ISDIGIT(p[len]))
		++len;
	    ఒకవేళ (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)cms, "%s");
		ఒకవేళ (cms2 != ఖాళీ && p - పంక్తి >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (పూర్ణాంకం)STRLEN(cms) - 2;
		}
	    }
	    ఒకవేళ (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		ఒకవేళ (newline != ఖాళీ)
		{
		    STRNCPY(newline, పంక్తి, p - పంక్తి);
		    STRCPY(newline + (p - పంక్తి), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    ఆపు;
	}
}

// get_foldtext() {{{2
/*
 * Return the text for a closed fold at line "lnum", with last line "lnume".
 * When 'foldtext' isn't set puts the result in "buf[FOLD_TEXT_LEN]".
 * Otherwise the result is in allocated memory.
 */
    char_u *
get_foldtext(
    win_T	*wp,
    linenr_T	lnum,
    linenr_T	lnume,
    foldinfo_T	*foldinfo,
    char_u	*buf)
{
    char_u	*పాఠ్యం = ఖాళీ;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    స్థిర పూర్ణాంకం	    got_fdt_error = FALSE;
    పూర్ణాంకం		    save_did_emsg = did_emsg;
    స్థిర win_T    *చివరి_wp = ఖాళీ;
    స్థిర linenr_T చివరి_lnum = 0;

    ఒకవేళ (చివరి_wp != wp || చివరి_wp == ఖాళీ
					|| చివరి_lnum > lnum || చివరి_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    ఒకవేళ (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    ఒకవేళ (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	పూర్ణాంకం	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	సమితి_vim_var_nr(VV_FOLDSTART, lnum);
	సమితి_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	ఒకవేళ (level > (పూర్ణాంకం)పరిమాణం(dashes) - 1)
	    level = (పూర్ణాంకం)పరిమాణం(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	సమితి_vim_var_string(VV_FOLDDASHES, dashes, level);
	సమితి_vim_var_nr(VV_FOLDLEVEL, (దీర్ఘ)level);

	// skip evaluating 'foldtext' on errors
	ఒకవేళ (!got_fdt_error)
	{
	    win_T   *save_curwin = curwin;
	    sctx_T  saved_sctx = current_sctx;

	    curwin = wp;
	    curbuf = wp->w_బఫరు;
	    current_sctx = wp->w_p_script_ctx[WV_FDT];

	    ++emsg_off; // handle exceptions, but don't display errors
	    పాఠ్యం = eval_గమ్యం_string_సురక్షిత(wp->w_p_fdt,
		       was_సమితి_insecurely(wp, (char_u *)"foldtext", OPT_LOCAL),
		       TRUE, TRUE);
	    --emsg_off;

	    ఒకవేళ (పాఠ్యం == ఖాళీ || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_బఫరు;
	    current_sctx = saved_sctx;
	}
	చివరి_lnum = lnum;
	చివరి_wp   = wp;
	సమితి_vim_var_string(VV_FOLDDASHES, ఖాళీ, -1);

	ఒకవేళ (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	ఒకవేళ (పాఠ్యం != ఖాళీ)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    కొరకు (p = పాఠ్యం; *p != NUL; ++p)
	    {
		పూర్ణాంకం	len;

		ఒకవేళ (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    ఒకవేళ (!vim_isprintc((*mb_ptr2char)(p)))
			ఆపు;
		    p += len - 1;
		}
		లేకపోతే
		    ఒకవేళ (*p == TAB)
			*p = ' ';
		    లేకపోతే ఒకవేళ (ptr2cells(p) > 1)
			ఆపు;
	    }
	    ఒకవేళ (*p != NUL)
	    {
		p = transstr(పాఠ్యం);
		vim_free(పాఠ్యం);
		పాఠ్యం = p;
	    }
	}
    }
    ఒకవేళ (పాఠ్యం == ఖాళీ)
# endif
    {
	దీర్ఘ లెక్క = (దీర్ఘ)(lnume - lnum + 1);

	vim_snprintf((అక్షరం *)buf, FOLD_పాఠ్యం_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", లెక్క),
		     లెక్క);
	పాఠ్యం = buf;
    }
    తిరిగి పాఠ్యం;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    స్థిర శూన్య
foldtext_cleanup(char_u *str)
{
    char_u	*cms_ప్రారంభం;	// first part or the whole comment
    పూర్ణాంకం		cms_slen = 0;	// length of cms_start
    char_u	*cms_చివర;	// last part of the comment or NULL
    పూర్ణాంకం		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    పూర్ణాంకం		len;
    పూర్ణాంకం		did1 = FALSE;
    పూర్ణాంకం		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_ప్రారంభం = skipwhite(curbuf->b_p_cms);
    cms_slen = (పూర్ణాంకం)STRLEN(cms_ప్రారంభం);
    అయితే (cms_slen > 0 && VIM_ISWHITE(cms_ప్రారంభం[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_చివర = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)cms_ప్రారంభం, "%s");
    ఒకవేళ (cms_చివర != ఖాళీ)
    {
	cms_elen = cms_slen - (పూర్ణాంకం)(cms_చివర - cms_ప్రారంభం);
	cms_slen = (పూర్ణాంకం)(cms_చివర - cms_ప్రారంభం);

	// exclude white space before "%s"
	అయితే (cms_slen > 0 && VIM_ISWHITE(cms_ప్రారంభం[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_చివర + 2);
	cms_elen -= (పూర్ణాంకం)(s - cms_చివర);
	cms_చివర = s;
    }
    parseMarker(curwin);

    కొరకు (s = str; *s != NUL; )
    {
	len = 0;
	ఒకవేళ (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	లేకపోతే ఒకవేళ (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	ఒకవేళ (len > 0)
	{
	    ఒకవేళ (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    కొరకు (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    ఒకవేళ (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_ప్రారంభం, cms_slen) == 0)
	    {
		len += (పూర్ణాంకం)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	లేకపోతే ఒకవేళ (cms_చివర != ఖాళీ)
	{
	    ఒకవేళ (!did1 && cms_slen > 0 && STRNCMP(s, cms_ప్రారంభం, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    లేకపోతే ఒకవేళ (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_చివర, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	ఒకవేళ (len != 0)
	{
	    అయితే (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	లేకపోతే
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
రకనిర్వచనం నిర్మాణం
{
    win_T	*wp;		//కిటికీ
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    పూర్ణాంకం		lvl;		// current level (-1 for undefined)
    పూర్ణాంకం		lvl_తదుపరి;	// level used for next line
    పూర్ణాంకం		ప్రారంభం;		// number of folds that are forced to start at
				// this line.
    పూర్ణాంకం		చివర;		// level of fold that is forced to end below
				// this line
    పూర్ణాంకం		had_చివర;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
స్థిర పూర్ణాంకం fold_మారింది;

// Function declarations. {{{2
స్థిర linenr_T foldUpdateIEMSRecurse(garray_T *gap, పూర్ణాంకం level, linenr_T startlnum, fline_T *flp, శూన్య (*getlevel)(fline_T *), linenr_T bot, పూర్ణాంకం topflags);
స్థిర పూర్ణాంకం fold_చొప్పించు(garray_T *gap, పూర్ణాంకం i);
స్థిర శూన్య fold_విభజించు(garray_T *gap, పూర్ణాంకం i, linenr_T శిఖరం, linenr_T bot);
స్థిర శూన్య foldRemove(garray_T *gap, linenr_T శిఖరం, linenr_T bot);
స్థిర శూన్య fold_విలీనం(fold_T *fp1, garray_T *gap, fold_T *fp2);
స్థిర శూన్య foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
స్థిర శూన్య foldlevelDiff(fline_T *flp);
# endif
స్థిర శూన్య foldlevel_సమాసం(fline_T *flp);
స్థిర శూన్య foldlevelMarker(fline_T *flp);
స్థిర శూన్య foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    స్థిర శూన్య
foldUpdateIEMS(win_T *wp, linenr_T శిఖరం, linenr_T bot)
{
    linenr_T	ప్రారంభం;
    linenr_T	చివర;
    fline_T	fline;
    శూన్య	(*getlevel)(fline_T *);
    పూర్ణాంకం		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    ఒకవేళ (invalid_శిఖరం != (linenr_T)0)
	తిరిగి;

    ఒకవేళ (wp->w_foldinvalid)
    {
	// Need to update all folds.
	శిఖరం = 1;
	bot = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	సమితి_Small_Maybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    ఒకవేళ (foldmethodIsDiff(wp))
    {
	ఒకవేళ (శిఖరం > diff_context)
	    శిఖరం -= diff_context;
	లేకపోతే
	    శిఖరం = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    ఒకవేళ (శిఖరం > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	శిఖరం = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;

    fold_మారింది = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_తదుపరి = -1;
    fline.ప్రారంభం = 0;
    fline.చివర = MAX_LEVEL + 1;
    fline.had_చివర = MAX_LEVEL + 1;

    invalid_శిఖరం = శిఖరం;
    invalid_bot = bot;

    ఒకవేళ (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	ఒకవేళ (శిఖరం > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, శిఖరం - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = శిఖరం - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    ఒకవేళ (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_తదుపరి);
	    లేకపోతే
		fline.lvl = fline.lvl_తదుపరి;
	}
	fline.lnum = శిఖరం;
	getlevel(&fline);
    }
    లేకపోతే
    {
	fline.lnum = శిఖరం;
	ఒకవేళ (foldmethod_Is_సమాసం(wp))
	{
	    getlevel = foldlevel_సమాసం;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    ఒకవేళ (శిఖరం > 1)
		--fline.lnum;
	}
	లేకపోతే ఒకవేళ (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	లేకపోతే ఒకవేళ (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	లేకపోతే
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    ఒకవేళ (శిఖరం > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	కొరకు ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_తదుపరి = -1;
	    getlevel(&fline);
	    ఒకవేళ (fline.lvl >= 0)
		ఆపు;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    ఒకవేళ (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ఖాళీ;
	పూర్ణాంకం	  current_fdl = 0;
	linenr_T  fold_ప్రారంభం_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	అయితే (current_fdl < fline.lvl)
	{
	    ఒకవేళ (!fold_కనుగొను(gap, lnum_rel, &fpn))
		ఆపు;
	    ++current_fdl;

	    fold_ప్రారంభం_lnum += fpn->fd_శిఖరం;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_శిఖరం;
	}
	ఒకవేళ (fpn != ఖాళీ && current_fdl == fline.lvl)
	{
	    linenr_T fold_చివర_lnum = fold_ప్రారంభం_lnum + fpn->fd_len;

	    ఒకవేళ (fold_చివర_lnum > bot)
		bot = fold_చివర_lnum;
	}
    }

    ప్రారంభం = fline.lnum;
    చివర = bot;
    // Do at least one line.
    ఒకవేళ (ప్రారంభం > చివర && చివర < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	చివర = ప్రారంభం;
    అయితే (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	ఒకవేళ (fline.lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;
	ఒకవేళ (fline.lnum > చివర)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    ఒకవేళ (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_సమాసం)
		ఆపు;
	    ఒకవేళ ((ప్రారంభం <= చివర
			&& fold_కనుగొను(&wp->w_folds, చివర, &fp)
			&& fp->fd_శిఖరం + fp->fd_len - 1 > చివర)
		    || (fline.lvl == 0
			&& fold_కనుగొను(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_శిఖరం < fline.lnum))
		చివర = fp->fd_శిఖరం + fp->fd_len - 1;
	    లేకపోతే ఒకవేళ (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		చివర = fline.lnum;
	    లేకపోతే
		ఆపు;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	ఒకవేళ (fline.lvl > 0)
	{
	    invalid_శిఖరం = fline.lnum;
	    invalid_bot = చివర;
	    చివర = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, ప్రారంభం, &fline, getlevel, చివర, FD_LEVEL);
	    ప్రారంభం = fline.lnum;
	}
	లేకపోతే
	{
	    ఒకవేళ (fline.lnum == wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
		ఆపు;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_తదుపరి;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, ప్రారంభం, చివర);

    // If some fold changed, need to redraw and position cursor.
    ఒకవేళ (fold_మారింది && wp->w_p_fen)
	మారింది_కిటికీ_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    ఒకవేళ (చివర != bot)
	redraw_win_range_later(wp, శిఖరం, చివర);

    invalid_శిఖరం = (linenr_T)0;
}

// foldUpdateIEMSRecurse() {{{2
/*
 * Update a fold that starts at "flp->lnum".  At this line there is always a
 * valid foldlevel, and its level >= "level".
 * "flp" is valid for "flp->lnum" when called and it's valid when returning.
 * "flp->lnum" is set to the lnum just below the fold, if it ends before
 * "bot", it's "bot" plus one if the fold continues and it's bigger when using
 * the marker method and a text change made following folds to change.
 * When returning, "flp->lnum_save" is the line number that was used to get
 * the level when the level at "flp->lnum" is invalid.
 * Remove any folds from "startlnum" up to here at this level.
 * Recursively update nested folds.
 * Below line "bot" there are no changes in the text.
 * "flp->lnum", "flp->lnum_save" and "bot" are relative to the start of the
 * outer fold.
 * "flp->off" is the offset to the real line number in the buffer.
 *
 * All this would be a lot simpler if all folds in the range would be deleted
 * and then created again.  But we would lose all information about the
 * folds, even when making changes that don't affect the folding (e.g. "vj~").
 *
 * Returns bot, which may have been increased for lines that also need to be
 * updated as a result of a detected change in the fold.
 */
    స్థిర linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    పూర్ణాంకం		level,
    linenr_T	startlnum,
    fline_T	*flp,
    శూన్య	(*getlevel)(fline_T *),
    linenr_T	bot,
    పూర్ణాంకం		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ఖాళీ;
    fold_T	*fp2;
    పూర్ణాంకం		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    పూర్ణాంకం		i;
    పూర్ణాంకం		ముగింపు = FALSE;
    linenr_T	linecount = flp->wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - flp->off;
    పూర్ణాంకం		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    ఒకవేళ (getlevel == foldlevelMarker && flp->ప్రారంభం <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(శూన్య)fold_కనుగొను(gap, startlnum - 1, &fp);
	ఒకవేళ (fp != ఖాళీ && (fp >= ((fold_T *)gap->ga_దత్తాంశం) + gap->ga_len
						   || fp->fd_శిఖరం >= startlnum))
	    fp = ఖాళీ;
    }

    /*
     * Loop over all lines in this fold, or until "bot" is hit.
     * Handle nested folds inside of this fold.
     * "flp->lnum" is the current line.  When finding the end of the fold, it
     * is just below the end of the fold.
     * "*flp" contains the level of the line "flp->lnum" or a following one if
     * there are lines with an invalid fold level.  "flp->lnum_save" is the
     * line number that was used to get the fold level (below "flp->lnum" when
     * it has an invalid fold level).  When called the fold level is always
     * valid, thus "flp->lnum_save" is equal to "flp->lnum".
     */
    flp->lnum_save = flp->lnum;
    అయితే (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	పంక్తి_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	ఒకవేళ (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	ఒకవేళ (flp->lnum > firstlnum
		&& (level > lvl - flp->ప్రారంభం || level >= flp->had_చివర))
	    lvl = 0;

	ఒకవేళ (flp->lnum > bot && !ముగింపు && fp != ఖాళీ)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    ఒకవేళ (getlevel != foldlevelMarker
		    && getlevel != foldlevel_సమాసం
		    && getlevel != foldlevelSyntax)
		ఆపు;
	    i = 0;
	    fp2 = fp;
	    ఒకవేళ (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_శిఖరం;
		అయితే (fold_కనుగొను(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_శిఖరం;
		}
	    }
	    ఒకవేళ (lvl < level + i)
	    {
		(శూన్య)fold_కనుగొను(&fp->fd_nested, flp->lnum - fp->fd_శిఖరం, &fp2);
		ఒకవేళ (fp2 != ఖాళీ)
		    bot = fp2->fd_శిఖరం + fp2->fd_len - 1 + fp->fd_శిఖరం;
	    }
	    లేకపోతే ఒకవేళ (fp->fd_శిఖరం + fp->fd_len <= flp->lnum && lvl >= level)
		ముగింపు = TRUE;
	    లేకపోతే
		ఆపు;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	ఒకవేళ (fp == ఖాళీ
		&& (lvl != level
		    || flp->lnum_save >= bot
		    || flp->ప్రారంభం != 0
		    || flp->had_చివర <= MAX_LEVEL
		    || flp->lnum == linecount))
	{
	    /*
	     * Remove or update folds that have lines between startlnum and
	     * firstlnum.
	     */
	    అయితే (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		ఒకవేళ (flp->ప్రారంభం != 0 || flp->had_చివర <= MAX_LEVEL)
		    concat = 0;
		లేకపోతే
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		ఒకవేళ (gap->ga_len > 0 && (fold_కనుగొను(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_దత్తాంశం) + gap->ga_len
			    && fp->fd_శిఖరం <= firstlnum)
			|| fold_కనుగొను(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_దత్తాంశం) + gap->ga_len
			    && ((lvl < level && fp->fd_శిఖరం < flp->lnum)
				|| (lvl >= level
					   && fp->fd_శిఖరం <= flp->lnum_save)))))
		{
		    ఒకవేళ (fp->fd_శిఖరం + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			ఒకవేళ (fp->fd_శిఖరం == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			లేకపోతే ఒకవేళ (fp->fd_శిఖరం >= startlnum)
			{
			    ఒకవేళ (fp->fd_శిఖరం > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(దీర్ఘ)(fp->fd_శిఖరం - firstlnum), 0L);
			    లేకపోతే
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(దీర్ఘ)(firstlnum - fp->fd_శిఖరం - 1),
					(linenr_T)MAXLNUM,
					(దీర్ఘ)(fp->fd_శిఖరం - firstlnum));
			    fp->fd_len += fp->fd_శిఖరం - firstlnum;
			    fp->fd_శిఖరం = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_మారింది = TRUE;
			}
			లేకపోతే ఒకవేళ ((flp->ప్రారంభం != 0 && lvl == level)
						     || firstlnum != startlnum)
			{
			    linenr_T breakstart;
			    linenr_T breakend;

			    /*
			     * Before there was a fold spanning from above
			     * startlnum to below firstlnum. This fold is valid
			     * above startlnum (because we are not updating
			     * that range), but there should now be a break in
			     * it.
			     * If the break is because we are now forced to
			     * start a new fold at the level "level" at line
			     * fline->lnum, then we need to split the fold at
			     * fline->lnum.
			     * If the break is because the range
			     * [startlnum, firstlnum) is now at a lower indent
			     * than "level", we need to split the fold in this
			     * range.
			     * Any splits have to be done recursively.
			     */
			    ఒకవేళ (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    లేకపోతే
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_శిఖరం,
						      breakend - fp->fd_శిఖరం);
			    i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం);
			    fold_విభజించు(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_దత్తాంశం + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    ఒకవేళ (getlevel == foldlevelMarker
				    || getlevel == foldlevel_సమాసం
				    || getlevel == foldlevelSyntax)
				ముగింపు = TRUE;
			}

			ఒకవేళ (fp->fd_శిఖరం == startlnum && concat)
			{
			    i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం);
			    ఒకవేళ (i != 0)
			    {
				fp2 = fp - 1;
				ఒకవేళ (fp2->fd_శిఖరం + fp2->fd_len == fp->fd_శిఖరం)
				{
				    fold_విలీనం(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			ఆపు;
		    }
		    ఒకవేళ (fp->fd_శిఖరం >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			delete_Fold_నమోదు(gap,
				     (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం), TRUE);
		    }
		    లేకపోతే
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_శిఖరం;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_మారింది = TRUE;
		    }
		}
		లేకపోతే
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    ఒకవేళ (gap->ga_len == 0)
			i = 0;
		    లేకపోతే
			i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం);
		    ఒకవేళ (fold_చొప్పించు(gap, i) != OK)
			తిరిగి bot;
		    fp = (fold_T *)gap->ga_దత్తాంశం + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_శిఖరం = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    ఒకవేళ (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    లేకపోతే ఒకవేళ (i <= 0)
		    {
			fp->fd_flags = topflags;
			ఒకవేళ (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    లేకపోతే
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    ఒకవేళ (getlevel == foldlevelMarker
			    || getlevel == foldlevel_సమాసం
			    || getlevel == foldlevelSyntax)
			ముగింపు = TRUE;
		    fold_మారింది = TRUE;
		    ఆపు;
		}
	    }
	}

	ఒకవేళ (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    ఆపు;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	ఒకవేళ (lvl > level && fp != ఖాళీ)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    ఒకవేళ (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_శిఖరం;
	    flp->off += fp->fd_శిఖరం;
	    i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_శిఖరం, flp, getlevel,
					      bot - fp->fd_శిఖరం, fp->fd_flags);
	    fp = (fold_T *)gap->ga_దత్తాంశం + i;
	    flp->lnum += fp->fd_శిఖరం;
	    flp->lnum_save += fp->fd_శిఖరం;
	    flp->off -= fp->fd_శిఖరం;
	    bot += fp->fd_శిఖరం;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	లేకపోతే
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    అయితే (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		ఒకవేళ (++flp->lnum > linecount)
		    ఆపు;
		flp->lvl = flp->lvl_తదుపరి;
		getlevel(flp);
		ఒకవేళ (flp->lvl >= 0 || flp->had_చివర <= MAX_LEVEL)
		    ఆపు;
	    }
	    prev_lnum = 0;
	    ఒకవేళ (flp->lnum > linecount)
		ఆపు;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    ఒకవేళ (fp == ఖాళీ)	// only happens when got_int is set
	తిరిగి bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    ఒకవేళ (fp->fd_len < flp->lnum - fp->fd_శిఖరం)
    {
	fp->fd_len = flp->lnum - fp->fd_శిఖరం;
	fp->fd_small = MAYBE;
	fold_మారింది = TRUE;
    }
    లేకపోతే ఒకవేళ (fp->fd_శిఖరం + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_శిఖరం + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_శిఖరం,
						  flp->lnum - 1 - fp->fd_శిఖరం);

    ఒకవేళ (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	ఒకవేళ (fp->fd_len != flp->lnum - fp->fd_శిఖరం)
	{
	    ఒకవేళ (fp->fd_శిఖరం + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		ఒకవేళ (getlevel == foldlevelMarker
			|| getlevel == foldlevel_సమాసం
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_శిఖరం + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_శిఖరం;
		}
		లేకపోతే
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం);
		    fold_విభజించు(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_దత్తాంశం + i;
		}
	    }
	    లేకపోతే
		fp->fd_len = flp->lnum - fp->fd_శిఖరం;
	    fold_మారింది = TRUE;
	}
    }

    // delete following folds that end before the current line
    కొరకు (;;)
    {
	fp2 = fp + 1;
	ఒకవేళ (fp2 >= (fold_T *)gap->ga_దత్తాంశం + gap->ga_len
						  || fp2->fd_శిఖరం > flp->lnum)
	    ఆపు;
	ఒకవేళ (fp2->fd_శిఖరం + fp2->fd_len > flp->lnum)
	{
	    ఒకవేళ (fp2->fd_శిఖరం < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (దీర్ఘ)(flp->lnum - fp2->fd_శిఖరం - 1),
			(linenr_T)MAXLNUM, (దీర్ఘ)(fp2->fd_శిఖరం - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_శిఖరం;
		fp2->fd_శిఖరం = flp->lnum;
		fold_మారింది = TRUE;
	    }

	    ఒకవేళ (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		fold_విలీనం(fp, gap, fp2);
	    }
	    ఆపు;
	}
	fold_మారింది = TRUE;
	delete_Fold_నమోదు(gap, (పూర్ణాంకం)(fp2 - (fold_T *)gap->ga_దత్తాంశం), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    ఒకవేళ (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    తిరిగి bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    స్థిర పూర్ణాంకం
fold_చొప్పించు(garray_T *gap, పూర్ణాంకం i)
{
    fold_T	*fp;

    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి FAIL;
    fp = (fold_T *)gap->ga_దత్తాంశం + i;
    ఒకవేళ (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, పరిమాణం(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, పరిమాణం(fold_T), 10);
    తిరిగి OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    స్థిర శూన్య
fold_విభజించు(
    garray_T	*gap,
    పూర్ణాంకం		i,
    linenr_T	శిఖరం,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		len;

    // The fold continues below bot, need to split it.
    ఒకవేళ (fold_చొప్పించు(gap, i + 1) == FAIL)
	తిరిగి;
    fp = (fold_T *)gap->ga_దత్తాంశం + i;
    fp[1].fd_శిఖరం = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_శిఖరం - fp->fd_శిఖరం);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (శూన్య)fold_కనుగొను(gap1, bot + 1 - fp->fd_శిఖరం, &fp2);
    ఒకవేళ (fp2 != ఖాళీ)
    {
	len = (పూర్ణాంకం)((fold_T *)gap1->ga_దత్తాంశం + gap1->ga_len - fp2);
	ఒకవేళ (len > 0 && ga_grow(gap2, len) == OK)
	{
	    కొరకు (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_దత్తాంశం)[idx] = fp2[idx];
		((fold_T *)gap2->ga_దత్తాంశం)[idx].fd_శిఖరం
						  -= fp[1].fd_శిఖరం - fp->fd_శిఖరం;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = శిఖరం - fp->fd_శిఖరం;
    fold_మారింది = TRUE;
}

// foldRemove() {{{2
/*
 * Remove folds within the range "top" to and including "bot".
 * Check for these situations:
 *      1  2  3
 *      1  2  3
 * top     2  3  4  5
 *	   2  3  4  5
 * bot	   2  3  4  5
 *	      3     5  6
 *	      3     5  6
 *
 * 1: not changed
 * 2: truncate to stop above "top"
 * 3: split in two parts, one stops above "top", other starts below "bot".
 * 4: deleted
 * 5: made to start below "bot".
 * 6: not changed
 */
    స్థిర శూన్య
foldRemove(garray_T *gap, linenr_T శిఖరం, linenr_T bot)
{
    fold_T	*fp = ఖాళీ;

    ఒకవేళ (bot < శిఖరం)
	తిరిగి;		// nothing to do

    అయితే (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	ఒకవేళ (fold_కనుగొను(gap, శిఖరం, &fp) && fp->fd_శిఖరం < శిఖరం)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, శిఖరం - fp->fd_శిఖరం, bot - fp->fd_శిఖరం);
	    ఒకవేళ (fp->fd_శిఖరం + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		fold_విభజించు(gap, (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం), శిఖరం, bot);
	    }
	    లేకపోతే
	    {
		// 2: truncate fold at "top".
		fp->fd_len = శిఖరం - fp->fd_శిఖరం;
	    }
	    fold_మారింది = TRUE;
	    కొనసాగు;
	}
	ఒకవేళ (fp >= (fold_T *)(gap->ga_దత్తాంశం) + gap->ga_len
		|| fp->fd_శిఖరం > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    ఆపు;
	}
	ఒకవేళ (fp->fd_శిఖరం >= శిఖరం)
	{
	    // Found an entry below top.
	    fold_మారింది = TRUE;
	    ఒకవేళ (fp->fd_శిఖరం + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (దీర్ఘ)(bot - fp->fd_శిఖరం),
			(linenr_T)MAXLNUM, (దీర్ఘ)(fp->fd_శిఖరం - bot - 1));
		fp->fd_len -= bot - fp->fd_శిఖరం + 1;
		fp->fd_శిఖరం = bot + 1;
		ఆపు;
	    }

	    // 4: Delete completely contained fold.
	    delete_Fold_నమోదు(gap, (పూర్ణాంకం)(fp - (fold_T *)gap->ga_దత్తాంశం), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    స్థిర శూన్య
fold_తిరగేయి_క్రమం(garray_T *gap, linenr_T ప్రారంభం_arg, linenr_T చివర_arg)
{
    fold_T *ఎడమ_2, *కుడి_2;
    fold_T tmp;
    linenr_T ప్రారంభం = ప్రారంభం_arg;
    linenr_T చివర = చివర_arg;

    కొరకు (; ప్రారంభం < చివర; ప్రారంభం++, చివర--)
    {
	ఎడమ_2 = (fold_T *)gap->ga_దత్తాంశం + ప్రారంభం;
	కుడి_2 = (fold_T *)gap->ga_దత్తాంశం + చివర;
	tmp  = *ఎడమ_2;
	*ఎడమ_2 = *కుడి_2;
	*కుడి_2 = tmp;
    }
}

// foldMoveRange() {{{2
/*
 * Move folds within the inclusive range "line1" to "line2" to after "dest"
 * requires "line1" <= "line2" <= "dest"
 *
 * There are the following situations for the first fold at or below line1 - 1.
 *       1  2  3  4
 *       1  2  3  4
 * line1    2  3  4
 *          2  3  4  5  6  7
 * line2       3  4  5  6  7
 *             3  4     6  7  8  9
 * dest           4        7  8  9
 *                4        7  8    10
 *                4        7  8    10
 *
 * In the following descriptions, "moved" means moving in the buffer, *and* in
 * the fold array.
 * Meanwhile, "shifted" just means moving in the buffer.
 * 1. not changed
 * 2. truncated above line1
 * 3. length reduced by  line2 - line1, folds starting between the end of 3 and
 *    dest are truncated and shifted up
 * 4. internal folds moved (from [line1, line2] to dest)
 * 5. moved to dest.
 * 6. truncated below line2 and moved.
 * 7. length reduced by line2 - dest, folds starting between line2 and dest are
 *    removed, top is moved down by move_len.
 * 8. truncated below dest and shifted up.
 * 9. shifted up
 * 10. not changed
 */

    స్థిర శూన్య
truncate_fold(fold_T *fp, linenr_T చివర)
{
    చివర += 1;
    foldRemove(&fp->fd_nested, చివర - fp->fd_శిఖరం, MAXLNUM);
    fp->fd_len = చివర - fp->fd_శిఖరం;
}

# define fold_చివర(fp) ((fp)->fd_శిఖరం + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_దత్తాంశం + (gap)->ga_len))
# define fold_సూచీ(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_దత్తాంశం)))

    శూన్య
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    పూర్ణాంకం at_ప్రారంభం = fold_కనుగొను(gap, line1 - 1, &fp);
    size_t move_ప్రారంభం = 0, move_చివర = 0, dest_సూచీ = 0;

    ఒకవేళ (at_ప్రారంభం)
    {
	ఒకవేళ (fold_చివర(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_శిఖరం, line2 -
		    fp->fd_శిఖరం, dest - fp->fd_శిఖరం);
	    తిరిగి;
	}
	లేకపోతే ఒకవేళ (fold_చివర(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_శిఖరం, line2 -
		    fp->fd_శిఖరం, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	లేకపోతే
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    ఒకవేళ (!valid_fold(fp, gap) || fp->fd_శిఖరం > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (fp->fd_శిఖరం > line2)
    {
	కొరకు (; valid_fold(fp, gap) && fold_చివర(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_శిఖరం -= range_len;

	ఒకవేళ (valid_fold(fp, gap) && fp->fd_శిఖరం <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_శిఖరం -= range_len;
	}
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (fold_చివర(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_శిఖరం, dest -
		fp->fd_శిఖరం, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_శిఖరం += move_len;
	తిరిగి;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_ప్రారంభం = fold_సూచీ(fp, gap);

    కొరకు (; valid_fold(fp, gap) && fp->fd_శిఖరం <= dest; fp++)
    {
	ఒకవేళ (fp->fd_శిఖరం <= line2)
	{
	    // 1. 2. or 3.
	    ఒకవేళ (fold_చివర(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_శిఖరం += move_len;
	    కొనసాగు;
	}

	// Record index of the first fold after the moved range.
	ఒకవేళ (move_చివర == 0)
	    move_చివర = fold_సూచీ(fp, gap);

	ఒకవేళ (fold_చివర(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_శిఖరం -= range_len;
    }

    dest_సూచీ = fold_సూచీ(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    ఒకవేళ (move_చివర == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	తిరిగి;
    fold_తిరగేయి_క్రమం(gap, (linenr_T)move_ప్రారంభం, (linenr_T)dest_సూచీ - 1);
    fold_తిరగేయి_క్రమం(gap, (linenr_T)move_ప్రారంభం,
			   (linenr_T)(move_ప్రారంభం + dest_సూచీ - move_చివర - 1));
    fold_తిరగేయి_క్రమం(gap, (linenr_T)(move_ప్రారంభం + dest_సూచీ - move_చివర),
						   (linenr_T)(dest_సూచీ - 1));
}
# undef fold_చివర
# undef valid_fold
# undef fold_సూచీ

// foldMerge() {{{2
/*
 * Merge two adjacent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjacent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    స్థిర శూన్య
fold_విలీనం(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    పూర్ణాంకం		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    ఒకవేళ (fold_కనుగొను(gap1, fp1->fd_len - 1L, &fp3) && fold_కనుగొను(gap2, 0L, &fp4))
	fold_విలీనం(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    ఒకవేళ (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	కొరకు (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_దత్తాంశం)[gap1->ga_len]
					= ((fold_T *)gap2->ga_దత్తాంశం)[idx];
	    ((fold_T *)gap1->ga_దత్తాంశం)[gap1->ga_len].fd_శిఖరం += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    delete_Fold_నమోదు(gap, (పూర్ణాంకం)(fp2 - (fold_T *)gap->ga_దత్తాంశం), TRUE);
    fold_మారింది = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    స్థిర శూన్య
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_బఫరు;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    ఒకవేళ (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ఖాళీ)
    {
	// first and last line can't be undefined, use level 0
	ఒకవేళ (lnum == 1 || lnum == buf->b_ml.ml_పంక్తి_లెక్క)
	    flp->lvl = 0;
	లేకపోతే
	    flp->lvl = -1;
    }
    లేకపోతే
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_విలువ(buf);
    ఒకవేళ (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	ఒకవేళ (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    స్థిర శూన్య
foldlevelDiff(fline_T *flp)
{
    ఒకవేళ (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    లేకపోతే
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    స్థిర శూన్య
foldlevel_సమాసం(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->ప్రారంభం = FALSE;
    flp->lvl = 0;
# లేకపోతే
    win_T	*win;
    పూర్ణాంకం		n;
    పూర్ణాంకం		c;
    linenr_T	lnum = flp->lnum + flp->off;
    పూర్ణాంకం		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_బఫరు;
    సమితి_vim_var_nr(VV_LNUM, lnum);

    flp->ప్రారంభం = 0;
    flp->had_చివర = flp->చివర;
    flp->చివర = MAX_LEVEL + 1;
    ఒకవేళ (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = కీలకం_Typed;
    n = eval_foldexpr(flp->wp, &c);
    కీలకం_Typed = save_keytyped;

    ఎంపిక (c)
    {
	// "a1", "a2", .. : add to the fold level
	స్థితి 'a': ఒకవేళ (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_తదుపరి = flp->lvl;
		  }
		  flp->ప్రారంభం = n;
		  ఆపు;

	// "s1", "s2", .. : subtract from the fold level
	స్థితి 's': ఒకవేళ (flp->lvl >= 0)
		  {
		      ఒకవేళ (n > flp->lvl)
			  flp->lvl_తదుపరి = 0;
		      లేకపోతే
			  flp->lvl_తదుపరి = flp->lvl - n;
		      flp->చివర = flp->lvl_తదుపరి + 1;
		  }
		  ఆపు;

	// ">1", ">2", .. : start a fold with a certain level
	స్థితి '>': flp->lvl = n;
		  flp->lvl_తదుపరి = n;
		  flp->ప్రారంభం = 1;
		  ఆపు;

	// "<1", "<2", .. : end a fold with a certain level
	స్థితి '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_తదుపరి = MIN(flp->lvl, n - 1);
		  flp->చివర = n;
		  ఆపు;

	// "=": No change in level
	స్థితి '=': flp->lvl_తదుపరి = flp->lvl;
		  ఆపు;

	// "-1", "0", "1", ..: set fold level
	అప్రమేయ:  ఒకవేళ (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_తదుపరి = flp->lvl;
		  లేకపోతే
		      flp->lvl_తదుపరి = n;
		  flp->lvl = n;
		  ఆపు;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    ఒకవేళ (flp->lvl < 0)
    {
	ఒకవేళ (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_తదుపరి = 0;
	}
	ఒకవేళ (lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
	    flp->lvl_తదుపరి = 0;
    }

    curwin = win;
    curbuf = curwin->w_బఫరు;
# endif
}

// parseMarker() {{{2
/*
 * Parse 'foldmarker' and set "foldendmarker", "foldstartmarkerlen" and
 * "foldendmarkerlen".
 * Relies on the option value to have been checked for correctness already.
 */
    స్థిర శూన్య
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (పూర్ణాంకం)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (పూర్ణాంకం)STRLEN(foldendmarker);
}

// foldlevelMarker() {{{2
/*
 * Low level function to get the foldlevel for the "marker" method.
 * "foldendmarker", "foldstartmarkerlen" and "foldendmarkerlen" must have been
 * set before calling this.
 * Requires that flp->lvl is set to the fold level of the previous line!
 * Careful: This means you can't call this function twice on the same line.
 * Doesn't use any caching.
 * Sets flp->start when a start marker was found.
 */
    స్థిర శూన్య
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    పూర్ణాంకం		cstart;
    పూర్ణాంకం		cend;
    పూర్ణాంకం		ప్రారంభం_lvl = flp->lvl;
    char_u	*s;
    పూర్ణాంకం		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->ప్రారంభం = 0;
    flp->lvl_తదుపరి = flp->lvl;

    s = ml_get_buf(flp->wp->w_బఫరు, flp->lnum + flp->off, FALSE);
    అయితే (*s)
    {
	ఒకవేళ (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    ఒకవేళ (VIM_ISDIGIT(*s))
	    {
		n = atoi((అక్షరం *)s);
		ఒకవేళ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_తదుపరి = n;
		    ఒకవేళ (n <= ప్రారంభం_lvl)
			flp->ప్రారంభం = 1;
		    లేకపోతే
			flp->ప్రారంభం = n - ప్రారంభం_lvl;
		}
	    }
	    లేకపోతే
	    {
		++flp->lvl;
		++flp->lvl_తదుపరి;
		++flp->ప్రారంభం;
	    }
	}
	లేకపోతే ఒకవేళ (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    ఒకవేళ (VIM_ISDIGIT(*s))
	    {
		n = atoi((అక్షరం *)s);
		ఒకవేళ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_తదుపరి = n - 1;
		    // never start a fold with an end marker
		    ఒకవేళ (flp->lvl_తదుపరి > ప్రారంభం_lvl)
			flp->lvl_తదుపరి = ప్రారంభం_lvl;
		}
	    }
	    లేకపోతే
		--flp->lvl_తదుపరి;
	}
	లేకపోతే
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    ఒకవేళ (flp->lvl_తదుపరి < 0)
	flp->lvl_తదుపరి = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    స్థిర శూన్య
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->ప్రారంభం = 0;
    flp->lvl = 0;
# లేకపోతే
    linenr_T	lnum = flp->lnum + flp->off;
    పూర్ణాంకం		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->ప్రారంభం = 0;
    ఒకవేళ (lnum < flp->wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	ఒకవేళ (n > flp->lvl)
	{
	    flp->ప్రారంభం = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# ఒకవేళ defined(FEAT_SESSION)
స్థిర పూర్ణాంకం put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
స్థిర పూర్ణాంకం put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
స్థిర పూర్ణాంకం put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    పూర్ణాంకం
put_folds(FILE *fd, win_T *wp)
{
    ఒకవేళ (foldmethodIsManual(wp))
    {
	ఒకవేళ (put_పంక్తి(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_పంక్తి(fd, "&fdl = &fdl") == FAIL)
	    తిరిగి FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    ఒకవేళ (wp->w_fold_manual)
	తిరిగి put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    తిరిగి OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    స్థిర పూర్ణాంకం
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    పూర్ణాంకం		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	ఒకవేళ (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_శిఖరం) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_శిఖరం + off,
					fp->fd_శిఖరం + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    తిరిగి FAIL;
	++fp;
    }
    తిరిగి OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    స్థిర పూర్ణాంకం
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; i++)
    {
	ఒకవేళ (fp->fd_flags != FD_LEVEL)
	{
	    ఒకవేళ (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		ఒకవేళ (fprintf(fd, ":%ld", fp->fd_శిఖరం + off) < 0
			|| put_eol(fd) == FAIL
			|| put_పంక్తి(fd, "sil! normal! zo") == FAIL)
		    తిరిగి FAIL;
		ఒకవేళ (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_శిఖరం)
			== FAIL)
		    తిరిగి FAIL;
		// close the parent when needed
		ఒకవేళ (fp->fd_flags == FD_CLOSED)
		{
		    ఒకవేళ (put_fold_open_close(fd, fp, off) == FAIL)
			తిరిగి FAIL;
		}
	    }
	    లేకపోతే
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_శిఖరం);
		ఒకవేళ ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		ఒకవేళ (put_fold_open_close(fd, fp, off) == FAIL)
		    తిరిగి FAIL;
	    }
	}
	++fp;
    }

    తిరిగి OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    స్థిర పూర్ణాంకం
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    ఒకవేళ (fprintf(fd, ":%ld", fp->fd_శిఖరం + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}
# endif // FEAT_SESSION

// }}}1
#అయితే_ముగింపు // defined(FEAT_FOLDING)

#ఒకవేళ defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    స్థిర శూన్య
foldclosed_both(
    typval_T	*argvars వాడని,
    typval_T	*rettv,
    పూర్ణాంకం		చివర వాడని)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	మొదటి, చివరి;
    win_T	*wp  = curwin;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_సంఖ్య(&argvars[1]), &tp);
	ఒకవేళ (wp != ఖాళీ && tp != ఖాళీ)
	{
	    ఒకవేళ (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	లేకపోతే
	{
	    rettv->vval.v_సంఖ్య = -2;
	    తిరిగి;
	}
    }
    లేకపోతే
	lnum = tv_get_lnum(argvars);

    ఒకవేళ (lnum >= 1 && lnum <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
    {
	ఒకవేళ (hasFoldingWin(wp, lnum, &మొదటి, &చివరి, FALSE, ఖాళీ))
	{
	    ఒకవేళ (చివర)
		rettv->vval.v_సంఖ్య = (varnumber_T)చివరి;
	    లేకపోతే
		rettv->vval.v_సంఖ్య = (varnumber_T)మొదటి;
	    తిరిగి;
	}
    }
# endif
    rettv->vval.v_సంఖ్య = -1;
}

/*
 * "foldclosed()" function
 */
    శూన్య
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    శూన్య
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    స్థిర పూర్ణాంకం
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    ఒకవేళ (invalid_శిఖరం == (linenr_T)0)
	checkupdate(curwin);
    లేకపోతే ఒకవేళ (lnum == prev_lnum && prev_lnum_lvl >= 0)
	తిరిగి prev_lnum_lvl;
    లేకపోతే ఒకవేళ (lnum >= invalid_శిఖరం && lnum <= invalid_bot)
	తిరిగి -1;

    // Return quickly when there is no folding at all in this window.
    ఒకవేళ (!hasAnyFolding(curwin))
	తిరిగి 0;

    తిరిగి foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    శూన్య
f_foldlevel(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_సంఖ్య(&argvars[1]), &tp);
	ఒకవేళ (wp != ఖాళీ && tp != ఖాళీ)
	{
	    ఒకవేళ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
		    rettv->vval.v_సంఖ్య = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	లేకపోతే
	    rettv->vval.v_సంఖ్య = -2;
    }
    లేకపోతే
    {
	lnum = tv_get_lnum(argvars);
	ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	    rettv->vval.v_సంఖ్య = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    శూన్య
f_foldtext(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    పూర్ణాంకం		len;
    అక్షరం	*txt;
    దీర్ఘ	లెక్క;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    ఒకవేళ (foldstart > 0 && foldend <= curbuf->b_ml.ml_పంక్తి_లెక్క
	    && dashes != ఖాళీ)
    {
	// Find first non-empty line in the fold.
	కొరకు (lnum = foldstart; lnum < foldend; ++lnum)
	    ఒకవేళ (!linewhite(lnum))
		ఆపు;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	ఒకవేళ (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    ఒకవేళ (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		ఒకవేళ (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	లెక్క = (దీర్ఘ)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", లెక్క);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	ఒకవేళ (r != ఖాళీ)
	{
	    sprintf((అక్షరం *)r, txt, dashes, లెక్క);
	    len = (పూర్ణాంకం)STRLEN(r);
	    STRCAT(r, s);
	    // remove 'foldmarker' and 'commentstring'
	    foldtext_cleanup(r + len);
	    rettv->vval.v_string = r;
	}
    }
# endif
}

/*
 * "foldtextresult(lnum)" function
 */
    శూన్య
f_foldtextresult(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*పాఠ్యం;
    char_u	buf[FOLD_పాఠ్యం_LEN];
    foldinfo_T  foldinfo;
    పూర్ణాంకం		fold_లెక్క;
    win_T	*wp  = curwin;
    స్థిర పూర్ణాంకం	entered = FALSE;
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (entered)
	తిరిగి; // reject recursive use
    entered = TRUE;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_సంఖ్య(&argvars[1]), &tp);
	ఒకవేళ (wp == ఖాళీ || tp == ఖాళీ) // wrong winid
	{
	    entered = FALSE;
	    తిరిగి;
	}
	ఒకవేళ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    లేకపోతే
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    ఒకవేళ (lnum < 0)
	lnum = 0;
    fold_లెక్క = folded_లెక్క(wp, lnum, &foldinfo);
    ఒకవేళ (fold_లెక్క > 0)
    {
	పాఠ్యం = get_foldtext(wp, lnum, lnum + fold_లెక్క - 1,
							       &foldinfo, buf);
	ఒకవేళ (పాఠ్యం == buf)
	    పాఠ్యం = vim_strsave(పాఠ్యం);
	rettv->vval.v_string = పాఠ్యం;
    }

    entered = FALSE;
# endif
}

#అయితే_ముగింపు // FEAT_EVAL
