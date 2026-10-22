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

#ಯದಿ defined(FEAT_DIFF)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_busy = FALSE;	    // using diff structs, don't change them
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_need_update = FALSE; // ex_diffupdate needs to be called

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
# define DIFF_INTERNAL	0x200	// use internal xdiff algorithm
# define DIFF_CLOSE_OFF	0x400	// diffoff when closing window
# define DIFF_FOLLOWWRAP	0x800	// follow the wrap option
# define DIFF_LINEMATCH  0x1000  // match most similar lines within diff
# define DIFF_INLINE_NONE    0x2000  // no inline highlight
# define DIFF_INLINE_SIMPLE  0x4000  // inline highlight with simple algorithm
# define DIFF_INLINE_CHAR    0x8000  // inline highlight with character diff
# define DIFF_INLINE_WORD    0x10000 // inline highlight with word diff
# define DIFF_ANCHOR	0x20000	// use 'diffanchors' to anchor the diff
# define ALL_WHITE_DIFF (DIFF_IWHITE | DIFF_IWHITEALL | DIFF_IWHITEEOL)
# define ALL_INLINE (DIFF_INLINE_NONE | DIFF_INLINE_SIMPLE | DIFF_INLINE_CHAR | DIFF_INLINE_WORD)
# define ALL_INLINE_DIFF (DIFF_INLINE_CHAR | DIFF_INLINE_WORD)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ಸ್ಥಿರ long diff_algorithm = XDF_INDENT_HEURISTIC;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# ಯದಿ defined(MSWIN)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    linenr_T lnum_orig;
    long     ಎಣಿಕೆ_orig;
    linenr_T lnum_new;
    long     ಎಣಿಕೆ_new;
} diffhunk_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    ಪೂರ್ಣಾಙ್ಕ		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    ಪೂರ್ಣಾಙ್ಕ		    dio_ctxlen;   // unified diff context length
} diffio_T;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_buf_idx(buf_T *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ಸ್ಥಿರ ಶೂನ್ಯ diff_mark_adjust_tp(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
ಸ್ಥಿರ ಶೂನ್ಯ diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_check_sanity(tabpage_T *tp, diff_T *dp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_external_diff(diffio_T *diffio);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_file(diffio_T *diffio);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_equal_entry(diff_T *dp, ಪೂರ್ಣಾಙ್ಕ idx1, ಪೂರ್ಣಾಙ್ಕ idx2);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ಸ್ಥಿರ ಶೂನ್ಯ diff_fold_update(diff_T *dp, ಪೂರ್ಣಾಙ್ಕ skip_idx);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ diff_read(ಪೂರ್ಣಾಙ್ಕ idx_orig, ಪೂರ್ಣಾಙ್ಕ idx_new, diffio_T *dio);
ಸ್ಥಿರ ಶೂನ್ಯ diff_copy_entry(diff_T *dprev, diff_T *dp, ಪೂರ್ಣಾಙ್ಕ idx_orig, ಪೂರ್ಣಾಙ್ಕ idx_new);
ಸ್ಥಿರ diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_diff_ed(char_u *ಸಾಲು, diffhunk_T *hunk);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_diff_unified(char_u *ಸಾಲು, diffhunk_T *hunk);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xdiff_out_indices(long start_a, long ಎಣಿಕೆ_a, long start_b, long ಎಣಿಕೆ_b, ಶೂನ್ಯ *priv);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xdiff_out_unified(ಶೂನ್ಯ *priv, mmbuffer_t *mb, ಪೂರ್ಣಾಙ್ಕ nbuf);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_diffanchors(ಪೂರ್ಣಾಙ್ಕ check_only, buf_T *buf, linenr_T *anchors, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    ಪ್ರತಿ ((dp) = (tp)->tp_first_diff; (dp) != ರಿಕ್ತ; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ಸ್ಥಿರ diff_T	    *diff_finger_dp = ರಿಕ್ತ;
ಸ್ಥಿರ tabpage_T    *diff_finger_tp = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    diff_finger_idx = -1;
ಸ್ಥಿರ linenr_T	    diff_finger_lnum = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_diffblock(diff_T *dp)
{
    ಯದಿ (dp == diff_finger_dp)
	diff_finger_dp = ರಿಕ್ತ;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ಸ್ಥಿರ diff_T *
diff_scan_start(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ idx, linenr_T lnum)
{
    ಯದಿ (diff_finger_dp != ರಿಕ್ತ
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	ಪ್ರತ್ಯಾಗಮ diff_finger_dp;
    ಪ್ರತ್ಯಾಗಮ tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_scan_remember(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ರಿಕ್ತ ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    ಶೂನ್ಯ
diff_buf_delete(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ಯದಿ (i != DB_ಎಣಿಕೆ)
	{
	    tp->tp_diffbuf[i] = ರಿಕ್ತ;
	    tp->tp_diff_invalid = TRUE;
	    ಯದಿ (tp == curtab)
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
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    ಯದಿ (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		ಭಙ್ಗ;
	ಯದಿ (wp == ರಿಕ್ತ)
	{
	    i = diff_buf_idx(win->w_buffer);
	    ಯದಿ (i != DB_ಎಣಿಕೆ)
	    {
		curtab->tp_diffbuf[i] = ರಿಕ್ತ;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    ಅನ್ಯಥಾ
	diff_buf_add(win->w_buffer);
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
diff_buf_add(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (diff_buf_idx(buf) != DB_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;		// It's already there.

    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಯದಿ (curtab->tp_diffbuf[i] == ರಿಕ್ತ)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    ಪ್ರತ್ಯಾಗಮ;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_ಎಣಿಕೆ);
}

/*
 * Remove all buffers to make diffs for.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_buf_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
	{
	    curtab->tp_diffbuf[i] = ರಿಕ್ತ;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_buf_idx(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಪ್ರತಿ (idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
	ಯದಿ (curtab->tp_diffbuf[idx] == buf)
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಪ್ರತಿ (idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
	ಯದಿ (tp->tp_diffbuf[idx] == buf)
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    ಶೂನ್ಯ
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ಯದಿ (i != DB_ಎಣಿಕೆ)
	{
	    tp->tp_diff_invalid = TRUE;
	    ಯದಿ (tp == curtab)
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
    long	amount,
    long	amount_after)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	ಯದಿ (idx != DB_ಎಣಿಕೆ)
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
    ಪೂರ್ಣಾಙ್ಕ		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		inserted, deleted;
    ಪೂರ್ಣಾಙ್ಕ		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    ಪೂರ್ಣಾಙ್ಕ		check_unchanged;

    diff_finger_dp = ರಿಕ್ತ;		// may shift df_lnum, invalidate the finger

    ಯದಿ (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    ಯದಿ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    ಅನ್ಯಥಾ
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ರಿಕ್ತ;
    dp = tp->tp_first_diff;
    ಪ್ರತಿ (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	ಯದಿ ((dp == ರಿಕ್ತ || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ರಿಕ್ತ
		    || dprev->df_lnum[idx] + dprev->df_ಎಣಿಕೆ[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    ಯದಿ (dnext == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_ಎಣಿಕೆ[idx] = inserted;
	    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		ಯದಿ (tp->tp_diffbuf[i] != ರಿಕ್ತ && i != idx)
		{
		    ಯದಿ (dprev == ರಿಕ್ತ)
			dnext->df_lnum[i] = line1;
		    ಅನ್ಯಥಾ
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_ಎಣಿಕೆ[i])
			    - (dprev->df_lnum[idx] + dprev->df_ಎಣಿಕೆ[idx]);
		    dnext->df_ಎಣಿಕೆ[i] = deleted;
		}
	}

	// if at end of the list, quit
	ಯದಿ (dp == ರಿಕ್ತ)
	    ಭಙ್ಗ;

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
	last = dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx] - 1;

	// 1. change completely above line1: nothing to do
	ಯದಿ (last >= line1 - 1)
	{
	    ಯದಿ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		ಯದಿ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		ಅನುವರ್ತ;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    ಯದಿ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		ಯದಿ (amount_after == 0)
		    ಭಙ್ಗ;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    ಅನ್ಯಥಾ
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		ಯದಿ (deleted > 0)
		{
		    off = 0;
		    ಯದಿ (dp->df_lnum[idx] >= line1)
		    {
			ಯದಿ (last <= line2)
			{
			    // 4. delete all lines of diff
			    ಯದಿ (dp->df_next != ರಿಕ್ತ
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_ಎಣಿಕೆ[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    ಅನ್ಯಥಾ
				n = deleted - dp->df_ಎಣಿಕೆ[idx];
			    dp->df_ಎಣಿಕೆ[idx] = 0;
			}
			ಅನ್ಯಥಾ
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_ಎಣಿಕೆ[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_ಎಣಿಕೆ[idx] -= last - lnum_deleted + 1;
			    ಯದಿ (dp->df_next != ರಿಕ್ತ
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    ಅನ್ಯಥಾ
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			ಅನ್ಯಥಾ
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_ಎಣಿಕೆ[idx] -= deleted;
			}
		    }

		    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
			ಯದಿ (tp->tp_diffbuf[i] != ರಿಕ್ತ && i != idx)
			{
			    ಯದಿ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    ಅನ್ಯಥಾ
				dp->df_lnum[i] = 1;
			    dp->df_ಎಣಿಕೆ[i] += n;
			}
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_ಎಣಿಕೆ[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    ಅನ್ಯಥಾ
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		ಯದಿ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	ಯದಿ (dprev != ರಿಕ್ತ && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_ಎಣಿಕೆ[idx]
							== dp->df_lnum[idx])
	{
	    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		ಯದಿ (tp->tp_diffbuf[i] != ರಿಕ್ತ)
		    dprev->df_ಎಣಿಕೆ[i] += dp->df_ಎಣಿಕೆ[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	ಅನ್ಯಥಾ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = ರಿಕ್ತ;
    dp = tp->tp_first_diff;
    ಯಾವತ್ (dp != ರಿಕ್ತ)
    {
	// All counts are zero, remove this entry.
	ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	    ಯದಿ (tp->tp_diffbuf[i] != ರಿಕ್ತ && dp->df_ಎಣಿಕೆ[i] != 0)
		ಭಙ್ಗ;
	ಯದಿ (i == DB_ಎಣಿಕೆ)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    ಯದಿ (dprev == ರಿಕ್ತ)
		tp->tp_first_diff = dnext;
	    ಅನ್ಯಥಾ
		dprev->df_next = dnext;
	}
	ಅನ್ಯಥಾ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    ಯದಿ (tp == curtab)
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
    ಯದಿ (dnew == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    diff_finger_dp = ರಿಕ್ತ;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    ಯದಿ (dprev == ರಿಕ್ತ)
	tp->tp_first_diff = dnew;
    ಅನ್ಯಥಾ
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, ಪರಿಮಾಣ(diffline_change_T), 20);
    ಪ್ರತ್ಯಾಗಮ dnew;
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
    ಪೂರ್ಣಾಙ್ಕ		i_org;
    ಪೂರ್ಣಾಙ್ಕ		i_new;
    ಪೂರ್ಣಾಙ್ಕ		off_org, off_new;
    char_u	*ಸಾಲು_org;
    ಪೂರ್ಣಾಙ್ಕ		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    ಪ್ರತಿ (i_org = 0; i_org < DB_ಎಣಿಕೆ; ++i_org)
	ಯದಿ (tp->tp_diffbuf[i_org] != ರಿಕ್ತ)
	    ಭಙ್ಗ;
    ಯದಿ (i_org == DB_ಎಣಿಕೆ)	// safety check
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (diff_check_sanity(tp, dp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    ಪ್ರತಿ (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	ಯಾವತ್ (dp->df_ಎಣಿಕೆ[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    ಯದಿ (dir == BACKWARD)
		off_org = dp->df_ಎಣಿಕೆ[i_org] - 1;
	    ಸಾಲು_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    ಯದಿ (ಸಾಲು_org == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    ಪ್ರತಿ (i_new = i_org + 1; i_new < DB_ಎಣಿಕೆ; ++i_new)
	    {
		ಯದಿ (tp->tp_diffbuf[i_new] == ರಿಕ್ತ)
		    ಅನುವರ್ತ;
		ಯದಿ (dir == BACKWARD)
		    off_new = dp->df_ಎಣಿಕೆ[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		ಯದಿ (off_new < 0 || off_new >= dp->df_ಎಣಿಕೆ[i_new])
		    ಭಙ್ಗ;
		ಯದಿ (diff_cmp(ಸಾಲು_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    ಭಙ್ಗ;
	    }
	    vim_free(ಸಾಲು_org);

	    // Stop when a line isn't equal in all diff buffers.
	    ಯದಿ (i_new != DB_ಎಣಿಕೆ)
		ಭಙ್ಗ;

	    // Line matched in all buffers, remove it from the diff.
	    ಪ್ರತಿ (i_new = i_org; i_new < DB_ಎಣಿಕೆ; ++i_new)
		ಯದಿ (tp->tp_diffbuf[i_new] != ರಿಕ್ತ)
		{
		    ಯದಿ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_ಎಣಿಕೆ[i_new];
		}
	}
	ಯದಿ (dir == BACKWARD)
	    ಭಙ್ಗ;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಯದಿ (tp->tp_diffbuf[i] != ರಿಕ್ತ)
	    ಯದಿ (dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    ಶೂನ್ಯ
diff_redraw(
    ಪೂರ್ಣಾಙ್ಕ		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		used_max_fill_other = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		used_max_fill_curwin = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	ಯದಿ (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    ಅನುವರ್ತ;

	redraw_win_later(wp, UPD_SOME_VALID);
	ಯದಿ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	ಯದಿ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	ಯದಿ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    ಯದಿ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    ಅನ್ಯಥಾ ಯದಿ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		ಯದಿ (wp == curwin)
		    used_max_fill_curwin = TRUE;
		ಅನ್ಯಥಾ ಯದಿ (wp_other != ರಿಕ್ತ)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    ಯದಿ (wp_other != ರಿಕ್ತ && curwin->w_p_scb)
    {
	ಯದಿ (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	ಅನ್ಯಥಾ ಯದಿ (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_diffin(diffin_T *din)
{
    ಯದಿ (din->din_fname == ರಿಕ್ತ)
	VIM_CLEAR(din->din_mmfile.ptr);
    ಅನ್ಯಥಾ
	mch_remove(din->din_fname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_diffout(diffout_T *dout)
{
    ಯದಿ (dout->dout_fname == ರಿಕ್ತ)
	ga_clear_strings(&dout->dout_ga);
    ಅನ್ಯಥಾ
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    ಯದಿ (end < 0)
	end = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = ರಿಕ್ತ;
	din->din_mmfile.size = 0;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // xdiff requires one big block of memory with all the text.
    ಪ್ರತಿ (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    ಯದಿ (ptr == ರಿಕ್ತ)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	ಯದಿ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    din->din_mmfile.ptr = (ವರ್ಣ *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    ಪ್ರತಿ (lnum = start; lnum <= end; ++lnum)
    {
	ಪ್ರತಿ (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    ಯದಿ (diff_flags & DIFF_ICASE)
	    {
		ಪೂರ್ಣಾಙ್ಕ c;
		ಪೂರ್ಣಾಙ್ಕ	orig_len;
		ಪೂರ್ಣಾಙ್ಕ	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		ಯದಿ (*s == NL)
		    c = NUL;
		ಅನ್ಯಥಾ
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		ಯದಿ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		ಅನ್ಯಥಾ
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    ಯದಿ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    ಅನ್ಯಥಾ
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		save_ml_flags;
    char_u	*save_ff;
    ಪೂರ್ಣಾಙ್ಕ		save_cmod_flags;

    ಯದಿ (din->din_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    ಯದಿ (frames_locked())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (end < 0)
	end = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    ಯದಿ (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, ರಿಕ್ತ,
			start, end,
			ರಿಕ್ತ, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    ಪ್ರತ್ಯಾಗಮ r;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lnum_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    ಯದಿ (lnum1 < lnum2)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (lnum1 > lnum2)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_try_update(
	diffio_T    *dio,
	ಪೂರ್ಣಾಙ್ಕ	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		idx_new;

    ಯದಿ (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, ಪರಿಮಾಣ(ವರ್ಣ *), 1000);
    }
    ಅನ್ಯಥಾ
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	ಯದಿ (dio->dio_orig.din_fname == ರಿಕ್ತ
		|| dio->dio_new.din_fname == ರಿಕ್ತ
		|| dio->dio_diff.dout_fname == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
    }

    // Check external diff is actually working.
    ಯದಿ (!dio->dio_internal && check_external_diff(dio) == FAIL)
	ಗಚ್ಛ theend;

    // :diffupdate!
    ಯದಿ (eap != ರಿಕ್ತ && eap->forceit)
	ಪ್ರತಿ (idx_new = idx_orig; idx_new < DB_ಎಣಿಕೆ; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ಯದಿ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_anchors = INT_MAX;
    linenr_T anchors[DB_ಎಣಿಕೆ][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    ಯದಿ (diff_flags & DIFF_ANCHOR)
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < DB_ಎಣಿಕೆ; idx++)
	{
	    ಯದಿ (curtab->tp_diffbuf[idx] == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ buf_ಅಂಕೆ_anchors = 0;
	    ಯದಿ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_ಅಂಕೆ_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		ಅಂಕೆ_anchors = 0;
		CLEAR_FIELD(anchors);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (buf_ಅಂಕೆ_anchors < ಅಂಕೆ_anchors)
		ಅಂಕೆ_anchors = buf_ಅಂಕೆ_anchors;

	    ಯದಿ (buf_ಅಂಕೆ_anchors > 0)
		qsort((ಶೂನ್ಯ *)anchors[idx],
			(size_t)buf_ಅಂಕೆ_anchors,
			ಪರಿಮಾಣ(linenr_T),
			lnum_compare);
	}
    }
    ಯದಿ (ಅಂಕೆ_anchors == INT_MAX)
	ಅಂಕೆ_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ anchor_i = 0; anchor_i <= ಅಂಕೆ_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ರಿಕ್ತ;
	ಯದಿ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = ರಿಕ್ತ;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == ಅಂಕೆ_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	ಯದಿ (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    ಯದಿ (orig_diff != ರಿಕ್ತ)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    ಗಚ್ಛ theend;
	}

	// Make a difference between the first buffer and every other.
	ಪ್ರತಿ (idx_new = idx_orig + 1; idx_new < DB_ಎಣಿಕೆ; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
		ಅನುವರ್ತ; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == ಅಂಕೆ_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    ಯದಿ (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		ಅನುವರ್ತ;
	    ಯದಿ (diff_file(dio) == FAIL)
		ಅನುವರ್ತ;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	ಯದಿ (anchor_i != 0)
	{
	    diff_finger_dp = ರಿಕ್ತ;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    ಪ್ರತಿ (diff_T *dp = curtab->tp_first_diff; dp != ರಿಕ್ತ; dp = dp->df_next)
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < DB_ಎಣಿಕೆ; idx++)
		{
		    ಯದಿ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    ಯದಿ (orig_diff != ರಿಕ್ತ)
	    {
		diff_T *last_diff = orig_diff;
		ಯಾವತ್ (last_diff->df_next != ರಿಕ್ತ)
		    last_diff = last_diff->df_next;
		last_diff->df_next = curtab->tp_first_diff;
		curtab->tp_first_diff = orig_diff;
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
    ಪೂರ್ಣಾಙ್ಕ
diff_internal(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_internal_failed(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ idx;

    // Only need to do something when there is another buffer.
    ಪ್ರತಿ (idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
	ಯದಿ (curtab->tp_diffbuf[idx] != ರಿಕ್ತ
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ		idx_orig;
    ಪೂರ್ಣಾಙ್ಕ		idx_new;
    diffio_T	diffio;
    ಪೂರ್ಣಾಙ್ಕ		had_diffs = curtab->tp_first_diff != ರಿಕ್ತ;

    ಯದಿ (diff_busy)
    {
	diff_need_update = TRUE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    ಪ್ರತಿ (idx_orig = 0; idx_orig < DB_ಎಣಿಕೆ; ++idx_orig)
	ಯದಿ (curtab->tp_diffbuf[idx_orig] != ರಿಕ್ತ)
	    ಭಙ್ಗ;
    ಯದಿ (idx_orig == DB_ಎಣಿಕೆ)
	ಗಚ್ಛ theend;

    // Only need to do something when there is another buffer.
    ಪ್ರತಿ (idx_new = idx_orig + 1; idx_new < DB_ಎಣಿಕೆ; ++idx_new)
	ಯದಿ (curtab->tp_diffbuf[idx_new] != ರಿಕ್ತ)
	    ಭಙ್ಗ;
    ಯದಿ (idx_new == DB_ಎಣಿಕೆ)
	ಗಚ್ಛ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    ಯದಿ (diffio.dio_internal && diff_internal_failed())
    {
	// Internal diff failed, use external diff instead.
	CLEAR_FIELD(diffio);
	diff_try_update(&diffio, idx_orig, eap);
    }

    // force updating cursor position on screen
    curwin->w_valid_cursor.lnum = 0;

theend:
    // A redraw is needed if there were diffs and they were cleared, or there
    // are diffs now, which means they got updated.
    ಯದಿ (had_diffs || curtab->tp_first_diff != ರಿಕ್ತ)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		ok;
    ಪೂರ್ಣಾಙ್ಕ		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    ಪ್ರತಿ (;;)
    {
	ok = FALSE;
	fd = mch_fopen((ವರ್ಣ *)diffio->dio_orig.din_fname, "w");
	ಯದಿ (fd == ರಿಕ್ತ)
	    io_error = TRUE;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((ವರ್ಣ *)diffio->dio_new.din_fname, "w");
	    ಯದಿ (fd == ರಿಕ್ತ)
		io_error = TRUE;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ರಿಕ್ತ;
		ಯದಿ (diff_file(diffio) == OK)
		    fd = mch_fopen((ವರ್ಣ *)diffio->dio_diff.dout_fname, "r");
		ಯದಿ (fd == ರಿಕ್ತ)
		    io_error = TRUE;
		ಅನ್ಯಥಾ
		{
		    char_u	linebuf[LBUFLEN];

		    ಪ್ರತಿ (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			ಯದಿ (vim_fgets(linebuf, LBUFLEN, fd))
			    ಭಙ್ಗ;
			ಯದಿ (STRNCMP(linebuf, "1c1", 3) == 0
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
	ಯದಿ (*p_dex != NUL)
	    ಭಙ್ಗ;
# endif

# ಯದಿ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	ಯದಿ (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	ಯದಿ (diff_a_works != MAYBE)
	    ಭಙ್ಗ;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	ಯದಿ (ok)
	    ಭಙ್ಗ;
    }
    ಯದಿ (!ok)
    {
	ಯದಿ (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# ಯದಿ defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Invoke the xdiff function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    ಯದಿ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    ಯದಿ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    ಯದಿ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    ಯದಿ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_ಸಾಲುಗಳು;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    ಯದಿ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    ಅನ್ಯಥಾ
	emit_cb.out_ಸಾಲು = xdiff_out_unified;
    ಯದಿ (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    ಯದಿ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ
# endif
    // Use xdiff for generating the diff.
    ಯದಿ (dio->dio_internal)
	ಪ್ರತ್ಯಾಗಮ diff_file_internal(dio);

    ಯದಿ (check_restricted())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    ಯದಿ (cmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    ಯದಿ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((ವರ್ಣ *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# ಯದಿ defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# ಅನ್ಯಥಾ
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (ಪೂರ್ಣಾಙ್ಕ)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (ಶೂನ್ಯ)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    ಪ್ರತ್ಯಾಗಮ OK;
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
    char_u	*buf = ರಿಕ್ತ;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ರಿಕ್ತ;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ರಿಕ್ತ;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ರಿಕ್ತ;

# ifdef FEAT_BROWSE
    ಯದಿ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ರಿಕ್ತ, ರಿಕ್ತ,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ರಿಕ್ತ);
	ಯದಿ (browseFile == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    ಯದಿ (tmp_orig == ರಿಕ್ತ || tmp_new == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    // Write the current buffer to "tmp_orig".
    ಯದಿ (buf_write(curbuf, tmp_orig, ರಿಕ್ತ,
		(linenr_T)1, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
				     ರಿಕ್ತ, FALSE, FALSE, FALSE, TRUE) == FAIL)
	ಗಚ್ಛ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ರಿಕ್ತ ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    ಯದಿ (esc_name == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    ಯದಿ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((ವರ್ಣ *)dirbuf) != 0)
	dirbuf[0] = NUL;
    ಅನ್ಯಥಾ
    {
#  ifdef TEMPDIRNAMES
	ಯದಿ (vim_tempdir != ರಿಕ್ತ)
	    vim_ignored = mch_chdir((ವರ್ಣ *)vim_tempdir);
	ಅನ್ಯಥಾ
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    ಯದಿ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ರಿಕ್ತ ? fullname :
#  endif
		eap->arg, tmp_new);
    ಅನ್ಯಥಾ
# endif
    {
	ಯದಿ (check_restricted())
	    ಗಚ್ಛ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((ವರ್ಣ *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(ಶೂನ್ಯ)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    ಯದಿ (dirbuf[0] != NUL)
    {
	ಯದಿ (mch_chdir((ವರ್ಣ *)dirbuf) != 0)
	    emsg(_(e_cannot_go_back_to_previous_directory));
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
    ಯದಿ (mch_stat((ವರ್ಣ *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    ಅನ್ಯಥಾ
    {
	ಯದಿ (curbuf->b_fname != ರಿಕ್ತ)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    ಯದಿ (newname != ರಿಕ್ತ)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	ಯದಿ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    ಯದಿ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		ಯದಿ (newname != ರಿಕ್ತ)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    ಯದಿ (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    ಯದಿ (tmp_orig != ರಿಕ್ತ)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    ಯದಿ (tmp_new != ರಿಕ್ತ)
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

    set_bufref(&old_curbuf, curbuf);
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    // Need to compute w_fraction when no redraw happened yet.
    validate_cursor();
    set_fraction(curwin);

    // don't use a new tab page, each tab page has its own diffs
    cmdmod.cmod_tab = 0;

    ಯದಿ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    ಯದಿ (curwin == old_curwin)		// split didn't work
	ಪ್ರತ್ಯಾಗಮ;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    ಯದಿ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	ಯದಿ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_ಸಾಲು(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    ಶೂನ್ಯ
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_diff_option(win_T *wp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"diff", (long)ಮೌಲ್ಯ, ರಿಕ್ತ, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    ಶೂನ್ಯ
diff_win_options(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    ಯದಿ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    ಯದಿ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    ಯದಿ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	ಯದಿ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    ಯದಿ (!wp->w_p_diff)
    {
	ಯದಿ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    ಯದಿ (!wp->w_p_diff)
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
    changed_window_setting_win(wp);
# endif
    ಯದಿ (vim_strchr(p_sbo, 'h') == ರಿಕ್ತ)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    ಯದಿ (addbuf)
	diff_buf_add(wp->w_buffer);
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
    ಪೂರ್ಣಾಙ್ಕ		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    ಯದಿ (wp->w_p_diff_saved)
	    {

		ಯದಿ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		ಯದಿ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		ಯದಿ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    ಯದಿ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		ಯದಿ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		ಯದಿ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		ಯದಿ (wp->w_p_fen)
		    wp->w_p_fen = foldmethodIsManual(wp) ? FALSE
							 : wp->w_p_fen_save;

		foldUpdateAll(wp);
# endif
	    }
	    // remove filler lines
	    wp->w_topfill = 0;

	    // make sure topline is not halfway a fold and cursor is
	    // invalidated
	    changed_window_setting_win(wp);

	    // Note: 'sbo' is not restored, it's a global option.
	    diff_buf_adjust(wp);
	}
	diffwin |= wp->w_p_diff;
    }

    // Also remove hidden buffers from the list.
    ಯದಿ (eap->forceit)
	diff_buf_clear();

    ಯದಿ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    ಯದಿ (!diffwin && vim_strchr(p_sbo, 'h') != ರಿಕ್ತ)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_read(
    ಪೂರ್ಣಾಙ್ಕ		idx_orig,	// idx of original file
    ಪೂರ್ಣಾಙ್ಕ		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ರಿಕ್ತ;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*ಸಾಲು;
    long	off;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ರಿಕ್ತ;	    // init to avoid gcc warning

    ಗಣನಾ {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    ಯದಿ (dout->dout_fname == ರಿಕ್ತ)
    {
	diffstyle = DIFF_UNIFIED;
    }
    ಅನ್ಯಥಾ
    {
	fd = mch_fopen((ವರ್ಣ *)dout->dout_fname, "r");
	ಯದಿ (fd == ರಿಕ್ತ)
	{
	    emsg(_(e_cannot_read_diff_output));
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	ಯದಿ (hunk == ರಿಕ್ತ)
	{
	    ಯದಿ (fd != ರಿಕ್ತ)
		fclose(fd);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (dio->dio_internal)
	{
	    ಯದಿ (ಸಾಲು_hunk_idx >= dout->dout_ga.ga_len)
		ಭಙ್ಗ;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[ಸಾಲು_hunk_idx++];
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (fd == ರಿಕ್ತ)
	    {
		ಯದಿ (ಸಾಲು_hunk_idx >= dout->dout_ga.ga_len)
		    ಭಙ್ಗ;	    // did last line
		ಸಾಲು = ((char_u **)dout->dout_ga.ga_data)[ಸಾಲು_hunk_idx++];
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (vim_fgets(linebuf, LBUFLEN, fd))
		    ಭಙ್ಗ;		// end of file
		ಸಾಲು = linebuf;
	    }

	    ಯದಿ (diffstyle == DIFF_NONE)
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
		ಯದಿ (SAFE_isdigit(*ಸಾಲು))
		    diffstyle = DIFF_ED;
		ಅನ್ಯಥಾ ಯದಿ ((STRNCMP(ಸಾಲು, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ಅನ್ಯಥಾ ಯದಿ ((STRNCMP(ಸಾಲು, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ಸಾಲು, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ಸಾಲು, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ಅನ್ಯಥಾ
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    ಅನುವರ್ತ;
	    }

	    ಯದಿ (diffstyle == DIFF_ED)
	    {
		ಯದಿ (!SAFE_isdigit(*ಸಾಲು))
		    ಅನುವರ್ತ;	// not the start of a diff block
		ಯದಿ (parse_diff_ed(ಸಾಲು, hunk) == FAIL)
		    ಅನುವರ್ತ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (diffstyle == DIFF_UNIFIED)
	    {
		ಯದಿ (STRNCMP(ಸಾಲು, "@@ ", 3)  != 0)
		    ಅನುವರ್ತ;	// not the start of a diff block
		ಯದಿ (parse_diff_unified(ಸಾಲು, hunk) == FAIL)
		    ಅನುವರ್ತ;
	    }
	    ಅನ್ಯಥಾ
	    {
		emsg(_(e_invalid_diff_format));
		ಭಙ್ಗ;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	ಯಾವತ್ (dp != ರಿಕ್ತ
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_ಎಣಿಕೆ[idx_orig])
	{
	    ಯದಿ (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	ಯದಿ (dp != ರಿಕ್ತ
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_ಎಣಿಕೆ[idx_orig]
		&& hunk->lnum_orig + hunk->ಎಣಿಕೆ_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    ಪ್ರತಿ (dpl = dp; dpl->df_next != ರಿಕ್ತ; dpl = dpl->df_next)
		ಯದಿ (hunk->lnum_orig + hunk->ಎಣಿಕೆ_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    ಭಙ್ಗ;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    ಯದಿ (off > 0)
	    {
		ಪ್ರತಿ (i = idx_orig; i < idx_new; ++i)
		    ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
		    {
			dp->df_lnum[i] -= off;
			dp->df_ಎಣಿಕೆ[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_ಎಣಿಕೆ[idx_new] = hunk->ಎಣಿಕೆ_new;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_ಎಣಿಕೆ[idx_new] = hunk->ಎಣಿಕೆ_new - off;
	    }
	    ಅನ್ಯಥಾ
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		ಪೂರ್ಣಾಙ್ಕ orig_size_in_dp = MIN(hunk->ಎಣಿಕೆ_orig,
			dp->df_lnum[idx_orig] +
			dp->df_ಎಣಿಕೆ[idx_orig] - hunk->lnum_orig);
		ಪೂರ್ಣಾಙ್ಕ size_diff = hunk->ಎಣಿಕೆ_new - orig_size_in_dp;
		dp->df_ಎಣಿಕೆ[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->ಎಣಿಕೆ_new
		    - (dp->df_lnum[idx_new] + dp->df_ಎಣಿಕೆ[idx_new]);
		ಯದಿ (off > 0)
		    dp->df_ಎಣಿಕೆ[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->ಎಣಿಕೆ_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_ಎಣಿಕೆ[idx_orig]);
	    ಯದಿ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		ಯದಿ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_ಎಣಿಕೆ[idx_new] += -off;
		}
		off = 0;
	    }
	    ಪ್ರತಿ (i = idx_orig; i < idx_new; ++i)
		ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
		    dp->df_ಎಣಿಕೆ[i] = dpl->df_lnum[i] + dpl->df_ಎಣಿಕೆ[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    ಯಾವತ್ (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    ಯದಿ (dp == ರಿಕ್ತ)
		ಗಚ್ಛ done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_ಎಣಿಕೆ[idx_orig] = hunk->ಎಣಿಕೆ_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_ಎಣಿಕೆ[idx_new] = hunk->ಎಣಿಕೆ_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    ಪ್ರತಿ (i = idx_orig + 1; i < idx_new; ++i)
		ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    ಯಾವತ್ (dp != ರಿಕ್ತ)
    {
	ಯದಿ (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    ಯದಿ (!dio->dio_internal)
	vim_free(hunk);

    ಯದಿ (fd != ರಿಕ್ತ)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    ಪೂರ್ಣಾಙ್ಕ		idx_orig,
    ಪೂರ್ಣಾಙ್ಕ		idx_new)
{
    long	off;

    ಯದಿ (dprev == ರಿಕ್ತ)
	off = 0;
    ಅನ್ಯಥಾ
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
    diff_T	*p, *next_p;

    ಪ್ರತಿ (p = tp->tp_first_diff; p != ರಿಕ್ತ; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = ರಿಕ್ತ;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_linematch(diff_T *dp)
{
    ಯದಿ (!(diff_flags & DIFF_LINEMATCH))
	ಪ್ರತ್ಯಾಗಮ 0;

    // are there more than three diff buffers?
    ಪೂರ್ಣಾಙ್ಕ tsize = 0;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    ಯದಿ (dp->df_ಎಣಿಕೆ[i] < 0)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    tsize += dp->df_ಎಣಿಕೆ[i];
	}
    }

    // avoid allocating a huge array because it will lag
    ಪ್ರತ್ಯಾಗಮ tsize <= linematch_ಸಾಲುಗಳು;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_max_diff_length(ಧ್ರುವ diff_T *dp)
{
    ಪೂರ್ಣಾಙ್ಕ maxlength = 0;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < DB_ಎಣಿಕೆ; k++)
    {
	ಯದಿ (curtab->tp_diffbuf[k] != ರಿಕ್ತ)
	{
	    ಯದಿ (dp->df_ಎಣಿಕೆ[k] > maxlength)
		maxlength = dp->df_ಎಣಿಕೆ[k];
	}
    }
    ಪ್ರತ್ಯಾಗಮ maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_ತುದಿ_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    ಪೂರ್ಣಾಙ್ಕ		fromidx,
    ಪೂರ್ಣಾಙ್ಕ		topline)
{
    diff_T	*topdiff = ರಿಕ್ತ;
    diff_T	*localtopdiff = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		topdiffchange = 0;

    ಪ್ರತಿ (topdiff = curtab->tp_first_diff; topdiff != ರಿಕ್ತ;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	ಯದಿ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	ಯದಿ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_ಎಣಿಕೆ[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    ಯದಿ ((*thistopdiff) == ರಿಕ್ತ)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	ಯದಿ (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_ಎಣಿಕೆ[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    ಯದಿ (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		ಭಙ್ಗ;
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
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	fromidx,
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	toidx,
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	from_topline,
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	from_topfill,
    ಪೂರ್ಣಾಙ್ಕ		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ರಿಕ್ತ;
    diff_T	*next_adjacent_blocks = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		virtual_ಸಾಲುಗಳು_passed = 0;
    ಪೂರ್ಣಾಙ್ಕ		curlinenum_to = 1;

    find_ತುದಿ_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    ಯಾವತ್ (curdif && (curdif->df_lnum[fromidx] + curdif->df_ಎಣಿಕೆ[fromidx])
							<= from_topline)
    {
	virtual_ಸಾಲುಗಳು_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    ಯದಿ (curdif != next_adjacent_blocks)
	virtual_ಸಾಲುಗಳು_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_ಸಾಲುಗಳು_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    ಯದಿ (virtual_ಸಾಲುಗಳು_passed < 0)
	virtual_ಸಾಲುಗಳು_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    ಯದಿ (thistopdiff != ರಿಕ್ತ) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    ಪೂರ್ಣಾಙ್ಕ virt_ಸಾಲುಗಳು_ಎಡ = virtual_ಸಾಲುಗಳು_passed;
    curdif = thistopdiff;
    ಯಾವತ್ (virt_ಸಾಲುಗಳು_ಎಡ > 0 && curdif != ರಿಕ್ತ && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_ಸಾಲುಗಳು_ಎಡ, curdif->df_ಎಣಿಕೆ[toidx]);
	virt_ಸಾಲುಗಳು_ಎಡ -= MIN(virt_ಸಾಲುಗಳು_ಎಡ, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    ಪೂರ್ಣಾಙ್ಕ max_virt_ಸಾಲುಗಳು = 0;
    ಪ್ರತಿ (diff_T *dp = thistopdiff; dp != ರಿಕ್ತ; dp = dp->df_next)
    {
	ಯದಿ (dp->df_lnum[toidx] + dp->df_ಎಣಿಕೆ[toidx] <= curlinenum_to)
	    max_virt_ಸಾಲುಗಳು += get_max_diff_length(dp);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_ಸಾಲುಗಳು += curlinenum_to - dp->df_lnum[toidx];
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_ಸಾಲುಗಳು - virtual_ಸಾಲುಗಳು_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_numbers[DB_ಎಣಿಕೆ];
    ಪೂರ್ಣಾಙ್ಕ		outputmap[DB_ಎಣಿಕೆ];
    size_t	ndiffs = 0;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
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
    ಪ್ರತಿ (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	ಯದಿ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < DB_ಎಣಿಕೆ; j++)
	    {
		ಯದಿ (curtab->tp_diffbuf[j] != ರಿಕ್ತ)
		{
		    dp_s->df_lnum[j] = ಸಾಲು_numbers[j];
		    dp_s->df_ಎಣಿಕೆ[j] = 0;
		}
	    }
	}
	ಪ್ರತಿ (size_t j = 0; j < ndiffs; j++)
	{
	    ಯದಿ (decisions[i] & (1 << j))
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
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_ಎಣಿಕೆ];
    ಧ್ರುವ mmfile_t	*diffbufs[DB_ಎಣಿಕೆ];
    ಪೂರ್ಣಾಙ್ಕ			diff_length[DB_ಎಣಿಕೆ];
    size_t		ndiffs = 0;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    ಯದಿ (dp->df_ಎಣಿಕೆ[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i] - 1);
	    }
	    ಅನ್ಯಥಾ
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ರಿಕ್ತ;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_ಎಣಿಕೆ[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    ಪೂರ್ಣಾಙ್ಕ *decisions = ರಿಕ್ತ;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    ಪ್ರತಿ (size_t i = 0; i < ndiffs; i++)
	ಮುಕ್ತ(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    ಮುಕ್ತ(decisions);
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
    ಪೂರ್ಣಾಙ್ಕ
diff_check_with_linestatus(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ *linestatus)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    ಪೂರ್ಣಾಙ್ಕ		maxcount;
    ಪೂರ್ಣಾಙ್ಕ		i;
    buf_T	*buf = wp->w_buffer;
    ಪೂರ್ಣಾಙ್ಕ		cmp;

    ಯದಿ (linestatus != ರಿಕ್ತ)
	*linestatus = 0;

    ಯದಿ (curtab->tp_diff_invalid)
	ex_diffupdate(ರಿಕ್ತ);		// update after a big change

    ಯದಿ (curtab->tp_first_diff == ರಿಕ್ತ || !wp->w_p_diff)	// no diffs at all
	ಪ್ರತ್ಯಾಗಮ 0;

    // safety check: "lnum" must be a buffer line
    ಯದಿ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಪ್ರತ್ಯಾಗಮ 0;

    idx = diff_buf_idx(buf);
    ಯದಿ (idx == DB_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    ಯದಿ (hasFoldingWin(wp, lnum, ರಿಕ್ತ, ರಿಕ್ತ, TRUE, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ರಿಕ್ತ;		// last block with df_lnum[idx] <= lnum
    ಪ್ರತಿ (dp = diff_scan_start(curtab, idx, lnum); dp != ರಿಕ್ತ; dp = dp->df_next)
    {
	ಯದಿ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	ಯದಿ (lnum <= dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ಭಙ್ಗ;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    ಯದಿ (dp == ರಿಕ್ತ || lnum < dp->df_lnum[idx])
	ಪ್ರತ್ಯಾಗಮ 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    ಯದಿ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_fill = 0;
    ಯಾವತ್ (lnum == dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	ಯದಿ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    ಅಂಕೆ_fill += maxcount - dp->df_ಎಣಿಕೆ[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	ಯದಿ (dp->df_next != ರಿಕ್ತ
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_ಎಣಿಕೆ[idx])
	    dp = dp->df_next;
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    ಯದಿ (lnum < dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
    {
	ಪೂರ್ಣಾಙ್ಕ	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	    ಯದಿ (i != idx && curtab->tp_diffbuf[i] != ರಿಕ್ತ)
	    {
		ಯದಿ (dp->df_ಎಣಿಕೆ[i] == 0)
		    zero = TRUE;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (dp->df_ಎಣಿಕೆ[i] != dp->df_ಎಣಿಕೆ[idx])
		    {
			ಯದಿ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_fill;
		    }
		    cmp = TRUE;
		}
	    }
	ಯದಿ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		ಯದಿ (i != idx && curtab->tp_diffbuf[i] != ರಿಕ್ತ
						      && dp->df_ಎಣಿಕೆ[i] != 0)
		    ಯದಿ (!diff_equal_entry(dp, idx, i))
		    {
			ಯದಿ (linestatus)
			    *linestatus = -1;
			ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	ಯದಿ (zero == FALSE)
	    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_fill;
	ಯದಿ (linestatus)
	    *linestatus = -2;
	ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_fill;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_equal_entry(diff_T *dp, ಪೂರ್ಣಾಙ್ಕ idx1, ಪೂರ್ಣಾಙ್ಕ idx2)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		cmp;

    ಯದಿ (dp->df_ಎಣಿಕೆ[idx1] != dp->df_ಎಣಿಕೆ[idx2])
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (diff_check_sanity(curtab, dp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (i = 0; i < dp->df_ಎಣಿಕೆ[idx1]; ++i)
    {
	ಸಾಲು = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	cmp = diff_cmp(ಸಾಲು, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(ಸಾಲು);
	ಯದಿ (cmp != 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_equal_char(char_u *p1, char_u *p2, ಪೂರ್ಣಾಙ್ಕ *len)
{
    ಪೂರ್ಣಾಙ್ಕ l  = (*mb_ptr2len)(p1);

    ಯದಿ (l != (*mb_ptr2len)(p2))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (l > 1)
    {
	ಯದಿ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	*len = l;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	*len = 1;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಙ್ಕ		l;

    ಯದಿ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	ಪ್ರತ್ಯಾಗಮ STRCMP(s1, s2);
    ಯದಿ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	ಪ್ರತ್ಯಾಗಮ MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    ಯಾವತ್ (*p1 != NUL && *p2 != NUL)
    {
	ಯದಿ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!diff_equal_char(p1, p2, &l))
		ಭಙ್ಗ;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    ಯದಿ (*p1 != NUL || *p2 != NUL)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    ಪೂರ್ಣಾಙ್ಕ
diff_check_fill(win_T *wp, linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		n;

    // be quick when there are no filler lines
    ಯದಿ (!(diff_flags & DIFF_FILLER))
	ಪ್ರತ್ಯಾಗಮ 0;
    n = diff_check_with_linestatus(wp, lnum, ರಿಕ್ತ);
    ಯದಿ (n <= 0)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    ಶೂನ್ಯ
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		fromidx;
    ಪೂರ್ಣಾಙ್ಕ		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    ಯದಿ (fromidx == DB_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;		// safety check

    ಯದಿ (curtab->tp_diff_invalid)
	ex_diffupdate(ರಿಕ್ತ);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ಯದಿ (lnum <= dp->df_lnum[fromidx] + dp->df_ಎಣಿಕೆ[fromidx])
	    ಭಙ್ಗ;
    ಯದಿ (dp == ರಿಕ್ತ)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
				       - (frombuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum);
    }
    ಅನ್ಯಥಾ
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	ಯದಿ (toidx == DB_ಎಣಿಕೆ)
	    ಪ್ರತ್ಯಾಗಮ;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	ಯದಿ (lnum >= dp->df_lnum[fromidx])
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
    ಯದಿ (towin->w_topline > towin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	towin->w_botfill = true;
    }
    ಯದಿ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_ಸಾಲು_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (ಶೂನ್ಯ)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_diffanchors(
    ಪೂರ್ಣಾಙ್ಕ		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_anchors)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ರಿಕ್ತ;
    ಯದಿ (check_only)
	bufwin = curwin;
    ಅನ್ಯಥಾ
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    ಯದಿ (bufwin->w_buffer == buf && bufwin->w_p_diff)
		ಭಙ್ಗ;
	ಯದಿ (bufwin == ರಿಕ್ತ && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತಿ (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	ಯದಿ (*dia == ',') // don't allow empty values
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(ರಿಕ್ತ, &dia, ADDR_ಸಾಲುಗಳು, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	ಯದಿ (dia == ರಿಕ್ತ) // error detected
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*dia != ',' && *dia != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1))
	{
	    emsg(_(e_invalid_range));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (anchors != ರಿಕ್ತ)
	    anchors[i] = lnum;

	ಯದಿ (*dia == ',')
	    dia++;
    }
    ಯದಿ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (ಅಂಕೆ_anchors != ರಿಕ್ತ)
	*ಅಂಕೆ_anchors = i;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    ಪೂರ್ಣಾಙ್ಕ
diffanchors_changed(ಪೂರ್ಣಾಙ್ಕ buflocal)
{
    ಪೂರ್ಣಾಙ್ಕ result = parse_diffanchors(TRUE, curbuf, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    ಯದಿ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < DB_ಎಣಿಕೆ; ++idx)
		    ಯದಿ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			ಭಙ್ಗ;
		    }
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    ಪೂರ್ಣಾಙ್ಕ
diffopt_changed(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		diff_context_new = 6;
    ಪೂರ್ಣಾಙ್ಕ		linematch_ಸಾಲುಗಳು_new = 0;
    ಪೂರ್ಣಾಙ್ಕ		diff_flags_new = 0;
    ಪೂರ್ಣಾಙ್ಕ		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    ಯಾವತ್ (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	ಯದಿ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    ಯದಿ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    ಯದಿ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_ಸಾಲುಗಳು_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	ಯದಿ (*p != ',' && *p != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    ಯದಿ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    ಯದಿ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
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

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    ಪೂರ್ಣಾಙ್ಕ
diffopt_horizontal(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    ಪೂರ್ಣಾಙ್ಕ
diffopt_hiddenoff(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    ಪೂರ್ಣಾಙ್ಕ
diffopt_closeoff(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    ಶೂನ್ಯ
diff_update_ಸಾಲು(linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    diff_T	*dp;

    ಯದಿ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	ಪ್ರತ್ಯಾಗಮ;

    idx = diff_buf_idx(curbuf);
    ಯದಿ (idx == DB_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ಯದಿ (lnum <= dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ಭಙ್ಗ;

    // clear the inline change cache as it's invalid
    ಯದಿ (dp != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    ಪೂರ್ಣಾಙ್ಕ *change_start,
    ಪೂರ್ಣಾಙ್ಕ *change_end)
{
    ಯದಿ (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    ಅನ್ಯಥಾ
	*change_start = change->dc_start[diffline->bufidx];
    ಯದಿ (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    ಅನ್ಯಥಾ
	*change_end = change->dc_end[diffline->bufidx];

    ಯದಿ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	ಯದಿ (i == diffline->bufidx)
	    ಅನುವರ್ತ;
	ಯದಿ (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		*startp,	// first char of the change
    ಪೂರ್ಣಾಙ್ಕ		*endp)		// last char of the change
{
    char_u	*ಸಾಲು_org;
    char_u	*ಸಾಲು_new;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		si_org, si_new;
    ಪೂರ್ಣಾಙ್ಕ		ei_org, ei_new;
    ಪೂರ್ಣಾಙ್ಕ		off;
    ಪೂರ್ಣಾಙ್ಕ		added = TRUE;
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಙ್ಕ		l;

    ಯದಿ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	ಸಾಲು_org = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	ಸಾಲು_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	ಯದಿ (ಸಾಲು_org == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    ಯದಿ (off >= dp->df_ಎಣಿಕೆ[i])
		ಅನುವರ್ತ;
	    added = FALSE;
	    ಯದಿ (diff_flags & DIFF_INLINE_NONE)
		ಭಙ್ಗ; // early terminate as we only care about the return value

	    ಸಾಲು_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    ಯಾವತ್ (ಸಾಲು_org[si_org] != NUL)
	    {
		ಯದಿ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(ಸಾಲು_org[si_org])
					      && VIM_ISWHITE(ಸಾಲು_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(ಸಾಲು_org[si_org])
					    || VIM_ISWHITE(ಸಾಲು_new[si_new]))))
		{
		    si_org = (ಪೂರ್ಣಾಙ್ಕ)(skipwhite(ಸಾಲು_org + si_org) - ಸಾಲು_org);
		    si_new = (ಪೂರ್ಣಾಙ್ಕ)(skipwhite(ಸಾಲು_new + si_new) - ಸಾಲು_new);
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (!diff_equal_char(ಸಾಲು_org + si_org, ಸಾಲು_new + si_new,
									   &l))
			ಭಙ್ಗ;
		    si_org += l;
		    si_new += l;
		}
	    }
	    ಯದಿ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(ಸಾಲು_org, ಸಾಲು_org + si_org);
		si_new -= (*mb_head_off)(ಸಾಲು_new, ಸಾಲು_new + si_new);
	    }
	    ಯದಿ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    ಯದಿ (ಸಾಲು_org[si_org] != NUL || ಸಾಲು_new[si_new] != NUL)
	    {
		ei_org = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು_org);
		ei_new = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು_new);
		ಯಾವತ್ (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    ಯದಿ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(ಸಾಲು_org[ei_org])
					      && VIM_ISWHITE(ಸಾಲು_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(ಸಾಲು_org[ei_org])
					    || VIM_ISWHITE(ಸಾಲು_new[ei_new]))))
		    {
			ಯಾವತ್ (ei_org >= *startp
					     && VIM_ISWHITE(ಸಾಲು_org[ei_org]))
			    --ei_org;
			ಯಾವತ್ (ei_new >= si_new
					     && VIM_ISWHITE(ಸಾಲು_new[ei_new]))
			    --ei_new;
		    }
		    ಅನ್ಯಥಾ
		    {
			p1 = ಸಾಲು_org + ei_org;
			p2 = ಸಾಲು_new + ei_new;
			p1 -= (*mb_head_off)(ಸಾಲು_org, p1);
			p2 -= (*mb_head_off)(ಸಾಲು_new, p2);
			ಯದಿ (!diff_equal_char(p1, p2, &l))
			    ಭಙ್ಗ;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		ಯದಿ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(ಸಾಲು_org);
    ಪ್ರತ್ಯಾಗಮ added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    long byte_start;
    long ಅಂಕೆ_bytes;
    ಪೂರ್ಣಾಙ್ಕ lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, ಪೂರ್ಣಾಙ್ಕ idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    ಪೂರ್ಣಾಙ್ಕ pass = 1;
    ಕುರು
    {
	ಪೂರ್ಣಾಙ್ಕ has_unmerged_gaps = FALSE;
	ಪೂರ್ಣಾಙ್ಕ has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	ಯಾವತ್ (dp!= ರಿಕ್ತ && dp->df_next != ರಿಕ್ತ)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    ಯದಿ (dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ಅನುವರ್ತ;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    ಯದಿ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ಅನುವರ್ತ;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1]);
	    ಯದಿ (gap <= 3)
	    {
		linenr_T max_df_ಎಣಿಕೆ = 0;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		    max_df_ಎಣಿಕೆ = MAX(max_df_ಎಣಿಕೆ, dp->df_ಎಣಿಕೆ[i] + dp->df_next->df_ಎಣಿಕೆ[i]);

		ಯದಿ (max_df_ಎಣಿಕೆ >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		    {
			dp->df_ಎಣಿಕೆ[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_ಎಣಿಕೆ[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    ಅನುವರ್ತ;
		}
		ಅನ್ಯಥಾ
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	ಯದಿ (!has_unmerged_gaps || !has_merged_gaps)
	    ಭಙ್ಗ;
    } ಯಾವತ್ (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, ಪೂರ್ಣಾಙ್ಕ idx1,
				  linenr_T start_lnum)
{
    ಪೂರ್ಣಾಙ್ಕ pass = 1;
    ಕುರು
    {
	diff_T *dp = dp_orig;

	ಯಾವತ್ (dp != ರಿಕ್ತ && dp->df_next != ರಿಕ್ತ)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    ಯದಿ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ಅನುವರ್ತ;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    ಯದಿ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ಅನುವರ್ತ;
	    }

	    // Calculate the gap between blocks
	    ಪೂರ್ಣಾಙ್ಕ gap_start = entry1->byte_start + entry1->ಅಂಕೆ_bytes;
	    ಪೂರ್ಣಾಙ್ಕ gap_end = entry2->byte_start;
	    ಪೂರ್ಣಾಙ್ಕ gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    ಯದಿ (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		ಅನುವರ್ತ;
	    }

	    // Get the text between the two blocks
	    char_u *ಸಾಲು = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = ಸಾಲು + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		ಪೂರ್ಣಾಙ್ಕ char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		ಯದಿ (char_class == 2)
		{
		    only_non_word = false;
		    ಭಙ್ಗ;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    ಯದಿ (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		{
		    ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
		    {
			// count bytes in the first block
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < dp->df_ಎಣಿಕೆ[i]; k++)
			{
			    ಪೂರ್ಣಾಙ್ಕ idx = dp->df_lnum[i] + k - 1;
			    ಯದಿ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].ಅಂಕೆ_bytes;
			}
			// count bytes in the next block
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < dp->df_next->df_ಎಣಿಕೆ[i]; k++)
			{
			    ಪೂರ್ಣಾಙ್ಕ idx = dp->df_next->df_lnum[i] + k - 1;
			    ಯದಿ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].ಅಂಕೆ_bytes;
			}
		    }
		}

		ಯದಿ (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
		    {
			ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
			{
			    dp->df_ಎಣಿಕೆ[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_ಎಣಿಕೆ[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    ಅನುವರ್ತ;
		}
	    }

	    dp = dp->df_next;
	}
    } ಯಾವತ್ (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_ಎಣಿಕೆ];
    garray_T	file1_str;
    garray_T	file2_str;
    ಪೂರ್ಣಾಙ್ಕ		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, ಪರಿಮಾಣ(ವರ್ಣ *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = ರಿಕ್ತ;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_ಎಣಿಕೆ]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, ಪರಿಮಾಣ(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	ga_init2(&linemap[i], ಪರಿಮಾಣ(linemap_entry_T), 128);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	    ಅನುವರ್ತ; // skip buffer that isn't loaded

	ಯದಿ (dp->df_ಎಣಿಕೆ[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ರಿಕ್ತ;
	    ಅನುವರ್ತ;
	}

	ಯದಿ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ off = 0; off < dp->df_ಎಣಿಕೆ[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    ಪೂರ್ಣಾಙ್ಕ in_keyword = FALSE;

	    // iwhiteeol support vars
	    ಪೂರ್ಣಾಙ್ಕ last_white = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ eol_ga_len = -1;
	    ಪೂರ್ಣಾಙ್ಕ eol_linemap_len = -1;
	    ಪೂರ್ಣಾಙ್ಕ eol_numlines = -1;

	    char_u *s;
	    ಪ್ರತಿ (s = curline; *s != NUL;)
	    {
		ಪೂರ್ಣಾಙ್ಕ new_in_keyword = FALSE;
		ಯದಿ (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		ಯದಿ (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ಯದಿ (VIM_ISWHITE(*s))
		{
		    ಯದಿ (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			ಅನುವರ್ತ;
		    }
		    ಅನ್ಯಥಾ ಯದಿ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ಯದಿ (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		ಪೂರ್ಣಾಙ್ಕ char_len = 1;
		ಯದಿ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		ಅನ್ಯಥಾ
		{
		    char_len = mb_ptr2len(s);

		    ಯದಿ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    ಯದಿ (diff_flags & DIFF_ICASE)
		    {
			ಪೂರ್ಣಾಙ್ಕ c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			ಪೂರ್ಣಾಙ್ಕ c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			ಪೂರ್ಣಾಙ್ಕ c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			ಯದಿ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    ಅನ್ಯಥಾ
			ga_concat_len(curstr, s, char_len);
		}

		ಯದಿ (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ಯದಿ (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.ಅಂಕೆ_bytes = char_len;
		    ಯದಿ (ga_grow(&linemap[i], 1) != OK)
			ಗಚ್ಛ done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		ಅನ್ಯಥಾ
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].ಅಂಕೆ_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    ಯದಿ (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    ಯದಿ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		ಯದಿ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    ಯದಿ (!(diff_flags & DIFF_IWHITEALL))
	    {
		// Add an empty line token mapped to the end-of-line in the
		// original file. This helps diff newline differences among
		// files, which will be visualized when using 'list' as the eol
		// listchar will be highlighted.
		ga_append(curstr, NL);
		numlines++;

		linemap_entry_T linemap_entry;
		linemap_entry.lineoff = off;
		linemap_entry.byte_start = s - curline;
		linemap_entry.ಅಂಕೆ_bytes = ಪರಿಮಾಣ(NL);
		ಯದಿ (ga_grow(&linemap[i], 1) != OK)
		    ಗಚ್ಛ done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	ಯದಿ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (ವರ್ಣ *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	ಅನ್ಯಥಾ
	{
	    dio.dio_orig.din_mmfile.ptr = (ವರ್ಣ *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	ಯದಿ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    ಪೂರ್ಣಾಙ್ಕ diff_status = diff_file_internal(&dio);
	    ಯದಿ (diff_status == FAIL)
		ಗಚ್ಛ done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    ಯದಿ (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    ಅನ್ಯಥಾ ಯದಿ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    ಪ್ರತಿ (; new_diff != ರಿಕ್ತ; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	{
	    ಯದಿ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		ಅನುವರ್ತ;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_ಎಣಿಕೆ[i];

	    ಯದಿ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    ಅನ್ಯಥಾ
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    ಯದಿ (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    ಅನ್ಯಥಾ ಯದಿ (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    ಅನ್ಯಥಾ
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].ಅಂಕೆ_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	ಯದಿ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    ಗಚ್ಛ done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, ಪರಿಮಾಣ(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    ಪೂರ್ಣಾಙ್ಕ
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		off;

    idx = diff_buf_idx(wp->w_buffer);
    ಯದಿ (idx == DB_ಎಣಿಕೆ)	// cannot happen
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ಯದಿ (lnum < dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ಭಙ್ಗ;
    ಯದಿ (dp == ರಿಕ್ತ || diff_check_sanity(curtab, dp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	ಪ್ರತ್ಯಾಗಮ FALSE;
    off = lnum - dp->df_lnum[idx];

    ಯದಿ (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	ಪೂರ್ಣಾಙ್ಕ	change_start = MAXCOL;	// first col of changed area
	ಪೂರ್ಣಾಙ್ಕ	change_end = -1;	// last col of changed area
	ಪೂರ್ಣಾಙ್ಕ	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->ಅಂಕೆ_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	ಪ್ರತ್ಯಾಗಮ ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    ಯದಿ (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_changes = 0;
    ಪೂರ್ಣಾಙ್ಕ change_idx = 0;
    diffline->changes = ರಿಕ್ತ;
    ಪ್ರತಿ (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	ಯದಿ (change->dc_end_lnum_off[idx] < off)
	    ಅನುವರ್ತ;
	ಯದಿ (change->dc_start_lnum_off[idx] > off)
	    ಭಙ್ಗ;
	ಯದಿ (diffline->changes == ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ added = FALSE;
    ಯದಿ (ಅಂಕೆ_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < DB_ಎಣಿಕೆ; i++)
	{
	    ಯದಿ (idx == i)
		ಅನುವರ್ತ;
	    ಯದಿ (curtab->tp_diffbuf[i] == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    ಯದಿ (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		ಭಙ್ಗ;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ added;
}

# ಯದಿ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    ಪೂರ್ಣಾಙ್ಕ
diff_infold(win_T *wp, linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		idx = -1;
    ಪೂರ್ಣಾಙ್ಕ		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    ಯದಿ (!wp->w_p_diff)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	ಅನ್ಯಥಾ ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    ಯದಿ (idx == -1 || !other)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (curtab->tp_diff_invalid)
	ex_diffupdate(ರಿಕ್ತ);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    ಯದಿ (curtab->tp_first_diff == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ರಿಕ್ತ;		// last block with df_lnum[idx] <= lnum
    ಪೂರ್ಣಾಙ್ಕ		result = TRUE;
    ಪ್ರತಿ (dp = diff_scan_start(curtab, idx, lnum); dp != ರಿಕ್ತ; dp = dp->df_next)
    {
	ಯದಿ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	ಯದಿ (dp->df_lnum[idx] - diff_context > lnum)
	    ಭಙ್ಗ;
	// If this change ends before the line we have a match.
	ಯದಿ (dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    ಭಙ್ಗ;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    ಪ್ರತ್ಯಾಗಮ result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    ಶೂನ್ಯ
nv_diffgetput(ಪೂರ್ಣಾಙ್ಕ put, long ಎಣಿಕೆ)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    ಯದಿ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
    ಯದಿ (ಎಣಿಕೆ == 0)
	ea.arg = (char_u *)"";
    ಅನ್ಯಥಾ
    {
	vim_snprintf((ವರ್ಣ *)buf, 30, "%ld", ಎಣಿಕೆ);
	ea.arg = buf;
    }
    ಯದಿ (put)
	ea.cmdidx = CMD_diffput;
    ಅನ್ಯಥಾ
	ea.cmdidx = CMD_diffget;
    ea.addr_ಎಣಿಕೆ = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ಯದಿ (dp == diff)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    ಶೂನ್ಯ
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    ಪೂರ್ಣಾಙ್ಕ		idx_cur;
    ಪೂರ್ಣಾಙ್ಕ		idx_other;
    ಪೂರ್ಣಾಙ್ಕ		idx_from;
    ಪೂರ್ಣಾಙ್ಕ		idx_to;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		start_skip, end_skip;
    ಪೂರ್ಣಾಙ್ಕ		new_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		buf_empty;
    ಪೂರ್ಣಾಙ್ಕ		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    ಯದಿ (idx_cur == DB_ಎಣಿಕೆ)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	ಪ್ರತಿ (idx_other = 0; idx_other < DB_ಎಣಿಕೆ; ++idx_other)
	    ಯದಿ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ರಿಕ್ತ)
	    {
		ಯದಿ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    ಭಙ್ಗ;
		found_not_ma = TRUE;
	    }
	ಯದಿ (idx_other == DB_ಎಣಿಕೆ)
	{
	    ಯದಿ (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    ಅನ್ಯಥಾ
		emsg(_(e_no_other_buffer_in_diff_mode));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// Check that there isn't a third buffer in the list
	ಪ್ರತಿ (i = idx_other + 1; i < DB_ಎಣಿಕೆ; ++i)
	    ಯದಿ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ರಿಕ್ತ
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		ಪ್ರತ್ಯಾಗಮ;
	    }
    }
    ಅನ್ಯಥಾ
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	ಯಾವತ್ (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	ಪ್ರತಿ (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	ಯದಿ (eap->arg + i == p)	    // digits only
	    i = atol((ವರ್ಣ *)eap->arg);
	ಅನ್ಯಥಾ
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    ಯದಿ (i < 0)
		ಪ್ರತ್ಯಾಗಮ;		// error message already given
	}
	buf = buflist_findnr(i);
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (buf == curbuf)
	    ಪ್ರತ್ಯಾಗಮ;		// nothing to do
	idx_other = diff_buf_idx(buf);
	ಯದಿ (idx_other == DB_ಎಣಿಕೆ)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	ಪೂರ್ಣಾಙ್ಕ linestatus = 0;
	ಯದಿ (eap->line1 == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	ಅನ್ಯಥಾ ಯದಿ (eap->line1 > 0)
	    --eap->line1;
    }

    ಯದಿ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    ಅನ್ಯಥಾ
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	ಯದಿ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    ಗಚ್ಛ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    ಯದಿ (!curbuf->b_changed)
    {
	change_warning(0);
	ಯದಿ (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    ಗಚ್ಛ theend;
	}
    }

    dprev = ರಿಕ್ತ;
    ಪ್ರತಿ (dp = curtab->tp_first_diff; dp != ರಿಕ್ತ; )
    {
	ಯದಿ (!eap->addr_ಎಣಿಕೆ)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    ಯಾವತ್ (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_ಎಣಿಕೆ[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	ಯದಿ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    ಭಙ್ಗ;	// past the range that was specified

	dfree = ರಿಕ್ತ;
	lnum = dp->df_lnum[idx_to];
	ಎಣಿಕೆ = dp->df_ಎಣಿಕೆ[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + ಎಣಿಕೆ + (ಎಣಿಕೆ == 0 && BUFEMPTY() ? 1 : 0);

	ಯದಿ (dp->df_lnum[idx_cur] + dp->df_ಎಣಿಕೆ[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		ಯದಿ (start_skip > 0)
		{
		    // range starts below start of current diff block
		    ಯದಿ (start_skip > ಎಣಿಕೆ)
		    {
			lnum += ಎಣಿಕೆ;
			ಎಣಿಕೆ = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಎಣಿಕೆ -= start_skip;
			lnum += start_skip;
		    }
		}
		ಅನ್ಯಥಾ
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_ಎಣಿಕೆ[idx_cur] - 1
							 - (eap->line2 + off);
		ಯದಿ (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    ಯದಿ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_ಎಣಿಕೆ[idx_cur] - start_skip - end_skip;
			ಯದಿ (ಎಣಿಕೆ > i)
			    ಎಣಿಕೆ = i;
		    }
		    ಅನ್ಯಥಾ			// :diffget
		    {
			ಎಣಿಕೆ -= end_skip;
			end_skip = dp->df_ಎಣಿಕೆ[idx_from] - start_skip - ಎಣಿಕೆ;
			ಯದಿ (end_skip < 0)
			    end_skip = 0;
		    }
		}
		ಅನ್ಯಥಾ
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1;
		ಯದಿ (ml_delete(lnum) == OK)
		    --added;
	    }
	    ಪ್ರತಿ (i = 0; i < dp->df_ಎಣಿಕೆ[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		ಯದಿ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ಭಙ್ಗ;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    ಯದಿ (buf_empty && curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_ಎಣಿಕೆ = dp->df_ಎಣಿಕೆ[idx_to] + added;
	    dp->df_ಎಣಿಕೆ[idx_to] = new_ಎಣಿಕೆ;

	    ಯದಿ (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
		    ಯದಿ (curtab->tp_diffbuf[i] != ರಿಕ್ತ && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			ಭಙ್ಗ;
		ಯದಿ (i == DB_ಎಣಿಕೆ)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    ಯದಿ (dprev == ರಿಕ್ತ)
			curtab->tp_first_diff = dp;
		    ಅನ್ಯಥಾ
			dprev->df_next = dp;
		}
	    }

	    ಯದಿ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + ಎಣಿಕೆ - 1, (long)MAXLNUM, (long)added);
		ಯದಿ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    ಯದಿ (curwin->w_cursor.lnum >= lnum + ಎಣಿಕೆ)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_ಸಾಲುಗಳು(lnum, 0, lnum + ಎಣಿಕೆ, (long)added);

	    ಯದಿ (dfree != ರಿಕ್ತ)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    ಯದಿ (added != 0 && !valid_diff(dp))
		ಭಙ್ಗ;

	    ಯದಿ (dfree == ರಿಕ್ತ)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_ಎಣಿಕೆ[idx_to] = new_ಎಣಿಕೆ;

	    // When changing the current buffer, keep track of line numbers
	    ಯದಿ (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	ಯದಿ (dfree == ರಿಕ್ತ)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    ಯದಿ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	ಯದಿ (ಕೀಲಿ_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    ಯದಿ (diff_need_update)
	ex_diffupdate(ರಿಕ್ತ);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_ಸಾಲು_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    ಯದಿ (curtab->tp_first_diff == ರಿಕ್ತ)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    ಯದಿ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    ಯದಿ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    ಅನ್ಯಥಾ
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
diff_fold_update(diff_T *dp, ಪೂರ್ಣಾಙ್ಕ skip_idx)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಪ್ರತಿ (i = 0; i < DB_ಎಣಿಕೆ; ++i)
	    ಯದಿ (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ಯದಿ (diff_buf_idx_tp(buf, tp) != DB_ಎಣಿಕೆ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    ಪೂರ್ಣಾಙ್ಕ
diff_move_to(ಪೂರ್ಣಾಙ್ಕ dir, long ಎಣಿಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    ಯದಿ (idx == DB_ಎಣಿಕೆ || curtab->tp_first_diff == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (curtab->tp_diff_invalid)
	ex_diffupdate(ರಿಕ್ತ);		// update after a big change

    ಯದಿ (curtab->tp_first_diff == ರಿಕ್ತ)		// no diffs today
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯಾವತ್ (--ಎಣಿಕೆ >= 0)
    {
	// Check if already before first diff.
	ಯದಿ (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    ಭಙ್ಗ;

	ಪ್ರತಿ (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    ಯದಿ (dp == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಯದಿ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == ರಿಕ್ತ
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		ಭಙ್ಗ;
	    }
	}
    }

    // don't end up past the end of the file
    ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // When the cursor didn't move at all we fail.
    ಯದಿ (lnum == curwin->w_cursor.lnum)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		idx1;
    ಪೂರ್ಣಾಙ್ಕ		idx2;
    diff_T	*dp;
    ಪೂರ್ಣಾಙ್ಕ		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    ಯದಿ (idx1 == DB_ಎಣಿಕೆ || idx2 == DB_ಎಣಿಕೆ || curtab->tp_first_diff == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ lnum1;

    ಯದಿ (curtab->tp_diff_invalid)
	ex_diffupdate(ರಿಕ್ತ);		// update after a big change

    ಯದಿ (curtab->tp_first_diff == ರಿಕ್ತ)		// no diffs today
	ಪ್ರತ್ಯಾಗಮ lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	ಯದಿ (dp->df_lnum[idx1] > lnum1)
	    ಪ್ರತ್ಯಾಗಮ lnum1 - baseline;
	ಯದಿ ((dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    ಯದಿ (baseline > dp->df_ಎಣಿಕೆ[idx2])
		baseline = dp->df_ಎಣಿಕೆ[idx2];

	    ಪ್ರತ್ಯಾಗಮ dp->df_lnum[idx2] + baseline;
	}
	ಯದಿ (    (dp->df_lnum[idx1] == lnum1)
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
	    ಪ್ರತ್ಯಾಗಮ curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_ಎಣಿಕೆ[idx1])
				   - (dp->df_lnum[idx2] + dp->df_ಎಣಿಕೆ[idx2]);
    }

    // If we get here then the cursor is after the last diff
    ಪ್ರತ್ಯಾಗಮ lnum1 - baseline;
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
    ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    ಯದಿ (idx == DB_ಎಣಿಕೆ)		// safety check
	ಪ್ರತ್ಯಾಗಮ (linenr_T)0;

    ಯದಿ (curtab->tp_diff_invalid)
	ex_diffupdate(ರಿಕ್ತ);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ಯದಿ (lnum <= dp->df_lnum[idx] + dp->df_ಎಣಿಕೆ[idx])
	    ಭಙ್ಗ;

    // When after the last change, compute relative to the last line number.
    ಯದಿ (dp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					- (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    ಯದಿ (i == DB_ಎಣಿಕೆ)			// safety check
	ಪ್ರತ್ಯಾಗಮ (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    ಯದಿ (n > dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i])
	n = dp->df_lnum[i] + dp->df_ಎಣಿಕೆ[i];
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_diff_ed(
	char_u	    *ಸಾಲು,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    ಪೂರ್ಣಾಙ್ಕ	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = ಸಾಲು;
    f1 = getdigits(&p);
    ಯದಿ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    ಅನ್ಯಥಾ
	l1 = f1;
    ಯದಿ (*p != 'a' && *p != 'c' && *p != 'd')
	ಪ್ರತ್ಯಾಗಮ FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    ಯದಿ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    ಅನ್ಯಥಾ
	l2 = f2;
    ಯದಿ (l1 < f1 || l2 < f2)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->ಎಣಿಕೆ_orig = 0;
    }
    ಅನ್ಯಥಾ
    {
	hunk->lnum_orig = f1;
	hunk->ಎಣಿಕೆ_orig = l1 - f1 + 1;
    }
    ಯದಿ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->ಎಣಿಕೆ_new = 0;
    }
    ಅನ್ಯಥಾ
    {
	hunk->lnum_new = f2;
	hunk->ಎಣಿಕೆ_new = l2 - f2 + 1;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_diff_unified(
	char_u	    *ಸಾಲು,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = ಸಾಲು;
    ಯದಿ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	ಯದಿ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	ಅನ್ಯಥಾ
	    oldcount = 1;
	ಯದಿ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    ಯದಿ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    ಅನ್ಯಥಾ
		newcount = 1;
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FAIL;	// invalid diff format

	ಯದಿ (oldcount == 0)
	    oldline += 1;
	ಯದಿ (newcount == 0)
	    newline += 1;
	ಯದಿ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->ಎಣಿಕೆ_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->ಎಣಿಕೆ_new = newcount;

	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
xdiff_out_indices(
	long start_a,
	long ಎಣಿಕೆ_a,
	long start_b,
	long ಎಣಿಕೆ_b,
	ಶೂನ್ಯ *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    p->lnum_orig  = start_a + 1;
    p->ಎಣಿಕೆ_orig = ಎಣಿಕೆ_a;
    p->lnum_new   = start_b + 1;
    p->ಎಣಿಕೆ_new  = ಎಣಿಕೆ_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
xdiff_out_unified(
	ಶೂನ್ಯ *priv,
	mmbuffer_t *mb,
	ಪೂರ್ಣಾಙ್ಕ nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    ಪ್ರತ್ಯಾಗಮ 0;
}

#endif	// FEAT_DIFF

#ಯದಿ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    ಶೂನ್ಯ
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    ಶೂನ್ಯ
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ಸ್ಥಿರ linenr_T	prev_lnum = 0;
    ಸ್ಥಿರ varnumber_T	changedtick = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		fnum = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prev_diff_flags = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		change_start = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		change_end = 0;
    ಸ್ಥಿರ hlf_T	hlID = (hlf_T)0;
    ಪೂರ್ಣಾಙ್ಕ			cache_results = TRUE;
    ಪೂರ್ಣಾಙ್ಕ			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    ಯದಿ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    ಯದಿ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	ಪೂರ್ಣಾಙ್ಕ linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	ಯದಿ (linestatus < 0)
	{
	    ಯದಿ (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		ಯದಿ (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		ಅನ್ಯಥಾ
		{
		    hlID = HLF_CHD;	// changed line
		    ಯದಿ (diffline.ಅಂಕೆ_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    ಅನ್ಯಥಾ
		hlID = HLF_ADD;	// added line
	}
	ಅನ್ಯಥಾ
	    hlID = (hlf_T)0;

	ಯದಿ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    ಯದಿ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1; // ignore type error in {col}
	ಯದಿ (cache_results)
	{
	    ಯದಿ (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    ಅನ್ಯಥಾ
		hlID = HLF_CHD;			// changed line
	}
	ಅನ್ಯಥಾ
	{
	    hlID = HLF_CHD;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < diffline.ಅಂಕೆ_changes; i++)
	    {
		ಪೂರ್ಣಾಙ್ಕ added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		ಯದಿ (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    ಭಙ್ಗ;
		}
		ಯದಿ (col < change_start)
		    // the remaining changes are past this column and not relevant
		    ಭಙ್ಗ;
	    }
	}
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = hlID == (hlf_T)0 ? 0 : (ಪೂರ್ಣಾಙ್ಕ)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_diff_optarg(
    typval_T	    *opts,
    ಪೂರ್ಣಾಙ್ಕ		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    ಪೂರ್ಣಾಙ್ಕ		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    ಯದಿ (algo != ರಿಕ್ತ)
    {
	ಯದಿ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    ಯದಿ (output_fmt != ರಿಕ್ತ)
    {
	ಯದಿ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    *diff_ctxlen = dict_get_ಸಂಖ್ಯೆ_def(d, "context", 0);
    ಯದಿ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    ಯದಿ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    ಯದಿ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    ಯದಿ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    ಯದಿ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    ಯದಿ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    ಯದಿ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_to_diffin(list_T *l, diffin_T *din, ಪೂರ್ಣಾಙ್ಕ icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	str = tv_get_string(&li->li_tv);
	ಯದಿ (icase)
	{
	    str = strlow_save(str);
	    ಯದಿ (str == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	ಯದಿ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (ವರ್ಣ *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ಸ್ಥಿರ dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    ಯದಿ (hunk_dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    dict_add_ಸಂಖ್ಯೆ(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_ಸಂಖ್ಯೆ(hunk_dict, "from_count", hunk->ಎಣಿಕೆ_orig);
    dict_add_ಸಂಖ್ಯೆ(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_ಸಂಖ್ಯೆ(hunk_dict, "to_count", hunk->ಎಣಿಕೆ_new);

    ಪ್ರತ್ಯಾಗಮ hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    ಶೂನ್ಯ
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    ಯದಿ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, ಪರಿಮಾಣ(ವರ್ಣ *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    ಪೂರ್ಣಾಙ್ಕ		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	ಯದಿ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    ಪೂರ್ಣಾಙ್ಕ	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    ಯದಿ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    ಪೂರ್ಣಾಙ್ಕ diff_status = diff_file(&dio);

    // restore 'diffexpr'
    ಯದಿ (restore_diffexpr)
	*p_dex = cc;

    ಯದಿ (diff_status == FAIL)
	ಗಚ್ಛ done;

    ಪೂರ್ಣಾಙ್ಕ		hunk_idx = 0;
    dict_T	*hunk_dict;

    ಯದಿ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	ಯದಿ (rettv_list_alloc(rettv) != OK)
	    ಗಚ್ಛ done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ರಿಕ್ತ;
	ಯಾವತ್ (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    ಯದಿ (hunk_dict == ರಿಕ್ತ)
		ಗಚ್ಛ done;

	    ಯದಿ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    ಅನ್ಯಥಾ
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    ಯದಿ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    ಅನ್ಯಥಾ
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
