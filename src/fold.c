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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
প্রকারলেখ কাঠামো
{
    linenr_T	fd_শীর্ষ;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    অক্ষর	fd_flags;	// see below
    অক্ষর	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
স্থির শূন্য newFoldLevelWin(win_T *wp);
স্থির পূর্ণসংখ্যা checkCloseRec(garray_T *gap, linenr_T lnum, পূর্ণসংখ্যা level);
স্থির পূর্ণসংখ্যা foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
স্থির পূর্ণসংখ্যা foldLevelWin(win_T *wp, linenr_T lnum);
স্থির শূন্য checkupdate(win_T *wp);
স্থির শূন্য setFoldRepeat(linenr_T lnum, long গুনতি, পূর্ণসংখ্যা do_open);
স্থির linenr_T setManualFold(linenr_T lnum, পূর্ণসংখ্যা opening, পূর্ণসংখ্যা recurse, পূর্ণসংখ্যা *donep);
স্থির linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, পূর্ণসংখ্যা opening, পূর্ণসংখ্যা recurse, পূর্ণসংখ্যা *donep);
স্থির শূন্য foldOpenNested(fold_T *fpr);
স্থির শূন্য deleteFoldEntry(garray_T *gap, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা recursive);
স্থির শূন্য foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
স্থির পূর্ণসংখ্যা getDeepestNestingRecurse(garray_T *gap);
স্থির পূর্ণসংখ্যা check_closed(win_T *win, fold_T *fp, পূর্ণসংখ্যা *use_levelp, পূর্ণসংখ্যা level, পূর্ণসংখ্যা *maybe_smallp, linenr_T lnum_off);
স্থির শূন্য checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
স্থির শূন্য setSmallMaybe(garray_T *gap);
স্থির শূন্য foldCreateMarkers(linenr_T start, linenr_T end);
স্থির শূন্য foldAddMarker(linenr_T lnum, char_u *marker, পূর্ণসংখ্যা markerlen);
স্থির শূন্য deleteFoldMarkers(fold_T *fp, পূর্ণসংখ্যা recursive, linenr_T lnum_off);
স্থির শূন্য foldDelMarker(linenr_T lnum, char_u *marker, পূর্ণসংখ্যা markerlen);
স্থির শূন্য foldUpdateIEMS(win_T *wp, linenr_T শীর্ষ_2, linenr_T bot);
স্থির শূন্য parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
স্থির linenr_T invalid_শীর্ষ = (linenr_T)0;
স্থির linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
স্থির linenr_T পূর্ব_lnum = 0;
স্থির পূর্ণসংখ্যা পূর্ব_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

স্থির পূর্ণসংখ্যা foldstartmarkerlen;
স্থির char_u *foldendmarker;
স্থির পূর্ণসংখ্যা foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    শূন্য
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
    পূর্ণসংখ্যা
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    ফেরত (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    পূর্ণসংখ্যা
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    ফেরত hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ফাঁকা);
}

// hasFoldingWin() {{{2
    পূর্ণসংখ্যা
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    পূর্ণসংখ্যা		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    পূর্ণসংখ্যা		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    পূর্ণসংখ্যা		x;
    fold_T	*fp;
    পূর্ণসংখ্যা		level = 0;
    পূর্ণসংখ্যা		use_level = FALSE;
    পূর্ণসংখ্যা		maybe_small = FALSE;
    garray_T	*gap;
    পূর্ণসংখ্যা		নিচের_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    যদি (!hasAnyFolding(win))
    {
	যদি (infop != ফাঁকা)
	    infop->fi_level = 0;
	ফেরত FALSE;
    }

    যদি (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	যদি (x >= 0)
	{
	    first = win->w_সারিগুলি[x].wl_lnum;
	    last = win->w_সারিগুলি[x].wl_lastlnum;
	    had_folded = win->w_সারিগুলি[x].wl_folded;
	}
    }

    যদি (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	জন্য (;;)
	{
	    যদি (!foldFind(gap, lnum_rel, &fp))
		বিরতি;

	    // Remember lowest level of fold that starts in "lnum".
	    যদি (lnum_rel == fp->fd_শীর্ষ && নিচের_level == 0)
		নিচের_level = level + 1;

	    first += fp->fd_শীর্ষ;
	    last += fp->fd_শীর্ষ;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    যদি (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		বিরতি;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_শীর্ষ;
	    ++level;
	}
    }

    যদি (!had_folded)
    {
	যদি (infop != ফাঁকা)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_নিচের_level = নিচের_level == 0 ? level : নিচের_level;
	}
	ফেরত FALSE;
    }

    যদি (last > win->w_বাফার->b_ml.ml_সারি_গুনতি)
	last = win->w_বাফার->b_ml.ml_সারি_গুনতি;
    যদি (lastp != ফাঁকা)
	*lastp = last;
    যদি (firstp != ফাঁকা)
	*firstp = first;
    যদি (infop != ফাঁকা)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_নিচের_level = নিচের_level == 0 ? level + 1 : নিচের_level;
    }
    ফেরত TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    পূর্ণসংখ্যা
সারি_Folded(win_T *win, linenr_T lnum)
{
    ফেরত folded_গুনতি(win, lnum, ফাঁকা) != 0;
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
folded_গুনতি(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    যদি (hasFoldingWin(win, lnum, ফাঁকা, &last, FALSE, infop))
	ফেরত (long)(last - lnum + 1);
    ফেরত 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    পূর্ণসংখ্যা
foldmethodIsManual(win_T *wp)
{
    ফেরত (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    পূর্ণসংখ্যা
foldmethodIsIndent(win_T *wp)
{
    ফেরত (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    পূর্ণসংখ্যা
foldmethod_Is_রাশিমালা(win_T *wp)
{
    ফেরত (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    পূর্ণসংখ্যা
foldmethodIsMarker(win_T *wp)
{
    ফেরত (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    পূর্ণসংখ্যা
foldmethodIsSyntax(win_T *wp)
{
    ফেরত (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    পূর্ণসংখ্যা
foldmethodIsDiff(win_T *wp)
{
    ফেরত (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    শূন্য
closeFold(linenr_T lnum, long গুনতি)
{
    setFoldRepeat(lnum, গুনতি, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    শূন্য
closeFoldRecurse(linenr_T lnum)
{
    (শূন্য)setManualFold(lnum, FALSE, TRUE, ফাঁকা);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    শূন্য
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    পূর্ণসংখ্যা		opening,	// TRUE to open, FALSE to close
    পূর্ণসংখ্যা		recurse,	// TRUE to do it recursively
    পূর্ণসংখ্যা		had_visual)	// TRUE when Visual selection used
{
    পূর্ণসংখ্যা		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_পরবর্তী;

    জন্য (lnum = first; lnum <= last; lnum = lnum_পরবর্তী + 1)
    {
	lnum_পরবর্তী = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	যদি (opening && !recurse)
	    (শূন্য)hasFolding(lnum, ফাঁকা, &lnum_পরবর্তী);
	(শূন্য)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	যদি (!opening && !recurse)
	    (শূন্য)hasFolding(lnum, ফাঁকা, &lnum_পরবর্তী);
    }
    যদি (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    যদি (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    শূন্য
openFold(linenr_T lnum, long গুনতি)
{
    setFoldRepeat(lnum, গুনতি, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    শূন্য
openFoldRecurse(linenr_T lnum)
{
    (শূন্য)setManualFold(lnum, TRUE, TRUE, ফাঁকা);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    শূন্য
foldOpenCursor(শূন্য)
{
    পূর্ণসংখ্যা		done;

    checkupdate(curwin);
    যদি (hasAnyFolding(curwin))
	জন্য (;;)
	{
	    done = DONE_NOTHING;
	    (শূন্য)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    যদি (!(done & DONE_ACTION))
		বিরতি;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    শূন্য
newFoldLevel(শূন্য)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    যদি (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    স্থির শূন্য
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    পূর্ণসংখ্যা		i;

    checkupdate(wp);
    যদি (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_তথ্য;
	জন্য (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    শূন্য
foldCheckClose(শূন্য)
{
    যদি (*p_fcl == NUL)
	ফেরত;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    যদি (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(পূর্ণসংখ্যা)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    স্থির পূর্ণসংখ্যা
checkCloseRec(garray_T *gap, linenr_T lnum, পূর্ণসংখ্যা level)
{
    fold_T	*fp;
    পূর্ণসংখ্যা		retval = FALSE;
    পূর্ণসংখ্যা		i;

    fp = (fold_T *)gap->ga_তথ্য;
    জন্য (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	যদি (fp[i].fd_flags == FD_OPEN)
	{
	    যদি (level <= 0 && (lnum < fp[i].fd_শীর্ষ
				      || lnum >= fp[i].fd_শীর্ষ + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    অন্যথা
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_শীর্ষ,
								   level - 1);
	}
    }
    ফেরত retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    পূর্ণসংখ্যা
foldManualAllowed(পূর্ণসংখ্যা create)
{
    যদি (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	ফেরত TRUE;
    যদি (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    অন্যথা
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    ফেরত FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    শূন্য
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		cont;
    পূর্ণসংখ্যা		use_level = FALSE;
    পূর্ণসংখ্যা		closed = FALSE;
    পূর্ণসংখ্যা		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    যদি (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    যদি (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	ফেরত;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    যদি (gap->ga_len == 0)
	i = 0;
    অন্যথা
    {
	জন্য (;;)
	{
	    যদি (!foldFind(gap, start_rel, &fp))
		বিরতি;
	    যদি (fp->fd_শীর্ষ + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_শীর্ষ;
		end_rel -= fp->fd_শীর্ষ;
		যদি (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    যদি (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		অন্যথা যদি (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    অন্যথা
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		বিরতি;
	    }
	}
	যদি (gap->ga_len == 0)
	    i = 0;
	অন্যথা
	    i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য);
    }

    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত;

    fp = (fold_T *)gap->ga_তথ্য + i;
    ga_init2(&fold_ga, আকার(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    জন্য (cont = 0; i + cont < gap->ga_len; ++cont)
	যদি (fp[cont].fd_শীর্ষ > end_rel)
	    বিরতি;
    যদি (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	যদি (start_rel > fp->fd_শীর্ষ)
	    start_rel = fp->fd_শীর্ষ;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	যদি (end_rel < fp[cont - 1].fd_শীর্ষ + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_শীর্ষ + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_তথ্য, fp, আকার(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	জন্য (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_তথ্য)[j].fd_শীর্ষ -= start_rel;
    }
    // Move remaining entries to after the new fold.
    যদি (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_তথ্য + i,
		আকার(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_শীর্ষ = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    যদি (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    যদি (!use_level)
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
    শূন্য
deleteFold(
    linenr_T	start,
    linenr_T	end,
    পূর্ণসংখ্যা		recursive,
    পূর্ণসংখ্যা		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = ফাঁকা;
    linenr_T	found_off = 0;
    পূর্ণসংখ্যা		use_level;
    পূর্ণসংখ্যা		maybe_small = FALSE;
    পূর্ণসংখ্যা		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    পূর্ণসংখ্যা		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    যখন (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = ফাঁকা;
	lnum_off = 0;
	use_level = FALSE;
	জন্য (;;)
	{
	    যদি (!foldFind(gap, lnum - lnum_off, &fp))
		বিরতি;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    যদি (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		বিরতি;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_শীর্ষ;
	    ++level;
	}
	যদি (found_ga == ফাঁকা)
	{
	    ++lnum;
	}
	অন্যথা
	{
	    lnum = found_fp->fd_শীর্ষ + found_fp->fd_len + found_off;

	    যদি (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (পূর্ণসংখ্যা)(found_fp - (fold_T *)found_ga->ga_তথ্য), recursive);
	    অন্যথা
	    {
		যদি (first_lnum > found_fp->fd_শীর্ষ + found_off)
		    first_lnum = found_fp->fd_শীর্ষ + found_off;
		যদি (last_lnum < lnum)
		    last_lnum = lnum;
		যদি (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    যদি (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	যদি (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    অন্যথা
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    যদি (last_lnum > 0)
	changed_সারিগুলি(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    শূন্য
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
    শূন্য
foldUpdate(win_T *wp, linenr_T শীর্ষ_2, linenr_T bot)
{
    fold_T	*fp;

    যদি (disable_fold_update > 0)
	ফেরত;
# ifdef FEAT_DIFF
    যদি (need_diff_redraw)
	// will update later
	ফেরত;
# endif

    যদি (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = শীর্ষ_2;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	যদি (শীর্ষ_2 > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = শীর্ষ_2;
	}
	(শূন্য)foldFind(&wp->w_folds, maybe_small_start, &fp);
	যখন (fp < (fold_T *)wp->w_folds.ga_তথ্য + wp->w_folds.ga_len
		&& fp->fd_শীর্ষ <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    যদি (foldmethodIsIndent(wp)
	    || foldmethod_Is_রাশিমালা(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	পূর্ণসংখ্যা save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, শীর্ষ_2, bot);
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
    শূন্য
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
    পূর্ণসংখ্যা
foldMoveTo(
    পূর্ণসংখ্যা		updown,
    পূর্ণসংখ্যা		dir,	    // FORWARD or BACKWARD
    long	গুনতি)
{
    long	n;
    পূর্ণসংখ্যা		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    পূর্ণসংখ্যা		use_level;
    পূর্ণসংখ্যা		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    পূর্ণসংখ্যা		level;
    পূর্ণসংখ্যা		last;

    checkupdate(curwin);

    // Repeat "count" times.
    জন্য (n = 0; n < গুনতি; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	যদি (gap->ga_len == 0)
	    বিরতি;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	জন্য (;;)
	{
	    যদি (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		যদি (!updown || gap->ga_len == 0)
		    বিরতি;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		যদি (dir == FORWARD)
		{
		    যদি (fp - (fold_T *)gap->ga_তথ্য >= gap->ga_len)
			বিরতি;
		    --fp;
		}
		অন্যথা
		{
		    যদি (fp == (fold_T *)gap->ga_তথ্য)
			বিরতি;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    যদি (!last)
	    {
		// Check if this fold is closed.
		যদি (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		যদি (last && !updown)
		    বিরতি;
	    }

	    যদি (updown)
	    {
		যদি (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    যদি (fp + 1 - (fold_T *)gap->ga_তথ্য < gap->ga_len)
		    {
			lnum = fp[1].fd_শীর্ষ + lnum_off;
			যদি (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		অন্যথা
		{
		    // to end of previous fold if there is one
		    যদি (fp > (fold_T *)gap->ga_তথ্য)
		    {
			lnum = fp[-1].fd_শীর্ষ + lnum_off + fp[-1].fd_len - 1;
			যদি (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    অন্যথা
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		যদি (dir == FORWARD)
		{
		    lnum = fp->fd_শীর্ষ + lnum_off + fp->fd_len - 1;
		    যদি (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		অন্যথা
		{
		    lnum = fp->fd_শীর্ষ + lnum_off;
		    যদি (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    যদি (last)
		বিরতি;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_শীর্ষ;
	    ++level;
	}
	যদি (lnum_found != curwin->w_cursor.lnum)
	{
	    যদি (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	অন্যথা
	    বিরতি;
    }

    ফেরত retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    শূন্য
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, আকার(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    পূর্ণসংখ্যা
find_wl_entry(win_T *win, linenr_T lnum)
{
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < win->w_সারিগুলি_valid; ++i)
	যদি (win->w_সারিগুলি[i].wl_valid)
	{
	    যদি (lnum < win->w_সারিগুলি[i].wl_lnum)
		ফেরত -1;
	    যদি (lnum <= win->w_সারিগুলি[i].wl_lastlnum)
		ফেরত i;
	}
    ফেরত -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    শূন্য
foldAdjustVisual(শূন্য)
{
    pos_T	*start, *end;

    যদি (!VIsual_active || !hasAnyFolding(curwin))
	ফেরত;

    যদি (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    অন্যথা
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    যদি (hasFolding(start->lnum, &start->lnum, ফাঁকা))
	start->col = 0;

    যদি (!hasFolding(end->lnum, ফাঁকা, &end->lnum))
	ফেরত;

    end->col = ml_get_len(end->lnum);
    যদি (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    যদি (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    শূন্য
foldAdjustCursor(শূন্য)
{
    (শূন্য)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ফাঁকা);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    শূন্য
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    পূর্ণসংখ্যা		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    যদি (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	ফেরত;

    from_p = (fold_T *)from->ga_তথ্য;
    to_p = (fold_T *)to->ga_তথ্য;

    জন্য (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_শীর্ষ = from_p->fd_শীর্ষ;
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
    স্থির পূর্ণসংখ্যা
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	নিচের, উপরের;
    fold_T	*fp;
    পূর্ণসংখ্যা		i;

    যদি (gap->ga_len == 0)
    {
	*fpp = ফাঁকা;
	ফেরত FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_তথ্য;
    নিচের = 0;
    উপরের = gap->ga_len - 1;
    যখন (নিচের <= উপরের)
    {
	i = (নিচের + উপরের) / 2;
	যদি (fp[i].fd_শীর্ষ > lnum)
	    // fold below lnum, adjust high
	    উপরের = i - 1;
	অন্যথা যদি (fp[i].fd_শীর্ষ + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    নিচের = i + 1;
	অন্যথা
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    ফেরত TRUE;
	}
    }
    *fpp = fp + নিচের;
    ফেরত FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    স্থির পূর্ণসংখ্যা
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    পূর্ণসংখ্যা		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    জন্য (;;)
    {
	যদি (!foldFind(gap, lnum_rel, &fp))
	    বিরতি;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_শীর্ষ;
	++level;
    }

    ফেরত level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    স্থির শূন্য
checkupdate(win_T *wp)
{
    যদি (!wp->w_foldinvalid)
	ফেরত;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    স্থির শূন্য
setFoldRepeat(linenr_T lnum, long গুনতি, পূর্ণসংখ্যা do_open)
{
    পূর্ণসংখ্যা		done;
    long	n;

    জন্য (n = 0; n < গুনতি; ++n)
    {
	done = DONE_NOTHING;
	(শূন্য)setManualFold(lnum, do_open, FALSE, &done);
	যদি (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    যদি (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    বিরতি;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    স্থির linenr_T
setManualFold(
    linenr_T	lnum,
    পূর্ণসংখ্যা		opening,    // TRUE when opening, FALSE when closing
    পূর্ণসংখ্যা		recurse,    // TRUE when closing/opening recursive
    পূর্ণসংখ্যা		*donep)
{
# ifdef FEAT_DIFF
    যদি (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		যদি (dlnum != 0)
		    (শূন্য)setManualFoldWin(wp, dlnum, opening, recurse, ফাঁকা);
	    }
	}
    }
# endif

    ফেরত setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    স্থির linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    পূর্ণসংখ্যা		opening,    // TRUE when opening, FALSE when closing
    পূর্ণসংখ্যা		recurse,    // TRUE when closing/opening recursive
    পূর্ণসংখ্যা		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = ফাঁকা;
    পূর্ণসংখ্যা		j;
    পূর্ণসংখ্যা		level = 0;
    পূর্ণসংখ্যা		use_level = FALSE;
    পূর্ণসংখ্যা		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	পরবর্তী = MAXLNUM;
    linenr_T	off = 0;
    পূর্ণসংখ্যা		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    জন্য (;;)
    {
	যদি (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    যদি (fp != ফাঁকা && fp < (fold_T *)gap->ga_তথ্য + gap->ga_len)
		পরবর্তী = fp->fd_শীর্ষ + off;
	    বিরতি;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	যদি (fp + 1 < (fold_T *)gap->ga_তথ্য + gap->ga_len)
	    পরবর্তী = fp[1].fd_শীর্ষ + off;

	// Change from level-dependent folding to manual.
	যদি (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    যদি (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    অন্যথা
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_তথ্য;
	    জন্য (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	যদি (!opening && recurse)
	{
	    যদি (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	অন্যথা যদি (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    যদি (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		যদি (recurse)
		    foldOpenNested(fp);
	    }
	    বিরতি;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_শীর্ষ;
	off += fp->fd_শীর্ষ;
	++level;
    }
    যদি (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	যদি (!opening && found != ফাঁকা)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	যদি (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    অন্যথা যদি (donep == ফাঁকা && wp == curwin)
	emsg(_(e_no_fold_found));

    যদি (donep != ফাঁকা)
	*donep |= done;

    ফেরত পরবর্তী;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    স্থির শূন্য
foldOpenNested(fold_T *fpr)
{
    পূর্ণসংখ্যা		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_তথ্য;
    জন্য (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    স্থির শূন্য
deleteFoldEntry(garray_T *gap, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা recursive)
{
    fold_T	*fp;
    পূর্ণসংখ্যা		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_তথ্য + idx;
    যদি (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	যদি (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, আকার(fold_T) * (gap->ga_len - idx));
    }
    অন্যথা
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	যদি (ga_grow(gap, (পূর্ণসংখ্যা)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_তথ্য + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_তথ্য;
	    জন্য (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_শীর্ষ += fp->fd_শীর্ষ;
		যদি (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		যদি (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    যদি (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  আকার(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(আকার(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    শূন্য
deleteFoldRecurse(garray_T *gap)
{
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_T *)(gap->ga_তথ্য))[i].fd_nested));
    ga_clear(gap);
}

// foldMarkAdjust() {{{2
/*
 * Update line numbers of folds for inserted/deleted lines.
 *
 * We are adjusting the folds in the range from line1 til line2,
 * make sure that line2 does not get smaller than line1
 */
    শূন্য
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    যদি (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    যদি (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    যদি ((State & MODE_ঢোকাও) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    স্থির শূন্য
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    পূর্ণসংখ্যা		i;
    linenr_T	last;
    linenr_T	শীর্ষ_2;

    যদি (gap->ga_len == 0)
	ফেরত;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    যদি ((State & MODE_ঢোকাও) && amount == (linenr_T)1 && line2 == MAXLNUM)
	শীর্ষ_2 = line1 + 1;
    অন্যথা
	শীর্ষ_2 = line1;

    // Find the fold containing or just below "line1".
    (শূন্য)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    জন্য (i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_শীর্ষ + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	যদি (last < line1)
	    চালু;

	// 6. fold below line2: only adjust for amount_after
	যদি (fp->fd_শীর্ষ > line2)
	{
	    যদি (amount_after == 0)
		বিরতি;
	    fp->fd_শীর্ষ += amount_after;
	}
	অন্যথা
	{
	    যদি (fp->fd_শীর্ষ >= শীর্ষ_2 && last <= line2)
	    {
		// 4. fold completely contained in range
		যদি (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		অন্যথা
		    fp->fd_শীর্ষ += amount;
	    }
	    অন্যথা
	    {
		যদি (fp->fd_শীর্ষ < শীর্ষ_2)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_শীর্ষ,
				  line2 - fp->fd_শীর্ষ, amount, amount_after);
		    যদি (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			যদি (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_শীর্ষ;
			অন্যথা
			    fp->fd_len += amount;
		    }
		    অন্যথা
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		অন্যথা
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    যদি (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_শীর্ষ,
				  amount,
				  amount_after + (fp->fd_শীর্ষ - শীর্ষ_2));
			fp->fd_len -= line2 - fp->fd_শীর্ষ + 1;
			fp->fd_শীর্ষ = line1;
		    }
		    অন্যথা
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_শীর্ষ,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_শীর্ষ += amount;
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
    পূর্ণসংখ্যা
getDeepestNesting(শূন্য)
{
    checkupdate(curwin);
    ফেরত getDeepestNestingRecurse(&curwin->w_folds);
}

    স্থির পূর্ণসংখ্যা
getDeepestNestingRecurse(garray_T *gap)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		level;
    পূর্ণসংখ্যা		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_তথ্য;
    জন্য (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	যদি (level > maxlevel)
	    maxlevel = level;
    }

    ফেরত maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    স্থির পূর্ণসংখ্যা
check_closed(
    win_T	*win,
    fold_T	*fp,
    পূর্ণসংখ্যা		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    পূর্ণসংখ্যা		level,		    // folding depth
    পূর্ণসংখ্যা		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    পূর্ণসংখ্যা		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    যদি (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	যদি (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    অন্যথা যদি (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    যদি (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    যদি (closed)
    {
	যদি (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	যদি (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    ফেরত closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    স্থির শূন্য
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    পূর্ণসংখ্যা		গুনতি;
    পূর্ণসংখ্যা		n;

    যদি (fp->fd_small != MAYBE)
	ফেরত;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    যদি (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    অন্যথা
    {
	গুনতি = 0;
	জন্য (n = 0; n < fp->fd_len; ++n)
	{
	    গুনতি += plines_win_nofold(wp, fp->fd_শীর্ষ + lnum_off + n);
	    যদি (গুনতি > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		ফেরত;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    স্থির শূন্য
setSmallMaybe(garray_T *gap)
{
    পূর্ণসংখ্যা		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_তথ্য;
    জন্য (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    স্থির শূন্য
foldCreateMarkers(linenr_T start, linenr_T end)
{
    যদি (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ফেরত;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_সারিগুলি(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    স্থির শূন্য
foldAddMarker(linenr_T lnum, char_u *marker, পূর্ণসংখ্যা markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*সারি;
    পূর্ণসংখ্যা		সারি_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((অক্ষর *)curbuf->b_p_cms, "%s");
    পূর্ণসংখ্যা		সারি_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    সারি = ml_get(lnum);
    সারি_len = ml_get_len(lnum);

    যদি (u_save(lnum - 1, lnum + 1) != OK)
	ফেরত;

    // Check if the line ends with an unclosed comment
    (শূন্য)skip_comment(সারি, FALSE, FALSE, &সারি_is_comment);
    newline = alloc(সারি_len + markerlen + STRLEN(cms) + 1);
    যদি (newline == ফাঁকা)
	ফেরত;
    STRCPY(newline, সারি);
    // Append the marker to the end of the line
    যদি (p == ফাঁকা || সারি_is_comment)
	vim_strncpy(newline + সারি_len, marker, markerlen);
    অন্যথা
    {
	STRCPY(newline + সারি_len, cms);
	STRNCPY(newline + সারি_len + (p - cms), marker, markerlen);
	STRCPY(newline + সারি_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    স্থির শূন্য
deleteFoldMarkers(
    fold_T	*fp,
    পূর্ণসংখ্যা		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    পূর্ণসংখ্যা		i;

    যদি (recursive)
	জন্য (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_তথ্য + i, TRUE,
						       lnum_off + fp->fd_শীর্ষ);
    foldDelMarker(fp->fd_শীর্ষ + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_শীর্ষ + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    স্থির শূন্য
foldDelMarker(linenr_T lnum, char_u *marker, পূর্ণসংখ্যা markerlen)
{
    char_u	*সারি;
    char_u	*newline;
    char_u	*p;
    পূর্ণসংখ্যা		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    যদি (lnum > curbuf->b_ml.ml_সারি_গুনতি)
	ফেরত;
    সারি = ml_get(lnum);
    জন্য (p = সারি; *p != NUL; ++p)
	যদি (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    যদি (VIM_ISDIGIT(p[len]))
		++len;
	    যদি (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((অক্ষর *)cms, "%s");
		যদি (cms2 != ফাঁকা && p - সারি >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (পূর্ণসংখ্যা)STRLEN(cms) - 2;
		}
	    }
	    যদি (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		যদি (newline != ফাঁকা)
		{
		    STRNCPY(newline, সারি, p - সারি);
		    STRCPY(newline + (p - সারি), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    বিরতি;
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
    char_u	*text = ফাঁকা;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    স্থির পূর্ণসংখ্যা	    got_fdt_error = FALSE;
    পূর্ণসংখ্যা		    save_did_emsg = did_emsg;
    স্থির win_T    *last_wp = ফাঁকা;
    স্থির linenr_T last_lnum = 0;

    যদি (last_wp != wp || last_wp == ফাঁকা
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    যদি (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    যদি (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	পূর্ণসংখ্যা	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	যদি (level > (পূর্ণসংখ্যা)আকার(dashes) - 1)
	    level = (পূর্ণসংখ্যা)আকার(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	যদি (!got_fdt_error)
	{
	    win_T   *save_curwin = curwin;
	    sctx_T  saved_sctx = current_sctx;

	    curwin = wp;
	    curbuf = wp->w_বাফার;
	    current_sctx = wp->w_p_script_ctx[WV_FDT];

	    ++emsg_off; // handle exceptions, but don't display errors
	    text = eval_to_string_safe(wp->w_p_fdt,
		       was_set_insecurely(wp, (char_u *)"foldtext", OPT_LOCAL),
		       TRUE, TRUE);
	    --emsg_off;

	    যদি (text == ফাঁকা || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_বাফার;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, ফাঁকা, -1);

	যদি (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	যদি (text != ফাঁকা)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    জন্য (p = text; *p != NUL; ++p)
	    {
		পূর্ণসংখ্যা	len;

		যদি (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    যদি (!vim_isprintc((*mb_ptr2char)(p)))
			বিরতি;
		    p += len - 1;
		}
		অন্যথা
		    যদি (*p == TAB)
			*p = ' ';
		    অন্যথা যদি (ptr2cells(p) > 1)
			বিরতি;
	    }
	    যদি (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    যদি (text == ফাঁকা)
# endif
    {
	long গুনতি = (long)(lnume - lnum + 1);

	vim_snprintf((অক্ষর *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", গুনতি),
		     গুনতি);
	text = buf;
    }
    ফেরত text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    স্থির শূন্য
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    পূর্ণসংখ্যা		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    পূর্ণসংখ্যা		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		did1 = FALSE;
    পূর্ণসংখ্যা		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (পূর্ণসংখ্যা)STRLEN(cms_start);
    যখন (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((অক্ষর *)cms_start, "%s");
    যদি (cms_end != ফাঁকা)
    {
	cms_elen = cms_slen - (পূর্ণসংখ্যা)(cms_end - cms_start);
	cms_slen = (পূর্ণসংখ্যা)(cms_end - cms_start);

	// exclude white space before "%s"
	যখন (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (পূর্ণসংখ্যা)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    জন্য (s = str; *s != NUL; )
    {
	len = 0;
	যদি (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	অন্যথা যদি (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	যদি (len > 0)
	{
	    যদি (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    জন্য (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    যদি (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (পূর্ণসংখ্যা)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	অন্যথা যদি (cms_end != ফাঁকা)
	{
	    যদি (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    অন্যথা যদি (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	যদি (len != 0)
	{
	    যখন (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	অন্যথা
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
প্রকারলেখ কাঠামো
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    পূর্ণসংখ্যা		lvl;		// current level (-1 for undefined)
    পূর্ণসংখ্যা		lvl_পরবর্তী;	// level used for next line
    পূর্ণসংখ্যা		start;		// number of folds that are forced to start at
				// this line.
    পূর্ণসংখ্যা		end;		// level of fold that is forced to end below
				// this line
    পূর্ণসংখ্যা		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
স্থির পূর্ণসংখ্যা fold_changed;

// Function declarations. {{{2
স্থির linenr_T foldUpdateIEMSRecurse(garray_T *gap, পূর্ণসংখ্যা level, linenr_T startlnum, fline_T *flp, শূন্য (*getlevel)(fline_T *), linenr_T bot, পূর্ণসংখ্যা topflags);
স্থির পূর্ণসংখ্যা fold_ঢোকাও(garray_T *gap, পূর্ণসংখ্যা i);
স্থির শূন্য foldSplit(garray_T *gap, পূর্ণসংখ্যা i, linenr_T শীর্ষ_2, linenr_T bot);
স্থির শূন্য foldRemove(garray_T *gap, linenr_T শীর্ষ_2, linenr_T bot);
স্থির শূন্য foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
স্থির শূন্য foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
স্থির শূন্য foldlevelDiff(fline_T *flp);
# endif
স্থির শূন্য foldlevel_রাশিমালা(fline_T *flp);
স্থির শূন্য foldlevelMarker(fline_T *flp);
স্থির শূন্য foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    স্থির শূন্য
foldUpdateIEMS(win_T *wp, linenr_T শীর্ষ_2, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    শূন্য	(*getlevel)(fline_T *);
    পূর্ণসংখ্যা		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    যদি (invalid_শীর্ষ != (linenr_T)0)
	ফেরত;

    যদি (wp->w_foldinvalid)
    {
	// Need to update all folds.
	শীর্ষ_2 = 1;
	bot = wp->w_বাফার->b_ml.ml_সারি_গুনতি;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    যদি (foldmethodIsDiff(wp))
    {
	যদি (শীর্ষ_2 > diff_context)
	    শীর্ষ_2 -= diff_context;
	অন্যথা
	    শীর্ষ_2 = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    যদি (শীর্ষ_2 > wp->w_বাফার->b_ml.ml_সারি_গুনতি)
	শীর্ষ_2 = wp->w_বাফার->b_ml.ml_সারি_গুনতি;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_পরবর্তী = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_শীর্ষ = শীর্ষ_2;
    invalid_bot = bot;

    যদি (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	যদি (শীর্ষ_2 > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, শীর্ষ_2 - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = শীর্ষ_2 - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    যদি (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_পরবর্তী);
	    অন্যথা
		fline.lvl = fline.lvl_পরবর্তী;
	}
	fline.lnum = শীর্ষ_2;
	getlevel(&fline);
    }
    অন্যথা
    {
	fline.lnum = শীর্ষ_2;
	যদি (foldmethod_Is_রাশিমালা(wp))
	{
	    getlevel = foldlevel_রাশিমালা;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    যদি (শীর্ষ_2 > 1)
		--fline.lnum;
	}
	অন্যথা যদি (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	অন্যথা যদি (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	অন্যথা
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    যদি (শীর্ষ_2 > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	জন্য ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_পরবর্তী = -1;
	    getlevel(&fline);
	    যদি (fline.lvl >= 0)
		বিরতি;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    যদি (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ফাঁকা;
	পূর্ণসংখ্যা	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	যখন (current_fdl < fline.lvl)
	{
	    যদি (!foldFind(gap, lnum_rel, &fpn))
		বিরতি;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_শীর্ষ;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_শীর্ষ;
	}
	যদি (fpn != ফাঁকা && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    যদি (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    যদি (start > end && end < wp->w_বাফার->b_ml.ml_সারি_গুনতি)
	end = start;
    যখন (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	যদি (fline.lnum > wp->w_বাফার->b_ml.ml_সারি_গুনতি)
	    বিরতি;
	যদি (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    যদি (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_রাশিমালা)
		বিরতি;
	    যদি ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_শীর্ষ + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_শীর্ষ < fline.lnum))
		end = fp->fd_শীর্ষ + fp->fd_len - 1;
	    অন্যথা যদি (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    অন্যথা
		বিরতি;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	যদি (fline.lvl > 0)
	{
	    invalid_শীর্ষ = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	অন্যথা
	{
	    যদি (fline.lnum == wp->w_বাফার->b_ml.ml_সারি_গুনতি)
		বিরতি;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_পরবর্তী;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    যদি (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    যদি (end != bot)
	redraw_win_range_later(wp, শীর্ষ_2, end);

    invalid_শীর্ষ = (linenr_T)0;
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
    স্থির linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    পূর্ণসংখ্যা		level,
    linenr_T	startlnum,
    fline_T	*flp,
    শূন্য	(*getlevel)(fline_T *),
    linenr_T	bot,
    পূর্ণসংখ্যা		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ফাঁকা;
    fold_T	*fp2;
    পূর্ণসংখ্যা		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		finish = FALSE;
    linenr_T	linecount = flp->wp->w_বাফার->b_ml.ml_সারি_গুনতি - flp->off;
    পূর্ণসংখ্যা		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    যদি (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(শূন্য)foldFind(gap, startlnum - 1, &fp);
	যদি (fp != ফাঁকা && (fp >= ((fold_T *)gap->ga_তথ্য) + gap->ga_len
						   || fp->fd_শীর্ষ >= startlnum))
	    fp = ফাঁকা;
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
    যখন (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	সারি_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	যদি (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	যদি (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	যদি (flp->lnum > bot && !finish && fp != ফাঁকা)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    যদি (getlevel != foldlevelMarker
		    && getlevel != foldlevel_রাশিমালা
		    && getlevel != foldlevelSyntax)
		বিরতি;
	    i = 0;
	    fp2 = fp;
	    যদি (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_শীর্ষ;
		যখন (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_শীর্ষ;
		}
	    }
	    যদি (lvl < level + i)
	    {
		(শূন্য)foldFind(&fp->fd_nested, flp->lnum - fp->fd_শীর্ষ, &fp2);
		যদি (fp2 != ফাঁকা)
		    bot = fp2->fd_শীর্ষ + fp2->fd_len - 1 + fp->fd_শীর্ষ;
	    }
	    অন্যথা যদি (fp->fd_শীর্ষ + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    অন্যথা
		বিরতি;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	যদি (fp == ফাঁকা
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
	    যখন (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		যদি (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		অন্যথা
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		যদি (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_তথ্য) + gap->ga_len
			    && fp->fd_শীর্ষ <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_তথ্য) + gap->ga_len
			    && ((lvl < level && fp->fd_শীর্ষ < flp->lnum)
				|| (lvl >= level
					   && fp->fd_শীর্ষ <= flp->lnum_save)))))
		{
		    যদি (fp->fd_শীর্ষ + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			যদি (fp->fd_শীর্ষ == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			অন্যথা যদি (fp->fd_শীর্ষ >= startlnum)
			{
			    যদি (fp->fd_শীর্ষ > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_শীর্ষ - firstlnum), 0L);
			    অন্যথা
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_শীর্ষ - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_শীর্ষ - firstlnum));
			    fp->fd_len += fp->fd_শীর্ষ - firstlnum;
			    fp->fd_শীর্ষ = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			অন্যথা যদি ((flp->start != 0 && lvl == level)
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
			    যদি (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    অন্যথা
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_শীর্ষ,
						      breakend - fp->fd_শীর্ষ);
			    i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_তথ্য + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    যদি (getlevel == foldlevelMarker
				    || getlevel == foldlevel_রাশিমালা
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			যদি (fp->fd_শীর্ষ == startlnum && concat)
			{
			    i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য);
			    যদি (i != 0)
			    {
				fp2 = fp - 1;
				যদি (fp2->fd_শীর্ষ + fp2->fd_len == fp->fd_শীর্ষ)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			বিরতি;
		    }
		    যদি (fp->fd_শীর্ষ >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য), TRUE);
		    }
		    অন্যথা
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_শীর্ষ;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		অন্যথা
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    যদি (gap->ga_len == 0)
			i = 0;
		    অন্যথা
			i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য);
		    যদি (fold_ঢোকাও(gap, i) != OK)
			ফেরত bot;
		    fp = (fold_T *)gap->ga_তথ্য + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_শীর্ষ = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    যদি (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    অন্যথা যদি (i <= 0)
		    {
			fp->fd_flags = topflags;
			যদি (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    অন্যথা
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    যদি (getlevel == foldlevelMarker
			    || getlevel == foldlevel_রাশিমালা
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    বিরতি;
		}
	    }
	}

	যদি (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    বিরতি;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	যদি (lvl > level && fp != ফাঁকা)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    যদি (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_শীর্ষ;
	    flp->off += fp->fd_শীর্ষ;
	    i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_শীর্ষ, flp, getlevel,
					      bot - fp->fd_শীর্ষ, fp->fd_flags);
	    fp = (fold_T *)gap->ga_তথ্য + i;
	    flp->lnum += fp->fd_শীর্ষ;
	    flp->lnum_save += fp->fd_শীর্ষ;
	    flp->off -= fp->fd_শীর্ষ;
	    bot += fp->fd_শীর্ষ;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	অন্যথা
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    যখন (!got_int)
	    {
		// Make the previous level available to foldlevel().
		পূর্ব_lnum = flp->lnum;
		পূর্ব_lnum_lvl = flp->lvl;

		যদি (++flp->lnum > linecount)
		    বিরতি;
		flp->lvl = flp->lvl_পরবর্তী;
		getlevel(flp);
		যদি (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    বিরতি;
	    }
	    পূর্ব_lnum = 0;
	    যদি (flp->lnum > linecount)
		বিরতি;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    যদি (fp == ফাঁকা)	// only happens when got_int is set
	ফেরত bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    যদি (fp->fd_len < flp->lnum - fp->fd_শীর্ষ)
    {
	fp->fd_len = flp->lnum - fp->fd_শীর্ষ;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    অন্যথা যদি (fp->fd_শীর্ষ + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_শীর্ষ + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_শীর্ষ,
						  flp->lnum - 1 - fp->fd_শীর্ষ);

    যদি (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	যদি (fp->fd_len != flp->lnum - fp->fd_শীর্ষ)
	{
	    যদি (fp->fd_শীর্ষ + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		যদি (getlevel == foldlevelMarker
			|| getlevel == foldlevel_রাশিমালা
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_শীর্ষ + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_শীর্ষ;
		}
		অন্যথা
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_তথ্য + i;
		}
	    }
	    অন্যথা
		fp->fd_len = flp->lnum - fp->fd_শীর্ষ;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    জন্য (;;)
    {
	fp2 = fp + 1;
	যদি (fp2 >= (fold_T *)gap->ga_তথ্য + gap->ga_len
						  || fp2->fd_শীর্ষ > flp->lnum)
	    বিরতি;
	যদি (fp2->fd_শীর্ষ + fp2->fd_len > flp->lnum)
	{
	    যদি (fp2->fd_শীর্ষ < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_শীর্ষ - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_শীর্ষ - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_শীর্ষ;
		fp2->fd_শীর্ষ = flp->lnum;
		fold_changed = TRUE;
	    }

	    যদি (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    বিরতি;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (পূর্ণসংখ্যা)(fp2 - (fold_T *)gap->ga_তথ্য), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    যদি (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    ফেরত bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    স্থির পূর্ণসংখ্যা
fold_ঢোকাও(garray_T *gap, পূর্ণসংখ্যা i)
{
    fold_T	*fp;

    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত FAIL;
    fp = (fold_T *)gap->ga_তথ্য + i;
    যদি (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, আকার(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, আকার(fold_T), 10);
    ফেরত OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    স্থির শূন্য
foldSplit(
    garray_T	*gap,
    পূর্ণসংখ্যা		i,
    linenr_T	শীর্ষ_2,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    পূর্ণসংখ্যা		idx;
    পূর্ণসংখ্যা		len;

    // The fold continues below bot, need to split it.
    যদি (fold_ঢোকাও(gap, i + 1) == FAIL)
	ফেরত;
    fp = (fold_T *)gap->ga_তথ্য + i;
    fp[1].fd_শীর্ষ = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_শীর্ষ - fp->fd_শীর্ষ);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (শূন্য)foldFind(gap1, bot + 1 - fp->fd_শীর্ষ, &fp2);
    যদি (fp2 != ফাঁকা)
    {
	len = (পূর্ণসংখ্যা)((fold_T *)gap1->ga_তথ্য + gap1->ga_len - fp2);
	যদি (len > 0 && ga_grow(gap2, len) == OK)
	{
	    জন্য (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_তথ্য)[idx] = fp2[idx];
		((fold_T *)gap2->ga_তথ্য)[idx].fd_শীর্ষ
						  -= fp[1].fd_শীর্ষ - fp->fd_শীর্ষ;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = শীর্ষ_2 - fp->fd_শীর্ষ;
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
    স্থির শূন্য
foldRemove(garray_T *gap, linenr_T শীর্ষ_2, linenr_T bot)
{
    fold_T	*fp = ফাঁকা;

    যদি (bot < শীর্ষ_2)
	ফেরত;		// nothing to do

    যখন (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	যদি (foldFind(gap, শীর্ষ_2, &fp) && fp->fd_শীর্ষ < শীর্ষ_2)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, শীর্ষ_2 - fp->fd_শীর্ষ, bot - fp->fd_শীর্ষ);
	    যদি (fp->fd_শীর্ষ + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য), শীর্ষ_2, bot);
	    }
	    অন্যথা
	    {
		// 2: truncate fold at "top".
		fp->fd_len = শীর্ষ_2 - fp->fd_শীর্ষ;
	    }
	    fold_changed = TRUE;
	    চালু;
	}
	যদি (fp >= (fold_T *)(gap->ga_তথ্য) + gap->ga_len
		|| fp->fd_শীর্ষ > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    বিরতি;
	}
	যদি (fp->fd_শীর্ষ >= শীর্ষ_2)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    যদি (fp->fd_শীর্ষ + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_শীর্ষ),
			(linenr_T)MAXLNUM, (long)(fp->fd_শীর্ষ - bot - 1));
		fp->fd_len -= bot - fp->fd_শীর্ষ + 1;
		fp->fd_শীর্ষ = bot + 1;
		বিরতি;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (পূর্ণসংখ্যা)(fp - (fold_T *)gap->ga_তথ্য), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    স্থির শূন্য
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *বাম, *ডান;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    জন্য (; start < end; start++, end--)
    {
	বাম = (fold_T *)gap->ga_তথ্য + start;
	ডান = (fold_T *)gap->ga_তথ্য + end;
	tmp  = *বাম;
	*বাম = *ডান;
	*ডান = tmp;
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

    স্থির শূন্য
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_শীর্ষ, MAXLNUM);
    fp->fd_len = end - fp->fd_শীর্ষ;
}

# define fold_end(fp) ((fp)->fd_শীর্ষ + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_তথ্য + (gap)->ga_len))
# define fold_সূচক(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_তথ্য)))

    শূন্য
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    পূর্ণসংখ্যা at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_সূচক = 0;

    যদি (at_start)
    {
	যদি (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_শীর্ষ, line2 -
		    fp->fd_শীর্ষ, dest - fp->fd_শীর্ষ);
	    ফেরত;
	}
	অন্যথা যদি (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_শীর্ষ, line2 -
		    fp->fd_শীর্ষ, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	অন্যথা
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    যদি (!valid_fold(fp, gap) || fp->fd_শীর্ষ > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	ফেরত;
    }
    অন্যথা যদি (fp->fd_শীর্ষ > line2)
    {
	জন্য (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_শীর্ষ -= range_len;

	যদি (valid_fold(fp, gap) && fp->fd_শীর্ষ <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_শীর্ষ -= range_len;
	}
	ফেরত;
    }
    অন্যথা যদি (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_শীর্ষ, dest -
		fp->fd_শীর্ষ, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_শীর্ষ += move_len;
	ফেরত;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_সূচক(fp, gap);

    জন্য (; valid_fold(fp, gap) && fp->fd_শীর্ষ <= dest; fp++)
    {
	যদি (fp->fd_শীর্ষ <= line2)
	{
	    // 1. 2. or 3.
	    যদি (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_শীর্ষ += move_len;
	    চালু;
	}

	// Record index of the first fold after the moved range.
	যদি (move_end == 0)
	    move_end = fold_সূচক(fp, gap);

	যদি (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_শীর্ষ -= range_len;
    }

    dest_সূচক = fold_সূচক(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    যদি (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	ফেরত;
    foldReverseOrder(gap, (linenr_T)move_start, (linenr_T)dest_সূচক - 1);
    foldReverseOrder(gap, (linenr_T)move_start,
			   (linenr_T)(move_start + dest_সূচক - move_end - 1));
    foldReverseOrder(gap, (linenr_T)(move_start + dest_সূচক - move_end),
						   (linenr_T)(dest_সূচক - 1));
}
# undef fold_end
# undef valid_fold
# undef fold_সূচক

// foldMerge() {{{2
/*
 * Merge two adjacent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjacent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    স্থির শূন্য
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    পূর্ণসংখ্যা		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    যদি (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    যদি (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	জন্য (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_তথ্য)[gap1->ga_len]
					= ((fold_T *)gap2->ga_তথ্য)[idx];
	    ((fold_T *)gap1->ga_তথ্য)[gap1->ga_len].fd_শীর্ষ += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (পূর্ণসংখ্যা)(fp2 - (fold_T *)gap->ga_তথ্য), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    স্থির শূন্য
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_বাফার;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    যদি (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ফাঁকা)
    {
	// first and last line can't be undefined, use level 0
	যদি (lnum == 1 || lnum == buf->b_ml.ml_সারি_গুনতি)
	    flp->lvl = 0;
	অন্যথা
	    flp->lvl = -1;
    }
    অন্যথা
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_মান(buf);
    যদি (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	যদি (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    স্থির শূন্য
foldlevelDiff(fline_T *flp)
{
    যদি (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    অন্যথা
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    স্থির শূন্য
foldlevel_রাশিমালা(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# অন্যথা
    win_T	*win;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		c;
    linenr_T	lnum = flp->lnum + flp->off;
    পূর্ণসংখ্যা		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_বাফার;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    যদি (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = চাবি_Typed;
    n = eval_foldexpr(flp->wp, &c);
    চাবি_Typed = save_keytyped;

    নির্বাচন (c)
    {
	// "a1", "a2", .. : add to the fold level
	ক্ষেত্র 'a': যদি (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_পরবর্তী = flp->lvl;
		  }
		  flp->start = n;
		  বিরতি;

	// "s1", "s2", .. : subtract from the fold level
	ক্ষেত্র 's': যদি (flp->lvl >= 0)
		  {
		      যদি (n > flp->lvl)
			  flp->lvl_পরবর্তী = 0;
		      অন্যথা
			  flp->lvl_পরবর্তী = flp->lvl - n;
		      flp->end = flp->lvl_পরবর্তী + 1;
		  }
		  বিরতি;

	// ">1", ">2", .. : start a fold with a certain level
	ক্ষেত্র '>': flp->lvl = n;
		  flp->lvl_পরবর্তী = n;
		  flp->start = 1;
		  বিরতি;

	// "<1", "<2", .. : end a fold with a certain level
	ক্ষেত্র '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_পরবর্তী = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  বিরতি;

	// "=": No change in level
	ক্ষেত্র '=': flp->lvl_পরবর্তী = flp->lvl;
		  বিরতি;

	// "-1", "0", "1", ..: set fold level
	পূর্বনির্ধারিত:  যদি (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_পরবর্তী = flp->lvl;
		  অন্যথা
		      flp->lvl_পরবর্তী = n;
		  flp->lvl = n;
		  বিরতি;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    যদি (flp->lvl < 0)
    {
	যদি (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_পরবর্তী = 0;
	}
	যদি (lnum == curbuf->b_ml.ml_সারি_গুনতি)
	    flp->lvl_পরবর্তী = 0;
    }

    curwin = win;
    curbuf = curwin->w_বাফার;
# endif
}

// parseMarker() {{{2
/*
 * Parse 'foldmarker' and set "foldendmarker", "foldstartmarkerlen" and
 * "foldendmarkerlen".
 * Relies on the option value to have been checked for correctness already.
 */
    স্থির শূন্য
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (পূর্ণসংখ্যা)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (পূর্ণসংখ্যা)STRLEN(foldendmarker);
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
    স্থির শূন্য
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    পূর্ণসংখ্যা		cstart;
    পূর্ণসংখ্যা		cend;
    পূর্ণসংখ্যা		start_lvl = flp->lvl;
    char_u	*s;
    পূর্ণসংখ্যা		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_পরবর্তী = flp->lvl;

    s = ml_get_buf(flp->wp->w_বাফার, flp->lnum + flp->off, FALSE);
    যখন (*s)
    {
	যদি (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    যদি (VIM_ISDIGIT(*s))
	    {
		n = atoi((অক্ষর *)s);
		যদি (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_পরবর্তী = n;
		    যদি (n <= start_lvl)
			flp->start = 1;
		    অন্যথা
			flp->start = n - start_lvl;
		}
	    }
	    অন্যথা
	    {
		++flp->lvl;
		++flp->lvl_পরবর্তী;
		++flp->start;
	    }
	}
	অন্যথা যদি (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    যদি (VIM_ISDIGIT(*s))
	    {
		n = atoi((অক্ষর *)s);
		যদি (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_পরবর্তী = n - 1;
		    // never start a fold with an end marker
		    যদি (flp->lvl_পরবর্তী > start_lvl)
			flp->lvl_পরবর্তী = start_lvl;
		}
	    }
	    অন্যথা
		--flp->lvl_পরবর্তী;
	}
	অন্যথা
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    যদি (flp->lvl_পরবর্তী < 0)
	flp->lvl_পরবর্তী = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    স্থির শূন্য
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# অন্যথা
    linenr_T	lnum = flp->lnum + flp->off;
    পূর্ণসংখ্যা		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    যদি (lnum < flp->wp->w_বাফার->b_ml.ml_সারি_গুনতি)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	যদি (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# যদি defined(FEAT_SESSION)
স্থির পূর্ণসংখ্যা put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
স্থির পূর্ণসংখ্যা put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
স্থির পূর্ণসংখ্যা put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    পূর্ণসংখ্যা
put_folds(FILE *fd, win_T *wp)
{
    যদি (foldmethodIsManual(wp))
    {
	যদি (put_সারি(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_সারি(fd, "&fdl = &fdl") == FAIL)
	    ফেরত FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    যদি (wp->w_fold_manual)
	ফেরত put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    ফেরত OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    স্থির পূর্ণসংখ্যা
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    পূর্ণসংখ্যা		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_তথ্য;
    জন্য (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	যদি (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_শীর্ষ) == FAIL)
	    ফেরত FAIL;
	যদি (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_শীর্ষ + off,
					fp->fd_শীর্ষ + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    ফেরত FAIL;
	++fp;
    }
    ফেরত OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    স্থির পূর্ণসংখ্যা
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_তথ্য;
    জন্য (i = 0; i < gap->ga_len; i++)
    {
	যদি (fp->fd_flags != FD_LEVEL)
	{
	    যদি (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		যদি (fprintf(fd, ":%ld", fp->fd_শীর্ষ + off) < 0
			|| put_eol(fd) == FAIL
			|| put_সারি(fd, "sil! normal! zo") == FAIL)
		    ফেরত FAIL;
		যদি (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_শীর্ষ)
			== FAIL)
		    ফেরত FAIL;
		// close the parent when needed
		যদি (fp->fd_flags == FD_CLOSED)
		{
		    যদি (put_fold_open_close(fd, fp, off) == FAIL)
			ফেরত FAIL;
		}
	    }
	    অন্যথা
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_শীর্ষ);
		যদি ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		যদি (put_fold_open_close(fd, fp, off) == FAIL)
		    ফেরত FAIL;
	    }
	}
	++fp;
    }

    ফেরত OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    স্থির পূর্ণসংখ্যা
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    যদি (fprintf(fd, ":%ld", fp->fd_শীর্ষ + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	ফেরত FAIL;

    ফেরত OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#যদি defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    স্থির শূন্য
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    পূর্ণসংখ্যা		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_সংখ্যা(&argvars[1]), &tp);
	যদি (wp != ফাঁকা && tp != ফাঁকা)
	{
	    যদি (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	অন্যথা
	{
	    rettv->vval.v_সংখ্যা = -2;
	    ফেরত;
	}
    }
    অন্যথা
	lnum = tv_get_lnum(argvars);

    যদি (lnum >= 1 && lnum <= wp->w_বাফার->b_ml.ml_সারি_গুনতি)
    {
	যদি (hasFoldingWin(wp, lnum, &first, &last, FALSE, ফাঁকা))
	{
	    যদি (end)
		rettv->vval.v_সংখ্যা = (varnumber_T)last;
	    অন্যথা
		rettv->vval.v_সংখ্যা = (varnumber_T)first;
	    ফেরত;
	}
    }
# endif
    rettv->vval.v_সংখ্যা = -1;
}

/*
 * "foldclosed()" function
 */
    শূন্য
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    শূন্য
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    স্থির পূর্ণসংখ্যা
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    যদি (invalid_শীর্ষ == (linenr_T)0)
	checkupdate(curwin);
    অন্যথা যদি (lnum == পূর্ব_lnum && পূর্ব_lnum_lvl >= 0)
	ফেরত পূর্ব_lnum_lvl;
    অন্যথা যদি (lnum >= invalid_শীর্ষ && lnum <= invalid_bot)
	ফেরত -1;

    // Return quickly when there is no folding at all in this window.
    যদি (!hasAnyFolding(curwin))
	ফেরত 0;

    ফেরত foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    শূন্য
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_সংখ্যা(&argvars[1]), &tp);
	যদি (wp != ফাঁকা && tp != ফাঁকা)
	{
	    যদি (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি)
		    rettv->vval.v_সংখ্যা = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	অন্যথা
	    rettv->vval.v_সংখ্যা = -2;
    }
    অন্যথা
    {
	lnum = tv_get_lnum(argvars);
	যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি)
	    rettv->vval.v_সংখ্যা = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    শূন্য
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    পূর্ণসংখ্যা		len;
    অক্ষর	*txt;
    long	গুনতি;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    যদি (foldstart > 0 && foldend <= curbuf->b_ml.ml_সারি_গুনতি
	    && dashes != ফাঁকা)
    {
	// Find first non-empty line in the fold.
	জন্য (lnum = foldstart; lnum < foldend; ++lnum)
	    যদি (!linewhite(lnum))
		বিরতি;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	যদি (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    যদি (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		যদি (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	গুনতি = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", গুনতি);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	যদি (r != ফাঁকা)
	{
	    sprintf((অক্ষর *)r, txt, dashes, গুনতি);
	    len = (পূর্ণসংখ্যা)STRLEN(r);
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
    শূন্য
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    পূর্ণসংখ্যা		fold_গুনতি;
    win_T	*wp  = curwin;
    স্থির পূর্ণসংখ্যা	entered = FALSE;
    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (entered)
	ফেরত; // reject recursive use
    entered = TRUE;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_সংখ্যা(&argvars[1]), &tp);
	যদি (wp == ফাঁকা || tp == ফাঁকা) // wrong winid
	{
	    entered = FALSE;
	    ফেরত;
	}
	যদি (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    অন্যথা
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    যদি (lnum < 0)
	lnum = 0;
    fold_গুনতি = folded_গুনতি(wp, lnum, &foldinfo);
    যদি (fold_গুনতি > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_গুনতি - 1,
							       &foldinfo, buf);
	যদি (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
