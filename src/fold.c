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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
प्रकारनिर्देश संरचना
{
    linenr_T	fd_शिखरम्;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    वर्ण	fd_flags;	// see below
    वर्ण	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
स्थिर शून्य newFoldLevelWin(win_T *wp);
स्थिर पूर्णाङ्क checkCloseRec(garray_T *gap, linenr_T lnum, पूर्णाङ्क level);
स्थिर पूर्णाङ्क foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
स्थिर पूर्णाङ्क foldLevelWin(win_T *wp, linenr_T lnum);
स्थिर शून्य checkupdate(win_T *wp);
स्थिर शून्य setFoldRepeat(linenr_T lnum, long गणनम्, पूर्णाङ्क do_open);
स्थिर linenr_T setManualFold(linenr_T lnum, पूर्णाङ्क opening, पूर्णाङ्क recurse, पूर्णाङ्क *donep);
स्थिर linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, पूर्णाङ्क opening, पूर्णाङ्क recurse, पूर्णाङ्क *donep);
स्थिर शून्य foldOpenNested(fold_T *fpr);
स्थिर शून्य deleteFoldEntry(garray_T *gap, पूर्णाङ्क idx, पूर्णाङ्क recursive);
स्थिर शून्य foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
स्थिर पूर्णाङ्क getDeepestNestingRecurse(garray_T *gap);
स्थिर पूर्णाङ्क check_closed(win_T *win, fold_T *fp, पूर्णाङ्क *use_levelp, पूर्णाङ्क level, पूर्णाङ्क *maybe_smallp, linenr_T lnum_off);
स्थिर शून्य checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
स्थिर शून्य setSmallMaybe(garray_T *gap);
स्थिर शून्य foldCreateMarkers(linenr_T start, linenr_T end);
स्थिर शून्य foldAddMarker(linenr_T lnum, char_u *marker, पूर्णाङ्क markerlen);
स्थिर शून्य deleteFoldMarkers(fold_T *fp, पूर्णाङ्क recursive, linenr_T lnum_off);
स्थिर शून्य foldDelMarker(linenr_T lnum, char_u *marker, पूर्णाङ्क markerlen);
स्थिर शून्य foldUpdateIEMS(win_T *wp, linenr_T शिखरम्, linenr_T bot);
स्थिर शून्य parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
स्थिर linenr_T invalid_शिखरम् = (linenr_T)0;
स्थिर linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
स्थिर linenr_T prev_lnum = 0;
स्थिर पूर्णाङ्क prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

स्थिर पूर्णाङ्क foldstartmarkerlen;
स्थिर char_u *foldendmarker;
स्थिर पूर्णाङ्क foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    शून्य
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
    पूर्णाङ्क
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    पटिनिवत्त (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    पूर्णाङ्क
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    पटिनिवत्त hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, रिक्त);
}

// hasFoldingWin() {{{2
    पूर्णाङ्क
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    पूर्णाङ्क		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    पूर्णाङ्क		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    पूर्णाङ्क		x;
    fold_T	*fp;
    पूर्णाङ्क		level = 0;
    पूर्णाङ्क		use_level = FALSE;
    पूर्णाङ्क		maybe_small = FALSE;
    garray_T	*gap;
    पूर्णाङ्क		नीचः_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    सचे (!hasAnyFolding(win))
    {
	सचे (infop != रिक्त)
	    infop->fi_level = 0;
	पटिनिवत्त FALSE;
    }

    सचे (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	सचे (x >= 0)
	{
	    first = win->w_पङ्क्तयः[x].wl_lnum;
	    last = win->w_पङ्क्तयः[x].wl_lastlnum;
	    had_folded = win->w_पङ्क्तयः[x].wl_folded;
	}
    }

    सचे (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	अत्थाय (;;)
	{
	    सचे (!foldFind(gap, lnum_rel, &fp))
		भङ्ग;

	    // Remember lowest level of fold that starts in "lnum".
	    सचे (lnum_rel == fp->fd_शिखरम् && नीचः_level == 0)
		नीचः_level = level + 1;

	    first += fp->fd_शिखरम्;
	    last += fp->fd_शिखरम्;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    सचे (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		भङ्ग;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_शिखरम्;
	    ++level;
	}
    }

    सचे (!had_folded)
    {
	सचे (infop != रिक्त)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_नीचः_level = नीचः_level == 0 ? level : नीचः_level;
	}
	पटिनिवत्त FALSE;
    }

    सचे (last > win->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	last = win->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
    सचे (lastp != रिक्त)
	*lastp = last;
    सचे (firstp != रिक्त)
	*firstp = first;
    सचे (infop != रिक्त)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_नीचः_level = नीचः_level == 0 ? level + 1 : नीचः_level;
    }
    पटिनिवत्त TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    पूर्णाङ्क
पङ्क्तिः_Folded(win_T *win, linenr_T lnum)
{
    पटिनिवत्त folded_गणनम्(win, lnum, रिक्त) != 0;
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
folded_गणनम्(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    सचे (hasFoldingWin(win, lnum, रिक्त, &last, FALSE, infop))
	पटिनिवत्त (long)(last - lnum + 1);
    पटिनिवत्त 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    पूर्णाङ्क
foldmethodIsManual(win_T *wp)
{
    पटिनिवत्त (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    पूर्णाङ्क
foldmethodIsIndent(win_T *wp)
{
    पटिनिवत्त (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    पूर्णाङ्क
foldmethod_Is_व्यञ्जकम्(win_T *wp)
{
    पटिनिवत्त (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    पूर्णाङ्क
foldmethodIsMarker(win_T *wp)
{
    पटिनिवत्त (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    पूर्णाङ्क
foldmethodIsSyntax(win_T *wp)
{
    पटिनिवत्त (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    पूर्णाङ्क
foldmethodIsDiff(win_T *wp)
{
    पटिनिवत्त (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    शून्य
closeFold(linenr_T lnum, long गणनम्)
{
    setFoldRepeat(lnum, गणनम्, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    शून्य
closeFoldRecurse(linenr_T lnum)
{
    (शून्य)setManualFold(lnum, FALSE, TRUE, रिक्त);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    शून्य
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    पूर्णाङ्क		opening,	// TRUE to open, FALSE to close
    पूर्णाङ्क		recurse,	// TRUE to do it recursively
    पूर्णाङ्क		had_visual)	// TRUE when Visual selection used
{
    पूर्णाङ्क		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_next;

    अत्थाय (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	सचे (opening && !recurse)
	    (शून्य)hasFolding(lnum, रिक्त, &lnum_next);
	(शून्य)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	सचे (!opening && !recurse)
	    (शून्य)hasFolding(lnum, रिक्त, &lnum_next);
    }
    सचे (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    सचे (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    शून्य
openFold(linenr_T lnum, long गणनम्)
{
    setFoldRepeat(lnum, गणनम्, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    शून्य
openFoldRecurse(linenr_T lnum)
{
    (शून्य)setManualFold(lnum, TRUE, TRUE, रिक्त);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    शून्य
foldOpenCursor(शून्य)
{
    पूर्णाङ्क		done;

    checkupdate(curwin);
    सचे (hasAnyFolding(curwin))
	अत्थाय (;;)
	{
	    done = DONE_NOTHING;
	    (शून्य)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    सचे (!(done & DONE_ACTION))
		भङ्ग;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    शून्य
newFoldLevel(शून्य)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    सचे (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    सचे (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    स्थिर शून्य
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    पूर्णाङ्क		i;

    checkupdate(wp);
    सचे (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_data;
	अत्थाय (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    शून्य
foldCheckClose(शून्य)
{
    सचे (*p_fcl == NUL)
	पटिनिवत्त;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    सचे (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(पूर्णाङ्क)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    स्थिर पूर्णाङ्क
checkCloseRec(garray_T *gap, linenr_T lnum, पूर्णाङ्क level)
{
    fold_T	*fp;
    पूर्णाङ्क		retval = FALSE;
    पूर्णाङ्क		i;

    fp = (fold_T *)gap->ga_data;
    अत्थाय (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	सचे (fp[i].fd_flags == FD_OPEN)
	{
	    सचे (level <= 0 && (lnum < fp[i].fd_शिखरम्
				      || lnum >= fp[i].fd_शिखरम् + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    अञ्ञथा
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_शिखरम्,
								   level - 1);
	}
    }
    पटिनिवत्त retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    पूर्णाङ्क
foldManualAllowed(पूर्णाङ्क create)
{
    सचे (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	पटिनिवत्त TRUE;
    सचे (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    अञ्ञथा
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    पटिनिवत्त FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    शून्य
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    पूर्णाङ्क		i, j;
    पूर्णाङ्क		cont;
    पूर्णाङ्क		use_level = FALSE;
    पूर्णाङ्क		closed = FALSE;
    पूर्णाङ्क		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    सचे (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    सचे (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	पटिनिवत्त;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    सचे (gap->ga_len == 0)
	i = 0;
    अञ्ञथा
    {
	अत्थाय (;;)
	{
	    सचे (!foldFind(gap, start_rel, &fp))
		भङ्ग;
	    सचे (fp->fd_शिखरम् + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_शिखरम्;
		end_rel -= fp->fd_शिखरम्;
		सचे (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    सचे (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		अञ्ञथा सचे (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    अञ्ञथा
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		भङ्ग;
	    }
	}
	सचे (gap->ga_len == 0)
	    i = 0;
	अञ्ञथा
	    i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data);
    }

    सचे (ga_grow(gap, 1) == FAIL)
	पटिनिवत्त;

    fp = (fold_T *)gap->ga_data + i;
    ga_init2(&fold_ga, परिमाण(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    अत्थाय (cont = 0; i + cont < gap->ga_len; ++cont)
	सचे (fp[cont].fd_शिखरम् > end_rel)
	    भङ्ग;
    सचे (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	सचे (start_rel > fp->fd_शिखरम्)
	    start_rel = fp->fd_शिखरम्;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	सचे (end_rel < fp[cont - 1].fd_शिखरम् + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_शिखरम् + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_data, fp, परिमाण(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	अत्थाय (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_data)[j].fd_शिखरम् -= start_rel;
    }
    // Move remaining entries to after the new fold.
    सचे (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_data + i,
		परिमाण(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_शिखरम् = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    सचे (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    सचे (!use_level)
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
    शून्य
deleteFold(
    linenr_T	start,
    linenr_T	end,
    पूर्णाङ्क		recursive,
    पूर्णाङ्क		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = रिक्त;
    linenr_T	found_off = 0;
    पूर्णाङ्क		use_level;
    पूर्णाङ्क		maybe_small = FALSE;
    पूर्णाङ्क		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    पूर्णाङ्क		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    याव (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = रिक्त;
	lnum_off = 0;
	use_level = FALSE;
	अत्थाय (;;)
	{
	    सचे (!foldFind(gap, lnum - lnum_off, &fp))
		भङ्ग;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    सचे (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		भङ्ग;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_शिखरम्;
	    ++level;
	}
	सचे (found_ga == रिक्त)
	{
	    ++lnum;
	}
	अञ्ञथा
	{
	    lnum = found_fp->fd_शिखरम् + found_fp->fd_len + found_off;

	    सचे (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (पूर्णाङ्क)(found_fp - (fold_T *)found_ga->ga_data), recursive);
	    अञ्ञथा
	    {
		सचे (first_lnum > found_fp->fd_शिखरम् + found_off)
		    first_lnum = found_fp->fd_शिखरम् + found_off;
		सचे (last_lnum < lnum)
		    last_lnum = lnum;
		सचे (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    सचे (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	सचे (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    अञ्ञथा
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    सचे (last_lnum > 0)
	changed_पङ्क्तयः(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    शून्य
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
    शून्य
foldUpdate(win_T *wp, linenr_T शिखरम्, linenr_T bot)
{
    fold_T	*fp;

    सचे (disable_fold_update > 0)
	पटिनिवत्त;
# ifdef FEAT_DIFF
    सचे (need_diff_redraw)
	// will update later
	पटिनिवत्त;
# endif

    सचे (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = शिखरम्;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	सचे (शिखरम् > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = शिखरम्;
	}
	(शून्य)foldFind(&wp->w_folds, maybe_small_start, &fp);
	याव (fp < (fold_T *)wp->w_folds.ga_data + wp->w_folds.ga_len
		&& fp->fd_शिखरम् <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    सचे (foldmethodIsIndent(wp)
	    || foldmethod_Is_व्यञ्जकम्(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	पूर्णाङ्क save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, शिखरम्, bot);
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
    शून्य
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
    पूर्णाङ्क
foldMoveTo(
    पूर्णाङ्क		updown,
    पूर्णाङ्क		dir,	    // FORWARD or BACKWARD
    long	गणनम्)
{
    long	n;
    पूर्णाङ्क		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    पूर्णाङ्क		use_level;
    पूर्णाङ्क		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    पूर्णाङ्क		level;
    पूर्णाङ्क		last;

    checkupdate(curwin);

    // Repeat "count" times.
    अत्थाय (n = 0; n < गणनम्; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	सचे (gap->ga_len == 0)
	    भङ्ग;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	अत्थाय (;;)
	{
	    सचे (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		सचे (!updown || gap->ga_len == 0)
		    भङ्ग;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		सचे (dir == FORWARD)
		{
		    सचे (fp - (fold_T *)gap->ga_data >= gap->ga_len)
			भङ्ग;
		    --fp;
		}
		अञ्ञथा
		{
		    सचे (fp == (fold_T *)gap->ga_data)
			भङ्ग;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    सचे (!last)
	    {
		// Check if this fold is closed.
		सचे (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		सचे (last && !updown)
		    भङ्ग;
	    }

	    सचे (updown)
	    {
		सचे (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    सचे (fp + 1 - (fold_T *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_शिखरम् + lnum_off;
			सचे (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		अञ्ञथा
		{
		    // to end of previous fold if there is one
		    सचे (fp > (fold_T *)gap->ga_data)
		    {
			lnum = fp[-1].fd_शिखरम् + lnum_off + fp[-1].fd_len - 1;
			सचे (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    अञ्ञथा
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		सचे (dir == FORWARD)
		{
		    lnum = fp->fd_शिखरम् + lnum_off + fp->fd_len - 1;
		    सचे (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		अञ्ञथा
		{
		    lnum = fp->fd_शिखरम् + lnum_off;
		    सचे (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    सचे (last)
		भङ्ग;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_शिखरम्;
	    ++level;
	}
	सचे (lnum_found != curwin->w_cursor.lnum)
	{
	    सचे (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	अञ्ञथा
	    भङ्ग;
    }

    पटिनिवत्त retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    शून्य
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, परिमाण(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    पूर्णाङ्क
find_wl_entry(win_T *win, linenr_T lnum)
{
    पूर्णाङ्क		i;

    अत्थाय (i = 0; i < win->w_पङ्क्तयः_valid; ++i)
	सचे (win->w_पङ्क्तयः[i].wl_valid)
	{
	    सचे (lnum < win->w_पङ्क्तयः[i].wl_lnum)
		पटिनिवत्त -1;
	    सचे (lnum <= win->w_पङ्क्तयः[i].wl_lastlnum)
		पटिनिवत्त i;
	}
    पटिनिवत्त -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    शून्य
foldAdjustVisual(शून्य)
{
    pos_T	*start, *end;

    सचे (!VIsual_active || !hasAnyFolding(curwin))
	पटिनिवत्त;

    सचे (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    अञ्ञथा
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    सचे (hasFolding(start->lnum, &start->lnum, रिक्त))
	start->col = 0;

    सचे (!hasFolding(end->lnum, रिक्त, &end->lnum))
	पटिनिवत्त;

    end->col = ml_get_len(end->lnum);
    सचे (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    सचे (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    शून्य
foldAdjustCursor(शून्य)
{
    (शून्य)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, रिक्त);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    शून्य
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    पूर्णाङ्क		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    सचे (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	पटिनिवत्त;

    from_p = (fold_T *)from->ga_data;
    to_p = (fold_T *)to->ga_data;

    अत्थाय (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_शिखरम् = from_p->fd_शिखरम्;
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
    स्थिर पूर्णाङ्क
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	नीचः, उच्चः;
    fold_T	*fp;
    पूर्णाङ्क		i;

    सचे (gap->ga_len == 0)
    {
	*fpp = रिक्त;
	पटिनिवत्त FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_data;
    नीचः = 0;
    उच्चः = gap->ga_len - 1;
    याव (नीचः <= उच्चः)
    {
	i = (नीचः + उच्चः) / 2;
	सचे (fp[i].fd_शिखरम् > lnum)
	    // fold below lnum, adjust high
	    उच्चः = i - 1;
	अञ्ञथा सचे (fp[i].fd_शिखरम् + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    नीचः = i + 1;
	अञ्ञथा
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    पटिनिवत्त TRUE;
	}
    }
    *fpp = fp + नीचः;
    पटिनिवत्त FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    स्थिर पूर्णाङ्क
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    पूर्णाङ्क		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    अत्थाय (;;)
    {
	सचे (!foldFind(gap, lnum_rel, &fp))
	    भङ्ग;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_शिखरम्;
	++level;
    }

    पटिनिवत्त level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    स्थिर शून्य
checkupdate(win_T *wp)
{
    सचे (!wp->w_foldinvalid)
	पटिनिवत्त;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    स्थिर शून्य
setFoldRepeat(linenr_T lnum, long गणनम्, पूर्णाङ्क do_open)
{
    पूर्णाङ्क		done;
    long	n;

    अत्थाय (n = 0; n < गणनम्; ++n)
    {
	done = DONE_NOTHING;
	(शून्य)setManualFold(lnum, do_open, FALSE, &done);
	सचे (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    सचे (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    भङ्ग;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    स्थिर linenr_T
setManualFold(
    linenr_T	lnum,
    पूर्णाङ्क		opening,    // TRUE when opening, FALSE when closing
    पूर्णाङ्क		recurse,    // TRUE when closing/opening recursive
    पूर्णाङ्क		*donep)
{
# ifdef FEAT_DIFF
    सचे (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    सचे (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		सचे (dlnum != 0)
		    (शून्य)setManualFoldWin(wp, dlnum, opening, recurse, रिक्त);
	    }
	}
    }
# endif

    पटिनिवत्त setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    स्थिर linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    पूर्णाङ्क		opening,    // TRUE when opening, FALSE when closing
    पूर्णाङ्क		recurse,    // TRUE when closing/opening recursive
    पूर्णाङ्क		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = रिक्त;
    पूर्णाङ्क		j;
    पूर्णाङ्क		level = 0;
    पूर्णाङ्क		use_level = FALSE;
    पूर्णाङ्क		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	next = MAXLNUM;
    linenr_T	off = 0;
    पूर्णाङ्क		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    अत्थाय (;;)
    {
	सचे (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    सचे (fp != रिक्त && fp < (fold_T *)gap->ga_data + gap->ga_len)
		next = fp->fd_शिखरम् + off;
	    भङ्ग;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	सचे (fp + 1 < (fold_T *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_शिखरम् + off;

	// Change from level-dependent folding to manual.
	सचे (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    सचे (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    अञ्ञथा
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_data;
	    अत्थाय (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	सचे (!opening && recurse)
	{
	    सचे (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	अञ्ञथा सचे (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    सचे (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		सचे (recurse)
		    foldOpenNested(fp);
	    }
	    भङ्ग;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_शिखरम्;
	off += fp->fd_शिखरम्;
	++level;
    }
    सचे (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	सचे (!opening && found != रिक्त)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	सचे (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    अञ्ञथा सचे (donep == रिक्त && wp == curwin)
	emsg(_(e_no_fold_found));

    सचे (donep != रिक्त)
	*donep |= done;

    पटिनिवत्त next;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    स्थिर शून्य
foldOpenNested(fold_T *fpr)
{
    पूर्णाङ्क		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_data;
    अत्थाय (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    स्थिर शून्य
deleteFoldEntry(garray_T *gap, पूर्णाङ्क idx, पूर्णाङ्क recursive)
{
    fold_T	*fp;
    पूर्णाङ्क		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_data + idx;
    सचे (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	सचे (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, परिमाण(fold_T) * (gap->ga_len - idx));
    }
    अञ्ञथा
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	सचे (ga_grow(gap, (पूर्णाङ्क)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_data + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_data;
	    अत्थाय (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_शिखरम् += fp->fd_शिखरम्;
		सचे (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		सचे (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    सचे (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  परिमाण(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(परिमाण(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    शून्य
deleteFoldRecurse(garray_T *gap)
{
    पूर्णाङ्क		i;

    अत्थाय (i = 0; i < gap->ga_len; ++i)
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
    शून्य
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    सचे (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    सचे (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    सचे ((State & MODE_निवेशय) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    स्थिर शून्य
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    पूर्णाङ्क		i;
    linenr_T	last;
    linenr_T	शिखरम्;

    सचे (gap->ga_len == 0)
	पटिनिवत्त;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    सचे ((State & MODE_निवेशय) && amount == (linenr_T)1 && line2 == MAXLNUM)
	शिखरम् = line1 + 1;
    अञ्ञथा
	शिखरम् = line1;

    // Find the fold containing or just below "line1".
    (शून्य)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    अत्थाय (i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_शिखरम् + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	सचे (last < line1)
	    अनुवर्त;

	// 6. fold below line2: only adjust for amount_after
	सचे (fp->fd_शिखरम् > line2)
	{
	    सचे (amount_after == 0)
		भङ्ग;
	    fp->fd_शिखरम् += amount_after;
	}
	अञ्ञथा
	{
	    सचे (fp->fd_शिखरम् >= शिखरम् && last <= line2)
	    {
		// 4. fold completely contained in range
		सचे (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		अञ्ञथा
		    fp->fd_शिखरम् += amount;
	    }
	    अञ्ञथा
	    {
		सचे (fp->fd_शिखरम् < शिखरम्)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_शिखरम्,
				  line2 - fp->fd_शिखरम्, amount, amount_after);
		    सचे (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			सचे (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_शिखरम्;
			अञ्ञथा
			    fp->fd_len += amount;
		    }
		    अञ्ञथा
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		अञ्ञथा
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    सचे (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_शिखरम्,
				  amount,
				  amount_after + (fp->fd_शिखरम् - शिखरम्));
			fp->fd_len -= line2 - fp->fd_शिखरम् + 1;
			fp->fd_शिखरम् = line1;
		    }
		    अञ्ञथा
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_शिखरम्,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_शिखरम् += amount;
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
    पूर्णाङ्क
getDeepestNesting(शून्य)
{
    checkupdate(curwin);
    पटिनिवत्त getDeepestNestingRecurse(&curwin->w_folds);
}

    स्थिर पूर्णाङ्क
getDeepestNestingRecurse(garray_T *gap)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		level;
    पूर्णाङ्क		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    अत्थाय (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	सचे (level > maxlevel)
	    maxlevel = level;
    }

    पटिनिवत्त maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    स्थिर पूर्णाङ्क
check_closed(
    win_T	*win,
    fold_T	*fp,
    पूर्णाङ्क		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    पूर्णाङ्क		level,		    // folding depth
    पूर्णाङ्क		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    पूर्णाङ्क		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    सचे (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	सचे (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    अञ्ञथा सचे (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    सचे (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    सचे (closed)
    {
	सचे (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	सचे (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    पटिनिवत्त closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    स्थिर शून्य
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    पूर्णाङ्क		गणनम्;
    पूर्णाङ्क		n;

    सचे (fp->fd_small != MAYBE)
	पटिनिवत्त;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    सचे (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    अञ्ञथा
    {
	गणनम् = 0;
	अत्थाय (n = 0; n < fp->fd_len; ++n)
	{
	    गणनम् += plines_win_nofold(wp, fp->fd_शिखरम् + lnum_off + n);
	    सचे (गणनम् > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		पटिनिवत्त;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    स्थिर शून्य
setSmallMaybe(garray_T *gap)
{
    पूर्णाङ्क		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    अत्थाय (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    स्थिर शून्य
foldCreateMarkers(linenr_T start, linenr_T end)
{
    सचे (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	पटिनिवत्त;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_पङ्क्तयः(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    स्थिर शून्य
foldAddMarker(linenr_T lnum, char_u *marker, पूर्णाङ्क markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*पङ्क्तिः;
    पूर्णाङ्क		पङ्क्तिः_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((वर्ण *)curbuf->b_p_cms, "%s");
    पूर्णाङ्क		पङ्क्तिः_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    पङ्क्तिः = ml_get(lnum);
    पङ्क्तिः_len = ml_get_len(lnum);

    सचे (u_save(lnum - 1, lnum + 1) != OK)
	पटिनिवत्त;

    // Check if the line ends with an unclosed comment
    (शून्य)skip_comment(पङ्क्तिः, FALSE, FALSE, &पङ्क्तिः_is_comment);
    newline = alloc(पङ्क्तिः_len + markerlen + STRLEN(cms) + 1);
    सचे (newline == रिक्त)
	पटिनिवत्त;
    STRCPY(newline, पङ्क्तिः);
    // Append the marker to the end of the line
    सचे (p == रिक्त || पङ्क्तिः_is_comment)
	vim_strncpy(newline + पङ्क्तिः_len, marker, markerlen);
    अञ्ञथा
    {
	STRCPY(newline + पङ्क्तिः_len, cms);
	STRNCPY(newline + पङ्क्तिः_len + (p - cms), marker, markerlen);
	STRCPY(newline + पङ्क्तिः_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    स्थिर शून्य
deleteFoldMarkers(
    fold_T	*fp,
    पूर्णाङ्क		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    पूर्णाङ्क		i;

    सचे (recursive)
	अत्थाय (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_शिखरम्);
    foldDelMarker(fp->fd_शिखरम् + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_शिखरम् + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    स्थिर शून्य
foldDelMarker(linenr_T lnum, char_u *marker, पूर्णाङ्क markerlen)
{
    char_u	*पङ्क्तिः;
    char_u	*newline;
    char_u	*p;
    पूर्णाङ्क		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    सचे (lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	पटिनिवत्त;
    पङ्क्तिः = ml_get(lnum);
    अत्थाय (p = पङ्क्तिः; *p != NUL; ++p)
	सचे (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    सचे (VIM_ISDIGIT(p[len]))
		++len;
	    सचे (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((वर्ण *)cms, "%s");
		सचे (cms2 != रिक्त && p - पङ्क्तिः >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (पूर्णाङ्क)STRLEN(cms) - 2;
		}
	    }
	    सचे (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		सचे (newline != रिक्त)
		{
		    STRNCPY(newline, पङ्क्तिः, p - पङ्क्तिः);
		    STRCPY(newline + (p - पङ्क्तिः), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    भङ्ग;
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
    char_u	*text = रिक्त;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    स्थिर पूर्णाङ्क	    got_fdt_error = FALSE;
    पूर्णाङ्क		    save_did_emsg = did_emsg;
    स्थिर win_T    *last_wp = रिक्त;
    स्थिर linenr_T last_lnum = 0;

    सचे (last_wp != wp || last_wp == रिक्त
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    सचे (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    सचे (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	पूर्णाङ्क	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	सचे (level > (पूर्णाङ्क)परिमाण(dashes) - 1)
	    level = (पूर्णाङ्क)परिमाण(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	सचे (!got_fdt_error)
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

	    सचे (text == रिक्त || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, रिक्त, -1);

	सचे (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	सचे (text != रिक्त)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    अत्थाय (p = text; *p != NUL; ++p)
	    {
		पूर्णाङ्क	len;

		सचे (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    सचे (!vim_isprintc((*mb_ptr2char)(p)))
			भङ्ग;
		    p += len - 1;
		}
		अञ्ञथा
		    सचे (*p == TAB)
			*p = ' ';
		    अञ्ञथा सचे (ptr2cells(p) > 1)
			भङ्ग;
	    }
	    सचे (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    सचे (text == रिक्त)
# endif
    {
	long गणनम् = (long)(lnume - lnum + 1);

	vim_snprintf((वर्ण *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", गणनम्),
		     गणनम्);
	text = buf;
    }
    पटिनिवत्त text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    स्थिर शून्य
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    पूर्णाङ्क		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    पूर्णाङ्क		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    पूर्णाङ्क		len;
    पूर्णाङ्क		did1 = FALSE;
    पूर्णाङ्क		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (पूर्णाङ्क)STRLEN(cms_start);
    याव (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((वर्ण *)cms_start, "%s");
    सचे (cms_end != रिक्त)
    {
	cms_elen = cms_slen - (पूर्णाङ्क)(cms_end - cms_start);
	cms_slen = (पूर्णाङ्क)(cms_end - cms_start);

	// exclude white space before "%s"
	याव (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (पूर्णाङ्क)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    अत्थाय (s = str; *s != NUL; )
    {
	len = 0;
	सचे (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	अञ्ञथा सचे (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	सचे (len > 0)
	{
	    सचे (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    अत्थाय (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    सचे (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (पूर्णाङ्क)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	अञ्ञथा सचे (cms_end != रिक्त)
	{
	    सचे (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    अञ्ञथा सचे (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	सचे (len != 0)
	{
	    याव (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	अञ्ञथा
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
प्रकारनिर्देश संरचना
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    पूर्णाङ्क		lvl;		// current level (-1 for undefined)
    पूर्णाङ्क		lvl_next;	// level used for next line
    पूर्णाङ्क		start;		// number of folds that are forced to start at
				// this line.
    पूर्णाङ्क		end;		// level of fold that is forced to end below
				// this line
    पूर्णाङ्क		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
स्थिर पूर्णाङ्क fold_changed;

// Function declarations. {{{2
स्थिर linenr_T foldUpdateIEMSRecurse(garray_T *gap, पूर्णाङ्क level, linenr_T startlnum, fline_T *flp, शून्य (*getlevel)(fline_T *), linenr_T bot, पूर्णाङ्क topflags);
स्थिर पूर्णाङ्क fold_निवेशय(garray_T *gap, पूर्णाङ्क i);
स्थिर शून्य foldSplit(garray_T *gap, पूर्णाङ्क i, linenr_T शिखरम्, linenr_T bot);
स्थिर शून्य foldRemove(garray_T *gap, linenr_T शिखरम्, linenr_T bot);
स्थिर शून्य foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
स्थिर शून्य foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
स्थिर शून्य foldlevelDiff(fline_T *flp);
# endif
स्थिर शून्य foldlevel_व्यञ्जकम्(fline_T *flp);
स्थिर शून्य foldlevelMarker(fline_T *flp);
स्थिर शून्य foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    स्थिर शून्य
foldUpdateIEMS(win_T *wp, linenr_T शिखरम्, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    शून्य	(*getlevel)(fline_T *);
    पूर्णाङ्क		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    सचे (invalid_शिखरम् != (linenr_T)0)
	पटिनिवत्त;

    सचे (wp->w_foldinvalid)
    {
	// Need to update all folds.
	शिखरम् = 1;
	bot = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    सचे (foldmethodIsDiff(wp))
    {
	सचे (शिखरम् > diff_context)
	    शिखरम् -= diff_context;
	अञ्ञथा
	    शिखरम् = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    सचे (शिखरम् > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	शिखरम् = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_शिखरम् = शिखरम्;
    invalid_bot = bot;

    सचे (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	सचे (शिखरम् > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, शिखरम् - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = शिखरम् - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    सचे (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_next);
	    अञ्ञथा
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = शिखरम्;
	getlevel(&fline);
    }
    अञ्ञथा
    {
	fline.lnum = शिखरम्;
	सचे (foldmethod_Is_व्यञ्जकम्(wp))
	{
	    getlevel = foldlevel_व्यञ्जकम्;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    सचे (शिखरम् > 1)
		--fline.lnum;
	}
	अञ्ञथा सचे (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	अञ्ञथा सचे (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	अञ्ञथा
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    सचे (शिखरम् > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	अत्थाय ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    सचे (fline.lvl >= 0)
		भङ्ग;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    सचे (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = रिक्त;
	पूर्णाङ्क	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	याव (current_fdl < fline.lvl)
	{
	    सचे (!foldFind(gap, lnum_rel, &fpn))
		भङ्ग;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_शिखरम्;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_शिखरम्;
	}
	सचे (fpn != रिक्त && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    सचे (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    सचे (start > end && end < wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	end = start;
    याव (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	सचे (fline.lnum > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	    भङ्ग;
	सचे (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    सचे (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_व्यञ्जकम्)
		भङ्ग;
	    सचे ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_शिखरम् + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_शिखरम् < fline.lnum))
		end = fp->fd_शिखरम् + fp->fd_len - 1;
	    अञ्ञथा सचे (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    अञ्ञथा
		भङ्ग;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	सचे (fline.lvl > 0)
	{
	    invalid_शिखरम् = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	अञ्ञथा
	{
	    सचे (fline.lnum == wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
		भङ्ग;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    सचे (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    सचे (end != bot)
	redraw_win_range_later(wp, शिखरम्, end);

    invalid_शिखरम् = (linenr_T)0;
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
    स्थिर linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    पूर्णाङ्क		level,
    linenr_T	startlnum,
    fline_T	*flp,
    शून्य	(*getlevel)(fline_T *),
    linenr_T	bot,
    पूर्णाङ्क		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = रिक्त;
    fold_T	*fp2;
    पूर्णाङ्क		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    पूर्णाङ्क		i;
    पूर्णाङ्क		finish = FALSE;
    linenr_T	linecount = flp->wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् - flp->off;
    पूर्णाङ्क		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    सचे (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(शून्य)foldFind(gap, startlnum - 1, &fp);
	सचे (fp != रिक्त && (fp >= ((fold_T *)gap->ga_data) + gap->ga_len
						   || fp->fd_शिखरम् >= startlnum))
	    fp = रिक्त;
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
    याव (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	पङ्क्तिः_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	सचे (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	सचे (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	सचे (flp->lnum > bot && !finish && fp != रिक्त)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    सचे (getlevel != foldlevelMarker
		    && getlevel != foldlevel_व्यञ्जकम्
		    && getlevel != foldlevelSyntax)
		भङ्ग;
	    i = 0;
	    fp2 = fp;
	    सचे (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_शिखरम्;
		याव (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_शिखरम्;
		}
	    }
	    सचे (lvl < level + i)
	    {
		(शून्य)foldFind(&fp->fd_nested, flp->lnum - fp->fd_शिखरम्, &fp2);
		सचे (fp2 != रिक्त)
		    bot = fp2->fd_शिखरम् + fp2->fd_len - 1 + fp->fd_शिखरम्;
	    }
	    अञ्ञथा सचे (fp->fd_शिखरम् + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    अञ्ञथा
		भङ्ग;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	सचे (fp == रिक्त
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
	    याव (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		सचे (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		अञ्ञथा
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		सचे (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && fp->fd_शिखरम् <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_शिखरम् < flp->lnum)
				|| (lvl >= level
					   && fp->fd_शिखरम् <= flp->lnum_save)))))
		{
		    सचे (fp->fd_शिखरम् + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			सचे (fp->fd_शिखरम् == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			अञ्ञथा सचे (fp->fd_शिखरम् >= startlnum)
			{
			    सचे (fp->fd_शिखरम् > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_शिखरम् - firstlnum), 0L);
			    अञ्ञथा
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_शिखरम् - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_शिखरम् - firstlnum));
			    fp->fd_len += fp->fd_शिखरम् - firstlnum;
			    fp->fd_शिखरम् = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			अञ्ञथा सचे ((flp->start != 0 && lvl == level)
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
			    सचे (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    अञ्ञथा
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_शिखरम्,
						      breakend - fp->fd_शिखरम्);
			    i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_data + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    सचे (getlevel == foldlevelMarker
				    || getlevel == foldlevel_व्यञ्जकम्
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			सचे (fp->fd_शिखरम् == startlnum && concat)
			{
			    i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data);
			    सचे (i != 0)
			    {
				fp2 = fp - 1;
				सचे (fp2->fd_शिखरम् + fp2->fd_len == fp->fd_शिखरम्)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			भङ्ग;
		    }
		    सचे (fp->fd_शिखरम् >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data), TRUE);
		    }
		    अञ्ञथा
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_शिखरम्;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		अञ्ञथा
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    सचे (gap->ga_len == 0)
			i = 0;
		    अञ्ञथा
			i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data);
		    सचे (fold_निवेशय(gap, i) != OK)
			पटिनिवत्त bot;
		    fp = (fold_T *)gap->ga_data + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_शिखरम् = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    सचे (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    अञ्ञथा सचे (i <= 0)
		    {
			fp->fd_flags = topflags;
			सचे (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    अञ्ञथा
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    सचे (getlevel == foldlevelMarker
			    || getlevel == foldlevel_व्यञ्जकम्
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    भङ्ग;
		}
	    }
	}

	सचे (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    भङ्ग;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	सचे (lvl > level && fp != रिक्त)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    सचे (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_शिखरम्;
	    flp->off += fp->fd_शिखरम्;
	    i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_शिखरम्, flp, getlevel,
					      bot - fp->fd_शिखरम्, fp->fd_flags);
	    fp = (fold_T *)gap->ga_data + i;
	    flp->lnum += fp->fd_शिखरम्;
	    flp->lnum_save += fp->fd_शिखरम्;
	    flp->off -= fp->fd_शिखरम्;
	    bot += fp->fd_शिखरम्;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	अञ्ञथा
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    याव (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		सचे (++flp->lnum > linecount)
		    भङ्ग;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		सचे (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    भङ्ग;
	    }
	    prev_lnum = 0;
	    सचे (flp->lnum > linecount)
		भङ्ग;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    सचे (fp == रिक्त)	// only happens when got_int is set
	पटिनिवत्त bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    सचे (fp->fd_len < flp->lnum - fp->fd_शिखरम्)
    {
	fp->fd_len = flp->lnum - fp->fd_शिखरम्;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    अञ्ञथा सचे (fp->fd_शिखरम् + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_शिखरम् + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_शिखरम्,
						  flp->lnum - 1 - fp->fd_शिखरम्);

    सचे (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	सचे (fp->fd_len != flp->lnum - fp->fd_शिखरम्)
	{
	    सचे (fp->fd_शिखरम् + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		सचे (getlevel == foldlevelMarker
			|| getlevel == foldlevel_व्यञ्जकम्
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_शिखरम् + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_शिखरम्;
		}
		अञ्ञथा
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_data + i;
		}
	    }
	    अञ्ञथा
		fp->fd_len = flp->lnum - fp->fd_शिखरम्;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    अत्थाय (;;)
    {
	fp2 = fp + 1;
	सचे (fp2 >= (fold_T *)gap->ga_data + gap->ga_len
						  || fp2->fd_शिखरम् > flp->lnum)
	    भङ्ग;
	सचे (fp2->fd_शिखरम् + fp2->fd_len > flp->lnum)
	{
	    सचे (fp2->fd_शिखरम् < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_शिखरम् - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_शिखरम् - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_शिखरम्;
		fp2->fd_शिखरम् = flp->lnum;
		fold_changed = TRUE;
	    }

	    सचे (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    भङ्ग;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (पूर्णाङ्क)(fp2 - (fold_T *)gap->ga_data), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    सचे (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    पटिनिवत्त bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    स्थिर पूर्णाङ्क
fold_निवेशय(garray_T *gap, पूर्णाङ्क i)
{
    fold_T	*fp;

    सचे (ga_grow(gap, 1) == FAIL)
	पटिनिवत्त FAIL;
    fp = (fold_T *)gap->ga_data + i;
    सचे (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, परिमाण(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, परिमाण(fold_T), 10);
    पटिनिवत्त OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    स्थिर शून्य
foldSplit(
    garray_T	*gap,
    पूर्णाङ्क		i,
    linenr_T	शिखरम्,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		len;

    // The fold continues below bot, need to split it.
    सचे (fold_निवेशय(gap, i + 1) == FAIL)
	पटिनिवत्त;
    fp = (fold_T *)gap->ga_data + i;
    fp[1].fd_शिखरम् = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_शिखरम् - fp->fd_शिखरम्);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (शून्य)foldFind(gap1, bot + 1 - fp->fd_शिखरम्, &fp2);
    सचे (fp2 != रिक्त)
    {
	len = (पूर्णाङ्क)((fold_T *)gap1->ga_data + gap1->ga_len - fp2);
	सचे (len > 0 && ga_grow(gap2, len) == OK)
	{
	    अत्थाय (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_data)[idx] = fp2[idx];
		((fold_T *)gap2->ga_data)[idx].fd_शिखरम्
						  -= fp[1].fd_शिखरम् - fp->fd_शिखरम्;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = शिखरम् - fp->fd_शिखरम्;
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
    स्थिर शून्य
foldRemove(garray_T *gap, linenr_T शिखरम्, linenr_T bot)
{
    fold_T	*fp = रिक्त;

    सचे (bot < शिखरम्)
	पटिनिवत्त;		// nothing to do

    याव (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	सचे (foldFind(gap, शिखरम्, &fp) && fp->fd_शिखरम् < शिखरम्)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, शिखरम् - fp->fd_शिखरम्, bot - fp->fd_शिखरम्);
	    सचे (fp->fd_शिखरम् + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data), शिखरम्, bot);
	    }
	    अञ्ञथा
	    {
		// 2: truncate fold at "top".
		fp->fd_len = शिखरम् - fp->fd_शिखरम्;
	    }
	    fold_changed = TRUE;
	    अनुवर्त;
	}
	सचे (fp >= (fold_T *)(gap->ga_data) + gap->ga_len
		|| fp->fd_शिखरम् > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    भङ्ग;
	}
	सचे (fp->fd_शिखरम् >= शिखरम्)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    सचे (fp->fd_शिखरम् + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_शिखरम्),
			(linenr_T)MAXLNUM, (long)(fp->fd_शिखरम् - bot - 1));
		fp->fd_len -= bot - fp->fd_शिखरम् + 1;
		fp->fd_शिखरम् = bot + 1;
		भङ्ग;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (पूर्णाङ्क)(fp - (fold_T *)gap->ga_data), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    स्थिर शून्य
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *वामः, *दक्षिणः;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    अत्थाय (; start < end; start++, end--)
    {
	वामः = (fold_T *)gap->ga_data + start;
	दक्षिणः = (fold_T *)gap->ga_data + end;
	tmp  = *वामः;
	*वामः = *दक्षिणः;
	*दक्षिणः = tmp;
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

    स्थिर शून्य
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_शिखरम्, MAXLNUM);
    fp->fd_len = end - fp->fd_शिखरम्;
}

# define fold_end(fp) ((fp)->fd_शिखरम् + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_data + (gap)->ga_len))
# define fold_index(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_data)))

    शून्य
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    पूर्णाङ्क at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_index = 0;

    सचे (at_start)
    {
	सचे (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_शिखरम्, line2 -
		    fp->fd_शिखरम्, dest - fp->fd_शिखरम्);
	    पटिनिवत्त;
	}
	अञ्ञथा सचे (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_शिखरम्, line2 -
		    fp->fd_शिखरम्, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	अञ्ञथा
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    सचे (!valid_fold(fp, gap) || fp->fd_शिखरम् > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	पटिनिवत्त;
    }
    अञ्ञथा सचे (fp->fd_शिखरम् > line2)
    {
	अत्थाय (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_शिखरम् -= range_len;

	सचे (valid_fold(fp, gap) && fp->fd_शिखरम् <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_शिखरम् -= range_len;
	}
	पटिनिवत्त;
    }
    अञ्ञथा सचे (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_शिखरम्, dest -
		fp->fd_शिखरम्, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_शिखरम् += move_len;
	पटिनिवत्त;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_index(fp, gap);

    अत्थाय (; valid_fold(fp, gap) && fp->fd_शिखरम् <= dest; fp++)
    {
	सचे (fp->fd_शिखरम् <= line2)
	{
	    // 1. 2. or 3.
	    सचे (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_शिखरम् += move_len;
	    अनुवर्त;
	}

	// Record index of the first fold after the moved range.
	सचे (move_end == 0)
	    move_end = fold_index(fp, gap);

	सचे (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_शिखरम् -= range_len;
    }

    dest_index = fold_index(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    सचे (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	पटिनिवत्त;
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
    स्थिर शून्य
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    पूर्णाङ्क		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    सचे (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    सचे (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	अत्थाय (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_data)[gap1->ga_len]
					= ((fold_T *)gap2->ga_data)[idx];
	    ((fold_T *)gap1->ga_data)[gap1->ga_len].fd_शिखरम् += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (पूर्णाङ्क)(fp2 - (fold_T *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    स्थिर शून्य
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    सचे (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != रिक्त)
    {
	// first and last line can't be undefined, use level 0
	सचे (lnum == 1 || lnum == buf->b_ml.ml_पङ्क्तिः_गणनम्)
	    flp->lvl = 0;
	अञ्ञथा
	    flp->lvl = -1;
    }
    अञ्ञथा
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_मानम्(buf);
    सचे (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	सचे (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    स्थिर शून्य
foldlevelDiff(fline_T *flp)
{
    सचे (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    अञ्ञथा
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    स्थिर शून्य
foldlevel_व्यञ्जकम्(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# अञ्ञथा
    win_T	*win;
    पूर्णाङ्क		n;
    पूर्णाङ्क		c;
    linenr_T	lnum = flp->lnum + flp->off;
    पूर्णाङ्क		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    सचे (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = कुञ्चिका_Typed;
    n = eval_foldexpr(flp->wp, &c);
    कुञ्चिका_Typed = save_keytyped;

    विकल्प (c)
    {
	// "a1", "a2", .. : add to the fold level
	पक्ष 'a': सचे (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  भङ्ग;

	// "s1", "s2", .. : subtract from the fold level
	पक्ष 's': सचे (flp->lvl >= 0)
		  {
		      सचे (n > flp->lvl)
			  flp->lvl_next = 0;
		      अञ्ञथा
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  भङ्ग;

	// ">1", ">2", .. : start a fold with a certain level
	पक्ष '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  भङ्ग;

	// "<1", "<2", .. : end a fold with a certain level
	पक्ष '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_next = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  भङ्ग;

	// "=": No change in level
	पक्ष '=': flp->lvl_next = flp->lvl;
		  भङ्ग;

	// "-1", "0", "1", ..: set fold level
	स्वभाव:  सचे (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_next = flp->lvl;
		  अञ्ञथा
		      flp->lvl_next = n;
		  flp->lvl = n;
		  भङ्ग;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    सचे (flp->lvl < 0)
    {
	सचे (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	सचे (lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
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
    स्थिर शून्य
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (पूर्णाङ्क)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (पूर्णाङ्क)STRLEN(foldendmarker);
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
    स्थिर शून्य
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    पूर्णाङ्क		cstart;
    पूर्णाङ्क		cend;
    पूर्णाङ्क		start_lvl = flp->lvl;
    char_u	*s;
    पूर्णाङ्क		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    याव (*s)
    {
	सचे (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    सचे (VIM_ISDIGIT(*s))
	    {
		n = atoi((वर्ण *)s);
		सचे (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    सचे (n <= start_lvl)
			flp->start = 1;
		    अञ्ञथा
			flp->start = n - start_lvl;
		}
	    }
	    अञ्ञथा
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	अञ्ञथा सचे (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    सचे (VIM_ISDIGIT(*s))
	    {
		n = atoi((वर्ण *)s);
		सचे (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    // never start a fold with an end marker
		    सचे (flp->lvl_next > start_lvl)
			flp->lvl_next = start_lvl;
		}
	    }
	    अञ्ञथा
		--flp->lvl_next;
	}
	अञ्ञथा
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    सचे (flp->lvl_next < 0)
	flp->lvl_next = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    स्थिर शून्य
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# अञ्ञथा
    linenr_T	lnum = flp->lnum + flp->off;
    पूर्णाङ्क		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    सचे (lnum < flp->wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	सचे (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# सचे defined(FEAT_SESSION)
स्थिर पूर्णाङ्क put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
स्थिर पूर्णाङ्क put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
स्थिर पूर्णाङ्क put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    पूर्णाङ्क
put_folds(FILE *fd, win_T *wp)
{
    सचे (foldmethodIsManual(wp))
    {
	सचे (put_पङ्क्तिः(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_पङ्क्तिः(fd, "&fdl = &fdl") == FAIL)
	    पटिनिवत्त FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    सचे (wp->w_fold_manual)
	पटिनिवत्त put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    पटिनिवत्त OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    स्थिर पूर्णाङ्क
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    पूर्णाङ्क		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    अत्थाय (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	सचे (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_शिखरम्) == FAIL)
	    पटिनिवत्त FAIL;
	सचे (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_शिखरम् + off,
					fp->fd_शिखरम् + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    पटिनिवत्त FAIL;
	++fp;
    }
    पटिनिवत्त OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    स्थिर पूर्णाङ्क
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    अत्थाय (i = 0; i < gap->ga_len; i++)
    {
	सचे (fp->fd_flags != FD_LEVEL)
	{
	    सचे (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		सचे (fprintf(fd, ":%ld", fp->fd_शिखरम् + off) < 0
			|| put_eol(fd) == FAIL
			|| put_पङ्क्तिः(fd, "sil! normal! zo") == FAIL)
		    पटिनिवत्त FAIL;
		सचे (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_शिखरम्)
			== FAIL)
		    पटिनिवत्त FAIL;
		// close the parent when needed
		सचे (fp->fd_flags == FD_CLOSED)
		{
		    सचे (put_fold_open_close(fd, fp, off) == FAIL)
			पटिनिवत्त FAIL;
		}
	    }
	    अञ्ञथा
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_शिखरम्);
		सचे ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		सचे (put_fold_open_close(fd, fp, off) == FAIL)
		    पटिनिवत्त FAIL;
	    }
	}
	++fp;
    }

    पटिनिवत्त OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    स्थिर पूर्णाङ्क
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    सचे (fprintf(fd, ":%ld", fp->fd_शिखरम् + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	पटिनिवत्त FAIL;

    पटिनिवत्त OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#सचे defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    स्थिर शून्य
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    पूर्णाङ्क		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    सचे (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सचे (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_सङ्ख्या(&argvars[1]), &tp);
	सचे (wp != रिक्त && tp != रिक्त)
	{
	    सचे (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	अञ्ञथा
	{
	    rettv->vval.v_सङ्ख्या = -2;
	    पटिनिवत्त;
	}
    }
    अञ्ञथा
	lnum = tv_get_lnum(argvars);

    सचे (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	सचे (hasFoldingWin(wp, lnum, &first, &last, FALSE, रिक्त))
	{
	    सचे (end)
		rettv->vval.v_सङ्ख्या = (varnumber_T)last;
	    अञ्ञथा
		rettv->vval.v_सङ्ख्या = (varnumber_T)first;
	    पटिनिवत्त;
	}
    }
# endif
    rettv->vval.v_सङ्ख्या = -1;
}

/*
 * "foldclosed()" function
 */
    शून्य
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    शून्य
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    स्थिर पूर्णाङ्क
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    सचे (invalid_शिखरम् == (linenr_T)0)
	checkupdate(curwin);
    अञ्ञथा सचे (lnum == prev_lnum && prev_lnum_lvl >= 0)
	पटिनिवत्त prev_lnum_lvl;
    अञ्ञथा सचे (lnum >= invalid_शिखरम् && lnum <= invalid_bot)
	पटिनिवत्त -1;

    // Return quickly when there is no folding at all in this window.
    सचे (!hasAnyFolding(curwin))
	पटिनिवत्त 0;

    पटिनिवत्त foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    शून्य
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    सचे (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सचे (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_सङ्ख्या(&argvars[1]), &tp);
	सचे (wp != रिक्त && tp != रिक्त)
	{
	    सचे (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		सचे (lnum >= 1 && lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		    rettv->vval.v_सङ्ख्या = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	अञ्ञथा
	    rettv->vval.v_सङ्ख्या = -2;
    }
    अञ्ञथा
    {
	lnum = tv_get_lnum(argvars);
	सचे (lnum >= 1 && lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    rettv->vval.v_सङ्ख्या = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    शून्य
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    पूर्णाङ्क		len;
    वर्ण	*txt;
    long	गणनम्;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    सचे (foldstart > 0 && foldend <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्
	    && dashes != रिक्त)
    {
	// Find first non-empty line in the fold.
	अत्थाय (lnum = foldstart; lnum < foldend; ++lnum)
	    सचे (!linewhite(lnum))
		भङ्ग;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	सचे (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    सचे (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		सचे (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	गणनम् = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", गणनम्);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	सचे (r != रिक्त)
	{
	    sprintf((वर्ण *)r, txt, dashes, गणनम्);
	    len = (पूर्णाङ्क)STRLEN(r);
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
    शून्य
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    पूर्णाङ्क		fold_गणनम्;
    win_T	*wp  = curwin;
    स्थिर पूर्णाङ्क	entered = FALSE;
    सचे (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सचे (entered)
	पटिनिवत्त; // reject recursive use
    entered = TRUE;

    सचे (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_सङ्ख्या(&argvars[1]), &tp);
	सचे (wp == रिक्त || tp == रिक्त) // wrong winid
	{
	    entered = FALSE;
	    पटिनिवत्त;
	}
	सचे (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    अञ्ञथा
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    सचे (lnum < 0)
	lnum = 0;
    fold_गणनम् = folded_गणनम्(wp, lnum, &foldinfo);
    सचे (fold_गणनम् > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_गणनम् - 1,
							       &foldinfo, buf);
	सचे (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
