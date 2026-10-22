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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	fd_ਸਿਖਰ;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    ਅੱਖਰ	fd_flags;	// see below
    ਅੱਖਰ	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
ਸਥਿਰ ਖਾਲੀ newFoldLevelWin(win_T *wp);
ਸਥਿਰ ਪੂਰਨ checkCloseRec(garray_T *gap, linenr_T lnum, ਪੂਰਨ level);
ਸਥਿਰ ਪੂਰਨ fold_ਲੱਭੋ(garray_T *gap, linenr_T lnum, fold_T **fpp);
ਸਥਿਰ ਪੂਰਨ foldLevelWin(win_T *wp, linenr_T lnum);
ਸਥਿਰ ਖਾਲੀ checkupdate(win_T *wp);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Fold_Repeat(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ do_open);
ਸਥਿਰ linenr_T ਸੈੱਟ_Manual_Fold(linenr_T lnum, ਪੂਰਨ opening, ਪੂਰਨ recurse, ਪੂਰਨ *donep);
ਸਥਿਰ linenr_T ਸੈੱਟ_Manual_Fold_Win(win_T *wp, linenr_T lnum, ਪੂਰਨ opening, ਪੂਰਨ recurse, ਪੂਰਨ *donep);
ਸਥਿਰ ਖਾਲੀ foldOpenNested(fold_T *fpr);
ਸਥਿਰ ਖਾਲੀ delete_Fold_ਇੰਦਰਾਜ(garray_T *gap, ਪੂਰਨ idx, ਪੂਰਨ recursive);
ਸਥਿਰ ਖਾਲੀ foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, ਲੰਮਾ amount, ਲੰਮਾ amount_after);
ਸਥਿਰ ਪੂਰਨ get_ਸਭ_ਤੋਂ_ਡੂੰਘਾ_Nesting_Recurse(garray_T *gap);
ਸਥਿਰ ਪੂਰਨ check_closed(win_T *win, fold_T *fp, ਪੂਰਨ *use_levelp, ਪੂਰਨ level, ਪੂਰਨ *maybe_smallp, linenr_T lnum_off);
ਸਥਿਰ ਖਾਲੀ checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Small_Maybe(garray_T *gap);
ਸਥਿਰ ਖਾਲੀ foldCreateMarkers(linenr_T ਸ਼ੁਰੂ, linenr_T ਸਿਰਾ);
ਸਥਿਰ ਖਾਲੀ fold_ਜੋੜੋ_Marker(linenr_T lnum, char_u *marker, ਪੂਰਨ markerlen);
ਸਥਿਰ ਖਾਲੀ deleteFoldMarkers(fold_T *fp, ਪੂਰਨ recursive, linenr_T lnum_off);
ਸਥਿਰ ਖਾਲੀ foldDelMarker(linenr_T lnum, char_u *marker, ਪੂਰਨ markerlen);
ਸਥਿਰ ਖਾਲੀ foldUpdateIEMS(win_T *wp, linenr_T ਸਿਖਰ, linenr_T bot);
ਸਥਿਰ ਖਾਲੀ parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
ਸਥਿਰ linenr_T invalid_ਸਿਖਰ = (linenr_T)0;
ਸਥਿਰ linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
ਸਥਿਰ linenr_T prev_lnum = 0;
ਸਥਿਰ ਪੂਰਨ prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define ਪੂਰਾ_NOTHING	0
# define ਪੂਰਾ_ACTION	1	// did close or open a fold
# define ਪੂਰਾ_FOLD	2	// did find a fold

ਸਥਿਰ ਪੂਰਨ foldstartmarkerlen;
ਸਥਿਰ char_u *foldendmarker;
ਸਥਿਰ ਪੂਰਨ foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    ਖਾਲੀ
copyFoldingState(win_T *wp_from, win_T *wp_ਮੰਜ਼ਿਲ)
{
    wp_ਮੰਜ਼ਿਲ->w_fold_manual = wp_from->w_fold_manual;
    wp_ਮੰਜ਼ਿਲ->w_foldinvalid = wp_from->w_foldinvalid;
    cloneFoldGrowArray(&wp_from->w_folds, &wp_ਮੰਜ਼ਿਲ->w_folds);
}

// hasAnyFolding() {{{2
/*
 * Return TRUE if there may be folded lines in window "win".
 */
    ਪੂਰਨ
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    ਵਾਪਸ (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    ਪੂਰਨ
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    ਵਾਪਸ hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ਨਿਰਾ);
}

// hasFoldingWin() {{{2
    ਪੂਰਨ
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    ਪੂਰਨ		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    ਪੂਰਨ		had_folded = FALSE;
    linenr_T	ਪਹਿਲਾ = 0;
    linenr_T	ਆਖਰੀ = 0;
    linenr_T	lnum_rel = lnum;
    ਪੂਰਨ		x;
    fold_T	*fp;
    ਪੂਰਨ		level = 0;
    ਪੂਰਨ		use_level = FALSE;
    ਪੂਰਨ		maybe_small = FALSE;
    garray_T	*gap;
    ਪੂਰਨ		ਨੀਵਾਂ_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    ਜੇ (!hasAnyFolding(win))
    {
	ਜੇ (infop != ਨਿਰਾ)
	    infop->fi_level = 0;
	ਵਾਪਸ FALSE;
    }

    ਜੇ (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = ਲੱਭੋ_wl_ਇੰਦਰਾਜ(win, lnum);
	ਜੇ (x >= 0)
	{
	    ਪਹਿਲਾ = win->w_ਸਤਰਾਂ[x].wl_lnum;
	    ਆਖਰੀ = win->w_ਸਤਰਾਂ[x].wl_lastlnum;
	    had_folded = win->w_ਸਤਰਾਂ[x].wl_folded;
	}
    }

    ਜੇ (ਪਹਿਲਾ == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	ਲਈ (;;)
	{
	    ਜੇ (!fold_ਲੱਭੋ(gap, lnum_rel, &fp))
		ਰੋਕੋ;

	    // Remember lowest level of fold that starts in "lnum".
	    ਜੇ (lnum_rel == fp->fd_ਸਿਖਰ && ਨੀਵਾਂ_level == 0)
		ਨੀਵਾਂ_level = level + 1;

	    ਪਹਿਲਾ += fp->fd_ਸਿਖਰ;
	    ਆਖਰੀ += fp->fd_ਸਿਖਰ;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    ਜੇ (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		ਆਖਰੀ += fp->fd_len - 1;
		ਰੋਕੋ;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_ਸਿਖਰ;
	    ++level;
	}
    }

    ਜੇ (!had_folded)
    {
	ਜੇ (infop != ਨਿਰਾ)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_ਨੀਵਾਂ_level = ਨੀਵਾਂ_level == 0 ? level : ਨੀਵਾਂ_level;
	}
	ਵਾਪਸ FALSE;
    }

    ਜੇ (ਆਖਰੀ > win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਆਖਰੀ = win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (lastp != ਨਿਰਾ)
	*lastp = ਆਖਰੀ;
    ਜੇ (firstp != ਨਿਰਾ)
	*firstp = ਪਹਿਲਾ;
    ਜੇ (infop != ਨਿਰਾ)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = ਪਹਿਲਾ;
	infop->fi_ਨੀਵਾਂ_level = ਨੀਵਾਂ_level == 0 ? level + 1 : ਨੀਵਾਂ_level;
    }
    ਵਾਪਸ TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    ਪੂਰਨ
ਸਤਰ_Folded(win_T *win, linenr_T lnum)
{
    ਵਾਪਸ folded_ਗਿਣਤੀ(win, lnum, ਨਿਰਾ) != 0;
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
    ਲੰਮਾ
folded_ਗਿਣਤੀ(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	ਆਖਰੀ;

    ਜੇ (hasFoldingWin(win, lnum, ਨਿਰਾ, &ਆਖਰੀ, FALSE, infop))
	ਵਾਪਸ (ਲੰਮਾ)(ਆਖਰੀ - lnum + 1);
    ਵਾਪਸ 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    ਪੂਰਨ
foldmethodIsManual(win_T *wp)
{
    ਵਾਪਸ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    ਪੂਰਨ
foldmethodIsIndent(win_T *wp)
{
    ਵਾਪਸ (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    ਪੂਰਨ
foldmethod_Is_ਵਿਅੰਜਕ(win_T *wp)
{
    ਵਾਪਸ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    ਪੂਰਨ
foldmethodIsMarker(win_T *wp)
{
    ਵਾਪਸ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    ਪੂਰਨ
foldmethodIsSyntax(win_T *wp)
{
    ਵਾਪਸ (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    ਪੂਰਨ
foldmethodIsDiff(win_T *wp)
{
    ਵਾਪਸ (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ਖਾਲੀ
closeFold(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਸੈੱਟ_Fold_Repeat(lnum, ਗਿਣਤੀ_2, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    ਖਾਲੀ
closeFoldRecurse(linenr_T lnum)
{
    (ਖਾਲੀ)ਸੈੱਟ_Manual_Fold(lnum, FALSE, TRUE, ਨਿਰਾ);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    ਖਾਲੀ
opFoldRange(
    linenr_T	ਪਹਿਲਾ,
    linenr_T	ਆਖਰੀ,
    ਪੂਰਨ		opening,	// TRUE to open, FALSE to close
    ਪੂਰਨ		recurse,	// TRUE to do it recursively
    ਪੂਰਨ		had_visual)	// TRUE when Visual selection used
{
    ਪੂਰਨ		ਪੂਰਾ = ਪੂਰਾ_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_ਅਗਲਾ;

    ਲਈ (lnum = ਪਹਿਲਾ; lnum <= ਆਖਰੀ; lnum = lnum_ਅਗਲਾ + 1)
    {
	lnum_ਅਗਲਾ = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	ਜੇ (opening && !recurse)
	    (ਖਾਲੀ)hasFolding(lnum, ਨਿਰਾ, &lnum_ਅਗਲਾ);
	(ਖਾਲੀ)ਸੈੱਟ_Manual_Fold(lnum, opening, recurse, &ਪੂਰਾ);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	ਜੇ (!opening && !recurse)
	    (ਖਾਲੀ)hasFolding(lnum, ਨਿਰਾ, &lnum_ਅਗਲਾ);
    }
    ਜੇ (ਪੂਰਾ == ਪੂਰਾ_NOTHING)
	emsg(_(e_no_fold_ਮਿਲਿਆ));
    // Force a redraw to remove the Visual highlighting.
    ਜੇ (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ਖਾਲੀ
openFold(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਸੈੱਟ_Fold_Repeat(lnum, ਗਿਣਤੀ_2, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    ਖਾਲੀ
openFoldRecurse(linenr_T lnum)
{
    (ਖਾਲੀ)ਸੈੱਟ_Manual_Fold(lnum, TRUE, TRUE, ਨਿਰਾ);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    ਖਾਲੀ
foldOpenCursor(ਖਾਲੀ)
{
    ਪੂਰਨ		ਪੂਰਾ;

    checkupdate(curwin);
    ਜੇ (hasAnyFolding(curwin))
	ਲਈ (;;)
	{
	    ਪੂਰਾ = ਪੂਰਾ_NOTHING;
	    (ਖਾਲੀ)ਸੈੱਟ_Manual_Fold(curwin->w_cursor.lnum, TRUE, FALSE, &ਪੂਰਾ);
	    ਜੇ (!(ਪੂਰਾ & ਪੂਰਾ_ACTION))
		ਰੋਕੋ;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    ਖਾਲੀ
newFoldLevel(ਖਾਲੀ)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    ਜੇ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    ਸਥਿਰ ਖਾਲੀ
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    ਪੂਰਨ		i;

    checkupdate(wp);
    ਜੇ (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_ਅੰਕੜੇ;
	ਲਈ (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    ਖਾਲੀ
foldCheckClose(ਖਾਲੀ)
{
    ਜੇ (*p_fcl == NUL)
	ਵਾਪਸ;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    ਜੇ (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(ਪੂਰਨ)curwin->w_p_fdl))
	ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
}

// checkCloseRec() {{{2
    ਸਥਿਰ ਪੂਰਨ
checkCloseRec(garray_T *gap, linenr_T lnum, ਪੂਰਨ level)
{
    fold_T	*fp;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		i;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	ਜੇ (fp[i].fd_flags == FD_OPEN)
	{
	    ਜੇ (level <= 0 && (lnum < fp[i].fd_ਸਿਖਰ
				      || lnum >= fp[i].fd_ਸਿਖਰ + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_ਸਿਖਰ,
								   level - 1);
	}
    }
    ਵਾਪਸ retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    ਪੂਰਨ
foldManualAllowed(ਪੂਰਨ create)
{
    ਜੇ (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	ਵਾਪਸ TRUE;
    ਜੇ (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    ਨਹੀਂਤਾਂ
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    ਵਾਪਸ FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    ਖਾਲੀ
foldCreate(linenr_T ਸ਼ੁਰੂ, linenr_T ਸਿਰਾ)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		cont;
    ਪੂਰਨ		use_level = FALSE;
    ਪੂਰਨ		closed = FALSE;
    ਪੂਰਨ		level = 0;
    linenr_T	ਸ਼ੁਰੂ_rel = ਸ਼ੁਰੂ;
    linenr_T	ਸਿਰਾ_rel = ਸਿਰਾ;

    ਜੇ (ਸ਼ੁਰੂ > ਸਿਰਾ)
    {
	// reverse the range
	ਸਿਰਾ = ਸ਼ੁਰੂ_rel;
	ਸ਼ੁਰੂ = ਸਿਰਾ_rel;
	ਸ਼ੁਰੂ_rel = ਸ਼ੁਰੂ;
	ਸਿਰਾ_rel = ਸਿਰਾ;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    ਜੇ (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(ਸ਼ੁਰੂ, ਸਿਰਾ);
	ਵਾਪਸ;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    ਜੇ (gap->ga_len == 0)
	i = 0;
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (;;)
	{
	    ਜੇ (!fold_ਲੱਭੋ(gap, ਸ਼ੁਰੂ_rel, &fp))
		ਰੋਕੋ;
	    ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len > ਸਿਰਾ_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		ਸ਼ੁਰੂ_rel -= fp->fd_ਸਿਖਰ;
		ਸਿਰਾ_rel -= fp->fd_ਸਿਖਰ;
		ਜੇ (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    ਜੇ (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		ਰੋਕੋ;
	    }
	}
	ਜੇ (gap->ga_len == 0)
	    i = 0;
	ਨਹੀਂਤਾਂ
	    i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ);
    }

    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i;
    ga_init2(&fold_ga, ਆਕਾਰ(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    ਲਈ (cont = 0; i + cont < gap->ga_len; ++cont)
	ਜੇ (fp[cont].fd_ਸਿਖਰ > ਸਿਰਾ_rel)
	    ਰੋਕੋ;
    ਜੇ (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	ਜੇ (ਸ਼ੁਰੂ_rel > fp->fd_ਸਿਖਰ)
	    ਸ਼ੁਰੂ_rel = fp->fd_ਸਿਖਰ;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	ਜੇ (ਸਿਰਾ_rel < fp[cont - 1].fd_ਸਿਖਰ + fp[cont - 1].fd_len - 1)
	    ਸਿਰਾ_rel = fp[cont - 1].fd_ਸਿਖਰ + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_ਅੰਕੜੇ, fp, ਆਕਾਰ(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	ਲਈ (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_ਅੰਕੜੇ)[j].fd_ਸਿਖਰ -= ਸ਼ੁਰੂ_rel;
    }
    // Move remaining entries to after the new fold.
    ਜੇ (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_ਅੰਕੜੇ + i,
		ਆਕਾਰ(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_ਸਿਖਰ = ਸ਼ੁਰੂ_rel;
    fp->fd_len = ਸਿਰਾ_rel - ਸ਼ੁਰੂ_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    ਜੇ (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(ਸ਼ੁਰੂ, 1L);
    ਜੇ (!use_level)
	curwin->w_fold_manual = true;
    fp->fd_flags = FD_CLOSED;
    fp->fd_small = MAYBE;

    // redraw
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
}

// deleteFold() {{{2
/*
 * Delete a fold at line "start" in the current window.
 * When "end" is not 0, delete all folds from "start" to "end".
 * When "recursive" is TRUE delete recursively.
 */
    ਖਾਲੀ
deleteFold(
    linenr_T	ਸ਼ੁਰੂ,
    linenr_T	ਸਿਰਾ,
    ਪੂਰਨ		recursive,
    ਪੂਰਨ		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*ਮਿਲਿਆ_ga;
    fold_T	*ਮਿਲਿਆ_fp = ਨਿਰਾ;
    linenr_T	ਮਿਲਿਆ_off = 0;
    ਪੂਰਨ		use_level;
    ਪੂਰਨ		maybe_small = FALSE;
    ਪੂਰਨ		level = 0;
    linenr_T	lnum = ਸ਼ੁਰੂ;
    linenr_T	lnum_off;
    ਪੂਰਨ		did_one = FALSE;
    linenr_T	ਪਹਿਲਾ_lnum = MAXLNUM;
    linenr_T	ਆਖਰੀ_lnum = 0;

    checkupdate(curwin);

    ਜਦੋਂ (lnum <= ਸਿਰਾ)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	ਮਿਲਿਆ_ga = ਨਿਰਾ;
	lnum_off = 0;
	use_level = FALSE;
	ਲਈ (;;)
	{
	    ਜੇ (!fold_ਲੱਭੋ(gap, lnum - lnum_off, &fp))
		ਰੋਕੋ;
	    // lnum is inside this fold, remember info
	    ਮਿਲਿਆ_ga = gap;
	    ਮਿਲਿਆ_fp = fp;
	    ਮਿਲਿਆ_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    ਜੇ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		ਰੋਕੋ;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ਸਿਖਰ;
	    ++level;
	}
	ਜੇ (ਮਿਲਿਆ_ga == ਨਿਰਾ)
	{
	    ++lnum;
	}
	ਨਹੀਂਤਾਂ
	{
	    lnum = ਮਿਲਿਆ_fp->fd_ਸਿਖਰ + ਮਿਲਿਆ_fp->fd_len + ਮਿਲਿਆ_off;

	    ਜੇ (foldmethodIsManual(curwin))
		delete_Fold_ਇੰਦਰਾਜ(ਮਿਲਿਆ_ga,
		    (ਪੂਰਨ)(ਮਿਲਿਆ_fp - (fold_T *)ਮਿਲਿਆ_ga->ga_ਅੰਕੜੇ), recursive);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਪਹਿਲਾ_lnum > ਮਿਲਿਆ_fp->fd_ਸਿਖਰ + ਮਿਲਿਆ_off)
		    ਪਹਿਲਾ_lnum = ਮਿਲਿਆ_fp->fd_ਸਿਖਰ + ਮਿਲਿਆ_off;
		ਜੇ (ਆਖਰੀ_lnum < lnum)
		    ਆਖਰੀ_lnum = lnum;
		ਜੇ (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(ਮਿਲਿਆ_fp, recursive, ਮਿਲਿਆ_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
	}
    }
    ਜੇ (!did_one)
    {
	emsg(_(e_no_fold_ਮਿਲਿਆ));
	// Force a redraw to remove the Visual highlighting.
	ਜੇ (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    ਨਹੀਂਤਾਂ
	// Deleting markers may make cursor column invalid.
	check_cursor_ਖਾਨਾ();

    ਜੇ (ਆਖਰੀ_lnum > 0)
	ਬਦਲਿਆ_ਸਤਰਾਂ(ਪਹਿਲਾ_lnum, (colnr_T)0, ਆਖਰੀ_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
foldUpdate(win_T *wp, linenr_T ਸਿਖਰ, linenr_T bot)
{
    fold_T	*fp;

    ਜੇ (disable_fold_update > 0)
	ਵਾਪਸ;
# ifdef FEAT_DIFF
    ਜੇ (need_diff_redraw)
	// will update later
	ਵਾਪਸ;
# endif

    ਜੇ (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_ਸ਼ੁਰੂ = ਸਿਖਰ;
	linenr_T	maybe_small_ਸਿਰਾ = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	ਜੇ (ਸਿਖਰ > bot)
	{
	    maybe_small_ਸ਼ੁਰੂ = bot;
	    maybe_small_ਸਿਰਾ = ਸਿਖਰ;
	}
	(ਖਾਲੀ)fold_ਲੱਭੋ(&wp->w_folds, maybe_small_ਸ਼ੁਰੂ, &fp);
	ਜਦੋਂ (fp < (fold_T *)wp->w_folds.ga_ਅੰਕੜੇ + wp->w_folds.ga_len
		&& fp->fd_ਸਿਖਰ <= maybe_small_ਸਿਰਾ)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    ਜੇ (foldmethodIsIndent(wp)
	    || foldmethod_Is_ਵਿਅੰਜਕ(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	ਪੂਰਨ save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, ਸਿਖਰ, bot);
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
    ਖਾਲੀ
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
    ਪੂਰਨ
fold_Move_ਮੰਜ਼ਿਲ(
    ਪੂਰਨ		updown,
    ਪੂਰਨ		dir,	    // FORWARD or BACKWARD
    ਲੰਮਾ	ਗਿਣਤੀ_2)
{
    ਲੰਮਾ	n;
    ਪੂਰਨ		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_ਮਿਲਿਆ;
    linenr_T	lnum;
    ਪੂਰਨ		use_level;
    ਪੂਰਨ		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    ਪੂਰਨ		level;
    ਪੂਰਨ		ਆਖਰੀ;

    checkupdate(curwin);

    // Repeat "count" times.
    ਲਈ (n = 0; n < ਗਿਣਤੀ_2; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	ਜੇ (gap->ga_len == 0)
	    ਰੋਕੋ;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_ਮਿਲਿਆ = curwin->w_cursor.lnum;
	level = 0;
	ਆਖਰੀ = FALSE;
	ਲਈ (;;)
	{
	    ਜੇ (!fold_ਲੱਭੋ(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		ਜੇ (!updown || gap->ga_len == 0)
		    ਰੋਕੋ;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		ਜੇ (dir == FORWARD)
		{
		    ਜੇ (fp - (fold_T *)gap->ga_ਅੰਕੜੇ >= gap->ga_len)
			ਰੋਕੋ;
		    --fp;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (fp == (fold_T *)gap->ga_ਅੰਕੜੇ)
			ਰੋਕੋ;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		ਆਖਰੀ = TRUE;
	    }

	    ਜੇ (!ਆਖਰੀ)
	    {
		// Check if this fold is closed.
		ਜੇ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    ਆਖਰੀ = TRUE;

		// "[z" and "]z" stop at closed fold
		ਜੇ (ਆਖਰੀ && !updown)
		    ਰੋਕੋ;
	    }

	    ਜੇ (updown)
	    {
		ਜੇ (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    ਜੇ (fp + 1 - (fold_T *)gap->ga_ਅੰਕੜੇ < gap->ga_len)
		    {
			lnum = fp[1].fd_ਸਿਖਰ + lnum_off;
			ਜੇ (lnum > curwin->w_cursor.lnum)
			    lnum_ਮਿਲਿਆ = lnum;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // to end of previous fold if there is one
		    ਜੇ (fp > (fold_T *)gap->ga_ਅੰਕੜੇ)
		    {
			lnum = fp[-1].fd_ਸਿਖਰ + lnum_off + fp[-1].fd_len - 1;
			ਜੇ (lnum < curwin->w_cursor.lnum)
			    lnum_ਮਿਲਿਆ = lnum;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		ਜੇ (dir == FORWARD)
		{
		    lnum = fp->fd_ਸਿਖਰ + lnum_off + fp->fd_len - 1;
		    ਜੇ (lnum > curwin->w_cursor.lnum)
			lnum_ਮਿਲਿਆ = lnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    lnum = fp->fd_ਸਿਖਰ + lnum_off;
		    ਜੇ (lnum < curwin->w_cursor.lnum)
			lnum_ਮਿਲਿਆ = lnum;
		}
	    }

	    ਜੇ (ਆਖਰੀ)
		ਰੋਕੋ;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ਸਿਖਰ;
	    ++level;
	}
	ਜੇ (lnum_ਮਿਲਿਆ != curwin->w_cursor.lnum)
	{
	    ਜੇ (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_ਮਿਲਿਆ;
	    curwin->w_cursor.ਖਾਨਾ = 0;
	    retval = OK;
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    ਵਾਪਸ retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    ਖਾਲੀ
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, ਆਕਾਰ(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    ਪੂਰਨ
ਲੱਭੋ_wl_ਇੰਦਰਾਜ(win_T *win, linenr_T lnum)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < win->w_ਸਤਰਾਂ_valid; ++i)
	ਜੇ (win->w_ਸਤਰਾਂ[i].wl_valid)
	{
	    ਜੇ (lnum < win->w_ਸਤਰਾਂ[i].wl_lnum)
		ਵਾਪਸ -1;
	    ਜੇ (lnum <= win->w_ਸਤਰਾਂ[i].wl_lastlnum)
		ਵਾਪਸ i;
	}
    ਵਾਪਸ -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    ਖਾਲੀ
foldAdjustVisual(ਖਾਲੀ)
{
    pos_T	*ਸ਼ੁਰੂ, *ਸਿਰਾ;

    ਜੇ (!VIsual_ਸਰਗਰਮ || !hasAnyFolding(curwin))
	ਵਾਪਸ;

    ਜੇ (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	ਸ਼ੁਰੂ = &VIsual;
	ਸਿਰਾ = &curwin->w_cursor;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸ਼ੁਰੂ = &curwin->w_cursor;
	ਸਿਰਾ = &VIsual;
    }
    ਜੇ (hasFolding(ਸ਼ੁਰੂ->lnum, &ਸ਼ੁਰੂ->lnum, ਨਿਰਾ))
	ਸ਼ੁਰੂ->ਖਾਨਾ = 0;

    ਜੇ (!hasFolding(ਸਿਰਾ->lnum, ਨਿਰਾ, &ਸਿਰਾ->lnum))
	ਵਾਪਸ;

    ਸਿਰਾ->ਖਾਨਾ = ml_get_len(ਸਿਰਾ->lnum);
    ਜੇ (ਸਿਰਾ->ਖਾਨਾ > 0 && *p_sel == 'o')
	--ਸਿਰਾ->ਖਾਨਾ;
    // prevent cursor from moving on the trail byte
    ਜੇ (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    ਖਾਲੀ
foldAdjustCursor(ਖਾਲੀ)
{
    (ਖਾਲੀ)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ਨਿਰਾ);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    ਖਾਲੀ
cloneFoldGrowArray(garray_T *from, garray_T *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		i;
    fold_T	*from_p;
    fold_T	*ਮੰਜ਼ਿਲ_p;

    ga_init2(ਮੰਜ਼ਿਲ, from->ga_itemsize, from->ga_growsize);
    ਜੇ (from->ga_len == 0 || ga_grow(ਮੰਜ਼ਿਲ, from->ga_len) == FAIL)
	ਵਾਪਸ;

    from_p = (fold_T *)from->ga_ਅੰਕੜੇ;
    ਮੰਜ਼ਿਲ_p = (fold_T *)ਮੰਜ਼ਿਲ->ga_ਅੰਕੜੇ;

    ਲਈ (i = 0; i < from->ga_len; i++)
    {
	ਮੰਜ਼ਿਲ_p->fd_ਸਿਖਰ = from_p->fd_ਸਿਖਰ;
	ਮੰਜ਼ਿਲ_p->fd_len = from_p->fd_len;
	ਮੰਜ਼ਿਲ_p->fd_flags = from_p->fd_flags;
	ਮੰਜ਼ਿਲ_p->fd_small = from_p->fd_small;
	cloneFoldGrowArray(&from_p->fd_nested, &ਮੰਜ਼ਿਲ_p->fd_nested);
	++ਮੰਜ਼ਿਲ->ga_len;
	++from_p;
	++ਮੰਜ਼ਿਲ_p;
    }
}

// foldFind() {{{2
/*
 * Search for line "lnum" in folds of growarray "gap".
 * Set "*fpp" to the fold struct for the fold that contains "lnum" or
 * the first fold below it (careful: it can be beyond the end of the array!).
 * Returns FALSE when there is no fold that contains "lnum".
 */
    ਸਥਿਰ ਪੂਰਨ
fold_ਲੱਭੋ(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	ਨੀਵਾਂ, ਉੱਚਾ;
    fold_T	*fp;
    ਪੂਰਨ		i;

    ਜੇ (gap->ga_len == 0)
    {
	*fpp = ਨਿਰਾ;
	ਵਾਪਸ FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_ਅੰਕੜੇ;
    ਨੀਵਾਂ = 0;
    ਉੱਚਾ = gap->ga_len - 1;
    ਜਦੋਂ (ਨੀਵਾਂ <= ਉੱਚਾ)
    {
	i = (ਨੀਵਾਂ + ਉੱਚਾ) / 2;
	ਜੇ (fp[i].fd_ਸਿਖਰ > lnum)
	    // fold below lnum, adjust high
	    ਉੱਚਾ = i - 1;
	ਨਹੀਂਤਾਂ ਜੇ (fp[i].fd_ਸਿਖਰ + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    ਨੀਵਾਂ = i + 1;
	ਨਹੀਂਤਾਂ
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    ਵਾਪਸ TRUE;
	}
    }
    *fpp = fp + ਨੀਵਾਂ;
    ਵਾਪਸ FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    ਸਥਿਰ ਪੂਰਨ
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    ਪੂਰਨ		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    ਲਈ (;;)
    {
	ਜੇ (!fold_ਲੱਭੋ(gap, lnum_rel, &fp))
	    ਰੋਕੋ;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_ਸਿਖਰ;
	++level;
    }

    ਵਾਪਸ level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    ਸਥਿਰ ਖਾਲੀ
checkupdate(win_T *wp)
{
    ਜੇ (!wp->w_foldinvalid)
	ਵਾਪਸ;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_Fold_Repeat(linenr_T lnum, ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ do_open)
{
    ਪੂਰਨ		ਪੂਰਾ;
    ਲੰਮਾ	n;

    ਲਈ (n = 0; n < ਗਿਣਤੀ_2; ++n)
    {
	ਪੂਰਾ = ਪੂਰਾ_NOTHING;
	(ਖਾਲੀ)ਸੈੱਟ_Manual_Fold(lnum, do_open, FALSE, &ਪੂਰਾ);
	ਜੇ (!(ਪੂਰਾ & ਪੂਰਾ_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    ਜੇ (n == 0 && !(ਪੂਰਾ & ਪੂਰਾ_FOLD))
		emsg(_(e_no_fold_ਮਿਲਿਆ));
	    ਰੋਕੋ;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    ਸਥਿਰ linenr_T
ਸੈੱਟ_Manual_Fold(
    linenr_T	lnum,
    ਪੂਰਨ		opening,    // TRUE when opening, FALSE when closing
    ਪੂਰਨ		recurse,    // TRUE when closing/opening recursive
    ਪੂਰਨ		*donep)
{
# ifdef FEAT_DIFF
    ਜੇ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		ਜੇ (dlnum != 0)
		    (ਖਾਲੀ)ਸੈੱਟ_Manual_Fold_Win(wp, dlnum, opening, recurse, ਨਿਰਾ);
	    }
	}
    }
# endif

    ਵਾਪਸ ਸੈੱਟ_Manual_Fold_Win(curwin, lnum, opening, recurse, donep);
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
    ਸਥਿਰ linenr_T
ਸੈੱਟ_Manual_Fold_Win(
    win_T	*wp,
    linenr_T	lnum,
    ਪੂਰਨ		opening,    // TRUE when opening, FALSE when closing
    ਪੂਰਨ		recurse,    // TRUE when closing/opening recursive
    ਪੂਰਨ		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*ਮਿਲਿਆ = ਨਿਰਾ;
    ਪੂਰਨ		j;
    ਪੂਰਨ		level = 0;
    ਪੂਰਨ		use_level = FALSE;
    ਪੂਰਨ		ਮਿਲਿਆ_fold = FALSE;
    garray_T	*gap;
    linenr_T	ਅਗਲਾ = MAXLNUM;
    linenr_T	off = 0;
    ਪੂਰਨ		ਪੂਰਾ = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    ਲਈ (;;)
    {
	ਜੇ (!fold_ਲੱਭੋ(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    ਜੇ (fp != ਨਿਰਾ && fp < (fold_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len)
		ਅਗਲਾ = fp->fd_ਸਿਖਰ + off;
	    ਰੋਕੋ;
	}

	// lnum is inside this fold
	ਮਿਲਿਆ_fold = TRUE;

	// If there is a following fold, continue there next time.
	ਜੇ (fp + 1 < (fold_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len)
	    ਅਗਲਾ = fp[1].fd_ਸਿਖਰ + off;

	// Change from level-dependent folding to manual.
	ਜੇ (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    ਜੇ (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    ਨਹੀਂਤਾਂ
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_ਅੰਕੜੇ;
	    ਲਈ (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	ਜੇ (!opening && recurse)
	{
	    ਜੇ (fp->fd_flags != FD_CLOSED)
	    {
		ਪੂਰਾ |= ਪੂਰਾ_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    ਜੇ (opening)
	    {
		fp->fd_flags = FD_OPEN;
		ਪੂਰਾ |= ਪੂਰਾ_ACTION;
		ਜੇ (recurse)
		    foldOpenNested(fp);
	    }
	    ਰੋਕੋ;
	}

	// fold is open, check nested folds
	ਮਿਲਿਆ = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_ਸਿਖਰ;
	off += fp->fd_ਸਿਖਰ;
	++level;
    }
    ਜੇ (ਮਿਲਿਆ_fold)
    {
	// When closing and not recurse, close deepest open fold.
	ਜੇ (!opening && ਮਿਲਿਆ != ਨਿਰਾ)
	{
	    ਮਿਲਿਆ->fd_flags = FD_CLOSED;
	    ਪੂਰਾ |= ਪੂਰਾ_ACTION;
	}
	wp->w_fold_manual = true;
	ਜੇ (ਪੂਰਾ & ਪੂਰਾ_ACTION)
	    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);
	ਪੂਰਾ |= ਪੂਰਾ_FOLD;
    }
    ਨਹੀਂਤਾਂ ਜੇ (donep == ਨਿਰਾ && wp == curwin)
	emsg(_(e_no_fold_ਮਿਲਿਆ));

    ਜੇ (donep != ਨਿਰਾ)
	*donep |= ਪੂਰਾ;

    ਵਾਪਸ ਅਗਲਾ;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    ਸਥਿਰ ਖਾਲੀ
foldOpenNested(fold_T *fpr)
{
    ਪੂਰਨ		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    ਸਥਿਰ ਖਾਲੀ
delete_Fold_ਇੰਦਰਾਜ(garray_T *gap, ਪੂਰਨ idx, ਪੂਰਨ recursive)
{
    fold_T	*fp;
    ਪੂਰਨ		i;
    ਲੰਮਾ	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + idx;
    ਜੇ (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	ਜੇ (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, ਆਕਾਰ(fold_T) * (gap->ga_len - idx));
    }
    ਨਹੀਂਤਾਂ
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	ਜੇ (ga_grow(gap, (ਪੂਰਨ)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_ਅੰਕੜੇ;
	    ਲਈ (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_ਸਿਖਰ += fp->fd_ਸਿਖਰ;
		ਜੇ (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		ਜੇ (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    ਜੇ (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  ਆਕਾਰ(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(ਆਕਾਰ(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    ਖਾਲੀ
deleteFoldRecurse(garray_T *gap)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_T *)(gap->ga_ਅੰਕੜੇ))[i].fd_nested));
    ga_clear(gap);
}

// foldMarkAdjust() {{{2
/*
 * Update line numbers of folds for inserted/deleted lines.
 *
 * We are adjusting the folds in the range from line1 til line2,
 * make sure that line2 does not get smaller than line1
 */
    ਖਾਲੀ
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    ਜੇ (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    ਜੇ (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    ਸਥਿਰ ਖਾਲੀ
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after)
{
    fold_T	*fp;
    ਪੂਰਨ		i;
    linenr_T	ਆਖਰੀ;
    linenr_T	ਸਿਖਰ;

    ਜੇ (gap->ga_len == 0)
	ਵਾਪਸ;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && amount == (linenr_T)1 && line2 == MAXLNUM)
	ਸਿਖਰ = line1 + 1;
    ਨਹੀਂਤਾਂ
	ਸਿਖਰ = line1;

    // Find the fold containing or just below "line1".
    (ਖਾਲੀ)fold_ਲੱਭੋ(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    ਲਈ (i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ); i < gap->ga_len; ++i, ++fp)
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

	ਆਖਰੀ = fp->fd_ਸਿਖਰ + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	ਜੇ (ਆਖਰੀ < line1)
	    ਜਾਰੀ;

	// 6. fold below line2: only adjust for amount_after
	ਜੇ (fp->fd_ਸਿਖਰ > line2)
	{
	    ਜੇ (amount_after == 0)
		ਰੋਕੋ;
	    fp->fd_ਸਿਖਰ += amount_after;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fp->fd_ਸਿਖਰ >= ਸਿਖਰ && ਆਖਰੀ <= line2)
	    {
		// 4. fold completely contained in range
		ਜੇ (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    delete_Fold_ਇੰਦਰਾਜ(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		ਨਹੀਂਤਾਂ
		    fp->fd_ਸਿਖਰ += amount;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (fp->fd_ਸਿਖਰ < ਸਿਖਰ)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ਸਿਖਰ,
				  line2 - fp->fd_ਸਿਖਰ, amount, amount_after);
		    ਜੇ (ਆਖਰੀ <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			ਜੇ (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_ਸਿਖਰ;
			ਨਹੀਂਤਾਂ
			    fp->fd_len += amount;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    ਜੇ (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ਸਿਖਰ,
				  amount,
				  amount_after + (fp->fd_ਸਿਖਰ - ਸਿਖਰ));
			fp->fd_len -= line2 - fp->fd_ਸਿਖਰ + 1;
			fp->fd_ਸਿਖਰ = line1;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ਸਿਖਰ,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_ਸਿਖਰ += amount;
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
    ਪੂਰਨ
get_ਸਭ_ਤੋਂ_ਡੂੰਘਾ_Nesting(ਖਾਲੀ)
{
    checkupdate(curwin);
    ਵਾਪਸ get_ਸਭ_ਤੋਂ_ਡੂੰਘਾ_Nesting_Recurse(&curwin->w_folds);
}

    ਸਥਿਰ ਪੂਰਨ
get_ਸਭ_ਤੋਂ_ਡੂੰਘਾ_Nesting_Recurse(garray_T *gap)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		level;
    ਪੂਰਨ		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	level = get_ਸਭ_ਤੋਂ_ਡੂੰਘਾ_Nesting_Recurse(&fp[i].fd_nested) + 1;
	ਜੇ (level > maxlevel)
	    maxlevel = level;
    }

    ਵਾਪਸ maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    ਸਥਿਰ ਪੂਰਨ
check_closed(
    win_T	*win,
    fold_T	*fp,
    ਪੂਰਨ		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    ਪੂਰਨ		level,		    // folding depth
    ਪੂਰਨ		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    ਪੂਰਨ		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    ਜੇ (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	ਜੇ (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    ਜੇ (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    ਜੇ (closed)
    {
	ਜੇ (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	ਜੇ (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    ਵਾਪਸ closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    ਸਥਿਰ ਖਾਲੀ
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		n;

    ਜੇ (fp->fd_small != MAYBE)
	ਵਾਪਸ;

    // Mark any nested folds to maybe-small
    ਸੈੱਟ_Small_Maybe(&fp->fd_nested);

    ਜੇ (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    ਨਹੀਂਤਾਂ
    {
	ਗਿਣਤੀ_2 = 0;
	ਲਈ (n = 0; n < fp->fd_len; ++n)
	{
	    ਗਿਣਤੀ_2 += plines_win_nofold(wp, fp->fd_ਸਿਖਰ + lnum_off + n);
	    ਜੇ (ਗਿਣਤੀ_2 > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		ਵਾਪਸ;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_Small_Maybe(garray_T *gap)
{
    ਪੂਰਨ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    ਸਥਿਰ ਖਾਲੀ
foldCreateMarkers(linenr_T ਸ਼ੁਰੂ, linenr_T ਸਿਰਾ)
{
    ਜੇ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ਵਾਪਸ;
    }
    parseMarker(curwin);

    fold_ਜੋੜੋ_Marker(ਸ਼ੁਰੂ, curwin->w_p_fmr, foldstartmarkerlen);
    fold_ਜੋੜੋ_Marker(ਸਿਰਾ, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    ਬਦਲਿਆ_ਸਤਰਾਂ(ਸ਼ੁਰੂ, (colnr_T)0, ਸਿਰਾ, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    ਸਥਿਰ ਖਾਲੀ
fold_ਜੋੜੋ_Marker(linenr_T lnum, char_u *marker, ਪੂਰਨ markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		ਸਤਰ_len;
    char_u	*newline;
    char_u	*p = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)curbuf->b_p_cms, "%s");
    ਪੂਰਨ		ਸਤਰ_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    ਸਤਰ = ml_get(lnum);
    ਸਤਰ_len = ml_get_len(lnum);

    ਜੇ (u_save(lnum - 1, lnum + 1) != OK)
	ਵਾਪਸ;

    // Check if the line ends with an unclosed comment
    (ਖਾਲੀ)skip_comment(ਸਤਰ, FALSE, FALSE, &ਸਤਰ_is_comment);
    newline = alloc(ਸਤਰ_len + markerlen + STRLEN(cms) + 1);
    ਜੇ (newline == ਨਿਰਾ)
	ਵਾਪਸ;
    STRCPY(newline, ਸਤਰ);
    // Append the marker to the end of the line
    ਜੇ (p == ਨਿਰਾ || ਸਤਰ_is_comment)
	vim_strncpy(newline + ਸਤਰ_len, marker, markerlen);
    ਨਹੀਂਤਾਂ
    {
	STRCPY(newline + ਸਤਰ_len, cms);
	STRNCPY(newline + ਸਤਰ_len + (p - cms), marker, markerlen);
	STRCPY(newline + ਸਤਰ_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    ਸਥਿਰ ਖਾਲੀ
deleteFoldMarkers(
    fold_T	*fp,
    ਪੂਰਨ		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    ਪੂਰਨ		i;

    ਜੇ (recursive)
	ਲਈ (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_ਅੰਕੜੇ + i, TRUE,
						       lnum_off + fp->fd_ਸਿਖਰ);
    foldDelMarker(fp->fd_ਸਿਖਰ + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_ਸਿਖਰ + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    ਸਥਿਰ ਖਾਲੀ
foldDelMarker(linenr_T lnum, char_u *marker, ਪੂਰਨ markerlen)
{
    char_u	*ਸਤਰ;
    char_u	*newline;
    char_u	*p;
    ਪੂਰਨ		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;
    ਸਤਰ = ml_get(lnum);
    ਲਈ (p = ਸਤਰ; *p != NUL; ++p)
	ਜੇ (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    ਜੇ (VIM_ISDIGIT(p[len]))
		++len;
	    ਜੇ (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)cms, "%s");
		ਜੇ (cms2 != ਨਿਰਾ && p - ਸਤਰ >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (ਪੂਰਨ)STRLEN(cms) - 2;
		}
	    }
	    ਜੇ (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		ਜੇ (newline != ਨਿਰਾ)
		{
		    STRNCPY(newline, ਸਤਰ, p - ਸਤਰ);
		    STRCPY(newline + (p - ਸਤਰ), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    ਰੋਕੋ;
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
    char_u	*ਪਾਠ = ਨਿਰਾ;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    ਸਥਿਰ ਪੂਰਨ	    got_fdt_error = FALSE;
    ਪੂਰਨ		    save_did_emsg = did_emsg;
    ਸਥਿਰ win_T    *ਆਖਰੀ_wp = ਨਿਰਾ;
    ਸਥਿਰ linenr_T ਆਖਰੀ_lnum = 0;

    ਜੇ (ਆਖਰੀ_wp != wp || ਆਖਰੀ_wp == ਨਿਰਾ
					|| ਆਖਰੀ_lnum > lnum || ਆਖਰੀ_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    ਜੇ (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    ਜੇ (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	ਪੂਰਨ	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	ਸੈੱਟ_vim_var_nr(VV_FOLDSTART, lnum);
	ਸੈੱਟ_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	ਜੇ (level > (ਪੂਰਨ)ਆਕਾਰ(dashes) - 1)
	    level = (ਪੂਰਨ)ਆਕਾਰ(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	ਸੈੱਟ_vim_var_string(VV_FOLDDASHES, dashes, level);
	ਸੈੱਟ_vim_var_nr(VV_FOLDLEVEL, (ਲੰਮਾ)level);

	// skip evaluating 'foldtext' on errors
	ਜੇ (!got_fdt_error)
	{
	    win_T   *save_curwin = curwin;
	    sctx_T  saved_sctx = current_sctx;

	    curwin = wp;
	    curbuf = wp->w_ਬਫਰ;
	    current_sctx = wp->w_p_script_ctx[WV_FDT];

	    ++emsg_off; // handle exceptions, but don't display errors
	    ਪਾਠ = eval_ਮੰਜ਼ਿਲ_string_ਸੁਰੱਖਿਅਤ(wp->w_p_fdt,
		       was_ਸੈੱਟ_insecurely(wp, (char_u *)"foldtext", OPT_LOCAL),
		       TRUE, TRUE);
	    --emsg_off;

	    ਜੇ (ਪਾਠ == ਨਿਰਾ || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_ਬਫਰ;
	    current_sctx = saved_sctx;
	}
	ਆਖਰੀ_lnum = lnum;
	ਆਖਰੀ_wp   = wp;
	ਸੈੱਟ_vim_var_string(VV_FOLDDASHES, ਨਿਰਾ, -1);

	ਜੇ (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	ਜੇ (ਪਾਠ != ਨਿਰਾ)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    ਲਈ (p = ਪਾਠ; *p != NUL; ++p)
	    {
		ਪੂਰਨ	len;

		ਜੇ (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    ਜੇ (!vim_isprintc((*mb_ptr2char)(p)))
			ਰੋਕੋ;
		    p += len - 1;
		}
		ਨਹੀਂਤਾਂ
		    ਜੇ (*p == TAB)
			*p = ' ';
		    ਨਹੀਂਤਾਂ ਜੇ (ptr2cells(p) > 1)
			ਰੋਕੋ;
	    }
	    ਜੇ (*p != NUL)
	    {
		p = transstr(ਪਾਠ);
		vim_free(ਪਾਠ);
		ਪਾਠ = p;
	    }
	}
    }
    ਜੇ (ਪਾਠ == ਨਿਰਾ)
# endif
    {
	ਲੰਮਾ ਗਿਣਤੀ_2 = (ਲੰਮਾ)(lnume - lnum + 1);

	vim_snprintf((ਅੱਖਰ *)buf, FOLD_ਪਾਠ_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", ਗਿਣਤੀ_2),
		     ਗਿਣਤੀ_2);
	ਪਾਠ = buf;
    }
    ਵਾਪਸ ਪਾਠ;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    ਸਥਿਰ ਖਾਲੀ
foldtext_cleanup(char_u *str)
{
    char_u	*cms_ਸ਼ੁਰੂ;	// first part or the whole comment
    ਪੂਰਨ		cms_slen = 0;	// length of cms_start
    char_u	*cms_ਸਿਰਾ;	// last part of the comment or NULL
    ਪੂਰਨ		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    ਪੂਰਨ		len;
    ਪੂਰਨ		did1 = FALSE;
    ਪੂਰਨ		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_ਸ਼ੁਰੂ = skipwhite(curbuf->b_p_cms);
    cms_slen = (ਪੂਰਨ)STRLEN(cms_ਸ਼ੁਰੂ);
    ਜਦੋਂ (cms_slen > 0 && VIM_ISWHITE(cms_ਸ਼ੁਰੂ[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_ਸਿਰਾ = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)cms_ਸ਼ੁਰੂ, "%s");
    ਜੇ (cms_ਸਿਰਾ != ਨਿਰਾ)
    {
	cms_elen = cms_slen - (ਪੂਰਨ)(cms_ਸਿਰਾ - cms_ਸ਼ੁਰੂ);
	cms_slen = (ਪੂਰਨ)(cms_ਸਿਰਾ - cms_ਸ਼ੁਰੂ);

	// exclude white space before "%s"
	ਜਦੋਂ (cms_slen > 0 && VIM_ISWHITE(cms_ਸ਼ੁਰੂ[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_ਸਿਰਾ + 2);
	cms_elen -= (ਪੂਰਨ)(s - cms_ਸਿਰਾ);
	cms_ਸਿਰਾ = s;
    }
    parseMarker(curwin);

    ਲਈ (s = str; *s != NUL; )
    {
	len = 0;
	ਜੇ (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	ਜੇ (len > 0)
	{
	    ਜੇ (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    ਲਈ (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    ਜੇ (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_ਸ਼ੁਰੂ, cms_slen) == 0)
	    {
		len += (ਪੂਰਨ)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (cms_ਸਿਰਾ != ਨਿਰਾ)
	{
	    ਜੇ (!did1 && cms_slen > 0 && STRNCMP(s, cms_ਸ਼ੁਰੂ, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_ਸਿਰਾ, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	ਜੇ (len != 0)
	{
	    ਜਦੋਂ (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	ਨਹੀਂਤਾਂ
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    win_T	*wp;		//ਖਿੜਕੀ
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    ਪੂਰਨ		lvl;		// current level (-1 for undefined)
    ਪੂਰਨ		lvl_ਅਗਲਾ;	// level used for next line
    ਪੂਰਨ		ਸ਼ੁਰੂ;		// number of folds that are forced to start at
				// this line.
    ਪੂਰਨ		ਸਿਰਾ;		// level of fold that is forced to end below
				// this line
    ਪੂਰਨ		had_ਸਿਰਾ;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
ਸਥਿਰ ਪੂਰਨ fold_ਬਦਲਿਆ;

// Function declarations. {{{2
ਸਥਿਰ linenr_T foldUpdateIEMSRecurse(garray_T *gap, ਪੂਰਨ level, linenr_T startlnum, fline_T *flp, ਖਾਲੀ (*getlevel)(fline_T *), linenr_T bot, ਪੂਰਨ topflags);
ਸਥਿਰ ਪੂਰਨ fold_ਪਾਓ(garray_T *gap, ਪੂਰਨ i);
ਸਥਿਰ ਖਾਲੀ fold_ਵੰਡੋ(garray_T *gap, ਪੂਰਨ i, linenr_T ਸਿਖਰ, linenr_T bot);
ਸਥਿਰ ਖਾਲੀ foldRemove(garray_T *gap, linenr_T ਸਿਖਰ, linenr_T bot);
ਸਥਿਰ ਖਾਲੀ fold_ਮਿਲਾਓ(fold_T *fp1, garray_T *gap, fold_T *fp2);
ਸਥਿਰ ਖਾਲੀ foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
ਸਥਿਰ ਖਾਲੀ foldlevelDiff(fline_T *flp);
# endif
ਸਥਿਰ ਖਾਲੀ foldlevel_ਵਿਅੰਜਕ(fline_T *flp);
ਸਥਿਰ ਖਾਲੀ foldlevelMarker(fline_T *flp);
ਸਥਿਰ ਖਾਲੀ foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    ਸਥਿਰ ਖਾਲੀ
foldUpdateIEMS(win_T *wp, linenr_T ਸਿਖਰ, linenr_T bot)
{
    linenr_T	ਸ਼ੁਰੂ;
    linenr_T	ਸਿਰਾ;
    fline_T	fline;
    ਖਾਲੀ	(*getlevel)(fline_T *);
    ਪੂਰਨ		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    ਜੇ (invalid_ਸਿਖਰ != (linenr_T)0)
	ਵਾਪਸ;

    ਜੇ (wp->w_foldinvalid)
    {
	// Need to update all folds.
	ਸਿਖਰ = 1;
	bot = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	ਸੈੱਟ_Small_Maybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    ਜੇ (foldmethodIsDiff(wp))
    {
	ਜੇ (ਸਿਖਰ > diff_context)
	    ਸਿਖਰ -= diff_context;
	ਨਹੀਂਤਾਂ
	    ਸਿਖਰ = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    ਜੇ (ਸਿਖਰ > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਸਿਖਰ = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    fold_ਬਦਲਿਆ = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_ਅਗਲਾ = -1;
    fline.ਸ਼ੁਰੂ = 0;
    fline.ਸਿਰਾ = MAX_LEVEL + 1;
    fline.had_ਸਿਰਾ = MAX_LEVEL + 1;

    invalid_ਸਿਖਰ = ਸਿਖਰ;
    invalid_bot = bot;

    ਜੇ (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	ਜੇ (ਸਿਖਰ > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, ਸਿਖਰ - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = ਸਿਖਰ - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    ਜੇ (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_ਅਗਲਾ);
	    ਨਹੀਂਤਾਂ
		fline.lvl = fline.lvl_ਅਗਲਾ;
	}
	fline.lnum = ਸਿਖਰ;
	getlevel(&fline);
    }
    ਨਹੀਂਤਾਂ
    {
	fline.lnum = ਸਿਖਰ;
	ਜੇ (foldmethod_Is_ਵਿਅੰਜਕ(wp))
	{
	    getlevel = foldlevel_ਵਿਅੰਜਕ;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    ਜੇ (ਸਿਖਰ > 1)
		--fline.lnum;
	}
	ਨਹੀਂਤਾਂ ਜੇ (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	ਨਹੀਂਤਾਂ ਜੇ (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	ਨਹੀਂਤਾਂ
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    ਜੇ (ਸਿਖਰ > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	ਲਈ ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_ਅਗਲਾ = -1;
	    getlevel(&fline);
	    ਜੇ (fline.lvl >= 0)
		ਰੋਕੋ;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    ਜੇ (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ਨਿਰਾ;
	ਪੂਰਨ	  current_fdl = 0;
	linenr_T  fold_ਸ਼ੁਰੂ_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	ਜਦੋਂ (current_fdl < fline.lvl)
	{
	    ਜੇ (!fold_ਲੱਭੋ(gap, lnum_rel, &fpn))
		ਰੋਕੋ;
	    ++current_fdl;

	    fold_ਸ਼ੁਰੂ_lnum += fpn->fd_ਸਿਖਰ;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_ਸਿਖਰ;
	}
	ਜੇ (fpn != ਨਿਰਾ && current_fdl == fline.lvl)
	{
	    linenr_T fold_ਸਿਰਾ_lnum = fold_ਸ਼ੁਰੂ_lnum + fpn->fd_len;

	    ਜੇ (fold_ਸਿਰਾ_lnum > bot)
		bot = fold_ਸਿਰਾ_lnum;
	}
    }

    ਸ਼ੁਰੂ = fline.lnum;
    ਸਿਰਾ = bot;
    // Do at least one line.
    ਜੇ (ਸ਼ੁਰੂ > ਸਿਰਾ && ਸਿਰਾ < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਸਿਰਾ = ਸ਼ੁਰੂ;
    ਜਦੋਂ (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	ਜੇ (fline.lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;
	ਜੇ (fline.lnum > ਸਿਰਾ)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    ਜੇ (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_ਵਿਅੰਜਕ)
		ਰੋਕੋ;
	    ਜੇ ((ਸ਼ੁਰੂ <= ਸਿਰਾ
			&& fold_ਲੱਭੋ(&wp->w_folds, ਸਿਰਾ, &fp)
			&& fp->fd_ਸਿਖਰ + fp->fd_len - 1 > ਸਿਰਾ)
		    || (fline.lvl == 0
			&& fold_ਲੱਭੋ(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_ਸਿਖਰ < fline.lnum))
		ਸਿਰਾ = fp->fd_ਸਿਖਰ + fp->fd_len - 1;
	    ਨਹੀਂਤਾਂ ਜੇ (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		ਸਿਰਾ = fline.lnum;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	ਜੇ (fline.lvl > 0)
	{
	    invalid_ਸਿਖਰ = fline.lnum;
	    invalid_bot = ਸਿਰਾ;
	    ਸਿਰਾ = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, ਸ਼ੁਰੂ, &fline, getlevel, ਸਿਰਾ, FD_LEVEL);
	    ਸ਼ੁਰੂ = fline.lnum;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fline.lnum == wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_ਅਗਲਾ;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, ਸ਼ੁਰੂ, ਸਿਰਾ);

    // If some fold changed, need to redraw and position cursor.
    ਜੇ (fold_ਬਦਲਿਆ && wp->w_p_fen)
	ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    ਜੇ (ਸਿਰਾ != bot)
	redraw_win_range_later(wp, ਸਿਖਰ, ਸਿਰਾ);

    invalid_ਸਿਖਰ = (linenr_T)0;
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
    ਸਥਿਰ linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    ਪੂਰਨ		level,
    linenr_T	startlnum,
    fline_T	*flp,
    ਖਾਲੀ	(*getlevel)(fline_T *),
    linenr_T	bot,
    ਪੂਰਨ		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ਨਿਰਾ;
    fold_T	*fp2;
    ਪੂਰਨ		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਅੰਤ = FALSE;
    linenr_T	linecount = flp->wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - flp->off;
    ਪੂਰਨ		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    ਜੇ (getlevel == foldlevelMarker && flp->ਸ਼ੁਰੂ <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(ਖਾਲੀ)fold_ਲੱਭੋ(gap, startlnum - 1, &fp);
	ਜੇ (fp != ਨਿਰਾ && (fp >= ((fold_T *)gap->ga_ਅੰਕੜੇ) + gap->ga_len
						   || fp->fd_ਸਿਖਰ >= startlnum))
	    fp = ਨਿਰਾ;
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
    ਜਦੋਂ (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	ਸਤਰ_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	ਜੇ (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	ਜੇ (flp->lnum > firstlnum
		&& (level > lvl - flp->ਸ਼ੁਰੂ || level >= flp->had_ਸਿਰਾ))
	    lvl = 0;

	ਜੇ (flp->lnum > bot && !ਅੰਤ && fp != ਨਿਰਾ)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    ਜੇ (getlevel != foldlevelMarker
		    && getlevel != foldlevel_ਵਿਅੰਜਕ
		    && getlevel != foldlevelSyntax)
		ਰੋਕੋ;
	    i = 0;
	    fp2 = fp;
	    ਜੇ (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_ਸਿਖਰ;
		ਜਦੋਂ (fold_ਲੱਭੋ(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_ਸਿਖਰ;
		}
	    }
	    ਜੇ (lvl < level + i)
	    {
		(ਖਾਲੀ)fold_ਲੱਭੋ(&fp->fd_nested, flp->lnum - fp->fd_ਸਿਖਰ, &fp2);
		ਜੇ (fp2 != ਨਿਰਾ)
		    bot = fp2->fd_ਸਿਖਰ + fp2->fd_len - 1 + fp->fd_ਸਿਖਰ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len <= flp->lnum && lvl >= level)
		ਅੰਤ = TRUE;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	ਜੇ (fp == ਨਿਰਾ
		&& (lvl != level
		    || flp->lnum_save >= bot
		    || flp->ਸ਼ੁਰੂ != 0
		    || flp->had_ਸਿਰਾ <= MAX_LEVEL
		    || flp->lnum == linecount))
	{
	    /*
	     * Remove or update folds that have lines between startlnum and
	     * firstlnum.
	     */
	    ਜਦੋਂ (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		ਜੇ (flp->ਸ਼ੁਰੂ != 0 || flp->had_ਸਿਰਾ <= MAX_LEVEL)
		    concat = 0;
		ਨਹੀਂਤਾਂ
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		ਜੇ (gap->ga_len > 0 && (fold_ਲੱਭੋ(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_ਅੰਕੜੇ) + gap->ga_len
			    && fp->fd_ਸਿਖਰ <= firstlnum)
			|| fold_ਲੱਭੋ(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_ਅੰਕੜੇ) + gap->ga_len
			    && ((lvl < level && fp->fd_ਸਿਖਰ < flp->lnum)
				|| (lvl >= level
					   && fp->fd_ਸਿਖਰ <= flp->lnum_save)))))
		{
		    ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			ਜੇ (fp->fd_ਸਿਖਰ == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			ਨਹੀਂਤਾਂ ਜੇ (fp->fd_ਸਿਖਰ >= startlnum)
			{
			    ਜੇ (fp->fd_ਸਿਖਰ > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(ਲੰਮਾ)(fp->fd_ਸਿਖਰ - firstlnum), 0L);
			    ਨਹੀਂਤਾਂ
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(ਲੰਮਾ)(firstlnum - fp->fd_ਸਿਖਰ - 1),
					(linenr_T)MAXLNUM,
					(ਲੰਮਾ)(fp->fd_ਸਿਖਰ - firstlnum));
			    fp->fd_len += fp->fd_ਸਿਖਰ - firstlnum;
			    fp->fd_ਸਿਖਰ = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_ਬਦਲਿਆ = TRUE;
			}
			ਨਹੀਂਤਾਂ ਜੇ ((flp->ਸ਼ੁਰੂ != 0 && lvl == level)
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
			    ਜੇ (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_ਸਿਖਰ,
						      breakend - fp->fd_ਸਿਖਰ);
			    i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ);
			    fold_ਵੰਡੋ(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    ਜੇ (getlevel == foldlevelMarker
				    || getlevel == foldlevel_ਵਿਅੰਜਕ
				    || getlevel == foldlevelSyntax)
				ਅੰਤ = TRUE;
			}

			ਜੇ (fp->fd_ਸਿਖਰ == startlnum && concat)
			{
			    i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ);
			    ਜੇ (i != 0)
			    {
				fp2 = fp - 1;
				ਜੇ (fp2->fd_ਸਿਖਰ + fp2->fd_len == fp->fd_ਸਿਖਰ)
				{
				    fold_ਮਿਲਾਓ(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			ਰੋਕੋ;
		    }
		    ਜੇ (fp->fd_ਸਿਖਰ >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			delete_Fold_ਇੰਦਰਾਜ(gap,
				     (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ), TRUE);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_ਸਿਖਰ;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_ਬਦਲਿਆ = TRUE;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    ਜੇ (gap->ga_len == 0)
			i = 0;
		    ਨਹੀਂਤਾਂ
			i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ);
		    ਜੇ (fold_ਪਾਓ(gap, i) != OK)
			ਵਾਪਸ bot;
		    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_ਸਿਖਰ = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    ਜੇ (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (i <= 0)
		    {
			fp->fd_flags = topflags;
			ਜੇ (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    ਨਹੀਂਤਾਂ
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    ਜੇ (getlevel == foldlevelMarker
			    || getlevel == foldlevel_ਵਿਅੰਜਕ
			    || getlevel == foldlevelSyntax)
			ਅੰਤ = TRUE;
		    fold_ਬਦਲਿਆ = TRUE;
		    ਰੋਕੋ;
		}
	    }
	}

	ਜੇ (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    ਰੋਕੋ;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	ਜੇ (lvl > level && fp != ਨਿਰਾ)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    ਜੇ (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_ਸਿਖਰ;
	    flp->off += fp->fd_ਸਿਖਰ;
	    i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_ਸਿਖਰ, flp, getlevel,
					      bot - fp->fd_ਸਿਖਰ, fp->fd_flags);
	    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i;
	    flp->lnum += fp->fd_ਸਿਖਰ;
	    flp->lnum_save += fp->fd_ਸਿਖਰ;
	    flp->off -= fp->fd_ਸਿਖਰ;
	    bot += fp->fd_ਸਿਖਰ;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    ਜਦੋਂ (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		ਜੇ (++flp->lnum > linecount)
		    ਰੋਕੋ;
		flp->lvl = flp->lvl_ਅਗਲਾ;
		getlevel(flp);
		ਜੇ (flp->lvl >= 0 || flp->had_ਸਿਰਾ <= MAX_LEVEL)
		    ਰੋਕੋ;
	    }
	    prev_lnum = 0;
	    ਜੇ (flp->lnum > linecount)
		ਰੋਕੋ;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    ਜੇ (fp == ਨਿਰਾ)	// only happens when got_int is set
	ਵਾਪਸ bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    ਜੇ (fp->fd_len < flp->lnum - fp->fd_ਸਿਖਰ)
    {
	fp->fd_len = flp->lnum - fp->fd_ਸਿਖਰ;
	fp->fd_small = MAYBE;
	fold_ਬਦਲਿਆ = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_ਸਿਖਰ + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_ਸਿਖਰ,
						  flp->lnum - 1 - fp->fd_ਸਿਖਰ);

    ਜੇ (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	ਜੇ (fp->fd_len != flp->lnum - fp->fd_ਸਿਖਰ)
	{
	    ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		ਜੇ (getlevel == foldlevelMarker
			|| getlevel == foldlevel_ਵਿਅੰਜਕ
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_ਸਿਖਰ + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_ਸਿਖਰ;
		}
		ਨਹੀਂਤਾਂ
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ);
		    fold_ਵੰਡੋ(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i;
		}
	    }
	    ਨਹੀਂਤਾਂ
		fp->fd_len = flp->lnum - fp->fd_ਸਿਖਰ;
	    fold_ਬਦਲਿਆ = TRUE;
	}
    }

    // delete following folds that end before the current line
    ਲਈ (;;)
    {
	fp2 = fp + 1;
	ਜੇ (fp2 >= (fold_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len
						  || fp2->fd_ਸਿਖਰ > flp->lnum)
	    ਰੋਕੋ;
	ਜੇ (fp2->fd_ਸਿਖਰ + fp2->fd_len > flp->lnum)
	{
	    ਜੇ (fp2->fd_ਸਿਖਰ < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (ਲੰਮਾ)(flp->lnum - fp2->fd_ਸਿਖਰ - 1),
			(linenr_T)MAXLNUM, (ਲੰਮਾ)(fp2->fd_ਸਿਖਰ - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_ਸਿਖਰ;
		fp2->fd_ਸਿਖਰ = flp->lnum;
		fold_ਬਦਲਿਆ = TRUE;
	    }

	    ਜੇ (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		fold_ਮਿਲਾਓ(fp, gap, fp2);
	    }
	    ਰੋਕੋ;
	}
	fold_ਬਦਲਿਆ = TRUE;
	delete_Fold_ਇੰਦਰਾਜ(gap, (ਪੂਰਨ)(fp2 - (fold_T *)gap->ga_ਅੰਕੜੇ), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    ਜੇ (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    ਵਾਪਸ bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
fold_ਪਾਓ(garray_T *gap, ਪੂਰਨ i)
{
    fold_T	*fp;

    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i;
    ਜੇ (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, ਆਕਾਰ(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, ਆਕਾਰ(fold_T), 10);
    ਵਾਪਸ OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    ਸਥਿਰ ਖਾਲੀ
fold_ਵੰਡੋ(
    garray_T	*gap,
    ਪੂਰਨ		i,
    linenr_T	ਸਿਖਰ,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		len;

    // The fold continues below bot, need to split it.
    ਜੇ (fold_ਪਾਓ(gap, i + 1) == FAIL)
	ਵਾਪਸ;
    fp = (fold_T *)gap->ga_ਅੰਕੜੇ + i;
    fp[1].fd_ਸਿਖਰ = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_ਸਿਖਰ - fp->fd_ਸਿਖਰ);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (ਖਾਲੀ)fold_ਲੱਭੋ(gap1, bot + 1 - fp->fd_ਸਿਖਰ, &fp2);
    ਜੇ (fp2 != ਨਿਰਾ)
    {
	len = (ਪੂਰਨ)((fold_T *)gap1->ga_ਅੰਕੜੇ + gap1->ga_len - fp2);
	ਜੇ (len > 0 && ga_grow(gap2, len) == OK)
	{
	    ਲਈ (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_ਅੰਕੜੇ)[idx] = fp2[idx];
		((fold_T *)gap2->ga_ਅੰਕੜੇ)[idx].fd_ਸਿਖਰ
						  -= fp[1].fd_ਸਿਖਰ - fp->fd_ਸਿਖਰ;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = ਸਿਖਰ - fp->fd_ਸਿਖਰ;
    fold_ਬਦਲਿਆ = TRUE;
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
    ਸਥਿਰ ਖਾਲੀ
foldRemove(garray_T *gap, linenr_T ਸਿਖਰ, linenr_T bot)
{
    fold_T	*fp = ਨਿਰਾ;

    ਜੇ (bot < ਸਿਖਰ)
	ਵਾਪਸ;		// nothing to do

    ਜਦੋਂ (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	ਜੇ (fold_ਲੱਭੋ(gap, ਸਿਖਰ, &fp) && fp->fd_ਸਿਖਰ < ਸਿਖਰ)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, ਸਿਖਰ - fp->fd_ਸਿਖਰ, bot - fp->fd_ਸਿਖਰ);
	    ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		fold_ਵੰਡੋ(gap, (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ), ਸਿਖਰ, bot);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// 2: truncate fold at "top".
		fp->fd_len = ਸਿਖਰ - fp->fd_ਸਿਖਰ;
	    }
	    fold_ਬਦਲਿਆ = TRUE;
	    ਜਾਰੀ;
	}
	ਜੇ (fp >= (fold_T *)(gap->ga_ਅੰਕੜੇ) + gap->ga_len
		|| fp->fd_ਸਿਖਰ > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    ਰੋਕੋ;
	}
	ਜੇ (fp->fd_ਸਿਖਰ >= ਸਿਖਰ)
	{
	    // Found an entry below top.
	    fold_ਬਦਲਿਆ = TRUE;
	    ਜੇ (fp->fd_ਸਿਖਰ + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (ਲੰਮਾ)(bot - fp->fd_ਸਿਖਰ),
			(linenr_T)MAXLNUM, (ਲੰਮਾ)(fp->fd_ਸਿਖਰ - bot - 1));
		fp->fd_len -= bot - fp->fd_ਸਿਖਰ + 1;
		fp->fd_ਸਿਖਰ = bot + 1;
		ਰੋਕੋ;
	    }

	    // 4: Delete completely contained fold.
	    delete_Fold_ਇੰਦਰਾਜ(gap, (ਪੂਰਨ)(fp - (fold_T *)gap->ga_ਅੰਕੜੇ), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    ਸਥਿਰ ਖਾਲੀ
fold_ਉਲਟ_ਕ੍ਰਮ(garray_T *gap, linenr_T ਸ਼ੁਰੂ_arg, linenr_T ਸਿਰਾ_arg)
{
    fold_T *ਖੱਬਾ, *ਸੱਜਾ;
    fold_T tmp;
    linenr_T ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_arg;
    linenr_T ਸਿਰਾ = ਸਿਰਾ_arg;

    ਲਈ (; ਸ਼ੁਰੂ < ਸਿਰਾ; ਸ਼ੁਰੂ++, ਸਿਰਾ--)
    {
	ਖੱਬਾ = (fold_T *)gap->ga_ਅੰਕੜੇ + ਸ਼ੁਰੂ;
	ਸੱਜਾ = (fold_T *)gap->ga_ਅੰਕੜੇ + ਸਿਰਾ;
	tmp  = *ਖੱਬਾ;
	*ਖੱਬਾ = *ਸੱਜਾ;
	*ਸੱਜਾ = tmp;
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

    ਸਥਿਰ ਖਾਲੀ
truncate_fold(fold_T *fp, linenr_T ਸਿਰਾ)
{
    ਸਿਰਾ += 1;
    foldRemove(&fp->fd_nested, ਸਿਰਾ - fp->fd_ਸਿਖਰ, MAXLNUM);
    fp->fd_len = ਸਿਰਾ - fp->fd_ਸਿਖਰ;
}

# define fold_ਸਿਰਾ(fp) ((fp)->fd_ਸਿਖਰ + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_ਅੰਕੜੇ + (gap)->ga_len))
# define fold_ਸੂਚਕਾਂਕ(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_ਅੰਕੜੇ)))

    ਖਾਲੀ
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    ਪੂਰਨ at_ਸ਼ੁਰੂ = fold_ਲੱਭੋ(gap, line1 - 1, &fp);
    size_t move_ਸ਼ੁਰੂ = 0, move_ਸਿਰਾ = 0, dest_ਸੂਚਕਾਂਕ = 0;

    ਜੇ (at_ਸ਼ੁਰੂ)
    {
	ਜੇ (fold_ਸਿਰਾ(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_ਸਿਖਰ, line2 -
		    fp->fd_ਸਿਖਰ, dest - fp->fd_ਸਿਖਰ);
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (fold_ਸਿਰਾ(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ਸਿਖਰ, line2 -
		    fp->fd_ਸਿਖਰ, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	ਨਹੀਂਤਾਂ
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    ਜੇ (!valid_fold(fp, gap) || fp->fd_ਸਿਖਰ > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fp->fd_ਸਿਖਰ > line2)
    {
	ਲਈ (; valid_fold(fp, gap) && fold_ਸਿਰਾ(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_ਸਿਖਰ -= range_len;

	ਜੇ (valid_fold(fp, gap) && fp->fd_ਸਿਖਰ <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_ਸਿਖਰ -= range_len;
	}
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fold_ਸਿਰਾ(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_ਸਿਖਰ, dest -
		fp->fd_ਸਿਖਰ, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_ਸਿਖਰ += move_len;
	ਵਾਪਸ;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_ਸ਼ੁਰੂ = fold_ਸੂਚਕਾਂਕ(fp, gap);

    ਲਈ (; valid_fold(fp, gap) && fp->fd_ਸਿਖਰ <= dest; fp++)
    {
	ਜੇ (fp->fd_ਸਿਖਰ <= line2)
	{
	    // 1. 2. or 3.
	    ਜੇ (fold_ਸਿਰਾ(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_ਸਿਖਰ += move_len;
	    ਜਾਰੀ;
	}

	// Record index of the first fold after the moved range.
	ਜੇ (move_ਸਿਰਾ == 0)
	    move_ਸਿਰਾ = fold_ਸੂਚਕਾਂਕ(fp, gap);

	ਜੇ (fold_ਸਿਰਾ(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_ਸਿਖਰ -= range_len;
    }

    dest_ਸੂਚਕਾਂਕ = fold_ਸੂਚਕਾਂਕ(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    ਜੇ (move_ਸਿਰਾ == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	ਵਾਪਸ;
    fold_ਉਲਟ_ਕ੍ਰਮ(gap, (linenr_T)move_ਸ਼ੁਰੂ, (linenr_T)dest_ਸੂਚਕਾਂਕ - 1);
    fold_ਉਲਟ_ਕ੍ਰਮ(gap, (linenr_T)move_ਸ਼ੁਰੂ,
			   (linenr_T)(move_ਸ਼ੁਰੂ + dest_ਸੂਚਕਾਂਕ - move_ਸਿਰਾ - 1));
    fold_ਉਲਟ_ਕ੍ਰਮ(gap, (linenr_T)(move_ਸ਼ੁਰੂ + dest_ਸੂਚਕਾਂਕ - move_ਸਿਰਾ),
						   (linenr_T)(dest_ਸੂਚਕਾਂਕ - 1));
}
# undef fold_ਸਿਰਾ
# undef valid_fold
# undef fold_ਸੂਚਕਾਂਕ

// foldMerge() {{{2
/*
 * Merge two adjacent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjacent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    ਸਥਿਰ ਖਾਲੀ
fold_ਮਿਲਾਓ(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    ਪੂਰਨ		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    ਜੇ (fold_ਲੱਭੋ(gap1, fp1->fd_len - 1L, &fp3) && fold_ਲੱਭੋ(gap2, 0L, &fp4))
	fold_ਮਿਲਾਓ(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    ਜੇ (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	ਲਈ (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_ਅੰਕੜੇ)[gap1->ga_len]
					= ((fold_T *)gap2->ga_ਅੰਕੜੇ)[idx];
	    ((fold_T *)gap1->ga_ਅੰਕੜੇ)[gap1->ga_len].fd_ਸਿਖਰ += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    delete_Fold_ਇੰਦਰਾਜ(gap, (ਪੂਰਨ)(fp2 - (fold_T *)gap->ga_ਅੰਕੜੇ), TRUE);
    fold_ਬਦਲਿਆ = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ਸਥਿਰ ਖਾਲੀ
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_ਬਫਰ;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    ਜੇ (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ਨਿਰਾ)
    {
	// first and last line can't be undefined, use level 0
	ਜੇ (lnum == 1 || lnum == buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    flp->lvl = 0;
	ਨਹੀਂਤਾਂ
	    flp->lvl = -1;
    }
    ਨਹੀਂਤਾਂ
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_ਮੁੱਲ(buf);
    ਜੇ (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	ਜੇ (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    ਸਥਿਰ ਖਾਲੀ
foldlevelDiff(fline_T *flp)
{
    ਜੇ (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    ਨਹੀਂਤਾਂ
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ਸਥਿਰ ਖਾਲੀ
foldlevel_ਵਿਅੰਜਕ(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->ਸ਼ੁਰੂ = FALSE;
    flp->lvl = 0;
# ਨਹੀਂਤਾਂ
    win_T	*win;
    ਪੂਰਨ		n;
    ਪੂਰਨ		c;
    linenr_T	lnum = flp->lnum + flp->off;
    ਪੂਰਨ		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_ਬਫਰ;
    ਸੈੱਟ_vim_var_nr(VV_LNUM, lnum);

    flp->ਸ਼ੁਰੂ = 0;
    flp->had_ਸਿਰਾ = flp->ਸਿਰਾ;
    flp->ਸਿਰਾ = MAX_LEVEL + 1;
    ਜੇ (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = ਕੁੰਜੀ_Typed;
    n = eval_foldexpr(flp->wp, &c);
    ਕੁੰਜੀ_Typed = save_keytyped;

    ਚੋਣ (c)
    {
	// "a1", "a2", .. : add to the fold level
	ਹਾਲਤ 'a': ਜੇ (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_ਅਗਲਾ = flp->lvl;
		  }
		  flp->ਸ਼ੁਰੂ = n;
		  ਰੋਕੋ;

	// "s1", "s2", .. : subtract from the fold level
	ਹਾਲਤ 's': ਜੇ (flp->lvl >= 0)
		  {
		      ਜੇ (n > flp->lvl)
			  flp->lvl_ਅਗਲਾ = 0;
		      ਨਹੀਂਤਾਂ
			  flp->lvl_ਅਗਲਾ = flp->lvl - n;
		      flp->ਸਿਰਾ = flp->lvl_ਅਗਲਾ + 1;
		  }
		  ਰੋਕੋ;

	// ">1", ">2", .. : start a fold with a certain level
	ਹਾਲਤ '>': flp->lvl = n;
		  flp->lvl_ਅਗਲਾ = n;
		  flp->ਸ਼ੁਰੂ = 1;
		  ਰੋਕੋ;

	// "<1", "<2", .. : end a fold with a certain level
	ਹਾਲਤ '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_ਅਗਲਾ = MIN(flp->lvl, n - 1);
		  flp->ਸਿਰਾ = n;
		  ਰੋਕੋ;

	// "=": No change in level
	ਹਾਲਤ '=': flp->lvl_ਅਗਲਾ = flp->lvl;
		  ਰੋਕੋ;

	// "-1", "0", "1", ..: set fold level
	ਮੂਲ:  ਜੇ (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_ਅਗਲਾ = flp->lvl;
		  ਨਹੀਂਤਾਂ
		      flp->lvl_ਅਗਲਾ = n;
		  flp->lvl = n;
		  ਰੋਕੋ;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    ਜੇ (flp->lvl < 0)
    {
	ਜੇ (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_ਅਗਲਾ = 0;
	}
	ਜੇ (lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    flp->lvl_ਅਗਲਾ = 0;
    }

    curwin = win;
    curbuf = curwin->w_ਬਫਰ;
# endif
}

// parseMarker() {{{2
/*
 * Parse 'foldmarker' and set "foldendmarker", "foldstartmarkerlen" and
 * "foldendmarkerlen".
 * Relies on the option value to have been checked for correctness already.
 */
    ਸਥਿਰ ਖਾਲੀ
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (ਪੂਰਨ)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (ਪੂਰਨ)STRLEN(foldendmarker);
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
    ਸਥਿਰ ਖਾਲੀ
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    ਪੂਰਨ		cstart;
    ਪੂਰਨ		cend;
    ਪੂਰਨ		ਸ਼ੁਰੂ_lvl = flp->lvl;
    char_u	*s;
    ਪੂਰਨ		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->ਸ਼ੁਰੂ = 0;
    flp->lvl_ਅਗਲਾ = flp->lvl;

    s = ml_get_buf(flp->wp->w_ਬਫਰ, flp->lnum + flp->off, FALSE);
    ਜਦੋਂ (*s)
    {
	ਜੇ (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    ਜੇ (VIM_ISDIGIT(*s))
	    {
		n = atoi((ਅੱਖਰ *)s);
		ਜੇ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_ਅਗਲਾ = n;
		    ਜੇ (n <= ਸ਼ੁਰੂ_lvl)
			flp->ਸ਼ੁਰੂ = 1;
		    ਨਹੀਂਤਾਂ
			flp->ਸ਼ੁਰੂ = n - ਸ਼ੁਰੂ_lvl;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		++flp->lvl;
		++flp->lvl_ਅਗਲਾ;
		++flp->ਸ਼ੁਰੂ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    ਜੇ (VIM_ISDIGIT(*s))
	    {
		n = atoi((ਅੱਖਰ *)s);
		ਜੇ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_ਅਗਲਾ = n - 1;
		    // never start a fold with an end marker
		    ਜੇ (flp->lvl_ਅਗਲਾ > ਸ਼ੁਰੂ_lvl)
			flp->lvl_ਅਗਲਾ = ਸ਼ੁਰੂ_lvl;
		}
	    }
	    ਨਹੀਂਤਾਂ
		--flp->lvl_ਅਗਲਾ;
	}
	ਨਹੀਂਤਾਂ
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    ਜੇ (flp->lvl_ਅਗਲਾ < 0)
	flp->lvl_ਅਗਲਾ = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    ਸਥਿਰ ਖਾਲੀ
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->ਸ਼ੁਰੂ = 0;
    flp->lvl = 0;
# ਨਹੀਂਤਾਂ
    linenr_T	lnum = flp->lnum + flp->off;
    ਪੂਰਨ		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->ਸ਼ੁਰੂ = 0;
    ਜੇ (lnum < flp->wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	ਜੇ (n > flp->lvl)
	{
	    flp->ਸ਼ੁਰੂ = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# ਜੇ defined(FEAT_SESSION)
ਸਥਿਰ ਪੂਰਨ put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
ਸਥਿਰ ਪੂਰਨ put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
ਸਥਿਰ ਪੂਰਨ put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    ਪੂਰਨ
put_folds(FILE *fd, win_T *wp)
{
    ਜੇ (foldmethodIsManual(wp))
    {
	ਜੇ (put_ਸਤਰ(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_ਸਤਰ(fd, "&fdl = &fdl") == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    ਜੇ (wp->w_fold_manual)
	ਵਾਪਸ put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    ਵਾਪਸ OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    ਸਥਿਰ ਪੂਰਨ
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    ਪੂਰਨ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	ਜੇ (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_ਸਿਖਰ) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_ਸਿਖਰ + off,
					fp->fd_ਸਿਖਰ + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    ਵਾਪਸ FAIL;
	++fp;
    }
    ਵਾਪਸ OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    ਸਥਿਰ ਪੂਰਨ
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; i++)
    {
	ਜੇ (fp->fd_flags != FD_LEVEL)
	{
	    ਜੇ (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		ਜੇ (fprintf(fd, ":%ld", fp->fd_ਸਿਖਰ + off) < 0
			|| put_eol(fd) == FAIL
			|| put_ਸਤਰ(fd, "sil! normal! zo") == FAIL)
		    ਵਾਪਸ FAIL;
		ਜੇ (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_ਸਿਖਰ)
			== FAIL)
		    ਵਾਪਸ FAIL;
		// close the parent when needed
		ਜੇ (fp->fd_flags == FD_CLOSED)
		{
		    ਜੇ (put_fold_open_close(fd, fp, off) == FAIL)
			ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_ਸਿਖਰ);
		ਜੇ ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		ਜੇ (put_fold_open_close(fd, fp, off) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}
	++fp;
    }

    ਵਾਪਸ OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    ਸਥਿਰ ਪੂਰਨ
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    ਜੇ (fprintf(fd, ":%ld", fp->fd_ਸਿਖਰ + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}
# endif // FEAT_SESSION

// }}}1
#ਜੇ_ਅੰਤ // defined(FEAT_FOLDING)

#ਜੇ defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    ਸਥਿਰ ਖਾਲੀ
foldclosed_both(
    typval_T	*argvars ਅਣਵਰਤਿਆ,
    typval_T	*rettv,
    ਪੂਰਨ		ਸਿਰਾ ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	ਪਹਿਲਾ, ਆਖਰੀ;
    win_T	*wp  = curwin;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_ਸੰਖਿਆ(&argvars[1]), &tp);
	ਜੇ (wp != ਨਿਰਾ && tp != ਨਿਰਾ)
	{
	    ਜੇ (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	ਨਹੀਂਤਾਂ
	{
	    rettv->vval.v_ਸੰਖਿਆ = -2;
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	lnum = tv_get_lnum(argvars);

    ਜੇ (lnum >= 1 && lnum <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	ਜੇ (hasFoldingWin(wp, lnum, &ਪਹਿਲਾ, &ਆਖਰੀ, FALSE, ਨਿਰਾ))
	{
	    ਜੇ (ਸਿਰਾ)
		rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)ਆਖਰੀ;
	    ਨਹੀਂਤਾਂ
		rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)ਪਹਿਲਾ;
	    ਵਾਪਸ;
	}
    }
# endif
    rettv->vval.v_ਸੰਖਿਆ = -1;
}

/*
 * "foldclosed()" function
 */
    ਖਾਲੀ
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    ਖਾਲੀ
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    ਸਥਿਰ ਪੂਰਨ
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    ਜੇ (invalid_ਸਿਖਰ == (linenr_T)0)
	checkupdate(curwin);
    ਨਹੀਂਤਾਂ ਜੇ (lnum == prev_lnum && prev_lnum_lvl >= 0)
	ਵਾਪਸ prev_lnum_lvl;
    ਨਹੀਂਤਾਂ ਜੇ (lnum >= invalid_ਸਿਖਰ && lnum <= invalid_bot)
	ਵਾਪਸ -1;

    // Return quickly when there is no folding at all in this window.
    ਜੇ (!hasAnyFolding(curwin))
	ਵਾਪਸ 0;

    ਵਾਪਸ foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    ਖਾਲੀ
f_foldlevel(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_ਸੰਖਿਆ(&argvars[1]), &tp);
	ਜੇ (wp != ਨਿਰਾ && tp != ਨਿਰਾ)
	{
	    ਜੇ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    rettv->vval.v_ਸੰਖਿਆ = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_ਸੰਖਿਆ = -2;
    }
    ਨਹੀਂਤਾਂ
    {
	lnum = tv_get_lnum(argvars);
	ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    rettv->vval.v_ਸੰਖਿਆ = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    ਖਾਲੀ
f_foldtext(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    ਪੂਰਨ		len;
    ਅੱਖਰ	*txt;
    ਲੰਮਾ	ਗਿਣਤੀ_2;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    ਜੇ (foldstart > 0 && foldend <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && dashes != ਨਿਰਾ)
    {
	// Find first non-empty line in the fold.
	ਲਈ (lnum = foldstart; lnum < foldend; ++lnum)
	    ਜੇ (!linewhite(lnum))
		ਰੋਕੋ;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	ਜੇ (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    ਜੇ (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		ਜੇ (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	ਗਿਣਤੀ_2 = (ਲੰਮਾ)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", ਗਿਣਤੀ_2);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	ਜੇ (r != ਨਿਰਾ)
	{
	    sprintf((ਅੱਖਰ *)r, txt, dashes, ਗਿਣਤੀ_2);
	    len = (ਪੂਰਨ)STRLEN(r);
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
    ਖਾਲੀ
f_foldtextresult(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*ਪਾਠ;
    char_u	buf[FOLD_ਪਾਠ_LEN];
    foldinfo_T  foldinfo;
    ਪੂਰਨ		fold_ਗਿਣਤੀ;
    win_T	*wp  = curwin;
    ਸਥਿਰ ਪੂਰਨ	entered = FALSE;
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (entered)
	ਵਾਪਸ; // reject recursive use
    entered = TRUE;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_ਸੰਖਿਆ(&argvars[1]), &tp);
	ਜੇ (wp == ਨਿਰਾ || tp == ਨਿਰਾ) // wrong winid
	{
	    entered = FALSE;
	    ਵਾਪਸ;
	}
	ਜੇ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    ਨਹੀਂਤਾਂ
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    ਜੇ (lnum < 0)
	lnum = 0;
    fold_ਗਿਣਤੀ = folded_ਗਿਣਤੀ(wp, lnum, &foldinfo);
    ਜੇ (fold_ਗਿਣਤੀ > 0)
    {
	ਪਾਠ = get_foldtext(wp, lnum, lnum + fold_ਗਿਣਤੀ - 1,
							       &foldinfo, buf);
	ਜੇ (ਪਾਠ == buf)
	    ਪਾਠ = vim_strsave(ਪਾਠ);
	rettv->vval.v_string = ਪਾਠ;
    }

    entered = FALSE;
# endif
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
