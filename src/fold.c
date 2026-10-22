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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    linenr_T	fd_ટોચ;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    અક્ષર	fd_flags;	// see below
    અક્ષર	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
સ્થિર શૂન્ય newFoldLevelWin(win_T *wp);
સ્થિર પૂર્ણાંક checkCloseRec(garray_T *gap, linenr_T lnum, પૂર્ણાંક level);
સ્થિર પૂર્ણાંક foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
સ્થિર પૂર્ણાંક foldLevelWin(win_T *wp, linenr_T lnum);
સ્થિર શૂન્ય checkupdate(win_T *wp);
સ્થિર શૂન્ય setFoldRepeat(linenr_T lnum, long ગણતરી, પૂર્ણાંક do_open);
સ્થિર linenr_T setManualFold(linenr_T lnum, પૂર્ણાંક opening, પૂર્ણાંક recurse, પૂર્ણાંક *donep);
સ્થિર linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, પૂર્ણાંક opening, પૂર્ણાંક recurse, પૂર્ણાંક *donep);
સ્થિર શૂન્ય foldOpenNested(fold_T *fpr);
સ્થિર શૂન્ય deleteFoldEntry(garray_T *gap, પૂર્ણાંક idx, પૂર્ણાંક recursive);
સ્થિર શૂન્ય foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
સ્થિર પૂર્ણાંક getDeepestNestingRecurse(garray_T *gap);
સ્થિર પૂર્ણાંક check_closed(win_T *win, fold_T *fp, પૂર્ણાંક *use_levelp, પૂર્ણાંક level, પૂર્ણાંક *maybe_smallp, linenr_T lnum_off);
સ્થિર શૂન્ય checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
સ્થિર શૂન્ય setSmallMaybe(garray_T *gap);
સ્થિર શૂન્ય foldCreateMarkers(linenr_T start, linenr_T end);
સ્થિર શૂન્ય foldAddMarker(linenr_T lnum, char_u *marker, પૂર્ણાંક markerlen);
સ્થિર શૂન્ય deleteFoldMarkers(fold_T *fp, પૂર્ણાંક recursive, linenr_T lnum_off);
સ્થિર શૂન્ય foldDelMarker(linenr_T lnum, char_u *marker, પૂર્ણાંક markerlen);
સ્થિર શૂન્ય foldUpdateIEMS(win_T *wp, linenr_T ટોચ, linenr_T bot);
સ્થિર શૂન્ય parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
સ્થિર linenr_T invalid_ટોચ = (linenr_T)0;
સ્થિર linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
સ્થિર linenr_T prev_lnum = 0;
સ્થિર પૂર્ણાંક prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

સ્થિર પૂર્ણાંક foldstartmarkerlen;
સ્થિર char_u *foldendmarker;
સ્થિર પૂર્ણાંક foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    શૂન્ય
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
    પૂર્ણાંક
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    પાછા (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    પૂર્ણાંક
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    પાછા hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ખાલી);
}

// hasFoldingWin() {{{2
    પૂર્ણાંક
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    પૂર્ણાંક		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    પૂર્ણાંક		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    પૂર્ણાંક		x;
    fold_T	*fp;
    પૂર્ણાંક		level = 0;
    પૂર્ણાંક		use_level = FALSE;
    પૂર્ણાંક		maybe_small = FALSE;
    garray_T	*gap;
    પૂર્ણાંક		નીચલું_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    જો (!hasAnyFolding(win))
    {
	જો (infop != ખાલી)
	    infop->fi_level = 0;
	પાછા FALSE;
    }

    જો (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	જો (x >= 0)
	{
	    first = win->w_લીટીઓ[x].wl_lnum;
	    last = win->w_લીટીઓ[x].wl_lastlnum;
	    had_folded = win->w_લીટીઓ[x].wl_folded;
	}
    }

    જો (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	માટે (;;)
	{
	    જો (!foldFind(gap, lnum_rel, &fp))
		તોડો;

	    // Remember lowest level of fold that starts in "lnum".
	    જો (lnum_rel == fp->fd_ટોચ && નીચલું_level == 0)
		નીચલું_level = level + 1;

	    first += fp->fd_ટોચ;
	    last += fp->fd_ટોચ;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    જો (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		તોડો;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_ટોચ;
	    ++level;
	}
    }

    જો (!had_folded)
    {
	જો (infop != ખાલી)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_નીચલું_level = નીચલું_level == 0 ? level : નીચલું_level;
	}
	પાછા FALSE;
    }

    જો (last > win->w_buffer->b_ml.ml_લીટી_ગણતરી)
	last = win->w_buffer->b_ml.ml_લીટી_ગણતરી;
    જો (lastp != ખાલી)
	*lastp = last;
    જો (firstp != ખાલી)
	*firstp = first;
    જો (infop != ખાલી)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_નીચલું_level = નીચલું_level == 0 ? level + 1 : નીચલું_level;
    }
    પાછા TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    પૂર્ણાંક
લીટી_Folded(win_T *win, linenr_T lnum)
{
    પાછા folded_ગણતરી(win, lnum, ખાલી) != 0;
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
folded_ગણતરી(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    જો (hasFoldingWin(win, lnum, ખાલી, &last, FALSE, infop))
	પાછા (long)(last - lnum + 1);
    પાછા 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    પૂર્ણાંક
foldmethodIsManual(win_T *wp)
{
    પાછા (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    પૂર્ણાંક
foldmethodIsIndent(win_T *wp)
{
    પાછા (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    પૂર્ણાંક
foldmethod_Is_પદાવલિ(win_T *wp)
{
    પાછા (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    પૂર્ણાંક
foldmethodIsMarker(win_T *wp)
{
    પાછા (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    પૂર્ણાંક
foldmethodIsSyntax(win_T *wp)
{
    પાછા (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    પૂર્ણાંક
foldmethodIsDiff(win_T *wp)
{
    પાછા (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    શૂન્ય
closeFold(linenr_T lnum, long ગણતરી)
{
    setFoldRepeat(lnum, ગણતરી, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    શૂન્ય
closeFoldRecurse(linenr_T lnum)
{
    (શૂન્ય)setManualFold(lnum, FALSE, TRUE, ખાલી);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    શૂન્ય
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    પૂર્ણાંક		opening,	// TRUE to open, FALSE to close
    પૂર્ણાંક		recurse,	// TRUE to do it recursively
    પૂર્ણાંક		had_visual)	// TRUE when Visual selection used
{
    પૂર્ણાંક		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_next;

    માટે (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	જો (opening && !recurse)
	    (શૂન્ય)hasFolding(lnum, ખાલી, &lnum_next);
	(શૂન્ય)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	જો (!opening && !recurse)
	    (શૂન્ય)hasFolding(lnum, ખાલી, &lnum_next);
    }
    જો (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    જો (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    શૂન્ય
openFold(linenr_T lnum, long ગણતરી)
{
    setFoldRepeat(lnum, ગણતરી, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    શૂન્ય
openFoldRecurse(linenr_T lnum)
{
    (શૂન્ય)setManualFold(lnum, TRUE, TRUE, ખાલી);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    શૂન્ય
foldOpenCursor(શૂન્ય)
{
    પૂર્ણાંક		done;

    checkupdate(curwin);
    જો (hasAnyFolding(curwin))
	માટે (;;)
	{
	    done = DONE_NOTHING;
	    (શૂન્ય)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    જો (!(done & DONE_ACTION))
		તોડો;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    શૂન્ય
newFoldLevel(શૂન્ય)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    જો (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    જો (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    સ્થિર શૂન્ય
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    પૂર્ણાંક		i;

    checkupdate(wp);
    જો (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_data;
	માટે (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    શૂન્ય
foldCheckClose(શૂન્ય)
{
    જો (*p_fcl == NUL)
	પાછા;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    જો (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(પૂર્ણાંક)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    સ્થિર પૂર્ણાંક
checkCloseRec(garray_T *gap, linenr_T lnum, પૂર્ણાંક level)
{
    fold_T	*fp;
    પૂર્ણાંક		retval = FALSE;
    પૂર્ણાંક		i;

    fp = (fold_T *)gap->ga_data;
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	જો (fp[i].fd_flags == FD_OPEN)
	{
	    જો (level <= 0 && (lnum < fp[i].fd_ટોચ
				      || lnum >= fp[i].fd_ટોચ + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    નહીંતર
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_ટોચ,
								   level - 1);
	}
    }
    પાછા retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    પૂર્ણાંક
foldManualAllowed(પૂર્ણાંક create)
{
    જો (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	પાછા TRUE;
    જો (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    નહીંતર
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    પાછા FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    શૂન્ય
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		cont;
    પૂર્ણાંક		use_level = FALSE;
    પૂર્ણાંક		closed = FALSE;
    પૂર્ણાંક		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    જો (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    જો (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	પાછા;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    જો (gap->ga_len == 0)
	i = 0;
    નહીંતર
    {
	માટે (;;)
	{
	    જો (!foldFind(gap, start_rel, &fp))
		તોડો;
	    જો (fp->fd_ટોચ + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_ટોચ;
		end_rel -= fp->fd_ટોચ;
		જો (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    જો (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		નહીંતર જો (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    નહીંતર
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		તોડો;
	    }
	}
	જો (gap->ga_len == 0)
	    i = 0;
	નહીંતર
	    i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data);
    }

    જો (ga_grow(gap, 1) == FAIL)
	પાછા;

    fp = (fold_T *)gap->ga_data + i;
    ga_init2(&fold_ga, કદ(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    માટે (cont = 0; i + cont < gap->ga_len; ++cont)
	જો (fp[cont].fd_ટોચ > end_rel)
	    તોડો;
    જો (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	જો (start_rel > fp->fd_ટોચ)
	    start_rel = fp->fd_ટોચ;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	જો (end_rel < fp[cont - 1].fd_ટોચ + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_ટોચ + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_data, fp, કદ(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	માટે (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_data)[j].fd_ટોચ -= start_rel;
    }
    // Move remaining entries to after the new fold.
    જો (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_data + i,
		કદ(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_ટોચ = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    જો (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    જો (!use_level)
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
    શૂન્ય
deleteFold(
    linenr_T	start,
    linenr_T	end,
    પૂર્ણાંક		recursive,
    પૂર્ણાંક		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = ખાલી;
    linenr_T	found_off = 0;
    પૂર્ણાંક		use_level;
    પૂર્ણાંક		maybe_small = FALSE;
    પૂર્ણાંક		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    પૂર્ણાંક		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    જ્યારે (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = ખાલી;
	lnum_off = 0;
	use_level = FALSE;
	માટે (;;)
	{
	    જો (!foldFind(gap, lnum - lnum_off, &fp))
		તોડો;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    જો (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		તોડો;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ટોચ;
	    ++level;
	}
	જો (found_ga == ખાલી)
	{
	    ++lnum;
	}
	નહીંતર
	{
	    lnum = found_fp->fd_ટોચ + found_fp->fd_len + found_off;

	    જો (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (પૂર્ણાંક)(found_fp - (fold_T *)found_ga->ga_data), recursive);
	    નહીંતર
	    {
		જો (first_lnum > found_fp->fd_ટોચ + found_off)
		    first_lnum = found_fp->fd_ટોચ + found_off;
		જો (last_lnum < lnum)
		    last_lnum = lnum;
		જો (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    જો (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	જો (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    નહીંતર
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    જો (last_lnum > 0)
	changed_લીટીઓ(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    શૂન્ય
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
    શૂન્ય
foldUpdate(win_T *wp, linenr_T ટોચ, linenr_T bot)
{
    fold_T	*fp;

    જો (disable_fold_update > 0)
	પાછા;
# ifdef FEAT_DIFF
    જો (need_diff_redraw)
	// will update later
	પાછા;
# endif

    જો (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = ટોચ;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	જો (ટોચ > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = ટોચ;
	}
	(શૂન્ય)foldFind(&wp->w_folds, maybe_small_start, &fp);
	જ્યારે (fp < (fold_T *)wp->w_folds.ga_data + wp->w_folds.ga_len
		&& fp->fd_ટોચ <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    જો (foldmethodIsIndent(wp)
	    || foldmethod_Is_પદાવલિ(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	પૂર્ણાંક save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, ટોચ, bot);
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
    શૂન્ય
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
    પૂર્ણાંક
foldMoveTo(
    પૂર્ણાંક		updown,
    પૂર્ણાંક		dir,	    // FORWARD or BACKWARD
    long	ગણતરી)
{
    long	n;
    પૂર્ણાંક		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    પૂર્ણાંક		use_level;
    પૂર્ણાંક		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    પૂર્ણાંક		level;
    પૂર્ણાંક		last;

    checkupdate(curwin);

    // Repeat "count" times.
    માટે (n = 0; n < ગણતરી; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	જો (gap->ga_len == 0)
	    તોડો;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	માટે (;;)
	{
	    જો (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		જો (!updown || gap->ga_len == 0)
		    તોડો;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		જો (dir == FORWARD)
		{
		    જો (fp - (fold_T *)gap->ga_data >= gap->ga_len)
			તોડો;
		    --fp;
		}
		નહીંતર
		{
		    જો (fp == (fold_T *)gap->ga_data)
			તોડો;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    જો (!last)
	    {
		// Check if this fold is closed.
		જો (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		જો (last && !updown)
		    તોડો;
	    }

	    જો (updown)
	    {
		જો (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    જો (fp + 1 - (fold_T *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_ટોચ + lnum_off;
			જો (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		નહીંતર
		{
		    // to end of previous fold if there is one
		    જો (fp > (fold_T *)gap->ga_data)
		    {
			lnum = fp[-1].fd_ટોચ + lnum_off + fp[-1].fd_len - 1;
			જો (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    નહીંતર
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		જો (dir == FORWARD)
		{
		    lnum = fp->fd_ટોચ + lnum_off + fp->fd_len - 1;
		    જો (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		નહીંતર
		{
		    lnum = fp->fd_ટોચ + lnum_off;
		    જો (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    જો (last)
		તોડો;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ટોચ;
	    ++level;
	}
	જો (lnum_found != curwin->w_cursor.lnum)
	{
	    જો (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	નહીંતર
	    તોડો;
    }

    પાછા retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    શૂન્ય
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, કદ(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    પૂર્ણાંક
find_wl_entry(win_T *win, linenr_T lnum)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < win->w_લીટીઓ_valid; ++i)
	જો (win->w_લીટીઓ[i].wl_valid)
	{
	    જો (lnum < win->w_લીટીઓ[i].wl_lnum)
		પાછા -1;
	    જો (lnum <= win->w_લીટીઓ[i].wl_lastlnum)
		પાછા i;
	}
    પાછા -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    શૂન્ય
foldAdjustVisual(શૂન્ય)
{
    pos_T	*start, *end;

    જો (!VIsual_active || !hasAnyFolding(curwin))
	પાછા;

    જો (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    નહીંતર
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    જો (hasFolding(start->lnum, &start->lnum, ખાલી))
	start->col = 0;

    જો (!hasFolding(end->lnum, ખાલી, &end->lnum))
	પાછા;

    end->col = ml_get_len(end->lnum);
    જો (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    જો (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    શૂન્ય
foldAdjustCursor(શૂન્ય)
{
    (શૂન્ય)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ખાલી);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    શૂન્ય
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    પૂર્ણાંક		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    જો (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	પાછા;

    from_p = (fold_T *)from->ga_data;
    to_p = (fold_T *)to->ga_data;

    માટે (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_ટોચ = from_p->fd_ટોચ;
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
    સ્થિર પૂર્ણાંક
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	નીચલું, ઉપલું;
    fold_T	*fp;
    પૂર્ણાંક		i;

    જો (gap->ga_len == 0)
    {
	*fpp = ખાલી;
	પાછા FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_data;
    નીચલું = 0;
    ઉપલું = gap->ga_len - 1;
    જ્યારે (નીચલું <= ઉપલું)
    {
	i = (નીચલું + ઉપલું) / 2;
	જો (fp[i].fd_ટોચ > lnum)
	    // fold below lnum, adjust high
	    ઉપલું = i - 1;
	નહીંતર જો (fp[i].fd_ટોચ + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    નીચલું = i + 1;
	નહીંતર
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    પાછા TRUE;
	}
    }
    *fpp = fp + નીચલું;
    પાછા FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    સ્થિર પૂર્ણાંક
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    પૂર્ણાંક		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    માટે (;;)
    {
	જો (!foldFind(gap, lnum_rel, &fp))
	    તોડો;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_ટોચ;
	++level;
    }

    પાછા level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    સ્થિર શૂન્ય
checkupdate(win_T *wp)
{
    જો (!wp->w_foldinvalid)
	પાછા;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    સ્થિર શૂન્ય
setFoldRepeat(linenr_T lnum, long ગણતરી, પૂર્ણાંક do_open)
{
    પૂર્ણાંક		done;
    long	n;

    માટે (n = 0; n < ગણતરી; ++n)
    {
	done = DONE_NOTHING;
	(શૂન્ય)setManualFold(lnum, do_open, FALSE, &done);
	જો (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    જો (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    તોડો;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    સ્થિર linenr_T
setManualFold(
    linenr_T	lnum,
    પૂર્ણાંક		opening,    // TRUE when opening, FALSE when closing
    પૂર્ણાંક		recurse,    // TRUE when closing/opening recursive
    પૂર્ણાંક		*donep)
{
# ifdef FEAT_DIFF
    જો (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    જો (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		જો (dlnum != 0)
		    (શૂન્ય)setManualFoldWin(wp, dlnum, opening, recurse, ખાલી);
	    }
	}
    }
# endif

    પાછા setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    સ્થિર linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    પૂર્ણાંક		opening,    // TRUE when opening, FALSE when closing
    પૂર્ણાંક		recurse,    // TRUE when closing/opening recursive
    પૂર્ણાંક		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = ખાલી;
    પૂર્ણાંક		j;
    પૂર્ણાંક		level = 0;
    પૂર્ણાંક		use_level = FALSE;
    પૂર્ણાંક		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	next = MAXLNUM;
    linenr_T	off = 0;
    પૂર્ણાંક		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    માટે (;;)
    {
	જો (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    જો (fp != ખાલી && fp < (fold_T *)gap->ga_data + gap->ga_len)
		next = fp->fd_ટોચ + off;
	    તોડો;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	જો (fp + 1 < (fold_T *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_ટોચ + off;

	// Change from level-dependent folding to manual.
	જો (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    જો (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    નહીંતર
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_data;
	    માટે (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	જો (!opening && recurse)
	{
	    જો (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	નહીંતર જો (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    જો (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		જો (recurse)
		    foldOpenNested(fp);
	    }
	    તોડો;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_ટોચ;
	off += fp->fd_ટોચ;
	++level;
    }
    જો (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	જો (!opening && found != ખાલી)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	જો (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    નહીંતર જો (donep == ખાલી && wp == curwin)
	emsg(_(e_no_fold_found));

    જો (donep != ખાલી)
	*donep |= done;

    પાછા next;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    સ્થિર શૂન્ય
foldOpenNested(fold_T *fpr)
{
    પૂર્ણાંક		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_data;
    માટે (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    સ્થિર શૂન્ય
deleteFoldEntry(garray_T *gap, પૂર્ણાંક idx, પૂર્ણાંક recursive)
{
    fold_T	*fp;
    પૂર્ણાંક		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_data + idx;
    જો (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	જો (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, કદ(fold_T) * (gap->ga_len - idx));
    }
    નહીંતર
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	જો (ga_grow(gap, (પૂર્ણાંક)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_data + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_data;
	    માટે (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_ટોચ += fp->fd_ટોચ;
		જો (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		જો (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    જો (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  કદ(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(કદ(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    શૂન્ય
deleteFoldRecurse(garray_T *gap)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < gap->ga_len; ++i)
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
    શૂન્ય
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    જો (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    જો (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    જો ((State & MODE_ઉમેરો) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    સ્થિર શૂન્ય
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    પૂર્ણાંક		i;
    linenr_T	last;
    linenr_T	ટોચ;

    જો (gap->ga_len == 0)
	પાછા;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    જો ((State & MODE_ઉમેરો) && amount == (linenr_T)1 && line2 == MAXLNUM)
	ટોચ = line1 + 1;
    નહીંતર
	ટોચ = line1;

    // Find the fold containing or just below "line1".
    (શૂન્ય)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    માટે (i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_ટોચ + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	જો (last < line1)
	    ચાલુ;

	// 6. fold below line2: only adjust for amount_after
	જો (fp->fd_ટોચ > line2)
	{
	    જો (amount_after == 0)
		તોડો;
	    fp->fd_ટોચ += amount_after;
	}
	નહીંતર
	{
	    જો (fp->fd_ટોચ >= ટોચ && last <= line2)
	    {
		// 4. fold completely contained in range
		જો (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		નહીંતર
		    fp->fd_ટોચ += amount;
	    }
	    નહીંતર
	    {
		જો (fp->fd_ટોચ < ટોચ)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ટોચ,
				  line2 - fp->fd_ટોચ, amount, amount_after);
		    જો (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			જો (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_ટોચ;
			નહીંતર
			    fp->fd_len += amount;
		    }
		    નહીંતર
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		નહીંતર
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    જો (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ટોચ,
				  amount,
				  amount_after + (fp->fd_ટોચ - ટોચ));
			fp->fd_len -= line2 - fp->fd_ટોચ + 1;
			fp->fd_ટોચ = line1;
		    }
		    નહીંતર
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ટોચ,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_ટોચ += amount;
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
    પૂર્ણાંક
getDeepestNesting(શૂન્ય)
{
    checkupdate(curwin);
    પાછા getDeepestNestingRecurse(&curwin->w_folds);
}

    સ્થિર પૂર્ણાંક
getDeepestNestingRecurse(garray_T *gap)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		level;
    પૂર્ણાંક		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	જો (level > maxlevel)
	    maxlevel = level;
    }

    પાછા maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    સ્થિર પૂર્ણાંક
check_closed(
    win_T	*win,
    fold_T	*fp,
    પૂર્ણાંક		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    પૂર્ણાંક		level,		    // folding depth
    પૂર્ણાંક		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    પૂર્ણાંક		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    જો (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	જો (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    નહીંતર જો (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    જો (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    જો (closed)
    {
	જો (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	જો (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    પાછા closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    સ્થિર શૂન્ય
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		n;

    જો (fp->fd_small != MAYBE)
	પાછા;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    જો (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    નહીંતર
    {
	ગણતરી = 0;
	માટે (n = 0; n < fp->fd_len; ++n)
	{
	    ગણતરી += plines_win_nofold(wp, fp->fd_ટોચ + lnum_off + n);
	    જો (ગણતરી > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		પાછા;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    સ્થિર શૂન્ય
setSmallMaybe(garray_T *gap)
{
    પૂર્ણાંક		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    માટે (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    સ્થિર શૂન્ય
foldCreateMarkers(linenr_T start, linenr_T end)
{
    જો (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	પાછા;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_લીટીઓ(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    સ્થિર શૂન્ય
foldAddMarker(linenr_T lnum, char_u *marker, પૂર્ણાંક markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*લીટી;
    પૂર્ણાંક		લીટી_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((અક્ષર *)curbuf->b_p_cms, "%s");
    પૂર્ણાંક		લીટી_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    લીટી = ml_get(lnum);
    લીટી_len = ml_get_len(lnum);

    જો (u_save(lnum - 1, lnum + 1) != OK)
	પાછા;

    // Check if the line ends with an unclosed comment
    (શૂન્ય)skip_comment(લીટી, FALSE, FALSE, &લીટી_is_comment);
    newline = alloc(લીટી_len + markerlen + STRLEN(cms) + 1);
    જો (newline == ખાલી)
	પાછા;
    STRCPY(newline, લીટી);
    // Append the marker to the end of the line
    જો (p == ખાલી || લીટી_is_comment)
	vim_strncpy(newline + લીટી_len, marker, markerlen);
    નહીંતર
    {
	STRCPY(newline + લીટી_len, cms);
	STRNCPY(newline + લીટી_len + (p - cms), marker, markerlen);
	STRCPY(newline + લીટી_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    સ્થિર શૂન્ય
deleteFoldMarkers(
    fold_T	*fp,
    પૂર્ણાંક		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    પૂર્ણાંક		i;

    જો (recursive)
	માટે (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_ટોચ);
    foldDelMarker(fp->fd_ટોચ + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_ટોચ + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    સ્થિર શૂન્ય
foldDelMarker(linenr_T lnum, char_u *marker, પૂર્ણાંક markerlen)
{
    char_u	*લીટી;
    char_u	*newline;
    char_u	*p;
    પૂર્ણાંક		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	પાછા;
    લીટી = ml_get(lnum);
    માટે (p = લીટી; *p != NUL; ++p)
	જો (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    જો (VIM_ISDIGIT(p[len]))
		++len;
	    જો (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((અક્ષર *)cms, "%s");
		જો (cms2 != ખાલી && p - લીટી >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (પૂર્ણાંક)STRLEN(cms) - 2;
		}
	    }
	    જો (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		જો (newline != ખાલી)
		{
		    STRNCPY(newline, લીટી, p - લીટી);
		    STRCPY(newline + (p - લીટી), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    તોડો;
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
    char_u	*text = ખાલી;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    સ્થિર પૂર્ણાંક	    got_fdt_error = FALSE;
    પૂર્ણાંક		    save_did_emsg = did_emsg;
    સ્થિર win_T    *last_wp = ખાલી;
    સ્થિર linenr_T last_lnum = 0;

    જો (last_wp != wp || last_wp == ખાલી
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    જો (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    જો (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	પૂર્ણાંક	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	જો (level > (પૂર્ણાંક)કદ(dashes) - 1)
	    level = (પૂર્ણાંક)કદ(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	જો (!got_fdt_error)
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

	    જો (text == ખાલી || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, ખાલી, -1);

	જો (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	જો (text != ખાલી)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    માટે (p = text; *p != NUL; ++p)
	    {
		પૂર્ણાંક	len;

		જો (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    જો (!vim_isprintc((*mb_ptr2char)(p)))
			તોડો;
		    p += len - 1;
		}
		નહીંતર
		    જો (*p == TAB)
			*p = ' ';
		    નહીંતર જો (ptr2cells(p) > 1)
			તોડો;
	    }
	    જો (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    જો (text == ખાલી)
# endif
    {
	long ગણતરી = (long)(lnume - lnum + 1);

	vim_snprintf((અક્ષર *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", ગણતરી),
		     ગણતરી);
	text = buf;
    }
    પાછા text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    સ્થિર શૂન્ય
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    પૂર્ણાંક		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    પૂર્ણાંક		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    પૂર્ણાંક		len;
    પૂર્ણાંક		did1 = FALSE;
    પૂર્ણાંક		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (પૂર્ણાંક)STRLEN(cms_start);
    જ્યારે (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((અક્ષર *)cms_start, "%s");
    જો (cms_end != ખાલી)
    {
	cms_elen = cms_slen - (પૂર્ણાંક)(cms_end - cms_start);
	cms_slen = (પૂર્ણાંક)(cms_end - cms_start);

	// exclude white space before "%s"
	જ્યારે (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (પૂર્ણાંક)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    માટે (s = str; *s != NUL; )
    {
	len = 0;
	જો (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	નહીંતર જો (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	જો (len > 0)
	{
	    જો (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    માટે (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    જો (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (પૂર્ણાંક)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	નહીંતર જો (cms_end != ખાલી)
	{
	    જો (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    નહીંતર જો (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	જો (len != 0)
	{
	    જ્યારે (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	નહીંતર
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
પ્રકારવ્યાખ્યા સંરચના
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    પૂર્ણાંક		lvl;		// current level (-1 for undefined)
    પૂર્ણાંક		lvl_next;	// level used for next line
    પૂર્ણાંક		start;		// number of folds that are forced to start at
				// this line.
    પૂર્ણાંક		end;		// level of fold that is forced to end below
				// this line
    પૂર્ણાંક		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
સ્થિર પૂર્ણાંક fold_changed;

// Function declarations. {{{2
સ્થિર linenr_T foldUpdateIEMSRecurse(garray_T *gap, પૂર્ણાંક level, linenr_T startlnum, fline_T *flp, શૂન્ય (*getlevel)(fline_T *), linenr_T bot, પૂર્ણાંક topflags);
સ્થિર પૂર્ણાંક fold_ઉમેરો(garray_T *gap, પૂર્ણાંક i);
સ્થિર શૂન્ય foldSplit(garray_T *gap, પૂર્ણાંક i, linenr_T ટોચ, linenr_T bot);
સ્થિર શૂન્ય foldRemove(garray_T *gap, linenr_T ટોચ, linenr_T bot);
સ્થિર શૂન્ય foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
સ્થિર શૂન્ય foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
સ્થિર શૂન્ય foldlevelDiff(fline_T *flp);
# endif
સ્થિર શૂન્ય foldlevel_પદાવલિ(fline_T *flp);
સ્થિર શૂન્ય foldlevelMarker(fline_T *flp);
સ્થિર શૂન્ય foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    સ્થિર શૂન્ય
foldUpdateIEMS(win_T *wp, linenr_T ટોચ, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    શૂન્ય	(*getlevel)(fline_T *);
    પૂર્ણાંક		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    જો (invalid_ટોચ != (linenr_T)0)
	પાછા;

    જો (wp->w_foldinvalid)
    {
	// Need to update all folds.
	ટોચ = 1;
	bot = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    જો (foldmethodIsDiff(wp))
    {
	જો (ટોચ > diff_context)
	    ટોચ -= diff_context;
	નહીંતર
	    ટોચ = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    જો (ટોચ > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	ટોચ = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_ટોચ = ટોચ;
    invalid_bot = bot;

    જો (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	જો (ટોચ > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, ટોચ - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = ટોચ - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    જો (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_next);
	    નહીંતર
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = ટોચ;
	getlevel(&fline);
    }
    નહીંતર
    {
	fline.lnum = ટોચ;
	જો (foldmethod_Is_પદાવલિ(wp))
	{
	    getlevel = foldlevel_પદાવલિ;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    જો (ટોચ > 1)
		--fline.lnum;
	}
	નહીંતર જો (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	નહીંતર જો (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	નહીંતર
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    જો (ટોચ > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	માટે ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    જો (fline.lvl >= 0)
		તોડો;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    જો (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ખાલી;
	પૂર્ણાંક	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	જ્યારે (current_fdl < fline.lvl)
	{
	    જો (!foldFind(gap, lnum_rel, &fpn))
		તોડો;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_ટોચ;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_ટોચ;
	}
	જો (fpn != ખાલી && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    જો (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    જો (start > end && end < wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	end = start;
    જ્યારે (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	જો (fline.lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    તોડો;
	જો (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    જો (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_પદાવલિ)
		તોડો;
	    જો ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_ટોચ + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_ટોચ < fline.lnum))
		end = fp->fd_ટોચ + fp->fd_len - 1;
	    નહીંતર જો (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    નહીંતર
		તોડો;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	જો (fline.lvl > 0)
	{
	    invalid_ટોચ = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	નહીંતર
	{
	    જો (fline.lnum == wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
		તોડો;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    જો (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    જો (end != bot)
	redraw_win_range_later(wp, ટોચ, end);

    invalid_ટોચ = (linenr_T)0;
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
    સ્થિર linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    પૂર્ણાંક		level,
    linenr_T	startlnum,
    fline_T	*flp,
    શૂન્ય	(*getlevel)(fline_T *),
    linenr_T	bot,
    પૂર્ણાંક		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ખાલી;
    fold_T	*fp2;
    પૂર્ણાંક		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    પૂર્ણાંક		i;
    પૂર્ણાંક		finish = FALSE;
    linenr_T	linecount = flp->wp->w_buffer->b_ml.ml_લીટી_ગણતરી - flp->off;
    પૂર્ણાંક		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    જો (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(શૂન્ય)foldFind(gap, startlnum - 1, &fp);
	જો (fp != ખાલી && (fp >= ((fold_T *)gap->ga_data) + gap->ga_len
						   || fp->fd_ટોચ >= startlnum))
	    fp = ખાલી;
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
    જ્યારે (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	લીટી_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	જો (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	જો (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	જો (flp->lnum > bot && !finish && fp != ખાલી)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    જો (getlevel != foldlevelMarker
		    && getlevel != foldlevel_પદાવલિ
		    && getlevel != foldlevelSyntax)
		તોડો;
	    i = 0;
	    fp2 = fp;
	    જો (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_ટોચ;
		જ્યારે (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_ટોચ;
		}
	    }
	    જો (lvl < level + i)
	    {
		(શૂન્ય)foldFind(&fp->fd_nested, flp->lnum - fp->fd_ટોચ, &fp2);
		જો (fp2 != ખાલી)
		    bot = fp2->fd_ટોચ + fp2->fd_len - 1 + fp->fd_ટોચ;
	    }
	    નહીંતર જો (fp->fd_ટોચ + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    નહીંતર
		તોડો;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	જો (fp == ખાલી
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
	    જ્યારે (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		જો (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		નહીંતર
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		જો (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && fp->fd_ટોચ <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_ટોચ < flp->lnum)
				|| (lvl >= level
					   && fp->fd_ટોચ <= flp->lnum_save)))))
		{
		    જો (fp->fd_ટોચ + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			જો (fp->fd_ટોચ == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			નહીંતર જો (fp->fd_ટોચ >= startlnum)
			{
			    જો (fp->fd_ટોચ > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_ટોચ - firstlnum), 0L);
			    નહીંતર
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_ટોચ - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_ટોચ - firstlnum));
			    fp->fd_len += fp->fd_ટોચ - firstlnum;
			    fp->fd_ટોચ = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			નહીંતર જો ((flp->start != 0 && lvl == level)
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
			    જો (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    નહીંતર
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_ટોચ,
						      breakend - fp->fd_ટોચ);
			    i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_data + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    જો (getlevel == foldlevelMarker
				    || getlevel == foldlevel_પદાવલિ
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			જો (fp->fd_ટોચ == startlnum && concat)
			{
			    i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data);
			    જો (i != 0)
			    {
				fp2 = fp - 1;
				જો (fp2->fd_ટોચ + fp2->fd_len == fp->fd_ટોચ)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			તોડો;
		    }
		    જો (fp->fd_ટોચ >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data), TRUE);
		    }
		    નહીંતર
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_ટોચ;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		નહીંતર
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    જો (gap->ga_len == 0)
			i = 0;
		    નહીંતર
			i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data);
		    જો (fold_ઉમેરો(gap, i) != OK)
			પાછા bot;
		    fp = (fold_T *)gap->ga_data + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_ટોચ = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    જો (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    નહીંતર જો (i <= 0)
		    {
			fp->fd_flags = topflags;
			જો (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    નહીંતર
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    જો (getlevel == foldlevelMarker
			    || getlevel == foldlevel_પદાવલિ
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    તોડો;
		}
	    }
	}

	જો (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    તોડો;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	જો (lvl > level && fp != ખાલી)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    જો (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_ટોચ;
	    flp->off += fp->fd_ટોચ;
	    i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_ટોચ, flp, getlevel,
					      bot - fp->fd_ટોચ, fp->fd_flags);
	    fp = (fold_T *)gap->ga_data + i;
	    flp->lnum += fp->fd_ટોચ;
	    flp->lnum_save += fp->fd_ટોચ;
	    flp->off -= fp->fd_ટોચ;
	    bot += fp->fd_ટોચ;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	નહીંતર
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    જ્યારે (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		જો (++flp->lnum > linecount)
		    તોડો;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		જો (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    તોડો;
	    }
	    prev_lnum = 0;
	    જો (flp->lnum > linecount)
		તોડો;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    જો (fp == ખાલી)	// only happens when got_int is set
	પાછા bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    જો (fp->fd_len < flp->lnum - fp->fd_ટોચ)
    {
	fp->fd_len = flp->lnum - fp->fd_ટોચ;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    નહીંતર જો (fp->fd_ટોચ + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_ટોચ + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_ટોચ,
						  flp->lnum - 1 - fp->fd_ટોચ);

    જો (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	જો (fp->fd_len != flp->lnum - fp->fd_ટોચ)
	{
	    જો (fp->fd_ટોચ + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		જો (getlevel == foldlevelMarker
			|| getlevel == foldlevel_પદાવલિ
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_ટોચ + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_ટોચ;
		}
		નહીંતર
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_data + i;
		}
	    }
	    નહીંતર
		fp->fd_len = flp->lnum - fp->fd_ટોચ;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    માટે (;;)
    {
	fp2 = fp + 1;
	જો (fp2 >= (fold_T *)gap->ga_data + gap->ga_len
						  || fp2->fd_ટોચ > flp->lnum)
	    તોડો;
	જો (fp2->fd_ટોચ + fp2->fd_len > flp->lnum)
	{
	    જો (fp2->fd_ટોચ < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_ટોચ - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_ટોચ - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_ટોચ;
		fp2->fd_ટોચ = flp->lnum;
		fold_changed = TRUE;
	    }

	    જો (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    તોડો;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (પૂર્ણાંક)(fp2 - (fold_T *)gap->ga_data), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    જો (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    પાછા bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    સ્થિર પૂર્ણાંક
fold_ઉમેરો(garray_T *gap, પૂર્ણાંક i)
{
    fold_T	*fp;

    જો (ga_grow(gap, 1) == FAIL)
	પાછા FAIL;
    fp = (fold_T *)gap->ga_data + i;
    જો (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, કદ(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, કદ(fold_T), 10);
    પાછા OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    સ્થિર શૂન્ય
foldSplit(
    garray_T	*gap,
    પૂર્ણાંક		i,
    linenr_T	ટોચ,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		len;

    // The fold continues below bot, need to split it.
    જો (fold_ઉમેરો(gap, i + 1) == FAIL)
	પાછા;
    fp = (fold_T *)gap->ga_data + i;
    fp[1].fd_ટોચ = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_ટોચ - fp->fd_ટોચ);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (શૂન્ય)foldFind(gap1, bot + 1 - fp->fd_ટોચ, &fp2);
    જો (fp2 != ખાલી)
    {
	len = (પૂર્ણાંક)((fold_T *)gap1->ga_data + gap1->ga_len - fp2);
	જો (len > 0 && ga_grow(gap2, len) == OK)
	{
	    માટે (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_data)[idx] = fp2[idx];
		((fold_T *)gap2->ga_data)[idx].fd_ટોચ
						  -= fp[1].fd_ટોચ - fp->fd_ટોચ;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = ટોચ - fp->fd_ટોચ;
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
    સ્થિર શૂન્ય
foldRemove(garray_T *gap, linenr_T ટોચ, linenr_T bot)
{
    fold_T	*fp = ખાલી;

    જો (bot < ટોચ)
	પાછા;		// nothing to do

    જ્યારે (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	જો (foldFind(gap, ટોચ, &fp) && fp->fd_ટોચ < ટોચ)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, ટોચ - fp->fd_ટોચ, bot - fp->fd_ટોચ);
	    જો (fp->fd_ટોચ + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data), ટોચ, bot);
	    }
	    નહીંતર
	    {
		// 2: truncate fold at "top".
		fp->fd_len = ટોચ - fp->fd_ટોચ;
	    }
	    fold_changed = TRUE;
	    ચાલુ;
	}
	જો (fp >= (fold_T *)(gap->ga_data) + gap->ga_len
		|| fp->fd_ટોચ > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    તોડો;
	}
	જો (fp->fd_ટોચ >= ટોચ)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    જો (fp->fd_ટોચ + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_ટોચ),
			(linenr_T)MAXLNUM, (long)(fp->fd_ટોચ - bot - 1));
		fp->fd_len -= bot - fp->fd_ટોચ + 1;
		fp->fd_ટોચ = bot + 1;
		તોડો;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (પૂર્ણાંક)(fp - (fold_T *)gap->ga_data), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    સ્થિર શૂન્ય
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *ડાબું, *જમણું;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    માટે (; start < end; start++, end--)
    {
	ડાબું = (fold_T *)gap->ga_data + start;
	જમણું = (fold_T *)gap->ga_data + end;
	tmp  = *ડાબું;
	*ડાબું = *જમણું;
	*જમણું = tmp;
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

    સ્થિર શૂન્ય
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_ટોચ, MAXLNUM);
    fp->fd_len = end - fp->fd_ટોચ;
}

# define fold_end(fp) ((fp)->fd_ટોચ + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_data + (gap)->ga_len))
# define fold_index(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_data)))

    શૂન્ય
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    પૂર્ણાંક at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_index = 0;

    જો (at_start)
    {
	જો (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_ટોચ, line2 -
		    fp->fd_ટોચ, dest - fp->fd_ટોચ);
	    પાછા;
	}
	નહીંતર જો (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ટોચ, line2 -
		    fp->fd_ટોચ, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	નહીંતર
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    જો (!valid_fold(fp, gap) || fp->fd_ટોચ > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	પાછા;
    }
    નહીંતર જો (fp->fd_ટોચ > line2)
    {
	માટે (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_ટોચ -= range_len;

	જો (valid_fold(fp, gap) && fp->fd_ટોચ <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_ટોચ -= range_len;
	}
	પાછા;
    }
    નહીંતર જો (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_ટોચ, dest -
		fp->fd_ટોચ, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_ટોચ += move_len;
	પાછા;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_index(fp, gap);

    માટે (; valid_fold(fp, gap) && fp->fd_ટોચ <= dest; fp++)
    {
	જો (fp->fd_ટોચ <= line2)
	{
	    // 1. 2. or 3.
	    જો (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_ટોચ += move_len;
	    ચાલુ;
	}

	// Record index of the first fold after the moved range.
	જો (move_end == 0)
	    move_end = fold_index(fp, gap);

	જો (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_ટોચ -= range_len;
    }

    dest_index = fold_index(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    જો (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	પાછા;
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
    સ્થિર શૂન્ય
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    પૂર્ણાંક		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    જો (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    જો (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	માટે (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_data)[gap1->ga_len]
					= ((fold_T *)gap2->ga_data)[idx];
	    ((fold_T *)gap1->ga_data)[gap1->ga_len].fd_ટોચ += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (પૂર્ણાંક)(fp2 - (fold_T *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    સ્થિર શૂન્ય
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    જો (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ખાલી)
    {
	// first and last line can't be undefined, use level 0
	જો (lnum == 1 || lnum == buf->b_ml.ml_લીટી_ગણતરી)
	    flp->lvl = 0;
	નહીંતર
	    flp->lvl = -1;
    }
    નહીંતર
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_કિંમત(buf);
    જો (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	જો (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    સ્થિર શૂન્ય
foldlevelDiff(fline_T *flp)
{
    જો (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    નહીંતર
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    સ્થિર શૂન્ય
foldlevel_પદાવલિ(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# નહીંતર
    win_T	*win;
    પૂર્ણાંક		n;
    પૂર્ણાંક		c;
    linenr_T	lnum = flp->lnum + flp->off;
    પૂર્ણાંક		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    જો (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = ચાવી_Typed;
    n = eval_foldexpr(flp->wp, &c);
    ચાવી_Typed = save_keytyped;

    પસંદગી (c)
    {
	// "a1", "a2", .. : add to the fold level
	સ્થિતિ 'a': જો (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  તોડો;

	// "s1", "s2", .. : subtract from the fold level
	સ્થિતિ 's': જો (flp->lvl >= 0)
		  {
		      જો (n > flp->lvl)
			  flp->lvl_next = 0;
		      નહીંતર
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  તોડો;

	// ">1", ">2", .. : start a fold with a certain level
	સ્થિતિ '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  તોડો;

	// "<1", "<2", .. : end a fold with a certain level
	સ્થિતિ '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_next = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  તોડો;

	// "=": No change in level
	સ્થિતિ '=': flp->lvl_next = flp->lvl;
		  તોડો;

	// "-1", "0", "1", ..: set fold level
	મૂળભૂત:  જો (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_next = flp->lvl;
		  નહીંતર
		      flp->lvl_next = n;
		  flp->lvl = n;
		  તોડો;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    જો (flp->lvl < 0)
    {
	જો (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	જો (lnum == curbuf->b_ml.ml_લીટી_ગણતરી)
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
    સ્થિર શૂન્ય
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (પૂર્ણાંક)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (પૂર્ણાંક)STRLEN(foldendmarker);
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
    સ્થિર શૂન્ય
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    પૂર્ણાંક		cstart;
    પૂર્ણાંક		cend;
    પૂર્ણાંક		start_lvl = flp->lvl;
    char_u	*s;
    પૂર્ણાંક		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    જ્યારે (*s)
    {
	જો (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    જો (VIM_ISDIGIT(*s))
	    {
		n = atoi((અક્ષર *)s);
		જો (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    જો (n <= start_lvl)
			flp->start = 1;
		    નહીંતર
			flp->start = n - start_lvl;
		}
	    }
	    નહીંતર
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	નહીંતર જો (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    જો (VIM_ISDIGIT(*s))
	    {
		n = atoi((અક્ષર *)s);
		જો (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    // never start a fold with an end marker
		    જો (flp->lvl_next > start_lvl)
			flp->lvl_next = start_lvl;
		}
	    }
	    નહીંતર
		--flp->lvl_next;
	}
	નહીંતર
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    જો (flp->lvl_next < 0)
	flp->lvl_next = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    સ્થિર શૂન્ય
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# નહીંતર
    linenr_T	lnum = flp->lnum + flp->off;
    પૂર્ણાંક		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    જો (lnum < flp->wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	જો (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# જો defined(FEAT_SESSION)
સ્થિર પૂર્ણાંક put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
સ્થિર પૂર્ણાંક put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
સ્થિર પૂર્ણાંક put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    પૂર્ણાંક
put_folds(FILE *fd, win_T *wp)
{
    જો (foldmethodIsManual(wp))
    {
	જો (put_લીટી(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_લીટી(fd, "&fdl = &fdl") == FAIL)
	    પાછા FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    જો (wp->w_fold_manual)
	પાછા put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    પાછા OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    સ્થિર પૂર્ણાંક
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    પૂર્ણાંક		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    માટે (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	જો (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_ટોચ) == FAIL)
	    પાછા FAIL;
	જો (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_ટોચ + off,
					fp->fd_ટોચ + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    પાછા FAIL;
	++fp;
    }
    પાછા OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    સ્થિર પૂર્ણાંક
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    માટે (i = 0; i < gap->ga_len; i++)
    {
	જો (fp->fd_flags != FD_LEVEL)
	{
	    જો (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		જો (fprintf(fd, ":%ld", fp->fd_ટોચ + off) < 0
			|| put_eol(fd) == FAIL
			|| put_લીટી(fd, "sil! normal! zo") == FAIL)
		    પાછા FAIL;
		જો (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_ટોચ)
			== FAIL)
		    પાછા FAIL;
		// close the parent when needed
		જો (fp->fd_flags == FD_CLOSED)
		{
		    જો (put_fold_open_close(fd, fp, off) == FAIL)
			પાછા FAIL;
		}
	    }
	    નહીંતર
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_ટોચ);
		જો ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		જો (put_fold_open_close(fd, fp, off) == FAIL)
		    પાછા FAIL;
	    }
	}
	++fp;
    }

    પાછા OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    સ્થિર પૂર્ણાંક
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    જો (fprintf(fd, ":%ld", fp->fd_ટોચ + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	પાછા FAIL;

    પાછા OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#જો defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    સ્થિર શૂન્ય
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    પૂર્ણાંક		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_સંખ્યા(&argvars[1]), &tp);
	જો (wp != ખાલી && tp != ખાલી)
	{
	    જો (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	નહીંતર
	{
	    rettv->vval.v_સંખ્યા = -2;
	    પાછા;
	}
    }
    નહીંતર
	lnum = tv_get_lnum(argvars);

    જો (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
    {
	જો (hasFoldingWin(wp, lnum, &first, &last, FALSE, ખાલી))
	{
	    જો (end)
		rettv->vval.v_સંખ્યા = (varnumber_T)last;
	    નહીંતર
		rettv->vval.v_સંખ્યા = (varnumber_T)first;
	    પાછા;
	}
    }
# endif
    rettv->vval.v_સંખ્યા = -1;
}

/*
 * "foldclosed()" function
 */
    શૂન્ય
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    શૂન્ય
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    સ્થિર પૂર્ણાંક
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    જો (invalid_ટોચ == (linenr_T)0)
	checkupdate(curwin);
    નહીંતર જો (lnum == prev_lnum && prev_lnum_lvl >= 0)
	પાછા prev_lnum_lvl;
    નહીંતર જો (lnum >= invalid_ટોચ && lnum <= invalid_bot)
	પાછા -1;

    // Return quickly when there is no folding at all in this window.
    જો (!hasAnyFolding(curwin))
	પાછા 0;

    પાછા foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    શૂન્ય
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_સંખ્યા(&argvars[1]), &tp);
	જો (wp != ખાલી && tp != ખાલી)
	{
	    જો (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
		    rettv->vval.v_સંખ્યા = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	નહીંતર
	    rettv->vval.v_સંખ્યા = -2;
    }
    નહીંતર
    {
	lnum = tv_get_lnum(argvars);
	જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
	    rettv->vval.v_સંખ્યા = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    શૂન્ય
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    પૂર્ણાંક		len;
    અક્ષર	*txt;
    long	ગણતરી;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    જો (foldstart > 0 && foldend <= curbuf->b_ml.ml_લીટી_ગણતરી
	    && dashes != ખાલી)
    {
	// Find first non-empty line in the fold.
	માટે (lnum = foldstart; lnum < foldend; ++lnum)
	    જો (!linewhite(lnum))
		તોડો;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	જો (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    જો (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		જો (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	ગણતરી = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", ગણતરી);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	જો (r != ખાલી)
	{
	    sprintf((અક્ષર *)r, txt, dashes, ગણતરી);
	    len = (પૂર્ણાંક)STRLEN(r);
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
    શૂન્ય
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    પૂર્ણાંક		fold_ગણતરી;
    win_T	*wp  = curwin;
    સ્થિર પૂર્ણાંક	entered = FALSE;
    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (entered)
	પાછા; // reject recursive use
    entered = TRUE;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_સંખ્યા(&argvars[1]), &tp);
	જો (wp == ખાલી || tp == ખાલી) // wrong winid
	{
	    entered = FALSE;
	    પાછા;
	}
	જો (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    નહીંતર
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    જો (lnum < 0)
	lnum = 0;
    fold_ગણતરી = folded_ગણતરી(wp, lnum, &foldinfo);
    જો (fold_ગણતરી > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_ગણતરી - 1,
							       &foldinfo, buf);
	જો (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
