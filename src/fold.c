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

#شامل "vim.h"

#اگر defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
قسمتعریف ساخت
{
    linenr_T	fd_چوٹی;		// first line of fold; for nested fold
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
ساکن خالی newFoldLevelWin(win_T *wp);
ساکن صحیح checkCloseRec(garray_T *gap, linenr_T lnum, صحیح level);
ساکن صحیح fold_ڈھونڈو(garray_T *gap, linenr_T lnum, fold_T **fpp);
ساکن صحیح foldLevelWin(win_T *wp, linenr_T lnum);
ساکن خالی checkupdate(win_T *wp);
ساکن خالی سیٹ_Fold_Repeat(linenr_T lnum, طویل گنتی, صحیح do_open);
ساکن linenr_T سیٹ_Manual_Fold(linenr_T lnum, صحیح opening, صحیح recurse, صحیح *donep);
ساکن linenr_T سیٹ_Manual_Fold_Win(win_T *wp, linenr_T lnum, صحیح opening, صحیح recurse, صحیح *donep);
ساکن خالی foldOpenNested(fold_T *fpr);
ساکن خالی delete_Fold_اندراج(garray_T *gap, صحیح idx, صحیح recursive);
ساکن خالی foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, طویل amount, طویل amount_after);
ساکن صحیح get_گہرا_ترین_Nesting_Recurse(garray_T *gap);
ساکن صحیح check_closed(win_T *win, fold_T *fp, صحیح *use_levelp, صحیح level, صحیح *maybe_smallp, linenr_T lnum_off);
ساکن خالی checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
ساکن خالی سیٹ_Small_Maybe(garray_T *gap);
ساکن خالی foldCreateMarkers(linenr_T آغاز, linenr_T انتہا);
ساکن خالی fold_جوڑو_Marker(linenr_T lnum, char_u *marker, صحیح markerlen);
ساکن خالی deleteFoldMarkers(fold_T *fp, صحیح recursive, linenr_T lnum_off);
ساکن خالی foldDelMarker(linenr_T lnum, char_u *marker, صحیح markerlen);
ساکن خالی foldUpdateIEMS(win_T *wp, linenr_T چوٹی, linenr_T bot);
ساکن خالی parseMarker(win_T *wp);

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
ساکن linenr_T invalid_چوٹی = (linenr_T)0;
ساکن linenr_T invalid_bot = (linenr_T)0;

/*
 * When using 'foldexpr' we sometimes get the level of the next line, which
 * calls foldlevel() to get the level of the current line, which hasn't been
 * stored yet.  To get around this chicken-egg problem the level of the
 * previous line is stored here when available.  prev_lnum is zero when the
 * level is not available.
 */
ساکن linenr_T پچھلا_lnum = 0;
ساکن صحیح پچھلا_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define مکمل_NOTHING	0
# define مکمل_ACTION	1	// did close or open a fold
# define مکمل_FOLD	2	// did find a fold

ساکن صحیح foldstartmarkerlen;
ساکن char_u *foldendmarker;
ساکن صحیح foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    خالی
copyFoldingState(win_T *wp_from, win_T *wp_مقصد)
{
    wp_مقصد->w_fold_manual = wp_from->w_fold_manual;
    wp_مقصد->w_foldinvalid = wp_from->w_foldinvalid;
    cloneFoldGrowArray(&wp_from->w_folds, &wp_مقصد->w_folds);
}

// hasAnyFolding() {{{2
/*
 * Return TRUE if there may be folded lines in window "win".
 */
    صحیح
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    واپس (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    صحیح
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    واپس hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, تہی);
}

// hasFoldingWin() {{{2
    صحیح
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    صحیح		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    صحیح		had_folded = FALSE;
    linenr_T	پہلا = 0;
    linenr_T	آخری = 0;
    linenr_T	lnum_rel = lnum;
    صحیح		x;
    fold_T	*fp;
    صحیح		level = 0;
    صحیح		use_level = FALSE;
    صحیح		maybe_small = FALSE;
    garray_T	*gap;
    صحیح		کم_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    اگر (!hasAnyFolding(win))
    {
	اگر (infop != تہی)
	    infop->fi_level = 0;
	واپس FALSE;
    }

    اگر (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = ڈھونڈو_wl_اندراج(win, lnum);
	اگر (x >= 0)
	{
	    پہلا = win->w_سطریں[x].wl_lnum;
	    آخری = win->w_سطریں[x].wl_lastlnum;
	    had_folded = win->w_سطریں[x].wl_folded;
	}
    }

    اگر (پہلا == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	برائے (;;)
	{
	    اگر (!fold_ڈھونڈو(gap, lnum_rel, &fp))
		توڑو;

	    // Remember lowest level of fold that starts in "lnum".
	    اگر (lnum_rel == fp->fd_چوٹی && کم_level == 0)
		کم_level = level + 1;

	    پہلا += fp->fd_چوٹی;
	    آخری += fp->fd_چوٹی;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    اگر (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		آخری += fp->fd_len - 1;
		توڑو;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_چوٹی;
	    ++level;
	}
    }

    اگر (!had_folded)
    {
	اگر (infop != تہی)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_کم_level = کم_level == 0 ? level : کم_level;
	}
	واپس FALSE;
    }

    اگر (آخری > win->w_بفر->b_ml.ml_سطر_گنتی)
	آخری = win->w_بفر->b_ml.ml_سطر_گنتی;
    اگر (lastp != تہی)
	*lastp = آخری;
    اگر (firstp != تہی)
	*firstp = پہلا;
    اگر (infop != تہی)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = پہلا;
	infop->fi_کم_level = کم_level == 0 ? level + 1 : کم_level;
    }
    واپس TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    صحیح
سطر_Folded(win_T *win, linenr_T lnum)
{
    واپس folded_گنتی(win, lnum, تہی) != 0;
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
    طویل
folded_گنتی(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	آخری;

    اگر (hasFoldingWin(win, lnum, تہی, &آخری, FALSE, infop))
	واپس (طویل)(آخری - lnum + 1);
    واپس 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    صحیح
foldmethodIsManual(win_T *wp)
{
    واپس (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    صحیح
foldmethodIsIndent(win_T *wp)
{
    واپس (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    صحیح
foldmethod_Is_اظہاریہ(win_T *wp)
{
    واپس (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    صحیح
foldmethodIsMarker(win_T *wp)
{
    واپس (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    صحیح
foldmethodIsSyntax(win_T *wp)
{
    واپس (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    صحیح
foldmethodIsDiff(win_T *wp)
{
    واپس (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    خالی
closeFold(linenr_T lnum, طویل گنتی)
{
    سیٹ_Fold_Repeat(lnum, گنتی, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    خالی
closeFoldRecurse(linenr_T lnum)
{
    (خالی)سیٹ_Manual_Fold(lnum, FALSE, TRUE, تہی);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    خالی
opFoldRange(
    linenr_T	پہلا,
    linenr_T	آخری,
    صحیح		opening,	// TRUE to open, FALSE to close
    صحیح		recurse,	// TRUE to do it recursively
    صحیح		had_visual)	// TRUE when Visual selection used
{
    صحیح		مکمل = مکمل_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_اگلا;

    برائے (lnum = پہلا; lnum <= آخری; lnum = lnum_اگلا + 1)
    {
	lnum_اگلا = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	اگر (opening && !recurse)
	    (خالی)hasFolding(lnum, تہی, &lnum_اگلا);
	(خالی)سیٹ_Manual_Fold(lnum, opening, recurse, &مکمل);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	اگر (!opening && !recurse)
	    (خالی)hasFolding(lnum, تہی, &lnum_اگلا);
    }
    اگر (مکمل == مکمل_NOTHING)
	emsg(_(e_no_fold_ملا));
    // Force a redraw to remove the Visual highlighting.
    اگر (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    خالی
openFold(linenr_T lnum, طویل گنتی)
{
    سیٹ_Fold_Repeat(lnum, گنتی, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    خالی
openFoldRecurse(linenr_T lnum)
{
    (خالی)سیٹ_Manual_Fold(lnum, TRUE, TRUE, تہی);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    خالی
foldOpenCursor(خالی)
{
    صحیح		مکمل;

    checkupdate(curwin);
    اگر (hasAnyFolding(curwin))
	برائے (;;)
	{
	    مکمل = مکمل_NOTHING;
	    (خالی)سیٹ_Manual_Fold(curwin->w_cursor.lnum, TRUE, FALSE, &مکمل);
	    اگر (!(مکمل & مکمل_ACTION))
		توڑو;
	}
}

// newFoldLevel() {{{2
/*
 * Set new foldlevel for current window.
 */
    خالی
newFoldLevel(خالی)
{
    newFoldLevelWin(curwin);

# ifdef FEAT_DIFF
    اگر (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		wp->w_p_fdl = curwin->w_p_fdl;
		newFoldLevelWin(wp);
	    }
	}
    }
# endif
}

    ساکن خالی
newFoldLevelWin(win_T *wp)
{
    fold_T	*fp;
    صحیح		i;

    checkupdate(wp);
    اگر (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_ڈیٹا;
	برائے (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    بدلا_کھڑکی_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    خالی
foldCheckClose(خالی)
{
    اگر (*p_fcl == NUL)
	واپس;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    اگر (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(صحیح)curwin->w_p_fdl))
	بدلا_کھڑکی_setting();
}

// checkCloseRec() {{{2
    ساکن صحیح
checkCloseRec(garray_T *gap, linenr_T lnum, صحیح level)
{
    fold_T	*fp;
    صحیح		retval = FALSE;
    صحیح		i;

    fp = (fold_T *)gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	اگر (fp[i].fd_flags == FD_OPEN)
	{
	    اگر (level <= 0 && (lnum < fp[i].fd_چوٹی
				      || lnum >= fp[i].fd_چوٹی + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    ورنہ
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_چوٹی,
								   level - 1);
	}
    }
    واپس retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    صحیح
foldManualAllowed(صحیح create)
{
    اگر (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	واپس TRUE;
    اگر (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    ورنہ
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    واپس FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    خالی
foldCreate(linenr_T آغاز, linenr_T انتہا)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    صحیح		i, j;
    صحیح		cont;
    صحیح		use_level = FALSE;
    صحیح		closed = FALSE;
    صحیح		level = 0;
    linenr_T	آغاز_rel = آغاز;
    linenr_T	انتہا_rel = انتہا;

    اگر (آغاز > انتہا)
    {
	// reverse the range
	انتہا = آغاز_rel;
	آغاز = انتہا_rel;
	آغاز_rel = آغاز;
	انتہا_rel = انتہا;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    اگر (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(آغاز, انتہا);
	واپس;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    اگر (gap->ga_len == 0)
	i = 0;
    ورنہ
    {
	برائے (;;)
	{
	    اگر (!fold_ڈھونڈو(gap, آغاز_rel, &fp))
		توڑو;
	    اگر (fp->fd_چوٹی + fp->fd_len > انتہا_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		آغاز_rel -= fp->fd_چوٹی;
		انتہا_rel -= fp->fd_چوٹی;
		اگر (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    اگر (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		ورنہ اگر (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    ورنہ
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		توڑو;
	    }
	}
	اگر (gap->ga_len == 0)
	    i = 0;
	ورنہ
	    i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا);
    }

    اگر (ga_grow(gap, 1) == FAIL)
	واپس;

    fp = (fold_T *)gap->ga_ڈیٹا + i;
    ga_init2(&fold_ga, حجم(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    برائے (cont = 0; i + cont < gap->ga_len; ++cont)
	اگر (fp[cont].fd_چوٹی > انتہا_rel)
	    توڑو;
    اگر (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	اگر (آغاز_rel > fp->fd_چوٹی)
	    آغاز_rel = fp->fd_چوٹی;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	اگر (انتہا_rel < fp[cont - 1].fd_چوٹی + fp[cont - 1].fd_len - 1)
	    انتہا_rel = fp[cont - 1].fd_چوٹی + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_ڈیٹا, fp, حجم(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	برائے (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_ڈیٹا)[j].fd_چوٹی -= آغاز_rel;
    }
    // Move remaining entries to after the new fold.
    اگر (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_ڈیٹا + i,
		حجم(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_چوٹی = آغاز_rel;
    fp->fd_len = انتہا_rel - آغاز_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    اگر (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(آغاز, 1L);
    اگر (!use_level)
	curwin->w_fold_manual = true;
    fp->fd_flags = FD_CLOSED;
    fp->fd_small = MAYBE;

    // redraw
    بدلا_کھڑکی_setting();
}

// deleteFold() {{{2
/*
 * Delete a fold at line "start" in the current window.
 * When "end" is not 0, delete all folds from "start" to "end".
 * When "recursive" is TRUE delete recursively.
 */
    خالی
deleteFold(
    linenr_T	آغاز,
    linenr_T	انتہا,
    صحیح		recursive,
    صحیح		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*ملا_ga;
    fold_T	*ملا_fp = تہی;
    linenr_T	ملا_off = 0;
    صحیح		use_level;
    صحیح		maybe_small = FALSE;
    صحیح		level = 0;
    linenr_T	lnum = آغاز;
    linenr_T	lnum_off;
    صحیح		did_one = FALSE;
    linenr_T	پہلا_lnum = MAXLNUM;
    linenr_T	آخری_lnum = 0;

    checkupdate(curwin);

    جب (lnum <= انتہا)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	ملا_ga = تہی;
	lnum_off = 0;
	use_level = FALSE;
	برائے (;;)
	{
	    اگر (!fold_ڈھونڈو(gap, lnum - lnum_off, &fp))
		توڑو;
	    // lnum is inside this fold, remember info
	    ملا_ga = gap;
	    ملا_fp = fp;
	    ملا_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    اگر (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		توڑو;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_چوٹی;
	    ++level;
	}
	اگر (ملا_ga == تہی)
	{
	    ++lnum;
	}
	ورنہ
	{
	    lnum = ملا_fp->fd_چوٹی + ملا_fp->fd_len + ملا_off;

	    اگر (foldmethodIsManual(curwin))
		delete_Fold_اندراج(ملا_ga,
		    (صحیح)(ملا_fp - (fold_T *)ملا_ga->ga_ڈیٹا), recursive);
	    ورنہ
	    {
		اگر (پہلا_lnum > ملا_fp->fd_چوٹی + ملا_off)
		    پہلا_lnum = ملا_fp->fd_چوٹی + ملا_off;
		اگر (آخری_lnum < lnum)
		    آخری_lnum = lnum;
		اگر (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(ملا_fp, recursive, ملا_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    بدلا_کھڑکی_setting();
	}
    }
    اگر (!did_one)
    {
	emsg(_(e_no_fold_ملا));
	// Force a redraw to remove the Visual highlighting.
	اگر (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    ورنہ
	// Deleting markers may make cursor column invalid.
	check_cursor_ستون();

    اگر (آخری_lnum > 0)
	بدلا_سطریں(پہلا_lnum, (colnr_T)0, آخری_lnum, 0L);
}

// clearFolding() {{{2
/*
 * Remove all folding for window "win".
 */
    خالی
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
    خالی
foldUpdate(win_T *wp, linenr_T چوٹی, linenr_T bot)
{
    fold_T	*fp;

    اگر (disable_fold_update > 0)
	واپس;
# ifdef FEAT_DIFF
    اگر (need_diff_redraw)
	// will update later
	واپس;
# endif

    اگر (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_آغاز = چوٹی;
	linenr_T	maybe_small_انتہا = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	اگر (چوٹی > bot)
	{
	    maybe_small_آغاز = bot;
	    maybe_small_انتہا = چوٹی;
	}
	(خالی)fold_ڈھونڈو(&wp->w_folds, maybe_small_آغاز, &fp);
	جب (fp < (fold_T *)wp->w_folds.ga_ڈیٹا + wp->w_folds.ga_len
		&& fp->fd_چوٹی <= maybe_small_انتہا)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    اگر (foldmethodIsIndent(wp)
	    || foldmethod_Is_اظہاریہ(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	صحیح save_got_int = got_int;

	// reset got_int here, otherwise it won't work
	got_int = FALSE;
	foldUpdateIEMS(wp, چوٹی, bot);
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
    خالی
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
    صحیح
fold_Move_مقصد(
    صحیح		updown,
    صحیح		dir,	    // FORWARD or BACKWARD
    طویل	گنتی)
{
    طویل	n;
    صحیح		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_ملا;
    linenr_T	lnum;
    صحیح		use_level;
    صحیح		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    صحیح		level;
    صحیح		آخری;

    checkupdate(curwin);

    // Repeat "count" times.
    برائے (n = 0; n < گنتی; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	اگر (gap->ga_len == 0)
	    توڑو;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_ملا = curwin->w_cursor.lnum;
	level = 0;
	آخری = FALSE;
	برائے (;;)
	{
	    اگر (!fold_ڈھونڈو(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		اگر (!updown || gap->ga_len == 0)
		    توڑو;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		اگر (dir == FORWARD)
		{
		    اگر (fp - (fold_T *)gap->ga_ڈیٹا >= gap->ga_len)
			توڑو;
		    --fp;
		}
		ورنہ
		{
		    اگر (fp == (fold_T *)gap->ga_ڈیٹا)
			توڑو;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		آخری = TRUE;
	    }

	    اگر (!آخری)
	    {
		// Check if this fold is closed.
		اگر (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    آخری = TRUE;

		// "[z" and "]z" stop at closed fold
		اگر (آخری && !updown)
		    توڑو;
	    }

	    اگر (updown)
	    {
		اگر (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    اگر (fp + 1 - (fold_T *)gap->ga_ڈیٹا < gap->ga_len)
		    {
			lnum = fp[1].fd_چوٹی + lnum_off;
			اگر (lnum > curwin->w_cursor.lnum)
			    lnum_ملا = lnum;
		    }
		}
		ورنہ
		{
		    // to end of previous fold if there is one
		    اگر (fp > (fold_T *)gap->ga_ڈیٹا)
		    {
			lnum = fp[-1].fd_چوٹی + lnum_off + fp[-1].fd_len - 1;
			اگر (lnum < curwin->w_cursor.lnum)
			    lnum_ملا = lnum;
		    }
		}
	    }
	    ورنہ
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		اگر (dir == FORWARD)
		{
		    lnum = fp->fd_چوٹی + lnum_off + fp->fd_len - 1;
		    اگر (lnum > curwin->w_cursor.lnum)
			lnum_ملا = lnum;
		}
		ورنہ
		{
		    lnum = fp->fd_چوٹی + lnum_off;
		    اگر (lnum < curwin->w_cursor.lnum)
			lnum_ملا = lnum;
		}
	    }

	    اگر (آخری)
		توڑو;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_چوٹی;
	    ++level;
	}
	اگر (lnum_ملا != curwin->w_cursor.lnum)
	{
	    اگر (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_ملا;
	    curwin->w_cursor.ستون = 0;
	    retval = OK;
	}
	ورنہ
	    توڑو;
    }

    واپس retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    خالی
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
    صحیح
ڈھونڈو_wl_اندراج(win_T *win, linenr_T lnum)
{
    صحیح		i;

    برائے (i = 0; i < win->w_سطریں_valid; ++i)
	اگر (win->w_سطریں[i].wl_valid)
	{
	    اگر (lnum < win->w_سطریں[i].wl_lnum)
		واپس -1;
	    اگر (lnum <= win->w_سطریں[i].wl_lastlnum)
		واپس i;
	}
    واپس -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    خالی
foldAdjustVisual(خالی)
{
    pos_T	*آغاز, *انتہا;

    اگر (!VIsual_فعال || !hasAnyFolding(curwin))
	واپس;

    اگر (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	آغاز = &VIsual;
	انتہا = &curwin->w_cursor;
    }
    ورنہ
    {
	آغاز = &curwin->w_cursor;
	انتہا = &VIsual;
    }
    اگر (hasFolding(آغاز->lnum, &آغاز->lnum, تہی))
	آغاز->ستون = 0;

    اگر (!hasFolding(انتہا->lnum, تہی, &انتہا->lnum))
	واپس;

    انتہا->ستون = ml_get_len(انتہا->lnum);
    اگر (انتہا->ستون > 0 && *p_sel == 'o')
	--انتہا->ستون;
    // prevent cursor from moving on the trail byte
    اگر (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    خالی
foldAdjustCursor(خالی)
{
    (خالی)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, تہی);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    خالی
cloneFoldGrowArray(garray_T *from, garray_T *مقصد)
{
    صحیح		i;
    fold_T	*from_p;
    fold_T	*مقصد_p;

    ga_init2(مقصد, from->ga_itemsize, from->ga_growsize);
    اگر (from->ga_len == 0 || ga_grow(مقصد, from->ga_len) == FAIL)
	واپس;

    from_p = (fold_T *)from->ga_ڈیٹا;
    مقصد_p = (fold_T *)مقصد->ga_ڈیٹا;

    برائے (i = 0; i < from->ga_len; i++)
    {
	مقصد_p->fd_چوٹی = from_p->fd_چوٹی;
	مقصد_p->fd_len = from_p->fd_len;
	مقصد_p->fd_flags = from_p->fd_flags;
	مقصد_p->fd_small = from_p->fd_small;
	cloneFoldGrowArray(&from_p->fd_nested, &مقصد_p->fd_nested);
	++مقصد->ga_len;
	++from_p;
	++مقصد_p;
    }
}

// foldFind() {{{2
/*
 * Search for line "lnum" in folds of growarray "gap".
 * Set "*fpp" to the fold struct for the fold that contains "lnum" or
 * the first fold below it (careful: it can be beyond the end of the array!).
 * Returns FALSE when there is no fold that contains "lnum".
 */
    ساکن صحیح
fold_ڈھونڈو(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	کم, بلند;
    fold_T	*fp;
    صحیح		i;

    اگر (gap->ga_len == 0)
    {
	*fpp = تہی;
	واپس FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_ڈیٹا;
    کم = 0;
    بلند = gap->ga_len - 1;
    جب (کم <= بلند)
    {
	i = (کم + بلند) / 2;
	اگر (fp[i].fd_چوٹی > lnum)
	    // fold below lnum, adjust high
	    بلند = i - 1;
	ورنہ اگر (fp[i].fd_چوٹی + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    کم = i + 1;
	ورنہ
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    واپس TRUE;
	}
    }
    *fpp = fp + کم;
    واپس FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    ساکن صحیح
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    صحیح		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    برائے (;;)
    {
	اگر (!fold_ڈھونڈو(gap, lnum_rel, &fp))
	    توڑو;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_چوٹی;
	++level;
    }

    واپس level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    ساکن خالی
checkupdate(win_T *wp)
{
    اگر (!wp->w_foldinvalid)
	واپس;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ساکن خالی
سیٹ_Fold_Repeat(linenr_T lnum, طویل گنتی, صحیح do_open)
{
    صحیح		مکمل;
    طویل	n;

    برائے (n = 0; n < گنتی; ++n)
    {
	مکمل = مکمل_NOTHING;
	(خالی)سیٹ_Manual_Fold(lnum, do_open, FALSE, &مکمل);
	اگر (!(مکمل & مکمل_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    اگر (n == 0 && !(مکمل & مکمل_FOLD))
		emsg(_(e_no_fold_ملا));
	    توڑو;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    ساکن linenr_T
سیٹ_Manual_Fold(
    linenr_T	lnum,
    صحیح		opening,    // TRUE when opening, FALSE when closing
    صحیح		recurse,    // TRUE when closing/opening recursive
    صحیح		*donep)
{
# ifdef FEAT_DIFF
    اگر (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		اگر (dlnum != 0)
		    (خالی)سیٹ_Manual_Fold_Win(wp, dlnum, opening, recurse, تہی);
	    }
	}
    }
# endif

    واپس سیٹ_Manual_Fold_Win(curwin, lnum, opening, recurse, donep);
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
    ساکن linenr_T
سیٹ_Manual_Fold_Win(
    win_T	*wp,
    linenr_T	lnum,
    صحیح		opening,    // TRUE when opening, FALSE when closing
    صحیح		recurse,    // TRUE when closing/opening recursive
    صحیح		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*ملا = تہی;
    صحیح		j;
    صحیح		level = 0;
    صحیح		use_level = FALSE;
    صحیح		ملا_fold = FALSE;
    garray_T	*gap;
    linenr_T	اگلا = MAXLNUM;
    linenr_T	off = 0;
    صحیح		مکمل = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    برائے (;;)
    {
	اگر (!fold_ڈھونڈو(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    اگر (fp != تہی && fp < (fold_T *)gap->ga_ڈیٹا + gap->ga_len)
		اگلا = fp->fd_چوٹی + off;
	    توڑو;
	}

	// lnum is inside this fold
	ملا_fold = TRUE;

	// If there is a following fold, continue there next time.
	اگر (fp + 1 < (fold_T *)gap->ga_ڈیٹا + gap->ga_len)
	    اگلا = fp[1].fd_چوٹی + off;

	// Change from level-dependent folding to manual.
	اگر (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    اگر (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    ورنہ
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_ڈیٹا;
	    برائے (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	اگر (!opening && recurse)
	{
	    اگر (fp->fd_flags != FD_CLOSED)
	    {
		مکمل |= مکمل_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	ورنہ اگر (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    اگر (opening)
	    {
		fp->fd_flags = FD_OPEN;
		مکمل |= مکمل_ACTION;
		اگر (recurse)
		    foldOpenNested(fp);
	    }
	    توڑو;
	}

	// fold is open, check nested folds
	ملا = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_چوٹی;
	off += fp->fd_چوٹی;
	++level;
    }
    اگر (ملا_fold)
    {
	// When closing and not recurse, close deepest open fold.
	اگر (!opening && ملا != تہی)
	{
	    ملا->fd_flags = FD_CLOSED;
	    مکمل |= مکمل_ACTION;
	}
	wp->w_fold_manual = true;
	اگر (مکمل & مکمل_ACTION)
	    بدلا_کھڑکی_setting_win(wp);
	مکمل |= مکمل_FOLD;
    }
    ورنہ اگر (donep == تہی && wp == curwin)
	emsg(_(e_no_fold_ملا));

    اگر (donep != تہی)
	*donep |= مکمل;

    واپس اگلا;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    ساکن خالی
foldOpenNested(fold_T *fpr)
{
    صحیح		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_ڈیٹا;
    برائے (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
    ساکن خالی
delete_Fold_اندراج(garray_T *gap, صحیح idx, صحیح recursive)
{
    fold_T	*fp;
    صحیح		i;
    طویل	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_ڈیٹا + idx;
    اگر (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	اگر (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, حجم(fold_T) * (gap->ga_len - idx));
    }
    ورنہ
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	اگر (ga_grow(gap, (صحیح)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_ڈیٹا + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_ڈیٹا;
	    برائے (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_چوٹی += fp->fd_چوٹی;
		اگر (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		اگر (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    اگر (idx + 1 < gap->ga_len)
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
    خالی
deleteFoldRecurse(garray_T *gap)
{
    صحیح		i;

    برائے (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_T *)(gap->ga_ڈیٹا))[i].fd_nested));
    ga_clear(gap);
}

// foldMarkAdjust() {{{2
/*
 * Update line numbers of folds for inserted/deleted lines.
 *
 * We are adjusting the folds in the range from line1 til line2,
 * make sure that line2 does not get smaller than line1
 */
    خالی
foldMarkAdjust(
    win_T	*wp,
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    اگر (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    اگر (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    اگر ((State & منوال_داخل_کرو) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    ساکن خالی
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after)
{
    fold_T	*fp;
    صحیح		i;
    linenr_T	آخری;
    linenr_T	چوٹی;

    اگر (gap->ga_len == 0)
	واپس;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    اگر ((State & منوال_داخل_کرو) && amount == (linenr_T)1 && line2 == MAXLNUM)
	چوٹی = line1 + 1;
    ورنہ
	چوٹی = line1;

    // Find the fold containing or just below "line1".
    (خالی)fold_ڈھونڈو(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    برائے (i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا); i < gap->ga_len; ++i, ++fp)
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

	آخری = fp->fd_چوٹی + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	اگر (آخری < line1)
	    جاری;

	// 6. fold below line2: only adjust for amount_after
	اگر (fp->fd_چوٹی > line2)
	{
	    اگر (amount_after == 0)
		توڑو;
	    fp->fd_چوٹی += amount_after;
	}
	ورنہ
	{
	    اگر (fp->fd_چوٹی >= چوٹی && آخری <= line2)
	    {
		// 4. fold completely contained in range
		اگر (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    delete_Fold_اندراج(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		ورنہ
		    fp->fd_چوٹی += amount;
	    }
	    ورنہ
	    {
		اگر (fp->fd_چوٹی < چوٹی)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_چوٹی,
				  line2 - fp->fd_چوٹی, amount, amount_after);
		    اگر (آخری <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			اگر (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_چوٹی;
			ورنہ
			    fp->fd_len += amount;
		    }
		    ورنہ
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		ورنہ
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    اگر (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_چوٹی,
				  amount,
				  amount_after + (fp->fd_چوٹی - چوٹی));
			fp->fd_len -= line2 - fp->fd_چوٹی + 1;
			fp->fd_چوٹی = line1;
		    }
		    ورنہ
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_چوٹی,
				  amount,
				  amount_after - amount);
			fp->fd_len += amount_after - amount;
			fp->fd_چوٹی += amount;
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
    صحیح
get_گہرا_ترین_Nesting(خالی)
{
    checkupdate(curwin);
    واپس get_گہرا_ترین_Nesting_Recurse(&curwin->w_folds);
}

    ساکن صحیح
get_گہرا_ترین_Nesting_Recurse(garray_T *gap)
{
    صحیح		i;
    صحیح		level;
    صحیح		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	level = get_گہرا_ترین_Nesting_Recurse(&fp[i].fd_nested) + 1;
	اگر (level > maxlevel)
	    maxlevel = level;
    }

    واپس maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    ساکن صحیح
check_closed(
    win_T	*win,
    fold_T	*fp,
    صحیح		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    صحیح		level,		    // folding depth
    صحیح		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    صحیح		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    اگر (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	اگر (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    ورنہ اگر (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    اگر (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    اگر (closed)
    {
	اگر (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	اگر (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    واپس closed;
}

// checkSmall() {{{2
/*
 * Update fd_small field of fold "fp".
 */
    ساکن خالی
checkSmall(
    win_T	*wp,
    fold_T	*fp,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    صحیح		گنتی;
    صحیح		n;

    اگر (fp->fd_small != MAYBE)
	واپس;

    // Mark any nested folds to maybe-small
    سیٹ_Small_Maybe(&fp->fd_nested);

    اگر (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    ورنہ
    {
	گنتی = 0;
	برائے (n = 0; n < fp->fd_len; ++n)
	{
	    گنتی += plines_win_nofold(wp, fp->fd_چوٹی + lnum_off + n);
	    اگر (گنتی > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		واپس;
	    }
	}
	fp->fd_small = TRUE;
    }
}

// setSmallMaybe() {{{2
/*
 * Set small flags in "gap" to MAYBE.
 */
    ساکن خالی
سیٹ_Small_Maybe(garray_T *gap)
{
    صحیح		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    ساکن خالی
foldCreateMarkers(linenr_T آغاز, linenr_T انتہا)
{
    اگر (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	واپس;
    }
    parseMarker(curwin);

    fold_جوڑو_Marker(آغاز, curwin->w_p_fmr, foldstartmarkerlen);
    fold_جوڑو_Marker(انتہا, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    بدلا_سطریں(آغاز, (colnr_T)0, انتہا, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    ساکن خالی
fold_جوڑو_Marker(linenr_T lnum, char_u *marker, صحیح markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*سطر;
    صحیح		سطر_len;
    char_u	*newline;
    char_u	*p = (char_u *)لڑی_تلاش((حرف *)curbuf->b_p_cms, "%s");
    صحیح		سطر_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    سطر = ml_get(lnum);
    سطر_len = ml_get_len(lnum);

    اگر (u_save(lnum - 1, lnum + 1) != OK)
	واپس;

    // Check if the line ends with an unclosed comment
    (خالی)skip_comment(سطر, FALSE, FALSE, &سطر_is_comment);
    newline = alloc(سطر_len + markerlen + STRLEN(cms) + 1);
    اگر (newline == تہی)
	واپس;
    STRCPY(newline, سطر);
    // Append the marker to the end of the line
    اگر (p == تہی || سطر_is_comment)
	vim_strncpy(newline + سطر_len, marker, markerlen);
    ورنہ
    {
	STRCPY(newline + سطر_len, cms);
	STRNCPY(newline + سطر_len + (p - cms), marker, markerlen);
	STRCPY(newline + سطر_len + (p - cms) + markerlen, p + 2);
    }

    ml_replace(lnum, newline, FALSE);
}

// deleteFoldMarkers() {{{2
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    ساکن خالی
deleteFoldMarkers(
    fold_T	*fp,
    صحیح		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    صحیح		i;

    اگر (recursive)
	برائے (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_ڈیٹا + i, TRUE,
						       lnum_off + fp->fd_چوٹی);
    foldDelMarker(fp->fd_چوٹی + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_چوٹی + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

// foldDelMarker() {{{2
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could be a missing
 * close-marker.
 */
    ساکن خالی
foldDelMarker(linenr_T lnum, char_u *marker, صحیح markerlen)
{
    char_u	*سطر;
    char_u	*newline;
    char_u	*p;
    صحیح		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
	واپس;
    سطر = ml_get(lnum);
    برائے (p = سطر; *p != NUL; ++p)
	اگر (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    اگر (VIM_ISDIGIT(p[len]))
		++len;
	    اگر (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)لڑی_تلاش((حرف *)cms, "%s");
		اگر (cms2 != تہی && p - سطر >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (صحیح)STRLEN(cms) - 2;
		}
	    }
	    اگر (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		اگر (newline != تہی)
		{
		    STRNCPY(newline, سطر, p - سطر);
		    STRCPY(newline + (p - سطر), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    توڑو;
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
    char_u	*متن = تہی;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    ساکن صحیح	    got_fdt_error = FALSE;
    صحیح		    save_did_emsg = did_emsg;
    ساکن win_T    *آخری_wp = تہی;
    ساکن linenr_T آخری_lnum = 0;

    اگر (آخری_wp != wp || آخری_wp == تہی
					|| آخری_lnum > lnum || آخری_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    اگر (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    اگر (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	صحیح	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	سیٹ_vim_var_nr(VV_FOLDSTART, lnum);
	سیٹ_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	اگر (level > (صحیح)حجم(dashes) - 1)
	    level = (صحیح)حجم(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	سیٹ_vim_var_string(VV_FOLDDASHES, dashes, level);
	سیٹ_vim_var_nr(VV_FOLDLEVEL, (طویل)level);

	// skip evaluating 'foldtext' on errors
	اگر (!got_fdt_error)
	{
	    win_T   *save_curwin = curwin;
	    sctx_T  saved_sctx = current_sctx;

	    curwin = wp;
	    curbuf = wp->w_بفر;
	    current_sctx = wp->w_p_script_ctx[WV_FDT];

	    ++emsg_off; // handle exceptions, but don't display errors
	    متن = eval_مقصد_string_محفوظ(wp->w_p_fdt,
		       was_سیٹ_insecurely(wp, (char_u *)"foldtext", OPT_LOCAL),
		       TRUE, TRUE);
	    --emsg_off;

	    اگر (متن == تہی || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_بفر;
	    current_sctx = saved_sctx;
	}
	آخری_lnum = lnum;
	آخری_wp   = wp;
	سیٹ_vim_var_string(VV_FOLDDASHES, تہی, -1);

	اگر (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	اگر (متن != تہی)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    برائے (p = متن; *p != NUL; ++p)
	    {
		صحیح	len;

		اگر (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    اگر (!vim_isprintc((*mb_ptr2char)(p)))
			توڑو;
		    p += len - 1;
		}
		ورنہ
		    اگر (*p == TAB)
			*p = ' ';
		    ورنہ اگر (ptr2cells(p) > 1)
			توڑو;
	    }
	    اگر (*p != NUL)
	    {
		p = transstr(متن);
		vim_free(متن);
		متن = p;
	    }
	}
    }
    اگر (متن == تہی)
# endif
    {
	طویل گنتی = (طویل)(lnume - lnum + 1);

	vim_snprintf((حرف *)buf, FOLD_متن_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", گنتی),
		     گنتی);
	متن = buf;
    }
    واپس متن;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    ساکن خالی
foldtext_cleanup(char_u *str)
{
    char_u	*cms_آغاز;	// first part or the whole comment
    صحیح		cms_slen = 0;	// length of cms_start
    char_u	*cms_انتہا;	// last part of the comment or NULL
    صحیح		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    صحیح		len;
    صحیح		did1 = FALSE;
    صحیح		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_آغاز = skipwhite(curbuf->b_p_cms);
    cms_slen = (صحیح)STRLEN(cms_آغاز);
    جب (cms_slen > 0 && VIM_ISWHITE(cms_آغاز[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_انتہا = (char_u *)لڑی_تلاش((حرف *)cms_آغاز, "%s");
    اگر (cms_انتہا != تہی)
    {
	cms_elen = cms_slen - (صحیح)(cms_انتہا - cms_آغاز);
	cms_slen = (صحیح)(cms_انتہا - cms_آغاز);

	// exclude white space before "%s"
	جب (cms_slen > 0 && VIM_ISWHITE(cms_آغاز[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_انتہا + 2);
	cms_elen -= (صحیح)(s - cms_انتہا);
	cms_انتہا = s;
    }
    parseMarker(curwin);

    برائے (s = str; *s != NUL; )
    {
	len = 0;
	اگر (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	ورنہ اگر (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	اگر (len > 0)
	{
	    اگر (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    برائے (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    اگر (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_آغاز, cms_slen) == 0)
	    {
		len += (صحیح)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	ورنہ اگر (cms_انتہا != تہی)
	{
	    اگر (!did1 && cms_slen > 0 && STRNCMP(s, cms_آغاز, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    ورنہ اگر (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_انتہا, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	اگر (len != 0)
	{
	    جب (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	ورنہ
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
قسمتعریف ساخت
{
    win_T	*wp;		//کھڑکی
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    صحیح		lvl;		// current level (-1 for undefined)
    صحیح		lvl_اگلا;	// level used for next line
    صحیح		آغاز;		// number of folds that are forced to start at
				// this line.
    صحیح		انتہا;		// level of fold that is forced to end below
				// this line
    صحیح		had_انتہا;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
ساکن صحیح fold_بدلا;

// Function declarations. {{{2
ساکن linenr_T foldUpdateIEMSRecurse(garray_T *gap, صحیح level, linenr_T startlnum, fline_T *flp, خالی (*getlevel)(fline_T *), linenr_T bot, صحیح topflags);
ساکن صحیح fold_داخل_کرو(garray_T *gap, صحیح i);
ساکن خالی fold_بانٹو(garray_T *gap, صحیح i, linenr_T چوٹی, linenr_T bot);
ساکن خالی foldRemove(garray_T *gap, linenr_T چوٹی, linenr_T bot);
ساکن خالی fold_ملاؤ(fold_T *fp1, garray_T *gap, fold_T *fp2);
ساکن خالی foldlevelIndent(fline_T *flp);
# ifdef FEAT_DIFF
ساکن خالی foldlevelDiff(fline_T *flp);
# endif
ساکن خالی foldlevel_اظہاریہ(fline_T *flp);
ساکن خالی foldlevelMarker(fline_T *flp);
ساکن خالی foldlevelSyntax(fline_T *flp);

// foldUpdateIEMS() {{{2
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    ساکن خالی
foldUpdateIEMS(win_T *wp, linenr_T چوٹی, linenr_T bot)
{
    linenr_T	آغاز;
    linenr_T	انتہا;
    fline_T	fline;
    خالی	(*getlevel)(fline_T *);
    صحیح		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    اگر (invalid_چوٹی != (linenr_T)0)
	واپس;

    اگر (wp->w_foldinvalid)
    {
	// Need to update all folds.
	چوٹی = 1;
	bot = wp->w_بفر->b_ml.ml_سطر_گنتی;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	سیٹ_Small_Maybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    اگر (foldmethodIsDiff(wp))
    {
	اگر (چوٹی > diff_context)
	    چوٹی -= diff_context;
	ورنہ
	    چوٹی = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    اگر (چوٹی > wp->w_بفر->b_ml.ml_سطر_گنتی)
	چوٹی = wp->w_بفر->b_ml.ml_سطر_گنتی;

    fold_بدلا = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_اگلا = -1;
    fline.آغاز = 0;
    fline.انتہا = MAX_LEVEL + 1;
    fline.had_انتہا = MAX_LEVEL + 1;

    invalid_چوٹی = چوٹی;
    invalid_bot = bot;

    اگر (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	اگر (چوٹی > 1)
	{
	    // Get the fold level at top - 1.
	    level = foldLevelWin(wp, چوٹی - 1);

	    // The fold may end just above the top, check for that.
	    fline.lnum = چوٹی - 1;
	    fline.lvl = level;
	    getlevel(&fline);

	    // If a fold started here, we already had the level, if it stops
	    // here, we need to use lvl_next.  Could also start and end a fold
	    // in the same line.
	    اگر (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_اگلا);
	    ورنہ
		fline.lvl = fline.lvl_اگلا;
	}
	fline.lnum = چوٹی;
	getlevel(&fline);
    }
    ورنہ
    {
	fline.lnum = چوٹی;
	اگر (foldmethod_Is_اظہاریہ(wp))
	{
	    getlevel = foldlevel_اظہاریہ;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    اگر (چوٹی > 1)
		--fline.lnum;
	}
	ورنہ اگر (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	ورنہ اگر (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	ورنہ
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    اگر (چوٹی > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	برائے ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_اگلا = -1;
	    getlevel(&fline);
	    اگر (fline.lvl >= 0)
		توڑو;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    اگر (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = تہی;
	صحیح	  current_fdl = 0;
	linenr_T  fold_آغاز_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	جب (current_fdl < fline.lvl)
	{
	    اگر (!fold_ڈھونڈو(gap, lnum_rel, &fpn))
		توڑو;
	    ++current_fdl;

	    fold_آغاز_lnum += fpn->fd_چوٹی;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_چوٹی;
	}
	اگر (fpn != تہی && current_fdl == fline.lvl)
	{
	    linenr_T fold_انتہا_lnum = fold_آغاز_lnum + fpn->fd_len;

	    اگر (fold_انتہا_lnum > bot)
		bot = fold_انتہا_lnum;
	}
    }

    آغاز = fline.lnum;
    انتہا = bot;
    // Do at least one line.
    اگر (آغاز > انتہا && انتہا < wp->w_بفر->b_ml.ml_سطر_گنتی)
	انتہا = آغاز;
    جب (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	اگر (fline.lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    توڑو;
	اگر (fline.lnum > انتہا)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    اگر (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_اظہاریہ)
		توڑو;
	    اگر ((آغاز <= انتہا
			&& fold_ڈھونڈو(&wp->w_folds, انتہا, &fp)
			&& fp->fd_چوٹی + fp->fd_len - 1 > انتہا)
		    || (fline.lvl == 0
			&& fold_ڈھونڈو(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_چوٹی < fline.lnum))
		انتہا = fp->fd_چوٹی + fp->fd_len - 1;
	    ورنہ اگر (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		انتہا = fline.lnum;
	    ورنہ
		توڑو;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	اگر (fline.lvl > 0)
	{
	    invalid_چوٹی = fline.lnum;
	    invalid_bot = انتہا;
	    انتہا = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, آغاز, &fline, getlevel, انتہا, FD_LEVEL);
	    آغاز = fline.lnum;
	}
	ورنہ
	{
	    اگر (fline.lnum == wp->w_بفر->b_ml.ml_سطر_گنتی)
		توڑو;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_اگلا;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, آغاز, انتہا);

    // If some fold changed, need to redraw and position cursor.
    اگر (fold_بدلا && wp->w_p_fen)
	بدلا_کھڑکی_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    اگر (انتہا != bot)
	redraw_win_range_later(wp, چوٹی, انتہا);

    invalid_چوٹی = (linenr_T)0;
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
    ساکن linenr_T
foldUpdateIEMSRecurse(
    garray_T	*gap,
    صحیح		level,
    linenr_T	startlnum,
    fline_T	*flp,
    خالی	(*getlevel)(fline_T *),
    linenr_T	bot,
    صحیح		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = تہی;
    fold_T	*fp2;
    صحیح		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    صحیح		i;
    صحیح		اختتام = FALSE;
    linenr_T	linecount = flp->wp->w_بفر->b_ml.ml_سطر_گنتی - flp->off;
    صحیح		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    اگر (getlevel == foldlevelMarker && flp->آغاز <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(خالی)fold_ڈھونڈو(gap, startlnum - 1, &fp);
	اگر (fp != تہی && (fp >= ((fold_T *)gap->ga_ڈیٹا) + gap->ga_len
						   || fp->fd_چوٹی >= startlnum))
	    fp = تہی;
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
    جب (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	سطر_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	اگر (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	اگر (flp->lnum > firstlnum
		&& (level > lvl - flp->آغاز || level >= flp->had_انتہا))
	    lvl = 0;

	اگر (flp->lnum > bot && !اختتام && fp != تہی)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    اگر (getlevel != foldlevelMarker
		    && getlevel != foldlevel_اظہاریہ
		    && getlevel != foldlevelSyntax)
		توڑو;
	    i = 0;
	    fp2 = fp;
	    اگر (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_چوٹی;
		جب (fold_ڈھونڈو(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_چوٹی;
		}
	    }
	    اگر (lvl < level + i)
	    {
		(خالی)fold_ڈھونڈو(&fp->fd_nested, flp->lnum - fp->fd_چوٹی, &fp2);
		اگر (fp2 != تہی)
		    bot = fp2->fd_چوٹی + fp2->fd_len - 1 + fp->fd_چوٹی;
	    }
	    ورنہ اگر (fp->fd_چوٹی + fp->fd_len <= flp->lnum && lvl >= level)
		اختتام = TRUE;
	    ورنہ
		توڑو;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	اگر (fp == تہی
		&& (lvl != level
		    || flp->lnum_save >= bot
		    || flp->آغاز != 0
		    || flp->had_انتہا <= MAX_LEVEL
		    || flp->lnum == linecount))
	{
	    /*
	     * Remove or update folds that have lines between startlnum and
	     * firstlnum.
	     */
	    جب (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		اگر (flp->آغاز != 0 || flp->had_انتہا <= MAX_LEVEL)
		    concat = 0;
		ورنہ
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		اگر (gap->ga_len > 0 && (fold_ڈھونڈو(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_ڈیٹا) + gap->ga_len
			    && fp->fd_چوٹی <= firstlnum)
			|| fold_ڈھونڈو(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_ڈیٹا) + gap->ga_len
			    && ((lvl < level && fp->fd_چوٹی < flp->lnum)
				|| (lvl >= level
					   && fp->fd_چوٹی <= flp->lnum_save)))))
		{
		    اگر (fp->fd_چوٹی + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			اگر (fp->fd_چوٹی == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			ورنہ اگر (fp->fd_چوٹی >= startlnum)
			{
			    اگر (fp->fd_چوٹی > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(طویل)(fp->fd_چوٹی - firstlnum), 0L);
			    ورنہ
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(طویل)(firstlnum - fp->fd_چوٹی - 1),
					(linenr_T)MAXLNUM,
					(طویل)(fp->fd_چوٹی - firstlnum));
			    fp->fd_len += fp->fd_چوٹی - firstlnum;
			    fp->fd_چوٹی = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_بدلا = TRUE;
			}
			ورنہ اگر ((flp->آغاز != 0 && lvl == level)
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
			    اگر (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    ورنہ
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_چوٹی,
						      breakend - fp->fd_چوٹی);
			    i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا);
			    fold_بانٹو(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_ڈیٹا + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    اگر (getlevel == foldlevelMarker
				    || getlevel == foldlevel_اظہاریہ
				    || getlevel == foldlevelSyntax)
				اختتام = TRUE;
			}

			اگر (fp->fd_چوٹی == startlnum && concat)
			{
			    i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا);
			    اگر (i != 0)
			    {
				fp2 = fp - 1;
				اگر (fp2->fd_چوٹی + fp2->fd_len == fp->fd_چوٹی)
				{
				    fold_ملاؤ(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			توڑو;
		    }
		    اگر (fp->fd_چوٹی >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			delete_Fold_اندراج(gap,
				     (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا), TRUE);
		    }
		    ورنہ
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_چوٹی;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_بدلا = TRUE;
		    }
		}
		ورنہ
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    اگر (gap->ga_len == 0)
			i = 0;
		    ورنہ
			i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا);
		    اگر (fold_داخل_کرو(gap, i) != OK)
			واپس bot;
		    fp = (fold_T *)gap->ga_ڈیٹا + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_چوٹی = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    اگر (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    ورنہ اگر (i <= 0)
		    {
			fp->fd_flags = topflags;
			اگر (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    ورنہ
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    اگر (getlevel == foldlevelMarker
			    || getlevel == foldlevel_اظہاریہ
			    || getlevel == foldlevelSyntax)
			اختتام = TRUE;
		    fold_بدلا = TRUE;
		    توڑو;
		}
	    }
	}

	اگر (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    توڑو;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	اگر (lvl > level && fp != تہی)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    اگر (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_چوٹی;
	    flp->off += fp->fd_چوٹی;
	    i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_چوٹی, flp, getlevel,
					      bot - fp->fd_چوٹی, fp->fd_flags);
	    fp = (fold_T *)gap->ga_ڈیٹا + i;
	    flp->lnum += fp->fd_چوٹی;
	    flp->lnum_save += fp->fd_چوٹی;
	    flp->off -= fp->fd_چوٹی;
	    bot += fp->fd_چوٹی;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	ورنہ
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    جب (!got_int)
	    {
		// Make the previous level available to foldlevel().
		پچھلا_lnum = flp->lnum;
		پچھلا_lnum_lvl = flp->lvl;

		اگر (++flp->lnum > linecount)
		    توڑو;
		flp->lvl = flp->lvl_اگلا;
		getlevel(flp);
		اگر (flp->lvl >= 0 || flp->had_انتہا <= MAX_LEVEL)
		    توڑو;
	    }
	    پچھلا_lnum = 0;
	    اگر (flp->lnum > linecount)
		توڑو;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    اگر (fp == تہی)	// only happens when got_int is set
	واپس bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    اگر (fp->fd_len < flp->lnum - fp->fd_چوٹی)
    {
	fp->fd_len = flp->lnum - fp->fd_چوٹی;
	fp->fd_small = MAYBE;
	fold_بدلا = TRUE;
    }
    ورنہ اگر (fp->fd_چوٹی + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_چوٹی + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_چوٹی,
						  flp->lnum - 1 - fp->fd_چوٹی);

    اگر (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	اگر (fp->fd_len != flp->lnum - fp->fd_چوٹی)
	{
	    اگر (fp->fd_چوٹی + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		اگر (getlevel == foldlevelMarker
			|| getlevel == foldlevel_اظہاریہ
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_چوٹی + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_چوٹی;
		}
		ورنہ
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا);
		    fold_بانٹو(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_ڈیٹا + i;
		}
	    }
	    ورنہ
		fp->fd_len = flp->lnum - fp->fd_چوٹی;
	    fold_بدلا = TRUE;
	}
    }

    // delete following folds that end before the current line
    برائے (;;)
    {
	fp2 = fp + 1;
	اگر (fp2 >= (fold_T *)gap->ga_ڈیٹا + gap->ga_len
						  || fp2->fd_چوٹی > flp->lnum)
	    توڑو;
	اگر (fp2->fd_چوٹی + fp2->fd_len > flp->lnum)
	{
	    اگر (fp2->fd_چوٹی < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (طویل)(flp->lnum - fp2->fd_چوٹی - 1),
			(linenr_T)MAXLNUM, (طویل)(fp2->fd_چوٹی - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_چوٹی;
		fp2->fd_چوٹی = flp->lnum;
		fold_بدلا = TRUE;
	    }

	    اگر (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		fold_ملاؤ(fp, gap, fp2);
	    }
	    توڑو;
	}
	fold_بدلا = TRUE;
	delete_Fold_اندراج(gap, (صحیح)(fp2 - (fold_T *)gap->ga_ڈیٹا), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    اگر (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    واپس bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    ساکن صحیح
fold_داخل_کرو(garray_T *gap, صحیح i)
{
    fold_T	*fp;

    اگر (ga_grow(gap, 1) == FAIL)
	واپس FAIL;
    fp = (fold_T *)gap->ga_ڈیٹا + i;
    اگر (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, حجم(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, حجم(fold_T), 10);
    واپس OK;
}

// foldSplit() {{{2
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    ساکن خالی
fold_بانٹو(
    garray_T	*gap,
    صحیح		i,
    linenr_T	چوٹی,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    صحیح		idx;
    صحیح		len;

    // The fold continues below bot, need to split it.
    اگر (fold_داخل_کرو(gap, i + 1) == FAIL)
	واپس;
    fp = (fold_T *)gap->ga_ڈیٹا + i;
    fp[1].fd_چوٹی = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_چوٹی - fp->fd_چوٹی);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (خالی)fold_ڈھونڈو(gap1, bot + 1 - fp->fd_چوٹی, &fp2);
    اگر (fp2 != تہی)
    {
	len = (صحیح)((fold_T *)gap1->ga_ڈیٹا + gap1->ga_len - fp2);
	اگر (len > 0 && ga_grow(gap2, len) == OK)
	{
	    برائے (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_ڈیٹا)[idx] = fp2[idx];
		((fold_T *)gap2->ga_ڈیٹا)[idx].fd_چوٹی
						  -= fp[1].fd_چوٹی - fp->fd_چوٹی;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = چوٹی - fp->fd_چوٹی;
    fold_بدلا = TRUE;
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
    ساکن خالی
foldRemove(garray_T *gap, linenr_T چوٹی, linenr_T bot)
{
    fold_T	*fp = تہی;

    اگر (bot < چوٹی)
	واپس;		// nothing to do

    جب (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	اگر (fold_ڈھونڈو(gap, چوٹی, &fp) && fp->fd_چوٹی < چوٹی)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, چوٹی - fp->fd_چوٹی, bot - fp->fd_چوٹی);
	    اگر (fp->fd_چوٹی + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		fold_بانٹو(gap, (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا), چوٹی, bot);
	    }
	    ورنہ
	    {
		// 2: truncate fold at "top".
		fp->fd_len = چوٹی - fp->fd_چوٹی;
	    }
	    fold_بدلا = TRUE;
	    جاری;
	}
	اگر (fp >= (fold_T *)(gap->ga_ڈیٹا) + gap->ga_len
		|| fp->fd_چوٹی > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    توڑو;
	}
	اگر (fp->fd_چوٹی >= چوٹی)
	{
	    // Found an entry below top.
	    fold_بدلا = TRUE;
	    اگر (fp->fd_چوٹی + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (طویل)(bot - fp->fd_چوٹی),
			(linenr_T)MAXLNUM, (طویل)(fp->fd_چوٹی - bot - 1));
		fp->fd_len -= bot - fp->fd_چوٹی + 1;
		fp->fd_چوٹی = bot + 1;
		توڑو;
	    }

	    // 4: Delete completely contained fold.
	    delete_Fold_اندراج(gap, (صحیح)(fp - (fold_T *)gap->ga_ڈیٹا), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    ساکن خالی
fold_الٹ_ترتیب(garray_T *gap, linenr_T آغاز_arg, linenr_T انتہا_arg)
{
    fold_T *بایاں, *دایاں;
    fold_T tmp;
    linenr_T آغاز = آغاز_arg;
    linenr_T انتہا = انتہا_arg;

    برائے (; آغاز < انتہا; آغاز++, انتہا--)
    {
	بایاں = (fold_T *)gap->ga_ڈیٹا + آغاز;
	دایاں = (fold_T *)gap->ga_ڈیٹا + انتہا;
	tmp  = *بایاں;
	*بایاں = *دایاں;
	*دایاں = tmp;
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

    ساکن خالی
truncate_fold(fold_T *fp, linenr_T انتہا)
{
    انتہا += 1;
    foldRemove(&fp->fd_nested, انتہا - fp->fd_چوٹی, MAXLNUM);
    fp->fd_len = انتہا - fp->fd_چوٹی;
}

# define fold_انتہا(fp) ((fp)->fd_چوٹی + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_ڈیٹا + (gap)->ga_len))
# define fold_اشاریہ(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_ڈیٹا)))

    خالی
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    صحیح at_آغاز = fold_ڈھونڈو(gap, line1 - 1, &fp);
    size_t move_آغاز = 0, move_انتہا = 0, dest_اشاریہ = 0;

    اگر (at_آغاز)
    {
	اگر (fold_انتہا(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_چوٹی, line2 -
		    fp->fd_چوٹی, dest - fp->fd_چوٹی);
	    واپس;
	}
	ورنہ اگر (fold_انتہا(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_چوٹی, line2 -
		    fp->fd_چوٹی, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	ورنہ
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    اگر (!valid_fold(fp, gap) || fp->fd_چوٹی > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	واپس;
    }
    ورنہ اگر (fp->fd_چوٹی > line2)
    {
	برائے (; valid_fold(fp, gap) && fold_انتہا(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_چوٹی -= range_len;

	اگر (valid_fold(fp, gap) && fp->fd_چوٹی <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_چوٹی -= range_len;
	}
	واپس;
    }
    ورنہ اگر (fold_انتہا(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_چوٹی, dest -
		fp->fd_چوٹی, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_چوٹی += move_len;
	واپس;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_آغاز = fold_اشاریہ(fp, gap);

    برائے (; valid_fold(fp, gap) && fp->fd_چوٹی <= dest; fp++)
    {
	اگر (fp->fd_چوٹی <= line2)
	{
	    // 1. 2. or 3.
	    اگر (fold_انتہا(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_چوٹی += move_len;
	    جاری;
	}

	// Record index of the first fold after the moved range.
	اگر (move_انتہا == 0)
	    move_انتہا = fold_اشاریہ(fp, gap);

	اگر (fold_انتہا(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_چوٹی -= range_len;
    }

    dest_اشاریہ = fold_اشاریہ(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    اگر (move_انتہا == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	واپس;
    fold_الٹ_ترتیب(gap, (linenr_T)move_آغاز, (linenr_T)dest_اشاریہ - 1);
    fold_الٹ_ترتیب(gap, (linenr_T)move_آغاز,
			   (linenr_T)(move_آغاز + dest_اشاریہ - move_انتہا - 1));
    fold_الٹ_ترتیب(gap, (linenr_T)(move_آغاز + dest_اشاریہ - move_انتہا),
						   (linenr_T)(dest_اشاریہ - 1));
}
# undef fold_انتہا
# undef valid_fold
# undef fold_اشاریہ

// foldMerge() {{{2
/*
 * Merge two adjacent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjacent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    ساکن خالی
fold_ملاؤ(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    صحیح		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    اگر (fold_ڈھونڈو(gap1, fp1->fd_len - 1L, &fp3) && fold_ڈھونڈو(gap2, 0L, &fp4))
	fold_ملاؤ(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    اگر (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	برائے (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_ڈیٹا)[gap1->ga_len]
					= ((fold_T *)gap2->ga_ڈیٹا)[idx];
	    ((fold_T *)gap1->ga_ڈیٹا)[gap1->ga_len].fd_چوٹی += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    delete_Fold_اندراج(gap, (صحیح)(fp2 - (fold_T *)gap->ga_ڈیٹا), TRUE);
    fold_بدلا = TRUE;
}

// foldlevelIndent() {{{2
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ساکن خالی
foldlevelIndent(fline_T *flp)
{
    char_u	*s;
    buf_T	*buf;
    linenr_T	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_بفر;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    اگر (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != تہی)
    {
	// first and last line can't be undefined, use level 0
	اگر (lnum == 1 || lnum == buf->b_ml.ml_سطر_گنتی)
	    flp->lvl = 0;
	ورنہ
	    flp->lvl = -1;
    }
    ورنہ
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_قدر(buf);
    اگر (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	اگر (flp->lvl < 0)
	    flp->lvl = 0;
    }
}

// foldlevelDiff() {{{2
# ifdef FEAT_DIFF
/*
 * Low level function to get the foldlevel for the "diff" method.
 * Doesn't use any caching.
 */
    ساکن خالی
foldlevelDiff(fline_T *flp)
{
    اگر (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    ورنہ
	flp->lvl = 0;
}
# endif

// foldlevelExpr() {{{2
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    ساکن خالی
foldlevel_اظہاریہ(fline_T *flp)
{
# ifndef FEAT_EVAL
    flp->آغاز = FALSE;
    flp->lvl = 0;
# ورنہ
    win_T	*win;
    صحیح		n;
    صحیح		c;
    linenr_T	lnum = flp->lnum + flp->off;
    صحیح		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_بفر;
    سیٹ_vim_var_nr(VV_LNUM, lnum);

    flp->آغاز = 0;
    flp->had_انتہا = flp->انتہا;
    flp->انتہا = MAX_LEVEL + 1;
    اگر (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = کلید_Typed;
    n = eval_foldexpr(flp->wp, &c);
    کلید_Typed = save_keytyped;

    انتخاب (c)
    {
	// "a1", "a2", .. : add to the fold level
	حالت 'a': اگر (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_اگلا = flp->lvl;
		  }
		  flp->آغاز = n;
		  توڑو;

	// "s1", "s2", .. : subtract from the fold level
	حالت 's': اگر (flp->lvl >= 0)
		  {
		      اگر (n > flp->lvl)
			  flp->lvl_اگلا = 0;
		      ورنہ
			  flp->lvl_اگلا = flp->lvl - n;
		      flp->انتہا = flp->lvl_اگلا + 1;
		  }
		  توڑو;

	// ">1", ">2", .. : start a fold with a certain level
	حالت '>': flp->lvl = n;
		  flp->lvl_اگلا = n;
		  flp->آغاز = 1;
		  توڑو;

	// "<1", "<2", .. : end a fold with a certain level
	حالت '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_اگلا = MIN(flp->lvl, n - 1);
		  flp->انتہا = n;
		  توڑو;

	// "=": No change in level
	حالت '=': flp->lvl_اگلا = flp->lvl;
		  توڑو;

	// "-1", "0", "1", ..: set fold level
	طےشدہ:  اگر (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_اگلا = flp->lvl;
		  ورنہ
		      flp->lvl_اگلا = n;
		  flp->lvl = n;
		  توڑو;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    اگر (flp->lvl < 0)
    {
	اگر (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_اگلا = 0;
	}
	اگر (lnum == curbuf->b_ml.ml_سطر_گنتی)
	    flp->lvl_اگلا = 0;
    }

    curwin = win;
    curbuf = curwin->w_بفر;
# endif
}

// parseMarker() {{{2
/*
 * Parse 'foldmarker' and set "foldendmarker", "foldstartmarkerlen" and
 * "foldendmarkerlen".
 * Relies on the option value to have been checked for correctness already.
 */
    ساکن خالی
parseMarker(win_T *wp)
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = (صحیح)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (صحیح)STRLEN(foldendmarker);
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
    ساکن خالی
foldlevelMarker(fline_T *flp)
{
    char_u	*startmarker;
    صحیح		cstart;
    صحیح		cend;
    صحیح		آغاز_lvl = flp->lvl;
    char_u	*s;
    صحیح		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->آغاز = 0;
    flp->lvl_اگلا = flp->lvl;

    s = ml_get_buf(flp->wp->w_بفر, flp->lnum + flp->off, FALSE);
    جب (*s)
    {
	اگر (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    اگر (VIM_ISDIGIT(*s))
	    {
		n = atoi((حرف *)s);
		اگر (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_اگلا = n;
		    اگر (n <= آغاز_lvl)
			flp->آغاز = 1;
		    ورنہ
			flp->آغاز = n - آغاز_lvl;
		}
	    }
	    ورنہ
	    {
		++flp->lvl;
		++flp->lvl_اگلا;
		++flp->آغاز;
	    }
	}
	ورنہ اگر (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    اگر (VIM_ISDIGIT(*s))
	    {
		n = atoi((حرف *)s);
		اگر (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_اگلا = n - 1;
		    // never start a fold with an end marker
		    اگر (flp->lvl_اگلا > آغاز_lvl)
			flp->lvl_اگلا = آغاز_lvl;
		}
	    }
	    ورنہ
		--flp->lvl_اگلا;
	}
	ورنہ
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    اگر (flp->lvl_اگلا < 0)
	flp->lvl_اگلا = 0;
}

// foldlevelSyntax() {{{2
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    ساکن خالی
foldlevelSyntax(fline_T *flp)
{
# ifndef FEAT_SYN_HL
    flp->آغاز = 0;
    flp->lvl = 0;
# ورنہ
    linenr_T	lnum = flp->lnum + flp->off;
    صحیح		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->آغاز = 0;
    اگر (lnum < flp->wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	اگر (n > flp->lvl)
	{
	    flp->آغاز = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# اگر defined(FEAT_SESSION)
ساکن صحیح put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
ساکن صحیح put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
ساکن صحیح put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    صحیح
put_folds(FILE *fd, win_T *wp)
{
    اگر (foldmethodIsManual(wp))
    {
	اگر (put_سطر(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_سطر(fd, "&fdl = &fdl") == FAIL)
	    واپس FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    اگر (wp->w_fold_manual)
	واپس put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    واپس OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    ساکن صحیح
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    صحیح		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	اگر (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_چوٹی) == FAIL)
	    واپس FAIL;
	اگر (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_چوٹی + off,
					fp->fd_چوٹی + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    واپس FAIL;
	++fp;
    }
    واپس OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    ساکن صحیح
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    صحیح		i;
    صحیح		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; i++)
    {
	اگر (fp->fd_flags != FD_LEVEL)
	{
	    اگر (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		اگر (fprintf(fd, ":%ld", fp->fd_چوٹی + off) < 0
			|| put_eol(fd) == FAIL
			|| put_سطر(fd, "sil! normal! zo") == FAIL)
		    واپس FAIL;
		اگر (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_چوٹی)
			== FAIL)
		    واپس FAIL;
		// close the parent when needed
		اگر (fp->fd_flags == FD_CLOSED)
		{
		    اگر (put_fold_open_close(fd, fp, off) == FAIL)
			واپس FAIL;
		}
	    }
	    ورنہ
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_چوٹی);
		اگر ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		اگر (put_fold_open_close(fd, fp, off) == FAIL)
		    واپس FAIL;
	    }
	}
	++fp;
    }

    واپس OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    ساکن صحیح
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    اگر (fprintf(fd, ":%ld", fp->fd_چوٹی + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	واپس FAIL;

    واپس OK;
}
# endif // FEAT_SESSION

// }}}1
#اگر_ختم // defined(FEAT_FOLDING)

#اگر defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    ساکن خالی
foldclosed_both(
    typval_T	*argvars غیر_مستعمل,
    typval_T	*rettv,
    صحیح		انتہا غیر_مستعمل)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	پہلا, آخری;
    win_T	*wp  = curwin;

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_نمبر(&argvars[1]), &tp);
	اگر (wp != تہی && tp != تہی)
	{
	    اگر (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	ورنہ
	{
	    rettv->vval.v_نمبر = -2;
	    واپس;
	}
    }
    ورنہ
	lnum = tv_get_lnum(argvars);

    اگر (lnum >= 1 && lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	اگر (hasFoldingWin(wp, lnum, &پہلا, &آخری, FALSE, تہی))
	{
	    اگر (انتہا)
		rettv->vval.v_نمبر = (varnumber_T)آخری;
	    ورنہ
		rettv->vval.v_نمبر = (varnumber_T)پہلا;
	    واپس;
	}
    }
# endif
    rettv->vval.v_نمبر = -1;
}

/*
 * "foldclosed()" function
 */
    خالی
f_foldclosed(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, FALSE);
}

/*
 * "foldclosedend()" function
 */
    خالی
f_foldclosedend(typval_T *argvars, typval_T *rettv)
{
    foldclosed_both(argvars, rettv, TRUE);
}

# ifdef FEAT_FOLDING
/*
 * Return fold level at line number "lnum" in the current window.
 */
    ساکن صحیح
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    اگر (invalid_چوٹی == (linenr_T)0)
	checkupdate(curwin);
    ورنہ اگر (lnum == پچھلا_lnum && پچھلا_lnum_lvl >= 0)
	واپس پچھلا_lnum_lvl;
    ورنہ اگر (lnum >= invalid_چوٹی && lnum <= invalid_bot)
	واپس -1;

    // Return quickly when there is no folding at all in this window.
    اگر (!hasAnyFolding(curwin))
	واپس 0;

    واپس foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    خالی
f_foldlevel(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_نمبر(&argvars[1]), &tp);
	اگر (wp != تہی && tp != تہی)
	{
	    اگر (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی)
		    rettv->vval.v_نمبر = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	ورنہ
	    rettv->vval.v_نمبر = -2;
    }
    ورنہ
    {
	lnum = tv_get_lnum(argvars);
	اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی)
	    rettv->vval.v_نمبر = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    خالی
f_foldtext(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    صحیح		len;
    حرف	*txt;
    طویل	گنتی;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    اگر (foldstart > 0 && foldend <= curbuf->b_ml.ml_سطر_گنتی
	    && dashes != تہی)
    {
	// Find first non-empty line in the fold.
	برائے (lnum = foldstart; lnum < foldend; ++lnum)
	    اگر (!linewhite(lnum))
		توڑو;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	اگر (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    اگر (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		اگر (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	گنتی = (طویل)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", گنتی);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	اگر (r != تہی)
	{
	    sprintf((حرف *)r, txt, dashes, گنتی);
	    len = (صحیح)STRLEN(r);
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
    خالی
f_foldtextresult(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*متن;
    char_u	buf[FOLD_متن_LEN];
    foldinfo_T  foldinfo;
    صحیح		fold_گنتی;
    win_T	*wp  = curwin;
    ساکن صحیح	entered = FALSE;
    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (entered)
	واپس; // reject recursive use
    entered = TRUE;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_نمبر(&argvars[1]), &tp);
	اگر (wp == تہی || tp == تہی) // wrong winid
	{
	    entered = FALSE;
	    واپس;
	}
	اگر (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    ورنہ
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    اگر (lnum < 0)
	lnum = 0;
    fold_گنتی = folded_گنتی(wp, lnum, &foldinfo);
    اگر (fold_گنتی > 0)
    {
	متن = get_foldtext(wp, lnum, lnum + fold_گنتی - 1,
							       &foldinfo, buf);
	اگر (متن == buf)
	    متن = vim_strsave(متن);
	rettv->vval.v_string = متن;
    }

    entered = FALSE;
# endif
}

#اگر_ختم // FEAT_EVAL
