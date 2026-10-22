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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xdiff/xdiff.h"

#ਜੇ defined(FEAT_DIFF)

ਸਥਿਰ ਪੂਰਨ diff_busy = FALSE;	    // using diff structs, don't change them
ਸਥਿਰ ਪੂਰਨ diff_need_update = FALSE; // ex_diffupdate needs to be called

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
# define DIFF_ਅੰਦਰੂਨੀ	0x200	// use internal xdiff algorithm
# define DIFF_CLOSE_OFF	0x400	// diffoff when closing window
# define DIFF_FOLLOWWRAP	0x800	// follow the wrap option
# define DIFF_LINEMATCH  0x1000  // match most similar lines within diff
# define DIFF_INLINE_NONE    0x2000  // no inline highlight
# define DIFF_INLINE_SIMPLE  0x4000  // inline highlight with simple algorithm
# define DIFF_INLINE_CHAR    0x8000  // inline highlight with character diff
# define DIFF_INLINE_ਸ਼ਬਦ    0x10000 // inline highlight with word diff
# define DIFF_ANCHOR	0x20000	// use 'diffanchors' to anchor the diff
# define ALL_WHITE_DIFF (DIFF_IWHITE | DIFF_IWHITEALL | DIFF_IWHITEEOL)
# define ALL_INLINE (DIFF_INLINE_NONE | DIFF_INLINE_SIMPLE | DIFF_INLINE_CHAR | DIFF_INLINE_ਸ਼ਬਦ)
# define ALL_INLINE_DIFF (DIFF_INLINE_CHAR | DIFF_INLINE_ਸ਼ਬਦ)
ਸਥਿਰ ਪੂਰਨ	diff_flags = DIFF_ਅੰਦਰੂਨੀ | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ਸਥਿਰ ਲੰਮਾ diff_algorithm = XDF_INDENT_HEURISTIC;
ਸਥਿਰ ਪੂਰਨ diff_ਸ਼ਬਦ_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_ਮਾਪ (1024L * 1024 * 1023)

ਸਥਿਰ ਪੂਰਨ diff_a_ਚੱਲਦਾ = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# ਜੇ defined(MSWIN)
ਸਥਿਰ ਪੂਰਨ diff_bin_ਚੱਲਦਾ = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    linenr_T lnum_orig;
    ਲੰਮਾ     ਗਿਣਤੀ_orig;
    linenr_T lnum_new;
    ਲੰਮਾ     ਗਿਣਤੀ_new;
} diffhunk_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    ਪੂਰਨ		    dio_ਅੰਦਰੂਨੀ; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    ਪੂਰਨ		    dio_ctxlen;   // unified diff context length
} diffio_T;

ਸਥਿਰ ਪੂਰਨ diff_buf_idx(buf_T *buf);
ਸਥਿਰ ਪੂਰਨ diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ਸਥਿਰ ਖਾਲੀ diff_mark_adjust_tp(tabpage_T *tp, ਪੂਰਨ idx, linenr_T line1, linenr_T line2, ਲੰਮਾ amount, ਲੰਮਾ amount_after);
ਸਥਿਰ ਖਾਲੀ diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ਸਥਿਰ ਪੂਰਨ diff_check_sanity(tabpage_T *tp, diff_T *dp);
ਸਥਿਰ ਪੂਰਨ check_external_diff(diffio_T *diffio);
ਸਥਿਰ ਪੂਰਨ diff_file(diffio_T *diffio);
ਸਥਿਰ ਪੂਰਨ diff_equal_ਇੰਦਰਾਜ(diff_T *dp, ਪੂਰਨ idx1, ਪੂਰਨ idx2);
ਸਥਿਰ ਪੂਰਨ diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ਸਥਿਰ ਖਾਲੀ diff_fold_update(diff_T *dp, ਪੂਰਨ skip_idx);
# endif
ਸਥਿਰ ਖਾਲੀ diff_read(ਪੂਰਨ idx_orig, ਪੂਰਨ idx_new, diffio_T *dio);
ਸਥਿਰ ਖਾਲੀ diff_copy_ਇੰਦਰਾਜ(diff_T *dprev, diff_T *dp, ਪੂਰਨ idx_orig, ਪੂਰਨ idx_new);
ਸਥਿਰ diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ਸਥਿਰ ਪੂਰਨ parse_diff_ed(char_u *ਸਤਰ, diffhunk_T *hunk);
ਸਥਿਰ ਪੂਰਨ parse_diff_unified(char_u *ਸਤਰ, diffhunk_T *hunk);
ਸਥਿਰ ਪੂਰਨ xdiff_ਬਾਹਰਲਾ_indices(ਲੰਮਾ ਸ਼ੁਰੂ_a, ਲੰਮਾ ਗਿਣਤੀ_a, ਲੰਮਾ ਸ਼ੁਰੂ_b, ਲੰਮਾ ਗਿਣਤੀ_b, ਖਾਲੀ *priv);
ਸਥਿਰ ਪੂਰਨ xdiff_ਬਾਹਰਲਾ_unified(ਖਾਲੀ *priv, mmbuffer_t *mb, ਪੂਰਨ nbuf);
ਸਥਿਰ ਪੂਰਨ parse_diffanchors(ਪੂਰਨ check_only, buf_T *buf, linenr_T *anchors, ਪੂਰਨ *ਅੰਕ_anchors);

# define FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(tp, dp) \
    ਲਈ ((dp) = (tp)->tp_ਪਹਿਲਾ_diff; (dp) != ਨਿਰਾ; (dp) = (dp)->df_ਅਗਲਾ)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ਸਥਿਰ diff_T	    *diff_finger_dp = ਨਿਰਾ;
ਸਥਿਰ tabpage_T    *diff_finger_tp = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	    diff_finger_idx = -1;
ਸਥਿਰ linenr_T	    diff_finger_lnum = 0;

    ਸਥਿਰ ਖਾਲੀ
clear_diffblock(diff_T *dp)
{
    ਜੇ (dp == diff_finger_dp)
	diff_finger_dp = ਨਿਰਾ;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ਸਥਿਰ diff_T *
diff_ਜਾਂਚ_ਸ਼ੁਰੂ(tabpage_T *tp, ਪੂਰਨ idx, linenr_T lnum)
{
    ਜੇ (diff_finger_dp != ਨਿਰਾ
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	ਵਾਪਸ diff_finger_dp;
    ਵਾਪਸ tp->tp_ਪਹਿਲਾ_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_ਜਾਂਚ_remember(tabpage_T *tp, ਪੂਰਨ idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ਨਿਰਾ ? dp : tp->tp_ਪਹਿਲਾ_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    ਖਾਲੀ
diff_buf_delete(buf_T *buf)
{
    ਪੂਰਨ		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ਜੇ (i != DB_ਗਿਣਤੀ)
	{
	    tp->tp_diffbuf[i] = ਨਿਰਾ;
	    tp->tp_diff_invalid = TRUE;
	    ਜੇ (tp == curtab)
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
    ਖਾਲੀ
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    ਪੂਰਨ		i;

    ਜੇ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_ਬਫਰ == win->w_ਬਫਰ && wp->w_p_diff)
		ਰੋਕੋ;
	ਜੇ (wp == ਨਿਰਾ)
	{
	    i = diff_buf_idx(win->w_ਬਫਰ);
	    ਜੇ (i != DB_ਗਿਣਤੀ)
	    {
		curtab->tp_diffbuf[i] = ਨਿਰਾ;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	diff_buf_ਜੋੜੋ(win->w_ਬਫਰ);
}

/*
 * Add a buffer to make diffs for.
 * Call this when a new buffer is being edited in the current window where
 * 'diff' is set.
 * Marks the current buffer as being part of the diff and requiring updating.
 * This must be done before any autocmd, because a command may use info
 * about the screen contents.
 */
    ਖਾਲੀ
diff_buf_ਜੋੜੋ(buf_T *buf)
{
    ਪੂਰਨ		i;

    ਜੇ (diff_buf_idx(buf) != DB_ਗਿਣਤੀ)
	ਵਾਪਸ;		// It's already there.

    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	ਜੇ (curtab->tp_diffbuf[i] == ਨਿਰਾ)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    ਵਾਪਸ;
	}

    semsg(_(e_cannot_diff_ਹੋਰ_than_nr_buffers), DB_ਗਿਣਤੀ);
}

/*
 * Remove all buffers to make diffs for.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_buf_clear(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
	{
	    curtab->tp_diffbuf[i] = ਨਿਰਾ;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_buf_idx(buf_T *buf)
{
    ਪੂਰਨ		idx;

    ਲਈ (idx = 0; idx < DB_ਗਿਣਤੀ; ++idx)
	ਜੇ (curtab->tp_diffbuf[idx] == buf)
	    ਰੋਕੋ;
    ਵਾਪਸ idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    ਪੂਰਨ		idx;

    ਲਈ (idx = 0; idx < DB_ਗਿਣਤੀ; ++idx)
	ਜੇ (tp->tp_diffbuf[idx] == buf)
	    ਰੋਕੋ;
    ਵਾਪਸ idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    ਖਾਲੀ
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    ਪੂਰਨ		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ਜੇ (i != DB_ਗਿਣਤੀ)
	{
	    tp->tp_diff_invalid = TRUE;
	    ਜੇ (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    ਖਾਲੀ
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after)
{
    ਪੂਰਨ		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	ਜੇ (idx != DB_ਗਿਣਤੀ)
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
    ਸਥਿਰ ਖਾਲੀ
diff_mark_adjust_tp(
    tabpage_T	*tp,
    ਪੂਰਨ		idx,
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    ਪੂਰਨ		i;
    ਪੂਰਨ		inserted, deleted;
    ਪੂਰਨ		n, off;
    linenr_T	ਆਖਰੀ;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    ਪੂਰਨ		check_unchanged;

    diff_finger_dp = ਨਿਰਾ;		// may shift df_lnum, invalidate the finger

    ਜੇ (diff_ਅੰਦਰੂਨੀ())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    ਜੇ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ਨਿਰਾ;
    dp = tp->tp_ਪਹਿਲਾ_diff;
    ਲਈ (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	ਜੇ ((dp == ਨਿਰਾ || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ਨਿਰਾ
		    || dprev->df_lnum[idx] + dprev->df_ਗਿਣਤੀ[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    ਜੇ (dnext == ਨਿਰਾ)
		ਵਾਪਸ;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_ਗਿਣਤੀ[idx] = inserted;
	    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
		ਜੇ (tp->tp_diffbuf[i] != ਨਿਰਾ && i != idx)
		{
		    ਜੇ (dprev == ਨਿਰਾ)
			dnext->df_lnum[i] = line1;
		    ਨਹੀਂਤਾਂ
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_ਗਿਣਤੀ[i])
			    - (dprev->df_lnum[idx] + dprev->df_ਗਿਣਤੀ[idx]);
		    dnext->df_ਗਿਣਤੀ[i] = deleted;
		}
	}

	// if at end of the list, quit
	ਜੇ (dp == ਨਿਰਾ)
	    ਰੋਕੋ;

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
	ਆਖਰੀ = dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx] - 1;

	// 1. change completely above line1: nothing to do
	ਜੇ (ਆਖਰੀ >= line1 - 1)
	{
	    ਜੇ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		ਜੇ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_ਅਗਲਾ;
		ਜਾਰੀ;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    ਜੇ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		ਜੇ (amount_after == 0)
		    ਰੋਕੋ;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		ਜੇ (deleted > 0)
		{
		    off = 0;
		    ਜੇ (dp->df_lnum[idx] >= line1)
		    {
			ਜੇ (ਆਖਰੀ <= line2)
			{
			    // 4. delete all lines of diff
			    ਜੇ (dp->df_ਅਗਲਾ != ਨਿਰਾ
				    && dp->df_ਅਗਲਾ->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_ਅਗਲਾ->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_ਗਿਣਤੀ[idx];
				lnum_deleted = dp->df_ਅਗਲਾ->df_lnum[idx];
			    }
			    ਨਹੀਂਤਾਂ
				n = deleted - dp->df_ਗਿਣਤੀ[idx];
			    dp->df_ਗਿਣਤੀ[idx] = 0;
			}
			ਨਹੀਂਤਾਂ
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_ਗਿਣਤੀ[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (ਆਖਰੀ < line2)
			{
			    // 2. delete at end of diff
			    dp->df_ਗਿਣਤੀ[idx] -= ਆਖਰੀ - lnum_deleted + 1;
			    ਜੇ (dp->df_ਅਗਲਾ != ਨਿਰਾ
				    && dp->df_ਅਗਲਾ->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_ਅਗਲਾ->df_lnum[idx] - 1 - ਆਖਰੀ;
				deleted -= dp->df_ਅਗਲਾ->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_ਅਗਲਾ->df_lnum[idx];
			    }
			    ਨਹੀਂਤਾਂ
				n = line2 - ਆਖਰੀ;
			    check_unchanged = TRUE;
			}
			ਨਹੀਂਤਾਂ
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_ਗਿਣਤੀ[idx] -= deleted;
			}
		    }

		    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
			ਜੇ (tp->tp_diffbuf[i] != ਨਿਰਾ && i != idx)
			{
			    ਜੇ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    ਨਹੀਂਤਾਂ
				dp->df_lnum[i] = 1;
			    dp->df_ਗਿਣਤੀ[i] += n;
			}
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_ਗਿਣਤੀ[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    ਨਹੀਂਤਾਂ
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		ਜੇ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	ਜੇ (dprev != ਨਿਰਾ && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_ਗਿਣਤੀ[idx]
							== dp->df_lnum[idx])
	{
	    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
		ਜੇ (tp->tp_diffbuf[i] != ਨਿਰਾ)
		    dprev->df_ਗਿਣਤੀ[i] += dp->df_ਗਿਣਤੀ[i];
	    dprev->df_ਅਗਲਾ = dp->df_ਅਗਲਾ;
	    clear_diffblock(dp);
	    dp = dprev->df_ਅਗਲਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_ਅਗਲਾ;
	}
    }

    dprev = ਨਿਰਾ;
    dp = tp->tp_ਪਹਿਲਾ_diff;
    ਜਦੋਂ (dp != ਨਿਰਾ)
    {
	// All counts are zero, remove this entry.
	ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	    ਜੇ (tp->tp_diffbuf[i] != ਨਿਰਾ && dp->df_ਗਿਣਤੀ[i] != 0)
		ਰੋਕੋ;
	ਜੇ (i == DB_ਗਿਣਤੀ)
	{
	    dnext = dp->df_ਅਗਲਾ;
	    clear_diffblock(dp);
	    dp = dnext;
	    ਜੇ (dprev == ਨਿਰਾ)
		tp->tp_ਪਹਿਲਾ_diff = dnext;
	    ਨਹੀਂਤਾਂ
		dprev->df_ਅਗਲਾ = dnext;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_ਅਗਲਾ;
	}

    }

    ਜੇ (tp == curtab)
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
    ਸਥਿਰ diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    ਜੇ (dnew == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    diff_finger_dp = ਨਿਰਾ;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_ਅਗਲਾ = dp;
    ਜੇ (dprev == ਨਿਰਾ)
	tp->tp_ਪਹਿਲਾ_diff = dnew;
    ਨਹੀਂਤਾਂ
	dprev->df_ਅਗਲਾ = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, ਆਕਾਰ(diffline_change_T), 20);
    ਵਾਪਸ dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    ਪੂਰਨ		i_org;
    ਪੂਰਨ		i_new;
    ਪੂਰਨ		off_org, off_new;
    char_u	*ਸਤਰ_org;
    ਪੂਰਨ		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    ਲਈ (i_org = 0; i_org < DB_ਗਿਣਤੀ; ++i_org)
	ਜੇ (tp->tp_diffbuf[i_org] != ਨਿਰਾ)
	    ਰੋਕੋ;
    ਜੇ (i_org == DB_ਗਿਣਤੀ)	// safety check
	ਵਾਪਸ;

    ਜੇ (diff_check_sanity(tp, dp) == FAIL)
	ਵਾਪਸ;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    ਲਈ (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	ਜਦੋਂ (dp->df_ਗਿਣਤੀ[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    ਜੇ (dir == BACKWARD)
		off_org = dp->df_ਗਿਣਤੀ[i_org] - 1;
	    ਸਤਰ_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    ਜੇ (ਸਤਰ_org == ਨਿਰਾ)
		ਵਾਪਸ;
	    ਲਈ (i_new = i_org + 1; i_new < DB_ਗਿਣਤੀ; ++i_new)
	    {
		ਜੇ (tp->tp_diffbuf[i_new] == ਨਿਰਾ)
		    ਜਾਰੀ;
		ਜੇ (dir == BACKWARD)
		    off_new = dp->df_ਗਿਣਤੀ[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		ਜੇ (off_new < 0 || off_new >= dp->df_ਗਿਣਤੀ[i_new])
		    ਰੋਕੋ;
		ਜੇ (diff_cmp(ਸਤਰ_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    ਰੋਕੋ;
	    }
	    vim_free(ਸਤਰ_org);

	    // Stop when a line isn't equal in all diff buffers.
	    ਜੇ (i_new != DB_ਗਿਣਤੀ)
		ਰੋਕੋ;

	    // Line matched in all buffers, remove it from the diff.
	    ਲਈ (i_new = i_org; i_new < DB_ਗਿਣਤੀ; ++i_new)
		ਜੇ (tp->tp_diffbuf[i_new] != ਨਿਰਾ)
		{
		    ਜੇ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_ਗਿਣਤੀ[i_new];
		}
	}
	ਜੇ (dir == BACKWARD)
	    ਰੋਕੋ;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	ਜੇ (tp->tp_diffbuf[i] != ਨਿਰਾ)
	    ਜੇ (dp->df_lnum[i] + dp->df_ਗਿਣਤੀ[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    ਖਾਲੀ
diff_redraw(
    ਪੂਰਨ		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ਨਿਰਾ;
    ਪੂਰਨ		ਵਰਤਿਆ_max_fill_other = FALSE;
    ਪੂਰਨ		ਵਰਤਿਆ_max_fill_curwin = FALSE;
    ਪੂਰਨ		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	ਜੇ (!wp->w_p_diff || !buf_valid(wp->w_ਬਫਰ))
	    ਜਾਰੀ;

	redraw_win_later(wp, UPD_SOME_VALID);
	ਜੇ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	ਜੇ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	ਜੇ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    ਜੇ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    ਨਹੀਂਤਾਂ ਜੇ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		ਜੇ (wp == curwin)
		    ਵਰਤਿਆ_max_fill_curwin = TRUE;
		ਨਹੀਂਤਾਂ ਜੇ (wp_other != ਨਿਰਾ)
		    ਵਰਤਿਆ_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    ਜੇ (wp_other != ਨਿਰਾ && curwin->w_p_scb)
    {
	ਜੇ (ਵਰਤਿਆ_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_ਸੈੱਟ_topline(wp_other, curwin);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਰਤਿਆ_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_ਸੈੱਟ_topline(curwin, wp_other);
    }
}

    ਸਥਿਰ ਖਾਲੀ
clear_diffin(diffin_T *din)
{
    ਜੇ (din->din_fname == ਨਿਰਾ)
	VIM_CLEAR(din->din_mmfile.ptr);
    ਨਹੀਂਤਾਂ
	mch_remove(din->din_fname);
}

    ਸਥਿਰ ਖਾਲੀ
clear_diffout(diffout_T *dout)
{
    ਜੇ (dout->dout_fname == ਨਿਰਾ)
	ga_clear_ਲੜੀਆਂ(&dout->dout_ga);
    ਨਹੀਂਤਾਂ
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_write_ਬਫਰ(buf_T *buf, diffin_T *din, linenr_T ਸ਼ੁਰੂ, linenr_T ਸਿਰਾ)
{
    linenr_T	lnum;
    char_u	*s;
    ਲੰਮਾ	len = 0;
    char_u	*ptr;

    ਜੇ (ਸਿਰਾ < 0)
	ਸਿਰਾ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (buf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ || ਸਿਰਾ < ਸ਼ੁਰੂ)
    {
	din->din_mmfile.ptr = ਨਿਰਾ;
	din->din_mmfile.ਮਾਪ = 0;
	ਵਾਪਸ OK;
    }

    // xdiff requires one big block of memory with all the text.
    ਲਈ (lnum = ਸ਼ੁਰੂ; lnum <= ਸਿਰਾ; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    ਜੇ (ptr == ਨਿਰਾ)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	ਵਾਪਸ FAIL;
    }
    din->din_mmfile.ptr = (ਅੱਖਰ *)ptr;
    din->din_mmfile.ਮਾਪ = len;

    len = 0;
    ਲਈ (lnum = ਸ਼ੁਰੂ; lnum <= ਸਿਰਾ; ++lnum)
    {
	ਲਈ (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    ਜੇ (diff_flags & DIFF_ICASE)
	    {
		ਪੂਰਨ c;
		ਪੂਰਨ	orig_len;
		ਪੂਰਨ	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		ਜੇ (*s == NL)
		    c = NUL;
		ਨਹੀਂਤਾਂ
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		ਜੇ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		ਨਹੀਂਤਾਂ
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    ਜੇ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    ਵਾਪਸ OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_write(buf_T *buf, diffin_T *din, linenr_T ਸ਼ੁਰੂ, linenr_T ਸਿਰਾ)
{
    ਪੂਰਨ		r;
    ਪੂਰਨ		save_ml_flags;
    char_u	*save_ff;
    ਪੂਰਨ		save_cmod_flags;

    ਜੇ (din->din_fname == ਨਿਰਾ)
	ਵਾਪਸ diff_write_ਬਫਰ(buf, din, ਸ਼ੁਰੂ, ਸਿਰਾ);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    ਜੇ (ਚੌਖਟੇ_locked())
	ਵਾਪਸ FAIL;

    ਜੇ (ਸਿਰਾ < 0)
	ਸਿਰਾ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    ਜੇ (ਸਿਰਾ < ਸ਼ੁਰੂ)
    {
	// The line range specifies a completely empty file.
	ਸਿਰਾ = ਸ਼ੁਰੂ;
	buf->b_ml.ml_flags |= ML_ਖਾਲੀ_ਥਾਵਾਂ;
    }
    r = buf_write(buf, din->din_fname, ਨਿਰਾ,
			ਸ਼ੁਰੂ, ਸਿਰਾ,
			ਨਿਰਾ, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_ਖਾਲੀ_ਥਾਵਾਂ) | (save_ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ);
    ਵਾਪਸ r;
}

    ਸਥਿਰ ਪੂਰਨ
lnum_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    ਜੇ (lnum1 < lnum2)
	ਵਾਪਸ -1;
    ਜੇ (lnum1 > lnum2)
	ਵਾਪਸ 1;
    ਵਾਪਸ 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_try_update(
	diffio_T    *dio,
	ਪੂਰਨ	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    ਪੂਰਨ		idx_new;

    ਜੇ (dio->dio_ਅੰਦਰੂਨੀ)
    {
	ga_init2(&dio->dio_diff.dout_ga, ਆਕਾਰ(ਅੱਖਰ *), 1000);
    }
    ਨਹੀਂਤਾਂ
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	ਜੇ (dio->dio_orig.din_fname == ਨਿਰਾ
		|| dio->dio_new.din_fname == ਨਿਰਾ
		|| dio->dio_diff.dout_fname == ਨਿਰਾ)
	    ਜਾਓ theend;
    }

    // Check external diff is actually working.
    ਜੇ (!dio->dio_ਅੰਦਰੂਨੀ && check_external_diff(dio) == FAIL)
	ਜਾਓ theend;

    // :diffupdate!
    ਜੇ (eap != ਨਿਰਾ && eap->forceit)
	ਲਈ (idx_new = idx_orig; idx_new < DB_ਗਿਣਤੀ; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ਜੇ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    ਪੂਰਨ ਅੰਕ_anchors = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    linenr_T anchors[DB_ਗਿਣਤੀ][MAX_DIFF_ANCHORS];
    CLEAR_ਖੇਤਰ(anchors);
    ਜੇ (diff_flags & DIFF_ANCHOR)
    {
	ਲਈ (ਪੂਰਨ idx = 0; idx < DB_ਗਿਣਤੀ; idx++)
	{
	    ਜੇ (curtab->tp_diffbuf[idx] == ਨਿਰਾ)
		ਜਾਰੀ;
	    ਪੂਰਨ buf_ਅੰਕ_anchors = 0;
	    ਜੇ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_ਅੰਕ_anchors) != OK)
	    {
		emsg(_(e_failed_ਮੰਜ਼ਿਲ_ਲੱਭੋ_all_diff_anchors));
		ਅੰਕ_anchors = 0;
		CLEAR_ਖੇਤਰ(anchors);
		ਰੋਕੋ;
	    }
	    ਜੇ (buf_ਅੰਕ_anchors < ਅੰਕ_anchors)
		ਅੰਕ_anchors = buf_ਅੰਕ_anchors;

	    ਜੇ (buf_ਅੰਕ_anchors > 0)
		qsort((ਖਾਲੀ *)anchors[idx],
			(size_t)buf_ਅੰਕ_anchors,
			ਆਕਾਰ(linenr_T),
			lnum_compare);
	}
    }
    ਜੇ (ਅੰਕ_anchors == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	ਅੰਕ_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    ਲਈ (ਪੂਰਨ anchor_i = 0; anchor_i <= ਅੰਕ_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ਨਿਰਾ;
	ਜੇ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_ਪਹਿਲਾ_diff;
	    curtab->tp_ਪਹਿਲਾ_diff = ਨਿਰਾ;
	}
	linenr_T lnum_ਸ਼ੁਰੂ = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_ਸਿਰਾ = (anchor_i == ਅੰਕ_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	ਜੇ (diff_write(buf, &dio->dio_orig, lnum_ਸ਼ੁਰੂ, lnum_ਸਿਰਾ) == FAIL)
	{
	    ਜੇ (orig_diff != ਨਿਰਾ)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_ਪਹਿਲਾ_diff = orig_diff;
		diff_clear(curtab);
	    }
	    ਜਾਓ theend;
	}

	// Make a difference between the first buffer and every other.
	ਲਈ (idx_new = idx_orig + 1; idx_new < DB_ਗਿਣਤੀ; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ)
		ਜਾਰੀ; // skip buffer that isn't loaded

	    lnum_ਸ਼ੁਰੂ = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_ਸਿਰਾ = anchor_i == ਅੰਕ_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    ਜੇ (diff_write(buf, &dio->dio_new, lnum_ਸ਼ੁਰੂ, lnum_ਸਿਰਾ) == FAIL)
		ਜਾਰੀ;
	    ਜੇ (diff_file(dio) == FAIL)
		ਜਾਰੀ;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	ਜੇ (anchor_i != 0)
	{
	    diff_finger_dp = ਨਿਰਾ;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    ਲਈ (diff_T *dp = curtab->tp_ਪਹਿਲਾ_diff; dp != ਨਿਰਾ; dp = dp->df_ਅਗਲਾ)
	    {
		ਲਈ (ਪੂਰਨ idx = 0; idx < DB_ਗਿਣਤੀ; idx++)
		{
		    ਜੇ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    ਜੇ (orig_diff != ਨਿਰਾ)
	    {
		diff_T *ਆਖਰੀ_diff = orig_diff;
		ਜਦੋਂ (ਆਖਰੀ_diff->df_ਅਗਲਾ != ਨਿਰਾ)
		    ਆਖਰੀ_diff = ਆਖਰੀ_diff->df_ਅਗਲਾ;
		ਆਖਰੀ_diff->df_ਅਗਲਾ = curtab->tp_ਪਹਿਲਾ_diff;
		curtab->tp_ਪਹਿਲਾ_diff = orig_diff;
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
    ਪੂਰਨ
diff_ਅੰਦਰੂਨੀ(ਖਾਲੀ)
{
    ਵਾਪਸ (diff_flags & DIFF_ਅੰਦਰੂਨੀ) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_ਅੰਦਰੂਨੀ_failed(ਖਾਲੀ)
{
    ਪੂਰਨ idx;

    // Only need to do something when there is another buffer.
    ਲਈ (idx = 0; idx < DB_ਗਿਣਤੀ; ++idx)
	ਜੇ (curtab->tp_diffbuf[idx] != ਨਿਰਾ
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    ਖਾਲੀ
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    ਪੂਰਨ		idx_orig;
    ਪੂਰਨ		idx_new;
    diffio_T	diffio;
    ਪੂਰਨ		had_diffs = curtab->tp_ਪਹਿਲਾ_diff != ਨਿਰਾ;

    ਜੇ (diff_busy)
    {
	diff_need_update = TRUE;
	ਵਾਪਸ;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    ਲਈ (idx_orig = 0; idx_orig < DB_ਗਿਣਤੀ; ++idx_orig)
	ਜੇ (curtab->tp_diffbuf[idx_orig] != ਨਿਰਾ)
	    ਰੋਕੋ;
    ਜੇ (idx_orig == DB_ਗਿਣਤੀ)
	ਜਾਓ theend;

    // Only need to do something when there is another buffer.
    ਲਈ (idx_new = idx_orig + 1; idx_new < DB_ਗਿਣਤੀ; ++idx_new)
	ਜੇ (curtab->tp_diffbuf[idx_new] != ਨਿਰਾ)
	    ਰੋਕੋ;
    ਜੇ (idx_new == DB_ਗਿਣਤੀ)
	ਜਾਓ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_ਖੇਤਰ(diffio);
    diffio.dio_ਅੰਦਰੂਨੀ = diff_ਅੰਦਰੂਨੀ() && !diff_ਅੰਦਰੂਨੀ_failed();

    diff_try_update(&diffio, idx_orig, eap);
    ਜੇ (diffio.dio_ਅੰਦਰੂਨੀ && diff_ਅੰਦਰੂਨੀ_failed())
    {
	// Internal diff failed, use external diff instead.
	CLEAR_ਖੇਤਰ(diffio);
	diff_try_update(&diffio, idx_orig, eap);
    }

    // force updating cursor position on screen
    curwin->w_valid_cursor.lnum = 0;

theend:
    // A redraw is needed if there were diffs and they were cleared, or there
    // are diffs now, which means they got updated.
    ਜੇ (had_diffs || curtab->tp_ਪਹਿਲਾ_diff != ਨਿਰਾ)
    {
	diff_redraw(TRUE);
	ਖਿੜਕੀ_layout_ਤਾਲਾ();
	apply_autocmds(EVENT_DIFFUPDATED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਖਿੜਕੀ_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ਸਥਿਰ ਪੂਰਨ
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    ਪੂਰਨ		ok;
    ਪੂਰਨ		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    ਲਈ (;;)
    {
	ok = FALSE;
	fd = mch_fopen((ਅੱਖਰ *)diffio->dio_orig.din_fname, "w");
	ਜੇ (fd == ਨਿਰਾ)
	    io_error = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((ਅੱਖਰ *)diffio->dio_new.din_fname, "w");
	    ਜੇ (fd == ਨਿਰਾ)
		io_error = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ਨਿਰਾ;
		ਜੇ (diff_file(diffio) == OK)
		    fd = mch_fopen((ਅੱਖਰ *)diffio->dio_diff.dout_fname, "r");
		ਜੇ (fd == ਨਿਰਾ)
		    io_error = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    char_u	linebuf[LBUFLEN];

		    ਲਈ (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			ਜੇ (vim_fgets(linebuf, LBUFLEN, fd))
			    ਰੋਕੋ;
			ਜੇ (STRNCMP(linebuf, "1c1", 3) == 0
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
	ਜੇ (*p_dex != NUL)
	    ਰੋਕੋ;
# endif

# ਜੇ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	ਜੇ (ok && diff_a_ਚੱਲਦਾ == MAYBE && diff_bin_ਚੱਲਦਾ == MAYBE)
	{
	    diff_a_ਚੱਲਦਾ = TRUE;
	    diff_bin_ਚੱਲਦਾ = TRUE;
	    ਜਾਰੀ;
	}
	ਜੇ (!ok && diff_a_ਚੱਲਦਾ == TRUE && diff_bin_ਚੱਲਦਾ == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_ਚੱਲਦਾ = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	ਜੇ (diff_a_ਚੱਲਦਾ != MAYBE)
	    ਰੋਕੋ;
	diff_a_ਚੱਲਦਾ = ok;

	// If "-a" works break here, otherwise retry without "-a".
	ਜੇ (ok)
	    ਰੋਕੋ;
    }
    ਜੇ (!ok)
    {
	ਜੇ (io_error)
	    emsg(_(e_cannot_read_or_write_ਆਰਜ਼ੀ_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_ਚੱਲਦਾ = MAYBE;
# ਜੇ defined(MSWIN)
	diff_bin_ਚੱਲਦਾ = MAYBE;
# endif
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Invoke the xdiff function.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_file_ਅੰਦਰੂਨੀ(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_ਖੇਤਰ(param);
    CLEAR_ਖੇਤਰ(emit_cfg);
    CLEAR_ਖੇਤਰ(emit_cb);

    param.flags = diff_algorithm;

    ਜੇ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    ਜੇ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    ਜੇ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    ਜੇ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_ਸਤਰਾਂ;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    ਜੇ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_ਬਾਹਰਲਾ_indices;
    ਨਹੀਂਤਾਂ
	emit_cb.ਬਾਹਰਲਾ_ਸਤਰ = xdiff_ਬਾਹਰਲਾ_unified;
    ਜੇ (diffio->dio_orig.din_mmfile.ਮਾਪ > MAX_XDIFF_ਮਾਪ ||
	diffio->dio_new.din_mmfile.ਮਾਪ > MAX_XDIFF_ਮਾਪ)
    {
	emsg(_(e_problem_creating_ਅੰਦਰੂਨੀ_diff));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_ਅੰਦਰੂਨੀ_diff));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ਸਥਿਰ ਪੂਰਨ
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    ਜੇ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ
# endif
    // Use xdiff for generating the diff.
    ਜੇ (dio->dio_ਅੰਦਰੂਨੀ)
	ਵਾਪਸ diff_file_ਅੰਦਰੂਨੀ(dio);

    ਜੇ (check_restricted())
	ਵਾਪਸ FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    ਜੇ (cmd == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    ਜੇ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((ਅੱਖਰ *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_ਚੱਲਦਾ == FALSE ? "" : "-a ",
# ਜੇ defined(MSWIN)
	    diff_bin_ਚੱਲਦਾ == TRUE ? "--binary " : "",
# ਨਹੀਂਤਾਂ
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (ਪੂਰਨ)len, p_srr, tmp_diff);
    ਖੰਡ_autocmds();	// avoid ShellCmdPost stuff
    (ਖਾਲੀ)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    ਵਾਪਸ OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    ਖਾਲੀ
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = ਨਿਰਾ;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ਨਿਰਾ;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ਨਿਰਾ;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ਨਿਰਾ;
    ਪੂਰਨ		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ਨਿਰਾ;

# ifdef FEAT_BROWSE
    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ਨਿਰਾ, ਨਿਰਾ,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ਨਿਰਾ);
	ਜੇ (browseFile == ਨਿਰਾ)
	    ਵਾਪਸ;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    ਜੇ (tmp_orig == ਨਿਰਾ || tmp_new == ਨਿਰਾ)
	ਜਾਓ theend;

    // Write the current buffer to "tmp_orig".
    ਜੇ (buf_write(curbuf, tmp_orig, ਨਿਰਾ,
		(linenr_T)1, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
				     ਨਿਰਾ, FALSE, FALSE, FALSE, TRUE) == FAIL)
	ਜਾਓ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = ਭਰਿਆ_Name_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ਨਿਰਾ ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    ਜੇ (esc_name == ਨਿਰਾ)
	ਜਾਓ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    ਜੇ (buf == ਨਿਰਾ)
	ਜਾਓ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    ਜੇ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((ਅੱਖਰ *)dirbuf) != 0)
	dirbuf[0] = NUL;
    ਨਹੀਂਤਾਂ
    {
#  ifdef TEMPDIRNAMES
	ਜੇ (vim_tempdir != ਨਿਰਾ)
	    vim_ignored = mch_chdir((ਅੱਖਰ *)vim_tempdir);
	ਨਹੀਂਤਾਂ
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    ਜੇ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ਨਿਰਾ ? fullname :
#  endif
		eap->arg, tmp_new);
    ਨਹੀਂਤਾਂ
# endif
    {
	ਜੇ (check_restricted())
	    ਜਾਓ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((ਅੱਖਰ *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	ਖੰਡ_autocmds();	// Avoid ShellCmdPost stuff
	(ਖਾਲੀ)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    ਜੇ (dirbuf[0] != NUL)
    {
	ਜੇ (mch_chdir((ਅੱਖਰ *)dirbuf) != 0)
	    emsg(_(e_cannot_go_ਪਿੱਠ_ਮੰਜ਼ਿਲ_ਪਿਛਲਾ_directory));
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
    ਜੇ (mch_stat((ਅੱਖਰ *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (curbuf->b_fname != ਨਿਰਾ)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    ਜੇ (newname != ਨਿਰਾ)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	ਜੇ (win_ਵੰਡੋ(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_ਵੰਡੋ;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    ਜੇ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		ਜੇ (newname != ਨਿਰਾ)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    ਜੇ (au_has_ਸਮੂਹ((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    ਜੇ (tmp_orig != ਨਿਰਾ)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    ਜੇ (tmp_new != ਨਿਰਾ)
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
    ਖਾਲੀ
ex_diffsplit(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    bufref_T	old_curbuf;

    ਸੈੱਟ_bufref(&old_curbuf, curbuf);
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    // Need to compute w_fraction when no redraw happened yet.
    validate_cursor();
    ਸੈੱਟ_fraction(curwin);

    // don't use a new tab page, each tab page has its own diffs
    cmdmod.cmod_tab = 0;

    ਜੇ (win_ਵੰਡੋ(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	ਵਾਪਸ;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_ਵੰਡੋ;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    ਜੇ (curwin == old_curwin)		// split didn't work
	ਵਾਪਸ;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    ਜੇ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	ਜੇ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_ਸਤਰ(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_ਮੰਜ਼ਿਲ_fraction(curwin, curwin->w_ਉਚਾਈ);
}

/*
 * Set options to show diffs for the current window.
 */
    ਖਾਲੀ
ex_diffthis(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_diff_option(win_T *wp, ਪੂਰਨ ਮੁੱਲ)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_ਬਫਰ;
    ++curbuf_ਤਾਲਾ;
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"diff", (ਲੰਮਾ)ਮੁੱਲ, ਨਿਰਾ, OPT_LOCAL);
    --curbuf_ਤਾਲਾ;
    curwin = old_curwin;
    curbuf = curwin->w_ਬਫਰ;
}

/*
 * Set options in window "wp" for diff mode.
 */
    ਖਾਲੀ
diff_win_options(
    win_T	*wp,
    ਪੂਰਨ		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    ਜੇ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    ਜੇ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    ਜੇ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	ਜੇ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    ਜੇ (!wp->w_p_diff)
    {
	ਜੇ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    ਜੇ (!wp->w_p_diff)
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
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);
# endif
    ਜੇ (vim_strchr(p_sbo, 'h') == ਨਿਰਾ)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    ਸੈੱਟ_diff_option(wp, TRUE);

    ਜੇ (addbuf)
	diff_buf_ਜੋੜੋ(wp->w_ਬਫਰ);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    ਖਾਲੀ
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    ਪੂਰਨ		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    ਸੈੱਟ_diff_option(wp, FALSE);

	    ਜੇ (wp->w_p_diff_saved)
	    {

		ਜੇ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		ਜੇ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		ਜੇ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    ਜੇ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		ਜੇ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		ਜੇ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		ਜੇ (wp->w_p_fen)
		    wp->w_p_fen = foldmethodIsManual(wp) ? FALSE
							 : wp->w_p_fen_save;

		foldUpdateAll(wp);
# endif
	    }
	    // remove filler lines
	    wp->w_topfill = 0;

	    // make sure topline is not halfway a fold and cursor is
	    // invalidated
	    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);

	    // Note: 'sbo' is not restored, it's a global option.
	    diff_buf_adjust(wp);
	}
	diffwin |= wp->w_p_diff;
    }

    // Also remove hidden buffers from the list.
    ਜੇ (eap->forceit)
	diff_buf_clear();

    ਜੇ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    ਜੇ (!diffwin && vim_strchr(p_sbo, 'h') != ਨਿਰਾ)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_read(
    ਪੂਰਨ		idx_orig,	// idx of original file
    ਪੂਰਨ		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ਨਿਰਾ;
    ਪੂਰਨ		ਸਤਰ_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ਨਿਰਾ;
    diff_T	*dp = curtab->tp_ਪਹਿਲਾ_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*ਸਤਰ;
    ਲੰਮਾ	off;
    ਪੂਰਨ		i;
    ਪੂਰਨ		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ਨਿਰਾ;	    // init to avoid gcc warning

    ਗਿਣਤੀ {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    ਜੇ (dout->dout_fname == ਨਿਰਾ)
    {
	diffstyle = DIFF_UNIFIED;
    }
    ਨਹੀਂਤਾਂ
    {
	fd = mch_fopen((ਅੱਖਰ *)dout->dout_fname, "r");
	ਜੇ (fd == ਨਿਰਾ)
	{
	    emsg(_(e_cannot_read_diff_output));
	    ਵਾਪਸ;
	}
    }

    ਜੇ (!dio->dio_ਅੰਦਰੂਨੀ)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	ਜੇ (hunk == ਨਿਰਾ)
	{
	    ਜੇ (fd != ਨਿਰਾ)
		fclose(fd);
	    ਵਾਪਸ;
	}
    }

    ਲਈ (;;)
    {
	ਜੇ (dio->dio_ਅੰਦਰੂਨੀ)
	{
	    ਜੇ (ਸਤਰ_hunk_idx >= dout->dout_ga.ga_len)
		ਰੋਕੋ;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_ਅੰਕੜੇ)[ਸਤਰ_hunk_idx++];
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fd == ਨਿਰਾ)
	    {
		ਜੇ (ਸਤਰ_hunk_idx >= dout->dout_ga.ga_len)
		    ਰੋਕੋ;	    // did last line
		ਸਤਰ = ((char_u **)dout->dout_ga.ga_ਅੰਕੜੇ)[ਸਤਰ_hunk_idx++];
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (vim_fgets(linebuf, LBUFLEN, fd))
		    ਰੋਕੋ;		// end of file
		ਸਤਰ = linebuf;
	    }

	    ਜੇ (diffstyle == DIFF_NONE)
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
		ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(*ਸਤਰ))
		    diffstyle = DIFF_ED;
		ਨਹੀਂਤਾਂ ਜੇ ((STRNCMP(ਸਤਰ, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ਨਹੀਂਤਾਂ ਜੇ ((STRNCMP(ਸਤਰ, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ਸਤਰ, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ਸਤਰ, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ਨਹੀਂਤਾਂ
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    ਜਾਰੀ;
	    }

	    ਜੇ (diffstyle == DIFF_ED)
	    {
		ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(*ਸਤਰ))
		    ਜਾਰੀ;	// not the start of a diff block
		ਜੇ (parse_diff_ed(ਸਤਰ, hunk) == FAIL)
		    ਜਾਰੀ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (diffstyle == DIFF_UNIFIED)
	    {
		ਜੇ (STRNCMP(ਸਤਰ, "@@ ", 3)  != 0)
		    ਜਾਰੀ;	// not the start of a diff block
		ਜੇ (parse_diff_unified(ਸਤਰ, hunk) == FAIL)
		    ਜਾਰੀ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		emsg(_(e_invalid_diff_format));
		ਰੋਕੋ;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	ਜਦੋਂ (dp != ਨਿਰਾ
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_ਗਿਣਤੀ[idx_orig])
	{
	    ਜੇ (notset)
		diff_copy_ਇੰਦਰਾਜ(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_ਅਗਲਾ;
	    notset = TRUE;
	}

	ਜੇ (dp != ਨਿਰਾ
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_ਗਿਣਤੀ[idx_orig]
		&& hunk->lnum_orig + hunk->ਗਿਣਤੀ_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    ਲਈ (dpl = dp; dpl->df_ਅਗਲਾ != ਨਿਰਾ; dpl = dpl->df_ਅਗਲਾ)
		ਜੇ (hunk->lnum_orig + hunk->ਗਿਣਤੀ_orig
					     < dpl->df_ਅਗਲਾ->df_lnum[idx_orig])
		    ਰੋਕੋ;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    ਜੇ (off > 0)
	    {
		ਲਈ (i = idx_orig; i < idx_new; ++i)
		    ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
		    {
			dp->df_lnum[i] -= off;
			dp->df_ਗਿਣਤੀ[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_ਗਿਣਤੀ[idx_new] = hunk->ਗਿਣਤੀ_new;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_ਗਿਣਤੀ[idx_new] = hunk->ਗਿਣਤੀ_new - off;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		ਪੂਰਨ orig_ਮਾਪ_ਵਿੱਚ_dp = MIN(hunk->ਗਿਣਤੀ_orig,
			dp->df_lnum[idx_orig] +
			dp->df_ਗਿਣਤੀ[idx_orig] - hunk->lnum_orig);
		ਪੂਰਨ ਮਾਪ_diff = hunk->ਗਿਣਤੀ_new - orig_ਮਾਪ_ਵਿੱਚ_dp;
		dp->df_ਗਿਣਤੀ[idx_new] += ਮਾਪ_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->ਗਿਣਤੀ_new
		    - (dp->df_lnum[idx_new] + dp->df_ਗਿਣਤੀ[idx_new]);
		ਜੇ (off > 0)
		    dp->df_ਗਿਣਤੀ[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->ਗਿਣਤੀ_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_ਗਿਣਤੀ[idx_orig]);
	    ਜੇ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		ਜੇ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_ਗਿਣਤੀ[idx_new] += -off;
		}
		off = 0;
	    }
	    ਲਈ (i = idx_orig; i < idx_new; ++i)
		ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
		    dp->df_ਗਿਣਤੀ[i] = dpl->df_lnum[i] + dpl->df_ਗਿਣਤੀ[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_ਅਗਲਾ;
	    dp->df_ਅਗਲਾ = dpl->df_ਅਗਲਾ;
	    ਜਦੋਂ (dn != dp->df_ਅਗਲਾ)
	    {
		dpl = dn->df_ਅਗਲਾ;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    ਜੇ (dp == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_ਗਿਣਤੀ[idx_orig] = hunk->ਗਿਣਤੀ_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_ਗਿਣਤੀ[idx_new] = hunk->ਗਿਣਤੀ_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    ਲਈ (i = idx_orig + 1; i < idx_new; ++i)
		ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
		    diff_copy_ਇੰਦਰਾਜ(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    ਜਦੋਂ (dp != ਨਿਰਾ)
    {
	ਜੇ (notset)
	    diff_copy_ਇੰਦਰਾਜ(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_ਅਗਲਾ;
	notset = TRUE;
    }

ਪੂਰਾ:
    ਜੇ (!dio->dio_ਅੰਦਰੂਨੀ)
	vim_free(hunk);

    ਜੇ (fd != ਨਿਰਾ)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ਸਥਿਰ ਖਾਲੀ
diff_copy_ਇੰਦਰਾਜ(
    diff_T	*dprev,
    diff_T	*dp,
    ਪੂਰਨ		idx_orig,
    ਪੂਰਨ		idx_new)
{
    ਲੰਮਾ	off;

    ਜੇ (dprev == ਨਿਰਾ)
	off = 0;
    ਨਹੀਂਤਾਂ
	off = (dprev->df_lnum[idx_orig] + dprev->df_ਗਿਣਤੀ[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_ਗਿਣਤੀ[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_ਗਿਣਤੀ[idx_new] = dp->df_ਗਿਣਤੀ[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    ਖਾਲੀ
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *ਅਗਲਾ_p;

    ਲਈ (p = tp->tp_ਪਹਿਲਾ_diff; p != ਨਿਰਾ; p = ਅਗਲਾ_p)
    {
	ਅਗਲਾ_p = p->df_ਅਗਲਾ;
	clear_diffblock(p);
    }
    tp->tp_ਪਹਿਲਾ_diff = ਨਿਰਾ;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ਸਥਿਰ ਪੂਰਨ
diff_linematch(diff_T *dp)
{
    ਜੇ (!(diff_flags & DIFF_LINEMATCH))
	ਵਾਪਸ 0;

    // are there more than three diff buffers?
    ਪੂਰਨ tsize = 0;
    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
    {
	ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    ਜੇ (dp->df_ਗਿਣਤੀ[i] < 0)
		ਵਾਪਸ FALSE;
	    tsize += dp->df_ਗਿਣਤੀ[i];
	}
    }

    // avoid allocating a huge array because it will lag
    ਵਾਪਸ tsize <= linematch_ਸਤਰਾਂ;
}

    ਸਥਿਰ ਪੂਰਨ
get_max_diff_ਲੰਮਾਈ(ਅਚਲ diff_T *dp)
{
    ਪੂਰਨ maxlength = 0;

    ਲਈ (ਪੂਰਨ k = 0; k < DB_ਗਿਣਤੀ; k++)
    {
	ਜੇ (curtab->tp_diffbuf[k] != ਨਿਰਾ)
	{
	    ਜੇ (dp->df_ਗਿਣਤੀ[k] > maxlength)
		maxlength = dp->df_ਗਿਣਤੀ[k];
	}
    }
    ਵਾਪਸ maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਸਿਖਰ_diff_ਖੰਡ(
    diff_T	**thistopdiff,
    diff_T	**ਅਗਲਾ_adjacent_ਖੰਡਾਂ,
    ਪੂਰਨ		fromidx,
    ਪੂਰਨ		topline)
{
    diff_T	*topdiff = ਨਿਰਾ;
    diff_T	*localtopdiff = ਨਿਰਾ;
    ਪੂਰਨ		topdiffchange = 0;

    ਲਈ (topdiff = curtab->tp_ਪਹਿਲਾ_diff; topdiff != ਨਿਰਾ;
						topdiff = topdiff->df_ਅਗਲਾ)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	ਜੇ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	ਜੇ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_ਗਿਣਤੀ[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    ਜੇ ((*thistopdiff) == ਨਿਰਾ)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	ਜੇ (!(topdiff->df_ਅਗਲਾ && (topdiff->df_ਅਗਲਾ->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_ਗਿਣਤੀ[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    ਜੇ (*thistopdiff)
	    {
		(*ਅਗਲਾ_adjacent_ਖੰਡਾਂ) = topdiff->df_ਅਗਲਾ;
		ਰੋਕੋ;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    ਸਥਿਰ ਖਾਲੀ
calculate_topfill_and_topline(
    ਅਚਲ ਪੂਰਨ	fromidx,
    ਅਚਲ ਪੂਰਨ	toidx,
    ਅਚਲ ਪੂਰਨ	from_topline,
    ਅਚਲ ਪੂਰਨ	from_topfill,
    ਪੂਰਨ		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ਨਿਰਾ;
    diff_T	*ਅਗਲਾ_adjacent_ਖੰਡਾਂ = ਨਿਰਾ;
    ਪੂਰਨ		virtual_ਸਤਰਾਂ_passed = 0;
    ਪੂਰਨ		curlinenum_ਮੰਜ਼ਿਲ = 1;

    ਲੱਭੋ_ਸਿਖਰ_diff_ਖੰਡ(&thistopdiff, &ਅਗਲਾ_adjacent_ਖੰਡਾਂ, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    ਜਦੋਂ (curdif && (curdif->df_lnum[fromidx] + curdif->df_ਗਿਣਤੀ[fromidx])
							<= from_topline)
    {
	virtual_ਸਤਰਾਂ_passed += get_max_diff_ਲੰਮਾਈ(curdif);

	curdif = curdif->df_ਅਗਲਾ;
    }

    ਜੇ (curdif != ਅਗਲਾ_adjacent_ਖੰਡਾਂ)
	virtual_ਸਤਰਾਂ_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_ਸਤਰਾਂ_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    ਜੇ (virtual_ਸਤਰਾਂ_passed < 0)
	virtual_ਸਤਰਾਂ_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    ਜੇ (thistopdiff != ਨਿਰਾ) // this should not be null, but just for safety
	curlinenum_ਮੰਜ਼ਿਲ = thistopdiff->df_lnum[toidx];

    ਪੂਰਨ virt_ਸਤਰਾਂ_ਖੱਬਾ = virtual_ਸਤਰਾਂ_passed;
    curdif = thistopdiff;
    ਜਦੋਂ (virt_ਸਤਰਾਂ_ਖੱਬਾ > 0 && curdif != ਨਿਰਾ && curdif != ਅਗਲਾ_adjacent_ਖੰਡਾਂ)
    {
	curlinenum_ਮੰਜ਼ਿਲ += MIN(virt_ਸਤਰਾਂ_ਖੱਬਾ, curdif->df_ਗਿਣਤੀ[toidx]);
	virt_ਸਤਰਾਂ_ਖੱਬਾ -= MIN(virt_ਸਤਰਾਂ_ਖੱਬਾ, get_max_diff_ਲੰਮਾਈ(curdif));
	curdif = curdif->df_ਅਗਲਾ;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    ਪੂਰਨ max_virt_ਸਤਰਾਂ = 0;
    ਲਈ (diff_T *dp = thistopdiff; dp != ਨਿਰਾ; dp = dp->df_ਅਗਲਾ)
    {
	ਜੇ (dp->df_lnum[toidx] + dp->df_ਗਿਣਤੀ[toidx] <= curlinenum_ਮੰਜ਼ਿਲ)
	    max_virt_ਸਤਰਾਂ += get_max_diff_ਲੰਮਾਈ(dp);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (dp->df_lnum[toidx] <= curlinenum_ਮੰਜ਼ਿਲ)
		max_virt_ਸਤਰਾਂ += curlinenum_ਮੰਜ਼ਿਲ - dp->df_lnum[toidx];
	    ਰੋਕੋ;
	}
    }

    ਜੇ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_ਸਤਰਾਂ - virtual_ਸਤਰਾਂ_passed;
    (*topline) = curlinenum_ਮੰਜ਼ਿਲ;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ਸਥਿਰ ਖਾਲੀ
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_ਲੰਮਾਈ,
    ਅਚਲ ਪੂਰਨ	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    ਪੂਰਨ		ਸਤਰ_numbers[DB_ਗਿਣਤੀ];
    ਪੂਰਨ		outputmap[DB_ਗਿਣਤੀ];
    size_t	ndiffs = 0;

    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
    {
	ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
	{
	    ਸਤਰ_numbers[i] = dp->df_lnum[i];
	    dp->df_ਗਿਣਤੀ[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    ਲਈ (size_t i = 0; i < decisions_ਲੰਮਾਈ; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	ਜੇ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_ਅਗਲਾ);
	    dp_s->is_linematched = TRUE;
	    ਲਈ (ਪੂਰਨ j = 0; j < DB_ਗਿਣਤੀ; j++)
	    {
		ਜੇ (curtab->tp_diffbuf[j] != ਨਿਰਾ)
		{
		    dp_s->df_lnum[j] = ਸਤਰ_numbers[j];
		    dp_s->df_ਗਿਣਤੀ[j] = 0;
		}
	    }
	}
	ਲਈ (size_t j = 0; j < ndiffs; j++)
	{
	    ਜੇ (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_ਗਿਣਤੀ[outputmap[j]]++;
		ਸਤਰ_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
ਲੜੀਵਾਰ_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_ਗਿਣਤੀ];
    ਅਚਲ mmfile_t	*diffbufs[DB_ਗਿਣਤੀ];
    ਪੂਰਨ			diff_ਲੰਮਾਈ[DB_ਗਿਣਤੀ];
    size_t		ndiffs = 0;

    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
    {
	ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    ਜੇ (dp->df_ਗਿਣਤੀ[i] > 0)
	    {
		diff_write_ਬਫਰ(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_ਗਿਣਤੀ[i] - 1);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		diffbufs_mm[ndiffs].din_mmfile.ਮਾਪ = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ਨਿਰਾ;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_ਲੰਮਾਈ[ndiffs] = dp->df_ਗਿਣਤੀ[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    ਪੂਰਨ *decisions = ਨਿਰਾ;
    ਅਚਲ ਪੂਰਨ iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_ਲੰਮਾਈ =
	linematch_nbuffers(diffbufs, diff_ਲੰਮਾਈ, ndiffs, &decisions, iwhite);

    ਲਈ (size_t i = 0; i < ndiffs; i++)
	ਮੁਕਤ(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_ਲੰਮਾਈ, decisions);

    ਮੁਕਤ(decisions);
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
    ਪੂਰਨ
diff_check_with_linestatus(win_T *wp, linenr_T lnum, ਪੂਰਨ *linestatus)
{
    ਪੂਰਨ		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    ਪੂਰਨ		maxcount;
    ਪੂਰਨ		i;
    buf_T	*buf = wp->w_ਬਫਰ;
    ਪੂਰਨ		cmp;

    ਜੇ (linestatus != ਨਿਰਾ)
	*linestatus = 0;

    ਜੇ (curtab->tp_diff_invalid)
	ex_diffupdate(ਨਿਰਾ);		// update after a big change

    ਜੇ (curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ || !wp->w_p_diff)	// no diffs at all
	ਵਾਪਸ 0;

    // safety check: "lnum" must be a buffer line
    ਜੇ (lnum < 1 || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	ਵਾਪਸ 0;

    idx = diff_buf_idx(buf);
    ਜੇ (idx == DB_ਗਿਣਤੀ)
	ਵਾਪਸ 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    ਜੇ (hasFoldingWin(wp, lnum, ਨਿਰਾ, ਨਿਰਾ, TRUE, ਨਿਰਾ))
	ਵਾਪਸ 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ਨਿਰਾ;		// last block with df_lnum[idx] <= lnum
    ਲਈ (dp = diff_ਜਾਂਚ_ਸ਼ੁਰੂ(curtab, idx, lnum); dp != ਨਿਰਾ; dp = dp->df_ਅਗਲਾ)
    {
	ਜੇ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	ਜੇ (lnum <= dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx])
	    ਰੋਕੋ;
    }
    diff_ਜਾਂਚ_remember(curtab, idx, lnum, le);
    ਜੇ (dp == ਨਿਰਾ || lnum < dp->df_lnum[idx])
	ਵਾਪਸ 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    ਜੇ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	ਲੜੀਵਾਰ_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    ਪੂਰਨ ਅੰਕ_fill = 0;
    ਜਦੋਂ (lnum == dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	ਜੇ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_ਲੰਮਾਈ(dp);
	    ਅੰਕ_fill += maxcount - dp->df_ਗਿਣਤੀ[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	ਜੇ (dp->df_ਅਗਲਾ != ਨਿਰਾ
		&& lnum >= dp->df_ਅਗਲਾ->df_lnum[idx]
		&& lnum <= dp->df_ਅਗਲਾ->df_lnum[idx] + dp->df_ਅਗਲਾ->df_ਗਿਣਤੀ[idx])
	    dp = dp->df_ਅਗਲਾ;
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    ਜੇ (lnum < dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx])
    {
	ਪੂਰਨ	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	    ਜੇ (i != idx && curtab->tp_diffbuf[i] != ਨਿਰਾ)
	    {
		ਜੇ (dp->df_ਗਿਣਤੀ[i] == 0)
		    zero = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (dp->df_ਗਿਣਤੀ[i] != dp->df_ਗਿਣਤੀ[idx])
		    {
			ਜੇ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			ਵਾਪਸ ਅੰਕ_fill;
		    }
		    cmp = TRUE;
		}
	    }
	ਜੇ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
		ਜੇ (i != idx && curtab->tp_diffbuf[i] != ਨਿਰਾ
						      && dp->df_ਗਿਣਤੀ[i] != 0)
		    ਜੇ (!diff_equal_ਇੰਦਰਾਜ(dp, idx, i))
		    {
			ਜੇ (linestatus)
			    *linestatus = -1;
			ਵਾਪਸ ਅੰਕ_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	ਜੇ (zero == FALSE)
	    ਵਾਪਸ ਅੰਕ_fill;
	ਜੇ (linestatus)
	    *linestatus = -2;
	ਵਾਪਸ ਅੰਕ_fill;
    }
    ਵਾਪਸ ਅੰਕ_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_equal_ਇੰਦਰਾਜ(diff_T *dp, ਪੂਰਨ idx1, ਪੂਰਨ idx2)
{
    ਪੂਰਨ		i;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		cmp;

    ਜੇ (dp->df_ਗਿਣਤੀ[idx1] != dp->df_ਗਿਣਤੀ[idx2])
	ਵਾਪਸ FALSE;
    ਜੇ (diff_check_sanity(curtab, dp) == FAIL)
	ਵਾਪਸ FALSE;
    ਲਈ (i = 0; i < dp->df_ਗਿਣਤੀ[idx1]; ++i)
    {
	ਸਤਰ = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	ਜੇ (ਸਤਰ == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
	cmp = diff_cmp(ਸਤਰ, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(ਸਤਰ);
	ਜੇ (cmp != 0)
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_equal_char(char_u *p1, char_u *p2, ਪੂਰਨ *len)
{
    ਪੂਰਨ l  = (*mb_ptr2len)(p1);

    ਜੇ (l != (*mb_ptr2len)(p2))
	ਵਾਪਸ FALSE;
    ਜੇ (l > 1)
    {
	ਜੇ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    ਵਾਪਸ FALSE;
	*len = l;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    ਵਾਪਸ FALSE;
	*len = 1;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ਸਥਿਰ ਪੂਰਨ
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    ਪੂਰਨ		l;

    ਜੇ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	ਵਾਪਸ 0;

    ਜੇ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	ਵਾਪਸ STRCMP(s1, s2);
    ਜੇ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	ਵਾਪਸ MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    ਜਦੋਂ (*p1 != NUL && *p2 != NUL)
    {
	ਜੇ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!diff_equal_char(p1, p2, &l))
		ਰੋਕੋ;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    ਜੇ (*p1 != NUL || *p2 != NUL)
	ਵਾਪਸ 1;
    ਵਾਪਸ 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    ਪੂਰਨ
diff_check_fill(win_T *wp, linenr_T lnum)
{
    ਪੂਰਨ		n;

    // be quick when there are no filler lines
    ਜੇ (!(diff_flags & DIFF_FILLER))
	ਵਾਪਸ 0;
    n = diff_check_with_linestatus(wp, lnum, ਨਿਰਾ);
    ਜੇ (n <= 0)
	ਵਾਪਸ 0;
    ਵਾਪਸ n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    ਖਾਲੀ
diff_ਸੈੱਟ_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_ਬਫਰ;
    linenr_T	lnum;
    ਪੂਰਨ		fromidx;
    ਪੂਰਨ		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    ਜੇ (fromidx == DB_ਗਿਣਤੀ)
	ਵਾਪਸ;		// safety check

    ਜੇ (curtab->tp_diff_invalid)
	ex_diffupdate(ਨਿਰਾ);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(curtab, dp)
	ਜੇ (lnum <= dp->df_lnum[fromidx] + dp->df_ਗਿਣਤੀ[fromidx])
	    ਰੋਕੋ;
    ਜੇ (dp == ਨਿਰਾ)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
				       - (frombuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum);
    }
    ਨਹੀਂਤਾਂ
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_ਬਫਰ);
	ਜੇ (toidx == DB_ਗਿਣਤੀ)
	    ਵਾਪਸ;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	ਜੇ (lnum >= dp->df_lnum[fromidx])
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
    ਜੇ (towin->w_topline > towin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	towin->w_topline = towin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	towin->w_botfill = true;
    }
    ਜੇ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    ਬਦਲਿਆ_ਸਤਰ_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (ਖਾਲੀ)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ਨਿਰਾ, TRUE, ਨਿਰਾ);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_diffanchors(
    ਪੂਰਨ		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    ਪੂਰਨ		*ਅੰਕ_anchors)
{
    ਪੂਰਨ i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ਨਿਰਾ;
    ਜੇ (check_only)
	bufwin = curwin;
    ਨਹੀਂਤਾਂ
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    ਜੇ (bufwin->w_ਬਫਰ == buf && bufwin->w_p_diff)
		ਰੋਕੋ;
	ਜੇ (bufwin == ਨਿਰਾ && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    ਵਾਪਸ FAIL;
	}
    }

    ਲਈ (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	ਜੇ (*dia == ',') // don't allow empty values
	    ਵਾਪਸ FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_ਪਤਾ(ਨਿਰਾ, &dia, ADDR_ਸਤਰਾਂ, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	ਜੇ (dia == ਨਿਰਾ) // error detected
	    ਵਾਪਸ FAIL;
	ਜੇ (*dia != ',' && *dia != NUL)
	    ਵਾਪਸ FAIL;

	ਜੇ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1))
	{
	    emsg(_(e_invalid_range));
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (anchors != ਨਿਰਾ)
	    anchors[i] = lnum;

	ਜੇ (*dia == ',')
	    dia++;
    }
    ਜੇ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_ਹੋਰ_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (ਅੰਕ_anchors != ਨਿਰਾ)
	*ਅੰਕ_anchors = i;
    ਵਾਪਸ OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    ਪੂਰਨ
diffanchors_ਬਦਲਿਆ(ਪੂਰਨ buflocal)
{
    ਪੂਰਨ ਨਤੀਜਾ = parse_diffanchors(TRUE, curbuf, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (ਨਤੀਜਾ == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    ਜੇ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		ਲਈ (ਪੂਰਨ idx = 0; idx < DB_ਗਿਣਤੀ; ++idx)
		    ਜੇ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			ਰੋਕੋ;
		    }
	    }
	}
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * This is called when 'diffopt' is changed.
 */
    ਪੂਰਨ
diffopt_ਬਦਲਿਆ(ਖਾਲੀ)
{
    char_u	*p;
    ਪੂਰਨ		diff_context_new = 6;
    ਪੂਰਨ		linematch_ਸਤਰਾਂ_new = 0;
    ਪੂਰਨ		diff_flags_new = 0;
    ਪੂਰਨ		diff_foldcolumn_new = 2;
    ਲੰਮਾ	diff_algorithm_new = 0;
    ਲੰਮਾ	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    ਜਦੋਂ (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	ਜੇ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_ਅੰਦਰੂਨੀ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    ਜੇ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_ਨਿਊਨਤਮ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    ਜੇ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_ਸ਼ਬਦ;
	    }
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_ਸਤਰਾਂ_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	ਜੇ (*p != ',' && *p != NUL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    ਜੇ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	ਵਾਪਸ FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    ਜੇ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_ਸਤਰਾਂ = linematch_ਸਤਰਾਂ_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    ਪੂਰਨ
diffopt_horizontal(ਖਾਲੀ)
{
    ਵਾਪਸ (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    ਪੂਰਨ
diffopt_hiddenoff(ਖਾਲੀ)
{
    ਵਾਪਸ (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    ਪੂਰਨ
diffopt_closeoff(ਖਾਲੀ)
{
    ਵਾਪਸ (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    ਖਾਲੀ
diff_update_ਸਤਰ(linenr_T lnum)
{
    ਪੂਰਨ		idx;
    diff_T	*dp;

    ਜੇ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	ਵਾਪਸ;

    idx = diff_buf_idx(curbuf);
    ਜੇ (idx == DB_ਗਿਣਤੀ)
	ਵਾਪਸ;
    FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(curtab, dp)
	ਜੇ (lnum <= dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx])
	    ਰੋਕੋ;

    // clear the inline change cache as it's invalid
    ਜੇ (dp != ਨਿਰਾ)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

ਸਥਿਰ diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    ਪੂਰਨ
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    ਪੂਰਨ *change_ਸ਼ੁਰੂ,
    ਪੂਰਨ *change_ਸਿਰਾ)
{
    ਜੇ (change->dc_ਸ਼ੁਰੂ_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_ਸ਼ੁਰੂ = 0;
    ਨਹੀਂਤਾਂ
	*change_ਸ਼ੁਰੂ = change->dc_ਸ਼ੁਰੂ[diffline->bufidx];
    ਜੇ (change->dc_ਸਿਰਾ_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_ਸਿਰਾ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਨਹੀਂਤਾਂ
	*change_ਸਿਰਾ = change->dc_ਸਿਰਾ[diffline->bufidx];

    ਜੇ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	ਵਾਪਸ FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
    {
	ਜੇ (i == diffline->bufidx)
	    ਜਾਰੀ;
	ਜੇ (change->dc_ਸ਼ੁਰੂ[i] != change->dc_ਸਿਰਾ[i]
		|| change->dc_ਸਿਰਾ_lnum_off[i] != change->dc_ਸ਼ੁਰੂ_lnum_off[i])
	{
	    ਵਾਪਸ FALSE;
	}
    }
    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ ਪੂਰਨ
diff_ਲੱਭੋ_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		*startp,	// first char of the change
    ਪੂਰਨ		*endp)		// last char of the change
{
    char_u	*ਸਤਰ_org;
    char_u	*ਸਤਰ_new;
    ਪੂਰਨ		i;
    ਪੂਰਨ		si_org, si_new;
    ਪੂਰਨ		ei_org, ei_new;
    ਪੂਰਨ		off;
    ਪੂਰਨ		added = TRUE;
    char_u	*p1, *p2;
    ਪੂਰਨ		l;

    ਜੇ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	ਸਤਰ_org = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	ਸਤਰ_org = vim_strsave(ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE));
	ਜੇ (ਸਤਰ_org == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    ਜੇ (off >= dp->df_ਗਿਣਤੀ[i])
		ਜਾਰੀ;
	    added = FALSE;
	    ਜੇ (diff_flags & DIFF_INLINE_NONE)
		ਰੋਕੋ; // early terminate as we only care about the return value

	    ਸਤਰ_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    ਜਦੋਂ (ਸਤਰ_org[si_org] != NUL)
	    {
		ਜੇ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(ਸਤਰ_org[si_org])
					      && VIM_ISWHITE(ਸਤਰ_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(ਸਤਰ_org[si_org])
					    || VIM_ISWHITE(ਸਤਰ_new[si_new]))))
		{
		    si_org = (ਪੂਰਨ)(skipwhite(ਸਤਰ_org + si_org) - ਸਤਰ_org);
		    si_new = (ਪੂਰਨ)(skipwhite(ਸਤਰ_new + si_new) - ਸਤਰ_new);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (!diff_equal_char(ਸਤਰ_org + si_org, ਸਤਰ_new + si_new,
									   &l))
			ਰੋਕੋ;
		    si_org += l;
		    si_new += l;
		}
	    }
	    ਜੇ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_ਸਿਰ_off)(ਸਤਰ_org, ਸਤਰ_org + si_org);
		si_new -= (*mb_ਸਿਰ_off)(ਸਤਰ_new, ਸਤਰ_new + si_new);
	    }
	    ਜੇ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    ਜੇ (ਸਤਰ_org[si_org] != NUL || ਸਤਰ_new[si_new] != NUL)
	    {
		ei_org = (ਪੂਰਨ)STRLEN(ਸਤਰ_org);
		ei_new = (ਪੂਰਨ)STRLEN(ਸਤਰ_new);
		ਜਦੋਂ (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    ਜੇ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(ਸਤਰ_org[ei_org])
					      && VIM_ISWHITE(ਸਤਰ_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(ਸਤਰ_org[ei_org])
					    || VIM_ISWHITE(ਸਤਰ_new[ei_new]))))
		    {
			ਜਦੋਂ (ei_org >= *startp
					     && VIM_ISWHITE(ਸਤਰ_org[ei_org]))
			    --ei_org;
			ਜਦੋਂ (ei_new >= si_new
					     && VIM_ISWHITE(ਸਤਰ_new[ei_new]))
			    --ei_new;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			p1 = ਸਤਰ_org + ei_org;
			p2 = ਸਤਰ_new + ei_new;
			p1 -= (*mb_ਸਿਰ_off)(ਸਤਰ_org, p1);
			p2 -= (*mb_ਸਿਰ_off)(ਸਤਰ_new, p2);
			ਜੇ (!diff_equal_char(p1, p2, &l))
			    ਰੋਕੋ;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		ਜੇ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(ਸਤਰ_org);
    ਵਾਪਸ added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਲੰਮਾ byte_ਸ਼ੁਰੂ;
    ਲੰਮਾ ਅੰਕ_ਬਾਈਟਾਂ;
    ਪੂਰਨ lineoff;
} linemap_ਇੰਦਰਾਜ_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, ਪੂਰਨ idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    ਪੂਰਨ pass = 1;
    ਕਰੋ
    {
	ਪੂਰਨ has_unmerged_gaps = FALSE;
	ਪੂਰਨ has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	ਜਦੋਂ (dp!= ਨਿਰਾ && dp->df_ਅਗਲਾ != ਨਿਰਾ)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    ਜੇ (dp->df_lnum[idx1] + dp->df_ਗਿਣਤੀ[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_ਅਗਲਾ->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_ਅਗਲਾ;
		ਜਾਰੀ;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_ਇੰਦਰਾਜ_T *entry1 = &((linemap_ਇੰਦਰਾਜ_T*)linemap[idx1].ga_ਅੰਕੜੇ)[dp->df_lnum[idx1] + dp->df_ਗਿਣਤੀ[idx1] - 1];
	    linemap_ਇੰਦਰਾਜ_T *entry2 = &((linemap_ਇੰਦਰਾਜ_T*)linemap[idx1].ga_ਅੰਕੜੇ)[dp->df_ਅਗਲਾ->df_lnum[idx1] - 1];
	    ਜੇ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_ਅਗਲਾ;
		ਜਾਰੀ;
	    }

	    linenr_T gap = dp->df_ਅਗਲਾ->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_ਗਿਣਤੀ[idx1]);
	    ਜੇ (gap <= 3)
	    {
		linenr_T max_df_ਗਿਣਤੀ = 0;
		ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
		    max_df_ਗਿਣਤੀ = MAX(max_df_ਗਿਣਤੀ, dp->df_ਗਿਣਤੀ[i] + dp->df_ਅਗਲਾ->df_ਗਿਣਤੀ[i]);

		ਜੇ (max_df_ਗਿਣਤੀ >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
		    {
			dp->df_ਗਿਣਤੀ[i] = dp->df_ਅਗਲਾ->df_lnum[i]
			    + dp->df_ਅਗਲਾ->df_ਗਿਣਤੀ[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_ਅਗਲਾ = dp->df_ਅਗਲਾ;
		    dp->df_ਅਗਲਾ = dp_ਅਗਲਾ->df_ਅਗਲਾ;
		    clear_diffblock(dp_ਅਗਲਾ);
		    has_merged_gaps = TRUE;
		    ਜਾਰੀ;
		}
		ਨਹੀਂਤਾਂ
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_ਅਗਲਾ;
	}
	ਜੇ (!has_unmerged_gaps || !has_merged_gaps)
	    ਰੋਕੋ;
    } ਜਦੋਂ (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_refine_inline_ਸ਼ਬਦ_highlight(diff_T *dp_orig, garray_T *linemap, ਪੂਰਨ idx1,
				  linenr_T ਸ਼ੁਰੂ_lnum)
{
    ਪੂਰਨ pass = 1;
    ਕਰੋ
    {
	diff_T *dp = dp_orig;

	ਜਦੋਂ (dp != ਨਿਰਾ && dp->df_ਅਗਲਾ != ਨਿਰਾ)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_ਗਿਣਤੀ[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_ਅਗਲਾ->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    ਜੇ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_ਅਗਲਾ;
		ਜਾਰੀ;
	    }

	    linemap_ਇੰਦਰਾਜ_T *entry1 =
		&((linemap_ਇੰਦਰਾਜ_T *)linemap[idx1].ga_ਅੰਕੜੇ)[idx_entry1];
	    linemap_ਇੰਦਰਾਜ_T *entry2 =
		&((linemap_ਇੰਦਰਾਜ_T *)linemap[idx1].ga_ਅੰਕੜੇ)[idx_entry2];

	    // Skip if blocks are on different lines
	    ਜੇ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_ਅਗਲਾ;
		ਜਾਰੀ;
	    }

	    // Calculate the gap between blocks
	    ਪੂਰਨ gap_ਸ਼ੁਰੂ = entry1->byte_ਸ਼ੁਰੂ + entry1->ਅੰਕ_ਬਾਈਟਾਂ;
	    ਪੂਰਨ gap_ਸਿਰਾ = entry2->byte_ਸ਼ੁਰੂ;
	    ਪੂਰਨ gap_ਮਾਪ = gap_ਸਿਰਾ - gap_ਸ਼ੁਰੂ;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    ਜੇ (gap_ਮਾਪ <= 0 || gap_ਮਾਪ > diff_ਸ਼ਬਦ_gap)
	    {
		dp = dp->df_ਅਗਲਾ;
		ਜਾਰੀ;
	    }

	    // Get the text between the two blocks
	    char_u *ਸਤਰ = ml_get_buf(curtab->tp_diffbuf[idx1],
		    ਸ਼ੁਰੂ_lnum + entry1->lineoff, FALSE);
	    char_u *gap_ਪਾਠ = ਸਤਰ + gap_ਸ਼ੁਰੂ;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_ਸ਼ਬਦ = true;
	    bool has_content = false;

	    ਲਈ (ਪੂਰਨ i = 0; i < gap_ਮਾਪ && gap_ਪਾਠ[i] != NUL; i++)
	    {
		has_content = true;
		ਪੂਰਨ char_class = mb_get_class_buf(gap_ਪਾਠ + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		ਜੇ (char_class == 2)
		{
		    only_non_ਸ਼ਬਦ = false;
		    ਰੋਕੋ;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    ਜੇ (has_content && only_non_ਸ਼ਬਦ)
	    {
		ਲੰਮਾ ਕੁੱਲ_change_ਬਾਈਟਾਂ = 0;
		ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
		{
		    ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
		    {
			// count bytes in the first block
			ਲਈ (ਪੂਰਨ k = 0; k < dp->df_ਗਿਣਤੀ[i]; k++)
			{
			    ਪੂਰਨ idx = dp->df_lnum[i] + k - 1;
			    ਜੇ (idx < linemap[i].ga_len)
				ਕੁੱਲ_change_ਬਾਈਟਾਂ +=
				    ((linemap_ਇੰਦਰਾਜ_T *)linemap[i].ga_ਅੰਕੜੇ)[idx].ਅੰਕ_ਬਾਈਟਾਂ;
			}
			// count bytes in the next block
			ਲਈ (ਪੂਰਨ k = 0; k < dp->df_ਅਗਲਾ->df_ਗਿਣਤੀ[i]; k++)
			{
			    ਪੂਰਨ idx = dp->df_ਅਗਲਾ->df_lnum[i] + k - 1;
			    ਜੇ (idx < linemap[i].ga_len)
				ਕੁੱਲ_change_ਬਾਈਟਾਂ +=
				    ((linemap_ਇੰਦਰਾਜ_T *)linemap[i].ga_ਅੰਕੜੇ)[idx].ਅੰਕ_ਬਾਈਟਾਂ;
			}
		    }
		}

		ਜੇ (ਕੁੱਲ_change_ਬਾਈਟਾਂ >= gap_ਮਾਪ * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
		    {
			ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
			{
			    dp->df_ਗਿਣਤੀ[i] = dp->df_ਅਗਲਾ->df_lnum[i]
				+ dp->df_ਅਗਲਾ->df_ਗਿਣਤੀ[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_ਅਗਲਾ = dp->df_ਅਗਲਾ;
		    dp->df_ਅਗਲਾ = dp_ਅਗਲਾ->df_ਅਗਲਾ;
		    clear_diffblock(dp_ਅਗਲਾ);

		    // Don't advance dp, check if can merge with the next block too
		    ਜਾਰੀ;
		}
	    }

	    dp = dp->df_ਅਗਲਾ;
	}
    } ਜਦੋਂ (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_ਲੱਭੋ_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_ਗਿਣਤੀ];
    garray_T	file1_str;
    garray_T	file2_str;
    ਪੂਰਨ		file1_idx = -1;

    ਲੰਮਾ	save_diff_algorithm = diff_algorithm;

    CLEAR_ਖੇਤਰ(dio);
    ga_init2(&dio.dio_diff.dout_ga, ਆਕਾਰ(ਅੱਖਰ *), 1000);

    // inline diff only supports internal algo
    dio.dio_ਅੰਦਰੂਨੀ = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_ਪਹਿਲਾ_diff;
    curtab->tp_ਪਹਿਲਾ_diff = ਨਿਰਾ;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    //ਬਫਰ
    buf_T	*(orig_diffbuf[DB_ਗਿਣਤੀ]);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(orig_diffbuf, curtab->tp_diffbuf, ਆਕਾਰ(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
	ga_init2(&linemap[i], ਆਕਾਰ(linemap_ਇੰਦਰਾਜ_T), 128);

    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ)
	    ਜਾਰੀ; // skip buffer that isn't loaded

	ਜੇ (dp->df_ਗਿਣਤੀ[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ਨਿਰਾ;
	    ਜਾਰੀ;
	}

	ਜੇ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	ਲਈ (ਪੂਰਨ off = 0; off < dp->df_ਗਿਣਤੀ[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    ਪੂਰਨ ਵਿੱਚ_keyword = FALSE;

	    // iwhiteeol support vars
	    ਪੂਰਨ ਆਖਰੀ_white = FALSE;
	    ਪੂਰਨ eol_ga_len = -1;
	    ਪੂਰਨ eol_linemap_len = -1;
	    ਪੂਰਨ eol_numlines = -1;

	    char_u *s;
	    ਲਈ (s = curline; *s != NUL;)
	    {
		ਪੂਰਨ new_ਵਿੱਚ_keyword = FALSE;
		ਜੇ (diff_flags & DIFF_INLINE_ਸ਼ਬਦ)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_ਵਿੱਚ_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		ਜੇ (ਵਿੱਚ_keyword && !new_ਵਿੱਚ_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ਜੇ (VIM_ISWHITE(*s))
		{
		    ਜੇ (diff_flags & DIFF_IWHITEALL)
		    {
			ਵਿੱਚ_keyword = FALSE;
			s = skipwhite(s);
			ਜਾਰੀ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ਜੇ (!ਆਖਰੀ_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    ਆਖਰੀ_white = TRUE;
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ਆਖਰੀ_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		ਪੂਰਨ char_len = 1;
		ਜੇ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		ਨਹੀਂਤਾਂ
		{
		    char_len = mb_ptr2len(s);

		    ਜੇ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    ਜੇ (diff_flags & DIFF_ICASE)
		    {
			ਪੂਰਨ c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			ਪੂਰਨ c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			ਪੂਰਨ c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			ਜੇ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    ਨਹੀਂਤਾਂ
			ga_concat_len(curstr, s, char_len);
		}

		ਜੇ (!new_ਵਿੱਚ_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ਜੇ (!new_ਵਿੱਚ_keyword || (new_ਵਿੱਚ_keyword && !ਵਿੱਚ_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_ਇੰਦਰਾਜ_T linemap_ਇੰਦਰਾਜ;
		    linemap_ਇੰਦਰਾਜ.lineoff = off;
		    linemap_ਇੰਦਰਾਜ.byte_ਸ਼ੁਰੂ = s - curline;
		    linemap_ਇੰਦਰਾਜ.ਅੰਕ_ਬਾਈਟਾਂ = char_len;
		    ਜੇ (ga_grow(&linemap[i], 1) != OK)
			ਜਾਓ ਪੂਰਾ;
		    ((linemap_ਇੰਦਰਾਜ_T*)(linemap[i].ga_ਅੰਕੜੇ))[linemap[i].ga_len]
			= linemap_ਇੰਦਰਾਜ;
		    linemap[i].ga_len += 1;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_ਇੰਦਰਾਜ_T*)linemap[i].ga_ਅੰਕੜੇ)[linemap[i].ga_len-1].ਅੰਕ_ਬਾਈਟਾਂ
			+= char_len;
		}

		ਵਿੱਚ_keyword = new_ਵਿੱਚ_keyword;
		s += char_len;
	    }
	    ਜੇ (ਵਿੱਚ_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    ਜੇ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		ਜੇ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    ਜੇ (!(diff_flags & DIFF_IWHITEALL))
	    {
		// Add an empty line token mapped to the end-of-line in the
		// original file. This helps diff newline differences among
		// files, which will be visualized when using 'list' as the eol
		// listchar will be highlighted.
		ga_append(curstr, NL);
		numlines++;

		linemap_ਇੰਦਰਾਜ_T linemap_ਇੰਦਰਾਜ;
		linemap_ਇੰਦਰਾਜ.lineoff = off;
		linemap_ਇੰਦਰਾਜ.byte_ਸ਼ੁਰੂ = s - curline;
		linemap_ਇੰਦਰਾਜ.ਅੰਕ_ਬਾਈਟਾਂ = ਆਕਾਰ(NL);
		ਜੇ (ga_grow(&linemap[i], 1) != OK)
		    ਜਾਓ ਪੂਰਾ;
		((linemap_ਇੰਦਰਾਜ_T*)(linemap[i].ga_ਅੰਕੜੇ))[linemap[i].ga_len]
		    = linemap_ਇੰਦਰਾਜ;
		linemap[i].ga_len += 1;
	    }
	}

	ਜੇ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (ਅੱਖਰ *)curstr->ga_ਅੰਕੜੇ;
	    dio.dio_new.din_mmfile.ਮਾਪ = curstr->ga_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    dio.dio_orig.din_mmfile.ptr = (ਅੱਖਰ *)curstr->ga_ਅੰਕੜੇ;
	    dio.dio_orig.din_mmfile.ਮਾਪ = curstr->ga_len;
	}
	ਜੇ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    ਪੂਰਨ diff_ਸਥਿਤੀ = diff_file_ਅੰਦਰੂਨੀ(&dio);
	    ਜੇ (diff_ਸਥਿਤੀ == FAIL)
		ਜਾਓ ਪੂਰਾ;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_ਪਹਿਲਾ_diff;

    ਜੇ (diff_flags & DIFF_INLINE_ਸ਼ਬਦ && file1_idx != -1)
	diff_refine_inline_ਸ਼ਬਦ_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    ਨਹੀਂਤਾਂ ਜੇ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    ਲਈ (; new_diff != ਨਿਰਾ; new_diff = new_diff->df_ਅਗਲਾ)
    {
	diffline_change_T change;
	CLEAR_ਖੇਤਰ(change);
	ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
	{
	    ਜੇ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		ਜਾਰੀ;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_ਸਿਰਾ = diff_lnum + new_diff->df_ਗਿਣਤੀ[i];

	    ਜੇ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_ਸ਼ੁਰੂ[i] = MAXCOL;
		change.dc_ਸ਼ੁਰੂ_lnum_off[i] = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		change.dc_ਸ਼ੁਰੂ[i] = ((linemap_ਇੰਦਰਾਜ_T*)linemap[i].ga_ਅੰਕੜੇ)[diff_lnum].byte_ਸ਼ੁਰੂ;
		change.dc_ਸ਼ੁਰੂ_lnum_off[i] = ((linemap_ਇੰਦਰਾਜ_T*)linemap[i].ga_ਅੰਕੜੇ)[diff_lnum].lineoff;
	    }

	    ਜੇ (diff_lnum == diff_lnum_ਸਿਰਾ)
	    {
		change.dc_ਸਿਰਾ[i] = change.dc_ਸ਼ੁਰੂ[i];
		change.dc_ਸਿਰਾ_lnum_off[i] = change.dc_ਸ਼ੁਰੂ_lnum_off[i];
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (diff_lnum_ਸਿਰਾ - 1 >= linemap[i].ga_len)
	    {
		change.dc_ਸਿਰਾ[i] = MAXCOL;
		change.dc_ਸਿਰਾ_lnum_off[i] = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		change.dc_ਸਿਰਾ[i] = ((linemap_ਇੰਦਰਾਜ_T*)linemap[i].ga_ਅੰਕੜੇ)[diff_lnum_ਸਿਰਾ-1].byte_ਸ਼ੁਰੂ +
		    ((linemap_ਇੰਦਰਾਜ_T*)linemap[i].ga_ਅੰਕੜੇ)[diff_lnum_ਸਿਰਾ-1].ਅੰਕ_ਬਾਈਟਾਂ;
		change.dc_ਸਿਰਾ_lnum_off[i] = ((linemap_ਇੰਦਰਾਜ_T*)linemap[i].ga_ਅੰਕੜੇ)[diff_lnum_ਸਿਰਾ-1].lineoff;
	    }
	}
	ਜੇ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    ਜਾਓ ਪੂਰਾ;
	}
	((diffline_change_T*)(dp->df_changes.ga_ਅੰਕੜੇ))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

ਪੂਰਾ:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_ਪਹਿਲਾ_diff = orig_diff;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(curtab->tp_diffbuf, orig_diffbuf, ਆਕਾਰ(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    ਪੂਰਨ
diff_ਲੱਭੋ_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		off;

    idx = diff_buf_idx(wp->w_ਬਫਰ);
    ਜੇ (idx == DB_ਗਿਣਤੀ)	// cannot happen
	ਵਾਪਸ FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(curtab, dp)
	ਜੇ (lnum < dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx])
	    ਰੋਕੋ;
    ਜੇ (dp == ਨਿਰਾ || diff_check_sanity(curtab, dp) == FAIL)
	ਵਾਪਸ FALSE;

    ਜੇ (lnum - dp->df_lnum[idx] > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	// Integer overflow protection
	ਵਾਪਸ FALSE;
    off = lnum - dp->df_lnum[idx];

    ਜੇ (!(diff_flags & ALL_INLINE_DIFF) || diff_ਅੰਦਰੂਨੀ_failed())
    {
	// Use simple algorithm
	ਪੂਰਨ	change_ਸ਼ੁਰੂ = MAXCOL;	// first col of changed area
	ਪੂਰਨ	change_ਸਿਰਾ = -1;	// last col of changed area
	ਪੂਰਨ	ret;

	ret = diff_ਲੱਭੋ_change_simple(wp, lnum, dp, idx, &change_ਸ਼ੁਰੂ, &change_ਸਿਰਾ);

	// convert from inclusive end to exclusive end per diffline's contract
	change_ਸਿਰਾ += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_ਖੇਤਰ(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->ਅੰਕ_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_ਸ਼ੁਰੂ[idx] = change_ਸ਼ੁਰੂ;
	simple_diffline_change.dc_ਸਿਰਾ[idx] = change_ਸਿਰਾ;
	simple_diffline_change.dc_ਸ਼ੁਰੂ_lnum_off[idx] = off;
	simple_diffline_change.dc_ਸਿਰਾ_lnum_off[idx] = off;
	ਵਾਪਸ ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    ਜੇ (!dp->has_changes)
	diff_ਲੱਭੋ_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    ਪੂਰਨ ਅੰਕ_changes = 0;
    ਪੂਰਨ change_idx = 0;
    diffline->changes = ਨਿਰਾ;
    ਲਈ (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_ਅੰਕੜੇ)[change_idx];
	ਜੇ (change->dc_ਸਿਰਾ_lnum_off[idx] < off)
	    ਜਾਰੀ;
	ਜੇ (change->dc_ਸ਼ੁਰੂ_lnum_off[idx] > off)
	    ਰੋਕੋ;
	ਜੇ (diffline->changes == ਨਿਰਾ)
	    diffline->changes = change;
	ਅੰਕ_changes++;
    }
    diffline->ਅੰਕ_changes = ਅੰਕ_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    ਪੂਰਨ added = FALSE;
    ਜੇ (ਅੰਕ_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	ਲਈ (ਪੂਰਨ i = 0; i < DB_ਗਿਣਤੀ; i++)
	{
	    ਜੇ (idx == i)
		ਜਾਰੀ;
	    ਜੇ (curtab->tp_diffbuf[i] == ਨਿਰਾ)
		ਜਾਰੀ;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_ਅੰਕੜੇ)[dp->df_changes.ga_len-1];
	    ਜੇ (change->dc_ਸ਼ੁਰੂ_lnum_off[i] != ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	    {
		added = FALSE;
		ਰੋਕੋ;
	    }
	}
    }
    ਵਾਪਸ added;
}

# ਜੇ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    ਪੂਰਨ
diff_infold(win_T *wp, linenr_T lnum)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		idx = -1;
    ਪੂਰਨ		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    ਜੇ (!wp->w_p_diff)
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
    {
	ਜੇ (curtab->tp_diffbuf[i] == wp->w_ਬਫਰ)
	    idx = i;
	ਨਹੀਂਤਾਂ ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    ਜੇ (idx == -1 || !other)
	ਵਾਪਸ FALSE;

    ਜੇ (curtab->tp_diff_invalid)
	ex_diffupdate(ਨਿਰਾ);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    ਜੇ (curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ਨਿਰਾ;		// last block with df_lnum[idx] <= lnum
    ਪੂਰਨ		ਨਤੀਜਾ = TRUE;
    ਲਈ (dp = diff_ਜਾਂਚ_ਸ਼ੁਰੂ(curtab, idx, lnum); dp != ਨਿਰਾ; dp = dp->df_ਅਗਲਾ)
    {
	ਜੇ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	ਜੇ (dp->df_lnum[idx] - diff_context > lnum)
	    ਰੋਕੋ;
	// If this change ends before the line we have a match.
	ਜੇ (dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx] + diff_context > lnum)
	{
	    ਨਤੀਜਾ = FALSE;
	    ਰੋਕੋ;
	}
    }
    diff_ਜਾਂਚ_remember(curtab, idx, lnum, le);
    ਵਾਪਸ ਨਤੀਜਾ;
}
# endif

/*
 * "dp" and "do" commands.
 */
    ਖਾਲੀ
nv_diffgetput(ਪੂਰਨ put, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_ਕੰਮ_CHANNEL
    ਜੇ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	ਵਾਪਸ;
    }
# endif
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ea.arg = (char_u *)"";
    ਨਹੀਂਤਾਂ
    {
	vim_snprintf((ਅੱਖਰ *)buf, 30, "%ld", ਗਿਣਤੀ_2);
	ea.arg = buf;
    }
    ਜੇ (put)
	ea.cmdidx = CMD_diffput;
    ਨਹੀਂਤਾਂ
	ea.cmdidx = CMD_diffget;
    ea.addr_ਗਿਣਤੀ = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ਸਥਿਰ ਪੂਰਨ
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(curtab, dp)
	ਜੇ (dp == diff)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    ਖਾਲੀ
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    ਪੂਰਨ		idx_cur;
    ਪੂਰਨ		idx_other;
    ਪੂਰਨ		idx_from;
    ਪੂਰਨ		idx_ਮੰਜ਼ਿਲ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    ਪੂਰਨ		ਸ਼ੁਰੂ_skip, ਸਿਰਾ_skip;
    ਪੂਰਨ		new_ਗਿਣਤੀ;
    ਪੂਰਨ		buf_ਖਾਲੀ_ਥਾਵਾਂ;
    ਪੂਰਨ		ਮਿਲਿਆ_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    ਜੇ (idx_cur == DB_ਗਿਣਤੀ)
    {
	emsg(_(e_current_ਬਫਰ_is_not_ਵਿੱਚ_diff_ਬਹੁਲਕ));
	ਵਾਪਸ;
    }

    ਜੇ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	ਲਈ (idx_other = 0; idx_other < DB_ਗਿਣਤੀ; ++idx_other)
	    ਜੇ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ਨਿਰਾ)
	    {
		ਜੇ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    ਰੋਕੋ;
		ਮਿਲਿਆ_not_ma = TRUE;
	    }
	ਜੇ (idx_other == DB_ਗਿਣਤੀ)
	{
	    ਜੇ (ਮਿਲਿਆ_not_ma)
		emsg(_(e_no_other_ਬਫਰ_ਵਿੱਚ_diff_ਬਹੁਲਕ_is_modifiable));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_no_other_ਬਫਰ_ਵਿੱਚ_diff_ਬਹੁਲਕ));
	    ਵਾਪਸ;
	}

	// Check that there isn't a third buffer in the list
	ਲਈ (i = idx_other + 1; i < DB_ਗਿਣਤੀ; ++i)
	    ਜੇ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ਨਿਰਾ
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_ਹੋਰ_than_two_buffers_ਵਿੱਚ_diff_ਬਹੁਲਕ_dont_know_which_one_ਮੰਜ਼ਿਲ_use));
		ਵਾਪਸ;
	    }
    }
    ਨਹੀਂਤਾਂ
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	ਜਦੋਂ (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	ਲਈ (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	ਜੇ (eap->arg + i == p)	    // digits only
	    i = atol((ਅੱਖਰ *)eap->arg);
	ਨਹੀਂਤਾਂ
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    ਜੇ (i < 0)
		ਵਾਪਸ;		// error message already given
	}
	buf = buflist_findnr(i);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    semsg(_(e_cant_ਲੱਭੋ_ਬਫਰ_str), eap->arg);
	    ਵਾਪਸ;
	}
	ਜੇ (buf == curbuf)
	    ਵਾਪਸ;		// nothing to do
	idx_other = diff_buf_idx(buf);
	ਜੇ (idx_other == DB_ਗਿਣਤੀ)
	{
	    semsg(_(e_ਬਫਰ_str_is_not_ਵਿੱਚ_diff_ਬਹੁਲਕ), eap->arg);
	    ਵਾਪਸ;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	ਪੂਰਨ linestatus = 0;
	ਜੇ (eap->line1 == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	ਨਹੀਂਤਾਂ ਜੇ (eap->line1 > 0)
	    --eap->line1;
    }

    ਜੇ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_ਮੰਜ਼ਿਲ = idx_cur;
    }
    ਨਹੀਂਤਾਂ
    {
	idx_from = idx_cur;
	idx_ਮੰਜ਼ਿਲ = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	ਜੇ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    ਜਾਓ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    ਜੇ (!curbuf->b_ਬਦਲਿਆ)
    {
	change_warning(0);
	ਜੇ (diff_buf_idx(curbuf) != idx_ਮੰਜ਼ਿਲ)
	{
	    emsg(_(e_ਬਫਰ_ਬਦਲਿਆ_unexpectedly));
	    ਜਾਓ theend;
	}
    }

    dprev = ਨਿਰਾ;
    ਲਈ (dp = curtab->tp_ਪਹਿਲਾ_diff; dp != ਨਿਰਾ; )
    {
	ਜੇ (!eap->addr_ਗਿਣਤੀ)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    ਜਦੋਂ (dp->df_ਅਗਲਾ
		    && dp->df_ਅਗਲਾ->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_ਗਿਣਤੀ[idx_cur]
		    && dp->df_ਅਗਲਾ->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_ਅਗਲਾ;
	    }
	}
	ਜੇ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    ਰੋਕੋ;	// past the range that was specified

	dfree = ਨਿਰਾ;
	lnum = dp->df_lnum[idx_ਮੰਜ਼ਿਲ];
	ਗਿਣਤੀ_2 = dp->df_ਗਿਣਤੀ[idx_ਮੰਜ਼ਿਲ];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + ਗਿਣਤੀ_2 + (ਗਿਣਤੀ_2 == 0 && BUFEMPTY() ? 1 : 0);

	ਜੇ (dp->df_lnum[idx_cur] + dp->df_ਗਿਣਤੀ[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    ਸ਼ੁਰੂ_skip = 0;
	    ਸਿਰਾ_skip = 0;
	    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		ਸ਼ੁਰੂ_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		ਜੇ (ਸ਼ੁਰੂ_skip > 0)
		{
		    // range starts below start of current diff block
		    ਜੇ (ਸ਼ੁਰੂ_skip > ਗਿਣਤੀ_2)
		    {
			lnum += ਗਿਣਤੀ_2;
			ਗਿਣਤੀ_2 = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਗਿਣਤੀ_2 -= ਸ਼ੁਰੂ_skip;
			lnum += ਸ਼ੁਰੂ_skip;
		    }
		}
		ਨਹੀਂਤਾਂ
		    ਸ਼ੁਰੂ_skip = 0;

		ਸਿਰਾ_skip = dp->df_lnum[idx_cur] + dp->df_ਗਿਣਤੀ[idx_cur] - 1
							 - (eap->line2 + off);
		ਜੇ (ਸਿਰਾ_skip > 0)
		{
		    // range ends above end of current/from diff block
		    ਜੇ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_ਗਿਣਤੀ[idx_cur] - ਸ਼ੁਰੂ_skip - ਸਿਰਾ_skip;
			ਜੇ (ਗਿਣਤੀ_2 > i)
			    ਗਿਣਤੀ_2 = i;
		    }
		    ਨਹੀਂਤਾਂ			// :diffget
		    {
			ਗਿਣਤੀ_2 -= ਸਿਰਾ_skip;
			ਸਿਰਾ_skip = dp->df_ਗਿਣਤੀ[idx_from] - ਸ਼ੁਰੂ_skip - ਗਿਣਤੀ_2;
			ਜੇ (ਸਿਰਾ_skip < 0)
			    ਸਿਰਾ_skip = 0;
		    }
		}
		ਨਹੀਂਤਾਂ
		    ਸਿਰਾ_skip = 0;
	    }

	    buf_ਖਾਲੀ_ਥਾਵਾਂ = BUFEMPTY();
	    added = 0;
	    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_ਖਾਲੀ_ਥਾਵਾਂ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 1;
		ਜੇ (ml_delete(lnum) == OK)
		    --added;
	    }
	    ਲਈ (i = 0; i < dp->df_ਗਿਣਤੀ[idx_from] - ਸ਼ੁਰੂ_skip - ਸਿਰਾ_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + ਸ਼ੁਰੂ_skip + i;
		ਜੇ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    ਰੋਕੋ;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		ਜੇ (p != ਨਿਰਾ)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    ਜੇ (buf_ਖਾਲੀ_ਥਾਵਾਂ && curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_ਗਿਣਤੀ = dp->df_ਗਿਣਤੀ[idx_ਮੰਜ਼ਿਲ] + added;
	    dp->df_ਗਿਣਤੀ[idx_ਮੰਜ਼ਿਲ] = new_ਗਿਣਤੀ;

	    ਜੇ (ਸ਼ੁਰੂ_skip == 0 && ਸਿਰਾ_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
		    ਜੇ (curtab->tp_diffbuf[i] != ਨਿਰਾ && i != idx_from
								&& i != idx_ਮੰਜ਼ਿਲ
			    && !diff_equal_ਇੰਦਰਾਜ(dp, idx_from, i))
			ਰੋਕੋ;
		ਜੇ (i == DB_ਗਿਣਤੀ)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_ਅਗਲਾ;
		    ਜੇ (dprev == ਨਿਰਾ)
			curtab->tp_ਪਹਿਲਾ_diff = dp;
		    ਨਹੀਂਤਾਂ
			dprev->df_ਅਗਲਾ = dp;
		}
	    }

	    ਜੇ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + ਗਿਣਤੀ_2 - 1, (ਲੰਮਾ)MAXLNUM, (ਲੰਮਾ)added);
		ਜੇ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    ਜੇ (curwin->w_cursor.lnum >= lnum + ਗਿਣਤੀ_2)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    ਬਦਲਿਆ_ਸਤਰਾਂ(lnum, 0, lnum + ਗਿਣਤੀ_2, (ਲੰਮਾ)added);

	    ਜੇ (dfree != ਨਿਰਾ)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_ਮੰਜ਼ਿਲ);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    ਜੇ (added != 0 && !valid_diff(dp))
		ਰੋਕੋ;

	    ਜੇ (dfree == ਨਿਰਾ)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_ਗਿਣਤੀ[idx_ਮੰਜ਼ਿਲ] = new_ਗਿਣਤੀ;

	    // When changing the current buffer, keep track of line numbers
	    ਜੇ (idx_cur == idx_ਮੰਜ਼ਿਲ)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	ਜੇ (dfree == ਨਿਰਾ)
	{
	    dprev = dp;
	    dp = dp->df_ਅਗਲਾ;
	}
    }

    // restore curwin/curbuf and a few other things
    ਜੇ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	ਜੇ (ਕੁੰਜੀ_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    ਜੇ (diff_need_update)
	ex_diffupdate(ਨਿਰਾ);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    ਬਦਲਿਆ_ਸਤਰ_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    ਜੇ (curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(curtab, wp)
	    ਜੇ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    ਜੇ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    ਨਹੀਂਤਾਂ
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	ਖਿੜਕੀ_layout_ਤਾਲਾ();
	apply_autocmds(EVENT_DIFFUPDATED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਖਿੜਕੀ_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ਸਥਿਰ ਖਾਲੀ
diff_fold_update(diff_T *dp, ਪੂਰਨ skip_idx)
{
    ਪੂਰਨ		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਲਈ (i = 0; i < DB_ਗਿਣਤੀ; ++i)
	    ਜੇ (curtab->tp_diffbuf[i] == wp->w_ਬਫਰ && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_ਗਿਣਤੀ[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    ਪੂਰਨ
diff_ਬਹੁਲਕ_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ਜੇ (diff_buf_idx_tp(buf, tp) != DB_ਗਿਣਤੀ)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    ਪੂਰਨ
diff_move_ਮੰਜ਼ਿਲ(ਪੂਰਨ dir, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਪੂਰਨ		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    ਜੇ (idx == DB_ਗਿਣਤੀ || curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (curtab->tp_diff_invalid)
	ex_diffupdate(ਨਿਰਾ);		// update after a big change

    ਜੇ (curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ)		// no diffs today
	ਵਾਪਸ FAIL;

    ਜਦੋਂ (--ਗਿਣਤੀ_2 >= 0)
    {
	// Check if already before first diff.
	ਜੇ (dir == BACKWARD && lnum <= curtab->tp_ਪਹਿਲਾ_diff->df_lnum[idx])
	    ਰੋਕੋ;

	ਲਈ (dp = curtab->tp_ਪਹਿਲਾ_diff; ; dp = dp->df_ਅਗਲਾ)
	{
	    ਜੇ (dp == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਜੇ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_ਅਗਲਾ == ਨਿਰਾ
			    || lnum <= dp->df_ਅਗਲਾ->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		ਰੋਕੋ;
	    }
	}
    }

    // don't end up past the end of the file
    ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // When the cursor didn't move at all we fail.
    ਜੇ (lnum == curwin->w_cursor.lnum)
	ਵਾਪਸ FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ਖਾਨਾ = 0;

    ਵਾਪਸ OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    ਸਥਿਰ linenr_T
diff_get_corresponding_ਸਤਰ_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    ਪੂਰਨ		idx1;
    ਪੂਰਨ		idx2;
    diff_T	*dp;
    ਪੂਰਨ		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    ਜੇ (idx1 == DB_ਗਿਣਤੀ || idx2 == DB_ਗਿਣਤੀ || curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ)
	ਵਾਪਸ lnum1;

    ਜੇ (curtab->tp_diff_invalid)
	ex_diffupdate(ਨਿਰਾ);		// update after a big change

    ਜੇ (curtab->tp_ਪਹਿਲਾ_diff == ਨਿਰਾ)		// no diffs today
	ਵਾਪਸ lnum1;

    FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(curtab, dp)
    {
	ਜੇ (dp->df_lnum[idx1] > lnum1)
	    ਵਾਪਸ lnum1 - baseline;
	ਜੇ ((dp->df_lnum[idx1] + dp->df_ਗਿਣਤੀ[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    ਜੇ (baseline > dp->df_ਗਿਣਤੀ[idx2])
		baseline = dp->df_ਗਿਣਤੀ[idx2];

	    ਵਾਪਸ dp->df_lnum[idx2] + baseline;
	}
	ਜੇ (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_ਗਿਣਤੀ[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_ਗਿਣਤੀ[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    ਵਾਪਸ curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_ਗਿਣਤੀ[idx1])
				   - (dp->df_lnum[idx2] + dp->df_ਗਿਣਤੀ[idx2]);
    }

    // If we get here then the cursor is after the last diff
    ਵਾਪਸ lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_ਸਤਰ(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_ਸਤਰ_int(buf1, lnum1);

    // don't end up past the end of the file
    ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਵਾਪਸ lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    ਜੇ (idx == DB_ਗਿਣਤੀ)		// safety check
	ਵਾਪਸ (linenr_T)0;

    ਜੇ (curtab->tp_diff_invalid)
	ex_diffupdate(ਨਿਰਾ);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_ਵਿੱਚ_TAB(curtab, dp)
	ਜੇ (lnum <= dp->df_lnum[idx] + dp->df_ਗਿਣਤੀ[idx])
	    ਰੋਕੋ;

    // When after the last change, compute relative to the last line number.
    ਜੇ (dp == ਨਿਰਾ)
	ਵਾਪਸ wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
					- (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_ਬਫਰ);
    ਜੇ (i == DB_ਗਿਣਤੀ)			// safety check
	ਵਾਪਸ (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    ਜੇ (n > dp->df_lnum[i] + dp->df_ਗਿਣਤੀ[i])
	n = dp->df_lnum[i] + dp->df_ਗਿਣਤੀ[i];
    ਵਾਪਸ n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_diff_ed(
	char_u	    *ਸਤਰ,
	diffhunk_T  *hunk)
{
    char_u *p;
    ਲੰਮਾ    f1, l1, f2, l2;
    ਪੂਰਨ	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = ਸਤਰ;
    f1 = getdigits(&p);
    ਜੇ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    ਨਹੀਂਤਾਂ
	l1 = f1;
    ਜੇ (*p != 'a' && *p != 'c' && *p != 'd')
	ਵਾਪਸ FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    ਜੇ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    ਨਹੀਂਤਾਂ
	l2 = f2;
    ਜੇ (l1 < f1 || l2 < f2)
	ਵਾਪਸ FAIL;

    ਜੇ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->ਗਿਣਤੀ_orig = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	hunk->lnum_orig = f1;
	hunk->ਗਿਣਤੀ_orig = l1 - f1 + 1;
    }
    ਜੇ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->ਗਿਣਤੀ_new = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	hunk->lnum_new = f2;
	hunk->ਗਿਣਤੀ_new = l2 - f2 + 1;
    }
    ਵਾਪਸ OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ਸਥਿਰ ਪੂਰਨ
parse_diff_unified(
	char_u	    *ਸਤਰ,
	diffhunk_T  *hunk)
{
    char_u *p;
    ਲੰਮਾ    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = ਸਤਰ;
    ਜੇ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	ਜੇ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	ਨਹੀਂਤਾਂ
	    oldcount = 1;
	ਜੇ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    ਜੇ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    ਨਹੀਂਤਾਂ
		newcount = 1;
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FAIL;	// invalid diff format

	ਜੇ (oldcount == 0)
	    oldline += 1;
	ਜੇ (newcount == 0)
	    newline += 1;
	ਜੇ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->ਗਿਣਤੀ_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->ਗਿਣਤੀ_new = newcount;

	ਵਾਪਸ OK;
    }

    ਵਾਪਸ FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ਸਥਿਰ ਪੂਰਨ
xdiff_ਬਾਹਰਲਾ_indices(
	ਲੰਮਾ ਸ਼ੁਰੂ_a,
	ਲੰਮਾ ਗਿਣਤੀ_a,
	ਲੰਮਾ ਸ਼ੁਰੂ_b,
	ਲੰਮਾ ਗਿਣਤੀ_b,
	ਖਾਲੀ *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	ਵਾਪਸ -1;
    }

    p->lnum_orig  = ਸ਼ੁਰੂ_a + 1;
    p->ਗਿਣਤੀ_orig = ਗਿਣਤੀ_a;
    p->lnum_new   = ਸ਼ੁਰੂ_b + 1;
    p->ਗਿਣਤੀ_new  = ਗਿਣਤੀ_b;
    ((diffhunk_T **)dout->dout_ga.ga_ਅੰਕੜੇ)[dout->dout_ga.ga_len++] = p;
    ਵਾਪਸ 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ਸਥਿਰ ਪੂਰਨ
xdiff_ਬਾਹਰਲਾ_unified(
	ਖਾਲੀ *priv,
	mmbuffer_t *mb,
	ਪੂਰਨ nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].ਮਾਪ);

    ਵਾਪਸ 0;
}

#ਜੇ_ਅੰਤ	// FEAT_DIFF

#ਜੇ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    ਖਾਲੀ
f_diff_filler(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_DIFF
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    ਖਾਲੀ
f_diff_hlID(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ਸਥਿਰ linenr_T	prev_lnum = 0;
    ਸਥਿਰ varnumber_T	changedtick = 0;
    ਸਥਿਰ ਪੂਰਨ		fnum = 0;
    ਸਥਿਰ ਪੂਰਨ		prev_diff_flags = 0;
    ਸਥਿਰ ਪੂਰਨ		change_ਸ਼ੁਰੂ = 0;
    ਸਥਿਰ ਪੂਰਨ		change_ਸਿਰਾ = 0;
    ਸਥਿਰ hlf_T	hlID = (hlf_T)0;
    ਪੂਰਨ			cache_results = TRUE;
    ਪੂਰਨ			ਖਾਨਾ;
    diffline_T		diffline;

    CLEAR_ਖੇਤਰ(diffline);

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    ਜੇ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    ਜੇ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	ਪੂਰਨ linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	ਜੇ (linestatus < 0)
	{
	    ਜੇ (linestatus == -1)
	    {
		change_ਸ਼ੁਰੂ = MAXCOL;
		change_ਸਿਰਾ = -1;
		ਜੇ (diff_ਲੱਭੋ_change(curwin, lnum, &diffline))
		    hlID = HLF_ਜੋੜੋ;	// added line
		ਨਹੀਂਤਾਂ
		{
		    hlID = HLF_CHD;	// changed line
		    ਜੇ (diffline.ਅੰਕ_changes > 0 && cache_results)
		    {
			change_ਸ਼ੁਰੂ = diffline.changes[0].dc_ਸ਼ੁਰੂ[diffline.bufidx];
			change_ਸਿਰਾ = diffline.changes[0].dc_ਸਿਰਾ[diffline.bufidx];
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
		hlID = HLF_ਜੋੜੋ;	// added line
	}
	ਨਹੀਂਤਾਂ
	    hlID = (hlf_T)0;

	ਜੇ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    ਜੇ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	ਖਾਨਾ = tv_get_ਸੰਖਿਆ(&argvars[1]) - 1; // ignore type error in {col}
	ਜੇ (cache_results)
	{
	    ਜੇ (ਖਾਨਾ >= change_ਸ਼ੁਰੂ && ਖਾਨਾ < change_ਸਿਰਾ)
		hlID = HLF_TXD;			// changed text
	    ਨਹੀਂਤਾਂ
		hlID = HLF_CHD;			// changed line
	}
	ਨਹੀਂਤਾਂ
	{
	    hlID = HLF_CHD;
	    ਲਈ (ਪੂਰਨ i = 0; i < diffline.ਅੰਕ_changes; i++)
	    {
		ਪੂਰਨ added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_ਸ਼ੁਰੂ, &change_ਸਿਰਾ);
		ਜੇ (ਖਾਨਾ >= change_ਸ਼ੁਰੂ && ਖਾਨਾ < change_ਸਿਰਾ)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    ਰੋਕੋ;
		}
		ਜੇ (ਖਾਨਾ < change_ਸ਼ੁਰੂ)
		    // the remaining changes are past this column and not relevant
		    ਰੋਕੋ;
	    }
	}
    }
    rettv->vval.v_ਸੰਖਿਆ = hlID == (hlf_T)0 ? 0 : (ਪੂਰਨ)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ਸਥਿਰ ਪੂਰਨ
parse_diff_optarg(
    typval_T	    *opts,
    ਪੂਰਨ		    *diffopts,
    ਲੰਮਾ	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    ਪੂਰਨ		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    ਜੇ (algo != ਨਿਰਾ)
    {
	ਜੇ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_ਨਿਊਨਤਮ;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    ਜੇ (output_fmt != ਨਿਰਾ)
    {
	ਜੇ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    ਵਾਪਸ FAIL;
	}
    }

    *diff_ctxlen = dict_get_ਸੰਖਿਆ_def(d, "context", 0);
    ਜੇ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    ਜੇ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    ਜੇ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    ਜੇ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    ਜੇ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    ਜੇ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    ਜੇ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    ਵਾਪਸ OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ਸਥਿਰ ਖਾਲੀ
list_ਮੰਜ਼ਿਲ_diffin(list_T *l, diffin_T *din, ਪੂਰਨ icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	str = tv_get_string(&li->li_tv);
	ਜੇ (icase)
	{
	    str = strlow_save(str);
	    ਜੇ (str == ਨਿਰਾ)
		ਜਾਰੀ;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	ਜੇ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ;
    din->din_mmfile.ਮਾਪ = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ਸਥਿਰ dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    ਜੇ (hunk_dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(hunk_dict, "from_count", hunk->ਗਿਣਤੀ_orig);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(hunk_dict, "to_count", hunk->ਗਿਣਤੀ_new);

    ਵਾਪਸ hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    ਖਾਲੀ
f_diff(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    ਜੇ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;

    CLEAR_ਖੇਤਰ(dio);
    dio.dio_ਅੰਦਰੂਨੀ = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, ਆਕਾਰ(ਅੱਖਰ *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    ਪੂਰਨ		save_diff_flags = diff_flags;
    ਲੰਮਾ	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_ਅੰਦਰੂਨੀ;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	ਜੇ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    ਵਾਪਸ;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_ਮੰਜ਼ਿਲ_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_ਮੰਜ਼ਿਲ_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    ਪੂਰਨ	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    ਜੇ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    ਪੂਰਨ diff_ਸਥਿਤੀ = diff_file(&dio);

    // restore 'diffexpr'
    ਜੇ (restore_diffexpr)
	*p_dex = cc;

    ਜੇ (diff_ਸਥਿਤੀ == FAIL)
	ਜਾਓ ਪੂਰਾ;

    ਪੂਰਨ		hunk_idx = 0;
    dict_T	*hunk_dict;

    ਜੇ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	ਜੇ (rettv_list_alloc(rettv) != OK)
	    ਜਾਓ ਪੂਰਾ;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ਨਿਰਾ;
	ਜਦੋਂ (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_ਅੰਕੜੇ)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    ਜੇ (hunk_dict == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;

	    ਜੇ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_ਅੰਕੜੇ);
    }

ਪੂਰਾ:
    clear_diffin(&dio.dio_new);
    ਜੇ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    ਨਹੀਂਤਾਂ
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#ਜੇ_ਅੰਤ
