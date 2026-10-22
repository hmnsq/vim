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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
வகைவரை கட்டமைப்பு
{
    linenr_T	fd_உச்சி;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    எழுத்து	fd_flags;	// see below
    எழுத்து	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
நிலையான வெறுமை newFoldLevelWin(win_T *wp);
நிலையான முழு checkCloseRec(garray_T *gap, linenr_T lnum, முழு level);
நிலையான முழு foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
நிலையான முழு foldLevelWin(win_T *wp, linenr_T lnum);
நிலையான வெறுமை checkupdate(win_T *wp);
நிலையான வெறுமை setFoldRepeat(linenr_T lnum, long மொத்தம், முழு do_open);
நிலையான linenr_T setManualFold(linenr_T lnum, முழு opening, முழு recurse, முழு *donep);
நிலையான linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, முழு opening, முழு recurse, முழு *donep);
நிலையான வெறுமை foldOpenNested(fold_T *fpr);
நிலையான வெறுமை deleteFoldEntry(garray_T *gap, முழு idx, முழு recursive);
நிலையான வெறுமை foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
நிலையான முழு getDeepestNestingRecurse(garray_T *gap);
நிலையான முழு check_closed(win_T *win, fold_T *fp, முழு *use_levelp, முழு level, முழு *maybe_smallp, linenr_T lnum_off);
நிலையான வெறுமை checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
நிலையான வெறுமை setSmallMaybe(garray_T *gap);
நிலையான வெறுமை foldCreateMarkers(linenr_T start, linenr_T end);
நிலையான வெறுமை foldAddMarker(linenr_T lnum, char_u *marker, முழு markerlen);
நிலையான வெறுமை deleteFoldMarkers(fold_T *fp, முழு recursive, linenr_T lnum_off);
நிலையான வெறுமை foldDelMarker(linenr_T lnum, char_u *marker, முழு markerlen);
நிலையான வெறுமை foldUpdateIEMS(win_T *wp, linenr_T உச்சி, linenr_T bot);
நிலையான வெறுமை parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
நிலையான linenr_T invalid_உச்சி = (linenr_T)0;
நிலையான linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
நிலையான linenr_T prev_lnum = 0;
நிலையான முழு prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

நிலையான முழு foldstartmarkerlen;
நிலையான char_u *foldendmarker;
நிலையான முழு foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    வெறுமை
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
    முழு
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    திரும்பு (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    முழு
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    திரும்பு hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, வெற்று);
}

// hasFoldingWin() {{{2
    முழு
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    முழு		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    முழு		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    முழு		x;
    fold_T	*fp;
    முழு		level = 0;
    முழு		use_level = FALSE;
    முழு		maybe_small = FALSE;
    garray_T	*gap;
    முழு		கீழ்_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    எனில் (!hasAnyFolding(win))
    {
	எனில் (infop != வெற்று)
	    infop->fi_level = 0;
	திரும்பு FALSE;
    }

    எனில் (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	எனில் (x >= 0)
	{
	    first = win->w_வரிகள்[x].wl_lnum;
	    last = win->w_வரிகள்[x].wl_lastlnum;
	    had_folded = win->w_வரிகள்[x].wl_folded;
	}
    }

    எனில் (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	க்கு (;;)
	{
	    எனில் (!foldFind(gap, lnum_rel, &fp))
		நிறுத்து;

	    // Remember lowest level of fold that starts in "lnum".
	    எனில் (lnum_rel == fp->fd_உச்சி && கீழ்_level == 0)
		கீழ்_level = level + 1;

	    first += fp->fd_உச்சி;
	    last += fp->fd_உச்சி;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    எனில் (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		நிறுத்து;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_உச்சி;
	    ++level;
	}
    }

    எனில் (!had_folded)
    {
	எனில் (infop != வெற்று)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_கீழ்_level = கீழ்_level == 0 ? level : கீழ்_level;
	}
	திரும்பு FALSE;
    }

    எனில் (last > win->w_buffer->b_ml.ml_வரி_மொத்தம்)
	last = win->w_buffer->b_ml.ml_வரி_மொத்தம்;
    எனில் (lastp != வெற்று)
	*lastp = last;
    எனில் (firstp != வெற்று)
	*firstp = first;
    எனில் (infop != வெற்று)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_கீழ்_level = கீழ்_level == 0 ? level + 1 : கீழ்_level;
    }
    திரும்பு TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    முழு
வரி_Folded(win_T *win, linenr_T lnum)
{
    திரும்பு folded_மொத்தம்(win, lnum, வெற்று) != 0;
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
folded_மொத்தம்(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    எனில் (hasFoldingWin(win, lnum, வெற்று, &last, FALSE, infop))
	திரும்பு (long)(last - lnum + 1);
    திரும்பு 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    முழு
foldmethodIsManual(win_T *wp)
{
    திரும்பு (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    முழு
foldmethodIsIndent(win_T *wp)
{
    திரும்பு (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    முழு
foldmethod_Is_கோவை(win_T *wp)
{
    திரும்பு (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    முழு
foldmethodIsMarker(win_T *wp)
{
    திரும்பு (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    முழு
foldmethodIsSyntax(win_T *wp)
{
    திரும்பு (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    முழு
foldmethodIsDiff(win_T *wp)
{
    திரும்பு (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    வெறுமை
closeFold(linenr_T lnum, long மொத்தம்)
{
    setFoldRepeat(lnum, மொத்தம், FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    வெறுமை
closeFoldRecurse(linenr_T lnum)
{
    (வெறுமை)setManualFold(lnum, FALSE, TRUE, வெற்று);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    வெறுமை
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    முழு		opening,	// TRUE to open, FALSE to close
    முழு		recurse,	// TRUE to do it recursively
    முழு		had_visual)	// TRUE when Visual selection used
{
    முழு		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_next;

    க்கு (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	எனில் (opening && !recurse)
	    (வெறுமை)hasFolding(lnum, வெற்று, &lnum_next);
	(வெறுமை)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	எனில் (!opening && !recurse)
	    (வெறுமை)hasFolding(lnum, வெற்று, &lnum_next);
    }
    எனில் (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    எனில் (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    வெறுமை
openFold(linenr_T lnum, long மொத்தம்)
{
    setFoldRepeat(lnum, மொத்தம், TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    வெறுமை
openFoldRecurse(linenr_T lnum)
{
    (வெறுமை)setManualFold(lnum, TRUE, TRUE, வெற்று);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    வெறுமை
foldOpenCursor(வெறுமை)
{
    முழு		done;

    checkupdate(curwin);
    எனில் (hasAnyFolding(curwin))
	க்கு (;;)
	{
	    done = DONE_NOTHING;
	    (வெறுமை)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    எனில் (!(done & DONE_ACTION))
		நிறுத்து;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    வெறுமை
newFoldLevel(வெறுமை)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    எனில் (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    நிலையான வெறுமை
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    முழு		i;

    checkupdate(wp);
    எனில் (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_data;
	க்கு (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    வெறுமை
foldCheckClose(வெறுமை)
{
    எனில் (*p_fcl == NUL)
	திரும்பு;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    எனில் (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(முழு)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    நிலையான முழு
checkCloseRec(garray_T *gap, linenr_T lnum, முழு level)
{
    fold_T	*fp;
    முழு		retval = FALSE;
    முழு		i;

    fp = (fold_T *)gap->ga_data;
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	எனில் (fp[i].fd_flags == FD_OPEN)
	{
	    எனில் (level <= 0 && (lnum < fp[i].fd_உச்சி
				      || lnum >= fp[i].fd_உச்சி + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    இல்லையெனில்
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_உச்சி,
								   level - 1);
	}
    }
    திரும்பு retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    முழு
foldManualAllowed(முழு create)
{
    எனில் (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	திரும்பு TRUE;
    எனில் (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    இல்லையெனில்
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    திரும்பு FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    வெறுமை
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    முழு		i, j;
    முழு		cont;
    முழு		use_level = FALSE;
    முழு		closed = FALSE;
    முழு		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    எனில் (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    எனில் (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	திரும்பு;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    எனில் (gap->ga_len == 0)
	i = 0;
    இல்லையெனில்
    {
	க்கு (;;)
	{
	    எனில் (!foldFind(gap, start_rel, &fp))
		நிறுத்து;
	    எனில் (fp->fd_உச்சி + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_உச்சி;
		end_rel -= fp->fd_உச்சி;
		எனில் (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    எனில் (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		இல்லையெனில் எனில் (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    இல்லையெனில்
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		நிறுத்து;
	    }
	}
	எனில் (gap->ga_len == 0)
	    i = 0;
	இல்லையெனில்
	    i = (முழு)(fp - (fold_T *)gap->ga_data);
    }

    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு;

    fp = (fold_T *)gap->ga_data + i;
    ga_init2(&fold_ga, அளவு(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    க்கு (cont = 0; i + cont < gap->ga_len; ++cont)
	எனில் (fp[cont].fd_உச்சி > end_rel)
	    நிறுத்து;
    எனில் (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	எனில் (start_rel > fp->fd_உச்சி)
	    start_rel = fp->fd_உச்சி;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	எனில் (end_rel < fp[cont - 1].fd_உச்சி + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_உச்சி + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_data, fp, அளவு(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	க்கு (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_data)[j].fd_உச்சி -= start_rel;
    }
    // Move remaining entries to after the new fold.
    எனில் (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_data + i,
		அளவு(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_உச்சி = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    எனில் (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    எனில் (!use_level)
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
    வெறுமை
deleteFold(
    linenr_T	start,
    linenr_T	end,
    முழு		recursive,
    முழு		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = வெற்று;
    linenr_T	found_off = 0;
    முழு		use_level;
    முழு		maybe_small = FALSE;
    முழு		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    முழு		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    போது (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = வெற்று;
	lnum_off = 0;
	use_level = FALSE;
	க்கு (;;)
	{
	    எனில் (!foldFind(gap, lnum - lnum_off, &fp))
		நிறுத்து;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    எனில் (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		நிறுத்து;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_உச்சி;
	    ++level;
	}
	எனில் (found_ga == வெற்று)
	{
	    ++lnum;
	}
	இல்லையெனில்
	{
	    lnum = found_fp->fd_உச்சி + found_fp->fd_len + found_off;

	    எனில் (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (முழு)(found_fp - (fold_T *)found_ga->ga_data), recursive);
	    இல்லையெனில்
	    {
		எனில் (first_lnum > found_fp->fd_உச்சி + found_off)
		    first_lnum = found_fp->fd_உச்சி + found_off;
		எனில் (last_lnum < lnum)
		    last_lnum = lnum;
		எனில் (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    எனில் (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	எனில் (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    இல்லையெனில்
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    எனில் (last_lnum > 0)
	changed_வரிகள்(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    வெறுமை
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
    வெறுமை
foldUpdate(win_T *wp, linenr_T உச்சி, linenr_T bot)
{
    fold_T	*fp;

    எனில் (disable_fold_update > 0)
	திரும்பு;
# ifdef FEAT_DIFF
    எனில் (need_diff_redraw)
	// will update later
	திரும்பு;
# endif

    எனில் (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = உச்சி;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	எனில் (உச்சி > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = உச்சி;
	}
	(வெறுமை)foldFind(&wp->w_folds, maybe_small_start, &fp);
	போது (fp < (fold_T *)wp->w_folds.ga_data + wp->w_folds.ga_len
		&& fp->fd_உச்சி <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    எனில் (foldmethodIsIndent(wp)
	    || foldmethod_Is_கோவை(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	முழு save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, உச்சி, bot);
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
    வெறுமை
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
    முழு
foldMoveTo(
    முழு		updown,
    முழு		dir,	    // FORWARD or BACKWARD
    long	மொத்தம்)
{
    long	n;
    முழு		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    முழு		use_level;
    முழு		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    முழு		level;
    முழு		last;

    checkupdate(curwin);

    // Repeat "count" times.
    க்கு (n = 0; n < மொத்தம்; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	எனில் (gap->ga_len == 0)
	    நிறுத்து;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	க்கு (;;)
	{
	    எனில் (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		எனில் (!updown || gap->ga_len == 0)
		    நிறுத்து;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		எனில் (dir == FORWARD)
		{
		    எனில் (fp - (fold_T *)gap->ga_data >= gap->ga_len)
			நிறுத்து;
		    --fp;
		}
		இல்லையெனில்
		{
		    எனில் (fp == (fold_T *)gap->ga_data)
			நிறுத்து;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    எனில் (!last)
	    {
		// Check if this fold is closed.
		எனில் (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		எனில் (last && !updown)
		    நிறுத்து;
	    }

	    எனில் (updown)
	    {
		எனில் (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    எனில் (fp + 1 - (fold_T *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_உச்சி + lnum_off;
			எனில் (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		இல்லையெனில்
		{
		    // to end of previous fold if there is one
		    எனில் (fp > (fold_T *)gap->ga_data)
		    {
			lnum = fp[-1].fd_உச்சி + lnum_off + fp[-1].fd_len - 1;
			எனில் (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    இல்லையெனில்
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		எனில் (dir == FORWARD)
		{
		    lnum = fp->fd_உச்சி + lnum_off + fp->fd_len - 1;
		    எனில் (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		இல்லையெனில்
		{
		    lnum = fp->fd_உச்சி + lnum_off;
		    எனில் (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    எனில் (last)
		நிறுத்து;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_உச்சி;
	    ++level;
	}
	எனில் (lnum_found != curwin->w_cursor.lnum)
	{
	    எனில் (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	இல்லையெனில்
	    நிறுத்து;
    }

    திரும்பு retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    வெறுமை
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, அளவு(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    முழு
find_wl_entry(win_T *win, linenr_T lnum)
{
    முழு		i;

    க்கு (i = 0; i < win->w_வரிகள்_valid; ++i)
	எனில் (win->w_வரிகள்[i].wl_valid)
	{
	    எனில் (lnum < win->w_வரிகள்[i].wl_lnum)
		திரும்பு -1;
	    எனில் (lnum <= win->w_வரிகள்[i].wl_lastlnum)
		திரும்பு i;
	}
    திரும்பு -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    வெறுமை
foldAdjustVisual(வெறுமை)
{
    pos_T	*start, *end;

    எனில் (!VIsual_active || !hasAnyFolding(curwin))
	திரும்பு;

    எனில் (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    இல்லையெனில்
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    எனில் (hasFolding(start->lnum, &start->lnum, வெற்று))
	start->col = 0;

    எனில் (!hasFolding(end->lnum, வெற்று, &end->lnum))
	திரும்பு;

    end->col = ml_get_len(end->lnum);
    எனில் (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    எனில் (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    வெறுமை
foldAdjustCursor(வெறுமை)
{
    (வெறுமை)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, வெற்று);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    வெறுமை
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    முழு		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    எனில் (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	திரும்பு;

    from_p = (fold_T *)from->ga_data;
    to_p = (fold_T *)to->ga_data;

    க்கு (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_உச்சி = from_p->fd_உச்சி;
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
    நிலையான முழு
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	கீழ், உயர்வு;
    fold_T	*fp;
    முழு		i;

    எனில் (gap->ga_len == 0)
    {
	*fpp = வெற்று;
	திரும்பு FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_data;
    கீழ் = 0;
    உயர்வு = gap->ga_len - 1;
    போது (கீழ் <= உயர்வு)
    {
	i = (கீழ் + உயர்வு) / 2;
	எனில் (fp[i].fd_உச்சி > lnum)
	    // fold below lnum, adjust high
	    உயர்வு = i - 1;
	இல்லையெனில் எனில் (fp[i].fd_உச்சி + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    கீழ் = i + 1;
	இல்லையெனில்
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    திரும்பு TRUE;
	}
    }
    *fpp = fp + கீழ்;
    திரும்பு FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    நிலையான முழு
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    முழு		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    க்கு (;;)
    {
	எனில் (!foldFind(gap, lnum_rel, &fp))
	    நிறுத்து;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_உச்சி;
	++level;
    }

    திரும்பு level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    நிலையான வெறுமை
checkupdate(win_T *wp)
{
    எனில் (!wp->w_foldinvalid)
	திரும்பு;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    நிலையான வெறுமை
setFoldRepeat(linenr_T lnum, long மொத்தம், முழு do_open)
{
    முழு		done;
    long	n;

    க்கு (n = 0; n < மொத்தம்; ++n)
    {
	done = DONE_NOTHING;
	(வெறுமை)setManualFold(lnum, do_open, FALSE, &done);
	எனில் (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    எனில் (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    நிறுத்து;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    நிலையான linenr_T
setManualFold(
    linenr_T	lnum,
    முழு		opening,    // TRUE when opening, FALSE when closing
    முழு		recurse,    // TRUE when closing/opening recursive
    முழு		*donep)
{
# ifdef FEAT_DIFF
    எனில் (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		எனில் (dlnum != 0)
		    (வெறுமை)setManualFoldWin(wp, dlnum, opening, recurse, வெற்று);
	    }
	}
    }
# endif

    திரும்பு setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    நிலையான linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    முழு		opening,    // TRUE when opening, FALSE when closing
    முழு		recurse,    // TRUE when closing/opening recursive
    முழு		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = வெற்று;
    முழு		j;
    முழு		level = 0;
    முழு		use_level = FALSE;
    முழு		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	next = MAXLNUM;
    linenr_T	off = 0;
    முழு		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    க்கு (;;)
    {
	எனில் (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    எனில் (fp != வெற்று && fp < (fold_T *)gap->ga_data + gap->ga_len)
		next = fp->fd_உச்சி + off;
	    நிறுத்து;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	எனில் (fp + 1 < (fold_T *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_உச்சி + off;

	// Change from level-dependent folding to manual.
	எனில் (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    எனில் (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    இல்லையெனில்
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_data;
	    க்கு (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	எனில் (!opening && recurse)
	{
	    எனில் (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	இல்லையெனில் எனில் (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    எனில் (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		எனில் (recurse)
		    foldOpenNested(fp);
	    }
	    நிறுத்து;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_உச்சி;
	off += fp->fd_உச்சி;
	++level;
    }
    எனில் (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	எனில் (!opening && found != வெற்று)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	எனில் (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    இல்லையெனில் எனில் (donep == வெற்று && wp == curwin)
	emsg(_(e_no_fold_found));

    எனில் (donep != வெற்று)
	*donep |= done;

    திரும்பு next;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    நிலையான வெறுமை
foldOpenNested(fold_T *fpr)
{
    முழு		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_data;
    க்கு (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    நிலையான வெறுமை
deleteFoldEntry(garray_T *gap, முழு idx, முழு recursive)
{
    fold_T	*fp;
    முழு		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_data + idx;
    எனில் (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	எனில் (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, அளவு(fold_T) * (gap->ga_len - idx));
    }
    இல்லையெனில்
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	எனில் (ga_grow(gap, (முழு)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_data + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_data;
	    க்கு (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_உச்சி += fp->fd_உச்சி;
		எனில் (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		எனில் (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    எனில் (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  அளவு(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(அளவு(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    வெறுமை
deleteFoldRecurse(garray_T *gap)
{
    முழு		i;

    க்கு (i = 0; i < gap->ga_len; ++i)
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
    வெறுமை
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    எனில் (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    எனில் (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    எனில் ((State & MODE_செருகு) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    நிலையான வெறுமை
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    முழு		i;
    linenr_T	last;
    linenr_T	உச்சி;

    எனில் (gap->ga_len == 0)
	திரும்பு;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    எனில் ((State & MODE_செருகு) && amount == (linenr_T)1 && line2 == MAXLNUM)
	உச்சி = line1 + 1;
    இல்லையெனில்
	உச்சி = line1;

    // Find the fold containing or just below "line1".
    (வெறுமை)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    க்கு (i = (முழு)(fp - (fold_T *)gap->ga_data); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_உச்சி + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	எனில் (last < line1)
	    தொடர்;

	// 6. fold below line2: only adjust for amount_after
	எனில் (fp->fd_உச்சி > line2)
	{
	    எனில் (amount_after == 0)
		நிறுத்து;
	    fp->fd_உச்சி += amount_after;
	}
	இல்லையெனில்
	{
	    எனில் (fp->fd_உச்சி >= உச்சி && last <= line2)
	    {
		// 4. fold completely contained in range
		எனில் (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		இல்லையெனில்
		    fp->fd_உச்சி += amount;
	    }
	    இல்லையெனில்
	    {
		எனில் (fp->fd_உச்சி < உச்சி)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_உச்சி,
				  line2 - fp->fd_உச்சி, amount, amount_after);
		    எனில் (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			எனில் (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_உச்சி;
			இல்லையெனில்
			    fp->fd_len += amount;
		    }
		    இல்லையெனில்
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		இல்லையெனில்
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    எனில் (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_உச்சி,
				  amount,
				  amount_after + (fp->fd_உச்சி - உச்சி));
			fp->fd_len -= line2 - fp->fd_உச்சி + 1;
			fp->fd_உச்சி = line1;
		    }
		    இல்லையெனில்
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_உச்சி,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_உச்சி += amount;
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
    முழு
getDeepestNesting(வெறுமை)
{
    checkupdate(curwin);
    திரும்பு getDeepestNestingRecurse(&curwin->w_folds);
}

    நிலையான முழு
getDeepestNestingRecurse(garray_T *gap)
{
    முழு		i;
    முழு		level;
    முழு		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	எனில் (level > maxlevel)
	    maxlevel = level;
    }

    திரும்பு maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    நிலையான முழு
check_closed(
    win_T	*win,
    fold_T	*fp,
    முழு		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    முழு		level,		    // folding depth
    முழு		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    முழு		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    எனில் (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	எனில் (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    இல்லையெனில் எனில் (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    எனில் (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    எனில் (closed)
    {
	எனில் (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	எனில் (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    திரும்பு closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    நிலையான வெறுமை
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    முழு		மொத்தம்;
    முழு		n;

    எனில் (fp->fd_small != MAYBE)
	திரும்பு;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    எனில் (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    இல்லையெனில்
    {
	மொத்தம் = 0;
	க்கு (n = 0; n < fp->fd_len; ++n)
	{
	    மொத்தம் += plines_win_nofold(wp, fp->fd_உச்சி + lnum_off + n);
	    எனில் (மொத்தம் > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		திரும்பு;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    நிலையான வெறுமை
setSmallMaybe(garray_T *gap)
{
    முழு		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    க்கு (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    நிலையான வெறுமை
foldCreateMarkers(linenr_T start, linenr_T end)
{
    எனில் (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	திரும்பு;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_வரிகள்(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    நிலையான வெறுமை
foldAddMarker(linenr_T lnum, char_u *marker, முழு markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*வரி;
    முழு		வரி_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((எழுத்து *)curbuf->b_p_cms, "%s");
    முழு		வரி_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    வரி = ml_get(lnum);
    வரி_len = ml_get_len(lnum);

    எனில் (u_save(lnum - 1, lnum + 1) != OK)
	திரும்பு;

    // Check if the line ends with an unclosed comment
    (வெறுமை)skip_comment(வரி, FALSE, FALSE, &வரி_is_comment);
    newline = alloc(வரி_len + markerlen + STRLEN(cms) + 1);
    எனில் (newline == வெற்று)
	திரும்பு;
    STRCPY(newline, வரி);
    // Append the marker to the end of the line
    எனில் (p == வெற்று || வரி_is_comment)
	vim_strncpy(newline + வரி_len, marker, markerlen);
    இல்லையெனில்
    {
	STRCPY(newline + வரி_len, cms);
	STRNCPY(newline + வரி_len + (p - cms), marker, markerlen);
	STRCPY(newline + வரி_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    நிலையான வெறுமை
deleteFoldMarkers(
    fold_T	*fp,
    முழு		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    முழு		i;

    எனில் (recursive)
	க்கு (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_உச்சி);
    foldDelMarker(fp->fd_உச்சி + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_உச்சி + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    நிலையான வெறுமை
foldDelMarker(linenr_T lnum, char_u *marker, முழு markerlen)
{
    char_u	*வரி;
    char_u	*newline;
    char_u	*p;
    முழு		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு;
    வரி = ml_get(lnum);
    க்கு (p = வரி; *p != NUL; ++p)
	எனில் (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    எனில் (VIM_ISDIGIT(p[len]))
		++len;
	    எனில் (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((எழுத்து *)cms, "%s");
		எனில் (cms2 != வெற்று && p - வரி >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (முழு)STRLEN(cms) - 2;
		}
	    }
	    எனில் (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		எனில் (newline != வெற்று)
		{
		    STRNCPY(newline, வரி, p - வரி);
		    STRCPY(newline + (p - வரி), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    நிறுத்து;
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
    char_u	*text = வெற்று;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    நிலையான முழு	    got_fdt_error = FALSE;
    முழு		    save_did_emsg = did_emsg;
    நிலையான win_T    *last_wp = வெற்று;
    நிலையான linenr_T last_lnum = 0;

    எனில் (last_wp != wp || last_wp == வெற்று
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    எனில் (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    எனில் (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	முழு	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	எனில் (level > (முழு)அளவு(dashes) - 1)
	    level = (முழு)அளவு(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	எனில் (!got_fdt_error)
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

	    எனில் (text == வெற்று || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, வெற்று, -1);

	எனில் (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	எனில் (text != வெற்று)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    க்கு (p = text; *p != NUL; ++p)
	    {
		முழு	len;

		எனில் (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    எனில் (!vim_isprintc((*mb_ptr2char)(p)))
			நிறுத்து;
		    p += len - 1;
		}
		இல்லையெனில்
		    எனில் (*p == TAB)
			*p = ' ';
		    இல்லையெனில் எனில் (ptr2cells(p) > 1)
			நிறுத்து;
	    }
	    எனில் (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    எனில் (text == வெற்று)
# endif
    {
	long மொத்தம் = (long)(lnume - lnum + 1);

	vim_snprintf((எழுத்து *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", மொத்தம்),
		     மொத்தம்);
	text = buf;
    }
    திரும்பு text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    நிலையான வெறுமை
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    முழு		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    முழு		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    முழு		len;
    முழு		did1 = FALSE;
    முழு		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (முழு)STRLEN(cms_start);
    போது (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((எழுத்து *)cms_start, "%s");
    எனில் (cms_end != வெற்று)
    {
	cms_elen = cms_slen - (முழு)(cms_end - cms_start);
	cms_slen = (முழு)(cms_end - cms_start);

	// exclude white space before "%s"
	போது (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (முழு)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    க்கு (s = str; *s != NUL; )
    {
	len = 0;
	எனில் (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	இல்லையெனில் எனில் (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	எனில் (len > 0)
	{
	    எனில் (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    க்கு (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    எனில் (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (முழு)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	இல்லையெனில் எனில் (cms_end != வெற்று)
	{
	    எனில் (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    இல்லையெனில் எனில் (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	எனில் (len != 0)
	{
	    போது (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	இல்லையெனில்
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
வகைவரை கட்டமைப்பு
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    முழு		lvl;		// current level (-1 for undefined)
    முழு		lvl_next;	// level used for next line
    முழு		start;		// number of folds that are forced to start at
				// this line.
    முழு		end;		// level of fold that is forced to end below
				// this line
    முழு		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
நிலையான முழு fold_changed;

// Function declarations. {{{2
நிலையான linenr_T foldUpdateIEMSRecurse(garray_T *gap, முழு level, linenr_T startlnum, fline_T *flp, வெறுமை (*getlevel)(fline_T *), linenr_T bot, முழு topflags);
நிலையான முழு fold_செருகு(garray_T *gap, முழு i);
நிலையான வெறுமை foldSplit(garray_T *gap, முழு i, linenr_T உச்சி, linenr_T bot);
நிலையான வெறுமை foldRemove(garray_T *gap, linenr_T உச்சி, linenr_T bot);
நிலையான வெறுமை foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
நிலையான வெறுமை foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
நிலையான வெறுமை foldlevelDiff(fline_T *flp);
# endif
நிலையான வெறுமை foldlevel_கோவை(fline_T *flp);
நிலையான வெறுமை foldlevelMarker(fline_T *flp);
நிலையான வெறுமை foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    நிலையான வெறுமை
foldUpdateIEMS(win_T *wp, linenr_T உச்சி, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    வெறுமை	(*getlevel)(fline_T *);
    முழு		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    எனில் (invalid_உச்சி != (linenr_T)0)
	திரும்பு;

    எனில் (wp->w_foldinvalid)
    {
	// Need to update all folds.
	உச்சி = 1;
	bot = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    எனில் (foldmethodIsDiff(wp))
    {
	எனில் (உச்சி > diff_context)
	    உச்சி -= diff_context;
	இல்லையெனில்
	    உச்சி = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    எனில் (உச்சி > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	உச்சி = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_உச்சி = உச்சி;
    invalid_bot = bot;

    எனில் (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	எனில் (உச்சி > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, உச்சி - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = உச்சி - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    எனில் (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_next);
	    இல்லையெனில்
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = உச்சி;
	getlevel(&fline);
    }
    இல்லையெனில்
    {
	fline.lnum = உச்சி;
	எனில் (foldmethod_Is_கோவை(wp))
	{
	    getlevel = foldlevel_கோவை;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    எனில் (உச்சி > 1)
		--fline.lnum;
	}
	இல்லையெனில் எனில் (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	இல்லையெனில் எனில் (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	இல்லையெனில்
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    எனில் (உச்சி > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	க்கு ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    எனில் (fline.lvl >= 0)
		நிறுத்து;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    எனில் (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = வெற்று;
	முழு	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	போது (current_fdl < fline.lvl)
	{
	    எனில் (!foldFind(gap, lnum_rel, &fpn))
		நிறுத்து;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_உச்சி;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_உச்சி;
	}
	எனில் (fpn != வெற்று && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    எனில் (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    எனில் (start > end && end < wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	end = start;
    போது (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	எனில் (fline.lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;
	எனில் (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    எனில் (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_கோவை)
		நிறுத்து;
	    எனில் ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_உச்சி + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_உச்சி < fline.lnum))
		end = fp->fd_உச்சி + fp->fd_len - 1;
	    இல்லையெனில் எனில் (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    இல்லையெனில்
		நிறுத்து;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	எனில் (fline.lvl > 0)
	{
	    invalid_உச்சி = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	இல்லையெனில்
	{
	    எனில் (fline.lnum == wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
		நிறுத்து;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    எனில் (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    எனில் (end != bot)
	redraw_win_range_later(wp, உச்சி, end);

    invalid_உச்சி = (linenr_T)0;
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
    நிலையான linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    முழு		level,
    linenr_T	startlnum,
    fline_T	*flp,
    வெறுமை	(*getlevel)(fline_T *),
    linenr_T	bot,
    முழு		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = வெற்று;
    fold_T	*fp2;
    முழு		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    முழு		i;
    முழு		finish = FALSE;
    linenr_T	linecount = flp->wp->w_buffer->b_ml.ml_வரி_மொத்தம் - flp->off;
    முழு		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    எனில் (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(வெறுமை)foldFind(gap, startlnum - 1, &fp);
	எனில் (fp != வெற்று && (fp >= ((fold_T *)gap->ga_data) + gap->ga_len
						   || fp->fd_உச்சி >= startlnum))
	    fp = வெற்று;
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
    போது (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	வரி_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	எனில் (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	எனில் (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	எனில் (flp->lnum > bot && !finish && fp != வெற்று)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    எனில் (getlevel != foldlevelMarker
		    && getlevel != foldlevel_கோவை
		    && getlevel != foldlevelSyntax)
		நிறுத்து;
	    i = 0;
	    fp2 = fp;
	    எனில் (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_உச்சி;
		போது (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_உச்சி;
		}
	    }
	    எனில் (lvl < level + i)
	    {
		(வெறுமை)foldFind(&fp->fd_nested, flp->lnum - fp->fd_உச்சி, &fp2);
		எனில் (fp2 != வெற்று)
		    bot = fp2->fd_உச்சி + fp2->fd_len - 1 + fp->fd_உச்சி;
	    }
	    இல்லையெனில் எனில் (fp->fd_உச்சி + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    இல்லையெனில்
		நிறுத்து;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	எனில் (fp == வெற்று
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
	    போது (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		எனில் (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		இல்லையெனில்
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		எனில் (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && fp->fd_உச்சி <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_உச்சி < flp->lnum)
				|| (lvl >= level
					   && fp->fd_உச்சி <= flp->lnum_save)))))
		{
		    எனில் (fp->fd_உச்சி + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			எனில் (fp->fd_உச்சி == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			இல்லையெனில் எனில் (fp->fd_உச்சி >= startlnum)
			{
			    எனில் (fp->fd_உச்சி > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_உச்சி - firstlnum), 0L);
			    இல்லையெனில்
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_உச்சி - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_உச்சி - firstlnum));
			    fp->fd_len += fp->fd_உச்சி - firstlnum;
			    fp->fd_உச்சி = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			இல்லையெனில் எனில் ((flp->start != 0 && lvl == level)
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
			    எனில் (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    இல்லையெனில்
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_உச்சி,
						      breakend - fp->fd_உச்சி);
			    i = (முழு)(fp - (fold_T *)gap->ga_data);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_data + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    எனில் (getlevel == foldlevelMarker
				    || getlevel == foldlevel_கோவை
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			எனில் (fp->fd_உச்சி == startlnum && concat)
			{
			    i = (முழு)(fp - (fold_T *)gap->ga_data);
			    எனில் (i != 0)
			    {
				fp2 = fp - 1;
				எனில் (fp2->fd_உச்சி + fp2->fd_len == fp->fd_உச்சி)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			நிறுத்து;
		    }
		    எனில் (fp->fd_உச்சி >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (முழு)(fp - (fold_T *)gap->ga_data), TRUE);
		    }
		    இல்லையெனில்
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_உச்சி;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		இல்லையெனில்
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    எனில் (gap->ga_len == 0)
			i = 0;
		    இல்லையெனில்
			i = (முழு)(fp - (fold_T *)gap->ga_data);
		    எனில் (fold_செருகு(gap, i) != OK)
			திரும்பு bot;
		    fp = (fold_T *)gap->ga_data + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_உச்சி = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    எனில் (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    இல்லையெனில் எனில் (i <= 0)
		    {
			fp->fd_flags = topflags;
			எனில் (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    இல்லையெனில்
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    எனில் (getlevel == foldlevelMarker
			    || getlevel == foldlevel_கோவை
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    நிறுத்து;
		}
	    }
	}

	எனில் (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    நிறுத்து;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	எனில் (lvl > level && fp != வெற்று)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    எனில் (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_உச்சி;
	    flp->off += fp->fd_உச்சி;
	    i = (முழு)(fp - (fold_T *)gap->ga_data);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_உச்சி, flp, getlevel,
					      bot - fp->fd_உச்சி, fp->fd_flags);
	    fp = (fold_T *)gap->ga_data + i;
	    flp->lnum += fp->fd_உச்சி;
	    flp->lnum_save += fp->fd_உச்சி;
	    flp->off -= fp->fd_உச்சி;
	    bot += fp->fd_உச்சி;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	இல்லையெனில்
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    போது (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		எனில் (++flp->lnum > linecount)
		    நிறுத்து;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		எனில் (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    நிறுத்து;
	    }
	    prev_lnum = 0;
	    எனில் (flp->lnum > linecount)
		நிறுத்து;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    எனில் (fp == வெற்று)	// only happens when got_int is set
	திரும்பு bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    எனில் (fp->fd_len < flp->lnum - fp->fd_உச்சி)
    {
	fp->fd_len = flp->lnum - fp->fd_உச்சி;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    இல்லையெனில் எனில் (fp->fd_உச்சி + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_உச்சி + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_உச்சி,
						  flp->lnum - 1 - fp->fd_உச்சி);

    எனில் (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	எனில் (fp->fd_len != flp->lnum - fp->fd_உச்சி)
	{
	    எனில் (fp->fd_உச்சி + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		எனில் (getlevel == foldlevelMarker
			|| getlevel == foldlevel_கோவை
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_உச்சி + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_உச்சி;
		}
		இல்லையெனில்
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (முழு)(fp - (fold_T *)gap->ga_data);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_data + i;
		}
	    }
	    இல்லையெனில்
		fp->fd_len = flp->lnum - fp->fd_உச்சி;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    க்கு (;;)
    {
	fp2 = fp + 1;
	எனில் (fp2 >= (fold_T *)gap->ga_data + gap->ga_len
						  || fp2->fd_உச்சி > flp->lnum)
	    நிறுத்து;
	எனில் (fp2->fd_உச்சி + fp2->fd_len > flp->lnum)
	{
	    எனில் (fp2->fd_உச்சி < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_உச்சி - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_உச்சி - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_உச்சி;
		fp2->fd_உச்சி = flp->lnum;
		fold_changed = TRUE;
	    }

	    எனில் (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    நிறுத்து;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (முழு)(fp2 - (fold_T *)gap->ga_data), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    எனில் (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    திரும்பு bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    நிலையான முழு
fold_செருகு(garray_T *gap, முழு i)
{
    fold_T	*fp;

    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு FAIL;
    fp = (fold_T *)gap->ga_data + i;
    எனில் (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, அளவு(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, அளவு(fold_T), 10);
    திரும்பு OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    நிலையான வெறுமை
foldSplit(
    garray_T	*gap,
    முழு		i,
    linenr_T	உச்சி,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    முழு		idx;
    முழு		len;

    // The fold continues below bot, need to split it.
    எனில் (fold_செருகு(gap, i + 1) == FAIL)
	திரும்பு;
    fp = (fold_T *)gap->ga_data + i;
    fp[1].fd_உச்சி = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_உச்சி - fp->fd_உச்சி);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (வெறுமை)foldFind(gap1, bot + 1 - fp->fd_உச்சி, &fp2);
    எனில் (fp2 != வெற்று)
    {
	len = (முழு)((fold_T *)gap1->ga_data + gap1->ga_len - fp2);
	எனில் (len > 0 && ga_grow(gap2, len) == OK)
	{
	    க்கு (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_data)[idx] = fp2[idx];
		((fold_T *)gap2->ga_data)[idx].fd_உச்சி
						  -= fp[1].fd_உச்சி - fp->fd_உச்சி;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = உச்சி - fp->fd_உச்சி;
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
    நிலையான வெறுமை
foldRemove(garray_T *gap, linenr_T உச்சி, linenr_T bot)
{
    fold_T	*fp = வெற்று;

    எனில் (bot < உச்சி)
	திரும்பு;		// nothing to do

    போது (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	எனில் (foldFind(gap, உச்சி, &fp) && fp->fd_உச்சி < உச்சி)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, உச்சி - fp->fd_உச்சி, bot - fp->fd_உச்சி);
	    எனில் (fp->fd_உச்சி + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (முழு)(fp - (fold_T *)gap->ga_data), உச்சி, bot);
	    }
	    இல்லையெனில்
	    {
		// 2: truncate fold at "top".
		fp->fd_len = உச்சி - fp->fd_உச்சி;
	    }
	    fold_changed = TRUE;
	    தொடர்;
	}
	எனில் (fp >= (fold_T *)(gap->ga_data) + gap->ga_len
		|| fp->fd_உச்சி > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    நிறுத்து;
	}
	எனில் (fp->fd_உச்சி >= உச்சி)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    எனில் (fp->fd_உச்சி + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_உச்சி),
			(linenr_T)MAXLNUM, (long)(fp->fd_உச்சி - bot - 1));
		fp->fd_len -= bot - fp->fd_உச்சி + 1;
		fp->fd_உச்சி = bot + 1;
		நிறுத்து;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (முழு)(fp - (fold_T *)gap->ga_data), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    நிலையான வெறுமை
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *இடது, *வலது;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    க்கு (; start < end; start++, end--)
    {
	இடது = (fold_T *)gap->ga_data + start;
	வலது = (fold_T *)gap->ga_data + end;
	tmp  = *இடது;
	*இடது = *வலது;
	*வலது = tmp;
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

    நிலையான வெறுமை
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_உச்சி, MAXLNUM);
    fp->fd_len = end - fp->fd_உச்சி;
}

# define fold_end(fp) ((fp)->fd_உச்சி + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_data + (gap)->ga_len))
# define fold_index(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_data)))

    வெறுமை
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    முழு at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_index = 0;

    எனில் (at_start)
    {
	எனில் (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_உச்சி, line2 -
		    fp->fd_உச்சி, dest - fp->fd_உச்சி);
	    திரும்பு;
	}
	இல்லையெனில் எனில் (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_உச்சி, line2 -
		    fp->fd_உச்சி, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	இல்லையெனில்
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    எனில் (!valid_fold(fp, gap) || fp->fd_உச்சி > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	திரும்பு;
    }
    இல்லையெனில் எனில் (fp->fd_உச்சி > line2)
    {
	க்கு (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_உச்சி -= range_len;

	எனில் (valid_fold(fp, gap) && fp->fd_உச்சி <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_உச்சி -= range_len;
	}
	திரும்பு;
    }
    இல்லையெனில் எனில் (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_உச்சி, dest -
		fp->fd_உச்சி, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_உச்சி += move_len;
	திரும்பு;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_index(fp, gap);

    க்கு (; valid_fold(fp, gap) && fp->fd_உச்சி <= dest; fp++)
    {
	எனில் (fp->fd_உச்சி <= line2)
	{
	    // 1. 2. or 3.
	    எனில் (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_உச்சி += move_len;
	    தொடர்;
	}

	// Record index of the first fold after the moved range.
	எனில் (move_end == 0)
	    move_end = fold_index(fp, gap);

	எனில் (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_உச்சி -= range_len;
    }

    dest_index = fold_index(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    எனில் (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	திரும்பு;
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
    நிலையான வெறுமை
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    முழு		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    எனில் (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    எனில் (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	க்கு (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_data)[gap1->ga_len]
					= ((fold_T *)gap2->ga_data)[idx];
	    ((fold_T *)gap1->ga_data)[gap1->ga_len].fd_உச்சி += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (முழு)(fp2 - (fold_T *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    நிலையான வெறுமை
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    எனில் (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != வெற்று)
    {
	// first and last line can't be undefined, use level 0
	எனில் (lnum == 1 || lnum == buf->b_ml.ml_வரி_மொத்தம்)
	    flp->lvl = 0;
	இல்லையெனில்
	    flp->lvl = -1;
    }
    இல்லையெனில்
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_மதிப்பு(buf);
    எனில் (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	எனில் (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    நிலையான வெறுமை
foldlevelDiff(fline_T *flp)
{
    எனில் (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    இல்லையெனில்
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    நிலையான வெறுமை
foldlevel_கோவை(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# இல்லையெனில்
    win_T	*win;
    முழு		n;
    முழு		c;
    linenr_T	lnum = flp->lnum + flp->off;
    முழு		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    எனில் (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = சாவி_Typed;
    n = eval_foldexpr(flp->wp, &c);
    சாவி_Typed = save_keytyped;

    தேர்வு (c)
    {
	// "a1", "a2", .. : add to the fold level
	நிலை 'a': எனில் (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  நிறுத்து;

	// "s1", "s2", .. : subtract from the fold level
	நிலை 's': எனில் (flp->lvl >= 0)
		  {
		      எனில் (n > flp->lvl)
			  flp->lvl_next = 0;
		      இல்லையெனில்
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  நிறுத்து;

	// ">1", ">2", .. : start a fold with a certain level
	நிலை '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  நிறுத்து;

	// "<1", "<2", .. : end a fold with a certain level
	நிலை '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_next = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  நிறுத்து;

	// "=": No change in level
	நிலை '=': flp->lvl_next = flp->lvl;
		  நிறுத்து;

	// "-1", "0", "1", ..: set fold level
	இயல்பு:  எனில் (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_next = flp->lvl;
		  இல்லையெனில்
		      flp->lvl_next = n;
		  flp->lvl = n;
		  நிறுத்து;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    எனில் (flp->lvl < 0)
    {
	எனில் (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	எனில் (lnum == curbuf->b_ml.ml_வரி_மொத்தம்)
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
    நிலையான வெறுமை
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (முழு)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (முழு)STRLEN(foldendmarker);
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
    நிலையான வெறுமை
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    முழு		cstart;
    முழு		cend;
    முழு		start_lvl = flp->lvl;
    char_u	*s;
    முழு		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    போது (*s)
    {
	எனில் (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    எனில் (VIM_ISDIGIT(*s))
	    {
		n = atoi((எழுத்து *)s);
		எனில் (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    எனில் (n <= start_lvl)
			flp->start = 1;
		    இல்லையெனில்
			flp->start = n - start_lvl;
		}
	    }
	    இல்லையெனில்
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	இல்லையெனில் எனில் (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    எனில் (VIM_ISDIGIT(*s))
	    {
		n = atoi((எழுத்து *)s);
		எனில் (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    // never start a fold with an end marker
		    எனில் (flp->lvl_next > start_lvl)
			flp->lvl_next = start_lvl;
		}
	    }
	    இல்லையெனில்
		--flp->lvl_next;
	}
	இல்லையெனில்
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    எனில் (flp->lvl_next < 0)
	flp->lvl_next = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    நிலையான வெறுமை
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# இல்லையெனில்
    linenr_T	lnum = flp->lnum + flp->off;
    முழு		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    எனில் (lnum < flp->wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	எனில் (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# எனில் defined(FEAT_SESSION)
நிலையான முழு put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
நிலையான முழு put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
நிலையான முழு put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    முழு
put_folds(FILE *fd, win_T *wp)
{
    எனில் (foldmethodIsManual(wp))
    {
	எனில் (put_வரி(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_வரி(fd, "&fdl = &fdl") == FAIL)
	    திரும்பு FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    எனில் (wp->w_fold_manual)
	திரும்பு put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    திரும்பு OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    நிலையான முழு
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    முழு		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    க்கு (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	எனில் (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_உச்சி) == FAIL)
	    திரும்பு FAIL;
	எனில் (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_உச்சி + off,
					fp->fd_உச்சி + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    திரும்பு FAIL;
	++fp;
    }
    திரும்பு OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    நிலையான முழு
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    முழு		i;
    முழு		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    க்கு (i = 0; i < gap->ga_len; i++)
    {
	எனில் (fp->fd_flags != FD_LEVEL)
	{
	    எனில் (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		எனில் (fprintf(fd, ":%ld", fp->fd_உச்சி + off) < 0
			|| put_eol(fd) == FAIL
			|| put_வரி(fd, "sil! normal! zo") == FAIL)
		    திரும்பு FAIL;
		எனில் (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_உச்சி)
			== FAIL)
		    திரும்பு FAIL;
		// close the parent when needed
		எனில் (fp->fd_flags == FD_CLOSED)
		{
		    எனில் (put_fold_open_close(fd, fp, off) == FAIL)
			திரும்பு FAIL;
		}
	    }
	    இல்லையெனில்
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_உச்சி);
		எனில் ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		எனில் (put_fold_open_close(fd, fp, off) == FAIL)
		    திரும்பு FAIL;
	    }
	}
	++fp;
    }

    திரும்பு OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    நிலையான முழு
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    எனில் (fprintf(fd, ":%ld", fp->fd_உச்சி + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#எனில் defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    நிலையான வெறுமை
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    முழு		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_எண்(&argvars[1]), &tp);
	எனில் (wp != வெற்று && tp != வெற்று)
	{
	    எனில் (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	இல்லையெனில்
	{
	    rettv->vval.v_எண் = -2;
	    திரும்பு;
	}
    }
    இல்லையெனில்
	lnum = tv_get_lnum(argvars);

    எனில் (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
    {
	எனில் (hasFoldingWin(wp, lnum, &first, &last, FALSE, வெற்று))
	{
	    எனில் (end)
		rettv->vval.v_எண் = (varnumber_T)last;
	    இல்லையெனில்
		rettv->vval.v_எண் = (varnumber_T)first;
	    திரும்பு;
	}
    }
# endif
    rettv->vval.v_எண் = -1;
}

/*
 * "foldclosed()" function
 */
    வெறுமை
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    வெறுமை
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    நிலையான முழு
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    எனில் (invalid_உச்சி == (linenr_T)0)
	checkupdate(curwin);
    இல்லையெனில் எனில் (lnum == prev_lnum && prev_lnum_lvl >= 0)
	திரும்பு prev_lnum_lvl;
    இல்லையெனில் எனில் (lnum >= invalid_உச்சி && lnum <= invalid_bot)
	திரும்பு -1;

    // Return quickly when there is no folding at all in this window.
    எனில் (!hasAnyFolding(curwin))
	திரும்பு 0;

    திரும்பு foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    வெறுமை
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_எண்(&argvars[1]), &tp);
	எனில் (wp != வெற்று && tp != வெற்று)
	{
	    எனில் (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
		    rettv->vval.v_எண் = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	இல்லையெனில்
	    rettv->vval.v_எண் = -2;
    }
    இல்லையெனில்
    {
	lnum = tv_get_lnum(argvars);
	எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
	    rettv->vval.v_எண் = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    வெறுமை
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    முழு		len;
    எழுத்து	*txt;
    long	மொத்தம்;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    எனில் (foldstart > 0 && foldend <= curbuf->b_ml.ml_வரி_மொத்தம்
	    && dashes != வெற்று)
    {
	// Find first non-empty line in the fold.
	க்கு (lnum = foldstart; lnum < foldend; ++lnum)
	    எனில் (!linewhite(lnum))
		நிறுத்து;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	எனில் (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    எனில் (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		எனில் (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	மொத்தம் = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", மொத்தம்);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	எனில் (r != வெற்று)
	{
	    sprintf((எழுத்து *)r, txt, dashes, மொத்தம்);
	    len = (முழு)STRLEN(r);
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
    வெறுமை
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    முழு		fold_மொத்தம்;
    win_T	*wp  = curwin;
    நிலையான முழு	entered = FALSE;
    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (entered)
	திரும்பு; // reject recursive use
    entered = TRUE;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_எண்(&argvars[1]), &tp);
	எனில் (wp == வெற்று || tp == வெற்று) // wrong winid
	{
	    entered = FALSE;
	    திரும்பு;
	}
	எனில் (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    இல்லையெனில்
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    எனில் (lnum < 0)
	lnum = 0;
    fold_மொத்தம் = folded_மொத்தம்(wp, lnum, &foldinfo);
    எனில் (fold_மொத்தம் > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_மொத்தம் - 1,
							       &foldinfo, buf);
	எனில் (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
