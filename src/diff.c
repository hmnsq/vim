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

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "xdiff/xdiff.h"

#যদি defined(FEAT_DIFF)

স্থির পূর্ণাঙ্ক diff_busy = FALSE;	    // using diff structs, don't change them
স্থির পূর্ণাঙ্ক diff_need_update = FALSE; // ex_diffupdate needs to be called

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
স্থির পূর্ণাঙ্ক	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

স্থির long diff_algorithm = XDF_INDENT_HEURISTIC;
স্থির পূর্ণাঙ্ক diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

স্থির পূর্ণাঙ্ক diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# যদি defined(MSWIN)
স্থির পূর্ণাঙ্ক diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
প্রকারনির্দেশ সংরচনা {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
প্রকারনির্দেশ সংরচনা {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
প্রকারনির্দেশ সংরচনা {
    linenr_T lnum_orig;
    long     গুনতি_orig;
    linenr_T lnum_new;
    long     গুনতি_new;
} diffhunk_T;

প্রকারনির্দেশ গণনা {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
প্রকারনির্দেশ সংরচনা {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    পূর্ণাঙ্ক		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    পূর্ণাঙ্ক		    dio_ctxlen;   // unified diff context length
} diffio_T;

স্থির পূর্ণাঙ্ক diff_buf_idx(buf_T *buf);
স্থির পূর্ণাঙ্ক diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
স্থির শূন্য diff_mark_adjust_tp(tabpage_T *tp, পূর্ণাঙ্ক idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
স্থির শূন্য diff_check_unchanged(tabpage_T *tp, diff_T *dp);
স্থির পূর্ণাঙ্ক diff_check_sanity(tabpage_T *tp, diff_T *dp);
স্থির পূর্ণাঙ্ক check_external_diff(diffio_T *diffio);
স্থির পূর্ণাঙ্ক diff_file(diffio_T *diffio);
স্থির পূর্ণাঙ্ক diff_equal_entry(diff_T *dp, পূর্ণাঙ্ক idx1, পূর্ণাঙ্ক idx2);
স্থির পূর্ণাঙ্ক diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
স্থির শূন্য diff_fold_update(diff_T *dp, পূর্ণাঙ্ক skip_idx);
# endif
স্থির শূন্য diff_read(পূর্ণাঙ্ক idx_orig, পূর্ণাঙ্ক idx_new, diffio_T *dio);
স্থির শূন্য diff_copy_entry(diff_T *dprev, diff_T *dp, পূর্ণাঙ্ক idx_orig, পূর্ণাঙ্ক idx_new);
স্থির diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
স্থির পূর্ণাঙ্ক parse_diff_ed(char_u *সাৰি, diffhunk_T *hunk);
স্থির পূর্ণাঙ্ক parse_diff_unified(char_u *সাৰি, diffhunk_T *hunk);
স্থির পূর্ণাঙ্ক xdiff_out_indices(long start_a, long গুনতি_a, long start_b, long গুনতি_b, শূন্য *priv);
স্থির পূর্ণাঙ্ক xdiff_out_unified(শূন্য *priv, mmbuffer_t *mb, পূর্ণাঙ্ক nbuf);
স্থির পূর্ণাঙ্ক parse_diffanchors(পূর্ণাঙ্ক check_only, buf_T *buf, linenr_T *anchors, পূর্ণাঙ্ক *অঙ্ক_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    বাবে ((dp) = (tp)->tp_first_diff; (dp) != রিক্ত; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
স্থির diff_T	    *diff_finger_dp = রিক্ত;
স্থির tabpage_T    *diff_finger_tp = রিক্ত;
স্থির পূর্ণাঙ্ক	    diff_finger_idx = -1;
স্থির linenr_T	    diff_finger_lnum = 0;

    স্থির শূন্য
clear_diffblock(diff_T *dp)
{
    যদি (dp == diff_finger_dp)
	diff_finger_dp = রিক্ত;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    স্থির diff_T *
diff_scan_start(tabpage_T *tp, পূর্ণাঙ্ক idx, linenr_T lnum)
{
    যদি (diff_finger_dp != রিক্ত
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	ঘূৰাই diff_finger_dp;
    ঘূৰাই tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    স্থির শূন্য
diff_scan_remember(tabpage_T *tp, পূর্ণাঙ্ক idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != রিক্ত ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    শূন্য
diff_buf_delete(buf_T *buf)
{
    পূর্ণাঙ্ক		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	যদি (i != DB_গুনতি)
	{
	    tp->tp_diffbuf[i] = রিক্ত;
	    tp->tp_diff_invalid = TRUE;
	    যদি (tp == curtab)
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
    শূন্য
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    পূর্ণাঙ্ক		i;

    যদি (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    যদি (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		ভঙ্গ;
	যদি (wp == রিক্ত)
	{
	    i = diff_buf_idx(win->w_buffer);
	    যদি (i != DB_গুনতি)
	    {
		curtab->tp_diffbuf[i] = রিক্ত;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    অন্যথা
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
    শূন্য
diff_buf_add(buf_T *buf)
{
    পূর্ণাঙ্ক		i;

    যদি (diff_buf_idx(buf) != DB_গুনতি)
	ঘূৰাই;		// It's already there.

    বাবে (i = 0; i < DB_গুনতি; ++i)
	যদি (curtab->tp_diffbuf[i] == রিক্ত)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    ঘূৰাই;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_গুনতি);
}

/*
 * Remove all buffers to make diffs for.
 */
    স্থির শূন্য
diff_buf_clear(শূন্য)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < DB_গুনতি; ++i)
	যদি (curtab->tp_diffbuf[i] != রিক্ত)
	{
	    curtab->tp_diffbuf[i] = রিক্ত;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    স্থির পূর্ণাঙ্ক
diff_buf_idx(buf_T *buf)
{
    পূর্ণাঙ্ক		idx;

    বাবে (idx = 0; idx < DB_গুনতি; ++idx)
	যদি (curtab->tp_diffbuf[idx] == buf)
	    ভঙ্গ;
    ঘূৰাই idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    স্থির পূর্ণাঙ্ক
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    পূর্ণাঙ্ক		idx;

    বাবে (idx = 0; idx < DB_গুনতি; ++idx)
	যদি (tp->tp_diffbuf[idx] == buf)
	    ভঙ্গ;
    ঘূৰাই idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    শূন্য
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    পূর্ণাঙ্ক		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	যদি (i != DB_গুনতি)
	{
	    tp->tp_diff_invalid = TRUE;
	    যদি (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    শূন্য
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    পূর্ণাঙ্ক		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	যদি (idx != DB_গুনতি)
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
    স্থির শূন্য
diff_mark_adjust_tp(
    tabpage_T	*tp,
    পূর্ণাঙ্ক		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		inserted, deleted;
    পূর্ণাঙ্ক		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    পূর্ণাঙ্ক		check_unchanged;

    diff_finger_dp = রিক্ত;		// may shift df_lnum, invalidate the finger

    যদি (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    যদি (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    অন্যথা যদি (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    অন্যথা
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = রিক্ত;
    dp = tp->tp_first_diff;
    বাবে (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	যদি ((dp == রিক্ত || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == রিক্ত
		    || dprev->df_lnum[idx] + dprev->df_গুনতি[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    যদি (dnext == রিক্ত)
		ঘূৰাই;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_গুনতি[idx] = inserted;
	    বাবে (i = 0; i < DB_গুনতি; ++i)
		যদি (tp->tp_diffbuf[i] != রিক্ত && i != idx)
		{
		    যদি (dprev == রিক্ত)
			dnext->df_lnum[i] = line1;
		    অন্যথা
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_গুনতি[i])
			    - (dprev->df_lnum[idx] + dprev->df_গুনতি[idx]);
		    dnext->df_গুনতি[i] = deleted;
		}
	}

	// if at end of the list, quit
	যদি (dp == রিক্ত)
	    ভঙ্গ;

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
	last = dp->df_lnum[idx] + dp->df_গুনতি[idx] - 1;

	// 1. change completely above line1: nothing to do
	যদি (last >= line1 - 1)
	{
	    যদি (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		যদি (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		অনুৱর্ত;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    যদি (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		যদি (amount_after == 0)
		    ভঙ্গ;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    অন্যথা
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		যদি (deleted > 0)
		{
		    off = 0;
		    যদি (dp->df_lnum[idx] >= line1)
		    {
			যদি (last <= line2)
			{
			    // 4. delete all lines of diff
			    যদি (dp->df_next != রিক্ত
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_গুনতি[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    অন্যথা
				n = deleted - dp->df_গুনতি[idx];
			    dp->df_গুনতি[idx] = 0;
			}
			অন্যথা
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_গুনতি[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    অন্যথা
		    {
			যদি (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_গুনতি[idx] -= last - lnum_deleted + 1;
			    যদি (dp->df_next != রিক্ত
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    অন্যথা
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			অন্যথা
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_গুনতি[idx] -= deleted;
			}
		    }

		    বাবে (i = 0; i < DB_গুনতি; ++i)
			যদি (tp->tp_diffbuf[i] != রিক্ত && i != idx)
			{
			    যদি (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    অন্যথা
				dp->df_lnum[i] = 1;
			    dp->df_গুনতি[i] += n;
			}
		}
		অন্যথা
		{
		    যদি (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_গুনতি[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    অন্যথা
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		যদি (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	যদি (dprev != রিক্ত && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_গুনতি[idx]
							== dp->df_lnum[idx])
	{
	    বাবে (i = 0; i < DB_গুনতি; ++i)
		যদি (tp->tp_diffbuf[i] != রিক্ত)
		    dprev->df_গুনতি[i] += dp->df_গুনতি[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	অন্যথা
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = রিক্ত;
    dp = tp->tp_first_diff;
    যেতিয়ালৈ (dp != রিক্ত)
    {
	// All counts are zero, remove this entry.
	বাবে (i = 0; i < DB_গুনতি; ++i)
	    যদি (tp->tp_diffbuf[i] != রিক্ত && dp->df_গুনতি[i] != 0)
		ভঙ্গ;
	যদি (i == DB_গুনতি)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    যদি (dprev == রিক্ত)
		tp->tp_first_diff = dnext;
	    অন্যথা
		dprev->df_next = dnext;
	}
	অন্যথা
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    যদি (tp == curtab)
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
    স্থির diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    যদি (dnew == রিক্ত)
	ঘূৰাই রিক্ত;
    diff_finger_dp = রিক্ত;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    যদি (dprev == রিক্ত)
	tp->tp_first_diff = dnew;
    অন্যথা
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, পরিমাণ(diffline_change_T), 20);
    ঘূৰাই dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    স্থির শূন্য
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    পূর্ণাঙ্ক		i_org;
    পূর্ণাঙ্ক		i_new;
    পূর্ণাঙ্ক		off_org, off_new;
    char_u	*সাৰি_org;
    পূর্ণাঙ্ক		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    বাবে (i_org = 0; i_org < DB_গুনতি; ++i_org)
	যদি (tp->tp_diffbuf[i_org] != রিক্ত)
	    ভঙ্গ;
    যদি (i_org == DB_গুনতি)	// safety check
	ঘূৰাই;

    যদি (diff_check_sanity(tp, dp) == FAIL)
	ঘূৰাই;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    বাবে (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	যেতিয়ালৈ (dp->df_গুনতি[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    যদি (dir == BACKWARD)
		off_org = dp->df_গুনতি[i_org] - 1;
	    সাৰি_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    যদি (সাৰি_org == রিক্ত)
		ঘূৰাই;
	    বাবে (i_new = i_org + 1; i_new < DB_গুনতি; ++i_new)
	    {
		যদি (tp->tp_diffbuf[i_new] == রিক্ত)
		    অনুৱর্ত;
		যদি (dir == BACKWARD)
		    off_new = dp->df_গুনতি[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		যদি (off_new < 0 || off_new >= dp->df_গুনতি[i_new])
		    ভঙ্গ;
		যদি (diff_cmp(সাৰি_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    ভঙ্গ;
	    }
	    vim_free(সাৰি_org);

	    // Stop when a line isn't equal in all diff buffers.
	    যদি (i_new != DB_গুনতি)
		ভঙ্গ;

	    // Line matched in all buffers, remove it from the diff.
	    বাবে (i_new = i_org; i_new < DB_গুনতি; ++i_new)
		যদি (tp->tp_diffbuf[i_new] != রিক্ত)
		{
		    যদি (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_গুনতি[i_new];
		}
	}
	যদি (dir == BACKWARD)
	    ভঙ্গ;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    স্থির পূর্ণাঙ্ক
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < DB_গুনতি; ++i)
	যদি (tp->tp_diffbuf[i] != রিক্ত)
	    যদি (dp->df_lnum[i] + dp->df_গুনতি[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_সাৰি_গুনতি)
		ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    শূন্য
diff_redraw(
    পূর্ণাঙ্ক		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = রিক্ত;
    পূর্ণাঙ্ক		used_max_fill_other = FALSE;
    পূর্ণাঙ্ক		used_max_fill_curwin = FALSE;
    পূর্ণাঙ্ক		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	যদি (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    অনুৱর্ত;

	redraw_win_later(wp, UPD_SOME_VALID);
	যদি (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	যদি (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	যদি ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    যদি (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    অন্যথা যদি (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		যদি (wp == curwin)
		    used_max_fill_curwin = TRUE;
		অন্যথা যদি (wp_other != রিক্ত)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    যদি (wp_other != রিক্ত && curwin->w_p_scb)
    {
	যদি (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	অন্যথা যদি (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    স্থির শূন্য
clear_diffin(diffin_T *din)
{
    যদি (din->din_fname == রিক্ত)
	VIM_CLEAR(din->din_mmfile.ptr);
    অন্যথা
	mch_remove(din->din_fname);
}

    স্থির শূন্য
clear_diffout(diffout_T *dout)
{
    যদি (dout->dout_fname == রিক্ত)
	ga_clear_strings(&dout->dout_ga);
    অন্যথা
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    স্থির পূর্ণাঙ্ক
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    যদি (end < 0)
	end = buf->b_ml.ml_সাৰি_গুনতি;

    যদি (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = রিক্ত;
	din->din_mmfile.size = 0;
	ঘূৰাই OK;
    }

    // xdiff requires one big block of memory with all the text.
    বাবে (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    যদি (ptr == রিক্ত)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	ঘূৰাই FAIL;
    }
    din->din_mmfile.ptr = (ৱর্ণ *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    বাবে (lnum = start; lnum <= end; ++lnum)
    {
	বাবে (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    যদি (diff_flags & DIFF_ICASE)
	    {
		পূর্ণাঙ্ক c;
		পূর্ণাঙ্ক	orig_len;
		পূর্ণাঙ্ক	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		যদি (*s == NL)
		    c = NUL;
		অন্যথা
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		যদি (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		অন্যথা
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    যদি (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    অন্যথা
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    ঘূৰাই OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    স্থির পূর্ণাঙ্ক
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    পূর্ণাঙ্ক		r;
    পূর্ণাঙ্ক		save_ml_flags;
    char_u	*save_ff;
    পূর্ণাঙ্ক		save_cmod_flags;

    যদি (din->din_fname == রিক্ত)
	ঘূৰাই diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    যদি (frames_locked())
	ঘূৰাই FAIL;

    যদি (end < 0)
	end = buf->b_ml.ml_সাৰি_গুনতি;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    যদি (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, রিক্ত,
			start, end,
			রিক্ত, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    ঘূৰাই r;
}

    স্থির পূর্ণাঙ্ক
lnum_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    যদি (lnum1 < lnum2)
	ঘূৰাই -1;
    যদি (lnum1 > lnum2)
	ঘূৰাই 1;
    ঘূৰাই 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    স্থির শূন্য
diff_try_update(
	diffio_T    *dio,
	পূর্ণাঙ্ক	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    পূর্ণাঙ্ক		idx_new;

    যদি (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, পরিমাণ(ৱর্ণ *), 1000);
    }
    অন্যথা
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	যদি (dio->dio_orig.din_fname == রিক্ত
		|| dio->dio_new.din_fname == রিক্ত
		|| dio->dio_diff.dout_fname == রিক্ত)
	    গচ্ছ theend;
    }

    // Check external diff is actually working.
    যদি (!dio->dio_internal && check_external_diff(dio) == FAIL)
	গচ্ছ theend;

    // :diffupdate!
    যদি (eap != রিক্ত && eap->forceit)
	বাবে (idx_new = idx_orig; idx_new < DB_গুনতি; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    যদি (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    পূর্ণাঙ্ক অঙ্ক_anchors = INT_MAX;
    linenr_T anchors[DB_গুনতি][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    যদি (diff_flags & DIFF_ANCHOR)
    {
	বাবে (পূর্ণাঙ্ক idx = 0; idx < DB_গুনতি; idx++)
	{
	    যদি (curtab->tp_diffbuf[idx] == রিক্ত)
		অনুৱর্ত;
	    পূর্ণাঙ্ক buf_অঙ্ক_anchors = 0;
	    যদি (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_অঙ্ক_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		অঙ্ক_anchors = 0;
		CLEAR_FIELD(anchors);
		ভঙ্গ;
	    }
	    যদি (buf_অঙ্ক_anchors < অঙ্ক_anchors)
		অঙ্ক_anchors = buf_অঙ্ক_anchors;

	    যদি (buf_অঙ্ক_anchors > 0)
		qsort((শূন্য *)anchors[idx],
			(size_t)buf_অঙ্ক_anchors,
			পরিমাণ(linenr_T),
			lnum_compare);
	}
    }
    যদি (অঙ্ক_anchors == INT_MAX)
	অঙ্ক_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    বাবে (পূর্ণাঙ্ক anchor_i = 0; anchor_i <= অঙ্ক_anchors; anchor_i++)
    {
	diff_T	*orig_diff = রিক্ত;
	যদি (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = রিক্ত;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == অঙ্ক_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	যদি (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    যদি (orig_diff != রিক্ত)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    গচ্ছ theend;
	}

	// Make a difference between the first buffer and every other.
	বাবে (idx_new = idx_orig + 1; idx_new < DB_গুনতি; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত)
		অনুৱর্ত; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == অঙ্ক_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    যদি (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		অনুৱর্ত;
	    যদি (diff_file(dio) == FAIL)
		অনুৱর্ত;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	যদি (anchor_i != 0)
	{
	    diff_finger_dp = রিক্ত;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    বাবে (diff_T *dp = curtab->tp_first_diff; dp != রিক্ত; dp = dp->df_next)
	    {
		বাবে (পূর্ণাঙ্ক idx = 0; idx < DB_গুনতি; idx++)
		{
		    যদি (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    যদি (orig_diff != রিক্ত)
	    {
		diff_T *last_diff = orig_diff;
		যেতিয়ালৈ (last_diff->df_next != রিক্ত)
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
    পূর্ণাঙ্ক
diff_internal(শূন্য)
{
    ঘূৰাই (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    স্থির পূর্ণাঙ্ক
diff_internal_failed(শূন্য)
{
    পূর্ণাঙ্ক idx;

    // Only need to do something when there is another buffer.
    বাবে (idx = 0; idx < DB_গুনতি; ++idx)
	যদি (curtab->tp_diffbuf[idx] != রিক্ত
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    শূন্য
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    পূর্ণাঙ্ক		idx_orig;
    পূর্ণাঙ্ক		idx_new;
    diffio_T	diffio;
    পূর্ণাঙ্ক		had_diffs = curtab->tp_first_diff != রিক্ত;

    যদি (diff_busy)
    {
	diff_need_update = TRUE;
	ঘূৰাই;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    বাবে (idx_orig = 0; idx_orig < DB_গুনতি; ++idx_orig)
	যদি (curtab->tp_diffbuf[idx_orig] != রিক্ত)
	    ভঙ্গ;
    যদি (idx_orig == DB_গুনতি)
	গচ্ছ theend;

    // Only need to do something when there is another buffer.
    বাবে (idx_new = idx_orig + 1; idx_new < DB_গুনতি; ++idx_new)
	যদি (curtab->tp_diffbuf[idx_new] != রিক্ত)
	    ভঙ্গ;
    যদি (idx_new == DB_গুনতি)
	গচ্ছ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    যদি (diffio.dio_internal && diff_internal_failed())
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
    যদি (had_diffs || curtab->tp_first_diff != রিক্ত)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, রিক্ত, রিক্ত, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    স্থির পূর্ণাঙ্ক
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    পূর্ণাঙ্ক		ok;
    পূর্ণাঙ্ক		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    বাবে (;;)
    {
	ok = FALSE;
	fd = mch_fopen((ৱর্ণ *)diffio->dio_orig.din_fname, "w");
	যদি (fd == রিক্ত)
	    io_error = TRUE;
	অন্যথা
	{
	    যদি (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((ৱর্ণ *)diffio->dio_new.din_fname, "w");
	    যদি (fd == রিক্ত)
		io_error = TRUE;
	    অন্যথা
	    {
		যদি (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = রিক্ত;
		যদি (diff_file(diffio) == OK)
		    fd = mch_fopen((ৱর্ণ *)diffio->dio_diff.dout_fname, "r");
		যদি (fd == রিক্ত)
		    io_error = TRUE;
		অন্যথা
		{
		    char_u	linebuf[LBUFLEN];

		    বাবে (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			যদি (vim_fgets(linebuf, LBUFLEN, fd))
			    ভঙ্গ;
			যদি (STRNCMP(linebuf, "1c1", 3) == 0
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
	যদি (*p_dex != NUL)
	    ভঙ্গ;
# endif

# যদি defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	যদি (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    অনুৱর্ত;
	}
	যদি (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	যদি (diff_a_works != MAYBE)
	    ভঙ্গ;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	যদি (ok)
	    ভঙ্গ;
    }
    যদি (!ok)
    {
	যদি (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# যদি defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Invoke the xdiff function.
 */
    স্থির পূর্ণাঙ্ক
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    যদি (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    যদি (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    যদি (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    যদি (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_সাৰিগুলি;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    যদি (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    অন্যথা
	emit_cb.out_সাৰি = xdiff_out_unified;
    যদি (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	ঘূৰাই FAIL;
    }
    যদি (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    স্থির পূর্ণাঙ্ক
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    যদি (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	ঘূৰাই OK;
    }
    অন্যথা
# endif
    // Use xdiff for generating the diff.
    যদি (dio->dio_internal)
	ঘূৰাই diff_file_internal(dio);

    যদি (check_restricted())
	ঘূৰাই FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    যদি (cmd == রিক্ত)
	ঘূৰাই FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    যদি (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((ৱর্ণ *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# যদি defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# অন্যথা
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (পূর্ণাঙ্ক)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (শূন্য)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    ঘূৰাই OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    শূন্য
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = রিক্ত;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = রিক্ত;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = রিক্ত;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = রিক্ত;
    পূর্ণাঙ্ক		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = রিক্ত;

# ifdef FEAT_BROWSE
    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, রিক্ত, রিক্ত,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), রিক্ত);
	যদি (browseFile == রিক্ত)
	    ঘূৰাই;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    যদি (tmp_orig == রিক্ত || tmp_new == রিক্ত)
	গচ্ছ theend;

    // Write the current buffer to "tmp_orig".
    যদি (buf_write(curbuf, tmp_orig, রিক্ত,
		(linenr_T)1, curbuf->b_ml.ml_সাৰি_গুনতি,
				     রিক্ত, FALSE, FALSE, FALSE, TRUE) == FAIL)
	গচ্ছ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != রিক্ত ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    যদি (esc_name == রিক্ত)
	গচ্ছ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    যদি (buf == রিক্ত)
	গচ্ছ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    যদি (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((ৱর্ণ *)dirbuf) != 0)
	dirbuf[0] = NUL;
    অন্যথা
    {
#  ifdef TEMPDIRNAMES
	যদি (vim_tempdir != রিক্ত)
	    vim_ignored = mch_chdir((ৱর্ণ *)vim_tempdir);
	অন্যথা
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    যদি (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != রিক্ত ? fullname :
#  endif
		eap->arg, tmp_new);
    অন্যথা
# endif
    {
	যদি (check_restricted())
	    গচ্ছ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((ৱর্ণ *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(শূন্য)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    যদি (dirbuf[0] != NUL)
    {
	যদি (mch_chdir((ৱর্ণ *)dirbuf) != 0)
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
    যদি (mch_stat((ৱর্ণ *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    অন্যথা
    {
	যদি (curbuf->b_fname != রিক্ত)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    যদি (newname != রিক্ত)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	যদি (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    যদি (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		যদি (newname != রিক্ত)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    যদি (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    যদি (tmp_orig != রিক্ত)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    যদি (tmp_new != রিক্ত)
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
    শূন্য
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

    যদি (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	ঘূৰাই;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    যদি (curwin == old_curwin)		// split didn't work
	ঘূৰাই;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    যদি (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	যদি (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_সাৰি(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    শূন্য
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    স্থির শূন্য
set_diff_option(win_T *wp, পূর্ণাঙ্ক মান)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_মান_give_err((char_u *)"diff", (long)মান, রিক্ত, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    শূন্য
diff_win_options(
    win_T	*wp,
    পূর্ণাঙ্ক		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    যদি (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    যদি (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    যদি (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	যদি (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    যদি (!wp->w_p_diff)
    {
	যদি (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    যদি (!wp->w_p_diff)
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
    যদি (vim_strchr(p_sbo, 'h') == রিক্ত)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    যদি (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    শূন্য
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    পূর্ণাঙ্ক		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	যদি (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    যদি (wp->w_p_diff_saved)
	    {

		যদি (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		যদি (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		যদি (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    যদি (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		যদি (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		যদি (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		যদি (wp->w_p_fen)
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
    যদি (eap->forceit)
	diff_buf_clear();

    যদি (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    যদি (!diffwin && vim_strchr(p_sbo, 'h') != রিক্ত)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    স্থির শূন্য
diff_read(
    পূর্ণাঙ্ক		idx_orig,	// idx of original file
    পূর্ণাঙ্ক		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = রিক্ত;
    পূর্ণাঙ্ক		সাৰি_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = রিক্ত;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*সাৰি;
    long	off;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = রিক্ত;	    // init to avoid gcc warning

    গণনা {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    যদি (dout->dout_fname == রিক্ত)
    {
	diffstyle = DIFF_UNIFIED;
    }
    অন্যথা
    {
	fd = mch_fopen((ৱর্ণ *)dout->dout_fname, "r");
	যদি (fd == রিক্ত)
	{
	    emsg(_(e_cannot_read_diff_output));
	    ঘূৰাই;
	}
    }

    যদি (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	যদি (hunk == রিক্ত)
	{
	    যদি (fd != রিক্ত)
		fclose(fd);
	    ঘূৰাই;
	}
    }

    বাবে (;;)
    {
	যদি (dio->dio_internal)
	{
	    যদি (সাৰি_hunk_idx >= dout->dout_ga.ga_len)
		ভঙ্গ;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[সাৰি_hunk_idx++];
	}
	অন্যথা
	{
	    যদি (fd == রিক্ত)
	    {
		যদি (সাৰি_hunk_idx >= dout->dout_ga.ga_len)
		    ভঙ্গ;	    // did last line
		সাৰি = ((char_u **)dout->dout_ga.ga_data)[সাৰি_hunk_idx++];
	    }
	    অন্যথা
	    {
		যদি (vim_fgets(linebuf, LBUFLEN, fd))
		    ভঙ্গ;		// end of file
		সাৰি = linebuf;
	    }

	    যদি (diffstyle == DIFF_NONE)
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
		যদি (SAFE_isdigit(*সাৰি))
		    diffstyle = DIFF_ED;
		অন্যথা যদি ((STRNCMP(সাৰি, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		অন্যথা যদি ((STRNCMP(সাৰি, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(সাৰি, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(সাৰি, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		অন্যথা
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    অনুৱর্ত;
	    }

	    যদি (diffstyle == DIFF_ED)
	    {
		যদি (!SAFE_isdigit(*সাৰি))
		    অনুৱর্ত;	// not the start of a diff block
		যদি (parse_diff_ed(সাৰি, hunk) == FAIL)
		    অনুৱর্ত;
	    }
	    অন্যথা যদি (diffstyle == DIFF_UNIFIED)
	    {
		যদি (STRNCMP(সাৰি, "@@ ", 3)  != 0)
		    অনুৱর্ত;	// not the start of a diff block
		যদি (parse_diff_unified(সাৰি, hunk) == FAIL)
		    অনুৱর্ত;
	    }
	    অন্যথা
	    {
		emsg(_(e_invalid_diff_format));
		ভঙ্গ;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	যেতিয়ালৈ (dp != রিক্ত
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_গুনতি[idx_orig])
	{
	    যদি (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	যদি (dp != রিক্ত
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_গুনতি[idx_orig]
		&& hunk->lnum_orig + hunk->গুনতি_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    বাবে (dpl = dp; dpl->df_next != রিক্ত; dpl = dpl->df_next)
		যদি (hunk->lnum_orig + hunk->গুনতি_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    ভঙ্গ;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    যদি (off > 0)
	    {
		বাবে (i = idx_orig; i < idx_new; ++i)
		    যদি (curtab->tp_diffbuf[i] != রিক্ত)
		    {
			dp->df_lnum[i] -= off;
			dp->df_গুনতি[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_গুনতি[idx_new] = hunk->গুনতি_new;
	    }
	    অন্যথা যদি (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_গুনতি[idx_new] = hunk->গুনতি_new - off;
	    }
	    অন্যথা
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		পূর্ণাঙ্ক orig_size_in_dp = MIN(hunk->গুনতি_orig,
			dp->df_lnum[idx_orig] +
			dp->df_গুনতি[idx_orig] - hunk->lnum_orig);
		পূর্ণাঙ্ক size_diff = hunk->গুনতি_new - orig_size_in_dp;
		dp->df_গুনতি[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->গুনতি_new
		    - (dp->df_lnum[idx_new] + dp->df_গুনতি[idx_new]);
		যদি (off > 0)
		    dp->df_গুনতি[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->গুনতি_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_গুনতি[idx_orig]);
	    যদি (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		যদি (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_গুনতি[idx_new] += -off;
		}
		off = 0;
	    }
	    বাবে (i = idx_orig; i < idx_new; ++i)
		যদি (curtab->tp_diffbuf[i] != রিক্ত)
		    dp->df_গুনতি[i] = dpl->df_lnum[i] + dpl->df_গুনতি[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    যেতিয়ালৈ (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	অন্যথা
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    যদি (dp == রিক্ত)
		গচ্ছ done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_গুনতি[idx_orig] = hunk->গুনতি_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_গুনতি[idx_new] = hunk->গুনতি_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    বাবে (i = idx_orig + 1; i < idx_new; ++i)
		যদি (curtab->tp_diffbuf[i] != রিক্ত)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    যেতিয়ালৈ (dp != রিক্ত)
    {
	যদি (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    যদি (!dio->dio_internal)
	vim_free(hunk);

    যদি (fd != রিক্ত)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    স্থির শূন্য
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    পূর্ণাঙ্ক		idx_orig,
    পূর্ণাঙ্ক		idx_new)
{
    long	off;

    যদি (dprev == রিক্ত)
	off = 0;
    অন্যথা
	off = (dprev->df_lnum[idx_orig] + dprev->df_গুনতি[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_গুনতি[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_গুনতি[idx_new] = dp->df_গুনতি[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    শূন্য
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    বাবে (p = tp->tp_first_diff; p != রিক্ত; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = রিক্ত;
}

/*
 *  return true if the options are set to use diff linematch
 */
    স্থির পূর্ণাঙ্ক
diff_linematch(diff_T *dp)
{
    যদি (!(diff_flags & DIFF_LINEMATCH))
	ঘূৰাই 0;

    // are there more than three diff buffers?
    পূর্ণাঙ্ক tsize = 0;
    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
    {
	যদি (curtab->tp_diffbuf[i] != রিক্ত)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    যদি (dp->df_গুনতি[i] < 0)
		ঘূৰাই FALSE;
	    tsize += dp->df_গুনতি[i];
	}
    }

    // avoid allocating a huge array because it will lag
    ঘূৰাই tsize <= linematch_সাৰিগুলি;
}

    স্থির পূর্ণাঙ্ক
get_max_diff_length(ধ্রুৱ diff_T *dp)
{
    পূর্ণাঙ্ক maxlength = 0;

    বাবে (পূর্ণাঙ্ক k = 0; k < DB_গুনতি; k++)
    {
	যদি (curtab->tp_diffbuf[k] != রিক্ত)
	{
	    যদি (dp->df_গুনতি[k] > maxlength)
		maxlength = dp->df_গুনতি[k];
	}
    }
    ঘূৰাই maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    স্থির শূন্য
find_শীৰ্ষ_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    পূর্ণাঙ্ক		fromidx,
    পূর্ণাঙ্ক		topline)
{
    diff_T	*topdiff = রিক্ত;
    diff_T	*localtopdiff = রিক্ত;
    পূর্ণাঙ্ক		topdiffchange = 0;

    বাবে (topdiff = curtab->tp_first_diff; topdiff != রিক্ত;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	যদি (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	যদি (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_গুনতি[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    যদি ((*thistopdiff) == রিক্ত)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	যদি (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_গুনতি[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    যদি (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		ভঙ্গ;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    স্থির শূন্য
calculate_topfill_and_topline(
    ধ্রুৱ পূর্ণাঙ্ক	fromidx,
    ধ্রুৱ পূর্ণাঙ্ক	toidx,
    ধ্রুৱ পূর্ণাঙ্ক	from_topline,
    ধ্রুৱ পূর্ণাঙ্ক	from_topfill,
    পূর্ণাঙ্ক		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = রিক্ত;
    diff_T	*next_adjacent_blocks = রিক্ত;
    পূর্ণাঙ্ক		virtual_সাৰিগুলি_passed = 0;
    পূর্ণাঙ্ক		curlinenum_to = 1;

    find_শীৰ্ষ_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    যেতিয়ালৈ (curdif && (curdif->df_lnum[fromidx] + curdif->df_গুনতি[fromidx])
							<= from_topline)
    {
	virtual_সাৰিগুলি_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    যদি (curdif != next_adjacent_blocks)
	virtual_সাৰিগুলি_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_সাৰিগুলি_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    যদি (virtual_সাৰিগুলি_passed < 0)
	virtual_সাৰিগুলি_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    যদি (thistopdiff != রিক্ত) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    পূর্ণাঙ্ক virt_সাৰিগুলি_বাম = virtual_সাৰিগুলি_passed;
    curdif = thistopdiff;
    যেতিয়ালৈ (virt_সাৰিগুলি_বাম > 0 && curdif != রিক্ত && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_সাৰিগুলি_বাম, curdif->df_গুনতি[toidx]);
	virt_সাৰিগুলি_বাম -= MIN(virt_সাৰিগুলি_বাম, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    পূর্ণাঙ্ক max_virt_সাৰিগুলি = 0;
    বাবে (diff_T *dp = thistopdiff; dp != রিক্ত; dp = dp->df_next)
    {
	যদি (dp->df_lnum[toidx] + dp->df_গুনতি[toidx] <= curlinenum_to)
	    max_virt_সাৰিগুলি += get_max_diff_length(dp);
	অন্যথা
	{
	    যদি (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_সাৰিগুলি += curlinenum_to - dp->df_lnum[toidx];
	    ভঙ্গ;
	}
    }

    যদি (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_সাৰিগুলি - virtual_সাৰিগুলি_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    স্থির শূন্য
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    ধ্রুৱ পূর্ণাঙ্ক	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    পূর্ণাঙ্ক		সাৰি_numbers[DB_গুনতি];
    পূর্ণাঙ্ক		outputmap[DB_গুনতি];
    size_t	ndiffs = 0;

    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
    {
	যদি (curtab->tp_diffbuf[i] != রিক্ত)
	{
	    সাৰি_numbers[i] = dp->df_lnum[i];
	    dp->df_গুনতি[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    বাবে (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	যদি (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    বাবে (পূর্ণাঙ্ক j = 0; j < DB_গুনতি; j++)
	    {
		যদি (curtab->tp_diffbuf[j] != রিক্ত)
		{
		    dp_s->df_lnum[j] = সাৰি_numbers[j];
		    dp_s->df_গুনতি[j] = 0;
		}
	    }
	}
	বাবে (size_t j = 0; j < ndiffs; j++)
	{
	    যদি (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_গুনতি[outputmap[j]]++;
		সাৰি_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    স্থির শূন্য
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_গুনতি];
    ধ্রুৱ mmfile_t	*diffbufs[DB_গুনতি];
    পূর্ণাঙ্ক			diff_length[DB_গুনতি];
    size_t		ndiffs = 0;

    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
    {
	যদি (curtab->tp_diffbuf[i] != রিক্ত)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    যদি (dp->df_গুনতি[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_গুনতি[i] - 1);
	    }
	    অন্যথা
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = রিক্ত;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_গুনতি[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    পূর্ণাঙ্ক *decisions = রিক্ত;
    ধ্রুৱ পূর্ণাঙ্ক iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    বাবে (size_t i = 0; i < ndiffs; i++)
	মুক্ত(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    মুক্ত(decisions);
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
    পূর্ণাঙ্ক
diff_check_with_linestatus(win_T *wp, linenr_T lnum, পূর্ণাঙ্ক *linestatus)
{
    পূর্ণাঙ্ক		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    পূর্ণাঙ্ক		maxcount;
    পূর্ণাঙ্ক		i;
    buf_T	*buf = wp->w_buffer;
    পূর্ণাঙ্ক		cmp;

    যদি (linestatus != রিক্ত)
	*linestatus = 0;

    যদি (curtab->tp_diff_invalid)
	ex_diffupdate(রিক্ত);		// update after a big change

    যদি (curtab->tp_first_diff == রিক্ত || !wp->w_p_diff)	// no diffs at all
	ঘূৰাই 0;

    // safety check: "lnum" must be a buffer line
    যদি (lnum < 1 || lnum > buf->b_ml.ml_সাৰি_গুনতি + 1)
	ঘূৰাই 0;

    idx = diff_buf_idx(buf);
    যদি (idx == DB_গুনতি)
	ঘূৰাই 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    যদি (hasFoldingWin(wp, lnum, রিক্ত, রিক্ত, TRUE, রিক্ত))
	ঘূৰাই 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = রিক্ত;		// last block with df_lnum[idx] <= lnum
    বাবে (dp = diff_scan_start(curtab, idx, lnum); dp != রিক্ত; dp = dp->df_next)
    {
	যদি (dp->df_lnum[idx] <= lnum)
	    le = dp;
	যদি (lnum <= dp->df_lnum[idx] + dp->df_গুনতি[idx])
	    ভঙ্গ;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    যদি (dp == রিক্ত || lnum < dp->df_lnum[idx])
	ঘূৰাই 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    যদি (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    পূর্ণাঙ্ক অঙ্ক_fill = 0;
    যেতিয়ালৈ (lnum == dp->df_lnum[idx] + dp->df_গুনতি[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	যদি (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    অঙ্ক_fill += maxcount - dp->df_গুনতি[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	যদি (dp->df_next != রিক্ত
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_গুনতি[idx])
	    dp = dp->df_next;
	অন্যথা
	    ভঙ্গ;
    }

    যদি (lnum < dp->df_lnum[idx] + dp->df_গুনতি[idx])
    {
	পূর্ণাঙ্ক	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	বাবে (i = 0; i < DB_গুনতি; ++i)
	    যদি (i != idx && curtab->tp_diffbuf[i] != রিক্ত)
	    {
		যদি (dp->df_গুনতি[i] == 0)
		    zero = TRUE;
		অন্যথা
		{
		    যদি (dp->df_গুনতি[i] != dp->df_গুনতি[idx])
		    {
			যদি (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			ঘূৰাই অঙ্ক_fill;
		    }
		    cmp = TRUE;
		}
	    }
	যদি (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    বাবে (i = 0; i < DB_গুনতি; ++i)
		যদি (i != idx && curtab->tp_diffbuf[i] != রিক্ত
						      && dp->df_গুনতি[i] != 0)
		    যদি (!diff_equal_entry(dp, idx, i))
		    {
			যদি (linestatus)
			    *linestatus = -1;
			ঘূৰাই অঙ্ক_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	যদি (zero == FALSE)
	    ঘূৰাই অঙ্ক_fill;
	যদি (linestatus)
	    *linestatus = -2;
	ঘূৰাই অঙ্ক_fill;
    }
    ঘূৰাই অঙ্ক_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    স্থির পূর্ণাঙ্ক
diff_equal_entry(diff_T *dp, পূর্ণাঙ্ক idx1, পূর্ণাঙ্ক idx2)
{
    পূর্ণাঙ্ক		i;
    char_u	*সাৰি;
    পূর্ণাঙ্ক		cmp;

    যদি (dp->df_গুনতি[idx1] != dp->df_গুনতি[idx2])
	ঘূৰাই FALSE;
    যদি (diff_check_sanity(curtab, dp) == FAIL)
	ঘূৰাই FALSE;
    বাবে (i = 0; i < dp->df_গুনতি[idx1]; ++i)
    {
	সাৰি = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	যদি (সাৰি == রিক্ত)
	    ঘূৰাই FALSE;
	cmp = diff_cmp(সাৰি, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(সাৰি);
	যদি (cmp != 0)
	    ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    স্থির পূর্ণাঙ্ক
diff_equal_char(char_u *p1, char_u *p2, পূর্ণাঙ্ক *len)
{
    পূর্ণাঙ্ক l  = (*mb_ptr2len)(p1);

    যদি (l != (*mb_ptr2len)(p2))
	ঘূৰাই FALSE;
    যদি (l > 1)
    {
	যদি (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    ঘূৰাই FALSE;
	*len = l;
    }
    অন্যথা
    {
	যদি ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    ঘূৰাই FALSE;
	*len = 1;
    }
    ঘূৰাই TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    স্থির পূর্ণাঙ্ক
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    পূর্ণাঙ্ক		l;

    যদি ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	ঘূৰাই 0;

    যদি ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	ঘূৰাই STRCMP(s1, s2);
    যদি ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	ঘূৰাই MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    যেতিয়ালৈ (*p1 != NUL && *p2 != NUL)
    {
	যদি (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	অন্যথা
	{
	    যদি (!diff_equal_char(p1, p2, &l))
		ভঙ্গ;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    যদি (*p1 != NUL || *p2 != NUL)
	ঘূৰাই 1;
    ঘূৰাই 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    পূর্ণাঙ্ক
diff_check_fill(win_T *wp, linenr_T lnum)
{
    পূর্ণাঙ্ক		n;

    // be quick when there are no filler lines
    যদি (!(diff_flags & DIFF_FILLER))
	ঘূৰাই 0;
    n = diff_check_with_linestatus(wp, lnum, রিক্ত);
    যদি (n <= 0)
	ঘূৰাই 0;
    ঘূৰাই n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    শূন্য
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    পূর্ণাঙ্ক		fromidx;
    পূর্ণাঙ্ক		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    যদি (fromidx == DB_গুনতি)
	ঘূৰাই;		// safety check

    যদি (curtab->tp_diff_invalid)
	ex_diffupdate(রিক্ত);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	যদি (lnum <= dp->df_lnum[fromidx] + dp->df_গুনতি[fromidx])
	    ভঙ্গ;
    যদি (dp == রিক্ত)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_সাৰি_গুনতি
				       - (frombuf->b_ml.ml_সাৰি_গুনতি - lnum);
    }
    অন্যথা
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	যদি (toidx == DB_গুনতি)
	    ঘূৰাই;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	যদি (lnum >= dp->df_lnum[fromidx])
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
    যদি (towin->w_topline > towin->w_buffer->b_ml.ml_সাৰি_গুনতি)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_সাৰি_গুনতি;
	towin->w_botfill = true;
    }
    যদি (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_সাৰি_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (শূন্য)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    রিক্ত, TRUE, রিক্ত);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    স্থির পূর্ণাঙ্ক
parse_diffanchors(
    পূর্ণাঙ্ক		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    পূর্ণাঙ্ক		*অঙ্ক_anchors)
{
    পূর্ণাঙ্ক i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = রিক্ত;
    যদি (check_only)
	bufwin = curwin;
    অন্যথা
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    যদি (bufwin->w_buffer == buf && bufwin->w_p_diff)
		ভঙ্গ;
	যদি (bufwin == রিক্ত && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    ঘূৰাই FAIL;
	}
    }

    বাবে (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	যদি (*dia == ',') // don't allow empty values
	    ঘূৰাই FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(রিক্ত, &dia, ADDR_সাৰিগুলি, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	যদি (dia == রিক্ত) // error detected
	    ঘূৰাই FAIL;
	যদি (*dia != ',' && *dia != NUL)
	    ঘূৰাই FAIL;

	যদি (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_সাৰি_গুনতি + 1))
	{
	    emsg(_(e_invalid_range));
	    ঘূৰাই FAIL;
	}

	যদি (anchors != রিক্ত)
	    anchors[i] = lnum;

	যদি (*dia == ',')
	    dia++;
    }
    যদি (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	ঘূৰাই FAIL;
    }
    যদি (অঙ্ক_anchors != রিক্ত)
	*অঙ্ক_anchors = i;
    ঘূৰাই OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    পূর্ণাঙ্ক
diffanchors_changed(পূর্ণাঙ্ক buflocal)
{
    পূর্ণাঙ্ক result = parse_diffanchors(TRUE, curbuf, রিক্ত, রিক্ত);
    যদি (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    যদি (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    অন্যথা
	    {
		বাবে (পূর্ণাঙ্ক idx = 0; idx < DB_গুনতি; ++idx)
		    যদি (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			ভঙ্গ;
		    }
	    }
	}
    }
    ঘূৰাই result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    পূর্ণাঙ্ক
diffopt_changed(শূন্য)
{
    char_u	*p;
    পূর্ণাঙ্ক		diff_context_new = 6;
    পূর্ণাঙ্ক		linematch_সাৰিগুলি_new = 0;
    পূর্ণাঙ্ক		diff_flags_new = 0;
    পূর্ণাঙ্ক		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    যেতিয়ালৈ (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	যদি (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	অন্যথা যদি (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	অন্যথা যদি (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	অন্যথা যদি (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	অন্যথা যদি (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	অন্যথা যদি (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	অন্যথা যদি (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	অন্যথা যদি (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	অন্যথা যদি (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	অন্যথা যদি (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	অন্যথা যদি (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	অন্যথা যদি (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	অন্যথা যদি (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	অন্যথা যদি (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	অন্যথা যদি (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	অন্যথা যদি (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	অন্যথা যদি (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    যদি (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    অন্যথা যদি (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    অন্যথা যদি (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    অন্যথা যদি (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    অন্যথা
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    যদি (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    অন্যথা যদি (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    অন্যথা যদি (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    অন্যথা যদি (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    অন্যথা
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_সাৰিগুলি_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	যদি (*p != ',' && *p != NUL)
	    ঘূৰাই FAIL;
	যদি (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    যদি ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	ঘূৰাই FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    যদি (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_সাৰিগুলি = linematch_সাৰিগুলি_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    ঘূৰাই OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    পূর্ণাঙ্ক
diffopt_horizontal(শূন্য)
{
    ঘূৰাই (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    পূর্ণাঙ্ক
diffopt_hiddenoff(শূন্য)
{
    ঘূৰাই (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    পূর্ণাঙ্ক
diffopt_closeoff(শূন্য)
{
    ঘূৰাই (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    শূন্য
diff_update_সাৰি(linenr_T lnum)
{
    পূর্ণাঙ্ক		idx;
    diff_T	*dp;

    যদি (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	ঘূৰাই;

    idx = diff_buf_idx(curbuf);
    যদি (idx == DB_গুনতি)
	ঘূৰাই;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	যদি (lnum <= dp->df_lnum[idx] + dp->df_গুনতি[idx])
	    ভঙ্গ;

    // clear the inline change cache as it's invalid
    যদি (dp != রিক্ত)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

স্থির diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    পূর্ণাঙ্ক
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    পূর্ণাঙ্ক *change_start,
    পূর্ণাঙ্ক *change_end)
{
    যদি (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    অন্যথা
	*change_start = change->dc_start[diffline->bufidx];
    যদি (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    অন্যথা
	*change_end = change->dc_end[diffline->bufidx];

    যদি (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	ঘূৰাই FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
    {
	যদি (i == diffline->bufidx)
	    অনুৱর্ত;
	যদি (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    ঘূৰাই FALSE;
	}
    }
    ঘূৰাই TRUE;
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
    স্থির পূর্ণাঙ্ক
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    পূর্ণাঙ্ক		idx,
    পূর্ণাঙ্ক		*startp,	// first char of the change
    পূর্ণাঙ্ক		*endp)		// last char of the change
{
    char_u	*সাৰি_org;
    char_u	*সাৰি_new;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		si_org, si_new;
    পূর্ণাঙ্ক		ei_org, ei_new;
    পূর্ণাঙ্ক		off;
    পূর্ণাঙ্ক		added = TRUE;
    char_u	*p1, *p2;
    পূর্ণাঙ্ক		l;

    যদি (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	সাৰি_org = রিক্ত;
    }
    অন্যথা
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	সাৰি_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	যদি (সাৰি_org == রিক্ত)
	    ঘূৰাই FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    বাবে (i = 0; i < DB_গুনতি; ++i)
	যদি (curtab->tp_diffbuf[i] != রিক্ত && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    যদি (off >= dp->df_গুনতি[i])
		অনুৱর্ত;
	    added = FALSE;
	    যদি (diff_flags & DIFF_INLINE_NONE)
		ভঙ্গ; // early terminate as we only care about the return value

	    সাৰি_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    যেতিয়ালৈ (সাৰি_org[si_org] != NUL)
	    {
		যদি (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(সাৰি_org[si_org])
					      && VIM_ISWHITE(সাৰি_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(সাৰি_org[si_org])
					    || VIM_ISWHITE(সাৰি_new[si_new]))))
		{
		    si_org = (পূর্ণাঙ্ক)(skipwhite(সাৰি_org + si_org) - সাৰি_org);
		    si_new = (পূর্ণাঙ্ক)(skipwhite(সাৰি_new + si_new) - সাৰি_new);
		}
		অন্যথা
		{
		    যদি (!diff_equal_char(সাৰি_org + si_org, সাৰি_new + si_new,
									   &l))
			ভঙ্গ;
		    si_org += l;
		    si_new += l;
		}
	    }
	    যদি (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(সাৰি_org, সাৰি_org + si_org);
		si_new -= (*mb_head_off)(সাৰি_new, সাৰি_new + si_new);
	    }
	    যদি (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    যদি (সাৰি_org[si_org] != NUL || সাৰি_new[si_new] != NUL)
	    {
		ei_org = (পূর্ণাঙ্ক)STRLEN(সাৰি_org);
		ei_new = (পূর্ণাঙ্ক)STRLEN(সাৰি_new);
		যেতিয়ালৈ (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    যদি (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(সাৰি_org[ei_org])
					      && VIM_ISWHITE(সাৰি_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(সাৰি_org[ei_org])
					    || VIM_ISWHITE(সাৰি_new[ei_new]))))
		    {
			যেতিয়ালৈ (ei_org >= *startp
					     && VIM_ISWHITE(সাৰি_org[ei_org]))
			    --ei_org;
			যেতিয়ালৈ (ei_new >= si_new
					     && VIM_ISWHITE(সাৰি_new[ei_new]))
			    --ei_new;
		    }
		    অন্যথা
		    {
			p1 = সাৰি_org + ei_org;
			p2 = সাৰি_new + ei_new;
			p1 -= (*mb_head_off)(সাৰি_org, p1);
			p2 -= (*mb_head_off)(সাৰি_new, p2);
			যদি (!diff_equal_char(p1, p2, &l))
			    ভঙ্গ;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		যদি (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(সাৰি_org);
    ঘূৰাই added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
প্রকারনির্দেশ সংরচনা
{
    long byte_start;
    long অঙ্ক_bytes;
    পূর্ণাঙ্ক lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    স্থির শূন্য
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, পূর্ণাঙ্ক idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    পূর্ণাঙ্ক pass = 1;
    কৰা
    {
	পূর্ণাঙ্ক has_unmerged_gaps = FALSE;
	পূর্ণাঙ্ক has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	যেতিয়ালৈ (dp!= রিক্ত && dp->df_next != রিক্ত)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    যদি (dp->df_lnum[idx1] + dp->df_গুনতি[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		অনুৱর্ত;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_গুনতি[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    যদি (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		অনুৱর্ত;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_গুনতি[idx1]);
	    যদি (gap <= 3)
	    {
		linenr_T max_df_গুনতি = 0;
		বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
		    max_df_গুনতি = MAX(max_df_গুনতি, dp->df_গুনতি[i] + dp->df_next->df_গুনতি[i]);

		যদি (max_df_গুনতি >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
		    {
			dp->df_গুনতি[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_গুনতি[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    অনুৱর্ত;
		}
		অন্যথা
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	যদি (!has_unmerged_gaps || !has_merged_gaps)
	    ভঙ্গ;
    } যেতিয়ালৈ (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    স্থির শূন্য
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, পূর্ণাঙ্ক idx1,
				  linenr_T start_lnum)
{
    পূর্ণাঙ্ক pass = 1;
    কৰা
    {
	diff_T *dp = dp_orig;

	যেতিয়ালৈ (dp != রিক্ত && dp->df_next != রিক্ত)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_গুনতি[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    যদি (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		অনুৱর্ত;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    যদি (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		অনুৱর্ত;
	    }

	    // Calculate the gap between blocks
	    পূর্ণাঙ্ক gap_start = entry1->byte_start + entry1->অঙ্ক_bytes;
	    পূর্ণাঙ্ক gap_end = entry2->byte_start;
	    পূর্ণাঙ্ক gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    যদি (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		অনুৱর্ত;
	    }

	    // Get the text between the two blocks
	    char_u *সাৰি = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = সাৰি + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    বাবে (পূর্ণাঙ্ক i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		পূর্ণাঙ্ক char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		যদি (char_class == 2)
		{
		    only_non_word = false;
		    ভঙ্গ;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    যদি (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
		{
		    যদি (curtab->tp_diffbuf[i] != রিক্ত)
		    {
			// count bytes in the first block
			বাবে (পূর্ণাঙ্ক k = 0; k < dp->df_গুনতি[i]; k++)
			{
			    পূর্ণাঙ্ক idx = dp->df_lnum[i] + k - 1;
			    যদি (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].অঙ্ক_bytes;
			}
			// count bytes in the next block
			বাবে (পূর্ণাঙ্ক k = 0; k < dp->df_next->df_গুনতি[i]; k++)
			{
			    পূর্ণাঙ্ক idx = dp->df_next->df_lnum[i] + k - 1;
			    যদি (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].অঙ্ক_bytes;
			}
		    }
		}

		যদি (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
		    {
			যদি (curtab->tp_diffbuf[i] != রিক্ত)
			{
			    dp->df_গুনতি[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_গুনতি[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    অনুৱর্ত;
		}
	    }

	    dp = dp->df_next;
	}
    } যেতিয়ালৈ (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    স্থির শূন্য
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_গুনতি];
    garray_T	file1_str;
    garray_T	file2_str;
    পূর্ণাঙ্ক		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, পরিমাণ(ৱর্ণ *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = রিক্ত;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_গুনতি]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, পরিমাণ(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
	ga_init2(&linemap[i], পরিমাণ(linemap_entry_T), 128);

    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত)
	    অনুৱর্ত; // skip buffer that isn't loaded

	যদি (dp->df_গুনতি[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = রিক্ত;
	    অনুৱর্ত;
	}

	যদি (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	বাবে (পূর্ণাঙ্ক off = 0; off < dp->df_গুনতি[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    পূর্ণাঙ্ক in_keyword = FALSE;

	    // iwhiteeol support vars
	    পূর্ণাঙ্ক last_white = FALSE;
	    পূর্ণাঙ্ক eol_ga_len = -1;
	    পূর্ণাঙ্ক eol_linemap_len = -1;
	    পূর্ণাঙ্ক eol_numlines = -1;

	    char_u *s;
	    বাবে (s = curline; *s != NUL;)
	    {
		পূর্ণাঙ্ক new_in_keyword = FALSE;
		যদি (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		যদি (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		যদি (VIM_ISWHITE(*s))
		{
		    যদি (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			অনুৱর্ত;
		    }
		    অন্যথা যদি ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			যদি (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		অন্যথা
		{
		    যদি ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		পূর্ণাঙ্ক char_len = 1;
		যদি (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		অন্যথা
		{
		    char_len = mb_ptr2len(s);

		    যদি (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    যদি (diff_flags & DIFF_ICASE)
		    {
			পূর্ণাঙ্ক c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			পূর্ণাঙ্ক c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			পূর্ণাঙ্ক c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			যদি (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    অন্যথা
			ga_concat_len(curstr, s, char_len);
		}

		যদি (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		যদি (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.অঙ্ক_bytes = char_len;
		    যদি (ga_grow(&linemap[i], 1) != OK)
			গচ্ছ done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		অন্যথা
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].অঙ্ক_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    যদি (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    যদি ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		যদি (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    যদি (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.অঙ্ক_bytes = পরিমাণ(NL);
		যদি (ga_grow(&linemap[i], 1) != OK)
		    গচ্ছ done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	যদি (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (ৱর্ণ *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	অন্যথা
	{
	    dio.dio_orig.din_mmfile.ptr = (ৱর্ণ *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	যদি (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    পূর্ণাঙ্ক diff_status = diff_file_internal(&dio);
	    যদি (diff_status == FAIL)
		গচ্ছ done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    যদি (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    অন্যথা যদি (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    বাবে (; new_diff != রিক্ত; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
	{
	    যদি (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		অনুৱর্ত;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_গুনতি[i];

	    যদি (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    অন্যথা
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    যদি (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    অন্যথা যদি (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    অন্যথা
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].অঙ্ক_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	যদি (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    গচ্ছ done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, পরিমাণ(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    পূর্ণাঙ্ক
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		off;

    idx = diff_buf_idx(wp->w_buffer);
    যদি (idx == DB_গুনতি)	// cannot happen
	ঘূৰাই FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	যদি (lnum < dp->df_lnum[idx] + dp->df_গুনতি[idx])
	    ভঙ্গ;
    যদি (dp == রিক্ত || diff_check_sanity(curtab, dp) == FAIL)
	ঘূৰাই FALSE;

    যদি (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	ঘূৰাই FALSE;
    off = lnum - dp->df_lnum[idx];

    যদি (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	পূর্ণাঙ্ক	change_start = MAXCOL;	// first col of changed area
	পূর্ণাঙ্ক	change_end = -1;	// last col of changed area
	পূর্ণাঙ্ক	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->অঙ্ক_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	ঘূৰাই ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    যদি (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    পূর্ণাঙ্ক অঙ্ক_changes = 0;
    পূর্ণাঙ্ক change_idx = 0;
    diffline->changes = রিক্ত;
    বাবে (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	যদি (change->dc_end_lnum_off[idx] < off)
	    অনুৱর্ত;
	যদি (change->dc_start_lnum_off[idx] > off)
	    ভঙ্গ;
	যদি (diffline->changes == রিক্ত)
	    diffline->changes = change;
	অঙ্ক_changes++;
    }
    diffline->অঙ্ক_changes = অঙ্ক_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    পূর্ণাঙ্ক added = FALSE;
    যদি (অঙ্ক_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	বাবে (পূর্ণাঙ্ক i = 0; i < DB_গুনতি; i++)
	{
	    যদি (idx == i)
		অনুৱর্ত;
	    যদি (curtab->tp_diffbuf[i] == রিক্ত)
		অনুৱর্ত;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    যদি (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		ভঙ্গ;
	    }
	}
    }
    ঘূৰাই added;
}

# যদি defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    পূর্ণাঙ্ক
diff_infold(win_T *wp, linenr_T lnum)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		idx = -1;
    পূর্ণাঙ্ক		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    যদি (!wp->w_p_diff)
	ঘূৰাই FALSE;

    বাবে (i = 0; i < DB_গুনতি; ++i)
    {
	যদি (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	অন্যথা যদি (curtab->tp_diffbuf[i] != রিক্ত)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    যদি (idx == -1 || !other)
	ঘূৰাই FALSE;

    যদি (curtab->tp_diff_invalid)
	ex_diffupdate(রিক্ত);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    যদি (curtab->tp_first_diff == রিক্ত)
	ঘূৰাই TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = রিক্ত;		// last block with df_lnum[idx] <= lnum
    পূর্ণাঙ্ক		result = TRUE;
    বাবে (dp = diff_scan_start(curtab, idx, lnum); dp != রিক্ত; dp = dp->df_next)
    {
	যদি (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	যদি (dp->df_lnum[idx] - diff_context > lnum)
	    ভঙ্গ;
	// If this change ends before the line we have a match.
	যদি (dp->df_lnum[idx] + dp->df_গুনতি[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    ভঙ্গ;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    ঘূৰাই result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    শূন্য
nv_diffgetput(পূর্ণাঙ্ক put, long গুনতি)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    যদি (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	ঘূৰাই;
    }
# endif
    যদি (গুনতি == 0)
	ea.arg = (char_u *)"";
    অন্যথা
    {
	vim_snprintf((ৱর্ণ *)buf, 30, "%ld", গুনতি);
	ea.arg = buf;
    }
    যদি (put)
	ea.cmdidx = CMD_diffput;
    অন্যথা
	ea.cmdidx = CMD_diffget;
    ea.addr_গুনতি = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    স্থির পূর্ণাঙ্ক
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	যদি (dp == diff)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    শূন্য
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		গুনতি;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    পূর্ণাঙ্ক		idx_cur;
    পূর্ণাঙ্ক		idx_other;
    পূর্ণাঙ্ক		idx_from;
    পূর্ণাঙ্ক		idx_to;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    পূর্ণাঙ্ক		start_skip, end_skip;
    পূর্ণাঙ্ক		new_গুনতি;
    পূর্ণাঙ্ক		buf_empty;
    পূর্ণাঙ্ক		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    যদি (idx_cur == DB_গুনতি)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	ঘূৰাই;
    }

    যদি (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	বাবে (idx_other = 0; idx_other < DB_গুনতি; ++idx_other)
	    যদি (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != রিক্ত)
	    {
		যদি (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    ভঙ্গ;
		found_not_ma = TRUE;
	    }
	যদি (idx_other == DB_গুনতি)
	{
	    যদি (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    অন্যথা
		emsg(_(e_no_other_buffer_in_diff_mode));
	    ঘূৰাই;
	}

	// Check that there isn't a third buffer in the list
	বাবে (i = idx_other + 1; i < DB_গুনতি; ++i)
	    যদি (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != রিক্ত
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		ঘূৰাই;
	    }
    }
    অন্যথা
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	যেতিয়ালৈ (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	বাবে (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	যদি (eap->arg + i == p)	    // digits only
	    i = atol((ৱর্ণ *)eap->arg);
	অন্যথা
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    যদি (i < 0)
		ঘূৰাই;		// error message already given
	}
	buf = buflist_findnr(i);
	যদি (buf == রিক্ত)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    ঘূৰাই;
	}
	যদি (buf == curbuf)
	    ঘূৰাই;		// nothing to do
	idx_other = diff_buf_idx(buf);
	যদি (idx_other == DB_গুনতি)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    ঘূৰাই;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    যদি (eap->addr_গুনতি == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	পূর্ণাঙ্ক linestatus = 0;
	যদি (eap->line1 == curbuf->b_ml.ml_সাৰি_গুনতি
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	অন্যথা যদি (eap->line1 > 0)
	    --eap->line1;
    }

    যদি (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    অন্যথা
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	যদি (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    গচ্ছ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    যদি (!curbuf->b_changed)
    {
	change_warning(0);
	যদি (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    গচ্ছ theend;
	}
    }

    dprev = রিক্ত;
    বাবে (dp = curtab->tp_first_diff; dp != রিক্ত; )
    {
	যদি (!eap->addr_গুনতি)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    যেতিয়ালৈ (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_গুনতি[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	যদি (dp->df_lnum[idx_cur] > eap->line2 + off)
	    ভঙ্গ;	// past the range that was specified

	dfree = রিক্ত;
	lnum = dp->df_lnum[idx_to];
	গুনতি = dp->df_গুনতি[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + গুনতি + (গুনতি == 0 && BUFEMPTY() ? 1 : 0);

	যদি (dp->df_lnum[idx_cur] + dp->df_গুনতি[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    যদি (eap->addr_গুনতি > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		যদি (start_skip > 0)
		{
		    // range starts below start of current diff block
		    যদি (start_skip > গুনতি)
		    {
			lnum += গুনতি;
			গুনতি = 0;
		    }
		    অন্যথা
		    {
			গুনতি -= start_skip;
			lnum += start_skip;
		    }
		}
		অন্যথা
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_গুনতি[idx_cur] - 1
							 - (eap->line2 + off);
		যদি (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    যদি (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_গুনতি[idx_cur] - start_skip - end_skip;
			যদি (গুনতি > i)
			    গুনতি = i;
		    }
		    অন্যথা			// :diffget
		    {
			গুনতি -= end_skip;
			end_skip = dp->df_গুনতি[idx_from] - start_skip - গুনতি;
			যদি (end_skip < 0)
			    end_skip = 0;
		    }
		}
		অন্যথা
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    বাবে (i = 0; i < গুনতি; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_সাৰি_গুনতি == 1;
		যদি (ml_delete(lnum) == OK)
		    --added;
	    }
	    বাবে (i = 0; i < dp->df_গুনতি[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		যদি (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_সাৰি_গুনতি)
		    ভঙ্গ;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		যদি (p != রিক্ত)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    যদি (buf_empty && curbuf->b_ml.ml_সাৰি_গুনতি == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_গুনতি = dp->df_গুনতি[idx_to] + added;
	    dp->df_গুনতি[idx_to] = new_গুনতি;

	    যদি (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		বাবে (i = 0; i < DB_গুনতি; ++i)
		    যদি (curtab->tp_diffbuf[i] != রিক্ত && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			ভঙ্গ;
		যদি (i == DB_গুনতি)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    যদি (dprev == রিক্ত)
			curtab->tp_first_diff = dp;
		    অন্যথা
			dprev->df_next = dp;
		}
	    }

	    যদি (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + গুনতি - 1, (long)MAXLNUM, (long)added);
		যদি (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    যদি (curwin->w_cursor.lnum >= lnum + গুনতি)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_সাৰি_গুনতি);
		    }
		    অন্যথা যদি (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_সাৰিগুলি(lnum, 0, lnum + গুনতি, (long)added);

	    যদি (dfree != রিক্ত)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    যদি (added != 0 && !valid_diff(dp))
		ভঙ্গ;

	    যদি (dfree == রিক্ত)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_গুনতি[idx_to] = new_গুনতি;

	    // When changing the current buffer, keep track of line numbers
	    যদি (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	যদি (dfree == রিক্ত)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    যদি (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	যদি (চাবি_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    যদি (diff_need_update)
	ex_diffupdate(রিক্ত);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_সাৰি_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    যদি (curtab->tp_first_diff == রিক্ত)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    যদি (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    যদি (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    অন্যথা
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, রিক্ত, রিক্ত, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    স্থির শূন্য
diff_fold_update(diff_T *dp, পূর্ণাঙ্ক skip_idx)
{
    পূর্ণাঙ্ক		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	বাবে (i = 0; i < DB_গুনতি; ++i)
	    যদি (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_গুনতি[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    পূর্ণাঙ্ক
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	যদি (diff_buf_idx_tp(buf, tp) != DB_গুনতি)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    পূর্ণাঙ্ক
diff_move_to(পূর্ণাঙ্ক dir, long গুনতি)
{
    পূর্ণাঙ্ক		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    যদি (idx == DB_গুনতি || curtab->tp_first_diff == রিক্ত)
	ঘূৰাই FAIL;

    যদি (curtab->tp_diff_invalid)
	ex_diffupdate(রিক্ত);		// update after a big change

    যদি (curtab->tp_first_diff == রিক্ত)		// no diffs today
	ঘূৰাই FAIL;

    যেতিয়ালৈ (--গুনতি >= 0)
    {
	// Check if already before first diff.
	যদি (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    ভঙ্গ;

	বাবে (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    যদি (dp == রিক্ত)
		ভঙ্গ;
	    যদি ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == রিক্ত
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		ভঙ্গ;
	    }
	}
    }

    // don't end up past the end of the file
    যদি (lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
	lnum = curbuf->b_ml.ml_সাৰি_গুনতি;

    // When the cursor didn't move at all we fail.
    যদি (lnum == curwin->w_cursor.lnum)
	ঘূৰাই FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    ঘূৰাই OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    স্থির linenr_T
diff_get_corresponding_সাৰি_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    পূর্ণাঙ্ক		idx1;
    পূর্ণাঙ্ক		idx2;
    diff_T	*dp;
    পূর্ণাঙ্ক		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    যদি (idx1 == DB_গুনতি || idx2 == DB_গুনতি || curtab->tp_first_diff == রিক্ত)
	ঘূৰাই lnum1;

    যদি (curtab->tp_diff_invalid)
	ex_diffupdate(রিক্ত);		// update after a big change

    যদি (curtab->tp_first_diff == রিক্ত)		// no diffs today
	ঘূৰাই lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	যদি (dp->df_lnum[idx1] > lnum1)
	    ঘূৰাই lnum1 - baseline;
	যদি ((dp->df_lnum[idx1] + dp->df_গুনতি[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    যদি (baseline > dp->df_গুনতি[idx2])
		baseline = dp->df_গুনতি[idx2];

	    ঘূৰাই dp->df_lnum[idx2] + baseline;
	}
	যদি (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_গুনতি[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_গুনতি[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    ঘূৰাই curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_গুনতি[idx1])
				   - (dp->df_lnum[idx2] + dp->df_গুনতি[idx2]);
    }

    // If we get here then the cursor is after the last diff
    ঘূৰাই lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_সাৰি(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_সাৰি_int(buf1, lnum1);

    // don't end up past the end of the file
    যদি (lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
	ঘূৰাই curbuf->b_ml.ml_সাৰি_গুনতি;
    ঘূৰাই lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    যদি (idx == DB_গুনতি)		// safety check
	ঘূৰাই (linenr_T)0;

    যদি (curtab->tp_diff_invalid)
	ex_diffupdate(রিক্ত);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	যদি (lnum <= dp->df_lnum[idx] + dp->df_গুনতি[idx])
	    ভঙ্গ;

    // When after the last change, compute relative to the last line number.
    যদি (dp == রিক্ত)
	ঘূৰাই wp->w_buffer->b_ml.ml_সাৰি_গুনতি
					- (curbuf->b_ml.ml_সাৰি_গুনতি - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    যদি (i == DB_গুনতি)			// safety check
	ঘূৰাই (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    যদি (n > dp->df_lnum[i] + dp->df_গুনতি[i])
	n = dp->df_lnum[i] + dp->df_গুনতি[i];
    ঘূৰাই n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    স্থির পূর্ণাঙ্ক
parse_diff_ed(
	char_u	    *সাৰি,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    পূর্ণাঙ্ক	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = সাৰি;
    f1 = getdigits(&p);
    যদি (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    অন্যথা
	l1 = f1;
    যদি (*p != 'a' && *p != 'c' && *p != 'd')
	ঘূৰাই FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    যদি (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    অন্যথা
	l2 = f2;
    যদি (l1 < f1 || l2 < f2)
	ঘূৰাই FAIL;

    যদি (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->গুনতি_orig = 0;
    }
    অন্যথা
    {
	hunk->lnum_orig = f1;
	hunk->গুনতি_orig = l1 - f1 + 1;
    }
    যদি (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->গুনতি_new = 0;
    }
    অন্যথা
    {
	hunk->lnum_new = f2;
	hunk->গুনতি_new = l2 - f2 + 1;
    }
    ঘূৰাই OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    স্থির পূর্ণাঙ্ক
parse_diff_unified(
	char_u	    *সাৰি,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = সাৰি;
    যদি (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	যদি (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	অন্যথা
	    oldcount = 1;
	যদি (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    যদি (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    অন্যথা
		newcount = 1;
	}
	অন্যথা
	    ঘূৰাই FAIL;	// invalid diff format

	যদি (oldcount == 0)
	    oldline += 1;
	যদি (newcount == 0)
	    newline += 1;
	যদি (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->গুনতি_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->গুনতি_new = newcount;

	ঘূৰাই OK;
    }

    ঘূৰাই FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    স্থির পূর্ণাঙ্ক
xdiff_out_indices(
	long start_a,
	long গুনতি_a,
	long start_b,
	long গুনতি_b,
	শূন্য *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    যদি (p == রিক্ত)
	ঘূৰাই -1;

    যদি (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	ঘূৰাই -1;
    }

    p->lnum_orig  = start_a + 1;
    p->গুনতি_orig = গুনতি_a;
    p->lnum_new   = start_b + 1;
    p->গুনতি_new  = গুনতি_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    ঘূৰাই 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    স্থির পূর্ণাঙ্ক
xdiff_out_unified(
	শূন্য *priv,
	mmbuffer_t *mb,
	পূর্ণাঙ্ক nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    ঘূৰাই 0;
}

#endif	// FEAT_DIFF

#যদি defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    শূন্য
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    শূন্য
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    স্থির linenr_T	prev_lnum = 0;
    স্থির varnumber_T	changedtick = 0;
    স্থির পূর্ণাঙ্ক		fnum = 0;
    স্থির পূর্ণাঙ্ক		prev_diff_flags = 0;
    স্থির পূর্ণাঙ্ক		change_start = 0;
    স্থির পূর্ণাঙ্ক		change_end = 0;
    স্থির hlf_T	hlID = (hlf_T)0;
    পূর্ণাঙ্ক			cache_results = TRUE;
    পূর্ণাঙ্ক			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    যদি (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    যদি (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	পূর্ণাঙ্ক linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	যদি (linestatus < 0)
	{
	    যদি (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		যদি (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		অন্যথা
		{
		    hlID = HLF_CHD;	// changed line
		    যদি (diffline.অঙ্ক_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    অন্যথা
		hlID = HLF_ADD;	// added line
	}
	অন্যথা
	    hlID = (hlf_T)0;

	যদি (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    যদি (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_সংখ্যা(&argvars[1]) - 1; // ignore type error in {col}
	যদি (cache_results)
	{
	    যদি (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    অন্যথা
		hlID = HLF_CHD;			// changed line
	}
	অন্যথা
	{
	    hlID = HLF_CHD;
	    বাবে (পূর্ণাঙ্ক i = 0; i < diffline.অঙ্ক_changes; i++)
	    {
		পূর্ণাঙ্ক added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		যদি (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    ভঙ্গ;
		}
		যদি (col < change_start)
		    // the remaining changes are past this column and not relevant
		    ভঙ্গ;
	    }
	}
    }
    rettv->vval.v_সংখ্যা = hlID == (hlf_T)0 ? 0 : (পূর্ণাঙ্ক)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    স্থির পূর্ণাঙ্ক
parse_diff_optarg(
    typval_T	    *opts,
    পূর্ণাঙ্ক		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    পূর্ণাঙ্ক		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    যদি (algo != রিক্ত)
    {
	যদি (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	অন্যথা যদি (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	অন্যথা যদি (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	অন্যথা যদি (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    যদি (output_fmt != রিক্ত)
    {
	যদি (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	অন্যথা যদি (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	অন্যথা
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    ঘূৰাই FAIL;
	}
    }

    *diff_ctxlen = dict_get_সংখ্যা_def(d, "context", 0);
    যদি (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    যদি (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    যদি (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    যদি (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    যদি (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    যদি (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    যদি (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    ঘূৰাই OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    স্থির শূন্য
list_to_diffin(list_T *l, diffin_T *din, পূর্ণাঙ্ক icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	str = tv_get_string(&li->li_tv);
	যদি (icase)
	{
	    str = strlow_save(str);
	    যদি (str == রিক্ত)
		অনুৱর্ত;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	যদি (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (ৱর্ণ *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    স্থির dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    যদি (hunk_dict == রিক্ত)
	ঘূৰাই রিক্ত;

    dict_add_সংখ্যা(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_সংখ্যা(hunk_dict, "from_count", hunk->গুনতি_orig);
    dict_add_সংখ্যা(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_সংখ্যা(hunk_dict, "to_count", hunk->গুনতি_new);

    ঘূৰাই hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    শূন্য
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    যদি (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	ঘূৰাই;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, পরিমাণ(ৱর্ণ *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    পূর্ণাঙ্ক		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
	যদি (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    ঘূৰাই;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    পূর্ণাঙ্ক	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    যদি (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    পূর্ণাঙ্ক diff_status = diff_file(&dio);

    // restore 'diffexpr'
    যদি (restore_diffexpr)
	*p_dex = cc;

    যদি (diff_status == FAIL)
	গচ্ছ done;

    পূর্ণাঙ্ক		hunk_idx = 0;
    dict_T	*hunk_dict;

    যদি (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	যদি (rettv_list_alloc(rettv) != OK)
	    গচ্ছ done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = রিক্ত;
	যেতিয়ালৈ (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    যদি (hunk_dict == রিক্ত)
		গচ্ছ done;

	    যদি (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    অন্যথা
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    যদি (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    অন্যথা
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
