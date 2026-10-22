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

#ಆದರೆ defined(FEAT_FOLDING)

// local declarations. {{{1
// typedef fold_T {{{2
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_T.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	fd_ತುದಿ;		// first line of fold; for nested fold
				// relative to parent
    linenr_T	fd_len;		// number of lines in the fold
    garray_T	fd_nested;	// array of nested folds
    ಅಕ್ಷರ	fd_flags;	// see below
    ಅಕ್ಷರ	fd_small;	// TRUE, FALSE or MAYBE: fold smaller than
				// 'foldminlines'; MAYBE applies to nested
				// folds too
} fold_T;

# define FD_OPEN		0	// fold is open (nested ones can be closed)
# define FD_CLOSED	1	// fold is closed
# define FD_LEVEL	2	// depends on 'foldlevel' (nested folds too)

# define MAX_LEVEL	20	// maximum fold depth

// static functions {{{2
ಸ್ಥಿರ ಶೂನ್ಯ newFoldLevelWin(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ checkCloseRec(garray_T *gap, linenr_T lnum, ಪೂರ್ಣಾಂಕ level);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fold_ಕಂಡುಹಿಡಿ(garray_T *gap, linenr_T lnum, fold_T **fpp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ foldLevelWin(win_T *wp, linenr_T lnum);
ಸ್ಥಿರ ಶೂನ್ಯ checkupdate(win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Fold_Repeat(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ do_open);
ಸ್ಥಿರ linenr_T ಗಣ_Manual_Fold(linenr_T lnum, ಪೂರ್ಣಾಂಕ opening, ಪೂರ್ಣಾಂಕ recurse, ಪೂರ್ಣಾಂಕ *donep);
ಸ್ಥಿರ linenr_T ಗಣ_Manual_Fold_Win(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಂಕ opening, ಪೂರ್ಣಾಂಕ recurse, ಪೂರ್ಣಾಂಕ *donep);
ಸ್ಥಿರ ಶೂನ್ಯ foldOpenNested(fold_T *fpr);
ಸ್ಥಿರ ಶೂನ್ಯ delete_Fold_ನಮೂದು(garray_T *gap, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ recursive);
ಸ್ಥಿರ ಶೂನ್ಯ foldMarkAdjustRecurse(garray_T *gap, linenr_T line1, linenr_T line2, ದೀರ್ಘ amount, ದೀರ್ಘ amount_after);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_ಅತ್ಯಂತಆಳ_Nesting_Recurse(garray_T *gap);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_closed(win_T *win, fold_T *fp, ಪೂರ್ಣಾಂಕ *use_levelp, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ *maybe_smallp, linenr_T lnum_off);
ಸ್ಥಿರ ಶೂನ್ಯ checkSmall(win_T *wp, fold_T *fp, linenr_T lnum_off);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Small_Maybe(garray_T *gap);
ಸ್ಥಿರ ಶೂನ್ಯ foldCreateMarkers(linenr_T ಆರಂಭ, linenr_T ಕೊನೆ);
ಸ್ಥಿರ ಶೂನ್ಯ fold_ಕೂಡಿಸು_Marker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಂಕ markerlen);
ಸ್ಥಿರ ಶೂನ್ಯ deleteFoldMarkers(fold_T *fp, ಪೂರ್ಣಾಂಕ recursive, linenr_T lnum_off);
ಸ್ಥಿರ ಶೂನ್ಯ foldDelMarker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಂಕ markerlen);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_lnum_lvl = -1;

// Flags used for "done" argument of setManualFold.
# define ಮುಗಿದ_NOTHING	0
# define ಮುಗಿದ_ACTION	1	// did close or open a fold
# define ಮುಗಿದ_FOLD	2	// did find a fold

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ foldstartmarkerlen;
ಸ್ಥಿರ char_u *foldendmarker;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ foldendmarkerlen;

// Exported folding functions. {{{1
// copyFoldingState() {{{2

/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    ಶೂನ್ಯ
copyFoldingState(win_T *wp_from, win_T *wp_ಗಮ್ಯ)
{
    wp_ಗಮ್ಯ->w_fold_manual = wp_from->w_fold_manual;
    wp_ಗಮ್ಯ->w_foldinvalid = wp_from->w_foldinvalid;
    cloneFoldGrowArray(&wp_from->w_folds, &wp_ಗಮ್ಯ->w_folds);
}

// hasAnyFolding() {{{2
/*
 * Return TRUE if there may be folded lines in window "win".
 */
    ಪೂರ್ಣಾಂಕ
hasAnyFolding(win_T *win)
{
    // very simple now, but can become more complex later
    ಹಿಂತಿರುಗಿ (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

// hasFolding() {{{2
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 */
    ಪೂರ್ಣಾಂಕ
hasFolding(linenr_T lnum, linenr_T *firstp, linenr_T *lastp)
{
    ಹಿಂತಿರುಗಿ hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, ಖಾಲಿ);
}

// hasFoldingWin() {{{2
    ಪೂರ್ಣಾಂಕ
hasFoldingWin(
    win_T	*win,
    linenr_T	lnum,
    linenr_T	*firstp,
    linenr_T	*lastp,
    ಪೂರ್ಣಾಂಕ		cache,		// when TRUE: use cached values of window
    foldinfo_T	*infop)		// where to store fold info
{
    ಪೂರ್ಣಾಂಕ		had_folded = FALSE;
    linenr_T	ಮೊದಲ = 0;
    linenr_T	ಕೊನೆಯ = 0;
    linenr_T	lnum_rel = lnum;
    ಪೂರ್ಣಾಂಕ		x;
    fold_T	*fp;
    ಪೂರ್ಣಾಂಕ		level = 0;
    ಪೂರ್ಣಾಂಕ		use_level = FALSE;
    ಪೂರ್ಣಾಂಕ		maybe_small = FALSE;
    garray_T	*gap;
    ಪೂರ್ಣಾಂಕ		ಕೆಳಗಿನ_level = 0;

    checkupdate(win);

    /*
     * Return quickly when there is no folding at all in this window.
     */
    ಆದರೆ (!hasAnyFolding(win))
    {
	ಆದರೆ (infop != ಖಾಲಿ)
	    infop->fi_level = 0;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = ಕಂಡುಹಿಡಿ_wl_ನಮೂದು(win, lnum);
	ಆದರೆ (x >= 0)
	{
	    ಮೊದಲ = win->w_ಸಾಲುಗಳು[x].wl_lnum;
	    ಕೊನೆಯ = win->w_ಸಾಲುಗಳು[x].wl_lastlnum;
	    had_folded = win->w_ಸಾಲುಗಳು[x].wl_folded;
	}
    }

    ಆದರೆ (ಮೊದಲ == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, lnum_rel, &fp))
		ನಿಲ್ಲಿಸು;

	    // Remember lowest level of fold that starts in "lnum".
	    ಆದರೆ (lnum_rel == fp->fd_ತುದಿ && ಕೆಳಗಿನ_level == 0)
		ಕೆಳಗಿನ_level = level + 1;

	    ಮೊದಲ += fp->fd_ತುದಿ;
	    ಕೊನೆಯ += fp->fd_ತುದಿ;

	    // is this fold closed?
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    ಆದರೆ (had_folded)
	    {
		// Fold closed: Set last and quit loop.
		ಕೊನೆಯ += fp->fd_len - 1;
		ನಿಲ್ಲಿಸು;
	    }

	    // Fold found, but it's open: Check nested folds.  Line number is
	    // relative to containing fold.
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_ತುದಿ;
	    ++level;
	}
    }

    ಆದರೆ (!had_folded)
    {
	ಆದರೆ (infop != ಖಾಲಿ)
	{
	    infop->fi_level = level;
	    infop->fi_lnum = lnum - lnum_rel;
	    infop->fi_ಕೆಳಗಿನ_level = ಕೆಳಗಿನ_level == 0 ? level : ಕೆಳಗಿನ_level;
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (ಕೊನೆಯ > win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆಯ = win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (lastp != ಖಾಲಿ)
	*lastp = ಕೊನೆಯ;
    ಆದರೆ (firstp != ಖಾಲಿ)
	*firstp = ಮೊದಲ;
    ಆದರೆ (infop != ಖಾಲಿ)
    {
	infop->fi_level = level + 1;
	infop->fi_lnum = ಮೊದಲ;
	infop->fi_ಕೆಳಗಿನ_level = ಕೆಳಗಿನ_level == 0 ? level + 1 : ಕೆಳಗಿನ_level;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

// lineFolded()	{{{2
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    ಪೂರ್ಣಾಂಕ
ಸಾಲು_Folded(win_T *win, linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ folded_ಎಣಿಕೆ(win, lnum, ಖಾಲಿ) != 0;
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
    ದೀರ್ಘ
folded_ಎಣಿಕೆ(win_T *win, linenr_T lnum, foldinfo_T *infop)
{
    linenr_T	ಕೊನೆಯ;

    ಆದರೆ (hasFoldingWin(win, lnum, ಖಾಲಿ, &ಕೊನೆಯ, FALSE, infop))
	ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)(ಕೊನೆಯ - lnum + 1);
    ಹಿಂತಿರುಗಿ 0;
}

// foldmethodIsManual() {{{2
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    ಪೂರ್ಣಾಂಕ
foldmethodIsManual(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[3] == 'u');
}

// foldmethodIsIndent() {{{2
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    ಪೂರ್ಣಾಂಕ
foldmethodIsIndent(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (wp->w_p_fdm[0] == 'i');
}

// foldmethodIsExpr() {{{2
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    ಪೂರ್ಣಾಂಕ
foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[1] == 'x');
}

// foldmethodIsMarker() {{{2
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    ಪೂರ್ಣಾಂಕ
foldmethodIsMarker(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (wp->w_p_fdm[0] != NUL && wp->w_p_fdm[2] == 'r');
}

// foldmethodIsSyntax() {{{2
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    ಪೂರ್ಣಾಂಕ
foldmethodIsSyntax(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (wp->w_p_fdm[0] == 's');
}

// foldmethodIsDiff() {{{2
/*
 * Return TRUE if 'foldmethod' is "diff"
 */
    ಪೂರ್ಣಾಂಕ
foldmethodIsDiff(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (wp->w_p_fdm[0] == 'd');
}

// closeFold() {{{2
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ಶೂನ್ಯ
closeFold(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಗಣ_Fold_Repeat(lnum, ಎಣಿಕೆ_2, FALSE);
}

// closeFoldRecurse() {{{2
/*
 * Close fold for current window at line "lnum" recursively.
 */
    ಶೂನ್ಯ
closeFoldRecurse(linenr_T lnum)
{
    (ಶೂನ್ಯ)ಗಣ_Manual_Fold(lnum, FALSE, TRUE, ಖಾಲಿ);
}

// opFoldRange() {{{2
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    ಶೂನ್ಯ
opFoldRange(
    linenr_T	ಮೊದಲ,
    linenr_T	ಕೊನೆಯ,
    ಪೂರ್ಣಾಂಕ		opening,	// TRUE to open, FALSE to close
    ಪೂರ್ಣಾಂಕ		recurse,	// TRUE to do it recursively
    ಪೂರ್ಣಾಂಕ		had_visual)	// TRUE when Visual selection used
{
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = ಮುಗಿದ_NOTHING;	// avoid error messages
    linenr_T	lnum;
    linenr_T	lnum_ಮುಂದಿನ;

    ಗಾಗಿ (lnum = ಮೊದಲ; lnum <= ಕೊನೆಯ; lnum = lnum_ಮುಂದಿನ + 1)
    {
	lnum_ಮುಂದಿನ = lnum;
	// Opening one level only: next fold to open is after the one going to
	// be opened.
	ಆದರೆ (opening && !recurse)
	    (ಶೂನ್ಯ)hasFolding(lnum, ಖಾಲಿ, &lnum_ಮುಂದಿನ);
	(ಶೂನ್ಯ)ಗಣ_Manual_Fold(lnum, opening, recurse, &ಮುಗಿದ);
	// Closing one level only: next line to close a fold is after just
	// closed fold.
	ಆದರೆ (!opening && !recurse)
	    (ಶೂನ್ಯ)hasFolding(lnum, ಖಾಲಿ, &lnum_ಮುಂದಿನ);
    }
    ಆದರೆ (ಮುಗಿದ == ಮುಗಿದ_NOTHING)
	emsg(_(e_no_fold_ಸಿಕ್ಕಿತು));
    // Force a redraw to remove the Visual highlighting.
    ಆದರೆ (had_visual)
	redraw_curbuf_later(UPD_INVERTED);
}

// openFold() {{{2
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ಶೂನ್ಯ
openFold(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಗಣ_Fold_Repeat(lnum, ಎಣಿಕೆ_2, TRUE);
}

// openFoldRecurse() {{{2
/*
 * Open fold for current window at line "lnum" recursively.
 */
    ಶೂನ್ಯ
openFoldRecurse(linenr_T lnum)
{
    (ಶೂನ್ಯ)ಗಣ_Manual_Fold(lnum, TRUE, TRUE, ಖಾಲಿ);
}

// foldOpenCursor() {{{2
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    ಶೂನ್ಯ
foldOpenCursor(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ;

    checkupdate(curwin);
    ಆದರೆ (hasAnyFolding(curwin))
	ಗಾಗಿ (;;)
	{
	    ಮುಗಿದ = ಮುಗಿದ_NOTHING;
	    (ಶೂನ್ಯ)ಗಣ_Manual_Fold(curwin->w_cursor.lnum, TRUE, FALSE, &ಮುಗಿದ);
	    ಆದರೆ (!(ಮುಗಿದ & ಮುಗಿದ_ACTION))
		ನಿಲ್ಲಿಸು;
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
    ಆದರೆ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;

	/*
	 * Set the same foldlevel in other windows in diff mode.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
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
    ಪೂರ್ಣಾಂಕ		i;

    checkupdate(wp);
    ಆದರೆ (wp->w_fold_manual)
    {
	// Set all flags for the first level of folds to FD_LEVEL.  Following
	// manual open/close will then change the flags to FD_OPEN or
	// FD_CLOSED for those folds that don't use 'foldlevel'.
	fp = (fold_T *)wp->w_folds.ga_ದತ್ತಾಂಶ;
	ಗಾಗಿ (i = 0; i < wp->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	wp->w_fold_manual = false;
    }
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);
}

// foldCheckClose() {{{2
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    ಶೂನ್ಯ
foldCheckClose(ಶೂನ್ಯ)
{
    ಆದರೆ (*p_fcl == NUL)
	ಹಿಂತಿರುಗಿ;

    // 'foldclose' can only be "all" right now
    checkupdate(curwin);
    ಆದರೆ (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
		(ಪೂರ್ಣಾಂಕ)curwin->w_p_fdl))
	ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
}

// checkCloseRec() {{{2
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
checkCloseRec(garray_T *gap, linenr_T lnum, ಪೂರ್ಣಾಂಕ level)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		i;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	// Only manually opened folds may need to be closed.
	ಆದರೆ (fp[i].fd_flags == FD_OPEN)
	{
	    ಆದರೆ (level <= 0 && (lnum < fp[i].fd_ತುದಿ
				      || lnum >= fp[i].fd_ತುದಿ + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_ತುದಿ,
								   level - 1);
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

// foldManualAllowed() {{{2
/*
 * Return TRUE if it's allowed to manually create or delete a fold.
 * Give an error message and return FALSE if not.
 */
    ಪೂರ್ಣಾಂಕ
foldManualAllowed(ಪೂರ್ಣಾಂಕ create)
{
    ಆದರೆ (foldmethodIsManual(curwin) || foldmethodIsMarker(curwin))
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (create)
	emsg(_(e_cannot_create_fold_with_current_foldmethod));
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_cannot_delete_fold_with_current_foldmethod));
    ಹಿಂತಿರುಗಿ FALSE;
}

// foldCreate() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    ಶೂನ್ಯ
foldCreate(linenr_T ಆರಂಭ, linenr_T ಕೊನೆ)
{
    fold_T	*fp;
    garray_T	*gap;
    garray_T	fold_ga;
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		cont;
    ಪೂರ್ಣಾಂಕ		use_level = FALSE;
    ಪೂರ್ಣಾಂಕ		closed = FALSE;
    ಪೂರ್ಣಾಂಕ		level = 0;
    linenr_T	ಆರಂಭ_rel = ಆರಂಭ;
    linenr_T	ಕೊನೆ_rel = ಕೊನೆ;

    ಆದರೆ (ಆರಂಭ > ಕೊನೆ)
    {
	// reverse the range
	ಕೊನೆ = ಆರಂಭ_rel;
	ಆರಂಭ = ಕೊನೆ_rel;
	ಆರಂಭ_rel = ಆರಂಭ;
	ಕೊನೆ_rel = ಕೊನೆ;
    }

    // When 'foldmethod' is "marker" add markers, which creates the folds.
    ಆದರೆ (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(ಆರಂಭ, ಕೊನೆ);
	ಹಿಂತಿರುಗಿ;
    }

    checkupdate(curwin);

    // Find the place to insert the new fold.
    gap = &curwin->w_folds;
    ಆದರೆ (gap->ga_len == 0)
	i = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, ಆರಂಭ_rel, &fp))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len > ಕೊನೆ_rel)
	    {
		// New fold is completely inside this fold: Go one level
		// deeper.
		gap = &fp->fd_nested;
		ಆರಂಭ_rel -= fp->fd_ತುದಿ;
		ಕೊನೆ_rel -= fp->fd_ತುದಿ;
		ಆದರೆ (use_level || fp->fd_flags == FD_LEVEL)
		{
		    use_level = TRUE;
		    ಆದರೆ (level >= curwin->w_p_fdl)
			closed = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_flags == FD_CLOSED)
		    closed = TRUE;
		++level;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// This fold and new fold overlap: Insert here and move some
		// folds inside the new fold.
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (gap->ga_len == 0)
	    i = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ);
    }

    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i;
    ga_init2(&fold_ga, ಗಾತ್ರ(fold_T), 10);

    // Count number of folds that will be contained in the new fold.
    ಗಾಗಿ (cont = 0; i + cont < gap->ga_len; ++cont)
	ಆದರೆ (fp[cont].fd_ತುದಿ > ಕೊನೆ_rel)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (cont > 0 && ga_grow(&fold_ga, cont) == OK)
    {
	// If the first fold starts before the new fold, let the new fold
	// start there.  Otherwise the existing fold would change.
	ಆದರೆ (ಆರಂಭ_rel > fp->fd_ತುದಿ)
	    ಆರಂಭ_rel = fp->fd_ತುದಿ;

	// When last contained fold isn't completely contained, adjust end
	// of new fold.
	ಆದರೆ (ಕೊನೆ_rel < fp[cont - 1].fd_ತುದಿ + fp[cont - 1].fd_len - 1)
	    ಕೊನೆ_rel = fp[cont - 1].fd_ತುದಿ + fp[cont - 1].fd_len - 1;
	// Move contained folds to inside new fold.
	mch_memmove(fold_ga.ga_ದತ್ತಾಂಶ, fp, ಗಾತ್ರ(fold_T) * cont);
	fold_ga.ga_len += cont;
	i += cont;

	// Adjust line numbers in contained folds to be relative to the
	// new fold.
	ಗಾಗಿ (j = 0; j < cont; ++j)
	    ((fold_T *)fold_ga.ga_ದತ್ತಾಂಶ)[j].fd_ತುದಿ -= ಆರಂಭ_rel;
    }
    // Move remaining entries to after the new fold.
    ಆದರೆ (i < gap->ga_len)
	mch_memmove(fp + 1, (fold_T *)gap->ga_ದತ್ತಾಂಶ + i,
		ಗಾತ್ರ(fold_T) * (gap->ga_len - i));
    gap->ga_len = gap->ga_len + 1 - cont;

    // insert new fold
    fp->fd_nested = fold_ga;
    fp->fd_ತುದಿ = ಆರಂಭ_rel;
    fp->fd_len = ಕೊನೆ_rel - ಆರಂಭ_rel + 1;

    // We want the new fold to be closed.  If it would remain open because
    // of using 'foldlevel', need to adjust fd_flags of containing folds.
    ಆದರೆ (use_level && !closed && level < curwin->w_p_fdl)
	closeFold(ಆರಂಭ, 1L);
    ಆದರೆ (!use_level)
	curwin->w_fold_manual = true;
    fp->fd_flags = FD_CLOSED;
    fp->fd_small = MAYBE;

    // redraw
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
}

// deleteFold() {{{2
/*
 * Delete a fold at line "start" in the current window.
 * When "end" is not 0, delete all folds from "start" to "end".
 * When "recursive" is TRUE delete recursively.
 */
    ಶೂನ್ಯ
deleteFold(
    linenr_T	ಆರಂಭ,
    linenr_T	ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		recursive,
    ಪೂರ್ಣಾಂಕ		had_visual)	// TRUE when Visual selection used
{
    garray_T	*gap;
    fold_T	*fp;
    garray_T	*ಸಿಕ್ಕಿತು_ga;
    fold_T	*ಸಿಕ್ಕಿತು_fp = ಖಾಲಿ;
    linenr_T	ಸಿಕ್ಕಿತು_off = 0;
    ಪೂರ್ಣಾಂಕ		use_level;
    ಪೂರ್ಣಾಂಕ		maybe_small = FALSE;
    ಪೂರ್ಣಾಂಕ		level = 0;
    linenr_T	lnum = ಆರಂಭ;
    linenr_T	lnum_off;
    ಪೂರ್ಣಾಂಕ		did_one = FALSE;
    linenr_T	ಮೊದಲ_lnum = MAXLNUM;
    linenr_T	ಕೊನೆಯ_lnum = 0;

    checkupdate(curwin);

    ಆಗ (lnum <= ಕೊನೆ)
    {
	// Find the deepest fold for "start".
	gap = &curwin->w_folds;
	ಸಿಕ್ಕಿತು_ga = ಖಾಲಿ;
	lnum_off = 0;
	use_level = FALSE;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, lnum - lnum_off, &fp))
		ನಿಲ್ಲಿಸು;
	    // lnum is inside this fold, remember info
	    ಸಿಕ್ಕಿತು_ga = gap;
	    ಸಿಕ್ಕಿತು_fp = fp;
	    ಸಿಕ್ಕಿತು_off = lnum_off;

	    // if "lnum" is folded, don't check nesting
	    ಆದರೆ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		ನಿಲ್ಲಿಸು;

	    // check nested folds
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ತುದಿ;
	    ++level;
	}
	ಆದರೆ (ಸಿಕ್ಕಿತು_ga == ಖಾಲಿ)
	{
	    ++lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    lnum = ಸಿಕ್ಕಿತು_fp->fd_ತುದಿ + ಸಿಕ್ಕಿತು_fp->fd_len + ಸಿಕ್ಕಿತು_off;

	    ಆದರೆ (foldmethodIsManual(curwin))
		delete_Fold_ನಮೂದು(ಸಿಕ್ಕಿತು_ga,
		    (ಪೂರ್ಣಾಂಕ)(ಸಿಕ್ಕಿತು_fp - (fold_T *)ಸಿಕ್ಕಿತು_ga->ga_ದತ್ತಾಂಶ), recursive);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಮೊದಲ_lnum > ಸಿಕ್ಕಿತು_fp->fd_ತುದಿ + ಸಿಕ್ಕಿತು_off)
		    ಮೊದಲ_lnum = ಸಿಕ್ಕಿತು_fp->fd_ತುದಿ + ಸಿಕ್ಕಿತು_off;
		ಆದರೆ (ಕೊನೆಯ_lnum < lnum)
		    ಕೊನೆಯ_lnum = lnum;
		ಆದರೆ (!did_one)
		    parseMarker(curwin);
		deleteFoldMarkers(ಸಿಕ್ಕಿತು_fp, recursive, ಸಿಕ್ಕಿತು_off);
	    }
	    did_one = TRUE;

	    // redraw window
	    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
	}
    }
    ಆದರೆ (!did_one)
    {
	emsg(_(e_no_fold_ಸಿಕ್ಕಿತು));
	// Force a redraw to remove the Visual highlighting.
	ಆದರೆ (had_visual)
	    redraw_curbuf_later(UPD_INVERTED);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Deleting markers may make cursor column invalid.
	check_cursor_ಲಂಬಸಾಲು();

    ಆದರೆ (ಕೊನೆಯ_lnum > 0)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ಮೊದಲ_lnum, (colnr_T)0, ಕೊನೆಯ_lnum, 0L);
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

    ಆದರೆ (disable_fold_update > 0)
	ಹಿಂತಿರುಗಿ;
# ifdef FEAT_DIFF
    ಆದರೆ (need_diff_redraw)
	// will update later
	ಹಿಂತಿರುಗಿ;
# endif

    ಆದರೆ (wp->w_folds.ga_len > 0)
    {
	linenr_T	maybe_small_ಆರಂಭ = ತುದಿ;
	linenr_T	maybe_small_ಕೊನೆ = bot;

	// Mark all folds from top to bot (or bot to top) as maybe-small.
	ಆದರೆ (ತುದಿ > bot)
	{
	    maybe_small_ಆರಂಭ = bot;
	    maybe_small_ಕೊನೆ = ತುದಿ;
	}
	(ಶೂನ್ಯ)fold_ಕಂಡುಹಿಡಿ(&wp->w_folds, maybe_small_ಆರಂಭ, &fp);
	ಆಗ (fp < (fold_T *)wp->w_folds.ga_ದತ್ತಾಂಶ + wp->w_folds.ga_len
		&& fp->fd_ತುದಿ <= maybe_small_ಕೊನೆ)
	{
	    fp->fd_small = MAYBE;
	    ++fp;
	}
    }

    ಆದರೆ (foldmethodIsIndent(wp)
	    || foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(wp)
	    || foldmethodIsMarker(wp)
# ifdef FEAT_DIFF
	    || foldmethodIsDiff(wp)
# endif
	    || foldmethodIsSyntax(wp))
    {
	ಪೂರ್ಣಾಂಕ save_got_int = got_int;

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
    ಪೂರ್ಣಾಂಕ
fold_Move_ಗಮ್ಯ(
    ಪೂರ್ಣಾಂಕ		updown,
    ಪೂರ್ಣಾಂಕ		dir,	    // FORWARD or BACKWARD
    ದೀರ್ಘ	ಎಣಿಕೆ_2)
{
    ದೀರ್ಘ	n;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    linenr_T	lnum_off;
    linenr_T	lnum_ಸಿಕ್ಕಿತು;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		use_level;
    ಪೂರ್ಣಾಂಕ		maybe_small;
    garray_T	*gap;
    fold_T	*fp;
    ಪೂರ್ಣಾಂಕ		level;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ;

    checkupdate(curwin);

    // Repeat "count" times.
    ಗಾಗಿ (n = 0; n < ಎಣಿಕೆ_2; ++n)
    {
	// Find nested folds.  Stop when a fold is closed.  The deepest fold
	// that moves the cursor is used.
	lnum_off = 0;
	gap = &curwin->w_folds;
	ಆದರೆ (gap->ga_len == 0)
	    ನಿಲ್ಲಿಸು;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_ಸಿಕ್ಕಿತು = curwin->w_cursor.lnum;
	level = 0;
	ಕೊನೆಯ = FALSE;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		ಆದರೆ (!updown || gap->ga_len == 0)
		    ನಿಲ್ಲಿಸು;

		// When moving up, consider a fold above the cursor; when
		// moving down consider a fold below the cursor.
		ಆದರೆ (dir == FORWARD)
		{
		    ಆದರೆ (fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ >= gap->ga_len)
			ನಿಲ್ಲಿಸು;
		    --fp;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (fp == (fold_T *)gap->ga_ದತ್ತಾಂಶ)
			ನಿಲ್ಲಿಸು;
		}
		// don't look for contained folds, they will always move
		// the cursor too far.
		ಕೊನೆಯ = TRUE;
	    }

	    ಆದರೆ (!ಕೊನೆಯ)
	    {
		// Check if this fold is closed.
		ಆದರೆ (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    ಕೊನೆಯ = TRUE;

		// "[z" and "]z" stop at closed fold
		ಆದರೆ (ಕೊನೆಯ && !updown)
		    ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (updown)
	    {
		ಆದರೆ (dir == FORWARD)
		{
		    // to start of next fold if there is one
		    ಆದರೆ (fp + 1 - (fold_T *)gap->ga_ದತ್ತಾಂಶ < gap->ga_len)
		    {
			lnum = fp[1].fd_ತುದಿ + lnum_off;
			ಆದರೆ (lnum > curwin->w_cursor.lnum)
			    lnum_ಸಿಕ್ಕಿತು = lnum;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // to end of previous fold if there is one
		    ಆದರೆ (fp > (fold_T *)gap->ga_ದತ್ತಾಂಶ)
		    {
			lnum = fp[-1].fd_ತುದಿ + lnum_off + fp[-1].fd_len - 1;
			ಆದರೆ (lnum < curwin->w_cursor.lnum)
			    lnum_ಸಿಕ್ಕಿತು = lnum;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Open fold found, set cursor to its start/end and then check
		// nested folds.
		ಆದರೆ (dir == FORWARD)
		{
		    lnum = fp->fd_ತುದಿ + lnum_off + fp->fd_len - 1;
		    ಆದರೆ (lnum > curwin->w_cursor.lnum)
			lnum_ಸಿಕ್ಕಿತು = lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    lnum = fp->fd_ತುದಿ + lnum_off;
		    ಆದರೆ (lnum < curwin->w_cursor.lnum)
			lnum_ಸಿಕ್ಕಿತು = lnum;
		}
	    }

	    ಆದರೆ (ಕೊನೆಯ)
		ನಿಲ್ಲಿಸು;

	    // Check nested folds (if any).
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_ತುದಿ;
	    ++level;
	}
	ಆದರೆ (lnum_ಸಿಕ್ಕಿತು != curwin->w_cursor.lnum)
	{
	    ಆದರೆ (retval == FAIL)
		setpcmark();
	    curwin->w_cursor.lnum = lnum_ಸಿಕ್ಕಿತು;
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    retval = OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ retval;
}

// foldInitWin() {{{2
/*
 * Init the fold info in a new window.
 */
    ಶೂನ್ಯ
foldInitWin(win_T *new_win)
{
    ga_init2(&new_win->w_folds, ಗಾತ್ರ(fold_T), 10);
}

// find_wl_entry() {{{2
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_wl_ನಮೂದು(win_T *win, linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < win->w_ಸಾಲುಗಳು_valid; ++i)
	ಆದರೆ (win->w_ಸಾಲುಗಳು[i].wl_valid)
	{
	    ಆದರೆ (lnum < win->w_ಸಾಲುಗಳು[i].wl_lnum)
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (lnum <= win->w_ಸಾಲುಗಳು[i].wl_lastlnum)
		ಹಿಂತಿರುಗಿ i;
	}
    ಹಿಂತಿರುಗಿ -1;
}

// foldAdjustVisual() {{{2
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    ಶೂನ್ಯ
foldAdjustVisual(ಶೂನ್ಯ)
{
    pos_T	*ಆರಂಭ, *ಕೊನೆ;

    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ || !hasAnyFolding(curwin))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (LTOREQ_POS(VIsual, curwin->w_cursor))
    {
	ಆರಂಭ = &VIsual;
	ಕೊನೆ = &curwin->w_cursor;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆರಂಭ = &curwin->w_cursor;
	ಕೊನೆ = &VIsual;
    }
    ಆದರೆ (hasFolding(ಆರಂಭ->lnum, &ಆರಂಭ->lnum, ಖಾಲಿ))
	ಆರಂಭ->ಲಂಬಸಾಲು = 0;

    ಆದರೆ (!hasFolding(ಕೊನೆ->lnum, ಖಾಲಿ, &ಕೊನೆ->lnum))
	ಹಿಂತಿರುಗಿ;

    ಕೊನೆ->ಲಂಬಸಾಲು = ml_get_len(ಕೊನೆ->lnum);
    ಆದರೆ (ಕೊನೆ->ಲಂಬಸಾಲು > 0 && *p_sel == 'o')
	--ಕೊನೆ->ಲಂಬಸಾಲು;
    // prevent cursor from moving on the trail byte
    ಆದರೆ (has_mbyte)
	mb_adjust_cursor();
}

// foldAdjustCursor() {{{2
/*
 * Move the cursor to the first line of a closed fold.
 */
    ಶೂನ್ಯ
foldAdjustCursor(ಶೂನ್ಯ)
{
    (ಶೂನ್ಯ)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ಖಾಲಿ);
}

// Internal functions for "fold_T" {{{1
// cloneFoldGrowArray() {{{2
/*
 * Will "clone" (i.e deep copy) a garray_T of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    ಶೂನ್ಯ
cloneFoldGrowArray(garray_T *from, garray_T *ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    fold_T	*from_p;
    fold_T	*ಗಮ್ಯ_p;

    ga_init2(ಗಮ್ಯ, from->ga_itemsize, from->ga_growsize);
    ಆದರೆ (from->ga_len == 0 || ga_grow(ಗಮ್ಯ, from->ga_len) == FAIL)
	ಹಿಂತಿರುಗಿ;

    from_p = (fold_T *)from->ga_ದತ್ತಾಂಶ;
    ಗಮ್ಯ_p = (fold_T *)ಗಮ್ಯ->ga_ದತ್ತಾಂಶ;

    ಗಾಗಿ (i = 0; i < from->ga_len; i++)
    {
	ಗಮ್ಯ_p->fd_ತುದಿ = from_p->fd_ತುದಿ;
	ಗಮ್ಯ_p->fd_len = from_p->fd_len;
	ಗಮ್ಯ_p->fd_flags = from_p->fd_flags;
	ಗಮ್ಯ_p->fd_small = from_p->fd_small;
	cloneFoldGrowArray(&from_p->fd_nested, &ಗಮ್ಯ_p->fd_nested);
	++ಗಮ್ಯ->ga_len;
	++from_p;
	++ಗಮ್ಯ_p;
    }
}

// foldFind() {{{2
/*
 * Search for line "lnum" in folds of growarray "gap".
 * Set "*fpp" to the fold struct for the fold that contains "lnum" or
 * the first fold below it (careful: it can be beyond the end of the array!).
 * Returns FALSE when there is no fold that contains "lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fold_ಕಂಡುಹಿಡಿ(garray_T *gap, linenr_T lnum, fold_T **fpp)
{
    linenr_T	ಕೆಳಗಿನ, ಮೇಲಿನ;
    fold_T	*fp;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (gap->ga_len == 0)
    {
	*fpp = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ;
    ಕೆಳಗಿನ = 0;
    ಮೇಲಿನ = gap->ga_len - 1;
    ಆಗ (ಕೆಳಗಿನ <= ಮೇಲಿನ)
    {
	i = (ಕೆಳಗಿನ + ಮೇಲಿನ) / 2;
	ಆದರೆ (fp[i].fd_ತುದಿ > lnum)
	    // fold below lnum, adjust high
	    ಮೇಲಿನ = i - 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp[i].fd_ತುದಿ + fp[i].fd_len <= lnum)
	    // fold above lnum, adjust low
	    ಕೆಳಗಿನ = i + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // lnum is inside this fold
	    *fpp = fp + i;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    *fpp = fp + ಕೆಳಗಿನ;
    ಹಿಂತಿರುಗಿ FALSE;
}

// foldLevelWin() {{{2
/*
 * Return fold level at line number "lnum" in window "wp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
foldLevelWin(win_T *wp, linenr_T lnum)
{
    fold_T	*fp;
    linenr_T	lnum_rel = lnum;
    ಪೂರ್ಣಾಂಕ		level =  0;
    garray_T	*gap;

    // Recursively search for a fold that contains "lnum".
    gap = &wp->w_folds;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, lnum_rel, &fp))
	    ನಿಲ್ಲಿಸು;
	// Check nested folds.  Line number is relative to containing fold.
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_ತುದಿ;
	++level;
    }

    ಹಿಂತಿರುಗಿ level;
}

// checkupdate() {{{2
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
checkupdate(win_T *wp)
{
    ಆದರೆ (!wp->w_foldinvalid)
	ಹಿಂತಿರುಗಿ;

    foldUpdate(wp, (linenr_T)1, (linenr_T)MAXLNUM); // will update all
    wp->w_foldinvalid = false;
}

// setFoldRepeat() {{{2
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_Fold_Repeat(linenr_T lnum, ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ do_open)
{
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ;
    ದೀರ್ಘ	n;

    ಗಾಗಿ (n = 0; n < ಎಣಿಕೆ_2; ++n)
    {
	ಮುಗಿದ = ಮುಗಿದ_NOTHING;
	(ಶೂನ್ಯ)ಗಣ_Manual_Fold(lnum, do_open, FALSE, &ಮುಗಿದ);
	ಆದರೆ (!(ಮುಗಿದ & ಮುಗಿದ_ACTION))
	{
	    // Only give an error message when no fold could be opened.
	    ಆದರೆ (n == 0 && !(ಮುಗಿದ & ಮುಗಿದ_FOLD))
		emsg(_(e_no_fold_ಸಿಕ್ಕಿತು));
	    ನಿಲ್ಲಿಸು;
	}
    }
}

// setManualFold() {{{2
/*
 * Open or close the fold in the current window which contains "lnum".
 * Also does this for other windows in diff mode when needed.
 */
    ಸ್ಥಿರ linenr_T
ಗಣ_Manual_Fold(
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		opening,    // TRUE when opening, FALSE when closing
    ಪೂರ್ಣಾಂಕ		recurse,    // TRUE when closing/opening recursive
    ಪೂರ್ಣಾಂಕ		*donep)
{
# ifdef FEAT_DIFF
    ಆದರೆ (foldmethodIsDiff(curwin) && curwin->w_p_scb)
    {
	win_T	    *wp;
	linenr_T    dlnum;

	/*
	 * Do the same operation in other windows in diff mode.  Calculate the
	 * line number from the diffs.
	 */
	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
	    {
		dlnum = diff_lnum_win(curwin->w_cursor.lnum, wp);
		ಆದರೆ (dlnum != 0)
		    (ಶೂನ್ಯ)ಗಣ_Manual_Fold_Win(wp, dlnum, opening, recurse, ಖಾಲಿ);
	    }
	}
    }
# endif

    ಹಿಂತಿರುಗಿ ಗಣ_Manual_Fold_Win(curwin, lnum, opening, recurse, donep);
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
ಗಣ_Manual_Fold_Win(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		opening,    // TRUE when opening, FALSE when closing
    ಪೂರ್ಣಾಂಕ		recurse,    // TRUE when closing/opening recursive
    ಪೂರ್ಣಾಂಕ		*donep)
{
    fold_T	*fp;
    fold_T	*fp2;
    fold_T	*ಸಿಕ್ಕಿತು = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		j;
    ಪೂರ್ಣಾಂಕ		level = 0;
    ಪೂರ್ಣಾಂಕ		use_level = FALSE;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_fold = FALSE;
    garray_T	*gap;
    linenr_T	ಮುಂದಿನ = MAXLNUM;
    linenr_T	off = 0;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = 0;

    checkupdate(wp);

    /*
     * Find the fold, open or close it.
     */
    gap = &wp->w_folds;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, lnum, &fp))
	{
	    // If there is a following fold, continue there next time.
	    ಆದರೆ (fp != ಖಾಲಿ && fp < (fold_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len)
		ಮುಂದಿನ = fp->fd_ತುದಿ + off;
	    ನಿಲ್ಲಿಸು;
	}

	// lnum is inside this fold
	ಸಿಕ್ಕಿತು_fold = TRUE;

	// If there is a following fold, continue there next time.
	ಆದರೆ (fp + 1 < (fold_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len)
	    ಮುಂದಿನ = fp[1].fd_ತುದಿ + off;

	// Change from level-dependent folding to manual.
	ಆದರೆ (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    ಆದರೆ (level >= wp->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    ಇಲ್ಲದಿದ್ದರೆ
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_T *)fp->fd_nested.ga_ದತ್ತಾಂಶ;
	    ಗಾಗಿ (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	// Simple case: Close recursively means closing the fold.
	ಆದರೆ (!opening && recurse)
	{
	    ಆದರೆ (fp->fd_flags != FD_CLOSED)
	    {
		ಮುಗಿದ |= ಮುಗಿದ_ACTION;
		fp->fd_flags = FD_CLOSED;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_flags == FD_CLOSED)
	{
	    // When opening, open topmost closed fold.
	    ಆದರೆ (opening)
	    {
		fp->fd_flags = FD_OPEN;
		ಮುಗಿದ |= ಮುಗಿದ_ACTION;
		ಆದರೆ (recurse)
		    foldOpenNested(fp);
	    }
	    ನಿಲ್ಲಿಸು;
	}

	// fold is open, check nested folds
	ಸಿಕ್ಕಿತು = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_ತುದಿ;
	off += fp->fd_ತುದಿ;
	++level;
    }
    ಆದರೆ (ಸಿಕ್ಕಿತು_fold)
    {
	// When closing and not recurse, close deepest open fold.
	ಆದರೆ (!opening && ಸಿಕ್ಕಿತು != ಖಾಲಿ)
	{
	    ಸಿಕ್ಕಿತು->fd_flags = FD_CLOSED;
	    ಮುಗಿದ |= ಮುಗಿದ_ACTION;
	}
	wp->w_fold_manual = true;
	ಆದರೆ (ಮುಗಿದ & ಮುಗಿದ_ACTION)
	    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);
	ಮುಗಿದ |= ಮುಗಿದ_FOLD;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (donep == ಖಾಲಿ && wp == curwin)
	emsg(_(e_no_fold_ಸಿಕ್ಕಿತು));

    ಆದರೆ (donep != ಖಾಲಿ)
	*donep |= ಮುಗಿದ;

    ಹಿಂತಿರುಗಿ ಮುಂದಿನ;
}

// foldOpenNested() {{{2
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldOpenNested(fold_T *fpr)
{
    ಪೂರ್ಣಾಂಕ		i;
    fold_T	*fp;

    fp = (fold_T *)fpr->fd_nested.ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < fpr->fd_nested.ga_len; ++i)
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
delete_Fold_ನಮೂದು(garray_T *gap, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ recursive)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಂಕ		i;
    ದೀರ್ಘ	moved;
    fold_T	*nfp;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + idx;
    ಆದರೆ (recursive || fp->fd_nested.ga_len == 0)
    {
	// recursively delete the contained folds
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	ಆದರೆ (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, ಗಾತ್ರ(fold_T) * (gap->ga_len - idx));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Move nested folds one level up, to overwrite the fold that is
	// deleted.
	moved = fp->fd_nested.ga_len;
	ಆದರೆ (ga_grow(gap, (ಪೂರ್ಣಾಂಕ)(moved - 1)) == OK)
	{
	    // Get "fp" again, the array may have been reallocated.
	    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + idx;

	    // adjust fd_top and fd_flags for the moved folds
	    nfp = (fold_T *)fp->fd_nested.ga_ದತ್ತಾಂಶ;
	    ಗಾಗಿ (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_ತುದಿ += fp->fd_ತುದಿ;
		ಆದರೆ (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		ಆದರೆ (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    // move the existing folds down to make room
	    ಆದರೆ (idx + 1 < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
				  ಗಾತ್ರ(fold_T) * (gap->ga_len - (idx + 1)));
	    // move the contained folds one level up
	    mch_memmove(fp, nfp, (size_t)(ಗಾತ್ರ(fold_T) * moved));
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
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_T *)(gap->ga_ದತ್ತಾಂಶ))[i].fd_nested));
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
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after)
{
    // If deleting marks from line1 to line2, but not deleting all those
    // lines, set line2 so that only deleted lines have their folds removed.
    ಆದರೆ (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    ಆದರೆ (line2 < line1)
	line2 = line1;
    // If appending a line in Insert mode, it should be included in the fold
    // just above the line.
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && amount == (linenr_T)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

// foldMarkAdjustRecurse() {{{2
    ಸ್ಥಿರ ಶೂನ್ಯ
foldMarkAdjustRecurse(
    garray_T	*gap,
    linenr_T	line1,
    linenr_T	line2,
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after)
{
    fold_T	*fp;
    ಪೂರ್ಣಾಂಕ		i;
    linenr_T	ಕೊನೆಯ;
    linenr_T	ತುದಿ;

    ಆದರೆ (gap->ga_len == 0)
	ಹಿಂತಿರುಗಿ;

    // In Insert mode an inserted line at the top of a fold is considered part
    // of the fold, otherwise it isn't.
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && amount == (linenr_T)1 && line2 == MAXLNUM)
	ತುದಿ = line1 + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	ತುದಿ = line1;

    // Find the fold containing or just below "line1".
    (ಶೂನ್ಯ)fold_ಕಂಡುಹಿಡಿ(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    ಗಾಗಿ (i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ); i < gap->ga_len; ++i, ++fp)
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

	ಕೊನೆಯ = fp->fd_ತುದಿ + fp->fd_len - 1; // last line of fold

	// 1. fold completely above line1: nothing to do
	ಆದರೆ (ಕೊನೆಯ < line1)
	    ಮುಂದುವರಿ;

	// 6. fold below line2: only adjust for amount_after
	ಆದರೆ (fp->fd_ತುದಿ > line2)
	{
	    ಆದರೆ (amount_after == 0)
		ನಿಲ್ಲಿಸು;
	    fp->fd_ತುದಿ += amount_after;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fp->fd_ತುದಿ >= ತುದಿ && ಕೊನೆಯ <= line2)
	    {
		// 4. fold completely contained in range
		ಆದರೆ (amount == MAXLNUM)
		{
		    // Deleting lines: delete the fold completely
		    delete_Fold_ನಮೂದು(gap, i, TRUE);
		    --i;    // adjust index for deletion
		    --fp;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    fp->fd_ತುದಿ += amount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (fp->fd_ತುದಿ < ತುದಿ)
		{
		    // 2 or 3: need to correct nested folds too
		    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ತುದಿ,
				  line2 - fp->fd_ತುದಿ, amount, amount_after);
		    ಆದರೆ (ಕೊನೆಯ <= line2)
		    {
			// 2. fold contains line1, line2 is below fold
			ಆದರೆ (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_ತುದಿ;
			ಇಲ್ಲದಿದ್ದರೆ
			    fp->fd_len += amount;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// 3. fold contains line1 and line2
			fp->fd_len += amount_after;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // 5. fold is below line1 and contains line2; need to
		    // correct nested folds too
		    ಆದರೆ (amount == MAXLNUM)
		    {
			foldMarkAdjustRecurse(&fp->fd_nested,
				  0,
				  line2 - fp->fd_ತುದಿ,
				  amount,
				  amount_after + (fp->fd_ತುದಿ - ತುದಿ));
			fp->fd_len -= line2 - fp->fd_ತುದಿ + 1;
			fp->fd_ತುದಿ = line1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ
get_ಅತ್ಯಂತಆಳ_Nesting(ಶೂನ್ಯ)
{
    checkupdate(curwin);
    ಹಿಂತಿರುಗಿ get_ಅತ್ಯಂತಆಳ_Nesting_Recurse(&curwin->w_folds);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಅತ್ಯಂತಆಳ_Nesting_Recurse(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		level;
    ಪೂರ್ಣಾಂಕ		maxlevel = 0;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	level = get_ಅತ್ಯಂತಆಳ_Nesting_Recurse(&fp[i].fd_nested) + 1;
	ಆದರೆ (level > maxlevel)
	    maxlevel = level;
    }

    ಹಿಂತಿರುಗಿ maxlevel;
}

// check_closed() {{{2
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_closed(
    win_T	*win,
    fold_T	*fp,
    ಪೂರ್ಣಾಂಕ		*use_levelp,	    // TRUE: outer fold had FD_LEVEL
    ಪೂರ್ಣಾಂಕ		level,		    // folding depth
    ಪೂರ್ಣಾಂಕ		*maybe_smallp,	    // TRUE: outer this had fd_small == MAYBE
    linenr_T	lnum_off)	    // line number offset for fp->fd_top
{
    ಪೂರ್ಣಾಂಕ		closed = FALSE;

    // Check if this fold is closed.  If the flag is FD_LEVEL this
    // fold and all folds it contains depend on 'foldlevel'.
    ಆದರೆ (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	ಆದರೆ (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    // Small fold isn't closed anyway.
    ಆದರೆ (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    ಆದರೆ (closed)
    {
	ಆದರೆ (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	ಆದರೆ (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    ಹಿಂತಿರುಗಿ closed;
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
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (fp->fd_small != MAYBE)
	ಹಿಂತಿರುಗಿ;

    // Mark any nested folds to maybe-small
    ಗಣ_Small_Maybe(&fp->fd_nested);

    ಆದರೆ (fp->fd_len > wp->w_p_fml)
	fp->fd_small = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಎಣಿಕೆ_2 = 0;
	ಗಾಗಿ (n = 0; n < fp->fd_len; ++n)
	{
	    ಎಣಿಕೆ_2 += plines_win_nofold(wp, fp->fd_ತುದಿ + lnum_off + n);
	    ಆದರೆ (ಎಣಿಕೆ_2 > wp->w_p_fml)
	    {
		fp->fd_small = FALSE;
		ಹಿಂತಿರುಗಿ;
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
ಗಣ_Small_Maybe(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

// foldCreateMarkers() {{{2
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldCreateMarkers(linenr_T ಆರಂಭ, linenr_T ಕೊನೆ)
{
    ಆದರೆ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಹಿಂತಿರುಗಿ;
    }
    parseMarker(curwin);

    fold_ಕೂಡಿಸು_Marker(ಆರಂಭ, curwin->w_p_fmr, foldstartmarkerlen);
    fold_ಕೂಡಿಸು_Marker(ಕೊನೆ, foldendmarker, foldendmarkerlen);

    // Update both changes here, to avoid all folds after the start are
    // changed when the start marker is inserted and the end isn't.
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ಆರಂಭ, (colnr_T)0, ಕೊನೆ, 0L);
}

// foldAddMarker() {{{2
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fold_ಕೂಡಿಸು_Marker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಂಕ markerlen)
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_len;
    char_u	*newline;
    char_u	*p = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)curbuf->b_p_cms, "%s");
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_is_comment = FALSE;

    // Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end
    ಸಾಲು = ml_get(lnum);
    ಸಾಲು_len = ml_get_len(lnum);

    ಆದರೆ (u_save(lnum - 1, lnum + 1) != OK)
	ಹಿಂತಿರುಗಿ;

    // Check if the line ends with an unclosed comment
    (ಶೂನ್ಯ)skip_comment(ಸಾಲು, FALSE, FALSE, &ಸಾಲು_is_comment);
    newline = alloc(ಸಾಲು_len + markerlen + STRLEN(cms) + 1);
    ಆದರೆ (newline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    STRCPY(newline, ಸಾಲು);
    // Append the marker to the end of the line
    ಆದರೆ (p == ಖಾಲಿ || ಸಾಲು_is_comment)
	vim_strncpy(newline + ಸಾಲು_len, marker, markerlen);
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ		recursive,
    linenr_T	lnum_off)	// offset for fp->fd_top
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (recursive)
	ಗಾಗಿ (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_T *)fp->fd_nested.ga_ದತ್ತಾಂಶ + i, TRUE,
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
foldDelMarker(linenr_T lnum, char_u *marker, ಪೂರ್ಣಾಂಕ markerlen)
{
    char_u	*ಸಾಲು;
    char_u	*newline;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    // end marker may be missing and fold extends below the last line
    ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;
    ಸಾಲು = ml_get(lnum);
    ಗಾಗಿ (p = ಸಾಲು; *p != NUL; ++p)
	ಆದರೆ (STRNCMP(p, marker, markerlen) == 0)
	{
	    // Found the marker, include a digit if it's there.
	    len = markerlen;
	    ಆದರೆ (VIM_ISDIGIT(p[len]))
		++len;
	    ಆದರೆ (*cms != NUL)
	    {
		// Also delete 'commentstring' if it matches.
		cms2 = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)cms, "%s");
		ಆದರೆ (cms2 != ಖಾಲಿ && p - ಸಾಲು >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += (ಪೂರ್ಣಾಂಕ)STRLEN(cms) - 2;
		}
	    }
	    ಆದರೆ (u_save(lnum - 1, lnum + 1) == OK)
	    {
		// Make new line: text-before-marker + text-after-marker
		newline = alloc(ml_get_len(lnum) - len + 1);
		ಆದರೆ (newline != ಖಾಲಿ)
		{
		    STRNCPY(newline, ಸಾಲು, p - ಸಾಲು);
		    STRCPY(newline + (p - ಸಾಲು), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    ನಿಲ್ಲಿಸು;
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
    char_u	*ಪಠ್ಯ = ಖಾಲಿ;
# ifdef FEAT_EVAL
     // an error occurred when evaluating 'fdt' setting
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    got_fdt_error = FALSE;
    ಪೂರ್ಣಾಂಕ		    save_did_emsg = did_emsg;
    ಸ್ಥಿರ win_T    *ಕೊನೆಯ_wp = ಖಾಲಿ;
    ಸ್ಥಿರ linenr_T ಕೊನೆಯ_lnum = 0;

    ಆದರೆ (ಕೊನೆಯ_wp != wp || ಕೊನೆಯ_wp == ಖಾಲಿ
					|| ಕೊನೆಯ_lnum > lnum || ಕೊನೆಯ_lnum == 0)
	// window changed, try evaluating foldtext setting once again
	got_fdt_error = FALSE;

    ಆದರೆ (!got_fdt_error)
	// a previous error should not abort evaluating 'foldexpr'
	did_emsg = FALSE;

    ಆದರೆ (*wp->w_p_fdt != NUL)
    {
	char_u	dashes[MAX_LEVEL + 2];
	ಪೂರ್ಣಾಂಕ	level;
	char_u	*p;

	// Set "v:foldstart" and "v:foldend".
	ಗಣ_vim_var_nr(VV_FOLDSTART, lnum);
	ಗಣ_vim_var_nr(VV_FOLDEND, lnume);

	// Set "v:folddashes" to a string of "level" dashes.
	// Set "v:foldlevel" to "level".
	level = foldinfo->fi_level;
	ಆದರೆ (level > (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(dashes) - 1)
	    level = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(dashes) - 1;
	vim_memset(dashes, '-', (size_t)level);
	dashes[level] = NUL;
	ಗಣ_vim_var_string(VV_FOLDDASHES, dashes, level);
	ಗಣ_vim_var_nr(VV_FOLDLEVEL, (ದೀರ್ಘ)level);

	// skip evaluating 'foldtext' on errors
	ಆದರೆ (!got_fdt_error)
	{
	    win_T   *save_curwin = curwin;
	    sctx_T  saved_sctx = current_sctx;

	    curwin = wp;
	    curbuf = wp->w_ಬಫರ್;
	    current_sctx = wp->w_p_script_ctx[WV_FDT];

	    ++emsg_off; // handle exceptions, but don't display errors
	    ಪಠ್ಯ = eval_ಗಮ್ಯ_string_ಸುರಕ್ಷಿತ(wp->w_p_fdt,
		       was_ಗಣ_insecurely(wp, (char_u *)"foldtext", OPT_LOCAL),
		       TRUE, TRUE);
	    --emsg_off;

	    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ || did_emsg)
		got_fdt_error = TRUE;

	    curwin = save_curwin;
	    curbuf = curwin->w_ಬಫರ್;
	    current_sctx = saved_sctx;
	}
	ಕೊನೆಯ_lnum = lnum;
	ಕೊನೆಯ_wp   = wp;
	ಗಣ_vim_var_string(VV_FOLDDASHES, ಖಾಲಿ, -1);

	ಆದರೆ (!did_emsg && save_did_emsg)
	    did_emsg = save_did_emsg;

	ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
	{
	    // Replace unprintable characters, if there are any.  But
	    // replace a TAB with a space.
	    ಗಾಗಿ (p = ಪಠ್ಯ; *p != NUL; ++p)
	    {
		ಪೂರ್ಣಾಂಕ	len;

		ಆದರೆ (has_mbyte && (len = (*mb_ptr2len)(p)) > 1)
		{
		    ಆದರೆ (!vim_isprintc((*mb_ptr2char)(p)))
			ನಿಲ್ಲಿಸು;
		    p += len - 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಆದರೆ (*p == TAB)
			*p = ' ';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ptr2cells(p) > 1)
			ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (*p != NUL)
	    {
		p = transstr(ಪಠ್ಯ);
		vim_free(ಪಠ್ಯ);
		ಪಠ್ಯ = p;
	    }
	}
    }
    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
# endif
    {
	ದೀರ್ಘ ಎಣಿಕೆ_2 = (ದೀರ್ಘ)(lnume - lnum + 1);

	vim_snprintf((ಅಕ್ಷರ *)buf, FOLD_ಪಠ್ಯ_LEN,
		     NGETTEXT("+--%3ld line folded ",
					       "+--%3ld lines folded ", ಎಣಿಕೆ_2),
		     ಎಣಿಕೆ_2);
	ಪಠ್ಯ = buf;
    }
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ;
}

// foldtext_cleanup() {{{2
# ifdef FEAT_EVAL
/*
 * Remove 'foldmarker' and 'commentstring' from "str" (in-place).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldtext_cleanup(char_u *str)
{
    char_u	*cms_ಆರಂಭ;	// first part or the whole comment
    ಪೂರ್ಣಾಂಕ		cms_slen = 0;	// length of cms_start
    char_u	*cms_ಕೊನೆ;	// last part of the comment or NULL
    ಪೂರ್ಣಾಂಕ		cms_elen = 0;	// length of cms_end
    char_u	*s;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		did1 = FALSE;
    ಪೂರ್ಣಾಂಕ		did2 = FALSE;

    // Ignore leading and trailing white space in 'commentstring'.
    cms_ಆರಂಭ = skipwhite(curbuf->b_p_cms);
    cms_slen = (ಪೂರ್ಣಾಂಕ)STRLEN(cms_ಆರಂಭ);
    ಆಗ (cms_slen > 0 && VIM_ISWHITE(cms_ಆರಂಭ[cms_slen - 1]))
	--cms_slen;

    // locate "%s" in 'commentstring', use the part before and after it.
    cms_ಕೊನೆ = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)cms_ಆರಂಭ, "%s");
    ಆದರೆ (cms_ಕೊನೆ != ಖಾಲಿ)
    {
	cms_elen = cms_slen - (ಪೂರ್ಣಾಂಕ)(cms_ಕೊನೆ - cms_ಆರಂಭ);
	cms_slen = (ಪೂರ್ಣಾಂಕ)(cms_ಕೊನೆ - cms_ಆರಂಭ);

	// exclude white space before "%s"
	ಆಗ (cms_slen > 0 && VIM_ISWHITE(cms_ಆರಂಭ[cms_slen - 1]))
	    --cms_slen;

	// skip "%s" and white space after it
	s = skipwhite(cms_ಕೊನೆ + 2);
	cms_elen -= (ಪೂರ್ಣಾಂಕ)(s - cms_ಕೊನೆ);
	cms_ಕೊನೆ = s;
    }
    parseMarker(curwin);

    ಗಾಗಿ (s = str; *s != NUL; )
    {
	len = 0;
	ಆದರೆ (STRNCMP(s, curwin->w_p_fmr, foldstartmarkerlen) == 0)
	    len = foldstartmarkerlen;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, foldendmarker, foldendmarkerlen) == 0)
	    len = foldendmarkerlen;
	ಆದರೆ (len > 0)
	{
	    ಆದರೆ (VIM_ISDIGIT(s[len]))
		++len;

	    // May remove 'commentstring' start.  Useful when it's a double
	    // quote and we already removed a double quote.
	    ಗಾಗಿ (p = s; p > str && VIM_ISWHITE(p[-1]); --p)
		;
	    ಆದರೆ (p >= str + cms_slen
			   && STRNCMP(p - cms_slen, cms_ಆರಂಭ, cms_slen) == 0)
	    {
		len += (ಪೂರ್ಣಾಂಕ)(s - p) + cms_slen;
		s = p - cms_slen;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cms_ಕೊನೆ != ಖಾಲಿ)
	{
	    ಆದರೆ (!did1 && cms_slen > 0 && STRNCMP(s, cms_ಆರಂಭ, cms_slen) == 0)
	    {
		len = cms_slen;
		did1 = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!did2 && cms_elen > 0
					&& STRNCMP(s, cms_ಕೊನೆ, cms_elen) == 0)
	    {
		len = cms_elen;
		did2 = TRUE;
	    }
	}
	ಆದರೆ (len != 0)
	{
	    ಆಗ (VIM_ISWHITE(s[len]))
		++len;
	    STRMOVE(s, s + len);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    MB_PTR_ADV(s);
	}
    }
}
# endif

// Folding by indent, expr, marker and syntax. {{{1
// Define "fline_T", passed to get fold level for a line. {{{2
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    win_T	*wp;		//ಕಿಟಕಿ
    linenr_T	lnum;		// current line number
    linenr_T	off;		// offset between lnum and real line number
    linenr_T	lnum_save;	// line nr used by foldUpdateIEMSRecurse()
    ಪೂರ್ಣಾಂಕ		lvl;		// current level (-1 for undefined)
    ಪೂರ್ಣಾಂಕ		lvl_ಮುಂದಿನ;	// level used for next line
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ;		// number of folds that are forced to start at
				// this line.
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ;		// level of fold that is forced to end below
				// this line
    ಪೂರ್ಣಾಂಕ		had_ಕೊನೆ;	// level of fold that is forced to end above
				// this line (copy of "end" of prev. line)
} fline_T;

// Flag is set when redrawing is needed.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fold_ಬದಲಾಯಿತು;

// Function declarations. {{{2
ಸ್ಥಿರ linenr_T foldUpdateIEMSRecurse(garray_T *gap, ಪೂರ್ಣಾಂಕ level, linenr_T startlnum, fline_T *flp, ಶೂನ್ಯ (*getlevel)(fline_T *), linenr_T bot, ಪೂರ್ಣಾಂಕ topflags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fold_ಸೇರಿಸು(garray_T *gap, ಪೂರ್ಣಾಂಕ i);
ಸ್ಥಿರ ಶೂನ್ಯ fold_ವಿಭಜಿಸು(garray_T *gap, ಪೂರ್ಣಾಂಕ i, linenr_T ತುದಿ, linenr_T bot);
ಸ್ಥಿರ ಶೂನ್ಯ foldRemove(garray_T *gap, linenr_T ತುದಿ, linenr_T bot);
ಸ್ಥಿರ ಶೂನ್ಯ fold_ವಿಲೀನ(fold_T *fp1, garray_T *gap, fold_T *fp2);
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
    linenr_T	ಆರಂಭ;
    linenr_T	ಕೊನೆ;
    fline_T	fline;
    ಶೂನ್ಯ	(*getlevel)(fline_T *);
    ಪೂರ್ಣಾಂಕ		level;
    fold_T	*fp;

    // Avoid problems when being called recursively.
    ಆದರೆ (invalid_ತುದಿ != (linenr_T)0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (wp->w_foldinvalid)
    {
	// Need to update all folds.
	ತುದಿ = 1;
	bot = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	wp->w_foldinvalid = false;

	// Mark all folds as maybe-small.
	ಗಣ_Small_Maybe(&wp->w_folds);
    }

# ifdef FEAT_DIFF
    // add the context for "diff" folding
    ಆದರೆ (foldmethodIsDiff(wp))
    {
	ಆದರೆ (ತುದಿ > diff_context)
	    ತುದಿ -= diff_context;
	ಇಲ್ಲದಿದ್ದರೆ
	    ತುದಿ = 1;
	bot += diff_context;
    }
# endif

    // When deleting lines at the end of the buffer "top" can be past the end
    // of the buffer.
    ಆದರೆ (ತುದಿ > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ತುದಿ = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    fold_ಬದಲಾಯಿತು = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_ಮುಂದಿನ = -1;
    fline.ಆರಂಭ = 0;
    fline.ಕೊನೆ = MAX_LEVEL + 1;
    fline.had_ಕೊನೆ = MAX_LEVEL + 1;

    invalid_ತುದಿ = ತುದಿ;
    invalid_bot = bot;

    ಆದರೆ (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	// Init marker variables to speed up foldlevelMarker().
	parseMarker(wp);

	// Need to get the level of the line above top, it is used if there is
	// no marker at the top.
	ಆದರೆ (ತುದಿ > 1)
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
	    ಆದರೆ (fline.lvl > level)
		fline.lvl = level - (fline.lvl - fline.lvl_ಮುಂದಿನ);
	    ಇಲ್ಲದಿದ್ದರೆ
		fline.lvl = fline.lvl_ಮುಂದಿನ;
	}
	fline.lnum = ತುದಿ;
	getlevel(&fline);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fline.lnum = ತುದಿ;
	ಆದರೆ (foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(wp))
	{
	    getlevel = foldlevel_ಅಭಿವ್ಯಕ್ತಿ;
	    // start one line back, because a "<1" may indicate the end of a
	    // fold in the topline
	    ಆದರೆ (ತುದಿ > 1)
		--fline.lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
# ifdef FEAT_DIFF
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (foldmethodIsDiff(wp))
	    getlevel = foldlevelDiff;
# endif
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    getlevel = foldlevelIndent;
	    // Start one line back, because if the line above "top" has an
	    // undefined fold level, folding it relies on the line under it,
	    // which is "top".
	    ಆದರೆ (ತುದಿ > 1)
		--fline.lnum;
	}

	// Backup to a line for which the fold level is defined.  Since it's
	// always defined for line one, we will stop there.
	fline.lvl = -1;
	ಗಾಗಿ ( ; !got_int; --fline.lnum)
	{
	    // Reset lvl_next each time, because it will be set to a value for
	    // the next line, but we search backwards here.
	    fline.lvl_ಮುಂದಿನ = -1;
	    getlevel(&fline);
	    ಆದರೆ (fline.lvl >= 0)
		ನಿಲ್ಲಿಸು;
	}
    }

    /*
     * If folding is defined by the syntax, it is possible that a change in
     * one line will cause all sub-folds of the current fold to change (e.g.,
     * closing a C-style comment can cause folds in the subsequent lines to
     * appear). To take that into account we should adjust the value of "bot"
     * to point to the end of the current fold:
     */
    ಆದರೆ (foldlevelSyntax == getlevel)
    {
	garray_T *gap = &wp->w_folds;
	fold_T	 *fpn = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	  current_fdl = 0;
	linenr_T  fold_ಆರಂಭ_lnum = 0;
	linenr_T  lnum_rel = fline.lnum;

	ಆಗ (current_fdl < fline.lvl)
	{
	    ಆದರೆ (!fold_ಕಂಡುಹಿಡಿ(gap, lnum_rel, &fpn))
		ನಿಲ್ಲಿಸು;
	    ++current_fdl;

	    fold_ಆರಂಭ_lnum += fpn->fd_ತುದಿ;
	    gap = &fpn->fd_nested;
	    lnum_rel -= fpn->fd_ತುದಿ;
	}
	ಆದರೆ (fpn != ಖಾಲಿ && current_fdl == fline.lvl)
	{
	    linenr_T fold_ಕೊನೆ_lnum = fold_ಆರಂಭ_lnum + fpn->fd_len;

	    ಆದರೆ (fold_ಕೊನೆ_lnum > bot)
		bot = fold_ಕೊನೆ_lnum;
	}
    }

    ಆರಂಭ = fline.lnum;
    ಕೊನೆ = bot;
    // Do at least one line.
    ಆದರೆ (ಆರಂಭ > ಕೊನೆ && ಕೊನೆ < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆ = ಆರಂಭ;
    ಆಗ (!got_int)
    {
	// Always stop at the end of the file ("end" can be past the end of
	// the file).
	ಆದರೆ (fline.lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (fline.lnum > ಕೊನೆ)
	{
	    // For "marker", "expr"  and "syntax"  methods: If a change caused
	    // a fold to be removed, we need to continue at least until where
	    // it ended.
	    ಆದರೆ (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevel_ಅಭಿವ್ಯಕ್ತಿ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((ಆರಂಭ <= ಕೊನೆ
			&& fold_ಕಂಡುಹಿಡಿ(&wp->w_folds, ಕೊನೆ, &fp)
			&& fp->fd_ತುದಿ + fp->fd_len - 1 > ಕೊನೆ)
		    || (fline.lvl == 0
			&& fold_ಕಂಡುಹಿಡಿ(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_ತುದಿ < fline.lnum))
		ಕೊನೆ = fp->fd_ತುದಿ + fp->fd_len - 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (getlevel == foldlevelSyntax
		    && foldLevelWin(wp, fline.lnum) != fline.lvl)
		// For "syntax" method: Compare the foldlevel that the syntax
		// tells us to the foldlevel from the existing folds.  If they
		// don't match continue updating folds.
		ಕೊನೆ = fline.lnum;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}

	// A level 1 fold starts at a line with foldlevel > 0.
	ಆದರೆ (fline.lvl > 0)
	{
	    invalid_ತುದಿ = fline.lnum;
	    invalid_bot = ಕೊನೆ;
	    ಕೊನೆ = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, ಆರಂಭ, &fline, getlevel, ಕೊನೆ, FD_LEVEL);
	    ಆರಂಭ = fline.lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fline.lnum == wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_ಮುಂದಿನ;
	    getlevel(&fline);
	}
    }

    // There can't be any folds from start until end now.
    foldRemove(&wp->w_folds, ಆರಂಭ, ಕೊನೆ);

    // If some fold changed, need to redraw and position cursor.
    ಆದರೆ (fold_ಬದಲಾಯಿತು && wp->w_p_fen)
	ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);

    // If we updated folds past "bot", need to redraw more lines.  Don't do
    // this in other situations, the changed lines will be redrawn anyway and
    // this method can cause the whole window to be updated.
    ಆದರೆ (ಕೊನೆ != bot)
	redraw_win_range_later(wp, ತುದಿ, ಕೊನೆ);

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
    ಪೂರ್ಣಾಂಕ		level,
    linenr_T	startlnum,
    fline_T	*flp,
    ಶೂನ್ಯ	(*getlevel)(fline_T *),
    linenr_T	bot,
    ಪೂರ್ಣಾಂಕ		topflags)	// flags used by containing fold
{
    linenr_T	ll;
    fold_T	*fp = ಖಾಲಿ;
    fold_T	*fp2;
    ಪೂರ್ಣಾಂಕ		lvl = level;
    linenr_T	startlnum2 = startlnum;
    linenr_T	firstlnum = flp->lnum;	// first lnum we got
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಮುಕ್ತಾಯ = FALSE;
    linenr_T	linecount = flp->wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - flp->off;
    ಪೂರ್ಣಾಂಕ		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * at the level we're dealing with and the level is non-zero, we must use
     * the previous fold.  But ignore a fold that starts at or below
     * startlnum, it must be deleted.
     */
    ಆದರೆ (getlevel == foldlevelMarker && flp->ಆರಂಭ <= flp->lvl - level
							      && flp->lvl > 0)
    {
	(ಶೂನ್ಯ)fold_ಕಂಡುಹಿಡಿ(gap, startlnum - 1, &fp);
	ಆದರೆ (fp != ಖಾಲಿ && (fp >= ((fold_T *)gap->ga_ದತ್ತಾಂಶ) + gap->ga_len
						   || fp->fd_ತುದಿ >= startlnum))
	    fp = ಖಾಲಿ;
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
    ಆಗ (!got_int)
    {
	// Updating folds can be slow, check for CTRL-C.
	ಸಾಲು_breakcheck();

	// Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	// and after the first line of the fold, set the level to zero to
	// force the fold to end.  Do the same when had_end is set: Previous
	// line was marked as end of a fold.
	lvl = flp->lvl;
	ಆದರೆ (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	ಆದರೆ (flp->lnum > firstlnum
		&& (level > lvl - flp->ಆರಂಭ || level >= flp->had_ಕೊನೆ))
	    lvl = 0;

	ಆದರೆ (flp->lnum > bot && !ಮುಕ್ತಾಯ && fp != ಖಾಲಿ)
	{
	    // For "marker" and "syntax" methods:
	    // - If a change caused a nested fold to be removed, we need to
	    //   delete it and continue at least until where it ended.
	    // - If a change caused a nested fold to be created, or this fold
	    //   to continue below its original end, need to finish this fold.
	    ಆದರೆ (getlevel != foldlevelMarker
		    && getlevel != foldlevel_ಅಭಿವ್ಯಕ್ತಿ
		    && getlevel != foldlevelSyntax)
		ನಿಲ್ಲಿಸು;
	    i = 0;
	    fp2 = fp;
	    ಆದರೆ (lvl >= level)
	    {
		// Compute how deep the folds currently are, if it's deeper
		// than "lvl" then some must be deleted, need to update
		// at least one nested fold.
		ll = flp->lnum - fp->fd_ತುದಿ;
		ಆಗ (fold_ಕಂಡುಹಿಡಿ(&fp2->fd_nested, ll, &fp2))
		{
		    ++i;
		    ll -= fp2->fd_ತುದಿ;
		}
	    }
	    ಆದರೆ (lvl < level + i)
	    {
		(ಶೂನ್ಯ)fold_ಕಂಡುಹಿಡಿ(&fp->fd_nested, flp->lnum - fp->fd_ತುದಿ, &fp2);
		ಆದರೆ (fp2 != ಖಾಲಿ)
		    bot = fp2->fd_ತುದಿ + fp2->fd_len - 1 + fp->fd_ತುದಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len <= flp->lnum && lvl >= level)
		ಮುಕ್ತಾಯ = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}

	// At the start of the first nested fold and at the end of the current
	// fold: check if existing folds at this level, before the current
	// one, need to be deleted or truncated.
	ಆದರೆ (fp == ಖಾಲಿ
		&& (lvl != level
		    || flp->lnum_save >= bot
		    || flp->ಆರಂಭ != 0
		    || flp->had_ಕೊನೆ <= MAX_LEVEL
		    || flp->lnum == linecount))
	{
	    /*
	     * Remove or update folds that have lines between startlnum and
	     * firstlnum.
	     */
	    ಆಗ (!got_int)
	    {
		// set concat to 1 if it's allowed to concatenate this fold
		// with a previous one that touches it.
		ಆದರೆ (flp->ಆರಂಭ != 0 || flp->had_ಕೊನೆ <= MAX_LEVEL)
		    concat = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    concat = 1;

		// Find an existing fold to re-use.  Preferably one that
		// includes startlnum, otherwise one that ends just before
		// startlnum or starts after it.
		ಆದರೆ (gap->ga_len > 0 && (fold_ಕಂಡುಹಿಡಿ(gap, startlnum, &fp)
			|| (fp < ((fold_T *)gap->ga_ದತ್ತಾಂಶ) + gap->ga_len
			    && fp->fd_ತುದಿ <= firstlnum)
			|| fold_ಕಂಡುಹಿಡಿ(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_T *)gap->ga_ದತ್ತಾಂಶ) + gap->ga_len
			    && ((lvl < level && fp->fd_ತುದಿ < flp->lnum)
				|| (lvl >= level
					   && fp->fd_ತುದಿ <= flp->lnum_save)))))
		{
		    ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len + concat > firstlnum)
		    {
			// Use existing fold for the new fold.  If it starts
			// before where we started looking, extend it.  If it
			// starts at another line, update nested folds to keep
			// their position, compensating for the new fd_top.
			ಆದರೆ (fp->fd_ತುದಿ == firstlnum)
			{
			    // have found a fold beginning where we want
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_ತುದಿ >= startlnum)
			{
			    ಆದರೆ (fp->fd_ತುದಿ > firstlnum)
				// like lines are inserted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0, (linenr_T)MAXLNUM,
					(ದೀರ್ಘ)(fp->fd_ತುದಿ - firstlnum), 0L);
			    ಇಲ್ಲದಿದ್ದರೆ
				// like lines are deleted
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_T)0,
					(ದೀರ್ಘ)(firstlnum - fp->fd_ತುದಿ - 1),
					(linenr_T)MAXLNUM,
					(ದೀರ್ಘ)(fp->fd_ತುದಿ - firstlnum));
			    fp->fd_len += fp->fd_ತುದಿ - firstlnum;
			    fp->fd_ತುದಿ = firstlnum;
			    fp->fd_small = MAYBE;
			    fold_ಬದಲಾಯಿತು = TRUE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((flp->ಆರಂಭ != 0 && lvl == level)
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
			    ಆದರೆ (firstlnum != startlnum)
			    {
				breakstart = startlnum;
				breakend = firstlnum;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				breakstart = flp->lnum;
				breakend = flp->lnum;
			    }
			    foldRemove(&fp->fd_nested, breakstart - fp->fd_ತುದಿ,
						      breakend - fp->fd_ತುದಿ);
			    i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ);
			    fold_ವಿಭಜಿಸು(gap, i, breakstart, breakend - 1);
			    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i + 1;

			    // If using the "marker" or "syntax" method, we
			    // need to continue until the end of the fold is
			    // found.
			    ಆದರೆ (getlevel == foldlevelMarker
				    || getlevel == foldlevel_ಅಭಿವ್ಯಕ್ತಿ
				    || getlevel == foldlevelSyntax)
				ಮುಕ್ತಾಯ = TRUE;
			}

			ಆದರೆ (fp->fd_ತುದಿ == startlnum && concat)
			{
			    i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ);
			    ಆದರೆ (i != 0)
			    {
				fp2 = fp - 1;
				ಆದರೆ (fp2->fd_ತುದಿ + fp2->fd_len == fp->fd_ತುದಿ)
				{
				    fold_ವಿಲೀನ(fp2, gap, fp);
				    fp = fp2;
				}
			    }
			}
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (fp->fd_ತುದಿ >= startlnum)
		    {
			// A fold that starts at or after startlnum and stops
			// before the new fold must be deleted.  Continue
			// looking for the next one.
			delete_Fold_ನಮೂದು(gap,
				     (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ), TRUE);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// A fold has some lines above startlnum, truncate it
			// to stop just above startlnum.
			fp->fd_len = startlnum - fp->fd_ತುದಿ;
			foldMarkAdjustRecurse(&fp->fd_nested,
				fp->fd_len, (linenr_T)MAXLNUM,
						       (linenr_T)MAXLNUM, 0L);
			fold_ಬದಲಾಯಿತು = TRUE;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Insert new fold.  Careful: ga_data may be NULL and it
		    // may change!
		    ಆದರೆ (gap->ga_len == 0)
			i = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
			i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ);
		    ಆದರೆ (fold_ಸೇರಿಸು(gap, i) != OK)
			ಹಿಂತಿರುಗಿ bot;
		    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i;
		    // The new fold continues until bot, unless we find the
		    // end earlier.
		    fp->fd_ತುದಿ = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    // When the containing fold is open, the new fold is open.
		    // The new fold is closed if the fold above it is closed.
		    // The first fold depends on the containing fold.
		    ಆದರೆ (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = true;
			fp->fd_flags = FD_OPEN;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i <= 0)
		    {
			fp->fd_flags = topflags;
			ಆದರೆ (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = true;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			fp->fd_flags = (fp - 1)->fd_flags;
		    fp->fd_small = MAYBE;
		    // If using the "marker", "expr" or "syntax" method, we
		    // need to continue until the end of the fold is found.
		    ಆದರೆ (getlevel == foldlevelMarker
			    || getlevel == foldlevel_ಅಭಿವ್ಯಕ್ತಿ
			    || getlevel == foldlevelSyntax)
			ಮುಕ್ತಾಯ = TRUE;
		    fold_ಬದಲಾಯಿತು = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}

	ಆದರೆ (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    ನಿಲ್ಲಿಸು;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 * Check "fp" for safety.
	 */
	ಆದರೆ (lvl > level && fp != ಖಾಲಿ)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    // At least do one line (can happen when finish is TRUE).
	    ಆದರೆ (bot < flp->lnum)
		bot = flp->lnum;

	    // Line numbers in the nested fold are relative to the start of
	    // this fold.
	    flp->lnum = flp->lnum_save - fp->fd_ತುದಿ;
	    flp->off += fp->fd_ತುದಿ;
	    i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ);
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_ತುದಿ, flp, getlevel,
					      bot - fp->fd_ತುದಿ, fp->fd_flags);
	    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i;
	    flp->lnum += fp->fd_ತುದಿ;
	    flp->lnum_save += fp->fd_ತುದಿ;
	    flp->off -= fp->fd_ತುದಿ;
	    bot += fp->fd_ತುದಿ;
	    startlnum2 = flp->lnum;

	    // This fold may end at the same line, don't incr. flp->lnum.
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    ಆಗ (!got_int)
	    {
		// Make the previous level available to foldlevel().
		prev_lnum = flp->lnum;
		prev_lnum_lvl = flp->lvl;

		ಆದರೆ (++flp->lnum > linecount)
		    ನಿಲ್ಲಿಸು;
		flp->lvl = flp->lvl_ಮುಂದಿನ;
		getlevel(flp);
		ಆದರೆ (flp->lvl >= 0 || flp->had_ಕೊನೆ <= MAX_LEVEL)
		    ನಿಲ್ಲಿಸು;
	    }
	    prev_lnum = 0;
	    ಆದರೆ (flp->lnum > linecount)
		ನಿಲ್ಲಿಸು;

	    // leave flp->lnum_save to lnum of the line that was used to get
	    // the level, flp->lnum to the lnum of the next line.
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    ಆದರೆ (fp == ಖಾಲಿ)	// only happens when got_int is set
	ಹಿಂತಿರುಗಿ bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    // Current fold at least extends until lnum.
    ಆದರೆ (fp->fd_len < flp->lnum - fp->fd_ತುದಿ)
    {
	fp->fd_len = flp->lnum - fp->fd_ತುದಿ;
	fp->fd_small = MAYBE;
	fold_ಬದಲಾಯಿತು = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len > linecount)
	// running into the end of the buffer (deleted last line)
	fp->fd_len = linecount - fp->fd_ತುದಿ + 1;

    // Delete contained folds from the end of the last one found until where
    // we stopped looking.
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_ತುದಿ,
						  flp->lnum - 1 - fp->fd_ತುದಿ);

    ಆದರೆ (lvl < level)
    {
	// End of fold found, update the length when it got shorter.
	ಆದರೆ (fp->fd_len != flp->lnum - fp->fd_ತುದಿ)
	{
	    ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len - 1 > bot)
	    {
		// fold continued below bot
		ಆದರೆ (getlevel == foldlevelMarker
			|| getlevel == foldlevel_ಅಭಿವ್ಯಕ್ತಿ
			|| getlevel == foldlevelSyntax)
		{
		    // marker method: truncate the fold and make sure the
		    // previously included lines are processed again
		    bot = fp->fd_ತುದಿ + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_ತುದಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // indent or expr method: split fold to create a new one
		    // below bot
		    i = (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ);
		    fold_ವಿಭಜಿಸು(gap, i, flp->lnum, bot);
		    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		fp->fd_len = flp->lnum - fp->fd_ತುದಿ;
	    fold_ಬದಲಾಯಿತು = TRUE;
	}
    }

    // delete following folds that end before the current line
    ಗಾಗಿ (;;)
    {
	fp2 = fp + 1;
	ಆದರೆ (fp2 >= (fold_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len
						  || fp2->fd_ತುದಿ > flp->lnum)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (fp2->fd_ತುದಿ + fp2->fd_len > flp->lnum)
	{
	    ಆದರೆ (fp2->fd_ತುದಿ < flp->lnum)
	    {
		// Make fold that includes lnum start at lnum.
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_T)0, (ದೀರ್ಘ)(flp->lnum - fp2->fd_ತುದಿ - 1),
			(linenr_T)MAXLNUM, (ದೀರ್ಘ)(fp2->fd_ತುದಿ - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_ತುದಿ;
		fp2->fd_ತುದಿ = flp->lnum;
		fold_ಬದಲಾಯಿತು = TRUE;
	    }

	    ಆದರೆ (lvl >= level)
	    {
		// merge new fold with existing fold that follows
		fold_ವಿಲೀನ(fp, gap, fp2);
	    }
	    ನಿಲ್ಲಿಸು;
	}
	fold_ಬದಲಾಯಿತು = TRUE;
	delete_Fold_ನಮೂದು(gap, (ಪೂರ್ಣಾಂಕ)(fp2 - (fold_T *)gap->ga_ದತ್ತಾಂಶ), TRUE);
    }

    // Need to redraw the lines we inspected, which might be further down than
    // was asked for.
    ಆದರೆ (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    ಹಿಂತಿರುಗಿ bot;
}

// foldInsert() {{{2
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fold_ಸೇರಿಸು(garray_T *gap, ಪೂರ್ಣಾಂಕ i)
{
    fold_T	*fp;

    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i;
    ಆದರೆ (gap->ga_len > 0 && i < gap->ga_len)
	mch_memmove(fp + 1, fp, ಗಾತ್ರ(fold_T) * (gap->ga_len - i));
    ++gap->ga_len;
    ga_init2(&fp->fd_nested, ಗಾತ್ರ(fold_T), 10);
    ಹಿಂತಿರುಗಿ OK;
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
fold_ವಿಭಜಿಸು(
    garray_T	*gap,
    ಪೂರ್ಣಾಂಕ		i,
    linenr_T	ತುದಿ,
    linenr_T	bot)
{
    fold_T	*fp;
    fold_T	*fp2;
    garray_T	*gap1;
    garray_T	*gap2;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		len;

    // The fold continues below bot, need to split it.
    ಆದರೆ (fold_ಸೇರಿಸು(gap, i + 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ + i;
    fp[1].fd_ತುದಿ = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_ತುದಿ - fp->fd_ತುದಿ);
    fp[1].fd_flags = fp->fd_flags;
    fp[1].fd_small = MAYBE;
    fp->fd_small = MAYBE;

    // Move nested folds below bot to new fold.  There can't be
    // any between top and bot, they have been removed by the caller.
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (ಶೂನ್ಯ)fold_ಕಂಡುಹಿಡಿ(gap1, bot + 1 - fp->fd_ತುದಿ, &fp2);
    ಆದರೆ (fp2 != ಖಾಲಿ)
    {
	len = (ಪೂರ್ಣಾಂಕ)((fold_T *)gap1->ga_ದತ್ತಾಂಶ + gap1->ga_len - fp2);
	ಆದರೆ (len > 0 && ga_grow(gap2, len) == OK)
	{
	    ಗಾಗಿ (idx = 0; idx < len; ++idx)
	    {
		((fold_T *)gap2->ga_ದತ್ತಾಂಶ)[idx] = fp2[idx];
		((fold_T *)gap2->ga_ದತ್ತಾಂಶ)[idx].fd_ತುದಿ
						  -= fp[1].fd_ತುದಿ - fp->fd_ತುದಿ;
	    }
	    gap2->ga_len = len;
	    gap1->ga_len -= len;
	}
    }
    fp->fd_len = ತುದಿ - fp->fd_ತುದಿ;
    fold_ಬದಲಾಯಿತು = TRUE;
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
    fold_T	*fp = ಖಾಲಿ;

    ಆದರೆ (bot < ತುದಿ)
	ಹಿಂತಿರುಗಿ;		// nothing to do

    ಆಗ (gap->ga_len > 0)
    {
	// Find fold that includes top or a following one.
	ಆದರೆ (fold_ಕಂಡುಹಿಡಿ(gap, ತುದಿ, &fp) && fp->fd_ತುದಿ < ತುದಿ)
	{
	    // 2: or 3: need to delete nested folds
	    foldRemove(&fp->fd_nested, ತುದಿ - fp->fd_ತುದಿ, bot - fp->fd_ತುದಿ);
	    ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len - 1 > bot)
	    {
		// 3: need to split it.
		fold_ವಿಭಜಿಸು(gap, (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ), ತುದಿ, bot);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// 2: truncate fold at "top".
		fp->fd_len = ತುದಿ - fp->fd_ತುದಿ;
	    }
	    fold_ಬದಲಾಯಿತು = TRUE;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (fp >= (fold_T *)(gap->ga_ದತ್ತಾಂಶ) + gap->ga_len
		|| fp->fd_ತುದಿ > bot)
	{
	    // 6: Found a fold below bot, can stop looking.
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (fp->fd_ತುದಿ >= ತುದಿ)
	{
	    // Found an entry below top.
	    fold_ಬದಲಾಯಿತು = TRUE;
	    ಆದರೆ (fp->fd_ತುದಿ + fp->fd_len - 1 > bot)
	    {
		// 5: Make fold that includes bot start below bot.
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_T)0, (ದೀರ್ಘ)(bot - fp->fd_ತುದಿ),
			(linenr_T)MAXLNUM, (ದೀರ್ಘ)(fp->fd_ತುದಿ - bot - 1));
		fp->fd_len -= bot - fp->fd_ತುದಿ + 1;
		fp->fd_ತುದಿ = bot + 1;
		ನಿಲ್ಲಿಸು;
	    }

	    // 4: Delete completely contained fold.
	    delete_Fold_ನಮೂದು(gap, (ಪೂರ್ಣಾಂಕ)(fp - (fold_T *)gap->ga_ದತ್ತಾಂಶ), TRUE);
	}
    }
}

// foldReverseOrder() {{{2
    ಸ್ಥಿರ ಶೂನ್ಯ
fold_ಹಿಮ್ಮುಖ_ಕ್ರಮ(garray_T *gap, linenr_T ಆರಂಭ_arg, linenr_T ಕೊನೆ_arg)
{
    fold_T *ಎಡ_2, *ಬಲ_2;
    fold_T tmp;
    linenr_T ಆರಂಭ = ಆರಂಭ_arg;
    linenr_T ಕೊನೆ = ಕೊನೆ_arg;

    ಗಾಗಿ (; ಆರಂಭ < ಕೊನೆ; ಆರಂಭ++, ಕೊನೆ--)
    {
	ಎಡ_2 = (fold_T *)gap->ga_ದತ್ತಾಂಶ + ಆರಂಭ;
	ಬಲ_2 = (fold_T *)gap->ga_ದತ್ತಾಂಶ + ಕೊನೆ;
	tmp  = *ಎಡ_2;
	*ಎಡ_2 = *ಬಲ_2;
	*ಬಲ_2 = tmp;
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
truncate_fold(fold_T *fp, linenr_T ಕೊನೆ)
{
    ಕೊನೆ += 1;
    foldRemove(&fp->fd_nested, ಕೊನೆ - fp->fd_ತುದಿ, MAXLNUM);
    fp->fd_len = ಕೊನೆ - fp->fd_ತುದಿ;
}

# define fold_ಕೊನೆ(fp) ((fp)->fd_ತುದಿ + (fp)->fd_len - 1)
# define valid_fold(fp, gap) ((gap)->ga_len > 0 && (fp) < ((fold_T *)(gap)->ga_ದತ್ತಾಂಶ + (gap)->ga_len))
# define fold_ಸೂಚ್ಯಂಕ(fp, gap) ((size_t)((fp) - ((fold_T *)(gap)->ga_ದತ್ತಾಂಶ)))

    ಶೂನ್ಯ
foldMoveRange(garray_T *gap, linenr_T line1, linenr_T line2, linenr_T dest)
{
    fold_T *fp;
    linenr_T range_len = line2 - line1 + 1;
    linenr_T move_len = dest - line2;
    ಪೂರ್ಣಾಂಕ at_ಆರಂಭ = fold_ಕಂಡುಹಿಡಿ(gap, line1 - 1, &fp);
    size_t move_ಆರಂಭ = 0, move_ಕೊನೆ = 0, dest_ಸೂಚ್ಯಂಕ = 0;

    ಆದರೆ (at_ಆರಂಭ)
    {
	ಆದರೆ (fold_ಕೊನೆ(fp) > dest)
	{
	    // Case 4
	   // don't have to change this fold, but have to move nested folds.
	    foldMoveRange(&fp->fd_nested, line1 - fp->fd_ತುದಿ, line2 -
		    fp->fd_ತುದಿ, dest - fp->fd_ತುದಿ);
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fold_ಕೊನೆ(fp) > line2)
	{
	    // Case 3
	    // Remove nested folds between line1 and line2 & reduce the
	    // length of fold by "range_len".
	    // Folds after this one must be dealt with.
	    foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_ತುದಿ, line2 -
		    fp->fd_ತುದಿ, MAXLNUM, -range_len);
	    fp->fd_len -= range_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Case 2 truncate fold, folds after this one must be dealt with.
	    truncate_fold(fp, line1 - 1);

	// Look at the next fold, and treat that one as if it were the first
	// after  "line1" (because now it is).
	fp = fp + 1;
    }

    ಆದರೆ (!valid_fold(fp, gap) || fp->fd_ತುದಿ > dest)
    {
	// Case 10
	// No folds after "line1" and before "dest"
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->fd_ತುದಿ > line2)
    {
	ಗಾಗಿ (; valid_fold(fp, gap) && fold_ಕೊನೆ(fp) <= dest; fp++)
	// Case 9. (for all case 9's) -- shift up.
	    fp->fd_ತುದಿ -= range_len;

	ಆದರೆ (valid_fold(fp, gap) && fp->fd_ತುದಿ <= dest)
	{
	    // Case 8. -- ensure truncated at dest, shift up
	    truncate_fold(fp, dest);
	    fp->fd_ತುದಿ -= range_len;
	}
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fold_ಕೊನೆ(fp) > dest)
    {
	// Case 7 -- remove nested folds and shrink
	foldMarkAdjustRecurse(&fp->fd_nested, line2 + 1 - fp->fd_ತುದಿ, dest -
		fp->fd_ತುದಿ, MAXLNUM, -move_len);
	fp->fd_len -= move_len;
	fp->fd_ತುದಿ += move_len;
	ಹಿಂತಿರುಗಿ;
    }

    // Case 5 or 6
    // changes rely on whether there are folds between the end of
    // this fold and "dest".
    move_ಆರಂಭ = fold_ಸೂಚ್ಯಂಕ(fp, gap);

    ಗಾಗಿ (; valid_fold(fp, gap) && fp->fd_ತುದಿ <= dest; fp++)
    {
	ಆದರೆ (fp->fd_ತುದಿ <= line2)
	{
	    // 1. 2. or 3.
	    ಆದರೆ (fold_ಕೊನೆ(fp) > line2)
		// 2. or 3., truncate before moving
		truncate_fold(fp, line2);

	    fp->fd_ತುದಿ += move_len;
	    ಮುಂದುವರಿ;
	}

	// Record index of the first fold after the moved range.
	ಆದರೆ (move_ಕೊನೆ == 0)
	    move_ಕೊನೆ = fold_ಸೂಚ್ಯಂಕ(fp, gap);

	ಆದರೆ (fold_ಕೊನೆ(fp) > dest)
	    truncate_fold(fp, dest);

	fp->fd_ತುದಿ -= range_len;
    }

    dest_ಸೂಚ್ಯಂಕ = fold_ಸೂಚ್ಯಂಕ(fp, gap);

    /*
     * All folds are now correct, but not necessarily in the correct order.  We
     * must swap folds in the range [move_end, dest_index) with those in the
     * range [move_start, move_end).
     */
    ಆದರೆ (move_ಕೊನೆ == 0)
	// There are no folds after those moved, hence no folds have been moved
	// out of order.
	ಹಿಂತಿರುಗಿ;
    fold_ಹಿಮ್ಮುಖ_ಕ್ರಮ(gap, (linenr_T)move_ಆರಂಭ, (linenr_T)dest_ಸೂಚ್ಯಂಕ - 1);
    fold_ಹಿಮ್ಮುಖ_ಕ್ರಮ(gap, (linenr_T)move_ಆರಂಭ,
			   (linenr_T)(move_ಆರಂಭ + dest_ಸೂಚ್ಯಂಕ - move_ಕೊನೆ - 1));
    fold_ಹಿಮ್ಮುಖ_ಕ್ರಮ(gap, (linenr_T)(move_ಆರಂಭ + dest_ಸೂಚ್ಯಂಕ - move_ಕೊನೆ),
						   (linenr_T)(dest_ಸೂಚ್ಯಂಕ - 1));
}
# undef fold_ಕೊನೆ
# undef valid_fold
# undef fold_ಸೂಚ್ಯಂಕ

// foldMerge() {{{2
/*
 * Merge two adjacent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjacent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fold_ವಿಲೀನ(fold_T *fp1, garray_T *gap, fold_T *fp2)
{
    fold_T	*fp3;
    fold_T	*fp4;
    ಪೂರ್ಣಾಂಕ		idx;
    garray_T	*gap1 = &fp1->fd_nested;
    garray_T	*gap2 = &fp2->fd_nested;

    // If the last nested fold in fp1 touches the first nested fold in fp2,
    // merge them recursively.
    ಆದರೆ (fold_ಕಂಡುಹಿಡಿ(gap1, fp1->fd_len - 1L, &fp3) && fold_ಕಂಡುಹಿಡಿ(gap2, 0L, &fp4))
	fold_ವಿಲೀನ(fp3, gap2, fp4);

    // Move nested folds in fp2 to the end of fp1.
    ಆದರೆ (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	ಗಾಗಿ (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_T *)gap1->ga_ದತ್ತಾಂಶ)[gap1->ga_len]
					= ((fold_T *)gap2->ga_ದತ್ತಾಂಶ)[idx];
	    ((fold_T *)gap1->ga_ದತ್ತಾಂಶ)[gap1->ga_len].fd_ತುದಿ += fp1->fd_len;
	    ++gap1->ga_len;
	}
	gap2->ga_len = 0;
    }

    fp1->fd_len += fp2->fd_len;
    delete_Fold_ನಮೂದು(gap, (ಪೂರ್ಣಾಂಕ)(fp2 - (fold_T *)gap->ga_ದತ್ತಾಂಶ), TRUE);
    fold_ಬದಲಾಯಿತು = TRUE;
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

    buf = flp->wp->w_ಬಫರ್;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    // empty line or lines starting with a character in 'foldignore': level
    // depends on surrounding lines
    ಆದರೆ (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != ಖಾಲಿ)
    {
	// first and last line can't be undefined, use level 0
	ಆದರೆ (lnum == 1 || lnum == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    flp->lvl = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    flp->lvl = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	flp->lvl = get_indent_buf(buf, lnum) / get_sw_ಮೌಲ್ಯ(buf);
    ಆದರೆ (flp->lvl > flp->wp->w_p_fdn)
    {
	flp->lvl = flp->wp->w_p_fdn;
	ಆದರೆ (flp->lvl < 0)
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
    ಆದರೆ (diff_infold(flp->wp, flp->lnum + flp->off))
	flp->lvl = 1;
    ಇಲ್ಲದಿದ್ದರೆ
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
    flp->ಆರಂಭ = FALSE;
    flp->lvl = 0;
# ಇಲ್ಲದಿದ್ದರೆ
    win_T	*win;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		c;
    linenr_T	lnum = flp->lnum + flp->off;
    ಪೂರ್ಣಾಂಕ		save_keytyped;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_ಬಫರ್;
    ಗಣ_vim_var_nr(VV_LNUM, lnum);

    flp->ಆರಂಭ = 0;
    flp->had_ಕೊನೆ = flp->ಕೊನೆ;
    flp->ಕೊನೆ = MAX_LEVEL + 1;
    ಆದರೆ (lnum <= 1)
	flp->lvl = 0;

    // KeyTyped may be reset to 0 when calling a function which invokes
    // do_cmdline().  To make 'foldopen' work correctly restore KeyTyped.
    save_keytyped = ಕೀಲಿ_Typed;
    n = eval_foldexpr(flp->wp, &c);
    ಕೀಲಿ_Typed = save_keytyped;

    ಆಯ್ಕೆ (c)
    {
	// "a1", "a2", .. : add to the fold level
	ಸ್ಥಿತಿ 'a': ಆದರೆ (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_ಮುಂದಿನ = flp->lvl;
		  }
		  flp->ಆರಂಭ = n;
		  ನಿಲ್ಲಿಸು;

	// "s1", "s2", .. : subtract from the fold level
	ಸ್ಥಿತಿ 's': ಆದರೆ (flp->lvl >= 0)
		  {
		      ಆದರೆ (n > flp->lvl)
			  flp->lvl_ಮುಂದಿನ = 0;
		      ಇಲ್ಲದಿದ್ದರೆ
			  flp->lvl_ಮುಂದಿನ = flp->lvl - n;
		      flp->ಕೊನೆ = flp->lvl_ಮುಂದಿನ + 1;
		  }
		  ನಿಲ್ಲಿಸು;

	// ">1", ">2", .. : start a fold with a certain level
	ಸ್ಥಿತಿ '>': flp->lvl = n;
		  flp->lvl_ಮುಂದಿನ = n;
		  flp->ಆರಂಭ = 1;
		  ನಿಲ್ಲಿಸು;

	// "<1", "<2", .. : end a fold with a certain level
	ಸ್ಥಿತಿ '<': // To prevent an unexpected start of a new fold, the next
		  // level must not exceed the level of the current fold.
		  flp->lvl_ಮುಂದಿನ = MIN(flp->lvl, n - 1);
		  flp->ಕೊನೆ = n;
		  ನಿಲ್ಲಿಸು;

	// "=": No change in level
	ಸ್ಥಿತಿ '=': flp->lvl_ಮುಂದಿನ = flp->lvl;
		  ನಿಲ್ಲಿಸು;

	// "-1", "0", "1", ..: set fold level
	ಪೂರ್ವನಿಯೋಜಿತ:  ಆದರೆ (n < 0)
		      // Use the current level for the next line, so that "a1"
		      // will work there.
		      flp->lvl_ಮುಂದಿನ = flp->lvl;
		  ಇಲ್ಲದಿದ್ದರೆ
		      flp->lvl_ಮುಂದಿನ = n;
		  flp->lvl = n;
		  ನಿಲ್ಲಿಸು;
    }

    // If the level is unknown for the first or the last line in the file, use
    // level 0.
    ಆದರೆ (flp->lvl < 0)
    {
	ಆದರೆ (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_ಮುಂದಿನ = 0;
	}
	ಆದರೆ (lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    flp->lvl_ಮುಂದಿನ = 0;
    }

    curwin = win;
    curbuf = curwin->w_ಬಫರ್;
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
    foldstartmarkerlen = (ಪೂರ್ಣಾಂಕ)(foldendmarker++ - wp->w_p_fmr);
    foldendmarkerlen = (ಪೂರ್ಣಾಂಕ)STRLEN(foldendmarker);
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
    ಪೂರ್ಣಾಂಕ		cstart;
    ಪೂರ್ಣಾಂಕ		cend;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_lvl = flp->lvl;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		n;

    // cache a few values for speed
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    // Default: no start found, next level is same as current level
    flp->ಆರಂಭ = 0;
    flp->lvl_ಮುಂದಿನ = flp->lvl;

    s = ml_get_buf(flp->wp->w_ಬಫರ್, flp->lnum + flp->off, FALSE);
    ಆಗ (*s)
    {
	ಆದರೆ (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    // found startmarker: set flp->lvl
	    s += foldstartmarkerlen;
	    ಆದರೆ (VIM_ISDIGIT(*s))
	    {
		n = atoi((ಅಕ್ಷರ *)s);
		ಆದರೆ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_ಮುಂದಿನ = n;
		    ಆದರೆ (n <= ಆರಂಭ_lvl)
			flp->ಆರಂಭ = 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			flp->ಆರಂಭ = n - ಆರಂಭ_lvl;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		++flp->lvl;
		++flp->lvl_ಮುಂದಿನ;
		++flp->ಆರಂಭ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    // found endmarker: set flp->lvl_next
	    s += foldendmarkerlen;
	    ಆದರೆ (VIM_ISDIGIT(*s))
	    {
		n = atoi((ಅಕ್ಷರ *)s);
		ಆದರೆ (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_ಮುಂದಿನ = n - 1;
		    // never start a fold with an end marker
		    ಆದರೆ (flp->lvl_ಮುಂದಿನ > ಆರಂಭ_lvl)
			flp->lvl_ಮುಂದಿನ = ಆರಂಭ_lvl;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		--flp->lvl_ಮುಂದಿನ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    MB_PTR_ADV(s);
    }

    // The level can't go negative, must be missing a start marker.
    ಆದರೆ (flp->lvl_ಮುಂದಿನ < 0)
	flp->lvl_ಮುಂದಿನ = 0;
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
    flp->ಆರಂಭ = 0;
    flp->lvl = 0;
# ಇಲ್ಲದಿದ್ದರೆ
    linenr_T	lnum = flp->lnum + flp->off;
    ಪೂರ್ಣಾಂಕ		n;

    // Use the maximum fold level at the start of this line and the next.
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->ಆರಂಭ = 0;
    ಆದರೆ (lnum < flp->wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	ಆದರೆ (n > flp->lvl)
	{
	    flp->ಆರಂಭ = n - flp->lvl;	// fold(s) start here
	    flp->lvl = n;
	}
    }
# endif
}

// functions for storing the fold state in a View {{{1
// put_folds() {{{2
# ಆದರೆ defined(FEAT_SESSION)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ put_foldopen_recurse(FILE *fd, win_T *wp, garray_T *gap, linenr_T off);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off);

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    ಪೂರ್ಣಾಂಕ
put_folds(FILE *fd, win_T *wp)
{
    ಆದರೆ (foldmethodIsManual(wp))
    {
	ಆದರೆ (put_ಸಾಲು(fd, "silent! normal! zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_T)0) == FAIL
		|| put_ಸಾಲು(fd, "&fdl = &fdl") == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // If some folds are manually opened/closed, need to restore that.
    ಆದರೆ (wp->w_fold_manual)
	ಹಿಂತಿರುಗಿ put_foldopen_recurse(fd, wp, &wp->w_folds, (linenr_T)0);

    ಹಿಂತಿರುಗಿ OK;
}

// put_folds_recurse() {{{2
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_folds_recurse(FILE *fd, garray_T *gap, linenr_T off)
{
    ಪೂರ್ಣಾಂಕ		i;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < gap->ga_len; i++)
    {
	// Do nested folds first, they will be created closed.
	ಆದರೆ (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_ತುದಿ) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (fprintf(fd, "sil! :%ld,%ldfold", fp->fd_ತುದಿ + off,
					fp->fd_ತುದಿ + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	++fp;
    }
    ಹಿಂತಿರುಗಿ OK;
}

// put_foldopen_recurse() {{{2
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_foldopen_recurse(
    FILE	*fd,
    win_T	*wp,
    garray_T	*gap,
    linenr_T	off)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		level;
    fold_T	*fp;

    fp = (fold_T *)gap->ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < gap->ga_len; i++)
    {
	ಆದರೆ (fp->fd_flags != FD_LEVEL)
	{
	    ಆದರೆ (fp->fd_nested.ga_len > 0)
	    {
		// open nested folds while this fold is open
		// ignore errors
		ಆದರೆ (fprintf(fd, ":%ld", fp->fd_ತುದಿ + off) < 0
			|| put_eol(fd) == FAIL
			|| put_ಸಾಲು(fd, "sil! normal! zo") == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (put_foldopen_recurse(fd, wp, &fp->fd_nested,
							     off + fp->fd_ತುದಿ)
			== FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		// close the parent when needed
		ಆದರೆ (fp->fd_flags == FD_CLOSED)
		{
		    ಆದರೆ (put_fold_open_close(fd, fp, off) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Open or close the leaf according to the window foldlevel.
		// Do not close a leaf that is already closed, as it will close
		// the parent.
		level = foldLevelWin(wp, off + fp->fd_ತುದಿ);
		ಆದರೆ ((fp->fd_flags == FD_CLOSED && wp->w_p_fdl >= level)
			|| (fp->fd_flags != FD_CLOSED && wp->w_p_fdl < level))
		ಆದರೆ (put_fold_open_close(fd, fp, off) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	++fp;
    }

    ಹಿಂತಿರುಗಿ OK;
}

// put_fold_open_close() {{{2
/*
 * Write the open or close command to "fd".
 * Returns FAIL when writing failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_fold_open_close(FILE *fd, fold_T *fp, linenr_T off)
{
    ಆದರೆ (fprintf(fd, ":%ld", fp->fd_ತುದಿ + off) < 0
	    || put_eol(fd) == FAIL
	    || fprintf(fd, "sil! normal! z%c",
			   fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
	    || put_eol(fd) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}
# endif // FEAT_SESSION

// }}}1
#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_FOLDING)

#ಆದರೆ defined(FEAT_EVAL)

/*
 * "foldclosed()" and "foldclosedend()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
foldclosed_both(
    typval_T	*argvars ಬಳಸದ,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ ಬಳಸದ)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    linenr_T	ಮೊದಲ, ಕೊನೆಯ;
    win_T	*wp  = curwin;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಆದರೆ (wp != ಖಾಲಿ && tp != ಖಾಲಿ)
	{
	    ಆದರೆ (switch_win(&switchwin, wp, tp, TRUE) == OK)
		lnum = tv_get_lnum(argvars);
	    restore_win(&switchwin, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = -2;
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lnum = tv_get_lnum(argvars);

    ಆದರೆ (lnum >= 1 && lnum <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಆದರೆ (hasFoldingWin(wp, lnum, &ಮೊದಲ, &ಕೊನೆಯ, FALSE, ಖಾಲಿ))
	{
	    ಆದರೆ (ಕೊನೆ)
		rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)ಕೊನೆಯ;
	    ಇಲ್ಲದಿದ್ದರೆ
		rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)ಮೊದಲ;
	    ಹಿಂತಿರುಗಿ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
foldLevel(linenr_T lnum)
{
    // While updating the folds lines between invalid_top and invalid_bot have
    // an undefined fold level.  Otherwise update the folds first.
    ಆದರೆ (invalid_ತುದಿ == (linenr_T)0)
	checkupdate(curwin);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == prev_lnum && prev_lnum_lvl >= 0)
	ಹಿಂತಿರುಗಿ prev_lnum_lvl;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum >= invalid_ತುದಿ && lnum <= invalid_bot)
	ಹಿಂತಿರುಗಿ -1;

    // Return quickly when there is no folding at all in this window.
    ಆದರೆ (!hasAnyFolding(curwin))
	ಹಿಂತಿರುಗಿ 0;

    ಹಿಂತಿರುಗಿ foldLevelWin(curwin, lnum);
}
# endif

/*
 * "foldlevel()" function
 */
    ಶೂನ್ಯ
f_foldlevel(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_FOLDING
    linenr_T	lnum;
    switchwin_T	switchwin;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T	*wp;
	wp = win_id2wp_tp(tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಆದರೆ (wp != ಖಾಲಿ && tp != ಖಾಲಿ)
	{
	    ಆದರೆ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    {
		lnum = tv_get_lnum(argvars);
		ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    rettv->vval.v_ಸಂಖ್ಯೆ = foldLevel(lnum);
	    }
	    restore_win(&switchwin, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_ಸಂಖ್ಯೆ = -2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	lnum = tv_get_lnum(argvars);
	ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    rettv->vval.v_ಸಂಖ್ಯೆ = foldLevel(lnum);
    }
# endif
}

/*
 * "foldtext()" function
 */
    ಶೂನ್ಯ
f_foldtext(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_FOLDING
    linenr_T	foldstart;
    linenr_T	foldend;
    char_u	*dashes;
    linenr_T	lnum;
    char_u	*s;
    char_u	*r;
    ಪೂರ್ಣಾಂಕ		len;
    ಅಕ್ಷರ	*txt;
    ದೀರ್ಘ	ಎಣಿಕೆ_2;
# endif

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
# ifdef FEAT_FOLDING
    foldstart = (linenr_T)get_vim_var_nr(VV_FOLDSTART);
    foldend = (linenr_T)get_vim_var_nr(VV_FOLDEND);
    dashes = get_vim_var_str(VV_FOLDDASHES);
    ಆದರೆ (foldstart > 0 && foldend <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && dashes != ಖಾಲಿ)
    {
	// Find first non-empty line in the fold.
	ಗಾಗಿ (lnum = foldstart; lnum < foldend; ++lnum)
	    ಆದರೆ (!linewhite(lnum))
		ನಿಲ್ಲಿಸು;

	// Find interesting text in this line.
	s = skipwhite(ml_get(lnum));
	// skip C comment-start
	ಆದರೆ (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
	{
	    s = skipwhite(s + 2);
	    ಆದರೆ (*skipwhite(s) == NUL
			    && lnum + 1 < (linenr_T)get_vim_var_nr(VV_FOLDEND))
	    {
		s = skipwhite(ml_get(lnum + 1));
		ಆದರೆ (*s == '*')
		    s = skipwhite(s + 1);
	    }
	}
	ಎಣಿಕೆ_2 = (ದೀರ್ಘ)(foldend - foldstart + 1);
	txt = NGETTEXT("+-%s%3ld line: ", "+-%s%3ld lines: ", ಎಣಿಕೆ_2);
	r = alloc(STRLEN(txt)
		    + STRLEN(dashes)	    // for %s
		    + 20		    // for %3ld
		    + STRLEN(s));	    // concatenated
	ಆದರೆ (r != ಖಾಲಿ)
	{
	    sprintf((ಅಕ್ಷರ *)r, txt, dashes, ಎಣಿಕೆ_2);
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(r);
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
f_foldtextresult(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

# ifdef FEAT_FOLDING
    linenr_T	lnum = 0;
    char_u	*ಪಠ್ಯ;
    char_u	buf[FOLD_ಪಠ್ಯ_LEN];
    foldinfo_T  foldinfo;
    ಪೂರ್ಣಾಂಕ		fold_ಎಣಿಕೆ;
    win_T	*wp  = curwin;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = FALSE;
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (entered)
	ಹಿಂತಿರುಗಿ; // reject recursive use
    entered = TRUE;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	switchwin_T	switchwin;
	// use window specified in the second argument
	wp = win_id2wp_tp(tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಆದರೆ (wp == ಖಾಲಿ || tp == ಖಾಲಿ) // wrong winid
	{
	    entered = FALSE;
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (switch_win(&switchwin, wp, tp, TRUE) == OK)
	    lnum = tv_get_lnum(argvars);
	restore_win(&switchwin, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lnum = tv_get_lnum(argvars);

    // treat illegal types and illegal string values for {lnum} the same
    ಆದರೆ (lnum < 0)
	lnum = 0;
    fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, &foldinfo);
    ಆದರೆ (fold_ಎಣಿಕೆ > 0)
    {
	ಪಠ್ಯ = get_foldtext(wp, lnum, lnum + fold_ಎಣಿಕೆ - 1,
							       &foldinfo, buf);
	ಆದರೆ (ಪಠ್ಯ == buf)
	    ಪಠ್ಯ = vim_strsave(ಪಠ್ಯ);
	rettv->vval.v_string = ಪಠ್ಯ;
    }

    entered = FALSE;
# endif
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
