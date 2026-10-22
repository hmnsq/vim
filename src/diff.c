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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "xdiff/xdiff.h"

#જો defined(FEAT_DIFF)

સ્થિર પૂર્ણાંક diff_busy = FALSE;	    // using diff structs, don't change them
સ્થિર પૂર્ણાંક diff_need_update = FALSE; // ex_diffupdate needs to be called

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
સ્થિર પૂર્ણાંક	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

સ્થિર long diff_algorithm = XDF_INDENT_HEURISTIC;
સ્થિર પૂર્ણાંક diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

સ્થિર પૂર્ણાંક diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# જો defined(MSWIN)
સ્થિર પૂર્ણાંક diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
પ્રકારવ્યાખ્યા સંરચના {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
પ્રકારવ્યાખ્યા સંરચના {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
પ્રકારવ્યાખ્યા સંરચના {
    linenr_T lnum_orig;
    long     ગણતરી_orig;
    linenr_T lnum_new;
    long     ગણતરી_new;
} diffhunk_T;

પ્રકારવ્યાખ્યા ગણના {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
પ્રકારવ્યાખ્યા સંરચના {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    પૂર્ણાંક		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    પૂર્ણાંક		    dio_ctxlen;   // unified diff context length
} diffio_T;

સ્થિર પૂર્ણાંક diff_buf_idx(buf_T *buf);
સ્થિર પૂર્ણાંક diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
સ્થિર શૂન્ય diff_mark_adjust_tp(tabpage_T *tp, પૂર્ણાંક idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
સ્થિર શૂન્ય diff_check_unchanged(tabpage_T *tp, diff_T *dp);
સ્થિર પૂર્ણાંક diff_check_sanity(tabpage_T *tp, diff_T *dp);
સ્થિર પૂર્ણાંક check_external_diff(diffio_T *diffio);
સ્થિર પૂર્ણાંક diff_file(diffio_T *diffio);
સ્થિર પૂર્ણાંક diff_equal_entry(diff_T *dp, પૂર્ણાંક idx1, પૂર્ણાંક idx2);
સ્થિર પૂર્ણાંક diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
સ્થિર શૂન્ય diff_fold_update(diff_T *dp, પૂર્ણાંક skip_idx);
# endif
સ્થિર શૂન્ય diff_read(પૂર્ણાંક idx_orig, પૂર્ણાંક idx_new, diffio_T *dio);
સ્થિર શૂન્ય diff_copy_entry(diff_T *dprev, diff_T *dp, પૂર્ણાંક idx_orig, પૂર્ણાંક idx_new);
સ્થિર diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
સ્થિર પૂર્ણાંક parse_diff_ed(char_u *લીટી, diffhunk_T *hunk);
સ્થિર પૂર્ણાંક parse_diff_unified(char_u *લીટી, diffhunk_T *hunk);
સ્થિર પૂર્ણાંક xdiff_out_indices(long start_a, long ગણતરી_a, long start_b, long ગણતરી_b, શૂન્ય *priv);
સ્થિર પૂર્ણાંક xdiff_out_unified(શૂન્ય *priv, mmbuffer_t *mb, પૂર્ણાંક nbuf);
સ્થિર પૂર્ણાંક parse_diffanchors(પૂર્ણાંક check_only, buf_T *buf, linenr_T *anchors, પૂર્ણાંક *અંક_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    માટે ((dp) = (tp)->tp_first_diff; (dp) != ખાલી; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
સ્થિર diff_T	    *diff_finger_dp = ખાલી;
સ્થિર tabpage_T    *diff_finger_tp = ખાલી;
સ્થિર પૂર્ણાંક	    diff_finger_idx = -1;
સ્થિર linenr_T	    diff_finger_lnum = 0;

    સ્થિર શૂન્ય
clear_diffblock(diff_T *dp)
{
    જો (dp == diff_finger_dp)
	diff_finger_dp = ખાલી;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    સ્થિર diff_T *
diff_scan_start(tabpage_T *tp, પૂર્ણાંક idx, linenr_T lnum)
{
    જો (diff_finger_dp != ખાલી
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	પાછા diff_finger_dp;
    પાછા tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    સ્થિર શૂન્ય
diff_scan_remember(tabpage_T *tp, પૂર્ણાંક idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ખાલી ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    શૂન્ય
diff_buf_delete(buf_T *buf)
{
    પૂર્ણાંક		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	જો (i != DB_ગણતરી)
	{
	    tp->tp_diffbuf[i] = ખાલી;
	    tp->tp_diff_invalid = TRUE;
	    જો (tp == curtab)
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
    શૂન્ય
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    પૂર્ણાંક		i;

    જો (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    જો (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		તોડો;
	જો (wp == ખાલી)
	{
	    i = diff_buf_idx(win->w_buffer);
	    જો (i != DB_ગણતરી)
	    {
		curtab->tp_diffbuf[i] = ખાલી;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    નહીંતર
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
    શૂન્ય
diff_buf_add(buf_T *buf)
{
    પૂર્ણાંક		i;

    જો (diff_buf_idx(buf) != DB_ગણતરી)
	પાછા;		// It's already there.

    માટે (i = 0; i < DB_ગણતરી; ++i)
	જો (curtab->tp_diffbuf[i] == ખાલી)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    પાછા;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_ગણતરી);
}

/*
 * Remove all buffers to make diffs for.
 */
    સ્થિર શૂન્ય
diff_buf_clear(શૂન્ય)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < DB_ગણતરી; ++i)
	જો (curtab->tp_diffbuf[i] != ખાલી)
	{
	    curtab->tp_diffbuf[i] = ખાલી;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    સ્થિર પૂર્ણાંક
diff_buf_idx(buf_T *buf)
{
    પૂર્ણાંક		idx;

    માટે (idx = 0; idx < DB_ગણતરી; ++idx)
	જો (curtab->tp_diffbuf[idx] == buf)
	    તોડો;
    પાછા idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    સ્થિર પૂર્ણાંક
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    પૂર્ણાંક		idx;

    માટે (idx = 0; idx < DB_ગણતરી; ++idx)
	જો (tp->tp_diffbuf[idx] == buf)
	    તોડો;
    પાછા idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    શૂન્ય
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    પૂર્ણાંક		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	જો (i != DB_ગણતરી)
	{
	    tp->tp_diff_invalid = TRUE;
	    જો (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    શૂન્ય
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    પૂર્ણાંક		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	જો (idx != DB_ગણતરી)
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
    સ્થિર શૂન્ય
diff_mark_adjust_tp(
    tabpage_T	*tp,
    પૂર્ણાંક		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    પૂર્ણાંક		i;
    પૂર્ણાંક		inserted, deleted;
    પૂર્ણાંક		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    પૂર્ણાંક		check_unchanged;

    diff_finger_dp = ખાલી;		// may shift df_lnum, invalidate the finger

    જો (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    જો (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    નહીંતર જો (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    નહીંતર
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ખાલી;
    dp = tp->tp_first_diff;
    માટે (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	જો ((dp == ખાલી || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ખાલી
		    || dprev->df_lnum[idx] + dprev->df_ગણતરી[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    જો (dnext == ખાલી)
		પાછા;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_ગણતરી[idx] = inserted;
	    માટે (i = 0; i < DB_ગણતરી; ++i)
		જો (tp->tp_diffbuf[i] != ખાલી && i != idx)
		{
		    જો (dprev == ખાલી)
			dnext->df_lnum[i] = line1;
		    નહીંતર
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_ગણતરી[i])
			    - (dprev->df_lnum[idx] + dprev->df_ગણતરી[idx]);
		    dnext->df_ગણતરી[i] = deleted;
		}
	}

	// if at end of the list, quit
	જો (dp == ખાલી)
	    તોડો;

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
	last = dp->df_lnum[idx] + dp->df_ગણતરી[idx] - 1;

	// 1. change completely above line1: nothing to do
	જો (last >= line1 - 1)
	{
	    જો (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		જો (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		ચાલુ;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    જો (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		જો (amount_after == 0)
		    તોડો;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    નહીંતર
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		જો (deleted > 0)
		{
		    off = 0;
		    જો (dp->df_lnum[idx] >= line1)
		    {
			જો (last <= line2)
			{
			    // 4. delete all lines of diff
			    જો (dp->df_next != ખાલી
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_ગણતરી[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    નહીંતર
				n = deleted - dp->df_ગણતરી[idx];
			    dp->df_ગણતરી[idx] = 0;
			}
			નહીંતર
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_ગણતરી[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    નહીંતર
		    {
			જો (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_ગણતરી[idx] -= last - lnum_deleted + 1;
			    જો (dp->df_next != ખાલી
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    નહીંતર
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			નહીંતર
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_ગણતરી[idx] -= deleted;
			}
		    }

		    માટે (i = 0; i < DB_ગણતરી; ++i)
			જો (tp->tp_diffbuf[i] != ખાલી && i != idx)
			{
			    જો (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    નહીંતર
				dp->df_lnum[i] = 1;
			    dp->df_ગણતરી[i] += n;
			}
		}
		નહીંતર
		{
		    જો (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_ગણતરી[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    નહીંતર
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		જો (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	જો (dprev != ખાલી && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_ગણતરી[idx]
							== dp->df_lnum[idx])
	{
	    માટે (i = 0; i < DB_ગણતરી; ++i)
		જો (tp->tp_diffbuf[i] != ખાલી)
		    dprev->df_ગણતરી[i] += dp->df_ગણતરી[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	નહીંતર
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = ખાલી;
    dp = tp->tp_first_diff;
    જ્યારે (dp != ખાલી)
    {
	// All counts are zero, remove this entry.
	માટે (i = 0; i < DB_ગણતરી; ++i)
	    જો (tp->tp_diffbuf[i] != ખાલી && dp->df_ગણતરી[i] != 0)
		તોડો;
	જો (i == DB_ગણતરી)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    જો (dprev == ખાલી)
		tp->tp_first_diff = dnext;
	    નહીંતર
		dprev->df_next = dnext;
	}
	નહીંતર
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    જો (tp == curtab)
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
    સ્થિર diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    જો (dnew == ખાલી)
	પાછા ખાલી;
    diff_finger_dp = ખાલી;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    જો (dprev == ખાલી)
	tp->tp_first_diff = dnew;
    નહીંતર
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, કદ(diffline_change_T), 20);
    પાછા dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    સ્થિર શૂન્ય
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    પૂર્ણાંક		i_org;
    પૂર્ણાંક		i_new;
    પૂર્ણાંક		off_org, off_new;
    char_u	*લીટી_org;
    પૂર્ણાંક		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    માટે (i_org = 0; i_org < DB_ગણતરી; ++i_org)
	જો (tp->tp_diffbuf[i_org] != ખાલી)
	    તોડો;
    જો (i_org == DB_ગણતરી)	// safety check
	પાછા;

    જો (diff_check_sanity(tp, dp) == FAIL)
	પાછા;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    માટે (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	જ્યારે (dp->df_ગણતરી[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    જો (dir == BACKWARD)
		off_org = dp->df_ગણતરી[i_org] - 1;
	    લીટી_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    જો (લીટી_org == ખાલી)
		પાછા;
	    માટે (i_new = i_org + 1; i_new < DB_ગણતરી; ++i_new)
	    {
		જો (tp->tp_diffbuf[i_new] == ખાલી)
		    ચાલુ;
		જો (dir == BACKWARD)
		    off_new = dp->df_ગણતરી[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		જો (off_new < 0 || off_new >= dp->df_ગણતરી[i_new])
		    તોડો;
		જો (diff_cmp(લીટી_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    તોડો;
	    }
	    vim_free(લીટી_org);

	    // Stop when a line isn't equal in all diff buffers.
	    જો (i_new != DB_ગણતરી)
		તોડો;

	    // Line matched in all buffers, remove it from the diff.
	    માટે (i_new = i_org; i_new < DB_ગણતરી; ++i_new)
		જો (tp->tp_diffbuf[i_new] != ખાલી)
		{
		    જો (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_ગણતરી[i_new];
		}
	}
	જો (dir == BACKWARD)
	    તોડો;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    સ્થિર પૂર્ણાંક
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < DB_ગણતરી; ++i)
	જો (tp->tp_diffbuf[i] != ખાલી)
	    જો (dp->df_lnum[i] + dp->df_ગણતરી[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_લીટી_ગણતરી)
		પાછા FAIL;
    પાછા OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    શૂન્ય
diff_redraw(
    પૂર્ણાંક		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ખાલી;
    પૂર્ણાંક		used_max_fill_other = FALSE;
    પૂર્ણાંક		used_max_fill_curwin = FALSE;
    પૂર્ણાંક		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	જો (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    ચાલુ;

	redraw_win_later(wp, UPD_SOME_VALID);
	જો (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	જો (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	જો ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    જો (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    નહીંતર જો (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		જો (wp == curwin)
		    used_max_fill_curwin = TRUE;
		નહીંતર જો (wp_other != ખાલી)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    જો (wp_other != ખાલી && curwin->w_p_scb)
    {
	જો (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	નહીંતર જો (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    સ્થિર શૂન્ય
clear_diffin(diffin_T *din)
{
    જો (din->din_fname == ખાલી)
	VIM_CLEAR(din->din_mmfile.ptr);
    નહીંતર
	mch_remove(din->din_fname);
}

    સ્થિર શૂન્ય
clear_diffout(diffout_T *dout)
{
    જો (dout->dout_fname == ખાલી)
	ga_clear_strings(&dout->dout_ga);
    નહીંતર
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    સ્થિર પૂર્ણાંક
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    જો (end < 0)
	end = buf->b_ml.ml_લીટી_ગણતરી;

    જો (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = ખાલી;
	din->din_mmfile.size = 0;
	પાછા OK;
    }

    // xdiff requires one big block of memory with all the text.
    માટે (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    જો (ptr == ખાલી)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	પાછા FAIL;
    }
    din->din_mmfile.ptr = (અક્ષર *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    માટે (lnum = start; lnum <= end; ++lnum)
    {
	માટે (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    જો (diff_flags & DIFF_ICASE)
	    {
		પૂર્ણાંક c;
		પૂર્ણાંક	orig_len;
		પૂર્ણાંક	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		જો (*s == NL)
		    c = NUL;
		નહીંતર
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		જો (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		નહીંતર
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    જો (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    નહીંતર
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    પાછા OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    સ્થિર પૂર્ણાંક
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    પૂર્ણાંક		r;
    પૂર્ણાંક		save_ml_flags;
    char_u	*save_ff;
    પૂર્ણાંક		save_cmod_flags;

    જો (din->din_fname == ખાલી)
	પાછા diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    જો (frames_locked())
	પાછા FAIL;

    જો (end < 0)
	end = buf->b_ml.ml_લીટી_ગણતરી;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    જો (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, ખાલી,
			start, end,
			ખાલી, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    પાછા r;
}

    સ્થિર પૂર્ણાંક
lnum_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    જો (lnum1 < lnum2)
	પાછા -1;
    જો (lnum1 > lnum2)
	પાછા 1;
    પાછા 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    સ્થિર શૂન્ય
diff_try_update(
	diffio_T    *dio,
	પૂર્ણાંક	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    પૂર્ણાંક		idx_new;

    જો (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, કદ(અક્ષર *), 1000);
    }
    નહીંતર
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	જો (dio->dio_orig.din_fname == ખાલી
		|| dio->dio_new.din_fname == ખાલી
		|| dio->dio_diff.dout_fname == ખાલી)
	    જાઓ theend;
    }

    // Check external diff is actually working.
    જો (!dio->dio_internal && check_external_diff(dio) == FAIL)
	જાઓ theend;

    // :diffupdate!
    જો (eap != ખાલી && eap->forceit)
	માટે (idx_new = idx_orig; idx_new < DB_ગણતરી; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    જો (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    પૂર્ણાંક અંક_anchors = INT_MAX;
    linenr_T anchors[DB_ગણતરી][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    જો (diff_flags & DIFF_ANCHOR)
    {
	માટે (પૂર્ણાંક idx = 0; idx < DB_ગણતરી; idx++)
	{
	    જો (curtab->tp_diffbuf[idx] == ખાલી)
		ચાલુ;
	    પૂર્ણાંક buf_અંક_anchors = 0;
	    જો (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_અંક_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		અંક_anchors = 0;
		CLEAR_FIELD(anchors);
		તોડો;
	    }
	    જો (buf_અંક_anchors < અંક_anchors)
		અંક_anchors = buf_અંક_anchors;

	    જો (buf_અંક_anchors > 0)
		qsort((શૂન્ય *)anchors[idx],
			(size_t)buf_અંક_anchors,
			કદ(linenr_T),
			lnum_compare);
	}
    }
    જો (અંક_anchors == INT_MAX)
	અંક_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    માટે (પૂર્ણાંક anchor_i = 0; anchor_i <= અંક_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ખાલી;
	જો (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = ખાલી;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == અંક_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	જો (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    જો (orig_diff != ખાલી)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    જાઓ theend;
	}

	// Make a difference between the first buffer and every other.
	માટે (idx_new = idx_orig + 1; idx_new < DB_ગણતરી; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી)
		ચાલુ; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == અંક_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    જો (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		ચાલુ;
	    જો (diff_file(dio) == FAIL)
		ચાલુ;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	જો (anchor_i != 0)
	{
	    diff_finger_dp = ખાલી;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    માટે (diff_T *dp = curtab->tp_first_diff; dp != ખાલી; dp = dp->df_next)
	    {
		માટે (પૂર્ણાંક idx = 0; idx < DB_ગણતરી; idx++)
		{
		    જો (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    જો (orig_diff != ખાલી)
	    {
		diff_T *last_diff = orig_diff;
		જ્યારે (last_diff->df_next != ખાલી)
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
    પૂર્ણાંક
diff_internal(શૂન્ય)
{
    પાછા (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    સ્થિર પૂર્ણાંક
diff_internal_failed(શૂન્ય)
{
    પૂર્ણાંક idx;

    // Only need to do something when there is another buffer.
    માટે (idx = 0; idx < DB_ગણતરી; ++idx)
	જો (curtab->tp_diffbuf[idx] != ખાલી
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    શૂન્ય
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    પૂર્ણાંક		idx_orig;
    પૂર્ણાંક		idx_new;
    diffio_T	diffio;
    પૂર્ણાંક		had_diffs = curtab->tp_first_diff != ખાલી;

    જો (diff_busy)
    {
	diff_need_update = TRUE;
	પાછા;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    માટે (idx_orig = 0; idx_orig < DB_ગણતરી; ++idx_orig)
	જો (curtab->tp_diffbuf[idx_orig] != ખાલી)
	    તોડો;
    જો (idx_orig == DB_ગણતરી)
	જાઓ theend;

    // Only need to do something when there is another buffer.
    માટે (idx_new = idx_orig + 1; idx_new < DB_ગણતરી; ++idx_new)
	જો (curtab->tp_diffbuf[idx_new] != ખાલી)
	    તોડો;
    જો (idx_new == DB_ગણતરી)
	જાઓ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    જો (diffio.dio_internal && diff_internal_failed())
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
    જો (had_diffs || curtab->tp_first_diff != ખાલી)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ખાલી, ખાલી, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    સ્થિર પૂર્ણાંક
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    પૂર્ણાંક		ok;
    પૂર્ણાંક		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    માટે (;;)
    {
	ok = FALSE;
	fd = mch_fopen((અક્ષર *)diffio->dio_orig.din_fname, "w");
	જો (fd == ખાલી)
	    io_error = TRUE;
	નહીંતર
	{
	    જો (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((અક્ષર *)diffio->dio_new.din_fname, "w");
	    જો (fd == ખાલી)
		io_error = TRUE;
	    નહીંતર
	    {
		જો (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ખાલી;
		જો (diff_file(diffio) == OK)
		    fd = mch_fopen((અક્ષર *)diffio->dio_diff.dout_fname, "r");
		જો (fd == ખાલી)
		    io_error = TRUE;
		નહીંતર
		{
		    char_u	linebuf[LBUFLEN];

		    માટે (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			જો (vim_fgets(linebuf, LBUFLEN, fd))
			    તોડો;
			જો (STRNCMP(linebuf, "1c1", 3) == 0
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
	જો (*p_dex != NUL)
	    તોડો;
# endif

# જો defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	જો (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    ચાલુ;
	}
	જો (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	જો (diff_a_works != MAYBE)
	    તોડો;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	જો (ok)
	    તોડો;
    }
    જો (!ok)
    {
	જો (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# જો defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Invoke the xdiff function.
 */
    સ્થિર પૂર્ણાંક
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    જો (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    જો (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    જો (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    જો (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_લીટીઓ;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    જો (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    નહીંતર
	emit_cb.out_લીટી = xdiff_out_unified;
    જો (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	પાછા FAIL;
    }
    જો (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    સ્થિર પૂર્ણાંક
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    જો (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	પાછા OK;
    }
    નહીંતર
# endif
    // Use xdiff for generating the diff.
    જો (dio->dio_internal)
	પાછા diff_file_internal(dio);

    જો (check_restricted())
	પાછા FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    જો (cmd == ખાલી)
	પાછા FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    જો (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((અક્ષર *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# જો defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# નહીંતર
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (પૂર્ણાંક)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (શૂન્ય)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    પાછા OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    શૂન્ય
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = ખાલી;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ખાલી;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ખાલી;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ખાલી;
    પૂર્ણાંક		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ખાલી;

# ifdef FEAT_BROWSE
    જો (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ખાલી, ખાલી,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ખાલી);
	જો (browseFile == ખાલી)
	    પાછા;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    જો (tmp_orig == ખાલી || tmp_new == ખાલી)
	જાઓ theend;

    // Write the current buffer to "tmp_orig".
    જો (buf_write(curbuf, tmp_orig, ખાલી,
		(linenr_T)1, curbuf->b_ml.ml_લીટી_ગણતરી,
				     ખાલી, FALSE, FALSE, FALSE, TRUE) == FAIL)
	જાઓ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ખાલી ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    જો (esc_name == ખાલી)
	જાઓ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    જો (buf == ખાલી)
	જાઓ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    જો (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((અક્ષર *)dirbuf) != 0)
	dirbuf[0] = NUL;
    નહીંતર
    {
#  ifdef TEMPDIRNAMES
	જો (vim_tempdir != ખાલી)
	    vim_ignored = mch_chdir((અક્ષર *)vim_tempdir);
	નહીંતર
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    જો (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ખાલી ? fullname :
#  endif
		eap->arg, tmp_new);
    નહીંતર
# endif
    {
	જો (check_restricted())
	    જાઓ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((અક્ષર *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(શૂન્ય)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    જો (dirbuf[0] != NUL)
    {
	જો (mch_chdir((અક્ષર *)dirbuf) != 0)
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
    જો (mch_stat((અક્ષર *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    નહીંતર
    {
	જો (curbuf->b_fname != ખાલી)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    જો (newname != ખાલી)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	જો (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    જો (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		જો (newname != ખાલી)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    જો (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    જો (tmp_orig != ખાલી)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    જો (tmp_new != ખાલી)
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
    શૂન્ય
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

    જો (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	પાછા;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    જો (curwin == old_curwin)		// split didn't work
	પાછા;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    જો (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	જો (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_લીટી(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    શૂન્ય
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    સ્થિર શૂન્ય
set_diff_option(win_T *wp, પૂર્ણાંક કિંમત)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_કિંમત_give_err((char_u *)"diff", (long)કિંમત, ખાલી, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    શૂન્ય
diff_win_options(
    win_T	*wp,
    પૂર્ણાંક		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    જો (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    જો (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    જો (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	જો (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    જો (!wp->w_p_diff)
    {
	જો (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    જો (!wp->w_p_diff)
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
    જો (vim_strchr(p_sbo, 'h') == ખાલી)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    જો (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    શૂન્ય
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    પૂર્ણાંક		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	જો (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    જો (wp->w_p_diff_saved)
	    {

		જો (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		જો (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		જો (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    જો (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		જો (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		જો (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		જો (wp->w_p_fen)
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
    જો (eap->forceit)
	diff_buf_clear();

    જો (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    જો (!diffwin && vim_strchr(p_sbo, 'h') != ખાલી)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    સ્થિર શૂન્ય
diff_read(
    પૂર્ણાંક		idx_orig,	// idx of original file
    પૂર્ણાંક		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ખાલી;
    પૂર્ણાંક		લીટી_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ખાલી;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*લીટી;
    long	off;
    પૂર્ણાંક		i;
    પૂર્ણાંક		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ખાલી;	    // init to avoid gcc warning

    ગણના {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    જો (dout->dout_fname == ખાલી)
    {
	diffstyle = DIFF_UNIFIED;
    }
    નહીંતર
    {
	fd = mch_fopen((અક્ષર *)dout->dout_fname, "r");
	જો (fd == ખાલી)
	{
	    emsg(_(e_cannot_read_diff_output));
	    પાછા;
	}
    }

    જો (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	જો (hunk == ખાલી)
	{
	    જો (fd != ખાલી)
		fclose(fd);
	    પાછા;
	}
    }

    માટે (;;)
    {
	જો (dio->dio_internal)
	{
	    જો (લીટી_hunk_idx >= dout->dout_ga.ga_len)
		તોડો;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[લીટી_hunk_idx++];
	}
	નહીંતર
	{
	    જો (fd == ખાલી)
	    {
		જો (લીટી_hunk_idx >= dout->dout_ga.ga_len)
		    તોડો;	    // did last line
		લીટી = ((char_u **)dout->dout_ga.ga_data)[લીટી_hunk_idx++];
	    }
	    નહીંતર
	    {
		જો (vim_fgets(linebuf, LBUFLEN, fd))
		    તોડો;		// end of file
		લીટી = linebuf;
	    }

	    જો (diffstyle == DIFF_NONE)
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
		જો (SAFE_isdigit(*લીટી))
		    diffstyle = DIFF_ED;
		નહીંતર જો ((STRNCMP(લીટી, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		નહીંતર જો ((STRNCMP(લીટી, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(લીટી, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(લીટી, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		નહીંતર
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    ચાલુ;
	    }

	    જો (diffstyle == DIFF_ED)
	    {
		જો (!SAFE_isdigit(*લીટી))
		    ચાલુ;	// not the start of a diff block
		જો (parse_diff_ed(લીટી, hunk) == FAIL)
		    ચાલુ;
	    }
	    નહીંતર જો (diffstyle == DIFF_UNIFIED)
	    {
		જો (STRNCMP(લીટી, "@@ ", 3)  != 0)
		    ચાલુ;	// not the start of a diff block
		જો (parse_diff_unified(લીટી, hunk) == FAIL)
		    ચાલુ;
	    }
	    નહીંતર
	    {
		emsg(_(e_invalid_diff_format));
		તોડો;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	જ્યારે (dp != ખાલી
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_ગણતરી[idx_orig])
	{
	    જો (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	જો (dp != ખાલી
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_ગણતરી[idx_orig]
		&& hunk->lnum_orig + hunk->ગણતરી_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    માટે (dpl = dp; dpl->df_next != ખાલી; dpl = dpl->df_next)
		જો (hunk->lnum_orig + hunk->ગણતરી_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    તોડો;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    જો (off > 0)
	    {
		માટે (i = idx_orig; i < idx_new; ++i)
		    જો (curtab->tp_diffbuf[i] != ખાલી)
		    {
			dp->df_lnum[i] -= off;
			dp->df_ગણતરી[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_ગણતરી[idx_new] = hunk->ગણતરી_new;
	    }
	    નહીંતર જો (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_ગણતરી[idx_new] = hunk->ગણતરી_new - off;
	    }
	    નહીંતર
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		પૂર્ણાંક orig_size_in_dp = MIN(hunk->ગણતરી_orig,
			dp->df_lnum[idx_orig] +
			dp->df_ગણતરી[idx_orig] - hunk->lnum_orig);
		પૂર્ણાંક size_diff = hunk->ગણતરી_new - orig_size_in_dp;
		dp->df_ગણતરી[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->ગણતરી_new
		    - (dp->df_lnum[idx_new] + dp->df_ગણતરી[idx_new]);
		જો (off > 0)
		    dp->df_ગણતરી[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->ગણતરી_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_ગણતરી[idx_orig]);
	    જો (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		જો (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_ગણતરી[idx_new] += -off;
		}
		off = 0;
	    }
	    માટે (i = idx_orig; i < idx_new; ++i)
		જો (curtab->tp_diffbuf[i] != ખાલી)
		    dp->df_ગણતરી[i] = dpl->df_lnum[i] + dpl->df_ગણતરી[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    જ્યારે (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	નહીંતર
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    જો (dp == ખાલી)
		જાઓ done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_ગણતરી[idx_orig] = hunk->ગણતરી_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_ગણતરી[idx_new] = hunk->ગણતરી_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    માટે (i = idx_orig + 1; i < idx_new; ++i)
		જો (curtab->tp_diffbuf[i] != ખાલી)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    જ્યારે (dp != ખાલી)
    {
	જો (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    જો (!dio->dio_internal)
	vim_free(hunk);

    જો (fd != ખાલી)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    સ્થિર શૂન્ય
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    પૂર્ણાંક		idx_orig,
    પૂર્ણાંક		idx_new)
{
    long	off;

    જો (dprev == ખાલી)
	off = 0;
    નહીંતર
	off = (dprev->df_lnum[idx_orig] + dprev->df_ગણતરી[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_ગણતરી[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_ગણતરી[idx_new] = dp->df_ગણતરી[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    શૂન્ય
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    માટે (p = tp->tp_first_diff; p != ખાલી; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = ખાલી;
}

/*
 *  return true if the options are set to use diff linematch
 */
    સ્થિર પૂર્ણાંક
diff_linematch(diff_T *dp)
{
    જો (!(diff_flags & DIFF_LINEMATCH))
	પાછા 0;

    // are there more than three diff buffers?
    પૂર્ણાંક tsize = 0;
    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
    {
	જો (curtab->tp_diffbuf[i] != ખાલી)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    જો (dp->df_ગણતરી[i] < 0)
		પાછા FALSE;
	    tsize += dp->df_ગણતરી[i];
	}
    }

    // avoid allocating a huge array because it will lag
    પાછા tsize <= linematch_લીટીઓ;
}

    સ્થિર પૂર્ણાંક
get_max_diff_length(અચલ diff_T *dp)
{
    પૂર્ણાંક maxlength = 0;

    માટે (પૂર્ણાંક k = 0; k < DB_ગણતરી; k++)
    {
	જો (curtab->tp_diffbuf[k] != ખાલી)
	{
	    જો (dp->df_ગણતરી[k] > maxlength)
		maxlength = dp->df_ગણતરી[k];
	}
    }
    પાછા maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    સ્થિર શૂન્ય
find_ટોચ_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    પૂર્ણાંક		fromidx,
    પૂર્ણાંક		topline)
{
    diff_T	*topdiff = ખાલી;
    diff_T	*localtopdiff = ખાલી;
    પૂર્ણાંક		topdiffchange = 0;

    માટે (topdiff = curtab->tp_first_diff; topdiff != ખાલી;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	જો (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	જો (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_ગણતરી[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    જો ((*thistopdiff) == ખાલી)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	જો (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_ગણતરી[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    જો (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		તોડો;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    સ્થિર શૂન્ય
calculate_topfill_and_topline(
    અચલ પૂર્ણાંક	fromidx,
    અચલ પૂર્ણાંક	toidx,
    અચલ પૂર્ણાંક	from_topline,
    અચલ પૂર્ણાંક	from_topfill,
    પૂર્ણાંક		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ખાલી;
    diff_T	*next_adjacent_blocks = ખાલી;
    પૂર્ણાંક		virtual_લીટીઓ_passed = 0;
    પૂર્ણાંક		curlinenum_to = 1;

    find_ટોચ_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    જ્યારે (curdif && (curdif->df_lnum[fromidx] + curdif->df_ગણતરી[fromidx])
							<= from_topline)
    {
	virtual_લીટીઓ_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    જો (curdif != next_adjacent_blocks)
	virtual_લીટીઓ_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_લીટીઓ_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    જો (virtual_લીટીઓ_passed < 0)
	virtual_લીટીઓ_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    જો (thistopdiff != ખાલી) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    પૂર્ણાંક virt_લીટીઓ_ડાબું = virtual_લીટીઓ_passed;
    curdif = thistopdiff;
    જ્યારે (virt_લીટીઓ_ડાબું > 0 && curdif != ખાલી && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_લીટીઓ_ડાબું, curdif->df_ગણતરી[toidx]);
	virt_લીટીઓ_ડાબું -= MIN(virt_લીટીઓ_ડાબું, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    પૂર્ણાંક max_virt_લીટીઓ = 0;
    માટે (diff_T *dp = thistopdiff; dp != ખાલી; dp = dp->df_next)
    {
	જો (dp->df_lnum[toidx] + dp->df_ગણતરી[toidx] <= curlinenum_to)
	    max_virt_લીટીઓ += get_max_diff_length(dp);
	નહીંતર
	{
	    જો (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_લીટીઓ += curlinenum_to - dp->df_lnum[toidx];
	    તોડો;
	}
    }

    જો (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_લીટીઓ - virtual_લીટીઓ_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    સ્થિર શૂન્ય
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    અચલ પૂર્ણાંક	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    પૂર્ણાંક		લીટી_numbers[DB_ગણતરી];
    પૂર્ણાંક		outputmap[DB_ગણતરી];
    size_t	ndiffs = 0;

    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
    {
	જો (curtab->tp_diffbuf[i] != ખાલી)
	{
	    લીટી_numbers[i] = dp->df_lnum[i];
	    dp->df_ગણતરી[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    માટે (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	જો (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    માટે (પૂર્ણાંક j = 0; j < DB_ગણતરી; j++)
	    {
		જો (curtab->tp_diffbuf[j] != ખાલી)
		{
		    dp_s->df_lnum[j] = લીટી_numbers[j];
		    dp_s->df_ગણતરી[j] = 0;
		}
	    }
	}
	માટે (size_t j = 0; j < ndiffs; j++)
	{
	    જો (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_ગણતરી[outputmap[j]]++;
		લીટી_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    સ્થિર શૂન્ય
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_ગણતરી];
    અચલ mmfile_t	*diffbufs[DB_ગણતરી];
    પૂર્ણાંક			diff_length[DB_ગણતરી];
    size_t		ndiffs = 0;

    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
    {
	જો (curtab->tp_diffbuf[i] != ખાલી)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    જો (dp->df_ગણતરી[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_ગણતરી[i] - 1);
	    }
	    નહીંતર
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ખાલી;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_ગણતરી[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    પૂર્ણાંક *decisions = ખાલી;
    અચલ પૂર્ણાંક iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    માટે (size_t i = 0; i < ndiffs; i++)
	મુક્ત(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    મુક્ત(decisions);
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
    પૂર્ણાંક
diff_check_with_linestatus(win_T *wp, linenr_T lnum, પૂર્ણાંક *linestatus)
{
    પૂર્ણાંક		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    પૂર્ણાંક		maxcount;
    પૂર્ણાંક		i;
    buf_T	*buf = wp->w_buffer;
    પૂર્ણાંક		cmp;

    જો (linestatus != ખાલી)
	*linestatus = 0;

    જો (curtab->tp_diff_invalid)
	ex_diffupdate(ખાલી);		// update after a big change

    જો (curtab->tp_first_diff == ખાલી || !wp->w_p_diff)	// no diffs at all
	પાછા 0;

    // safety check: "lnum" must be a buffer line
    જો (lnum < 1 || lnum > buf->b_ml.ml_લીટી_ગણતરી + 1)
	પાછા 0;

    idx = diff_buf_idx(buf);
    જો (idx == DB_ગણતરી)
	પાછા 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    જો (hasFoldingWin(wp, lnum, ખાલી, ખાલી, TRUE, ખાલી))
	પાછા 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ખાલી;		// last block with df_lnum[idx] <= lnum
    માટે (dp = diff_scan_start(curtab, idx, lnum); dp != ખાલી; dp = dp->df_next)
    {
	જો (dp->df_lnum[idx] <= lnum)
	    le = dp;
	જો (lnum <= dp->df_lnum[idx] + dp->df_ગણતરી[idx])
	    તોડો;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    જો (dp == ખાલી || lnum < dp->df_lnum[idx])
	પાછા 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    જો (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    પૂર્ણાંક અંક_fill = 0;
    જ્યારે (lnum == dp->df_lnum[idx] + dp->df_ગણતરી[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	જો (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    અંક_fill += maxcount - dp->df_ગણતરી[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	જો (dp->df_next != ખાલી
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_ગણતરી[idx])
	    dp = dp->df_next;
	નહીંતર
	    તોડો;
    }

    જો (lnum < dp->df_lnum[idx] + dp->df_ગણતરી[idx])
    {
	પૂર્ણાંક	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	માટે (i = 0; i < DB_ગણતરી; ++i)
	    જો (i != idx && curtab->tp_diffbuf[i] != ખાલી)
	    {
		જો (dp->df_ગણતરી[i] == 0)
		    zero = TRUE;
		નહીંતર
		{
		    જો (dp->df_ગણતરી[i] != dp->df_ગણતરી[idx])
		    {
			જો (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			પાછા અંક_fill;
		    }
		    cmp = TRUE;
		}
	    }
	જો (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    માટે (i = 0; i < DB_ગણતરી; ++i)
		જો (i != idx && curtab->tp_diffbuf[i] != ખાલી
						      && dp->df_ગણતરી[i] != 0)
		    જો (!diff_equal_entry(dp, idx, i))
		    {
			જો (linestatus)
			    *linestatus = -1;
			પાછા અંક_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	જો (zero == FALSE)
	    પાછા અંક_fill;
	જો (linestatus)
	    *linestatus = -2;
	પાછા અંક_fill;
    }
    પાછા અંક_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    સ્થિર પૂર્ણાંક
diff_equal_entry(diff_T *dp, પૂર્ણાંક idx1, પૂર્ણાંક idx2)
{
    પૂર્ણાંક		i;
    char_u	*લીટી;
    પૂર્ણાંક		cmp;

    જો (dp->df_ગણતરી[idx1] != dp->df_ગણતરી[idx2])
	પાછા FALSE;
    જો (diff_check_sanity(curtab, dp) == FAIL)
	પાછા FALSE;
    માટે (i = 0; i < dp->df_ગણતરી[idx1]; ++i)
    {
	લીટી = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	જો (લીટી == ખાલી)
	    પાછા FALSE;
	cmp = diff_cmp(લીટી, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(લીટી);
	જો (cmp != 0)
	    પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    સ્થિર પૂર્ણાંક
diff_equal_char(char_u *p1, char_u *p2, પૂર્ણાંક *len)
{
    પૂર્ણાંક l  = (*mb_ptr2len)(p1);

    જો (l != (*mb_ptr2len)(p2))
	પાછા FALSE;
    જો (l > 1)
    {
	જો (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    પાછા FALSE;
	*len = l;
    }
    નહીંતર
    {
	જો ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    પાછા FALSE;
	*len = 1;
    }
    પાછા TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    સ્થિર પૂર્ણાંક
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    પૂર્ણાંક		l;

    જો ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	પાછા 0;

    જો ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	પાછા STRCMP(s1, s2);
    જો ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	પાછા MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    જ્યારે (*p1 != NUL && *p2 != NUL)
    {
	જો (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	નહીંતર
	{
	    જો (!diff_equal_char(p1, p2, &l))
		તોડો;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    જો (*p1 != NUL || *p2 != NUL)
	પાછા 1;
    પાછા 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    પૂર્ણાંક
diff_check_fill(win_T *wp, linenr_T lnum)
{
    પૂર્ણાંક		n;

    // be quick when there are no filler lines
    જો (!(diff_flags & DIFF_FILLER))
	પાછા 0;
    n = diff_check_with_linestatus(wp, lnum, ખાલી);
    જો (n <= 0)
	પાછા 0;
    પાછા n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    શૂન્ય
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    પૂર્ણાંક		fromidx;
    પૂર્ણાંક		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    જો (fromidx == DB_ગણતરી)
	પાછા;		// safety check

    જો (curtab->tp_diff_invalid)
	ex_diffupdate(ખાલી);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	જો (lnum <= dp->df_lnum[fromidx] + dp->df_ગણતરી[fromidx])
	    તોડો;
    જો (dp == ખાલી)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_લીટી_ગણતરી
				       - (frombuf->b_ml.ml_લીટી_ગણતરી - lnum);
    }
    નહીંતર
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	જો (toidx == DB_ગણતરી)
	    પાછા;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	જો (lnum >= dp->df_lnum[fromidx])
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
    જો (towin->w_topline > towin->w_buffer->b_ml.ml_લીટી_ગણતરી)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_લીટી_ગણતરી;
	towin->w_botfill = true;
    }
    જો (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_લીટી_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (શૂન્ય)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ખાલી, TRUE, ખાલી);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    સ્થિર પૂર્ણાંક
parse_diffanchors(
    પૂર્ણાંક		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    પૂર્ણાંક		*અંક_anchors)
{
    પૂર્ણાંક i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ખાલી;
    જો (check_only)
	bufwin = curwin;
    નહીંતર
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    જો (bufwin->w_buffer == buf && bufwin->w_p_diff)
		તોડો;
	જો (bufwin == ખાલી && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    પાછા FAIL;
	}
    }

    માટે (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	જો (*dia == ',') // don't allow empty values
	    પાછા FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(ખાલી, &dia, ADDR_લીટીઓ, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	જો (dia == ખાલી) // error detected
	    પાછા FAIL;
	જો (*dia != ',' && *dia != NUL)
	    પાછા FAIL;

	જો (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_લીટી_ગણતરી + 1))
	{
	    emsg(_(e_invalid_range));
	    પાછા FAIL;
	}

	જો (anchors != ખાલી)
	    anchors[i] = lnum;

	જો (*dia == ',')
	    dia++;
    }
    જો (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	પાછા FAIL;
    }
    જો (અંક_anchors != ખાલી)
	*અંક_anchors = i;
    પાછા OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    પૂર્ણાંક
diffanchors_changed(પૂર્ણાંક buflocal)
{
    પૂર્ણાંક result = parse_diffanchors(TRUE, curbuf, ખાલી, ખાલી);
    જો (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    જો (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    નહીંતર
	    {
		માટે (પૂર્ણાંક idx = 0; idx < DB_ગણતરી; ++idx)
		    જો (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			તોડો;
		    }
	    }
	}
    }
    પાછા result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    પૂર્ણાંક
diffopt_changed(શૂન્ય)
{
    char_u	*p;
    પૂર્ણાંક		diff_context_new = 6;
    પૂર્ણાંક		linematch_લીટીઓ_new = 0;
    પૂર્ણાંક		diff_flags_new = 0;
    પૂર્ણાંક		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    જ્યારે (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	જો (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	નહીંતર જો (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	નહીંતર જો (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	નહીંતર જો (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	નહીંતર જો (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	નહીંતર જો (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	નહીંતર જો (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	નહીંતર જો (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	નહીંતર જો (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	નહીંતર જો (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	નહીંતર જો (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	નહીંતર જો (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	નહીંતર જો (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	નહીંતર જો (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	નહીંતર જો (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	નહીંતર જો (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	નહીંતર જો (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    જો (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    નહીંતર જો (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    નહીંતર જો (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    નહીંતર જો (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    નહીંતર
		પાછા FAIL;
	}
	નહીંતર જો (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    જો (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    નહીંતર જો (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    નહીંતર જો (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    નહીંતર જો (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    નહીંતર
		પાછા FAIL;
	}
	નહીંતર જો (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_લીટીઓ_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	જો (*p != ',' && *p != NUL)
	    પાછા FAIL;
	જો (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    જો ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	પાછા FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    જો (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_લીટીઓ = linematch_લીટીઓ_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    પાછા OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    પૂર્ણાંક
diffopt_horizontal(શૂન્ય)
{
    પાછા (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    પૂર્ણાંક
diffopt_hiddenoff(શૂન્ય)
{
    પાછા (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    પૂર્ણાંક
diffopt_closeoff(શૂન્ય)
{
    પાછા (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    શૂન્ય
diff_update_લીટી(linenr_T lnum)
{
    પૂર્ણાંક		idx;
    diff_T	*dp;

    જો (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	પાછા;

    idx = diff_buf_idx(curbuf);
    જો (idx == DB_ગણતરી)
	પાછા;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	જો (lnum <= dp->df_lnum[idx] + dp->df_ગણતરી[idx])
	    તોડો;

    // clear the inline change cache as it's invalid
    જો (dp != ખાલી)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

સ્થિર diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    પૂર્ણાંક
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    પૂર્ણાંક *change_start,
    પૂર્ણાંક *change_end)
{
    જો (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    નહીંતર
	*change_start = change->dc_start[diffline->bufidx];
    જો (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    નહીંતર
	*change_end = change->dc_end[diffline->bufidx];

    જો (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	પાછા FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
    {
	જો (i == diffline->bufidx)
	    ચાલુ;
	જો (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    પાછા FALSE;
	}
    }
    પાછા TRUE;
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
    સ્થિર પૂર્ણાંક
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    પૂર્ણાંક		idx,
    પૂર્ણાંક		*startp,	// first char of the change
    પૂર્ણાંક		*endp)		// last char of the change
{
    char_u	*લીટી_org;
    char_u	*લીટી_new;
    પૂર્ણાંક		i;
    પૂર્ણાંક		si_org, si_new;
    પૂર્ણાંક		ei_org, ei_new;
    પૂર્ણાંક		off;
    પૂર્ણાંક		added = TRUE;
    char_u	*p1, *p2;
    પૂર્ણાંક		l;

    જો (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	લીટી_org = ખાલી;
    }
    નહીંતર
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	લીટી_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	જો (લીટી_org == ખાલી)
	    પાછા FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    માટે (i = 0; i < DB_ગણતરી; ++i)
	જો (curtab->tp_diffbuf[i] != ખાલી && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    જો (off >= dp->df_ગણતરી[i])
		ચાલુ;
	    added = FALSE;
	    જો (diff_flags & DIFF_INLINE_NONE)
		તોડો; // early terminate as we only care about the return value

	    લીટી_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    જ્યારે (લીટી_org[si_org] != NUL)
	    {
		જો (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(લીટી_org[si_org])
					      && VIM_ISWHITE(લીટી_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(લીટી_org[si_org])
					    || VIM_ISWHITE(લીટી_new[si_new]))))
		{
		    si_org = (પૂર્ણાંક)(skipwhite(લીટી_org + si_org) - લીટી_org);
		    si_new = (પૂર્ણાંક)(skipwhite(લીટી_new + si_new) - લીટી_new);
		}
		નહીંતર
		{
		    જો (!diff_equal_char(લીટી_org + si_org, લીટી_new + si_new,
									   &l))
			તોડો;
		    si_org += l;
		    si_new += l;
		}
	    }
	    જો (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(લીટી_org, લીટી_org + si_org);
		si_new -= (*mb_head_off)(લીટી_new, લીટી_new + si_new);
	    }
	    જો (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    જો (લીટી_org[si_org] != NUL || લીટી_new[si_new] != NUL)
	    {
		ei_org = (પૂર્ણાંક)STRLEN(લીટી_org);
		ei_new = (પૂર્ણાંક)STRLEN(લીટી_new);
		જ્યારે (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    જો (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(લીટી_org[ei_org])
					      && VIM_ISWHITE(લીટી_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(લીટી_org[ei_org])
					    || VIM_ISWHITE(લીટી_new[ei_new]))))
		    {
			જ્યારે (ei_org >= *startp
					     && VIM_ISWHITE(લીટી_org[ei_org]))
			    --ei_org;
			જ્યારે (ei_new >= si_new
					     && VIM_ISWHITE(લીટી_new[ei_new]))
			    --ei_new;
		    }
		    નહીંતર
		    {
			p1 = લીટી_org + ei_org;
			p2 = લીટી_new + ei_new;
			p1 -= (*mb_head_off)(લીટી_org, p1);
			p2 -= (*mb_head_off)(લીટી_new, p2);
			જો (!diff_equal_char(p1, p2, &l))
			    તોડો;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		જો (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(લીટી_org);
    પાછા added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    long byte_start;
    long અંક_bytes;
    પૂર્ણાંક lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    સ્થિર શૂન્ય
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, પૂર્ણાંક idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    પૂર્ણાંક pass = 1;
    કરો
    {
	પૂર્ણાંક has_unmerged_gaps = FALSE;
	પૂર્ણાંક has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	જ્યારે (dp!= ખાલી && dp->df_next != ખાલી)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    જો (dp->df_lnum[idx1] + dp->df_ગણતરી[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ચાલુ;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_ગણતરી[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    જો (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ચાલુ;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_ગણતરી[idx1]);
	    જો (gap <= 3)
	    {
		linenr_T max_df_ગણતરી = 0;
		માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
		    max_df_ગણતરી = MAX(max_df_ગણતરી, dp->df_ગણતરી[i] + dp->df_next->df_ગણતરી[i]);

		જો (max_df_ગણતરી >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
		    {
			dp->df_ગણતરી[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_ગણતરી[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    ચાલુ;
		}
		નહીંતર
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	જો (!has_unmerged_gaps || !has_merged_gaps)
	    તોડો;
    } જ્યારે (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    સ્થિર શૂન્ય
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, પૂર્ણાંક idx1,
				  linenr_T start_lnum)
{
    પૂર્ણાંક pass = 1;
    કરો
    {
	diff_T *dp = dp_orig;

	જ્યારે (dp != ખાલી && dp->df_next != ખાલી)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_ગણતરી[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    જો (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ચાલુ;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    જો (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ચાલુ;
	    }

	    // Calculate the gap between blocks
	    પૂર્ણાંક gap_start = entry1->byte_start + entry1->અંક_bytes;
	    પૂર્ણાંક gap_end = entry2->byte_start;
	    પૂર્ણાંક gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    જો (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		ચાલુ;
	    }

	    // Get the text between the two blocks
	    char_u *લીટી = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = લીટી + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    માટે (પૂર્ણાંક i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		પૂર્ણાંક char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		જો (char_class == 2)
		{
		    only_non_word = false;
		    તોડો;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    જો (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
		{
		    જો (curtab->tp_diffbuf[i] != ખાલી)
		    {
			// count bytes in the first block
			માટે (પૂર્ણાંક k = 0; k < dp->df_ગણતરી[i]; k++)
			{
			    પૂર્ણાંક idx = dp->df_lnum[i] + k - 1;
			    જો (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].અંક_bytes;
			}
			// count bytes in the next block
			માટે (પૂર્ણાંક k = 0; k < dp->df_next->df_ગણતરી[i]; k++)
			{
			    પૂર્ણાંક idx = dp->df_next->df_lnum[i] + k - 1;
			    જો (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].અંક_bytes;
			}
		    }
		}

		જો (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
		    {
			જો (curtab->tp_diffbuf[i] != ખાલી)
			{
			    dp->df_ગણતરી[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_ગણતરી[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    ચાલુ;
		}
	    }

	    dp = dp->df_next;
	}
    } જ્યારે (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    સ્થિર શૂન્ય
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_ગણતરી];
    garray_T	file1_str;
    garray_T	file2_str;
    પૂર્ણાંક		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, કદ(અક્ષર *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = ખાલી;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_ગણતરી]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, કદ(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
	ga_init2(&linemap[i], કદ(linemap_entry_T), 128);

    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી)
	    ચાલુ; // skip buffer that isn't loaded

	જો (dp->df_ગણતરી[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ખાલી;
	    ચાલુ;
	}

	જો (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	માટે (પૂર્ણાંક off = 0; off < dp->df_ગણતરી[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    પૂર્ણાંક in_keyword = FALSE;

	    // iwhiteeol support vars
	    પૂર્ણાંક last_white = FALSE;
	    પૂર્ણાંક eol_ga_len = -1;
	    પૂર્ણાંક eol_linemap_len = -1;
	    પૂર્ણાંક eol_numlines = -1;

	    char_u *s;
	    માટે (s = curline; *s != NUL;)
	    {
		પૂર્ણાંક new_in_keyword = FALSE;
		જો (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		જો (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		જો (VIM_ISWHITE(*s))
		{
		    જો (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			ચાલુ;
		    }
		    નહીંતર જો ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			જો (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		નહીંતર
		{
		    જો ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		પૂર્ણાંક char_len = 1;
		જો (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		નહીંતર
		{
		    char_len = mb_ptr2len(s);

		    જો (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    જો (diff_flags & DIFF_ICASE)
		    {
			પૂર્ણાંક c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			પૂર્ણાંક c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			પૂર્ણાંક c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			જો (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    નહીંતર
			ga_concat_len(curstr, s, char_len);
		}

		જો (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		જો (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.અંક_bytes = char_len;
		    જો (ga_grow(&linemap[i], 1) != OK)
			જાઓ done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		નહીંતર
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].અંક_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    જો (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    જો ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		જો (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    જો (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.અંક_bytes = કદ(NL);
		જો (ga_grow(&linemap[i], 1) != OK)
		    જાઓ done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	જો (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (અક્ષર *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	નહીંતર
	{
	    dio.dio_orig.din_mmfile.ptr = (અક્ષર *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	જો (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    પૂર્ણાંક diff_status = diff_file_internal(&dio);
	    જો (diff_status == FAIL)
		જાઓ done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    જો (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    નહીંતર જો (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    માટે (; new_diff != ખાલી; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
	{
	    જો (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		ચાલુ;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_ગણતરી[i];

	    જો (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    નહીંતર
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    જો (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    નહીંતર જો (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    નહીંતર
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].અંક_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	જો (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    જાઓ done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, કદ(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    પૂર્ણાંક
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		off;

    idx = diff_buf_idx(wp->w_buffer);
    જો (idx == DB_ગણતરી)	// cannot happen
	પાછા FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	જો (lnum < dp->df_lnum[idx] + dp->df_ગણતરી[idx])
	    તોડો;
    જો (dp == ખાલી || diff_check_sanity(curtab, dp) == FAIL)
	પાછા FALSE;

    જો (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	પાછા FALSE;
    off = lnum - dp->df_lnum[idx];

    જો (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	પૂર્ણાંક	change_start = MAXCOL;	// first col of changed area
	પૂર્ણાંક	change_end = -1;	// last col of changed area
	પૂર્ણાંક	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->અંક_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	પાછા ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    જો (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    પૂર્ણાંક અંક_changes = 0;
    પૂર્ણાંક change_idx = 0;
    diffline->changes = ખાલી;
    માટે (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	જો (change->dc_end_lnum_off[idx] < off)
	    ચાલુ;
	જો (change->dc_start_lnum_off[idx] > off)
	    તોડો;
	જો (diffline->changes == ખાલી)
	    diffline->changes = change;
	અંક_changes++;
    }
    diffline->અંક_changes = અંક_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    પૂર્ણાંક added = FALSE;
    જો (અંક_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	માટે (પૂર્ણાંક i = 0; i < DB_ગણતરી; i++)
	{
	    જો (idx == i)
		ચાલુ;
	    જો (curtab->tp_diffbuf[i] == ખાલી)
		ચાલુ;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    જો (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		તોડો;
	    }
	}
    }
    પાછા added;
}

# જો defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    પૂર્ણાંક
diff_infold(win_T *wp, linenr_T lnum)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		idx = -1;
    પૂર્ણાંક		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    જો (!wp->w_p_diff)
	પાછા FALSE;

    માટે (i = 0; i < DB_ગણતરી; ++i)
    {
	જો (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	નહીંતર જો (curtab->tp_diffbuf[i] != ખાલી)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    જો (idx == -1 || !other)
	પાછા FALSE;

    જો (curtab->tp_diff_invalid)
	ex_diffupdate(ખાલી);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    જો (curtab->tp_first_diff == ખાલી)
	પાછા TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ખાલી;		// last block with df_lnum[idx] <= lnum
    પૂર્ણાંક		result = TRUE;
    માટે (dp = diff_scan_start(curtab, idx, lnum); dp != ખાલી; dp = dp->df_next)
    {
	જો (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	જો (dp->df_lnum[idx] - diff_context > lnum)
	    તોડો;
	// If this change ends before the line we have a match.
	જો (dp->df_lnum[idx] + dp->df_ગણતરી[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    તોડો;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    પાછા result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    શૂન્ય
nv_diffgetput(પૂર્ણાંક put, long ગણતરી)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    જો (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	પાછા;
    }
# endif
    જો (ગણતરી == 0)
	ea.arg = (char_u *)"";
    નહીંતર
    {
	vim_snprintf((અક્ષર *)buf, 30, "%ld", ગણતરી);
	ea.arg = buf;
    }
    જો (put)
	ea.cmdidx = CMD_diffput;
    નહીંતર
	ea.cmdidx = CMD_diffget;
    ea.addr_ગણતરી = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    સ્થિર પૂર્ણાંક
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	જો (dp == diff)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    શૂન્ય
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    પૂર્ણાંક		ગણતરી;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    પૂર્ણાંક		idx_cur;
    પૂર્ણાંક		idx_other;
    પૂર્ણાંક		idx_from;
    પૂર્ણાંક		idx_to;
    પૂર્ણાંક		i;
    પૂર્ણાંક		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    પૂર્ણાંક		start_skip, end_skip;
    પૂર્ણાંક		new_ગણતરી;
    પૂર્ણાંક		buf_empty;
    પૂર્ણાંક		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    જો (idx_cur == DB_ગણતરી)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	પાછા;
    }

    જો (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	માટે (idx_other = 0; idx_other < DB_ગણતરી; ++idx_other)
	    જો (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ખાલી)
	    {
		જો (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    તોડો;
		found_not_ma = TRUE;
	    }
	જો (idx_other == DB_ગણતરી)
	{
	    જો (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    નહીંતર
		emsg(_(e_no_other_buffer_in_diff_mode));
	    પાછા;
	}

	// Check that there isn't a third buffer in the list
	માટે (i = idx_other + 1; i < DB_ગણતરી; ++i)
	    જો (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ખાલી
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		પાછા;
	    }
    }
    નહીંતર
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	જ્યારે (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	માટે (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	જો (eap->arg + i == p)	    // digits only
	    i = atol((અક્ષર *)eap->arg);
	નહીંતર
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    જો (i < 0)
		પાછા;		// error message already given
	}
	buf = buflist_findnr(i);
	જો (buf == ખાલી)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    પાછા;
	}
	જો (buf == curbuf)
	    પાછા;		// nothing to do
	idx_other = diff_buf_idx(buf);
	જો (idx_other == DB_ગણતરી)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    પાછા;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    જો (eap->addr_ગણતરી == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	પૂર્ણાંક linestatus = 0;
	જો (eap->line1 == curbuf->b_ml.ml_લીટી_ગણતરી
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	નહીંતર જો (eap->line1 > 0)
	    --eap->line1;
    }

    જો (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    નહીંતર
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	જો (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    જાઓ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    જો (!curbuf->b_changed)
    {
	change_warning(0);
	જો (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    જાઓ theend;
	}
    }

    dprev = ખાલી;
    માટે (dp = curtab->tp_first_diff; dp != ખાલી; )
    {
	જો (!eap->addr_ગણતરી)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    જ્યારે (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_ગણતરી[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	જો (dp->df_lnum[idx_cur] > eap->line2 + off)
	    તોડો;	// past the range that was specified

	dfree = ખાલી;
	lnum = dp->df_lnum[idx_to];
	ગણતરી = dp->df_ગણતરી[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + ગણતરી + (ગણતરી == 0 && BUFEMPTY() ? 1 : 0);

	જો (dp->df_lnum[idx_cur] + dp->df_ગણતરી[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    જો (eap->addr_ગણતરી > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		જો (start_skip > 0)
		{
		    // range starts below start of current diff block
		    જો (start_skip > ગણતરી)
		    {
			lnum += ગણતરી;
			ગણતરી = 0;
		    }
		    નહીંતર
		    {
			ગણતરી -= start_skip;
			lnum += start_skip;
		    }
		}
		નહીંતર
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_ગણતરી[idx_cur] - 1
							 - (eap->line2 + off);
		જો (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    જો (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_ગણતરી[idx_cur] - start_skip - end_skip;
			જો (ગણતરી > i)
			    ગણતરી = i;
		    }
		    નહીંતર			// :diffget
		    {
			ગણતરી -= end_skip;
			end_skip = dp->df_ગણતરી[idx_from] - start_skip - ગણતરી;
			જો (end_skip < 0)
			    end_skip = 0;
		    }
		}
		નહીંતર
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    માટે (i = 0; i < ગણતરી; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_લીટી_ગણતરી == 1;
		જો (ml_delete(lnum) == OK)
		    --added;
	    }
	    માટે (i = 0; i < dp->df_ગણતરી[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		જો (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_લીટી_ગણતરી)
		    તોડો;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		જો (p != ખાલી)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    જો (buf_empty && curbuf->b_ml.ml_લીટી_ગણતરી == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_ગણતરી = dp->df_ગણતરી[idx_to] + added;
	    dp->df_ગણતરી[idx_to] = new_ગણતરી;

	    જો (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		માટે (i = 0; i < DB_ગણતરી; ++i)
		    જો (curtab->tp_diffbuf[i] != ખાલી && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			તોડો;
		જો (i == DB_ગણતરી)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    જો (dprev == ખાલી)
			curtab->tp_first_diff = dp;
		    નહીંતર
			dprev->df_next = dp;
		}
	    }

	    જો (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + ગણતરી - 1, (long)MAXLNUM, (long)added);
		જો (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    જો (curwin->w_cursor.lnum >= lnum + ગણતરી)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_લીટી_ગણતરી);
		    }
		    નહીંતર જો (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_લીટીઓ(lnum, 0, lnum + ગણતરી, (long)added);

	    જો (dfree != ખાલી)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    જો (added != 0 && !valid_diff(dp))
		તોડો;

	    જો (dfree == ખાલી)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_ગણતરી[idx_to] = new_ગણતરી;

	    // When changing the current buffer, keep track of line numbers
	    જો (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	જો (dfree == ખાલી)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    જો (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	જો (ચાવી_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    જો (diff_need_update)
	ex_diffupdate(ખાલી);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_લીટી_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    જો (curtab->tp_first_diff == ખાલી)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    જો (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    જો (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    નહીંતર
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ખાલી, ખાલી, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    સ્થિર શૂન્ય
diff_fold_update(diff_T *dp, પૂર્ણાંક skip_idx)
{
    પૂર્ણાંક		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	માટે (i = 0; i < DB_ગણતરી; ++i)
	    જો (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_ગણતરી[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    પૂર્ણાંક
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	જો (diff_buf_idx_tp(buf, tp) != DB_ગણતરી)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    પૂર્ણાંક
diff_move_to(પૂર્ણાંક dir, long ગણતરી)
{
    પૂર્ણાંક		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    જો (idx == DB_ગણતરી || curtab->tp_first_diff == ખાલી)
	પાછા FAIL;

    જો (curtab->tp_diff_invalid)
	ex_diffupdate(ખાલી);		// update after a big change

    જો (curtab->tp_first_diff == ખાલી)		// no diffs today
	પાછા FAIL;

    જ્યારે (--ગણતરી >= 0)
    {
	// Check if already before first diff.
	જો (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    તોડો;

	માટે (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    જો (dp == ખાલી)
		તોડો;
	    જો ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == ખાલી
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		તોડો;
	    }
	}
    }

    // don't end up past the end of the file
    જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	lnum = curbuf->b_ml.ml_લીટી_ગણતરી;

    // When the cursor didn't move at all we fail.
    જો (lnum == curwin->w_cursor.lnum)
	પાછા FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    પાછા OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    સ્થિર linenr_T
diff_get_corresponding_લીટી_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    પૂર્ણાંક		idx1;
    પૂર્ણાંક		idx2;
    diff_T	*dp;
    પૂર્ણાંક		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    જો (idx1 == DB_ગણતરી || idx2 == DB_ગણતરી || curtab->tp_first_diff == ખાલી)
	પાછા lnum1;

    જો (curtab->tp_diff_invalid)
	ex_diffupdate(ખાલી);		// update after a big change

    જો (curtab->tp_first_diff == ખાલી)		// no diffs today
	પાછા lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	જો (dp->df_lnum[idx1] > lnum1)
	    પાછા lnum1 - baseline;
	જો ((dp->df_lnum[idx1] + dp->df_ગણતરી[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    જો (baseline > dp->df_ગણતરી[idx2])
		baseline = dp->df_ગણતરી[idx2];

	    પાછા dp->df_lnum[idx2] + baseline;
	}
	જો (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_ગણતરી[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_ગણતરી[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    પાછા curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_ગણતરી[idx1])
				   - (dp->df_lnum[idx2] + dp->df_ગણતરી[idx2]);
    }

    // If we get here then the cursor is after the last diff
    પાછા lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_લીટી(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_લીટી_int(buf1, lnum1);

    // don't end up past the end of the file
    જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	પાછા curbuf->b_ml.ml_લીટી_ગણતરી;
    પાછા lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    જો (idx == DB_ગણતરી)		// safety check
	પાછા (linenr_T)0;

    જો (curtab->tp_diff_invalid)
	ex_diffupdate(ખાલી);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	જો (lnum <= dp->df_lnum[idx] + dp->df_ગણતરી[idx])
	    તોડો;

    // When after the last change, compute relative to the last line number.
    જો (dp == ખાલી)
	પાછા wp->w_buffer->b_ml.ml_લીટી_ગણતરી
					- (curbuf->b_ml.ml_લીટી_ગણતરી - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    જો (i == DB_ગણતરી)			// safety check
	પાછા (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    જો (n > dp->df_lnum[i] + dp->df_ગણતરી[i])
	n = dp->df_lnum[i] + dp->df_ગણતરી[i];
    પાછા n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    સ્થિર પૂર્ણાંક
parse_diff_ed(
	char_u	    *લીટી,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    પૂર્ણાંક	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = લીટી;
    f1 = getdigits(&p);
    જો (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    નહીંતર
	l1 = f1;
    જો (*p != 'a' && *p != 'c' && *p != 'd')
	પાછા FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    જો (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    નહીંતર
	l2 = f2;
    જો (l1 < f1 || l2 < f2)
	પાછા FAIL;

    જો (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->ગણતરી_orig = 0;
    }
    નહીંતર
    {
	hunk->lnum_orig = f1;
	hunk->ગણતરી_orig = l1 - f1 + 1;
    }
    જો (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->ગણતરી_new = 0;
    }
    નહીંતર
    {
	hunk->lnum_new = f2;
	hunk->ગણતરી_new = l2 - f2 + 1;
    }
    પાછા OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    સ્થિર પૂર્ણાંક
parse_diff_unified(
	char_u	    *લીટી,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = લીટી;
    જો (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	જો (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	નહીંતર
	    oldcount = 1;
	જો (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    જો (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    નહીંતર
		newcount = 1;
	}
	નહીંતર
	    પાછા FAIL;	// invalid diff format

	જો (oldcount == 0)
	    oldline += 1;
	જો (newcount == 0)
	    newline += 1;
	જો (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->ગણતરી_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->ગણતરી_new = newcount;

	પાછા OK;
    }

    પાછા FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    સ્થિર પૂર્ણાંક
xdiff_out_indices(
	long start_a,
	long ગણતરી_a,
	long start_b,
	long ગણતરી_b,
	શૂન્ય *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    જો (p == ખાલી)
	પાછા -1;

    જો (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	પાછા -1;
    }

    p->lnum_orig  = start_a + 1;
    p->ગણતરી_orig = ગણતરી_a;
    p->lnum_new   = start_b + 1;
    p->ગણતરી_new  = ગણતરી_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    પાછા 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    સ્થિર પૂર્ણાંક
xdiff_out_unified(
	શૂન્ય *priv,
	mmbuffer_t *mb,
	પૂર્ણાંક nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    પૂર્ણાંક		i;

    માટે (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    પાછા 0;
}

#endif	// FEAT_DIFF

#જો defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    શૂન્ય
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    શૂન્ય
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    સ્થિર linenr_T	prev_lnum = 0;
    સ્થિર varnumber_T	changedtick = 0;
    સ્થિર પૂર્ણાંક		fnum = 0;
    સ્થિર પૂર્ણાંક		prev_diff_flags = 0;
    સ્થિર પૂર્ણાંક		change_start = 0;
    સ્થિર પૂર્ણાંક		change_end = 0;
    સ્થિર hlf_T	hlID = (hlf_T)0;
    પૂર્ણાંક			cache_results = TRUE;
    પૂર્ણાંક			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    જો (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    જો (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	પૂર્ણાંક linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	જો (linestatus < 0)
	{
	    જો (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		જો (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		નહીંતર
		{
		    hlID = HLF_CHD;	// changed line
		    જો (diffline.અંક_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    નહીંતર
		hlID = HLF_ADD;	// added line
	}
	નહીંતર
	    hlID = (hlf_T)0;

	જો (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    જો (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_સંખ્યા(&argvars[1]) - 1; // ignore type error in {col}
	જો (cache_results)
	{
	    જો (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    નહીંતર
		hlID = HLF_CHD;			// changed line
	}
	નહીંતર
	{
	    hlID = HLF_CHD;
	    માટે (પૂર્ણાંક i = 0; i < diffline.અંક_changes; i++)
	    {
		પૂર્ણાંક added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		જો (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    તોડો;
		}
		જો (col < change_start)
		    // the remaining changes are past this column and not relevant
		    તોડો;
	    }
	}
    }
    rettv->vval.v_સંખ્યા = hlID == (hlf_T)0 ? 0 : (પૂર્ણાંક)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    સ્થિર પૂર્ણાંક
parse_diff_optarg(
    typval_T	    *opts,
    પૂર્ણાંક		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    પૂર્ણાંક		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    જો (algo != ખાલી)
    {
	જો (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	નહીંતર જો (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	નહીંતર જો (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	નહીંતર જો (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    જો (output_fmt != ખાલી)
    {
	જો (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	નહીંતર જો (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	નહીંતર
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    પાછા FAIL;
	}
    }

    *diff_ctxlen = dict_get_સંખ્યા_def(d, "context", 0);
    જો (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    જો (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    જો (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    જો (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    જો (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    જો (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    જો (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    પાછા OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    સ્થિર શૂન્ય
list_to_diffin(list_T *l, diffin_T *din, પૂર્ણાંક icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	str = tv_get_string(&li->li_tv);
	જો (icase)
	{
	    str = strlow_save(str);
	    જો (str == ખાલી)
		ચાલુ;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	જો (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (અક્ષર *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    સ્થિર dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    જો (hunk_dict == ખાલી)
	પાછા ખાલી;

    dict_add_સંખ્યા(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_સંખ્યા(hunk_dict, "from_count", hunk->ગણતરી_orig);
    dict_add_સંખ્યા(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_સંખ્યા(hunk_dict, "to_count", hunk->ગણતરી_new);

    પાછા hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    શૂન્ય
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    જો (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	પાછા;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, કદ(અક્ષર *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    પૂર્ણાંક		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    જો (argvars[2].v_type != VAR_UNKNOWN)
	જો (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    પાછા;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    પૂર્ણાંક	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    જો (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    પૂર્ણાંક diff_status = diff_file(&dio);

    // restore 'diffexpr'
    જો (restore_diffexpr)
	*p_dex = cc;

    જો (diff_status == FAIL)
	જાઓ done;

    પૂર્ણાંક		hunk_idx = 0;
    dict_T	*hunk_dict;

    જો (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	જો (rettv_list_alloc(rettv) != OK)
	    જાઓ done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ખાલી;
	જ્યારે (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    જો (hunk_dict == ખાલી)
		જાઓ done;

	    જો (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    નહીંતર
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    જો (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    નહીંતર
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
