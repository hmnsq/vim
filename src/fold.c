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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    linenr_T	fd_ತುದಿ;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    ವರ್ಣ	fd_flags;	// see below
    ವರ್ಣ	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
ಸ್ಥಿರ ಶೂನ್ಯ newFoldLevelWin(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ checkCloseRec(garray_T *gap, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ level);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ foldLevelWin(win_T *wp, linenr_T lnum);
ಸ್ಥಿರ ಶೂನ್ಯ checkupdate(win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ setFoldRepeat(linenr_T lnum, long ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ do_open);
ಸ್ಥಿರ linenr_T setManualFold(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ opening, ಪೂರ್ಣಾಙ್ಕ recurse, ಪೂರ್ಣಾಙ್ಕ *donep);
ಸ್ಥಿರ linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ opening, ಪೂರ್ಣಾಙ್ಕ recurse, ಪೂರ್ಣಾಙ್ಕ *donep);
ಸ್ಥಿರ ಶೂನ್ಯ foldOpenNested(fold_T *fpr);
ಸ್ಥಿರ ಶೂನ್ಯ deleteFoldEntry(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ recursive);
ಸ್ಥಿರ ಶೂನ್ಯ foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ getDeepestNestingRecurse(garray_T *gap);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_closed(win_T *win, fold_T *fp, ಪೂರ್ಣಾಙ್ಕ *use_levelp, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ *maybe_smallp, linenr_T lnum_off);
ಸ್ಥಿರ ಶೂನ್ಯ checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
ಸ್ಥಿರ ಶೂನ್ಯ setSmallMaybe(garray_T *gap);
ಸ್ಥಿರ ಶೂನ್ಯ foldCreateMarkers(linenr_T start, linenr_T end);
ಸ್ಥಿರ ಶೂನ್ಯ foldAddMarker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಙ್ಕ markerlen);
ಸ್ಥಿರ ಶೂನ್ಯ deleteFoldMarkers(fold_T *fp, ಪೂರ್ಣಾಙ್ಕ recursive, linenr_T lnum_off);
ಸ್ಥಿರ ಶೂನ್ಯ foldDelMarker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಙ್ಕ markerlen);
ಸ್ಥಿರ ಶೂನ್ಯ foldUpdateIEMS(win_T *wp, linenr_T ತುದಿ, linenr_T bot);
ಸ್ಥಿರ ಶೂನ್ಯ parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
ಸ್ಥಿರ linenr_T invalid_ತುದಿ = (linenr_T)0;
ಸ್ಥಿರ linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
ಸ್ಥಿರ linenr_T prev_lnum = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ foldstartmarkerlen;
ಸ್ಥಿರ char_u *foldendmarker;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    ಶೂನ್ಯ
copyFoldingState(win_T *wp_from, win_T *wp_to)
{
    wp_to->w_fold_manual = wp_from->w_fold_manual;
    wp_to->w_foldinvalid = wp_from->w_foldinvalid;
    cloneFoldGrowArray(&wp_from->w_folds, &wp_to->w_folds);
}

// hasAnyFolding() {{{2
/*
 * Return TRUE if there may be folded lines in window "win".
 */
    ಪೂರ್ಣಾಙ್ಕ
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    ಪ್ರತ್ಯಾಗಮ (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    ಪೂರ್ಣಾಙ್ಕ
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    ಪ್ರತ್ಯಾಗಮ hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ರಿಕ್ತ);
}

// hasFoldingWin() {{{2
    ಪೂರ್ಣಾಙ್ಕ
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    ಪೂರ್ಣಾಙ್ಕ		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    ಪೂರ್ಣಾಙ್ಕ		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    ಪೂರ್ಣಾಙ್ಕ		x;
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		level = 0;
    ಪೂರ್ಣಾಙ್ಕ		use_level = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		maybe_small = FALSE;
    garray_T	*gap;
    ಪೂರ್ಣಾಙ್ಕ		ಕೆಳಗಿನ_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    ಯದಿ (!hasAnyFolding(win))
    {
	ಯದಿ (infop != ರಿಕ್ತ)
	    infop->fi_level = 0;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	ಯದಿ (x >= 0)
	{
	    first = win->w_ಸಾಲುಗಳು[x].wl_lnum;
	    last = win->w_ಸಾಲುಗಳು[x].wl_lastlnum;
	    had_folded = win->w_ಸಾಲುಗಳು[x].wl_folded;
	}
    }

    ಯದಿ (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (!foldFind(gap, lnum_rel, &fp))
		ಭಙ್ಗ;

	    // Remember lowest level of fold that starts in "lnum".
	    ಯದಿ (lnum_rel == fp->fd_ತುದಿ && ಕೆಳಗಿನ_level == 0)
		ಕೆಳಗಿನ_level = level + 1;

	    first += fp->fd_ತುದಿ;
	    last += fp->fd_ತುದಿ;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    ಯದಿ (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		ಭಙ್ಗ;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_ತುದಿ;
	    ++level;
	}
    }

    ಯದಿ (!had_folded)
    {
	ಯದಿ (infop != ರಿಕ್ತ)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_ಕೆಳಗಿನ_level = ಕೆಳಗಿನ_level == 0 ? level : ಕೆಳಗಿನ_level;
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ (last > win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	last = win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಯದಿ (lastp != ರಿಕ್ತ)
	*lastp = last;
    ಯದಿ (firstp != ರಿಕ್ತ)
	*firstp = first;
    ಯದಿ (infop != ರಿಕ್ತ)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_ಕೆಳಗಿನ_level = ಕೆಳಗಿನ_level == 0 ? level + 1 : ಕೆಳಗಿನ_level;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಸಾಲು_Folded(win_T *win, linenr_T lnum)
{
    ಪ್ರತ್ಯಾಗಮ folded_ಎಣಿಕೆ(win, lnum, ರಿಕ್ತ) != 0;
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
    long
folded_ಎಣಿಕೆ(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    ಯದಿ (hasFoldingWin(win, lnum, ರಿಕ್ತ, &last, FALSE, infop))
	ಪ್ರತ್ಯಾಗಮ (long)(last - lnum + 1);
    ಪ್ರತ್ಯಾಗಮ 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    ಪೂರ್ಣಾಙ್ಕ
foldmethodIsManual(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    ಪೂರ್ಣಾಙ್ಕ
foldmethodIsIndent(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    ಪೂರ್ಣಾಙ್ಕ
foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    ಪೂರ್ಣಾಙ್ಕ
foldmethodIsMarker(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    ಪೂರ್ಣಾಙ್ಕ
foldmethodIsSyntax(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    ಪೂರ್ಣಾಙ್ಕ
foldmethodIsDiff(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ಶೂನ್ಯ
closeFold(linenr_T lnum, long ಎಣಿಕೆ)
{
    setFoldRepeat(lnum, ಎಣಿಕೆ, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    ಶೂನ್ಯ
closeFoldRecurse(linenr_T lnum)
{
    (ಶೂನ್ಯ)setManualFold(lnum, FALSE, TRUE, ರಿಕ್ತ);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    ಶೂನ್ಯ
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    ಪೂರ್ಣಾಙ್ಕ		opening,	// TRUE to open, FALSE to close
    ಪೂರ್ಣಾಙ್ಕ		recurse,	// TRUE to do it recursively
    ಪೂರ್ಣಾಙ್ಕ		had_visual)	// TRUE when Visual selection used
{
    ಪೂರ್ಣಾಙ್ಕ		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_next;

    ಪ್ರತಿ (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	ಯದಿ (opening && !recurse)
	    (ಶೂನ್ಯ)hasFolding(lnum, ರಿಕ್ತ, &lnum_next);
	(ಶೂನ್ಯ)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	ಯದಿ (!opening && !recurse)
	    (ಶೂನ್ಯ)hasFolding(lnum, ರಿಕ್ತ, &lnum_next);
    }
    ಯದಿ (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    ಯದಿ (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ಶೂನ್ಯ
openFold(linenr_T lnum, long ಎಣಿಕೆ)
{
    setFoldRepeat(lnum, ಎಣಿಕೆ, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    ಶೂನ್ಯ
openFoldRecurse(linenr_T lnum)
{
    (ಶೂನ್ಯ)setManualFold(lnum, TRUE, TRUE, ರಿಕ್ತ);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    ಶೂನ್ಯ
foldOpenCursor(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		done;

    checkupdate(curwin);
    ಯದಿ (hasAnyFolding(curwin))
	ಪ್ರತಿ (;;)
	{
	    done = DONE_NOTHING;
	    (ಶೂನ್ಯ)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    ಯದಿ (!(done & DONE_ACTION))
		ಭಙ್ಗ;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    ಶೂನ್ಯ
newFoldLevel(ಶೂನ್ಯ)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    ಯದಿ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    checkupdate(wp);
    ಯದಿ (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_data;
	ಪ್ರತಿ (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    ಶೂನ್ಯ
foldCheckClose(ಶೂನ್ಯ)
{
    ಯದಿ (*p_fcl == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    ಯದಿ (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(ಪೂರ್ಣಾಙ್ಕ)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
checkCloseRec(garray_T *gap, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ level)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;

    fp = (fold_T *)gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	ಯದಿ (fp[i].fd_flags == FD_OPEN)
	{
	    ಯದಿ (level <= 0 && (lnum < fp[i].fd_ತುದಿ
				      || lnum >= fp[i].fd_ತುದಿ + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    ಅನ್ಯಥಾ
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_ತುದಿ,
								   level - 1);
	}
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
foldManualAllowed(ಪೂರ್ಣಾಙ್ಕ create)
{
    ಯದಿ (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    ಅನ್ಯಥಾ
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    ಶೂನ್ಯ
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		cont;
    ಪೂರ್ಣಾಙ್ಕ		use_level = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		closed = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    ಯದಿ (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    ಯದಿ (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	ಪ್ರತ್ಯಾಗಮ;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    ಯದಿ (gap->ga_len == 0)
	i = 0;
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (!foldFind(gap, start_rel, &fp))
		ಭಙ್ಗ;
	    ಯದಿ (fp->fd_ತುದಿ + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_ತುದಿ;
		end_rel -= fp->fd_ತುದಿ;
		ಯದಿ (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    ಯದಿ (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		ಅನ್ಯಥಾ ಯದಿ (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    ಅನ್ಯಥಾ
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (gap->ga_len == 0)
	    i = 0;
	ಅನ್ಯಥಾ
	    i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data);
    }

    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    fp = (fold_T *)gap->ga_data + i;
    ga_init2(&fold_ga, ಪರಿಮಾಣ(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    ಪ್ರತಿ (cont = 0; i + cont < gap->ga_len; ++cont)
	ಯದಿ (fp[cont].fd_ತುದಿ > end_rel)
	    ಭಙ್ಗ;
    ಯದಿ (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	ಯದಿ (start_rel > fp->fd_ತುದಿ)
	    start_rel = fp->fd_ತುದಿ;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	ಯದಿ (end_rel < fp[cont - 1].fd_ತುದಿ + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_ತುದಿ + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_data, fp, ಪರಿಮಾಣ(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	ಪ್ರತಿ (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_data)[j].fd_ತುದಿ -= start_rel;
    }
    // Move remaining entries to after the new fold.
    ಯದಿ (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_data + i,
		ಪರಿಮಾಣ(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_ತುದಿ = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    ಯದಿ (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    ಯದಿ (!use_level)
	curwin->w_fold_manual = true;
    fp->fd_flags = FD_CLOSED;
    fp->fd_small = MAYBE;

    // redraw
    changed_window_setting();
}

// deleteFold() {{{2
/*
 * Delete a fold at line "start" in the current window.
 * When "end" is not 0, delete all folds from "start" to "end".
 * When "recursive" is TRUE delete recursively.
 */
    ಶೂನ್ಯ
deleteFold(
    linenr_T	start,
    linenr_T	end,
    ಪೂರ್ಣಾಙ್ಕ		recursive,
    ಪೂರ್ಣಾಙ್ಕ		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = ರಿಕ್ತ;
    linenr_T	found_off = 0;
    ಪೂರ್ಣಾಙ್ಕ		use_level;
    ಪೂರ್ಣಾಙ್ಕ		maybe_small = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    ಪೂರ್ಣಾಙ್ಕ		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    ಯಾವತ್ (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = ರಿಕ್ತ;
	lnum_off = 0;
	use_level = FALSE;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (!foldFind(gap, lnum - lnum_off, &fp))
		ಭಙ್ಗ;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    ಯದಿ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		ಭಙ್ಗ;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ತುದಿ;
	    ++level;
	}
	ಯದಿ (found_ga == ರಿಕ್ತ)
	{
	    ++lnum;
	}
	ಅನ್ಯಥಾ
	{
	    lnum = found_fp->fd_ತುದಿ + found_fp->fd_len + found_off;

	    ಯದಿ (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (ಪೂರ್ಣಾಙ್ಕ)(found_fp - (fold_T *)found_ga->ga_data), recursive);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (first_lnum > found_fp->fd_ತುದಿ + found_off)
		    first_lnum = found_fp->fd_ತುದಿ + found_off;
		ಯದಿ (last_lnum < lnum)
		    last_lnum = lnum;
		ಯದಿ (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    ಯದಿ (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	ಯದಿ (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    ಅನ್ಯಥಾ
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    ಯದಿ (last_lnum > 0)
	changed_ಸಾಲುಗಳು(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
foldUpdate(win_T *wp, linenr_T ತುದಿ, linenr_T bot)
{
    fold_T	*fp;

    ಯದಿ (disable_fold_update > 0)
	ಪ್ರತ್ಯಾಗಮ;
# ifdef FEAT_DIFF
    ಯದಿ (need_diff_redraw)
	// will update later
	ಪ್ರತ್ಯಾಗಮ;
# endif

    ಯದಿ (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = ತುದಿ;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	ಯದಿ (ತುದಿ > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = ತುದಿ;
	}
	(ಶೂನ್ಯ)foldFind(&wp->w_folds, maybe_small_start, &fp);
	ಯಾವತ್ (fp < (fold_T *)wp->w_folds.ga_data + wp->w_folds.ga_len
		&& fp->fd_ತುದಿ <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    ಯದಿ (foldmethodIsIndent(wp)
	    || foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	ಪೂರ್ಣಾಙ್ಕ save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, ತುದಿ, bot);
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
    ಶೂನ್ಯ
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
    ಪೂರ್ಣಾಙ್ಕ
foldMoveTo(
    ಪೂರ್ಣಾಙ್ಕ		updown,
    ಪೂರ್ಣಾಙ್ಕ		dir,	    // FORWARD or BACKWARD
    long	ಎಣಿಕೆ)
{
    long	n;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		use_level;
    ಪೂರ್ಣಾಙ್ಕ		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		level;
    ಪೂರ್ಣಾಙ್ಕ		last;

    checkupdate(curwin);

    // Repeat "count" times.
    ಪ್ರತಿ (n = 0; n < ಎಣಿಕೆ; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	ಯದಿ (gap->ga_len == 0)
	    ಭಙ್ಗ;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		ಯದಿ (!updown || gap->ga_len == 0)
		    ಭಙ್ಗ;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		ಯದಿ (dir == FORWARD)
		{
		    ಯದಿ (fp - (fold_T *)gap->ga_data >= gap->ga_len)
			ಭಙ್ಗ;
		    --fp;
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (fp == (fold_T *)gap->ga_data)
			ಭಙ್ಗ;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    ಯದಿ (!last)
	    {
		// Check if this fold is closed.
		ಯದಿ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		ಯದಿ (last && !updown)
		    ಭಙ್ಗ;
	    }

	    ಯದಿ (updown)
	    {
		ಯದಿ (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    ಯದಿ (fp + 1 - (fold_T *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_ತುದಿ + lnum_off;
			ಯದಿ (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // to end of previous fold if there is one
		    ಯದಿ (fp > (fold_T *)gap->ga_data)
		    {
			lnum = fp[-1].fd_ತುದಿ + lnum_off + fp[-1].fd_len - 1;
			ಯದಿ (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		ಯದಿ (dir == FORWARD)
		{
		    lnum = fp->fd_ತುದಿ + lnum_off + fp->fd_len - 1;
		    ಯದಿ (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		ಅನ್ಯಥಾ
		{
		    lnum = fp->fd_ತುದಿ + lnum_off;
		    ಯದಿ (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    ಯದಿ (last)
		ಭಙ್ಗ;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ತುದಿ;
	    ++level;
	}
	ಯದಿ (lnum_found != curwin->w_cursor.lnum)
	{
	    ಯದಿ (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    ಶೂನ್ಯ
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, ಪರಿಮಾಣ(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_wl_entry(win_T *win, linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < win->w_ಸಾಲುಗಳು_valid; ++i)
	ಯದಿ (win->w_ಸಾಲುಗಳು[i].wl_valid)
	{
	    ಯದಿ (lnum < win->w_ಸಾಲುಗಳು[i].wl_lnum)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (lnum <= win->w_ಸಾಲುಗಳು[i].wl_lastlnum)
		ಪ್ರತ್ಯಾಗಮ i;
	}
    ಪ್ರತ್ಯಾಗಮ -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    ಶೂನ್ಯ
foldAdjustVisual(ಶೂನ್ಯ)
{
    pos_T	*start, *end;

    ಯದಿ (!VIsual_active || !hasAnyFolding(curwin))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    ಅನ್ಯಥಾ
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    ಯದಿ (hasFolding(start->lnum, &start->lnum, ರಿಕ್ತ))
	start->col = 0;

    ಯದಿ (!hasFolding(end->lnum, ರಿಕ್ತ, &end->lnum))
	ಪ್ರತ್ಯಾಗಮ;

    end->col = ml_get_len(end->lnum);
    ಯದಿ (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    ಯದಿ (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    ಶೂನ್ಯ
foldAdjustCursor(ಶೂನ್ಯ)
{
    (ಶೂನ್ಯ)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ರಿಕ್ತ);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    ಶೂನ್ಯ
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    ಯದಿ (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    from_p = (fold_T *)from->ga_data;
    to_p = (fold_T *)to->ga_data;

    ಪ್ರತಿ (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_ತುದಿ = from_p->fd_ತುದಿ;
	to_p->fd_len = from_p->fd_len;
	to_p->fd_flags = from_p->fd_flags;
	to_p->fd_small = from_p->fd_small;
	cloneFoldGrowArray(&from_p->fd_nested, &to_p->fd_nested);
	++to->ga_len;
	++from_p;
	++to_p;
    }
}

// foldFind() {{{2
/*
 * Search for line "lnum" in folds of growarray "gap".
 * Set "*fpp" to the fold struct for the fold that contains "lnum" or
 * the first fold below it (careful: it can be beyond the end of the array!).
 * Returns FALSE when there is no fold that contains "lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	ಕೆಳಗಿನ, ಮೇಲಿನ;
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (gap->ga_len == 0)
    {
	*fpp = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_data;
    ಕೆಳಗಿನ = 0;
    ಮೇಲಿನ = gap->ga_len - 1;
    ಯಾವತ್ (ಕೆಳಗಿನ <= ಮೇಲಿನ)
    {
	i = (ಕೆಳಗಿನ + ಮೇಲಿನ) / 2;
	ಯದಿ (fp[i].fd_ತುದಿ > lnum)
	    // fold below lnum, adjust high
	    ಮೇಲಿನ = i - 1;
	ಅನ್ಯಥಾ ಯದಿ (fp[i].fd_ತುದಿ + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    ಕೆಳಗಿನ = i + 1;
	ಅನ್ಯಥಾ
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    *fpp = fp + ಕೆಳಗಿನ;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    ಪೂರ್ಣಾಙ್ಕ		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (!foldFind(gap, lnum_rel, &fp))
	    ಭಙ್ಗ;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_ತುದಿ;
	++level;
    }

    ಪ್ರತ್ಯಾಗಮ level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
checkupdate(win_T *wp)
{
    ಯದಿ (!wp->w_foldinvalid)
	ಪ್ರತ್ಯಾಗಮ;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
setFoldRepeat(linenr_T lnum, long ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ do_open)
{
    ಪೂರ್ಣಾಙ್ಕ		done;
    long	n;

    ಪ್ರತಿ (n = 0; n < ಎಣಿಕೆ; ++n)
    {
	done = DONE_NOTHING;
	(ಶೂನ್ಯ)setManualFold(lnum, do_open, FALSE, &done);
	ಯದಿ (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    ಯದಿ (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    ಭಙ್ಗ;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    ಸ್ಥಿರ linenr_T
setManualFold(
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		opening,    // TRUE when opening, FALSE when closing
    ಪೂರ್ಣಾಙ್ಕ		recurse,    // TRUE when closing/opening recursive
    ಪೂರ್ಣಾಙ್ಕ		*donep)
{
# ifdef FEAT_DIFF
    ಯದಿ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		ಯದಿ (dlnum != 0)
		    (ಶೂನ್ಯ)setManualFoldWin(wp, dlnum, opening, recurse, ರಿಕ್ತ);
	    }
	}
    }
# endif

    ಪ್ರತ್ಯಾಗಮ setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    ಸ್ಥಿರ linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		opening,    // TRUE when opening, FALSE when closing
    ಪೂರ್ಣಾಙ್ಕ		recurse,    // TRUE when closing/opening recursive
    ಪೂರ್ಣಾಙ್ಕ		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		j;
    ಪೂರ್ಣಾಙ್ಕ		level = 0;
    ಪೂರ್ಣಾಙ್ಕ		use_level = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	next = MAXLNUM;
    linenr_T	off = 0;
    ಪೂರ್ಣಾಙ್ಕ		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    ಯದಿ (fp != ರಿಕ್ತ && fp < (fold_T *)gap->ga_data + gap->ga_len)
		next = fp->fd_ತುದಿ + off;
	    ಭಙ್ಗ;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	ಯದಿ (fp + 1 < (fold_T *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_ತುದಿ + off;

	// Change from level-dependent folding to manual.
	ಯದಿ (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    ಯದಿ (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    ಅನ್ಯಥಾ
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_data;
	    ಪ್ರತಿ (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	ಯದಿ (!opening && recurse)
	{
	    ಯದಿ (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    ಯದಿ (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		ಯದಿ (recurse)
		    foldOpenNested(fp);
	    }
	    ಭಙ್ಗ;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_ತುದಿ;
	off += fp->fd_ತುದಿ;
	++level;
    }
    ಯದಿ (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	ಯದಿ (!opening && found != ರಿಕ್ತ)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	ಯದಿ (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    ಅನ್ಯಥಾ ಯದಿ (donep == ರಿಕ್ತ && wp == curwin)
	emsg(_(e_no_fold_found));

    ಯದಿ (donep != ರಿಕ್ತ)
	*donep |= done;

    ಪ್ರತ್ಯಾಗಮ next;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldOpenNested(fold_T *fpr)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_data;
    ಪ್ರತಿ (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
deleteFoldEntry(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ recursive)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_data + idx;
    ಯದಿ (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	ಯದಿ (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, ಪರಿಮಾಣ(fold_T) * (gap->ga_len - idx));
    }
    ಅನ್ಯಥಾ
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	ಯದಿ (ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_data + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_data;
	    ಪ್ರತಿ (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_ತುದಿ += fp->fd_ತುದಿ;
		ಯದಿ (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		ಯದಿ (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    ಯದಿ (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  ಪರಿಮಾಣ(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(ಪರಿಮಾಣ(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    ಶೂನ್ಯ
deleteFoldRecurse(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_T *)(gap->ga_data))[i].fd_nested));
    ga_clear(gap);
}

// foldMarkAdjust() {{{2
/*
 * Update line numbers of folds for inserted/deleted lines.
 *
 * We are adjusting the folds in the range from line1 til line2,
 * make sure that line2 does not get smaller than line1
 */
    ಶೂನ್ಯ
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    ಯದಿ (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    ಯದಿ (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    ಯದಿ ((State & MODE_ಸೇರಿಸು) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    ಸ್ಥಿರ ಶೂನ್ಯ
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    linenr_T	last;
    linenr_T	ತುದಿ;

    ಯದಿ (gap->ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    ಯದಿ ((State & MODE_ಸೇರಿಸು) && amount == (linenr_T)1 && line2 == MAXLNUM)
	ತುದಿ = line1 + 1;
    ಅನ್ಯಥಾ
	ತುದಿ = line1;

    // Find the fold containing or just below "line1".
    (ಶೂನ್ಯ)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    ಪ್ರತಿ (i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_ತುದಿ + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	ಯದಿ (last < line1)
	    ಅನುವರ್ತ;

	// 6. fold below line2: only adjust for amount_after
	ಯದಿ (fp->fd_ತುದಿ > line2)
	{
	    ಯದಿ (amount_after == 0)
		ಭಙ್ಗ;
	    fp->fd_ತುದಿ += amount_after;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (fp->fd_ತುದಿ >= ತುದಿ && last <= line2)
	    {
		// 4. fold completely contained in range
		ಯದಿ (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		ಅನ್ಯಥಾ
		    fp->fd_ತುದಿ += amount;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (fp->fd_ತುದಿ < ತುದಿ)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ತುದಿ,
				  line2 - fp->fd_ತುದಿ, amount, amount_after);
		    ಯದಿ (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			ಯದಿ (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_ತುದಿ;
			ಅನ್ಯಥಾ
			    fp->fd_len += amount;
		    }
		    ಅನ್ಯಥಾ
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    ಯದಿ (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ತುದಿ,
				  amount,
				  amount_after + (fp->fd_ತುದಿ - ತುದಿ));
			fp->fd_len -= line2 - fp->fd_ತುದಿ + 1;
			fp->fd_ತುದಿ = line1;
		    }
		    ಅನ್ಯಥಾ
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ತುದಿ,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_ತುದಿ += amount;
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
    ಪೂರ್ಣಾಙ್ಕ
getDeepestNesting(ಶೂನ್ಯ)
{
    checkupdate(curwin);
    ಪ್ರತ್ಯಾಗಮ getDeepestNestingRecurse(&curwin->w_folds);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getDeepestNestingRecurse(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		level;
    ಪೂರ್ಣಾಙ್ಕ		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	ಯದಿ (level > maxlevel)
	    maxlevel = level;
    }

    ಪ್ರತ್ಯಾಗಮ maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_closed(
    win_T	*win,
    fold_T	*fp,
    ಪೂರ್ಣಾಙ್ಕ		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    ಪೂರ್ಣಾಙ್ಕ		level,		    // folding depth
    ಪೂರ್ಣಾಙ್ಕ		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    ಪೂರ್ಣಾಙ್ಕ		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    ಯದಿ (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	ಯದಿ (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    ಯದಿ (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    ಯದಿ (closed)
    {
	ಯದಿ (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	ಯದಿ (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (fp->fd_small != MAYBE)
	ಪ್ರತ್ಯಾಗಮ;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    ಯದಿ (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    ಅನ್ಯಥಾ
    {
	ಎಣಿಕೆ = 0;
	ಪ್ರತಿ (n = 0; n < fp->fd_len; ++n)
	{
	    ಎಣಿಕೆ += plines_win_nofold(wp, fp->fd_ತುದಿ + lnum_off + n);
	    ಯದಿ (ಎಣಿಕೆ > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
setSmallMaybe(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldCreateMarkers(linenr_T start, linenr_T end)
{
    ಯದಿ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಪ್ರತ್ಯಾಗಮ;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_ಸಾಲುಗಳು(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldAddMarker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಙ್ಕ markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((ವರ್ಣ *)curbuf->b_p_cms, "%s");
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    ಸಾಲು = ml_get(lnum);
    ಸಾಲು_len = ml_get_len(lnum);

    ಯದಿ (u_save(lnum - 1, lnum + 1) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    // Check if the line ends with an unclosed comment
    (ಶೂನ್ಯ)skip_comment(ಸಾಲು, FALSE, FALSE, &ಸಾಲು_is_comment);
    newline = alloc(ಸಾಲು_len + markerlen + STRLEN(cms) + 1);
    ಯದಿ (newline == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    STRCPY(newline, ಸಾಲು);
    // Append the marker to the end of the line
    ಯದಿ (p == ರಿಕ್ತ || ಸಾಲು_is_comment)
	vim_strncpy(newline + ಸಾಲು_len, marker, markerlen);
    ಅನ್ಯಥಾ
    {
	STRCPY(newline + ಸಾಲು_len, cms);
	STRNCPY(newline + ಸಾಲು_len + (p - cms), marker, markerlen);
	STRCPY(newline + ಸಾಲು_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
deleteFoldMarkers(
    fold_T	*fp,
    ಪೂರ್ಣಾಙ್ಕ		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (recursive)
	ಪ್ರತಿ (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_ತುದಿ);
    foldDelMarker(fp->fd_ತುದಿ + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_ತುದಿ + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldDelMarker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಙ್ಕ markerlen)
{
    char_u	*ಸಾಲು;
    char_u	*newline;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;
    ಸಾಲು = ml_get(lnum);
    ಪ್ರತಿ (p = ಸಾಲು; *p != NUL; ++p)
	ಯದಿ (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    ಯದಿ (VIM_ISDIGIT(p[len]))
		++len;
	    ಯದಿ (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((ವರ್ಣ *)cms, "%s");
		ಯದಿ (cms2 != ರಿಕ್ತ && p - ಸಾಲು >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(cms) - 2;
		}
	    }
	    ಯದಿ (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		ಯದಿ (newline != ರಿಕ್ತ)
		{
		    STRNCPY(newline, ಸಾಲು, p - ಸಾಲು);
		    STRCPY(newline + (p - ಸಾಲು), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    ಭಙ್ಗ;
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
    char_u	*text = ರಿಕ್ತ;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    got_fdt_error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    save_did_emsg = did_emsg;
    ಸ್ಥಿರ win_T    *last_wp = ರಿಕ್ತ;
    ಸ್ಥಿರ linenr_T last_lnum = 0;

    ಯದಿ (last_wp != wp || last_wp == ರಿಕ್ತ
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    ಯದಿ (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    ಯದಿ (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	ಪೂರ್ಣಾಙ್ಕ	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	ಯದಿ (level > (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(dashes) - 1)
	    level = (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	ಯದಿ (!got_fdt_error)
	{
	    win_T   *save_curwin = curwin;
	    sctx_T  saved_sctx = current_sctx;

	    curwin = wp;
	    curbuf = wp->w_buffer;
	    current_sctx = wp->w_p_script_ctx[WV_FDT];

	    ++emsg_off; // handle exceptions, but don't display errors
	    text = eval_to_string_safe(wp->w_p_fdt,
		       was_set_insecurely(wp, (char_u *)"foldtext", OPT_LOCAL),
		       TRUE, TRUE);
	    --emsg_off;

	    ಯದಿ (text == ರಿಕ್ತ || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, ರಿಕ್ತ, -1);

	ಯದಿ (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	ಯದಿ (text != ರಿಕ್ತ)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    ಪ್ರತಿ (p = text; *p != NUL; ++p)
	    {
		ಪೂರ್ಣಾಙ್ಕ	len;

		ಯದಿ (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    ಯದಿ (!vim_isprintc((*mb_ptr2char)(p)))
			ಭಙ್ಗ;
		    p += len - 1;
		}
		ಅನ್ಯಥಾ
		    ಯದಿ (*p == TAB)
			*p = ' ';
		    ಅನ್ಯಥಾ ಯದಿ (ptr2cells(p) > 1)
			ಭಙ್ಗ;
	    }
	    ಯದಿ (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    ಯದಿ (text == ರಿಕ್ತ)
# endif
    {
	long ಎಣಿಕೆ = (long)(lnume - lnum + 1);

	vim_snprintf((ವರ್ಣ *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", ಎಣಿಕೆ),
		     ಎಣಿಕೆ);
	text = buf;
    }
    ಪ್ರತ್ಯಾಗಮ text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    ಪೂರ್ಣಾಙ್ಕ		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    ಪೂರ್ಣಾಙ್ಕ		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		did1 = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(cms_start);
    ಯಾವತ್ (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((ವರ್ಣ *)cms_start, "%s");
    ಯದಿ (cms_end != ರಿಕ್ತ)
    {
	cms_elen = cms_slen - (ಪೂರ್ಣಾಙ್ಕ)(cms_end - cms_start);
	cms_slen = (ಪೂರ್ಣಾಙ್ಕ)(cms_end - cms_start);

	// exclude white space before "%s"
	ಯಾವತ್ (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (ಪೂರ್ಣಾಙ್ಕ)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    ಪ್ರತಿ (s = str; *s != NUL; )
    {
	len = 0;
	ಯದಿ (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	ಯದಿ (len > 0)
	{
	    ಯದಿ (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    ಪ್ರತಿ (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    ಯದಿ (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (ಪೂರ್ಣಾಙ್ಕ)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (cms_end != ರಿಕ್ತ)
	{
	    ಯದಿ (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	ಯದಿ (len != 0)
	{
	    ಯಾವತ್ (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	ಅನ್ಯಥಾ
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    ಪೂರ್ಣಾಙ್ಕ		lvl;		// current level (-1 for undefined)
    ಪೂರ್ಣಾಙ್ಕ		lvl_next;	// level used for next line
    ಪೂರ್ಣಾಙ್ಕ		start;		// number of folds that are forced to start at
				// this line.
    ಪೂರ್ಣಾಙ್ಕ		end;		// level of fold that is forced to end below
				// this line
    ಪೂರ್ಣಾಙ್ಕ		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fold_changed;

// Function declarations. {{{2
ಸ್ಥಿರ linenr_T foldUpdateIEMSRecurse(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ level, linenr_T startlnum, fline_T *flp, ಶೂನ್ಯ (*getlevel)(fline_T *), linenr_T bot, ಪೂರ್ಣಾಙ್ಕ topflags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fold_ಸೇರಿಸು(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ i);
ಸ್ಥಿರ ಶೂನ್ಯ foldSplit(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ i, linenr_T ತುದಿ, linenr_T bot);
ಸ್ಥಿರ ಶೂನ್ಯ foldRemove(garray_T *gap, linenr_T ತುದಿ, linenr_T bot);
ಸ್ಥಿರ ಶೂನ್ಯ foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
ಸ್ಥಿರ ಶೂನ್ಯ foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
ಸ್ಥಿರ ಶೂನ್ಯ foldlevelDiff(fline_T *flp);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ foldlevel_ಅಭಿವ್ಯಕ್ತಿ(fline_T *flp);
ಸ್ಥಿರ ಶೂನ್ಯ foldlevelMarker(fline_T *flp);
ಸ್ಥಿರ ಶೂನ್ಯ foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldUpdateIEMS(win_T *wp, linenr_T ತುದಿ, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    ಶೂನ್ಯ	(*getlevel)(fline_T *);
    ಪೂರ್ಣಾಙ್ಕ		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    ಯದಿ (invalid_ತುದಿ != (linenr_T)0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (wp->w_foldinvalid)
    {
	// Need to update all folds.
	ತುದಿ = 1;
	bot = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    ಯದಿ (foldmethodIsDiff(wp))
    {
	ಯದಿ (ತುದಿ > diff_context)
	    ತುದಿ -= diff_context;
	ಅನ್ಯಥಾ
	    ತುದಿ = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    ಯದಿ (ತುದಿ > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ತುದಿ = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_ತುದಿ = ತುದಿ;
    invalid_bot = bot;

    ಯದಿ (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	ಯದಿ (ತುದಿ > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, ತುದಿ - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = ತುದಿ - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    ಯದಿ (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_next);
	    ಅನ್ಯಥಾ
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = ತುದಿ;
	getlevel(&fline);
    }
    ಅನ್ಯಥಾ
    {
	fline.lnum = ತುದಿ;
	ಯದಿ (foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(wp))
	{
	    getlevel = foldlevel_ಅಭಿವ್ಯಕ್ತಿ;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    ಯದಿ (ತುದಿ > 1)
		--fline.lnum;
	}
	ಅನ್ಯಥಾ ಯದಿ (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	ಅನ್ಯಥಾ ಯದಿ (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	ಅನ್ಯಥಾ
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    ಯದಿ (ತುದಿ > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	ಪ್ರತಿ ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    ಯದಿ (fline.lvl >= 0)
		ಭಙ್ಗ;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    ಯದಿ (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	ಯಾವತ್ (current_fdl < fline.lvl)
	{
	    ಯದಿ (!foldFind(gap, lnum_rel, &fpn))
		ಭಙ್ಗ;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_ತುದಿ;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_ತುದಿ;
	}
	ಯದಿ (fpn != ರಿಕ್ತ && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    ಯದಿ (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    ಯದಿ (start > end && end < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	end = start;
    ಯಾವತ್ (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	ಯದಿ (fline.lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;
	ಯದಿ (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    ಯದಿ (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_ಅಭಿವ್ಯಕ್ತಿ)
		ಭಙ್ಗ;
	    ಯದಿ ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_ತುದಿ + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_ತುದಿ < fline.lnum))
		end = fp->fd_ತುದಿ + fp->fd_len - 1;
	    ಅನ್ಯಥಾ ಯದಿ (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	ಯದಿ (fline.lvl > 0)
	{
	    invalid_ತುದಿ = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (fline.lnum == wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    ಯದಿ (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    ಯದಿ (end != bot)
	redraw_win_range_later(wp, ತುದಿ, end);

    invalid_ತುದಿ = (linenr_T)0;
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
    ಸ್ಥಿರ linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    ಪೂರ್ಣಾಙ್ಕ		level,
    linenr_T	startlnum,
    fline_T	*flp,
    ಶೂನ್ಯ	(*getlevel)(fline_T *),
    linenr_T	bot,
    ಪೂರ್ಣಾಙ್ಕ		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ರಿಕ್ತ;
    fold_T	*fp2;
    ಪೂರ್ಣಾಙ್ಕ		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		finish = FALSE;
    linenr_T	linecount = flp->wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - flp->off;
    ಪೂರ್ಣಾಙ್ಕ		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    ಯದಿ (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(ಶೂನ್ಯ)foldFind(gap, startlnum - 1, &fp);
	ಯದಿ (fp != ರಿಕ್ತ && (fp >= ((fold_T *)gap->ga_data) + gap->ga_len
						   || fp->fd_ತುದಿ >= startlnum))
	    fp = ರಿಕ್ತ;
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
    ಯಾವತ್ (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	ಸಾಲು_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	ಯದಿ (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	ಯದಿ (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	ಯದಿ (flp->lnum > bot && !finish && fp != ರಿಕ್ತ)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    ಯದಿ (getlevel != foldlevelMarker
		    && getlevel != foldlevel_ಅಭಿವ್ಯಕ್ತಿ
		    && getlevel != foldlevelSyntax)
		ಭಙ್ಗ;
	    i = 0;
	    fp2 = fp;
	    ಯದಿ (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_ತುದಿ;
		ಯಾವತ್ (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_ತುದಿ;
		}
	    }
	    ಯದಿ (lvl < level + i)
	    {
		(ಶೂನ್ಯ)foldFind(&fp->fd_nested, flp->lnum - fp->fd_ತುದಿ, &fp2);
		ಯದಿ (fp2 != ರಿಕ್ತ)
		    bot = fp2->fd_ತುದಿ + fp2->fd_len - 1 + fp->fd_ತುದಿ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (fp->fd_ತುದಿ + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	ಯದಿ (fp == ರಿಕ್ತ
		&& (lvl != level
		    || flp->lnum_save >= bot
		    || flp->start != 0
		    || flp->had_end <= MAX_LEVEL
		    || flp->lnum == linecount))
	{
	    /*
	     * Remove or update folds that have lines between startlnum and
	     * firstlnum.
	     */
	    ಯಾವತ್ (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		ಯದಿ (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		ಅನ್ಯಥಾ
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		ಯದಿ (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && fp->fd_ತುದಿ <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_ತುದಿ < flp->lnum)
				|| (lvl >= level
					   && fp->fd_ತುದಿ <= flp->lnum_save)))))
		{
		    ಯದಿ (fp->fd_ತುದಿ + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			ಯದಿ (fp->fd_ತುದಿ == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			ಅನ್ಯಥಾ ಯದಿ (fp->fd_ತುದಿ >= startlnum)
			{
			    ಯದಿ (fp->fd_ತುದಿ > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_ತುದಿ - firstlnum), 0L);
			    ಅನ್ಯಥಾ
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_ತುದಿ - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_ತುದಿ - firstlnum));
			    fp->fd_len += fp->fd_ತುದಿ - firstlnum;
			    fp->fd_ತುದಿ = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			ಅನ್ಯಥಾ ಯದಿ ((flp->start != 0 && lvl == level)
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
			    ಯದಿ (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    ಅನ್ಯಥಾ
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_ತುದಿ,
						      breakend - fp->fd_ತುದಿ);
			    i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_data + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    ಯದಿ (getlevel == foldlevelMarker
				    || getlevel == foldlevel_ಅಭಿವ್ಯಕ್ತಿ
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			ಯದಿ (fp->fd_ತುದಿ == startlnum && concat)
			{
			    i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data);
			    ಯದಿ (i != 0)
			    {
				fp2 = fp - 1;
				ಯದಿ (fp2->fd_ತುದಿ + fp2->fd_len == fp->fd_ತುದಿ)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			ಭಙ್ಗ;
		    }
		    ಯದಿ (fp->fd_ತುದಿ >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data), TRUE);
		    }
		    ಅನ್ಯಥಾ
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_ತುದಿ;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    ಯದಿ (gap->ga_len == 0)
			i = 0;
		    ಅನ್ಯಥಾ
			i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data);
		    ಯದಿ (fold_ಸೇರಿಸು(gap, i) != OK)
			ಪ್ರತ್ಯಾಗಮ bot;
		    fp = (fold_T *)gap->ga_data + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_ತುದಿ = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    ಯದಿ (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (i <= 0)
		    {
			fp->fd_flags = topflags;
			ಯದಿ (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    ಅನ್ಯಥಾ
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    ಯದಿ (getlevel == foldlevelMarker
			    || getlevel == foldlevel_ಅಭಿವ್ಯಕ್ತಿ
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	}

	ಯದಿ (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    ಭಙ್ಗ;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	ಯದಿ (lvl > level && fp != ರಿಕ್ತ)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    ಯದಿ (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_ತುದಿ;
	    flp->off += fp->fd_ತುದಿ;
	    i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_ತುದಿ, flp, getlevel,
					      bot - fp->fd_ತುದಿ, fp->fd_flags);
	    fp = (fold_T *)gap->ga_data + i;
	    flp->lnum += fp->fd_ತುದಿ;
	    flp->lnum_save += fp->fd_ತುದಿ;
	    flp->off -= fp->fd_ತುದಿ;
	    bot += fp->fd_ತುದಿ;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    ಯಾವತ್ (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		ಯದಿ (++flp->lnum > linecount)
		    ಭಙ್ಗ;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		ಯದಿ (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    ಭಙ್ಗ;
	    }
	    prev_lnum = 0;
	    ಯದಿ (flp->lnum > linecount)
		ಭಙ್ಗ;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    ಯದಿ (fp == ರಿಕ್ತ)	// only happens when got_int is set
	ಪ್ರತ್ಯಾಗಮ bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    ಯದಿ (fp->fd_len < flp->lnum - fp->fd_ತುದಿ)
    {
	fp->fd_len = flp->lnum - fp->fd_ತುದಿ;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (fp->fd_ತುದಿ + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_ತುದಿ + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_ತುದಿ,
						  flp->lnum - 1 - fp->fd_ತುದಿ);

    ಯದಿ (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	ಯದಿ (fp->fd_len != flp->lnum - fp->fd_ತುದಿ)
	{
	    ಯದಿ (fp->fd_ತುದಿ + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		ಯದಿ (getlevel == foldlevelMarker
			|| getlevel == foldlevel_ಅಭಿವ್ಯಕ್ತಿ
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_ತುದಿ + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_ತುದಿ;
		}
		ಅನ್ಯಥಾ
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_data + i;
		}
	    }
	    ಅನ್ಯಥಾ
		fp->fd_len = flp->lnum - fp->fd_ತುದಿ;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    ಪ್ರತಿ (;;)
    {
	fp2 = fp + 1;
	ಯದಿ (fp2 >= (fold_T *)gap->ga_data + gap->ga_len
						  || fp2->fd_ತುದಿ > flp->lnum)
	    ಭಙ್ಗ;
	ಯದಿ (fp2->fd_ತುದಿ + fp2->fd_len > flp->lnum)
	{
	    ಯದಿ (fp2->fd_ತುದಿ < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_ತುದಿ - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_ತುದಿ - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_ತುದಿ;
		fp2->fd_ತುದಿ = flp->lnum;
		fold_changed = TRUE;
	    }

	    ಯದಿ (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    ಭಙ್ಗ;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (ಪೂರ್ಣಾಙ್ಕ)(fp2 - (fold_T *)gap->ga_data), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    ಯದಿ (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    ಪ್ರತ್ಯಾಗಮ bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fold_ಸೇರಿಸು(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ i)
{
    fold_T	*fp;

    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    fp = (fold_T *)gap->ga_data + i;
    ಯದಿ (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, ಪರಿಮಾಣ(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, ಪರಿಮಾಣ(fold_T), 10);
    ಪ್ರತ್ಯಾಗಮ OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldSplit(
    garray_T	*gap,
    ಪೂರ್ಣಾಙ್ಕ		i,
    linenr_T	ತುದಿ,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // The fold continues below bot, need to split it.
    ಯದಿ (fold_ಸೇರಿಸು(gap, i + 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    fp = (fold_T *)gap->ga_data + i;
    fp[1].fd_ತುದಿ = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_ತುದಿ - fp->fd_ತುದಿ);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (ಶೂನ್ಯ)foldFind(gap1, bot + 1 - fp->fd_ತುದಿ, &fp2);
    ಯದಿ (fp2 != ರಿಕ್ತ)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)((fold_T *)gap1->ga_data + gap1->ga_len - fp2);
	ಯದಿ (len > 0 && ga_grow(gap2, len) == OK)
	{
	    ಪ್ರತಿ (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_data)[idx] = fp2[idx];
		((fold_T *)gap2->ga_data)[idx].fd_ತುದಿ
						  -= fp[1].fd_ತುದಿ - fp->fd_ತುದಿ;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = ತುದಿ - fp->fd_ತುದಿ;
    fold_changed = TRUE;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
foldRemove(garray_T *gap, linenr_T ತುದಿ, linenr_T bot)
{
    fold_T	*fp = ರಿಕ್ತ;

    ಯದಿ (bot < ತುದಿ)
	ಪ್ರತ್ಯಾಗಮ;		// nothing to do

    ಯಾವತ್ (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	ಯದಿ (foldFind(gap, ತುದಿ, &fp) && fp->fd_ತುದಿ < ತುದಿ)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, ತುದಿ - fp->fd_ತುದಿ, bot - fp->fd_ತುದಿ);
	    ಯದಿ (fp->fd_ತುದಿ + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data), ತುದಿ, bot);
	    }
	    ಅನ್ಯಥಾ
	    {
		// 2: truncate fold at "top".
		fp->fd_len = ತುದಿ - fp->fd_ತುದಿ;
	    }
	    fold_changed = TRUE;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (fp >= (fold_T *)(gap->ga_data) + gap->ga_len
		|| fp->fd_ತುದಿ > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    ಭಙ್ಗ;
	}
	ಯದಿ (fp->fd_ತುದಿ >= ತುದಿ)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    ಯದಿ (fp->fd_ತುದಿ + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_ತುದಿ),
			(linenr_T)MAXLNUM, (long)(fp->fd_ತುದಿ - bot - 1));
		fp->fd_len -= bot - fp->fd_ತುದಿ + 1;
		fp->fd_ತುದಿ = bot + 1;
		ಭಙ್ಗ;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (ಪೂರ್ಣಾಙ್ಕ)(fp - (fold_T *)gap->ga_data), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    ಸ್ಥಿರ ಶೂನ್ಯ
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *ಎಡ, *ಬಲ;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    ಪ್ರತಿ (; start < end; start++, end--)
    {
	ಎಡ = (fold_T *)gap->ga_data + start;
	ಬಲ = (fold_T *)gap->ga_data + end;
	tmp  = *ಎಡ;
	*ಎಡ = *ಬಲ;
	*ಬಲ = tmp;
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

    ಸ್ಥಿರ ಶೂನ್ಯ
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_ತುದಿ, MAXLNUM);
    fp->fd_len = end - fp->fd_ತುದಿ;
}

# define fold_end(fp) ((fp)->fd_ತುದಿ + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_data + (gap)->ga_len))
# define fold_index(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_data)))

    ಶೂನ್ಯ
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    ಪೂರ್ಣಾಙ್ಕ at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_index = 0;

    ಯದಿ (at_start)
    {
	ಯದಿ (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_ತುದಿ, line2 -
		    fp->fd_ತುದಿ, dest - fp->fd_ತುದಿ);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ ಯದಿ (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ತುದಿ, line2 -
		    fp->fd_ತುದಿ, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	ಅನ್ಯಥಾ
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    ಯದಿ (!valid_fold(fp, gap) || fp->fd_ತುದಿ > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (fp->fd_ತುದಿ > line2)
    {
	ಪ್ರತಿ (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_ತುದಿ -= range_len;

	ಯದಿ (valid_fold(fp, gap) && fp->fd_ತುದಿ <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_ತುದಿ -= range_len;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_ತುದಿ, dest -
		fp->fd_ತುದಿ, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_ತುದಿ += move_len;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_index(fp, gap);

    ಪ್ರತಿ (; valid_fold(fp, gap) && fp->fd_ತುದಿ <= dest; fp++)
    {
	ಯದಿ (fp->fd_ತುದಿ <= line2)
	{
	    // 1. 2. or 3.
	    ಯದಿ (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_ತುದಿ += move_len;
	    ಅನುವರ್ತ;
	}

	// Record index of the first fold after the moved range.
	ಯದಿ (move_end == 0)
	    move_end = fold_index(fp, gap);

	ಯದಿ (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_ತುದಿ -= range_len;
    }

    dest_index = fold_index(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    ಯದಿ (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	ಪ್ರತ್ಯಾಗಮ;
    foldReverseOrder(gap, (linenr_T)move_start, (linenr_T)dest_index - 1);
    foldReverseOrder(gap, (linenr_T)move_start,
			   (linenr_T)(move_start + dest_index - move_end - 1));
    foldReverseOrder(gap, (linenr_T)(move_start + dest_index - move_end),
						   (linenr_T)(dest_index - 1));
}
# undef fold_end
# undef valid_fold
# undef fold_index

// foldMerge() {{{2
/*
 * Merge two adjacent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjacent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    ಯದಿ (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    ಯದಿ (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	ಪ್ರತಿ (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_data)[gap1->ga_len]
					= ((fold_T *)gap2->ga_data)[idx];
	    ((fold_T *)gap1->ga_data)[gap1->ga_len].fd_ತುದಿ += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (ಪೂರ್ಣಾಙ್ಕ)(fp2 - (fold_T *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    ಯದಿ (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ರಿಕ್ತ)
    {
	// first and last line can't be undefined, use level 0
	ಯದಿ (lnum == 1 || lnum == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    flp->lvl = 0;
	ಅನ್ಯಥಾ
	    flp->lvl = -1;
    }
    ಅನ್ಯಥಾ
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_ಮೌಲ್ಯ(buf);
    ಯದಿ (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	ಯದಿ (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldlevelDiff(fline_T *flp)
{
    ಯದಿ (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    ಅನ್ಯಥಾ
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldlevel_ಅಭಿವ್ಯಕ್ತಿ(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# ಅನ್ಯಥಾ
    win_T	*win;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		c;
    linenr_T	lnum = flp->lnum + flp->off;
    ಪೂರ್ಣಾಙ್ಕ		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    ಯದಿ (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = ಕೀಲಿ_Typed;
    n = eval_foldexpr(flp->wp, &c);
    ಕೀಲಿ_Typed = save_keytyped;

    ವಿಕಲ್ಪ (c)
    {
	// "a1", "a2", .. : add to the fold level
	ಪಕ್ಷ 'a': ಯದಿ (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  ಭಙ್ಗ;

	// "s1", "s2", .. : subtract from the fold level
	ಪಕ್ಷ 's': ಯದಿ (flp->lvl >= 0)
		  {
		      ಯದಿ (n > flp->lvl)
			  flp->lvl_next = 0;
		      ಅನ್ಯಥಾ
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  ಭಙ್ಗ;

	// ">1", ">2", .. : start a fold with a certain level
	ಪಕ್ಷ '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  ಭಙ್ಗ;

	// "<1", "<2", .. : end a fold with a certain level
	ಪಕ್ಷ '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_next = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  ಭಙ್ಗ;

	// "=": No change in level
	ಪಕ್ಷ '=': flp->lvl_next = flp->lvl;
		  ಭಙ್ಗ;

	// "-1", "0", "1", ..: set fold level
	ಸ್ವಭಾವ:  ಯದಿ (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_next = flp->lvl;
		  ಅನ್ಯಥಾ
		      flp->lvl_next = n;
		  flp->lvl = n;
		  ಭಙ್ಗ;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    ಯದಿ (flp->lvl < 0)
    {
	ಯದಿ (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	ಯದಿ (lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    flp->lvl_next = 0;
    }

    curwin = win;
    curbuf = curwin->w_buffer;
# endif
}

// parseMarker() {{{2
/*
 * Parse 'foldmarker' and set "foldendmarker", "foldstartmarkerlen" and
 * "foldendmarkerlen".
 * Relies on the option value to have been checked for correctness already.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (ಪೂರ್ಣಾಙ್ಕ)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(foldendmarker);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    ಪೂರ್ಣಾಙ್ಕ		cstart;
    ಪೂರ್ಣಾಙ್ಕ		cend;
    ಪೂರ್ಣಾಙ್ಕ		start_lvl = flp->lvl;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    ಯಾವತ್ (*s)
    {
	ಯದಿ (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    ಯದಿ (VIM_ISDIGIT(*s))
	    {
		n = atoi((ವರ್ಣ *)s);
		ಯದಿ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    ಯದಿ (n <= start_lvl)
			flp->start = 1;
		    ಅನ್ಯಥಾ
			flp->start = n - start_lvl;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    ಯದಿ (VIM_ISDIGIT(*s))
	    {
		n = atoi((ವರ್ಣ *)s);
		ಯದಿ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    // never start a fold with an end marker
		    ಯದಿ (flp->lvl_next > start_lvl)
			flp->lvl_next = start_lvl;
		}
	    }
	    ಅನ್ಯಥಾ
		--flp->lvl_next;
	}
	ಅನ್ಯಥಾ
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    ಯದಿ (flp->lvl_next < 0)
	flp->lvl_next = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# ಅನ್ಯಥಾ
    linenr_T	lnum = flp->lnum + flp->off;
    ಪೂರ್ಣಾಙ್ಕ		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    ಯದಿ (lnum < flp->wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	ಯದಿ (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# ಯದಿ defined(FEAT_SESSION)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    ಪೂರ್ಣಾಙ್ಕ
put_folds(FILE *fd, win_T *wp)
{
    ಯದಿ (foldmethodIsManual(wp))
    {
	ಯದಿ (put_ಸಾಲು(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_ಸಾಲು(fd, "&fdl = &fdl") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    ಯದಿ (wp->w_fold_manual)
	ಪ್ರತ್ಯಾಗಮ put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    ಪ್ರತ್ಯಾಗಮ OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	ಯದಿ (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_ತುದಿ) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_ತುದಿ + off,
					fp->fd_ತುದಿ + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	++fp;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; i++)
    {
	ಯದಿ (fp->fd_flags != FD_LEVEL)
	{
	    ಯದಿ (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		ಯದಿ (fprintf(fd, ":%ld", fp->fd_ತುದಿ + off) < 0
			|| put_eol(fd) == FAIL
			|| put_ಸಾಲು(fd, "sil! normal! zo") == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_ತುದಿ)
			== FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// close the parent when needed
		ಯದಿ (fp->fd_flags == FD_CLOSED)
		{
		    ಯದಿ (put_fold_open_close(fd, fp, off) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_ತುದಿ);
		ಯದಿ ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		ಯದಿ (put_fold_open_close(fd, fp, off) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	++fp;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    ಯದಿ (fprintf(fd, ":%ld", fp->fd_ತುದಿ + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#ಯದಿ defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಯದಿ (wp != ರಿಕ್ತ && tp != ರಿಕ್ತ)
	{
	    ಯದಿ (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	ಅನ್ಯಥಾ
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = -2;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	lnum = tv_get_lnum(argvars);

    ಯದಿ (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಯದಿ (hasFoldingWin(wp, lnum, &first, &last, FALSE, ರಿಕ್ತ))
	{
	    ಯದಿ (end)
		rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)last;
	    ಅನ್ಯಥಾ
		rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)first;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
# endif
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

/*
 * "foldclosed()" function
 */
    ಶೂನ್ಯ
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    ಶೂನ್ಯ
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    ಯದಿ (invalid_ತುದಿ == (linenr_T)0)
	checkupdate(curwin);
    ಅನ್ಯಥಾ ಯದಿ (lnum == prev_lnum && prev_lnum_lvl >= 0)
	ಪ್ರತ್ಯಾಗಮ prev_lnum_lvl;
    ಅನ್ಯಥಾ ಯದಿ (lnum >= invalid_ತುದಿ && lnum <= invalid_bot)
	ಪ್ರತ್ಯಾಗಮ -1;

    // Return quickly when there is no folding at all in this window.
    ಯದಿ (!hasAnyFolding(curwin))
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    ಶೂನ್ಯ
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಯದಿ (wp != ರಿಕ್ತ && tp != ರಿಕ್ತ)
	{
	    ಯದಿ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    rettv->vval.v_ಸಂಖ್ಯೆ = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	ಅನ್ಯಥಾ
	    rettv->vval.v_ಸಂಖ್ಯೆ = -2;
    }
    ಅನ್ಯಥಾ
    {
	lnum = tv_get_lnum(argvars);
	ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    rettv->vval.v_ಸಂಖ್ಯೆ = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    ಶೂನ್ಯ
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ವರ್ಣ	*txt;
    long	ಎಣಿಕೆ;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    ಯದಿ (foldstart > 0 && foldend <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && dashes != ರಿಕ್ತ)
    {
	// Find first non-empty line in the fold.
	ಪ್ರತಿ (lnum = foldstart; lnum < foldend; ++lnum)
	    ಯದಿ (!linewhite(lnum))
		ಭಙ್ಗ;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	ಯದಿ (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    ಯದಿ (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		ಯದಿ (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	ಎಣಿಕೆ = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", ಎಣಿಕೆ);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	ಯದಿ (r != ರಿಕ್ತ)
	{
	    sprintf((ವರ್ಣ *)r, txt, dashes, ಎಣಿಕೆ);
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(r);
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
    ಶೂನ್ಯ
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    ಪೂರ್ಣಾಙ್ಕ		fold_ಎಣಿಕೆ;
    win_T	*wp  = curwin;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entered = FALSE;
    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (entered)
	ಪ್ರತ್ಯಾಗಮ; // reject recursive use
    entered = TRUE;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಯದಿ (wp == ರಿಕ್ತ || tp == ರಿಕ್ತ) // wrong winid
	{
	    entered = FALSE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    ಅನ್ಯಥಾ
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    ಯದಿ (lnum < 0)
	lnum = 0;
    fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, &foldinfo);
    ಯದಿ (fold_ಎಣಿಕೆ > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_ಎಣಿಕೆ - 1,
							       &foldinfo, buf);
	ಯದಿ (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
