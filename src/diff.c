/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * diff.c: code for diff'ing two, three or four buffers.
 *
 * There are three ways to diff:
 * - Shell out to an external diff program, using files.
 * - Use the compiled-in xdiff library.
 * - Let 'diffexpr' do the work, using files.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "xdiff/xdiff.h"

#ಆದರೆ defined(FEAT_DIFF)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_busy = FALSE;	    // using diff structs, don't change them
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_need_update = FALSE; // ex_diffupdate needs to be called

// flags obtained from the 'diffopt' option
# define DIFF_FILLER	0x001	// display filler lines
# define DIFF_IBLANK	0x002	// ignore empty lines
# define DIFF_ICASE	0x004	// ignore case
# define DIFF_IWHITE	0x008	// ignore change in white space
# define DIFF_IWHITEALL	0x010	// ignore all white space changes
# define DIFF_IWHITEEOL	0x020	// ignore change in white space at EOL
# define DIFF_HORIZONTAL	0x040	// horizontal splits
# define DIFF_VERTICAL	0x080	// vertical splits
# define DIFF_HIDDEN_OFF	0x100	// diffoff when hidden
# define DIFF_ಆಂತರಿಕ	0x200	// use internal xdiff algorithm
# define DIFF_CLOSE_OFF	0x400	// diffoff when closing window
# define DIFF_FOLLOWWRAP	0x800	// follow the wrap option
# define DIFF_LINEMATCH  0x1000  // match most similar lines within diff
# define DIFF_INLINE_NONE    0x2000  // no inline highlight
# define DIFF_INLINE_SIMPLE  0x4000  // inline highlight with simple algorithm
# define DIFF_INLINE_CHAR    0x8000  // inline highlight with character diff
# define DIFF_INLINE_ಶಬ್ದ    0x10000 // inline highlight with word diff
# define DIFF_ANCHOR	0x20000	// use 'diffanchors' to anchor the diff
# define ALL_WHITE_DIFF (DIFF_IWHITE | DIFF_IWHITEALL | DIFF_IWHITEEOL)
# define ALL_INLINE (DIFF_INLINE_NONE | DIFF_INLINE_SIMPLE | DIFF_INLINE_CHAR | DIFF_INLINE_ಶಬ್ದ)
# define ALL_INLINE_DIFF (DIFF_INLINE_CHAR | DIFF_INLINE_ಶಬ್ದ)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	diff_flags = DIFF_ಆಂತರಿಕ | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ಸ್ಥಿರ ದೀರ್ಘ diff_algorithm = XDF_INDENT_HEURISTIC;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_ಶಬ್ದ_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_ಗಾತ್ರ (1024L * 1024 * 1023)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# ಆದರೆ defined(MSWIN)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    linenr_T lnum_orig;
    ದೀರ್ಘ     ಎಣಿಕೆ_orig;
    linenr_T lnum_new;
    ದೀರ್ಘ     ಎಣಿಕೆ_new;
} diffhunk_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    ಪೂರ್ಣಾಂಕ		    dio_ಆಂತರಿಕ; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    ಪೂರ್ಣಾಂಕ		    dio_ctxlen;   // unified diff context length
} diffio_T;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_buf_idx(buf_T *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ಸ್ಥಿರ ಶೂನ್ಯ diff_mark_adjust_tp(tabpage_T *tp, ಪೂರ್ಣಾಂಕ idx, linenr_T line1, linenr_T line2, ದೀರ್ಘ amount, ದೀರ್ಘ amount_after);
ಸ್ಥಿರ ಶೂನ್ಯ diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_check_sanity(tabpage_T *tp, diff_T *dp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_external_diff(diffio_T *diffio);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_file(diffio_T *diffio);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_equal_ನಮೂದು(diff_T *dp, ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ಸ್ಥಿರ ಶೂನ್ಯ diff_fold_update(diff_T *dp, ಪೂರ್ಣಾಂಕ skip_idx);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ diff_read(ಪೂರ್ಣಾಂಕ idx_orig, ಪೂರ್ಣಾಂಕ idx_new, diffio_T *dio);
ಸ್ಥಿರ ಶೂನ್ಯ diff_copy_ನಮೂದು(diff_T *dprev, diff_T *dp, ಪೂರ್ಣಾಂಕ idx_orig, ಪೂರ್ಣಾಂಕ idx_new);
ಸ್ಥಿರ diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_diff_ed(char_u *ಸಾಲು, diffhunk_T *hunk);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_diff_unified(char_u *ಸಾಲು, diffhunk_T *hunk);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdiff_ಹೊರಹೋಗು_indices(ದೀರ್ಘ ಆರಂಭ_a, ದೀರ್ಘ ಎಣಿಕೆ_a, ದೀರ್ಘ ಆರಂಭ_b, ದೀರ್ಘ ಎಣಿಕೆ_b, ಶೂನ್ಯ *priv);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdiff_ಹೊರಹೋಗು_unified(ಶೂನ್ಯ *priv, mmbuffer_t *mb, ಪೂರ್ಣಾಂಕ nbuf);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_diffanchors(ಪೂರ್ಣಾಂಕ check_only, buf_T *buf, linenr_T *anchors, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_anchors);

# define FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(tp, dp) \
    ಗಾಗಿ ((dp) = (tp)->tp_ಮೊದಲ_diff; (dp) != ಖಾಲಿ; (dp) = (dp)->df_ಮುಂದಿನ)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ಸ್ಥಿರ diff_T	    *diff_finger_dp = ಖಾಲಿ;
ಸ್ಥಿರ tabpage_T    *diff_finger_tp = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    diff_finger_idx = -1;
ಸ್ಥಿರ linenr_T	    diff_finger_lnum = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_diffblock(diff_T *dp)
{
    ಆದರೆ (dp == diff_finger_dp)
	diff_finger_dp = ಖಾಲಿ;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ಸ್ಥಿರ diff_T *
diff_ಪರಿಶೀಲನೆ_ಆರಂಭ(tabpage_T *tp, ಪೂರ್ಣಾಂಕ idx, linenr_T lnum)
{
    ಆದರೆ (diff_finger_dp != ಖಾಲಿ
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	ಹಿಂತಿರುಗಿ diff_finger_dp;
    ಹಿಂತಿರುಗಿ tp->tp_ಮೊದಲ_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_ಪರಿಶೀಲನೆ_remember(tabpage_T *tp, ಪೂರ್ಣಾಂಕ idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ಖಾಲಿ ? dp : tp->tp_ಮೊದಲ_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    ಶೂನ್ಯ
diff_buf_delete(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ಆದರೆ (i != DB_ಎಣಿಕೆ)
	{
	    tp->tp_diffbuf[i] = ಖಾಲಿ;
	    tp->tp_diff_invalid = TRUE;
	    ಆದರೆ (tp == curtab)
	    {
		// don't redraw right away, more might change or buffer state
		// is invalid right now
		need_diff_redraw = TRUE;
		redraw_later(UPD_VALID);
	    }
	}
    }
}

/*
 * Check if the current buffer should be added to or removed from the list of
 * diff buffers.
 */
    ಶೂನ್ಯ
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == win->w_ಬಫರ್ && wp->w_p_diff)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (wp == ಖಾಲಿ)
	{
	    i = diff_buf_idx(win->w_ಬಫರ್);
	    ಆದರೆ (i != DB_ಎಣಿಕೆ)
	    {
		curtab->tp_diffbuf[i] = ಖಾಲಿ;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	diff_buf_ಕೂಡಿಸು(win->w_ಬಫರ್);
}

/*
 * Add a buffer to make diffs for.
 * Call this when a new buffer is being edited in the current window where
 * 'diff' is set.
 * Marks the current buffer as being part of the diff and requiring updating.
 * This must be done before any autocmd, because a command may use info
 * about the screen contents.
 */
    ಶೂನ್ಯ
diff_buf_ಕೂಡಿಸು(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (diff_buf_idx(buf) != DB_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;		// It's already there.

    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಆದರೆ (curtab->tp_diffbuf[i] == ಖಾಲಿ)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    ಹಿಂತಿರುಗಿ;
	}

    semsg(_(e_cannot_diff_ಇನ್ನಷ್ಟು_than_nr_buffers), DB_ಎಣಿಕೆ);
}

/*
 * Remove all buffers to make diffs for.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_buf_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
	{
	    curtab->tp_diffbuf[i] = ಖಾಲಿ;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_buf_idx(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ		idx;

    ಗಾಗಿ (idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
	ಆದರೆ (curtab->tp_diffbuf[idx] == buf)
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    ಪೂರ್ಣಾಂಕ		idx;

    ಗಾಗಿ (idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
	ಆದರೆ (tp->tp_diffbuf[idx] == buf)
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    ಶೂನ್ಯ
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ಆದರೆ (i != DB_ಎಣಿಕೆ)
	{
	    tp->tp_diff_invalid = TRUE;
	    ಆದರೆ (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    ಶೂನ್ಯ
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after)
{
    ಪೂರ್ಣಾಂಕ		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	ಆದರೆ (idx != DB_ಎಣಿಕೆ)
	    diff_mark_adjust_tp(tp, idx, line1, line2, amount, amount_after);
    }
}

/*
 * Update line numbers in tab page "tp" for "curbuf" with index "idx".
 * This attempts to update the changes as much as possible:
 * When inserting/deleting lines outside of existing change blocks, create a
 * new change block and update the line numbers in following blocks.
 * When inserting/deleting lines in existing change blocks, update them.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_mark_adjust_tp(
    tabpage_T	*tp,
    ಪೂರ್ಣಾಂಕ		idx,
    linenr_T	line1,
    linenr_T	line2,
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		inserted, deleted;
    ಪೂರ್ಣಾಂಕ		n, off;
    linenr_T	ಕೊನೆಯ;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    ಪೂರ್ಣಾಂಕ		check_unchanged;

    diff_finger_dp = ಖಾಲಿ;		// may shift df_lnum, invalidate the finger

    ಆದರೆ (diff_ಆಂತರಿಕ())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    ಆದರೆ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ಖಾಲಿ;
    dp = tp->tp_ಮೊದಲ_diff;
    ಗಾಗಿ (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	ಆದರೆ ((dp == ಖಾಲಿ || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ಖಾಲಿ
		    || dprev->df_lnum[idx] + dprev->df_ಎಣಿಕೆ[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    ಆದರೆ (dnext == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_ಎಣಿಕೆ[idx] = inserted;
	    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		ಆದರೆ (tp->tp_diffbuf[i] != ಖಾಲಿ && i != idx)
		{
		    ಆದರೆ (dprev == ಖಾಲಿ)
			dnext->df_lnum[i] = line1;
		    ಇಲ್ಲದಿದ್ದರೆ
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_ಎಣಿಕೆ[i])
			    - (dprev->df_lnum[idx] + dprev->df_ಎಣಿಕೆ[idx]);
		    dnext->df_ಎಣಿಕೆ[i] = deleted;
		}
	}

	// if at end of the list, quit
	ಆದರೆ (dp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

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
	// compute last line of this change
	ಕೊನೆಯ = dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx] - 1;

	// 1. change completely above line1: nothing to do
	ಆದರೆ (ಕೊನೆಯ >= line1 - 1)
	{
	    ಆದರೆ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		ಆದರೆ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_ಮುಂದಿನ;
		ಮುಂದುವರಿ;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    ಆದರೆ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		ಆದರೆ (amount_after == 0)
		    ನಿಲ್ಲಿಸು;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		ಆದರೆ (deleted > 0)
		{
		    off = 0;
		    ಆದರೆ (dp->df_lnum[idx] >= line1)
		    {
			ಆದರೆ (ಕೊನೆಯ <= line2)
			{
			    // 4. delete all lines of diff
			    ಆದರೆ (dp->df_ಮುಂದಿನ != ಖಾಲಿ
				    && dp->df_ಮುಂದಿನ->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_ಮುಂದಿನ->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_ಎಣಿಕೆ[idx];
				lnum_deleted = dp->df_ಮುಂದಿನ->df_lnum[idx];
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				n = deleted - dp->df_ಎಣಿಕೆ[idx];
			    dp->df_ಎಣಿಕೆ[idx] = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_ಎಣಿಕೆ[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (ಕೊನೆಯ < line2)
			{
			    // 2. delete at end of diff
			    dp->df_ಎಣಿಕೆ[idx] -= ಕೊನೆಯ - lnum_deleted + 1;
			    ಆದರೆ (dp->df_ಮುಂದಿನ != ಖಾಲಿ
				    && dp->df_ಮುಂದಿನ->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_ಮುಂದಿನ->df_lnum[idx] - 1 - ಕೊನೆಯ;
				deleted -= dp->df_ಮುಂದಿನ->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_ಮುಂದಿನ->df_lnum[idx];
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				n = line2 - ಕೊನೆಯ;
			    check_unchanged = TRUE;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_ಎಣಿಕೆ[idx] -= deleted;
			}
		    }

		    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
			ಆದರೆ (tp->tp_diffbuf[i] != ಖಾಲಿ && i != idx)
			{
			    ಆದರೆ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    ಇಲ್ಲದಿದ್ದರೆ
				dp->df_lnum[i] = 1;
			    dp->df_ಎಣಿಕೆ[i] += n;
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_ಎಣಿಕೆ[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		ಆದರೆ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	ಆದರೆ (dprev != ಖಾಲಿ && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_ಎಣಿಕೆ[idx]
							== dp->df_lnum[idx])
	{
	    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		ಆದರೆ (tp->tp_diffbuf[i] != ಖಾಲಿ)
		    dprev->df_ಎಣಿಕೆ[i] += dp->df_ಎಣಿಕೆ[i];
	    dprev->df_ಮುಂದಿನ = dp->df_ಮುಂದಿನ;
	    clear_diffblock(dp);
	    dp = dprev->df_ಮುಂದಿನ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_ಮುಂದಿನ;
	}
    }

    dprev = ಖಾಲಿ;
    dp = tp->tp_ಮೊದಲ_diff;
    ಆಗ (dp != ಖಾಲಿ)
    {
	// All counts are zero, remove this entry.
	ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	    ಆದರೆ (tp->tp_diffbuf[i] != ಖಾಲಿ && dp->df_ಎಣಿಕೆ[i] != 0)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (i == DB_ಎಣಿಕೆ)
	{
	    dnext = dp->df_ಮುಂದಿನ;
	    clear_diffblock(dp);
	    dp = dnext;
	    ಆದರೆ (dprev == ಖಾಲಿ)
		tp->tp_ಮೊದಲ_diff = dnext;
	    ಇಲ್ಲದಿದ್ದರೆ
		dprev->df_ಮುಂದಿನ = dnext;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_ಮುಂದಿನ;
	}

    }

    ಆದರೆ (tp == curtab)
    {
	// Don't redraw right away, this updates the diffs, which can be slow.
	need_diff_redraw = TRUE;

	// Need to recompute the scroll binding, may remove or add filler
	// lines (e.g., when adding lines above w_topline). But it's slow when
	// making many changes, postpone until redrawing.
	diff_need_scrollbind = TRUE;
    }
}

/*
 * Allocate a new diff block and link it between "dprev" and "dp".
 */
    ಸ್ಥಿರ diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    ಆದರೆ (dnew == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    diff_finger_dp = ಖಾಲಿ;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_ಮುಂದಿನ = dp;
    ಆದರೆ (dprev == ಖಾಲಿ)
	tp->tp_ಮೊದಲ_diff = dnew;
    ಇಲ್ಲದಿದ್ದರೆ
	dprev->df_ಮುಂದಿನ = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, ಗಾತ್ರ(diffline_change_T), 20);
    ಹಿಂತಿರುಗಿ dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    ಪೂರ್ಣಾಂಕ		i_org;
    ಪೂರ್ಣಾಂಕ		i_new;
    ಪೂರ್ಣಾಂಕ		off_org, off_new;
    char_u	*ಸಾಲು_org;
    ಪೂರ್ಣಾಂಕ		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    ಗಾಗಿ (i_org = 0; i_org < DB_ಎಣಿಕೆ; ++i_org)
	ಆದರೆ (tp->tp_diffbuf[i_org] != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i_org == DB_ಎಣಿಕೆ)	// safety check
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (diff_check_sanity(tp, dp) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    ಗಾಗಿ (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	ಆಗ (dp->df_ಎಣಿಕೆ[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    ಆದರೆ (dir == BACKWARD)
		off_org = dp->df_ಎಣಿಕೆ[i_org] - 1;
	    ಸಾಲು_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    ಆದರೆ (ಸಾಲು_org == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    ಗಾಗಿ (i_new = i_org + 1; i_new < DB_ಎಣಿಕೆ; ++i_new)
	    {
		ಆದರೆ (tp->tp_diffbuf[i_new] == ಖಾಲಿ)
		    ಮುಂದುವರಿ;
		ಆದರೆ (dir == BACKWARD)
		    off_new = dp->df_ಎಣಿಕೆ[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		ಆದರೆ (off_new < 0 || off_new >= dp->df_ಎಣಿಕೆ[i_new])
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (diff_cmp(ಸಾಲು_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    ನಿಲ್ಲಿಸು;
	    }
	    vim_free(ಸಾಲು_org);

	    // Stop when a line isn't equal in all diff buffers.
	    ಆದರೆ (i_new != DB_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;

	    // Line matched in all buffers, remove it from the diff.
	    ಗಾಗಿ (i_new = i_org; i_new < DB_ಎಣಿಕೆ; ++i_new)
		ಆದರೆ (tp->tp_diffbuf[i_new] != ಖಾಲಿ)
		{
		    ಆದರೆ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_ಎಣಿಕೆ[i_new];
		}
	}
	ಆದರೆ (dir == BACKWARD)
	    ನಿಲ್ಲಿಸು;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಆದರೆ (tp->tp_diffbuf[i] != ಖಾಲಿ)
	    ಆದರೆ (dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    ಶೂನ್ಯ
diff_redraw(
    ಪೂರ್ಣಾಂಕ		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ_max_fill_other = FALSE;
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ_max_fill_curwin = FALSE;
    ಪೂರ್ಣಾಂಕ		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	ಆದರೆ (!wp->w_p_diff || !buf_valid(wp->w_ಬಫರ್))
	    ಮುಂದುವರಿ;

	redraw_win_later(wp, UPD_SOME_VALID);
	ಆದರೆ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	ಆದರೆ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	ಆದರೆ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    ಆದರೆ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		ಆದರೆ (wp == curwin)
		    ಬಳಸಿದ_max_fill_curwin = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp_other != ಖಾಲಿ)
		    ಬಳಸಿದ_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    ಆದರೆ (wp_other != ಖಾಲಿ && curwin->w_p_scb)
    {
	ಆದರೆ (ಬಳಸಿದ_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_ಗಣ_topline(wp_other, curwin);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಳಸಿದ_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_ಗಣ_topline(curwin, wp_other);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_diffin(diffin_T *din)
{
    ಆದರೆ (din->din_fname == ಖಾಲಿ)
	VIM_CLEAR(din->din_mmfile.ptr);
    ಇಲ್ಲದಿದ್ದರೆ
	mch_remove(din->din_fname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_diffout(diffout_T *dout)
{
    ಆದರೆ (dout->dout_fname == ಖಾಲಿ)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&dout->dout_ga);
    ಇಲ್ಲದಿದ್ದರೆ
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_write_ಬಫರ್(buf_T *buf, diffin_T *din, linenr_T ಆರಂಭ, linenr_T ಕೊನೆ)
{
    linenr_T	lnum;
    char_u	*s;
    ದೀರ್ಘ	len = 0;
    char_u	*ptr;

    ಆದರೆ (ಕೊನೆ < 0)
	ಕೊನೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (buf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು || ಕೊನೆ < ಆರಂಭ)
    {
	din->din_mmfile.ptr = ಖಾಲಿ;
	din->din_mmfile.ಗಾತ್ರ_2 = 0;
	ಹಿಂತಿರುಗಿ OK;
    }

    // xdiff requires one big block of memory with all the text.
    ಗಾಗಿ (lnum = ಆರಂಭ; lnum <= ಕೊನೆ; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    ಆದರೆ (ptr == ಖಾಲಿ)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }
    din->din_mmfile.ptr = (ಅಕ್ಷರ *)ptr;
    din->din_mmfile.ಗಾತ್ರ_2 = len;

    len = 0;
    ಗಾಗಿ (lnum = ಆರಂಭ; lnum <= ಕೊನೆ; ++lnum)
    {
	ಗಾಗಿ (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    ಆದರೆ (diff_flags & DIFF_ICASE)
	    {
		ಪೂರ್ಣಾಂಕ c;
		ಪೂರ್ಣಾಂಕ	orig_len;
		ಪೂರ್ಣಾಂಕ	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		ಆದರೆ (*s == NL)
		    c = NUL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		ಆದರೆ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    ಆದರೆ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_write(buf_T *buf, diffin_T *din, linenr_T ಆರಂಭ, linenr_T ಕೊನೆ)
{
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		save_ml_flags;
    char_u	*save_ff;
    ಪೂರ್ಣಾಂಕ		save_cmod_flags;

    ಆದರೆ (din->din_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ diff_write_ಬಫರ್(buf, din, ಆರಂಭ, ಕೊನೆ);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    ಆದರೆ (ಚೌಕಟ್ಟುಗಳು_locked())
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ಕೊನೆ < 0)
	ಕೊನೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    ಆದರೆ (ಕೊನೆ < ಆರಂಭ)
    {
	// The line range specifies a completely empty file.
	ಕೊನೆ = ಆರಂಭ;
	buf->b_ml.ml_flags |= ML_ಖಾಲಿಗಳು;
    }
    r = buf_write(buf, din->din_fname, ಖಾಲಿ,
			ಆರಂಭ, ಕೊನೆ,
			ಖಾಲಿ, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_ಖಾಲಿಗಳು) | (save_ml_flags & ML_ಖಾಲಿಗಳು);
    ಹಿಂತಿರುಗಿ r;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lnum_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    ಆದರೆ (lnum1 < lnum2)
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (lnum1 > lnum2)
	ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_try_update(
	diffio_T    *dio,
	ಪೂರ್ಣಾಂಕ	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		idx_new;

    ಆದರೆ (dio->dio_ಆಂತರಿಕ)
    {
	ga_init2(&dio->dio_diff.dout_ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 1000);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	ಆದರೆ (dio->dio_orig.din_fname == ಖಾಲಿ
		|| dio->dio_new.din_fname == ಖಾಲಿ
		|| dio->dio_diff.dout_fname == ಖಾಲಿ)
	    ಹೋಗು theend;
    }

    // Check external diff is actually working.
    ಆದರೆ (!dio->dio_ಆಂತರಿಕ && check_external_diff(dio) == FAIL)
	ಹೋಗು theend;

    // :diffupdate!
    ಆದರೆ (eap != ಖಾಲಿ && eap->forceit)
	ಗಾಗಿ (idx_new = idx_orig; idx_new < DB_ಎಣಿಕೆ; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ಆದರೆ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_anchors = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    linenr_T anchors[DB_ಎಣಿಕೆ][MAX_DIFF_ANCHORS];
    CLEAR_ಕ್ಷೇತ್ರ(anchors);
    ಆದರೆ (diff_flags & DIFF_ANCHOR)
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < DB_ಎಣಿಕೆ; idx++)
	{
	    ಆದರೆ (curtab->tp_diffbuf[idx] == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    ಪೂರ್ಣಾಂಕ buf_ಅಂಕೆ_anchors = 0;
	    ಆದರೆ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_ಅಂಕೆ_anchors) != OK)
	    {
		emsg(_(e_failed_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ_all_diff_anchors));
		ಅಂಕೆ_anchors = 0;
		CLEAR_ಕ್ಷೇತ್ರ(anchors);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (buf_ಅಂಕೆ_anchors < ಅಂಕೆ_anchors)
		ಅಂಕೆ_anchors = buf_ಅಂಕೆ_anchors;

	    ಆದರೆ (buf_ಅಂಕೆ_anchors > 0)
		qsort((ಶೂನ್ಯ *)anchors[idx],
			(size_t)buf_ಅಂಕೆ_anchors,
			ಗಾತ್ರ(linenr_T),
			lnum_compare);
	}
    }
    ಆದರೆ (ಅಂಕೆ_anchors == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	ಅಂಕೆ_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ anchor_i = 0; anchor_i <= ಅಂಕೆ_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ಖಾಲಿ;
	ಆದರೆ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_ಮೊದಲ_diff;
	    curtab->tp_ಮೊದಲ_diff = ಖಾಲಿ;
	}
	linenr_T lnum_ಆರಂಭ = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_ಕೊನೆ = (anchor_i == ಅಂಕೆ_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	ಆದರೆ (diff_write(buf, &dio->dio_orig, lnum_ಆರಂಭ, lnum_ಕೊನೆ) == FAIL)
	{
	    ಆದರೆ (orig_diff != ಖಾಲಿ)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_ಮೊದಲ_diff = orig_diff;
		diff_clear(curtab);
	    }
	    ಹೋಗು theend;
	}

	// Make a difference between the first buffer and every other.
	ಗಾಗಿ (idx_new = idx_orig + 1; idx_new < DB_ಎಣಿಕೆ; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ)
		ಮುಂದುವರಿ; // skip buffer that isn't loaded

	    lnum_ಆರಂಭ = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_ಕೊನೆ = anchor_i == ಅಂಕೆ_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    ಆದರೆ (diff_write(buf, &dio->dio_new, lnum_ಆರಂಭ, lnum_ಕೊನೆ) == FAIL)
		ಮುಂದುವರಿ;
	    ಆದರೆ (diff_file(dio) == FAIL)
		ಮುಂದುವರಿ;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	ಆದರೆ (anchor_i != 0)
	{
	    diff_finger_dp = ಖಾಲಿ;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    ಗಾಗಿ (diff_T *dp = curtab->tp_ಮೊದಲ_diff; dp != ಖಾಲಿ; dp = dp->df_ಮುಂದಿನ)
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < DB_ಎಣಿಕೆ; idx++)
		{
		    ಆದರೆ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    ಆದರೆ (orig_diff != ಖಾಲಿ)
	    {
		diff_T *ಕೊನೆಯ_diff = orig_diff;
		ಆಗ (ಕೊನೆಯ_diff->df_ಮುಂದಿನ != ಖಾಲಿ)
		    ಕೊನೆಯ_diff = ಕೊನೆಯ_diff->df_ಮುಂದಿನ;
		ಕೊನೆಯ_diff->df_ಮುಂದಿನ = curtab->tp_ಮೊದಲ_diff;
		curtab->tp_ಮೊದಲ_diff = orig_diff;
	    }
	}
    }

theend:
    vim_free(dio->dio_orig.din_fname);
    vim_free(dio->dio_new.din_fname);
    vim_free(dio->dio_diff.dout_fname);
}

/*
 * Return TRUE if the options are set to use the internal diff library.
 * Note that if the internal diff failed for one of the buffers, the external
 * diff will be used anyway.
 */
    ಪೂರ್ಣಾಂಕ
diff_ಆಂತರಿಕ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (diff_flags & DIFF_ಆಂತರಿಕ) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_ಆಂತರಿಕ_failed(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ idx;

    // Only need to do something when there is another buffer.
    ಗಾಗಿ (idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
	ಆದರೆ (curtab->tp_diffbuf[idx] != ಖಾಲಿ
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    ಶೂನ್ಯ
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    ಪೂರ್ಣಾಂಕ		idx_orig;
    ಪೂರ್ಣಾಂಕ		idx_new;
    diffio_T	diffio;
    ಪೂರ್ಣಾಂಕ		had_diffs = curtab->tp_ಮೊದಲ_diff != ಖಾಲಿ;

    ಆದರೆ (diff_busy)
    {
	diff_need_update = TRUE;
	ಹಿಂತಿರುಗಿ;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    ಗಾಗಿ (idx_orig = 0; idx_orig < DB_ಎಣಿಕೆ; ++idx_orig)
	ಆದರೆ (curtab->tp_diffbuf[idx_orig] != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (idx_orig == DB_ಎಣಿಕೆ)
	ಹೋಗು theend;

    // Only need to do something when there is another buffer.
    ಗಾಗಿ (idx_new = idx_orig + 1; idx_new < DB_ಎಣಿಕೆ; ++idx_new)
	ಆದರೆ (curtab->tp_diffbuf[idx_new] != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (idx_new == DB_ಎಣಿಕೆ)
	ಹೋಗು theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_ಕ್ಷೇತ್ರ(diffio);
    diffio.dio_ಆಂತರಿಕ = diff_ಆಂತರಿಕ() && !diff_ಆಂತರಿಕ_failed();

    diff_try_update(&diffio, idx_orig, eap);
    ಆದರೆ (diffio.dio_ಆಂತರಿಕ && diff_ಆಂತರಿಕ_failed())
    {
	// Internal diff failed, use external diff instead.
	CLEAR_ಕ್ಷೇತ್ರ(diffio);
	diff_try_update(&diffio, idx_orig, eap);
    }

    // force updating cursor position on screen
    curwin->w_valid_cursor.lnum = 0;

theend:
    // A redraw is needed if there were diffs and they were cleared, or there
    // are diffs now, which means they got updated.
    ಆದರೆ (had_diffs || curtab->tp_ಮೊದಲ_diff != ಖಾಲಿ)
    {
	diff_redraw(TRUE);
	ಕಿಟಕಿ_layout_ಬೀಗ();
	apply_autocmds(EVENT_DIFFUPDATED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಕಿಟಕಿ_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    ಪೂರ್ಣಾಂಕ		ok;
    ಪೂರ್ಣಾಂಕ		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    ಗಾಗಿ (;;)
    {
	ok = FALSE;
	fd = mch_fopen((ಅಕ್ಷರ *)diffio->dio_orig.din_fname, "w");
	ಆದರೆ (fd == ಖಾಲಿ)
	    io_error = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((ಅಕ್ಷರ *)diffio->dio_new.din_fname, "w");
	    ಆದರೆ (fd == ಖಾಲಿ)
		io_error = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ಖಾಲಿ;
		ಆದರೆ (diff_file(diffio) == OK)
		    fd = mch_fopen((ಅಕ್ಷರ *)diffio->dio_diff.dout_fname, "r");
		ಆದರೆ (fd == ಖಾಲಿ)
		    io_error = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u	linebuf[LBUFLEN];

		    ಗಾಗಿ (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			ಆದರೆ (vim_fgets(linebuf, LBUFLEN, fd))
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (STRNCMP(linebuf, "1c1", 3) == 0
				|| STRNCMP(linebuf, "@@ -1 +1 @@", 11) == 0)
			    ok = TRUE;
		    }
		    fclose(fd);
		}
		mch_remove(diffio->dio_diff.dout_fname);
		mch_remove(diffio->dio_new.din_fname);
	    }
	    mch_remove(diffio->dio_orig.din_fname);
	}

# ifdef FEAT_EVAL
	// When using 'diffexpr' break here.
	ಆದರೆ (*p_dex != NUL)
	    ನಿಲ್ಲಿಸು;
# endif

# ಆದರೆ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	ಆದರೆ (ok && diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ == MAYBE && diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ == MAYBE)
	{
	    diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ = TRUE;
	    diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ = TRUE;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (!ok && diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ == TRUE && diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	ಆದರೆ (diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ != MAYBE)
	    ನಿಲ್ಲಿಸು;
	diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ = ok;

	// If "-a" works break here, otherwise retry without "-a".
	ಆದರೆ (ok)
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (!ok)
    {
	ಆದರೆ (io_error)
	    emsg(_(e_cannot_read_or_write_ತಾತ್ಕಾಲಿಕ_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ = MAYBE;
# ಆದರೆ defined(MSWIN)
	diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ = MAYBE;
# endif
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Invoke the xdiff function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_file_ಆಂತರಿಕ(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_ಕ್ಷೇತ್ರ(param);
    CLEAR_ಕ್ಷೇತ್ರ(emit_cfg);
    CLEAR_ಕ್ಷೇತ್ರ(emit_cb);

    param.flags = diff_algorithm;

    ಆದರೆ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    ಆದರೆ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    ಆದರೆ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    ಆದರೆ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_ಸಾಲುಗಳು;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    ಆದರೆ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_ಹೊರಹೋಗು_indices;
    ಇಲ್ಲದಿದ್ದರೆ
	emit_cb.ಹೊರಹೋಗು_ಸಾಲು = xdiff_ಹೊರಹೋಗು_unified;
    ಆದರೆ (diffio->dio_orig.din_mmfile.ಗಾತ್ರ_2 > MAX_XDIFF_ಗಾತ್ರ ||
	diffio->dio_new.din_mmfile.ಗಾತ್ರ_2 > MAX_XDIFF_ಗಾತ್ರ)
    {
	emsg(_(e_problem_creating_ಆಂತರಿಕ_diff));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_ಆಂತರಿಕ_diff));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    ಆದರೆ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    // Use xdiff for generating the diff.
    ಆದರೆ (dio->dio_ಆಂತರಿಕ)
	ಹಿಂತಿರುಗಿ diff_file_ಆಂತರಿಕ(dio);

    ಆದರೆ (check_restricted())
	ಹಿಂತಿರುಗಿ FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    ಆದರೆ (cmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    ಆದರೆ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((ಅಕ್ಷರ *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_ಕೆಲಸಮಾಡುತ್ತದೆ == FALSE ? "" : "-a ",
# ಆದರೆ defined(MSWIN)
	    diff_bin_ಕೆಲಸಮಾಡುತ್ತದೆ == TRUE ? "--binary " : "",
# ಇಲ್ಲದಿದ್ದರೆ
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (ಪೂರ್ಣಾಂಕ)len, p_srr, tmp_diff);
    ಖಂಡ_autocmds();	// avoid ShellCmdPost stuff
    (ಶೂನ್ಯ)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    ಶೂನ್ಯ
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = ಖಾಲಿ;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ಖಾಲಿ;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ಖಾಲಿ;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ಖಾಲಿ;

# ifdef FEAT_BROWSE
    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ಖಾಲಿ, ಖಾಲಿ,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ಖಾಲಿ);
	ಆದರೆ (browseFile == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    ಆದರೆ (tmp_orig == ಖಾಲಿ || tmp_new == ಖಾಲಿ)
	ಹೋಗು theend;

    // Write the current buffer to "tmp_orig".
    ಆದರೆ (buf_write(curbuf, tmp_orig, ಖಾಲಿ,
		(linenr_T)1, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
				     ಖಾಲಿ, FALSE, FALSE, FALSE, TRUE) == FAIL)
	ಹೋಗು theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = ತುಂಬಿದ_Name_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ಖಾಲಿ ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    ಆದರೆ (esc_name == ಖಾಲಿ)
	ಹೋಗು theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹೋಗು theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    ಆದರೆ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((ಅಕ್ಷರ *)dirbuf) != 0)
	dirbuf[0] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
#  ifdef TEMPDIRNAMES
	ಆದರೆ (vim_tempdir != ಖಾಲಿ)
	    vim_ignored = mch_chdir((ಅಕ್ಷರ *)vim_tempdir);
	ಇಲ್ಲದಿದ್ದರೆ
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    ಆದರೆ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ಖಾಲಿ ? fullname :
#  endif
		eap->arg, tmp_new);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	ಆದರೆ (check_restricted())
	    ಹೋಗು theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((ಅಕ್ಷರ *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	ಖಂಡ_autocmds();	// Avoid ShellCmdPost stuff
	(ಶೂನ್ಯ)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    ಆದರೆ (dirbuf[0] != NUL)
    {
	ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)dirbuf) != 0)
	    emsg(_(e_cannot_go_ಹಿಂಭಾಗ_ಗಮ್ಯ_ಹಿಂದಿನ_directory));
	shorten_fnames(TRUE);
    }
# endif

    // patch probably has written over the screen
    redraw_later(UPD_CLEAR);

    // Delete any .orig or .rej file created.
    STRCPY(buf, tmp_new);
    STRCAT(buf, ".orig");
    mch_remove(buf);
    STRCPY(buf, tmp_new);
    STRCAT(buf, ".rej");
    mch_remove(buf);

    // Only continue if the output file was created.
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (curbuf->b_fname != ಖಾಲಿ)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    ಆದರೆ (newname != ಖಾಲಿ)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	ಆದರೆ (win_ವಿಭಜಿಸು(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_ವಿಭಜಿಸು;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    ಆದರೆ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		ಆದರೆ (newname != ಖಾಲಿ)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    ಆದರೆ (au_has_ಗುಂಪು((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    ಆದರೆ (tmp_orig != ಖಾಲಿ)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    ಆದರೆ (tmp_new != ಖಾಲಿ)
	mch_remove(tmp_new);
    vim_free(tmp_new);
    vim_free(newname);
    vim_free(buf);
# ifdef UNIX
    vim_free(fullname);
# endif
    vim_free(esc_name);
# ifdef FEAT_BROWSE
    vim_free(browseFile);
    cmdmod.cmod_flags = save_cmod_flags;
# endif
}

/*
 * Split the window and edit another file, setting options to show the diffs.
 */
    ಶೂನ್ಯ
ex_diffsplit(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    bufref_T	old_curbuf;

    ಗಣ_bufref(&old_curbuf, curbuf);
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    // Need to compute w_fraction when no redraw happened yet.
    validate_cursor();
    ಗಣ_fraction(curwin);

    // don't use a new tab page, each tab page has its own diffs
    cmdmod.cmod_tab = 0;

    ಆದರೆ (win_ವಿಭಜಿಸು(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_ವಿಭಜಿಸು;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    ಆದರೆ (curwin == old_curwin)		// split didn't work
	ಹಿಂತಿರುಗಿ;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    ಆದರೆ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	ಆದರೆ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_ಸಾಲು(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_ಗಮ್ಯ_fraction(curwin, curwin->w_ಎತ್ತರ);
}

/*
 * Set options to show diffs for the current window.
 */
    ಶೂನ್ಯ
ex_diffthis(exarg_T *eap ಬಳಸದ)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_diff_option(win_T *wp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_ಬಫರ್;
    ++curbuf_ಬೀಗ;
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"diff", (ದೀರ್ಘ)ಮೌಲ್ಯ, ಖಾಲಿ, OPT_LOCAL);
    --curbuf_ಬೀಗ;
    curwin = old_curwin;
    curbuf = curwin->w_ಬಫರ್;
}

/*
 * Set options in window "wp" for diff mode.
 */
    ಶೂನ್ಯ
diff_win_options(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    ಆದರೆ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    ಆದರೆ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    ಆದರೆ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	ಆದರೆ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    ಆದರೆ (!wp->w_p_diff)
    {
	ಆದರೆ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    ಆದರೆ (!wp->w_p_diff)
    {
	wp->w_p_fdc_save = wp->w_p_fdc;
	wp->w_p_fen_save = wp->w_p_fen;
	wp->w_p_fdl_save = wp->w_p_fdl;
    }
    wp->w_p_fdc = diff_foldcolumn;
    wp->w_p_fen = TRUE;
    wp->w_p_fdl = 0;
    foldUpdateAll(wp);
    // make sure topline is not halfway a fold
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);
# endif
    ಆದರೆ (vim_strchr(p_sbo, 'h') == ಖಾಲಿ)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    ಗಣ_diff_option(wp, TRUE);

    ಆದರೆ (addbuf)
	diff_buf_ಕೂಡಿಸು(wp->w_ಬಫರ್);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    ಶೂನ್ಯ
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    ಗಣ_diff_option(wp, FALSE);

	    ಆದರೆ (wp->w_p_diff_saved)
	    {

		ಆದರೆ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		ಆದರೆ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		ಆದರೆ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    ಆದರೆ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		ಆದರೆ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		ಆದರೆ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		ಆದರೆ (wp->w_p_fen)
		    wp->w_p_fen = foldmethodIsManual(wp) ? FALSE
							 : wp->w_p_fen_save;

		foldUpdateAll(wp);
# endif
	    }
	    // remove filler lines
	    wp->w_topfill = 0;

	    // make sure topline is not halfway a fold and cursor is
	    // invalidated
	    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);

	    // Note: 'sbo' is not restored, it's a global option.
	    diff_buf_adjust(wp);
	}
	diffwin |= wp->w_p_diff;
    }

    // Also remove hidden buffers from the list.
    ಆದರೆ (eap->forceit)
	diff_buf_clear();

    ಆದರೆ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    ಆದರೆ (!diffwin && vim_strchr(p_sbo, 'h') != ಖಾಲಿ)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_read(
    ಪೂರ್ಣಾಂಕ		idx_orig,	// idx of original file
    ಪೂರ್ಣಾಂಕ		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ಖಾಲಿ;
    diff_T	*dp = curtab->tp_ಮೊದಲ_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*ಸಾಲು;
    ದೀರ್ಘ	off;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ಖಾಲಿ;	    // init to avoid gcc warning

    ಎಣಿಕೆ {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    ಆದರೆ (dout->dout_fname == ಖಾಲಿ)
    {
	diffstyle = DIFF_UNIFIED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fd = mch_fopen((ಅಕ್ಷರ *)dout->dout_fname, "r");
	ಆದರೆ (fd == ಖಾಲಿ)
	{
	    emsg(_(e_cannot_read_diff_output));
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (!dio->dio_ಆಂತರಿಕ)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	ಆದರೆ (hunk == ಖಾಲಿ)
	{
	    ಆದರೆ (fd != ಖಾಲಿ)
		fclose(fd);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (dio->dio_ಆಂತರಿಕ)
	{
	    ಆದರೆ (ಸಾಲು_hunk_idx >= dout->dout_ga.ga_len)
		ನಿಲ್ಲಿಸು;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_ದತ್ತಾಂಶ)[ಸಾಲು_hunk_idx++];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fd == ಖಾಲಿ)
	    {
		ಆದರೆ (ಸಾಲು_hunk_idx >= dout->dout_ga.ga_len)
		    ನಿಲ್ಲಿಸು;	    // did last line
		ಸಾಲು = ((char_u **)dout->dout_ga.ga_ದತ್ತಾಂಶ)[ಸಾಲು_hunk_idx++];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (vim_fgets(linebuf, LBUFLEN, fd))
		    ನಿಲ್ಲಿಸು;		// end of file
		ಸಾಲು = linebuf;
	    }

	    ಆದರೆ (diffstyle == DIFF_NONE)
	    {
		// Determine diff style.
		// ed like diff looks like this:
		// {first}[,{last}]c{first}[,{last}]
		// {first}a{first}[,{last}]
		// {first}[,{last}]d{first}
		//
		// unified diff looks like this:
		// --- file1       2018-03-20 13:23:35.783153140 +0100
		// +++ file2       2018-03-20 13:23:41.183156066 +0100
		// @@ -1,3 +1,5 @@
		ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(*ಸಾಲು))
		    diffstyle = DIFF_ED;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((STRNCMP(ಸಾಲು, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((STRNCMP(ಸಾಲು, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ಸಾಲು, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ಸಾಲು, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ಇಲ್ಲದಿದ್ದರೆ
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (diffstyle == DIFF_ED)
	    {
		ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(*ಸಾಲು))
		    ಮುಂದುವರಿ;	// not the start of a diff block
		ಆದರೆ (parse_diff_ed(ಸಾಲು, hunk) == FAIL)
		    ಮುಂದುವರಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (diffstyle == DIFF_UNIFIED)
	    {
		ಆದರೆ (STRNCMP(ಸಾಲು, "@@ ", 3)  != 0)
		    ಮುಂದುವರಿ;	// not the start of a diff block
		ಆದರೆ (parse_diff_unified(ಸಾಲು, hunk) == FAIL)
		    ಮುಂದುವರಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		emsg(_(e_invalid_diff_format));
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	ಆಗ (dp != ಖಾಲಿ
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_ಎಣಿಕೆ[idx_orig])
	{
	    ಆದರೆ (notset)
		diff_copy_ನಮೂದು(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_ಮುಂದಿನ;
	    notset = TRUE;
	}

	ಆದರೆ (dp != ಖಾಲಿ
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_ಎಣಿಕೆ[idx_orig]
		&& hunk->lnum_orig + hunk->ಎಣಿಕೆ_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    ಗಾಗಿ (dpl = dp; dpl->df_ಮುಂದಿನ != ಖಾಲಿ; dpl = dpl->df_ಮುಂದಿನ)
		ಆದರೆ (hunk->lnum_orig + hunk->ಎಣಿಕೆ_orig
					     < dpl->df_ಮುಂದಿನ->df_lnum[idx_orig])
		    ನಿಲ್ಲಿಸು;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    ಆದರೆ (off > 0)
	    {
		ಗಾಗಿ (i = idx_orig; i < idx_new; ++i)
		    ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
		    {
			dp->df_lnum[i] -= off;
			dp->df_ಎಣಿಕೆ[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_ಎಣಿಕೆ[idx_new] = hunk->ಎಣಿಕೆ_new;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_ಎಣಿಕೆ[idx_new] = hunk->ಎಣಿಕೆ_new - off;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		ಪೂರ್ಣಾಂಕ orig_ಗಾತ್ರ_ಒಳ_dp = MIN(hunk->ಎಣಿಕೆ_orig,
			dp->df_lnum[idx_orig] +
			dp->df_ಎಣಿಕೆ[idx_orig] - hunk->lnum_orig);
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_diff = hunk->ಎಣಿಕೆ_new - orig_ಗಾತ್ರ_ಒಳ_dp;
		dp->df_ಎಣಿಕೆ[idx_new] += ಗಾತ್ರ_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->ಎಣಿಕೆ_new
		    - (dp->df_lnum[idx_new] + dp->df_ಎಣಿಕೆ[idx_new]);
		ಆದರೆ (off > 0)
		    dp->df_ಎಣಿಕೆ[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->ಎಣಿಕೆ_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_ಎಣಿಕೆ[idx_orig]);
	    ಆದರೆ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		ಆದರೆ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_ಎಣಿಕೆ[idx_new] += -off;
		}
		off = 0;
	    }
	    ಗಾಗಿ (i = idx_orig; i < idx_new; ++i)
		ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
		    dp->df_ಎಣಿಕೆ[i] = dpl->df_lnum[i] + dpl->df_ಎಣಿಕೆ[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_ಮುಂದಿನ;
	    dp->df_ಮುಂದಿನ = dpl->df_ಮುಂದಿನ;
	    ಆಗ (dn != dp->df_ಮುಂದಿನ)
	    {
		dpl = dn->df_ಮುಂದಿನ;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    ಆದರೆ (dp == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_ಎಣಿಕೆ[idx_orig] = hunk->ಎಣಿಕೆ_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_ಎಣಿಕೆ[idx_new] = hunk->ಎಣಿಕೆ_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    ಗಾಗಿ (i = idx_orig + 1; i < idx_new; ++i)
		ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
		    diff_copy_ನಮೂದು(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    ಆಗ (dp != ಖಾಲಿ)
    {
	ಆದರೆ (notset)
	    diff_copy_ನಮೂದು(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_ಮುಂದಿನ;
	notset = TRUE;
    }

ಮುಗಿದ:
    ಆದರೆ (!dio->dio_ಆಂತರಿಕ)
	vim_free(hunk);

    ಆದರೆ (fd != ಖಾಲಿ)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_copy_ನಮೂದು(
    diff_T	*dprev,
    diff_T	*dp,
    ಪೂರ್ಣಾಂಕ		idx_orig,
    ಪೂರ್ಣಾಂಕ		idx_new)
{
    ದೀರ್ಘ	off;

    ಆದರೆ (dprev == ಖಾಲಿ)
	off = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	off = (dprev->df_lnum[idx_orig] + dprev->df_ಎಣಿಕೆ[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_ಎಣಿಕೆ[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_ಎಣಿಕೆ[idx_new] = dp->df_ಎಣಿಕೆ[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    ಶೂನ್ಯ
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *ಮುಂದಿನ_p;

    ಗಾಗಿ (p = tp->tp_ಮೊದಲ_diff; p != ಖಾಲಿ; p = ಮುಂದಿನ_p)
    {
	ಮುಂದಿನ_p = p->df_ಮುಂದಿನ;
	clear_diffblock(p);
    }
    tp->tp_ಮೊದಲ_diff = ಖಾಲಿ;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_linematch(diff_T *dp)
{
    ಆದರೆ (!(diff_flags & DIFF_LINEMATCH))
	ಹಿಂತಿರುಗಿ 0;

    // are there more than three diff buffers?
    ಪೂರ್ಣಾಂಕ tsize = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    ಆದರೆ (dp->df_ಎಣಿಕೆ[i] < 0)
		ಹಿಂತಿರುಗಿ FALSE;
	    tsize += dp->df_ಎಣಿಕೆ[i];
	}
    }

    // avoid allocating a huge array because it will lag
    ಹಿಂತಿರುಗಿ tsize <= linematch_ಸಾಲುಗಳು;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_max_diff_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ diff_T *dp)
{
    ಪೂರ್ಣಾಂಕ maxlength = 0;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < DB_ಎಣಿಕೆ; k++)
    {
	ಆದರೆ (curtab->tp_diffbuf[k] != ಖಾಲಿ)
	{
	    ಆದರೆ (dp->df_ಎಣಿಕೆ[k] > maxlength)
		maxlength = dp->df_ಎಣಿಕೆ[k];
	}
    }
    ಹಿಂತಿರುಗಿ maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ತುದಿ_diff_ಖಂಡ(
    diff_T	**thistopdiff,
    diff_T	**ಮುಂದಿನ_adjacent_ಖಂಡಗಳು,
    ಪೂರ್ಣಾಂಕ		fromidx,
    ಪೂರ್ಣಾಂಕ		topline)
{
    diff_T	*topdiff = ಖಾಲಿ;
    diff_T	*localtopdiff = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		topdiffchange = 0;

    ಗಾಗಿ (topdiff = curtab->tp_ಮೊದಲ_diff; topdiff != ಖಾಲಿ;
						topdiff = topdiff->df_ಮುಂದಿನ)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	ಆದರೆ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	ಆದರೆ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_ಎಣಿಕೆ[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    ಆದರೆ ((*thistopdiff) == ಖಾಲಿ)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	ಆದರೆ (!(topdiff->df_ಮುಂದಿನ && (topdiff->df_ಮುಂದಿನ->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_ಎಣಿಕೆ[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    ಆದರೆ (*thistopdiff)
	    {
		(*ಮುಂದಿನ_adjacent_ಖಂಡಗಳು) = topdiff->df_ಮುಂದಿನ;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
calculate_topfill_and_topline(
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	fromidx,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	toidx,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	from_topline,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	from_topfill,
    ಪೂರ್ಣಾಂಕ		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ಖಾಲಿ;
    diff_T	*ಮುಂದಿನ_adjacent_ಖಂಡಗಳು = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		virtual_ಸಾಲುಗಳು_passed = 0;
    ಪೂರ್ಣಾಂಕ		curlinenum_ಗಮ್ಯ = 1;

    ಕಂಡುಹಿಡಿ_ತುದಿ_diff_ಖಂಡ(&thistopdiff, &ಮುಂದಿನ_adjacent_ಖಂಡಗಳು, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    ಆಗ (curdif && (curdif->df_lnum[fromidx] + curdif->df_ಎಣಿಕೆ[fromidx])
							<= from_topline)
    {
	virtual_ಸಾಲುಗಳು_passed += get_max_diff_ಉದ್ದಳತೆ(curdif);

	curdif = curdif->df_ಮುಂದಿನ;
    }

    ಆದರೆ (curdif != ಮುಂದಿನ_adjacent_ಖಂಡಗಳು)
	virtual_ಸಾಲುಗಳು_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_ಸಾಲುಗಳು_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    ಆದರೆ (virtual_ಸಾಲುಗಳು_passed < 0)
	virtual_ಸಾಲುಗಳು_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    ಆದರೆ (thistopdiff != ಖಾಲಿ) // this should not be null, but just for safety
	curlinenum_ಗಮ್ಯ = thistopdiff->df_lnum[toidx];

    ಪೂರ್ಣಾಂಕ virt_ಸಾಲುಗಳು_ಎಡ = virtual_ಸಾಲುಗಳು_passed;
    curdif = thistopdiff;
    ಆಗ (virt_ಸಾಲುಗಳು_ಎಡ > 0 && curdif != ಖಾಲಿ && curdif != ಮುಂದಿನ_adjacent_ಖಂಡಗಳು)
    {
	curlinenum_ಗಮ್ಯ += MIN(virt_ಸಾಲುಗಳು_ಎಡ, curdif->df_ಎಣಿಕೆ[toidx]);
	virt_ಸಾಲುಗಳು_ಎಡ -= MIN(virt_ಸಾಲುಗಳು_ಎಡ, get_max_diff_ಉದ್ದಳತೆ(curdif));
	curdif = curdif->df_ಮುಂದಿನ;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    ಪೂರ್ಣಾಂಕ max_virt_ಸಾಲುಗಳು = 0;
    ಗಾಗಿ (diff_T *dp = thistopdiff; dp != ಖಾಲಿ; dp = dp->df_ಮುಂದಿನ)
    {
	ಆದರೆ (dp->df_lnum[toidx] + dp->df_ಎಣಿಕೆ[toidx] <= curlinenum_ಗಮ್ಯ)
	    max_virt_ಸಾಲುಗಳು += get_max_diff_ಉದ್ದಳತೆ(dp);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (dp->df_lnum[toidx] <= curlinenum_ಗಮ್ಯ)
		max_virt_ಸಾಲುಗಳು += curlinenum_ಗಮ್ಯ - dp->df_lnum[toidx];
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_ಸಾಲುಗಳು - virtual_ಸಾಲುಗಳು_passed;
    (*topline) = curlinenum_ಗಮ್ಯ;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_ಉದ್ದಳತೆ,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_numbers[DB_ಎಣಿಕೆ];
    ಪೂರ್ಣಾಂಕ		outputmap[DB_ಎಣಿಕೆ];
    size_t	ndiffs = 0;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
	{
	    ಸಾಲು_numbers[i] = dp->df_lnum[i];
	    dp->df_ಎಣಿಕೆ[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    ಗಾಗಿ (size_t i = 0; i < decisions_ಉದ್ದಳತೆ; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	ಆದರೆ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_ಮುಂದಿನ);
	    dp_s->is_linematched = TRUE;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < DB_ಎಣಿಕೆ; j++)
	    {
		ಆದರೆ (curtab->tp_diffbuf[j] != ಖಾಲಿ)
		{
		    dp_s->df_lnum[j] = ಸಾಲು_numbers[j];
		    dp_s->df_ಎಣಿಕೆ[j] = 0;
		}
	    }
	}
	ಗಾಗಿ (size_t j = 0; j < ndiffs; j++)
	{
	    ಆದರೆ (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_ಎಣಿಕೆ[outputmap[j]]++;
		ಸಾಲು_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸತತ_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_ಎಣಿಕೆ];
    ಸ್ಥಿರಾಂಕ mmfile_t	*diffbufs[DB_ಎಣಿಕೆ];
    ಪೂರ್ಣಾಂಕ			diff_ಉದ್ದಳತೆ[DB_ಎಣಿಕೆ];
    size_t		ndiffs = 0;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    ಆದರೆ (dp->df_ಎಣಿಕೆ[i] > 0)
	    {
		diff_write_ಬಫರ್(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i] - 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		diffbufs_mm[ndiffs].din_mmfile.ಗಾತ್ರ_2 = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ಖಾಲಿ;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_ಉದ್ದಳತೆ[ndiffs] = dp->df_ಎಣಿಕೆ[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    ಪೂರ್ಣಾಂಕ *decisions = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_ಉದ್ದಳತೆ =
	linematch_nbuffers(diffbufs, diff_ಉದ್ದಳತೆ, ndiffs, &decisions, iwhite);

    ಗಾಗಿ (size_t i = 0; i < ndiffs; i++)
	ಬಿಡುಗಡೆ(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_ಉದ್ದಳತೆ, decisions);

    ಬಿಡುಗಡೆ(decisions);
}

/*
 * Check diff status for line "lnum" in buffer "buf":
 * Returns > 0 for inserting that many filler lines above it (never happens
 * when 'diffopt' doesn't contain "filler"). Otherwise returns 0.
 *
 * "linestatus" (can be NULL) will be set to:
 * 0 for nothing special.
 * -1 for a line that should be highlighted as changed.
 * -2 for a line that should be highlighted as added/deleted.
 *
 * This should only be used for windows where 'diff' is set.
 *
 * Note that it's possible for a changed/added/deleted line to also have filler
 * lines above it. This happens when using linematch or using diff anchors (at
 * the anchored lines).
 */
    ಪೂರ್ಣಾಂಕ
diff_check_with_linestatus(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಂಕ *linestatus)
{
    ಪೂರ್ಣಾಂಕ		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    ಪೂರ್ಣಾಂಕ		maxcount;
    ಪೂರ್ಣಾಂಕ		i;
    buf_T	*buf = wp->w_ಬಫರ್;
    ಪೂರ್ಣಾಂಕ		cmp;

    ಆದರೆ (linestatus != ಖಾಲಿ)
	*linestatus = 0;

    ಆದರೆ (curtab->tp_diff_invalid)
	ex_diffupdate(ಖಾಲಿ);		// update after a big change

    ಆದರೆ (curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ || !wp->w_p_diff)	// no diffs at all
	ಹಿಂತಿರುಗಿ 0;

    // safety check: "lnum" must be a buffer line
    ಆದರೆ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಹಿಂತಿರುಗಿ 0;

    idx = diff_buf_idx(buf);
    ಆದರೆ (idx == DB_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    ಆದರೆ (hasFoldingWin(wp, lnum, ಖಾಲಿ, ಖಾಲಿ, TRUE, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ಖಾಲಿ;		// last block with df_lnum[idx] <= lnum
    ಗಾಗಿ (dp = diff_ಪರಿಶೀಲನೆ_ಆರಂಭ(curtab, idx, lnum); dp != ಖಾಲಿ; dp = dp->df_ಮುಂದಿನ)
    {
	ಆದರೆ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	ಆದರೆ (lnum <= dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ನಿಲ್ಲಿಸು;
    }
    diff_ಪರಿಶೀಲನೆ_remember(curtab, idx, lnum, le);
    ಆದರೆ (dp == ಖಾಲಿ || lnum < dp->df_lnum[idx])
	ಹಿಂತಿರುಗಿ 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    ಆದರೆ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	ಸತತ_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_fill = 0;
    ಆಗ (lnum == dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	ಆದರೆ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_ಉದ್ದಳತೆ(dp);
	    ಅಂಕೆ_fill += maxcount - dp->df_ಎಣಿಕೆ[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	ಆದರೆ (dp->df_ಮುಂದಿನ != ಖಾಲಿ
		&& lnum >= dp->df_ಮುಂದಿನ->df_lnum[idx]
		&& lnum <= dp->df_ಮುಂದಿನ->df_lnum[idx] + dp->df_ಮುಂದಿನ->df_ಎಣಿಕೆ[idx])
	    dp = dp->df_ಮುಂದಿನ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (lnum < dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
    {
	ಪೂರ್ಣಾಂಕ	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	    ಆದರೆ (i != idx && curtab->tp_diffbuf[i] != ಖಾಲಿ)
	    {
		ಆದರೆ (dp->df_ಎಣಿಕೆ[i] == 0)
		    zero = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (dp->df_ಎಣಿಕೆ[i] != dp->df_ಎಣಿಕೆ[idx])
		    {
			ಆದರೆ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			ಹಿಂತಿರುಗಿ ಅಂಕೆ_fill;
		    }
		    cmp = TRUE;
		}
	    }
	ಆದರೆ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		ಆದರೆ (i != idx && curtab->tp_diffbuf[i] != ಖಾಲಿ
						      && dp->df_ಎಣಿಕೆ[i] != 0)
		    ಆದರೆ (!diff_equal_ನಮೂದು(dp, idx, i))
		    {
			ಆದರೆ (linestatus)
			    *linestatus = -1;
			ಹಿಂತಿರುಗಿ ಅಂಕೆ_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	ಆದರೆ (zero == FALSE)
	    ಹಿಂತಿರುಗಿ ಅಂಕೆ_fill;
	ಆದರೆ (linestatus)
	    *linestatus = -2;
	ಹಿಂತಿರುಗಿ ಅಂಕೆ_fill;
    }
    ಹಿಂತಿರುಗಿ ಅಂಕೆ_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_equal_ನಮೂದು(diff_T *dp, ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		cmp;

    ಆದರೆ (dp->df_ಎಣಿಕೆ[idx1] != dp->df_ಎಣಿಕೆ[idx2])
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (diff_check_sanity(curtab, dp) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (i = 0; i < dp->df_ಎಣಿಕೆ[idx1]; ++i)
    {
	ಸಾಲು = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
	cmp = diff_cmp(ಸಾಲು, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(ಸಾಲು);
	ಆದರೆ (cmp != 0)
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_equal_char(char_u *p1, char_u *p2, ಪೂರ್ಣಾಂಕ *len)
{
    ಪೂರ್ಣಾಂಕ l  = (*mb_ptr2len)(p1);

    ಆದರೆ (l != (*mb_ptr2len)(p2))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (l > 1)
    {
	ಆದರೆ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    ಹಿಂತಿರುಗಿ FALSE;
	*len = l;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    ಹಿಂತಿರುಗಿ FALSE;
	*len = 1;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	ಹಿಂತಿರುಗಿ STRCMP(s1, s2);
    ಆದರೆ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	ಹಿಂತಿರುಗಿ MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    ಆಗ (*p1 != NUL && *p2 != NUL)
    {
	ಆದರೆ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!diff_equal_char(p1, p2, &l))
		ನಿಲ್ಲಿಸು;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    ಆದರೆ (*p1 != NUL || *p2 != NUL)
	ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    ಪೂರ್ಣಾಂಕ
diff_check_fill(win_T *wp, linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ		n;

    // be quick when there are no filler lines
    ಆದರೆ (!(diff_flags & DIFF_FILLER))
	ಹಿಂತಿರುಗಿ 0;
    n = diff_check_with_linestatus(wp, lnum, ಖಾಲಿ);
    ಆದರೆ (n <= 0)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    ಶೂನ್ಯ
diff_ಗಣ_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_ಬಫರ್;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		fromidx;
    ಪೂರ್ಣಾಂಕ		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    ಆದರೆ (fromidx == DB_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;		// safety check

    ಆದರೆ (curtab->tp_diff_invalid)
	ex_diffupdate(ಖಾಲಿ);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(curtab, dp)
	ಆದರೆ (lnum <= dp->df_lnum[fromidx] + dp->df_ಎಣಿಕೆ[fromidx])
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (dp == ಖಾಲಿ)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
				       - (frombuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_ಬಫರ್);
	ಆದರೆ (toidx == DB_ಎಣಿಕೆ)
	    ಹಿಂತಿರುಗಿ;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	ಆದರೆ (lnum >= dp->df_lnum[fromidx])
	{
	    calculate_topfill_and_topline(fromidx, toidx,
					    fromwin->w_topline,
					    fromwin->w_topfill,
					    &towin->w_topfill,
					    &towin->w_topline);
	}
    }

    // safety check (if diff info gets outdated strange things may happen)
    towin->w_botfill = false;
    ಆದರೆ (towin->w_topline > towin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	towin->w_topline = towin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	towin->w_botfill = true;
    }
    ಆದರೆ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (ಶೂನ್ಯ)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ಖಾಲಿ, TRUE, ಖಾಲಿ);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_diffanchors(
    ಪೂರ್ಣಾಂಕ		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_anchors)
{
    ಪೂರ್ಣಾಂಕ i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ಖಾಲಿ;
    ಆದರೆ (check_only)
	bufwin = curwin;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    ಆದರೆ (bufwin->w_ಬಫರ್ == buf && bufwin->w_p_diff)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (bufwin == ಖಾಲಿ && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಗಾಗಿ (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	ಆದರೆ (*dia == ',') // don't allow empty values
	    ಹಿಂತಿರುಗಿ FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_ವಿಳಾಸ(ಖಾಲಿ, &dia, ADDR_ಸಾಲುಗಳು, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	ಆದರೆ (dia == ಖಾಲಿ) // error detected
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*dia != ',' && *dia != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1))
	{
	    emsg(_(e_invalid_range));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (anchors != ಖಾಲಿ)
	    anchors[i] = lnum;

	ಆದರೆ (*dia == ',')
	    dia++;
    }
    ಆದರೆ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_ಇನ್ನಷ್ಟು_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ಅಂಕೆ_anchors != ಖಾಲಿ)
	*ಅಂಕೆ_anchors = i;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    ಪೂರ್ಣಾಂಕ
diffanchors_ಬದಲಾಯಿತು(ಪೂರ್ಣಾಂಕ buflocal)
{
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = parse_diffanchors(TRUE, curbuf, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (ಫಲಿತಾಂಶ == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    ಆದರೆ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
		    ಆದರೆ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
	    }
	}
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * This is called when 'diffopt' is changed.
 */
    ಪೂರ್ಣಾಂಕ
diffopt_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		diff_context_new = 6;
    ಪೂರ್ಣಾಂಕ		linematch_ಸಾಲುಗಳು_new = 0;
    ಪೂರ್ಣಾಂಕ		diff_flags_new = 0;
    ಪೂರ್ಣಾಂಕ		diff_foldcolumn_new = 2;
    ದೀರ್ಘ	diff_algorithm_new = 0;
    ದೀರ್ಘ	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    ಆಗ (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	ಆದರೆ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_ಆಂತರಿಕ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    ಆದರೆ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_ಕನಿಷ್ಠತಮ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    ಆದರೆ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_ಶಬ್ದ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_ಸಾಲುಗಳು_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	ಆದರೆ (*p != ',' && *p != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    ಆದರೆ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	ಹಿಂತಿರುಗಿ FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    ಆದರೆ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_ಸಾಲುಗಳು = linematch_ಸಾಲುಗಳು_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    ಪೂರ್ಣಾಂಕ
diffopt_horizontal(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    ಪೂರ್ಣಾಂಕ
diffopt_hiddenoff(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    ಪೂರ್ಣಾಂಕ
diffopt_closeoff(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    ಶೂನ್ಯ
diff_update_ಸಾಲು(linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ		idx;
    diff_T	*dp;

    ಆದರೆ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	ಹಿಂತಿರುಗಿ;

    idx = diff_buf_idx(curbuf);
    ಆದರೆ (idx == DB_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;
    FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(curtab, dp)
	ಆದರೆ (lnum <= dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ನಿಲ್ಲಿಸು;

    // clear the inline change cache as it's invalid
    ಆದರೆ (dp != ಖಾಲಿ)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

ಸ್ಥಿರ diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    ಪೂರ್ಣಾಂಕ
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    ಪೂರ್ಣಾಂಕ *change_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ *change_ಕೊನೆ)
{
    ಆದರೆ (change->dc_ಆರಂಭ_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_ಆರಂಭ = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	*change_ಆರಂಭ = change->dc_ಆರಂಭ[diffline->bufidx];
    ಆದರೆ (change->dc_ಕೊನೆ_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_ಕೊನೆ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಇಲ್ಲದಿದ್ದರೆ
	*change_ಕೊನೆ = change->dc_ಕೊನೆ[diffline->bufidx];

    ಆದರೆ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಆದರೆ (i == diffline->bufidx)
	    ಮುಂದುವರಿ;
	ಆದರೆ (change->dc_ಆರಂಭ[i] != change->dc_ಕೊನೆ[i]
		|| change->dc_ಕೊನೆ_lnum_off[i] != change->dc_ಆರಂಭ_lnum_off[i])
	{
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Find the difference within a changed line and returns [startp,endp] byte
 * positions.  Performs a simple algorithm by finding a single range in the
 * middle.
 *
 * If diffopt has DIFF_INLINE_NONE set, then this will only calculate the return
 * value (added or changed), but startp/endp will not be calculated.
 *
 * Returns TRUE if the line was added, no other buffer has it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
diff_ಕಂಡುಹಿಡಿ_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		*startp,	// first char of the change
    ಪೂರ್ಣಾಂಕ		*endp)		// last char of the change
{
    char_u	*ಸಾಲು_org;
    char_u	*ಸಾಲು_new;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		si_org, si_new;
    ಪೂರ್ಣಾಂಕ		ei_org, ei_new;
    ಪೂರ್ಣಾಂಕ		off;
    ಪೂರ್ಣಾಂಕ		added = TRUE;
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	ಸಾಲು_org = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	ಸಾಲು_org = vim_strsave(ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE));
	ಆದರೆ (ಸಾಲು_org == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    ಆದರೆ (off >= dp->df_ಎಣಿಕೆ[i])
		ಮುಂದುವರಿ;
	    added = FALSE;
	    ಆದರೆ (diff_flags & DIFF_INLINE_NONE)
		ನಿಲ್ಲಿಸು; // early terminate as we only care about the return value

	    ಸಾಲು_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    ಆಗ (ಸಾಲು_org[si_org] != NUL)
	    {
		ಆದರೆ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(ಸಾಲು_org[si_org])
					      && VIM_ISWHITE(ಸಾಲು_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(ಸಾಲು_org[si_org])
					    || VIM_ISWHITE(ಸಾಲು_new[si_new]))))
		{
		    si_org = (ಪೂರ್ಣಾಂಕ)(skipwhite(ಸಾಲು_org + si_org) - ಸಾಲು_org);
		    si_new = (ಪೂರ್ಣಾಂಕ)(skipwhite(ಸಾಲು_new + si_new) - ಸಾಲು_new);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (!diff_equal_char(ಸಾಲು_org + si_org, ಸಾಲು_new + si_new,
									   &l))
			ನಿಲ್ಲಿಸು;
		    si_org += l;
		    si_new += l;
		}
	    }
	    ಆದರೆ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_ತಲೆ_off)(ಸಾಲು_org, ಸಾಲು_org + si_org);
		si_new -= (*mb_ತಲೆ_off)(ಸಾಲು_new, ಸಾಲು_new + si_new);
	    }
	    ಆದರೆ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    ಆದರೆ (ಸಾಲು_org[si_org] != NUL || ಸಾಲು_new[si_new] != NUL)
	    {
		ei_org = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು_org);
		ei_new = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು_new);
		ಆಗ (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    ಆದರೆ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(ಸಾಲು_org[ei_org])
					      && VIM_ISWHITE(ಸಾಲು_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(ಸಾಲು_org[ei_org])
					    || VIM_ISWHITE(ಸಾಲು_new[ei_new]))))
		    {
			ಆಗ (ei_org >= *startp
					     && VIM_ISWHITE(ಸಾಲು_org[ei_org]))
			    --ei_org;
			ಆಗ (ei_new >= si_new
					     && VIM_ISWHITE(ಸಾಲು_new[ei_new]))
			    --ei_new;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			p1 = ಸಾಲು_org + ei_org;
			p2 = ಸಾಲು_new + ei_new;
			p1 -= (*mb_ತಲೆ_off)(ಸಾಲು_org, p1);
			p2 -= (*mb_ತಲೆ_off)(ಸಾಲು_new, p2);
			ಆದರೆ (!diff_equal_char(p1, p2, &l))
			    ನಿಲ್ಲಿಸು;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		ಆದರೆ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(ಸಾಲು_org);
    ಹಿಂತಿರುಗಿ added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ದೀರ್ಘ byte_ಆರಂಭ;
    ದೀರ್ಘ ಅಂಕೆ_ಬೈಟುಗಳು;
    ಪೂರ್ಣಾಂಕ lineoff;
} linemap_ನಮೂದು_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, ಪೂರ್ಣಾಂಕ idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    ಪೂರ್ಣಾಂಕ pass = 1;
    ಮಾಡು
    {
	ಪೂರ್ಣಾಂಕ has_unmerged_gaps = FALSE;
	ಪೂರ್ಣಾಂಕ has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	ಆಗ (dp!= ಖಾಲಿ && dp->df_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    ಆದರೆ (dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_ಮುಂದಿನ->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_ಮುಂದಿನ;
		ಮುಂದುವರಿ;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_ನಮೂದು_T *entry1 = &((linemap_ನಮೂದು_T*)linemap[idx1].ga_ದತ್ತಾಂಶ)[dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1] - 1];
	    linemap_ನಮೂದು_T *entry2 = &((linemap_ನಮೂದು_T*)linemap[idx1].ga_ದತ್ತಾಂಶ)[dp->df_ಮುಂದಿನ->df_lnum[idx1] - 1];
	    ಆದರೆ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_ಮುಂದಿನ;
		ಮುಂದುವರಿ;
	    }

	    linenr_T gap = dp->df_ಮುಂದಿನ->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1]);
	    ಆದರೆ (gap <= 3)
	    {
		linenr_T max_df_ಎಣಿಕೆ = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		    max_df_ಎಣಿಕೆ = MAX(max_df_ಎಣಿಕೆ, dp->df_ಎಣಿಕೆ[i] + dp->df_ಮುಂದಿನ->df_ಎಣಿಕೆ[i]);

		ಆದರೆ (max_df_ಎಣಿಕೆ >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		    {
			dp->df_ಎಣಿಕೆ[i] = dp->df_ಮುಂದಿನ->df_lnum[i]
			    + dp->df_ಮುಂದಿನ->df_ಎಣಿಕೆ[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_ಮುಂದಿನ = dp->df_ಮುಂದಿನ;
		    dp->df_ಮುಂದಿನ = dp_ಮುಂದಿನ->df_ಮುಂದಿನ;
		    clear_diffblock(dp_ಮುಂದಿನ);
		    has_merged_gaps = TRUE;
		    ಮುಂದುವರಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_ಮುಂದಿನ;
	}
	ಆದರೆ (!has_unmerged_gaps || !has_merged_gaps)
	    ನಿಲ್ಲಿಸು;
    } ಆಗ (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_refine_inline_ಶಬ್ದ_highlight(diff_T *dp_orig, garray_T *linemap, ಪೂರ್ಣಾಂಕ idx1,
				  linenr_T ಆರಂಭ_lnum)
{
    ಪೂರ್ಣಾಂಕ pass = 1;
    ಮಾಡು
    {
	diff_T *dp = dp_orig;

	ಆಗ (dp != ಖಾಲಿ && dp->df_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_ಮುಂದಿನ->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    ಆದರೆ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_ಮುಂದಿನ;
		ಮುಂದುವರಿ;
	    }

	    linemap_ನಮೂದು_T *entry1 =
		&((linemap_ನಮೂದು_T *)linemap[idx1].ga_ದತ್ತಾಂಶ)[idx_entry1];
	    linemap_ನಮೂದು_T *entry2 =
		&((linemap_ನಮೂದು_T *)linemap[idx1].ga_ದತ್ತಾಂಶ)[idx_entry2];

	    // Skip if blocks are on different lines
	    ಆದರೆ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_ಮುಂದಿನ;
		ಮುಂದುವರಿ;
	    }

	    // Calculate the gap between blocks
	    ಪೂರ್ಣಾಂಕ gap_ಆರಂಭ = entry1->byte_ಆರಂಭ + entry1->ಅಂಕೆ_ಬೈಟುಗಳು;
	    ಪೂರ್ಣಾಂಕ gap_ಕೊನೆ = entry2->byte_ಆರಂಭ;
	    ಪೂರ್ಣಾಂಕ gap_ಗಾತ್ರ = gap_ಕೊನೆ - gap_ಆರಂಭ;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    ಆದರೆ (gap_ಗಾತ್ರ <= 0 || gap_ಗಾತ್ರ > diff_ಶಬ್ದ_gap)
	    {
		dp = dp->df_ಮುಂದಿನ;
		ಮುಂದುವರಿ;
	    }

	    // Get the text between the two blocks
	    char_u *ಸಾಲು = ml_get_buf(curtab->tp_diffbuf[idx1],
		    ಆರಂಭ_lnum + entry1->lineoff, FALSE);
	    char_u *gap_ಪಠ್ಯ = ಸಾಲು + gap_ಆರಂಭ;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_ಶಬ್ದ = true;
	    bool has_content = false;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < gap_ಗಾತ್ರ && gap_ಪಠ್ಯ[i] != NUL; i++)
	    {
		has_content = true;
		ಪೂರ್ಣಾಂಕ char_class = mb_get_class_buf(gap_ಪಠ್ಯ + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		ಆದರೆ (char_class == 2)
		{
		    only_non_ಶಬ್ದ = false;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    ಆದರೆ (has_content && only_non_ಶಬ್ದ)
	    {
		ದೀರ್ಘ ಒಟ್ಟು_change_ಬೈಟುಗಳು = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		{
		    ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
		    {
			// count bytes in the first block
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < dp->df_ಎಣಿಕೆ[i]; k++)
			{
			    ಪೂರ್ಣಾಂಕ idx = dp->df_lnum[i] + k - 1;
			    ಆದರೆ (idx < linemap[i].ga_len)
				ಒಟ್ಟು_change_ಬೈಟುಗಳು +=
				    ((linemap_ನಮೂದು_T *)linemap[i].ga_ದತ್ತಾಂಶ)[idx].ಅಂಕೆ_ಬೈಟುಗಳು;
			}
			// count bytes in the next block
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < dp->df_ಮುಂದಿನ->df_ಎಣಿಕೆ[i]; k++)
			{
			    ಪೂರ್ಣಾಂಕ idx = dp->df_ಮುಂದಿನ->df_lnum[i] + k - 1;
			    ಆದರೆ (idx < linemap[i].ga_len)
				ಒಟ್ಟು_change_ಬೈಟುಗಳು +=
				    ((linemap_ನಮೂದು_T *)linemap[i].ga_ದತ್ತಾಂಶ)[idx].ಅಂಕೆ_ಬೈಟುಗಳು;
			}
		    }
		}

		ಆದರೆ (ಒಟ್ಟು_change_ಬೈಟುಗಳು >= gap_ಗಾತ್ರ * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		    {
			ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
			{
			    dp->df_ಎಣಿಕೆ[i] = dp->df_ಮುಂದಿನ->df_lnum[i]
				+ dp->df_ಮುಂದಿನ->df_ಎಣಿಕೆ[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_ಮುಂದಿನ = dp->df_ಮುಂದಿನ;
		    dp->df_ಮುಂದಿನ = dp_ಮುಂದಿನ->df_ಮುಂದಿನ;
		    clear_diffblock(dp_ಮುಂದಿನ);

		    // Don't advance dp, check if can merge with the next block too
		    ಮುಂದುವರಿ;
		}
	    }

	    dp = dp->df_ಮುಂದಿನ;
	}
    } ಆಗ (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_ಕಂಡುಹಿಡಿ_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_ಎಣಿಕೆ];
    garray_T	file1_str;
    garray_T	file2_str;
    ಪೂರ್ಣಾಂಕ		file1_idx = -1;

    ದೀರ್ಘ	save_diff_algorithm = diff_algorithm;

    CLEAR_ಕ್ಷೇತ್ರ(dio);
    ga_init2(&dio.dio_diff.dout_ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 1000);

    // inline diff only supports internal algo
    dio.dio_ಆಂತರಿಕ = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_ಮೊದಲ_diff;
    curtab->tp_ಮೊದಲ_diff = ಖಾಲಿ;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    //ಬಫರ್
    buf_T	*(orig_diffbuf[DB_ಎಣಿಕೆ]);
    ಸ್ಮರಣೆ_ನಕಲು(orig_diffbuf, curtab->tp_diffbuf, ಗಾತ್ರ(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	ga_init2(&linemap[i], ಗಾತ್ರ(linemap_ನಮೂದು_T), 128);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ)
	    ಮುಂದುವರಿ; // skip buffer that isn't loaded

	ಆದರೆ (dp->df_ಎಣಿಕೆ[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ಖಾಲಿ;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ off = 0; off < dp->df_ಎಣಿಕೆ[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    ಪೂರ್ಣಾಂಕ ಒಳ_keyword = FALSE;

	    // iwhiteeol support vars
	    ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_white = FALSE;
	    ಪೂರ್ಣಾಂಕ eol_ga_len = -1;
	    ಪೂರ್ಣಾಂಕ eol_linemap_len = -1;
	    ಪೂರ್ಣಾಂಕ eol_numlines = -1;

	    char_u *s;
	    ಗಾಗಿ (s = curline; *s != NUL;)
	    {
		ಪೂರ್ಣಾಂಕ new_ಒಳ_keyword = FALSE;
		ಆದರೆ (diff_flags & DIFF_INLINE_ಶಬ್ದ)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_ಒಳ_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		ಆದರೆ (ಒಳ_keyword && !new_ಒಳ_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ಆದರೆ (VIM_ISWHITE(*s))
		{
		    ಆದರೆ (diff_flags & DIFF_IWHITEALL)
		    {
			ಒಳ_keyword = FALSE;
			s = skipwhite(s);
			ಮುಂದುವರಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ಆದರೆ (!ಕೊನೆಯ_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    ಕೊನೆಯ_white = TRUE;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ಕೊನೆಯ_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		ಪೂರ್ಣಾಂಕ char_len = 1;
		ಆದರೆ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_len = mb_ptr2len(s);

		    ಆದರೆ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    ಆದರೆ (diff_flags & DIFF_ICASE)
		    {
			ಪೂರ್ಣಾಂಕ c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			ಪೂರ್ಣಾಂಕ c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			ಪೂರ್ಣಾಂಕ c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			ಆದರೆ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ga_concat_len(curstr, s, char_len);
		}

		ಆದರೆ (!new_ಒಳ_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ಆದರೆ (!new_ಒಳ_keyword || (new_ಒಳ_keyword && !ಒಳ_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_ನಮೂದು_T linemap_ನಮೂದು;
		    linemap_ನಮೂದು.lineoff = off;
		    linemap_ನಮೂದು.byte_ಆರಂಭ = s - curline;
		    linemap_ನಮೂದು.ಅಂಕೆ_ಬೈಟುಗಳು = char_len;
		    ಆದರೆ (ga_grow(&linemap[i], 1) != OK)
			ಹೋಗು ಮುಗಿದ;
		    ((linemap_ನಮೂದು_T*)(linemap[i].ga_ದತ್ತಾಂಶ))[linemap[i].ga_len]
			= linemap_ನಮೂದು;
		    linemap[i].ga_len += 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_ನಮೂದು_T*)linemap[i].ga_ದತ್ತಾಂಶ)[linemap[i].ga_len-1].ಅಂಕೆ_ಬೈಟುಗಳು
			+= char_len;
		}

		ಒಳ_keyword = new_ಒಳ_keyword;
		s += char_len;
	    }
	    ಆದರೆ (ಒಳ_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    ಆದರೆ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		ಆದರೆ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    ಆದರೆ (!(diff_flags & DIFF_IWHITEALL))
	    {
		// Add an empty line token mapped to the end-of-line in the
		// original file. This helps diff newline differences among
		// files, which will be visualized when using 'list' as the eol
		// listchar will be highlighted.
		ga_append(curstr, NL);
		numlines++;

		linemap_ನಮೂದು_T linemap_ನಮೂದು;
		linemap_ನಮೂದು.lineoff = off;
		linemap_ನಮೂದು.byte_ಆರಂಭ = s - curline;
		linemap_ನಮೂದು.ಅಂಕೆ_ಬೈಟುಗಳು = ಗಾತ್ರ(NL);
		ಆದರೆ (ga_grow(&linemap[i], 1) != OK)
		    ಹೋಗು ಮುಗಿದ;
		((linemap_ನಮೂದು_T*)(linemap[i].ga_ದತ್ತಾಂಶ))[linemap[i].ga_len]
		    = linemap_ನಮೂದು;
		linemap[i].ga_len += 1;
	    }
	}

	ಆದರೆ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (ಅಕ್ಷರ *)curstr->ga_ದತ್ತಾಂಶ;
	    dio.dio_new.din_mmfile.ಗಾತ್ರ_2 = curstr->ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dio.dio_orig.din_mmfile.ptr = (ಅಕ್ಷರ *)curstr->ga_ದತ್ತಾಂಶ;
	    dio.dio_orig.din_mmfile.ಗಾತ್ರ_2 = curstr->ga_len;
	}
	ಆದರೆ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    ಪೂರ್ಣಾಂಕ diff_ಸ್ಥಿತಿ = diff_file_ಆಂತರಿಕ(&dio);
	    ಆದರೆ (diff_ಸ್ಥಿತಿ == FAIL)
		ಹೋಗು ಮುಗಿದ;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_ಮೊದಲ_diff;

    ಆದರೆ (diff_flags & DIFF_INLINE_ಶಬ್ದ && file1_idx != -1)
	diff_refine_inline_ಶಬ್ದ_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    ಗಾಗಿ (; new_diff != ಖಾಲಿ; new_diff = new_diff->df_ಮುಂದಿನ)
    {
	diffline_change_T change;
	CLEAR_ಕ್ಷೇತ್ರ(change);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	{
	    ಆದರೆ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		ಮುಂದುವರಿ;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_ಕೊನೆ = diff_lnum + new_diff->df_ಎಣಿಕೆ[i];

	    ಆದರೆ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_ಆರಂಭ[i] = MAXCOL;
		change.dc_ಆರಂಭ_lnum_off[i] = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		change.dc_ಆರಂಭ[i] = ((linemap_ನಮೂದು_T*)linemap[i].ga_ದತ್ತಾಂಶ)[diff_lnum].byte_ಆರಂಭ;
		change.dc_ಆರಂಭ_lnum_off[i] = ((linemap_ನಮೂದು_T*)linemap[i].ga_ದತ್ತಾಂಶ)[diff_lnum].lineoff;
	    }

	    ಆದರೆ (diff_lnum == diff_lnum_ಕೊನೆ)
	    {
		change.dc_ಕೊನೆ[i] = change.dc_ಆರಂಭ[i];
		change.dc_ಕೊನೆ_lnum_off[i] = change.dc_ಆರಂಭ_lnum_off[i];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (diff_lnum_ಕೊನೆ - 1 >= linemap[i].ga_len)
	    {
		change.dc_ಕೊನೆ[i] = MAXCOL;
		change.dc_ಕೊನೆ_lnum_off[i] = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		change.dc_ಕೊನೆ[i] = ((linemap_ನಮೂದು_T*)linemap[i].ga_ದತ್ತಾಂಶ)[diff_lnum_ಕೊನೆ-1].byte_ಆರಂಭ +
		    ((linemap_ನಮೂದು_T*)linemap[i].ga_ದತ್ತಾಂಶ)[diff_lnum_ಕೊನೆ-1].ಅಂಕೆ_ಬೈಟುಗಳು;
		change.dc_ಕೊನೆ_lnum_off[i] = ((linemap_ನಮೂದು_T*)linemap[i].ga_ದತ್ತಾಂಶ)[diff_lnum_ಕೊನೆ-1].lineoff;
	    }
	}
	ಆದರೆ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    ಹೋಗು ಮುಗಿದ;
	}
	((diffline_change_T*)(dp->df_changes.ga_ದತ್ತಾಂಶ))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

ಮುಗಿದ:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_ಮೊದಲ_diff = orig_diff;
    ಸ್ಮರಣೆ_ನಕಲು(curtab->tp_diffbuf, orig_diffbuf, ಗಾತ್ರ(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    ಪೂರ್ಣಾಂಕ
diff_ಕಂಡುಹಿಡಿ_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		off;

    idx = diff_buf_idx(wp->w_ಬಫರ್);
    ಆದರೆ (idx == DB_ಎಣಿಕೆ)	// cannot happen
	ಹಿಂತಿರುಗಿ FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(curtab, dp)
	ಆದರೆ (lnum < dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (dp == ಖಾಲಿ || diff_check_sanity(curtab, dp) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (lnum - dp->df_lnum[idx] > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	// Integer overflow protection
	ಹಿಂತಿರುಗಿ FALSE;
    off = lnum - dp->df_lnum[idx];

    ಆದರೆ (!(diff_flags & ALL_INLINE_DIFF) || diff_ಆಂತರಿಕ_failed())
    {
	// Use simple algorithm
	ಪೂರ್ಣಾಂಕ	change_ಆರಂಭ = MAXCOL;	// first col of changed area
	ಪೂರ್ಣಾಂಕ	change_ಕೊನೆ = -1;	// last col of changed area
	ಪೂರ್ಣಾಂಕ	ret;

	ret = diff_ಕಂಡುಹಿಡಿ_change_simple(wp, lnum, dp, idx, &change_ಆರಂಭ, &change_ಕೊನೆ);

	// convert from inclusive end to exclusive end per diffline's contract
	change_ಕೊನೆ += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_ಕ್ಷೇತ್ರ(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->ಅಂಕೆ_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_ಆರಂಭ[idx] = change_ಆರಂಭ;
	simple_diffline_change.dc_ಕೊನೆ[idx] = change_ಕೊನೆ;
	simple_diffline_change.dc_ಆರಂಭ_lnum_off[idx] = off;
	simple_diffline_change.dc_ಕೊನೆ_lnum_off[idx] = off;
	ಹಿಂತಿರುಗಿ ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    ಆದರೆ (!dp->has_changes)
	diff_ಕಂಡುಹಿಡಿ_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_changes = 0;
    ಪೂರ್ಣಾಂಕ change_idx = 0;
    diffline->changes = ಖಾಲಿ;
    ಗಾಗಿ (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_ದತ್ತಾಂಶ)[change_idx];
	ಆದರೆ (change->dc_ಕೊನೆ_lnum_off[idx] < off)
	    ಮುಂದುವರಿ;
	ಆದರೆ (change->dc_ಆರಂಭ_lnum_off[idx] > off)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (diffline->changes == ಖಾಲಿ)
	    diffline->changes = change;
	ಅಂಕೆ_changes++;
    }
    diffline->ಅಂಕೆ_changes = ಅಂಕೆ_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    ಪೂರ್ಣಾಂಕ added = FALSE;
    ಆದರೆ (ಅಂಕೆ_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	{
	    ಆದರೆ (idx == i)
		ಮುಂದುವರಿ;
	    ಆದರೆ (curtab->tp_diffbuf[i] == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_ದತ್ತಾಂಶ)[dp->df_changes.ga_len-1];
	    ಆದರೆ (change->dc_ಆರಂಭ_lnum_off[i] != ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	    {
		added = FALSE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ added;
}

# ಆದರೆ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    ಪೂರ್ಣಾಂಕ
diff_infold(win_T *wp, linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		idx = -1;
    ಪೂರ್ಣಾಂಕ		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    ಆದರೆ (!wp->w_p_diff)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
    {
	ಆದರೆ (curtab->tp_diffbuf[i] == wp->w_ಬಫರ್)
	    idx = i;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    ಆದರೆ (idx == -1 || !other)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (curtab->tp_diff_invalid)
	ex_diffupdate(ಖಾಲಿ);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    ಆದರೆ (curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ಖಾಲಿ;		// last block with df_lnum[idx] <= lnum
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = TRUE;
    ಗಾಗಿ (dp = diff_ಪರಿಶೀಲನೆ_ಆರಂಭ(curtab, idx, lnum); dp != ಖಾಲಿ; dp = dp->df_ಮುಂದಿನ)
    {
	ಆದರೆ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	ಆದರೆ (dp->df_lnum[idx] - diff_context > lnum)
	    ನಿಲ್ಲಿಸು;
	// If this change ends before the line we have a match.
	ಆದರೆ (dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx] + diff_context > lnum)
	{
	    ಫಲಿತಾಂಶ = FALSE;
	    ನಿಲ್ಲಿಸು;
	}
    }
    diff_ಪರಿಶೀಲನೆ_remember(curtab, idx, lnum, le);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
# endif

/*
 * "dp" and "do" commands.
 */
    ಶೂನ್ಯ
nv_diffgetput(ಪೂರ್ಣಾಂಕ put, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	ಹಿಂತಿರುಗಿ;
    }
# endif
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ea.arg = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_snprintf((ಅಕ್ಷರ *)buf, 30, "%ld", ಎಣಿಕೆ_2);
	ea.arg = buf;
    }
    ಆದರೆ (put)
	ea.cmdidx = CMD_diffput;
    ಇಲ್ಲದಿದ್ದರೆ
	ea.cmdidx = CMD_diffget;
    ea.addr_ಎಣಿಕೆ = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(curtab, dp)
	ಆದರೆ (dp == diff)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    ಶೂನ್ಯ
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    ಪೂರ್ಣಾಂಕ		idx_cur;
    ಪೂರ್ಣಾಂಕ		idx_other;
    ಪೂರ್ಣಾಂಕ		idx_from;
    ಪೂರ್ಣಾಂಕ		idx_ಗಮ್ಯ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_skip, ಕೊನೆ_skip;
    ಪೂರ್ಣಾಂಕ		new_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		buf_ಖಾಲಿಗಳು;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    ಆದರೆ (idx_cur == DB_ಎಣಿಕೆ)
    {
	emsg(_(e_current_ಬಫರ್_is_not_ಒಳ_diff_ಬಹುಲಕ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	ಗಾಗಿ (idx_other = 0; idx_other < DB_ಎಣಿಕೆ; ++idx_other)
	    ಆದರೆ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ಖಾಲಿ)
	    {
		ಆದರೆ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    ನಿಲ್ಲಿಸು;
		ಸಿಕ್ಕಿತು_not_ma = TRUE;
	    }
	ಆದರೆ (idx_other == DB_ಎಣಿಕೆ)
	{
	    ಆದರೆ (ಸಿಕ್ಕಿತು_not_ma)
		emsg(_(e_no_other_ಬಫರ್_ಒಳ_diff_ಬಹುಲಕ_is_modifiable));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_no_other_ಬಫರ್_ಒಳ_diff_ಬಹುಲಕ));
	    ಹಿಂತಿರುಗಿ;
	}

	// Check that there isn't a third buffer in the list
	ಗಾಗಿ (i = idx_other + 1; i < DB_ಎಣಿಕೆ; ++i)
	    ಆದರೆ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ಖಾಲಿ
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_ಇನ್ನಷ್ಟು_than_two_buffers_ಒಳ_diff_ಬಹುಲಕ_dont_know_which_one_ಗಮ್ಯ_use));
		ಹಿಂತಿರುಗಿ;
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	ಆಗ (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	ಗಾಗಿ (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	ಆದರೆ (eap->arg + i == p)	    // digits only
	    i = atol((ಅಕ್ಷರ *)eap->arg);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    ಆದರೆ (i < 0)
		ಹಿಂತಿರುಗಿ;		// error message already given
	}
	buf = buflist_findnr(i);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    semsg(_(e_cant_ಕಂಡುಹಿಡಿ_ಬಫರ್_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (buf == curbuf)
	    ಹಿಂತಿರುಗಿ;		// nothing to do
	idx_other = diff_buf_idx(buf);
	ಆದರೆ (idx_other == DB_ಎಣಿಕೆ)
	{
	    semsg(_(e_ಬಫರ್_str_is_not_ಒಳ_diff_ಬಹುಲಕ), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	ಪೂರ್ಣಾಂಕ linestatus = 0;
	ಆದರೆ (eap->line1 == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->line1 > 0)
	    --eap->line1;
    }

    ಆದರೆ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_ಗಮ್ಯ = idx_cur;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	idx_from = idx_cur;
	idx_ಗಮ್ಯ = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	ಆದರೆ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    ಹೋಗು theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    ಆದರೆ (!curbuf->b_ಬದಲಾಯಿತು)
    {
	change_warning(0);
	ಆದರೆ (diff_buf_idx(curbuf) != idx_ಗಮ್ಯ)
	{
	    emsg(_(e_ಬಫರ್_ಬದಲಾಯಿತು_unexpectedly));
	    ಹೋಗು theend;
	}
    }

    dprev = ಖಾಲಿ;
    ಗಾಗಿ (dp = curtab->tp_ಮೊದಲ_diff; dp != ಖಾಲಿ; )
    {
	ಆದರೆ (!eap->addr_ಎಣಿಕೆ)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    ಆಗ (dp->df_ಮುಂದಿನ
		    && dp->df_ಮುಂದಿನ->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_ಎಣಿಕೆ[idx_cur]
		    && dp->df_ಮುಂದಿನ->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_ಮುಂದಿನ;
	    }
	}
	ಆದರೆ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    ನಿಲ್ಲಿಸು;	// past the range that was specified

	dfree = ಖಾಲಿ;
	lnum = dp->df_lnum[idx_ಗಮ್ಯ];
	ಎಣಿಕೆ_2 = dp->df_ಎಣಿಕೆ[idx_ಗಮ್ಯ];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + ಎಣಿಕೆ_2 + (ಎಣಿಕೆ_2 == 0 && BUFEMPTY() ? 1 : 0);

	ಆದರೆ (dp->df_lnum[idx_cur] + dp->df_ಎಣಿಕೆ[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    ಆರಂಭ_skip = 0;
	    ಕೊನೆ_skip = 0;
	    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		ಆರಂಭ_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		ಆದರೆ (ಆರಂಭ_skip > 0)
		{
		    // range starts below start of current diff block
		    ಆದರೆ (ಆರಂಭ_skip > ಎಣಿಕೆ_2)
		    {
			lnum += ಎಣಿಕೆ_2;
			ಎಣಿಕೆ_2 = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಎಣಿಕೆ_2 -= ಆರಂಭ_skip;
			lnum += ಆರಂಭ_skip;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಆರಂಭ_skip = 0;

		ಕೊನೆ_skip = dp->df_lnum[idx_cur] + dp->df_ಎಣಿಕೆ[idx_cur] - 1
							 - (eap->line2 + off);
		ಆದರೆ (ಕೊನೆ_skip > 0)
		{
		    // range ends above end of current/from diff block
		    ಆದರೆ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_ಎಣಿಕೆ[idx_cur] - ಆರಂಭ_skip - ಕೊನೆ_skip;
			ಆದರೆ (ಎಣಿಕೆ_2 > i)
			    ಎಣಿಕೆ_2 = i;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ			// :diffget
		    {
			ಎಣಿಕೆ_2 -= ಕೊನೆ_skip;
			ಕೊನೆ_skip = dp->df_ಎಣಿಕೆ[idx_from] - ಆರಂಭ_skip - ಎಣಿಕೆ_2;
			ಆದರೆ (ಕೊನೆ_skip < 0)
			    ಕೊನೆ_skip = 0;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಕೊನೆ_skip = 0;
	    }

	    buf_ಖಾಲಿಗಳು = BUFEMPTY();
	    added = 0;
	    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_ಖಾಲಿಗಳು = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1;
		ಆದರೆ (ml_delete(lnum) == OK)
		    --added;
	    }
	    ಗಾಗಿ (i = 0; i < dp->df_ಎಣಿಕೆ[idx_from] - ಆರಂಭ_skip - ಕೊನೆ_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + ಆರಂಭ_skip + i;
		ಆದರೆ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ನಿಲ್ಲಿಸು;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    ಆದರೆ (buf_ಖಾಲಿಗಳು && curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_ಖಾಲಿಗಳು = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_ಎಣಿಕೆ = dp->df_ಎಣಿಕೆ[idx_ಗಮ್ಯ] + added;
	    dp->df_ಎಣಿಕೆ[idx_ಗಮ್ಯ] = new_ಎಣಿಕೆ;

	    ಆದರೆ (ಆರಂಭ_skip == 0 && ಕೊನೆ_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		    ಆದರೆ (curtab->tp_diffbuf[i] != ಖಾಲಿ && i != idx_from
								&& i != idx_ಗಮ್ಯ
			    && !diff_equal_ನಮೂದು(dp, idx_from, i))
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (i == DB_ಎಣಿಕೆ)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_ಮುಂದಿನ;
		    ಆದರೆ (dprev == ಖಾಲಿ)
			curtab->tp_ಮೊದಲ_diff = dp;
		    ಇಲ್ಲದಿದ್ದರೆ
			dprev->df_ಮುಂದಿನ = dp;
		}
	    }

	    ಆದರೆ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + ಎಣಿಕೆ_2 - 1, (ದೀರ್ಘ)MAXLNUM, (ದೀರ್ಘ)added);
		ಆದರೆ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    ಆದರೆ (curwin->w_cursor.lnum >= lnum + ಎಣಿಕೆ_2)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(lnum, 0, lnum + ಎಣಿಕೆ_2, (ದೀರ್ಘ)added);

	    ಆದರೆ (dfree != ಖಾಲಿ)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_ಗಮ್ಯ);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    ಆದರೆ (added != 0 && !valid_diff(dp))
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (dfree == ಖಾಲಿ)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_ಎಣಿಕೆ[idx_ಗಮ್ಯ] = new_ಎಣಿಕೆ;

	    // When changing the current buffer, keep track of line numbers
	    ಆದರೆ (idx_cur == idx_ಗಮ್ಯ)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	ಆದರೆ (dfree == ಖಾಲಿ)
	{
	    dprev = dp;
	    dp = dp->df_ಮುಂದಿನ;
	}
    }

    // restore curwin/curbuf and a few other things
    ಆದರೆ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	ಆದರೆ (ಕೀಲಿ_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    ಆದರೆ (diff_need_update)
	ex_diffupdate(ಖಾಲಿ);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    ಆದರೆ (curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_ಒಳ_TAB(curtab, wp)
	    ಆದರೆ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    ಆದರೆ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	ಕಿಟಕಿ_layout_ಬೀಗ();
	apply_autocmds(EVENT_DIFFUPDATED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಕಿಟಕಿ_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_fold_update(diff_T *dp, ಪೂರ್ಣಾಂಕ skip_idx)
{
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಗಾಗಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	    ಆದರೆ (curtab->tp_diffbuf[i] == wp->w_ಬಫರ್ && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    ಪೂರ್ಣಾಂಕ
diff_ಬಹುಲಕ_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ಆದರೆ (diff_buf_idx_tp(buf, tp) != DB_ಎಣಿಕೆ)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    ಪೂರ್ಣಾಂಕ
diff_move_ಗಮ್ಯ(ಪೂರ್ಣಾಂಕ dir, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    ಆದರೆ (idx == DB_ಎಣಿಕೆ || curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (curtab->tp_diff_invalid)
	ex_diffupdate(ಖಾಲಿ);		// update after a big change

    ಆದರೆ (curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ)		// no diffs today
	ಹಿಂತಿರುಗಿ FAIL;

    ಆಗ (--ಎಣಿಕೆ_2 >= 0)
    {
	// Check if already before first diff.
	ಆದರೆ (dir == BACKWARD && lnum <= curtab->tp_ಮೊದಲ_diff->df_lnum[idx])
	    ನಿಲ್ಲಿಸು;

	ಗಾಗಿ (dp = curtab->tp_ಮೊದಲ_diff; ; dp = dp->df_ಮುಂದಿನ)
	{
	    ಆದರೆ (dp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_ಮುಂದಿನ == ಖಾಲಿ
			    || lnum <= dp->df_ಮುಂದಿನ->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    // don't end up past the end of the file
    ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // When the cursor didn't move at all we fail.
    ಆದರೆ (lnum == curwin->w_cursor.lnum)
	ಹಿಂತಿರುಗಿ FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    ಸ್ಥಿರ linenr_T
diff_get_corresponding_ಸಾಲು_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    ಪೂರ್ಣಾಂಕ		idx1;
    ಪೂರ್ಣಾಂಕ		idx2;
    diff_T	*dp;
    ಪೂರ್ಣಾಂಕ		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    ಆದರೆ (idx1 == DB_ಎಣಿಕೆ || idx2 == DB_ಎಣಿಕೆ || curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ lnum1;

    ಆದರೆ (curtab->tp_diff_invalid)
	ex_diffupdate(ಖಾಲಿ);		// update after a big change

    ಆದರೆ (curtab->tp_ಮೊದಲ_diff == ಖಾಲಿ)		// no diffs today
	ಹಿಂತಿರುಗಿ lnum1;

    FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(curtab, dp)
    {
	ಆದರೆ (dp->df_lnum[idx1] > lnum1)
	    ಹಿಂತಿರುಗಿ lnum1 - baseline;
	ಆದರೆ ((dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    ಆದರೆ (baseline > dp->df_ಎಣಿಕೆ[idx2])
		baseline = dp->df_ಎಣಿಕೆ[idx2];

	    ಹಿಂತಿರುಗಿ dp->df_lnum[idx2] + baseline;
	}
	ಆದರೆ (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_ಎಣಿಕೆ[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_ಎಣಿಕೆ[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    ಹಿಂತಿರುಗಿ curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1])
				   - (dp->df_lnum[idx2] + dp->df_ಎಣಿಕೆ[idx2]);
    }

    // If we get here then the cursor is after the last diff
    ಹಿಂತಿರುಗಿ lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_ಸಾಲು(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_ಸಾಲು_int(buf1, lnum1);

    // don't end up past the end of the file
    ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    ಆದರೆ (idx == DB_ಎಣಿಕೆ)		// safety check
	ಹಿಂತಿರುಗಿ (linenr_T)0;

    ಆದರೆ (curtab->tp_diff_invalid)
	ex_diffupdate(ಖಾಲಿ);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_ಒಳ_TAB(curtab, dp)
	ಆದರೆ (lnum <= dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ನಿಲ್ಲಿಸು;

    // When after the last change, compute relative to the last line number.
    ಆದರೆ (dp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					- (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_ಬಫರ್);
    ಆದರೆ (i == DB_ಎಣಿಕೆ)			// safety check
	ಹಿಂತಿರುಗಿ (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    ಆದರೆ (n > dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i])
	n = dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i];
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_diff_ed(
	char_u	    *ಸಾಲು,
	diffhunk_T  *hunk)
{
    char_u *p;
    ದೀರ್ಘ    f1, l1, f2, l2;
    ಪೂರ್ಣಾಂಕ	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = ಸಾಲು;
    f1 = getdigits(&p);
    ಆದರೆ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	l1 = f1;
    ಆದರೆ (*p != 'a' && *p != 'c' && *p != 'd')
	ಹಿಂತಿರುಗಿ FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    ಆದರೆ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	l2 = f2;
    ಆದರೆ (l1 < f1 || l2 < f2)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->ಎಣಿಕೆ_orig = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	hunk->lnum_orig = f1;
	hunk->ಎಣಿಕೆ_orig = l1 - f1 + 1;
    }
    ಆದರೆ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->ಎಣಿಕೆ_new = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	hunk->lnum_new = f2;
	hunk->ಎಣಿಕೆ_new = l2 - f2 + 1;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_diff_unified(
	char_u	    *ಸಾಲು,
	diffhunk_T  *hunk)
{
    char_u *p;
    ದೀರ್ಘ    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = ಸಾಲು;
    ಆದರೆ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	ಆದರೆ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    oldcount = 1;
	ಆದರೆ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    ಆದರೆ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		newcount = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FAIL;	// invalid diff format

	ಆದರೆ (oldcount == 0)
	    oldline += 1;
	ಆದರೆ (newcount == 0)
	    newline += 1;
	ಆದರೆ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->ಎಣಿಕೆ_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->ಎಣಿಕೆ_new = newcount;

	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
xdiff_ಹೊರಹೋಗು_indices(
	ದೀರ್ಘ ಆರಂಭ_a,
	ದೀರ್ಘ ಎಣಿಕೆ_a,
	ದೀರ್ಘ ಆರಂಭ_b,
	ದೀರ್ಘ ಎಣಿಕೆ_b,
	ಶೂನ್ಯ *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	ಹಿಂತಿರುಗಿ -1;
    }

    p->lnum_orig  = ಆರಂಭ_a + 1;
    p->ಎಣಿಕೆ_orig = ಎಣಿಕೆ_a;
    p->lnum_new   = ಆರಂಭ_b + 1;
    p->ಎಣಿಕೆ_new  = ಎಣಿಕೆ_b;
    ((diffhunk_T **)dout->dout_ga.ga_ದತ್ತಾಂಶ)[dout->dout_ga.ga_len++] = p;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
xdiff_ಹೊರಹೋಗು_unified(
	ಶೂನ್ಯ *priv,
	mmbuffer_t *mb,
	ಪೂರ್ಣಾಂಕ nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].ಗಾತ್ರ_2);

    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ_ಅಂತ್ಯ	// FEAT_DIFF

#ಆದರೆ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    ಶೂನ್ಯ
f_diff_filler(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_DIFF
    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    ಶೂನ್ಯ
f_diff_hlID(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ಸ್ಥಿರ linenr_T	prev_lnum = 0;
    ಸ್ಥಿರ varnumber_T	changedtick = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		fnum = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prev_diff_flags = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		change_ಆರಂಭ = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		change_ಕೊನೆ = 0;
    ಸ್ಥಿರ hlf_T	hlID = (hlf_T)0;
    ಪೂರ್ಣಾಂಕ			cache_results = TRUE;
    ಪೂರ್ಣಾಂಕ			ಲಂಬಸಾಲು;
    diffline_T		diffline;

    CLEAR_ಕ್ಷೇತ್ರ(diffline);

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    ಆದರೆ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    ಆದರೆ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	ಪೂರ್ಣಾಂಕ linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	ಆದರೆ (linestatus < 0)
	{
	    ಆದರೆ (linestatus == -1)
	    {
		change_ಆರಂಭ = MAXCOL;
		change_ಕೊನೆ = -1;
		ಆದರೆ (diff_ಕಂಡುಹಿಡಿ_change(curwin, lnum, &diffline))
		    hlID = HLF_ಕೂಡಿಸು;	// added line
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    hlID = HLF_CHD;	// changed line
		    ಆದರೆ (diffline.ಅಂಕೆ_changes > 0 && cache_results)
		    {
			change_ಆರಂಭ = diffline.changes[0].dc_ಆರಂಭ[diffline.bufidx];
			change_ಕೊನೆ = diffline.changes[0].dc_ಕೊನೆ[diffline.bufidx];
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		hlID = HLF_ಕೂಡಿಸು;	// added line
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    hlID = (hlf_T)0;

	ಆದರೆ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    ಆದರೆ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1; // ignore type error in {col}
	ಆದರೆ (cache_results)
	{
	    ಆದರೆ (ಲಂಬಸಾಲು >= change_ಆರಂಭ && ಲಂಬಸಾಲು < change_ಕೊನೆ)
		hlID = HLF_TXD;			// changed text
	    ಇಲ್ಲದಿದ್ದರೆ
		hlID = HLF_CHD;			// changed line
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    hlID = HLF_CHD;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < diffline.ಅಂಕೆ_changes; i++)
	    {
		ಪೂರ್ಣಾಂಕ added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_ಆರಂಭ, &change_ಕೊನೆ);
		ಆದರೆ (ಲಂಬಸಾಲು >= change_ಆರಂಭ && ಲಂಬಸಾಲು < change_ಕೊನೆ)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (ಲಂಬಸಾಲು < change_ಆರಂಭ)
		    // the remaining changes are past this column and not relevant
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = hlID == (hlf_T)0 ? 0 : (ಪೂರ್ಣಾಂಕ)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_diff_optarg(
    typval_T	    *opts,
    ಪೂರ್ಣಾಂಕ		    *diffopts,
    ದೀರ್ಘ	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    ಪೂರ್ಣಾಂಕ		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    ಆದರೆ (algo != ಖಾಲಿ)
    {
	ಆದರೆ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_ಕನಿಷ್ಠತಮ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    ಆದರೆ (output_fmt != ಖಾಲಿ)
    {
	ಆದರೆ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    *diff_ctxlen = dict_get_ಸಂಖ್ಯೆ_def(d, "context", 0);
    ಆದರೆ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    ಆದರೆ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    ಆದರೆ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    ಆದರೆ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    ಆದರೆ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    ಆದರೆ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    ಆದರೆ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_ಗಮ್ಯ_diffin(list_T *l, diffin_T *din, ಪೂರ್ಣಾಂಕ icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	str = tv_get_string(&li->li_tv);
	ಆದರೆ (icase)
	{
	    str = strlow_save(str);
	    ಆದರೆ (str == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	ಆದರೆ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ;
    din->din_mmfile.ಗಾತ್ರ_2 = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ಸ್ಥಿರ dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    ಆದರೆ (hunk_dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(hunk_dict, "from_count", hunk->ಎಣಿಕೆ_orig);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(hunk_dict, "to_count", hunk->ಎಣಿಕೆ_new);

    ಹಿಂತಿರುಗಿ hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    ಶೂನ್ಯ
f_diff(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    ಆದರೆ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    CLEAR_ಕ್ಷೇತ್ರ(dio);
    dio.dio_ಆಂತರಿಕ = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    ಪೂರ್ಣಾಂಕ		save_diff_flags = diff_flags;
    ದೀರ್ಘ	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_ಆಂತರಿಕ;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	ಆದರೆ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    ಹಿಂತಿರುಗಿ;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_ಗಮ್ಯ_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_ಗಮ್ಯ_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    ಪೂರ್ಣಾಂಕ	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    ಆದರೆ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    ಪೂರ್ಣಾಂಕ diff_ಸ್ಥಿತಿ = diff_file(&dio);

    // restore 'diffexpr'
    ಆದರೆ (restore_diffexpr)
	*p_dex = cc;

    ಆದರೆ (diff_ಸ್ಥಿತಿ == FAIL)
	ಹೋಗು ಮುಗಿದ;

    ಪೂರ್ಣಾಂಕ		hunk_idx = 0;
    dict_T	*hunk_dict;

    ಆದರೆ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	ಆದರೆ (rettv_list_alloc(rettv) != OK)
	    ಹೋಗು ಮುಗಿದ;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ಖಾಲಿ;
	ಆಗ (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_ದತ್ತಾಂಶ)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    ಆದರೆ (hunk_dict == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;

	    ಆದರೆ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_ದತ್ತಾಂಶ);
    }

ಮುಗಿದ:
    clear_diffin(&dio.dio_new);
    ಆದರೆ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    ಇಲ್ಲದಿದ್ದರೆ
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#ಆದರೆ_ಅಂತ್ಯ
