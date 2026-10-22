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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
തരനിർവചനം ഘടന
{
    linenr_T	fd_മുകൾ;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    അക്ഷരം	fd_flags;	// see below
    അക്ഷരം	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
സ്ഥിതം ശൂന്യം newFoldLevelWin(win_T *wp);
സ്ഥിതം പൂർണ്ണസംഖ്യ checkCloseRec(garray_T *gap, linenr_T lnum, പൂർണ്ണസംഖ്യ level);
സ്ഥിതം പൂർണ്ണസംഖ്യ foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp);
സ്ഥിതം പൂർണ്ണസംഖ്യ foldLevelWin(win_T *wp, linenr_T lnum);
സ്ഥിതം ശൂന്യം checkupdate(win_T *wp);
സ്ഥിതം ശൂന്യം setFoldRepeat(linenr_T lnum, long എണ്ണം, പൂർണ്ണസംഖ്യ do_open);
സ്ഥിതം linenr_T setManualFold(linenr_T lnum, പൂർണ്ണസംഖ്യ opening, പൂർണ്ണസംഖ്യ recurse, പൂർണ്ണസംഖ്യ *donep);
സ്ഥിതം linenr_T setManualFoldWin(win_T *wp, linenr_T lnum, പൂർണ്ണസംഖ്യ opening, പൂർണ്ണസംഖ്യ recurse, പൂർണ്ണസംഖ്യ *donep);
സ്ഥിതം ശൂന്യം foldOpenNested(fold_T *fpr);
സ്ഥിതം ശൂന്യം deleteFoldEntry(garray_T *gap, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ recursive);
സ്ഥിതം ശൂന്യം foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, long amount, long amount_after);
സ്ഥിതം പൂർണ്ണസംഖ്യ getDeepestNestingRecurse(garray_T *gap);
സ്ഥിതം പൂർണ്ണസംഖ്യ check_closed(win_T *win, fold_T *fp, പൂർണ്ണസംഖ്യ *use_levelp, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ *maybe_smallp, linenr_T lnum_off);
സ്ഥിതം ശൂന്യം checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
സ്ഥിതം ശൂന്യം setSmallMaybe(garray_T *gap);
സ്ഥിതം ശൂന്യം foldCreateMarkers(linenr_T start, linenr_T end);
സ്ഥിതം ശൂന്യം foldAddMarker(linenr_T lnum, char_u *marker, പൂർണ്ണസംഖ്യ markerlen);
സ്ഥിതം ശൂന്യം deleteFoldMarkers(fold_T *fp, പൂർണ്ണസംഖ്യ recursive, linenr_T lnum_off);
സ്ഥിതം ശൂന്യം foldDelMarker(linenr_T lnum, char_u *marker, പൂർണ്ണസംഖ്യ markerlen);
സ്ഥിതം ശൂന്യം foldUpdateIEMS(win_T *wp, linenr_T മുകൾ, linenr_T bot);
സ്ഥിതം ശൂന്യം parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
സ്ഥിതം linenr_T invalid_മുകൾ = (linenr_T)0;
സ്ഥിതം linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
സ്ഥിതം linenr_T prev_lnum = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define DONE_NOTHING	0
# define DONE_ACTION	1	// did close or open a fold
# define DONE_FOLD	2	// did find a fold

സ്ഥിതം പൂർണ്ണസംഖ്യ foldstartmarkerlen;
സ്ഥിതം char_u *foldendmarker;
സ്ഥിതം പൂർണ്ണസംഖ്യ foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    തിരികെ (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    പൂർണ്ണസംഖ്യ
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    തിരികെ hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ശൂന്യ);
}

// hasFoldingWin() {{{2
    പൂർണ്ണസംഖ്യ
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    പൂർണ്ണസംഖ്യ		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    പൂർണ്ണസംഖ്യ		had_folded = FALSE;
    linenr_T	first = 0;
    linenr_T	last = 0;
    linenr_T	lnum_rel = lnum;
    പൂർണ്ണസംഖ്യ		x;
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		level = 0;
    പൂർണ്ണസംഖ്യ		use_level = FALSE;
    പൂർണ്ണസംഖ്യ		maybe_small = FALSE;
    garray_T	*gap;
    പൂർണ്ണസംഖ്യ		താഴെയുള്ള_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    എങ്കിൽ (!hasAnyFolding(win))
    {
	എങ്കിൽ (infop != ശൂന്യ)
	    infop->fi_level = 0;
	തിരികെ FALSE;
    }

    എങ്കിൽ (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	എങ്കിൽ (x >= 0)
	{
	    first = win->w_വരികൾ[x].wl_lnum;
	    last = win->w_വരികൾ[x].wl_lastlnum;
	    had_folded = win->w_വരികൾ[x].wl_folded;
	}
    }

    എങ്കിൽ (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (!foldFind(gap, lnum_rel, &fp))
		നിർത്തുക;

	    // Remember lowest level of fold that starts in "lnum".
	    എങ്കിൽ (lnum_rel == fp->fd_മുകൾ && താഴെയുള്ള_level == 0)
		താഴെയുള്ള_level = level + 1;

	    first += fp->fd_മുകൾ;
	    last += fp->fd_മുകൾ;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    എങ്കിൽ (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		last += fp->fd_len - 1;
		നിർത്തുക;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_മുകൾ;
	    ++level;
	}
    }

    എങ്കിൽ (!had_folded)
    {
	എങ്കിൽ (infop != ശൂന്യ)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_താഴെയുള്ള_level = താഴെയുള്ള_level == 0 ? level : താഴെയുള്ള_level;
	}
	തിരികെ FALSE;
    }

    എങ്കിൽ (last > win->w_buffer->b_ml.ml_വരി_എണ്ണം)
	last = win->w_buffer->b_ml.ml_വരി_എണ്ണം;
    എങ്കിൽ (lastp != ശൂന്യ)
	*lastp = last;
    എങ്കിൽ (firstp != ശൂന്യ)
	*firstp = first;
    എങ്കിൽ (infop != ശൂന്യ)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = first;
	infop->fi_താഴെയുള്ള_level = താഴെയുള്ള_level == 0 ? level + 1 : താഴെയുള്ള_level;
    }
    തിരികെ TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    പൂർണ്ണസംഖ്യ
വരി_Folded(win_T *win, linenr_T lnum)
{
    തിരികെ folded_എണ്ണം(win, lnum, ശൂന്യ) != 0;
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
folded_എണ്ണം(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	last;

    എങ്കിൽ (hasFoldingWin(win, lnum, ശൂന്യ, &last, FALSE, infop))
	തിരികെ (long)(last - lnum + 1);
    തിരികെ 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    പൂർണ്ണസംഖ്യ
foldmethodIsManual(win_T *wp)
{
    തിരികെ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    പൂർണ്ണസംഖ്യ
foldmethodIsIndent(win_T *wp)
{
    തിരികെ (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    പൂർണ്ണസംഖ്യ
foldmethod_Is_വ്യഞ്ജകം(win_T *wp)
{
    തിരികെ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    പൂർണ്ണസംഖ്യ
foldmethodIsMarker(win_T *wp)
{
    തിരികെ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    പൂർണ്ണസംഖ്യ
foldmethodIsSyntax(win_T *wp)
{
    തിരികെ (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    പൂർണ്ണസംഖ്യ
foldmethodIsDiff(win_T *wp)
{
    തിരികെ (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ശൂന്യം
closeFold(linenr_T lnum, long എണ്ണം)
{
    setFoldRepeat(lnum, എണ്ണം, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    ശൂന്യം
closeFoldRecurse(linenr_T lnum)
{
    (ശൂന്യം)setManualFold(lnum, FALSE, TRUE, ശൂന്യ);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    ശൂന്യം
opFoldRange(
    linenr_T	first,
    linenr_T	last,
    പൂർണ്ണസംഖ്യ		opening,	// TRUE to open, FALSE to close
    പൂർണ്ണസംഖ്യ		recurse,	// TRUE to do it recursively
    പൂർണ്ണസംഖ്യ		had_visual)	// TRUE when Visual selection used
{
    പൂർണ്ണസംഖ്യ		done = DONE_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_next;

    വേണ്ടി (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	എങ്കിൽ (opening && !recurse)
	    (ശൂന്യം)hasFolding(lnum, ശൂന്യ, &lnum_next);
	(ശൂന്യം)setManualFold(lnum, opening, recurse, &done);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	എങ്കിൽ (!opening && !recurse)
	    (ശൂന്യം)hasFolding(lnum, ശൂന്യ, &lnum_next);
    }
    എങ്കിൽ (done == DONE_NOTHING)
	emsg(_(e_no_fold_found));
    // Force a redraw to remove the Visual highlighting.
    എങ്കിൽ (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ശൂന്യം
openFold(linenr_T lnum, long എണ്ണം)
{
    setFoldRepeat(lnum, എണ്ണം, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    ശൂന്യം
openFoldRecurse(linenr_T lnum)
{
    (ശൂന്യം)setManualFold(lnum, TRUE, TRUE, ശൂന്യ);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    ശൂന്യം
foldOpenCursor(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		done;

    checkupdate(curwin);
    എങ്കിൽ (hasAnyFolding(curwin))
	വേണ്ടി (;;)
	{
	    done = DONE_NOTHING;
	    (ശൂന്യം)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    എങ്കിൽ (!(done & DONE_ACTION))
		നിർത്തുക;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    ശൂന്യം
newFoldLevel(ശൂന്യം)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    എങ്കിൽ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    സ്ഥിതം ശൂന്യം
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		i;

    checkupdate(wp);
    എങ്കിൽ (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_data;
	വേണ്ടി (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    changed_window_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    ശൂന്യം
foldCheckClose(ശൂന്യം)
{
    എങ്കിൽ (*p_fcl == NUL)
	തിരികെ;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    എങ്കിൽ (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(പൂർണ്ണസംഖ്യ)curwin->w_p_fdl))
	changed_window_setting();
}

// checkCloseRec() {{{2
    സ്ഥിതം പൂർണ്ണസംഖ്യ
checkCloseRec(garray_T *gap, linenr_T lnum, പൂർണ്ണസംഖ്യ level)
{
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		i;

    fp = (fold_T *)gap->ga_data;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	എങ്കിൽ (fp[i].fd_flags == FD_OPEN)
	{
	    എങ്കിൽ (level <= 0 && (lnum < fp[i].fd_മുകൾ
				      || lnum >= fp[i].fd_മുകൾ + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    അല്ലെങ്കിൽ
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_മുകൾ,
								   level - 1);
	}
    }
    തിരികെ retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    പൂർണ്ണസംഖ്യ
foldManualAllowed(പൂർണ്ണസംഖ്യ create)
{
    എങ്കിൽ (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	തിരികെ TRUE;
    എങ്കിൽ (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    അല്ലെങ്കിൽ
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    തിരികെ FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    ശൂന്യം
foldCreate(linenr_T start, linenr_T end)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		cont;
    പൂർണ്ണസംഖ്യ		use_level = FALSE;
    പൂർണ്ണസംഖ്യ		closed = FALSE;
    പൂർണ്ണസംഖ്യ		level = 0;
    linenr_T	start_rel = start;
    linenr_T	end_rel = end;

    എങ്കിൽ (start > end)
    {
	// reverse the range
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    എങ്കിൽ (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	തിരികെ;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    എങ്കിൽ (gap->ga_len == 0)
	i = 0;
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (!foldFind(gap, start_rel, &fp))
		നിർത്തുക;
	    എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len > end_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		start_rel -= fp->fd_മുകൾ;
		end_rel -= fp->fd_മുകൾ;
		എങ്കിൽ (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    എങ്കിൽ (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    അല്ലെങ്കിൽ
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (gap->ga_len == 0)
	    i = 0;
	അല്ലെങ്കിൽ
	    i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data);
    }

    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ;

    fp = (fold_T *)gap->ga_data + i;
    ga_init2(&fold_ga, വലുപ്പം(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    വേണ്ടി (cont = 0; i + cont < gap->ga_len; ++cont)
	എങ്കിൽ (fp[cont].fd_മുകൾ > end_rel)
	    നിർത്തുക;
    എങ്കിൽ (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	എങ്കിൽ (start_rel > fp->fd_മുകൾ)
	    start_rel = fp->fd_മുകൾ;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	എങ്കിൽ (end_rel < fp[cont - 1].fd_മുകൾ + fp[cont - 1].fd_len - 1)
	    end_rel = fp[cont - 1].fd_മുകൾ + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_data, fp, വലുപ്പം(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	വേണ്ടി (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_data)[j].fd_മുകൾ -= start_rel;
    }
    // Move remaining entries to after the new fold.
    എങ്കിൽ (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_data + i,
		വലുപ്പം(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_മുകൾ = start_rel;
    fp->fd_len = end_rel - start_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    എങ്കിൽ (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(start, 1L);
    എങ്കിൽ (!use_level)
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
    ശൂന്യം
deleteFold(
    linenr_T	start,
    linenr_T	end,
    പൂർണ്ണസംഖ്യ		recursive,
    പൂർണ്ണസംഖ്യ		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*found_ga;
    fold_T	*found_fp = ശൂന്യ;
    linenr_T	found_off = 0;
    പൂർണ്ണസംഖ്യ		use_level;
    പൂർണ്ണസംഖ്യ		maybe_small = FALSE;
    പൂർണ്ണസംഖ്യ		level = 0;
    linenr_T	lnum = start;
    linenr_T	lnum_off;
    പൂർണ്ണസംഖ്യ		did_one = FALSE;
    linenr_T	first_lnum = MAXLNUM;
    linenr_T	last_lnum = 0;

    checkupdate(curwin);

    ആകുമ്പോൾ (lnum <= end)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	found_ga = ശൂന്യ;
	lnum_off = 0;
	use_level = FALSE;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (!foldFind(gap, lnum - lnum_off, &fp))
		നിർത്തുക;
	    // lnum is inside this fold, remember info
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    എങ്കിൽ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		നിർത്തുക;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_മുകൾ;
	    ++level;
	}
	എങ്കിൽ (found_ga == ശൂന്യ)
	{
	    ++lnum;
	}
	അല്ലെങ്കിൽ
	{
	    lnum = found_fp->fd_മുകൾ + found_fp->fd_len + found_off;

	    എങ്കിൽ (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (പൂർണ്ണസംഖ്യ)(found_fp - (fold_T *)found_ga->ga_data), recursive);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (first_lnum > found_fp->fd_മുകൾ + found_off)
		    first_lnum = found_fp->fd_മുകൾ + found_off;
		എങ്കിൽ (last_lnum < lnum)
		    last_lnum = lnum;
		എങ്കിൽ (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    changed_window_setting();
	}
    }
    എങ്കിൽ (!did_one)
    {
	emsg(_(e_no_fold_found));
	// Force a redraw to remove the Visual highlighting.
	എങ്കിൽ (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    അല്ലെങ്കിൽ
	// Deleting markers may make cursor column invalid.
	check_cursor_col();

    എങ്കിൽ (last_lnum > 0)
	changed_വരികൾ(first_lnum, (colnr_T)0, last_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    ശൂന്യം
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
    ശൂന്യം
foldUpdate(win_T *wp, linenr_T മുകൾ, linenr_T bot)
{
    fold_T	*fp;

    എങ്കിൽ (disable_fold_update > 0)
	തിരികെ;
# ifdef FEAT_DIFF
    എങ്കിൽ (need_diff_redraw)
	// will update later
	തിരികെ;
# endif

    എങ്കിൽ (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_start = മുകൾ;
	linenr_T	maybe_small_end = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	എങ്കിൽ (മുകൾ > bot)
	{
	    maybe_small_start = bot;
	    maybe_small_end = മുകൾ;
	}
	(ശൂന്യം)foldFind(&wp->w_folds, maybe_small_start, &fp);
	ആകുമ്പോൾ (fp < (fold_T *)wp->w_folds.ga_data + wp->w_folds.ga_len
		&& fp->fd_മുകൾ <= maybe_small_end)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    എങ്കിൽ (foldmethodIsIndent(wp)
	    || foldmethod_Is_വ്യഞ്ജകം(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	പൂർണ്ണസംഖ്യ save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, മുകൾ, bot);
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
    ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
foldMoveTo(
    പൂർണ്ണസംഖ്യ		updown,
    പൂർണ്ണസംഖ്യ		dir,	    // FORWARD or BACKWARD
    long	എണ്ണം)
{
    long	n;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_found;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		use_level;
    പൂർണ്ണസംഖ്യ		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		level;
    പൂർണ്ണസംഖ്യ		last;

    checkupdate(curwin);

    // Repeat "count" times.
    വേണ്ടി (n = 0; n < എണ്ണം; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	എങ്കിൽ (gap->ga_len == 0)
	    നിർത്തുക;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		എങ്കിൽ (!updown || gap->ga_len == 0)
		    നിർത്തുക;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		എങ്കിൽ (dir == FORWARD)
		{
		    എങ്കിൽ (fp - (fold_T *)gap->ga_data >= gap->ga_len)
			നിർത്തുക;
		    --fp;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (fp == (fold_T *)gap->ga_data)
			നിർത്തുക;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		last = TRUE;
	    }

	    എങ്കിൽ (!last)
	    {
		// Check if this fold is closed.
		എങ്കിൽ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		// "[z" and "]z" stop at closed fold
		എങ്കിൽ (last && !updown)
		    നിർത്തുക;
	    }

	    എങ്കിൽ (updown)
	    {
		എങ്കിൽ (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    എങ്കിൽ (fp + 1 - (fold_T *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_മുകൾ + lnum_off;
			എങ്കിൽ (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // to end of previous fold if there is one
		    എങ്കിൽ (fp > (fold_T *)gap->ga_data)
		    {
			lnum = fp[-1].fd_മുകൾ + lnum_off + fp[-1].fd_len - 1;
			എങ്കിൽ (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		എങ്കിൽ (dir == FORWARD)
		{
		    lnum = fp->fd_മുകൾ + lnum_off + fp->fd_len - 1;
		    എങ്കിൽ (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		അല്ലെങ്കിൽ
		{
		    lnum = fp->fd_മുകൾ + lnum_off;
		    എങ്കിൽ (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    എങ്കിൽ (last)
		നിർത്തുക;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_മുകൾ;
	    ++level;
	}
	എങ്കിൽ (lnum_found != curwin->w_cursor.lnum)
	{
	    എങ്കിൽ (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    തിരികെ retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    ശൂന്യം
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, വലുപ്പം(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    പൂർണ്ണസംഖ്യ
find_wl_entry(win_T *win, linenr_T lnum)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < win->w_വരികൾ_valid; ++i)
	എങ്കിൽ (win->w_വരികൾ[i].wl_valid)
	{
	    എങ്കിൽ (lnum < win->w_വരികൾ[i].wl_lnum)
		തിരികെ -1;
	    എങ്കിൽ (lnum <= win->w_വരികൾ[i].wl_lastlnum)
		തിരികെ i;
	}
    തിരികെ -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    ശൂന്യം
foldAdjustVisual(ശൂന്യം)
{
    pos_T	*start, *end;

    എങ്കിൽ (!VIsual_active || !hasAnyFolding(curwin))
	തിരികെ;

    എങ്കിൽ (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    അല്ലെങ്കിൽ
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    എങ്കിൽ (hasFolding(start->lnum, &start->lnum, ശൂന്യ))
	start->col = 0;

    എങ്കിൽ (!hasFolding(end->lnum, ശൂന്യ, &end->lnum))
	തിരികെ;

    end->col = ml_get_len(end->lnum);
    എങ്കിൽ (end->col > 0 && *p_sel == 'o')
	--end->col;
    // prevent cursor from moving on the trail byte
    എങ്കിൽ (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    ശൂന്യം
foldAdjustCursor(ശൂന്യം)
{
    (ശൂന്യം)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ശൂന്യ);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    ശൂന്യം
cloneFoldGrowArray(garray_T *from, garray_T *to)
{
    പൂർണ്ണസംഖ്യ		i;
    fold_T	*from_p;
    fold_T	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    എങ്കിൽ (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	തിരികെ;

    from_p = (fold_T *)from->ga_data;
    to_p = (fold_T *)to->ga_data;

    വേണ്ടി (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_മുകൾ = from_p->fd_മുകൾ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
foldFind(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	താഴെയുള്ള, മുകളിലെ;
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (gap->ga_len == 0)
    {
	*fpp = ശൂന്യ;
	തിരികെ FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_data;
    താഴെയുള്ള = 0;
    മുകളിലെ = gap->ga_len - 1;
    ആകുമ്പോൾ (താഴെയുള്ള <= മുകളിലെ)
    {
	i = (താഴെയുള്ള + മുകളിലെ) / 2;
	എങ്കിൽ (fp[i].fd_മുകൾ > lnum)
	    // fold below lnum, adjust high
	    മുകളിലെ = i - 1;
	അല്ലെങ്കിൽ എങ്കിൽ (fp[i].fd_മുകൾ + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    താഴെയുള്ള = i + 1;
	അല്ലെങ്കിൽ
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    തിരികെ TRUE;
	}
    }
    *fpp = fp + താഴെയുള്ള;
    തിരികെ FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    പൂർണ്ണസംഖ്യ		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (!foldFind(gap, lnum_rel, &fp))
	    നിർത്തുക;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_മുകൾ;
	++level;
    }

    തിരികെ level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    സ്ഥിതം ശൂന്യം
checkupdate(win_T *wp)
{
    എങ്കിൽ (!wp->w_foldinvalid)
	തിരികെ;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    സ്ഥിതം ശൂന്യം
setFoldRepeat(linenr_T lnum, long എണ്ണം, പൂർണ്ണസംഖ്യ do_open)
{
    പൂർണ്ണസംഖ്യ		done;
    long	n;

    വേണ്ടി (n = 0; n < എണ്ണം; ++n)
    {
	done = DONE_NOTHING;
	(ശൂന്യം)setManualFold(lnum, do_open, FALSE, &done);
	എങ്കിൽ (!(done & DONE_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    എങ്കിൽ (n == 0 && !(done & DONE_FOLD))
		emsg(_(e_no_fold_found));
	    നിർത്തുക;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    സ്ഥിതം linenr_T
setManualFold(
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		opening,    // TRUE when opening, FALSE when closing
    പൂർണ്ണസംഖ്യ		recurse,    // TRUE when closing/opening recursive
    പൂർണ്ണസംഖ്യ		*donep)
{
# ifdef FEAT_DIFF
    എങ്കിൽ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		എങ്കിൽ (dlnum != 0)
		    (ശൂന്യം)setManualFoldWin(wp, dlnum, opening, recurse, ശൂന്യ);
	    }
	}
    }
# endif

    തിരികെ setManualFoldWin(curwin, lnum, opening, recurse, donep);
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
    സ്ഥിതം linenr_T
setManualFoldWin(
    win_T	*wp,
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		opening,    // TRUE when opening, FALSE when closing
    പൂർണ്ണസംഖ്യ		recurse,    // TRUE when closing/opening recursive
    പൂർണ്ണസംഖ്യ		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*found = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		j;
    പൂർണ്ണസംഖ്യ		level = 0;
    പൂർണ്ണസംഖ്യ		use_level = FALSE;
    പൂർണ്ണസംഖ്യ		found_fold = FALSE;
    garray_T	*gap;
    linenr_T	next = MAXLNUM;
    linenr_T	off = 0;
    പൂർണ്ണസംഖ്യ		done = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (!foldFind(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    എങ്കിൽ (fp != ശൂന്യ && fp < (fold_T *)gap->ga_data + gap->ga_len)
		next = fp->fd_മുകൾ + off;
	    നിർത്തുക;
	}

	// lnum is inside this fold
	found_fold = TRUE;

	// If there is a following fold, continue there next time.
	എങ്കിൽ (fp + 1 < (fold_T *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_മുകൾ + off;

	// Change from level-dependent folding to manual.
	എങ്കിൽ (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    എങ്കിൽ (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    അല്ലെങ്കിൽ
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_data;
	    വേണ്ടി (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	എങ്കിൽ (!opening && recurse)
	{
	    എങ്കിൽ (fp->fd_flags != FD_CLOSED)
	    {
		done |= DONE_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    എങ്കിൽ (opening)
	    {
		fp->fd_flags = FD_OPEN;
		done |= DONE_ACTION;
		എങ്കിൽ (recurse)
		    foldOpenNested(fp);
	    }
	    നിർത്തുക;
	}

	// fold is open, check nested folds
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_മുകൾ;
	off += fp->fd_മുകൾ;
	++level;
    }
    എങ്കിൽ (found_fold)
    {
	// When closing and not recurse, close deepest open fold.
	എങ്കിൽ (!opening && found != ശൂന്യ)
	{
	    found->fd_flags = FD_CLOSED;
	    done |= DONE_ACTION;
	}
	wp->w_fold_manual = true;
	എങ്കിൽ (done & DONE_ACTION)
	    changed_window_setting_win(wp);
	done |= DONE_FOLD;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (donep == ശൂന്യ && wp == curwin)
	emsg(_(e_no_fold_found));

    എങ്കിൽ (donep != ശൂന്യ)
	*donep |= done;

    തിരികെ next;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    സ്ഥിതം ശൂന്യം
foldOpenNested(fold_T *fpr)
{
    പൂർണ്ണസംഖ്യ		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_data;
    വേണ്ടി (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    സ്ഥിതം ശൂന്യം
deleteFoldEntry(garray_T *gap, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ recursive)
{
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		i;
    long	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_data + idx;
    എങ്കിൽ (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	എങ്കിൽ (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, വലുപ്പം(fold_T) * (gap->ga_len - idx));
    }
    അല്ലെങ്കിൽ
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	എങ്കിൽ (ga_grow(gap, (പൂർണ്ണസംഖ്യ)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_data + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_data;
	    വേണ്ടി (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_മുകൾ += fp->fd_മുകൾ;
		എങ്കിൽ (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		എങ്കിൽ (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    എങ്കിൽ (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  വലുപ്പം(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(വലുപ്പം(fold_T) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	}
    }
}

// deleteFoldRecurse() {{{2
/*
 * Delete nested folds in a fold.
 */
    ശൂന്യം
deleteFoldRecurse(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
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
    ശൂന്യം
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    എങ്കിൽ (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    എങ്കിൽ (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    എങ്കിൽ ((State & MODE_തിരുകുക) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    സ്ഥിതം ശൂന്യം
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    fold_T	*fp;
    പൂർണ്ണസംഖ്യ		i;
    linenr_T	last;
    linenr_T	മുകൾ;

    എങ്കിൽ (gap->ga_len == 0)
	തിരികെ;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    എങ്കിൽ ((State & MODE_തിരുകുക) && amount == (linenr_T)1 && line2 == MAXLNUM)
	മുകൾ = line1 + 1;
    അല്ലെങ്കിൽ
	മുകൾ = line1;

    // Find the fold containing or just below "line1".
    (ശൂന്യം)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    വേണ്ടി (i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data); i < gap->ga_len; ++i, ++fp)
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

	last = fp->fd_മുകൾ + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	എങ്കിൽ (last < line1)
	    തുടരുക;

	// 6. fold below line2: only adjust for amount_after
	എങ്കിൽ (fp->fd_മുകൾ > line2)
	{
	    എങ്കിൽ (amount_after == 0)
		നിർത്തുക;
	    fp->fd_മുകൾ += amount_after;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (fp->fd_മുകൾ >= മുകൾ && last <= line2)
	    {
		// 4. fold completely contained in range
		എങ്കിൽ (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		അല്ലെങ്കിൽ
		    fp->fd_മുകൾ += amount;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (fp->fd_മുകൾ < മുകൾ)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_മുകൾ,
				  line2 - fp->fd_മുകൾ, amount, amount_after);
		    എങ്കിൽ (last <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			എങ്കിൽ (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_മുകൾ;
			അല്ലെങ്കിൽ
			    fp->fd_len += amount;
		    }
		    അല്ലെങ്കിൽ
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    എങ്കിൽ (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_മുകൾ,
				  amount,
				  amount_after + (fp->fd_മുകൾ - മുകൾ));
			fp->fd_len -= line2 - fp->fd_മുകൾ + 1;
			fp->fd_മുകൾ = line1;
		    }
		    അല്ലെങ്കിൽ
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_മുകൾ,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_മുകൾ += amount;
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
    പൂർണ്ണസംഖ്യ
getDeepestNesting(ശൂന്യം)
{
    checkupdate(curwin);
    തിരികെ getDeepestNestingRecurse(&curwin->w_folds);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
getDeepestNestingRecurse(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		level;
    പൂർണ്ണസംഖ്യ		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	എങ്കിൽ (level > maxlevel)
	    maxlevel = level;
    }

    തിരികെ maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_closed(
    win_T	*win,
    fold_T	*fp,
    പൂർണ്ണസംഖ്യ		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    പൂർണ്ണസംഖ്യ		level,		    // folding depth
    പൂർണ്ണസംഖ്യ		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    പൂർണ്ണസംഖ്യ		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    എങ്കിൽ (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	എങ്കിൽ (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    എങ്കിൽ (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    എങ്കിൽ (closed)
    {
	എങ്കിൽ (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	എങ്കിൽ (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    തിരികെ closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    സ്ഥിതം ശൂന്യം
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (fp->fd_small != MAYBE)
	തിരികെ;

    // Mark any nested folds to maybe-small
    setSmallMaybe(&fp->fd_nested);

    എങ്കിൽ (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    അല്ലെങ്കിൽ
    {
	എണ്ണം = 0;
	വേണ്ടി (n = 0; n < fp->fd_len; ++n)
	{
	    എണ്ണം += plines_win_nofold(wp, fp->fd_മുകൾ + lnum_off + n);
	    എങ്കിൽ (എണ്ണം > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		തിരികെ;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    സ്ഥിതം ശൂന്യം
setSmallMaybe(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    സ്ഥിതം ശൂന്യം
foldCreateMarkers(linenr_T start, linenr_T end)
{
    എങ്കിൽ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	തിരികെ;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    changed_വരികൾ(start, (colnr_T)0, end, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    സ്ഥിതം ശൂന്യം
foldAddMarker(linenr_T lnum, char_u *marker, പൂർണ്ണസംഖ്യ markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		വരി_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((അക്ഷരം *)curbuf->b_p_cms, "%s");
    പൂർണ്ണസംഖ്യ		വരി_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    വരി = ml_get(lnum);
    വരി_len = ml_get_len(lnum);

    എങ്കിൽ (u_save(lnum - 1, lnum + 1) != OK)
	തിരികെ;

    // Check if the line ends with an unclosed comment
    (ശൂന്യം)skip_comment(വരി, FALSE, FALSE, &വരി_is_comment);
    newline = alloc(വരി_len + markerlen + STRLEN(cms) + 1);
    എങ്കിൽ (newline == ശൂന്യ)
	തിരികെ;
    STRCPY(newline, വരി);
    // Append the marker to the end of the line
    എങ്കിൽ (p == ശൂന്യ || വരി_is_comment)
	vim_strncpy(newline + വരി_len, marker, markerlen);
    അല്ലെങ്കിൽ
    {
	STRCPY(newline + വരി_len, cms);
	STRNCPY(newline + വരി_len + (p - cms), marker, markerlen);
	STRCPY(newline + വരി_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    സ്ഥിതം ശൂന്യം
deleteFoldMarkers(
    fold_T	*fp,
    പൂർണ്ണസംഖ്യ		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (recursive)
	വേണ്ടി (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_മുകൾ);
    foldDelMarker(fp->fd_മുകൾ + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_മുകൾ + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    സ്ഥിതം ശൂന്യം
foldDelMarker(linenr_T lnum, char_u *marker, പൂർണ്ണസംഖ്യ markerlen)
{
    char_u	*വരി;
    char_u	*newline;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ;
    വരി = ml_get(lnum);
    വേണ്ടി (p = വരി; *p != NUL; ++p)
	എങ്കിൽ (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    എങ്കിൽ (VIM_ISDIGIT(p[len]))
		++len;
	    എങ്കിൽ (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)strstr((അക്ഷരം *)cms, "%s");
		എങ്കിൽ (cms2 != ശൂന്യ && p - വരി >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (പൂർണ്ണസംഖ്യ)STRLEN(cms) - 2;
		}
	    }
	    എങ്കിൽ (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		എങ്കിൽ (newline != ശൂന്യ)
		{
		    STRNCPY(newline, വരി, p - വരി);
		    STRCPY(newline + (p - വരി), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    നിർത്തുക;
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
    char_u	*text = ശൂന്യ;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    got_fdt_error = FALSE;
    പൂർണ്ണസംഖ്യ		    save_did_emsg = did_emsg;
    സ്ഥിതം win_T    *last_wp = ശൂന്യ;
    സ്ഥിതം linenr_T last_lnum = 0;

    എങ്കിൽ (last_wp != wp || last_wp == ശൂന്യ
					|| last_lnum > lnum || last_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    എങ്കിൽ (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    എങ്കിൽ (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	പൂർണ്ണസംഖ്യ	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	set_vim_var_nr(VV_FOLDSTART, lnum);
	set_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	എങ്കിൽ (level > (പൂർണ്ണസംഖ്യ)വലുപ്പം(dashes) - 1)
	    level = (പൂർണ്ണസംഖ്യ)വലുപ്പം(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	set_vim_var_string(VV_FOLDDASHES, dashes, level);
	set_vim_var_nr(VV_FOLDLEVEL, (long)level);

	// skip evaluating 'foldtext' on errors
	എങ്കിൽ (!got_fdt_error)
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

	    എങ്കിൽ (text == ശൂന്യ || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    current_sctx = saved_sctx;
	}
	last_lnum = lnum;
	last_wp   = wp;
	set_vim_var_string(VV_FOLDDASHES, ശൂന്യ, -1);

	എങ്കിൽ (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	എങ്കിൽ (text != ശൂന്യ)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    വേണ്ടി (p = text; *p != NUL; ++p)
	    {
		പൂർണ്ണസംഖ്യ	len;

		എങ്കിൽ (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    എങ്കിൽ (!vim_isprintc((*mb_ptr2char)(p)))
			നിർത്തുക;
		    p += len - 1;
		}
		അല്ലെങ്കിൽ
		    എങ്കിൽ (*p == TAB)
			*p = ' ';
		    അല്ലെങ്കിൽ എങ്കിൽ (ptr2cells(p) > 1)
			നിർത്തുക;
	    }
	    എങ്കിൽ (*p != NUL)
	    {
		p = transstr(text);
		vim_free(text);
		text = p;
	    }
	}
    }
    എങ്കിൽ (text == ശൂന്യ)
# endif
    {
	long എണ്ണം = (long)(lnume - lnum + 1);

	vim_snprintf((അക്ഷരം *)buf, FOLD_TEXT_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", എണ്ണം),
		     എണ്ണം);
	text = buf;
    }
    തിരികെ text;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    സ്ഥിതം ശൂന്യം
foldtext_cleanup(char_u *str)
{
    char_u	*cms_start;	// first part or the whole comment
    പൂർണ്ണസംഖ്യ		cms_slen = 0;	// length of cms_start
    char_u	*cms_end;	// last part of the comment or NULL
    പൂർണ്ണസംഖ്യ		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		did1 = FALSE;
    പൂർണ്ണസംഖ്യ		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_start = skipwhite(curbuf->b_p_cms);
    cms_slen = (പൂർണ്ണസംഖ്യ)STRLEN(cms_start);
    ആകുമ്പോൾ (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_end = (char_u *)strstr((അക്ഷരം *)cms_start, "%s");
    എങ്കിൽ (cms_end != ശൂന്യ)
    {
	cms_elen = cms_slen - (പൂർണ്ണസംഖ്യ)(cms_end - cms_start);
	cms_slen = (പൂർണ്ണസംഖ്യ)(cms_end - cms_start);

	// exclude white space before "%s"
	ആകുമ്പോൾ (cms_slen > 0 && VIM_ISWHITE(cms_start[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_end + 2);
	cms_elen -= (പൂർണ്ണസംഖ്യ)(s - cms_end);
	cms_end = s;
    }
    parseMarker(curwin);

    വേണ്ടി (s = str; *s != NUL; )
    {
	len = 0;
	എങ്കിൽ (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	എങ്കിൽ (len > 0)
	{
	    എങ്കിൽ (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    വേണ്ടി (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    എങ്കിൽ (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_start, cms_slen) == 0)
	    {
		len += (പൂർണ്ണസംഖ്യ)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cms_end != ശൂന്യ)
	{
	    എങ്കിൽ (!did1 && cms_slen > 0 && STRNCMP(s, cms_start, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_end, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	എങ്കിൽ (len != 0)
	{
	    ആകുമ്പോൾ (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	അല്ലെങ്കിൽ
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
തരനിർവചനം ഘടന
{
    win_T	*wp;		// window
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    പൂർണ്ണസംഖ്യ		lvl;		// current level (-1 for undefined)
    പൂർണ്ണസംഖ്യ		lvl_next;	// level used for next line
    പൂർണ്ണസംഖ്യ		start;		// number of folds that are forced to start at
				// this line.
    പൂർണ്ണസംഖ്യ		end;		// level of fold that is forced to end below
				// this line
    പൂർണ്ണസംഖ്യ		had_end;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
സ്ഥിതം പൂർണ്ണസംഖ്യ fold_changed;

// Function declarations. {{{2
സ്ഥിതം linenr_T foldUpdateIEMSRecurse(garray_T *gap, പൂർണ്ണസംഖ്യ level, linenr_T startlnum, fline_T *flp, ശൂന്യം (*getlevel)(fline_T *), linenr_T bot, പൂർണ്ണസംഖ്യ topflags);
സ്ഥിതം പൂർണ്ണസംഖ്യ fold_തിരുകുക(garray_T *gap, പൂർണ്ണസംഖ്യ i);
സ്ഥിതം ശൂന്യം foldSplit(garray_T *gap, പൂർണ്ണസംഖ്യ i, linenr_T മുകൾ, linenr_T bot);
സ്ഥിതം ശൂന്യം foldRemove(garray_T *gap, linenr_T മുകൾ, linenr_T bot);
സ്ഥിതം ശൂന്യം foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2);
സ്ഥിതം ശൂന്യം foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
സ്ഥിതം ശൂന്യം foldlevelDiff(fline_T *flp);
# endif
സ്ഥിതം ശൂന്യം foldlevel_വ്യഞ്ജകം(fline_T *flp);
സ്ഥിതം ശൂന്യം foldlevelMarker(fline_T *flp);
സ്ഥിതം ശൂന്യം foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    സ്ഥിതം ശൂന്യം
foldUpdateIEMS(win_T *wp, linenr_T മുകൾ, linenr_T bot)
{
    linenr_T	start;
    linenr_T	end;
    fline_T	fline;
    ശൂന്യം	(*getlevel)(fline_T *);
    പൂർണ്ണസംഖ്യ		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    എങ്കിൽ (invalid_മുകൾ != (linenr_T)0)
	തിരികെ;

    എങ്കിൽ (wp->w_foldinvalid)
    {
	// Need to update all folds.
	മുകൾ = 1;
	bot = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	setSmallMaybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    എങ്കിൽ (foldmethodIsDiff(wp))
    {
	എങ്കിൽ (മുകൾ > diff_context)
	    മുകൾ -= diff_context;
	അല്ലെങ്കിൽ
	    മുകൾ = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    എങ്കിൽ (മുകൾ > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	മുകൾ = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_മുകൾ = മുകൾ;
    invalid_bot = bot;

    എങ്കിൽ (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	എങ്കിൽ (മുകൾ > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, മുകൾ - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = മുകൾ - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    എങ്കിൽ (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_next);
	    അല്ലെങ്കിൽ
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = മുകൾ;
	getlevel(&fline);
    }
    അല്ലെങ്കിൽ
    {
	fline.lnum = മുകൾ;
	എങ്കിൽ (foldmethod_Is_വ്യഞ്ജകം(wp))
	{
	    getlevel = foldlevel_വ്യഞ്ജകം;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    എങ്കിൽ (മുകൾ > 1)
		--fline.lnum;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	അല്ലെങ്കിൽ എങ്കിൽ (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	അല്ലെങ്കിൽ
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    എങ്കിൽ (മുകൾ > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	വേണ്ടി ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    എങ്കിൽ (fline.lvl >= 0)
		നിർത്തുക;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    എങ്കിൽ (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	  current_fdl = 0;
	linenr_T  fold_start_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	ആകുമ്പോൾ (current_fdl < fline.lvl)
	{
	    എങ്കിൽ (!foldFind(gap, lnum_rel, &fpn))
		നിർത്തുക;
	    ++current_fdl;

	    fold_start_lnum += fpn->fd_മുകൾ;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_മുകൾ;
	}
	എങ്കിൽ (fpn != ശൂന്യ && current_fdl == fline.lvl)
	{
	    linenr_T fold_end_lnum = fold_start_lnum + fpn->fd_len;

	    എങ്കിൽ (fold_end_lnum > bot)
		bot = fold_end_lnum;
	}
    }

    start = fline.lnum;
    end = bot;
    // Do at least one line.
    എങ്കിൽ (start > end && end < wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	end = start;
    ആകുമ്പോൾ (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	എങ്കിൽ (fline.lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;
	എങ്കിൽ (fline.lnum > end)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    എങ്കിൽ (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_വ്യഞ്ജകം)
		നിർത്തുക;
	    എങ്കിൽ ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_മുകൾ + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_മുകൾ < fline.lnum))
		end = fp->fd_മുകൾ + fp->fd_len - 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		end = fline.lnum;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	എങ്കിൽ (fline.lvl > 0)
	{
	    invalid_മുകൾ = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (fline.lnum == wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
		നിർത്തുക;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, start, end);

    // If some fold changed, need to redraw and position cursor.
    എങ്കിൽ (fold_changed && wp->w_p_fen)
	changed_window_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    എങ്കിൽ (end != bot)
	redraw_win_range_later(wp, മുകൾ, end);

    invalid_മുകൾ = (linenr_T)0;
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
    സ്ഥിതം linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    പൂർണ്ണസംഖ്യ		level,
    linenr_T	startlnum,
    fline_T	*flp,
    ശൂന്യം	(*getlevel)(fline_T *),
    linenr_T	bot,
    പൂർണ്ണസംഖ്യ		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ശൂന്യ;
    fold_T	*fp2;
    പൂർണ്ണസംഖ്യ		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		finish = FALSE;
    linenr_T	linecount = flp->wp->w_buffer->b_ml.ml_വരി_എണ്ണം - flp->off;
    പൂർണ്ണസംഖ്യ		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    എങ്കിൽ (getlevel == foldlevelMarker && flp->start <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(ശൂന്യം)foldFind(gap, startlnum - 1, &fp);
	എങ്കിൽ (fp != ശൂന്യ && (fp >= ((fold_T *)gap->ga_data) + gap->ga_len
						   || fp->fd_മുകൾ >= startlnum))
	    fp = ശൂന്യ;
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
    ആകുമ്പോൾ (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	വരി_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	എങ്കിൽ (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	എങ്കിൽ (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	എങ്കിൽ (flp->lnum > bot && !finish && fp != ശൂന്യ)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    എങ്കിൽ (getlevel != foldlevelMarker
		    && getlevel != foldlevel_വ്യഞ്ജകം
		    && getlevel != foldlevelSyntax)
		നിർത്തുക;
	    i = 0;
	    fp2 = fp;
	    എങ്കിൽ (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_മുകൾ;
		ആകുമ്പോൾ (foldFind(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_മുകൾ;
		}
	    }
	    എങ്കിൽ (lvl < level + i)
	    {
		(ശൂന്യം)foldFind(&fp->fd_nested, flp->lnum - fp->fd_മുകൾ, &fp2);
		എങ്കിൽ (fp2 != ശൂന്യ)
		    bot = fp2->fd_മുകൾ + fp2->fd_len - 1 + fp->fd_മുകൾ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	എങ്കിൽ (fp == ശൂന്യ
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
	    ആകുമ്പോൾ (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		എങ്കിൽ (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		അല്ലെങ്കിൽ
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		എങ്കിൽ (gap->ga_len > 0 && (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && fp->fd_മുകൾ <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_മുകൾ < flp->lnum)
				|| (lvl >= level
					   && fp->fd_മുകൾ <= flp->lnum_save)))))
		{
		    എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			എങ്കിൽ (fp->fd_മുകൾ == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_മുകൾ >= startlnum)
			{
			    എങ്കിൽ (fp->fd_മുകൾ > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(long)(fp->fd_മുകൾ - firstlnum), 0L);
			    അല്ലെങ്കിൽ
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(long)(firstlnum - fp->fd_മുകൾ - 1),
					(linenr_T)MAXLNUM,
					(long)(fp->fd_മുകൾ - firstlnum));
			    fp->fd_len += fp->fd_മുകൾ - firstlnum;
			    fp->fd_മുകൾ = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_changed = TRUE;
			}
			അല്ലെങ്കിൽ എങ്കിൽ ((flp->start != 0 && lvl == level)
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
			    എങ്കിൽ (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    അല്ലെങ്കിൽ
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_മുകൾ,
						      breakend - fp->fd_മുകൾ);
			    i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data);
			    foldSplit(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_data + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    എങ്കിൽ (getlevel == foldlevelMarker
				    || getlevel == foldlevel_വ്യഞ്ജകം
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}

			എങ്കിൽ (fp->fd_മുകൾ == startlnum && concat)
			{
			    i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data);
			    എങ്കിൽ (i != 0)
			    {
				fp2 = fp - 1;
				എങ്കിൽ (fp2->fd_മുകൾ + fp2->fd_len == fp->fd_മുകൾ)
				{
				    foldMerge(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			നിർത്തുക;
		    }
		    എങ്കിൽ (fp->fd_മുകൾ >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			deleteFoldEntry(gap,
				     (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data), TRUE);
		    }
		    അല്ലെങ്കിൽ
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_മുകൾ;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    എങ്കിൽ (gap->ga_len == 0)
			i = 0;
		    അല്ലെങ്കിൽ
			i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data);
		    എങ്കിൽ (fold_തിരുകുക(gap, i) != OK)
			തിരികെ bot;
		    fp = (fold_T *)gap->ga_data + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_മുകൾ = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    എങ്കിൽ (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (i <= 0)
		    {
			fp->fd_flags = topflags;
			എങ്കിൽ (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    അല്ലെങ്കിൽ
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    എങ്കിൽ (getlevel == foldlevelMarker
			    || getlevel == foldlevel_വ്യഞ്ജകം
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    നിർത്തുക;
		}
	    }
	}

	എങ്കിൽ (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    നിർത്തുക;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	എങ്കിൽ (lvl > level && fp != ശൂന്യ)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    എങ്കിൽ (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_മുകൾ;
	    flp->off += fp->fd_മുകൾ;
	    i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_മുകൾ, flp, getlevel,
					      bot - fp->fd_മുകൾ, fp->fd_flags);
	    fp = (fold_T *)gap->ga_data + i;
	    flp->lnum += fp->fd_മുകൾ;
	    flp->lnum_save += fp->fd_മുകൾ;
	    flp->off -= fp->fd_മുകൾ;
	    bot += fp->fd_മുകൾ;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    ആകുമ്പോൾ (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		എങ്കിൽ (++flp->lnum > linecount)
		    നിർത്തുക;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		എങ്കിൽ (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    നിർത്തുക;
	    }
	    prev_lnum = 0;
	    എങ്കിൽ (flp->lnum > linecount)
		നിർത്തുക;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    എങ്കിൽ (fp == ശൂന്യ)	// only happens when got_int is set
	തിരികെ bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    എങ്കിൽ (fp->fd_len < flp->lnum - fp->fd_മുകൾ)
    {
	fp->fd_len = flp->lnum - fp->fd_മുകൾ;
	fp->fd_small = MAYBE;
	fold_changed = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_മുകൾ + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_മുകൾ,
						  flp->lnum - 1 - fp->fd_മുകൾ);

    എങ്കിൽ (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	എങ്കിൽ (fp->fd_len != flp->lnum - fp->fd_മുകൾ)
	{
	    എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		എങ്കിൽ (getlevel == foldlevelMarker
			|| getlevel == foldlevel_വ്യഞ്ജകം
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_മുകൾ + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_മുകൾ;
		}
		അല്ലെങ്കിൽ
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data);
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_data + i;
		}
	    }
	    അല്ലെങ്കിൽ
		fp->fd_len = flp->lnum - fp->fd_മുകൾ;
	    fold_changed = TRUE;
	}
    }

    // delete following folds that end before the current line
    വേണ്ടി (;;)
    {
	fp2 = fp + 1;
	എങ്കിൽ (fp2 >= (fold_T *)gap->ga_data + gap->ga_len
						  || fp2->fd_മുകൾ > flp->lnum)
	    നിർത്തുക;
	എങ്കിൽ (fp2->fd_മുകൾ + fp2->fd_len > flp->lnum)
	{
	    എങ്കിൽ (fp2->fd_മുകൾ < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (long)(flp->lnum - fp2->fd_മുകൾ - 1),
			(linenr_T)MAXLNUM, (long)(fp2->fd_മുകൾ - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_മുകൾ;
		fp2->fd_മുകൾ = flp->lnum;
		fold_changed = TRUE;
	    }

	    എങ്കിൽ (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		foldMerge(fp, gap, fp2);
	    }
	    നിർത്തുക;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (പൂർണ്ണസംഖ്യ)(fp2 - (fold_T *)gap->ga_data), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    എങ്കിൽ (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    തിരികെ bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fold_തിരുകുക(garray_T *gap, പൂർണ്ണസംഖ്യ i)
{
    fold_T	*fp;

    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ FAIL;
    fp = (fold_T *)gap->ga_data + i;
    എങ്കിൽ (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, വലുപ്പം(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, വലുപ്പം(fold_T), 10);
    തിരികെ OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    സ്ഥിതം ശൂന്യം
foldSplit(
    garray_T	*gap,
    പൂർണ്ണസംഖ്യ		i,
    linenr_T	മുകൾ,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		len;

    // The fold continues below bot, need to split it.
    എങ്കിൽ (fold_തിരുകുക(gap, i + 1) == FAIL)
	തിരികെ;
    fp = (fold_T *)gap->ga_data + i;
    fp[1].fd_മുകൾ = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_മുകൾ - fp->fd_മുകൾ);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (ശൂന്യം)foldFind(gap1, bot + 1 - fp->fd_മുകൾ, &fp2);
    എങ്കിൽ (fp2 != ശൂന്യ)
    {
	len = (പൂർണ്ണസംഖ്യ)((fold_T *)gap1->ga_data + gap1->ga_len - fp2);
	എങ്കിൽ (len > 0 && ga_grow(gap2, len) == OK)
	{
	    വേണ്ടി (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_data)[idx] = fp2[idx];
		((fold_T *)gap2->ga_data)[idx].fd_മുകൾ
						  -= fp[1].fd_മുകൾ - fp->fd_മുകൾ;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = മുകൾ - fp->fd_മുകൾ;
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
    സ്ഥിതം ശൂന്യം
foldRemove(garray_T *gap, linenr_T മുകൾ, linenr_T bot)
{
    fold_T	*fp = ശൂന്യ;

    എങ്കിൽ (bot < മുകൾ)
	തിരികെ;		// nothing to do

    ആകുമ്പോൾ (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	എങ്കിൽ (foldFind(gap, മുകൾ, &fp) && fp->fd_മുകൾ < മുകൾ)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, മുകൾ - fp->fd_മുകൾ, bot - fp->fd_മുകൾ);
	    എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		foldSplit(gap, (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data), മുകൾ, bot);
	    }
	    അല്ലെങ്കിൽ
	    {
		// 2: truncate fold at "top".
		fp->fd_len = മുകൾ - fp->fd_മുകൾ;
	    }
	    fold_changed = TRUE;
	    തുടരുക;
	}
	എങ്കിൽ (fp >= (fold_T *)(gap->ga_data) + gap->ga_len
		|| fp->fd_മുകൾ > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    നിർത്തുക;
	}
	എങ്കിൽ (fp->fd_മുകൾ >= മുകൾ)
	{
	    // Found an entry below top.
	    fold_changed = TRUE;
	    എങ്കിൽ (fp->fd_മുകൾ + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (long)(bot - fp->fd_മുകൾ),
			(linenr_T)MAXLNUM, (long)(fp->fd_മുകൾ - bot - 1));
		fp->fd_len -= bot - fp->fd_മുകൾ + 1;
		fp->fd_മുകൾ = bot + 1;
		നിർത്തുക;
	    }

	    // 4: Delete completely contained fold.
	    deleteFoldEntry(gap, (പൂർണ്ണസംഖ്യ)(fp - (fold_T *)gap->ga_data), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    സ്ഥിതം ശൂന്യം
foldReverseOrder(garray_T *gap, linenr_T start_arg, linenr_T end_arg)
{
    fold_T *ഇടത്, *വലത്;
    fold_T tmp;
    linenr_T start = start_arg;
    linenr_T end = end_arg;

    വേണ്ടി (; start < end; start++, end--)
    {
	ഇടത് = (fold_T *)gap->ga_data + start;
	വലത് = (fold_T *)gap->ga_data + end;
	tmp  = *ഇടത്;
	*ഇടത് = *വലത്;
	*വലത് = tmp;
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

    സ്ഥിതം ശൂന്യം
truncate_fold(fold_T *fp, linenr_T end)
{
    end += 1;
    foldRemove(&fp->fd_nested, end - fp->fd_മുകൾ, MAXLNUM);
    fp->fd_len = end - fp->fd_മുകൾ;
}

# define fold_end(fp) ((fp)->fd_മുകൾ + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_data + (gap)->ga_len))
# define fold_index(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_data)))

    ശൂന്യം
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    പൂർണ്ണസംഖ്യ at_start = foldFind(gap, line1 - 1, &fp);
    size_t move_start = 0, move_end = 0, dest_index = 0;

    എങ്കിൽ (at_start)
    {
	എങ്കിൽ (fold_end(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_മുകൾ, line2 -
		    fp->fd_മുകൾ, dest - fp->fd_മുകൾ);
	    തിരികെ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (fold_end(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_മുകൾ, line2 -
		    fp->fd_മുകൾ, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	അല്ലെങ്കിൽ
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    എങ്കിൽ (!valid_fold(fp, gap) || fp->fd_മുകൾ > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fp->fd_മുകൾ > line2)
    {
	വേണ്ടി (; valid_fold(fp, gap) && fold_end(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_മുകൾ -= range_len;

	എങ്കിൽ (valid_fold(fp, gap) && fp->fd_മുകൾ <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_മുകൾ -= range_len;
	}
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fold_end(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_മുകൾ, dest -
		fp->fd_മുകൾ, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_മുകൾ += move_len;
	തിരികെ;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_start = fold_index(fp, gap);

    വേണ്ടി (; valid_fold(fp, gap) && fp->fd_മുകൾ <= dest; fp++)
    {
	എങ്കിൽ (fp->fd_മുകൾ <= line2)
	{
	    // 1. 2. or 3.
	    എങ്കിൽ (fold_end(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_മുകൾ += move_len;
	    തുടരുക;
	}

	// Record index of the first fold after the moved range.
	എങ്കിൽ (move_end == 0)
	    move_end = fold_index(fp, gap);

	എങ്കിൽ (fold_end(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_മുകൾ -= range_len;
    }

    dest_index = fold_index(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    എങ്കിൽ (move_end == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	തിരികെ;
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
    സ്ഥിതം ശൂന്യം
foldMerge(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    പൂർണ്ണസംഖ്യ		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    എങ്കിൽ (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    എങ്കിൽ (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	വേണ്ടി (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_data)[gap1->ga_len]
					= ((fold_T *)gap2->ga_data)[idx];
	    ((fold_T *)gap1->ga_data)[gap1->ga_len].fd_മുകൾ += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (പൂർണ്ണസംഖ്യ)(fp2 - (fold_T *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    സ്ഥിതം ശൂന്യം
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    എങ്കിൽ (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ശൂന്യ)
    {
	// first and last line can't be undefined, use level 0
	എങ്കിൽ (lnum == 1 || lnum == buf->b_ml.ml_വരി_എണ്ണം)
	    flp->lvl = 0;
	അല്ലെങ്കിൽ
	    flp->lvl = -1;
    }
    അല്ലെങ്കിൽ
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_മൂല്യം(buf);
    എങ്കിൽ (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	എങ്കിൽ (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    സ്ഥിതം ശൂന്യം
foldlevelDiff(fline_T *flp)
{
    എങ്കിൽ (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    അല്ലെങ്കിൽ
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    സ്ഥിതം ശൂന്യം
foldlevel_വ്യഞ്ജകം(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->start = FALSE;
    flp->lvl = 0;
# അല്ലെങ്കിൽ
    win_T	*win;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		c;
    linenr_T	lnum = flp->lnum + flp->off;
    പൂർണ്ണസംഖ്യ		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    എങ്കിൽ (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = താക്കോൽ_Typed;
    n = eval_foldexpr(flp->wp, &c);
    താക്കോൽ_Typed = save_keytyped;

    തിരഞ്ഞെടുപ്പ് (c)
    {
	// "a1", "a2", .. : add to the fold level
	അവസ്ഥ 'a': എങ്കിൽ (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  നിർത്തുക;

	// "s1", "s2", .. : subtract from the fold level
	അവസ്ഥ 's': എങ്കിൽ (flp->lvl >= 0)
		  {
		      എങ്കിൽ (n > flp->lvl)
			  flp->lvl_next = 0;
		      അല്ലെങ്കിൽ
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  നിർത്തുക;

	// ">1", ">2", .. : start a fold with a certain level
	അവസ്ഥ '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  നിർത്തുക;

	// "<1", "<2", .. : end a fold with a certain level
	അവസ്ഥ '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_next = MIN(flp->lvl, n - 1);
		  flp->end = n;
		  നിർത്തുക;

	// "=": No change in level
	അവസ്ഥ '=': flp->lvl_next = flp->lvl;
		  നിർത്തുക;

	// "-1", "0", "1", ..: set fold level
	സ്ഥിരസ്ഥിതി:  എങ്കിൽ (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_next = flp->lvl;
		  അല്ലെങ്കിൽ
		      flp->lvl_next = n;
		  flp->lvl = n;
		  നിർത്തുക;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    എങ്കിൽ (flp->lvl < 0)
    {
	എങ്കിൽ (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	എങ്കിൽ (lnum == curbuf->b_ml.ml_വരി_എണ്ണം)
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
    സ്ഥിതം ശൂന്യം
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (പൂർണ്ണസംഖ്യ)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (പൂർണ്ണസംഖ്യ)STRLEN(foldendmarker);
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
    സ്ഥിതം ശൂന്യം
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    പൂർണ്ണസംഖ്യ		cstart;
    പൂർണ്ണസംഖ്യ		cend;
    പൂർണ്ണസംഖ്യ		start_lvl = flp->lvl;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    ആകുമ്പോൾ (*s)
    {
	എങ്കിൽ (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    എങ്കിൽ (VIM_ISDIGIT(*s))
	    {
		n = atoi((അക്ഷരം *)s);
		എങ്കിൽ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    എങ്കിൽ (n <= start_lvl)
			flp->start = 1;
		    അല്ലെങ്കിൽ
			flp->start = n - start_lvl;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    എങ്കിൽ (VIM_ISDIGIT(*s))
	    {
		n = atoi((അക്ഷരം *)s);
		എങ്കിൽ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    // never start a fold with an end marker
		    എങ്കിൽ (flp->lvl_next > start_lvl)
			flp->lvl_next = start_lvl;
		}
	    }
	    അല്ലെങ്കിൽ
		--flp->lvl_next;
	}
	അല്ലെങ്കിൽ
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    എങ്കിൽ (flp->lvl_next < 0)
	flp->lvl_next = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    സ്ഥിതം ശൂന്യം
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->start = 0;
    flp->lvl = 0;
# അല്ലെങ്കിൽ
    linenr_T	lnum = flp->lnum + flp->off;
    പൂർണ്ണസംഖ്യ		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = 0;
    എങ്കിൽ (lnum < flp->wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	എങ്കിൽ (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# എങ്കിൽ defined(FEAT_SESSION)
സ്ഥിതം പൂർണ്ണസംഖ്യ put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
സ്ഥിതം പൂർണ്ണസംഖ്യ put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
സ്ഥിതം പൂർണ്ണസംഖ്യ put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    പൂർണ്ണസംഖ്യ
put_folds(FILE *fd, win_T *wp)
{
    എങ്കിൽ (foldmethodIsManual(wp))
    {
	എങ്കിൽ (put_വരി(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_വരി(fd, "&fdl = &fdl") == FAIL)
	    തിരികെ FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    എങ്കിൽ (wp->w_fold_manual)
	തിരികെ put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    തിരികെ OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    പൂർണ്ണസംഖ്യ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    വേണ്ടി (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	എങ്കിൽ (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_മുകൾ) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_മുകൾ + off,
					fp->fd_മുകൾ + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    തിരികെ FAIL;
	++fp;
    }
    തിരികെ OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_data;
    വേണ്ടി (i = 0; i < gap->ga_len; i++)
    {
	എങ്കിൽ (fp->fd_flags != FD_LEVEL)
	{
	    എങ്കിൽ (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		എങ്കിൽ (fprintf(fd, ":%ld", fp->fd_മുകൾ + off) < 0
			|| put_eol(fd) == FAIL
			|| put_വരി(fd, "sil! normal! zo") == FAIL)
		    തിരികെ FAIL;
		എങ്കിൽ (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_മുകൾ)
			== FAIL)
		    തിരികെ FAIL;
		// close the parent when needed
		എങ്കിൽ (fp->fd_flags == FD_CLOSED)
		{
		    എങ്കിൽ (put_fold_open_close(fd, fp, off) == FAIL)
			തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_മുകൾ);
		എങ്കിൽ ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		എങ്കിൽ (put_fold_open_close(fd, fp, off) == FAIL)
		    തിരികെ FAIL;
	    }
	}
	++fp;
    }

    തിരികെ OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    എങ്കിൽ (fprintf(fd, ":%ld", fp->fd_മുകൾ + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}
# endif // FEAT_SESSION

// }}}1
#endif // defined(FEAT_FOLDING)

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    സ്ഥിതം ശൂന്യം
foldclosed_both(
    typval_T	*argvars UNUSED,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		end UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	first, last;
    win_T	*wp  = curwin;

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_സംഖ്യ(&argvars[1]), &tp);
	എങ്കിൽ (wp != ശൂന്യ && tp != ശൂന്യ)
	{
	    എങ്കിൽ (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	അല്ലെങ്കിൽ
	{
	    rettv->vval.v_സംഖ്യ = -2;
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	lnum = tv_get_lnum(argvars);

    എങ്കിൽ (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
    {
	എങ്കിൽ (hasFoldingWin(wp, lnum, &first, &last, FALSE, ശൂന്യ))
	{
	    എങ്കിൽ (end)
		rettv->vval.v_സംഖ്യ = (varnumber_T)last;
	    അല്ലെങ്കിൽ
		rettv->vval.v_സംഖ്യ = (varnumber_T)first;
	    തിരികെ;
	}
    }
# endif
    rettv->vval.v_സംഖ്യ = -1;
}

/*
 * "foldclosed()" function
 */
    ശൂന്യം
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    ശൂന്യം
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    എങ്കിൽ (invalid_മുകൾ == (linenr_T)0)
	checkupdate(curwin);
    അല്ലെങ്കിൽ എങ്കിൽ (lnum == prev_lnum && prev_lnum_lvl >= 0)
	തിരികെ prev_lnum_lvl;
    അല്ലെങ്കിൽ എങ്കിൽ (lnum >= invalid_മുകൾ && lnum <= invalid_bot)
	തിരികെ -1;

    // Return quickly when there is no folding at all in this window.
    എങ്കിൽ (!hasAnyFolding(curwin))
	തിരികെ 0;

    തിരികെ foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    ശൂന്യം
f_foldlevel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_സംഖ്യ(&argvars[1]), &tp);
	എങ്കിൽ (wp != ശൂന്യ && tp != ശൂന്യ)
	{
	    എങ്കിൽ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
		    rettv->vval.v_സംഖ്യ = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	അല്ലെങ്കിൽ
	    rettv->vval.v_സംഖ്യ = -2;
    }
    അല്ലെങ്കിൽ
    {
	lnum = tv_get_lnum(argvars);
	എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
	    rettv->vval.v_സംഖ്യ = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    ശൂന്യം
f_foldtext(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    പൂർണ്ണസംഖ്യ		len;
    അക്ഷരം	*txt;
    long	എണ്ണം;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    എങ്കിൽ (foldstart > 0 && foldend <= curbuf->b_ml.ml_വരി_എണ്ണം
	    && dashes != ശൂന്യ)
    {
	// Find first non-empty line in the fold.
	വേണ്ടി (lnum = foldstart; lnum < foldend; ++lnum)
	    എങ്കിൽ (!linewhite(lnum))
		നിർത്തുക;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	എങ്കിൽ (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    എങ്കിൽ (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		എങ്കിൽ (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	എണ്ണം = (long)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", എണ്ണം);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	എങ്കിൽ (r != ശൂന്യ)
	{
	    sprintf((അക്ഷരം *)r, txt, dashes, എണ്ണം);
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(r);
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
    ശൂന്യം
f_foldtextresult(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*text;
    char_u	buf[FOLD_TEXT_LEN];
    foldinfo_T  foldinfo;
    പൂർണ്ണസംഖ്യ		fold_എണ്ണം;
    win_T	*wp  = curwin;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	entered = FALSE;
    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (entered)
	തിരികെ; // reject recursive use
    entered = TRUE;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_സംഖ്യ(&argvars[1]), &tp);
	എങ്കിൽ (wp == ശൂന്യ || tp == ശൂന്യ) // wrong winid
	{
	    entered = FALSE;
	    തിരികെ;
	}
	എങ്കിൽ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    അല്ലെങ്കിൽ
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    എങ്കിൽ (lnum < 0)
	lnum = 0;
    fold_എണ്ണം = folded_എണ്ണം(wp, lnum, &foldinfo);
    എങ്കിൽ (fold_എണ്ണം > 0)
    {
	text = get_foldtext(wp, lnum, lnum + fold_എണ്ണം - 1,
							       &foldinfo, buf);
	എങ്കിൽ (text == buf)
	    text = vim_strsave(text);
	rettv->vval.v_string = text;
    }

    entered = FALSE;
# endif
}

#endif // FEAT_EVAL
