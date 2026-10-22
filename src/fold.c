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

#ضمن "vim.h"

#אם defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
تعريفنوع بنية
{
    linenr_T	fd_top;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    حرف	fd_flags;	// see below
    حرف	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
ساكن فراغ newFoldLevelWin(win_T *wp);
ساكن صحيح checkCloseRec(garray_T *gap, linenr_T lnum, صحيح level);
ساكن صحيح foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
ساكن صحيح foldLevelWin(win_T *wp, linenr_T lnum);
ساكن فراغ checkupdate(win_T *wp);
ساكن فراغ setFoldRepeat(linenr_T lnum, long count, صحيح do_open);
ساكن linenr_T setManualFold(linenr_T lnum, صحيح opening, صحيح recurse, صحيح *donep);
ساكن linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, صحيح opening, صحيح recurse, صحيح *donep);
ساكن فراغ foldOpenNested(fold_T *fpr);
ساكن فراغ deleteFoldEntry(garray_T *gap, صحيح idx, صحيح recursive);
ساكن فراغ foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
ساكن صحيح getDeepestNestingRecurse(garray_T *gap);
ساكن صحيح check_closed(win_T *win, fold_T *fp, صحيح *use_levelp, صحيح level, صحيح *maybe_smallp, linenr_T lnum_off);
ساكن فراغ checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
ساكن فراغ setSmallMaybe(garray_T *gap);
ساكن فراغ foldCreateMarkers(linenr_T start, linenr_T end);
ساكن فراغ foldAddMarker(linenr_T lnum, char_u *marker, صحيح markerlen);
ساكن فراغ deleteFoldMarkers(fold_T *fp, صحيح recursive, linenr_T lnum_off);
ساكن فراغ foldDelMarker(linenr_T lnum, char_u *marker, صحيح markerlen);
ساكن فراغ foldUpdateIEMS(win_T *wp, linenr_T top, linenr_T bot);
ساكن فراغ parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
ساكن linenr_T invalid_top = (linenr_T)0;
ساكن linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
ساكن linenr_T prev_lnum = 0;
ساكن صحيح prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

ساكن صحيح foldstartmarkerlen;
ساكن char_u *foldendmarker;
ساكن صحيح foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    فراغ
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
    صحيح
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    החזר (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    صحيح
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    החזר hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, عدم);
}

// hasFoldingWin() {{{2
    صحيح
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    صحيح		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    صحيح		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    صحيح		x;
    fold_T	*fp;
    صحيح		level = 0;
    صحيح		use_level = FALSE;
    صحيح		maybe_small = FALSE;
    garray_T	*gap;
    صحيح		low_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    אם (!hasAnyFolding(win))
    {
	אם (infop != عدم)
	    infop->fi_level = 0;
	החזר FALSE;
    }

    אם (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	אם (x >= 0)
	{
	    first = win->w_lines[x].wl_lnum;
	    last = win->w_lines[x].wl_lastlnum;
	    had_folded = win->w_lines[x].wl_folded;
	}
    }

    אם (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	עבור (;;)
	{
	    אם (!foldFind(gap, lnum_rel, &fp))
		اكسر;

	    // Remember lowest level of fold that starts in "lnum".
	    אם (lnum_rel == fp->fd_top && low_level == 0)
		low_level = level + 1;

	    first += fp->fd_top;
	    last += fp->fd_top;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    אם (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		اكسر;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_top;
	    ++level;
	}
    }

    אם (!had_folded)
    {
	אם (infop != عدم)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_low_level = low_level == 0 ? level : low_level;
	}
	החזר FALSE;
    }

    אם (last > win->w_buffer->b_ml.ml_line_count)
	last = win->w_buffer->b_ml.ml_line_count;
    אם (lastp != عدم)
	*lastp = last;
    אם (firstp != عدم)
	*firstp = first;
    אם (infop != عدم)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_low_level = low_level == 0 ? level + 1 : low_level;
    }
    החזר TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    صحيح
lineFolded(win_T *win, linenr_T lnum)
{
    החזר foldedCount(win, lnum, عدم) != 0;
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
foldedCount(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    אם (hasFoldingWin(win, lnum, عدم, &last, FALSE, infop))
	החזר (long)(last - lnum + 1);
    החזר 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    صحيح
foldmethodIsManual(win_T *wp)
{
    החזר (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    صحيح
foldmethodIsIndent(win_T *wp)
{
    החזר (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    صحيح
foldmethodIsExpr(win_T *wp)
{
    החזר (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    صحيح
foldmethodIsMarker(win_T *wp)
{
    החזר (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    صحيح
foldmethodIsSyntax(win_T *wp)
{
    החזר (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    صحيح
foldmethodIsDiff(win_T *wp)
{
    החזר (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    فراغ
closeFold(linenr_T lnum, long count)
{
    setFoldRepeat(lnum, count, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    فراغ
closeFoldRecurse(linenr_T lnum)
{
    (فراغ)setManualFold(lnum, FALSE, TRUE, عدم);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    فراغ
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    صحيح		opening,	// TRUE to open, FALSE to close
    صحيح		recurse,	// TRUE to do it recursively
    صحيح		had_visual)	// TRUE when Visual selection used
{
    صحيح		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_next;

    עבור (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	אם (opening && !recurse)
	    (فراغ)hasFolding(lnum, عدم, &lnum_next);
	(فراغ)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	אם (!opening && !recurse)
	    (فراغ)hasFolding(lnum, عدم, &lnum_next);
    }
    אם (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    אם (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    فراغ
openFold(linenr_T lnum, long count)
{
    setFoldRepeat(lnum, count, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    فراغ
openFoldRecurse(linenr_T lnum)
{
    (فراغ)setManualFold(lnum, TRUE, TRUE, عدم);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    فراغ
foldOpenCursor(فراغ)
{
    صحيح		done;

    checkupdate(curwin);
    אם (hasAnyFolding(curwin))
	עבור (;;)
	{
	    done = DONE_NOTHING;
	    (فراغ)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    אם (!(done & DONE_ACTION))
		اكسر;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    فراغ
newFoldLevel(فراغ)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    אם (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    אם (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    ساكن فراغ
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    صحيح		i;

    checkupdate(wp);
    אם (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_data;
	עבור (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    فراغ
foldCheckClose(فراغ)
{
    אם (*p_fcl == NUL)
	החזר;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    אם (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(صحيح)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    ساكن صحيح
checkCloseRec(garray_T *gap, linenr_T lnum, صحيح level)
{
    fold_T	*fp;
    صحيح		retval = FALSE;
    صحيح		i;

    fp = (fold_T *)gap->ga_data;
    עבור (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	אם (fp[i].fd_flags == FD_OPEN)
	{
	    אם (level <= 0 && (lnum < fp[i].fd_top
				      || lnum >= fp[i].fd_top + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    אחרת
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_top,
								   level - 1);
	}
    }
    החזר retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    صحيح
foldManualAllowed(صحيح create)
{
    אם (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	החזר TRUE;
    אם (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    אחרת
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    החזר FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    فراغ
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    صحيح		i, j;
    صحيح		cont;
    صحيح		use_level = FALSE;
    صحيح		closed = FALSE;
    صحيح		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    אם (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    אם (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	החזר;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    אם (gap->ga_len == 0)
	i = 0;
    אחרת
    {
	עבור (;;)
	{
	    אם (!foldFind(gap, start_rel, &fp))
		اكسر;
	    אם (fp->fd_top + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_top;
		end_rel -= fp->fd_top;
		אם (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    אם (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		אחרת אם (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    אחרת
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		اكسر;
	    }
	}
	אם (gap->ga_len == 0)
	    i = 0;
	אחרת
	    i = (صحيح)(fp - (fold_T *)gap->ga_data);
    }

    אם (ga_grow(gap, 1) == FAIL)
	החזר;

    fp = (fold_T *)gap->ga_data + i;
    ga_init2(&fold_ga, حجم(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    עבור (cont = 0; i + cont < gap->ga_len; ++cont)
	אם (fp[cont].fd_top > end_rel)
	    اكسر;
    אם (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	אם (start_rel > fp->fd_top)
	    start_rel = fp->fd_top;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	אם (end_rel < fp[cont - 1].fd_top + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_top + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_data, fp, حجم(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	עבור (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_data)[j].fd_top -= start_rel;
    }
    // Move remaining entries to after the new fold.
    אם (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_data + i,
		حجم(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_top = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    אם (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    אם (!use_level)
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
    فراغ
deleteFold(
    linenr_T	start,
    linenr_T	end,
    صحيح		recursive,
    صحيح		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = عدم;
    linenr_T	found_off = 0;
    صحيح		use_level;
    صحيح		maybe_small = FALSE;
    صحيح		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    صحيح		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    כלעוד (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = عدم;
	lnum_off = 0;
	use_level = FALSE;
	עבור (;;)
	{
	    אם (!foldFind(gap, lnum - lnum_off, &fp))
		اكسر;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    אם (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		اكسر;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_top;
	    ++level;
	}
	אם (found_ga == عدم)
	{
	    ++lnum;
	}
	אחרת
	{
	    lnum = found_fp->fd_top + found_fp->fd_len + found_off;

	    אם (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (صحيح)(found_fp - (fold_T *)found_ga->ga_data), recursive);
	    אחרת
	    {
		אם (first_lnum > found_fp->fd_top + found_off)
		    first_lnum = found_fp->fd_top + found_off;
		אם (last_lnum < lnum)
		    last_lnum = lnum;
		אם (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    אם (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	אם (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    אחרת
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    אם (last_lnum > 0)
	changed_lines(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    فراغ
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
    فراغ
foldUpdate(win_T *wp, linenr_T top, linenr_T bot)
{
    fold_T	*fp;

    אם (disable_fold_update > 0)
	החזר;
# ifdef FEAT_DIFF
    אם (need_diff_redraw)
	// will update later
	החזר;
# endif

    אם (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = top;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	אם (top > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = top;
	}
	(فراغ)foldFind(&wp->w_folds, maybe_small_start, &fp);
	כלעוד (fp < (fold_T *)wp->w_folds.ga_data + wp->w_folds.ga_len
		&& fp->fd_top <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    אם (foldmethodIsIndent(wp)
	    || foldmethodIsExpr(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	صحيح save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, top, bot);
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
    فراغ
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
    صحيح
foldMoveTo(
    صحيح		updown,
    صحيح		dir,	    // FORWARD or BACKWARD
    long	count)
{
    long	n;
    صحيح		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    صحيح		use_level;
    صحيح		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    صحيح		level;
    صحيح		last;

    checkupdate(curwin);

    // Repeat "count" times.
    עבור (n = 0; n < count; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	אם (gap->ga_len == 0)
	    اكسر;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	עבור (;;)
	{
	    אם (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		אם (!updown || gap->ga_len == 0)
		    اكسر;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		אם (dir == FORWARD)
		{
		    אם (fp - (fold_T *)gap->ga_data >= gap->ga_len)
			اكسر;
		    --fp;
		}
		אחרת
		{
		    אם (fp == (fold_T *)gap->ga_data)
			اكسر;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    אם (!last)
	    {
		// Check if this fold is closed.
		אם (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		אם (last && !updown)
		    اكسر;
	    }

	    אם (updown)
	    {
		אם (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    אם (fp + 1 - (fold_T *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_top + lnum_off;
			אם (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		אחרת
		{
		    // to end of previous fold if there is one
		    אם (fp > (fold_T *)gap->ga_data)
		    {
			lnum = fp[-1].fd_top + lnum_off + fp[-1].fd_len - 1;
			אם (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    אחרת
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		אם (dir == FORWARD)
		{
		    lnum = fp->fd_top + lnum_off + fp->fd_len - 1;
		    אם (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		אחרת
		{
		    lnum = fp->fd_top + lnum_off;
		    אם (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    אם (last)
		اكسر;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_top;
	    ++level;
	}
	אם (lnum_found != curwin->w_cursor.lnum)
	{
	    אם (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	אחרת
	    اكسر;
    }

    החזר retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    فراغ
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, حجم(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    صحيح
find_wl_entry(win_T *win, linenr_T lnum)
{
    صحيح		i;

    עבור (i = 0; i < win->w_lines_valid; ++i)
	אם (win->w_lines[i].wl_valid)
	{
	    אם (lnum < win->w_lines[i].wl_lnum)
		החזר -1;
	    אם (lnum <= win->w_lines[i].wl_lastlnum)
		החזר i;
	}
    החזר -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    فراغ
foldAdjustVisual(فراغ)
{
    pos_T	*start, *end;

    אם (!VIsual_active || !hasAnyFolding(curwin))
	החזר;

    אם (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    אחרת
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    אם (hasFolding(start->lnum, &start->lnum, عدم))
	start->col = 0;

    אם (!hasFolding(end->lnum, عدم, &end->lnum))
	החזר;

    end->col = ml_get_len(end->lnum);
    אם (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    אם (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    فراغ
foldAdjustCursor(فراغ)
{
    (فراغ)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, عدم);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    فراغ
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    صحيح		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    אם (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	החזר;

    from_p = (fold_T *)from->ga_data;
    to_p = (fold_T *)to->ga_data;

    עבור (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_top = from_p->fd_top;
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
    ساكن صحيح
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	low, high;
    fold_T	*fp;
    صحيح		i;

    אם (gap->ga_len == 0)
    {
	*fpp = عدم;
	החזר FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_data;
    low = 0;
    high = gap->ga_len - 1;
    כלעוד (low <= high)
    {
	i = (low + high) / 2;
	אם (fp[i].fd_top > lnum)
	    // fold below lnum, adjust high
	    high = i - 1;
	אחרת אם (fp[i].fd_top + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    low = i + 1;
	אחרת
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    החזר TRUE;
	}
    }
    *fpp = fp + low;
    החזר FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    ساكن صحيح
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    صحيح		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    עבור (;;)
    {
	אם (!foldFind(gap, lnum_rel, &fp))
	    اكسر;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_top;
	++level;
    }

    החזר level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    ساكن فراغ
checkupdate(win_T *wp)
{
    אם (!wp->w_foldinvalid)
	החזר;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ساكن فراغ
setFoldRepeat(linenr_T lnum, long count, صحيح do_open)
{
    صحيح		done;
    long	n;

    עבור (n = 0; n < count; ++n)
    {
	done = DONE_NOTHING;
	(فراغ)setManualFold(lnum, do_open, FALSE, &done);
	אם (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    אם (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    اكسر;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    ساكن linenr_T
setManualFold(
    linenr_T	lnum,
    صحيح		opening,    // TRUE when opening, FALSE when closing
    صحيح		recurse,    // TRUE when closing/opening recursive
    صحيح		*donep)
{
# ifdef FEAT_DIFF
    אם (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    אם (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		אם (dlnum != 0)
		    (فراغ)setManualFoldWin(wp, dlnum, opening, recurse, عدم);
	    }
	}
    }
# endif

    החזר setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    ساكن linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    صحيح		opening,    // TRUE when opening, FALSE when closing
    صحيح		recurse,    // TRUE when closing/opening recursive
    صحيح		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = عدم;
    صحيح		j;
    صحيح		level = 0;
    صحيح		use_level = FALSE;
    صحيح		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	next = MAXLNUM;
    linenr_T	off = 0;
    صحيح		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    עבור (;;)
    {
	אם (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    אם (fp != عدم && fp < (fold_T *)gap->ga_data + gap->ga_len)
		next = fp->fd_top + off;
	    اكسر;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	אם (fp + 1 < (fold_T *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_top + off;

	// Change from level-dependent folding to manual.
	אם (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    אם (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    אחרת
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_data;
	    עבור (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	אם (!opening && recurse)
	{
	    אם (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	אחרת אם (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    אם (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		אם (recurse)
		    foldOpenNested(fp);
	    }
	    اكسر;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_top;
	off += fp->fd_top;
	++level;
    }
    אם (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	אם (!opening && found != عدم)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	אם (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    אחרת אם (donep == عدم && wp == curwin)
	emsg(_(e_no_fold_found));

    אם (donep != عدم)
	*donep |= done;

    החזר next;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    ساكن فراغ
foldOpenNested(fold_T *fpr)
{
    صحيح		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_data;
    עבור (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    ساكن فراغ
deleteFoldEntry(garray_T *gap, صحيح idx, صحيح recursive)
{
    fold_T	*fp;
    صحيح		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_data + idx;
    אם (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	אם (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, حجم(fold_T) * (gap->ga_len - idx));
    }
    אחרת
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	אם (ga_grow(gap, (صحيح)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_data + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_data;
	    עבור (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_top += fp->fd_top;
		אם (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		אם (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    אם (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  حجم(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(حجم(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    فراغ
deleteFoldRecurse(garray_T *gap)
{
    صحيح		i;

    עבור (i = 0; i < gap->ga_len; ++i)
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
    فراغ
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    אם (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    אם (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    אם ((State & MODE_INSERT) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    ساكن فراغ
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    صحيح		i;
    linenr_T	last;
    linenr_T	top;

    אם (gap->ga_len == 0)
	החזר;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    אם ((State & MODE_INSERT) && amount == (linenr_T)1 && line2 == MAXLNUM)
	top = line1 + 1;
    אחרת
	top = line1;

    // Find the fold containing or just below "line1".
    (فراغ)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    עבור (i = (صحيح)(fp - (fold_T *)gap->ga_data); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_top + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	אם (last < line1)
	    استمر;

	// 6. fold below line2: only adjust for amount_after
	אם (fp->fd_top > line2)
	{
	    אם (amount_after == 0)
		اكسر;
	    fp->fd_top += amount_after;
	}
	אחרת
	{
	    אם (fp->fd_top >= top && last <= line2)
	    {
		// 4. fold completely contained in range
		אם (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		אחרת
		    fp->fd_top += amount;
	    }
	    אחרת
	    {
		אם (fp->fd_top < top)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_top,
				  line2 - fp->fd_top, amount, amount_after);
		    אם (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			אם (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_top;
			אחרת
			    fp->fd_len += amount;
		    }
		    אחרת
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		אחרת
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    אם (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_top,
				  amount,
				  amount_after + (fp->fd_top - top));
			fp->fd_len -= line2 - fp->fd_top + 1;
			fp->fd_top = line1;
		    }
		    אחרת
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_top,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_top += amount;
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
    صحيح
getDeepestNesting(فراغ)
{
    checkupdate(curwin);
    החזר getDeepestNestingRecurse(&curwin->w_folds);
}

    ساكن صحيح
getDeepestNestingRecurse(garray_T *gap)
{
    صحيح		i;
    صحيح		level;
    صحيح		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    עבור (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	אם (level > maxlevel)
	    maxlevel = level;
    }

    החזר maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    ساكن صحيح
check_closed(
    win_T	*win,
    fold_T	*fp,
    صحيح		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    صحيح		level,		    // folding depth
    صحيح		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    صحيح		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    אם (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	אם (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    אחרת אם (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    אם (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    אם (closed)
    {
	אם (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	אם (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    החזר closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    ساكن فراغ
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    صحيح		count;
    صحيح		n;

    אם (fp->fd_small != MAYBE)
	החזר;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    אם (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    אחרת
    {
	count = 0;
	עבור (n = 0; n < fp->fd_len; ++n)
	{
	    count += plines_win_nofold(wp, fp->fd_top + lnum_off + n);
	    אם (count > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		החזר;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    ساكن فراغ
setSmallMaybe(garray_T *gap)
{
    صحيح		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    עבור (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    ساكن فراغ
foldCreateMarkers(linenr_T start, linenr_T end)
{
    אם (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	החזר;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_lines(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    ساكن فراغ
foldAddMarker(linenr_T lnum, char_u *marker, صحيح markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*line;
    صحيح		line_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((حرف *)curbuf->b_p_cms, "%s");
    صحيح		line_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    line = ml_get(lnum);
    line_len = ml_get_len(lnum);

    אם (u_save(lnum - 1, lnum + 1) != OK)
	החזר;

    // Check if the line ends with an unclosed comment
    (فراغ)skip_comment(line, FALSE, FALSE, &line_is_comment);
    newline = alloc(line_len + markerlen + STRLEN(cms) + 1);
    אם (newline == عدم)
	החזר;
    STRCPY(newline, line);
    // Append the marker to the end of the line
    אם (p == عدم || line_is_comment)
	vim_strncpy(newline + line_len, marker, markerlen);
    אחרת
    {
	STRCPY(newline + line_len, cms);
	STRNCPY(newline + line_len + (p - cms), marker, markerlen);
	STRCPY(newline + line_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    ساكن فراغ
deleteFoldMarkers(
    fold_T	*fp,
    صحيح		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    صحيح		i;

    אם (recursive)
	עבור (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_top);
    foldDelMarker(fp->fd_top + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_top + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    ساكن فراغ
foldDelMarker(linenr_T lnum, char_u *marker, صحيح markerlen)
{
    char_u	*line;
    char_u	*newline;
    char_u	*p;
    صحيح		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    אם (lnum > curbuf->b_ml.ml_line_count)
	החזר;
    line = ml_get(lnum);
    עבור (p = line; *p != NUL; ++p)
	אם (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    אם (VIM_ISDIGIT(p[len]))
		++len;
	    אם (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((حرف *)cms, "%s");
		אם (cms2 != عدم && p - line >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (صحيح)STRLEN(cms) - 2;
		}
	    }
	    אם (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		אם (newline != عدم)
		{
		    STRNCPY(newline, line, p - line);
		    STRCPY(newline + (p - line), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    اكسر;
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
    char_u	*text = عدم;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    ساكن صحيح	    got_fdt_error = FALSE;
    صحيح		    save_did_emsg = did_emsg;
    ساكن win_T    *last_wp = عدم;
    ساكن linenr_T last_lnum = 0;

    אם (last_wp != wp || last_wp == عدم
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    אם (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    אם (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	صحيح	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	אם (level > (صحيح)حجم(dashes) - 1)
	    level = (صحيح)حجم(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	אם (!got_fdt_error)
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

	    אם (text == عدم || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, عدم, -1);

	אם (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	אם (text != عدم)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    עבור (p = text; *p != NUL; ++p)
	    {
		صحيح	len;

		אם (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    אם (!vim_isprintc((*mb_ptr2char)(p)))
			اكسر;
		    p += len - 1;
		}
		אחרת
		    אם (*p == TAB)
			*p = ' ';
		    אחרת אם (ptr2cells(p) > 1)
			اكسر;
	    }
	    אם (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    אם (text == عدم)
# endif
    {
	long count = (long)(lnume - lnum + 1);

	vim_snprintf((حرف *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", count),
		     count);
	text = buf;
    }
    החזר text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    ساكن فراغ
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    صحيح		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    صحيح		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    صحيح		len;
    صحيح		did1 = FALSE;
    صحيح		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (صحيح)STRLEN(cms_start);
    כלעוד (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((حرف *)cms_start, "%s");
    אם (cms_end != عدم)
    {
	cms_elen = cms_slen - (صحيح)(cms_end - cms_start);
	cms_slen = (صحيح)(cms_end - cms_start);

	// exclude white space before "%s"
	כלעוד (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (صحيح)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    עבור (s = str; *s != NUL; )
    {
	len = 0;
	אם (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	אחרת אם (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	אם (len > 0)
	{
	    אם (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    עבור (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    אם (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (صحيح)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	אחרת אם (cms_end != عدم)
	{
	    אם (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    אחרת אם (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	אם (len != 0)
	{
	    כלעוד (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	אחרת
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
تعريفنوع بنية
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    صحيح		lvl;		// current level (-1 for undefined)
    صحيح		lvl_next;	// level used for next line
    صحيح		start;		// number of folds that are forced to start at
				// this line.
    صحيح		end;		// level of fold that is forced to end below
				// this line
    صحيح		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
ساكن صحيح fold_changed;

// Function declarations. {{{2
ساكن linenr_T foldUpdateIEMSRecurse(garray_T *gap, صحيح level, linenr_T startlnum, fline_T *flp, فراغ (*getlevel)(fline_T *), linenr_T bot, صحيح topflags);
ساكن صحيح foldInsert(garray_T *gap, صحيح i);
ساكن فراغ foldSplit(garray_T *gap, صحيح i, linenr_T top, linenr_T bot);
ساكن فراغ foldRemove(garray_T *gap, linenr_T top, linenr_T bot);
ساكن فراغ foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
ساكن فراغ foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
ساكن فراغ foldlevelDiff(fline_T *flp);
# endif
ساكن فراغ foldlevelExpr(fline_T *flp);
ساكن فراغ foldlevelMarker(fline_T *flp);
ساكن فراغ foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    ساكن فراغ
foldUpdateIEMS(win_T *wp, linenr_T top, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    فراغ	(*getlevel)(fline_T *);
    صحيح		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    אם (invalid_top != (linenr_T)0)
	החזר;

    אם (wp->w_foldinvalid)
    {
	// Need to update all folds.
	top = 1;
	bot = wp->w_buffer->b_ml.ml_line_count;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    אם (foldmethodIsDiff(wp))
    {
	אם (top > diff_context)
	    top -= diff_context;
	אחרת
	    top = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    אם (top > wp->w_buffer->b_ml.ml_line_count)
	top = wp->w_buffer->b_ml.ml_line_count;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_top = top;
    invalid_bot = bot;

    אם (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	אם (top > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, top - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = top - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    אם (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_next);
	    אחרת
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = top;
	getlevel(&fline);
    }
    אחרת
    {
	fline.lnum = top;
	אם (foldmethodIsExpr(wp))
	{
	    getlevel = foldlevelExpr;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    אם (top > 1)
		--fline.lnum;
	}
	אחרת אם (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	אחרת אם (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	אחרת
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    אם (top > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	עבור ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    אם (fline.lvl >= 0)
		اكسر;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    אם (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = عدم;
	صحيح	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	כלעוד (current_fdl < fline.lvl)
	{
	    אם (!foldFind(gap, lnum_rel, &fpn))
		اكسر;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_top;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_top;
	}
	אם (fpn != عدم && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    אם (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    אם (start > end && end < wp->w_buffer->b_ml.ml_line_count)
	end = start;
    כלעוד (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	אם (fline.lnum > wp->w_buffer->b_ml.ml_line_count)
	    اكسر;
	אם (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    אם (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevelExpr)
		اكسر;
	    אם ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_top + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_top < fline.lnum))
		end = fp->fd_top + fp->fd_len - 1;
	    אחרת אם (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    אחרת
		اكسر;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	אם (fline.lvl > 0)
	{
	    invalid_top = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	אחרת
	{
	    אם (fline.lnum == wp->w_buffer->b_ml.ml_line_count)
		اكسر;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    אם (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    אם (end != bot)
	redraw_win_range_later(wp, top, end);

    invalid_top = (linenr_T)0;
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
    ساكن linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    صحيح		level,
    linenr_T	startlnum,
    fline_T	*flp,
    فراغ	(*getlevel)(fline_T *),
    linenr_T	bot,
    صحيح		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = عدم;
    fold_T	*fp2;
    صحيح		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    صحيح		i;
    صحيح		finish = FALSE;
    linenr_T	linecount = flp->wp->w_buffer->b_ml.ml_line_count - flp->off;
    صحيح		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    אם (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(فراغ)foldFind(gap, startlnum - 1, &fp);
	אם (fp != عدم && (fp >= ((fold_T *)gap->ga_data) + gap->ga_len
						   || fp->fd_top >= startlnum))
	    fp = عدم;
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
    כלעוד (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	line_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	אם (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	אם (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	אם (flp->lnum > bot && !finish && fp != عدم)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    אם (getlevel != foldlevelMarker
		    && getlevel != foldlevelExpr
		    && getlevel != foldlevelSyntax)
		اكسر;
	    i = 0;
	    fp2 = fp;
	    אם (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_top;
		כלעוד (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_top;
		}
	    }
	    אם (lvl < level + i)
	    {
		(فراغ)foldFind(&fp->fd_nested, flp->lnum - fp->fd_top, &fp2);
		אם (fp2 != عدم)
		    bot = fp2->fd_top + fp2->fd_len - 1 + fp->fd_top;
	    }
	    אחרת אם (fp->fd_top + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    אחרת
		اكسر;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	אם (fp == عدم
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
	    כלעוד (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		אם (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		אחרת
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		אם (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && fp->fd_top <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_top < flp->lnum)
				|| (lvl >= level
					   && fp->fd_top <= flp->lnum_save)))))
		{
		    אם (fp->fd_top + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			אם (fp->fd_top == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			אחרת אם (fp->fd_top >= startlnum)
			{
			    אם (fp->fd_top > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_top - firstlnum), 0L);
			    אחרת
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_top - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_top - firstlnum));
			    fp->fd_len += fp->fd_top - firstlnum;
			    fp->fd_top = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			אחרת אם ((flp->start != 0 && lvl == level)
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
			    אם (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    אחרת
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_top,
						      breakend - fp->fd_top);
			    i = (صحيح)(fp - (fold_T *)gap->ga_data);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_data + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    אם (getlevel == foldlevelMarker
				    || getlevel == foldlevelExpr
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			אם (fp->fd_top == startlnum && concat)
			{
			    i = (صحيح)(fp - (fold_T *)gap->ga_data);
			    אם (i != 0)
			    {
				fp2 = fp - 1;
				אם (fp2->fd_top + fp2->fd_len == fp->fd_top)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			اكسر;
		    }
		    אם (fp->fd_top >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (صحيح)(fp - (fold_T *)gap->ga_data), TRUE);
		    }
		    אחרת
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_top;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		אחרת
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    אם (gap->ga_len == 0)
			i = 0;
		    אחרת
			i = (صحيح)(fp - (fold_T *)gap->ga_data);
		    אם (foldInsert(gap, i) != OK)
			החזר bot;
		    fp = (fold_T *)gap->ga_data + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_top = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    אם (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    אחרת אם (i <= 0)
		    {
			fp->fd_flags = topflags;
			אם (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    אחרת
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    אם (getlevel == foldlevelMarker
			    || getlevel == foldlevelExpr
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    اكسر;
		}
	    }
	}

	אם (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    اكسر;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	אם (lvl > level && fp != عدم)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    אם (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_top;
	    flp->off += fp->fd_top;
	    i = (صحيح)(fp - (fold_T *)gap->ga_data);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_top, flp, getlevel,
					      bot - fp->fd_top, fp->fd_flags);
	    fp = (fold_T *)gap->ga_data + i;
	    flp->lnum += fp->fd_top;
	    flp->lnum_save += fp->fd_top;
	    flp->off -= fp->fd_top;
	    bot += fp->fd_top;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	אחרת
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    כלעוד (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		אם (++flp->lnum > linecount)
		    اكسر;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		אם (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    اكسر;
	    }
	    prev_lnum = 0;
	    אם (flp->lnum > linecount)
		اكسر;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    אם (fp == عدم)	// only happens when got_int is set
	החזר bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    אם (fp->fd_len < flp->lnum - fp->fd_top)
    {
	fp->fd_len = flp->lnum - fp->fd_top;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    אחרת אם (fp->fd_top + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_top + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_top,
						  flp->lnum - 1 - fp->fd_top);

    אם (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	אם (fp->fd_len != flp->lnum - fp->fd_top)
	{
	    אם (fp->fd_top + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		אם (getlevel == foldlevelMarker
			|| getlevel == foldlevelExpr
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_top + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_top;
		}
		אחרת
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (صحيح)(fp - (fold_T *)gap->ga_data);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_data + i;
		}
	    }
	    אחרת
		fp->fd_len = flp->lnum - fp->fd_top;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    עבור (;;)
    {
	fp2 = fp + 1;
	אם (fp2 >= (fold_T *)gap->ga_data + gap->ga_len
						  || fp2->fd_top > flp->lnum)
	    اكسر;
	אם (fp2->fd_top + fp2->fd_len > flp->lnum)
	{
	    אם (fp2->fd_top < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_top - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_top - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_top;
		fp2->fd_top = flp->lnum;
		fold_changed = TRUE;
	    }

	    אם (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    اكسر;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (صحيح)(fp2 - (fold_T *)gap->ga_data), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    אם (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    החזר bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    ساكن صحيح
foldInsert(garray_T *gap, صحيح i)
{
    fold_T	*fp;

    אם (ga_grow(gap, 1) == FAIL)
	החזר FAIL;
    fp = (fold_T *)gap->ga_data + i;
    אם (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, حجم(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, حجم(fold_T), 10);
    החזר OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    ساكن فراغ
foldSplit(
    garray_T	*gap,
    صحيح		i,
    linenr_T	top,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    صحيح		idx;
    صحيح		len;

    // The fold continues below bot, need to split it.
    אם (foldInsert(gap, i + 1) == FAIL)
	החזר;
    fp = (fold_T *)gap->ga_data + i;
    fp[1].fd_top = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_top - fp->fd_top);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (فراغ)foldFind(gap1, bot + 1 - fp->fd_top, &fp2);
    אם (fp2 != عدم)
    {
	len = (صحيح)((fold_T *)gap1->ga_data + gap1->ga_len - fp2);
	אם (len > 0 && ga_grow(gap2, len) == OK)
	{
	    עבור (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_data)[idx] = fp2[idx];
		((fold_T *)gap2->ga_data)[idx].fd_top
						  -= fp[1].fd_top - fp->fd_top;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = top - fp->fd_top;
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
    ساكن فراغ
foldRemove(garray_T *gap, linenr_T top, linenr_T bot)
{
    fold_T	*fp = عدم;

    אם (bot < top)
	החזר;		// nothing to do

    כלעוד (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	אם (foldFind(gap, top, &fp) && fp->fd_top < top)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, top - fp->fd_top, bot - fp->fd_top);
	    אם (fp->fd_top + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (صحيح)(fp - (fold_T *)gap->ga_data), top, bot);
	    }
	    אחרת
	    {
		// 2: truncate fold at "top".
		fp->fd_len = top - fp->fd_top;
	    }
	    fold_changed = TRUE;
	    استمر;
	}
	אם (fp >= (fold_T *)(gap->ga_data) + gap->ga_len
		|| fp->fd_top > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    اكسر;
	}
	אם (fp->fd_top >= top)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    אם (fp->fd_top + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_top),
			(linenr_T)MAXLNUM, (long)(fp->fd_top - bot - 1));
		fp->fd_len -= bot - fp->fd_top + 1;
		fp->fd_top = bot + 1;
		اكسر;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (صحيح)(fp - (fold_T *)gap->ga_data), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    ساكن فراغ
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *left, *right;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    עבור (; start < end; start++, end--)
    {
	left = (fold_T *)gap->ga_data + start;
	right = (fold_T *)gap->ga_data + end;
	tmp  = *left;
	*left = *right;
	*right = tmp;
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

    ساكن فراغ
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_top, MAXLNUM);
    fp->fd_len = end - fp->fd_top;
}

# define fold_end(fp) ((fp)->fd_top + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_data + (gap)->ga_len))
# define fold_index(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_data)))

    فراغ
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    صحيح at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_index = 0;

    אם (at_start)
    {
	אם (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_top, line2 -
		    fp->fd_top, dest - fp->fd_top);
	    החזר;
	}
	אחרת אם (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_top, line2 -
		    fp->fd_top, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	אחרת
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    אם (!valid_fold(fp, gap) || fp->fd_top > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	החזר;
    }
    אחרת אם (fp->fd_top > line2)
    {
	עבור (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_top -= range_len;

	אם (valid_fold(fp, gap) && fp->fd_top <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_top -= range_len;
	}
	החזר;
    }
    אחרת אם (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_top, dest -
		fp->fd_top, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_top += move_len;
	החזר;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_index(fp, gap);

    עבור (; valid_fold(fp, gap) && fp->fd_top <= dest; fp++)
    {
	אם (fp->fd_top <= line2)
	{
	    // 1. 2. or 3.
	    אם (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_top += move_len;
	    استمر;
	}

	// Record index of the first fold after the moved range.
	אם (move_end == 0)
	    move_end = fold_index(fp, gap);

	אם (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_top -= range_len;
    }

    dest_index = fold_index(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    אם (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	החזר;
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
    ساكن فراغ
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    صحيح		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    אם (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    אם (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	עבור (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_data)[gap1->ga_len]
					= ((fold_T *)gap2->ga_data)[idx];
	    ((fold_T *)gap1->ga_data)[gap1->ga_len].fd_top += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (صحيح)(fp2 - (fold_T *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ساكن فراغ
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    אם (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != عدم)
    {
	// first and last line can't be undefined, use level 0
	אם (lnum == 1 || lnum == buf->b_ml.ml_line_count)
	    flp->lvl = 0;
	אחרת
	    flp->lvl = -1;
    }
    אחרת
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_value(buf);
    אם (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	אם (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    ساكن فراغ
foldlevelDiff(fline_T *flp)
{
    אם (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    אחרת
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ساكن فراغ
foldlevelExpr(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# אחרת
    win_T	*win;
    صحيح		n;
    صحيح		c;
    linenr_T	lnum = flp->lnum + flp->off;
    صحيح		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    אם (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = KeyTyped;
    n = eval_foldexpr(flp->wp, &c);
    KeyTyped = save_keytyped;

    بدل (c)
    {
	// "a1", "a2", .. : add to the fold level
	حالة 'a': אם (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  اكسر;

	// "s1", "s2", .. : subtract from the fold level
	حالة 's': אם (flp->lvl >= 0)
		  {
		      אם (n > flp->lvl)
			  flp->lvl_next = 0;
		      אחרת
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  اكسر;

	// ">1", ">2", .. : start a fold with a certain level
	حالة '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  اكسر;

	// "<1", "<2", .. : end a fold with a certain level
	حالة '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_next = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  اكسر;

	// "=": No change in level
	حالة '=': flp->lvl_next = flp->lvl;
		  اكسر;

	// "-1", "0", "1", ..: set fold level
	افتراضي:  אם (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_next = flp->lvl;
		  אחרת
		      flp->lvl_next = n;
		  flp->lvl = n;
		  اكسر;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    אם (flp->lvl < 0)
    {
	אם (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	אם (lnum == curbuf->b_ml.ml_line_count)
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
    ساكن فراغ
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (صحيح)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (صحيح)STRLEN(foldendmarker);
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
    ساكن فراغ
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    صحيح		cstart;
    صحيح		cend;
    صحيح		start_lvl = flp->lvl;
    char_u	*s;
    صحيح		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    כלעוד (*s)
    {
	אם (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    אם (VIM_ISDIGIT(*s))
	    {
		n = atoi((حرف *)s);
		אם (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    אם (n <= start_lvl)
			flp->start = 1;
		    אחרת
			flp->start = n - start_lvl;
		}
	    }
	    אחרת
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	אחרת אם (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    אם (VIM_ISDIGIT(*s))
	    {
		n = atoi((حرف *)s);
		אם (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    // never start a fold with an end marker
		    אם (flp->lvl_next > start_lvl)
			flp->lvl_next = start_lvl;
		}
	    }
	    אחרת
		--flp->lvl_next;
	}
	אחרת
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    אם (flp->lvl_next < 0)
	flp->lvl_next = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    ساكن فراغ
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# אחרת
    linenr_T	lnum = flp->lnum + flp->off;
    صحيح		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    אם (lnum < flp->wp->w_buffer->b_ml.ml_line_count)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	אם (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# אם defined(FEAT_SESSION)
ساكن صحيح put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
ساكن صحيح put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
ساكن صحيح put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    صحيح
put_folds(FILE *fd, win_T *wp)
{
    אם (foldmethodIsManual(wp))
    {
	אם (put_line(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_line(fd, "&fdl = &fdl") == FAIL)
	    החזר FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    אם (wp->w_fold_manual)
	החזר put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    החזר OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    ساكن صحيح
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    صحيح		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    עבור (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	אם (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_top) == FAIL)
	    החזר FAIL;
	אם (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_top + off,
					fp->fd_top + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    החזר FAIL;
	++fp;
    }
    החזר OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    ساكن صحيح
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    صحيح		i;
    صحيح		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    עבור (i = 0; i < gap->ga_len; i++)
    {
	אם (fp->fd_flags != FD_LEVEL)
	{
	    אם (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		אם (fprintf(fd, ":%ld", fp->fd_top + off) < 0
			|| put_eol(fd) == FAIL
			|| put_line(fd, "sil! normal! zo") == FAIL)
		    החזר FAIL;
		אם (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_top)
			== FAIL)
		    החזר FAIL;
		// close the parent when needed
		אם (fp->fd_flags == FD_CLOSED)
		{
		    אם (put_fold_open_close(fd, fp, off) == FAIL)
			החזר FAIL;
		}
	    }
	    אחרת
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_top);
		אם ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		אם (put_fold_open_close(fd, fp, off) == FAIL)
		    החזר FAIL;
	    }
	}
	++fp;
    }

    החזר OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    ساكن صحيح
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    אם (fprintf(fd, ":%ld", fp->fd_top + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	החזר FAIL;

    החזר OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#אם defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    ساكن فراغ
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    صحيح		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    אם (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	החזר;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_number(&argvars[1]), &tp);
	אם (wp != عدم && tp != عدم)
	{
	    אם (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	אחרת
	{
	    rettv->vval.v_number = -2;
	    החזר;
	}
    }
    אחרת
	lnum = tv_get_lnum(argvars);

    אם (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_line_count)
    {
	אם (hasFoldingWin(wp, lnum, &first, &last, FALSE, عدم))
	{
	    אם (end)
		rettv->vval.v_number = (varnumber_T)last;
	    אחרת
		rettv->vval.v_number = (varnumber_T)first;
	    החזר;
	}
    }
# endif
    rettv->vval.v_number = -1;
}

/*
 * "foldclosed()" function
 */
    فراغ
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    فراغ
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    ساكن صحيح
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    אם (invalid_top == (linenr_T)0)
	checkupdate(curwin);
    אחרת אם (lnum == prev_lnum && prev_lnum_lvl >= 0)
	החזר prev_lnum_lvl;
    אחרת אם (lnum >= invalid_top && lnum <= invalid_bot)
	החזר -1;

    // Return quickly when there is no folding at all in this window.
    אם (!hasAnyFolding(curwin))
	החזר 0;

    החזר foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    فراغ
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    אם (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	החזר;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_number(&argvars[1]), &tp);
	אם (wp != عدم && tp != عدم)
	{
	    אם (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		אם (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
		    rettv->vval.v_number = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	אחרת
	    rettv->vval.v_number = -2;
    }
    אחרת
    {
	lnum = tv_get_lnum(argvars);
	אם (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
	    rettv->vval.v_number = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    فراغ
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    صحيح		len;
    حرف	*txt;
    long	count;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    אם (foldstart > 0 && foldend <= curbuf->b_ml.ml_line_count
	    && dashes != عدم)
    {
	// Find first non-empty line in the fold.
	עבור (lnum = foldstart; lnum < foldend; ++lnum)
	    אם (!linewhite(lnum))
		اكسر;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	אם (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    אם (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		אם (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	count = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", count);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	אם (r != عدم)
	{
	    sprintf((حرف *)r, txt, dashes, count);
	    len = (صحيح)STRLEN(r);
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
    فراغ
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    صحيح		fold_count;
    win_T	*wp  = curwin;
    ساكن صحيح	entered = FALSE;
    אם (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	החזר;

    אם (entered)
	החזר; // reject recursive use
    entered = TRUE;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_number(&argvars[1]), &tp);
	אם (wp == عدم || tp == عدم) // wrong winid
	{
	    entered = FALSE;
	    החזר;
	}
	אם (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    אחרת
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    אם (lnum < 0)
	lnum = 0;
    fold_count = foldedCount(wp, lnum, &foldinfo);
    אם (fold_count > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_count - 1,
							       &foldinfo, buf);
	אם (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
